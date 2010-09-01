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
* Description:    Video on Demand setting item class for download condition*
*/




// INCLUDES
#include <StringLoader.h>
#include <vcxnsscheduleview.rsg>

#include "vcxnsservicesettings.h"
#include "CVcxNsScheduleSettingsDlConditionItem.h"

// CONSTANTS
const TInt KEnumTextArraySize = 4;

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsDlConditionItem::CVcxNsScheduleSettingsDlConditionItem(
        TInt aResourceId, 
        TInt& aValue,
        TBool aWlanSupported )
    : CAknEnumeratedTextPopupSettingItem( aResourceId, aValue ), 
      iWlanSupported(aWlanSupported)
    {
    
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsDlConditionItem::~CVcxNsScheduleSettingsDlConditionItem()
    {

    }

// -----------------------------------------------------------------------------
// CVcxNsScheduleSettingsDlConditionItem::CompleteConstructionL
// -----------------------------------------------------------------------------
//    
void CVcxNsScheduleSettingsDlConditionItem::CompleteConstructionL()
    {
    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();    
    
    // Note: More elegant way of creating the setting item would be
    // reading enumerated text items from resource file and dynamically
    // remove the WLAN-item if necessary.
    // 
    // Current implementation creates just empty array from resource file
    // and text items are dynamically added to text array.
   
    CArrayPtrFlat<CAknEnumeratedText>* enumTextArray =
        new (ELeave) CArrayPtrFlat<CAknEnumeratedText>( KEnumTextArraySize );
    CleanupStack::PushL( enumTextArray );
        
    CArrayPtr<HBufC>* popupTextArray = 
        new (ELeave) CArrayPtrFlat<HBufC>( KEnumTextArraySize );
    CleanupStack::PushL( popupTextArray );
    
    HBufC* manualConditionString = 
        StringLoader::LoadLC( R_VCXNS_SCHEDULE_MANUAL_DOWNLOAD );
    HBufC* autohomeConditionString = 
        StringLoader::LoadLC( R_VCXNS_SCHEDULE_AUTO_HOME_CELL );
    HBufC* autoalwaysConditionString = 
        StringLoader::LoadLC( R_VCXNS_SCHEDULE_AUTO_ALWAYS );
    HBufC* wlanConditionString = NULL;
    if ( iWlanSupported )
        {    
        wlanConditionString = StringLoader::LoadLC( R_VCXNS_SCHEDULE_AUTO_WLAN );
        }
                  
    CAknEnumeratedText* enumeratedText1 = new (ELeave) CAknEnumeratedText(
            EManual,
            manualConditionString);
    CleanupStack::PushL( enumeratedText1 );
    enumTextArray->AppendL( enumeratedText1 );
    CleanupStack::Pop( enumeratedText1 );
    
    if ( iWlanSupported ) 
        {  
        CAknEnumeratedText* enumeratedText2 = new (ELeave) CAknEnumeratedText(
                EWLAN,
                wlanConditionString);
        CleanupStack::PushL( enumeratedText2 );
        enumTextArray->AppendL( enumeratedText2 );
        CleanupStack::Pop( enumeratedText2 );
        }

    CAknEnumeratedText* enumeratedText3 = new (ELeave) CAknEnumeratedText(
            EHomeCellular,
            autohomeConditionString);
    CleanupStack::PushL( enumeratedText3 );
    enumTextArray->AppendL( enumeratedText3 );
    CleanupStack::Pop( enumeratedText3 );

    CAknEnumeratedText* enumeratedText4 = new (ELeave) CAknEnumeratedText(
            EAlways,
            autoalwaysConditionString);
    CleanupStack::PushL( enumeratedText4 );
    enumTextArray->AppendL( enumeratedText4 );
    CleanupStack::Pop( enumeratedText4 );
        
    SetEnumeratedTextArrays( enumTextArray, popupTextArray );
    HandleTextArrayUpdateL(); 
  
    if ( iWlanSupported )
        {
        CleanupStack::Pop( wlanConditionString );
        }
    CleanupStack::Pop( autoalwaysConditionString );
    CleanupStack::Pop( autohomeConditionString );
    CleanupStack::Pop( manualConditionString );
    CleanupStack::Pop( popupTextArray );
    CleanupStack::Pop( enumTextArray );
    }

// End of file
