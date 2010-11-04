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
* Description:   MPSettingsUDPPortSettingItem.cpp*
*/


// Version : %version: 3 %




// INCLUDE FILES
#include    <akntextsettingpage.h>

#include    "mediasettings.hrh"
#include    "MPSettingsUDPPortSettingItem.h"
#include	"mpxlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMPSettingsUDPPortSettingItem::CMPSettingsUDPPortSettingItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsUDPPortSettingItem::CMPSettingsUDPPortSettingItem(TInt aIdentifier, TInt& aValue, TInt& aRefValue) :
    CAknIntegerEdwinSettingItem(aIdentifier, aValue), iRefValue(aRefValue)
    {
	MPX_DEBUG2(_L("#MS# CMPSettingsUDPPortSettingItem::CMPSettingsUDPPortSettingItem(%d)"),aIdentifier);
    }
  
// Destructor
CMPSettingsUDPPortSettingItem::~CMPSettingsUDPPortSettingItem()
    {   
    }

// -----------------------------------------------------------------------------
// CMPSettingsUDPPortSettingItem::EditItemL
// -----------------------------------------------------------------------------
//
void CMPSettingsUDPPortSettingItem::EditItemL(TBool /*aCalledFromMenu*/)
    {
    MPX_FUNC("#MS# CMPSettingsUDPPortSettingItem::EditItemL()");
    CAknSettingPage* dlg = new( ELeave )CAknIntegerSettingPage( 
                                SettingPageResourceId(),
                                InternalValueRef(),
                                SettingPageFlags());

    SetSettingPage(dlg);
    SettingPage()->SetSettingPageObserver(this);
    SettingPage()->ConstructL();

    switch(Identifier())
        {
        case EMPSettMinUDPPortSettingId:
            static_cast<CAknIntegerSettingPage*>(SettingPage())->IntegerEditorControl()->SetMaximumIntegerValue(iRefValue);
            break;
        case EMPSettMaxUDPPortSettingId:
            static_cast<CAknIntegerSettingPage*>(SettingPage())->IntegerEditorControl()->SetMinimumIntegerValue(iRefValue);
            break;
        default:
            break;
        }

    SettingPage()->ExecuteLD(CAknSettingPage::EUpdateWhenChanged);
    SetSettingPage(NULL);
    }

//  End of File  
