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
* Description:   Container class for Network setting list view.*
*/


// Version : %version: 7 %



// INCLUDE FILES
#include    <aknlists.h>
#include    <barsread.h>
#include    <MediaSettings.rsg>
#include    <csxhelp/iptv.hlp.hrh>

#include    "MPSettingsApp.h"
#include    "MPSettingsNetworkContainer.h"
#include    "MPSettingsNetworkSettingItemList.h"
#include    "MPSettingsConstants.h"
#include	"mpxlog.h"

class CMPSettingsModelForROP;
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsNetworkContainer::CMPSettingsNetworkContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsNetworkContainer::CMPSettingsNetworkContainer(CMPSettingsModelForROP* aModel) : iModel( aModel)
    {
	MPX_FUNC("#MS# CMPSettingsNetworkContainer::CMPSettingsNetworkContainer()");
    }

// ---------------------------------------------------------
// CMPSettingsNetworkContainer::~CMPSettingsNetworkContainer
// Destructor
// ---------------------------------------------------------
//
CMPSettingsNetworkContainer::~CMPSettingsNetworkContainer()
    {
    MPX_FUNC("#MS# CMPSettingsNetworkContainer::~CMPSettingsNetworkContainer()");
	if (iSettingList) 
		{
		delete iSettingList;
		}
    }

// ---------------------------------------------------------
// CMPSettingsNetworkContainer::EditCurrentItemFromMenuL
// ---------------------------------------------------------
//
void CMPSettingsNetworkContainer::EditCurrentItemFromMenuL()
    {
    MPX_FUNC("#MS# CMPSettingsNetworkContainer::EditCurrentItemFromMenuL()");
    TInt index = iListBox->CurrentItemIndex();
    iSettingList->EditItemL(index, ETrue);
    }

// ---------------------------------------------------------
// CMPSettingsNetworkContainer::ConstructComponentControlsL
// ---------------------------------------------------------
//
void CMPSettingsNetworkContainer::ConstructComponentControlsL()
    {
    MPX_FUNC("#MS# CMPSettingsNetworkContainer::ConstructComponentControlsL()");
    iSettingList = new(ELeave) CMPSettingsNetworkSettingItemList(iModel);

    iSettingList->SetContainerWindowL(*this);

    iSettingList->ConstructFromResourceL(R_MPSETT_NETWORK_SETTING_ITEM_LIST);

    iComponentControl = iSettingList;
    iListBox = iSettingList->ListBox();
    }

// ---------------------------------------------------------
// CMPSettingsNetworkContainer::TitleResourceId
// ---------------------------------------------------------
//
TInt CMPSettingsNetworkContainer::TitleResourceId()
    {
    return R_MPSETT_NETWORK_VIEW_TITLE;
    }

// ---------------------------------------------------------
// CMPSettingsNetworkContainer::GetHelpContext
// ---------------------------------------------------------
//
void CMPSettingsNetworkContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    MPX_FUNC("#MS# CMPSettingsNetworkContainer::GetHelpContext()");
    aContext.iMajor = KIptvHelpContextUid;
    aContext.iContext = KIPTV_HLP_SETTINGS;
    }

// ---------------------------------------------------------
// CMPSettingsNetworkContainer::FocusChanged
// ---------------------------------------------------------
//
void CMPSettingsNetworkContainer::FocusChanged(TDrawNow /*aDrawNow*/)
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
// CMPSettingsNetworkContainer::SizeChanged()
// Called by framwork when the view size is changed
// ---------------------------------------------------------------------------
//
void CMPSettingsNetworkContainer::SizeChanged()
    {
		if (iListBox)
        {
        iListBox->SetRect(Rect());
        }

		if( iComponentControl)
        {
        iComponentControl->SetRect(Rect());
        }
    }
    
// ---------------------------------------------------------
// CMPSettingsNetworkContainer::HandleResourceChange
// Handles a resource relative event 
// ---------------------------------------------------------
//
void CMPSettingsNetworkContainer::HandleResourceChange( TInt aType )
    {
		MPX_DEBUG2(_L("#MS# CMPSettingsNetworkContainer::HandleResourceChange(0x%X)"),aType);
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
