/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Video on Demand setting item class for dowload time selection.*
*/




// INCLUDES
#include <StringLoader.h>
#include <vcxnsscheduleview.rsg>

#include "vcxnsservicesettings.h"
#include "CVcxNsScheduleSettingsDlTimeItem.h"
#include "CVcxNsScheduleSettingsDlTimePage.h"

// CONSTANTS

// Needed for enum value iteration
const TInt KScheduleDlTimesCount = 5;     
const TInt KScheduleDlTimes[] = { 
        ENight,
        EMorning,
        ENoon,
        EAfternoon,
        EEvening 
        };

const TInt KItemArrayGranularity = 5;

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlTimeItem::CVcxNsScheduleSettingsDlTimeItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsDlTimeItem::CVcxNsScheduleSettingsDlTimeItem( 
        TInt aIdentifier, 
        TUint& aDlTimes )
    : CAknSettingItem( aIdentifier ),
      iPopupArray( KItemArrayGranularity ),
      iExtDlTimes( aDlTimes )
    {
   
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsDlTimeItem::~CVcxNsScheduleSettingsDlTimeItem()
    {
    iPopupArray.ResetAndDestroy();
    delete iSettingText;
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlTimeItem::InitializePopupArrayL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduleSettingsDlTimeItem::InitializePopupArrayL()
    {
    iPopupArray.ResetAndDestroy();

    RPointerArray<HBufC> stringArray;
    CleanupClosePushL( stringArray );
    stringArray.AppendL( StringLoader::LoadLC( R_VCXNS_SCHEDULE_NIGHT ) );
    stringArray.AppendL( StringLoader::LoadLC( R_VCXNS_SCHEDULE_MORNING ) );
    stringArray.AppendL( StringLoader::LoadLC( R_VCXNS_SCHEDULE_NOON ) );
    stringArray.AppendL( StringLoader::LoadLC( R_VCXNS_SCHEDULE_AFTERNOON ) );
    stringArray.AppendL( StringLoader::LoadLC( R_VCXNS_SCHEDULE_EVENING ) );

    for ( TInt i=0; i<stringArray.Count(); i++ )
        {
        CSelectableItem* item = 
            new (ELeave) CSelectableItem( *stringArray[i], EFalse );
        CleanupStack::PushL( item );
        item->ConstructL();
        iPopupArray.AppendL( item ); // Ownership is transferred to popup array
        CleanupStack::Pop( item );                 
        }
    
    CleanupStack::PopAndDestroy( stringArray.Count() );  // Pop all strings
    CleanupStack::PopAndDestroy( &stringArray );
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlTimeItem::PopulatePopupArrayL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduleSettingsDlTimeItem::PopulatePopupArrayL()
    {
    TInt indexOfOnlyItem = 0;

    iSelectedItems = 0;
    for ( TInt i=0; i<iPopupArray.Count(); i++ )
        {
        if ( i<KScheduleDlTimesCount )
            {
            if ( iDownloadTimes & KScheduleDlTimes[i] )
                {
                iPopupArray[i]->SetSelectionStatus( ETrue );					
                iSelectedItems++;
                indexOfOnlyItem = i;
                }
            else
                {
                iPopupArray[i]->SetSelectionStatus( EFalse );   
                }
            }
        }
    if ( iSelectedItems == 1 )
        {
        delete iSettingText;
        iSettingText = NULL;
        iSettingText = 
            HBufC::NewL( iPopupArray[indexOfOnlyItem]->ItemText().Length() );
        *iSettingText = iPopupArray[indexOfOnlyItem]->ItemText();
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlTimeItem::CompleteConstructionL
// -----------------------------------------------------------------------------
//	
void CVcxNsScheduleSettingsDlTimeItem::CompleteConstructionL()
    {
    InitializePopupArrayL();
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlTimeItem::EditItemL
// -----------------------------------------------------------------------------
//	
void CVcxNsScheduleSettingsDlTimeItem::EditItemL(TBool /*aCalledFromMenu*/)
    {
    CVcxNsScheduleSettingsDlTimePage* dlg;
    TInt settingPageResId;
      
    // Note: Two separate setting page recources with different initial softkeys
    // are needed because correct CBA for setting page must be set before showing
    // the setting page.
    
    if ( iPopupArray.At(0)->SelectionStatus() )  // Check if first item is marked 
        {
        settingPageResId = R_VCXNS_SCHEDULE_SCHEDULESETTINGSVIEW_DOWNLOAD_TIME_PAGE_UNMARKED;
        }
    else
        {
        settingPageResId = R_VCXNS_SCHEDULE_SCHEDULESETTINGSVIEW_DOWNLOAD_TIME_PAGE_MARKED;     
        }   
    
    dlg = new (ELeave) CVcxNsScheduleSettingsDlTimePage( 
            settingPageResId,
            &iPopupArray );
              
    SetSettingPage( dlg );
    dlg->SetSettingPageObserver( this );
    dlg->ExecuteLD();
    SetSettingPage( NULL );
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlTimeItem::SettingTextL
// -----------------------------------------------------------------------------
//  
const TDesC& CVcxNsScheduleSettingsDlTimeItem::SettingTextL()
    {
    if ( iSelectedItems == 0 )
        {
        delete iSettingText;
        iSettingText = NULL;
        iSettingText = StringLoader::LoadL( R_VCXNS_SCHEDULE_NONE_SELECTED );
    }
    else if ( iSelectedItems > 1 )
        {
        delete iSettingText;
        iSettingText = NULL;
        iSettingText = StringLoader::LoadL( R_VCXNS_SCHEDULE_MULTIPLE_SELECTED, 
                                            iSelectedItems );
        }	
    return *iSettingText;
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlTimeItem::HandleSettingPageEventL
// -----------------------------------------------------------------------------
//  
void CVcxNsScheduleSettingsDlTimeItem::HandleSettingPageEventL(
    CAknSettingPage* aSettingPage,
    TAknSettingPageEvent aEventType )
    {
    if ( aEventType == EEventSettingOked )	
        {
        SaveSelectionsL();
        PopulatePopupArrayL();
	    UpdateListBoxTextL();
        }
    else if ( aEventType == EEventSettingChanged )
        {
        static_cast<CVcxNsScheduleSettingsDlTimePage*>(aSettingPage)->SetCbaL();
        }
    else if ( aEventType == EEventSettingCancelled )
        {
        PopulatePopupArrayL(); // Restore old settings
        }

    CAknSettingItem::HandleSettingPageEventL( aSettingPage, aEventType );
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlTimeItem::SaveSelectionsL
// -----------------------------------------------------------------------------
//  
void CVcxNsScheduleSettingsDlTimeItem::SaveSelectionsL()
    {
    iDownloadTimes = 0;
    for ( TInt i=0; i<KScheduleDlTimesCount; i++ )
        {
        if ( iPopupArray.Count() > i && iPopupArray[i]->SelectionStatus() )
            {
            iDownloadTimes = iDownloadTimes | KScheduleDlTimes[i];
            }	
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlTimeItem::LoadL
// -----------------------------------------------------------------------------
//  
void CVcxNsScheduleSettingsDlTimeItem::LoadL()
    {
    iDownloadTimes = iExtDlTimes;
    PopulatePopupArrayL();
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlTimeItem::StoreL
// -----------------------------------------------------------------------------
//     
void CVcxNsScheduleSettingsDlTimeItem::StoreL()
    {
    iExtDlTimes = iDownloadTimes;
    }
    
// End of File
