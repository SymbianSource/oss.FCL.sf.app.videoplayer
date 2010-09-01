/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Server main.*
*/




#include "CIptvServer.h"
#include "CIptvServerSession.h"
#include "CIptvServiceManager.h"
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
#include "CIptvVodDownloadManager.h"
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
#include "CIptvEpgManagerImpl.h"
#include "MIptvMyVideos.h"
#include "CIptvNetworkSelection.h"
#include "IptvDebug.h"

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
#include "iptvvideostoragemyvideosadaptation.h"
#include "iptvvideostoragemyvideoscallback.h"
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

#include "CIptvSearchManager.h"
#include "iptvvodscheduleddownloadscheduler.h"

const TInt KIptvServerShutdownDelay( 1000000 );       // 1 second
const TInt KIptvServerShutdownPollPeriod( 30000000 ); // 30 seconds

// Platform security. Custom check is applied to all IPCs.

static const int KRangeCount( 1 );

static const TInt SecurityRanges[KRangeCount] =
    {
    0, // Range is from 0 to KMaxTInt
    };

static const TUint8 SecurityRangesPolicy[KRangeCount] =
    {
    CPolicyServer::ECustomCheck
    };

static const CPolicyServer::TPolicy Policy =
    {
    CPolicyServer::EAlwaysPass,
    KRangeCount,
    SecurityRanges,
    SecurityRangesPolicy,
    NULL,
    };

// ---------------------------------------------------------------------------
// E32Main()
// Provides the API for the operating system to start the executable.
// Returns the address of the function to be called.
// ---------------------------------------------------------------------------
//
TInt E32Main()
    {
    return CIptvServer::ThreadFunction( NULL );
    }

// --------------------------------------------------------------------------
// CIptvServer::ThreadFunction()
// Main function for the server thread.
// --------------------------------------------------------------------------
//
TInt CIptvServer::ThreadFunction( TAny* /*aNone*/ )
    {

#ifdef _DEBUG
__UHEAP_MARK;
#endif

    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !( cleanupStack ) )
        {
        PanicServer( ECreateTrapCleanup );
        }

    TInt functionError = KErrNone;

    TRAPD( error, functionError = ThreadFunctionL() );

    delete cleanupStack;
    cleanupStack = NULL;

#ifdef _DEBUG
//
// This is to cause panic on server exit for any unreleased heap object.
// You can use for exaple Symbian HookLogger to detect the leaked object
// and the code point were it was allocated.
// Hexadecimal pointer of the leaked object is shown in the screen and
// HookLogger shows the object.
//
__UHEAP_MARKEND;
#endif

    if ( error != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvServer::ThreadFunction() returning %d", error );
        return error;
        }
    else if ( functionError != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvServer::ThreadFunction() returning %d", functionError );
        return functionError;
        }
    else
        {
        return KErrNone;
        }
    }

// --------------------------------------------------------------------------
// CIptvServer::ThreadFunctionL()
// Second stage startup for the server thread.
// --------------------------------------------------------------------------
//
TInt CIptvServer::ThreadFunctionL()
    {
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( activeScheduler );

    // Install active scheduler
    // We don't need to check whether an active scheduler is already
    // installed as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );

    CIptvServer* iptvServer = NULL;
    TRAPD( error, iptvServer = CIptvServer::NewL() ); // Anonymous

    if ( error == KErrNone )
        {
        CleanupStack::PushL( iptvServer );
        }

    RSemaphore semaphore;
    User::LeaveIfError(
        semaphore.OpenGlobal( KIptvServerSemaphoreName , EOwnerProcess ) );

    // Semaphore opened ok
    semaphore.Signal();
    semaphore.Close();

    // Start handling requests
    if ( error == KErrNone )
        {
        CActiveScheduler::Start();
        CleanupStack::PopAndDestroy( iptvServer );
        }

    CleanupStack::PopAndDestroy( activeScheduler );
    return error;
    }

