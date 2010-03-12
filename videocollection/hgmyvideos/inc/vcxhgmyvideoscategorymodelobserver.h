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
* Description:      Observer interface for category data changes.*
*/




#ifndef VCXHGMYVIDEOSCATEGORYMODELOBSERVER_H
#define VCXHGMYVIDEOSCATEGORYMODELOBSERVER_H

// INCLUDES

#include <mpxcollectionmessagedefs.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS

class CMPXMediaArray;
class CMPXMedia;

// CLASS DECLARATION

/**
 * Observer interface for category data changes.
 * 
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( MVcxHgMyVideosCategoryModelObserver )
    {
    public:
    
        /**
         * Called when a list of categories is available.
         * Ownership of array is transferred.
         * 
         * @param aCategoryList List of categories
         * @param aIsPartial    ETrue if list fetching continues, and KVcxMessageMyVideosListComplete 
         *                      message will arrive. EFalse if this is last event of list fetching.
         */
        virtual void NewCategoryListL( CMPXMediaArray* aCategoryList, TBool aIsPartial ) = 0;

        /**
         * Called when the list of categories has been modified.
         */
        virtual void CategoryListModifiedL() = 0;

        /**
         * Called when a single category has been modified (add/remove/modify).
         * 
         * @param aEventType Type of modification event (add/remove/modify).
         * @param aMpxItemId MPX Item Id of the modified item.
         */
        virtual void CategoryModifiedL( TMPXChangeEventType aEventType, 
                                        TMPXItemId& aMpxItemId ) = 0;
        
        /*
         * Called when a single video item has been fetched.
         * Ownership of video is transferred.
         * 
         * @param aVideo Video item.
         */
        virtual void VideoFetchingCompletedL( CMPXMedia* aVideo ) = 0;
        
        /**
         * Called when category list items have been fetched.
         */
        virtual void CategoryListFetchingCompletedL() = 0;

    };

#endif // VCXHGMYVIDEOSCATEGORYMODELOBSERVER_H
