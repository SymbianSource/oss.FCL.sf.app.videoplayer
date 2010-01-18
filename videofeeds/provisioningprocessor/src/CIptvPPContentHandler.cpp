/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/




#include <e32base.h>
#include <utf.h>
#include "IptvDebug.h"
#include "iptvlocalisationliterals.h"
#include <bautils.h>
#include <hash.h>

#include "CIptvService.h"
#include "CIptvServices.h"
#include "CIptvPPContentHandler.h"
#include "CIptvProvisioningProcessor.h"

const TInt KIptvIdleState = 0;
const TInt KIptvServiceProviderState = 1;
const TInt KIptvServiceState = 2;
const TInt KIptvServiceDetailsState = 3;
const TInt KIptvTextState = 4;
const TInt KIptvTitleState = 5;
const TInt KIptvDescriptionState = 6;
const TInt KIptvServiceUriState = 7;
const TInt KIptvIconUriState = 8;
const TInt KIptvIapState = 9;
const TInt KIptvServiceAccountUriState = 10;
const TInt KIptvAuthState = 11;
const TInt KIptvUsernameState = 12;
const TInt KIptvPasswordState = 13;
const TInt KIptvAppUIDState = 14;
const TInt KIptvSearchUriState = 15;
const TInt KIptvServiceGroupIdState = 16;
const TInt KIptvUseCategoriesState = 17;
const TInt KIptvUploadProviderState = 18;
const TInt KIptvRecordUriState = 19;

const TInt KIptvBufferMaxSize( 255 );
const TInt KIptvMaxServiceCount( 20 );
const TInt KIptvIapDefaultPriority( 50 );
const TInt KIptvIapBufferLength( 100 );
const TInt KIptvAppUIDMaxSize( 8 ); // Application UID in hex format: 12345678
const TInt KIptvDriveLetterSpace( 2 );

_LIT( KIptvServiceProvider, "ServiceProvider" );
_LIT( KIptvServiceGroupProvider, "ServiceGroupProvider" );
_LIT( KIptvService, "Service" );
_LIT( KIptvServiceFeed, "ServiceFeed" );
_LIT( KIptvServiceGroup, "ServiceGroup" );
_LIT( KIptvServiceDetails, "ServiceDetails" );
_LIT( KIptvUseCategories, "UseCategories" );
_LIT( KIptvAppUID, "AppUID" );
_LIT( KIptvText, "Text" );
_LIT( KIptvTitle, "Title" );
_LIT( KIptvDescription, "Description" );
_LIT( KIptvServiceGroupId, "ServiceGroupId" );
_LIT( KIptvServiceUri, "ServiceURI" );
_LIT( KIptvIconUri, "IconURI" );
_LIT( KIptvUploadProvider, "UploadProvider" );
_LIT( KIptvServiceAccountUriSmallCase, "serviceAccountURI" );
_LIT( KIptvServiceAccountUriCapitalCase, "ServiceAccountURI" );
_LIT( KIptvAuth, "Auth" );
_LIT( KIptvUsername, "UserName" );
_LIT( KIptvPassword, "Password" );
_LIT( KIptvIap, "IAP" );
_LIT( KIptvType, "Type" );
_LIT( KIptvProtocol, "Protocol" );
_LIT( KIptvSearchURISmallCase, "searchURI" );
_LIT( KIptvSearchURICapitalCase, "SearchURI" );
_LIT( KIptvRecordURI, "RecordURI" );
_LIT( KIptvEmpty, "" );

_LIT( KIptvServiceAttrProviderId, "ProviderID" );
_LIT( KIptvTextAttrLang, "lang" );

// Type
_LIT( KIptvVod, "VOD" );
_LIT( KIptvVodCast, "VC" );
_LIT( KIptvLiveTv, "LiveTV" );
_LIT( KIptvBrowser, "Browser" );
_LIT( KIptvSearch, "Search" );
_LIT( KIptvUpload, "Upload" );
_LIT( KIptvApplication, "Application" );
_LIT( KIptvMobileTv, "MobileTv" );
_LIT( KIptvSecureProvisioningServer, "SecureProvisioningServer" );

// Protocol
_LIT( KIptvProtocolRss, "RSS2.0" );
_LIT( KIptvProtocolDvbIpi, "DVB-IPI" );
_LIT( KIptvProtocolHTTPProp, "HTTP-PROP" );   // Application and plugin specific communication mechanism
_LIT( KIptvProtocolHTTP, "HTTP" );            // Browser
_LIT( KIptvProtocolXMLTV, "XMLTV" );
_LIT( KIptvHttpPrefix, "http://" );

