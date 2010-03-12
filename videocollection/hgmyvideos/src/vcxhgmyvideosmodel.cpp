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
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideoscollectionclient.h"
#include "vcxhgmyvideosdownloadclient.h"
#include "thumbnaildata.h"
#include "vcxhgmyvideoscenrepkeys.h"
#include "vcxhgmyvideosthumbnailmanager.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosModel::CVcxHgMyVideosModel()
  : iAppState( EVcxMyVideosAppStateUnknown ),
    iPreviousAppState( EVcxMyVideosAppStateUnknown ),
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
    iTnManager = CVcxHgMyVideosThumbnailManager::NewL();
    InitMyVideosCenRepL();
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
    delete iMyVideosCenRep;
    delete iTnManager;
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
        TInt err = iFsSession.Connect();
        if ( err == KErrNone )
            {
            // For sending handles to thumbnailmanager
            err = iFsSession.ShareProtected();
            }
        if ( err != KErrNone )
            {
            iFsSession.Close();
            User::Leave( err );
            }
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
// CVcxHgMyVideosModel::GetLastWatchedIdL()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosModel::GetLastWatchedIdL( TInt& aId )
    {
    if ( ! iCollectionCenRep )
        {
        iCollectionCenRep = CRepository::NewL( TUid::Uid( KVcxMyVideosCollectionCenrepUid ) );
        }

    return iCollectionCenRep->Get( KVcxMyVideosCollectionCenrepKeyLastWatchedMpxId, aId );
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
// CVcxHgMyVideosModel::ThumbnailManager()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosThumbnailManager& CVcxHgMyVideosModel::ThumbnailManager() const
    {
    return *iTnManager;
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

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::GetMyVideosCustomizationIntL()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosModel::GetMyVideosCustomizationInt( const TInt& aKey, 
                                                       TInt& aValue )
    {
    if ( iMyVideosCenRep )
        {
        return iMyVideosCenRep->Get( aKey, aValue );
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::GetMyVideosCustomizationString()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosModel::GetMyVideosCustomizationString( const TInt& aKey, 
                                                          TDes& aValue )
    {
    if ( iMyVideosCenRep )
        {
        return iMyVideosCenRep->Get( aKey, aValue );
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosModel::InitMyVideosCenRepL()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosModel::InitMyVideosCenRepL()
    {
    TInt retVal = KErrNone;
    if ( !iMyVideosCenRep )
        {
        TRAPD( error, iMyVideosCenRep = CRepository::NewL( KMyVideosMainViewCenRepUid ) );
        retVal = error;
        }
    return retVal;
    }
