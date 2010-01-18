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
* Description:   Setting item list class for Network setting list view.*
*/


// Version : %version: 4 %




#ifndef MPSETTINGSNETWORKSETTINGITEMLIST_H
#define MPSETTINGSNETWORKSETTINGITEMLIST_H

//  INCLUDES
#include    <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CMPSettingsModelForROP;

// CONSTANTS
const TInt KMaxApNameLength = 255;

// CLASS DECLARATION

/**
*  CMPSettingsNetworkSettingItemList
*  Setting item list class for Network setting list view.
*
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsNetworkSettingItemList : public CAknSettingItemList
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMPSettingsNetworkSettingItemList(CMPSettingsModelForROP* aModel);

	    /**
	    * 2nd-stage construction from resource id, replaces non-virtual base 
        * class function.
	    */
	    void ConstructFromResourceL(TInt aResourceId);

        /**
        * From CCoeControl, 2nd-stage construction from constructed resource reader
        */
        void ConstructFromResourceL(TResourceReader& aReader);
        
        /**
        * Destructor.
        */
        virtual ~CMPSettingsNetworkSettingItemList();

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
        
        /**
        * Launches the platform dialog for the connection selection.
        *  
        */
        void LaunchConnectionSelectionL();        

        /**
        * Updates currently selected connection's name to setting item.
        */
        void UpdateSelectedConnectionNameL();
    
	private: // From MEikListBoxObserver

        /**
         * Event handling section
         */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aListBoxEvent );
        
    private:
		CMPSettingsModelForROP* iModel;

        TInt iBandwUsage;
        TInt iConnTimeout;
        TInt iSrvTimeout;
        TUint32 iDefaultAP;
        TInt iMinUDPPort;
        TInt iMaxUDPPort;
        
        TBuf<KMaxApNameLength> iSelectedApName;
        CAknTextSettingItem* iSettingConnectionItem;
    };

#endif      // MPSETTINGSNETWORKSETTINGITEMLIST_H   
            
// End of File
