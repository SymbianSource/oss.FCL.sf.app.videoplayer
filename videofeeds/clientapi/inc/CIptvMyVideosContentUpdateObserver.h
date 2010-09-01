/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Observer for content updates from server.*
*/




#ifndef CIPTVMYVIDEOSCONTENTUPDATEOBSERVER_H
#define CIPTVMYVIDEOSCONTENTUPDATEOBSERVER_H

#include <e32base.h>

class MIptvMyVideosClientObserver;
class RIptvClientSession;

/**
 * Observer for content updates from server.
 *
 * @lib IptvClientApi.dll
 */
class CIptvMyVideosContentUpdateObserver : public CActive
    {

public:

    /**
     * Two-phased constructor.
     *
     * @param aClientObserver Observer.
     * @param aSession        Server client session.
     */
    static CIptvMyVideosContentUpdateObserver* NewL(
        MIptvMyVideosClientObserver& aClientObserver,
        RIptvClientSession& aSession );

    /**
     * Destructor.
     */
    ~CIptvMyVideosContentUpdateObserver();

// From CActive

    /**
     * Called when request completes.
     */
    void RunL();

    /**
     * Called when request needs to be cancelled.
     */
    void DoCancel();

protected:

    /**
     * Constructor.
     *
     * @param aClientObserver Observer.
     * @param aSession        Server client session.
     */
    CIptvMyVideosContentUpdateObserver(
        MIptvMyVideosClientObserver& aClientObserver,
        RIptvClientSession& aSession );

    /**
     * EPOC constructor.
     */
    void ConstructL();

private:

    /**
     * Send observer request to server.
     */
    void SendObserverRequestL();

protected: // Data

    /**
     * Reference to MIptvVodContentClientObserver.
     */
    MIptvMyVideosClientObserver& iClientObserver;

    /**
     * Reference to RIptvClientSession.
     */
    RIptvClientSession& iSession;

    /**
     * Heap object to restore memory for IPC messaging.
     * Own.
     */
    HBufC8* iObserverMsg;

    /**
     * Passed to RIptvClientSession object to store the sent and received
     * data. Points to iData heap object.
     */
    TPtr8 iObserverMsgPtr;
    };

#endif // CIPTVMYVIDEOSCONTENTUPDATEOBSERVER_H
