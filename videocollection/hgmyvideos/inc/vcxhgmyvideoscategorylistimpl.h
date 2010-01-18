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
* Description:      Implementation of UI functionality for category list.*
*/




#ifndef VCXHGMYVIDEOSCATEGORYLISTIMPL_H
#define VCXHGMYVIDEOSCATEGORYLISTIMPL_H

// INCLUDE FILES
#include <ganes/HgSelectionObserverIface.h> 
#include "vcxhgmyvideoslistbase.h"

// FORWARD DECLARATIONS
class CVcxHgMyVideosCategoryModelHandler;
class CVcxHgMyVideosModel;
class CVcxHgMyVideosMainView;
class CHgScroller;

// CLASS DECLARATION

/**
 * Implementation of UI functionality for category list.
 * 
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosCategoryListImpl ) : public CVcxHgMyVideosListBase,
                                                      public MHgSelectionObserver
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @param aModel Component model
         * @param aView Component view
         * @param aScroller Hg list control
         * @return Pointer to newly created instance
         */
        static CVcxHgMyVideosCategoryListImpl* NewL(
            CVcxHgMyVideosModel& aModel,
            CVcxHgMyVideosMainView& aView,
            CHgScroller& aScroller );

        /**
         * Two-phased constructor.
         *
         * @param aModel Component model 
         * @param aView Component view
         * @param aScroller Hg list control
         * @return Pointer to newly created instance
         */
        static CVcxHgMyVideosCategoryListImpl* NewLC(
            CVcxHgMyVideosModel& aModel,
            CVcxHgMyVideosMainView& aView,
            CHgScroller& aScroller );
    
        /**
        * Destructor.
        */
        virtual ~CVcxHgMyVideosCategoryListImpl();

        /**
         * Activate
         * 
         * @param aIndex Index of video list to activate.
         */
        void DoListActivateL( TInt aIndex = KErrNotFound );

        /**
         * Deactivate
         */
        void DoListDeactivate();

        /**
         * Gets list highlight.
         * 
         * @return Highlight.
         */
        TInt Highlight();     

    public: // From MHgSelectionObserver
        
        /**
         * Handle selection.
         * 
         * @param aIndex Selected item index.
         */
        void HandleSelectL( TInt aIndex );
        
        /**
         * Handle opening.
         * 
         * @param aIndex Opened item index.
         */
        void HandleOpenL( TInt aIndex );
        
    protected:    

        /**
         * Default constructor
         *
         * @param aModel Component model
         * @param aView Component view
         * @param aScroller Hg list control
         */
        CVcxHgMyVideosCategoryListImpl( CVcxHgMyVideosModel& iModel,
                                        CVcxHgMyVideosMainView& aView,
                                        CHgScroller& aScroller );
    
        /**
         * 2nd phase constructor.
         */
        void ConstructL();

        /**
         * Set empty list text for list control. 
         */
        void SetEmptyListTextL();

        /**
         * Set title text for view.
         * 
         * @param aIndex Index of active video list.
         */
        void SetTitleTextL( TInt aIndex );

    protected:

        /**
         * Category model handler.
         * Own.
         */
        CVcxHgMyVideosCategoryModelHandler* iCategoryModel;
    };

#endif // VCXHGMYVIDEOSCATEGORYLISTIMPL_H
