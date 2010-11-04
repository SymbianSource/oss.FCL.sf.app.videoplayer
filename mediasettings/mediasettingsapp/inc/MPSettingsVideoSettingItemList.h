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
* Description:   Setting item list class for Video settings list view.*
*/


// Version : %version: 7 %




#ifndef MPSETTINGSVIDEOSETTINGITEMLIST_H
#define MPSETTINGSVIDEOSETTINGITEMLIST_H

//  INCLUDES
#include <aknsettingitemlist.h>

// CONSTANTS


// When video contrast is not drawn, we know to shift the contents
const TInt KVideoContrastNotSupported ( -99 );

// FORWARD DECLARATIONS
class CMPSettingsModelForROP;
class CVcxNsSettingsEngine;

// CLASS DECLARATION

/**
*  CMPSettingsVideoSettingItemList
*  Setting item list class for Video settings list view.
*
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsVideoSettingItemList : public CAknSettingItemList 
    {
    public:  // Constructors and destructor
        
     /**
     * Setting listbox's item idexes
     */
    typedef enum
        {
        EVideoSettingContrast = 0,
        EVcSettingsViewParentControlIndex,
        EVcSettingsViewMemoryItemIndex,
        EVcSettingsViewThumbnailsIndex,
        EVideoPlayerVersion,
        EVcSettingsViewIadUpdateItemIndex // non-editable item
        } TVideoSettingItemIndex;
        
        /**
        * C++ default constructor.
        */
        CMPSettingsVideoSettingItemList( CMPSettingsModelForROP* aModel, 
                                         CVcxNsSettingsEngine& aVideoSettingsEngine );
        
        /**
        * Destructor.
        */
        virtual ~CMPSettingsVideoSettingItemList();

    public:
 
        /**
        * Check if specified item is currently selected. 
		* 
		* @param aIndex			Item index
        */
        TBool IsCurrentItem( TInt aIndex );
        
        /**
         * Check wheter or not the Contrast settings item shold be drawn
         */
        TBool VideoContrastIsSupportedL();
        
    public:     // Functions from base classes

        /**
        * From CAknSettingItemList
        */
        void EditItemL(TInt aIndex, TBool aCalledFromMenu);
        
        /**
         * Opens current setting item for editing.
         *
         * @param aCalledFromMenu ETrue if started from menu.
         */
        void EditCurrentItemL( TBool aCalledFromMenu );
        
        /**
        * From CAknSettingItemList
        */
        void LoadSettingsL();
        
    protected:  // Functions from base classes
        
        /**
        * From CAknSettingItemList
        */
        CAknSettingItem* CreateSettingItemL(TInt aSettingId);

    private: //New

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
        * Loads all setting values.
        * @since 2.0
        */
        void LoadAllL();
        
        /**
         * Event handling section, from MEikListBoxObserver.
         */
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aListBoxEvent);
        
    private:
		CMPSettingsModelForROP* iModel;
		CVcxNsSettingsEngine& iVideoSettingsEngine;  // Video settings engine
		
	    TInt iSettingTemp; // Dummy setting placeholder for settings sublist items
	    
        TInt iVideoContrast;
        TBool iVideoRepeatMode;
        TBool iVideoDefaultViewMode;
        TBool iRockerKeysMode;
        TBool iRockerKeysSupported;
		TBool iLandscapeSupportedByDevice;
    };

#endif      // MPSETTINGSVIDEOSETTINGITEMLIST_H   
            
// End of File
