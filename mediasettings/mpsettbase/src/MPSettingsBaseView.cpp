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
* Description:   Base class for MediaSettings views.*
*/


// Version : %version: 4 %




// INCLUDE FILES
#include    <aknViewAppUi.h>
#include    <akntitle.h>
#include    <barsread.h>
#include    <aknnavi.h>
#include    <aknnavide.h>
#include    <featmgr.h>
#include    <eikmenup.h>
#include    <MediaSettings.rsg>
#include    <StringLoader.h> 
#include    "mediasettings.hrh"
#include    "MPSettingsAppUi.h"
#include    "MPSettingsBaseView.h"
#include    "MPSettingsBaseContainer.h"
#include    "MPSettingsNaviPaneController.h"
#include    "MPSettingsConstants.h"
#include    "mpxlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsBaseView::CMPSettingsBaseView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CMPSettingsBaseView::CMPSettingsBaseView()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsBaseView::CMPSettingsBaseView()"));
    iNaviCntrl = static_cast<CMPSettingsAppUi*>(AppUi());
    }

// -----------------------------------------------------------------------------
// CMPSettingsBaseView::~CMPSettingsBaseView
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMPSettingsBaseView::~CMPSettingsBaseView()
    {
        MPX_DEBUG1(_L("#MS# CMPSettingsBaseView::~CMPSettingsBaseView()"));
        if (iContainer)
        {
        AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        }

        if (iNaviPaneContext && iNaviPaneContext != iNaviCntrl->MPTabGroup())
        {
        delete iNaviPaneContext;
        }   
    }

// ---------------------------------------------------------
// CMPSettingsBaseView::HandleClientRectChange
// ---------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseView::HandleClientRectChange()
    {
        MPX_DEBUG1(_L("#MS# CMPSettingsBaseView::HandleClientRectChange()"));
        if (iContainer)
        {
        iContainer->SetRect(ClientRect());
        }
    }

// ---------------------------------------------------------------------------
// CMPSettingsBaseView::HandleClientRectChange
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseView::SetCurrentItem(TInt aIndex)
    {
        MPX_DEBUG2(_L("#MS# CMPSettingsBaseView::SetCurrentItem(%d)"),aIndex);
        iCurrentItem = aIndex;
    }

// ---------------------------------------------------------
// CMPSettingsBaseView::DoActivateL
// ---------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
        MPX_DEBUG1(_L("#MS# CMPSettingsBaseView::DoActivateL()"));
        // Set correct navi pane
        SetNaviPaneL();

        // Create iContainer if one does not exist
        if (!iContainer)
        {
            iContainer = NewContainerL();

            __ASSERT_DEBUG(iContainer, User::Panic(_L("DoActivateL"), EMPViewPanicNullPtr));

            iContainer->SetMopParent(this);

            // Construction of component control could fail => delete iContainer in order 
            // to quarantee that it is constructed again the next time it is used.
            TRAPD(error, iContainer->ConstructL(ClientRect()));
            if (error)
                {
                delete iContainer;
                iContainer = NULL;
                User::Leave(error);
                }

            AppUi()->AddToStackL(*this, iContainer);

            DynInitContainerL();
        } 
   }

// ---------------------------------------------------------
// CMPSettingsBaseView::HandleCommandL
// ---------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseView::DoDeactivate()
    {
        MPX_DEBUG1(_L("#MS# CMPSettingsBaseView::DoDeactivate()"));
        if (iContainer)
        {
            CEikTextListBox* listbox = iContainer->ListBox();
            if (listbox)
                {
                iCurrentItem = listbox->CurrentItemIndex();
                iTopItemIndex = listbox->TopItemIndex();
                }

            AppUi()->RemoveFromStack(iContainer);
            delete iContainer;
            iContainer = NULL;
        }    
    }

// ---------------------------------------------------------------------------
// CMPSettingsBaseView::DynInitMenuPaneL
// Before showing a options menu
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
        MPX_DEBUG2(_L("#MS# CMPSettingsBaseView::DynInitMenuPaneL(0x%X)"),aResourceId);
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
// CMPSettingsBaseView::SetNaviPaneL
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseView::SetNaviPaneL()
    {
        MPX_DEBUG1(_L("#MS# CMPSettingsBaseView::SetNaviPaneL()"));
        if (iNaviPaneContext)
        {
            iNaviCntrl->NaviContainer()->PushL(*iNaviPaneContext);
            if (iNaviPaneContext == iNaviCntrl->MPTabGroup())
                {
                iNaviCntrl->UpdateTabIndex();
                }
        }
        else 
        {
            iNaviCntrl->NaviContainer()->PushDefaultL();
        }
    }

// ---------------------------------------------------------------------------
// CMPSettingsBaseView::DynInitContainerL
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseView::DynInitContainerL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsBaseView::DynInitContainerL()"));
    // Do nothing.
    }

// ---------------------------------------------------------------------------
// CMPSettingsBaseView::SetMiddleSoftKeyLabelL
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPSettingsBaseView::SetMiddleSoftKeyLabelL( const TInt aResourceId, const TInt aCommandId )
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsBaseView::SetMiddleSoftKeyLabelL()"));
    CEikButtonGroupContainer* cbaGroup = Cba();
    if ( cbaGroup )
        {
        HBufC* middleSKText = StringLoader::LoadLC( aResourceId );
        TPtr mskPtr = middleSKText->Des();
        MPX_DEBUG2(_L("#MS# CMPSettingsBaseView::SetMiddleSoftKeyLabelL(%S)"),&mskPtr);
        cbaGroup->AddCommandToStackL(
            KMediaSettingsMSKControlID,
            aCommandId,
            mskPtr );
        CleanupStack::PopAndDestroy( middleSKText );
        }
    }

//  End of File  
