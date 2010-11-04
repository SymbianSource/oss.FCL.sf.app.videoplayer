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
* Description:   Container class for Streaming settings list view.*
*/


// Version : %version: 8 %



// INCLUDE FILES
#include    <aknlists.h>
#include    <barsread.h>
#include    <MediaSettings.rsg>
#include    <csxhelp/iptv.hlp.hrh>

#include    "MPSettingsApp.h"
#include    "MPSettingsStreamingContainer.h"
#include    "MPSettingsStreamingSettingItemList.h"
#include    "MPSettingsConstants.h"
#include	"mpxlog.h"

class CMPSettingsModelForROP;
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMPSettingsStreamingContainer::CMPSettingsStreamingContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsStreamingContainer::CMPSettingsStreamingContainer(CMPSettingsModelForROP* aModel) : iModel( aModel)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsStreamingContainer::CMPSettingsStreamingContainer()"));
    }

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::~CMPSettingsStreamingContainer
// Destructor
// ---------------------------------------------------------
//
CMPSettingsStreamingContainer::~CMPSettingsStreamingContainer()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsStreamingContainer::~CMPSettingsStreamingContainer()"));

    delete iSettingList;
    }

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::ConstructComponentControlsL
// ---------------------------------------------------------
//
void CMPSettingsStreamingContainer::ConstructComponentControlsL()
    {
    MPX_FUNC("#MS# CMPSettingsStreamingContainer::ConstructComponentControlsL()");
    
    iSettingList = new(ELeave) CMPSettingsStreamingSettingItemList( iModel );
    iSettingList->SetContainerWindowL( *this );
    iSettingList->ConstructFromResourceL( R_MPSETT_STREAMING_SETTING_ITEM_LIST );

    iComponentControl = iSettingList;
    iListBox = iSettingList->ListBox();
    }

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::TitleResourceId
// ---------------------------------------------------------
//
TInt CMPSettingsStreamingContainer::TitleResourceId()
    {
    return R_MPSETT_STREAMING_VIEW_TITLE;
    }

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::GetHelpContext
// ---------------------------------------------------------
//
void CMPSettingsStreamingContainer::GetHelpContext(TCoeHelpContext& aContext) const
    {
    MPX_FUNC("#MS# CMPSettingsStreamingContainer::GetHelpContext()");
    aContext.iMajor = KIptvHelpContextUid;
    aContext.iContext = KIPTV_HLP_SETTINGS;
    }

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::FocusChanged
// ---------------------------------------------------------
//
void CMPSettingsStreamingContainer::FocusChanged(TDrawNow /*aDrawNow*/)
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

// ---------------------------------------------------------
// CMPSettingsStreamingContainer::EditCurrentItemFromMenuL
// ---------------------------------------------------------
//
void CMPSettingsStreamingContainer::EditCurrentItemFromMenuL(TBool iMenuOption)
    {
    MPX_DEBUG2(_L("#MS# CMPSettingsProxyContainer::EditCurrentItemFromMenuL(%d)"),iMenuOption);
    TInt index = iListBox->CurrentItemIndex();
    iSettingList->EditItemL(index, iMenuOption);
    }
// End of File  
