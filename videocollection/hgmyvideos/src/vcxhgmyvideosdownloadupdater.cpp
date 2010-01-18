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




// INCLUDE FILES
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>

#include "IptvDebug.h"
#include <vcxmyvideosdefs.h>

#include "vcxhgmyvideosdownloadupdater.h"
#include "vcxhgmyvideosvideolist.h"
#include "vcxhgmyvideosvideomodelhandler.h"
#include "vcxhgmyvideospanics.h"

// CONSTANTS
const TInt KVcxHgUpdateDelay = 2500000; // 2,5 seconds

// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::CVcxHgMyVideosDownloadUpdater()
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosDownloadUpdater::CVcxHgMyVideosDownloadUpdater(
        CVcxHgMyVideosVideoModelHandler& aParent,
        CVcxHgMyVideosVideoList& aVideoArray )
 :  iParent( aParent ),
    iVideoArray( aVideoArray )
    {
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::ConstructL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadUpdater::ConstructL()
    {
    iTimer = CPeriodic::NewL( EPriorityNormal );
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::NewL()
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosDownloadUpdater* CVcxHgMyVideosDownloadUpdater::NewL(
        CVcxHgMyVideosVideoModelHandler& aParent,
        CVcxHgMyVideosVideoList& aVideoArray )
    {
    CVcxHgMyVideosDownloadUpdater* self = 
        new( ELeave ) CVcxHgMyVideosDownloadUpdater( aParent, aVideoArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::~CVcxHgMyVideosDownloadUpdater()
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosDownloadUpdater::~CVcxHgMyVideosDownloadUpdater()
    {
    if( iTimer )
        {
        iTimer->Cancel();
        }
    delete iTimer;
    iDownloadArray.Close();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::VideoArrayChangedL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosDownloadUpdater::VideoArrayChangedL()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosDownloadUpdater::VideoArrayChangedL()" );

    ResetDownloadsToFollowL();
    ContinueToFollowDownloads();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::VideoModifiedL()
// 
// Note: Currently expects and handles only aEventType == EMPXItemModified.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosDownloadUpdater::VideoModifiedL( TMPXChangeEventType /*aEventType*/,
                                                    TMPXItemId aMpxItemId,
                                                    TInt32 /*aExtraInfo*/ )
    {
    CMPXMedia* media = NULL;

    // If object is on download array, check if download has completed.
    for ( TInt i = 0; i < iDownloadArray.Count(); i++ )
        {
        media = iDownloadArray[i].iMedia;

        if ( media->IsSupported( KMPXMediaGeneralId ) )
            {
            if ( aMpxItemId == media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) )
                {
                TBool remove( EFalse );

                if ( ! media->IsSupported( KVcxMediaMyVideosDownloadId ) ||
                     ! media->IsSupported( KVcxMediaMyVideosDownloadState ) )
                    {
                    remove = ETrue;
                    }
                else if ( *( media->Value<TUint32>( KVcxMediaMyVideosDownloadId ) ) == 0 )
                    {
                    remove = ETrue;
                    }
                else
                    {
                    TVcxMyVideosDownloadState state = static_cast<TVcxMyVideosDownloadState>(
                        *( media->Value<TUint8>( KVcxMediaMyVideosDownloadState ) ) );

                    if ( state == EVcxMyVideosDlStateNone ||
                         state == EVcxMyVideosDlStateDownloaded )
                        {
                        remove = ETrue;
                        }
                    }

                if ( remove )
                    {
                    iDownloadArray.Remove( i );
                    }

                return;
                }
            }
        }

    // If object was not on download array, check if it should be added.
    media = iVideoArray.MPXMediaByMPXItemId( aMpxItemId );

    if ( media && FindDownload( media ) == KErrNotFound )
        {
        if ( media->IsSupported( KVcxMediaMyVideosDownloadId ) &&
             media->IsSupported( KVcxMediaMyVideosDownloadState ) )
            {
            // Download ID is non-zero if download status exists.
            if ( *( media->Value<TUint32>( KVcxMediaMyVideosDownloadId ) ) != 0 )
                {
                TVcxMyVideosDownloadState state( EVcxMyVideosDlStateNone );

                state = static_cast<TVcxMyVideosDownloadState>(
                    *( media->Value<TUint8>( KVcxMediaMyVideosDownloadState ) ) );
    
                if ( state == EVcxMyVideosDlStateDownloading ||
                     state == EVcxMyVideosDlStateFailed ||
                     state == EVcxMyVideosDlStatePaused )
                    {
                    TDownloadItem item;
                    item.iMedia = media;
                    item.iProgress = 0;
                    item.iState = state;
                    item.iProgressivePlayQueried = EFalse;
                    item.iIsPlayPossible = EFalse;
                    iDownloadArray.AppendL( item );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::IsPlayPossible()
// ---------------------------------------------------------------------------
//
TBool CVcxHgMyVideosDownloadUpdater::IsPlayPossible( TInt aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # CVcxHgMyVideosDownloadUpdater::IsPlayPossible" );

    TInt dlIndex( KErrNotFound );
    CMPXMedia* media = iVideoArray.MPXMedia( aIndex );
    
    if ( ! media )
        {
        return EFalse;
        }

    TInt count( iDownloadArray.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        // Compare pointers as they are the same in this case, and fast to use.
        if ( media == iDownloadArray[i].iMedia )
            {
            dlIndex = i;
            break;
            }
        }

    if ( dlIndex != KErrNotFound )
        {
        // These two lines make sure that class starts to follow availability
        // of progressive play, if it is not being followed yet.
        iDownloadArray[dlIndex].iProgressivePlayQueried = ETrue;
        ContinueToFollowDownloads();       

        iDownloadArray[dlIndex].iIsPlayPossible =
                ( iDownloadArray[dlIndex].iMedia->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress ) > 0 &&
                iDownloadArray[dlIndex].iMedia->ValueText( KMPXMediaGeneralUri ).Length() > 0 );

        // Return the response immediately, as it is needed from use cases like
        // DynInitMenuPaneL(). Real status will be updated
        // to UI asynchronously when background timer has checked the playability.
        return iDownloadArray[dlIndex].iIsPlayPossible;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::SetPausedL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadUpdater::SetPausedL( TBool aPaused )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosDownloadUpdater::SetPausedL(%d)", aPaused );
    
    iPaused = aPaused;

    ContinueToFollowDownloads();
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::RemoveDownload()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadUpdater::RemoveDownload( TMPXItemId aMPXItemId )
    {    
    CMPXMedia* media( NULL );
    media = iVideoArray.MPXMediaByMPXItemId( aMPXItemId );
    
    if ( media )
    	{
    	TInt index( KErrNotFound );
		index = FindDownload( media );
    	
		if ( index != KErrNotFound )
    	    {
    	    iDownloadArray.Remove( index );
    	    }
    	    
        if ( iDownloadArray.Count() < 1 )
    	    {
    	    iTimer->Cancel();        
    	    }
    	}
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::FindDownload()
// ---------------------------------------------------------------------------
//
TInt CVcxHgMyVideosDownloadUpdater::FindDownload( CMPXMedia* aMedia )
    {
    TInt index( KErrNotFound );
    
    if ( aMedia )
    	{  
        for ( TInt i = iDownloadArray.Count() - 1; i >= 0; i-- )
            {
            if ( iDownloadArray[i].iMedia->Match( *aMedia, KMPXMediaGeneralId ) )
                {
                index = i;
                break;
                }
            }
    	}
    return index;
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::ContinueToFollowDownloads()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadUpdater::ContinueToFollowDownloads()
    {
    if ( iPaused || iDownloadArray.Count() == 0 )
        {
        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        }
    else
        {
        if ( ! iTimer->IsActive() )
            {
            TCallBack callback = TCallBack( Callback, this );
            const TTimeIntervalMicroSeconds32 KUpdateInterval( KVcxHgUpdateDelay );
            iTimer->Start( KUpdateInterval, KUpdateInterval, callback );
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::ResetDownloadsToFollowL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadUpdater::ResetDownloadsToFollowL()
    {
    iDownloadArray.Reset();

    TVcxMyVideosDownloadState state( EVcxMyVideosDlStateNone );
    TDownloadItem item;
    CMPXMedia* media = NULL;
    TInt count( iVideoArray.VideoCount() );

    for ( TInt i = 0; i < count; i++ )
        {
        media = iVideoArray.MPXMedia( i );

        if ( media &&
             media->IsSupported( KVcxMediaMyVideosDownloadId ) &&
             media->IsSupported( KVcxMediaMyVideosDownloadState ) )
            {
            // Download ID is non-zero if download status exists.
            if ( *( media->Value<TUint32>( KVcxMediaMyVideosDownloadId ) ) != 0 )
                {
                state = static_cast<TVcxMyVideosDownloadState>(
                    *( media->Value<TUint8>( KVcxMediaMyVideosDownloadState ) ) );

                if ( state == EVcxMyVideosDlStateDownloading ||
                     state == EVcxMyVideosDlStateFailed ||
                     state == EVcxMyVideosDlStatePaused )
                    {
                    item.iMedia = media;
                    item.iProgress = 0;
                    item.iState = state;
                    item.iProgressivePlayQueried = EFalse;
                    item.iIsPlayPossible = EFalse;
                    iDownloadArray.AppendL( item );

                    IPTVLOGSTRING3_LOW_LEVEL( 
                        "MPX My Videos UI # Download to follow: Id:%d, State:%d", 
                        static_cast<TInt>( 
                            media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ),
                        static_cast<TInt>(
                            media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState ) ) );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::Callback()
// ---------------------------------------------------------------------------
//
TInt CVcxHgMyVideosDownloadUpdater::Callback( TAny *aPtr )
    {
    CVcxHgMyVideosDownloadUpdater* ptr = static_cast<CVcxHgMyVideosDownloadUpdater*>( aPtr );
    TRAP_IGNORE( ptr->UpdateDownloadsL() ); 
    return 0;
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::UpdateDownloadsL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadUpdater::UpdateDownloadsL()
    {
    for ( TInt i = 0; i < iDownloadArray.Count(); i++ )
        {
        UpdateDownloadL( iDownloadArray[i] );
        }
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosDownloadUpdater::UpdateDownloadL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadUpdater::UpdateDownloadL( TDownloadItem& aItem )
    {
    TBool itemChanged( EFalse );

    // Check download progress.
    TVcxMyVideosDownloadState state = static_cast<TVcxMyVideosDownloadState>(
        aItem.iMedia->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState ) );
    TInt8 progress = aItem.iMedia->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress );

    if ( progress == 0 && aItem.iProgress > 0 )
        {
        IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # UpdateDownloadL() Resume started from beginning." );
        TUint32 mpxId = aItem.iMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1;
        iParent.ResumeStartedFromBeginningL( mpxId );
        }
    if ( state != aItem.iState || progress != aItem.iProgress )
        {
        aItem.iState = state;
        aItem.iProgress = progress;
        itemChanged = ETrue;
        }

    // Progressive playback check is needed in two cases:
    // 1) It has been queried by user and old status is EFalse, in this case the status
    //    has not been check ever yet.
    // 2) Progress has changed, and old status is EFalse. In this case we might have
    //    received enough data to support progressive play.
    if ( ( aItem.iProgressivePlayQueried && ! aItem.iIsPlayPossible ) ||
         ( itemChanged && ! aItem.iIsPlayPossible ) )
        {
        aItem.iProgressivePlayQueried = EFalse;

	    if ( aItem.iMedia->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress ) > 0 &&
	         aItem.iMedia->ValueText( KMPXMediaGeneralUri ).Length() > 0 ) 
	        {
	        aItem.iIsPlayPossible = ETrue;
	        }

        if ( aItem.iIsPlayPossible )
            {
            itemChanged = ETrue;
            }
        }

    IPTVLOGSTRING4_LOW_LEVEL( 
            "MPX My Videos UI # UpdateDownloadL() Id:%d State:%d Progress:%d",
            static_cast<TInt>( aItem.iMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ),
            static_cast<TInt>( state ), static_cast<TInt>( progress ) );     
    IPTVLOGSTRING3_LOW_LEVEL( 
            "MPX My Videos UI # UpdateDownloadL() Path:%S Progressive Play:%d",
            &( aItem.iMedia->ValueText( KMPXMediaGeneralUri ) ),
            static_cast<TInt>( aItem.iIsPlayPossible ) ); 

    if ( itemChanged )
        {
        iParent.VideoModifiedL( 
            EMPXItemModified, 
            aItem.iMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ),
            EVcxMyVideosListNoInfo,
            ETrue ); // Mark event as simulated, not originated from MPX Collection.
        }
    }
