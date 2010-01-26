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


// Version : %version: 7 %



// INCLUDE FILES
#include    <aknViewAppUi.h>
#include    <MediaSettings.rsg>
#include    <calslbs.h>
#include    <featmgr.h>
#include    <hlplch.h>             // For HlpLauncher
#include    "mediasettings.hrh"
#include    "MPSettingsNaviPaneController.h"
#include    "MPSettingsStreamingView.h"
#include    "MPSettingsStreamingContainer.h"
#include    "MPSettingsConstants.h" 
#include    "mpxlog.h"

class CMPSettingsModelForROP;

// CONSTANTS

const   TInt    KMPSettProxyViewListItemId  =   0;
const   TInt    KMPSettNetworkViewListItemId  =   1;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsStreamingView::CMPSettingsStreamingView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsStreamingView::CMPSettingsStreamingView(CMPSettingsModelForROP* aModel) : iModel( aModel)
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
        case EMPSettCmdOpen:
            HandleListBoxSelectionL();
            break;
        case EAknSoftkeyBack:
            AppUi()->ActivateLocalViewL(KMPSettMainViewId);
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
// CMPSettingsStreamingView::HandleListBoxEventL
// ---------------------------------------------------------------------------

void CMPSettingsStreamingView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
    {
    MPX_FUNC("#MS# CMPSettingsStreamingView::HandleListBoxEventL()");
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
// CMPSettingsStreamingView::DynInitContainerL
// ---------------------------------------------------------------------------
//
void CMPSettingsStreamingView::DynInitContainerL()
    {
    MPX_FUNC("#MS# CMPSettingsStreamingView::DynInitContainerL()");
    CEikTextListBox* listbox = iContainer->ListBox();

    listbox->SetListBoxObserver(this);
    listbox->SetTopItemIndex(iTopItemIndex);

    if (iCurrentItem >= 0 && iCurrentItem < listbox->Model()->NumberOfItems()) // magic
        {
        listbox->SetCurrentItemIndexAndDraw(iCurrentItem);
        }
    CMPSettingsBaseView::SetMiddleSoftKeyLabelL(R_MEDIASETTING_MSK_OPEN,EMPSettCmdOpen);
    }

// ---------------------------------------------------------------------------
// CMPSettingsStreamingView::HandleListBoxSelectionL()
// Handles listbox selection.
// ---------------------------------------------------------------------------
//
void CMPSettingsStreamingView::HandleListBoxSelectionL() 
    {
    iCurrentItem = iContainer->ListBox()->CurrentItemIndex();
    iTopItemIndex = iContainer->ListBox()->TopItemIndex();
    MPX_DEBUG3(_L("#MS# CMPSettingsStreamingView::HandleListBoxSelectionL() iCurrentItem(%d) iTopItemIndex(%d)"),iCurrentItem,iTopItemIndex);

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

// End of File

