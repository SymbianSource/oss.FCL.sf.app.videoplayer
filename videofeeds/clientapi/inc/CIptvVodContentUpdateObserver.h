/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Observer VoD content downloading*
*/




#ifndef CIPTVVODCONTENTUPDATEOBSERVER_H
#define CIPTVVODCONTENTUPDATEOBSERVER_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MIptvVodContentClientObserver;
class RIptvClientSession;

/**
*  VoD ECG content API
*
*  @lib IptvClientApi.lib
*  @since Engine 1.0
*/

// CLASS DECLARATION
class CIptvVodContentUpdateObserver : public CActive
    {
    public:
        /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvVodContentUpdateObserver* NewL(MIptvVodContentClientObserver& aObserver, RIptvClientSession& aSession, TIptvServiceId aServiceId);

        /**
         * Destructor.
         */
        virtual ~CIptvVodContentUpdateObserver();

    public:
        /**
         * Handles an active object’s request completion event.
         */
        void RunL();

        /**
         * Implements cancellation of an outstanding request.
         */
        void DoCancel();
                                
    private: // New functions           
        /**
        * C++ default constructor.
        */
        CIptvVodContentUpdateObserver(MIptvVodContentClientObserver& aObserver, RIptvClientSession& aSession, TIptvServiceId aServiceId);

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();

    private:
        /**
        * Send observer request to server.
        */
        void SendObserverRequestL();
                  
    private: // Data members
        /**
        * Reference to MIptvVodContentClientObserver.
        */
        MIptvVodContentClientObserver& iObserver;

        /**
        * Reference to RIptvClientSession.
        */
        RIptvClientSession& iSession;
        
        /**
        * iObserverMsg Heap object to restore memory for IPC messaging.
        */
        HBufC8* iObserverMsg;

        /**
        * iObserverMsgPtr Passed to RIptvClientSession object to store the sent and received data.
        * Points to iData heap object.
        */
        TPtr8 iObserverMsgPtr;

        /**
        * Service id.
        */
        TIptvServiceId iServiceId;
    };

#endif  // CIPTVVODCONTENTUPDATEOBSERVER_H
