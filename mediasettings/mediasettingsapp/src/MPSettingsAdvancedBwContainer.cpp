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
* Description:   Container class for Advanced bandwidth setting list view.*
*/


// Version : %version: 7 %



// INCLUDE FILES
#include    <aknlists.h>
#include    <barsread.h>
#include    <MediaSettings.rsg>
#include    <csxhelp/iptv.hlp.hrh>

#include    "MPSettingsApp.h"
#include    "MPSettingsAdvancedBwContainer.h"
#include    "MPSettingsAdvancedBwSettingItemList.h"
#include    "MPSettingsConstants.h"
#include	"mpxlog.h"

class CMPSettingsModelForROP;
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsAdvancedBwContainer::CMPSettingsAdvancedBwContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsAdvancedBwContainer::CMPSettingsAdvancedBwContainer(CMPSettingsModelForROP* aModel) 
	: iModel( aModel)
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwContainer::CMPSettingsAdvancedBwContainer()");
    }

// ---------------------------------------------------------
// CMPSettingsAdvancedBwContainer::~CMPSettingsAdvancedBwContainer
// Destructor
// ---------------------------------------------------------
//
CMPSettingsAdvancedBwContainer::~CMPSettingsAdvancedBwContainer()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwContainer::~CMPSettingsAdvancedBwContainer()");
	if (iSettingList) 
		{
		delete iSettingList;
		}
    }

// ---------------------------------------------------------
// CMPSettingsAdvancedBwContainer::EditCurrentItemFromMenuL
// ---------------------------------------------------------
//
void CMPSettingsAdvancedBwContainer::EditCurrentItemFromMenuL()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwContainer::EditCurrentItemFromMenuL()");
    TInt index = iListBox->CurrentItemIndex();
    iSettingList->EditItemL(index, ETrue);
    }

// ---------------------------------------------------------
// CMPSettingsAdvancedBwContainer::ConstructComponentControlsL
// ---------------------------------------------------------
//
void CMPSettingsAdvancedBwContainer::ConstructComponentControlsL()
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwContainer::ConstructComponentControlsL()");
    iSettingList = new(ELeave) CMPSettingsAdvancedBwSettingItemList(iModel);

    iSettingList->SetContainerWindowL(*this);

    iSettingList->ConstructFromResourceL(R_MPSETT_BEARER_BW_SETTING_ITEM_LIST);

    iComponentControl = iSettingList;
    iListBox = iSettingList->ListBox();
    }

// ---------------------------------------------------------
// CMPSettingsAdvancedBwContainer::TitleResourceId
// ---------------------------------------------------------
//
TInt CMPSettingsAdvancedBwContainer::TitleResourceId()
    {
    return R_MPSETT_NETWORK_VIEW_TITLE;
    }

// ---------------------------------------------------------
// CMPSettingsAdvancedBwContainer::GetHelpContext
// ---------------------------------------------------------
//
void CMPSettingsAdvancedBwContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    MPX_FUNC("#MS# CMPSettingsAdvancedBwContainer::GetHelpContext()");
    aContext.iMajor = KIptvHelpContextUid;
    aContext.iContext = KIPTV_HLP_SETTINGS;
    }

// ---------------------------------------------------------
// CMPSettingsAdvancedBwContainer::FocusChanged
// ---------------------------------------------------------
//
void CMPSettingsAdvancedBwContainer::FocusChanged(TDrawNow /*aDrawNow*/)
	{
		if( iListBox)
        {
        iListBox->SetFocus( IsFocused() );
        }

		if( iComponentControl)
        {
        iComponentControl->SetFocus( IsFocused() );
        }
	}

// ---------------------------------------------------------------------------
// CMPSettingsAdvancedBwContainer::SizeChanged()
// Called by framwork when the view size is changed
// ---------------------------------------------------------------------------
//
void CMPSettingsAdvancedBwContainer::SizeChanged()
    {
		if (iListBox)
        {
        iListBox->SetRect(Rect());
        }

		if (iComponentControl)
        {
        iComponentControl->SetRect(Rect());
        }
    }
    
// ---------------------------------------------------------
// CMPSettingsAdvancedBwContainer::HandleResourceChange
// Handles a resource relative event 
// ---------------------------------------------------------
//
void CMPSettingsAdvancedBwContainer::HandleResourceChange( TInt aType )
    {
        MPX_DEBUG2(_L("#MS# CMPSettingsAdvancedBwContainer::HandleResourceChange(0x%X)"),aType);
		if ( aType == KEikDynamicLayoutVariantSwitch ) //Handle change in layout orientation
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
		DrawNow();
		}
		CCoeControl::HandleResourceChange( aType );  
    }
// End of File  
