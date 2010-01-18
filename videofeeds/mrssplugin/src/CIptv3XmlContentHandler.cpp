/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    XML Parser handler*
*/




#include <e32base.h>
#include "MIptvEpgVodCallback.h"
#include "CIptvEpgCategory.h"
#include "CIptvEpgContent.h"
#include "IptvDebug.h"
#include <xml/xmlparsererrors.h>
#include <utf.h>
#include "CIptvMediaContent.h"
#include "CIptvTerminalVideoSupport.h"
#include "CIptv3XmlContentHandler.h"
#include "CIptvRssPlugin.h"
#include "CIptvDownloadItem.h"
#include "ciptvxmlconsts.h"
#include "ciptvxmldatetime.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::ConstructL()
    {
    iTerminalVideoSupport = CIptvTerminalVideoSupport::NewL();
    }

// --------------------------------------------------------------------------
// Two-phased constructor.
// Create instance of concrete interface implementation
// --------------------------------------------------------------------------
//
CIptvXmlContentHandler* CIptvXmlContentHandler::NewL(
    CIptvRssPlugin& aPlugin )
    {
    CIptvXmlContentHandler* self =
        new ( ELeave ) CIptvXmlContentHandler( aPlugin );
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CIptvXmlContentHandler::~CIptvXmlContentHandler()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::~CIptvXmlContentHandler" );

    delete iTerminalVideoSupport;

    // Delete only if iChannelCategoryKey has the original value. Otherwise
    // the iChannelCategory has been appended to iCategoryTable array.
    // Deletingin this case leads to double deletion & crash. This happened
    // when XML feed had an error in item and parsing was interrupted.
    //
    if ( 0 == iChannelCategoryKey )
        {
        delete iChannelCategory;
        }
    iChannelCategory = NULL;

    delete iContent;
    delete iBuffer;

    iVodCallback = NULL;

    iMediaContents.ResetAndDestroy();
    iMediaContentSet.Close();
    iCurrentCategoryKeys.Close();
    iCategoryTable.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// C++ default constructor
// --------------------------------------------------------------------------
//
CIptvXmlContentHandler::CIptvXmlContentHandler( CIptvRssPlugin& aPlugin ) :
    iPlugin( aPlugin ),
    iState( EIptvRssIdleState ),
    iMediaRatingScheme( EMediaRatingSimple ),
    iCategoryState( ENoChannel )
    {
    }

// --------------------------------------------------------------------------
// Set callback for storing XML parse results.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::SetVodCallback(
    MIptvEpgVodCallback* aVodCallback )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::SetVodCallback" );
    iVodCallback = aVodCallback;
    }

