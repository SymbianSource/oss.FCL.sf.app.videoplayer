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




// INCLUDE FILES
#include <e32math.h>
#include <e32svr.h>

#include "MIptvStreamObject.h"
#include <sysutil.h> 
#include <f32file.h>
#include "IptvDebug.h"

#include "IptvClientServerCommon.h"
#include "CIptvService.h"
#include "CIptvServiceManager.h"
#include "RIptvClientSession.h"

// FUNCTION PROTOTYPES
static TInt StartServer();
static TInt CreateServerProcess();

// CONSTANTS
const TUint KDefaultMessageSlots = 20;
const TInt KIptvServerFreeSpace = 512000; // 500 KB

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// RIptvClientSession::RIptvClientSession()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RIptvClientSession::RIptvClientSession()
: RSessionBase(), iIpcMessagePtr((unsigned char*)0, 0)
    {
    // No implementation required
    }

EXPORT_C RIptvClientSession::~RIptvClientSession()
    {
    delete iIpcMessage;
    }

// -----------------------------------------------------------------------------
// RIptvClientSession::Connect()
// Connects to the server and create a session.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RIptvClientSession::Connect()
    {
    IPTVLOGSTRING_LOW_LEVEL("RIptvClientSession::Connect()");
    
    const TInt KOneSecondDelay = 1000000;
    const TInt KMaxRetries     = 10;

    TInt error        = TryConnect();
    TInt retryCounter = 0;

    while (error != KErrNone)
        {
        // In OoM case we stop trying. Otherwise other errors would hide the real reason for failure.
        if (error == KErrNoMemory)
            {
            IPTVLOGSTRING_LOW_LEVEL("RIptvClientSession:: failed to connect to server, out of memory.");
            break;
            }
        // In case of "KErrNotFound" or "Server Terminated", we might be out of disk  on
        // first start-up. To make sure check the available disk space here on client side.
        else if (error == KErrNotFound || error == KErrServerTerminated || error == KErrDiskFull)
            {
            RFs fsSession;
            if (fsSession.Connect() == KErrNone)
                {
                TBool checkResult = EFalse;
                TRAPD(checkError, checkResult = SysUtil::DiskSpaceBelowCriticalLevelL(&fsSession, KIptvServerFreeSpace, EDriveC));
                fsSession.Close();

                if (checkError != KErrNone || checkResult)
                    {
                    IPTVLOGSTRING_LOW_LEVEL("RIptvClientSession::Connect() returning KErrDiskFull");
                    return KErrDiskFull;
                    }
                }
            }

        retryCounter++;
        if(retryCounter > KMaxRetries)
            {
            IPTVLOGSTRING_LOW_LEVEL("RIptvClientSession:: failed to connect to server, giving up");
            break;
            }

        IPTVLOGSTRING_LOW_LEVEL("RIptvClientSession:: failed to connect to server, retrying after 1 second");
        User::After(KOneSecondDelay);

        error = TryConnect();
        }    

    IPTVLOGSTRING2_LOW_LEVEL("RIptvClientSession:: returning %d", error);        
    IPTVLOGSTRING_LOW_LEVEL("RIptvClientSession::Connect() exit");
    return error;
    }
    
// -----------------------------------------------------------------------------
// RIptvClientSession::TryConnect()
// Connects to the server and creates a session.
// -----------------------------------------------------------------------------
//
TInt RIptvClientSession::TryConnect()
    {
    IPTVLOGSTRING_LOW_LEVEL("RIptvClientSession::TryConnect()");

    TInt error;

    error = ::StartServer();
    
    if (error != KErrNone)
        {
        IPTVLOGSTRING2_LOW_LEVEL("RIptvClientSession:: failed to start server process: %d", error);
        IPTVLOGSTRING_LOW_LEVEL("RIptvClientSession::TryConnect() exit");
        return error;
        }
	      
    error = CreateSession( KIptvServerName,
                           Version(),
                           KDefaultMessageSlots );

    if (error != KErrNone)
        {
        IPTVLOGSTRING2_LOW_LEVEL("RIptvClientSession:: failed to create session: %d", error);
        }

    IPTVLOGSTRING_LOW_LEVEL("RIptvClientSession::TryConnect() exit");
    return error;
    }