_LIT( KIptvYes, "yes" );
_LIT( KIptvOne, "1" );

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::NewL
// Two-phased constructor.
// Create instance of concrete interface implementation
// ---------------------------------------------------------------------------
//
CIptvPPContentHandler* CIptvPPContentHandler::NewL(
    CIptvProvisioningProcessor& aProcessor,
    const RFs& aFs )
    {
    CIptvPPContentHandler* self = new ( ELeave ) CIptvPPContentHandler( aProcessor, aFs );
    return self;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::~CIptvPPContentHandler
// Destructor
// ---------------------------------------------------------------------------
//
CIptvPPContentHandler::~CIptvPPContentHandler()
    {
    delete iBuffer;
    delete iService;
    iServices = NULL;
    delete iPendingIcon;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CIptvPPContentHandler
// C++ default constructor
// ---------------------------------------------------------------------------
//
CIptvPPContentHandler::CIptvPPContentHandler(
    CIptvProvisioningProcessor& aProcessor,
    const RFs& aFs )
  : iState( KIptvIdleState ),
    iServiceCount( 0 ),
    iProcessor( aProcessor ),
    iService( NULL ),
    iBuffer( NULL ),
    iServices( NULL ),
    iError( KErrNone ),
    iServiceTitleState( EIptvServiceTitleNone ),
    iFs( aFs )
    {
    iCurrentDeviceLanguage = User::Language();
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::SetMode
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::SetMode( TBool aLocal )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::SetMode" );

    iLocal = aLocal;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnStartDocumentL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnStartDocumentL(
    const RDocumentParameters& /*aDocParam*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnStartDocumentL" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnEndDocumentL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnEndDocumentL( TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnEndDocumentL" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnStartElementL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnStartElementL(
    const RTagInfo& aElement,
    const RAttributeArray& aAttributes,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnStartElementL" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }

    delete iBuffer;
    iBuffer = NULL;

    TBuf<KIptvBufferMaxSize> localName;
    TInt length = aElement.LocalName().DesC().Length();
    TInt valueLength = Min( length, KIptvBufferMaxSize );
    localName.Copy( aElement.LocalName().DesC().Mid( 0, valueLength ) );

    IPTVLOGSTRING2_LOW_LEVEL( "CIptvPPContentHandler::OnStartElementL %S", &localName );

    switch ( iState )
        {
        case KIptvIdleState:
            {
            if ( localName.Compare( KIptvServiceProvider ) == 0 )
                {
                iState = KIptvServiceProviderState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvIdleState to KIptvServiceProviderState" );
                }
            else if ( localName.Compare( KIptvServiceGroupProvider ) == 0 )
                {
                iState = KIptvServiceProviderState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvIdleState to KIptvServiceProviderState with Grouping" );
                }
            }
            break;

        case KIptvServiceProviderState:
            {
            if ( localName.Compare( KIptvServiceFeed ) == 0 ||
               ( localName.Compare( KIptvService ) == 0 && !iServiceGroupFeed ) )
                {
                // Start of new service
                delete iService;
                iService = NULL;
                iService = CIptvService::NewL();
                GetServiceTypeL( aAttributes );
                iState = KIptvServiceState;
                iServiceTitleState = EIptvServiceTitleNone;
                iServiceGroupId = 0;
                if ( iPendingIcon )
                    {
                    delete iPendingIcon;
                    iPendingIcon = NULL;
                    }

                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceProviderState to KIptvServiceState" );
                }
            }
            break;

        case KIptvServiceState:
            {
            if ( localName.Compare( KIptvServiceDetails ) == 0 )
                {
                // Initially set this to false since all language specific data is
                // presented inside service details block
                iTitleLanguageDataSet = EFalse;
                iDescriptionLanguageDataSet = EFalse;
                iIconUriLanguageDataSet = EFalse;
                iState = KIptvServiceDetailsState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvServiceDetailsState" );
                }
            else if ( localName.Compare( KIptvServiceUri ) == 0 )
                {
                iState = KIptvServiceUriState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvServiceUriState" );
                }
            else if ( localName.Compare( KIptvIconUri ) == 0 )
                {
                iState = KIptvIconUriState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvIconUriState" );
                }
            else if ( localName.Compare( KIptvUploadProvider ) == 0 )
                {
                iState = KIptvUploadProviderState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvUploadProviderState" );
                }
            else if ( localName.Compare( KIptvIap ) == 0 )
                {
                iState = KIptvIapState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvIapState" );
                }
            else if ( ( localName.Compare( KIptvServiceAccountUriSmallCase ) == 0 ) ||
                      ( localName.Compare( KIptvServiceAccountUriCapitalCase ) == 0 ) )
                {
                iState = KIptvServiceAccountUriState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvServiceAccountUriState" );
                }
            else if ( localName.Compare( KIptvAuth ) == 0 )
                {
                iState = KIptvAuthState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvAuthState" );
                }
            else if ( localName.Compare( KIptvAppUID ) == 0 )
                {
                iState = KIptvAppUIDState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvAppUIDState" );
                }
            else if ( ( localName.Compare( KIptvSearchURISmallCase ) == 0 ) ||
                      ( localName.Compare( KIptvSearchURICapitalCase ) == 0 ) )
                {
                iState = KIptvSearchUriState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvSearchUriState" );
                }
            else if ( localName.Compare( KIptvRecordURI ) == 0 )
                {
                iState = KIptvRecordUriState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvRecordUriState" );
                }
            }
            break;

        case KIptvServiceDetailsState:
            {
            if ( localName.Compare( KIptvText ) == 0 )
                {
                GetLanguageCode( aAttributes );
                iState = KIptvTextState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceDetailsState to KIptvTextState" );
                }
            else if ( localName.Compare( KIptvServiceGroupId ) == 0 )
                {
                iState = KIptvServiceGroupIdState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceDetailsState to KIptvServiceGroupIdState" );
                }
            else if ( localName.Compare( KIptvUseCategories ) == 0 )
                {
                iState = KIptvUseCategoriesState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceDetailsState to KIptvUseCategoriesState" );
                }
            }
            break;

        case KIptvTextState:
            {
            if ( localName.Compare( KIptvTitle ) == 0 )
                {
                iState = KIptvTitleState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvTextState to KIptvTitleState" );
                }
            else if ( localName.Compare( KIptvDescription ) == 0 )
                {
                iState = KIptvDescriptionState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvTextState to KIptvDescriptionState" );
                }
            }
            break;

        case KIptvAuthState:
            {
            if ( localName.Compare( KIptvUsername ) == 0 )
                {
                iState = KIptvUsernameState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvAuthState to KIptvUsernameState" );
                }
            else if ( localName.Compare( KIptvPassword ) == 0 )
                {
                iState = KIptvPasswordState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvAuthState to KIptvPasswordState" );
                }
            }
            break;

        default:
            IPTVLOGSTRING_LOW_LEVEL( "Service provision::Unknown state" );
            break;
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnEndElementL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnEndElementL( const RTagInfo& aElement, TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnEndElementL" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }

    TBuf<KIptvBufferMaxSize> localName;
    TInt length = aElement.LocalName().DesC().Length();
    TInt valueLength = Min( length, KIptvBufferMaxSize );
    localName.Copy( aElement.LocalName().DesC().Mid( 0, valueLength ) );
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvPPContentHandler::OnEndElementL %S", &localName );

    if ( iBuffer )
        {
        TPtr16 ptr = iBuffer->Des();
        ptr.Trim();

        // character conversion
        length = iBuffer->Length();
        HBufC* temp = HBufC::NewL( length );
        CleanupStack::PushL( temp );
        TPtr16 tempPtr = temp->Des();

        HBufC8* buffer = HBufC8::NewL( length );
        CleanupStack::PushL( buffer );
        buffer->Des().Copy( ptr );

        delete iBuffer;
        iBuffer = NULL;

        CnvUtfConverter::ConvertToUnicodeFromUtf8( tempPtr, *buffer );

        iBuffer = HBufC::NewL( length );
        iBuffer->Des().Copy( tempPtr );

        CleanupStack::PopAndDestroy( buffer );
        CleanupStack::PopAndDestroy( temp );
        //
        }

    switch ( iState )
        {
        case KIptvIdleState:
            break;

        case KIptvServiceProviderState:
            {
            if ( localName.Compare( KIptvServiceProvider ) == 0 )
                {
                iState = KIptvIdleState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceProviderState to KIptvIdleState" );
                }
            }
            break;

        case KIptvServiceState:
            {
            iServiceGroupFeed = !localName.Compare( KIptvServiceFeed );
            if ( iServiceGroupFeed || localName.Compare( KIptvService ) == 0 )
                {
                // End of one service
                AddServiceL();
                iProtocol.Zero();
                delete iService;
                iService = NULL;

                iState = KIptvServiceProviderState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceState to KIptvServiceProviderState" );
                }
            }
            break;

        case KIptvServiceDetailsState:
            {
            if ( localName.Compare( KIptvServiceDetails ) == 0 )
                {
                iTitleLanguageDataSet = EFalse;
                iDescriptionLanguageDataSet = EFalse;
                iIconUriLanguageDataSet = EFalse;
                iState = KIptvServiceState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceDetailsState to KIptvServiceState" );
                }
            }
            break;

        case KIptvTextState:
            {
            if ( localName.Compare( KIptvText ) == 0 )
                {
                iState = KIptvServiceDetailsState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvTextState to KIptvServiceDetailsState" );
                }
            }
            break;

        case KIptvTitleState:
            {
            if ( localName.Compare( KIptvTitle ) == 0 )
                {
                // If data is not already set or we hit correct language for the device
                // then update title
                if ( !iTitleLanguageDataSet ||
                    iCurrentlyParsedDetailLanguage == iCurrentDeviceLanguage )
                    {
                    CompleteTitleL();
                    }
                iState = KIptvTextState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvTitleState to KIptvTextState" );
                }
            }
            break;

        case KIptvDescriptionState:
            {
            if ( localName.Compare( KIptvDescription ) == 0 )
                {
                // If data is not already set or we hit correct language for the device
                // then update description
                if ( !iDescriptionLanguageDataSet ||
                     iCurrentlyParsedDetailLanguage == iCurrentDeviceLanguage )
                    {
                    CompleteDescriptionL();
                    }
                iState = KIptvTextState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvDescriptionState to KIptvTextState" );
                }
            }
            break;

        case KIptvServiceGroupIdState:
            {
            if ( localName.Compare( KIptvServiceGroupId ) == 0 )
                {
                CompleteGroupIdL();
                iState = KIptvServiceDetailsState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceGroupIdState to KIptvServiceDetailsState" );
                }
            }
            break;

        case KIptvUseCategoriesState:
            {
            if ( localName.Compare( KIptvUseCategories ) == 0 )
                {
                CompleteUseCategoryL();
                iState = KIptvServiceDetailsState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvUseCategoriesState to KIptvServiceDetailsState" );
                }
            }
            break;

        case KIptvServiceUriState:
            {
            if ( localName.Compare( KIptvServiceUri ) == 0 )
                {
                CompleteUriL();
                iState = KIptvServiceState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceUriState to KIptvServiceState" );
                }
            }
            break;

        case KIptvIconUriState:
            {
            if ( localName.Compare( KIptvIconUri ) == 0 )
                {
                if ( iServiceGroupId > 0 )
                    {
                    // Use rel icon url for grouped services
                    CompleteIconUrlL();
                    }
                else
                    {
                    // If data is not already set or we hit correct language for the device
                    // then update icon uri
                    if ( !iIconUriLanguageDataSet ||
                          iCurrentlyParsedDetailLanguage == iCurrentDeviceLanguage )
                        {
                        CompleteIconPathL();
                        }
                    }

                iState = KIptvServiceState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvIconUriState to KIptvServiceState" );
                }
            }
            break;

        case KIptvUploadProviderState:
            if ( localName.Compare( KIptvUploadProvider ) == 0 )
                {
                CompleteUploadProvider();
                iState = KIptvServiceState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvUploadProviderState to KIptvServiceState" );
                }
            break;

        case KIptvIapState:
            {
            if ( localName.Compare( KIptvIap ) == 0 )
                {
                CompleteIapL();
                iState = KIptvServiceState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvIapState to KIptvServiceState" );
                }
            }
            break;

        case KIptvServiceAccountUriState:
            {
            if ( ( localName.Compare( KIptvServiceAccountUriSmallCase ) == 0 ) ||
                 ( localName.Compare( KIptvServiceAccountUriCapitalCase ) == 0 ) )
                {
                CompleteAccountMgmtUri();
                iState = KIptvServiceState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvServiceAccountUriState to KIptvServiceState" );
                }
            }
            break;

        case KIptvAuthState:
            {
            if ( localName.Compare( KIptvAuth ) == 0 )
                {
                iState = KIptvServiceState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvAuthState to KIptvServiceState" );
                }
            }
            break;

        case KIptvUsernameState:
            {
            if ( localName.Compare( KIptvUsername ) == 0 )
                {
                CompleteUsernameL();
                iState = KIptvAuthState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvUsernameState to KIptvAuthState" );
                }
            }
            break;

        case KIptvPasswordState:
            {
            if ( localName.Compare( KIptvPassword ) == 0 )
                {
                CompletePasswordL();
                iState = KIptvAuthState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvPasswordState to KIptvAuthState" );
                }
            }
            break;

        case KIptvAppUIDState:
            {
            if ( localName.Compare( KIptvAppUID ) == 0 )
                {
                CompleteAppUIDL();
                iState = KIptvServiceState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvAppUIDState to KIptvServiceState" );
                }
            }
            break;

        case KIptvSearchUriState:
            {
            if ( ( localName.Compare( KIptvSearchURISmallCase ) == 0 ) ||
                 ( localName.Compare( KIptvSearchURICapitalCase ) == 0 ) )
                {
                CompleteSearchUriL();
                iState = KIptvServiceState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvSearchUriState to KIptvServiceState" );
                }
            }
            break;

        case KIptvRecordUriState:
            {
            if ( localName.Compare( KIptvRecordURI ) == 0 )
                {
                CompleteRecordUriL();
                iState = KIptvServiceState;
                IPTVLOGSTRING_LOW_LEVEL( "Service provision::State changed from KIptvAppUID to KIptvServiceState" );
                }
            }
            break;

        default:
            IPTVLOGSTRING_LOW_LEVEL( "Service provision::Unknown state" );
            break;
        }
    delete iBuffer;
    iBuffer = NULL;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnContentL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnContentL( const TDesC8& aBytes, TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnContentL" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }

    TInt length = aBytes.Length();
    if ( iBuffer )
        {
        length = length + iBuffer->Length();
        }

    HBufC8* temp = HBufC8::NewL( length );
    CleanupStack::PushL( temp );
    TPtr8 ptr = temp->Des();

    if ( iBuffer )
        {
        ptr.Append( iBuffer->Des() );
        ptr.Append( aBytes );
        }
    else
        {
        ptr.Append( aBytes );
        }

    delete iBuffer;
    iBuffer = NULL;
    iBuffer = HBufC::NewL( length );
    TPtr16 desc = iBuffer->Des();
    desc.Copy( temp->Des() );
    CleanupStack::PopAndDestroy( temp );
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteSearchUriL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteSearchUriL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteSearchUriL" );
    if ( iBuffer && iService )
        {
        TInt length = iBuffer->Length();
        TInt dataToCopy = Min( length, KIptvSmServicesDbSearchUrlMaxLength );
        HBufC* temp = HBufC::NewL( dataToCopy );
        CleanupStack::PushL( temp ); // 1->
        TPtr tempPtr( temp->Des() );
        tempPtr.Copy( iBuffer->Mid( 0, dataToCopy ) );
        iService->SetSearchUrlL( tempPtr );
        CleanupStack::PopAndDestroy( temp ); // <-1
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteRecordUriL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteRecordUriL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteRecordUriL" );
    if ( iBuffer && iService )
        {
        TInt length = iBuffer->Length();
        TInt dataToCopy = Min( length, KIptvSmServicesDbRecordUrlMaxLength );
        HBufC* temp = HBufC::NewL( dataToCopy );
        CleanupStack::PushL( temp ); // 1->
        TPtr tempPtr( temp->Des() );
        tempPtr.Copy( iBuffer->Mid( 0, dataToCopy ) );
        iService->SetRecordUrlL( tempPtr );
        CleanupStack::PopAndDestroy( temp ); // <-1
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteTitleL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteTitleL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteTitleL" );

    if ( iBuffer )
        {
        TInt length = iBuffer->Length();
        TInt dataToCopy = Min( length, KIptvSmServicesDbNameMaxLength );
        TBuf<KIptvSmServicesDbNameMaxLength> temp;
        temp.Copy( iBuffer->Mid( 0, dataToCopy ) );

        if ( iService )
            {
            iService->SetName( temp );

            if ( temp.CompareF( KQtnIptvVideoStoreList ) == 0 )
                {
                iServiceTitleState = EIptvServiceTitleStore;
                if ( iPendingIcon )
                    {
                    // Store and delete pending icon.
                    SetIconPathL( *iPendingIcon );

                    delete iPendingIcon;
                    iPendingIcon = NULL;
                    }
                }
             else
                {
                iServiceTitleState = EIptvServiceTitleNormal;
                if ( iPendingIcon )
                    {
                    // Delete pending icon.
                    delete iPendingIcon;
                    iPendingIcon = NULL;
                    }
                }
            }
        }

    iTitleLanguageDataSet = ETrue;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteDescriptionL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteDescriptionL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteDescriptionL" );

    if ( iBuffer )
        {
        TInt length = iBuffer->Length();
        TInt dataToCopy = Min( length, KIptvSmServicesDbDescMaxLength );
        TBuf<KIptvSmServicesDbDescMaxLength> temp;
        temp.Copy( iBuffer->Mid( 0, dataToCopy ) );

        if ( iService )
            {
            iService->SetDesc( temp );
            }
        }
    iDescriptionLanguageDataSet = ETrue;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteGroupIdL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteGroupIdL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteGroupIdL" );

    if ( iBuffer )
        {
        TInt length( Min( iBuffer->Length(), KIptvSmServicesDbTempIdMaxLength ) );
        TBuf8<KIptvSmServicesDbTempIdMaxLength> groupId;
        groupId.Copy( iBuffer->Mid( 0, length ) );

        if ( iService )
            {
            // Make unique combined service provider id ( provider id + group id )
            length = Min( iService->GetProviderId().Length(),
                KIptvSmServicesDbTempIdMaxLength );
            TBuf16<KIptvSmServicesDbTempIdMaxLength> providerId;
            providerId.Copy( iService->GetProviderId().Mid( 0, length ) );

            HBufC* comboId = HBufC::NewLC( length + groupId.Length() );
            TPtr ptr( comboId->Des() );
            ptr.Copy( groupId );
            ptr.Insert( 0, providerId );
            iService->SetProviderId( ptr );
            CleanupStack::PopAndDestroy( comboId );

            // Count hash
            CMD5* md5Calculator = CMD5::NewL();
            CleanupStack::PushL( md5Calculator );
            TPtrC8 hash( md5Calculator->Hash( groupId ) );

            // Group id
            TUint32 id( 0 );
            for ( TInt i( 0 ); i < hash.Length(); i++ )
                {
                id += hash[i];
                }

            CleanupStack::PopAndDestroy( md5Calculator );
            iService->SetGroupId( id );
            iServiceGroupId = id;
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteUseCategoryL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteUseCategoryL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteUseCategoryL" );

    if ( iBuffer )
        {
        TInt length( Min( iBuffer->Length(), KMaxName ) );
        TName buffer( KNullDesC );
        buffer.Copy( iBuffer->Mid( 0, length ) );

        if ( iService && buffer.Length() > 0 )
            {
            buffer.LowerCase();
            TBool state = ( buffer.Compare( KIptvYes ) == 0 ||
                            buffer.Compare( KIptvOne ) == 0 );
            iService->SetUseCategory( state );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteUriL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteUriL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteUriL" );

    TBuf<KIptvSmServicesDbAddressMaxLength> temp;
    if ( iBuffer )
        {
        TInt length = iBuffer->Length();
        TInt dataToCopy = Min( length, KIptvSmServicesDbAddressMaxLength );
        temp.Copy( iBuffer->Mid( 0, dataToCopy ) );
        }
    else
        {
        // No URI available.
        temp.Copy( KIptvEmpty );
        }

    if ( iService )
        {
        iService->SetAddress( temp );
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteIconUrlL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteIconUrlL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteIconUrlL" );

    TBuf<KIptvSmServicesDbAddressMaxLength> temp;
    if ( iBuffer )
        {
        TInt length = iBuffer->Length();
        TInt dataToCopy = Min( length, KIptvSmServicesDbAddressMaxLength );
        temp.Copy( iBuffer->Mid( 0, dataToCopy ) );
        }
    else
        {
        // No URI available.
        temp.Copy( KIptvEmpty );
        }

    if ( iService )
        {
        iService->SetIconUrl( temp );
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteIconPathL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteIconPathL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteIconPathL" );

    if ( iBuffer )
        {
        TInt length = iBuffer->Length();
        TInt dataToCopy = Min( length, KIptvSmServicesDbIconPathMaxLength );
        TBuf<KIptvSmServicesDbIconPathMaxLength> temp;
        temp.Copy( iBuffer->Mid( 0, dataToCopy ) );

        temp.Trim();

        // We must accept local icon always for VideoStore Service.
        TBool setIcon( ETrue );
        if ( !iLocal && ( iServiceTitleState != EIptvServiceTitleStore ) )
            {
            // If this is from over the air path must not point to local file
            if ( temp.Find( KIptvHttpPrefix ) != 0 )
                {
                if ( iServiceTitleState == EIptvServiceTitleNone )
                    {
                    // We don't yet know whether this is VideoStore Service.
                    // Save 1st occurence of Icon.
                    if ( !iPendingIcon )
                        {
                        iPendingIcon = HBufC::NewL( temp.Length() );
                        iPendingIcon->Des().Copy( temp );
                        setIcon = EFalse;
                        }
                    }
                else
                    {
                    IPTVLOGSTRING_LOW_LEVEL(
                        "CIptvPPContentHandler::CompleteIconPathL: Error: local path not allowed" );
                    setIcon = EFalse;
                    }
                }
            }

        if ( setIcon )
            {
            SetIconPathL( temp );
            }
        }

    iIconUriLanguageDataSet = ETrue;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteUploadProvider
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteUploadProvider()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteUploadProvider" );

    if ( iBuffer )
        {
        TInt length( Min( iBuffer->Length(), KIptvSmServicesDbNameMaxLength ) );
        TBuf<KIptvSmServicesDbNameMaxLength> temp( KNullDesC );
        temp.Copy( iBuffer->Mid( 0, length ) );

        if ( iService && temp.Length() > 0 )
            {
            iService->SetUploadProvider( temp );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteIapL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteIapL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteIapL" );

    if ( !iLocal )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteIapL, Not allowed OTA" );
        return;
        }

    if ( iBuffer )
        {
        TInt length = iBuffer->Length();
        TBuf<KIptvIapBufferLength> buffer;
        buffer.Copy( iBuffer->Mid( 0, length ) );

        if ( buffer.Length() > 0 )
            {
            TInt  iapValue = 0;
            TBool iapAdded = EFalse;

            // Iap is defined either as number or with name.
            if ( HasOnlyNumbers( buffer ) )
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteIapL, IAP defined as number." );

                TLex l( buffer );
                l.Val( iapValue );

                // If IAP number available, add to service.
                if ( iService && iapValue != 0 )
                    {
                    TIptvIap iap;
                    iap.iId = iapValue;
                    iap.iPriority = KIptvIapDefaultPriority;

                    iService->iIapList->AddIap( iap );
                    iapAdded = ETrue;
                    }
                }
            else
                {
                IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteIapL, IAP defined with name." );

                iService->SetIapNameL( buffer.Left( KIptvSmServicesDbIapNameMaxLength ) );
                iapAdded = ETrue;
                }

            // Mark service so that IAPs cannot be modified.
            if ( iapAdded )
                {
                iService->SetFlags(
                    iService->GetFlags() | CIptvService::EReadOnlyIaps );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::HasOnlyNumbers
//
// ---------------------------------------------------------------------------
//
TBool CIptvPPContentHandler::HasOnlyNumbers( const TDesC& aString ) const
    {
    for ( TInt i = 0; i < aString.Length(); i++ )
        {
        TChar character = aString[i];

        if ( !( character == '0' || character == '1' || character == '2' ||
                character == '3' || character == '4' || character == '5' ||
                character == '6' || character == '7' || character == '8' ||
                character == '9' ) )
            {
            return EFalse;
            }
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteUsernameL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteUsernameL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteUsernameL" );

    if ( iBuffer )
        {
        TInt length = iBuffer->Length();
        TInt dataToCopy = Min( length, KIptvSmPasswordsDbUserNameMaxLength );
        TBuf<KIptvSmPasswordsDbUserNameMaxLength> temp;
        temp.Copy( iBuffer->Mid( 0, dataToCopy ) );

        if ( iService )
            {
            iService->SetUserName( temp );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompletePasswordL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompletePasswordL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompletePasswordL" );

    if ( iBuffer )
       {
       TInt length = iBuffer->Length();
       TInt dataToCopy = Min( length, KIptvSmPasswordsDbPasswordMaxLength );
       TBuf<KIptvSmPasswordsDbPasswordMaxLength> temp;
       temp.Copy( iBuffer->Mid( 0, dataToCopy ) );

       if ( iService )
            {
            iService->SetPassword( temp );
            }
       }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteAccountMgmtUriL
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteAccountMgmtUri()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteAccountMgmtUri" );

    TBuf<KIptvSmServicesDbAccountManagementUrlMaxLength> temp;
    if ( iBuffer )
        {
        TInt length = iBuffer->Length();
        TInt dataToCopy = Min( length, KIptvSmServicesDbAccountManagementUrlMaxLength );
        temp.Copy( iBuffer->Mid( 0, dataToCopy ) );
        }

    if ( iService )
        {
        iService->SetAccountManagementUrl( temp );
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnStartPrefixMappingL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnStartPrefixMappingL(
    const RString& /*aPrefix*/,
    const RString& /*aUri*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnStartPrefixMappingL" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnEndPrefixMappingL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnEndPrefixMappingL(
    const RString& /*aPrefix*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnEndPrefixMappingL" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnIgnorableWhiteSpaceL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnIgnorableWhiteSpaceL(
    const TDesC8& /*aBytes*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnIgnorableWhiteSpaceL" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnSkippedEntityL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnSkippedEntityL(
    const RString& /*aName*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnSkippedEntityL" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnProcessingInstructionL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnProcessingInstructionL(
    const TDesC8& /*aTarget*/,
    const TDesC8& /*aData*/,
    TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnProcessingInstructionL" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::OnError
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::OnError( TInt aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::OnError" );

    if ( iError == KErrNone )
        {
        iError = aErrorCode;
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::GetExtendedInterface
//
// ---------------------------------------------------------------------------
//
TAny* CIptvPPContentHandler::GetExtendedInterface( const TInt32 /*aUid*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::GetExtendedInterface" );

    return NULL;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::Reset
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::Reset()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::Reset" );

    iState = KIptvIdleState;
    iServiceCount = 0;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::isValidServiceL
//
// ---------------------------------------------------------------------------
//
TBool CIptvPPContentHandler::IsValidServiceL( TInt& aError )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::IsValidServiceL" );

    TBool validType( EFalse );
    aError = KErrNone;

    if ( iService->GetType() == CIptvService::EVodCast )
        {
        if ( iProtocol.Compare( KIptvProtocolRss ) == 0 )
            {
            validType = ETrue;
            }
        else
            {
            aError = KErrNotSupported;
            }
        }
    else if ( iService->GetType() == CIptvService::EVod )
        {
        if ( ( iProtocol.Compare( KIptvProtocolRss ) == 0 ) ||
             ( iProtocol.Compare( KIptvProtocolDvbIpi ) == 0 ) )
            {
            validType = ETrue;
            }
        else
            {
            aError = KErrNotSupported;
            }
        }
    else if ( iService->GetType() == CIptvService::ELiveTv )
        {
        validType = EFalse;
        }
    else if ( iService->GetType() == CIptvService::EServiceGroup )
        {
        if ( iProtocol.Compare( KIptvProtocolRss ) == 0 &&
             iService->GetGroupId() > 0 )
            {
            validType = ETrue;
            }
        else
            {
            aError = KErrNotSupported;
            }
        }
    else if ( iService->GetType() == CIptvService::EBrowser )
        {
        validType = ETrue;
        }
    else if ( iService->GetType() == CIptvService::ESearch )
        {
        validType = ETrue;
        }
    else if ( iService->GetType() == CIptvService::EUpload )
        {
        validType = ETrue;
        }
    else if ( iService->GetType() == CIptvService::EApplication )
        {
        validType = ETrue;
        }
    else if ( iService->GetType() == CIptvService::EMobileTv )
        {
        validType = ETrue;
        }
    else if ( iService->GetType() == CIptvService::ESecureProvisioningServer )
        {
        validType = ETrue;
        }
    else
        {
        aError = KIptvInvalidServiceType;
        }

    if ( validType )
        {
        if ( iService->GetName().Length() == 0 )
            {
            aError = KIptvRssParserError;
            validType = EFalse;
            }
        if ( iService->SearchUrl().Length() > KIptvUrlMaxLength )
            {
            aError = KIptvInvalidUrl;
            validType = EFalse;
            }
        }

    return validType;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::AddService
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::AddServiceL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::AddService" );

    if ( iServiceCount < KIptvMaxServiceCount && iError == KErrNone )
        {
        iService->SetDisplayOrder( iServiceCount );

        // Enable service
        iService->SetId( 0 );
        if ( iService->GetGroupId() == 0 ||
             iService->GetType() == CIptvService::EServiceGroup )
            {
            iService->SetFlags( iService->GetFlags() | CIptvService::ESelected );
            }
        else if ( iService->GetGroupId() > 0 &&
                  iService->GetType() != CIptvService::EServiceGroup )
            {
            iService->SetFlags( iService->GetFlags() | CIptvService::EGroupedService );
            }

        if ( iLocal )
            {
            if ( iServices )
                {
                // Local provisioning sets readonly services.
                iService->SetFlags( iService->GetFlags() | CIptvService::EReadOnly );

                TInt error( KErrNone );
                if ( IsValidServiceL( error ) )
                    {
                    iServiceCount++;
                    iServices->AddL( *iService );
                    }
                else
                    {
                    iError = error;
                    }
                }
            }
        else
            {
            if ( iServices )
                {
                TInt error( KErrNone );
                if ( IsValidServiceL( error ) )
                    {
                    iServiceCount++;
                    iServices->AddL( *iService );
                    }
                else
                    {
                    iError = error;
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::GetServiceType
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::GetServiceTypeL( const RAttributeArray& aAttributes )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::GetServiceType" );

    TInt count = aAttributes.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        const RTagInfo& tag = aAttributes[i].Attribute();
        TBuf<KIptvBufferMaxSize> name;
        TInt length = tag.LocalName().DesC().Length();
        TInt valueLength = Min( length, KIptvBufferMaxSize );
        name.Copy( tag.LocalName().DesC().Mid( 0, valueLength ) );

        TBuf<KIptvBufferMaxSize> value;
        length = aAttributes[i].Value().DesC().Length();
        valueLength = Min( length, KIptvBufferMaxSize );
        value.Copy( aAttributes[i].Value().DesC().Mid( 0, valueLength ) );

        // Type
        if ( name.Compare( KIptvType ) == 0 )
            {
            SetServiceTypeL( value );
            }
        // Protocol
        else if ( name.Compare( KIptvProtocol ) == 0 )
            {
            SetProtocolL( value );
            }
        // Provider id
        else if ( name.Compare( KIptvServiceAttrProviderId ) == 0 )
            {
            iService->SetProviderId( value );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::SetServiceTypeL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::SetServiceTypeL( const TDesC& aServiceType )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::SetServiceTypeL" );

    if ( iService )
       {
        CIptvService::TServiceType type( CIptvService::EOther );
        if ( aServiceType.Compare( KIptvVodCast ) == 0 )
            {
            type = CIptvService::EVodCast;
            iService->SetFlags( 0 ); // Not main service
            }
        else if ( aServiceType.Compare( KIptvLiveTv ) == 0 )
            {
            type = CIptvService::ELiveTv;
            iService->SetFlags( CIptvService::EMainService );
            }
        else if ( aServiceType.Compare( KIptvVod ) == 0 )
            {
            type = CIptvService::EVod;
            iService->SetFlags( CIptvService::EMainService );
            }
        else if ( aServiceType.Compare( KIptvBrowser ) == 0 )
            {
            type = CIptvService::EBrowser;
            iService->SetFlags( CIptvService::EMainService );
            }
        else if ( aServiceType.Compare( KIptvApplication ) == 0 )
            {
            type = CIptvService::EApplication;
            iService->SetFlags( CIptvService::EMainService );
            }
        else if ( aServiceType.Compare( KIptvMobileTv ) == 0 )
            {
            type = CIptvService::EMobileTv;
            iService->SetFlags( CIptvService::EMainService );
            }
        else if ( aServiceType.Compare( KIptvSecureProvisioningServer ) == 0 )
            {
            type = CIptvService::ESecureProvisioningServer;
            iService->SetFlags( CIptvService::EMainService );
            }
        else if ( aServiceType.Compare( KIptvServiceGroup ) == 0 )
            {
            type = CIptvService::EServiceGroup;
            iService->SetFlags( 0 ); // Not main service
            }
        else if ( aServiceType.Compare( KIptvSearch ) == 0 )
            {
            type = CIptvService::ESearch;
            iService->SetFlags( 0 ); // Not main service
            }
        else if ( aServiceType.Compare( KIptvUpload ) == 0 )
            {
            type = CIptvService::EUpload;
            iService->SetFlags( 0 ); // Not main service
            }
        else
            {
            // All other services go under other
            iService->SetFlags( CIptvService::EMainService );
            }

        iService->SetType( type );
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::SetProtocolL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::SetProtocolL( const TDesC& aProtocol )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::SetProtocolL" );

    if ( iService )
        {
        if ( aProtocol.Compare( KIptvProtocolRss ) == 0 )
            {
            IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::SetProtocolL RSS" );

            iProtocol.Copy( KIptvProtocolRss );
            TUid uid;
            uid.iUid = KIptvRssPluginImplementationUid;
            iService->SetEpgPluginUid( uid );
            // No Live TV plugin in vodcast service
            uid.iUid = 0;
            iService->SetIptvPluginUid( uid );
            // Set download plugin as http download plugin. RSS uses HTTP downloads
            uid.iUid = IPTV_HTTP_VOD_DOWNLOAD_PLUGIN_IMPLEMENTATION_UID;
            iService->SetVodPluginUid( uid );
            }
        else if ( aProtocol.Compare( KIptvProtocolDvbIpi ) == 0 )
            {
            IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::SetProtocolL DVB-IPI" );

            iProtocol.Copy( KIptvProtocolDvbIpi );
            TUid uid;
            uid.iUid = KIptvSmartvisionPluginUid;

            // Same plugin handles Live TV program guide and content quide
            iService->SetEpgPluginUid( uid );
            iService->SetIptvPluginUid( uid );

            // HTTP downloads used
            uid.iUid = IPTV_HTTP_VOD_DOWNLOAD_PLUGIN_IMPLEMENTATION_UID;
            iService->SetVodPluginUid( uid );
            }
        else if ( aProtocol.Compare( KIptvProtocolXMLTV ) == 0 )
            {
            iProtocol.Copy( KIptvProtocolXMLTV );
            TUid uid;
            uid.iUid = KIptvLiveTvXmlTvPluginUid;
            iService->SetIptvPluginUid( uid );
            iService->SetEpgPluginUid( uid );
            uid.iUid = IPTV_HTTP_VOD_DOWNLOAD_PLUGIN_IMPLEMENTATION_UID;
            iService->SetVodPluginUid( uid );
            }
        else if ( aProtocol.Compare( KIptvProtocolHTTP ) == 0 )
            {
            IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::SetProtocolL, HTTP" );

            iProtocol.Copy( KIptvProtocolHTTP );
            }
        else if ( aProtocol.Compare( KIptvProtocolHTTPProp ) == 0 )
            {
            IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::SetProtocolL, HTTP-PROP" );

            iProtocol.Copy( KIptvProtocolHTTPProp );
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::SetProtocolL, Unknown protocol" );
            // Not allowed, protocol must be something supported

            iProtocol.Zero();
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::CompleteAppUIDL
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::CompleteAppUIDL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteAppUIDL" );

    if ( !iLocal )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteAppUIDL, Not allowed OTA" );
        return;
        }

    if ( iBuffer )
        {
        TUint32 appUid = 0;
        TInt length = iBuffer->Length();
        TInt dataToCopy = Min( length, KIptvAppUIDMaxSize );
        TBuf<KIptvAppUIDMaxSize + KIptvDriveLetterSpace> temp;
        temp.Copy( iBuffer->Mid( 0, dataToCopy ) );

        temp.ZeroTerminate();
        // Number is always 8 digits long and in hexadecimal format
        TLex16 converter( temp );
        if ( converter.Val( appUid, EHex ) != KErrNone )
            {
            IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::CompleteAppUIDL, Invalid App UID" );
            // implement cancel processing of this service
            return;
            }

        if ( iService )
            {
            iService->SetApplicationUid( appUid );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::SetServicesArray
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::SetServicesArray( CIptvServices* aServices )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::SetServicesArray" );

    iServices = aServices;
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::GetLanguageCode
//
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::GetLanguageCode( const RAttributeArray& aAttributes )
    {
    iCurrentLanguageCode.Zero();

    TInt count = aAttributes.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        const RTagInfo& tag = aAttributes[i].Attribute();
        TPtrC8 attrLocalName = tag.LocalName().DesC();
        TBuf<KIptvBufferMaxSize> localName;
        localName.Copy( attrLocalName );
        if ( localName.Compare( KIptvTextAttrLang ) == 0 )
            {
            iCurrentLanguageCode.Copy(
                aAttributes[i].Value().DesC().Left( KMaxLanguageCodeStringLength ) );
            }
        }
    if ( iCurrentLanguageCode.Length() > 0 )
        {
        iCurrentlyParsedDetailLanguage =
            CIptvUtil::GetLanguageByCountryCode( iCurrentLanguageCode );
        }
    }

// ---------------------------------------------------------------------------
// CIptvPPContentHandler::SetIconPathL()
// ---------------------------------------------------------------------------
//
void CIptvPPContentHandler::SetIconPathL( const TDesC& aIconPath )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvPPContentHandler::SetIconPathL" );

    // Check for a file name without drive letter.
    if ( ( aIconPath.Length() > 0 ) && ( aIconPath[0] == '\\' ) )
        {
        TBool accept( EFalse );
        TDriveList driveList;
        TDriveInfo driveInfo;

        if ( KErrNone == iFs.DriveList( driveList ) )
            {
            HBufC* tempName =
                HBufC::NewLC( aIconPath.Length() + KIptvDriveLetterSpace );
            TChar character;

            for ( TInt i = EDriveA; i <= EDriveZ && !accept; i++ )
                {
                if ( driveList[i] &&
                     ( KErrNone == iFs.Drive( driveInfo, i ) ) &&
                     ( KErrNone == RFs::DriveToChar( i, character ) ) )
                    {
                    tempName->Des().Zero();
                    tempName->Des().Append( character );
                    tempName->Des().Append( ':' );
                    tempName->Des().Append( aIconPath );

                    IPTVLOGSTRING2_LOW_LEVEL(
                        "Scanning icon from %S", tempName );
                    if ( BaflUtils::FileExists( iFs, *tempName ) )
                        {
                        IPTVLOGSTRING_LOW_LEVEL( " ---- Icon found" );
                        accept = ETrue;
                        }
                    }
                }

            if ( accept && iService )
                {
                iService->SetIconPath( tempName->Des() );
                }

            CleanupStack::PopAndDestroy( tempName );
            }
        }
    else if ( iService )
        {
        iService->SetIconPath( aIconPath );
        }
    }
