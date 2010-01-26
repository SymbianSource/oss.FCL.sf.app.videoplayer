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
* Description:   View class for Streaming list view.*
*/


// Version : %version: 8 %



// INCLUDE FILES
#include    <avkon.hrh>
#include    <aknViewAppUi.h>
#include    <eikmenup.h>
#include    <featmgr.h>
#include    <akntabgrp.h>
#include    <barsread.h>
#include    <MediaSettings.rsg>
#include    <calslbs.h>
#include    <gstabhelper.h>
#include    <hlplch.h>             // For HlpLauncher
#include    "mediasettings.hrh"
#include    "GSVideoPlugin.h"
#include    <mediasettings.mbg>
#include    "GSMediaPlayerStreamingView.h"
#include    "MPSettingsStreamingContainer.h"
#include    "MPSettingsConstants.h" 
#include    "MPSettingsBaseView.h"
#include	"mpxlog.h"

class CMPSettingsModelForROP;

// CONSTANTS

const   TInt    KMPSettProxyViewListItemId  =   0;
const   TInt    KMPSettNetworkViewListItemId  =   1;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::CGSMediaPlayerStreamingView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGSMediaPlayerStreamingView::CGSMediaPlayerStreamingView(CMPSettingsModelForROP* aModel, CArrayPtrFlat<MGSTabbedView>* /* aTabViewArray */ ) 
	: iModel( aModel)
    {
    MPX_FUNC("#MS# CGSMediaPlayerStreamingView::CGSMediaPlayerStreamingView()");
    }

// ---------------------------------------------------------
// CGSMediaPlayerStreamingView::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
void CGSMediaPlayerStreamingView::ConstructL(CArrayPtrFlat<MGSTabbedView>* aTabViewArray)
    {
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::ConstructL()");
		iTabHelper = CGSTabHelper::NewL();
		aTabViewArray->AppendL(this);
		BaseConstructL(R_GS_MPSETT_STREAMING_VIEW);
    }

// -----------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSMediaPlayerStreamingView* CGSMediaPlayerStreamingView::NewLC(CMPSettingsModelForROP* aModel, CArrayPtrFlat<MGSTabbedView>* aTabViewArray)
    {
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::NewLC()");
		CGSMediaPlayerStreamingView* self = new(ELeave) CGSMediaPlayerStreamingView(aModel,aTabViewArray);
    
		CleanupStack::PushL(self);
		self->ConstructL( aTabViewArray );

		return self;
    }

// ---------------------------------------------------------
// CGSMediaPlayerStreamingView::~CGSMediaPlayerStreamingView
// Destructor
// ---------------------------------------------------------
//
CGSMediaPlayerStreamingView::~CGSMediaPlayerStreamingView()
    {
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::~CGSMediaPlayerStreamingView()");
	    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
	    delete iTabHelper;    
    }

// ---------------------------------------------------------
// TUid CGSMediaPlayerStreamingView::Id
// ---------------------------------------------------------
//
TUid CGSMediaPlayerStreamingView::Id() const
    {
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::Id()");
	    return KMPSettStreamingViewId;
    }

