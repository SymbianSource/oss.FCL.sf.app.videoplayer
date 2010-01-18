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




// INCLUDE FILES
#include <StringLoader.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>

#include <ganes/HgScroller.h>
#include <ganes/HgItem.h>

#include <vcxhgmyvideos.rsg>
#include <vcxmyvideosdefs.h>
#include <MPFileDetailsDialog.h>
#include <myvideosindicator.h>
#include "IptvDebug.h"

#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideosmainview.h"
#include "vcxhgmyvideoscollectionclient.h"
#include "vcxhgmyvideosdownloadclient.h"
#include "vcxhgmyvideosvideomodelhandler.h"
#include "vcxhgmyvideosvideolistimpl.h"
#include "vcxhgmyvideosvideodataupdater.h"
#include "vcxhgmyvideosdownloadupdater.h"

#include "vcxhgmyvideos.hrh"
#include "vcxhgmyvideoslistbase.h"
#include "vcxhgmyvideosindicatorhelper.h"
#include "vcxhgmyvideosvideodetailsdialog.h"
#include "vcxhgmyvideosvideolist.h"
#include "vcxhgmyvideospanics.h"


#include <videoplayercustommessage.h>

// CONSTANTS
const TUint KVcxSecondsInMinute( 60 );
const TUint KVcxSecondsInHour( 3600 );
const TUint KVcxVideoSizeGB( 0x40000000 );
const TUint KVcxVideoSizeHalfGB( 0x20000000 );
const TUint KVcxVideoSizeMB( 0x100000 );
const TUint KVcxVideoSizeHalfMB( 0x80000 );
const TUint KVcxVideoSizeKB( 0x400 );
const TUint KVcxVideoSizeHalfKB( 0x200 ); 

