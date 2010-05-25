/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      UI level handler for category data.*
*/




#ifndef VCXHGMYVIDEOSCATEGORYMODELHANDLER_H
#define VCXHGMYVIDEOSCATEGORYMODELHANDLER_H

// INCLUDE FILES
#include "vcxhgmyvideoscategorymodelobserver.h"


#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>

// FORWARD DECLARATIONS
class CMPXMedia;
class CVcxHgMyVideosModel;
class CHgScroller;
class CMPXMedia;
class CMyVideosIndicator;
class CVcxHgMyVideosThumbnailManager;
class TAknsItemID;

// ENUMERATIONS
enum TVcxHgMyVideosCategoryItemType
    {
    TVcxHgMyVideosCategoryItemTypeUndefined = 0,
    TVcxHgMyVideosCategoryItemTypeUid,
    TVcxHgMyVideosCategoryItemTypeUrl
    };

// CONSTANTS
const TInt KMyVideosTitleStringMaxLength = 256;
const TInt KMyVideosTitleUrlMaxLength    = 256;

// CLASS DECLARATION

/**
 * UI level handler for category data.
 * 
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosCategoryModelHandler ) : 
        public CBase,
        public MVcxHgMyVideosCategoryModelObserver,
        public MThumbnailManagerObserver
    {
    public:
        
        /**
         * Default constructor
         *
         * @param aModel Application model
         * @param aScroller Hg list component.
         */
        static CVcxHgMyVideosCategoryModelHandler* NewL(
            CVcxHgMyVideosModel& aModel,
            CHgScroller& aScroller );  
        
        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosCategoryModelHandler();
    
        /**
         * Activates category model.
         */
        void DoModelActivateL();
        
        /**
         * Deactivates category model.
         */
        void DoModelDeactivate();        

        /**
         * Makes sure model has correct category list.
         */
        void GetCategoryListL();        
        
        /**
         * Gets list highlight.
         * 
         * @return Highlight.
         */
        TInt Highlight();   
        
        /**
         * Returns category index corresponding the Ganes index.
         * 
         * @param Ganes scroller list index
         * @return Index of category
         */
        TInt ResolveCategoryId( TInt aScrollerIndex );
        
        /**
         * Plays last watched videdo.
         * 
         * @param Ganes scroller list index
         * @return Index of category
         */
        void PlayLastWatchedVidedoL();
        
        /**
         * Returns video indicator.
         * 
         * @return Reference to video indicator.
         */
        CMyVideosIndicator& VideoIndicatorL();
        
        /**
         * Handles "ExtraItem" list item selection.
		 *
		 * @param aCategoryId  Selected category id
		 * @param aItemType    On return, item type
		 * @param aUid         On return, UID if selected item contains UID 
		 * @param aUrl         On return, URL string if selected item contains URL
         */
        void HandleExtraItemSelectionL( TInt aCategoryId,
                                        TVcxHgMyVideosCategoryItemType& aItemType,
                                        TUid& aUid,
                                        TDes& aUrl );

        /**
         * Create empty icon to be used as category list default icon
         * 
         * @return empty icon
         */
        CGulIcon* CreateEmptyHgListIconL();
        
    public: // From MVcxHgMyVideosCategoryModelObserver

        /**
         * Called when a list of categories is available.
         * Ownership of array is transferred.
         * 
         * @param aCategoryList List of categories
         * @param aIsPartial    ETrue if list fetching continues, and KVcxMessageMyVideosListComplete 
         *                      message will arrive. EFalse if this is last event of list fetching.
         */
        void NewCategoryListL( CMPXMediaArray* aCategoryList, TBool aIsPartial );
        
        /**
         * Called when the list of categories has been modified.
         */
        void CategoryListModifiedL();         

        /**
         * Called when a single category has been modified (add/remove/modify).
         * 
         * @param aEventType Type of modification event (add/remove/modify).
         * @param aMpxItemId MPX Item Id of the modified item.
         */
        void CategoryModifiedL( TMPXChangeEventType aEventType,
                                TMPXItemId& aMpxItemId );
        
        /**
         * Called when category list items have been fetched.
		 */
        void CategoryListFetchingCompletedL();

    public: // From MThumbnailManagerObserver)

        /**
         * Preview thumbnail generation or loading is complete.
         *
         * @param aThumbnail     An object representing the resulting thumbnail.
         * @param aId            Request ID for the operation
         */
        void ThumbnailPreviewReady( MThumbnailData& aThumbnail,
            TThumbnailRequestId aId );

        /**
         * Final thumbnail bitmap generation or loading is complete.
         *
         * @param aError         Error code.
         * @param aThumbnail     An object representing the resulting thumbnail.
         * @param aId            Request ID for the operation.
         */
        void ThumbnailReady( TInt aError, MThumbnailData& aThumbnail,
            TThumbnailRequestId aId );

    private:    
        
        /**
         * C++ constructor
         *
         * @param aModel Application model
         * @param aScroller Hg list component.
         */
        CVcxHgMyVideosCategoryModelHandler(
            CVcxHgMyVideosModel& aModel,
            CHgScroller& aScroller );  

        /**
         * Loads name of category from resources.
         * 
         * @param aCategoryId Index of category.
         * @return Name of category.
         */
        HBufC* GetCategoryNameLC( TInt aCategoryId );

        /**
         * Loads icon for category
         * 
         * @param aCategoryId Id of category
         * @return Category icon
         */
        CGulIcon* GetCategoryIconL( TInt aCategoryId );

        /**
         * Formats second row text for categories.
         * 
         * @param aMedia MPX media object for row.
         * @return Second row text for category.
         */
        HBufC* FormatCategorySecondRowLC( CMPXMedia& aMedia );        
        
        /**
         * Formats second row text for categories.
         * 
		 * @param   aCategoryId Category id.
		 * @return  Second row text for category.
         */
        HBufC* FormatCategorySecondRowLC( TInt aCategoryId );
        
        /**
         * Translates MPX Item Id to index on UI list. 
         * 
         * @param aMpxItemId MPX Item Id.
         * @return Index on UI, or KErrNotFound.
         */
        TInt UiIndexFromMpxItemId( TMPXItemId& aMpxItemId );

        /**
         * Updates the whole category list.
         */
        void UpdateCategoryListL();
        
        /**
         * Updates list item from specified index.
         *
         * @param aListIndex Item's index on UI list.
         */
        void UpdateCategoryListItemL( TInt aListIndex );
        
        /**
         * Removes list item from specefied index.
         *
         * @param aListIndex Item's index on UI list.
         */
        void RemoveCategoryListItem( TInt aListIndex );

        /**
         * Goes through given number of categories and
         * fills iCategoryIdArray from the accepted ones
         */
        void MakeCategoryIdArray();
        
        /**
         * Checks if the given category is accepted or not.
         *
         * @param aCategoryId category id
         * @return TBool if accepted
         */
        TBool AcceptCategory( TInt aCategoryId );
        
        /**
         * Finds correct array index for given category id
         * from iCategoryIdAraay
         *
         * @param aCategoryId category id to be found
         * @return Array index
         */
        TInt ResolveCategoryArrayIndexById( TInt aCategoryId );
        
        /**
         * @param aCategoryId Category id to be found
         * @return CMPXMedia
         */
        CMPXMedia* GetCategoryDataL( TInt aCategoryId );
		
        /**
         * Gets the last watched videos icon from tbn server.
		 * Result is returned asynchronously to ThumbnailReady callback.
         */
        void LoadLastWatchedIconL();
        
        /**
         * Clears the last watched videos icon and indicator.
         */
        void ClearLastWatchedIconL();

        /**
         * Set indicator for last watched item
         */
        void SetLastWatchedIndicatorL();

        /**
         * Final thumbnail bitmap generation or loading is complete.
         * @param aError         Error code.
         * @param aThumbnail     An object representing the resulting thumbnail.
         * @param aId            Request ID for the operation.
         */
        void ThumbnailReadyL( TInt aError, MThumbnailData& aThumbnail,
            TThumbnailRequestId aId );
        
        /**
         * Create hg list icon from file
         * 
         * @param aSkinId Skin-Id of the icon
         * @param aFileName mif file name
         * @param aBitmapId bitmap index in mif file (used if skin-Id is not found)
         * @param aMaskId mask index in mif file (used if skin-Id is not found)
         * @return Icon
         */
        CGulIcon* CreateHgListIconL(
                const TAknsItemID& aSkinId,
                const TFileName& aFileName,
                TInt aBitmapId,
                TInt aMaskId );
        
        /**
         * Check, if the last watched cenrep key is set
         * 
         * @return ETrue if the key has been set, otherwise EFalse
         */
        TBool LastWatchedSetL();
        
        /**
         * Check, if the preloaded video exists
         * If the key exists, but the file is missing, it is considered, 
         * as it does not exist.
         *  
         * @return ETrue if the preloaded does exist, otherwise EFalse
         */
        TBool PreloadedExistsL();

    private:
    
        /**
         * Application model.
         * Not own.
         */
        CVcxHgMyVideosModel& iModel;
        
        /**
         * Reference to Hg list component.
         * Not own.
         */
        CHgScroller& iScroller;
        
        /**
         * Array of categories.
         * Own.
         */
        CMPXMediaArray* iCategoryList;
        
        /**
         * TArray containing category ID's. Each index 
         * corresponds to the scroller list (eg. correct
         * category ID for the 1st item in scroller list can
         * be found from index 0).  
         */
        RArray<TInt> iCategoryIdArray;
        
        /**
         * CVcxHgMyVideosThumbnailManager instance, not own.
         */
        CVcxHgMyVideosThumbnailManager& iTnManager;
        
        /**
         * Request ID of ongoing thumbnail request, or KErrNotFound.
         */
        TThumbnailRequestId iTnRequestId;
        
        /**
         * Video indicator class instance. Own.
         */
        CMyVideosIndicator* iVideoIndicator;
    };

#endif // VCXHGMYVIDEOSCATEGORYMODELHANDLER_H