// --------------------------------------------------------------------------
// CIptvServer::NewL()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CIptvServer* CIptvServer::NewL()
    {
    CIptvServer* self = CIptvServer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CIptvServer::NewLC()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CIptvServer* CIptvServer::NewLC()
    {
    CIptvServer* iptvServer =
        new ( ELeave ) CIptvServer( EPriorityNormal, Policy );
    CleanupStack::PushL( iptvServer );
    iptvServer->ConstructL();
    return iptvServer;
    }

// --------------------------------------------------------------------------
// CIptvServer::CIptvServer()
// C++ default constructor can NOT contain any code, that might leave.
// --------------------------------------------------------------------------
//

CIptvServer::CIptvServer( TInt aPriority, const TPolicy& aPolicy )
  : CPolicyServer( aPriority, aPolicy ),
    iVodScheduledDlScheduler( NULL ),
    iEpgUpdating(EFalse)
    {
    }

// --------------------------------------------------------------------------
// CIptvServer::ConstructL()
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CIptvServer::ConstructL()
    {
    iServiceManager     = CIptvServiceManager::NewL();
    iEpgManager         = CIptvEpgManagerImpl::NewL( *this );
    iNetworkSelection   = CIptvNetworkSelection::NewL( *this );
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)    
    iVodDownloadManager = CIptvVodDownloadManager::NewL( *this );
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    
    iVodScheduledDlScheduler =
        CIptvVODScheduledDownloadScheduler::NewL( *iServiceManager );

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    GetVideoStorageAdaptationL();

    GetVideoStorageMyVideosAdaptationL()->RegisterClientObserver( iActiveSpaceUpdater );

#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    
    // Trapped because this failure must not stop our server startup.
    TRAP_IGNORE(
        {
        iActiveSpaceUpdater->InitialiseL( GetVideoStorageAdaptationL(), iServiceManager );
        });

    StartL( KIptvServerName );
    }

// --------------------------------------------------------------------------
// CIptvServer::NewSessionL()
// Creates a new server session.
// --------------------------------------------------------------------------
//
CSession2* CIptvServer::NewSessionL(
    const TVersion& aVersion,
    const RMessage2& aMessage ) const
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvServer::NewSessionL()" );

    // Check we are the right version
    if ( !User::QueryVersionSupported(
            TVersion( KIptvServMajorVersionNumber,
                      KIptvServMinorVersionNumber,
                      KIptvServBuildVersionNumber ),
            aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    // Make new session
    RThread client;
    User::LeaveIfError( aMessage.Client( client ) );

    IPTVLOGSTRING_LOW_LEVEL( "CIptvServer::NewSessionL() exit" );

    return CIptvServerSession::NewL(
        client,
        *const_cast<CIptvServer*> ( this ) );

    }

// --------------------------------------------------------------------------
// CIptvServer::CustomSecurityCheckL()
// --------------------------------------------------------------------------
//
CPolicyServer::TCustomResult CIptvServer::CustomSecurityCheckL(
    const RMessage2& aMsg,
    TInt& /*aAction*/,
    TSecurityInfo& /*aMissing*/ )
    {
    // Check message range.
    if ( aMsg.Function() > EIptvEngineNone &&
         aMsg.Function() < EIptvEngineLast )
        {
        // These basic capabilities are needed for all messages.
        if ( ! aMsg.HasCapability( ECapabilityReadUserData ) ||
             ! aMsg.HasCapability( ECapabilityWriteUserData ) ||
             ! aMsg.HasCapability( ECapabilityReadDeviceData ) )
            {
            IPTVLOGSTRING2_HIGH_LEVEL(
                "CIptvServer::CustomSecurityCheckL() No capability for message %d!!!",
                aMsg.Function() );
            return EFail;
            }

        // Service Management special capabilities.
        // WriteDeviceData
        if ( aMsg.Function() >= EIptvEngineSmAddServiceReq &&
             aMsg.Function() <= EIptvEngineNsCancel )
            {
            if ( ! aMsg.HasCapability( ECapabilityWriteDeviceData ) )
                {
                IPTVLOGSTRING2_HIGH_LEVEL(
                    "CIptvServer::CustomSecurityCheckL() No capability for message %d!!!",
                    aMsg.Function() );
                return EFail;
                }
            }

        // Vod Content special capabilities.
        // NetworkServices
        else if ( aMsg.Function() >= EIptvEngineVodContentSetIap &&
                  aMsg.Function() <= EIptvEngineVodContentRssSearchCancel )
            {
            if ( ! aMsg.HasCapability( ECapabilityNetworkServices ) )
                {
                IPTVLOGSTRING2_HIGH_LEVEL(
                    "CIptvServer::CustomSecurityCheckL() No capability for message %d!!!",
                    aMsg.Function() );
                return EFail;
                }
            }

        // Vod DL special capabilities.
        // WriteDeviceData
        // NetworkServices
        else if ( aMsg.Function() >= EIptvEngineVodDlGetEventSizeReq &&
                  aMsg.Function() <= EIptvEngineVodDlGetDownloadProgressReq )
            {
            if ( ! aMsg.HasCapability( ECapabilityWriteDeviceData ) ||
                 ! aMsg.HasCapability( ECapabilityNetworkServices ) )
                {
                IPTVLOGSTRING2_HIGH_LEVEL(
                    "CIptvServer::CustomSecurityCheckL() No capability for message %d!!!",
                    aMsg.Function() );
                return EFail;
                }
            }

        return EPass;
        }
    else
        {
        IPTVLOGSTRING2_HIGH_LEVEL(
            "CIptvServer::CustomSecurityCheckL() Message %d outside known range!!!",
            aMsg.Function() );
        return EFail;
        }
    }

// --------------------------------------------------------------------------
// CIptvServer::PanicClient()
// Panics the client.
// --------------------------------------------------------------------------
//
void CIptvServer::PanicClient( const RMessage2& aMessage, TInt aReason )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvServer::PanicClient()" );

    aMessage.Panic( KIptvServer, aReason );
    }

