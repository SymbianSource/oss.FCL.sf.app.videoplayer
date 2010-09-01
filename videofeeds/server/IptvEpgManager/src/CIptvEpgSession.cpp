/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Controls epg services update*
*/




#include "IptvDebug.h"
#include "CIptvEpgSession.h"
#include "IptvEngineUids.h"
#include <bautils.h>
#include "CIptvEpgPluginInterface.h"
#include "TIptvRssSearchQuery.h"
#include "CIptvEpgDatabase.h"

#include "IptvLiveEpgConstants.h"
#include "CIptvEpgDb.h"
#include "CIptvService.h"
#include "CIptvEpgService.h"
#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvEpgVodCallbackImpl.h"
#include "CIptvEpgLiveTvCallbackImpl.h"
#include "CIptvEpgVodClientImpl.h"
#include "CIptvServer.h"
#include "CIptvEpgManagerUpdater.h"
#include "CIptvServiceManager.h"
#include "CIptvEpgVodMsqQueue.h"
#include "CIptvEpgManagerImpl.h"
#include "CIptvSearchManager.h"
#include "CIptvVodDlDownloadList.h"
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
#include "CIptvVodDownloadManager.h"
#include "iptvvideostoragemyvideosadaptation.h"
#include "iptvvideostoragesideloaderobserver.h"
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

#include "CIptvMyVideosGlobalFolderId.h"
#include "CIptvMyVideosFolder.h"
#include "vcxgroupupdater.h"

_LIT( KIptvEmptyAddress, "" );

#ifdef _DEBUG
const TInt KIptvHundred = 100;
#endif

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
static void CleanupVideoPointerArray( TAny* item )
    {
    ( ( RPointerArray<CIptvMyVideosVideoBriefDetails>* ) item )->ResetAndDestroy();
    }

static void CleanupFolderPointerArray( TAny* item )
    {
    ( ( RPointerArray<CIptvMyVideosFolder>* ) item )->ResetAndDestroy();
    }

