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
* Description:      Class for following and updating download progress.*
*/




#ifndef VCXHGMYVIDEOSDOWNLOADUPDATER_H
#define VCXHGMYVIDEOSDOWNLOADUPDATER_H

// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include "CIptvUtil.h"
#include <mpxcollectionmessagedefs.h>

class CVcxHgMyVideosVideoModelHandler;
class CPeriodic;
class CMPXMedia;
class CMPXMediaArray;
class CVcxHgMyVideosVideoList;

/**
 * Follows and updates download progress.
 * 
 * @lib vcxhgmyvideos.lib 
 */
NONSHARABLE_CLASS( CVcxHgMyVideosDownloadUpdater ) : public CBase
    {
    protected:

        /**
         * TDownloadItem
         * Private inner class for storing polled downloads and their
         * previous statuses and progresses.
         */
        class TDownloadItem
            {
            public:

                /**
                 * Mpx media object. 
                 * Not own.
                 */
                CMPXMedia* iMedia;

                /**
                 * Old state, used for determine refresh need.
                 */
                TVcxMyVideosDownloadState iState;

                /**
                 * Old progress, used for determine refresh need.
                 */
                TUint32 iProgress;

                /**
                 * Flag for marking that progressive play possibility has been queried.
                 * Status is followed only for items where query has been made before. 
                 */
                TBool iProgressivePlayQueried;
                
                /**
                 * Previous response to CViaPlayerFileDetails::IsPlayPossible(). 
                 */
                TBool iIsPlayPossible;
            };

    public:
    
        /**
         * Two-phased constructor.
         * 
         * @param aParent Parent class.
         * @param aVideoArray Reference to video array.
         */
        static CVcxHgMyVideosDownloadUpdater* NewL(
            CVcxHgMyVideosVideoModelHandler& aParent,
            CVcxHgMyVideosVideoList& aVideoArray );

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosDownloadUpdater();
    
        /**
         * Sets video download updater to pause or normal mode.
         * 
         * @param aPaused If ETrue, updater is paused.
         */
        void SetPausedL( TBool aPaused );

        /**
         * Removes the download from download array.
         *
         * @param aMpxItemId MPX item ID of media object.
         */
        void RemoveDownload( TMPXItemId aMpxItemId );
        
        /**
         * Finds the download from download array.
         * 
         * @param aMedia MPX media object.
         * @return Index of array or KErrNotFound.
         */
        TInt FindDownload( CMPXMedia* aMedia );
        
        /**
         * Called to inform that array of videos has changed. Causes download
         * data being re-setted to make sure correct downloads are followed.  
         */
        void VideoArrayChangedL();         
        
        /**
         * Checks video data modifications and updates download array.
         * 
         * @param aEventType Event type, only expects EMPXItemModified.
         * @param aMpxItemId MPX item ID of modified video.
         * @param aExtraInfo Extra information for modification event.
         */
        void VideoModifiedL( TMPXChangeEventType aEventType,
                             TMPXItemId aMpxItemId,
                             TInt32 aExtraInfo );
        
        /**
         * Checks if progressive play (playback during download) is possible
         * for given video clip.
         * 
         * @param aIndex Video index on video array.
         * @return ETrue if progressive play is possible.
         */
        TBool IsPlayPossible( TInt aIndex );

        /**
        * Periodic's callback.
        */
        static TInt Callback( TAny *aPtr );

    protected:

        CVcxHgMyVideosDownloadUpdater( 
            CVcxHgMyVideosVideoModelHandler& aParent,
            CVcxHgMyVideosVideoList& aVideoArray );                

        void ConstructL();

        /**
         * Makes sure that timer that is following download is running, and runs
         * only when it is needed.
         */
        void ContinueToFollowDownloads();        
        
        /**
         * Resets the list of downloads to follow based on new video array.
         */
        void ResetDownloadsToFollowL();

        /**
         * Update the downloads.
         */
        void UpdateDownloadsL();

        /**
         * Update given download if it's status has changed.
         * 
         * @param aItem Download item.
         */
        void UpdateDownloadL( TDownloadItem& aItem );        

    protected: // data

        /**
         * Parent class.
         */
        CVcxHgMyVideosVideoModelHandler& iParent;
        
        /**
         * CPeriodic. 
         * Own.
         */
        CPeriodic* iTimer;

        /**
         * Reference to array of videos.
         * Not own.
         */
        CVcxHgMyVideosVideoList& iVideoArray;

        /**
         * Flag for pause state of download progress follower.
         */
        TBool iPaused;

        /**
         * Array of polled downloads.
         * Own.
         */
        RArray<TDownloadItem> iDownloadArray;
    };

#endif // VCXHGMYVIDEOSDOWNLOADUPDATER_H
