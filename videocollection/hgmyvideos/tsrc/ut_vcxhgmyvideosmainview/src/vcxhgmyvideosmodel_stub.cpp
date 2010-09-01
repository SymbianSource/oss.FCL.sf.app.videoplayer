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
* Description:     Stub class for unit testing*
*/


#include "VcxHgMyVideosModel.h"

// INCLUDE FILES

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
// 
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
// 
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
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel::~CVcxHgMyVideosModel()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosModel::TouchSupport()
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCollectionClient& CVcxHgMyVideosModel::CollectionClient()
    {
    return *iCollection;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosDownloadClient& CVcxHgMyVideosModel::DownloadClient()
    {
    return *( CVcxHgMyVideosDownloadClient* ) 0;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
RFs& CVcxHgMyVideosModel::FileServerSessionL()
    {
    return *( RFs* ) 0;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CIptvDriveMonitor& CVcxHgMyVideosModel::DriveMonitorL()
    {
    return *( CIptvDriveMonitor* ) 0;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::SetAppState( 
        CVcxHgMyVideosModel::TVcxMyVideosAppState aAppState )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel::TVcxMyVideosAppState CVcxHgMyVideosModel::AppState()
    {
    return ( CVcxHgMyVideosModel::TVcxMyVideosAppState )0;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel::TVcxMyVideosAppState CVcxHgMyVideosModel::PreviousAppState()
    {
    return ( CVcxHgMyVideosModel::TVcxMyVideosAppState )0;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::SetVideolistSortOrderL( TVcxMyVideosSortingOrder aSortOrder )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TVcxMyVideosSortingOrder CVcxHgMyVideosModel::VideolistSortOrderL()
    {
    return EVcxMyVideosSortingNone;
    }

// -----------------------------------------------------------------------------
//       Landscape    Portrait
// QVGA  320x240  or  240x320
// QHD   640x360  or  360x640
// VGA   640x480  or  480x640
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel::TVcxScreenResolution CVcxHgMyVideosModel::GetScreenResolution( 
        CCoeEnv* aCoeEnv )
    {
    return EVcxScreenResolutionQVGA;
    }

void CVcxHgMyVideosModel::SetVideoAsLastWatchedL( TVideoPlayerCustomMessage& aVideoInfo, 
                                                  TInt aMpxId1,
                                                  TUint32 aAgeProfile )
    {   
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::UpdateLastWatchedPlayPositionL( TUint32 aLastVideoPlayPoint )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::GetLastWatchedDataL( TVideoPlayerCustomMessage& aVideoInfo,
                                               TUint32& /*aMpxId1 */,
                                               TUint32& aAgeProfile )
    {
    }


// -----------------------------------------------------------------------------
// From MThumbnailManagerObserver, not used in Video Center.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::ThumbnailPreviewReady( MThumbnailData& /*aThumbnail*/, 
                                                 TThumbnailRequestId /*aId*/ )
    {
    }

// -----------------------------------------------------------------------------
// From MThumbnailManagerObserver
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::ThumbnailReady( TInt aError,
                                          MThumbnailData& aThumbnail,
                                          TThumbnailRequestId aId )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::HandleThumbnailReadyL( TInt aError,
                                                 MThumbnailData& aThumbnail,
                                                 TThumbnailRequestId aId )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::ResetDownloadNotification()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CThumbnailManager* CVcxHgMyVideosModel::ThumbnailManagerL()
    {
    return (CThumbnailManager*) 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::CopyVideoPlayerCustomMessageL( TVideoPlayerCustomMessage& aSource,
                                                         TVideoPlayerCustomMessage& aTarget )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TVideoPlayerCustomMessage* CVcxHgMyVideosModel::VideoPlayerCustomMessageL()
    {
    return (TVideoPlayerCustomMessage*) NULL;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CIptvLastWatchedApi* CVcxHgMyVideosModel::LastWatchedApiL()
    {
    return (CIptvLastWatchedApi*) NULL;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CIptvLastWatchedData* CVcxHgMyVideosModel::LastWatchedDataL()
    {
    return (CIptvLastWatchedData*) 0;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosModel::HandleDriveMonitorEvent( TIptvDriveMonitorEvent& /*aEvent*/ )
    {
    }