static void CleanupFolderIdPointerArray( TAny* item )
    {
    ( ( RPointerArray<CIptvMyVideosGlobalFolderId>* ) item )->ResetAndDestroy();
    }
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIptvEpgSession::CIptvEpgSession
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CIptvEpgSession::CIptvEpgSession(
    CIptvEpgDb* aEpgDb,
    TUint32 aServiceId,
    CIptvEpgVodMsqQueue* aMsgQueue,
    CIptvServer& aServer,
    TBool aPluginCanRun ) :
    iServer( aServer ),
    iUpdater( NULL ),
    iVodClient( NULL ),
    iPlugin( NULL ),
    iVodCallback( NULL ),
    iEpgDb( aEpgDb ),
    iService( NULL ),
    iMsgQueue( aMsgQueue ),
    iPluginStarted( EFalse ),
    iDeleteDatabase( EFalse ),
    iServiceId( aServiceId ),
    iPluginCanRun( aPluginCanRun ),
    iReferenceCount( 0 ),
    iInitialized( EFalse ),
    iIsIap( EFalse ),
    iIap( 0 ),
    iUpdatePending( EFalse ),
    iReady( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::CIptvEpgSession
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CIptvEpgSession::CIptvEpgSession(
    CIptvEpgDatabase* aLiveTvDb,
    TUint32 aServiceId,
    CIptvEpgVodMsqQueue* aMsgQueue,
    CIptvServer& aServer,
    TBool aPluginCanRun ) :
    iServer( aServer ),
    iUpdater( NULL ),
    iVodClient( NULL ),
    iPlugin( NULL ),
    iVodCallback( NULL ),
    iEpgDb( NULL ),
    iLiveTvDatabase( aLiveTvDb ),
    iService( NULL ),
    iMsgQueue( aMsgQueue ),
    iPluginStarted( EFalse ),
    iDeleteDatabase( EFalse ),
    iServiceId( aServiceId ),
    iPluginCanRun( aPluginCanRun ),
    iReferenceCount( 0 ),
    iInitialized( EFalse ),
    iIsIap( EFalse ),
    iIap( 0 ),
    iUpdatePending( EFalse ),
    iReady( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
TInt CIptvEpgSession::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::ConstructL" );

    TInt error = iFs.Connect();
    if ( error != KErrNone )
        {
        User::Leave( error );
        }

    iSearchManager = iServer.GetSearchManagerL( iServiceId, *this, iMsgQueue );

    error = GetServiceInformationL();

    iSearchManager->SetServiceL( iService );

    if ( ! iSearchManager->IsSearchPossible() )
        {
        // Nullify the pointer. Search not supported so no need to use the implementation.
        iServer.RemoveQueueFromSearchManagerL( iServiceId, iMsgQueue );
        iSearchManager = NULL;
        }

    if ( error == KErrNone )
        {
        if ( iEpgDb )
            {
            iVodClient = CIptvEpgVodClientImpl::NewL( *iEpgDb, *this, iSearchManager );
            }
        }

    iUpdater = CIptvEpgManagerUpdater::NewL( *this );

    User::LeaveIfError( iServer.iServiceManager->RegisterObserver( this ) );

    return error;
    }

// -----------------------------------------------------------------------------
// CIptvEpgSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvEpgSession* CIptvEpgSession::NewL(
    CIptvEpgDb* aEpgDb,
    TUint32 aServiceId,
    CIptvEpgVodMsqQueue* aMsgQueue,
    CIptvServer& aServer,
    TBool aPluginCanRun,
    TInt& aError )
    {
    CIptvEpgSession* self = new( ELeave ) CIptvEpgSession(
        aEpgDb,
        aServiceId,
        aMsgQueue,
        aServer,
        aPluginCanRun );

    CleanupStack::PushL( self );
    aError = self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIptvEpgSession* CIptvEpgSession::NewL(
    CIptvEpgDatabase* aLiveTvDb,
    TUint32 aServiceId,
    CIptvEpgVodMsqQueue* aMsgQueue,
    CIptvServer& aServer,
    TBool aPluginCanRun,
    TInt& aError )
    {
    CIptvEpgSession* self = new( ELeave ) CIptvEpgSession(
        aLiveTvDb,
        aServiceId,
        aMsgQueue,
        aServer,
        aPluginCanRun );

    CleanupStack::PushL( self );
    aError = self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::~CIptvEpgSession()
// Destructor.
// ---------------------------------------------------------------------------
//
CIptvEpgSession::~CIptvEpgSession()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::~CIptvEpgSession" );

	if ( iPluginStarted && iServer.EpgUpdating() )
        {
        iServer.SetEpgUpdating( EFalse );
        }

    delete iUpdater;
    delete iPlugin;
    delete iGroupUpdater;
    REComSession::FinalClose();
    
    delete iVodCallback;
    delete iLiveTvCallback;
    delete iService;
    delete iVodClient;

    if ( iDeleteDatabase )
        {
        // iDeleteDatabase basically means service is being deleted.
        // Therefore we also need to make sure search manager clears
        // it's data to avoid i.e. thumbnails to stay in device even though
        // the service itself has been deleted
        if ( iSearchManager )
            {
            iSearchManager->ClearAllResults();
            }

        // CIptvEpgDb will delete database file after it has been closed
        }

    if ( iSearchManager )
        {
        TRAP_IGNORE( iServer.RemoveQueueFromSearchManagerL( iServiceId, iMsgQueue ) );
        }

    iFs.Close();

    iServer.iServiceManager->DeRegisterObserver( this );

    CIptvEpgManagerImpl& epgManager = iServer.GetEpgManager();
    epgManager.CloseSession( iServiceId );
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::UpdateEcg
// ---------------------------------------------------------------------------
TInt CIptvEpgSession::UpdateEcg( TBool aForceUpdate )
    {
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvEpgSession::UpdateEcg, aForce: %d", aForceUpdate );
    
    TInt retval( KErrNone );
    TRAPD( err, retval = GetServiceInformationL() );
    if ( iService && err == KErrNone && retval == KErrNone )
        {
        switch ( iService->iServiceType )
            {
            case CIptvService::EVod:
            case CIptvService::EVodCast:
            case CIptvService::EServiceGroup:
                {
                TRAPD( error, UpdateVodEcgL( aForceUpdate ) );
                return error;
                }
            case CIptvService::ELiveTv:
                {
                TInt err( UpdateLiveEpg() );
                return err;
                }
            case CIptvService::EOther:
                break;
            default:
                break;
            }
        }

    TInt status( KErrNone );
    if ( err != KErrNone )
        {
        status = err;
        }
    else if ( retval != KErrNone )
        {
        status = retval;
        }

    return status;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::Search
// ---------------------------------------------------------------------------
//
TInt CIptvEpgSession::Search( TIptvRssSearchQuery& aSearchQuery )
    {
    if ( ! iIsIap )
        {
        // Service id = 0 because we dont have to bring this information
        // over IPC. Service ID is already on the client side.

        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "------------------CIptvEpgSession::Search------------------" );
        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "SEARCH CANCELLED! NO IAP! KIptvErrorVodNoIap sent to client." );
        IPTVLOGSTRING_LOW_LEVEL( "Update pending! Update is made when we get IAP!" );
        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "------------------CIptvEpgSession::Search------------------" );
        IPTVLOGSTRING_LOW_LEVEL( "" );

        // Now we disable update pending flag. There is a reason this is done:
        // If we start a vod UI and automatic update is initiated and user
        // is prompted for IAP selection while updating normal vod and that
        // dialog has been canceled -> user does not want to start vod update.
        // Then user select 'Search' and IAP dialog is prompted again. Now
        // if we dont disable update pending flag also vod content update
        // is started and that's something user already decided not to do.
        // This is the reason update pending flag is disabled here.
        iUpdatePending = EFalse;

        TRAP_IGNORE( SendMessageToClientL( KIptvErrorRssSearchNoIap, 0 ) );
        return KErrCouldNotConnect;
        }

    TInt error( KErrNone );

    TRAP_IGNORE( DoSearchL( error ) );

    if ( error != KErrNone )
        {
        return error;
        }

    return iSearchManager->Search( aSearchQuery );
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::DoSearchL
// ---------------------------------------------------------------------------
//
void CIptvEpgSession::DoSearchL( TInt& error )
    {
    // Refresh the service data. This is done to prevent situation where
    // search address in the service has been changed.
    GetServiceInformationL();

    SendMessageToClientL( KIptvErrorRssSearchStarted, 0 );

    if ( iSearchManager )
        {
        // Before starting a new search, we gather a list of Content IDs that have
        // been used by downloads started from previous search results. This way the
        // Search Manager knows not to re-use the same IDs again, and we avoid the
        // possible ID collisions between old and new downloads.
        //
        UpdateSearchUsedContentIdsListL();

        iSearchManager->SetServiceL( iService );
        }
    else
        {
        error = KErrNotSupported;
        SendMessageToClientL( KIptvErrorRssSearchFailed, error );
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::UpdateSearchUsedContentIdsListL
// ---------------------------------------------------------------------------
void CIptvEpgSession::UpdateSearchUsedContentIdsListL()
    {
    RArray<TInt> usedIdArray;
    CleanupClosePushL( usedIdArray );

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)    
    // First gather a list of used Content IDs from Download Manager.
    if ( iServer.iVodDownloadManager )
        {
        CIptvVodDlDownloadList* downloadList = CIptvVodDlDownloadList::NewL();

        if ( downloadList )
            {
            CleanupStack::PushL( downloadList );

            // Note, the method leaves if list is empty.
            TRAPD( dlMgrError, iServer.iVodDownloadManager->GetDownloadListL( *downloadList ) );

            if ( dlMgrError == KErrNone )
                {
                for ( TInt i = 0; i < downloadList->iList.Count(); i++ )
                    {
                    CIptvVodDlDownloadListItem* downloadItem = downloadList->iList[i];

                    // Check for downloads with our service Id, and "search" content Id.
                    if ( downloadItem->iServiceId == iService->iServiceId &&
                         downloadItem->iContentId >= KIptvRssSearchInitialContentId &&
                         downloadItem->iContentId != ( TIptvContentId ) KErrNotFound )
                        {
                        usedIdArray.InsertInOrder( downloadItem->iContentId );
                        }
                    }
                }

            CleanupStack::PopAndDestroy( downloadList );
            }
        }
#else

    // REL6: Similar functionality with MPX Collection?

#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    // Then gather a list of used Content IDs from My Videos Manager.
    TUint32 totalNum = 0;

    RPointerArray<CIptvMyVideosVideoBriefDetails> files;
    TCleanupItem cleanupVideo( CleanupVideoPointerArray, &files );
    CleanupStack::PushL( cleanupVideo );

    RPointerArray<CIptvMyVideosFolder> folders;
    TCleanupItem cleanupFolder( CleanupFolderPointerArray, &folders );
    CleanupStack::PushL( cleanupFolder );

    RPointerArray<CIptvMyVideosGlobalFolderId> folderIds;
    TCleanupItem cleanupFolderId( CleanupFolderIdPointerArray, &folderIds );
    CleanupStack::PushL( cleanupFolderId );

    CIptvMyVideosGlobalFolderId* rootFolder = CIptvMyVideosGlobalFolderId::NewRootFolderL();
    CleanupStack::PushL( rootFolder );

    iServer.GetVideoStorageAdaptationL()->GetFolderListL(
        *rootFolder,
        0,
        0,
        totalNum,
        folders );

    for ( TInt i = 0; i < folders.Count(); i++ )
        {
        CIptvMyVideosGlobalFolderId* newFolderId = CIptvMyVideosGlobalFolderId::NewLC();

        newFolderId->iDrive    = folders[i]->iFolderId->iDrive;
        newFolderId->iFolderId = folders[i]->iFolderId->iFolderId;

        folderIds.AppendL( newFolderId );
        CleanupStack::Pop( newFolderId );
        }
    folderIds.AppendL( rootFolder );
    CleanupStack::Pop( rootFolder );

    for ( TInt j = 0; j < folderIds.Count(); j++ )
        {
        iServer.GetVideoStorageAdaptationL()->GetVideoListL(
            *( folderIds[j] ),
            0,
            0,
            totalNum,
            files );

        for ( TInt i = 0; i < files.Count(); i++ )
            {
            // Check for downloads with our service Id, and "search" content Id.
            if ( files[i]->iServiceId == iService->iServiceId &&
                 files[i]->iContentId >= KIptvRssSearchInitialContentId &&
                 files[i]->iContentId != ( TIptvContentId ) KErrNotFound )
                {
                usedIdArray.InsertInOrder( files[i]->iContentId ); // Note, prevents duplicates.
                }
            }
        files.ResetAndDestroy();
        }

    CleanupStack::PopAndDestroy( &folderIds );
    CleanupStack::PopAndDestroy( &folders );
    CleanupStack::PopAndDestroy( &files );
#else

    // REL6: Similar functionality with MPX Collection?

#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    // Finally call Search Manager with our updated list.
    iSearchManager->UpdateUsedContentIdsListL( usedIdArray );

    CleanupStack::PopAndDestroy( &usedIdArray );
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::UpdateChangedContentIdsL
// ---------------------------------------------------------------------------
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
void CIptvEpgSession::UpdateChangedContentIdsL( RArray<TUint32>& aOldContentIds,
                                                RArray<TUint32>& aNewContentIds )
    {
    __ASSERT_DEBUG( aOldContentIds.Count() == aNewContentIds.Count(),
                    User::Panic( KIptvEpgDb, EIptvEngineServerBadState ) );

    if ( iServer.iVodDownloadManager )
        {
        iServer.iVodDownloadManager->UpdateChangedContentIdsL( iServiceId,
                                                               aOldContentIds,
                                                               aNewContentIds );
        }
    }
#else
void CIptvEpgSession::UpdateChangedContentIdsL( RArray<TUint32>& /*aOldContentIds*/,
                                                RArray<TUint32>& /*aNewContentIds*/ )
    {
    // REL6?
    }
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

// ---------------------------------------------------------------------------
// CIptvEpgSession::CancelSearch
// ---------------------------------------------------------------------------
TInt CIptvEpgSession::CancelSearch()
    {
    TInt retVal = KErrNone;
    if ( iSearchManager )
        {
        retVal = iSearchManager->CancelSearch();
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::SendMessageToClientL
// ---------------------------------------------------------------------------
void CIptvEpgSession::SendMessageToClientL( TInt aMsg, TInt aInfo )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::SendMessageToClientL" );

    if ( iMsgQueue )
        {
        iMsgQueue->SendMessageToClientL( aMsg, aInfo );
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::UpdateVodEcgL
// ---------------------------------------------------------------------------
//
void CIptvEpgSession::UpdateVodEcgL( TBool aForceUpdate )
    {
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvEpgSession::UpdateVodEcgL(), aForceUpdate: %d", aForceUpdate );

    TInt err( KErrNone );
    // Update is not possible if service has no address
    if ( iService && iService->iAddress.Compare( KIptvEmptyAddress ) == 0 )
        {
        IPTVLOGSTRING_LOW_LEVEL( "Update failed. Address of service is empty!! " );
        // Service id = 0 because we dont have to bring this information
        // over IPC. Service ID is already on the client side.
        if ( iService->SearchUrl().Compare( KIptvEmptyAddress ) != 0 )
            {
            // Service has only search URL, don't report failed update 
            SendMessageToClientL( KIptvVodUpdateNotStarted, 0 );
            }
        else
            {
            SendMessageToClientL( KIptvErrorEpgUpdateFailed, 0 );
            SendMessageToClientL( KIptvErrorEpgUpdateStopped, 0 );
            }
        }
    else if ( !IsVodUpdateAllowedL( err ) )
        {
        // Service id = 0 because we dont have to bring this information
        // over IPC. Service ID is already on the client side.
        SendMessageToClientL( KIptvVodUpdateNotStarted, err );
        }
    else
        {
        if ( !iUpdatePending )
            {
            IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::UpdateVodEcgL(), Manual update !" );
            RunVodPluginL( aForceUpdate );
            iForceUpdatePending = ETrue;
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::UpdateVodEcgL(), Manual update ignored !" );
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::UpdateLiveEpg
// ---------------------------------------------------------------------------
TInt CIptvEpgSession::UpdateLiveEpg()
    {
    if ( !iIsIap )
        {
        return KErrNotReady;
        }

    TRAPD( err, RunSmartVisionPluginL() );
    return err;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::RunSmartVisionPluginL
// ---------------------------------------------------------------------------
void CIptvEpgSession::RunSmartVisionPluginL()
    {
    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        }

    if ( iLiveTvCallback )
        {
        delete iLiveTvCallback;
        iLiveTvCallback = NULL;
        }

    if ( iGroupUpdater )
        {
        delete iGroupUpdater;
        iGroupUpdater = NULL;
        }

    iPlugin = CIptvEpgPluginInterface::NewL( iService->iUid );

    iLiveTvCallback = CIptvEpgLiveTvCallbackImpl::NewL( *iLiveTvDatabase );
    iLiveTvCallback->SetServiceId( iServiceId );

    if ( iPlugin && iLiveTvCallback )
        {
        iPluginStarted = ETrue;
        iPlugin->SetVodCallback( NULL );
        iPlugin->SetLiveTvCallback( iLiveTvCallback );
        iPlugin->SetServiceInformation( iService );
        iPlugin->RunPlugin();
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession EPG plugin = NULL" );

        // Service id = 0 because we dont have to bring this information
        // over IPC. Service ID is already on the client side.
        SendMessageToClientL( KIptvErrorFailedToLoadPlugin, 0 );
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::GetServiceInformationL
// ---------------------------------------------------------------------------
TInt CIptvEpgSession::GetServiceInformationL()
    {
    IPTVLOGSTRING2_LOW_LEVEL( "CIptvEpgSession::GetServiceInformationL:: Service ID = %d", iServiceId );

    CIptvService* service = NULL;
    service = iServer.iServiceManager->GetServiceL( iServiceId, ETrue /* do open/close */ );
    if ( !service )
        {
        return KIptvErrorNoService;
        }

    CleanupStack::PushL( service );

    // Create EPG service
    delete iService;
    iService = NULL;
    iService = CIptvEpgService::NewL();
    iService->iAddress.Copy( service->GetAddress() );
    iService->SetSearchUrlL( service->SearchUrl() );
    iService->iUserName.Copy( service->GetUserName() );
    iService->iPassword.Copy( service->GetPassword() );
    iService->iUid = service->GetEpgPluginUid();
    iService->iServiceType = service->GetType();
    iService->iServiceId = iServiceId;
    iService->iIap = iIap;
    if ( CIptvService::EServiceGroup == service->GetType() )
        {
        iService->iLastUpdated = service->GetEpgUpdateTimeL();
        }

    CleanupStack::PopAndDestroy( service );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::SetReady
// -----------------------------------------------------------------------------
void CIptvEpgSession::SetReady()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::SetReady" );

    iReady = ETrue;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::IsReadyL
// ---------------------------------------------------------------------------
TBool CIptvEpgSession::IsReadyL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::IsReadyL" );

    TBool ready = EFalse;

    if ( iService )
        {
        switch ( iService->iServiceType )
            {
            case CIptvService::EVod:
            case CIptvService::EVodCast:
            case CIptvService::EServiceGroup:
            case CIptvService::ESearch:
                {
                ready = ETrue;
                }
                break;
            case CIptvService::ELiveTv:
                break;
            case CIptvService::EOther:
                break;
            default:
                break;
            }
        }

    return ready;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::IsValidVodDbAvailableL
// ---------------------------------------------------------------------------
//
TBool CIptvEpgSession::IsValidVodDbAvailableL(
    TTime& aLastUpdated,
    TTime& aInterval )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::IsValidVodDbAvailableL" );

    TInt error = iEpgDb->GetServiceInformationL( aLastUpdated, aInterval );
    if ( error != KErrNone )
        {
        if ( error == KIptvErrorGetUpdateInformationLNotFound )
            {
            return EFalse;
            }
        }

    // check if no data in database
    if ( aLastUpdated.Int64() == 0 || aInterval.Int64() == 0 )
        {
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::CheckIsVodUpdateNeededL
// ---------------------------------------------------------------------------
void CIptvEpgSession::CheckIsVodUpdateNeededL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::CheckIsVodUpdateNeededL() in" );

    TInt err( KErrNone );
    if ( !IsVodUpdateAllowedL( err ) )
        {
        // Service id = 0 because we dont have to bring this information
        // over IPC. Service ID is already on the client side.
       
        SendMessageToClientL( KIptvVodUpdateNotStarted, err );
        return;
        }

    TTime lastUpdated;
    TTime interval;
    TBool update = EFalse;

    if ( !IsValidVodDbAvailableL( lastUpdated, interval ) )
        {
        update = ETrue;
        }

    if ( update )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::CheckIsVodUpdateNeededL(), UPDATE NEEDED! DATABASE MISSING OR OLD DATA!" );

        RunVodPluginL();
        iForceUpdatePending = EFalse;
        return;
        }

#ifdef _DEBUG

    TBuf<KIptvHundred> datePrint;
    _LIT( KIptvDatePrint, "%D%M%Y%/0%1%/1%2%/2%3%/3" );
    lastUpdated.FormatL( datePrint, KIptvDatePrint );

    TBuf<KIptvHundred> timePrint;
    _LIT( KIptvTimePrint, "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B" );
    lastUpdated.FormatL( timePrint, KIptvTimePrint );

    TBuf<KIptvHundred> intervalDate;
    interval.FormatL( intervalDate, KIptvDatePrint );

    TBuf<KIptvHundred> intervalTime;
    interval.FormatL( intervalTime, KIptvTimePrint );

    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::CheckIsVodUpdateNeededL()" );
    IPTVLOGSTRING3_LOW_LEVEL( "EPG database was last updated : %S %S", &datePrint, &timePrint );
    IPTVLOGSTRING3_LOW_LEVEL( "Data is valid                 : %S %S", &intervalDate, &intervalTime );

#endif

    TTimeIntervalMicroSeconds add( interval.Int64() );
    lastUpdated += add;

    TTime current;
    current.UniversalTime();

    if ( current < lastUpdated )
        {

#ifdef _DEBUG

        TBuf<KIptvHundred> nextDate;
        lastUpdated.FormatL( nextDate, KIptvDatePrint );

        TBuf<KIptvHundred> nextTime;
        lastUpdated.FormatL( nextTime, KIptvTimePrint );

        IPTVLOGSTRING3_LOW_LEVEL( "Next update:                  : %S %S", &nextDate, &nextTime );
        IPTVLOGSTRING_LOW_LEVEL( "UPDATE NOT NEEDED!" );

#endif

        // Service id = 0 because we dont have to bring this information
        // over IPC. Service ID is already on the client side.
        SendMessageToClientL( KIptvVodUpdateNotStarted, 0 );
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::CheckIsVodUpdateNeededL(), UPDATE NEEDED! DATABASE MISSING OR OLD DATA!" );

        RunVodPluginL();
        iForceUpdatePending = EFalse;
        }

    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::CheckIsVodUpdateNeededL(), out" );
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::IsUpdateAllowed
// ---------------------------------------------------------------------------
//
TBool CIptvEpgSession::IsVodUpdateAllowedL( TInt& aErrorCode )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::IsVodUpdateAllowedL" );
    aErrorCode = KErrNone;

    if ( iServer.EpgUpdating() )
        {
        // This enables and requires retry.
        if ( IsVodUpdateNeededL() )
            {
            aErrorCode = KErrServerBusy;
            }
        return EFalse;
        }

    if ( !iService )
        {
        return EFalse;
        }

    if ( iService->iAddress.Compare( KIptvEmptyAddress ) == 0 )
        {
        return EFalse;
        }

    if ( !iPluginCanRun )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::IsVodUpdateAllowedL -- iPluginCanRun == EFalse" );
        return EFalse;
        }

    if ( iVodCallback )
        {
        if ( iVodCallback->IsPluginRunning() )
            {
            IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::IsVodUpdateAllowedL -- Plugin already running!" );
            return EFalse;
            }
        }

    if ( iPluginStarted )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::IsVodUpdateAllowedL -- Plugin already started!" );
        return EFalse;
        }

    if ( iPlugin )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::IsVodUpdateAllowedL -- Plugin already started!" );
        return EFalse;
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::IsVodUpdateAllowedL -- Plugin is NULL" );
        }

    return ETrue;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::ResetGlobalId
// ---------------------------------------------------------------------------
TInt CIptvEpgSession::ResetGlobalId( CIptvMyVideosGlobalFileId& aId )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::ResetGlobalId" );

    TInt error = KErrNotFound;

    if ( iEpgDb )
        {
        error = iEpgDb->ResetGlobalId( aId );
        }

    if ( iSearchManager )
        {
        TInt error2 = iSearchManager->ResetGlobalId( aId );
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::SetGlobalId
// ---------------------------------------------------------------------------
TInt CIptvEpgSession::SetGlobalId(
    TUint32 aContentKey,
    CIptvMyVideosGlobalFileId& aId,
    TUint32 aIndex )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::SetGlobalId" );

    TInt error = iEpgDb->SetGlobalId( aContentKey, aId, aIndex );

    if ( iSearchManager )
        {
        iSearchManager->SetGlobalId( aContentKey, aId, aIndex );
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::DeleteDatabaseFile
// ---------------------------------------------------------------------------
void CIptvEpgSession::DeleteDatabaseFile()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::DeleteDatabaseFile" );

    if ( iEpgDb )
        {
        iEpgDb->DeleteDatabaseFiles();
        }

    iDeleteDatabase = ETrue;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::RunVodPluginL
// ---------------------------------------------------------------------------
//
void CIptvEpgSession::RunVodPluginL( TBool aForceUpdate )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::RunVodPluginL" );

    if ( !iIsIap )
        {
        // Service id = 0 because we dont have to bring this information
        // over IPC. Service ID is already on the client side.

        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "------------------CIptvEpgSession::RunVodPluginL------------------" );
        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "UPDATE CANCELLED! NO IAP! KIptvErrorVodNoIap sent to client." );
        IPTVLOGSTRING_LOW_LEVEL( "Update pending! Update is made when we get IAP!" );
        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "------------------CIptvEpgSession::RunVodPluginL------------------" );
        IPTVLOGSTRING_LOW_LEVEL( "" );

        iUpdatePending = ETrue;
        SendMessageToClientL( KIptvErrorVodNoIap, 0 );
        return;
        }

    iUpdatePending = EFalse;

    if ( !iService )
        {
        return;
        }
    
    if ( CIptvService::EServiceGroup == iService->iServiceType )
        {
        RunVodPluginForGroupL( aForceUpdate );
        }
    else
        {
        RunVodPluginForFeedL( aForceUpdate );
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::RunVodPluginForFeedL
// ---------------------------------------------------------------------------
//
void CIptvEpgSession::RunVodPluginForFeedL( TBool aForceUpdate )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::RunVodPluginForFeedL" );

    if ( iGroupUpdater )
        {
        delete iGroupUpdater;
        iGroupUpdater = NULL;
        }

    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        }

    if ( iVodCallback )
        {
        delete iVodCallback;
        iVodCallback = NULL;
        }

    iPlugin = CIptvEpgPluginInterface::NewL( iService->iUid );
    iVodCallback = CIptvEpgVodCallbackImpl::NewL( *iEpgDb, iMsgQueue, *this );

    if ( iPlugin && iVodCallback )
        {
        TTime lastUpdated;
        TTime interval;

        if ( iEpgDb )
            {
            iEpgDb->GetServiceInformationL( lastUpdated, interval );
            }

        iService->iLastUpdated = lastUpdated;

        // Service id = 0 because we dont have to bring this information
        // over IPC. Service ID is already on the client side.
        SendMessageToClientL( KIptvVodUpdateStarted, 0 );

        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "------------------CIptvEpgSession::RunVodPluginForFeedL------------------" );
        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "UPDATE STARTED!!! KIptvVodUpdateStarted sent to client!" );
        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "------------------CIptvEpgSession::RunVodPluginForFeedL------------------" );

        iUpdater->Start();
        iPluginStarted = ETrue;
        iPlugin->SetVodCallback( iVodCallback );
        iPlugin->SetLiveTvCallback( NULL );

        if ( aForceUpdate )
            {
            iService->iLastUpdated = TTime( 0L );
            }
        iPlugin->SetServiceInformation( iService );
        iPlugin->RunPlugin();
        iServer.SetEpgUpdating( ETrue );
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession EPG plugin = NULL" );
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::RunVodPluginForGroupL
// ---------------------------------------------------------------------------
//
void CIptvEpgSession::RunVodPluginForGroupL( TBool aForceUpdate )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::RunVodPluginForGroupL" );

    if ( iGroupUpdater )
        {
        delete iGroupUpdater;
        iGroupUpdater = NULL;
        }

    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        }

    iGroupUpdater = CVcxGroupUpdater::NewL(
        iServer,
        *this,
        *iMsgQueue );

    if ( iGroupUpdater )
        {
        // Service id = 0 because we dont have to bring this information
        // over IPC. Service ID is already on the client side.
        SendMessageToClientL( KIptvVodUpdateStarted, 0 );

        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "------------------CIptvEpgSession::RunVodPluginForGroupL------------------" );
        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "UPDATE STARTED!!! KIptvVodUpdateStarted sent to client!" );
        IPTVLOGSTRING_LOW_LEVEL( "" );
        IPTVLOGSTRING_LOW_LEVEL( "------------------CIptvEpgSession::RunVodPluginForGroupL------------------" );

        iUpdater->Start();
        iPluginStarted = ETrue;

        if ( aForceUpdate )
            {
            iService->iLastUpdated = TTime( 0L );
            }
        
        iGroupUpdater->StartGroupUpdateL( iService );
        iServer.SetEpgUpdating( ETrue );
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::VodPluginStopped
// ---------------------------------------------------------------------------
//
void CIptvEpgSession::VodPluginStopped()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::VodPluginStopped" );

    iUpdater->Stop();
    iPluginStarted = EFalse;
    delete iVodCallback;
    iVodCallback = NULL;
    delete iPlugin;
    iPlugin = NULL;
    delete iGroupUpdater;
    iGroupUpdater = NULL;

    if ( iSearchManager )
        {
        if ( ! iSearchManager->IsPluginRunning() )
            {
            REComSession::FinalClose();
            }
        }
    else
        {
        REComSession::FinalClose();
        }

    iServer.SetEpgUpdating( EFalse );

    // Service id = 0 because we dont have to bring this information
    // over IPC. Service ID is already on the client side.
    TRAP_IGNORE( SendMessageToClientL( KIptvErrorEpgUpdateStopped, 0 ) );
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::GetVodClientImpl
// ---------------------------------------------------------------------------
//
CIptvEpgVodClientImpl* CIptvEpgSession::GetVodClientImplL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::GetVodClientImpl" );

    return iVodClient;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::CheckIsPluginRunning
// ---------------------------------------------------------------------------
//
void CIptvEpgSession::CheckIsPluginRunning()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::CheckIsPluginRunning" );

    TBool running( EFalse );

    if ( iGroupUpdater )
        {
        running = iGroupUpdater->IsGroupUpdateOngoing();
        }

    if ( iVodCallback && !running )
        {
        running = iVodCallback->IsPluginRunning();
        }

    if ( !running )
        {
        VodPluginStopped();
        }

    if ( iLiveTvCallback )
        {
        if ( !iLiveTvCallback->IsPluginRunning() )
            {
            // check
            // LiveTvPluginStopped()
            }
        }
    else
        {
        // check
        // LiveTvPluginStopped()
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::InitializeL
// ---------------------------------------------------------------------------
void CIptvEpgSession::InitializeL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::InitializeL" );

    if ( iService )
        {
        if ( !iInitialized )
            {
            iInitialized = ETrue;
            switch ( iService->iServiceType )
                {
                case CIptvService::EVod:
                case CIptvService::EVodCast:
                    CheckIsVodUpdateNeededL();
                    break;
                case CIptvService::ELiveTv:
                    // check
                    // CheckIsLiveTvUpdateNeededL?
                    break;
                case CIptvService::EOther:
                    break;
                default:
                    break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::SetIapL
// ---------------------------------------------------------------------------
void CIptvEpgSession::SetIapL( TUint32 aIap )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::SetIapL" );

    if ( iService )
        {
        iService->iIap = aIap;
        iIap = aIap;
        iIsIap = ETrue;
        
#if !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
        if( iIap == 0 )
            {
            iIsIap = EFalse;
            }
#endif // !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)
        
        if ( iUpdatePending )
            {
            RunVodPluginL( iForceUpdatePending );
            iForceUpdatePending = EFalse;
            }
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::UpdateServiceThumbnailL
// ---------------------------------------------------------------------------
//
void CIptvEpgSession::UpdateServiceThumbnailL(
    const TDesC& aIconFilePath, const TBool aGrouped )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::UpdateServiceThumbnailL" );

    CIptvService* service = NULL;
    service = iServer.iServiceManager->GetServiceL( iServiceId, ETrue /* do open/close */ );
    if ( service )
        {
        CleanupStack::PushL( service );
        if ( aGrouped || !( service->GetFlags() & CIptvService::EGroupedService ) )
            {
            service->SetIconPath( aIconFilePath );
            iServer.iServiceManager->UpdateServiceL( *service );
            }
        CleanupStack::PopAndDestroy( service );
        }

    return;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::IsVodUpdateNeededL()
// ---------------------------------------------------------------------------
//
TBool CIptvEpgSession::IsVodUpdateNeededL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::IsVodUpdateNeededL()" );
    
    TTime lastUpdated;
    TTime interval;
    
    if ( !IsValidVodDbAvailableL( lastUpdated, interval ) )
        {
        return ETrue;
        }

    TTimeIntervalMicroSeconds add( interval.Int64() );
    lastUpdated += add;

    TTime current;
    current.UniversalTime();

    if ( current > lastUpdated )
        {
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::VodPluginCancelled()
// ---------------------------------------------------------------------------
//
void CIptvEpgSession::VodPluginCancelled()
    {
    if ( iPluginStarted )
        {
        IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::VodPluginCancelled" );
        iUpdater->Stop();
        iPluginStarted = EFalse;
        delete iVodCallback;
        iVodCallback = NULL;
        delete iPlugin;
        iPlugin = NULL;
        
        if ( iSearchManager )
            {
            if ( ! iSearchManager->IsPluginRunning() )
                {
                REComSession::FinalClose();
                }
            }
        else
            {
            REComSession::FinalClose();
            }
        
        iServer.SetEpgUpdating( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// CIptvEpgSession::HandleSmEvent
// ---------------------------------------------------------------------------
//
void CIptvEpgSession::HandleSmEvent( CIptvSmEvent& aEvent )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::HandleSmEvent" );

    if ( aEvent.iEvent == CIptvSmEvent::EServiceModified )
        {
        if ( aEvent.iServiceId == iServiceId )
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "CIptvEpgSession::HandleSmEvent EServiceModified --> Cancel plugin" );
            if ( iPluginStarted )
                {
                VodPluginStopped();
                TRAP_IGNORE( SendMessageToClientL(
                    KIptvErrorEpgUpdateFailed, EIptvDlGeneral ) );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CIptvEpgSession::SetLastUpdateTimeL( const TTime& aLastUpdate )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvEpgSession::SetLastUpdateTimeL" );
    TInt err( KErrNone );
    if ( iEpgDb )
        {
        TTime tempLastUpdate( 0 );
        TTime interval( 0 );
        err = iEpgDb->GetServiceInformationL( tempLastUpdate, interval );
        if ( err == KErrNone )
            {
            err = iEpgDb->UpdateServiceInformationL( aLastUpdate, interval );
            }
        }
    return err;
    }

