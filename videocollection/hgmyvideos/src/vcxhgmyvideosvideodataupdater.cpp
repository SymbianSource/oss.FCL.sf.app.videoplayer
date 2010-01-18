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

const TInt KRefreshTimerInterval( 1000000 ); // 1 second

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
    if ( iRefreshTimer )
	    {
	    // Calling cancel without checking if the timer is active is safe.        
        iRefreshTimer->Cancel();
        delete iRefreshTimer;
		}
    
    CancelAndDeleteFetchArray();
    delete iTnEngine;
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
        CVcxHgMyVideosVideoData::TVideoDataState state = iFetchArray[aIndex]->State();
    
        if ( state == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailStarted )
            {
            // Cancel thumbnail generation. Safe to ignore leave, as the ThumbnailManagerL 
			// can only leave if iTnEngine is not created yet, so there cannot be 
			// any outstanding requests either. 
            TRAP_IGNORE( ThumbnailManagerL()->CancelRequest( 
                    iFetchArray[aIndex]->ThumbnailConversionId() ) );
            }
        else if ( state == CVcxHgMyVideosVideoData::EVideoDataStateDrmStarted )
            {
            // Cancel DRM checking.
            Cancel();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::CancelAndDeleteFetchArray()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::CancelAndDeleteFetchArray()
    {    
    for ( TInt i = 0; i < iFetchArray.Count(); i++ )
        {
        RemoveItem( i );
        }
    iFetchArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::ContinueVideoDataFetchingL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::ContinueVideoDataFetchingL()
    {
    if ( iFetchArray.Count() >= 1 && ! iPaused && iVideoArray.VideoCount() > 0 )
        {
        TBool go = EFalse;

        // If first item is in idle state, we need to start new operation.
        if ( iFetchArray[0]->State() == CVcxHgMyVideosVideoData::EVideoDataStateNone )
            {
            // It's safe to ignore leave here, because in that case we just use the first index
			// of fetch array.
			TRAP_IGNORE( SelectNextIndexL() );
            go = ETrue;
            }   
        // If thumbnail generation has finished, start DRM checking.
        else if ( iFetchArray[0]->State() == CVcxHgMyVideosVideoData::EVideoDataStateThumbnailFinished )
            {
            go = ETrue;
            }
        // If first item has completed all operations, remove it and start with second one.
        else if ( iFetchArray[0]->State() == CVcxHgMyVideosVideoData::EVideoDataStateDrmFinished )
            {
            UpdateVideoDataToUiL( *( iFetchArray[0] ) );
            RemoveItem( 0 );            
			
            if ( iFetchArray.Count() >= 1 )
                {
                TRAP_IGNORE( SelectNextIndexL() );
                go = ETrue;
                }
            }

        if ( go )
            {
            if ( iFetchArray[0]->State() == CVcxHgMyVideosVideoData::EVideoDataStateNone )
                {
                CMPXMedia* media = iVideoArray.MPXMediaByMPXItemId( iFetchArray[0]->MPXItemId() );

                if ( media && media->IsSupported( KMPXMediaGeneralUri ) )
                    {
                    HBufC* mimeType = NULL;
                    if ( media->IsSupported( KMPXMediaGeneralMimeType ) )
                        {
                        mimeType = media->ValueText( KMPXMediaGeneralMimeType ).AllocLC();
                        }
                    else
                        {
                        mimeType = HBufC::NewLC( 1 );
                        }

                    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC( 
                        media->ValueText( KMPXMediaGeneralUri ), 
                        *mimeType );
                    iFetchArray[0]->SetThumbnailConversionId( 
                        ThumbnailManagerL()->GetThumbnailL( *source ) );
                    CleanupStack::PopAndDestroy( source );
                    CleanupStack::PopAndDestroy( mimeType );

                    IPTVLOGSTRING3_LOW_LEVEL( 
                            "MPX My Videos UI # GetThumbnailL() called thumbID %d for %S.",
                            iFetchArray[0]->ThumbnailConversionId(),
                            &media->ValueText( KMPXMediaGeneralUri ) );

                    iRefreshTimer->Cancel();
                    iRefreshTimer->Start( KRefreshTimerInterval, 
                                          KRefreshTimerInterval, 
                                          TCallBack( RefreshTimerCallBack, this ) );
                    
                    iFetchArray[0]->SetState(
                        CVcxHgMyVideosVideoData::EVideoDataStateThumbnailStarted );
                    }
                }
            else if ( iFetchArray[0]->State() == 
                      CVcxHgMyVideosVideoData::EVideoDataStateThumbnailFinished )
                {
                if ( ! IsActive() )
                    {
                    SetActive();
                    iStatus = KRequestPending;
                    TRequestStatus* stat = &iStatus;
                    User::RequestComplete(stat, KErrNone);

                    iFetchArray[0]->SetState(
                        CVcxHgMyVideosVideoData::EVideoDataStateDrmStarted );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::UpdateVideoDataToUiL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::UpdateVideoDataToUiL( CVcxHgMyVideosVideoData& videoData )
    {
    TInt index = iVideoArray.IndexByMPXItemId( videoData.MPXItemId() );
    
    if ( index >= 0 && index < iScroller.ItemCount() )
        {       
        TBool drmUpdate = videoData.DrmProtected();
        CHgItem& listItem = iScroller.ItemL( index );
                
        if ( videoData.Thumbnail() )
            {
            CGulIcon* thumbnail = CGulIcon::NewL( videoData.Thumbnail( ETrue ) );
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

                if ( videoData.ValidDrmRights() )
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
    for ( TInt i = 0; i < iFetchArray.Count(); i++ )
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
void CVcxHgMyVideosVideoDataUpdater::SelectNextIndexL()
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
        
            if ( index > 0 )
                {
                // Move selected index to first index of the fetch array. 
                iFetchArray.Insert( iFetchArray[index], 0 );
                iFetchArray.Remove( index + 1 );
                break;
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
// CVcxHgMyVideosVideoDataUpdater::ThumbnailManagerL()
// -----------------------------------------------------------------------------
//
CThumbnailManager* CVcxHgMyVideosVideoDataUpdater::ThumbnailManagerL()
    {
    if ( !iTnEngine )
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # CVcxHgMyVideosVideoDataUpdater::ThumbnailManagerL: Creating thumbnail manager." );
        
        iTnEngine = CThumbnailManager::NewL( *this );
        iTnEngine->SetThumbnailSizeL( EVideoListThumbnailSize );
        iTnEngine->SetDisplayModeL( EColor16M );
        }
    
    return iTnEngine;
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

    if ( iFetchArray.Count() > 0 )
        {
        if ( aError == KErrNone 
             && aId == iFetchArray[0]->ThumbnailConversionId() )
            {
            // Never delete this, ownership gone to Ganes list
            iFetchArray[0]->SetThumbnail( aThumbnail.DetachBitmap() );
            }
        else
            {
            iFetchArray[0]->SetThumbnail( NULL );
            }

        iFetchArray[0]->SetState( CVcxHgMyVideosVideoData::EVideoDataStateThumbnailFinished );
        }

    TRAP_IGNORE( ContinueVideoDataFetchingL() );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoDataUpdater::RunL()
// From CActive
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoDataUpdater::RunL()
    {
    if ( iFetchArray.Count() > 0 )
        {
        CMPXMedia* media = iVideoArray.MPXMediaByMPXItemId( iFetchArray[0]->MPXItemId() );
        
        if ( media )
            {
            if ( media->IsSupported( KMPXMediaGeneralUri ) )
                {
                TUint32 flags = 0;
                if ( media->IsSupported( KMPXMediaGeneralFlags ) )
                    {
                    flags = media->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
                    }

                if ( flags & EVcxMyVideosVideoDrmProtected )
                    {
                    ContentAccess::CData* cData = NULL;

                    iFetchArray[0]->SetDrmProtected( ETrue );
                    iFetchArray[0]->SetValidDrmRights( EFalse );

                    TRAPD( trapError,
                           cData = CData::NewL( 
                                       (TVirtualPathPtr) media->ValueText( KMPXMediaGeneralUri ),
                                       EPeek,
                                       EContentShareReadWrite ) );                    

                    if ( cData )
                        {
                        if ( trapError == KErrNone )
                            {
                            TInt intentResult = cData->EvaluateIntent( ContentAccess::EPlay );

                            // Not valid rights should return KErrCANoRights, KErrCANoPermission,
                            // or in rare cases KErrCAPendingRights. But we don't trust those and
                            // just compare against KErrNone.
                            if ( intentResult == KErrNone )
                                {
                                iFetchArray[0]->SetValidDrmRights( ETrue );
                                }
                            }
                        delete cData;
                        }
                    }
                }
            }

        iFetchArray[0]->SetState( CVcxHgMyVideosVideoData::EVideoDataStateDrmFinished );
        }

    ContinueVideoDataFetchingL();
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
        // Remove current item from fetch array
        RemoveItem( 0 );            
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
