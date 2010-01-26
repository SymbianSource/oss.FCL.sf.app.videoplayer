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
* Description:   Container class for Proxy settings list view.*
*/


// Version : %version: 7 %



// INCLUDE FILES
#include    <aknlists.h>
#include    <barsread.h>
#include    <MediaSettings.rsg>
#include    <csxhelp/iptv.hlp.hrh>

#include    "MPSettingsApp.h"
#include    "MPSettingsProxyContainer.h"
#include    "MPSettingsProxySettingItemList.h"
#include    "MPSettingsConstants.h"
#include	"mpxlog.h"

class CMPSettingsModelForROP;
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsProxyContainer::CMPSettingsProxyContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsProxyContainer::CMPSettingsProxyContainer(CMPSettingsModelForROP* aModel) 
	: iModel( aModel)
    {
	MPX_FUNC("#MS# CMPSettingsProxyContainer::CMPSettingsProxyContainer()");
    }

// ---------------------------------------------------------
// CMPSettingsProxyContainer::~CMPSettingsProxyContainer
// Destructor
// ---------------------------------------------------------
//
CMPSettingsProxyContainer::~CMPSettingsProxyContainer()
    {
    MPX_FUNC("#MS# CMPSettingsProxyContainer::~CMPSettingsProxyContainer()");
	if (iSettingList) 
		{
		delete iSettingList;
		}
    }

// ---------------------------------------------------------
// CMPSettingsProxyContainer::EditCurrentItemFromMenuL
// ---------------------------------------------------------
//
void CMPSettingsProxyContainer::EditCurrentItemFromMenuL(TBool iMenuOption)
    {
	MPX_DEBUG2(_L("#MS# CMPSettingsProxyContainer::EditCurrentItemFromMenuL(%d)"),iMenuOption);
    TInt index = iListBox->CurrentItemIndex();
    iSettingList->EditItemL(index, iMenuOption);
    }

// ---------------------------------------------------------
// CMPSettingsProxyContainer::ConstructComponentControlsL
// ---------------------------------------------------------
//
void CMPSettingsProxyContainer::ConstructComponentControlsL()
    {
    MPX_FUNC("#MS# CMPSettingsProxyContainer::ConstructComponentControlsL()");
    iSettingList = new(ELeave) CMPSettingsProxySettingItemList(iModel);
    iSettingList->SetContainerWindowL(*this);
    iSettingList->ConstructFromResourceL(R_MPSETT_PROXY_SETTING_ITEM_LIST);

    iComponentControl = iSettingList;
    iListBox = iSettingList->ListBox();
    }

// ---------------------------------------------------------
// CMPSettingsProxyContainer::TitleResourceId
// ---------------------------------------------------------
//
TInt CMPSettingsProxyContainer::TitleResourceId()
    {
    return R_MPSETT_PROXY_VIEW_TITLE;
    }

// ---------------------------------------------------------
// CMPSettingsProxyContainer::GetHelpContext
// ---------------------------------------------------------
//
void CMPSettingsProxyContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    MPX_FUNC("#MS# CMPSettingsProxyContainer::GetHelpContext()");
    aContext.iMajor = KIptvHelpContextUid;
    aContext.iContext = KIPTV_HLP_SETTINGS;
    }

// ---------------------------------------------------------
// CMPSettingsProxyContainer::FocusChanged
// ---------------------------------------------------------
//
void CMPSettingsProxyContainer::FocusChanged(TDrawNow /*aDrawNow*/)
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
// CMPSettingsProxyContainer::SizeChanged()
// Called by framwork when the view size is changed
// ---------------------------------------------------------------------------
//
void CMPSettingsProxyContainer::SizeChanged()
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
// CMPSettingsProxyContainer::HandleResourceChange
// Handles a resource relative event 
// ---------------------------------------------------------
//
void CMPSettingsProxyContainer::HandleResourceChange( TInt aType )
    {
	MPX_DEBUG2(_L("#MS# CMPSettingsProxyContainer::HandleResourceChange(0x%X)"),aType);
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
