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
* Description:   Setting item list class for Advanced bandwidth setting list view.*
*/


// Version : %version: 3 %




#ifndef MPSETTINGSADVANCEDBWSETTINGITEMLIST_H
#define MPSETTINGSADVANCEDBWSETTINGITEMLIST_H

//  INCLUDES
#include    <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CMPSettingsModelForROP;

// CLASS DECLARATION

/**
*  CMPSettingsAdvancedBwSettingItemList
*  Setting item list class for Advanced bandwidth setting list view.
*
*  @lib MediaSettings.app
*  @since 2.1
*/
class CMPSettingsAdvancedBwSettingItemList : public CAknSettingItemList
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMPSettingsAdvancedBwSettingItemList(CMPSettingsModelForROP* aModel);

	    /**
	    * 2nd-stage construction from resource id
	    */
	    void ConstructFromResourceL(TInt aResourceId);

        /**
        * From CCoeControl, 2nd-stage construction from constructed resource reader
        */
        void ConstructFromResourceL(TResourceReader& aReader);
        
        /**
        * Destructor.
        */
        virtual ~CMPSettingsAdvancedBwSettingItemList();

    public:     // Functions from base classes

        /**
        * From CAknSettingItemList
        * @param aIndex Current item's (Visible) index in the list
        * @param aCalledFromMenu True if function was called from Options menu.
        */
        void EditItemL(TInt aIndex, TBool aCalledFromMenu);

    protected:  // Functions from base classes
        
        /**
        * From CAknSettingItemList
        * Framework method to create a setting item based upon the user id aSettingId. The 
        * client code decides what type to contruct.  new (ELeave) must then be used and the resulting 
        * pointer returned. Ownership is thereafter base class's responsiblity.
        *
        * @param aSettingId ID to use to determine the type of the setting item
        * @return a constructed (not 2nd-stage constructed) setting item.
        */
        CAknSettingItem* CreateSettingItemL(TInt aSettingId);

    private: //New

        /**
        * Loads all setting values.
        * @since 2.1
        */
        void LoadAllL();       

        /**
        * Loads a specific setting value.
        * @since 2.1
        * @param aSettingId ID of the setting to be loaded.
        */
        void LoadSettingL(TInt aSettingId);

        /**
        * Stores a specific setting value.
        * @since 2.1
        * @param aSettingId ID of the setting to be stored.
        */
        void StoreSettingL(TInt aSettingId);

        /**
        * Searches aValue from aArray. If aValue is found, aListValue is its index
        * in aArray. Otherwise aListValue is aArray's count.
        */
        void CalculateListPosition(TInt aValue, TInt& aListValue, RArray<TInt>& aArray);

    private:
        CMPSettingsModelForROP* iModel;

        RArray<TInt> iGRPSSustainPresets;
        RArray<TInt> iEGRPSSustainPresets;
        RArray<TInt> iWCDMASustainPresets;
        RArray<TInt> iWLANSustainPresets;
        RArray<TInt> iHSDPASustainPresets;
        
        RArray<TInt> iGRPSMaxPresets;
        RArray<TInt> iEGRPSMaxPresets;
        RArray<TInt> iWCDMAMaxPresets;
        RArray<TInt> iWLANMaxPresets;
        RArray<TInt> iHSDPAMaxPresets;
        
        TInt iGPRSListValue;
        TInt iGPRSBw;
        TInt iEGPRSListValue;
        TInt iEGPRSBw;
        TInt iWCDMAListValue;
        TInt iWCDMABw;
        TInt iWLANListValue;
        TInt iWLANBw;
        TInt iHSDPAListValue;
        TInt iHSDPABw;
        
    };

#endif      // MPSETTINGSADVANCEDBWSETTINGITEMLIST_H   
            
// End of File