// --------------------------------------------------------------------------
// CIptvServer::~CIptvServer()
// Destructor.
// --------------------------------------------------------------------------
//
CIptvServer::~CIptvServer()
    {
    IPTVLOGSTRING_LOW_LEVEL( "~CIptvServer()" );

    if ( iShutdownTimer )
        {
        iShutdownTimer->Cancel();
        }

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
  
    if ( iVideoStorageAdaptation )
        {
        iVideoStorageAdaptation->UnregisterClientObserver( iActiveSpaceUpdater );
        }

#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    
    delete iVodScheduledDlScheduler;
    delete iShutdownTimer;
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    delete iVodDownloadManager;
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    delete iServiceManager;
    delete iNetworkSelection;
    delete iEpgManager;
    
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    delete iVideoStorageAdaptation;
    delete iVideoStorageCallback;
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    delete iActiveSpaceUpdater;
    
    for ( TInt i = 0; i < iSearchManagers.Count(); i++ )
        {
        delete iSearchManagers[i].iManager;
        iSearchManagers[i].iManager = NULL;
        }
    iSearchManagers.Reset();
    }

// --------------------------------------------------------------------------
// CIptvServer::PanicServer()
// Panics the server.
// --------------------------------------------------------------------------
//
void CIptvServer::PanicServer( TInt aReason )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvServer::PanicServer()" );

    User::Panic( KIptvServer, aReason );
    }


// --------------------------------------------------------------------------
// CIptvServer::IncrementSessions()
// Increments the count of the active sessions for this server.
// --------------------------------------------------------------------------
//
void CIptvServer::IncrementSessions()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvServer::IncrementSessions()" );

    iSessionCount++;
    }

