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
* Description:   CVcxHgMyVideosVideoDataUpdater implementation*
*/




// INCLUDE FILES
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>

#include <ganes/HgScroller.h>
#include <ganes/HgItem.h>

#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include <thumbnailobjectsource.h>
#include <thumbnaildata.h>
#include <DRMCommon.h> // DRM
#include <gulicon.h>

#include "IptvDebug.h"
#include <myvideosindicator.h>
#include <vcxmyvideosdefs.h>
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideosmainview.h"
#include "vcxhgmyvideosvideolist.h"
#include "vcxhgmyvideosvideodataupdater.h"
#include "vcxhgmyvideosindicatorhelper.h"
#include "vcxhgmyvideosthumbnailmanager.h"

const TInt KRefreshTimerInterval( 1000000 );    // 1 second
const TInt KMaxThumbnailReqs( 2 );              // Max count of peek and get reqs combined
const TInt KMaxThumbnailGetReqs( 1 );           // Max count of get reqs
const TInt KMaxPredictiveSelect( 10 );          // Max range for selecting items before/after visible area
const TInt KScrollCheckInterval( 250000 );      // 0.25 seconds

// -----------------------------------------------------------------------------
// TimeStamp
// -----------------------------------------------------------------------------
//
static TInt64 TimeStamp()
    {
    TTime time;
    time.UniversalTime();
    return time.Int64();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoDataUpdater* CVcxHgMyVideosVideoDataUpdater::NewL(
        CVcxHgMyVideosModel& aModel,
        CHgScroller& aScroller,
        CVcxHgMyVideosVideoList& aVideoArray,
        CMyVideosIndicator& aVideosIndicator )
    {
    CVcxHgMyVideosVideoDataUpdater* self = 
        CVcxHgMyVideosVideoDataUpdater::NewLC( aModel,
                                               aScroller,
                                               aVideoArray,
                                               aVideosIndicator );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoDataUpdater* CVcxHgMyVideosVideoDataUpdater::NewLC(
        CVcxHgMyVideosModel& aModel,
        CHgScroller& aScroller,
        CVcxHgMyVideosVideoList& aVideoArray,
        CMyVideosIndicator& aVideosIndicator)
    {
    CVcxHgMyVideosVideoDataUpdater* self = 
        new (ELeave) CVcxHgMyVideosVideoDataUpdater( aModel,
                                                     aScroller,
                                                     aVideoArray,
                                                     aVideosIndicator );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::CVcxHgMyVideosVideoDataUpdater()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoDataUpdater::CVcxHgMyVideosVideoDataUpdater(
        CVcxHgMyVideosModel& aModel,
        CHgScroller& aScroller,
        CVcxHgMyVideosVideoList& aVideoArray,
        CMyVideosIndicator& aVideosIndicator )
  : CActive( EPriorityStandard ),
    iModel( aModel ),
    iScroller( aScroller ),
    iVideoArray( aVideoArray ),
    iVideosIndicator( aVideosIndicator ),
    iPaused( EFalse )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::ConstructL()
    {
    iRefreshTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iRetryTimer =   CPeriodic::NewL( CActive::EPriorityStandard );
    iModel.ThumbnailManager().AddObserverL( *this );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::InfoArrayChanged()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoDataUpdater::InfoArrayChanged()
    {
    // Can we optimise this? Many times new video list contains lot of items
    // from the old one. But how to make sure there isn't any radical changes...

    CancelAndDeleteFetchArray();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::~CVcxHgMyVideosVideoDataUpdater()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoDataUpdater::~CVcxHgMyVideosVideoDataUpdater()
    {
    iModel.ThumbnailManager().RemoveObserver( *this );
    Cancel();
    delete iRefreshTimer; // Cancels active timer
    delete iRetryTimer;
    iFetchArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::SetPausedL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::SetPausedL( TBool aPaused )
    {
    iPaused = aPaused;

    if ( ! iPaused )
        {
        ContinueVideoDataFetchingL();
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RequestDataL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::RequestDataL( TMPXItemId aMPXItemId )
    {
    AddItemToFetchArrayL( aMPXItemId );
    ContinueVideoDataFetchingL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::AddToRequestBufferL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::AddToRequestBufferL( TMPXItemId aMPXItemId )
    {
    AddItemToFetchArrayL( aMPXItemId );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::FlushRequestBufferL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::FlushRequestBufferL()
    {
    ContinueVideoDataFetchingL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::ReleaseData()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::ReleaseData( TMPXItemId aMPXItemId )
    {
    TInt index = IndexByMPXItemId( aMPXItemId );
    if ( index >= 0 )
        {
        RemoveItem( index ); 
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::PrepareForMoveOrDelete()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::PrepareForMoveOrDelete( TMPXItemId aMPXItemId )
    {
    TInt index = IndexByMPXItemId( aMPXItemId );
    if ( index >= 0 )
        {
        RemoveAndCancelThumbnailGeneration( index ); 
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RemoveAndCancelThumbnailGeneration()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::RemoveAndCancelThumbnailGeneration( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iFetchArray.Count() )
        {
        // Can be enabled when cancellation of (hd) thumbnail gets faster and
        // does not hang up UI
        CancelActivities( aIndex );
        delete iFetchArray[aIndex];
        iFetchArray.Remove( aIndex );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RemoveItem()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::RemoveItem( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iFetchArray.Count() )
        {    
        CVcxHgMyVideosVideoData* item = iFetchArray[aIndex];
        
        // When scrolling around canceling thumbnail creation is sometimes so slow
        // that it hangs UI for while. Thumbnail is needed sooner or later anyway.
        // Therefore let creation get finished in peace. It is possible to fetch already 
        // created thumbs during creation but not during hang up. 
        if ( item && !CancelNeeded( *item ) )
            {
            delete item;
            iFetchArray.Remove( aIndex );
            }         
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::AddItemToFetchArrayL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::AddItemToFetchArrayL( TMPXItemId aMPXItemId )
    {
    CVcxHgMyVideosVideoData* newItem = CVcxHgMyVideosVideoData::NewLC();
    newItem->SetMPXItemId( aMPXItemId );
    iFetchArray.AppendL( newItem );   
    
    CleanupStack::Pop( newItem );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::CancelActivities()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::CancelActivities( TInt aIndex )
    {    
    if ( aIndex >= 0 && aIndex < iFetchArray.Count() )
        {
        CVcxHgMyVideosVideoData* item = iFetchArray[aIndex];
        if ( item && CancelNeeded( *item ) )
            {
            iModel.ThumbnailManager().Cancel( item->ThumbnailConversionId() );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::CancelAndDeleteFetchArray()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::CancelAndDeleteFetchArray()
    {
    TInt count = iFetchArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CancelActivities( i );
        }
    iFetchArray.ResetAndDestroy();
    
    iPreviousFirstScrollerIndexTime = 0;
    iPreviousFirstScrollerIndex = iScroller.FirstIndexOnScreen();
    iPreviousModifiedIndexOnScreen = EFalse;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::ContinueVideoDataFetchingL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::ContinueVideoDataFetchingL()
    {
    iRetryTimer->Cancel();
    if ( !iPaused && iVideoArray.VideoCount() > 0 && iFetchArray.Count() > 0 )
        {
        TInt64 time = TimeStamp();
        TBool refreshTimerNeeded = EFalse;
        TInt peekReqs = 0;
        TInt getReqs = 0;
        GetActiveRequestCount( peekReqs, getReqs );
        TInt reqs = peekReqs + getReqs;
        if ( reqs < KMaxThumbnailReqs )
            {
            CVcxHgMyVideosVideoData::TVideoDataState state =
                        CVcxHgMyVideosVideoData::EVideoDataStateNone;
            CVcxHgMyVideosVideoData* prevItem = NULL;
            CVcxHgMyVideosVideoData* item = NULL;
            do
                {
                TInt err = KErrNone;
                prevItem = item;
                if ( !SelectNextIndexL( getReqs >= KMaxThumbnailGetReqs ) )
                    {
                    // Nothing to be started
                    if ( !reqs && iFetchArray.Count() > 0 )
                        {
                        // To ensure that thumbnail creation continues after
                        // disabled while scrolling
                        iRetryTimer->Start( KScrollCheckInterval, KScrollCheckInterval,
                            TCallBack( RetryTimerCallBack, this ) );
                        iPreviousFirstScrollerIndexTime = 0; // Force scroll check update
                        IPTVLOGSTRING_LOW_LEVEL(
                           "MPX My Videos UI # ContinueVideoDataFetchingL # iRetryTimer start" );
                        }
                    break;
                    }
                item = iFetchArray[0];
                state = item->State();
                if ( state == CVcxHgMyVideosVideoData::EVideoDataStateNone )
                    {
                    // Try first a quick peek with thumbnail creation denied
                    TRAP( err, StartThumbnailL( *item, ETrue ) );
                    if( err == KErrNone )
                        {
                        ++reqs;
                        refreshTimerNeeded = ETrue;
                        }
                    }
                else if ( state == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailPeekFinished ) 
                    {
                    if ( getReqs < KMaxThumbnailGetReqs )
                        {
                        // Try then get with thumbnail creation allowed
                        TRAP( err, StartThumbnailL( *item, EFalse ) );
                        if ( err == KErrNone )
                            {
                            ++reqs;
                            ++getReqs;
                            refreshTimerNeeded = ETrue;
                            }
                        }
                    }
                if ( err != KErrNone )
                    {
                    RemoveItem( 0 );
                    }                
                }
            while ( iFetchArray.Count() > 0 && reqs < KMaxThumbnailReqs && prevItem != item );
            }
        if ( refreshTimerNeeded && !iRefreshTimer->IsActive() )
            {
            iRefreshTimer->Start( KRefreshTimerInterval, KRefreshTimerInterval,
                TCallBack( RefreshTimerCallBack, this ) );
            IPTVLOGSTRING_LOW_LEVEL(
               "MPX My Videos UI # ContinueVideoDataFetchingL # iRefreshTimer start" );
            }
        if ( time - iPreviousFirstScrollerIndexTime >= KScrollCheckInterval )
            {
            // Store values for scroll direction check
            iPreviousFirstScrollerIndexTime = time;
            iPreviousFirstScrollerIndex = iScroller.FirstIndexOnScreen();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::UpdateVideoDataToUiL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::UpdateVideoDataToUiL( CVcxHgMyVideosVideoData& aVideoData )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxHgMyVideosVideoDataUpdater::UpdateVideoDataToUiL() ENTER" );

    TInt index = iVideoArray.IndexByMPXItemId( aVideoData.MPXItemId() );
    
    if ( index >= 0 && index < iScroller.ItemCount() )
        {       
        TBool drmUpdate = aVideoData.DrmProtected();
        CHgItem& listItem = iScroller.ItemL( index );
                
        if ( aVideoData.Thumbnail() )
            {
            CGulIcon* thumbnail = CGulIcon::NewL( aVideoData.Thumbnail( ETrue ) );
            listItem.SetIcon( thumbnail );
            CMPXMedia* media = iVideoArray.MPXMediaByMPXItemId( aVideoData.MPXItemId() );
            if ( iVideosIndicator.IsIndicatorShown( *media ) )
                {
                IPTVLOGSTRING_LOW_LEVEL("CVcxHgMyVideosVideoDataUpdater::UpdateVideoDataToUiL() has icon, has indicator" );
                listItem.SetFlags( CHgItem::EHgItemFlagsIconOverlayIndicator );
                }
            else
                {
                IPTVLOGSTRING_LOW_LEVEL("CVcxHgMyVideosVideoDataUpdater::UpdateVideoDataToUiL() has icon, no indicator" );
                listItem.ClearFlags( CHgItem::EHgItemFlagsIconOverlayIndicator );
                }
            }
        
        if ( drmUpdate )
            {
            TVcxHgMyVideosIndicatorHelper::TIndicatorDrmStatus drmStatus;
            CMPXMedia* media = iVideoArray.MPXMedia( index );
            
            if ( media )
                {
                TVcxHgMyVideosIndicatorHelper indicatorHelper;
                TInt indicator1( 0 );
                TInt indicator2( 0 );

                if ( aVideoData.ValidDrmRights() )
                    {
                    drmStatus = TVcxHgMyVideosIndicatorHelper::EIndicatorDrmStatusValid;
                    }
                else
                    {
                    drmStatus = TVcxHgMyVideosIndicatorHelper::EIndicatorDrmStatusExpired;
                    }

                TBool isNewVideo( EFalse );
                if ( media->IsSupported( KMPXMediaGeneralFlags ) )
                    {
                    if ( media->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags ) &
                        EVcxMyVideosVideoNew )
                        {
                        isNewVideo = ETrue;
                        }
                    }                    

                indicatorHelper.GetIndicatorsForVideoL(
                    iModel,
                    isNewVideo,
                    EFalse, // *Recordings*
                    media->ValueText( KMPXMediaGeneralUri ),
                    drmStatus,
                    indicator1,
                    indicator2 );
            
                if ( indicator1 )
                    {
                    listItem.SetFlags( indicator1 );
                    }
                if ( indicator2 )
                    {
                    listItem.SetFlags( indicator2 );
                    }
                }
            }
        
        if ( ListRefreshNeeded( index ) )
            {
            RefreshScreen();
            }
        }
    IPTVLOGSTRING_LOW_LEVEL("CVcxHgMyVideosVideoDataUpdater::UpdateVideoDataToUiL() RETURN" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::IndexByMPXItemId()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoDataUpdater::IndexByMPXItemId( TMPXItemId aMPXItemId )
    {   
    TInt count = iFetchArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iFetchArray[i]->MPXItemId() == aMPXItemId )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::SelectNextIndexL()
// -----------------------------------------------------------------------------
// 
TBool CVcxHgMyVideosVideoDataUpdater::SelectNextIndexL( TBool aSelectForPeekOnly )
    {
    TBool selected = EFalse;
    if ( iScroller.ItemCount() > 0 )
        {
        TInt firstIndexOnScreen = 0;
        TInt lastIndexOnScreen = 0;
        TInt lastIndex = 0;
        GetScrollerArea( firstIndexOnScreen, lastIndexOnScreen, lastIndex );

        // Determine scroll direction for optimal selection
        TInt maxPredict = KMaxPredictiveSelect;
        TBool scrollUp = iPreviousFirstScrollerIndex > firstIndexOnScreen;
        TBool scrollDown = iPreviousFirstScrollerIndex < firstIndexOnScreen;
        if ( scrollUp || scrollDown )
            {
            if ( scrollUp )
                {
                IPTVLOGSTRING_LOW_LEVEL(
                   "MPX My Videos UI # CVcxHgMyVideosVideoDataUpdater # scroll up" );
                }
            else
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "MPX My Videos UI # CVcxHgMyVideosVideoDataUpdater # scroll down" );
                }
            aSelectForPeekOnly = ETrue; // Disable thumb creation while scrolling
            }
        else
            {
            maxPredict /= 2; // Split range when checking both directions
            }

        if ( !aSelectForPeekOnly || scrollUp )
            {
            // Try visible area first with thumb creation disabled to get
            // already created thumbs as fast as possible
            selected = TrySelectFromScrollerAreaL( firstIndexOnScreen, 
                                                   lastIndexOnScreen,
                                                   ETrue );
            }
        if ( !selected && !scrollUp )
            {
            // Try visible area and items below
            TInt end = Min( lastIndexOnScreen + maxPredict, lastIndex );
            selected = TrySelectFromScrollerAreaL( firstIndexOnScreen, end, 
                    aSelectForPeekOnly );
            }
        if ( !selected && !scrollDown && firstIndexOnScreen > 0 )
            {
            // Try items above visible area
            TInt end = Max( firstIndexOnScreen - maxPredict - 1, 0 );
            selected = TrySelectFromScrollerAreaL( firstIndexOnScreen - 1, end,
                    aSelectForPeekOnly );
            }
        if ( !selected )
            {
            // Try any item
            TInt count = iFetchArray.Count();
            for ( TInt i = 0; i < count; i++ )
                {
                if ( TrySelectL( i, aSelectForPeekOnly ) )
                    {
                    selected = ETrue;
                    break;
                    }
                }
            }
        }
    return selected;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RefreshTimerCallBack()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoDataUpdater::RefreshTimerCallBack( TAny* aAny )
    {
    CVcxHgMyVideosVideoDataUpdater* self = static_cast<CVcxHgMyVideosVideoDataUpdater*>( aAny ); 
    if ( !self->iPaused && self->iFetchArray.Count() > 0 )
        {
        // Do refresh only if on screen item has been modified
        if ( self->iPreviousModifiedIndexOnScreen )
            {
            self->iPreviousModifiedIndexOnScreen = EFalse; // Reset refresh checking
            self->RefreshScreen();
            }
        }
    else
        {
        self->iRefreshTimer->Cancel();
        IPTVLOGSTRING_LOW_LEVEL(
           "MPX My Videos UI # RefreshTimerCallBack # iRefreshTimer stop" );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RefreshScreen()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::RefreshScreen()
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoDataUpdater::RefreshScreen()" );
    iScroller.DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::ListRefreshNeeded()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoDataUpdater::ListRefreshNeeded( TInt aIndex )
    {
    TInt firstIndexOnScreen = 0;
    TInt lastIndexOnScreen = 0;
    TInt lastIndex = 0;
    GetScrollerArea( firstIndexOnScreen, lastIndexOnScreen, lastIndex );
    
    TBool modifiedIndexOnScreen = aIndex >= firstIndexOnScreen &&
        aIndex <= lastIndexOnScreen;
    TBool refreshNeeded( EFalse );

    // Refresh rules:
    // 1) Refresh if off screen item is detected after on screen item
    // 2) Refresh if item is the last
    if ( ( iPreviousModifiedIndexOnScreen && !modifiedIndexOnScreen ) ||
         iFetchArray.Count() <= 1 )
        {
        // Restart refresh timer if there are items left after current one
        iRefreshTimer->Cancel();
        if ( iFetchArray.Count() > 1 )
            {
            iRefreshTimer->Start( KRefreshTimerInterval, KRefreshTimerInterval,
                TCallBack( RefreshTimerCallBack, this ) );
            IPTVLOGSTRING_LOW_LEVEL(
               "MPX My Videos UI # ListRefreshNeeded # iRefreshTimer start" );
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL(
               "MPX My Videos UI # ListRefreshNeeded # iRefreshTimer stop" );
            }
        refreshNeeded = ETrue;
        }
    iPreviousModifiedIndexOnScreen = modifiedIndexOnScreen;
    return refreshNeeded;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::ThumbnailPreviewReady()
// From MThumbnailManagerObserver, not used in Video Center.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::ThumbnailPreviewReady( MThumbnailData& /*aThumbnail*/, 
                                                            TThumbnailRequestId /*aId*/ )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::ThumbnailReady()
// From MThumbnailManagerObserver
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::ThumbnailReady( TInt aError,
                                                     MThumbnailData& aThumbnail,
                                                     TThumbnailRequestId aId )
    {
    IPTVLOGSTRING3_LOW_LEVEL( 
        "MPX My Videos UI # ThumbnailReady(error=%d, thumbID=%d)", aError, aId );

    TInt count = iFetchArray.Count();
    for( TInt i = 0; i < count; ++i )
        {
        CVcxHgMyVideosVideoData* item = iFetchArray[i];
        if ( aId == item->ThumbnailConversionId() )
            {
            if ( aError == KErrNone ||
			     aError == KErrCompletion || // Accept blacklisted
                 item->State() == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailStarted )
                {
                // Never delete this, ownership gone to Ganes list
                item->SetThumbnail( aError == KErrNone ? aThumbnail.DetachBitmap() : NULL );
                item->SetState( CVcxHgMyVideosVideoData::EVideoDataStateThumbnailFinished );
                StartFinalActions();
                }
            else if ( aError == KErrNotFound &&
            	item->State() == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailPeekStarted )
                {
                // Try getting thumbnail with create allowed when peek failed with not found
                item->SetState( CVcxHgMyVideosVideoData::EVideoDataStateThumbnailPeekFinished );
                }
            else
                {
                // Stop thumbnail peek attemps
                item->SetState( CVcxHgMyVideosVideoData::EVideoDataStateThumbnailFinished );
                StartFinalActions();
                }

            TRAPD( err, ContinueVideoDataFetchingL() );
            if ( err != KErrNone )
                {
                IPTVLOGSTRING2_LOW_LEVEL(
                "MPX My Videos UI # CVcxHgMyVideosVideoDataUpdater::ThumbnailReady, err = %d",
                err );
                }
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RunL()
// From CActive
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::RunL()
    {
    if ( !iPaused )
        {
        TInt i = iFetchArray.Count() - 1;
        while( i >= 0 )
            {
            CVcxHgMyVideosVideoData* item = iFetchArray[i];
            if ( item->State() == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailFinished )
                {
                TRAP_IGNORE(
                    {
                    CheckDrmL( *item );
                    UpdateVideoDataToUiL( *item );
                    } );				
                delete item;
                iFetchArray.Remove(i);
#if 0
                if ( iFetchArray.Count() > 0 )
                    {
                    // If drm checking is time consuming, proceed finalisation later
                    StartFinalActions();
                    break;
                    }
#endif
                }
            --i;
            }
        if ( !iFetchArray.Count() )
            {
            // No items left, timers are not needed anymore
            iRefreshTimer->Cancel();
            iRetryTimer->Cancel();
            IPTVLOGSTRING_LOW_LEVEL(
               "MPX My Videos UI # RunL # iRefreshTimer stop" );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RunError()
// From CActive
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoDataUpdater::RunError( TInt aError )
    {
    IPTVLOGSTRING2_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosVideoDataUpdater::RunError, aError = %d", aError );
    
    if ( aError != KErrNone )
        {
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::DoCancel()
// From CActive
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::CheckDrmL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::CheckDrmL( CVcxHgMyVideosVideoData& aVideoData )
    {
    CMPXMedia* media = iVideoArray.MPXMediaByMPXItemId( aVideoData.MPXItemId() );
    if ( media && media->IsSupported( KMPXMediaGeneralUri ) )
        {
        TUint32 flags = 0;
        if ( media->IsSupported( KMPXMediaGeneralFlags ) )
            {
            flags = media->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
            }
        if ( flags & EVcxMyVideosVideoDrmProtected )
            {
            aVideoData.SetDrmProtected( ETrue );
            aVideoData.SetValidDrmRights( EFalse );

            ContentAccess::CData* cData = CData::NewLC( 
                               (TVirtualPathPtr) media->ValueText( KMPXMediaGeneralUri ),
                               EPeek,
                               EContentShareReadWrite );
            TInt intentResult = cData->EvaluateIntent( ContentAccess::EPlay );

            // Not valid rights should return KErrCANoRights, KErrCANoPermission,
            // or in rare cases KErrCAPendingRights. But we don't trust those and
            // just compare against KErrNone.
            if ( intentResult == KErrNone )
                {
                aVideoData.SetValidDrmRights( ETrue );
                }
            CleanupStack::PopAndDestroy( cData );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::GetActiveRequestCount()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::GetActiveRequestCount(
        TInt& aPeekRequests, TInt& aGetRequests )
    {
    aPeekRequests = 0;
    aGetRequests = 0;
    TInt count = iFetchArray.Count();
    for( TInt i = 0; i < count; ++i )
        {
        CVcxHgMyVideosVideoData::TVideoDataState state = iFetchArray[i]->State();
        if ( state == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailPeekStarted )
            {
            ++aPeekRequests;
            }
        else if ( state == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailStarted )
            {
            ++aGetRequests;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::StartThumbnailL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::StartThumbnailL(
        CVcxHgMyVideosVideoData& aItem, TBool aPeek )
    {
    CMPXMedia* media = iVideoArray.MPXMediaByMPXItemId( aItem.MPXItemId() );
    if ( media && media->IsSupported( KMPXMediaGeneralUri ) )
        {
        TPtrC uri = media->ValueText( KMPXMediaGeneralUri ); 
        TPtrC mime = media->IsSupported( KMPXMediaGeneralMimeType ) ? 
                        media->ValueText( KMPXMediaGeneralMimeType ) : KNullDesC;
        TThumbnailRequestId id = 0;
        if ( aPeek )
            {
            CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(
                    uri, mime );
            id = iModel.ThumbnailManager().PeekL( *source );
            CleanupStack::PopAndDestroy( source );
            }
        else
            {
            CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(
                    uri, mime );
            id = iModel.ThumbnailManager().GetL( *source );
            CleanupStack::PopAndDestroy( source ); 
            }
        aItem.SetThumbnailConversionId( id );

        IPTVLOGSTRING4_LOW_LEVEL( 
                "MPX My Videos UI # StartThumbnailL() called thumbID %d for %S (peek %d)",
                aItem.ThumbnailConversionId(),
                &media->ValueText( KMPXMediaGeneralUri ),
                aPeek );

        aItem.SetState( aPeek ?
                CVcxHgMyVideosVideoData::EVideoDataStateThumbnailPeekStarted :
                CVcxHgMyVideosVideoData::EVideoDataStateThumbnailStarted );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::TrySelectL()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoDataUpdater::TrySelectL( TInt aIndex, 
                                                  TBool aSelectForPeekOnly )
    {
    // Move selected index to first index of the fetch array
    TBool selected = EFalse;
    CVcxHgMyVideosVideoData* item = iFetchArray[aIndex];
    CVcxHgMyVideosVideoData::TVideoDataState state = item->State();
    if ( aSelectForPeekOnly )
        {
        // Accept item only for peeking
        if ( state == CVcxHgMyVideosVideoData::EVideoDataStateNone )
            {
            iFetchArray.InsertL( item, 0 );
            iFetchArray.Remove( aIndex + 1 );
            selected = ETrue;
            }
        }
    else if ( state == CVcxHgMyVideosVideoData::EVideoDataStateNone ||
              state == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailPeekFinished )
        {
        // Accept any item that waits to be fetched
        iFetchArray.InsertL( item, 0 );
        iFetchArray.Remove( aIndex + 1 );
        selected = ETrue;
        }
    return selected;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::TrySelectFromScrollerL()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoDataUpdater::TrySelectFromScrollerL(
    TInt aPos, TBool aSelectForPeekOnly )
    {
    TBool selected = EFalse;
    CGulIcon* icon = iScroller.ItemL( aPos ).Icon();
    TMPXItemId mpxItemId = iVideoArray.ArrayIndexToMpxItemIdL( aPos );
    TInt index = IndexByMPXItemId( mpxItemId );
    if ( index >= 0 )
        {
		// Skip fetch selection if icon already exist
        if ( !icon )
            {
            if ( TrySelectL( index, aSelectForPeekOnly ) )
                {
                selected = ETrue;
                }
            }
        else
            {
            iFetchArray[index]->SetState(
                CVcxHgMyVideosVideoData::EVideoDataStateThumbnailFinished );
            StartFinalActions();
            }
        }
    return selected;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::TrySelectFromScrollerAreaL()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoDataUpdater::TrySelectFromScrollerAreaL( 
    TInt aStartPos, TInt aEndPos, TBool aSelectForPeekOnly )
    {
    TBool selected = EFalse;
    if ( aEndPos >= aStartPos )
        {
        // Search forwards
        for ( TInt i = aStartPos; i <= aEndPos; i++ )
            {
            if ( TrySelectFromScrollerL( i, aSelectForPeekOnly ) )
                {
                selected = ETrue;
                break;
                }
            }
        }
    else
        {
        // Search backwards
        for ( TInt i = aStartPos; i >= aEndPos; i-- )
            {
            if ( TrySelectFromScrollerL( i, aSelectForPeekOnly ) )
                {
                selected = ETrue;
                break;
                }
            }
        }
    return selected;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::StartFinalActions()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::StartFinalActions()
    {
    if ( !IsActive() )
        {
        SetActive();
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::CancelNeeded()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoDataUpdater::CancelNeeded( CVcxHgMyVideosVideoData& aItem )
    {
    CVcxHgMyVideosVideoData::TVideoDataState state = aItem.State();
    return ( state == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailPeekStarted ||
             state == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailStarted );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RetryTimerCallBack()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoDataUpdater::RetryTimerCallBack( TAny* aAny )
    {
    CVcxHgMyVideosVideoDataUpdater* self = static_cast<CVcxHgMyVideosVideoDataUpdater*>( aAny ); 
    self->iRetryTimer->Cancel();
    TRAPD( err, self->ContinueVideoDataFetchingL() );
    if ( err != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
        "MPX My Videos UI # CVcxHgMyVideosVideoDataUpdater::RetryTimerCallBack, err = %d",
        err );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::GetScrollerArea()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::GetScrollerArea( TInt& aFirstIndexOnScreen, 
                                                      TInt& aLastIndexOnScreen, 
                                                      TInt& aLastIndex )
    {
    aLastIndex = Max( iScroller.ItemCount() - 1, 0 );
    aFirstIndexOnScreen = Max( iScroller.FirstIndexOnScreen(), 0 );
    aLastIndexOnScreen = Min( aFirstIndexOnScreen + iScroller.ItemsOnScreen(), aLastIndex );
    }
