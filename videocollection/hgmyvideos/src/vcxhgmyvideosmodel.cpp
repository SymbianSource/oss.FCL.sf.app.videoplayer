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
* Description:      Common model class for component.*
*/




// INCLUDE FILES
#include <bldvariant.hrh>
#include <AknUtils.h>
#include <centralrepository.h>
#include "IptvDebug.h"
#include <MediatorEventProvider.h>
#include "iptvlastwatcheddata.h"
#include "iptvlastwatchedapi.h"
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideoscollectionclient.h"
#include "vcxhgmyvideosdownloadclient.h"
#include "vcxnsmediatorids.h"
#include "thumbnaildata.h"

_LIT( KTemporaryThumbPath, "C:\\Data\\videocenter\\ecg\\lastwatched.bmp" ); 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel::CVcxHgMyVideosModel()
  : iAppState( EVcxMyVideosAppStateUnknown ),
    iPreviousAppState( EVcxMyVideosAppStateUnknown ),
    iTnRequestId( KErrNotFound ),
    iSortOrder( EVcxMyVideosSortingNone )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel* CVcxHgMyVideosModel::NewL()
    {
    CVcxHgMyVideosModel* self = 
        CVcxHgMyVideosModel::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel* CVcxHgMyVideosModel::NewLC()
    {
    CVcxHgMyVideosModel* self = 
        new (ELeave) CVcxHgMyVideosModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::ConstructL()
    {
    iCollection = CVcxHgMyVideosCollectionClient::NewL();
    iTouchSupport = AknLayoutUtils::PenEnabled();
    iMediatorEventProvider = CMediatorEventProvider::NewL();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::~CVcxHgMyVideosModel()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel::~CVcxHgMyVideosModel()
    {
    if ( iFsSession.Handle() )
        {
        iFsSession.Close();
        }
    delete iDriveMonitor;
    delete iCollection;
    delete iCollectionCenRep;
    delete iLastWatchedApi;
    delete iLastWatchedData;
    delete iVideoPlayerCustomMessage;

    if ( iTnManager && iTnRequestId != KErrNotFound )
        {
        iTnManager->CancelRequest( iTnRequestId );
        }
    delete iTnManager;
    delete iMediatorEventProvider;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::TouchSupport()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosModel::TouchSupport()
    {
    return iTouchSupport;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::CollectionClient()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCollectionClient& CVcxHgMyVideosModel::CollectionClient()
    {
    return *iCollection;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::DownloadClient()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosDownloadClient& CVcxHgMyVideosModel::DownloadClient()
    {
    return iCollection->DownloadClient();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::FileServerSessionL()
// -----------------------------------------------------------------------------
//
RFs& CVcxHgMyVideosModel::FileServerSessionL()
    {
    if ( ! iFsSession.Handle() )
        {
        User::LeaveIfError( iFsSession.Connect() );
        }

    return iFsSession;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::DriveMonitorL()
// -----------------------------------------------------------------------------
//
CIptvDriveMonitor& CVcxHgMyVideosModel::DriveMonitorL()
    {
    if ( ! iDriveMonitor )
        {
        iDriveMonitor = CIptvDriveMonitor::NewL();
        iDriveMonitor->RegisterObserverL( this );
        }

    return *iDriveMonitor;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::SetAppState()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::SetAppState( 
        CVcxHgMyVideosModel::TVcxMyVideosAppState aAppState )
    {
    iPreviousAppState = iAppState;
    iAppState = aAppState;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::AppState()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel::TVcxMyVideosAppState CVcxHgMyVideosModel::AppState()
    {
    return iAppState;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::PreviousAppState()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel::TVcxMyVideosAppState CVcxHgMyVideosModel::PreviousAppState()
    {
    return iPreviousAppState;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::SetVideolistSortOrderL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::SetVideolistSortOrderL( TVcxMyVideosSortingOrder aSortOrder )
    {
    if ( ! iCollectionCenRep )
        {
        iCollectionCenRep = CRepository::NewL( TUid::Uid( KVcxMyVideosCollectionCenrepUid ) );
        }

    iCollectionCenRep->Set( KVcxMyVideosCollectionCenrepKeySortingOrder, 
                            static_cast<TInt>( aSortOrder ) );
    
    iSortOrder = aSortOrder;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::VideolistSortOrderL()
// -----------------------------------------------------------------------------
//
TVcxMyVideosSortingOrder CVcxHgMyVideosModel::VideolistSortOrderL()
    {
    if ( iSortOrder == EVcxMyVideosSortingNone )
    	{
        if ( ! iCollectionCenRep )
            {
            iCollectionCenRep = CRepository::NewL( TUid::Uid( KVcxMyVideosCollectionCenrepUid ) );
            }

        TInt value( KErrNotFound );

        TInt error = iCollectionCenRep->Get( KVcxMyVideosCollectionCenrepKeySortingOrder, value );
        iSortOrder = static_cast<TVcxMyVideosSortingOrder>( value );
    
        if ( ( error != KErrNone ) || 
             ( ( iSortOrder != EVcxMyVideosSortingCreationDate ) &&
               ( iSortOrder != EVcxMyVideosSortingName ) &&
               ( iSortOrder != EVcxMyVideosSortingSize ) ) )
            {
            iSortOrder = EVcxMyVideosSortingCreationDate;
            }
    	}
    return iSortOrder;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::GetScreenResolution()
//
//       Landscape    Portrait
// QVGA  320x240  or  240x320
// QHD   640x360  or  360x640
// VGA   640x480  or  480x640
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel::TVcxScreenResolution CVcxHgMyVideosModel::GetScreenResolution( 
        CCoeEnv* aCoeEnv )
    {
    TVcxScreenResolution resolution( EVcxScreenResolutionUnknown );
    TSize screenSize = aCoeEnv->ScreenDevice()->SizeInPixels(); 
    TBool landscape = screenSize.iWidth > screenSize.iHeight;

    if ( ( landscape && screenSize.iHeight <= 240 ) ||
         ( ! landscape && screenSize.iWidth <= 240 ) )
        {
        resolution = EVcxScreenResolutionQVGA;
        }
    else if ( ( landscape && screenSize.iHeight <= 360 ) ||
              ( ! landscape && screenSize.iWidth <= 360 ) )
        {
        resolution = EVcxScreenResolutionQHD;
        }
    else if ( ( landscape && screenSize.iHeight <= 480 ) ||
              ( ! landscape && screenSize.iWidth <= 480 ) )
        {
        resolution = EVcxScreenResolutionVGA;
        }

    return resolution;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::SetVideoAsLastWatchedL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::SetVideoAsLastWatchedL( TVideoPlayerCustomMessage& aVideoInfo, 
                                                  TInt aMpxId1,
                                                  TUint32 aAgeProfile )
    {
    if ( iTnRequestId != KErrNotFound )
        {
        ThumbnailManagerL()->CancelRequest( iTnRequestId );
        iTnRequestId = KErrNotFound;
        }

    CopyVideoPlayerCustomMessageL( aVideoInfo, *VideoPlayerCustomMessageL() );
    
    // Store data from Video Player Custom message to 'Last Watched' item on 
    // disk. Un-used (legacy) data fields are resetted to dummy values.
    
    LastWatchedDataL()->SetIconPathL( VideoPlayerCustomMessageL()->iIcon );
    /*
    LastWatchedDataL()->SetLastVideoPlayPoint( ViaPlayerCustomMessageL()->iStartPosition ); 
    LastWatchedDataL()->SetMimeTypeL( ViaPlayerCustomMessageL()->iMimeType ); 
    */
    
    LastWatchedDataL()->SetNameL( VideoPlayerCustomMessageL()->iName );
    LastWatchedDataL()->SetUriL( VideoPlayerCustomMessageL()->iContent ); 
    
	// LastWatchedDataL()->SetContentType( ViaPlayerCustomMessageL()->iContentType );
        
    LastWatchedDataL()->SetMpxId( aMpxId1 );
    LastWatchedDataL()->SetServiceId( KIdUndefined );
    LastWatchedDataL()->SetContentId( KIdUndefined );
    LastWatchedDataL()->SetContentIndex( -1 );    
    LastWatchedDataL()->SetParentalControl( aAgeProfile );
    LastWatchedDataL()->SetParametersL( KNullDesC8 );
    LastWatchedDataL()->SetAppUid( TUid::Uid( 0 ) );
    LastWatchedDataL()->SetViewUid( TUid::Uid( 0 ) );
    LastWatchedDataL()->SetParameterId( TUid::Uid( 0 ) );
    LastWatchedDataL()->SetLaunchType( CIptvLastWatchedData::EUndefined );
    
    // Start fetching or generating thumbnail for last watched video.
    // The 'Last Watched' data is published only when thumb is available.
    CThumbnailObjectSource* source = CThumbnailObjectSource::NewLC( 
        VideoPlayerCustomMessageL()->iContent, 
        KNullDesC ); 
    iTnRequestId = ThumbnailManagerL()->GetThumbnailL( *source );
    CleanupStack::PopAndDestroy( source );    
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::UpdateLastWatchedPlayPositionL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::UpdateLastWatchedPlayPositionL( TUint32 aLastVideoPlayPoint )
    {
    if ( iTnRequestId != KErrNotFound )
        {
        LastWatchedDataL()->SetLastVideoPlayPoint( aLastVideoPlayPoint );
        }
    else
        {
        LastWatchedApiL()->UpdateLastVideoPlayPointL( aLastVideoPlayPoint );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::GetLastWatchedDataL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::GetLastWatchedDataL( TVideoPlayerCustomMessage& aVideoInfo,
                                               TUint32& /*aMpxId1 */,
                                               TUint32& aAgeProfile )
    {
    LastWatchedApiL()->GetLastWatchedDataL( *LastWatchedDataL() );

    aVideoInfo.iContent = LastWatchedDataL()->Uri();
    aVideoInfo.iName = LastWatchedDataL()->Name();
    aVideoInfo.iIcon = LastWatchedDataL()->IconPath();
    
    aAgeProfile = LastWatchedDataL()->ParentalControl();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::ThumbnailPreviewReady()
// From MThumbnailManagerObserver, not used in Video Center.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::ThumbnailPreviewReady( MThumbnailData& /*aThumbnail*/, 
                                                 TThumbnailRequestId /*aId*/ )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::ThumbnailReady()
// From MThumbnailManagerObserver
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::ThumbnailReady( TInt aError,
                                          MThumbnailData& aThumbnail,
                                          TThumbnailRequestId aId )
    {
    TRAP_IGNORE( HandleThumbnailReadyL( aError, aThumbnail, aId ) );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::HandleThumbnailReadyL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::HandleThumbnailReadyL( TInt aError,
                                                 MThumbnailData& aThumbnail,
                                                 TThumbnailRequestId aId )
    {
    IPTVLOGSTRING3_LOW_LEVEL(
        "MPX My Videos UI # HandleThumbnailReadyL(error=%d, ID=%d)", aError, aId );

    if ( aError == KErrNone && aId == iTnRequestId )
        {
        iTnRequestId = KErrNotFound;

        // Save a copy of the bitmap to local file that Matrix Menu can access.
        CFbsBitmap* bitmap = aThumbnail.Bitmap();
        bitmap->Save( KTemporaryThumbPath );
        LastWatchedDataL()->SetIconPathL( KTemporaryThumbPath );
        VideoPlayerCustomMessageL()->iIcon = KTemporaryThumbPath;
        }
    else
        {
        iTnRequestId = KErrNotFound;
        }

    LastWatchedApiL()->SetLastWatchedDataL( *LastWatchedDataL() );
    TPckg<TVideoPlayerCustomMessage> videoInfoPckg( *VideoPlayerCustomMessageL() );
    iMediatorEventProvider->RaiseEvent( KVcxNsVideoCenterMediatorDomain,
                                        KVcxNsPlayerGeneralCategory,
                                        KVcxNsMediatorEventPlayerLastWatch,
                                        TVersion( KVcxNsMediatorPlayerVersion, 0, 0 ),
                                        videoInfoPckg );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::ResetDownloadNotification()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::ResetDownloadNotification()
    {
    iMediatorEventProvider->RaiseEvent( TUid::Uid( KVcxNsMpxMediatorDomain ),
                                        TUid::Uid( KVcxNsMpxMediatorCategory ), 
                                        KVcxNsMpxEventResetDownloadNotification,
                                        TVersion( KVcxNsMpxEventVersion, 0, 0 ),
                                        KNullDesC8() ); 
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::ThumbnailManagerL()
// -----------------------------------------------------------------------------
//
CThumbnailManager* CVcxHgMyVideosModel::ThumbnailManagerL()
    {
    if ( ! iTnManager )
        {
        iTnManager = CThumbnailManager::NewL( *this );
        iTnManager->SetThumbnailSizeL( EVideoListThumbnailSize );
        iTnManager->SetDisplayModeL( EColor16M );
        }
    return iTnManager;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::CopyVideoPlayerCustomMessageL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::CopyVideoPlayerCustomMessageL( TVideoPlayerCustomMessage& aSource,
                                                         TVideoPlayerCustomMessage& aTarget )
    {
    aTarget.iContent = aSource.iContent;
    aTarget.iName = aSource.iName;
    aTarget.iIcon = aSource.iIcon;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::VideoPlayerCustomMessageL()
// -----------------------------------------------------------------------------
//
TVideoPlayerCustomMessage* CVcxHgMyVideosModel::VideoPlayerCustomMessageL()
    {
    if ( ! iVideoPlayerCustomMessage ) 
        {
        iVideoPlayerCustomMessage = new (ELeave) TVideoPlayerCustomMessage;
        }
    return iVideoPlayerCustomMessage;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::LastWatchedApiL()
// -----------------------------------------------------------------------------
//
CIptvLastWatchedApi* CVcxHgMyVideosModel::LastWatchedApiL()
    {
    if ( ! iLastWatchedApi )
        {
        iLastWatchedApi = CIptvLastWatchedApi::NewL();
        }
    return iLastWatchedApi;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::LastWatchedDataL()
// -----------------------------------------------------------------------------
//
CIptvLastWatchedData* CVcxHgMyVideosModel::LastWatchedDataL()
    {
    if ( ! iLastWatchedData )
        {
        iLastWatchedData = CIptvLastWatchedData::NewL();
        }
    return iLastWatchedData;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::HandleDriveMonitorEvent()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::HandleDriveMonitorEvent( TIptvDriveMonitorEvent& /*aEvent*/ )
    {
    // We are not interested about the event, we are observing just to keep
    // iDriveMonitor.iAvailableDrives up to date. 
    }
