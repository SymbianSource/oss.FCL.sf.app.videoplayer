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

// FORWARD DECLARATIONS
class CMPXMedia;
class CVcxHgMyVideosModel;
class CHgScroller;

// CLASS DECLARATION

/**
 * UI level handler for category data.
 * 
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosCategoryModelHandler ) : 
        public CBase,
        public MVcxHgMyVideosCategoryModelObserver
    {
    public:
        
        /**
         * Default constructor
         *
         * @param aModel Application model
         * @param aScroller Hg list component.
         */
        CVcxHgMyVideosCategoryModelHandler(
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
        
    public: // From MVcxHgMyVideosCategoryModelObserver

        /**
         * Called when a list of categories is available.
         * Ownership of array is transferred.
         * 
         * @param aCategoryList List of categories
         */
        void NewCategoryListL( CMPXMediaArray* aCategoryList );
        
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

    private:    

        /**
         * Loads name of category from resources.
         * 
         * @param aIndex Index of category.
         * @return Name of category.
         */
        HBufC* GetCategoryNameLC( TInt aIndex );

        /**
         * Formats second row text for categories.
         * 
         * @param aMedia MPX media object for row.
         * @return Second row text for category.
         */
        HBufC* FormatCategorySecondRowLC( CMPXMedia& aMedia );        

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
         *
         * @param aCategoriesAvailable Number of categories
         */
        void MakeCategoryIdArray( TInt aCategoriesAvailable );
        
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
    };

#endif // VCXHGMYVIDEOSCATEGORYMODELHANDLER_H
