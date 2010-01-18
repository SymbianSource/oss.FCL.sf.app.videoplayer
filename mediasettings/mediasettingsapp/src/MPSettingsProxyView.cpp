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
* Description:   View class for Proxy setting list view.*
*/


// Version : %version: 4 %



// INCLUDE FILES
#include    <aknViewAppUi.h>
#include    <hlplch.h>             // For HlpLauncher
#include    <featmgr.h>
#include    <MediaSettings.rsg>
#include    "mediasettings.hrh"
#include    "MPSettingsNaviPaneController.h"
#include    "MPSettingsProxyView.h"
#include    "MPSettingsProxyContainer.h"
#include    "MPSettingsConstants.h" 
#include    "mpxlog.h"

class CMPSettingsModelForROP;
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsProxyView::CMPSettingsProxyView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsProxyView::CMPSettingsProxyView(CMPSettingsModelForROP* aModel, TBool iConstructAsGsPlugin) 
    : iModel( aModel), iGsPlugin(iConstructAsGsPlugin)
    {
        MPX_FUNC("#MS# CMPSettingsProxyView::CMPSettingsProxyView()");
    }

// ---------------------------------------------------------
// CMPSettingsProxyView::ConstructL(const TRect& aRect)
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
void CMPSettingsProxyView::ConstructL()
    {
    MPX_FUNC("#MS# CMPSettingsProxyView::ConstructL()");
    BaseConstructL(R_MPSETT_PROXY_VIEW);
    }

// -----------------------------------------------------------------------------
// CMPSettingsProxyView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPSettingsProxyView* CMPSettingsProxyView::NewLC(CMPSettingsModelForROP* aModel, TBool iConstructAsGsPlugin)
    {
    MPX_FUNC("#MS# CMPSettingsProxyView::NewLC()");
    CMPSettingsProxyView* self = new(ELeave) CMPSettingsProxyView(aModel,iConstructAsGsPlugin);
    
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------
// CMPSettingsProxyView::~CMPSettingsProxyView()
// Destructor
// ---------------------------------------------------------
//
CMPSettingsProxyView::~CMPSettingsProxyView()
    {
    MPX_FUNC("#MS# CMPSettingsProxyView::~CMPSettingsProxyView()");
    }

// ---------------------------------------------------------
// TUid CMPSettingsProxyView::Id
// ---------------------------------------------------------
//
TUid CMPSettingsProxyView::Id() const
    {
    MPX_FUNC("#MS# CMPSettingsProxyView::Id()");
    return KMPSettProxyViewId;
    }

// ---------------------------------------------------------
// CMPSettingsProxyView::HandleCommandL
// ---------------------------------------------------------
//
void CMPSettingsProxyView::HandleCommandL(TInt aCommand)
    {   
    MPX_DEBUG2(_L("#MS# CMPSettingsProxyView::HandleCommandL(%d)"),aCommand);
    switch (aCommand)
        {
        case EMPSettCmdChange:
            static_cast<CMPSettingsProxyContainer*>(iContainer)->EditCurrentItemFromMenuL(ETrue);
            break;
        case EMPMiddleSoftKeyChange:
            static_cast<CMPSettingsProxyContainer*>(iContainer)->EditCurrentItemFromMenuL(EFalse);
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
// CMPSettingsProxyView::NewContainerL
// ---------------------------------------------------------------------------
//
CMPSettingsBaseContainer* CMPSettingsProxyView::NewContainerL()
    {
    MPX_FUNC("#MS# CMPSettingsProxyView::NewContainerL()");
    return new(ELeave) CMPSettingsProxyContainer(iModel);
    }  

// ---------------------------------------------------------------------------
// CMPSettingsProxyView::SetNaviPaneL
// Sets Navipane texts.
// ---------------------------------------------------------------------------
//
void CMPSettingsProxyView::SetNaviPaneL()
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsProxyView::SetNaviPaneL() iGsPlugin(%d)"),iGsPlugin);
    if (!iGsPlugin)
        {
        iNaviPaneContext = NULL;
        CMPSettingsBaseView::SetNaviPaneL();
        }
    CMPSettingsBaseView::SetMiddleSoftKeyLabelL(R_MEDIASETTING_MSK_CHANGE,EMPMiddleSoftKeyChange);
    }
// End of File