const TUint KHgBufferSize( 250 );
const TUint KHgBufferTreshold( 50 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoModelHandler* CVcxHgMyVideosVideoModelHandler::NewL( 
        CVcxHgMyVideosVideoListImpl& aVideoListImpl,
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )        
    {
    CVcxHgMyVideosVideoModelHandler* self = 
        CVcxHgMyVideosVideoModelHandler::NewLC( 
            aVideoListImpl, aModel, aView, aScroller );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoModelHandler* CVcxHgMyVideosVideoModelHandler::NewLC(
        CVcxHgMyVideosVideoListImpl& aVideoListImpl,
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )        
    {
    CVcxHgMyVideosVideoModelHandler* self = 
        new (ELeave) CVcxHgMyVideosVideoModelHandler( 
            aVideoListImpl, aModel, aView, aScroller );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoModelHandler::CVcxHgMyVideosVideoModelHandler(
        CVcxHgMyVideosVideoListImpl& aVideoListImpl,
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
    : iVideoListImpl( aVideoListImpl ),
      iModel( aModel ),
      iView( aView ),
      iScroller( aScroller ),
      iCurrentCategoryIndex( KErrNotFound ),
      iRestoreHighlightPosition( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ConstructL()
    {
    iVideoArray = CVcxHgMyVideosVideoList::NewL();

    iDataUpdater = CVcxHgMyVideosVideoDataUpdater::NewL( iModel,
                                                         iScroller,                                                        
                                                         *iVideoArray );

    iDownloadUpdater = CVcxHgMyVideosDownloadUpdater::NewL( *this, *iVideoArray );
    
    iVideoIndicator = CMyVideosIndicator::NewL();
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoModelHandler::~CVcxHgMyVideosVideoModelHandler()
    {
    iResumeArray.Close();
    
    delete iVideoIndicator;
    delete iDataUpdater;
    delete iDownloadUpdater;
    delete iVideoArray;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::DoModelActivateL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::DoModelActivateL()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosVideoModelHandler::DoModelActivateL() - Enter" );
    
    iDataUpdater->SetPausedL( EFalse );
    iDownloadUpdater->SetPausedL( EFalse );
    iModel.CollectionClient().SetVideoModelObserver( this );

    // Set scroller strip type
    TVcxMyVideosSortingOrder sortOrder = iModel.VideolistSortOrderL();
    UpdateScrollbarTypeL( sortOrder );

    // Set up scroll buffer
    iScroller.EnableScrollBufferL( *this, KHgBufferSize, KHgBufferTreshold ); 
    
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosVideoModelHandler::DoModelActivateL() - Exit" );    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::DoModelDeactivate()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::DoModelDeactivate()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosVideoModelHandler::DoModelDeactivate() - Enter" );
    
    TRAP_IGNORE( iDataUpdater->SetPausedL( ETrue ) );
    TRAP_IGNORE( iDownloadUpdater->SetPausedL( ETrue ) );
    iModel.CollectionClient().SetVideoModelObserver( NULL );
    iScroller.DisableScrollBuffer();
    
    TRAP_IGNORE( SaveHighlightedItemIdL() );
    
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosVideoModelHandler::DoModelDeactivate() - Exit" );    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::SaveHighlightedItemIdL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::SaveHighlightedItemIdL()
    {    
    iHighlightedItemId = iVideoArray->ArrayIndexToMpxItemIdL( Highlight() );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::UpdateVideoListL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::UpdateVideoListL( TInt aCategoryIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosVideoModelHandler::UpdateVideoListL() - Enter" );

    CVcxHgMyVideosCollectionClient& collectionClient = iModel.CollectionClient();

    // If we are re-opening the same video list again, then try 
    // restore the highlight to previous position.
    aCategoryIndex == iCurrentCategoryIndex ? iRestoreHighlightPosition = ETrue :
                                              iRestoreHighlightPosition = EFalse;
    // Removes videos from video list.
    iVideoArray->RemoveVideoList();
         
    // Removes videos from scroller. After this command, list highlight 
    // disappears, so don't forget to set highlight in NewVideoListL() 
    // function.
    iScroller.Reset();
        
    // Ask for video list from MPX collection.
    collectionClient.GetVideoListL( aCategoryIndex );

    iCurrentCategoryIndex = aCategoryIndex;
    
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosVideoModelHandler::UpdateVideoListL() - Exit" );    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ResortVideoListL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ResortVideoListL()
    {    
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # ResortVideoListL() - Enter" );
        	
    if ( iScroller.ItemCount() > 0 )
        {
        iDownloadUpdater->SetPausedL( ETrue );
    	
        TVcxMyVideosSortingOrder sortOrder = iModel.VideolistSortOrderL();
        iVideoArray->ResortVideoListL( sortOrder );
        
        TInt highlight = iScroller.SelectedIndex();
        
        // It is enough to reset scroll buffer only, not whole scroller
        iScroller.DisableScrollBuffer();
        iScroller.EnableScrollBufferL( *this, KHgBufferSize, KHgBufferTreshold );
        
        iScroller.SetSelectedIndex( highlight );
        
        iView.DynInitMskL();
    	
        iDownloadUpdater->VideoArrayChangedL();
        iDownloadUpdater->SetPausedL( EFalse );

        // Switch to appropriate scroller strip
        UpdateScrollbarTypeL( sortOrder );
        }
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # ResortVideoListL() - Exit" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::Highlight()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::Highlight()
    {
    return iScroller.SelectedIndex();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoCount()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::VideoCount()
    {
    return iScroller.ItemCount();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::CurrentCategoryIndex() 
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::CurrentCategoryIndex()
    {
    return iCurrentCategoryIndex;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::MarkedVideosL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::MarkedVideosL( 
        RArray<TInt>& aMarkedVideos )
    {
    iScroller.GetMarkedItemsL( aMarkedVideos );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::OngoingDownloads()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::OngoingDownloads(
        RArray<TInt>& aDownloads )
    {
    iVideoArray->GetOngoingDownloads( aDownloads );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoDownloadState()
// -----------------------------------------------------------------------------
//
TVcxMyVideosDownloadState CVcxHgMyVideosVideoModelHandler::VideoDownloadState( 
        TInt aIndex )
    {
    return iVideoArray->VideoDownloadState( aIndex );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::IsProgressivePlayPossible()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoModelHandler::IsProgressivePlayPossible( TInt aIndex )
    {
    return iDownloadUpdater->IsPlayPossible( aIndex );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::HandleMarkCommandL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::HandleMarkCommandL( TInt aMarkCommand )
    {
    switch ( aMarkCommand )
        {
        case EVcxHgMyVideosCmdMark:
            {
            TInt highlight = Highlight();
            iScroller.Mark( highlight );
            iScroller.RefreshScreen( highlight );
            }
            break;
        case EVcxHgMyVideosCmdUnmark:
            {
            TInt highlight = Highlight();
            iScroller.UnMark( highlight );
            iScroller.RefreshScreen( highlight );
            }            
            break;
        case EVcxHgMyVideosCmdMarkAll:
            {
            iScroller.MarkAll();
            }
            break;
        case EVcxHgMyVideosCmdUnmarkAll:
            {
            iScroller.UnMarkAll();
            }
            break;
        default:
            {
            IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # HandleMarkCommandL - LEAVE: Invalid mark command" );
            User::Leave( KErrNotSupported );
            }
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::GetVideoPlayerCustomMessage()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::GetVideoPlayerCustomMessage(
        TInt aIndex, TVideoPlayerCustomMessage& aVideoInfo, TInt& aMpxId1 )
    {
    CMPXMedia* media = iVideoArray->MPXMedia( aIndex );

    if ( ( ! media ) || ( ! media->IsSupported( KMPXMediaGeneralId ) ) )
        {
        return KErrNotFound;
        }
    
    TMPXItemId mpxItemId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
    
    if ( GetVideoPlayerCustomMessage( mpxItemId, aVideoInfo ) != KErrNone )
        {
        return KErrNotFound;
        }
		
    aMpxId1 = ( media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ).iId1;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::GetVideoPlayerCustomMessage()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::GetVideoPlayerCustomMessage(
        TMPXItemId& aMpxItemId, TVideoPlayerCustomMessage& aVideoInfo )
    {
    CMPXMedia* media = iVideoArray->MPXMediaByMPXItemId( aMpxItemId );

    if ( ! media )
        {
        return KErrNotFound;
        }

    if ( media->IsSupported( KMPXMediaGeneralUri ) )
        {
        aVideoInfo.iContent = media->ValueText( KMPXMediaGeneralUri );
        }

    if ( media->IsSupported( KMPXMediaGeneralTitle ) )
        {
        aVideoInfo.iName = media->ValueText( KMPXMediaGeneralTitle );
        }

    if ( media->IsSupported( KMPXMediaGeneralThumbnail1 ) )
        {
        aVideoInfo.iIcon = media->ValueText( KMPXMediaGeneralThumbnail1 );
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoAgeProfileL()
// -----------------------------------------------------------------------------
//
TInt32 CVcxHgMyVideosVideoModelHandler::VideoAgeProfileL( TInt aIndex )
    {
    return VideoAgeProfileL( iVideoArray->MPXMedia( aIndex ) );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoAgeProfileL()
// -----------------------------------------------------------------------------
//
TInt32 CVcxHgMyVideosVideoModelHandler::VideoAgeProfileL( TMPXItemId aId )
    {
    return VideoAgeProfileL( iVideoArray->MPXMediaByMPXItemId( aId ) );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoAgeProfileL()
// -----------------------------------------------------------------------------
//
TInt32 CVcxHgMyVideosVideoModelHandler::VideoAgeProfileL( CMPXMedia* aMedia )
    {
    TInt32 ageProfile( 0 );

    if ( aMedia )
        {
        if ( aMedia->IsSupported( KVcxMediaMyVideosAgeProfile ) )
            {
            ageProfile = aMedia->ValueTObjectL<TInt32>( KVcxMediaMyVideosAgeProfile );
            }
        }
    
    return ageProfile;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::FindVideoMpxIdL()
// Tries to find MPX Item Id based on URI. Does not modify MPX Item Id's initial
// value if video not found.
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoModelHandler::FindVideoMpxIdL( const TDesC& aVideoPath,
                                                        TMPXItemId& aMpxItemId )
    {
    CMPXMedia* media = iVideoArray->MPXMediaByUri( aVideoPath );
    if ( media )
        {
        if ( media->IsSupported( KMPXMediaGeneralId ) )
            {
            aMpxItemId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ClearNewVideoIndicatorL()
// Informative call about video being played. Method removes possible new indicator
// from video.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ClearNewVideoIndicatorL( TInt aIndex )
    {
    CMPXMedia* media = iVideoArray->MPXMedia( aIndex );

    if ( media )
        {
        if ( media->IsSupported( KMPXMediaGeneralFlags ) &&
             media->IsSupported( KMPXMediaGeneralId ) )
            {
            TUint32 flags = media->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
            if ( flags & EVcxMyVideosVideoNew )
                {
                flags &= ~EVcxMyVideosVideoNew;
                CVcxHgMyVideosCollectionClient& collectionClient = iModel.CollectionClient();
                collectionClient.SetAttributeL( *media, KMPXMediaGeneralFlags, flags );
                
                if ( media->IsSupported( KVcxMediaMyVideosOrigin ) && 
                     ( media->ValueTObjectL<TUint8>( KVcxMediaMyVideosOrigin ) == EVcxMyVideosOriginDownloaded ) )
                    {
                    iModel.ResetDownloadNotification();
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ClearNewVideoIndicatorL()
// Informative call about video being played. Method removes possible new indicator
// from video.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ClearNewVideoIndicatorL( TMPXItemId& aMpxItemId )
    {
    CMPXMedia* media = iVideoArray->MPXMediaByMPXItemId( aMpxItemId );

    if ( media )
        {
        if ( media->IsSupported( KMPXMediaGeneralFlags ) &&
             media->IsSupported( KMPXMediaGeneralId ) )
            {
            TUint32 flags = media->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
            if ( flags & EVcxMyVideosVideoNew )
                {
                flags &= ~EVcxMyVideosVideoNew;
                CVcxHgMyVideosCollectionClient& collectionClient = iModel.CollectionClient();
                collectionClient.SetAttributeL( *media, KMPXMediaGeneralFlags, flags );
                
                if ( media->IsSupported( KVcxMediaMyVideosOrigin ) && 
                     ( media->ValueTObjectL<TUint8>( KVcxMediaMyVideosOrigin ) == EVcxMyVideosOriginDownloaded ) )
                    {
                    iModel.ResetDownloadNotification();
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ShowVideoDetailsDialogL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ShowVideoDetailsDialogL()
    {
    TInt highlight = Highlight();
    
    if ( highlight != KErrNotFound )
        {
        TMPXItemId mpxItemId = iVideoArray->ArrayIndexToMpxItemIdL( highlight );
        iModel.CollectionClient().GetVideoDetailsL( mpxItemId );        
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CMyVideosIndicator& CVcxHgMyVideosVideoModelHandler::VideoIndicator()
    {
    return *iVideoIndicator;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::LastPlaybackPosition()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoModelHandler::LastPlaybackPosition( const TDesC& aContentUri )
    {
    TInt position = 0;
    
    if ( CIptvUtil::LastPlaybackPositionFeatureSupported() )
        {
        CMPXMedia* media = iVideoArray->MPXMediaByUri( aContentUri );

        if ( media )
            {
            if ( media->IsSupported( KMPXMediaGeneralLastPlaybackPosition ) )
                {
                position = *media->Value<TInt>( KMPXMediaGeneralLastPlaybackPosition );            
                }
            }
        IPTVLOGSTRING3_LOW_LEVEL( "MPX My Videos UI # Found position %d for clip %S", 
                                  position, &aContentUri );
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # LastPlaybackPosition() - Feature is not supported, position = 0" );
        }		

    return position;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::SetLastPlaybackPositionL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::SetLastPlaybackPositionL( const TDesC& aContentUri,
                                                                TInt aPosition )
    {
    if ( CIptvUtil::LastPlaybackPositionFeatureSupported() )
        {
        CMPXMedia* media = iVideoArray->MPXMediaByUri( aContentUri );

        // If media is not in our own list, it was launched from external source. In this
        // case the MPX Media object could be fetched on background to allow position storing.
        if ( ! media )
            {
            media = iModel.CollectionClient().GetLatestFetchedMpxMediaL();

            if ( media )
                {
                TBool match( EFalse );            
                if ( media->IsSupported( KMPXMediaGeneralUri ) )
                    {
                    if ( aContentUri.CompareF( media->ValueText( KMPXMediaGeneralUri ) ) == 0 )
                        {
                        match = ETrue;
                        }
                    }
                if ( ! match )
                    {
                    media = NULL;
                    }
                }
            }

        if ( media )
            {
            IPTVLOGSTRING3_LOW_LEVEL( "MPX My Videos UI # Storing position %d for clip %S", 
                                      aPosition, &aContentUri );

            // Change data type of last playback position?

            iModel.CollectionClient().SetAttributeL( *media, 
                                                     KMPXMediaGeneralLastPlaybackPosition, 
                                                     TUint32( aPosition ) );
            }
        iModel.UpdateLastWatchedPlayPositionL( aPosition );    
        }
    else
	    {
        IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # SetLastPlaybackPositionL() - Feature is not supported" ); 
        }    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::GetVideoName()
// -----------------------------------------------------------------------------
//
const TDesC& CVcxHgMyVideosVideoModelHandler::GetVideoName( TInt aIndex )
    {
    CMPXMedia* media = iVideoArray->MPXMedia( aIndex );

    if ( media && media->IsSupported( KMPXMediaGeneralTitle ) )
        {
        return media->ValueText( KMPXMediaGeneralTitle );
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::GetVideoUri()
// -----------------------------------------------------------------------------
//
const TDesC& CVcxHgMyVideosVideoModelHandler::GetVideoUri( TInt aIndex )
    {
    CMPXMedia* media = iVideoArray->MPXMedia( aIndex );

    if ( media && media->IsSupported( KMPXMediaGeneralUri ) )
        {
        return media->ValueText( KMPXMediaGeneralUri );
        }
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::GetVideoSize()
// -----------------------------------------------------------------------------
//
TInt64 CVcxHgMyVideosVideoModelHandler::GetVideoSize( TInt aIndex )
    {
    CMPXMedia* media = iVideoArray->MPXMedia( aIndex );

    if ( media && media->IsSupported( KMPXMediaGeneralExtSizeInt64 ) )
        {
        return media->ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 );
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::DeleteVideosL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::DeleteVideosL( 
        const RArray<TInt>& aOperationTargets )
    {
    CMPXMedia* media = NULL;
    TMPXItemId mpxItemId;
    CMPXMedia* mediaToDelete = NULL;
    CMPXMediaArray* mediasToDelete = CMPXMediaArray::NewL();
    CleanupStack::PushL( mediasToDelete );

    for ( TInt i = 0; i < aOperationTargets.Count(); i++ )
        {
        media = iVideoArray->MPXMedia( aOperationTargets[i] );

        if ( media && media->IsSupported( KMPXMediaGeneralId ) )
            {
            mpxItemId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            mediaToDelete = CMPXMedia::NewL();
            CleanupStack::PushL( mediaToDelete );
            mediaToDelete->SetTObjectValueL( KMPXMediaGeneralId, mpxItemId );
            mediasToDelete->AppendL( *mediaToDelete );
            CleanupStack::PopAndDestroy( mediaToDelete );
            }
        }

    TRAPD( err, iModel.CollectionClient().DeleteVideosL( mediasToDelete ) );
    	
    if ( err != KErrNone )
        {
        VideoDeletionCompletedL( aOperationTargets.Count(), mpxItemId );
		if ( err != KErrNotReady )
		    {
			User::Leave( err );
			}
        }
    CleanupStack::PopAndDestroy( mediasToDelete );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::MoveOrCopyVideosL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::MoveOrCopyVideosL( 
        const RArray<TInt>& aOperationTargets,
        TInt aTargetDrive,
        TBool aCopy )
    {
    CMPXMedia* media = NULL;
    TMPXItemId mpxItemId;
    CMPXMedia* mediaToMoveOrCopy = NULL;
    CMPXMediaArray* mediasToMoveOrCopy = CMPXMediaArray::NewL();
    CleanupStack::PushL( mediasToMoveOrCopy );

    for ( TInt i = 0; i < aOperationTargets.Count(); i++ )
        {
        media = iVideoArray->MPXMedia( aOperationTargets[i] );
                    
        if ( media && media->IsSupported( KMPXMediaGeneralId ) )
            {
            mpxItemId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            mediaToMoveOrCopy = CMPXMedia::NewL();
            CleanupStack::PushL( mediaToMoveOrCopy );
            mediaToMoveOrCopy->SetTObjectValueL( KMPXMediaGeneralId, mpxItemId );
            mediasToMoveOrCopy->AppendL( *mediaToMoveOrCopy );
            CleanupStack::PopAndDestroy( mediaToMoveOrCopy );
            }
        }

    iModel.CollectionClient().MoveOrCopyVideosL( mediasToMoveOrCopy, aTargetDrive, aCopy );
    CleanupStack::PopAndDestroy( mediasToMoveOrCopy );    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::PlayVideoL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::PlayVideoL( TInt aIndex )
    {
    CMPXMedia* media = iVideoArray->MPXMedia( aIndex );

    if ( media && media->IsSupported( KMPXMediaGeneralId ) )
        {
        // Playback should be tried always when single clicking a video in My Videos
        TMPXItemId mpxItemId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
        IPTVLOGSTRING3_LOW_LEVEL( "CVcxHgMyVideosVideoModelHandler::PlayVideoL() aIndex=%d mpxItemId=%d", aIndex, (TInt) mpxItemId );
        iModel.CollectionClient().PlayVideoL( mpxItemId );        
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ResumeDownloadL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::ResumeDownloadL( TInt aIndex )
    {
    CMPXMedia* media = iVideoArray->MPXMedia( aIndex );

    if ( media && media->IsSupported( KMPXMediaGeneralId ) )
        {
        TVcxMyVideosDownloadState dlState = VideoDownloadState( aIndex );
        
        if ( dlState == EVcxMyVideosDlStatePaused || dlState == EVcxMyVideosDlStateFailed )
            {
            iModel.DownloadClient().ResumeDownloadL( *media );
            
            TUint32 mpxId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1;
            if ( iResumeArray.Find( mpxId ) == KErrNotFound )
                {
                iResumeArray.AppendL( mpxId );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::CancelDownloadL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::CancelDownloadL( TInt aIndex )
    {
    CMPXMedia* media = iVideoArray->MPXMedia( aIndex );

    if ( media )
        {            
        iModel.DownloadClient().CancelDownloadL( *media );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::Release()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::Release( TInt aReleaseStart, 
                                               TInt aReleaseEnd )
    {   
    for ( TInt i = aReleaseStart; i <= aReleaseEnd; i++ )
        {
        TRAP_IGNORE( iDataUpdater->ReleaseData( iVideoArray->ArrayIndexToMpxItemIdL( i ) ) );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::Request()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::Request( TInt aRequestStart, 
                                               TInt aRequestEnd,
                                               THgScrollDirection aDirection )
    {
    TRAP_IGNORE( HandleRequestL( aRequestStart, aRequestEnd, aDirection ) );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::HandleRequestL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoModelHandler::HandleRequestL( TInt aRequestStart, 
                                                      TInt aRequestEnd,
                                                      THgScrollDirection /*aDirection*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "MPX My Videos UI # CVcxHgMyVideosVideoModelHandler::HandleRequestL - Enter" );
    
    TInt videoCount = iVideoArray->VideoCount();
    TInt requestStart = ( aRequestStart >= 0 ? aRequestStart : 0 );
    TInt requestEnd = ( aRequestEnd < videoCount ? aRequestEnd : ( videoCount - 1 ) );
    
    IPTVLOGSTRING3_LOW_LEVEL(
        "MPX My Videos UI # CVcxHgMyVideosVideoModelHandler::HandleRequestL - aRequestStart: %d, aRequestEnd: %d", requestStart, requestEnd );
        
    for ( TInt i = requestStart; i <= requestEnd; i++ )
        {
        UpdateVideoListItemL( i );
        iDataUpdater->RequestDataL( iVideoArray->ArrayIndexToMpxItemIdL( i ) );
        }

    iScroller.DrawDeferred();
        
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosVideoModelHandler::HandleRequestL() - Exit" );    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::FormatVideoSecondRowLC()
// -----------------------------------------------------------------------------
// 
HBufC* CVcxHgMyVideosVideoModelHandler::FormatVideoSecondRowLC(
            TInt aIndex,
            CMPXMedia& aMedia,
            TBool& aIsDownloading )

    {
    if ( VideoDownloadState( aIndex ) == EVcxMyVideosDlStateNone )
        {
        aIsDownloading = EFalse;
        return FormatVideoSecondRowCompletedLC( aMedia );
        }
    else
        {
        aIsDownloading = ETrue;
        return FormatVideoSecondRowDownloadingLC( aIndex, aMedia );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::FormatVideoSecondRowCompletedLC()
// -----------------------------------------------------------------------------
// 
HBufC* CVcxHgMyVideosVideoModelHandler::FormatVideoSecondRowCompletedLC( 
        CMPXMedia& aMedia )
    {
    _LIT( KVcxSecondLineSeparator, "," );

    HBufC* details = NULL;
    HBufC* sizeStr = NULL;
    HBufC* lengthStr = NULL;
    TReal32 length( 0 );
    TInt64 size( 0 );
    TInt detailsSize( 0 );

    // Use KMPXMediaGeneralDuration or KVcxMediaMyVideosDuration?
    if ( aMedia.IsSupported( KVcxMediaMyVideosDuration ) )
        {
        length = *( aMedia.Value<TReal32>( KVcxMediaMyVideosDuration ) ); // In seconds
        IPTVLOGSTRING2_LOW_LEVEL( "MPX My Videos UI # FormatVideoSecondRowCompletedLC() length=%f", length );
        }

    if ( aMedia.IsSupported( KMPXMediaGeneralExtSizeInt64 ) )
        {
        size = *( aMedia.Value<TInt64>( KMPXMediaGeneralExtSizeInt64 ) ); // In bytes
        IPTVLOGSTRING2_LOW_LEVEL( "MPX My Videos UI # FormatVideoSecondRowCompletedLC() size=%ld", size );
        }

    if ( size > 0 )
        {
        TUint64 bytes = static_cast<TUint64>( size );
        TUint64 dispSize = 0;

        if ( bytes >= KVcxVideoSizeGB )
            {
            dispSize  = bytes + KVcxVideoSizeHalfGB;
            dispSize /= KVcxVideoSizeGB;
            sizeStr = StringLoader::LoadLC( R_VCXHGMYVIDEOS_SIZE_LIST_GB, dispSize );
            }
        else if ( bytes >= KVcxVideoSizeMB )
            {
            dispSize  = bytes + KVcxVideoSizeHalfMB;
            dispSize /= KVcxVideoSizeMB;
            sizeStr = StringLoader::LoadLC( R_VCXHGMYVIDEOS_SIZE_LIST_MB, dispSize );
            }
        else
            {
            dispSize  = bytes + KVcxVideoSizeHalfKB;
            dispSize /= KVcxVideoSizeKB;
            sizeStr = StringLoader::LoadLC( R_VCXHGMYVIDEOS_LIST_SIZE_KB, dispSize );
            }
        detailsSize += sizeStr->Length();
        }
    if ( length > 0 )
        {
        TUint total = static_cast<TUint>( length );
        
        TUint hours = total / KVcxSecondsInHour;
        TUint minutes = total / KVcxSecondsInMinute % KVcxSecondsInMinute;
        TUint seconds = total % KVcxSecondsInMinute;

        if ( hours > 0 )
            {
            CArrayFix<TInt>* numbers = new (ELeave) CArrayFixFlat<TInt>(2);
            CleanupStack::PushL( numbers );
            numbers->AppendL( hours );
            numbers->AppendL( minutes );
            lengthStr = StringLoader::LoadL( R_VCXHGMYVIDEOS_VIDEO_LIST_LENGTH_H_MIN, *numbers );
            CleanupStack::PopAndDestroy( numbers );
            CleanupStack::PushL( lengthStr );
            }
        else if ( minutes > 0 )
            {
            CArrayFix<TInt>* numbers = new (ELeave) CArrayFixFlat<TInt>(2);
            CleanupStack::PushL( numbers );
            numbers->AppendL( minutes );
            numbers->AppendL( seconds );
            lengthStr = StringLoader::LoadL( R_VCXHGMYVIDEOS_VIDEO_LIST_LENGTH_MIN_SEC, *numbers );
            CleanupStack::PopAndDestroy( numbers );
            CleanupStack::PushL( lengthStr );
            }
        else
            {
            lengthStr = StringLoader::LoadLC( R_VCXHGMYVIDEOS_VIDEO_LIST_LENGTH_SEC, seconds ); 
            }
        detailsSize += lengthStr->Length();
        }      

    details = HBufC::NewL( detailsSize + KVcxSecondLineSeparator().Length() );
    if ( lengthStr )
        {
        details->Des().Append( *lengthStr );
        CleanupStack::PopAndDestroy( lengthStr );
        }
    if ( sizeStr )
        {
        if ( lengthStr ) // Add separator when both are available.
            {
            details->Des().Append( KVcxSecondLineSeparator );
            }
        details->Des().Append( *sizeStr );
        CleanupStack::PopAndDestroy( sizeStr );
        }

    CleanupStack::PushL( details );
    return details;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::FormatVideoSecondRowDownloadingLC()
// -----------------------------------------------------------------------------
// 
HBufC* CVcxHgMyVideosVideoModelHandler::FormatVideoSecondRowDownloadingLC(
            TInt aIndex,
            CMPXMedia& aMedia )
    {
    HBufC* details = NULL;
    TInt progress = DownloadProgressL( aMedia );

    switch ( VideoDownloadState( aIndex ) )
        {
        case EVcxMyVideosDlStateDownloading:
            {
            details = StringLoader::LoadLC( R_VCXHGMYVIDEOS_VIDEO_DOWNLOADING, progress );
            break;
            }
        case EVcxMyVideosDlStateFailed:
            {
            details = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DOWNLOAD_FAILED, progress );
            break;
            }
        case EVcxMyVideosDlStatePaused:
            {
            details = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DOWNLOAD_PAUSED, progress );
            break;
            }
        case EVcxMyVideosDlStateNone:            
        case EVcxMyVideosDlStateDownloaded:
        default:
            {
            IPTVLOGSTRING2_LOW_LEVEL( 
                "MPX My Videos UI # FormatVideoSecondRowDownloadingL() Unexpected state (%d)!",
                VideoDownloadState( aIndex ) );
            #ifdef _DEBUG
            User::Panic( KVcxHgMyVideosPanic, EVcxHgMyVideosPanicLogicalVideo );
            #else
            User::Leave( KErrArgument );
            #endif // _DEBUG
            }
        }

    return details;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ReplaceVideoArrayL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::ReplaceVideoArrayL( CMPXMediaArray& aVideoList )
    {
    iVideoArray->ReplaceVideoListL( aVideoList );
    iDataUpdater->InfoArrayChanged();
    iDownloadUpdater->VideoArrayChangedL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::DownloadProgressL()
// -----------------------------------------------------------------------------
// 
TInt CVcxHgMyVideosVideoModelHandler::DownloadProgressL( const CMPXMedia& aMpxMedia )
    {
    TInt progress = 0;

    if ( aMpxMedia.IsSupported( KVcxMediaMyVideosDownloadProgress ) )
        {
        progress = static_cast<TInt>( 
                       aMpxMedia.ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress ) );
        }

    IPTVLOGSTRING3_LOW_LEVEL(
        "MPX My Videos UI # DownloadProgressL() Id:%d Progress:%d",
        static_cast<TInt>( aMpxMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ),
        progress );

    progress = ( progress >= 0 ? progress : 0 );
    progress = ( progress < 100 ? progress : 99 );

    return progress;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::UpdateVideoListItemL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::UpdateVideoListItemL( TInt aListIndex )
    {
    if ( aListIndex >= 0 && aListIndex < iScroller.ItemCount() )
        {
        TVcxHgMyVideosIndicatorHelper indicatorHelper;
        CMPXMedia* media( NULL );
        
        media = iVideoArray->MPXMedia( aListIndex );

        if ( media )
            {           
            CHgItem& item = iScroller.ItemL( aListIndex );
            TBool isMarked = ( item.Flags() & CHgItem::EHgItemFlagMarked );
            item.ClearFlags( item.Flags() );
            
            if ( media->IsSupported( KMPXMediaGeneralTitle ) )
                {
                HBufC* name( NULL );        
                name = media->ValueText( KMPXMediaGeneralTitle ).AllocLC();
                if ( name->Length() > 0 )
                    {
                    item.SetTitleL( *name );
                    }
                CleanupStack::PopAndDestroy( name );
                }
            TBool isDownloading( EFalse );
            HBufC* details( NULL );
            
            details = FormatVideoSecondRowLC( aListIndex, *media, isDownloading );
            if ( details->Length() > 0 )
                {
                item.SetTextL( *details );
                }
            CleanupStack::PopAndDestroy( details );
    
            TBool isNewVideo( EFalse );
            if ( media->IsSupported( KMPXMediaGeneralFlags ) )
                {
                if ( media->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags ) &
                     EVcxMyVideosVideoNew )
                    {
                    isNewVideo = ETrue;
                    }
                }
            TInt indicator1( 0 );
            TInt indicator2( 0 );            
            
            indicatorHelper.GetIndicatorsForVideoL(
                iModel,
                isNewVideo,
                EFalse, // *Recordings*
                media->ValueText( KMPXMediaGeneralUri ),
                TVcxHgMyVideosIndicatorHelper::EIndicatorDrmStatusUnknown,
                indicator1,
                indicator2 );   
            if ( indicator1 )
                {
                item.SetFlags( indicator1 );
                }
            if ( indicator2 )
                {
                item.SetFlags( indicator2 );
                }
            if ( isMarked )
                {
                item.SetFlags( CHgItem::EHgItemFlagMarked );
                }
            if ( VideoIndicator().IsIndicatorShown( *media ) )
                {
                item.SetFlags( CHgItem::EHgItemFlagsIconOverlayIndicator );
                }
            if ( media->IsSupported( KMPXMediaGeneralDate ) )
                {
                TInt64 itemDate = media->ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
                item.SetTime( itemDate );
                }    
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::DeleteItemL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::DeleteItemL( TMPXItemId aMpxItemId )
    {
    // Remove video from data and download updaters.
    iDataUpdater->ReleaseData( aMpxItemId );
    iDownloadUpdater->RemoveDownload( aMpxItemId );

    // Remove video from video array.
    TInt removedIndex = iVideoArray->RemoveVideo( aMpxItemId );
    
    if ( removedIndex != KErrNotFound )
        {
        // Remove item from the list.
        iScroller.RemoveItem( removedIndex );
        
        if ( iScroller.ItemCount() > 0 )
            {
            TInt selectedIndex = iScroller.SelectedIndex();
            
            // Selected index number is greater than
            // item count. Set the last item to be highlighted.
            if ( selectedIndex >= iScroller.ItemCount() )
                {
                iScroller.SetSelectedIndex( iScroller.ItemCount() - 1 );
                }
            }
        else
            {
            // Last item has deleted, set empty text and reset list.
            iVideoListImpl.SetEmptyListTextByResourceL( R_VCXHGMYVIDEOS_NO_VIDEOS );
            iScroller.Reset();
            }
        
        // fix for error where highlight was sometimes lost after delete. Problem is 
        // that someone is 'stealing' keyboard focus from the scroller (probably the
        // confirmation note, as user needs to clicks them to start the delete).
        iScroller.SetFocus( ETrue ); 
        
        iScroller.DrawDeferred();
        iView.DynInitMskL();
        }
    
    TInt index( KErrNotFound );
    index = iResumeArray.Find( aMpxItemId.iId1 );
    
    if ( index >= 0 && index < iResumeArray.Count() )
        {
        iResumeArray.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::InsertVideoL()
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::InsertVideoL( CMPXMedia* aVideo )
    {
    if ( aVideo )
        {		
        if ( aVideo->IsSupported( KMPXMediaGeneralId ) )
            {           
            CleanupStack::PushL( aVideo );
		    TMPXItemId itemId = aVideo->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            
            // Get current sort order.
            TVcxMyVideosSortingOrder sortOrder = iModel.VideolistSortOrderL();
            
            // Ownership of video item is transferred to video array.
            CleanupStack::Pop( aVideo );
            iVideoArray->AddToCorrectPlaceL( aVideo, sortOrder );
            
            TInt index = iVideoArray->IndexByMPXItemId( itemId );
            
            if ( index >= 0 )
                {
                // Create a new Hg list item. Ownership of list item  
                // is transferred to scroller.
                CHgItem* listItem = CHgItem::NewL();
                iScroller.InsertItem( listItem, index );
                            
                if ( iScroller.SelectedIndex() == KErrNotFound )
                    {
                    iScroller.SetSelectedIndex( 0 );
                    }    
                UpdateVideoListItemL( index );
                
                // Update screen.
                iScroller.DrawDeferred();
                iView.DynInitMskL();
                                                    
                iDataUpdater->RequestDataL( itemId );    
                iDownloadUpdater->VideoModifiedL( 
                    EMPXItemInserted, itemId, EVcxMyVideosListNoInfo );
                }
            }
         else
            {
            delete aVideo;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ValidOrigin()
// -----------------------------------------------------------------------------
// 
TBool CVcxHgMyVideosVideoModelHandler::ValidOrigin( CMPXMedia& aMedia )
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # ValidOrigin - Enter" );
    
    TBool isValid( EFalse );
	
    if ( aMedia.IsSupported( KVcxMediaMyVideosOrigin ) )
        {
        TUint8 mediaOrigin( EVcxMyVideosOriginOther );
        mediaOrigin = aMedia.ValueTObjectL<TUint8>( KVcxMediaMyVideosOrigin );   
		
        // All videos
        if ( CurrentCategoryIndex() == KVcxMvcCategoryIdAll )
            {
            IPTVLOGSTRING_LOW_LEVEL( 
                "MPX My Videos UI # ValidOrigin - Origin: All" );
            isValid = ETrue;
            }
        // Captured
        else if ( CurrentCategoryIndex() == KVcxMvcCategoryIdCaptured && 
                mediaOrigin == EVcxMyVideosOriginCapturedWithCamera )
            {
            IPTVLOGSTRING_LOW_LEVEL( 
                "MPX My Videos UI # ValidOrigin - Origin: Captured" );
            isValid = ETrue;
            }
        // Downloads
        else if ( CurrentCategoryIndex() == KVcxMvcCategoryIdDownloads &&
                mediaOrigin == EVcxMyVideosOriginDownloaded )
            {
            IPTVLOGSTRING_LOW_LEVEL( 
                "MPX My Videos UI # ValidOrigin - Origin: Downloads" );    	
            isValid = ETrue;
            }
        // Other
        else if ( CurrentCategoryIndex() == KVcxMvcCategoryIdOther && 
                ( mediaOrigin == EVcxMyVideosOriginSideLoaded ||
                mediaOrigin == EVcxMyVideosOriginOther ) )
            {
            IPTVLOGSTRING_LOW_LEVEL( 
                "MPX My Videos UI # ValidOrigin - Origin: Other" ); 
            isValid = ETrue;
            }
        }
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # ValidOrigin, isValid %b - Exit", isValid );
    return isValid;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::NewVideoListL()
// MPX Collection calls this callback when video list is available.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::NewVideoListL( CMPXMediaArray& aVideoList )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # NewVideoListL(count=%d) - Enter", aVideoList.Count() );
        
    ReplaceVideoArrayL( aVideoList );
            
    if ( iVideoArray->VideoCount() > 0 )
        {
        iScroller.ResizeL( iVideoArray->VideoCount() );               
		
        TInt highlight( KErrNotFound );
		
        if ( iRestoreHighlightPosition )
            {
            highlight = iVideoArray->IndexByMPXItemId( iHighlightedItemId );
            }
        
        highlight != KErrNotFound ? iScroller.SetSelectedIndex( highlight ) :
		                            iScroller.SetSelectedIndex( 0 );
        }
    else
        {
        iVideoListImpl.SetEmptyListTextByResourceL( R_VCXHGMYVIDEOS_NO_VIDEOS );
        iScroller.DrawDeferred();
        }        
    iView.DynInitMskL();
 
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # NewVideoListL() - Exit" );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoListModifiedL()
// MPX Collection calls this callback when the video list has been modified.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoListModifiedL()
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # VideoListModifiedL(count=%d) - Enter", 
        iVideoArray->VideoCount() );

    TInt videoCount = iVideoArray->VideoCount();
        
    if ( iScroller.ItemCount() != videoCount )
        {
        iScroller.ResizeL( videoCount );
        }
        
    if ( iScroller.ItemCount() > 0 )
       {
       iScroller.DrawDeferred();
       }
              
    iView.DynInitMskL();       

    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # VideoListModifiedL() - Exit" );            
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoModifiedL()
// MPX Collection calls this callback when single video item has been modified.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoModifiedL( TMPXChangeEventType aEventType,
                                                      TMPXItemId aMpxItemId,
                                                      TInt32 aExtraInfo,
                                                      TBool aSimulated )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # VideoModifiedL(event=%d)", aEventType );
        
    switch ( aEventType )
        {
        case EMPXItemInserted:
            {
            if ( aMpxItemId.iId2 < 2 )
                {
                iModel.CollectionClient().FetchMpxMediaByMpxIdL( aMpxItemId );
                }
            }
            break;
            
        case EMPXItemModified:
            {
            if ( aExtraInfo == EVcxMyVideosVideoListOrderChanged )
                {
                if ( aMpxItemId.iId1 == iCurrentCategoryIndex &&
                     aMpxItemId.iId2 == 1 )
                    {                    
                    // Re-fetch current list completely, MMC card has 
                    // removed or inserted.
                    iModel.CollectionClient().GetVideoListL( KErrNotFound );
                    }
                }
            else
                {
                TInt index = iVideoArray->IndexByMPXItemId( aMpxItemId );
                if ( index != KErrNotFound )
                    {
                    UpdateVideoListItemL( index );              
                    iScroller.RefreshScreen( index );
                
                    if ( ! aSimulated )
                        {
                        iDownloadUpdater->VideoModifiedL( aEventType, aMpxItemId, aExtraInfo );
                        iDataUpdater->RequestDataL( aMpxItemId );    
					    }                      
                    iView.DynInitMskL();
                    }
                }
            }
            break;
        
        case EMPXItemDeleted:
            {
            if ( aMpxItemId.iId2 < 2 )
                {
                DeleteItemL( aMpxItemId );    
                }
            }
            break;
        
        default:
            {
            IPTVLOGSTRING_LOW_LEVEL( 
                "MPX My Videos UI # VideoModifiedL() Unexpected change!" );
            #ifdef _DEBUG
            User::Panic( KVcxHgMyVideosPanic, EVcxHgMyVideosPanicLogicalVideo );
            #endif // _DEBUG            
            }
            break;
        } 
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoFetchingCompletedL()
// MPX Collection calls this callback when a single video has fetched.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoFetchingCompletedL( CMPXMedia* aVideo )
    {
    if ( ValidOrigin( *aVideo ) )
        {
        // Ownership of video item is transferred.
        InsertVideoL( aVideo );	
        }
    else
        {
        delete aVideo;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::ResumeStartedFromBeginningL()
// Download updater calls this when resume has started from beginning.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::ResumeStartedFromBeginningL( TUint32 aMpxId )
    {
    TInt index = iResumeArray.Find( aMpxId );
    if ( index >= 0 )
        {
        iResumeArray.Remove( index );
        iVideoListImpl.ShowResumeStartsFromBeginningNoteL();
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoDeletionCompletedL()
// MPX Collection calls this callback when deletion of multiple videos completes.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoDeletionCompletedL( TInt aFailedCount,
                                                               TMPXItemId aFailedMpxId )
    {
    CMPXMedia* media = iVideoArray->MPXMediaByMPXItemId( aFailedMpxId );

    if ( aFailedCount == 1 && media && media->IsSupported( KMPXMediaGeneralTitle ) )
        {
        iVideoListImpl.VideoDeletionCompletedL( aFailedCount, 
                                                media->ValueText( KMPXMediaGeneralTitle ) );
        }
    else
        {
        iVideoListImpl.VideoDeletionCompletedL( aFailedCount, 
                                                KNullDesC );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoMoveOrCopyCompletedL()
// MPX Collection calls this callback when move or copy command is completed.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoMoveOrCopyCompletedL( TInt aFailedCount,
                                                                 TMPXItemId aFailedMpxId )
    {
    CMPXMedia* media = iVideoArray->MPXMediaByMPXItemId( aFailedMpxId );

    if ( aFailedCount == 1 && media && media->IsSupported( KMPXMediaGeneralTitle ) )
        {    
        iVideoListImpl.VideoMoveOrCopyCompletedL( aFailedCount, 
                                                  media->ValueText( KMPXMediaGeneralTitle ) );
        }
    else
        {
        iVideoListImpl.VideoMoveOrCopyCompletedL( aFailedCount, 
                                                  KNullDesC );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::VideoDetailsCompletedL()
// MPX Collection calls this callback when details fetching is completed.
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::VideoDetailsCompletedL( const CMPXMedia& aMedia )
    {
    iVideoDetails->ShowVideoDetailsDialogL( aMedia );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoModelHandler::UpdateScrollbarTypeL()
// Update scrollbar strip type depending on sort order
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosVideoModelHandler::UpdateScrollbarTypeL( const TVcxMyVideosSortingOrder& aSortingOrder )
    {
    if ( EVcxMyVideosSortingName == aSortingOrder )
        {
        // First letter on strip
        iScroller.SetScrollBarTypeL( CHgScroller::EHgScrollerLetterStripLite );        
        }
    else if ( EVcxMyVideosSortingCreationDate == aSortingOrder )
        {
        // Show date on strip
        iScroller.SetScrollBarTypeL( CHgScroller::EHgScrollerTimeStrip );
        }
    else
        {
        // No strip
        iScroller.SetScrollBarTypeL( CHgScroller::EHgScrollerScrollBar );
        }
    }
