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
* Description:   Setting item list class for Proxy setting list view.*
*/


// Version : %version: 3 %




#ifndef MPSETTINGSPROXYSETTINGITEMLIST_H
#define MPSETTINGSPROXYSETTINGITEMLIST_H

//  INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CMPSettingsModelForROP;

// CLASS DECLARATION

/**
*  CMPSettingsProxySettingItemList
*  Setting item list class for Proxy setting list view.
*
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsProxySettingItemList : public CAknSettingItemList
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMPSettingsProxySettingItemList(CMPSettingsModelForROP* aModel);

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
        virtual ~CMPSettingsProxySettingItemList();

    public:     // Functions from base classes

        /**
        * From CAknSettingItemList
        */
        void EditItemL(TInt aIndex, TBool aCalledFromMenu);

    protected:  // Functions from base classes
        
        /**
        * From CAknSettingItemList
        */
        CAknSettingItem* CreateSettingItemL(TInt aSettingId);

    private: //New

        /**
        * Sets Proxy host and Proxy port item visibility according to Proxy mode.
        * @since 2.0
        */
        void SetItemVisibilityL();

        /**
        * Loads all setting values.
        * @since 2.0
        */
        void LoadAllL();

        /**
        * Loads a specific setting value.
        * @since 2.0
        * @param aSettingId ID of the setting to be loaded.
        */
        void LoadSettingL(TInt aSettingId);

        /**
        * Stores a specific setting value.
        * @since 2.0
        * @param aSettingId ID of the setting to be stored.
        */
        void StoreSettingL(TInt aSettingId);

    private:
        CMPSettingsModelForROP* iModel;

        TBool iProxyMode;
        HBufC* iProxyHost;
        TPtr iProxyHostPtr;
        TInt iProxyPort;
    };

#endif      // MPSETTINGSPROXYSETTINGITEMLIST_H   
            
// End of File