// --------------------------------------------------------------------------
// CIptvServer::DecrementSessionsL()
// Decrements the count of the active sessions for this server.
// --------------------------------------------------------------------------
//
void CIptvServer::DecrementSessionsL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvServer::DecrementSessionsL()" );

    iSessionCount--;

    if ( iSessionCount == 0 )
        {
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)        
        TInt activeDownloadCount =
            iVodDownloadManager->GetActiveDownloadCount(
                ETrue /* ignore paused ones */ );
        IPTVLOGSTRING2_HIGH_LEVEL(
            "DecrementSessions:: Active downloads: %d", activeDownloadCount );
        TBool activeNotifications =
            iVodDownloadManager->IsActiveNotifications();

        if ( activeDownloadCount > 0 && !activeNotifications )
            {
            iVodDownloadManager->LaunchExitingNotificationL();
            }
#else
        // REL6 implementation?
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

        if ( iShutdownTimer )
            {
            iShutdownTimer->Cancel();
            }
        else
            {
            iShutdownTimer =
                CIptvTimer::NewL( CActive::EPriorityUserInput, *this );
            }
        iShutdownTimer->After( KIptvServerShutdownDelay );
        }

    IPTVLOGSTRING_LOW_LEVEL( "CIptvServer::DecrementSessionsL() exit" );
    }

// --------------------------------------------------------------------------
// CIptvServer::TimerExpired
// From MIptvTimerObserver interface.
// --------------------------------------------------------------------------
//
void CIptvServer::TimerExpired( CIptvTimer* /*aTimer*/ )
    {
    IPTVLOGSTRING_HIGH_LEVEL( "CIptvServer::TimerExpired()" );

    // if a new client has connected we cancel shutdown process
    if ( iSessionCount > 0 )
        {
        IPTVLOGSTRING_HIGH_LEVEL(
            "CIptvServer:: a new client has connected, cancelling shutdown" );
        return;
        }

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)    
    TInt activeDownloadCount =
        iVodDownloadManager->GetActiveDownloadCount(
            ETrue /* ignore paused ones */ );
    IPTVLOGSTRING2_HIGH_LEVEL(
        "CIptvServer::TimerExpired() Active downloads: %d",
        activeDownloadCount );

    TBool activeNotifications = iVodDownloadManager->IsActiveNotifications();

    TBool myVideosManagerBusy = EFalse;
#else
    TInt activeDownloadCount = 0;
    TBool activeNotifications = EFalse;
    TBool myVideosManagerBusy = EFalse;    
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    if ( iVideoStorageAdaptation )
        {
        myVideosManagerBusy =
            iVideoStorageAdaptation->IsMyVideosManagerBusy();
        }

#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    
    if ( ( activeDownloadCount > 0 ) ||
           activeNotifications ||
           myVideosManagerBusy )
        {
        if ( iShutdownTimer->IsActive() )
            {
            iShutdownTimer->Cancel();
            }
        iShutdownTimer->After( KIptvServerShutdownPollPeriod );
        }
    else
        {
        IPTVLOGSTRING_HIGH_LEVEL(
            "CIptvServer:: clients exited, no active downloads or notifications, My Videos not busy." );
        IPTVLOGSTRING_HIGH_LEVEL(
            "CIptvServer:: -> Starting server shutdown." );
        CActiveScheduler::Stop();
        }

    IPTVLOGSTRING_HIGH_LEVEL( "CIptvServer::TimerExpired() exit" );
    }

// --------------------------------------------------------------------------
// CIptvServer::GetEpgManager()
// --------------------------------------------------------------------------
//
CIptvEpgManagerImpl& CIptvServer::GetEpgManager()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CIptvServer::GetEpgManager()" );

    return *iEpgManager;
    }

