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
* Description:   View class for Video settings list view.*
*/


// Version : %version: 7 %



// INCLUDE FILES
#include    <avkon.hrh>
#include    <aknViewAppUi.h>
#include    <akntabgrp.h>
#include    <akntitle.h>  // CAknTitlePane
#include    <featmgr.h>
#include    <eikmenup.h>
#include    <barsread.h>
#include    <eiktxlbm.h>
#include    <MediaSettings.rsg>
#include    <gstabhelper.h>
#include    <hlplch.h>             // For HlpLauncher

#include    "GSVideoPlugin.h"
#include    "mediasettings.hrh"
#include    <mediasettings.mbg>
#include    "GSMediaPlayerVideoView.h"
#include    "MPSettingsVideoContainer.h"
#include    "MPSettingsConstants.h" 
#include    "MPSettingsModelForROP.h"
#include    "mpxlog.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CGSMediaPlayerVideoView::CGSMediaPlayerVideoView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGSMediaPlayerVideoView::CGSMediaPlayerVideoView( 
        CMPSettingsModelForROP* aModel, 
        CArrayPtrFlat<MGSTabbedView>* /* aTabViewArray */ ) 
    : iModel( aModel)
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::CGSMediaPlayerVideoView()");
    }

// ---------------------------------------------------------
// CGSMediaPlayerVideoView::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
void CGSMediaPlayerVideoView::ConstructL(CArrayPtrFlat<MGSTabbedView>* aTabViewArray)
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::ConstructL()");
    iTabHelper = CGSTabHelper::NewL();
    aTabViewArray->AppendL(this);
    BaseConstructL(R_GS_MPSETT_VIDEO_VIEW);
    }

// -----------------------------------------------------------------------------
// CGSMediaPlayerVideoView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSMediaPlayerVideoView* CGSMediaPlayerVideoView::NewLC( 
        CMPSettingsModelForROP* aModel, 
        CArrayPtrFlat<MGSTabbedView>* aTabViewArray )
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::NewLC()");
    CGSMediaPlayerVideoView* self = 
        new(ELeave) CGSMediaPlayerVideoView( aModel, aTabViewArray );
    
    CleanupStack::PushL(self);
    self->ConstructL( aTabViewArray );    

    return self;
    }

// ---------------------------------------------------------
// CGSMediaPlayerVideoView::~CGSMediaPlayerVideoView()
// Destructor
// ---------------------------------------------------------
//
CGSMediaPlayerVideoView::~CGSMediaPlayerVideoView()
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::~CGSMediaPlayerVideoView()");
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
    delete iTabHelper;
    }

// ---------------------------------------------------------
// TUid CGSMediaPlayerVideoView::Id
// ---------------------------------------------------------
//
TUid CGSMediaPlayerVideoView::Id() const
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::Id()");
    return KMPSettVideoViewId;
    }

