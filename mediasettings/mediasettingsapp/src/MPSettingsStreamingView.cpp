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
#include    <aknViewAppUi.h>
#include    <MediaSettings.rsg>
#include    <calslbs.h>
#include    <featmgr.h>
#include    <hlplch.h>             // For HlpLauncher
#include    "mediasettings.hrh"
#include    "MPSettingsAppUi.h"
#include    "MPSettingsNaviPaneController.h"
#include    "MPSettingsStreamingView.h"
#include    "MPSettingsStreamingContainer.h"
#include    "MPSettingsConstants.h" 
#include    "mpxlog.h"

class CMPSettingsModelForROP;


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsStreamingView::CMPSettingsStreamingView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsStreamingView::CMPSettingsStreamingView(CMPSettingsModelForROP* aModel) 
    : iModel( aModel)
    {
    MPX_FUNC("#MS# CMPSettingsStreamingView::CMPSettingsStreamingView()");
    iNaviPaneContext = iNaviCntrl->MPTabGroup();
    }

// ---------------------------------------------------------
// CMPSettingsStreamingView::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
void CMPSettingsStreamingView::ConstructL()
    {
    MPX_FUNC("#MS# CMPSettingsStreamingView::ConstructL()");
    BaseConstructL(R_MPSETT_STREAMING_VIEW);
    }

// -----------------------------------------------------------------------------
// CMPSettingsStreamingView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPSettingsStreamingView* CMPSettingsStreamingView::NewLC(CMPSettingsModelForROP* aModel)
    {
    MPX_FUNC("#MS# CMPSettingsStreamingView::NewLC()");
    CMPSettingsStreamingView* self = new(ELeave) CMPSettingsStreamingView(aModel);
    
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------
// CMPSettingsStreamingView::~CMPSettingsStreamingView
// Destructor
// ---------------------------------------------------------
//
CMPSettingsStreamingView::~CMPSettingsStreamingView()
    {
    MPX_FUNC("#MS# CMPSettingsStreamingView::~CMPSettingsStreamingView()");
    }

// ---------------------------------------------------------
// TUid CMPSettingsStreamingView::Id
// ---------------------------------------------------------
//
TUid CMPSettingsStreamingView::Id() const
    {
    MPX_FUNC("#MS# CMPSettingsStreamingView::Id()");
    return KMPSettStreamingViewId;
    }

// ---------------------------------------------------------
// CMPSettingsStreamingView::HandleCommandL
// ---------------------------------------------------------
//
void CMPSettingsStreamingView::HandleCommandL(TInt aCommand)
    {   
    MPX_DEBUG2(_L("#MS# CMPSettingsStreamingView::HandleCommandL(%d)"),aCommand);
    switch (aCommand)
        {
        case EMPSettCmdChange:
            static_cast<CMPSettingsStreamingContainer*>(iContainer)->EditCurrentItemFromMenuL(ETrue);
            break;
        case EMPMiddleSoftKeyChange:
            static_cast<CMPSettingsStreamingContainer*>(iContainer)->EditCurrentItemFromMenuL(EFalse);
            break;  
        case EMPSettCmdAdvancedSett:
            AppUi()->ActivateLocalViewL( KMPSettAdvancedBwViewId );
            break;            
        case EMPSettCmdOpen:
            break;
        case EAknSoftkeyBack:
            {
            // If video view is empty, then exit Settings
            CMPSettingsAppUi* appUi = static_cast<CMPSettingsAppUi*>(AppUi());
            appUi->ActivateLocalViewL( KMPSettMainViewId );
            }
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
// CMPSettingsStreamingView::NewContainerL
// ---------------------------------------------------------------------------
//
CMPSettingsBaseContainer* CMPSettingsStreamingView::NewContainerL()
    {
    MPX_FUNC("#MS# CMPSettingsStreamingView::NewContainerL()");
    return new(ELeave) CMPSettingsStreamingContainer(iModel);
    }  


// ---------------------------------------------------------------------------
// CMPSettingsStreamingView::DynInitContainerL
// ---------------------------------------------------------------------------
//
void CMPSettingsStreamingView::DynInitContainerL()
    {
    MPX_FUNC("#MS# CMPSettingsStreamingView::DynInitContainerL()");
    CEikTextListBox* listbox = iContainer->ListBox();

    listbox->SetTopItemIndex(iTopItemIndex);

    if (iCurrentItem >= 0 && iCurrentItem < listbox->Model()->NumberOfItems()) // magic
        {
        listbox->SetCurrentItemIndexAndDraw(iCurrentItem);
        }
    CMPSettingsBaseView::SetMiddleSoftKeyLabelL(R_MEDIASETTING_MSK_OPEN,EMPSettCmdOpen);
    }


// End of File

