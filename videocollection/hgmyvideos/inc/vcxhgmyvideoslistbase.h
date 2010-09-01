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
* Description:      Base class for Hg based video list implementations.*
*/




#ifndef VCXHGMYVIDEOSLISTBASE_H
#define VCXHGMYVIDEOSLISTBASE_H

// INCLUDE FILES
#include <e32def.h>
#include <e32base.h>
#include <eikmenup.h>
 
// FORWARD DECLARATIONS
class CVcxHgMyVideosModel;
class CVcxHgMyVideosMainView;
class CHgScroller;

// CLASS DECLARATION

/**
 * Base class for video list implementations.
 * 
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosListBase ) : public CBase
    {
    public:
    
        /**
        * Destructor.
        */
        virtual ~CVcxHgMyVideosListBase();

        /**
         * Activate.
         * 
         * @param aIndex Index of parent list item to activate.
         */
        void DoListActivateL( TInt aIndex = KErrNotFound );

        /**
         * Deactivate.
         */
        void DoListDeactivate();
        
        /**
         * Update control layout.
         *
         * @param aRect New rectangle.
         */
        void UpdateLayout( TRect& aRect );
        
        /**
         * Gets list highlight.
         * 
         * @return Highlight.
         */
        TInt Highlight();

        /**
         * Set empty list text for list control. 
         *
         * @param aResourceId Resource id
         */
        void SetEmptyListTextByResourceL( TInt aResourceId );        

        /**
         * Set title text for view.
         * 
         * @param aResourceId Resource id
         */
        void SetTitleTextByResourceL( TInt aResourceId );

        /**
         * Handles dynamic initialisation of menu items specific to video list.
         * 
         * @param aResourceId Resource ID of menu to be activated.
         * @param aMenuPane Pointer to menu pane.
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );        
        
    public:
        
        /**
         * Set empty list text for list control. Deriving class must implement. 
         */        
        virtual void SetEmptyListTextL() = 0;
    
        /**
         * Set title text for view. Deriving class must implement.
         * 
         * @param aIndex Index of parent list item (title based on parent).
         */      
        virtual void SetTitleTextL( TInt aIndex ) = 0;
                
    protected:    

        /**
         * Default constructor
         *
         * @param aModel Component model
         * @param aView Component view
         * @param aScroller Hg list control
         */
        CVcxHgMyVideosListBase( CVcxHgMyVideosModel& iModel,
                                CVcxHgMyVideosMainView& aView,
                                CHgScroller& aScroller );

        /**
         * 2nd phase constructor.
         */
        void ConstructL();

    protected:

        /**
         * Reference to component model.
         * Not Own.
         */
        CVcxHgMyVideosModel& iModel;

        /**
         * Reference to component view.
         * Not own.
         */
        CVcxHgMyVideosMainView& iView;
        
        /**
         * Hg list component.
         * Own.
         */
        CHgScroller* iScroller;            
    };

#endif // VCXHGMYVIDEOSLISTBASE_H
