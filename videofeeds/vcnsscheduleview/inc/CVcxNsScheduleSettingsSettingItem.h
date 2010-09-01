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




#ifndef CVCXNSSCHEDULESETTINGSSETTINGITEM_H
#define CVCXNSSCHEDULESETTINGSSETTINGITEM_H

// INCLUDES
#include "CVcxNsScheduleSettingsViewContainer.h"

// FORWARD DECLARATIONS
class CVcxNsScheduleSettingsList;

/**
 * 
 *  VoD schedule settings view setting item class.
 *
 */
class CVcxNsScheduleSettingsSettingItem : public CBase
    {
    public: // constructors and destructor
        
        /**
        * Two-phased constructor
        *
        * @param Parent container
        * @param Scheduled download settings
        */
        static CVcxNsScheduleSettingsSettingItem* NewL( 
            CVcxNsScheduleSettingsViewContainer* aParent,
            CVcxNsServiceSettings& aScheduleSettings ); 
        
        /**
        * Destructor.
        */
        virtual ~CVcxNsScheduleSettingsSettingItem();
        
    private: // Construction.

        /**
        * Default C++ constructor.
        *
        * @param aParent            Parent container
        */    
        CVcxNsScheduleSettingsSettingItem( 
            CVcxNsScheduleSettingsViewContainer* aParent );

        /**
        * EPOC constructor.
		*
		* @param aScheduleSettings  Scheduled download settings
        */    
        void ConstructL( CVcxNsServiceSettings& aScheduleSettings );

    public: // New methods

        /**
         * Activate setting item.
         */    
        void ActivateL();

        /**
         * Deactivate settings item.
         */        
        void Deactivate();

		/**
		 * Edit selected item.
		 */
		void ChangeSelectedItemL();

		/**
		* Returns pointer to the settings list box.
		* Ownership is not transferred.
		*
		* @return List box pointer.
		*/
		CVcxNsScheduleSettingsList* Listbox();

    private: // Data
        
        /**
        * Pointer to parent container
        * Not own.  
        */    
        CVcxNsScheduleSettingsViewContainer* iParent;
        
        /**
        * Settings list box
        * Own.
        */    
        CVcxNsScheduleSettingsList* iListbox;
                       
    };
    
#endif /* CVCXNSSCHEDULESETTINGSSETTINGITEM_H */		

// End of File
