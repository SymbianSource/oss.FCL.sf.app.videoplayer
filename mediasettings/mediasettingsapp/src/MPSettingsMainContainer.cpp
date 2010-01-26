/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Container class for Main view.*
*/


// Version : %version: 9 %




// INCLUDE FILES
#include    <barsread.h>
#include    <MediaSettings.rsg>
#include    <aknlists.h>
#include    <AknIconArray.h>
#include    <AknsUtils.h>
#include    <gulicon.h>
#include    <calslbs.h>
#include    <eikclbd.h>
#include    <StringLoader.h>
#include    <mediasettings.mbg>
#include    <featmgr.h>
#include    <aknnotewrappers.h>
#include    <textresolver.h>
#include    <AknCommonDialogs.h>
#include    <csxhelp/iptv.hlp.hrh>

#include    "MPSettingsApp.h"
#include    "MPSettingsMainContainer.h"
#include    "MPSettingsConstants.h"
#include	"mpxlog.h"

// CONSTANTS
const TInt KIconArrayGranularity = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsMainContainer::CMPSettingsMainContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsMainContainer::CMPSettingsMainContainer()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsMainContainer::CMPSettingsMainContainer()"));
    }

// -----------------------------------------------------------------------------
// CMPSettingsMainContainer::~CMPSettingsMainContainer
// Destructor
// -----------------------------------------------------------------------------
//
CMPSettingsMainContainer::~CMPSettingsMainContainer()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsMainContainer::~CMPSettingsMainContainer()"));
	if (iListBox) 
		{
		delete iListBox;
		}
    }

// ---------------------------------------------------------
// CMPSettingsMainContainer::ConstructComponentControlsL
// ---------------------------------------------------------
//
void CMPSettingsMainContainer::ConstructComponentControlsL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsMainContainer::ConstructComponentControlsL()"));
    // * Create listbox *

    iListBox = new(ELeave) CAknSingleLargeStyleListBox;
   
    // Set's iListBox to be contained in this container
    iListBox->SetContainerWindowL(*this); 

    // Construct from resource
    TResourceReader rReader;
    iCoeEnv->Static()->CreateResourceReaderLC(rReader, 
            R_MPSETT_MAIN_LBX_RESOURCE);

    // Construct's iListBox from resource file
    iListBox->ConstructFromResourceL(rReader); 
    CleanupStack::PopAndDestroy(); // rReader
    
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    MakeIconArrayL();

    iComponentControl = iListBox;
    }

// ---------------------------------------------------------
// CMPSettingsMainContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CMPSettingsMainContainer::HandleResourceChange( TInt aType )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsMainContainer::HandleResourceChange(0x%X)"),aType);
    CCoeControl::HandleResourceChange( aType );
 
    if( aType == KAknsMessageSkinChange )
        {
        TRAPD(err,MakeIconArrayL());
            
        if( err )
            {
            TRAP_IGNORE( HandleResourceChangeErrorL( err ) );
            }
        }
        else if ( aType == KEikDynamicLayoutVariantSwitch ) //Handle change in layout orientation
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
		DrawNow();
        }
    }

// ---------------------------------------------------------
// CMPSettingsMainContainer::HandleResourceChangeErrorL
// ---------------------------------------------------------
//
void CMPSettingsMainContainer::HandleResourceChangeErrorL( TInt aError )
    {
    CTextResolver* textresolver = NULL;
    
    // TextResolver instance for error resolving.
    textresolver = CTextResolver::NewL();
    
    if ( textresolver )
        {
        // Resolve the error text
        TPtrC text;
        text.Set(textresolver->ResolveErrorString( aError ));
    
        CAknErrorNote* dlg = new (ELeave) CAknErrorNote(ETrue);
        dlg->ExecuteLD(text);

        delete textresolver;
        }
    }

// ---------------------------------------------------------
// CMPSettingsMainContainer::TitleResourceId
// ---------------------------------------------------------
//
TInt CMPSettingsMainContainer::TitleResourceId()
    {
    return R_MPSETT_MAIN_VIEW_TITLE;
    }

// ---------------------------------------------------------
// CMPSettingsMainContainer::GetHelpContext
// ---------------------------------------------------------
//
void CMPSettingsMainContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsMainContainer::GetHelpContext()"));
    aContext.iMajor = KIptvHelpContextUid;
    aContext.iContext = KIPTV_HLP_SETTINGS;
    }

// ---------------------------------------------------------
// CMPSettingsMainContainer::MakeIconArrayL
// ---------------------------------------------------------
//
void CMPSettingsMainContainer::MakeIconArrayL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsMainContainer::MakeIconArrayL()"));

	TFileName fileName;

    // Construct CAknIconArray for iListBox
    CEikColumnListBox* listbox = 
        static_cast<CEikColumnListBox*>( iListBox );

    delete listbox->ItemDrawer()->ColumnData()->IconArray();
    listbox->ItemDrawer()->ColumnData()->SetIconArray( NULL );

    HBufC* mbmpath = StringLoader::LoadLC( R_MPSETT_ICON_MBM_FILE );
    
    // This is done to ensure upgraded file is used first. 
    // If no upgraded file is found, default file in Z: drive will be used.
	TFindFile find( CCoeEnv::Static()->FsSession() );
	TInt err = find.FindByDir( *mbmpath, KNullDesC );
	
	if ( err )
		{
		fileName.Append( *mbmpath );		
		}
	else
		{
		fileName.Append( find.File() );    
		}

    CAknIconArray* icons = 
        new ( ELeave ) CAknIconArray( KIconArrayGranularity );
    CleanupStack::PushL( icons );

    AppendIconToArrayL( 
        icons, 
        KAknsIIDQgnPropSetMpVideoSub, 
        fileName, 
        EMbmMediasettingsQgn_prop_set_mp_video_sub, 
        EMbmMediasettingsQgn_prop_set_mp_video_sub_mask );

    AppendIconToArrayL(
        icons, 
        KAknsIIDQgnPropSetMpStreamSub, 
		fileName, 
        EMbmMediasettingsQgn_prop_set_mp_stream_sub, 
        EMbmMediasettingsQgn_prop_set_mp_stream_sub_mask );
  
    AppendIconToArrayL( 
        icons, 
        KAknsIIDQgnPropSetMpVideoSub, 
        fileName, 
        EMbmMediasettingsQgn_prop_set_service, 
        EMbmMediasettingsQgn_prop_set_service_mask );

    CleanupStack::Pop(); // icons
    CleanupStack::PopAndDestroy(); // mbmpath

    listbox->ItemDrawer()->ColumnData()->SetIconArray( icons );

    }

// ---------------------------------------------------------
// CMPSettingsMainContainer::AppendIconToArrayL
// ---------------------------------------------------------
//
void CMPSettingsMainContainer::AppendIconToArrayL(
    CAknIconArray* aArray,
    const TAknsItemID& aID,
    const TDesC& aMbmFile,
    TInt aBitmapId,
    TInt aMaskId )
    {
    
    MPX_DEBUG1(_L("#MS# CMPSettingsMainContainer::AppendIconToArrayL()"));
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC( skin, aID,
        bitmap, mask, aMbmFile, aBitmapId, aMaskId );

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop(2); // mask, bitmap
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL( icon );
    aArray->AppendL( icon );

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop();
    }

// ---------------------------------------------------------
// CMPSettingsMainContainer::FocusChanged
// ---------------------------------------------------------
//
void CMPSettingsMainContainer::FocusChanged(TDrawNow /*aDrawNow*/)
{
	if( iListBox)
        {
        iListBox->SetFocus( IsFocused() );
        }
}

// End of File  
