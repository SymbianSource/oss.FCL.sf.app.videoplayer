/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
class CVcxHgMyVideosVideoDetailsDialog;
class CVcxHgMyVideosVideoList;
class CHgScroller;
class CMyVideosIndicator;
class CRepository;

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
         * Handles mark command to video list.
         * 
         * @param aMarkCommand Mark command.
         */
        void HandleMarkCommandL( TInt aMarkCommand );

        /**
         * Returns video's name.
         * 
         * @param aId Video's unique id.
         * @return Video's name or empty string.
         */
        const TDesC& GetVideoName( TInt aId );
        
        /**
         * Returns video's URI.
         * 
         * @param aIndex Video's index on UI list.
         * @return Video's URI or empty string.
         */        
        const TDesC& GetVideoUri( TInt aIndex );
        
        /**
         * Returns video's unique mpx id.
         * 
         * @param aIndex Video's index on UI list.
         * @return Video's id or KErrNotFound.
         */  
        TInt GetVideoId( TInt aIndex );
        
        /**
         * Returns video size in bytes
         * 
         * @param aIndex Video's index on UI list.
         * @return Video size in bytes or zero.
         */
        TInt64 GetVideoSize( TInt aIndex );

        /**
         * Deletes videos with requested ids.
         * 
         * @param aOperationTargets Videos' mpx ids.
         */
        void DeleteVideosL( const RArray<TInt>& aOperationTargets );

        /**
         * Moves or copies videos with requested ids to given target.
         * 
         * @param aOperationTargets Videos' mpx ids.
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
         * @param aMedia MPX Media object (video).
         * @return Second row string.
         */
        HBufC* FormatVideoSecondRowLC( CMPXMedia& aMedia );
        
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
         * Saves MPX id of first (topmost) visible item.
         */
        void SaveFirstListItemIdL();

        /**
         * Update scrollbar strip type depending on sort order
         */
        void UpdateScrollbarTypeL( const TVcxMyVideosSortingOrder& aSortingOrder );

        /**
         * Resets scroller items for refetch only
         */
        void ResetScrollerItemsL();

        /** 
         * Resets scroller buffer and items for refetch
         * This is fastest way to reset scroller when item count is unchanged
         */
        void ResetScrollerBufferAndItemsL();

        /** 
         * Resizes scroller with refetch
         * This is faster way to reset scroller than CHgScroller::Reset() and 
         * CHgScroller::ResizeL() calls combined
         * @param aNewItemCount New item count
         */
        void ResizeScrollerL( TInt aNewItemCount );
		
        /**
         * Sets video as last watched.
         *    
         * @param aMedia Media that will be last watched.    
         */
        void SetVideoLastWatchedL( CMPXMedia& aMedia );
        
        /**
         * Clears new video tag from the file.
         * 
         * @param aMedia Media from which new video tag will be cleared.         
         */
        void ClearNewVideoFlagL( CMPXMedia& aMedia );

        /**
         * Saves the marked MPXMedia items to iMarkedMediaList. This is called before
         * re-sorting the videos list if marking mode is enabled to preserve the
         * marking status of marked items.
         */
        void SaveMarkingsL();

        /**
         * Restores the marking status of the MPXMedia items from iMarkedMediaList.
         * This is called after re-sorting the list if marking mode is enabled to
         * restore the marking status of the marked items.
         * This method resets iMarkedMediaList before returning.
         */
        void RestoreMarkingsL();
        
        /**
         * Resets the empty thumbnail from DRM protected video clip.
         * This causes the thumbnail to be attempted to be regenerated
         * next time the thumbnail is fetched fron thumbnail manager.
         *
         * @param aMedia Media from which new video tag will be cleared.   
         * @param aIndex Index of the visible item.
         */
        void ResetEmptyDrmThumnailL( CMPXMedia& aMedia, TInt aIndex );

    private:
        
        /**
         * Video details dialog.
         * Own.
         */
        CVcxHgMyVideosVideoDetailsDialog* iVideoDetails;
        
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
         * Hg list component.
         * Not own.
         */
        CHgScroller& iScroller;
        
        /**
         * Index of MPX collection video list currently displayed.
         */
        TInt iCurrentCategoryIndex;

        /**
         * Flag for indicating that list should restore to the position
         * where it was before model deactivating. 
         */ 
        TBool iRestoreListPosition;
        
        /**
         * MPX id of first (topmost) visible item in a video list.
         */
        TMPXItemId iFirstListItemId;

        /**
         * Video indicator gives an overlay icon for video thumbnails.
         */
        CMyVideosIndicator* iVideoIndicator;

        /**
         * Array of marked MPXMedia items to temporarily hold a list of marked items in video list.
         * The array is temporarily filled with pointers to marked media items prior to re-sorting
         * the video list and is cleared after sorting.
         */
        RPointerArray<CMPXMedia> iMarkedMediaList;
		
        /**
         * Cenrep session.
         */
        CRepository* iRepository;

    };

#endif // VCXHGMYVIDEOSVIDEOMODELHANDLER_H
