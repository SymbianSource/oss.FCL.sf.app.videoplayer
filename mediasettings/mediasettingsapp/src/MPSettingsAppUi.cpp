/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Application UI class required by AVKON application architecture.*
*/


// Version : %version: 7 %



// INCLUDE FILES
#include    <MediaSettings.rsg>
#include    <avkon.hrh>
#include    <barsread.h>
#include    <akntabgrp.h>
#include    <featmgr.h>
#include    <hlplch.h>
#include    <StringLoader.h>
#include    <AknsUtils.h>
#include    <mediasettings.mbg>

#include    "mediasettings.hrh"
#include    "MPSettingsAppUi.h"
#include    "MPSettingsMainView.h"
#include    "MPSettingsVideoView.h"
#include    "MPSettingsStreamingView.h"
#include    "MPSettingsProxyView.h"
#include    "MPSettingsNetworkView.h"
#include    "MPSettingsConstants.h"
#include    "MPSettingsPluginView.h"
#include    "MPSettingsAdvancedBwView.h"
#include    "MPSettingsDocument.h"    
#include    "mpsettingsmodel.h"
#include    "MPSettingsModelForROP.h"
#include    "mpxlog.h"
#include    "feedsettingsview.h"

// CONSTANTS
const TInt KMPSettViewIdArrayGranularity = 3;

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// CMPSettingsAppUi::CMPSettingsAppUi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsAppUi::CMPSettingsAppUi() : iConstructAsGsPlugin(EFalse)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsAppUi::CMPSettingsAppUi()"));
    }

// ----------------------------------------------------------
// CMPSettingsAppUi::ConstructL()
// Symbian OS default constructor.
// ----------------------------------------------------------
//
void CMPSettingsAppUi::ConstructL() 
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsAppUi::ConstructL()"));
    // Enable skin support in base construction
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible ); 

    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();   

    CMPSettingsDocument* doc = static_cast<CMPSettingsDocument*>(static_cast<CEikAppUi*>
        (CEikonEnv::Static()->AppUi())->Document());
    iModel = doc->SettingsModel();
    iRopModel = static_cast<CMPSettingsModelForROP*>(iModel);

    // Fetch pointer to the default navi pane control
    CEikStatusPane* sp = StatusPane();
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        (sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));

    // Construct navipane tabs 
    iDecoratedTabGroup = iNaviPane->CreateTabGroupL(this);

    // Construct tabs
    AddTabSettingsGroupL();

    // Create vector which contains view id's for all setting folders. 
    iViewIds = new(ELeave) CArrayFixFlat<TUid>(KMPSettViewIdArrayGranularity);
    iViewIds->AppendL(KMPSettVideoViewId);
    iViewIds->AppendL(KMPSettStreamingViewId);
    iViewIds->AppendL(KMPSettFeedsViewId);

    // * Create application views *

    CAknView* view;  

    view = CMPSettingsMainView::NewLC(iViewIds,iRopModel,iConstructAsGsPlugin);
    AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view

    // iMainView is not owned, i.e. it is not deleted in the destructor ->
    // main view cannot be directly creted to iMainView, as it's against the coding
    // conventions to place class' pointers to cleanup stack => static_cast.
    iMainView = static_cast<CMPSettingsMainView*>(view);

    view = CMPSettingsVideoView::NewLC(iRopModel);
    AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view

    view = CMPSettingsStreamingView::NewLC(iRopModel);
    AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view 

    view = CVcxNsSettingsView::NewLC();
    AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view 
    
    view = CMPSettingsProxyView::NewLC(iRopModel,iConstructAsGsPlugin);
    AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view 

    view = CMPSettingsNetworkView::NewLC(iRopModel,iConstructAsGsPlugin);
    AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view 

    view = CMPSettingsAdvancedBwView::NewLC(iRopModel,iConstructAsGsPlugin);
    AddViewL(view);      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();    // view 


    // Construct plug-in views (if any exists)
    CMPSettingsPluginView::ListImplementationsL(iImplInfoArray);
    TInt count = iImplInfoArray.Count();

    if (count > 0)
        {
        CMPSettingsPluginView* pluginView = NULL;

        for (TInt ii(0); ii < count; ++ii)
            {
            TUid uid = iImplInfoArray[ii]->ImplementationUid();
            pluginView = CMPSettingsPluginView::NewL(uid);
            CleanupStack::PushL(pluginView);
            AddViewL(pluginView);
            CleanupStack::Pop();    // pluginView

            if (pluginView->OpenedFromMainView())
                {
                iViewIds->AppendL(pluginView->Id());
                }
            }

        // Sort UID array to rising order
        TKeyArrayFix sortKey(0, ECmpTUint);
        User::LeaveIfError(iViewIds->Sort(sortKey));
        }
 
    SetDefaultViewL(*iMainView);
    }

// ----------------------------------------------------
// CMPSettingsAppUi::~CMPSettingsAppUi
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CMPSettingsAppUi::~CMPSettingsAppUi()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsAppUi::~CMPSettingsAppUi()"));
    if (iDecoratedTabGroup) 
        {
        delete iDecoratedTabGroup;
        }

    if (iDoorObserver)
        {
        iDoorObserver->NotifyExit(MApaEmbeddedDocObserver::EEmpty);
        }

    if (iViewIds) 
        {
        delete iViewIds;
        }

    iImplInfoArray.ResetAndDestroy();
    FeatureManager::UnInitializeLib();
    }

// ----------------------------------------------------
// CMPSettingsAppUi::HandleCommandL
// ----------------------------------------------------
//
void CMPSettingsAppUi::HandleCommandL(TInt aCommand)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsAppUi::HandleCommandL(%d)"),aCommand);
    switch (aCommand)
        {
        case EEikCmdExit:
            Exit();
            break;
        case EMPSettCmdHelp:
        case EAknCmdHelp:
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), 
			                                     AppHelpContextL() );
            break;
        default:
            CAknViewAppUi::HandleCommandL(aCommand);
            break;      
        }
    }

