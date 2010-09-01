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




#ifndef CVCXNSSCHEDULESETTINGSDLTIMEITEM_H
#define CVCXNSSCHEDULESETTINGSDLTIMEITEM_H

// INCLUDES
#include <aknsettingitemlist.h>
#include <akncheckboxsettingpage.h>

// CLASS DECLARATION

/**
*
*  Video on Demand setting item for download time.
*
*/
class CVcxNsScheduleSettingsDlTimeItem : public CAknSettingItem
    {
    public: // constructors and destructor 
    
        /**
        * C++ constructor.
        *
        * @param aIdentifier    Resource identifier.
        * @param aDlTimes       Download times.     
        */
        CVcxNsScheduleSettingsDlTimeItem( TInt aIdentifier, 
                                            TUint& aDlTimes );
        
        /**
        * Destructor
        */
        virtual ~CVcxNsScheduleSettingsDlTimeItem();
        
    public: // From CAknSettingItem
    
        /**
        * Load setting from given external variable.
        */   
        void LoadL(); 
        
        /**
        * Store current setting to given external variable.
        */   
        void StoreL();

    public: // New methods        
        
        /**
        * Generates the popup array for download time selections.
        *
        */
        void InitializePopupArrayL();
        
        /**
        * Populates the popup array with correct values.
        *
        */    
        void PopulatePopupArrayL(); 
        
        /**
        * Saves the current selected download times from popup array.
        *
        */          
        void SaveSelectionsL();
    
    private: // From CAknSettingItem
        
        /**
        * Completes construction of setting item.
        *
        */
        void CompleteConstructionL();
        
        /**
        * Opens item for editing.
        *
        * @param aCalledFromMenu Is iten called from menu.
        */
        void EditItemL( TBool aCalledFromMenu );
        
    	/**
        * Called to get text for this setting item.
        *
        * @return Text for this setting item.
        */
        const TDesC& SettingTextL();
        
    private: // From MAknSettingPageObserver     
        
        /**
        * Called on setting page events 
        * (when editing).
        *
        * @param aSettingPage   Setting page to be observed.
        * @param aEventType     Event type.
        */
        void HandleSettingPageEventL( CAknSettingPage* aSettingPage, 
                                      TAknSettingPageEvent aEventType );
    
    private:
        
        /**
        * Popup array for for download time selection.
        */	
        CSelectionItemList iPopupArray;
        
        /**
        * Text for this setting item. 
        * Own.
        */       
        HBufC* iSettingText;
        
        /**
        * Number of selected items.
        */  
        TInt iSelectedItems;

        /**
        * Combined internal download times.
        */  
        TUint iDownloadTimes;

        /**
        * Combined external download times.
        */  
        TUint& iExtDlTimes;
    };

#endif /* CVCXNSSCHEDULESETTINGSDLTIMEITEM_H */

// End of File
