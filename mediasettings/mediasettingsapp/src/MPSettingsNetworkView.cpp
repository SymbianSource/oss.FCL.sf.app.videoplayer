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
* Description:   View class for Network settings list view.*
*/


// Version : %version: 4 %



// INCLUDE FILES
#include    <aknViewAppUi.h>
#include    <hlplch.h>             // For HlpLauncher
#include    <featmgr.h>
#include    <MediaSettings.rsg>
#include    "mediasettings.hrh"
#include    "MPSettingsNaviPaneController.h"
#include    "MPSettingsNetworkView.h"
#include    "MPSettingsNetworkContainer.h"
#include    "MPSettingsConstants.h" 
#include    "mpxlog.h"

class CMPSettingsModelForROP;
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsNetworkView::CMPSettingsNetworkView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsNetworkView::CMPSettingsNetworkView(CMPSettingsModelForROP* aModel, TBool iConstructAsGsPlugin) 
    : iModel( aModel), iGsPlugin(iConstructAsGsPlugin)
    {
    MPX_FUNC("#MS# CMPSettingsNetworkView::CMPSettingsNetworkView()");
    }

// ---------------------------------------------------------
// CMPSettingsNetworkView::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
void CMPSettingsNetworkView::ConstructL()
    {
    MPX_FUNC("#MS# CMPSettingsNetworkView::ConstructL()");
    BaseConstructL(R_MPSETT_NETWORK_VIEW);
    }

// -----------------------------------------------------------------------------
// CMPSettingsNetworkView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPSettingsNetworkView* CMPSettingsNetworkView::NewLC(CMPSettingsModelForROP* aModel, TBool iConstructAsGsPlugin)
    {
    MPX_FUNC("#MS# CMPSettingsNetworkView::NewLC()");
    CMPSettingsNetworkView* self = new(ELeave) CMPSettingsNetworkView(aModel,iConstructAsGsPlugin);
    
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------
// CMPSettingsNetworkView::~CMPSettingsNetworkView()
// Destructor
// ---------------------------------------------------------
//
CMPSettingsNetworkView::~CMPSettingsNetworkView()
    {
    MPX_FUNC("#MS# CMPSettingsNetworkView::~CMPSettingsNetworkView()");
    }

// ---------------------------------------------------------
// TUid CMPSettingsNetworkView::Id
// ---------------------------------------------------------
//
TUid CMPSettingsNetworkView::Id() const
    {
    MPX_FUNC("#MS# CMPSettingsNetworkView::Id()");
    return KMPSettNetworkViewId;
    }

// ---------------------------------------------------------
// CMPSettingsNetworkView::HandleCommandL
// ---------------------------------------------------------
//
void CMPSettingsNetworkView::HandleCommandL(TInt aCommand)
    {   
    MPX_DEBUG2(_L("#MS# CMPSettingsNetworkView::HandleCommandL(%d)"),aCommand);
    switch (aCommand)
        {
        case EMPSettCmdChange:
            static_cast<CMPSettingsNetworkContainer*>(iContainer)->EditCurrentItemFromMenuL();
            break;
        case EMPSettCmdAdvancedSett:
            AppUi()->ActivateLocalViewL(KMPSettAdvancedBwViewId);
            break;
        case EAknSoftkeyBack:
            AppUi()->ActivateLocalViewL(KMPSettStreamingViewId);
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
// CMPSettingsNetworkView::NewContainerL
// ---------------------------------------------------------------------------
//
CMPSettingsBaseContainer* CMPSettingsNetworkView::NewContainerL()
    {
    MPX_FUNC("#MS# CMPSettingsNetworkView::NewContainerL()");
    return new(ELeave) CMPSettingsNetworkContainer(iModel);
    }  

// ---------------------------------------------------------------------------
// CMPSettingsNetworkView::SetNaviPaneL
// Sets Navipane texts.
// ---------------------------------------------------------------------------
//
void CMPSettingsNetworkView::SetNaviPaneL()
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsNetworkView::SetNaviPaneL() iGsPlugin(%d)"),iGsPlugin);
    if (!iGsPlugin)
        {
        iNaviPaneContext = NULL;
        CMPSettingsBaseView::SetNaviPaneL();
        }
    CMPSettingsBaseView::SetMiddleSoftKeyLabelL(R_MEDIASETTING_MSK_CHANGE,EMPSettCmdChange);
    }
// End of File

