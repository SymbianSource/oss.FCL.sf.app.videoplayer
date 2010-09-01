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
* Description:   Container class for Video settings list view.*
*/


// Version : %version: 8 %



// INCLUDE FILES
#include    <aknlists.h>
#include    <barsread.h>
#include    <MediaSettings.rsg>
#include    <csxhelp/iptv.hlp.hrh>

#include    "MPSettingsApp.h"
#include    "MPSettingsVideoContainer.h"
#include    "MPSettingsVideoSettingItemList.h"
#include    "MPSettingsConstants.h"
#include    "MPSettingsModelForROP.h"
#include	"mpxlog.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsVideoContainer::CMPSettingsVideoContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsVideoContainer::CMPSettingsVideoContainer(CMPSettingsModelForROP* aModel) 
	: iModel( aModel)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsVideoContainer::CMPSettingsVideoContainer()"));
    }

// ---------------------------------------------------------
// CMPSettingsVideoContainer::~CMPSettingsVideoContainer
// Destructor
// ---------------------------------------------------------
//
CMPSettingsVideoContainer::~CMPSettingsVideoContainer()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsVideoContainer::~CMPSettingsVideoContainer()"));
	if (iSettingList) 
		{
		delete iSettingList;
		}
	
	delete iVideoSettingsEngine;
    }

// ---------------------------------------------------------
// CMPSettingsVideoContainer::EditCurrentItemFromMenuL
// ---------------------------------------------------------
//
void CMPSettingsVideoContainer::EditCurrentItemFromMenuL(TBool iMenuOption)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsVideoContainer::EditCurrentItemFromMenuL(%d)"),iMenuOption);
    static_cast<CMPSettingsVideoSettingItemList*>(iSettingList)->EditCurrentItemL( iMenuOption );
    }

// ---------------------------------------------------------
// CMPSettingsVideoContainer::ConstructComponentControlsL
// ---------------------------------------------------------
//
void CMPSettingsVideoContainer::ConstructComponentControlsL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsVideoContainer::ConstructComponentControlsL()"));
    
    iVideoSettingsEngine = CVcxNsSettingsEngine::NewL( );
    
    iSettingList = new(ELeave) CMPSettingsVideoSettingItemList(iModel, *iVideoSettingsEngine );

    iSettingList->SetContainerWindowL(*this);

    iSettingList->ConstructFromResourceL(R_MPSETT_VIDEO_SETTING_ITEM_LIST);
    iSettingList->LoadSettingsL();

    iComponentControl = iSettingList;
    iListBox = iSettingList->ListBox();
    }

// ---------------------------------------------------------
// CMPSettingsVideoContainer::TitleResourceId
// ---------------------------------------------------------
//
TInt CMPSettingsVideoContainer::TitleResourceId()
    {
    return R_MPSETT_VIDEO_VIEW_TITLE;
    }

// ---------------------------------------------------------
// CMPSettingsVideoContainer::GetHelpContext
// ---------------------------------------------------------
//
void CMPSettingsVideoContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsVideoContainer::GetHelpContext()"));
    aContext.iMajor = KIptvHelpContextUid;
    aContext.iContext = KIPTV_HLP_SETTINGS;
    }

// ---------------------------------------------------------
// CMPSettingsVideoContainer::FocusChanged
// ---------------------------------------------------------
//
void CMPSettingsVideoContainer::FocusChanged(TDrawNow /*aDrawNow*/)
	{
	if ( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }

	if ( iComponentControl )
        {
        iComponentControl->SetFocus( IsFocused() );
        }
	}

// ---------------------------------------------------------------------------
// CMPSettingsVideoContainer::SizeChanged()
// Called by framwork when the view size is changed
// ---------------------------------------------------------------------------
//
void CMPSettingsVideoContainer::SizeChanged()
    {
	if ( iListBox )
        {
        iListBox->SetRect(Rect());
        }

	if ( iComponentControl )
        {
        iComponentControl->SetRect(Rect());
        }
    }
    
// ---------------------------------------------------------
// CMPSettingsVideoContainer::HandleResourceChange
// Handles a resource relative event 
// ---------------------------------------------------------
//
void CMPSettingsVideoContainer::HandleResourceChange( TInt aType )
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsVideoContainer::HandleResourceChange(0x%X)"),aType);
	if ( aType == KEikDynamicLayoutVariantSwitch ) //Handle change in layout orientation
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
		DrawNow();
		}
	CCoeControl::HandleResourceChange( aType );  
    }

// ---------------------------------------------------------
// CMPSettingsVideoContainer::IsCurrentItem
// ---------------------------------------------------------
//
TBool CMPSettingsVideoContainer::IsCurrentItem( TInt aIndex )
    {
    return static_cast<CMPSettingsVideoSettingItemList*>(iSettingList)
        ->IsCurrentItem( aIndex );
    }


// End of File  
