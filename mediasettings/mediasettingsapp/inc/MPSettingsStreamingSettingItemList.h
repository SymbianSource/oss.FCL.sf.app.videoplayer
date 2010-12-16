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
* Description:   Setting item list class for Streaming setting list view.*
*/


// Version : %version: 2.1.1 %




#ifndef MPSETTINGSSTREAMINGSETTINGITEMLIST_H
#define MPSETTINGSSTREAMINGSETTINGITEMLIST_H

//  INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CMPSettingsModelForROP;

// CONSTANTS
const TInt KMaxApNameLength = 255;

// CLASS DECLARATION

/**
*  CMPSettingsStreamingSettingItemList
*  Setting item list class for Streaming setting list view.
*
*/
class CMPSettingsStreamingSettingItemList : public CAknSettingItemList
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMPSettingsStreamingSettingItemList(CMPSettingsModelForROP* aModel);

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
        virtual ~CMPSettingsStreamingSettingItemList();

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
        */
        void SetItemVisibilityL();

        /**
        * Loads all setting values.
        */
        void LoadAllL();

        /**
        * Loads a specific setting value.
        * @param aSettingId ID of the setting to be loaded.
        */
        void LoadSettingL(TInt aSettingId);

        /**
        * Stores a specific setting value.
        * @param aSettingId ID of the setting to be stored.
        */
        void StoreSettingL(TInt aSettingId);

        /**
        * Launches the platform dialog for the connection selection.
        */
        void LaunchConnectionSelectionL();        

        /**
        * Updates currently selected connection's name to setting item.
        */
        void UpdateSelectedConnectionNameL();
        
        /**
        * Sets proxy mode to disabled. 
        */
        void CheckProxyDisableL();
        
        /**
         * Gets WAP id from IAP id. Leaves with KErrNotFound if no record with given 
         * IAP id is found.
         * 
		 * @param  aIapId	IAP id.
         * @return Access point id matching the given IAP id.
         */
        TUint32 WapIdFromIapIdL( TUint32 aIapId );

        /**
         * Gets IAP id from access point id. Leaves with KErrNotFound if no record with given 
         * access point id is found.
         * 
		 * @param  aWapId	Access point id.
         * @return IAP id matching the given access point id.
         */
        TUint32 IapIdFromWapIdL( TUint32 aWapId );        
        
    private: // From MEikListBoxObserver

        /**
         * Event handling section
         */
        void HandleListBoxEventL( CEikListBox* aListBox, 
                                  TListBoxEvent aListBoxEvent );        
        
    private:
        
        /**
         * ROP model. Not own. 
         */
        CMPSettingsModelForROP* iModel;

        /**
         * Proxy mode (on/off).
         */
        TBool iProxyMode;

        /**
         * Proxy host. Own.
         */
        HBufC* iProxyHost;

        /**
         * Pointer to Proxy host.
         */
        TPtr iProxyHostPtr;

        /**
         * Proxy port number.
         */
        TInt iProxyPort;

        /**
         * Connection timeout setting value.
         */
        TInt iConnTimeout;
        
        /**
         * Server timeout setting value.
         */
        TInt iSrvTimeout;
        
        /**
         * Access point id. 
         */
        TUint32 iDefaultAP;

        /**
         * Minimum UDP port number.
         */
        TInt iMinUDPPort;

        /**
         * Maximum UDP port number.
         */
        TInt iMaxUDPPort;

        /**
         * Name of selected access point. Own.
         */
        TBuf<KMaxApNameLength> iSelectedApName;
      
        /**
         *  Connection setting item. Not own.
         */
        CAknTextSettingItem* iSettingConnectionItem;
        
        /**
         *  Proxy mode item. Not own.
         */
        CAknBinaryPopupSettingItem* iSettingProxyItem;
        
    };

#endif      // MPSETTINGSSTREAMINGSETTINGITEMLIST_H   
            
// End of File