// ---------------------------------------------------------
// CGSMediaPlayerVideoView::HandleCommandL
// ---------------------------------------------------------
//
void CGSMediaPlayerVideoView::HandleCommandL( TInt aCommand )
    {   
    MPX_DEBUG2(_L("#MS# CGSMediaPlayerVideoView::HandleCommandL(%d)"),aCommand);
    CGSVideoPlugin* parent =
        static_cast<CGSVideoPlugin*> (
            AppUi()->View( KGSVideoPluginUid ) );
    switch ( aCommand )
        {
        case EMPSettCmdChange:
            {
            static_cast<CMPSettingsVideoContainer*>(iContainer)->EditCurrentItemFromMenuL(ETrue);
            break;
            }
            
        case EMPMiddleSoftKeyChange:
            {
            static_cast<CMPSettingsVideoContainer*>(iContainer)->EditCurrentItemFromMenuL(EFalse);
            break;
            }
            
        case EAknSoftkeyBack:
            {
            parent->SetCurrentItem(EGSMediaSettingsVideoIndex);
            parent->ResetSelectedItemIndex();
            AppUi()->ActivateLocalViewL(KGSVideoPluginUid);
            break;
            }
            
        case EMPSettCmdHelp:
            {
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), 
                                                     AppUi()->AppHelpContextL() );
                }
            break;
            }
            
        default:
            {
            AppUi()->HandleCommandL(aCommand);
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerVideoView::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerVideoView::DoActivateL( const TVwsViewId& /* aPrevViewId */,
                                          TUid /*aCustomMessageId*/,
                                          const TDesC8& /*aCustomMessage*/ )
    { 
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::DoActivateL()");
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
// CGSMediaPlayerVideoView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerVideoView::DoDeactivate()
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::DoDeactivate()");
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
// CGSMediaPlayerVideoView::Container
// Returns network container item
// ---------------------------------------------------------------------------
//
CMPSettingsBaseContainer* CGSMediaPlayerVideoView::Container()
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::Container()");
    return static_cast <CMPSettingsVideoContainer*> ( iContainer );
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerVideoView::DynInitMenuPaneL
// Before showing a options menu
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerVideoView::DynInitMenuPaneL( TInt aResourceId, 
                                                CEikMenuPane* aMenuPane)
    {
    MPX_DEBUG2(_L("#MS# CGSMediaPlayerVideoView::DynInitMenuPaneL(0x%X)"),aResourceId);
    if ( aResourceId == R_MPSETT_APP_MENU )
        {
        // Help should be displayed only if the feature is supported according
        // to Feature Manager
        if ( !FeatureManager::FeatureSupported(KFeatureIdHelp) )
            {
            aMenuPane->SetItemDimmed(EMPSettCmdHelp, ETrue);
            }
        
        }
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerVideoView::NewContainerL()
// Creates new iContainer.
// ---------------------------------------------------------------------------
void CGSMediaPlayerVideoView::NewContainerL()
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::NewContainerL()");
    iContainer = new (ELeave) CMPSettingsVideoContainer(iModel);
    }

// -----------------------------------------------------------------------------
// CGSMediaPlayerVideoView::HandleClientRectChange
// Handle screen size change.
// -----------------------------------------------------------------------------
//
void CGSMediaPlayerVideoView::HandleClientRectChange()
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::HandleClientRectChange()");
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        } 
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerVideoView::CreateContainerL()
// Creates new iContainer.
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerVideoView::CreateContainerL()
    {        
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::CreateContainerL()");
    NewContainerL();
    __ASSERT_DEBUG( 
        iContainer, User::Panic( KGSDoActivateError, EGSViewPanicNullPtr ) );
    iContainer->SetMopParent( this );

    TRAPD( error, iContainer->ConstructL( ClientRect() ) );
    if ( error )
        {
        delete iContainer;
        iContainer = NULL;
        User::Leave( error );
        }
    }  
    
// ---------------------------------------------------------------------------
// CGSMediaPlayerVideoView::CreateTabIconL()
// Creates icon for tab.
// ---------------------------------------------------------------------------
//    
CGulIcon* CGSMediaPlayerVideoView::CreateTabIconL()
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::CreateTabIconL()");
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
        EMbmMediasettingsQgn_prop_set_mp_video_tab2,
        EMbmMediasettingsQgn_prop_set_mp_video_tab2_mask );

    return icon;
    }       
        
// ---------------------------------------------------------------------------
// CGSMediaPlayerVideoView::DynInitContainerL
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerVideoView::DynInitContainerL()
    {
    MPX_FUNC("#MS# CGSMediaPlayerVideoView::DynInitContainerL()");
    CEikTextListBox* listbox = iContainer->ListBox();

    listbox->SetTopItemIndex(iTopItemIndex);

    if ( iCurrentItem >= 0 && 
         iCurrentItem < listbox->Model()->NumberOfItems()) // magic
        {
        listbox->SetCurrentItemIndexAndDraw(iCurrentItem);
        }
    }

// ---------------------------------------------------------------------------
// CGSMediaPlayerVideoView::HandleClientRectChange
// ---------------------------------------------------------------------------
//
void CGSMediaPlayerVideoView::SetCurrentItem(TInt aIndex)
    {
    MPX_DEBUG2(_L("#MS# CGSMediaPlayerVideoView::SetCurrentItem(%d)"),aIndex);
    iCurrentItem = aIndex;
    }   
// End of File

