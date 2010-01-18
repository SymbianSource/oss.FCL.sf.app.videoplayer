/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/





#ifndef __RIPTVCLIENTSESSION_H__
#define __RIPTVCLIENTSESSION_H__

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>
#include "IptvEngineUids.h"

// CONSTANTS
// Number of message slots to reserve for this client server session.

const TUid KIptvEngineServerUid3 = { IPTV_SERVER_UID };

#ifdef __WINS__
static const TUint KServerMinHeapSize =  0x1000;  //  4K
static const TUint KServerMaxHeapSize = 0x10000;  // 64K
#endif

class MIptvStreamObject;

// CLASS DECLARATION
/**
 *  Provides the client-side interface to the server session.
 */
class RIptvClientSession : public RSessionBase
    {
    public:

        /**
        * Constructs the object.
        */
        IMPORT_C RIptvClientSession();

        /*!
        *Destructor
        */
        IMPORT_C ~RIptvClientSession();

        /**
        * Connects to the server and creates a session.
        * Retries 10 times.
        * @return System wide error code.
        */
        IMPORT_C TInt Connect();

        /**
        * Gets the version number.
        * @return The version.
        */
        IMPORT_C TVersion Version() const;

        /**
        * Checks if session handle is null.
        *
        * @return ETrue if handle is null.
        */
        TBool IsNull() const;

        /**
        * Sends message to IPTV Engine server.
        * @param aMsgId ID of the message to send.
        * @param aMsg descriptor where both client and server write data.
        * @param aStatus The TRequestStatus object to be used for async comms.
        */
        IMPORT_C void SendRequest(TUint8 aMsgId,
                                  TDes8& aMsg,
                                  TRequestStatus& aStatus);

        IMPORT_C TInt SendRequest(TUint8 aMsgId,
                                  TDes8& aMsg);

        /**
        * Sends message to IPTV Engine server.
        * @param aMsgId ID of the message to send.
        * @param aStatus The TRequestStatus object to be used for async comms.
        */
        IMPORT_C void SendRequest(TUint8 aMsgId,
                                  TRequestStatus& aStatus);
		
        /**
         * Send message to Iptv Engine Server synchronously.
         */
        IMPORT_C TInt SendRequest(TUint8 aMsgId) const;

        /**
        * Sends message to IPTV Engine server synchronously.
        * @param aMsgId ID of the message to send.
        * @param aArgs Message arguments. 
        * @return KErrNone if ok, otherwise system-wide error codes.
        */
        TInt SendRequest( TUint8 aMsgId, TIpcArgs& aArgs ) const;

    private:
    
        /**
         * Tries connection to the IPTV server once.
         */    
        TInt TryConnect();


    private:
    
		/**
         * iIpcMessage Heap object to reserve memory for IPC messaging.
         */
        HBufC8* iIpcMessage;
        
       /**
        * iIpcMessagePtr Points to iIpcMessage heap object. This _must_ be a member variable.
        */
        TPtr8   iIpcMessagePtr;

		
    };


#endif // __RIPTVCLIENTSESSION_H__


// End of File
