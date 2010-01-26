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
* Description:   MPSettingsMainView.cpp*
*/


// Version : %version: 7 %



// INCLUDE FILES
#include    <avkon.hrh>
#include    <MediaSettings.rsg>
#include    <calslbs.h>
#include    "MPSettingsAppUi.h"
#include    "MPSettingsMainView.h"
#include    "MPSettingsMainContainer.h" 
#include    "mediasettings.hrh"
#include    "MPSettingsConstants.h"
#include    "MPSettingsModelForROP.h"
#include    "mpxlog.h"

// CONSTANTS
const TInt KMPSettTopItemIndex = 0;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsMainView::CMPSettingsMainView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsMainView::CMPSettingsMainView(CArrayFix<TUid>* aViewIds, CMPSettingsModelForROP* aModel, TBool iConstructAsGsPlugin) 
    : iViewIds(aViewIds), iModel(aModel), iGsPlugin(iConstructAsGsPlugin)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsMainView::CMPSettingsMainView() iGsPlugin(%d)"),iGsPlugin);
    }

// ---------------------------------------------------------
// CMPSettingsMainView::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CMPSettingsMainView::ConstructL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsMainView::ConstructL()"));
    BaseConstructL(R_MPSETT_MAIN_VIEW);
    }

// -----------------------------------------------------------------------------
// CMPSettingsMainView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPSettingsMainView* CMPSettingsMainView::NewLC(CArrayFix<TUid>* aViewIds, CMPSettingsModelForROP* aModel, TBool iConstructAsGsPlugin)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsMainView::NewLC()"));
    CMPSettingsMainView* self = new(ELeave) CMPSettingsMainView(aViewIds,aModel,iConstructAsGsPlugin);
    
    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------
// CMPSettingsMainView::~CMPSettingsMainView
// ---------------------------------------------------------
//
CMPSettingsMainView::~CMPSettingsMainView()
    {   
    MPX_DEBUG1(_L("#MS# CMPSettingsMainView::~CMPSettingsMainView()"));
    }

// ---------------------------------------------------------
// TUid CMPSettingsMainView::Id
// ---------------------------------------------------------
//
TUid CMPSettingsMainView::Id() const
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsMainView::Id()"));
    return KMPSettMainViewId;
    }

// ---------------------------------------------------------
// CMPSettingsMainView::HandleCommandL
// ---------------------------------------------------------
//
void CMPSettingsMainView::HandleCommandL(TInt aCommand)
    {   
    MPX_DEBUG2(_L("#MS# CMPSettingsMainView::HandleCommandL(%d)"),aCommand);
    switch (aCommand)
        {
        case EMPSettCmdOpen:
            HandleListBoxSelectionL();
            break;
        case EAknSoftkeyBack:
            if (iGsPlugin)
            {
                AppUi()->ActivateLocalViewL( KGSVideoPluginUid );       
            }
            else
            {
                AppUi()->HandleCommandL(EEikCmdExit);
            }
            break;
        default:
            AppUi()->HandleCommandL(aCommand);
            break;
        }
    }

// ---------------------------------------------------------------------------
// CMPSettingsMainView::NewContainerL
// ---------------------------------------------------------------------------
//
CMPSettingsBaseContainer* CMPSettingsMainView::NewContainerL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsMainView::NewContainerL()"));
    return new(ELeave) CMPSettingsMainContainer;
    }  

// ---------------------------------------------------------------------------
// CMPSettingsMainView::ActiveFolder
// ---------------------------------------------------------------------------
//
TInt CMPSettingsMainView::ActiveFolder()
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsMainView::ActiveFolder() ret %d"),iCurrentItem);
    return iCurrentItem;
    }

// ---------------------------------------------------------------------------
// CMPSettingsMainView::SetActiveFolder
// ---------------------------------------------------------------------------
//
void CMPSettingsMainView::SetActiveFolder(TInt aCurrentItem)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsMainView::SetActiveFolder(%d)"),aCurrentItem);
    iCurrentItem = aCurrentItem;
    }

// ---------------------------------------------------------------------------
// CMPSettingsMainView::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CMPSettingsMainView::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsMainView::HandleListBoxEventL(0x%X)"),aEventType);
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
// CMPSettingsMainView::DynInitContainerL
// ---------------------------------------------------------------------------
//
void CMPSettingsMainView::DynInitContainerL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsMainView::DynInitContainerL()"));
    CEikTextListBox* listbox = iContainer->ListBox();

    listbox->SetListBoxObserver(this);
    listbox->SetTopItemIndex(iTopItemIndex);

    if (iCurrentItem >= 0 && iCurrentItem < listbox->Model()->NumberOfItems())
        {
        listbox->SetCurrentItemIndexAndDraw(iCurrentItem);
        }
    CMPSettingsBaseView::SetMiddleSoftKeyLabelL(R_MEDIASETTING_MSK_OPEN,EMPSettCmdOpen);
    }

// ---------------------------------------------------------------------------
// CMPSettingsMainView::HandleListBoxSelectionL()
// Handles listbox selection.
// ---------------------------------------------------------------------------
//
void CMPSettingsMainView::HandleListBoxSelectionL() 
    {
    iCurrentItem = iContainer->ListBox()->CurrentItemIndex();
    iTopItemIndex = iContainer->ListBox()->TopItemIndex();
    MPX_DEBUG3(_L("#MS# CMPSettingsMainView::HandleListBoxSelectionL() iCurrentItem(%d,%d)"),iCurrentItem,iTopItemIndex);
    
    CMPSettingsBaseView* view = NULL;
    TInt count = iViewIds->Count();

    for (TInt i = 0; i < count; i++)
        {
        view = static_cast<CMPSettingsBaseView*>(AppUi()->View(iViewIds->At(i)));
        view->SetCurrentItem(KMPSettTopItemIndex);
        }

    AppUi()->ActivateLocalViewL(iViewIds->At(iCurrentItem));
    }


// End of File

