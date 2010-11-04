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
* Description:   View class for Advanced bandwidth settings list view.*
*/


// Version : %version: 8 %



// INCLUDE FILES
#include    <aknViewAppUi.h>
#include    <hlplch.h>             // For HlpLauncher
#include    <featmgr.h>
#include    <MediaSettings.rsg>

#include    "mediasettings.hrh"
#include    "MPSettingsNaviPaneController.h"
#include    "MPSettingsAdvancedBwView.h"
#include    "MPSettingsAdvancedBwContainer.h"
#include    "MPSettingsConstants.h" 
#include    "mpxlog.h"

class CMPSettingsModelForROP; 
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwView::CMPSettingsAdvancedBwView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsAdvancedBwView::CMPSettingsAdvancedBwView( 
        CMPSettingsModelForROP* aModel,
        TBool iConstructAsGsPlugin ) 
    : iModel( aModel), 
      iGsPlugin(iConstructAsGsPlugin)
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwView::CMPSettingsAdvancedBwView()");
    }

// ---------------------------------------------------------
// CMPSettingsAdvancedBwView::ConstructL
// Symbian OS two-phased constructor
// ---------------------------------------------------------
//
void CMPSettingsAdvancedBwView::ConstructL()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwView::ConstructL()");
    BaseConstructL(R_MPSETT_ADVANCED_BW_VIEW);
    }

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPSettingsAdvancedBwView* CMPSettingsAdvancedBwView::NewLC(
        CMPSettingsModelForROP* aModel, 
        TBool iConstructAsGsPlugin )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwView::NewLC(%d)"),iConstructAsGsPlugin);
    CMPSettingsAdvancedBwView* self = new(ELeave) CMPSettingsAdvancedBwView(aModel,iConstructAsGsPlugin);
    
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------
// CMPSettingsAdvancedBwView::~CMPSettingsAdvancedBwView()
// Destructor
// ---------------------------------------------------------
//
CMPSettingsAdvancedBwView::~CMPSettingsAdvancedBwView()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwView::~CMPSettingsAdvancedBwView()");
    }

// ---------------------------------------------------------
// TUid CMPSettingsAdvancedBwView::Id
// ---------------------------------------------------------
//
TUid CMPSettingsAdvancedBwView::Id() const
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwView::Id()");
    return KMPSettAdvancedBwViewId;
    }

// ---------------------------------------------------------
// CMPSettingsAdvancedBwView::HandleCommandL
// ---------------------------------------------------------
//
void CMPSettingsAdvancedBwView::HandleCommandL(TInt aCommand)
    {   
    MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwView::HandleCommandL(%d)"),aCommand);
    switch (aCommand)
        {
        case EMPSettCmdChange:
            static_cast<CMPSettingsAdvancedBwContainer*>(iContainer)->EditCurrentItemFromMenuL();
            break;
        case EAknSoftkeyBack:
            AppUi()->ActivateLocalViewL( KMPSettStreamingViewId );
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
// CMPSettingsAdvancedBwView::NewContainerL
// ---------------------------------------------------------------------------
//
CMPSettingsBaseContainer* CMPSettingsAdvancedBwView::NewContainerL()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwView::NewContainerL()");
    return new(ELeave) CMPSettingsAdvancedBwContainer(iModel);
    }  

// ---------------------------------------------------------------------------
// CMPSettingsAdvancedBwView::SetNaviPaneL
// Sets Navipane texts.
// ---------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwView::SetNaviPaneL()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwView::SetNaviPaneL()");
    if (!iGsPlugin)
        {
        iNaviPaneContext = NULL;
        CMPSettingsBaseView::SetNaviPaneL();
        }
    CMPSettingsBaseView::SetMiddleSoftKeyLabelL(R_MEDIASETTING_MSK_CHANGE,EMPSettCmdChange);
    }
// End of File

