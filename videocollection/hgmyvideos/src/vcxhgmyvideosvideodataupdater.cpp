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
#include <vcxmyvideosdefs.h>
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideosmainview.h"
#include "vcxhgmyvideosvideolist.h"
#include "vcxhgmyvideosvideodataupdater.h"
#include "vcxhgmyvideosindicatorhelper.h"
#include "vcxhgmyvideosthumbnailmanager.h"

const TInt KRefreshTimerInterval( 1000000 ); // 1 second
const TInt KMaxThumbnailReqs( 2 ); // Max count of peek and get reqs combined
const TInt KMaxThumbnailGetReqs( 1 ); // Max count of get reqs

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoDataUpdater* CVcxHgMyVideosVideoDataUpdater::NewL(
        CVcxHgMyVideosModel& aModel,
        CHgScroller& aScroller,
        CVcxHgMyVideosVideoList& aVideoArray )
    {
    CVcxHgMyVideosVideoDataUpdater* self = 
        CVcxHgMyVideosVideoDataUpdater::NewLC( aModel,
                                               aScroller,
                                               aVideoArray );
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
        CVcxHgMyVideosVideoList& aVideoArray )
    {
    CVcxHgMyVideosVideoDataUpdater* self = 
        new (ELeave) CVcxHgMyVideosVideoDataUpdater( aModel,
                                                     aScroller,
                                                     aVideoArray );
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
        CVcxHgMyVideosVideoList& aVideoArray )
  : CActive( EPriorityStandard ),
    iModel( aModel ),
    iScroller( aScroller ),
    iVideoArray( aVideoArray ),
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
    RemoveItem( IndexByMPXItemId( aMPXItemId ) );  
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RemoveItem()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::RemoveItem( TInt aIndex )
    {
    if ( aIndex >= 0 && aIndex < iFetchArray.Count() )
        {
        CancelActivities( aIndex );
        
        delete iFetchArray[aIndex];
        iFetchArray[aIndex] = NULL;
        iFetchArray.Remove( aIndex );
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
        CVcxHgMyVideosVideoData::TVideoDataState state = item->State();
        if ( state == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailPeekStarted ||
             state == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailStarted )
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
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::ContinueVideoDataFetchingL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::ContinueVideoDataFetchingL()
    {
    if ( !iPaused && iVideoArray.VideoCount() > 0 && iFetchArray.Count() > 0 )
        {
        TBool startRefreshTimer = EFalse;
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
                SelectNextIndexL( getReqs >= KMaxThumbnailGetReqs );
                item = iFetchArray[0];
                state = item->State();
                if ( state == CVcxHgMyVideosVideoData::EVideoDataStateNone )
                    {
                    // Try first a quick peek with thumbnail creation denied
                    TRAP( err, StartThumbnailL( *item, ETrue ) );
                    if( err == KErrNone )
                        {
                        ++reqs;
                        startRefreshTimer = ETrue;
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
                            startRefreshTimer = ETrue;
                            }
                        }
                    }
                else
                    {
                    break; // Nothing to be started
                    }
                if ( err != KErrNone )
                    {
                    RemoveItem( 0 );
                    }                
                }
            while ( iFetchArray.Count() > 0 && reqs < KMaxThumbnailReqs && prevItem != item );
            }
        if ( startRefreshTimer )
            {
            iRefreshTimer->Cancel();
            iRefreshTimer->Start( KRefreshTimerInterval, KRefreshTimerInterval,
                TCallBack( RefreshTimerCallBack, this ) );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::UpdateVideoDataToUiL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::UpdateVideoDataToUiL( CVcxHgMyVideosVideoData& aVideoData )
    {
    TInt index = iVideoArray.IndexByMPXItemId( aVideoData.MPXItemId() );
    
    if ( index >= 0 && index < iScroller.ItemCount() )
        {       
        TBool drmUpdate = aVideoData.DrmProtected();
        CHgItem& listItem = iScroller.ItemL( index );
                
        if ( aVideoData.Thumbnail() )
            {
            CGulIcon* thumbnail = CGulIcon::NewL( aVideoData.Thumbnail( ETrue ) );
            listItem.SetIcon( thumbnail ); 
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
void CVcxHgMyVideosVideoDataUpdater::SelectNextIndexL( TBool aSelectForPeekOnly )
    {   
    TInt firstIndexOnScreen = iScroller.FirstIndexOnScreen();
    
    if ( firstIndexOnScreen < 0 ) 
        {
        firstIndexOnScreen = 0;
        }
    
    TInt lastIndexOnScreen = firstIndexOnScreen + iScroller.ItemsOnScreen();
   
    if ( lastIndexOnScreen >= iScroller.ItemCount() )
        {
        lastIndexOnScreen = iScroller.ItemCount() - 1;
        }
         
    // If visible items not found, updater takes object from 0 index. 
    for ( TInt i = firstIndexOnScreen; i <= lastIndexOnScreen; i++ )
        {      
        TInt index( KErrNotFound );
        TMPXItemId mpxItemId;
        CGulIcon* icon( NULL );
        
        // Skip if list item already have a thumbnail.
        icon = iScroller.ItemL( i ).Icon();
        if ( !icon )
            {
            mpxItemId = iVideoArray.ArrayIndexToMpxItemIdL( i );
            index = IndexByMPXItemId( mpxItemId );

            if ( index >= 0 )
                {
                CVcxHgMyVideosVideoData* item = iFetchArray[index];
                CVcxHgMyVideosVideoData::TVideoDataState state = item->State();
                // Move selected index to first index of the fetch array. 
                if ( aSelectForPeekOnly )
                    {
                    if ( state == CVcxHgMyVideosVideoData::EVideoDataStateNone )
                        {
                        iFetchArray.InsertL( item, 0 );
                        iFetchArray.Remove( index + 1 );
                        break;
                        }
                    }
                else if ( state == CVcxHgMyVideosVideoData::EVideoDataStateNone ||
                          state == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailPeekFinished )
                    {
                    iFetchArray.InsertL( item, 0 );
                    iFetchArray.Remove( index + 1 );
                    break;
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RefreshTimerCallBack()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoDataUpdater::RefreshTimerCallBack( TAny* aAny )
    {
    CVcxHgMyVideosVideoDataUpdater* self = static_cast<CVcxHgMyVideosVideoDataUpdater*>( aAny ); 
    self->iRefreshTimer->Cancel();
    if ( self->iListRefreshIsDelayed )
        {
        self->iListRefreshIsDelayed = EFalse;
        if ( !self->iPaused )
            {
            self->RefreshScreen();
            }
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
    TBool modifiedIndexOnScreen( EFalse );
    TInt firstIndexOnScreen( iScroller.FirstIndexOnScreen() );
    
    if ( firstIndexOnScreen < 0 )
        {
        firstIndexOnScreen = 0;
        }
    
    TInt lastIndexOnScreen = firstIndexOnScreen + iScroller.ItemsOnScreen(); 
    
    if ( lastIndexOnScreen >= iScroller.ItemCount() )
        {
        lastIndexOnScreen = iScroller.ItemCount() - 1;
        }
         
    if ( aIndex >= firstIndexOnScreen && aIndex <= lastIndexOnScreen )
        {
        modifiedIndexOnScreen = ETrue;
        }
    
    TBool timerHasExpired( ! iRefreshTimer->IsActive() );
    TBool refreshNeeded( EFalse );
    
    if ( ( iListRefreshIsDelayed && !modifiedIndexOnScreen ) 
            || iFetchArray.Count() <= 1 || timerHasExpired )
        {
        iListRefreshIsDelayed = EFalse;
        refreshNeeded = ETrue;        
        }        
    else if ( modifiedIndexOnScreen )
        {
        iListRefreshIsDelayed = ETrue;    
        }
    
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
                }

			// Start DRM check if thumb finished
            if ( item->State() == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailFinished && 
			     !IsActive() )
                {
                SetActive();
                TRequestStatus* stat = &iStatus;
                User::RequestComplete( stat, KErrNone );
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
        TInt i = 0;
        while( i < iFetchArray.Count() )
            {
            CVcxHgMyVideosVideoData* item = iFetchArray[i];
            if ( item->State() == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailFinished )
                {
                TRAP_IGNORE(
                    {
                    CheckDrmL( *item );
                    UpdateVideoDataToUiL( *item );
                    } );
                delete iFetchArray[i];
                iFetchArray.Remove(i);
                }
            else
                {
                ++i;
                }
            }
        ContinueVideoDataFetchingL();
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
            // Use shared file handle to minimize thumbnailserver overhead 
            RFs& fs = iModel.FileServerSessionL();
            RFile64 file;
            User::LeaveIfError( file.Open( fs, uri, EFileShareReadersOrWriters ));
            CleanupClosePushL( file );
            CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC(
                    file, mime );
            id = iModel.ThumbnailManager().GetL( *source );
            CleanupStack::PopAndDestroy( source ); 
            CleanupStack::PopAndDestroy( &file );
            }
        aItem.SetThumbnailConversionId( id );

        IPTVLOGSTRING3_LOW_LEVEL( 
                "MPX My Videos UI # GetThumbnailL() called thumbID %d for %S.",
                aItem.ThumbnailConversionId(),
                &media->ValueText( KMPXMediaGeneralUri ) );

        aItem.SetState( aPeek ?
                CVcxHgMyVideosVideoData::EVideoDataStateThumbnailPeekStarted :
                CVcxHgMyVideosVideoData::EVideoDataStateThumbnailStarted );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }
