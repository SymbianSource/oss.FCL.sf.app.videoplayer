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




#ifndef VCXHGMYVIDEOSVIDEODATAUPDATER_H
#define VCXHGMYVIDEOSVIDEODATAUPDATER_H

// INCLUDE FILES
#include <coemain.h>
 
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include "vcxhgmyvideosvideodata.h"

// FORWARD DECLARATIONS
class CVcxHgMyVideosModel;
class CHgScroller;
class CMPXMediaArray;
class CThumbnailManager;
class CVcxHgMyVideosVideoList;
class CPeriodic;
class CMyVideosIndicator;

// CLASS DECLARATION

/**
 *
 * @lib vcxhgmyvideos.lib
 */
NONSHARABLE_CLASS( CVcxHgMyVideosVideoDataUpdater ) :
        public CActive,
        public MThumbnailManagerObserver 
    {
    public:

        /**
         * Two-phased constructor.
         * 
         * @param aModel Reference to component model.
         * @param aScroller Reference to Hg list control.
         * @param aVideoArray Reference to video array.
         * @return New object.
         */
        static CVcxHgMyVideosVideoDataUpdater* NewL( 
                CVcxHgMyVideosModel& aModel,
                CHgScroller& aScroller,
                CVcxHgMyVideosVideoList& aVideoArray,
                CMyVideosIndicator& aVideosIndicator );

        /**
         * Two-phased constructor.
         * 
         * @param aModel Reference to component model.
         * @param aScroller Reference to Hg list control.
         * @param aVideoArray Reference to video array.
         * @return New object. 
         */
        static CVcxHgMyVideosVideoDataUpdater* NewLC( 
                CVcxHgMyVideosModel& aModel,
                CHgScroller& aScroller,
                CVcxHgMyVideosVideoList& aVideoArray,
                CMyVideosIndicator& aVideosIndicator );

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosVideoDataUpdater();

        /**
         * Sets data updater to pause or normal mode.
         * 
         * @param aPaused If ETrue, updater is paused.
         */
        void SetPausedL( TBool aPaused );

        /**
         * Called to inform video data updater that info array has changed.
         * Causes re-setting of all data as info array is the bases for fetched
         * additional video data.  
         */
        void InfoArrayChanged();        

        /**
         * Called by video list implementation when new data is needed.
         * Appends data item into buffer and flushes it to start fetching.
         * 
         * @param aMPXItemId MPX item id of video data.
         */
        void RequestDataL( TMPXItemId aMPXItemId );

        /**
         * Called by video list implementation when new data is needed.
         * Appends data item into buffer that needs to be flushed using 
         * FlushRequestBufferL() to start fetching.
         * 
         * @param aMPXItemId MPX item id of video data.
         */
        void AddToRequestBufferL( TMPXItemId aMPXItemId );

        /**
         * Called by video list implementation when new data is needed.
         * Flushes fetch requests from buffer.
         */
        void FlushRequestBufferL();

        /**
         * Called by video list implementation when data is not needed anymore.
         * Removes data item from buffer.
         * 
         * @param aMPXItemId MPX item id of video data.
         */
        void ReleaseData( TMPXItemId aMPXItemId );
        
        /**
         * Called when user wants to move or delete video from UI. Removes
         * data item from buffer and cancels ongoing thumbnail generation.
         * 
         * @param aMPXItemId MPX item id of video data.         
         */
        void PrepareForMoveOrDelete( TMPXItemId aMPXItemId );
        
    public: // From MThumbnailManagerObserver
        
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
                
    public: // From CActive

        /**
         * From CActive.
         */
        void RunL();

        /**
         * From CActive. 
         * 
         * @param aError Error code.
         * @return KErrNone if continuing is ok.
         */        
        TInt RunError( TInt aError );
        
        /**
         * From CActive.
         */        
        void DoCancel();

    protected:    
        
        /**
         * Default constructor
         *
         * @param aModel Reference to component model.
         * @param aScroller Reference to Hg list control.
         * @param aVideoArray Reference to video array.
         */
        CVcxHgMyVideosVideoDataUpdater( 
                CVcxHgMyVideosModel& aModel,
                CHgScroller& aScroller,
                CVcxHgMyVideosVideoList& aVideoArray,
                CMyVideosIndicator& aVideosIndicator );
    
        /**
         * 2nd phase constructor.
         */
        void ConstructL();
		
		/**
         * Returns index of fetch array based on MPX Item Id.
         * 
         * @param aMpxItemId MPX Item Id.
         * @return Index of array or KErrNotFound.
         */
        TInt IndexByMPXItemId( TMPXItemId aMpxItemId );
        
        /**
         * Selects next index to be fetched.
         * 
         * @param aSelectForPeekOnly    Select for peek only.
         * @return ETrue if selected otherwise EFalse.
         */
        TBool SelectNextIndexL( TBool aSelectForPeekOnly );
        
        /**
         * Removes item from fetch array.
         * 
         * @param aIndex Index of fetch array.
         */
        void RemoveItem( TInt aIndex );
        
        /**
         * Removes item from fetch array and cancel thumbnail generation.
         * 
         * @param aIndex Index of fetch array.         
         */
        void RemoveAndCancelThumbnailGeneration( TInt aIndex );
                
        /**
         * Cancels possibly ongoing activities (thumbnail generation, DRM check).
         *
         * @param aIndex Index of fetch array.
         */
        void CancelActivities( TInt aIndex );
                
        /**
         * Adds item into fetch array. 
         * 
         * @param aMPXItemId MPX item id.
         */
        void AddItemToFetchArrayL( TMPXItemId aMPXItemId );
        
        /**
         * Cancels any on-going video data operations and deletes fetch array.
         */
        void CancelAndDeleteFetchArray();
        
        /**
         * Makes sure that video data fetching continues.
         */
        void ContinueVideoDataFetchingL();
        
        /**
         * Updates fetched video data to UI.
         */
        void UpdateVideoDataToUiL( CVcxHgMyVideosVideoData& aVideoData );
        
        /**
         * Checks if list refreshing is needed.
		 * 
		 * @param aIndex The item index on the Ganes list that was modified last.
         * @return ETrue if refreshing is needed, otherwise EFalse.
		 */
        TBool ListRefreshNeeded( TInt aIndex );
        
        /**
         * Callback method for RefreshTimer.  
         */
        static TInt RefreshTimerCallBack( TAny* aAny );
        
        /**
         * Refreshes the ganes list.
         */
        void RefreshScreen();

        /**
         * Checks DRM properties
		 *
		 * @param aVideoData	Reference to video data.
         */
        void CheckDrmL( CVcxHgMyVideosVideoData& aVideoData );

        /**
         * Gets active request count
         *
		 * @param aPeekRequests	On return number of peek requests.
		 * @param aGetRequests	On return number of get requests.
		 */
        void GetActiveRequestCount( TInt& aPeekRequests, TInt& aGetRequests );

        /**
         * Starts thumbnail fetch
         */
        void StartThumbnailL( CVcxHgMyVideosVideoData& aItem, TBool aPeek );

        /**
         * Tries to select item for fetch.
		 * 
		 * @param aIndex				Index
		 * @param aSelectForPeekOnly	ETrue
		 * @return ETrue if selected, otherwise EFalse
         */
        TBool TrySelectL( TInt aIndex, TBool aSelectForPeekOnly );

        /**
         * Tries to select item for fetch.
		 *
		 * @param aPos                Scroller position.
		 * @param aSelectForPeekOnly  Select for peek only status.
		 * @return ETrue if selected, otherwise EFalse.
         */
        TBool TrySelectFromScrollerL( TInt aPos, TBool aSelectForPeekOnly );

        /**
         * Tries to select item for fetch.
		 *
		 * @param aStartPos           Start position.
         * @param aEndPos             End position.
		 * @param aSelectForPeekOnly  Select for peek only status.
		 * @return ETrue if selected, otherwise EFalse.
         */
        TBool TrySelectFromScrollerAreaL( TInt aStartPos, 
                                          TInt aEndPos,
                                          TBool aSelectForPeekOnly );

        /**
         * Starts final actions for finished thumbnails
         */
        void StartFinalActions();

        /**
         * Checks if cancellation of activities is needed for item
		 *
		 * @param aItem		Reference to data item.
		 * @return ETrue if cancel is needed, otherwise EFalse.
         */
        TBool CancelNeeded( CVcxHgMyVideosVideoData& aItem );

        /**
         * Callback method for RetryTimer.  
         */
        static TInt RetryTimerCallBack( TAny* aAny );

        /**
         * Gets scroller area
		 *
		 * @param aFirstIndexOnScreen     First index on screen.
 		 * @param aLastIndexOnScreen      Last index on screen.
		 * @param aLastIndex              Last index.
         */
        void GetScrollerArea( TInt& aFirstIndexOnScreen, 
							  TInt& aLastIndexOnScreen,
        					  TInt& aLastIndex );

    protected:

        /**
         * Reference to component's model.
         * Not own.
         */
        CVcxHgMyVideosModel& iModel;
        
        /**
         * Hg list component.
         * Not own.
         */
        CHgScroller& iScroller;
        
        /**
         * Array of videos that need data fetching.
         * Own
         */
        RPointerArray<CVcxHgMyVideosVideoData> iFetchArray; 
        
        /**
         * Reference to video array.
         * Not own.
         */
        CVcxHgMyVideosVideoList& iVideoArray; 
                        
        /** 
         * Used for indicating that list needs to be refreshed.         
         * Own.
         */ 
        CPeriodic* iRefreshTimer;
        
        /**
         * Flag for pause state.
         */
        TBool iPaused;

        /**
         * Stores previous first scroller index for checking scroll direction 
         */
        TInt iPreviousFirstScrollerIndex;

        /**
         * Stores time of previous scroller first index update
         */
        TInt64 iPreviousFirstScrollerIndexTime;

        /** 
         * Used for restarting thumbnail creation after scroll
         * Own.
         */
        CPeriodic* iRetryTimer;

        /**
         * For refresh logic
         */
        TBool iPreviousModifiedIndexOnScreen;

        /**
         * Reference to overlay indicator handler
         * Not own.
         */
        CMyVideosIndicator& iVideosIndicator;

    };

#endif // VCXHGMYVIDEOSVIDEODATAUPDATER_H