// -----------------------------------------------------------------------------
// RIptvClientSession::Version()
// Gets the version number.
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion RIptvClientSession::Version() const
    {
    return( TVersion( KIptvServMajorVersionNumber,
                      KIptvServMinorVersionNumber,
                      KIptvServBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// RIptvClientSession::IsNull()
// -----------------------------------------------------------------------------
//
TBool RIptvClientSession::IsNull() const
    {
    return iHandle == NULL;
    }

// -----------------------------------------------------------------------------
// RIptvClientSession::SendRequest()
// Issues a request to the server asynchronously.
// -----------------------------------------------------------------------------
//
EXPORT_C void RIptvClientSession::SendRequest(TUint8 aMsgId,
                                              TDes8& aMsg,
        							                        TRequestStatus& aStatus)
    {
    TIpcArgs ipcArgs(&aMsg);
    SendReceive(aMsgId, ipcArgs, aStatus);
    }

// -----------------------------------------------------------------------------
// RIptvClientSession::SendRequest()
// Issues a request to the server synchronously.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RIptvClientSession::SendRequest(TUint8 aMsgId,
                                              TDes8& aMsg)
    {
    TIpcArgs ipcArgs(&aMsg);
    return SendReceive(aMsgId, ipcArgs);
    }

// -----------------------------------------------------------------------------
// RIptvClientSession::SendRequest()
// Issues a request to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C void RIptvClientSession::SendRequest(TUint8 aMsgId,
        							                        TRequestStatus& aStatus)
    {
    SendReceive(aMsgId, aStatus);
    }

// -----------------------------------------------------------------------------
// RIptvClientSession::SendRequest()
// Send synchrounous request to server.
// -----------------------------------------------------------------------------
EXPORT_C TInt RIptvClientSession::SendRequest(TUint8 aMsgId) const
    {
    return SendReceive(aMsgId, TIpcArgs());
    }

// -----------------------------------------------------------------------------
// RIptvClientSession::SendRequest()
// Issues a request to the server synchronously.
// -----------------------------------------------------------------------------
//
TInt RIptvClientSession::SendRequest( TUint8 aMsgId,
                                      TIpcArgs& aArgs ) const
    {
    return SendReceive( aMsgId, aArgs);
    }


// ============================= OTHER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// StartServer()
// Starts the server if it is not already running
// -----------------------------------------------------------------------------
//
static TInt StartServer()
    {

    TInt result;

    TFindServer findIptvServer( KIptvServerName );
    TFullName name;

    result = findIptvServer.Next( name );
    if ( result == KErrNone )
        {
        // Server already running
        return KErrNone;
        }

    RSemaphore semaphore;
    result = semaphore.CreateGlobal( KIptvServerSemaphoreName,0 ,EOwnerProcess);

    if(result != KErrNone)
        {
        IPTVLOGSTRING2_HIGH_LEVEL("Failed to create semaphore, reason: %d", result);
        return  result;
        }

    IPTVLOGSTRING_HIGH_LEVEL("Client created semaphore, init value 0");

    result = CreateServerProcess();
    if(result != KErrNone)
        {
        IPTVLOGSTRING2_HIGH_LEVEL("Failed to start server, reason: %d", result);
        return  result;
        }

    //Decrease semaphore by 1.
    //If semaphore is negative this thread is suspended until server signals.
    //If server has already signalled, we don't stop at all.
    //Server increases semaphore by 1 (with semaphore.Signal()) when it's running.
    //Result is that this thread will wait here until server has signalled.
    //Signalling may also happen before this thread gets to semaphore.Wait().
    IPTVLOGSTRING_LOW_LEVEL("client waiting semaphore");
    semaphore.Wait();
    IPTVLOGSTRING_LOW_LEVEL("client finished waiting semaphore");
    IPTVLOGSTRING_LOW_LEVEL("Now we can be sure that server is running...");

    semaphore.Close();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CreateServerProcess()
// Creates the Iptv Engine server process
// -----------------------------------------------------------------------------
//
static TInt CreateServerProcess()
    {
    IPTVLOGSTRING_LOW_LEVEL("CreateServerProcess()");
    
    TUid uid1 = { 0x1000007a };
    TUid uid2 = { /*0x1000008d*/ 0x00000000 };
	
    const TUidType serverUid(uid1, /*KNullUid*/uid2, KIptvEngineServerUid3 );

    TInt result;		
    RProcess server;
    result = server.Create(KIptvServerFileName, KIptvEmptyDes, EOwnerProcess);
    
    if(result != KErrNone)
        {
        IPTVLOGSTRING_LOW_LEVEL("CreateServerProcess() failed to create process");
        }
    else
        {
        server.Resume();
        server.Close();
        }
        
    IPTVLOGSTRING_LOW_LEVEL("CreateServerProcess() exit");
    return  result;
    }

// End of File
