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


// Version : %version: 8 %



// INCLUDE FILES
#include    <aknViewAppUi.h>
#include    <MediaSettings.rsg>
#include    <eikmenup.h>
#include    <eiktxlbm.h>
#include    <hlplch.h>             // For HlpLauncher
#include    <featmgr.h>
#include    "mediasettings.hrh"
#include    "MPSettingsNaviPaneController.h"
#include    "MPSettingsVideoView.h"
#include    "MPSettingsVideoContainer.h"
#include    "MPSettingsConstants.h" 
#include    "MPSettingsModelForROP.h"
#include    "mpxlog.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsVideoView::CMPSettingsVideoView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsVideoView::CMPSettingsVideoView(CMPSettingsModelForROP* aModel) 
    : iModel( aModel)
    {
        MPX_FUNC("#MS# CMPSettingsVideoView::CMPSettingsVideoView()");
        iNaviPaneContext = iNaviCntrl->MPTabGroup();
    }

// ---------------------------------------------------------
// CMPSettingsVideoView::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
void CMPSettingsVideoView::ConstructL()
    {
    MPX_FUNC("#MS# CMPSettingsVideoView::ConstructL()");
    BaseConstructL(R_MPSETT_VIDEO_VIEW);
    }

// -----------------------------------------------------------------------------
// CMPSettingsVideoView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPSettingsVideoView* CMPSettingsVideoView::NewLC(CMPSettingsModelForROP* aModel)
    {
    MPX_FUNC("#MS# CMPSettingsVideoView::NewLC()");
    CMPSettingsVideoView* self = new(ELeave) CMPSettingsVideoView(aModel);
    
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------
// CMPSettingsVideoView::~CMPSettingsVideoView()
// Destructor
// ---------------------------------------------------------
//
CMPSettingsVideoView::~CMPSettingsVideoView()
    {
    MPX_FUNC("#MS# CMPSettingsVideoView::~CMPSettingsVideoView()");
    }

// ---------------------------------------------------------
// TUid CMPSettingsVideoView::Id
// ---------------------------------------------------------
//
TUid CMPSettingsVideoView::Id() const
    {
    MPX_FUNC("#MS# CMPSettingsVideoView::Id()");
    return KMPSettVideoViewId;
    }

// ---------------------------------------------------------
// CMPSettingsVideoView::HandleCommandL
// ---------------------------------------------------------
//
void CMPSettingsVideoView::HandleCommandL(TInt aCommand)
    {   
    MPX_DEBUG2(_L("#MS# CMPSettingsVideoView::HandleCommandL(%d)"),aCommand);
    switch (aCommand)
        {
        case EMPSettCmdChange:
            static_cast<CMPSettingsVideoContainer*>(iContainer)->EditCurrentItemFromMenuL(ETrue);
            break;
        
        case EMPMiddleSoftKeyChange:
            static_cast<CMPSettingsVideoContainer*>(iContainer)->EditCurrentItemFromMenuL(EFalse);
            break;
        
        case EAknSoftkeyBack:
            AppUi()->ActivateLocalViewL(KMPSettMainViewId);
            break;
        
        case EMPSettCmdHelp:
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), 
                                                     AppUi()->AppHelpContextL() );
                }
            break;
        
        default:
            AppUi()->HandleCommandL(aCommand);
            break;
        }
    }

// ---------------------------------------------------------------------------
// CMPSettingsVideoView::NewContainerL
// ---------------------------------------------------------------------------
//
CMPSettingsBaseContainer* CMPSettingsVideoView::NewContainerL()
    {
    MPX_FUNC("#MS# CMPSettingsVideoView::NewContainerL()");
    return new(ELeave) CMPSettingsVideoContainer(iModel);
    }  

// ---------------------------------------------------------------------------
// CMPSettingsVideoView::DynInitContainerL
// ---------------------------------------------------------------------------
//
void CMPSettingsVideoView::DynInitContainerL()
    {
    MPX_FUNC("#MS# CMPSettingsVideoView::DynInitContainerL()");
    CEikTextListBox* listbox = iContainer->ListBox();

    listbox->SetTopItemIndex(iTopItemIndex);

    if (iCurrentItem >= 0 && iCurrentItem < listbox->Model()->NumberOfItems()) // magic
        {
        listbox->SetCurrentItemIndexAndDraw(iCurrentItem);
        }
    CMPSettingsBaseView::SetMiddleSoftKeyLabelL(R_MEDIASETTING_MSK_CHANGE,EMPMiddleSoftKeyChange);
    }

// ---------------------------------------------------------------------------
// CMPSettingsVideoView::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CMPSettingsVideoView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane ) 
   {
    MPX_FUNC("#MS# CMPSettingsVideoView::DynInitMenuPaneL()");

    CMPSettingsBaseView::DynInitMenuPaneL( aResourceId, aMenuPane );
    }

// End of File

