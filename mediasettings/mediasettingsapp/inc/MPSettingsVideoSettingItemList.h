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


// Version : %version: 6 %




#ifndef MPSETTINGSVIDEOSETTINGITEMLIST_H
#define MPSETTINGSVIDEOSETTINGITEMLIST_H

//  INCLUDES
#include <aknsettingitemlist.h>
#include "videosettingsdrivemonitor.h"
#include "mvideosettingsdrivemonitorobserver.h"

// CONSTANTS
// VideoCenter SIS package UID
const TUid KSisUid = { 0x200159B2 };

// formatting string for version number
_LIT( KVersionNumberFormat, "%d.%d(%d)" );

// Max length of the above version number string, whose format is
// major.minor(build). Header e32cmn.h states that their ranges are:
//   1. the major version number, ranging from 0 to 127, inclusive
//   2. the minor version number, ranging from 0 to 99 inclusive
//   3. the build number, ranging from 0 to 32767 inclusive.
// Which makes the total max length 3+1+2+1+5+1 = 13
const TInt KMaxVersionNumberLength( 13 );


// When video contrast is not drawn, we know to shift the contents
const TInt KVideoContrastNotSupported ( -99 );

// FORWARD DECLARATIONS
class CMPSettingsModelForROP;
class CVcxNsSettingsEngine;
class TIptvDriveMonitorEvent;

// CLASS DECLARATION

/**
*  CMPSettingsVideoSettingItemList
*  Setting item list class for Video settings list view.
*
*  @lib MediaSettings.app
*  @since 2.0
*/
class CMPSettingsVideoSettingItemList : public CAknSettingItemList, 
                                        public MIptvDriveMonitorObserver
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
        * Edits thumbnail setting item.
        * 
        * @param itemIndex       Setting list index.
        * @param aCalledFromMenu ETrue if started from menu.
        */
        void EditThumbnailItemL( TInt itemIndex, TBool aCalledFromMenu );

        /**
        * Edits parent control settting item.
        * 
        * @param itemIndex       Setting list index.
        * @param aCalledFromMenu ETrue if started from menu.
        */
        void EditParentControlItemL( TInt itemIndex, TBool aCalledFromMenu );

        /**
        * Edits preferred memory settting item.
        * 
        * @param itemIndex       Setting list index.
        * @param aCalledFromMenu ETrue if started from menu.
        */
        void EditPreferredMemoryItemL( TInt itemIndex, TBool aCalledFromMenu );
  
        /**
        * Check if specified item is currently selected. 
		* 
		* @param aIndex			Item index
        */
        TBool IsCurrentItem( TInt aIndex );
        
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
        
        /**
         * Finishes construction of listbox. Should be called after the
         * listbox is otherwise fully constructed.
         */
        void FinishConstructionL();
        
    public: // From MIptvDriveMonitorObserver

        void HandleDriveMonitorEvent( TIptvDriveMonitorEvent& aEvent );
        
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
         * Updates version number item to settings list.
         */
        void UpdateVersionNumberTextL();
        
        /**
        * Updates text of "parent control" setting item.
        */
        void UpdateParentControlTextL();
        
        /**
         * Fetch current parent control age and return corresponding index.
         *
         * @return Parent control age.
         */
        TInt GetCurrentParentControl();
        
        /**
            * Convert and save parent control index as age.
            *
            * @param aParentControlIndex Index of parent control selection.
            */
        void SetCurrentParentControl( TInt aParentControlIndex );
        
        /**
         * Updates text of "memory" setting item.
         */
        void UpdateMemoryItemTextL();
        
        /**
         * Adds phone memory descriptor to arrays.
         *
         * @param aIndex index in enumeration list.
         * @param aEnumeratedTextArray
         * @param aPoppedUpTextArray
         */
        void AddPhoneMemoryDesToArraysL( TInt aIndex,
                CArrayPtrFlat<CAknEnumeratedText>& aEnumeratedTextArray,
                CArrayPtr<HBufC>& aPoppedUpTextArray );

        /**
         * Adds memory card descriptor to arrays.
         *
         * @param aIndex index in enumeration list.
         * @param aEnumeratedTextArray
         * @param aPoppedUpTextArray
         */
        void AddMemoryCardDesToArraysL( TInt aIndex,
                CArrayPtrFlat<CAknEnumeratedText>& aEnumeratedTextArray,
                CArrayPtr<HBufC>& aPoppedUpTextArray );
        /**
         * Adds locked memory card descriptor to arrays.
         *
         * @param aIndex index in enumeration list.
         * @param aEnumeratedTextArray
         * @param aPoppedUpTextArray
         */
        void AddLockedMemoryCardDesToArraysL( TInt aIndex,
                CArrayPtrFlat<CAknEnumeratedText>& aEnumeratedTextArray,
                CArrayPtr<HBufC>& aPoppedUpTextArray );

        /**
         * Adds hard disk descriptor to arrays.
         *
         * @param aIndex index in enumeration list.
         * @param aEnumeratedTextArray
         * @param aPoppedUpTextArray
         */
        void AddHardDiskDesToArraysL( TInt aIndex,
                CArrayPtrFlat<CAknEnumeratedText>& aEnumeratedTextArray,
                CArrayPtr<HBufC>& aPoppedUpTextArray );

        /**
         * Adds descriptor to arrays.
         *
         * @param aIndex index in enumeration list.
         * @param aText Descriptor to add. Ownership moves.
         * @param aEnumeratedTextArray
         * @param aPoppedUpTextArray
         */
        void AddDesToArraysL( TInt aIndex,
                HBufC* aText,
                CArrayPtrFlat<CAknEnumeratedText>& aEnumeratedTextArray,
                CArrayPtr<HBufC>& aPoppedUpTextArray );

        /**
         * Check wheter or not the Contrast settings item shold be drawn
         */
        TBool VideoContrastIsSupportedL();
        
        /**
         * Event handling section, from MEikListBoxObserver.
         */
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aListBoxEvent);
        
    private:
		CMPSettingsModelForROP* iModel;
		CVcxNsSettingsEngine& iVideoSettingsEngine;  // Video settings engine
		
		CVideoSettingsDriveMonitor* iDriveMonitor; // Drive monitor
		
	    TBuf< KMaxVersionNumberLength > iSettingVersionText; // version text
	    CAknTextSettingItem* iSettingIadUpdateItem;  // IAD update setting item
	    
	    CAknEnumeratedTextPopupSettingItem* iSettingThumbnailsItem; // Thumbnail setting item
	    TInt iSettingThumbnails;   // Selected thumbnail setting value
	    
	    CAknEnumeratedTextPopupSettingItem* iSettingParentControlItem; // Parent control setting item
	    TInt iParentControlIndex;  // Selected parent control setting value
	    /**
	     * Is the phone's lock code entered succesfully, 
	     * required for editing parent control settings.
	     */
	    TBool iLockCodeAccepted;
	    
	    CAknEnumeratedTextPopupSettingItem* iSettingMemoryItem; // Memory setting item
	    TInt iSettingMemory;   // Selected memory setting value
	    
	    CAknEnumeratedTextPopupSettingItem* iSettingServiceItem; // Service selection item
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
