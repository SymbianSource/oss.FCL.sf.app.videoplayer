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
* Description:      Observer interface for video data changes.*
*/




#ifndef VCXHGMYVIDEOSVIDEOMODELOBSERVER_H
#define VCXHGMYVIDEOSVIDEOMODELOBSERVER_H

// INCLUDE FILES
#include <mpxcollectionmessagedefs.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS
class CMPXMediaArray;
class CMPXMedia;

// CLASS DECLARATION

/**
 * Observer interface for video data changes.
 *
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( MVcxHgMyVideosVideoModelObserver )
    {
    public:

        /**
         * Called when a list of videos is available.
         * 
         * @param aVideoList List of videos
         */
        virtual void NewVideoListL( CMPXMediaArray& aVideoList ) = 0;
        
        /**
         * Called when the whole video list has been modified.
         * Normally this means that new items have been appended to list.
         */
        virtual void VideoListModifiedL() = 0;
        
        /*
         * Called when a single video item has fetched.
         * Ownership of video is transferred.
         * 
         * @param aVideo Video item.
         */
        virtual void VideoFetchingCompletedL( CMPXMedia* aVideo ) = 0;
                
        /**
         * Called when a single video item has been modified (add/remove/modify).
         * 
         * @param aEventType Type of modification event (add/remove/modify).
         * @param aMpxItemId MPX Item Id of the modified item.
         * @param aExtraInfo Extra information for modification event.
         * @param aSimulated If ETrue, event is not originated from MPX Collection.
         */
        virtual void VideoModifiedL( TMPXChangeEventType aEventType,
                                     TMPXItemId aMpxItemId,
                                     TInt32 aExtraInfo,
                                     TBool aSimulated = EFalse ) = 0;
        
        /**
         * Called when multiple video deletion command is completed.
         * 
         * @param aFailedCount Count of failed deletions, if any.
         * @param aFailedMpxId If above 1, MPXItemId of failed deletion.
         */
        virtual void VideoDeletionCompletedL( TInt aFailedCount,
                                              TMPXItemId aFailedMpxId ) = 0;

        /**
         * Called when move or copy command is completed.
         * 
         * @param aFailedCount Count of failed moves/copies, if any.
         * @param aFailedMpxId If above 1, MPXItemId of failed move/copy.
         */
        virtual void VideoMoveOrCopyCompletedL( TInt aFailedCount,
                                                TMPXItemId aFailedMpxId ) = 0;
        
        /**
         * Called when all details for a video have been fetched.
         * 
         * @param aMedia MPX Media that contains all details.
         */
        virtual void VideoDetailsCompletedL( const CMPXMedia& aMedia ) = 0;
    };

#endif // VCXHGMYVIDEOSVIDEOMODELOBSERVER_H
