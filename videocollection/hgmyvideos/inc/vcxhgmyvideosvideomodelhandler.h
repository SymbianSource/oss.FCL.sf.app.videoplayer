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
* Description:      UI level handler for video data.*
*/




#ifndef VCXHGMYVIDEOSVIDEOMODELHANDLER_H
#define VCXHGMYVIDEOSVIDEOMODELHANDLER_H

// INCLUDE FILES
#include "vcxmyvideosdefs.h"
#include "vcxhgmyvideosvideomodelobserver.h"

#include <e32cmn.h>
#include <ganes/HgScrollBufferObserverIface.h>

// FORWARD DECLARATIONS
class CVcxHgMyVideosVideoListImpl;
class CVcxHgMyVideosModel;
class CVcxHgMyVideosMainView;
class CMPXMedia;
class CVcxHgMyVideosVideoDataUpdater;
class CVcxHgMyVideosDownloadUpdater;
class TVcxHgMyVideosVideoDetailsDialog;
class CVcxHgMyVideosVideoList;
class CHgScroller;
class TVideoPlayerCustomMessage;
class CMyVideosIndicator;

// CLASS DECLARATION

/**
 * UI level handler for video data.
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS(  CVcxHgMyVideosVideoModelHandler ) :
        public CBase,
        public MVcxHgMyVideosVideoModelObserver,
        public MHgScrollBufferObserver
    {
    public:
    
        /**
         * Two-phased constructor.
         * 
         * @param aVideoListImpl Video list implementation
         * @param aModel Application model
         * @param aView Application view
         * @param aScroller Hg list control   
         * @return New object.
         */
        static CVcxHgMyVideosVideoModelHandler* NewL( 
            CVcxHgMyVideosVideoListImpl& aVideoListImpl,
            CVcxHgMyVideosModel& aModel,
            CVcxHgMyVideosMainView& aView,
            CHgScroller& aScroller );

        /**
         * Two-phased constructor.
         * 
         * @param aVideoListImpl Video list implementation
         * @param aModel Application model
         * @param aView Application view
         * @param aScroller Hg list control   
         * @return New object. 
         */
        static CVcxHgMyVideosVideoModelHandler* NewLC( 
            CVcxHgMyVideosVideoListImpl& aVideoListImpl, 
            CVcxHgMyVideosModel& aModel,
            CVcxHgMyVideosMainView& aView,
            CHgScroller& aScroller );

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosVideoModelHandler();
    
        /**
         * Activates video model.
         */
        void DoModelActivateL();
        
        /**
         * Deactivates video model.
         */
        void DoModelDeactivate();        

        /**
         * Makes sure model has correct video list.
         * 
         * @param aCategoryIndex of video list in MPX collection.
         */
        void UpdateVideoListL( TInt aCategoryIndex );
        
        /**
         * Resorts current video list, called when list sort-order changes.
         */
        void ResortVideoListL();
        
        /**
         * Gets list highlight.
         * 
         * @return Highlight.
         */
        TInt Highlight();        
        
        /**
         * Gets video's download state base on video index.
         * 
         * @param aIndex Video's index on UI list.
         * @return Video's download state as TVcxMyVideosDownloadState structure.
         */
        TVcxMyVideosDownloadState VideoDownloadState( TInt aIndex );
        
        /**
         * Checks if progressive play (playback during download) is possible.
         * 
         * @param aIndex Video's index on UI list.
         * @return ETrue if progressive play is possible.
         */
        TBool IsProgressivePlayPossible( TInt aIndex );

        /**
         * Returns the count of videos in UI list model.
         *
         * @return Count of videos in UI list model.
         */
        TInt VideoCount();

        /**
         * Returns the index of currently open video list, or KErrNotFound.
         * 0=All videos, 1=Downloads, 2=...
         * 
         * @return Index of currently open video list.
         */
        TInt CurrentCategoryIndex();

        /**
         * Returns a list of currently marked videos.
         * 
         * @return List of currently marked video indexes.
         */
        void MarkedVideosL( RArray<TInt>& aMarkedVideos );
        
        /**
         * Returns a list of item indexes which download 
		 * state is different than EVcxMyVideosDlStateNone. 
		 * 
		 * @return List of item indexes.
         */
        void OngoingDownloads( RArray<TInt>& aDownloads );
        
        /**
         * Handles mark command to video list.
         * 
         * @param aMarkCommand Mark command.
         */
        void HandleMarkCommandL( TInt aMarkCommand );
        
        /**
         * Returns last playback position of video, video identified by URI.
         * 
         * @param aContentUri URL or path to video.
         * @return Last playback position.
         */
        TInt LastPlaybackPosition( const TDesC& aContentUri );

        /**
         * Stores last playback position of video, video identified by URI.
         * 
         * @param aContentUri URL or path to video.
         * @param aPosition Last playback position.
         */
        void SetLastPlaybackPositionL( const TDesC& aContentUri, TInt aPosition );        

        /**
         * Returns video's name.
         * 
         * @param aIndex Video's index on UI list.
         * @return Video's name or empty string.
         */
        const TDesC& GetVideoName( TInt aIndex );
        
        /**
         * Returns video's URI.
         * 
         * @param aIndex Video's index on UI list.
         * @return Video's URI or empty string.
         */        
        const TDesC& GetVideoUri( TInt aIndex );
        
        /**
         * Returns video size in bytes
         * 
         * @param aIndex Video's index on UI list.
         * @return Video size in bytes or zero.
         */
        TInt64 GetVideoSize( TInt aIndex );

        /**
         * Deletes videos from requested indexes.
         * 
         * @param aOperationTargets Video indexes.
         */
        void DeleteVideosL( const RArray<TInt>& aOperationTargets );

        /**
         * Moves or copies videos from requested indexes to given target.
         * 
         * @param aOperationTargets Video indexes.
         * @param aTargetDrive Target drive.
         * @param aCopy If ETrue, copy is performed. If EFalse, move.
         */
        void MoveOrCopyVideosL( const RArray<TInt>& aOperationTargets,
                                TInt aTargetDrive,
                                TBool aCopy );

        /**
         * Play a video from requested index.
         * 
         * @param aIndex Video's index on UI list.
         */
        void PlayVideoL( TInt aIndex );
        
        /**
         * Resumes paused or failed download from requested index.
         * 
         * @param aIndex Video's index on UI list.
         */        
        void ResumeDownloadL( TInt aIndex );

        /**
         * Cancels a video download from requested index.
         * 
         * @param aIndex Video's index on UI list.
         */
        void CancelDownloadL( TInt aIndex );
        
        /**
         * Returns Video Player custom message of a video.
         * 
         * @param aFilteredIndex Video's index on filtered UI list.
         * @param aVideoInfo On return, Video Player custom message with video information.
         * @param aMpxId1 On return, MPX ID 1 of the video.
         */
        TInt GetVideoPlayerCustomMessage( TInt aFilteredIndex, 
                                          TVideoPlayerCustomMessage& aVideoInfo,
                                          TInt& aMpxId1 );

        /**
         * Returns Video Player custom message of a video.
         * 
         * @param aMpxItemId MPX Item Id of the video.
         * @param aVideoInfo On return, Video Player custom message with video information.
         */
        TInt GetVideoPlayerCustomMessage( TMPXItemId& aMpxItemId,
                                          TVideoPlayerCustomMessage& aVideoInfo );

        /**
         * Returns age profile of a video.
         * 
         * @param aIndex Video's index on UI list.
         * @return Age profile, or 0 if not defined.
         */
        TInt32 VideoAgeProfileL( TInt aIndex );

        /**
         * Returns age profile of a video.
         * 
         * @param aId TMPXItemId for the video.
         * @return Age profile, or 0 if not defined.
         */
        TInt32 VideoAgeProfileL( TMPXItemId aId );

        /**
         * Returns age profile of a video.
         * 
         * @param aMedia MPX media for the video.
         * @return Age profile, or 0 if not defined.
         */
        TInt32 VideoAgeProfileL( CMPXMedia* aMedia );

        /*
         * Tries to find MPX Item Id of unknown video.
         *
         * @param aVideoPath Path of the video.
         * @param aMpxItemId On return, MPX Item Id if found.
         * @return ETrue if video was found.
         */
        TBool FindVideoMpxIdL( const TDesC& aVideoPath, TMPXItemId& aMpxItemId );

        /**
         * Informative call about video being played. Method removes possible new
         * indicator from video.
         * 
         * @param aIndex Video's index on UI list.
         */
        void ClearNewVideoIndicatorL( TInt aIndex );        

        /**
         * Informative call about video being played. Method removes possible new
         * indicator from video.
         * 
         * @param aMpxItemId Video's MPX item Id.
         */
        void ClearNewVideoIndicatorL( TMPXItemId& aMpxItemId );

        /**
         * Gets data of currently highlighted video, and passes it to
         * TVcxHgMyVideosVideoDetailsDialog::ShowVideoDetailsDialogL().
         */
        void ShowVideoDetailsDialogL();
        
        /**
         * Returns video indicator.
         * 
         * @return Reference to video indicator.
         */
        CMyVideosIndicator& VideoIndicator();
        
    public: // From MVcxHgMyVideosVideoModelObserver
        
        /**
         * Called when a list of videos is available.
         * 
         * @param aVideoList List of videos
         */
        void NewVideoListL( CMPXMediaArray& aVideoList );
         
        /**
         * Called when the whole video list has been modified.
         * Normally this means that new items have been appended to list.
         */
        void VideoListModifiedL();
         
        /**
         * Called when a single video item has been modified (add/remove/modify).
         * 
         * @param aEventType Type of modification event (add/remove/modify).
         * @param aMpxItemId MPX Item Id of the modified item.
         * @param aExtraInfo Extra information for modification event.
         * @param aSimulated If ETrue, event is not originated from MPX Collection.
         */
        void VideoModifiedL( TMPXChangeEventType aEventType,
                              TMPXItemId aMpxItemId,
                              TInt32 aExtraInfo,
                              TBool aSimulated );
        
        /*
         * Called when a single video item has fetched.
         * Ownership of video is transferred.
         * 
         * @param aVideo Video item.
         */
        void VideoFetchingCompletedL( CMPXMedia* aVideo );
        
        /**
         * Called when resume has started from beginning.
         * @param aMpxId MPX ID of the video object.
         */
        void ResumeStartedFromBeginningL( TUint32 aMpxId );

        /**
         * Called when multiple video deletion command is completed.
         * 
         * @param aFailedCount Count of failed deletions, if any.
         * @param aFailedMpxId If above 1, MPXItemId of failed deletion.
         */
        void VideoDeletionCompletedL( TInt aFailedCount, TMPXItemId aFailedMpxId );         
         
        /**
         * Called when move or copy command is completed.
         * 
         * @param aFailedCount Count of failed moves/copies, if any.
         * @param aFailedMpxId If above 1, MPXItemId of failed move/copy.
         */
        void VideoMoveOrCopyCompletedL( TInt aFailedCount, TMPXItemId aFailedMpxId );
        
        /**
         * Called when all details for a video have been fetched.
         * 
         * @param aMedia MPX Media that contains all details.
         */
        void VideoDetailsCompletedL( const CMPXMedia& aMedia );
		
    public: // From MHgScrollBufferObserver
        
        /**
         * Buffer position changed. 
         * The items between the aBufferStart and aBufferEnd should be released.
         * 
         * @param aBufferStart The index of the first item in buffer.
         * @param aBufferEnd The index of the last item in buffer.
         */
        void Release( TInt aReleaseStart, TInt aReleaseEnd );
    
        /**
         * Buffer position changed. 
         * The items between the aBufferStart and aBufferEnd should be loaded as soon as possible.
         * 
         * @param aBufferStart The index of the first item in buffer.
         * @param aBufferEnd The index of the last item in buffer.
         * @param aDirection The direction of request.
         */
        void Request( TInt aRequestStart, TInt aRequestEnd, THgScrollDirection aDirection );
        
    private:    
        
        /**
         * Default constructor
         *
         * @param aVideoListImpl Video list implementation
         * @param aModel Application model
         * @param aView Application view
         * @param aScroller Hg list control
         */
        CVcxHgMyVideosVideoModelHandler( 
            CVcxHgMyVideosVideoListImpl& aVideoListImpl,
            CVcxHgMyVideosModel& aModel,
            CVcxHgMyVideosMainView& aView,
            CHgScroller& aScroller );            
    
        /**
         * 2nd phase constructor.
         */
        void ConstructL( );
    
        /**
         * Replaces old video array with new one. Old array is deleted. 
         * 
         * @param aVideoList Reference to new video array.
         */
        void ReplaceVideoArrayL( CMPXMediaArray& aVideoList );

        /**
         * Formats the second row string for video item.
         * 
         * @param aIndex List index of video
         * @param aMedia MPX Media object (video).
         * @param aIsDownloading On return, ETrue if video is downloading.
         * @return Second row string.
         */
        HBufC* FormatVideoSecondRowLC(
                TInt aIndex,
                CMPXMedia& aMedia,
                TBool& aIsDownloading );        
        
        /**
         * Formats the second row string for completed video.
         * 
         * @param aMedia MPX Media object (video).
         * @return Second row string.
         */
        HBufC* FormatVideoSecondRowCompletedLC( CMPXMedia& aMedia );

        /**
         * Formats the second row string for downloading video.
         * 
         * @param aIndex List index of video
         * @param aMedia MPX Media object (video).
         * @return Second row string.
         */
        HBufC* FormatVideoSecondRowDownloadingLC( TInt aIndex, CMPXMedia& aMedia );

        /**
         * Reads and returns download progress, makes sure that progress value
         * returned gets a value between 0% and 99%.
         * 
         * @param aMpxMedia Reference to MPX Media object.
         * @return Download progress. 
         */
        TInt DownloadProgressL( const CMPXMedia& aMpxMedia );
        
        /**
         * Updates video list item at given list index.
         * @param aListIndex Index.
         */
        void UpdateVideoListItemL( TInt aListIndex );
          
        /**
         * Deletes item from video array and UI list.
         * @param aMpxItemId MPX Item Id of the deleted item.
         */
        void DeleteItemL( TMPXItemId aMpxItemId );
        
        /**
         * List control calls this when it needs new buffer of data.
         * 
         * @param aRequestStart The index of the first item in buffer.
         * @param aRequestEnd The index of the last item in buffer.
         * @param aDirection The direction of request.
         */
        void HandleRequestL( TInt aRequestStart, 
                             TInt aRequestEnd, 
                             THgScrollDirection aDirection );
        
        /**
         * Inserts new video to video array and UI list.  
         *  
         * @param aVideo Video item. Ownership of video is transferred. 
         */ 
        void InsertVideoL( CMPXMedia* aVideo );
        
        /**
         * Compares MPX origin information to currently displayed category. 
         * 
         * @return ETrue if media belongs to category, otherwise EFalse.
         */
        TBool ValidOrigin( CMPXMedia& aMedia );
                
        /** 
         * Saves MPX id of highlighted item. 
         */
        void SaveHighlightedItemIdL();
        
        /** 
         * Update scrollbar strip type depending on sort order
         */
        void UpdateScrollbarTypeL( const TVcxMyVideosSortingOrder& aSortingOrder );

    private:
        
        /**
         * Video details dialog.
         * Own.
         */
        TVcxHgMyVideosVideoDetailsDialog* iVideoDetails;
        
        /**
         * Reference to video list implementation.
         * Not own.
         */
        CVcxHgMyVideosVideoListImpl& iVideoListImpl;
        
        /**
         * Reference to application model.
         * Not own.
         */
        CVcxHgMyVideosModel& iModel;

        /**
         * Reference to application view.
         * Not own.
         */        
        CVcxHgMyVideosMainView& iView;
        
        /**
         * Array of videos.
         * Own.
         */
        CVcxHgMyVideosVideoList* iVideoArray;
                
        /**
         * Class for updating video data (thumbnail, DRM status) asynchronously.
         * Own.
         */
        CVcxHgMyVideosVideoDataUpdater* iDataUpdater;
        
        /**
         * Class for polling & updating video download progress asynchrounously.
         * Own.
         */
        CVcxHgMyVideosDownloadUpdater* iDownloadUpdater; 
        
        /**
         * Hg list component.
         * Not own.
         */
        CHgScroller& iScroller;
        
        /**
         * Index of MPX collection video list currently displayed.
         */
        TInt iCurrentCategoryIndex;

        /**
         * Flag for indicating that highlight should restore to the position
         * where it was before model deactivating. 
         */ 
        TBool iRestoreHighlightPosition;
        
        /**
         * MPX id of highlighted item in a video list.
         */
        TMPXItemId iHighlightedItemId;
        
        /**
         * Downloading has been resumed for these MPX IDs. Needed for showing
		 * note "Resuming download not possible, download starts from beginning".
         */
        RArray<TUint32> iResumeArray;

        /**
         * Video indicator gives an overlay icon for video thumbnails.
         */
        CMyVideosIndicator* iVideoIndicator;
    };

#endif // VCXHGMYVIDEOSVIDEOMODELHANDLER_H
