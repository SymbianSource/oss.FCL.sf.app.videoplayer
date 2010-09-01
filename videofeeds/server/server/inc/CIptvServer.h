/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Server*
*/




#ifndef CIPTVSERVER_H
#define CIPTVSERVER_H

#include <e32base.h>
#include <e32svr.h>

#include "IptvClientServerCommon.h"
#include "IptvServer.pan"
#include "CIptvTimer.h"
#include "MIptvTimerObserver.h"
#include "CIptvUtil.h"
#include "MIptvMyVideosClientObserver.h"
#include "iptvactivespaceupdater.h"

class CIptvServiceManager;
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
class CIptvVodDownloadManager;
class CIptvVideoStorageCallback;
class MIptvMyVideos;
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
class CIptvEpgManagerImpl;
class CIptvNetworkSelection;

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

 class CIptvVideoStorageMyVideosAdaptation;

#else

 // For S60 5.0, we have just dummy forward declarations to compile ok.
 class MIptvMyVideos;
 class CIptvVideoStorageMyVideosAdaptation;

#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

class CIptvSearchManager;
class CIptvEpgVodMsqQueue;
class CIptvEpgSession;
class CIptvVODScheduledDownloadScheduler;

/**
 *  Server
 *
 *  @lib IptvServer.exe
 */
class CIptvServer : public CPolicyServer,
                    public MIptvTimerObserver
    {

public : // Constructors and destructors

    /**
     * Two-phased constructor.
     * @return Pointer to created CIptvServer object.
     */
    static CIptvServer* NewL();

    /**
     * Two-phased constructor.
     * @return Pointer to created CIptvServer object.
     */
    static CIptvServer* NewLC();

    /**
     * Destructor.
     */
    virtual ~CIptvServer();

    /**
     * ThreadFunction. Main function for the server thread.
     * @param aStarted A semaphore to be flagged when server
     *                 has been started.
     * @return Error code.
     */
    static TInt ThreadFunction( TAny* aStarted );

    /**
     * IncrementSessions.
     * Increments the count of the active sessions for this server.
     */
    void IncrementSessions();

    /**
     * DecrementSessionsL.
     * Decrements the count of the active sessions for this server.
     */
    void DecrementSessionsL();

    /**
     * GetEpgManager.
     * Gets Epg Manager for a service.
     */
    CIptvEpgManagerImpl& GetEpgManager();

    /**
     * Gets VideoStorageAdaptation. If there aren't one already, it's created.
     */
    MIptvMyVideos* GetVideoStorageAdaptationL();

    /**
     * Gets VideoStorageAdaptation. If there aren't one already, it's created.
     * This is for server internal use only.
     */
    CIptvVideoStorageMyVideosAdaptation*
        GetVideoStorageMyVideosAdaptationL();
    
    /**
     * GetSearchManagerL returns Service Id specific instance of
     * Search Manager. If instance for some Service Id has been already
     * created by someone else, returns pointer to the same instance.
     * Does not transfer ownership.
     *
     * @param aServiceId   Service Id.
     * @param aParent      Parent EPG Session.
     * @param aVodMsgQueue Message queue of the EPG Manager that will be
     *                     attached to the Search Manager (they share same
     *                     queue for messages).
     * @return Pointer to Search Manager.
     */
    CIptvSearchManager* GetSearchManagerL(
        TIptvServiceId aServiceId,
        CIptvEpgSession& aParent,
        CIptvEpgVodMsqQueue* aVodMsgQueue );

    /**
     * Removes client from Search Manager.
     *
     * @param aServiceId   Service Id.
     * @param aVodMsgQueue Message queue of the EPG Manager that was
     *                     attached to Manager.
     */
    void RemoveQueueFromSearchManagerL(
        TIptvServiceId aServiceId,
        CIptvEpgVodMsqQueue* aVodMsgQueue );


    /**
     * Return ETrue if the EPG of any service is being updated currently.
     *
     * @return ETrue if the EPG of any service is being updated currently.
     */
    TBool EpgUpdating();

    /**
     * Set the EPG updating status flag.
     *
     * @param aUpdating Is EPG of any service being updated.
     */
    void SetEpgUpdating( TBool aUpdating );

// From MIptvTimerObserver

    /**
     * Handles a timeout event.
     */
    void TimerExpired( CIptvTimer* aTimer );

protected:

// From CPolicyServer

    /**
     * Called by framework to perform custom security check for any
     * client messages.
     *
     * @param aMsg     Message.
     * @param aAction  Action.
     * @param aMissing Security info.
     * @return Result of security check.
     */
    TCustomResult CustomSecurityCheckL(
        const RMessage2 &aMsg,
        TInt &aAction,
        TSecurityInfo &aMissing );

private:

    /**
     * C++ default constructor.
     *
     * @param aPriority priority for this thread.
     * @param aPolicy   Reference to a policy object describing the
     *                  security checks required for each message type.
     */
    CIptvServer( TInt aPriority, const TPolicy &aPolicy );

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * PanicClient.
     * Panics the client.
     * @param aMessage The message channel to the client.
     * @param aReason The reason code for the panic.
     */
    static void PanicClient(
        const RMessage2& aMessage,
        TInt aReason );

    /**
     * PanicServer.
     * Panics the server.
     * @param aPanic The panic code.
     */
    static void PanicServer( TInt aReason );

    /**
     * ThreadFunctionL.
     * Second stage startup for the server thread.
     */
    static TInt ThreadFunctionL();

// From CServer
    /**
     * Creates an Iptv server session.
     * @param aVersion The client version.
     * @return Pointer to new session.
     */
    CSession2* NewSessionL(
        const TVersion& aVersion,
        const RMessage2& aMessage ) const;

private: // Data

    /**
     * iSessionCount, the number of session owned by this server.
     */
    TInt iSessionCount;

    /**
     * Timer that periodically checks if server can be shut down.
     */
    CIptvTimer* iShutdownTimer;

    /**
     * Helper class for managing Search Manager instances.
     */
    class TIptvSearchManagerItem
        {
        public:

            TIptvServiceId      iServiceId;
            CIptvSearchManager* iManager;
            TInt                iReferenceCount;
        };

public:

    /**
     *
     */
    CIptvServiceManager* iServiceManager;

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    /**
     *
     */
    CIptvVodDownloadManager* iVodDownloadManager;
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    /**
     *
     */
    CIptvEpgManagerImpl* iEpgManager;
        
    /**
     *
     */
    CIptvNetworkSelection* iNetworkSelection;

    /**
     * Object that follows videos and services and updates Active Space.
     * Own
     */
    CIptvActiveSpaceUpdater* iActiveSpaceUpdater;

#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)

    /**
     * Server callback for video storage.
     * Own
     */
    CIptvVideoStorageCallback* iVideoStorageCallback;

    /**
     * My videos database adaptation.
     * Own
     */
    CIptvVideoStorageMyVideosAdaptation* iVideoStorageAdaptation;

#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    
    /**
     *
     */
    CIptvVODScheduledDownloadScheduler* iVodScheduledDlScheduler;

    /**
     *
     */
    RArray<TIptvSearchManagerItem> iSearchManagers;


    /**
     * Is EPG of any service being updated currently.
     */
    TBool iEpgUpdating;
    };

#endif // CIPTVSERVER_H