// --------------------------------------------------------------------------
// Gets VideoStorageAdaptation. If there aren't one already, it's created.
// --------------------------------------------------------------------------
//
MIptvMyVideos* CIptvServer::GetVideoStorageAdaptationL()
    {
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    if ( !iVideoStorageAdaptation )
        {
        if ( !iActiveSpaceUpdater )
            {
            iActiveSpaceUpdater = CIptvActiveSpaceUpdater::NewL();
            }

        if ( !iVideoStorageCallback )
            {
            iVideoStorageCallback = CIptvVideoStorageCallback::NewL(
                    *this );
            }

        iVideoStorageAdaptation =
            CIptvVideoStorageMyVideosAdaptation::NewL(
                iVideoStorageCallback );
        }
    
    return iVideoStorageAdaptation;

#else

    if ( !iActiveSpaceUpdater )
        {
        iActiveSpaceUpdater = CIptvActiveSpaceUpdater::NewL();
        }    
    
    return NULL;
    
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    }

// --------------------------------------------------------------------------
// Gets VideoStorageAdaptation. If there aren't one already, it's created.
// This is for server internal use.
// --------------------------------------------------------------------------
//
CIptvVideoStorageMyVideosAdaptation*
    CIptvServer::GetVideoStorageMyVideosAdaptationL()
    {
    
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    
    if ( !iVideoStorageAdaptation )
        {
        if ( !iVideoStorageCallback )
            {
            iVideoStorageCallback = CIptvVideoStorageCallback::NewL( *this );
            }

        iVideoStorageAdaptation =
            CIptvVideoStorageMyVideosAdaptation::NewL(
                iVideoStorageCallback );
        }

    return iVideoStorageAdaptation;
    
#else    
    
    return NULL;
    
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    
    }

// --------------------------------------------------------------------------
// CIptvServer::GetSearchManagerL()
// --------------------------------------------------------------------------
//
CIptvSearchManager* CIptvServer::GetSearchManagerL(
    TIptvServiceId aServiceId,
    CIptvEpgSession& aParent,
    CIptvEpgVodMsqQueue* aVodMsgQueue )
    {
    for ( TInt i = 0; i < iSearchManagers.Count(); i++ )
        {
        if ( iSearchManagers[i].iServiceId == aServiceId )
            {
            if ( aVodMsgQueue )
                {
                iSearchManagers[i].iManager->AddEpgVodMsgQueue( aVodMsgQueue );
                }

            iSearchManagers[i].iReferenceCount++;

            return iSearchManagers[i].iManager;
            }
        }

    TIptvSearchManagerItem newManager;

    newManager.iManager =
        CIptvSearchManager::NewL( *this, aParent, aVodMsgQueue );
    newManager.iServiceId = aServiceId;
    newManager.iReferenceCount = 1;

    CleanupStack::PushL( newManager.iManager );
    iSearchManagers.AppendL( newManager );
    CleanupStack::Pop( newManager.iManager );

    return newManager.iManager;
    }

// --------------------------------------------------------------------------
// CIptvServer::RemoveQueueFromSearchManagerL()
// --------------------------------------------------------------------------
//
void CIptvServer::RemoveQueueFromSearchManagerL(
    TIptvServiceId aServiceId,
    CIptvEpgVodMsqQueue* aVodMsgQueue )
    {
    for ( TInt i = 0; i < iSearchManagers.Count(); i++ )
        {
        if ( iSearchManagers[i].iServiceId == aServiceId )
            {
            // No need to remove queue if pointer is NULL.
            if ( aVodMsgQueue )
                {
                iSearchManagers[i].iManager->RemoveEpgVodMsgQueue(
                    aVodMsgQueue );
                }

            iSearchManagers[i].iReferenceCount--;

            // If reference count is zero, we can free up the Search Manager.
            if ( iSearchManagers[i].iReferenceCount == 0 )
                {
                delete iSearchManagers[i].iManager;
                iSearchManagers[i].iManager = NULL;
                iSearchManagers.Remove( i );
                return;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CIptvServer::EpgUpdating()
// --------------------------------------------------------------------------
//
TBool CIptvServer::EpgUpdating()
    {
    return iEpgUpdating;
    }

// --------------------------------------------------------------------------
// CIptvServer::SetEpgUpdating()
// --------------------------------------------------------------------------
//
void CIptvServer::SetEpgUpdating( TBool aUpdating )
    {
    iEpgUpdating = aUpdating;
    }
