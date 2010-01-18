/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Video on Demand schedule settings item class.*
*/




// INCLUDES
#include <StringLoader.h>

#include <vcxnsscheduleview.rsg>
#include "vcxnsscheduleview.hrh"

#include "CVcxNsScheduleSettingsSettingItem.h"
#include "CVcxNsScheduleSettingsList.h"

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsSettingItem::CVcxNsScheduleSettingsSettingItem
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsSettingItem::CVcxNsScheduleSettingsSettingItem( 
    CVcxNsScheduleSettingsViewContainer* aParent )
    : iParent( aParent )
    {
    
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsSettingItem::~CVcxNsScheduleSettingsSettingItem()
    {
    delete iListbox;
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsSettingItem::NewL
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsSettingItem* CVcxNsScheduleSettingsSettingItem::NewL( 
            CVcxNsScheduleSettingsViewContainer* aParent,
            CVcxNsServiceSettings& aScheduleSettings )
    {
    CVcxNsScheduleSettingsSettingItem* self = 
        new (ELeave) CVcxNsScheduleSettingsSettingItem( aParent );
    CleanupStack::PushL( self );
    self->ConstructL( aScheduleSettings );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsSettingItem::ConstructL
// -----------------------------------------------------------------------------
// 
void CVcxNsScheduleSettingsSettingItem::ConstructL( 
    CVcxNsServiceSettings& aScheduleSettings )
    {
    iListbox = new (ELeave) CVcxNsScheduleSettingsList( aScheduleSettings );
    iListbox->SetContainerWindowL( *iParent );
	iListbox->SetMopParent( iParent );
    iListbox->ConstructFromResourceL( 
        R_VCXNS_SCHEDULE_SCHEDULESETTINGSVIEW_SETTING_ITEM_LIST );
    iListbox->SetRect( iParent->Rect() );
    }
                                    
// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsSettingItem::ActivateL
// -----------------------------------------------------------------------------
//   
void CVcxNsScheduleSettingsSettingItem::ActivateL()
    {
    iListbox->MakeVisible( ETrue );
    iListbox->DrawNow(); 
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsSettingItem::Deactivate
// -----------------------------------------------------------------------------
//          
void CVcxNsScheduleSettingsSettingItem::Deactivate()
    {
    iListbox->MakeVisible( EFalse );   
    }
    
// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsSettingItem::ChangeSelectedItemL
// -----------------------------------------------------------------------------
//   
void CVcxNsScheduleSettingsSettingItem::ChangeSelectedItemL()
	{
	iListbox->ChangeSelectedItemL(); 
	}

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsSettingItem::Listbox
// -----------------------------------------------------------------------------
//   
CVcxNsScheduleSettingsList* CVcxNsScheduleSettingsSettingItem::Listbox()
    {
    return iListbox;
    }

//  End of File