// ---------------------------------------------------------
// CGSMediaPlayerStreamingView::HandleCommandL
// ---------------------------------------------------------
//
void CGSMediaPlayerStreamingView::HandleCommandL(TInt aCommand)
    {   
        MPX_DEBUG2(_L("#MS# CGSMediaPlayerStreamingView::HandleCommandL(%d)"),aCommand);
	    CGSVideoPlugin* parent =
			static_cast<CGSVideoPlugin*> (
				AppUi()->View( KGSVideoPluginUid ) );

		switch (aCommand)
        {
			case EMPSettCmdOpen:
				HandleListBoxSelectionL();
				break;
			case EAknSoftkeyBack:
				parent->SetCurrentItem(EGSMediaSettingsStreamingIndex);
				parent->ResetSelectedItemIndex();
     			AppUi()->ActivateLocalViewL(KGSVideoPluginUid);
				break;
			case EMPSettCmdHelp:
				if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
				{
				HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
				}
				break;
			default:
				AppUi()->HandleCommandL(aCommand);
				break;
        }
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerStreamingView::DoActivateL( const TVwsViewId& /* aPrevViewId */,
                                          TUid /*aCustomMessageId*/,
                                          const TDesC8& /*aCustomMessage*/ )
    { 
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::DoActivateL()");
	    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }

	    CreateContainerL();
	    AppUi()->AddToViewStackL( *this, iContainer );
   
	    CGSVideoPlugin* parent = 
	        static_cast<CGSVideoPlugin*> ( 
	            AppUi()->View( KGSVideoPluginUid ) );
        
	    if( parent )
        {
        iTabHelper->CreateTabGroupL( Id(), 
            static_cast<CArrayPtrFlat<MGSTabbedView> *> (parent->TabbedViews() ) ); 
        } 
	    iContainer->SetRect( ClientRect() );   
	    iContainer->ActivateL();   
    
	    DynInitContainerL();
    
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerStreamingView::DoDeactivate()
    {

        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::DoDeactivate()");
        if ( iContainer )
        {        
            CEikTextListBox* listbox = iContainer->ListBox();
            if (listbox)
            {
            iCurrentItem = listbox->CurrentItemIndex();
            iTopItemIndex = listbox->TopItemIndex();
            } 
            
            AppUi()->RemoveFromStack( iContainer );
            delete iContainer;
            iContainer = NULL;
            iTabHelper->RemoveTabGroup();                   
        }
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::Container
// Returns network container item
// ---------------------------------------------------------------------------
//
CMPSettingsBaseContainer* CGSMediaPlayerStreamingView::Container()
    {
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::Container()");
	    return static_cast <CMPSettingsStreamingContainer*> ( iContainer );
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::DynInitMenuPaneL
// Before showing a options menu
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerStreamingView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
        MPX_DEBUG2(_L("#MS# CGSMediaPlayerStreamingView::DynInitMenuPaneL(0x%X)"),aResourceId);
	    if (aResourceId == R_MPSETT_APP_MENU)
        {
			// Help should be displayed only if the feature is supported according
			// to Feature Manager
			if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
            {
            aMenuPane->SetItemDimmed(EMPSettCmdHelp, ETrue);
            }
        }
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::NewContainerL()
// Creates new iContainer.
// ---------------------------------------------------------------------------
void CGSMediaPlayerStreamingView::NewContainerL()
    {
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::NewContainerL()");
	    iContainer = new (ELeave) CMPSettingsStreamingContainer(iModel);
    }

// -----------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::HandleClientRectChange
// Handle screen size change.
// -----------------------------------------------------------------------------
//
void CGSMediaPlayerStreamingView::HandleClientRectChange()
    {
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::HandleClientRectChange()");
	    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        } 
    }
                  
// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::HandleListBoxEventL
// ---------------------------------------------------------------------------

void CGSMediaPlayerStreamingView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
    {
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::HandleListBoxEventL()");
	    switch (aEventType)
        {
			case EEventEnterKeyPressed:
			case EEventItemSingleClicked:
			case EEventItemDoubleClicked:
				HandleListBoxSelectionL();
				break;
			default:
			   break;
        }
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::DynInitContainerL
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerStreamingView::DynInitContainerL()
    {
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::DynInitContainerL()");
		CEikTextListBox* listbox = iContainer->ListBox();

		listbox->SetListBoxObserver(this);
		listbox->SetTopItemIndex(iTopItemIndex);

		if (iCurrentItem >= 0 && iCurrentItem < listbox->Model()->NumberOfItems()) // magic
			{
			listbox->SetCurrentItemIndexAndDraw(iCurrentItem);
			}
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::HandleListBoxSelectionL()
// Handles listbox selection.
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerStreamingView::HandleListBoxSelectionL() 
    {
		iCurrentItem = iContainer->ListBox()->CurrentItemIndex();
		iTopItemIndex = iContainer->ListBox()->TopItemIndex();
		MPX_DEBUG3(_L("#MS# CGSMediaPlayerStreamingView::HandleListBoxSelectionL() iCurrentItem(%d) iTopItemIndex(%d)"),iCurrentItem,iTopItemIndex);

		switch (iCurrentItem)
			{
			case KMPSettProxyViewListItemId:
				if (!FeatureManager::FeatureSupported(KFeatureIdProtocolCdma))
				{
				// If KFeatureIdProtocolCdma is not supported then show the 
				// Proxy view. Otherwise, fall through to show Network view because
				// only Network setting is available.
				AppUi()->ActivateLocalViewL(KMPSettProxyViewId);
				break;
				}
			case KMPSettNetworkViewListItemId:
				AppUi()->ActivateLocalViewL(KMPSettNetworkViewId);
				break;
			default:
				break;
			}
    }

    
// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::CreateContainerL()
// Creates new iContainer.
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerStreamingView::CreateContainerL()
    {
        
        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::CreateContainerL()");
		NewContainerL();
		__ASSERT_DEBUG( 
			iContainer, User::Panic( KGSDoActivateError, EGSViewPanicNullPtr ) );
		iContainer->SetMopParent( this );

		//TRAPD( error, iContainer->ConstructL() );
		TRAPD( error, iContainer->ConstructL( ClientRect() ) );

		if ( error )
		{
			delete iContainer;
			iContainer = NULL;
			User::Leave( error );
		}
    }
        
// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::CreateTabIconL()
// Creates tab icon for this view.
// ---------------------------------------------------------------------------
//    
CGulIcon* CGSMediaPlayerStreamingView::CreateTabIconL()
    {

        MPX_FUNC("#MS# CGSMediaPlayerStreamingView::CreateTabIconL()");
		CGulIcon* icon;
	    TFileName fileName;

		CGSVideoPlugin* parent = 
			static_cast<CGSVideoPlugin*> ( 
				AppUi()->View( KGSVideoPluginUid ) );
		
	    if( parent )
        {
	        parent->LocateFilePathL( fileName );    
        } 
	    
		icon = AknsUtils::CreateGulIconL(
			AknsUtils::SkinInstance(), 
			KAknsIIDDefault, 
			fileName,
			EMbmMediasettingsQgn_prop_set_mp_stream_tab2,
			EMbmMediasettingsQgn_prop_set_mp_stream_tab2_mask );
    
		return icon;

    } 
    
// ---------------------------------------------------------------------------
// CGSMediaPlayerStreamingView::HandleClientRectChange
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerStreamingView::SetCurrentItem(TInt aIndex)
    {
        MPX_DEBUG2(_L("#MS# CGSMediaPlayerStreamingView::SetCurrentItem(%d)"),aIndex);
	    iCurrentItem = aIndex;
    }      
// End of File