// ----------------------------------------------------
// CMPSettingsAppUi::HandleResourceChangeL
// ----------------------------------------------------
//
void CMPSettingsAppUi::HandleResourceChangeL( TInt aType )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsAppUi::HandleResourceChangeL(0x%X)"),aType);
    CAknViewAppUi::HandleResourceChangeL( aType ); 
 
    if( aType == KAknsMessageSkinChange )
        {
        AddTabSettingsGroupL();
        }
    }
 

// ----------------------------------------------------
// CMPSettingsAppUi::TabChangedL
// MAknTabObserver callback for handling tab changed event.
// ----------------------------------------------------
//
void CMPSettingsAppUi::TabChangedL(TInt aIndex)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsAppUi::TabChangedL(%d)"),aIndex);
    iMainView->SetActiveFolder(aIndex);
    ActivateLocalViewL(iViewIds->At(aIndex));
    }

// ----------------------------------------------------
// CMPSettingsAppUi::NaviContainer
// ----------------------------------------------------
//
CAknNavigationControlContainer* CMPSettingsAppUi::NaviContainer() const
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsAppUi::NaviContainer()"));
    return iNaviPane;
    }

// ----------------------------------------------------
// CMPSettingsAppUi::MPTabGroup
// ----------------------------------------------------
//
CAknNavigationDecorator* CMPSettingsAppUi::MPTabGroup() const
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsAppUi::MPTabGroup()"));
    return iDecoratedTabGroup;
    }

// ----------------------------------------------------
// CMPSettingsAppUi::UpdateTabIndex
// Updates tabgroup.
// ----------------------------------------------------
//
void CMPSettingsAppUi::UpdateTabIndex()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsAppUi::UpdateTabIndex()"));
    iTabGroup->SetActiveTabByIndex(iMainView->ActiveFolder());
    }

// ------------------------------------------------------------------------------
// CMPSettingsAppUi::DynInitMenuPaneL
//  This function is called by the AVKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CMPSettingsAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsAppUi::DynInitMenuPaneL()"));
    }

// ----------------------------------------------------
// CMPSettingsAppUi::HandleKeyEventL
// Forwards key events to tabgroup when appropriate.
// ----------------------------------------------------
//
TKeyResponse CMPSettingsAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsAppUi::HandleKeyEventL()"));
    // Check if the event should be handled by iTabGroup
    if (aType != EEventKey ||  iNaviPane->Top() != iDecoratedTabGroup)
        {
        return EKeyWasNotConsumed;
        }

    return iTabGroup->OfferKeyEventL(aKeyEvent, aType);
    }


// ----------------------------------------------------
// CMPSettingsAppUi::AddTabSettingsGroupL()
// (Re)create tab group with skin support
// ----------------------------------------------------
//
void CMPSettingsAppUi::AddTabSettingsGroupL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsAppUi::AddTabSettingsGroupL()"));
    TBool iPushed = EFalse;
    TFileName fileName;

    // switch view if in tab
    if(iNaviPane->Top() == iDecoratedTabGroup)
        {
        iNaviPane->PushDefaultL();
        iPushed = ETrue;
        }

    iTabGroup = static_cast<CAknTabGroup*>
        (iDecoratedTabGroup->DecoratedControl());

    iTabGroup->SetTabFixedWidthL(KTabWidthWithThreeTabs);

    HBufC* mbmpath = StringLoader::LoadLC(R_MPSETT_ICON_MBM_FILE);

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

    // Create skinnable tabs
    AddTabSettingsL(KAknsIIDQgnPropSetMpVideoTab3,
        fileName,
        EMbmMediasettingsQgn_prop_set_mp_video_tab2,
        EMbmMediasettingsQgn_prop_set_mp_video_tab2_mask,
        EMPViewVideoTab);

    AddTabSettingsL(KAknsIIDQgnPropSetMpStreamTab3,
        fileName,
        EMbmMediasettingsQgn_prop_set_mp_stream_tab2,
        EMbmMediasettingsQgn_prop_set_mp_stream_tab2_mask,
        EMPViewStreamTab);

    AddTabSettingsL(KAknsIIDQgnPropSetMpStreamTab3,
        fileName,
        EMbmMediasettingsQgn_prop_set_mp_video_tab3,
        EMbmMediasettingsQgn_prop_set_mp_video_tab3_mask,
        EMPViewFeedsTab);
    
    CleanupStack::PopAndDestroy();  // mbmpath

    // return to previous view & update correct tab index
    if (iPushed) 
        {
        iNaviPane->PushL(*iDecoratedTabGroup);
        UpdateTabIndex();
        }
    }

// ----------------------------------------------------
// CMPSettingsAppUi::AddTabSettingsL
// Adds individual tabs.
// ----------------------------------------------------
//
void CMPSettingsAppUi::AddTabSettingsL(
    const TAknsItemID& aID,
    const TDesC& aMbmFile,
    TInt aBitmapId,
    TInt aMaskId,
    TInt aTabId)
    {
    
    MPX_DEBUG1(_L("#MS# CMPSettingsAppUi::AddTabSettingsL()"));
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC(skin, aID,
        bitmap, mask, aMbmFile, aBitmapId, aMaskId);

    // delete if exists
    if(iTabGroup->TabIndexFromId(aTabId) >= 0)
        {
        iTabGroup->DeleteTabL(aTabId);
        }

    iTabGroup->AddTabL(aTabId, bitmap, mask);
    
    CleanupStack::Pop(2); // bitmap, mask
    }

// End of File  