// --------------------------------------------------------------------------
// Callback to indicate the start of the document.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnStartDocumentL(
    const RDocumentParameters& /*aDocParam*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::OnStartDocumentL" );

    if ( aErrorCode != KErrNone )
        {
        iPlugin.SetParsingFailed();
        }

    iServiceThumbnailAdded = EFalse;
    iRSSContentFound = EFalse;
    }

// --------------------------------------------------------------------------
// Callback to indicate the end of the document.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnEndDocumentL( TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::OnEndDocumentL" );

    AddCategories();

    if ( ( KErrNone != aErrorCode ) || !iRSSContentFound )
        {
        iPlugin.SetParsingFailed();
        }

    iPlugin.ContentUpdateCompletedL();
    }

// --------------------------------------------------------------------------
// Callback to indicate an element parsing start.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnStartElementL(
    const RTagInfo& aElement,
    const RAttributeArray& aAttributes,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::OnStartElementL" );

    if ( aErrorCode != KErrNone )
        {
        iPlugin.SetParsingFailed();
        return;
        }

    delete iBuffer;
    iBuffer = NULL;

    TInt dataToCopy = Min(
        aElement.LocalName().DesC().Length(),
        KIptvRssItemNameMaxLength );
    HBufC* name = HBufC::NewLC( dataToCopy );
    TPtr localName = name->Des();
    localName.Copy( aElement.LocalName().DesC().Mid( 0, dataToCopy ) );

    // Parse name prefix.
    iMediaPrefix = EFalse;
    dataToCopy = Min(
        aElement.Prefix().DesC().Length(),
        KIptvRssItemNameMaxLength );
    if ( 0 < dataToCopy )
        {
        HBufC* prefix = HBufC::NewLC( dataToCopy );
        TPtr prefixName = prefix->Des();
        prefixName.Copy( aElement.Prefix().DesC().Mid( 0, dataToCopy ) );
        if ( prefixName.CompareF( KIptvRssMedia ) == 0 )
            {
            iMediaPrefix = ETrue;
            }
        CleanupStack::PopAndDestroy( prefix );
        }

    switch ( iState )
        {
        case EIptvRssIdleState:
            {
            if ( localName.CompareF( KIptvRssRss ) == 0 )
                {
                iState = EIptvRssState;
                iRSSContentFound = ETrue;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssIdleState to EIptvRssState" );
                HandleEnclosureL( aAttributes );
                }
            }
            break;

        case EIptvRssState:
            {
            if ( localName.CompareF( KIptvRssChannel ) == 0 )
                {
                if ( SetCategoryState( EChannelFound ) )
                    {
                    iState = EIptvRssChannelState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssState to EIptvRssChannelState" );
                    delete iChannelCategory;
                    iChannelCategory = NULL;
                    iChannelCategory = CreateCategoryL();
                    iChannelCategoryKey = 0;
                    }
                }
            }
            break;

        case EIptvRssChannelState:
            {
            //lint -e{961} Else block not needed, default is no operation.
            if ( localName.CompareF( KIptvRssDescription ) == 0 )
                {
                iState = EIptvRssChannelDescriptionState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelState to EIptvRssChannelDescriptionState" );
                }
            else if ( localName.CompareF( KIptvRssLink ) == 0 )
                {
                iState = EIptvRssChannelLinkState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelState to EIptvRssChannelLinkState" );
                }
            else if ( localName.CompareF( KIptvRssTitle ) == 0 )
                {
                iState = EIptvRssChannelTitleState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelState to EIptvRssChannelTitleState" );
                }
            else if ( localName.CompareF( KIptvRssImage ) == 0 )
                {
                iState = EIptvRssChannelImageState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelState to EIptvRssChannelImageState" );
                HandleEnclosureL( aAttributes );
                }
            else if ( localName.CompareF( KIptvRssItem ) == 0 )
                {
                iCurrentCategoryKeys.Reset();
                SetCategoryState( EItemFound );
                iState = EIptvRssItemState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelState to EIptvRssItemState" );
                CreateContentL();

                iMediaDescriptionFixed = EFalse;
                iMediaTitleFixed = EFalse;
                iMediaRatingFixed = EFalse;
                iMediaCopyrightFixed = EFalse;
                iMediaThumbnailFixed = EFalse;
                }
            }
            break;

        case EIptvRssChannelImageState:
            {
            if ( localName.CompareF( KIptvRssUrl ) == 0 )
                {
                iState = EIptvRssChannelImageUrlState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelImageState to EIptvRssChannelImageUrlState" );
                }
            }
            break;

        case EIptvRssChannelImageUrlState:
            break;

        case EIptvRssChannelDescriptionState:
            break;

        case EIptvRssChannelLinkState:
            break;

        case EIptvRssChannelTitleState:
            break;

        case EIptvRssItemState:
            {
            //lint -e{961} Else block not needed, default is no operation.
            if ( localName.CompareF( KIptvRssDescription ) == 0 )
                {
                iState = EIptvRssItemDescriptionState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemDescriptionState" );
                }
            else if ( localName.CompareF( KIptvRssLink ) == 0 )
                {
                iState = EIptvRssItemLinkState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemLinkState" );
                }
            else if ( localName.CompareF( KIptvRssTitle ) == 0 )
                {
                iState = EIptvRssItemTitleState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemTitleState" );
                }
            else if ( localName.CompareF( KIptvRssEnclosure ) == 0 )
                {
                iState = EIptvRssItemEnclosureState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemEnclosureState" );
                HandleEnclosureL( aAttributes );
                }
            else if ( localName.CompareF( KIptvRssGuid ) == 0 )
                {
                iState = EIptvRssItemGuidState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemGuidState" );
                }
            else if ( localName.CompareF( KIptvRssPubDate ) == 0 )
                {
                iState = EIptvRssItemPubDate;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemPubDate" );
                }
            else if ( localName.CompareF( KIptvRssThumbnail ) == 0 )
                {
                iState = EIptvRssItemMediaThumbnailState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemMediaThumbnailState" );
                HandleEnclosureL( aAttributes );
                }
            else if ( localName.CompareF( KIptvRssMediaGroup ) == 0 )
                {
                iMediaGroup = ETrue;
                iState = EIptvRssItemMediaGroupState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemMediaGroupState" );
                }
            else if ( localName.CompareF( KIptvRssMediaContent ) == 0 )
                {
                iMediaGroup = EFalse;
                iState = EIptvRssItemMediaContentState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemMediaContentState" );
                HandleEnclosureL( aAttributes );
                }
            else if ( localName.CompareF( KIptvRssAuthor ) == 0 )
                {
                iState = EIptvRssItemAuthorState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemAuthorState" );
                }
            else if ( localName.CompareF( KIptvRssMediaCopyright ) == 0 )
                {
                iState = EIptvRssItemMediaCopyrightState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemMediaCopyrightState" );
                }
            else if ( localName.CompareF( KIptvRssMediaRating ) == 0 )
                {
                iState = EIptvRssItemRatingState;
                iMediaRatingScheme = EMediaRatingSimple;
                HandleEnclosureL( aAttributes );
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemRatingState" );
                }
            else if ( localName.CompareF( KIptvRssMediaCategory ) == 0 )
                {
                if ( iMediaPrefix )
                    {
                    iState = EIptvRssItemMediaCategoryState;
                    HandleEnclosureL( aAttributes );
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemState to EIptvRssItemMediaCategoryState" );
                    }
                }
            }
            break;

        case EIptvRssItemDescriptionState:
            break;

        case EIptvRssItemLinkState:
            break;

        case EIptvRssItemTitleState:
            break;

        case EIptvRssItemGuidState:
            break;

        case EIptvRssItemMediaGroupState:
            {
            //lint -e{961} Else block not needed, default is no operation.
            if ( localName.CompareF( KIptvRssDescription ) == 0 )
                {
                iState = EIptvRssItemDescriptionState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemMediaGroupState to EIptvRssItemDescriptionState" );
                }
            else if ( localName.CompareF( KIptvRssTitle ) == 0 )
                {
                iState = EIptvRssItemTitleState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemMediaGroupState to EIptvRssItemTitleState" );
                }
            else if ( localName.CompareF( KIptvRssMediaContent ) == 0 )
                {
                iState = EIptvRssItemMediaContentState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemMediaGroupState to EIptvRssItemMediaContentState" );
                HandleEnclosureL( aAttributes );
                }
            else if ( localName.CompareF( KIptvRssThumbnail ) == 0 )
                {
                iState = EIptvRssItemMediaThumbnailState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemMediaGroupState to EIptvRssItemMediaThumbnailState" );
                HandleEnclosureL( aAttributes );
                }
            else if ( localName.CompareF( KIptvRssMediaCopyright ) == 0 )
                {
                iState = EIptvRssItemMediaCopyrightState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemMediaGroupState to EIptvRssItemMediaCopyrightState" );
                }
            else if ( localName.CompareF( KIptvRssMediaRating ) == 0 )
                {
                iState = EIptvRssItemRatingState;
                iMediaRatingScheme = EMediaRatingSimple;
                HandleEnclosureL( aAttributes );
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemMediaGroupState to EIptvRssItemRatingState" );
                }
            else if ( localName.CompareF( KIptvRssMediaCategory ) == 0 )
                {
                if ( SetCategoryState( ECategoryFound ) )
                    {
                    iState = EIptvRssItemMediaCategoryState;
                    HandleEnclosureL( aAttributes );
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaGroupState to EIptvRssItemMediaCategoryState" );
                    }
                }
            }
            break;

        default:
            break;
        }

    CleanupStack::PopAndDestroy( name );
    name = NULL;

    iMediaPrefix = EFalse;
    }

// --------------------------------------------------------------------------
// Callback to indicate the element parsing completion.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnEndElementL(
    const RTagInfo& aElement,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::OnEndElementL" );

    if ( aErrorCode != KErrNone )
        {
        iPlugin.SetParsingFailed();
        return;
        }

    TInt dataToCopy = Min(
        aElement.LocalName().DesC().Length(),
        KIptvRssItemNameMaxLength );
    HBufC* name = HBufC::NewLC( dataToCopy );
    TPtr localName = name->Des();
    localName.Copy( aElement.LocalName().DesC().Mid( 0, dataToCopy ) );

    // Parse name prefix.
    iMediaPrefix = EFalse;
    dataToCopy = Min(
        aElement.Prefix().DesC().Length(),
        KIptvRssItemNameMaxLength );
    if ( 0 < dataToCopy )
        {
        HBufC* prefix = HBufC::NewLC( dataToCopy );
        TPtr prefixName = prefix->Des();
        prefixName.Copy( aElement.Prefix().DesC().Mid( 0, dataToCopy ) );
        if ( prefixName.CompareF( KIptvRssMedia ) == 0 )
            {
            iMediaPrefix = ETrue;
            }
        CleanupStack::PopAndDestroy( prefix );
        }

    switch ( iState )
        {
        case EIptvRssIdleState:
            break;

        case EIptvRssState:
            {
            if ( localName.CompareF( KIptvRssRss ) == 0 )
                {
                iState = EIptvRssIdleState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssState to EIptvRssIdleState" );
                }
            }
            break;

        case EIptvRssChannelState:
            {
            if ( localName.CompareF( KIptvRssChannel ) == 0 )
                {
                SetCategoryState( EChannelCompleted );
                if ( 0 == iChannelCategoryKey )
                    {
                    // Channel category can be deleted when not used.
                    // Otherwise it has been appended to iCategoryTable and
                    // can not be deleted here.
                    delete iChannelCategory;
                    }
                iChannelCategory = NULL;
                iChannelCategoryKey = 0;
                iState = EIptvRssState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelState to EIptvRssState" );
                }
            }
            break;

        case EIptvRssChannelDescriptionState:
            {
            if ( localName.CompareF( KIptvRssDescription ) == 0 )
                {
                CompleteDescription();
                iState = EIptvRssChannelState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelDescriptionState to EIptvRssChannelState" );
                }
            }
            break;

        case EIptvRssChannelLinkState:
            {
            if ( localName.CompareF( KIptvRssLink ) == 0 )
                {
                CompleteLink();
                iState = EIptvRssChannelState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelLinkState to EIptvRssChannelState" );
                }
            }
            break;

        case EIptvRssChannelTitleState:
            {
            if ( localName.CompareF( KIptvRssTitle ) == 0 )
                {
                CompleteTitle();
                iState = EIptvRssChannelState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelTitleState to EIptvRssChannelState" );
                }
            }
            break;

        case EIptvRssChannelImageUrlState:
            {
            if ( localName.CompareF( KIptvRssUrl ) == 0 )
                {
                if ( iBuffer )
                    {
                    AddServiceToDlQueueL( iBuffer->Des() );
                    }

                iState = EIptvRssChannelImageState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelImageUrlState to EIptvRssChannelImageState" );
                }
            }
            break;

        case EIptvRssChannelImageState:
            {
            if ( localName.CompareF( KIptvRssImage ) == 0 )
                {
                iState = EIptvRssChannelState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssChannelImageState to EIptvRssChannelState " );
                }
            }
            break;

        case EIptvRssItemState:
            {
            if ( localName.CompareF( KIptvRssItem ) == 0 )
                {
                SetCategoryState( EItemCompleted );
                PriorizeL();
                if ( 0 == iCurrentCategoryKeys.Count() )
                    {
                    SetChannelCategoryL(); // default when others not set
                    }
                AddContentL();

                iMediaDescriptionFixed = EFalse;
                iMediaTitleFixed = EFalse;
                iMediaRatingFixed = EFalse;
                iMediaCopyrightFixed = EFalse;
                iMediaThumbnailFixed = EFalse;

                iState = EIptvRssChannelState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemState to EIptvRssChannelState" );
                }
            }
            break;

        case EIptvRssItemDescriptionState:
            {
            if ( localName.CompareF( KIptvRssDescription ) == 0 )
                {
                CompleteDescription();
                if ( iMediaGroup )
                    {
                    iState = EIptvRssItemMediaGroupState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemDescriptionState to EIptvRssItemMediaGroupState" );
                    }
                else
                    {
                    iState = EIptvRssItemState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemDescriptionState to EIptvRssItemState" );
                    }
                }
            }
            break;

        case EIptvRssItemLinkState:
            {
            if ( localName.CompareF( KIptvRssLink ) == 0 )
                {
                CompleteLink();
                iState = EIptvRssItemState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemLinkState to EIptvRssItemState" );
                }
            }
            break;

        case EIptvRssItemTitleState:
            {
            if ( localName.CompareF( KIptvRssTitle ) == 0 )
                {
                CompleteTitle();
                if ( iMediaGroup )
                    {
                    iState = EIptvRssItemMediaGroupState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemTitleState to EIptvRssItemMediaGroupState" );
                    }
                else
                    {
                    iState = EIptvRssItemState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemTitleState to EIptvRssItemState" );
                    }
                }
            }
            break;

        case EIptvRssItemEnclosureState:
            {
            if ( localName.CompareF( KIptvRssEnclosure ) == 0 )
                {
                iState = EIptvRssItemState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemEnclosureState to EIptvRssItemState" );
                }
            }
            break;

        case EIptvRssItemGuidState:
            {
            if ( localName.CompareF( KIptvRssGuid ) == 0 )
                {
                CompleteGuid();
                iState = EIptvRssItemState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemGuidState to EIptvRssItemState" );
                }
            }
            break;

        case EIptvRssItemPubDate:
            {
            if ( localName.CompareF( KIptvRssPubDate ) == 0 )
                {
                CompletePubDate();
                iState = EIptvRssItemState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemPubDate to EIptvRssItemState" );
                }
            }
            break;

        case EIptvRssItemMediaContentState:
            {
            if ( localName.CompareF( KIptvRssMediaContent ) == 0 )
                {
                if ( iMediaGroup )
                    {
                    iState = EIptvRssItemMediaGroupState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaContentState to EIptvRssItemMediaGroupState" );
                    }
                else
                    {
                    iMediaGroup = EFalse;
                    iState = EIptvRssItemState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaContentState to EIptvRssItemState" );
                    }
                }
            }
            break;

        case EIptvRssItemMediaGroupState:
            {
            if ( localName.CompareF( KIptvRssMediaGroup ) == 0 )
                {
                iMediaGroup = EFalse;
                iState = EIptvRssItemState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemMediaGroupState to EIptvRssItemState" );
                }
            }
            break;

        case EIptvRssItemMediaThumbnailState:
            {
            if ( localName.CompareF( KIptvRssThumbnail ) == 0 )
                {
                if ( iMediaGroup )
                    {
                    iState = EIptvRssItemMediaGroupState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaThumbnailState to EIptvRssItemMediaGroupState" );
                    }
                else
                    {
                    iMediaGroup = EFalse;
                    iState = EIptvRssItemState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaThumbnailState to EIptvRssItemState" );
                    }
                }
            }
            break;

        case EIptvRssItemAuthorState:
            {
            if ( localName.CompareF( KIptvRssAuthor ) == 0 )
                {
                CompleteAuthor();
                iState = EIptvRssItemState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemAuthorState to EIptvRssItemState" );
                }
            }
            break;

        case EIptvRssItemMediaCopyrightState:
            {
            if ( localName.CompareF( KIptvRssMediaCopyright ) == 0 )
                {
                CompleteCopyright();
                if ( iMediaGroup )
                    {
                    iState = EIptvRssItemMediaGroupState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaCopyrightState to EIptvRssItemMediaGroupState" );
                    }
                else
                    {
                    iState = EIptvRssItemState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaCopyrightState to EIptvRssItemState" );
                    }
                }
            }
            break;

        case EIptvRssItemRatingState:
            {
            if ( localName.CompareF( KIptvRssMediaRating ) == 0 )
                {
                CompleteRating();
                if ( iMediaGroup )
                    {
                    iState = EIptvRssItemMediaGroupState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemRatingState to EIptvRssItemMediaGroupState" );
                    }
                else
                    {
                    iState = EIptvRssItemState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemRatingState to EIptvRssItemState" );
                    }
                }
            }
            break;

        case EIptvRssItemMediaCategoryState:
            {
            if ( localName.CompareF( KIptvRssMediaCategory ) == 0 )
                {
                CompleteCategoryL();
                if ( iMediaGroup )
                    {
                    iState = EIptvRssItemMediaGroupState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaCategoryState to EIptvRssItemMediaGroupState" );
                    }
                else
                    {
                    iState = EIptvRssItemState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaCategoryState to EIptvRssItemState" );
                    }
                }
            }
            break;

        case EIptvRssItemMediaCategoryEnclosureState:
            {
            if ( localName.CompareF( KIptvRssMediaCategory ) == 0 )
                {
                // Category already got as label.
                if ( iMediaGroup )
                    {
                    iState = EIptvRssItemMediaGroupState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaCategoryEnclosureState to EIptvRssItemMediaGroupState" );
                    }
                else
                    {
                    iState = EIptvRssItemState;
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::State changed from EIptvRssItemMediaCategoryEnclosureState to EIptvRssItemState" );
                    }
                }
            }
            break;

        default:
            break;
        }

    CleanupStack::PopAndDestroy( name );
    name = NULL;

    delete iBuffer;
    iBuffer = NULL;

    iMediaPrefix = EFalse;
    }

// --------------------------------------------------------------------------
// Callback that sends the content of the element.
// Not all the content may be returned in one go. The data may be sent in
// chunks. When an OnEndElementL is received this means there is no more
// content to be sent.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnContentL(
    const TDesC8& aBytes,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::OnContentL" );

    if ( aErrorCode != KErrNone )
        {
        iPlugin.SetParsingFailed();
        return;
        }

    // Number of new bytes.
    TInt length = aBytes.Length();

    HBufC* convBuf = HBufC::NewLC( length );
    TPtr16 convPtr = convBuf->Des();

    // Convert new bytes.
    CnvUtfConverter::ConvertToUnicodeFromUtf8( convPtr, aBytes );

    // Total number of bytes.
    if ( iBuffer )
        {
        length = length + iBuffer->Length();
        }

    // Create new buffer.
    HBufC* newBuf = HBufC::NewLC( length );
    TPtr16 newPtr = newBuf->Des();

    // Append and delete old buffer
    if ( iBuffer )
        {
        newPtr.Append( iBuffer->Des() );
        delete iBuffer;
        iBuffer = NULL;
        }

    newPtr.Append( convPtr );
    iBuffer = newBuf;

    CleanupStack::Pop( newBuf );
    CleanupStack::PopAndDestroy( convBuf );
    convBuf = NULL;
    }

// --------------------------------------------------------------------------
// Notification of the beginning of the scope of a prefix-URI Namespace
// mapping.This method is always called before the corresponding
// OnStartElementL method.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnStartPrefixMappingL(
    const RString& /*aPrefix*/,
    const RString& /*aUri*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::OnStartPrefixMappingL" );

    if ( aErrorCode != KErrNone )
        {
        iPlugin.SetParsingFailed();
        }
    }

// --------------------------------------------------------------------------
// Notification of the end of the scope of a prefix-URI mapping.
// This method is called after the corresponding DoEndElementL method.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnEndPrefixMappingL(
    const RString& /*aPrefix*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::OnEndPrefixMappingL" );

    if ( aErrorCode != KErrNone )
        {
        iPlugin.SetParsingFailed();
        }
    }

// --------------------------------------------------------------------------
// Notification of ignorable whitespace in element content.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnIgnorableWhiteSpaceL(
    const TDesC8& /*aBytes*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::OnIgnorableWhiteSpaceL" );

    if ( aErrorCode != KErrNone )
        {
        iPlugin.SetParsingFailed();
        }
    }

// --------------------------------------------------------------------------
// Notification of a skipped entity. If the parser encounters an
// external entity it does not need to expand it - it can return the
// entity as aName for the client to deal with.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnSkippedEntityL(
    const RString& /*aName*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::OnSkippedEntityL" );

    if ( aErrorCode != KErrNone )
        {
        iPlugin.SetParsingFailed();
        }
    }

// --------------------------------------------------------------------------
// Notification of a processing instruction.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnProcessingInstructionL(
    const TDesC8& /*aTarget*/,
    const TDesC8& /*aData*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::OnProcessingInstructionL" );

    if ( aErrorCode != KErrNone )
        {
        iPlugin.SetParsingFailed();
        }
    }

// --------------------------------------------------------------------------
// Notification of XML file parsing is failure.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::OnError( TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "RSS Plugin::CIptvXmlContentHandler::OnError" );

    switch ( aErrorCode )
        {
        case EXmlParserError:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlParserError" );
            break;

        case EXmlSyntax:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlSyntax" );
            break;

        case EXmlNoElements:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlNoElements" );
            break;

        case EXmlInvalidToken:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlInvalidToken" );
            break;

        case EXmlUnclosedToken:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlUnclosedToken" );
            break;

        case EXmlPartialChar:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlPartialChar" );
            break;

        case EXmlTagMismatch:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlTagMismatch" );
            break;

        case EXmlDuplicateAttribute:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlDuplicateAttribute" );
            break;

        case EXmlJunkAfterDocElement:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlJunkAfterDocElement" );
            break;

        case EXmlPeRef:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlPeRef" );
            break;

        case EXmlUndefinedEntity:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlUndefinedEntity" );
            break;

        case EXmlRecursiveEntity:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlRecursiveEntity" );
            break;

        case EXmlAsyncEntity:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlAsyncEntity" );
            break;

        case EXmlBadCharRef:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlBadCharRef" );
            break;

        case EXmlBinaryEntityRef:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlBinaryEntityRef" );
            break;

        case EXmlAttributeExternalEntityRef:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlAttributeExternalEntityRef" );
            break;

        case EXmlMisplacedPi:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlMisplacedPi" );
            break;

        case EXmlUnknownEncoding:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlUnknownEncoding" );
            break;

        case EXmlIncorrectEncoding:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlIncorrectEncoding" );
            break;

        case EXmlUnclosedCdata:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlUnclosedCdata" );
            break;

        case EXmlExternalEntityHandling:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlExternalEntityHandling" );
            break;

        case EXmlNotStandalone:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlNotStandalone" );
            break;

        case EXmlUnexpectedState:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlUnexpectedState" );
            break;

        case EXmlEntityDeclInPe:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlEntityDeclInPe" );
            break;

        case EXmlDtdRequired:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlDtdRequired" );
            break;

        case EXmlFeatureLockedWhileParsing:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- EXmlFeatureLockedWhileParsing" );
            break;

        default:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::OnError --- Unknown error" );
            break;
        }

    iPlugin.SetParsingFailed();
    }

// --------------------------------------------------------------------------
// Obtains the interface matching the specified uid.
// --------------------------------------------------------------------------
//
TAny* CIptvXmlContentHandler::GetExtendedInterface( const TInt32 /*aUid*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::GetExtendedInterface" );

    return NULL;
    }

// --------------------------------------------------------------------------
// Handles item copyright value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CompleteCopyright()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CompleteCopyright" );

    if ( iBuffer )
        {
        // Media Description should override other Description values.
        if ( iMediaPrefix || iMediaGroup || !iMediaCopyrightFixed )
            {
            TInt descLength = iBuffer->Length();
            if ( iState == EIptvRssItemMediaCopyrightState )
                {
                TInt dataToCopy =
                    Min( descLength, KIptvEpgContentCopyrightLength );
                if ( iContent )
                    {
                    iContent->iCopyright.Copy( iBuffer->Mid( 0, dataToCopy ) );
                    iMediaCopyrightFixed = iMediaPrefix || iMediaGroup;
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// Handles parent rating value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CompleteRating()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CompleteRating" );

    // Media Rating should override other Rating values.
    if ( iBuffer && ( iMediaPrefix || iMediaGroup || !iMediaRatingFixed ) )
        {
        if ( iState == EIptvRssItemRatingState )
            {
            if ( EMediaRatingSimple == iMediaRatingScheme )
                {
                if ( 0 == iBuffer->CompareF( KIptvRssMediaAdult ) )
                    {
                    iContent->iRatingAge = KRatingAgeAdult;
                    }
                else
                    {
                    iContent->iRatingAge = KRatingAgeUnlimited;
                    }
                }
            else if ( EMediaRatingNum == iMediaRatingScheme )
                {
                TBuf<KIptvMediaRatingAgeLength> ageValue;
                TInt16 ratingAge( KRatingAgeUnlimited );
                TInt dataToCopy( Min(
                    KIptvMediaRatingAgeLength, iBuffer->Length() ) );
                ageValue.Copy( iBuffer->Mid( 0, dataToCopy ) );

                if ( ageValue.Compare( KIptvEmptyDes ) != 0 )
                    {
                    TLex age( ageValue );
                    age.Val( ratingAge );
                    }

                if ( KRatingAgeAdult < ratingAge )
                    {
                    iContent->iRatingAge = KRatingAgeAdult;
                    }
                else if ( 0 > ratingAge )
                    {
                    iContent->iRatingAge = KRatingAgeUnlimited;
                    }
                else
                    {
                    iContent->iRatingAge = ratingAge;
                    }
                }
            else
                {
                iContent->iRatingAge = KRatingAgeUnlimited;
                }

            iMediaRatingFixed = iMediaPrefix || iMediaGroup;
            }
        }
    }

// --------------------------------------------------------------------------
// Handles item category value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CompleteCategoryL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CompleteCategoryL" );

    if ( iBuffer )
        {
        if ( EIptvRssItemMediaCategoryState == iState )
            {
            SetItemCategoryL( iBuffer );
            }
        }
    }

// --------------------------------------------------------------------------
// Handles item category value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CompleteCategoryL( const HBufC* aName )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CompleteCategoryL" );

    if ( aName )
        {
        if ( EIptvRssItemMediaCategoryEnclosureState == iState )
            {
            SetItemCategoryL( aName );
            }
        }
    }

// --------------------------------------------------------------------------
// Handles item author value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CompleteAuthor()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CompleteAuthor" );

    if ( iBuffer )
        {
        TInt descLength = iBuffer->Length();
        if ( iState == EIptvRssItemAuthorState )
            {
            TInt dataToCopy = Min( descLength, KIptvEpgContentAuthorLength );
            if ( iContent )
                {
                iContent->iAuthor.Copy( iBuffer->Mid( 0, dataToCopy ) );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// Handles item and category description value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CompleteDescription()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CompleteDescription" );

    if ( iBuffer )
        {
        TInt descLength = iBuffer->Length();
        //lint -e{961} Else block not needed, default is no operation.
        if ( iState == EIptvRssChannelDescriptionState )
            {
            TInt dataToCopy =
                Min( descLength, KIptvEpgCategoryDescriptionMaxLength );
            if ( iChannelCategory )
                {
                iChannelCategory->iDescription.Copy(
                    iBuffer->Mid( 0, dataToCopy ) );
                }
            }
        else if ( iState == EIptvRssItemDescriptionState )
            {
            // Media Description should override other Description values.
            if ( iMediaPrefix || iMediaGroup || !iMediaDescriptionFixed )
                {
                TInt dataToCopy =
                    Min( descLength, KIptvEpgContentDescriptionMaxLength );
                if ( iContent )
                    {
                    iContent->iDescription.Copy(
                        iBuffer->Mid( 0, dataToCopy ) );
                    iMediaDescriptionFixed = iMediaPrefix || iMediaGroup;
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// Handles item and category title value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CompleteTitle()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CompleteTitle" );

    if ( iBuffer )
        {
        TInt descLength = iBuffer->Length();
        //lint -e{961} Else block not needed, default is no operation.
        if ( iState == EIptvRssChannelTitleState )
            {
            TInt dataToCopy =
                Min( descLength , KIptvEpgCategoryNameMaxLength );
            if ( iChannelCategory )
                {
                iChannelCategory->iName.Copy(
                    iBuffer->Mid( 0, dataToCopy ) );
                }
            }
        else if ( iState == EIptvRssItemTitleState )
            {
            // Media Title should override other Title values.
            if ( iMediaPrefix || iMediaGroup || !iMediaTitleFixed )
                {
                TInt dataToCopy =
                    Min( descLength, KIptvEpgContentNameMaxLength );
                if ( iContent )
                    {
                    iContent->iName.Copy( iBuffer->Mid( 0, dataToCopy ) );
                    iMediaTitleFixed = iMediaPrefix || iMediaGroup;
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// Handles item url link value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CompleteLink()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CompleteLink" );

    if ( iBuffer )
        {
        TInt descLength = iBuffer->Length();
        if ( iState == EIptvRssItemLinkState )
            {
            if ( iContent )
                {
                TInt dataToCopy = Min(
                    descLength,
                    KIptvEpgContentBrowserUrlMaxLength );

                if ( dataToCopy <= KIptvUrlMaxLength )
                    {
                    iContent->iBrowserUrl.Copy( iBuffer->Mid( 0, dataToCopy ) );
                    }
                }
            }
        }
    }


// --------------------------------------------------------------------------
// Handles item publish time value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CompletePubDate()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CompletePubDate" );

    if ( iState == EIptvRssItemPubDate )
        {
        TTime time;
        TRAPD( error, CIptvXmlDateTime::ParseGmtL( iBuffer, time ) );
        if ( error == KErrNone )
            {
            if ( iContent )
                {
                iContent->iPubDate = time;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// Handles item guid value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CompleteGuid()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CompleteGuid" );

    if ( iBuffer )
        {
        TInt descLength = iBuffer->Length();
        if ( iState == EIptvRssItemGuidState )
            {
            if ( iContent )
                {
                TInt dataToCopy = Min( descLength, KIptvIdMaxLength );
                iContent->iId.Copy( iBuffer->Right( dataToCopy ) );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// Handles element attributes.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::HandleEnclosureL(
    const RAttributeArray& aAttributeList )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::HandleEnclosureL" );

    CIptvMediaContent* content( CIptvMediaContent::NewL() );
    CleanupStack::PushL( content );

    TInt count = aAttributeList.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        // Name
        const RTagInfo& tag = aAttributeList[i].Attribute();
        const TDesC8& n = tag.LocalName().DesC();
        TInt dataToCopy = Min( n.Length(), KIptvRssItemNameMaxLength );
        HBufC* name = HBufC::NewLC( dataToCopy );
        TPtr namePtr = name->Des();
        namePtr.Copy( n.Mid( 0, dataToCopy ) );

        // Value
        const TDesC8& v = aAttributeList[i].Value().DesC();
        dataToCopy =
            Min( v.Length(), KIptvRssEnclosureParameterMaxValueLength );
        HBufC* value = HBufC::NewLC( dataToCopy );
        TPtr valuePtr = value->Des();
        valuePtr.Copy( v.Mid( 0, dataToCopy ) );

        TInt valueLength = valuePtr.Length();

        // RSS 2.0
        //lint -e{961} Else block not needed, default is no operation.
        if ( iState == EIptvRssItemEnclosureState )
            {
            if ( iContent )
                {
                // Url
                //lint -e{961} Else block not needed, default is no operation.
                if ( namePtr.CompareF( KIptvRssUrl ) == 0 &&
                     valueLength <= KIptvUrlMaxLength )
                    {
                    content->SetMediaContentUrlL( valuePtr );
                    content->iExpression = CIptvMediaContent::EFull;

                    // Set download type according protocol,
                    // mime-type can change this later
                    if ( valuePtr.Left( KIptvProtocolNameLength ).CompareF(
                         KIptvRssHttp ) == 0 )
                        {
                        content->iDownloadType = EIptvDownloadTypeImmediate;
                        }
                    else if ( valuePtr.Left( KIptvProtocolNameLength ).CompareF(
                              KIptvRssRtsp ) == 0 )
                        {
                        content->iDownloadType = EIptvDownloadTypeStream;
                        }
                    }
                // Type
                else if ( namePtr.CompareF( KIptvRssType ) == 0 )
                    {
                    dataToCopy = Min( valueLength, KIptvCAMimeTypeLength );
                    content->iMimeType.Copy( valuePtr.Mid( 0, dataToCopy ) );
                    }
                // Length
                else if ( namePtr.CompareF( KIptvRssLength ) == 0 )
                    {
                    if ( valuePtr.Compare( KIptvEmptyDes ) != 0 )
                        {
                        TLex l( valuePtr );
                        l.Val( content->iFileSize, EDecimal );
                        }
                    }
                }
            }
        else if ( iState == EIptvRssChannelImageState )
            {
            if ( namePtr.CompareF( KIptvRssHref ) == 0 )
                {
                AddServiceToDlQueueL( valuePtr );
                }
            }
        else if ( iState == EIptvRssState )
            {
            if ( namePtr.CompareF( KIptvRssMedia ) == 0 )
                {
                if ( valuePtr.CompareF( KIptvYahooRss ) == 0 )
                    {
                    iYahoo = ETrue;
                    }
                }
            }
        // Yahoo media RSS
        else if ( iState == EIptvRssItemMediaThumbnailState )
            {
            // Media Description should override other Description values.
            if ( iMediaPrefix || iMediaGroup || !iMediaThumbnailFixed )
                {
                if ( namePtr.CompareF( KIptvRssUrl ) == 0 &&
                    valueLength <= KIptvUrlMaxLength )
                    {
                    dataToCopy =
                        Min( valueLength, KIptvEpgContentIconFilePathMaxLength );
                    iContent->iIconFilePath.Copy( valuePtr.Mid( 0, dataToCopy ) );
                    iContent->iIsIconFilePathUrl = ETrue;
                    iMediaThumbnailFixed = iMediaPrefix || iMediaGroup;
                    }
                }
            }
        else if ( iState == EIptvRssItemMediaContentState )
            {
            if ( iContent )
                {
                // url
                //lint -e{961} Else block not needed, default is no operation.
                if ( namePtr.CompareF( KIptvRssUrl ) == 0 &&
                     valueLength <= KIptvUrlMaxLength )
                    {
                    content->SetMediaContentUrlL( valuePtr );

                    // Set download type according protocol,
                    // mime-type can change this later
                    if ( valuePtr.Left( KIptvProtocolNameLength ).CompareF(
                         KIptvRssHttp ) == 0 )
                        {
                        content->iDownloadType = EIptvDownloadTypeImmediate;
                        }
                    else if ( valuePtr.Left( KIptvProtocolNameLength ).CompareF(
                              KIptvRssRtsp ) == 0 )
                        {
                        content->iDownloadType = EIptvDownloadTypeStream;
                        }
                    }
                // filesize
                else if ( namePtr.CompareF( KIptvRssMediaFileSize ) == 0 )
                    {
                    if ( valuePtr.Compare( KIptvEmptyDes ) != 0 )
                        {
                        TLex l( valuePtr );
                        l.Val( content->iFileSize, EDecimal );
                        }
                    }
                // mime-type
                else if ( namePtr.CompareF( KIptvRssType ) == 0 )
                    {
                    dataToCopy = Min( valueLength, KIptvCAMimeTypeLength );
                    content->iMimeType.Copy( valuePtr.Mid( 0, dataToCopy ) );
                    }
                // medium
                else if ( namePtr.CompareF( KIptvRssMediaMedium ) == 0 )
                    {
                    dataToCopy = Min( valueLength, KIptvMediumMaxLength );
                    content->iMedium.Copy( valuePtr.Mid( 0, dataToCopy ) );
                    }
                // isDefault
                else if ( namePtr.CompareF( KIptvRssMediaIsDefault ) == 0 )
                    {
                    if ( valuePtr.CompareF( KIptvTrue ) == 0 )
                        {
                        content->iIsDefault = ETrue;
                        }
                    else
                        {
                        content->iIsDefault = EFalse;
                        }
                    }
                // expression
                else if ( namePtr.CompareF( KIptvRssMediaExpression ) == 0 )
                    {
                    if ( valuePtr.CompareF( KIptvRssMediaSample ) == 0 )
                        {
                        content->iExpression = CIptvMediaContent::ESample;
                        }
                    else if ( valuePtr.CompareF( KIptvRssMediaFull ) == 0 )
                        {
                        content->iExpression = CIptvMediaContent::EFull;
                        }
                    else if ( valuePtr.CompareF( KIptvRssMediaNonStop ) == 0 )
                        {
                        content->iExpression = CIptvMediaContent::ENonStop;
                        }
                    else
                        {
                        content->iExpression = CIptvMediaContent::EFull;
                        }
                    }
                // bitrate
                else if ( namePtr.CompareF( KIptvRssMediaBitrate ) == 0 )
                    {
                    TInt bitrate = 0;
                    if ( valuePtr.Compare( KIptvEmptyDes ) != 0 )
                        {
                        TLex l( valuePtr );
                        l.Val( bitrate );
                        }

                    if ( 0 < bitrate )
                        {
                        content->iBitrate = bitrate;
                        }
                    }
                // framerate
                else if ( namePtr.CompareF( KIptvRssMediaFramerate ) == 0 )
                    {
                    TInt framerate = 0;
                    if ( valuePtr.Compare( KIptvEmptyDes ) != 0 )
                        {
                        TLex l( valuePtr );
                        l.Val( framerate );
                        }

                    if ( 0 < framerate )
                        {
                        content->iFramerate = framerate;
                        }
                    }
                // samplingrate
                else if ( namePtr.CompareF( KIptvRssMediaSamplingrate ) == 0 )
                    {
                    TInt samplingrate = 0;
                    if ( valuePtr.Compare( KIptvEmptyDes ) != 0 )
                        {
                        TLex l( valuePtr );
                        l.Val( samplingrate );
                        }

                    if ( 0 < samplingrate )
                        {
                        content->iSamplingrate = samplingrate;
                        }
                    }
                // duration
                else if ( namePtr.CompareF( KIptvRssMediaDuration ) == 0 )
                    {
                    TInt duration = 0;
                    if ( valuePtr.Compare( KIptvEmptyDes ) != 0 )
                        {
                        TLex l( valuePtr );
                        l.Val( duration );
                        }

                    if ( 0 < duration )
                        {
                        content->iDuration = duration;
                        }
                    }
                // height
                else if ( namePtr.CompareF( KIptvRssMediaHeight ) == 0 )
                    {
                    TInt height = 0;
                    if ( valuePtr.Compare( KIptvEmptyDes ) != 0 )
                        {
                        TLex l( valuePtr );
                        l.Val( height );
                        }

                    if ( 0 < height )
                        {
                        content->iHeight = height;
                        }
                    }
                // width
                else if ( namePtr.CompareF( KIptvRssMediaWidth ) == 0 )
                    {
                    TInt width = 0;
                    if ( valuePtr.Compare( KIptvEmptyDes ) != 0 )
                        {
                        TLex l( valuePtr );
                        l.Val( width );
                        }

                    if ( 0 < width )
                        {
                        content->iWidth = width;
                        }
                    }
                // language
                else if ( namePtr.CompareF( KIptvRssMediaLanguage ) == 0 )
                    {
                    dataToCopy = Min( valueLength, KIptvCALanguageLength );
                    content->iLanguage.Copy( valuePtr.Mid( 0, dataToCopy ) );
                    }
                }
            }
        else if ( EIptvRssItemRatingState == iState )
            {
            if ( 0 == namePtr.CompareF( KIptvRssMediaRatingScheme ) )
                {
                if ( 0 == valuePtr.CompareF( KIptvRssMediaRatingSchemeSimple ) )
                    {
                    iMediaRatingScheme = EMediaRatingSimple;
                    }
                else if ( 0 == valuePtr.CompareF( KIptvRssMediaRatingSchemeNum ) )
                    {
                    iMediaRatingScheme = EMediaRatingNum;
                    }
                else
                    {
                    iMediaRatingScheme = EMediaRatingSimple;
                    }
                }
            }
        else if ( EIptvRssItemMediaCategoryState == iState )
            {
            if ( 0 == namePtr.CompareF( KIptvRssMediaLabel ) )
                {
                iState = EIptvRssItemMediaCategoryEnclosureState;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::State changed from EIptvRssItemMediaCategoryState to EIptvRssItemMediaCategoryEnclosureState" );
                CompleteCategoryL( value );
                }
            }

        // Value and Name.
        CleanupStack::PopAndDestroy( KIptvPopValueAndName, name );
        value = NULL;
        name = NULL;
        }

    if ( content )
        {
        // Check that URL is not empty
        if ( content->GetMediaContentUrl().Length() > 0 )
            {
            SetDownloadTypeFromMimeType( *content );
            content->PrintL();
            iMediaContents.AppendL( content );
            CleanupStack::Pop( content );
            }
        else
            {
            CleanupStack::PopAndDestroy( content );
            content = NULL;
            }
        }
    }

// --------------------------------------------------------------------------
// Create a new category descriptor.
// --------------------------------------------------------------------------
//
CIptvEpgCategory*  CIptvXmlContentHandler::CreateCategoryL() const
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CreateCategoryL" );

    CIptvEpgCategory* category( NULL );

    category = CIptvEpgCategory::NewL();

    // Key is auto-increment

    // Parent key
    category->iParentKey = KIptvVodContentCategoryRootId;

    // Id
    category->iId = KIptvEmptyDes;

    // Name
    category->iName = KIptvEmptyDes;

    // Icon file Id
    category->iIconFileId = 0; // Obsolete field

    // Is icon file path URL
    category->iIsIconFilePathUrl = ETrue; // Obsolete field

    // Icon file path
    category->iIconFilePath = KIptvEmptyDes; // Obsolete field

    // Description
    category->iDescription = KIptvEmptyDes;

    // Item count
    category->iContentCount = 0;

    return category;
    }

// --------------------------------------------------------------------------
// Set a category for current item.
// When category has been used for some other item it already exists and
// is restored, Otherwise a new one is created and a key value given.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::SetItemCategoryL( const HBufC* aName )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::SetItemCategoryL" );

    // When no name defined, retain default category.
    if ( aName )
        {
        if ( 0 < aName->Length() )
            {
            TUint32 categoryKey( 0 );
            TInt categoryCount = iCategoryTable.Count();

            // Name of the Category candidate.
            HBufC* name = HBufC::NewLC( KIptvEpgCategoryNameMaxLength );
            TPtr16 namePtr = name->Des();
            TInt dataToCopy =
                Min( aName->Length(), KIptvEpgCategoryNameMaxLength );
            namePtr.Copy( aName->Mid( 0, dataToCopy ) );

            // Search category whether already exist.
            for ( TInt i = 0; ( i < categoryCount ) && ( 0 == categoryKey ); i++ )
                {
                if ( 0 == iCategoryTable[i]->iName.CompareF( namePtr ) )
                    {
                    categoryKey = i + 1;
                    }
                }

            if ( 0 != categoryKey )
                {
                // Whether category alreaady exists, add its key for content.
                // Avoid double linking.
                if ( 0 != iCurrentCategoryKeys.Find( categoryKey ) )
                    {
                    iCurrentCategoryKeys.AppendL( categoryKey );
                    }
                }
            else if ( iCategoryCount < KIptvEpgMaxCategoryCount )
                {
                // Whether category not yet exists, add a new one.
                CIptvEpgCategory* newCategory = CreateCategoryL();
                CleanupStack::PushL( newCategory );

                // Use defaults from channel category.
                if ( newCategory )
                    {
                    newCategory->iName.Copy( namePtr );
                    if ( iChannelCategory )
                        {
                        newCategory->iDescription.Copy(
                            iChannelCategory->iDescription );
                        }

                    iCategoryTable.AppendL( newCategory );
                    iCurrentCategoryKeys.AppendL( iCategoryTable.Count() );
                    }

                CleanupStack::Pop( newCategory );
                }
            else
                {
                // New category can not be set, retain selection before.
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::SetItemCategoryL -- Category limit reached! Category not added!" );
                }

            CleanupStack::PopAndDestroy( name );
            name = NULL;
            }
        }
    }

// --------------------------------------------------------------------------
// Set channel as default category for current Item.
// When channel category has already been used, restore it,
// Otherwise save it and give a key value.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::SetChannelCategoryL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::SetChannelCategoryL" );

    if ( iChannelCategory )
        {
        // Check whether channel category has not yet already been used.
        if ( ( 0 == iChannelCategoryKey ) &&
             ( iCategoryCount < KIptvEpgMaxCategoryCount ) )
            {
            iCategoryTable.AppendL( iChannelCategory );
            iChannelCategoryKey = iCategoryTable.Count();
            }

        // Add key for current item content.
        if ( 0 != iChannelCategoryKey )
            {
            iCurrentCategoryKeys.AppendL( iChannelCategoryKey );
            }
        else
            {
            // Channel category can not be set, retain selection before.
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::SetChannelCategoryL -- Category limit reached! Category not added!" );
            }
        }
    }

// --------------------------------------------------------------------------
// Stores all categories used by current itam to database.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::AddCategories()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::AddCategories" );

    TInt categoryCount( iCategoryTable.Count() );

    for ( TInt i = 0; i < categoryCount; i++ )
        {
        TUint32 newKey( 0 );

#ifdef _DEBUG
        TBuf<KIptvEpgCategoryNameMaxLength + 1> name;
        name.Zero();
        name.Copy( iCategoryTable[i]->iName );

        IPTVLOGSTRING3_LOW_LEVEL(
            "RSS Plugin::AddCategories Category '%S' added with %d items",
            &name,
            iCategoryTable[i]->iContentCount );
#endif

        TRAPD(
            error,
            iVodCallback->AddCategoryL( *( iCategoryTable[i] ), newKey ) );
        //lint -e{961} Else block not needed, only error handling here.
        if ( KErrNone != error )
            {
            IPTVLOGSTRING2_LOW_LEVEL( "RSS Plugin::AddCategories Error = %d",
                 error );
            }
        else if ( newKey != ( i + 1 ) )
            {
            IPTVLOGSTRING3_LOW_LEVEL(
                "RSS Plugin::AddCategories Unexpected key %d, %d expected",
                 newKey, i + 1 );
            }
        }

    // Categories saved to database, descriptors no more needed.
    iCategoryTable.ResetAndDestroy();
    if ( iChannelCategory && ( 0 == iChannelCategoryKey ) )
        {
        // Current channel category is not included to iCategoryTable and
        // so has not been deleted above.
        delete iChannelCategory;
        }
    iChannelCategory = NULL;
    iChannelCategoryKey = 0;
    iCurrentCategoryKeys.Reset();
    }

// --------------------------------------------------------------------------
// Add current content to dabase and link it to categories.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::AddContentL()
    {
#ifdef _DEBUG

    TBuf<KIptvEpgContentNameMaxLength + 1> name;
    name.Zero();
    if ( iContent )
        {
        name.Copy( iContent->iName );
        }

    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::AddContentL: '%S'",
        &name );
#endif

    TBool addThis( iContent && ( iMediaContentSet.Count() > 0 ) &&
                   iContent->iBrowserUrl.Length() < KIptvUrlMaxLength );
    TInt itemCategories( iCurrentCategoryKeys.Count() );

    // Check category limits.
    if ( addThis )
        {
        addThis = EFalse;
        if ( iContentCount < KIptvEpgMaxContentCount )
            {
            // Check whether this item fits to any selected category
            for ( TInt key = 0;
                  ( key < itemCategories ) && ( EFalse == addThis );
                  key++ )
                {
                TUint32 categoryKey = iCurrentCategoryKeys[key];

                if ( 0 < categoryKey )
                    {
                    if ( iCategoryTable[categoryKey - 1]->iContentCount <
                         KIptvEpgMaxContentCountInCategory )
                        {
                        addThis = ETrue;
                        }
                    }
                }

            if ( !addThis )
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::AddContentL Content not added! Category limits reached!" );
                }
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::AddContentL Content not added! Limit reached!" );
            IPTVLOGSTRING3_LOW_LEVEL(
                "RSS Plugin::AddContentL Total content count:    %d/%d",
                iContentCount,
                KIptvEpgMaxContentCount );
            }
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "RSS Plugin::AddContentL Content not added! Required info missing!" );
        }

    TUint32 newContentKey( 0 );

    if ( addThis )
        {
        // Save content's thumbnail path
        TBuf<KIptvCAContentUrlLength> thumbnailUrl;
        thumbnailUrl.Copy( iContent->iIconFilePath );

        // Set thumbnail path to zero, we dont want 2k URL's to database
        iContent->iIconFilePath.Zero();

        // Add order to content data.
        // This is the item detection order and is not continuous
        // inside one category. Anyway order is increasing inside each
        // category and  can so be used to sort items of category.
        iContent->iOrder = iContentCount;

        // Get size, duration and language from media content's
        // full version to "parent" content class
        // This becomes redundant information
        TInt count = iMediaContentSet.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            if ( iMediaContentSet[i]->iExpression ==
                 CIptvMediaContent::EFull )
                {
                iContent->iSize = iMediaContentSet[i]->iFileSize;
                iContent->iDuration = iMediaContentSet[i]->iDuration;
                iContent->iLanguage.Copy(
                    iMediaContentSet[i]->iLanguage );
                i = count;
                }
            }

        TInt error = iVodCallback->AddContentL(
            *iContent, iMediaContentSet, newContentKey );
        if ( error == KErrNone )
            {
            iContentCount++;
            AddContentToDlQueueL( newContentKey, thumbnailUrl );

            IPTVLOGSTRING3_LOW_LEVEL(
                "RSS Plugin::AddContentL Content added, total content count: %d/%d",
                iContentCount,
                KIptvEpgMaxContentCount );
            }
        else
            {
            addThis = EFalse;
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin::AddContentL error = %d", error );
            }
        }

    if ( addThis )
        {
        // Add relation to selected categories.
        for ( TInt key = 0; key < itemCategories; key++ )
            {
            TUint32 categoryKey = iCurrentCategoryKeys[key];

            if ( 0 < categoryKey )
                {
                if ( iCategoryTable[categoryKey - 1]->iContentCount <
                     KIptvEpgMaxContentCountInCategory )
                    {
                    iVodCallback->AddRelationL( categoryKey, newContentKey );
                    iCategoryTable[categoryKey - 1]->iContentCount++;

                    IPTVLOGSTRING3_LOW_LEVEL(
                        "RSS Plugin::AddContentL Content count in category:          %d/%d",
                        iCategoryTable[categoryKey - 1]->iContentCount,
                        KIptvEpgMaxContentCountInCategory );
                    }
                }
            }
        }
    else
        {
        // After non added item, cleanup for empty categories.
        // Because empty categories are checked after each rejected item,
        // they can be assumed to be the last ones.
        TInt categoryCount( iCategoryTable.Count() );

        while ( 0 < categoryCount )
            {
            categoryCount--;
            if ( 0 == iCategoryTable[categoryCount]->iContentCount )
                {
                if ( iCategoryTable[categoryCount] != iChannelCategory )
                    {
#ifdef _DEBUG
                    name.Zero();
                    name.Copy( iCategoryTable[categoryCount]->iName );

                    IPTVLOGSTRING2_LOW_LEVEL(
                        "RSS Plugin::AddContentL Empty category '%S' removed ",
                        &name );
#endif
                    delete iCategoryTable[categoryCount];
                    iCategoryTable[categoryCount] = NULL;
                    }
                else
                    {
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::AddContentL Channel category unlinked" );
                    iChannelCategoryKey = 0;
                    iCategoryTable[categoryCount] = NULL;
                    }

                iCategoryTable.Remove( categoryCount );
                }
            else
                {
                categoryCount = 0;
                }
            }
        }

    iMediaContents.ResetAndDestroy();
    iMediaContentSet.Reset();
    iCurrentCategoryKeys.Reset();

    delete iContent;
    iContent = NULL;
    }

// --------------------------------------------------------------------------
// Create a new itme content descriptor.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::CreateContentL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::CreateContentL" );

    delete iContent;
    iContent = NULL;
    iContent = CIptvEpgContent::NewL();

    // Key is auto-increment

    // Id
    iContent->iId = KIptvEmptyDes;

    // Name
    iContent->iName = KIptvEmptyDes;

    // Content protection
    iContent->iContentProtection = 0;

    // Icon file Id
    iContent->iIconFileId = 0;

    // Is icon filepath URL
    iContent->iIsIconFilePathUrl = ETrue;

    // Icon file path
    iContent->iIconFilePath = KIptvEmptyDes;

    // Content file Id
    iContent->iContentFileId = 0;

    // Content description
    iContent->iDescription = KIptvEmptyDes;

    // Author
    iContent->iAuthor = KIptvEmptyDes;

    // Copyright
    iContent->iCopyright = KIptvEmptyDes;

    // Size
    iContent->iSize = 0;

    // Duration
    iContent->iDuration = 0;

    // Language
    iContent->iLanguage = KIptvEmptyDes;

    // rating age by default unlimited.
    iContent->iRatingAge = KRatingAgeUnlimited;
    }

// --------------------------------------------------------------------------
// Adjust download type according to Mime type.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::SetDownloadTypeFromMimeType(
    CIptvMediaContent& aContent ) const
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::SetDownloadTypeFromMimeType" );

    // For some MIME types we force the download type.
    //
    if ( aContent.iMimeType.CompareF( KIptvMimeApplicationSdp ) == 0 )
        {
        aContent.iDownloadType = EIptvDownloadTypeStream;
        }
    }

// --------------------------------------------------------------------------
// Item containing thumbnails is added to download queue.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::AddContentToDlQueueL(
    TUint32 contentKey,
    const TDesC& aThumbnailUrl )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::AddContentToDlQueueL" );

    // If thumbnail path -> add to download queue
    if ( ( aThumbnailUrl.Length() > 0 ) &&
         ( aThumbnailUrl.Length() <= KIptvUrlMaxLength ) &&
         ( aThumbnailUrl.Left( KIptvProtocolNameLength ).CompareF(
            KIptvRssHttp ) == 0 ) )
        {
        CIptvDownloadItem* item = CIptvDownloadItem::NewL();
        CleanupStack::PushL( item );
        item->iThumbnailUrl.Copy( aThumbnailUrl );
        item->iServiceThumbnail = EFalse;
        item->iId = contentKey;
        iPlugin.AddToDlQueueL( item );
        CleanupStack::Pop( item );
        }
    }

// --------------------------------------------------------------------------
// Service image thumbnails url is added to download queue.
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::AddServiceToDlQueueL(
    const TDesC& aThumbnailUrl )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::AddServiceToDlQueueL" );

    // Only first valid occurence is stored as service image.
    if ( !iServiceThumbnailAdded )
        {
        TInt urlLength = aThumbnailUrl.Length();

        // Url can not be truncated, if too long wait a new one.
        // If thumbnail path -> add to download queue
        if ( ( 0 < urlLength ) &&
             ( KIptvEpgCategoryIconFilePathMaxLength >= urlLength ) &&
             ( aThumbnailUrl.Left( KIptvProtocolNameLength ).CompareF(
                KIptvRssHttp ) == 0 ) )
            {
            CIptvDownloadItem* item = CIptvDownloadItem::NewL();
            CleanupStack::PushL( item );
            item->iThumbnailUrl.Copy( aThumbnailUrl );
            item->iServiceThumbnail = ETrue;
            item->iId = iChannelCategoryKey;
            iPlugin.AddToDlQueueL( item );
            CleanupStack::Pop( item );

            iServiceThumbnailAdded = ETrue;
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::AddServiceToDlQueueL Service thumbnail added" );
            }
        }
    }

// --------------------------------------------------------------------------
// CIptvXmlContentHandler::PriorizeL
// --------------------------------------------------------------------------
//
void CIptvXmlContentHandler::PriorizeL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::PriorizeL" );

    TInt count = iMediaContents.Count();
    if ( count > 0 )
        {
        RPointerArray<CIptvMediaContent> previewStreams;
        RPointerArray<CIptvMediaContent> previewDownloads;
        RPointerArray<CIptvMediaContent> streams;
        RPointerArray<CIptvMediaContent> downloads;

        for ( TInt i = 0; i < count; i++ )
            {
            // Preview stream
            //lint -e{961} Else block not needed, default is no operation.
            if ( ( iMediaContents[i]->iExpression == CIptvMediaContent::ESample ) &&
                 ( iMediaContents[i]->iDownloadType == EIptvDownloadTypeStream ) )
                {
                previewStreams.AppendL( iMediaContents[i] );
                }
            // Preview download
            else if ( ( iMediaContents[i]->iExpression == CIptvMediaContent::ESample ) &&
                      ( iMediaContents[i]->iDownloadType == EIptvDownloadTypeImmediate ) )
                {
                previewDownloads.AppendL( iMediaContents[i] );
                }
            // Stream ( full or nonstop )
            else if ( ( iMediaContents[i]->iExpression != CIptvMediaContent::ESample ) &&
                      ( iMediaContents[i]->iDownloadType == EIptvDownloadTypeStream ) )
                {
                streams.AppendL( iMediaContents[i] );
                }
            // Download
            else if ( ( iMediaContents[i]->iExpression == CIptvMediaContent::EFull ) &&
                      ( iMediaContents[i]->iDownloadType == EIptvDownloadTypeImmediate ) )
                {
                downloads.AppendL( iMediaContents[i] );
                }
            }

        if ( iTerminalVideoSupport )
            {
            TInt index = 0;
            if ( previewStreams.Count() > 0 )
                {
                index = iTerminalVideoSupport->SelectMimeTypeL( previewStreams );
                if ( index != KErrNotFound )
                    {
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::CIptvXmlContentHandler::PriorizeL Best preview stream is:" );
                    previewStreams[index]->PrintL();

                    // Add best items of preview streams, preview downloads,
                    // streams and downloads to array
                    iMediaContentSet.AppendL( previewStreams[index] );
                    }
                }
            if ( previewDownloads.Count() > 0 )
                {
                index = iTerminalVideoSupport->SelectMimeTypeL( previewDownloads );
                if ( index != KErrNotFound )
                    {
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::CIptvXmlContentHandler::PriorizeL Best preview download is:" );
                    previewDownloads[index]->PrintL();

                    iMediaContentSet.AppendL( previewDownloads[index] );
                    }
                }
            if ( streams.Count() > 0 )
                {
                index = iTerminalVideoSupport->SelectMimeTypeL( streams );
                if ( index != KErrNotFound )
                    {
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::CIptvXmlContentHandler::PriorizeL Best stream is:" );
                    streams[index]->PrintL();

                    iMediaContentSet.AppendL( streams[index] );
                    }
                }
            if ( downloads.Count() > 0 )
                {
                index = iTerminalVideoSupport->SelectMimeTypeL( downloads );
                if ( index != KErrNotFound )
                    {
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin::CIptvXmlContentHandler::PriorizeL Best download is:" );
                    downloads[index]->PrintL();

                    iMediaContentSet.AppendL( downloads[ index ] );
                    }
                }
            }

        previewStreams.Close();
        previewDownloads.Close();
        streams.Close();
        downloads.Close();
        }
    }

// --------------------------------------------------------------------------
// Advances category state according to given event and decides whether
// current object should be processed.
// --------------------------------------------------------------------------
//
TBool CIptvXmlContentHandler::SetCategoryState( TCategoryStateEvent aEvent )
    {
    IPTVLOGSTRING3_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::SetCategoryState State:%d, Event:%d",
        iCategoryState, aEvent );

    TBool processThis( ETrue ); // By default we process this object

    /**
     * Search category attribute inside an item.
     */
    //lint -e{961} Else block not needed, default is no change.
    if ( ECategoryFound == aEvent )
        {
        iCategoryFound = ETrue;
        }
    else if ( EItemFound == aEvent )
        {
        iCategoryFound = EFalse;
        }

    /**
     * State transitions according to event.
     */
    switch ( iCategoryState )
        {
        /**
         * Not even the first channel found yet.
         */
        case ENoChannel:
            {
            if ( EChannelFound == aEvent )
                {
                iCategoryState = EChannelNoItem;
                }
            }
            break;

        /**
         * No items found from first channel yet.
         */
        case EChannelNoItem:
            {
            //lint -e{961} Else block not needed, default is no state change.
            if ( EItemCompleted == aEvent )
                {
                if ( iCategoryFound )
                    {
                    iCategoryState = EItemsWithCategory;
                    iCategoryFound = EFalse;
                    }
                else
                    {
                    iCategoryState = EItemsNoCategory;
                    }
                }
            else if ( EChannelCompleted == aEvent )
                {
                processThis = EFalse; // Ignore empty channel.
                iCategoryState = ENoChannel;
                }
            }
            break;

        /**
         * Only items with category definition found yet fron first channel.
         */
        case EItemsWithCategory:
            {
            //lint -e{961} Else block not needed, default is no state change.
            if ( EItemCompleted == aEvent )
                {
                if ( iCategoryFound )
                    {
                    iCategoryFound = EFalse;
                    }
                else
                    {
                    iCategoryState = EItemsBoth;
                    }
                }
            else if ( EChannelCompleted == aEvent )
                {
                iCategoryState = ESkipFurtherChannels;
                processThis = EFalse; // Ignore channel name as category.
                }
            }
            break;

        /**
         * Only items without category definition found yet fron first channel.
         */
        case EItemsNoCategory:
            {
            //lint -e{961} Else block not needed, default is no state change.
            if ( EItemCompleted == aEvent )
                {
                if ( iCategoryFound )
                    {
                    iCategoryState = EItemsBoth;
                    iCategoryFound = EFalse;
                    }
                }
            else if ( EChannelCompleted == aEvent )
                {
                iCategoryState = ESkipFurtherCategories;
                }
            }
            break;

        /**
         * First channel contains items both with and without categories.
         */
        case EItemsBoth:
            {
            if ( EChannelCompleted == aEvent )
                {
                iCategoryState = ESkipFurtherChannels;
                }
            }
            break;

        /**
         * We have processed a channel with category definitions, so we are
         * satisfied and ignore further channels.
         */
        case ESkipFurtherChannels:
            {
            if ( EChannelFound == aEvent )
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::SetCategoryState ESkipFurtherChannels" );
                processThis = EFalse;
                }
            }
            break;

        /**
         * We have processed a channel without any category definitions,
         * so we use channels as categories and ignore further category
         * definitions.
         */
        case ESkipFurtherCategories:
            {
            if ( ECategoryFound == aEvent )
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::SetCategoryState SkipFurtherCategories" );
                processThis = EFalse;
                }
            }
            break;

        default:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::SetCategoryState Unknown state" );
            }
        }

    IPTVLOGSTRING3_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::SetCategoryState New State:%d, Process:%d",
        iCategoryState, processThis );

    return processThis;
    }
