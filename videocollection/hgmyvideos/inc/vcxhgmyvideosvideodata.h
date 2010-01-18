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
* Description:   CVcxHgMyVideosVideoData class definition*
*/




#ifndef VCXHGMYVIDEOSVIDEODATA_H
#define VCXHGMYVIDEOSVIDEODATA_H

// INCLUDE FILES
#include <coemain.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosVideoData ) :
        public CBase
    {
    public:

        enum TVideoDataState
            {
            EVideoDataStateNone = 0,
            EVideoDataStateThumbnailStarted,
            EVideoDataStateThumbnailFinished,
            EVideoDataStateDrmStarted,
            EVideoDataStateDrmFinished
            };

    public:

        /**
         * Two-phased constructor.
         * 
         * @return New object.
         */
        static CVcxHgMyVideosVideoData* NewL();

        /**
         * Two-phased constructor.
         * 
         * @return New object. 
         */
        static CVcxHgMyVideosVideoData* NewLC();

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosVideoData();
        
        /**
         * Returns MPX item id of video item.
         */
        TMPXItemId MPXItemId();

        /**
         * Returns data fetching state of video item.
         * 
         * @return State.
         */        
        TVideoDataState State();

        /**
         * Retuns thumbnail conversion Id of video item.
         * 
         * @return Thumbnail conversion Id.
         */        
        TInt ThumbnailConversionId();

        /**
         * Returns thumbnail bitmap of video item. Ownership transfer
         * depends on method parameter.
         * 
         * @param aGiveOwnership If ETrue, ownership is transferred.
         * @return Pointer to bitmap.
         */
        CFbsBitmap* Thumbnail( TBool aGiveOwnership = EFalse );

        /**
         * Return DRM protection status of video item.
         * 
         * @return DRM protection status.
         */
        TBool DrmProtected();

        /**
         * Return DRM key validity status for video item.
         * 
         * @return DRM key validity status for video item.
         */        
        TBool ValidDrmRights();        
        
        /**
         * Sets MPX item id.
         * 
         * @param aMPXItemId MPX item id.
         */
        void SetMPXItemId( TMPXItemId aMPXItemId ); 

        /**
         * Sets data fetching state of video item.
         * 
         * @param aState State.
         */
        void SetState( TVideoDataState aState );
        
        /**
         * Sets thumbnail conversion Id of video item.
         * 
         * @param aConversionId Thumbnail conversion Id.
         */        
        void SetThumbnailConversionId( TInt aConversionId );        

        /**
         * Sets thumbnail bitmap of video item. Ownership is transferred.
         * 
         * @param aThumbnail Pointer to bitmap.
         */
        void SetThumbnail( CFbsBitmap* aThumbnail );
        
        /**
         * Sets DRM protection status of video item.
         * 
         * @param aDrmProtected DRM protection status.
         */        
        void SetDrmProtected( TBool aDrmProtected );
        
        /**
         * Sets DRM key validity status for video item.
         * 
         * @param aDrmProtected DRM key validity status for video item.
         */        
        void SetValidDrmRights( TBool aValidDrmRights );        
        
    protected:

        /**
         * Default constructor
         *
         */
        CVcxHgMyVideosVideoData();

        /**
         * 2nd phase constructor.
         */
        void ConstructL();

    protected:
        
        /**
         * MPX item id.
         */
        TMPXItemId iMPXItemId;

        /**
         * Data fetching state of this item.
         */        
        TVideoDataState iState;

        /**
         * Thumbnail conversion ID for this item.
         */
        TInt iConversionId;

        /**
         * Thumbnail.
         * Own.
         */
        CFbsBitmap* iThumbnail;
        
        /**
         * DRM protection status.
         */
        TBool iDrmProtected;
        
        /**
         * DRM key validity status.
         */
        TBool iValidDrmRights;        
    };

#endif // VCXHGMYVIDEOSVIDEODATA_H
