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
* Description:    Video on Demand download time setting selection page*
*/




// INCLUDES
#include <vcxnsscheduleview.rsg>
#include "CVcxNsScheduleSettingsDlTimePage.h"

// -----------------------------------------------------------------------------
// C++ default constructor
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsDlTimePage::CVcxNsScheduleSettingsDlTimePage(	
                            TInt aResourceID,
                            CSelectionItemList* aItemArray)
    : CAknCheckBoxSettingPage( aResourceID, aItemArray ),
      iItemArray( *aItemArray )
	{
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVcxNsScheduleSettingsDlTimePage::~CVcxNsScheduleSettingsDlTimePage()
	{
	}

// ---------------------------------------------------------
// CVcxNsScheduleSettingsDlTimePage::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CVcxNsScheduleSettingsDlTimePage::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    
    TInt oldInd = ListBoxControl()->CurrentItemIndex();
    
    // Offer event to base class
    TKeyResponse retVal = CAknCheckBoxSettingPage::OfferKeyEventL( aKeyEvent,
                                                                   aType );
    // Get new index
    TInt curInd = ListBoxControl()->CurrentItemIndex();
    if ( curInd != oldInd )
        {
        SetCbaL();
        }
    return retVal;
    }
    
// ---------------------------------------------------------
// CVcxNsScheduleSettingsDlTimePage::SetCbaL
// ---------------------------------------------------------
//
void CVcxNsScheduleSettingsDlTimePage::SetCbaL()
    {
    TInt resId;
        
    if ( ListBoxControl() && Cba() )
        {
        TInt curSel = ListBoxControl()->CurrentItemIndex();
        if ( curSel >= 0 && curSel < iItemArray.Count() )
            {
            if ( iItemArray.At( curSel )->SelectionStatus() )
                {
                resId = R_VCXNS_SCHEDULE_SCHEDULESETTINGSVIEW_SOFTKEYS_UNMARK_DONE;
                }
            else
                {
                resId = R_VCXNS_SCHEDULE_SCHEDULESETTINGSVIEW_SOFTKEYS_MARK_DONE;
                }

            Cba()->SetCommandSetL( resId );
            Cba()->DrawDeferred();
            }
        }
    }
