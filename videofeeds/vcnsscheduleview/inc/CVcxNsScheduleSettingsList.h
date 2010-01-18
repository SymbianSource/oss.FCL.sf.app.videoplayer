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




#ifndef CVCXNSSCHEDULESETTINGSLIST_H
#define CVCXNSSCHEDULESETTINGSLIST_H

// INCLUDES
#include <aknsettingitemlist.h>

#include "CIptvUtil.h"
#include "iptvvodscheduleddownloaddefinitions.h"

// FORWARD DECLARATIONS
class CVcxNsScheduleSettingsViewContainer;
class CVcxNsServiceSettings;

// CLASS DECLARATION
/**
 *  VoD schedule settings list
 *
 */
class CVcxNsScheduleSettingsList : public CAknSettingItemList
    {
    public: // constructors and destructor
    
        /**
        * Constructor.
        */       
        CVcxNsScheduleSettingsList( 
            CVcxNsServiceSettings& aScheduleSettings );
   
        /**
        * Destructor.
        */ 
        virtual ~CVcxNsScheduleSettingsList();
    
    public: // From CAknSettingItemList
                      
        /**
        * Create one setting item at a time, identified by id.
        * CAknSettingItemList calls this method and takes ownership
        * of the returned value. 
        * 
        * @param id Index of the setting item to create.
        * @return Pointer to CAknSettingItem object.
        */
        CAknSettingItem* CreateSettingItemL( TInt id );
                
        /**
        * Opens an item for editing.
        *
        * @param aIndex          Index of the setting item to edit.
        * @param aCalledFromMenu ETrue if started from menu.
        */
        void EditItemL( TInt aIndex, TBool aCalledFromMenu );

        /** 
        * Handles the key events.
        *
        */
        virtual TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                             TEventCode aType );
    
    public: // New methods
    
        /**
        *  Handle the "Change" option on the Options menu.  This is an
        *  alternative to the Selection key that forces the settings page
        *  to come up rather than changing the value in place (if possible).
        */
        void ChangeSelectedItemL();
        
        /**
        * Load the initial contents of the setting items.  By default,
        * the setting items are populated with the default values from
        * the design.  You can override those values here.
        * 
        * @param aServiceId     Service ID
        */
        void LoadSettingValuesL( TIptvServiceId aServiceId );
        
        /**
        * Save the contents of the setting items.  Note, this is called
        * whenever an item is changed and stored to the model, so it
        * may be called multiple times or not at all.
        *
        * @param aServiceId     Service ID
        */
        void SaveSettingValuesL( TIptvServiceId aServiceId );

        /**
        * Check if scheduling is active i.e. download condition is
        * not set to manual and at least one download time is selected. 
        *
        * @return ETrue if scheduling is active, otherwise EFalse.
        */
        TBool IsSchedulingActive();

    private:
        
        /**
        * Called by framework when the size is changed.
        */
        void SizeChanged();
    
    private: // data
      
        /**
        * Schedule settings
        */
        CVcxNsServiceSettings& iDlScheduleSettings; 
                   
        /**
        * Current download condition setting
        */
        TIptvVodScheduleConnectionCondition iDownloadCondition;

        /**
        * Current download time setting
        */
        TIptvVodScheduleDownloadtimeCombination iDownloadTime;
     
        /**
        * Current download type setting
        */
        TIptvVodScheduleDownloadTypeCombination iDownloadType;
     
        /**
        * WLAN support
        */
        TBool iWlanSupported;
     
    };

#endif /* CVCXNSSCHEDULESETTINGSLIST_H */

// End of File
