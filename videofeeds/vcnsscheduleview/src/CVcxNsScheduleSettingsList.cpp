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
* Description:    Video on Demand schedule settings list.*
*/




// INCLUDES
#include <vcxnsscheduleview.rsg>
#include "vcxnsscheduleview.hrh" 

#include <StringLoader.h>
#include <AknPopupFieldText.h>
#include <aknmessagequerydialog.h> 
#include <featmgr.h>

#include "CVcxNsScheduleSettingsViewContainer.h"
#include "CVcxNsScheduleSettingsDlConditionItem.h"
#include "CVcxNsScheduleSettingsDlTimeItem.h"
#include "CVcxNsScheduleSettingsList.h"
#include "vcxnsservicesettings.h"

// CONSTANTS
const TInt KIptvDownloadTimeSettingIndex = 0;
const TInt KIptvDownloadConditionSettingIndex = 1;

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsList::CVcxNsScheduleSettingsList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsList::CVcxNsScheduleSettingsList(
    CVcxNsServiceSettings& aScheduleSettings )
    : iDlScheduleSettings(aScheduleSettings)  
    {
    iWlanSupported = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsList::~CVcxNsScheduleSettingsList()
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsList::SizeChanged
// -----------------------------------------------------------------------------
//
void CVcxNsScheduleSettingsList::SizeChanged()
    {
    CAknSettingItemList::SizeChanged();
    if ( ListBox() ) 
        {
        ListBox()->SetRect( Rect() );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsList::CreateSettingItemL
// -----------------------------------------------------------------------------
//
CAknSettingItem* CVcxNsScheduleSettingsList::CreateSettingItemL( TInt aId )
    {
    switch ( aId )
        {
        case EIptvVodScheduleSettingsViewDownloadConditionItem:
            {			
            CVcxNsScheduleSettingsDlConditionItem* item = new (ELeave) 
                CVcxNsScheduleSettingsDlConditionItem( 
                    aId,
                    (TInt&)iDownloadCondition,
                    iWlanSupported );
            return item;
            }
  		       
        case EIptvVodScheduleSettingsViewDownloadTimeItem:
            {			
            CVcxNsScheduleSettingsDlTimeItem* item = new (ELeave)
                 CVcxNsScheduleSettingsDlTimeItem( 
                    aId, 
                    (TUint&)iDownloadTime );
            return item;
            }
        
        default:
            {
            return NULL;    
            }
        }
    }
	
// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsList::EditItemL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduleSettingsList::EditItemL ( TInt aIndex, 
                                               TBool aCalledFromMenu )
    {
    if ( aIndex >= 0 && aIndex < ( *SettingItemArray() ).Count() )
        {
        TIptvVodScheduleConnectionCondition oldDownloadCondition =
            iDownloadCondition;
        CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
        CAknSettingItem* item = ( *SettingItemArray() ) [ aIndex ];
        item->StoreL();
               	
        if ( aIndex == KIptvDownloadTimeSettingIndex
             && iDownloadCondition != EManual  
             && iDownloadCondition != oldDownloadCondition )
            {
            HBufC* messageText = StringLoader::LoadLC( 
                R_VCXNS_SCHEDULE_AUTOMATIC_DOWNLOAD_NOTE );
            if ( messageText )
                {
                CAknMessageQueryDialog* dlg = 
                    CAknMessageQueryDialog::NewL( *messageText );
                dlg->ExecuteLD( R_VCXNS_SCHEDULE_AUTOMATIC_DOWNLOAD_QUERY_DIALOG );
                CleanupStack::PopAndDestroy( messageText );
                }
            }
    
        // Show/hide download time setting item as required    
        if ( aIndex == KIptvDownloadTimeSettingIndex 
             && KIptvDownloadConditionSettingIndex < (*SettingItemArray()).Count() )
            {
            if ( iDownloadCondition == EManual )
                {
                item = ( *SettingItemArray() )[KIptvDownloadConditionSettingIndex];
                item->SetHidden( ETrue );
                }
            else
                {
                item = ( *SettingItemArray() )[KIptvDownloadConditionSettingIndex];
                item->SetHidden( EFalse );    
                }
            
            HandleChangeInItemArrayOrVisibilityL();
            }
        }
    }
  
// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsList::ChangeSelectedItemL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduleSettingsList::ChangeSelectedItemL()
    {
    if ( ListBox()->CurrentItemIndex() >= 0 )
        {
        EditItemL( ListBox()->CurrentItemIndex(), ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsList::LoadSettingValuesL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduleSettingsList::LoadSettingValuesL( TIptvServiceId aServiceId )
    {
    iDlScheduleSettings.GetScheduledDownloadSettingsL(
        aServiceId,
        iDownloadTime,
        iDownloadCondition,
        iDownloadType );
    
    // Note: This is needed in the case when download condition is set to WLAN,
    // but device doesn't support it. 
    if ( !iWlanSupported && iDownloadCondition == EWLAN )
        {
        iDownloadCondition = EManual;            
        }

    // Set default value for download time to "night".
    if ( iDownloadCondition == EManual )
        {
        iDownloadTime = ENight;
        }

    LoadSettingsL();
    
    // Hide download time setting if manual download is selected.
    if ( iDownloadCondition == EManual )
        {
        if ( KIptvDownloadConditionSettingIndex < (*SettingItemArray()).Count()) 
            {
            CAknSettingItem* item = 
                ( *SettingItemArray() ) [ KIptvDownloadConditionSettingIndex ];
            item->SetHidden( ETrue ); 
            HandleChangeInItemArrayOrVisibilityL();
            }
        }
    }
	    
// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsList::SaveSettingValuesL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduleSettingsList::SaveSettingValuesL( TIptvServiceId aServiceId )
    {
    StoreSettingsL();
    
    iDlScheduleSettings.SetScheduledDownloadSettingsL(
        aServiceId,
        iDownloadTime,
        iDownloadCondition,
        EDownloadAddedAfterLastScheduledDownload ); // No UI for this yet
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsList::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CVcxNsScheduleSettingsList::OfferKeyEventL( 
            const TKeyEvent& aKeyEvent, 
            TEventCode aType )
    {
    return CAknSettingItemList::OfferKeyEventL( aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsList::IsSchedulingActive
// -----------------------------------------------------------------------------
//
TBool CVcxNsScheduleSettingsList::IsSchedulingActive() 
    {
    if ( iDownloadCondition == EManual || iDownloadTime == ENoSchedule)
        {
        return EFalse;
        }
    return ETrue; 
    }

//  End of File
