/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CVcxHgMyVideosThumbnailManager class definition*
*/


#ifndef VCXHGMYVIDEOSTHUMBNAILMANAGER_H
#define VCXHGMYVIDEOSTHUMBNAILMANAGER_H

// INCLUDE FILES
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosThumbnailManager ) :
        public CBase,
        public MThumbnailManagerObserver 
    {
    public:

        /**
         * Two-phased constructor.
         * 
         * @return New object.
         */
        static CVcxHgMyVideosThumbnailManager* NewL();

        /**
         * Two-phased constructor.
         * 
         * @return New object. 
         */
        static CVcxHgMyVideosThumbnailManager* NewLC();

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosThumbnailManager();

    private: // From MThumbnailManagerObserver
        
        /**
         * Preview thumbnail generation or loading is complete.
         *
         * @param aThumbnail An object representing the resulting thumbnail.
         * @param aId Request ID for the operation
         */
        void ThumbnailPreviewReady( MThumbnailData& aThumbnail, TThumbnailRequestId aId );
    
        /**
         * Final thumbnail bitmap generation or loading is complete.
         *
         * @param aError         Error code.
         * @param aThumbnail     An object representing the resulting thumbnail.
         * @param aId            Request ID for the operation.
         */
        void ThumbnailReady( TInt aError, MThumbnailData& aThumbnail, TThumbnailRequestId aId );

    public: // New methods
        /**
         * Peeks if there is thumbnail and fetches it if it exist.
         * No thumbnail creation is started when thumbnail does not exist.
         *
         * @param aSource 
         * @return TThumbnailRequestId.
         */
        TThumbnailRequestId PeekL( CThumbnailObjectSource& aSource );

        /**
         * Gets thumbnail and creates it if it does not exist.
         *
         * @param aSource 
         * @return TThumbnailRequestId.
         */
        TThumbnailRequestId GetL( CThumbnailObjectSource& aSource );

        /**
         * Cancels thumbnail request
         *
         * @param aRequestId
         */
        void Cancel( TThumbnailRequestId aRequestId );
        
        /**
         * Reset the generated thumbnails from given source
         * 
         * aSource Thumbnail source
         */
        void Reset( CThumbnailObjectSource& aSource );

        /**
         * Adds observer.
         *
         * @param aObserver 
         */
        void AddObserverL( MThumbnailManagerObserver& aObserver );

        /**
         * Removes observer.
         *
         * @param aObserver 
         */
        void RemoveObserver( MThumbnailManagerObserver& aObserver );

    private:    

        /**
         * Default constructor
         */
        CVcxHgMyVideosThumbnailManager();

        /**
         * SessionL
         * @return CThumbnailManager ref.
         */
        CThumbnailManager& SessionL();

    private:

        /**
         * S60 Thumbnail Manager.
         * Own.
         */
        CThumbnailManager* iTnManager;

        /** 
         * Used for storing the observers. Pointers are not own.
         */ 
        RPointerArray<MThumbnailManagerObserver> iTnObservers;

        /** 
         * Stores current thumbnail generation flags
         */ 
        CThumbnailManager::TThumbnailFlags iCurrentFlags;
    };

#endif // VCXHGMYVIDEOSTHUMBNAILMANAGER_H
