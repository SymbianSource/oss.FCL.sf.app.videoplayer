/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Listens VOD DL Events*
*/





// INCLUDE FILES
#include "IptvDebug.h"
#include "TIptvUint32StreamObject.h"

#include "CIptvVodDlClient.h"
#include "IptvClientServerCommon.h"
#include "CIptvVodDlDownloadEvents.h"
#include "CIptvVodDlDownloadEvent.h"
#include "IptvServer.pan"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvVodDlEventListener::CIptvVodDlEventListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvVodDlEventListener::CIptvVodDlEventListener(CIptvVodDlClient& aClient, TUint32 aClientId)
: CActive(EPriorityStandard), iClient(aClient), iClientId(aClientId), iIpcMsgPtr((unsigned char*)0, 0)
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIptvVodDlEventListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvVodDlEventListener::ConstructL()
    {
    SendSizeReqL();
    }

// -----------------------------------------------------------------------------
// CIptvVodDlEventListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvVodDlEventListener* CIptvVodDlEventListener::NewL(CIptvVodDlClient& aClient, TUint32 aClientId)
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvVodDlEventListener::NewL()");

    CIptvVodDlEventListener* self = new( ELeave ) CIptvVodDlEventListener(aClient, aClientId);
    CleanupStack::PushL( self );

    IPTVLOGSTRING2_HIGH_LEVEL("CIptvVodDlEventListener:: this = %d", self);
    
    self->ConstructL();
    CleanupStack::Pop(self);

    IPTVLOGSTRING_HIGH_LEVEL("CIptvVodDlEventListener::NewL() exit");
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlEventListener::RunL
// -----------------------------------------------------------------------------
//
void CIptvVodDlEventListener::RunL()
    {
    TInt error = iStatus.Int();
    
    const TInt KMaxErrors = 10;
    
    if (error != KErrNone)
        {
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvVodDlEventListener:: request completed with error code: %d", error);
        if (error == KErrServerTerminated)
            {
            IPTVLOGSTRING_HIGH_LEVEL("CIptvVodDlEventListener:: server has terminated, panicing client!");
            User::Panic(KIptvVodDlEventListener, error);
            }
        else
            {
            iErrorCounter++;
            if (iErrorCounter < KMaxErrors)
                {
                IPTVLOGSTRING_HIGH_LEVEL("CIptvVodDlEventListener:: sending new size request");
                SendSizeReqL();
                }
            else
                {
                IPTVLOGSTRING_HIGH_LEVEL("CIptvVodDlEventListener:: too many errors, panicing client");
                User::Panic(KIptvVodDlEventListener, EMaxErrorsExceeded);
                }
            }
        }
    else
        {
        iErrorCounter = 0;
        }
        
    switch ( iResponseMsgId )
        {
        case EIptvEngineVodDlRequestEventSizeResp:
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvVodDlEventListener:: (%d) EIptvEngineVodDlRequestEventSizeResp", this);
            RDesReadStream readStream;
            readStream.Open(iSizeRequestIpcMsg);
            CleanupClosePushL( readStream );
            TUint32 size = readStream.ReadUint32L();
            CleanupStack::PopAndDestroy( &readStream );
            CIptvVodDlDownloadEvents* events = CIptvVodDlDownloadEvents::NewL();
            CleanupStack::PushL(events); // 1->
            TIptvUint32StreamObject clientId;
            clientId.iData = iClientId;

            IPTVLOGSTRING3_HIGH_LEVEL("CIptvVodDlEventListener:: (%d) sending data req (client id = %d)", this, iClientId);

            TInt err = iClient.SendRequestL(static_cast<TUint16>(EIptvEngineVodDlRequestEvent),
                                            clientId,
                                            *events,
                                            static_cast<TInt>(size));
            if(err == KErrNone)
                {
                TInt eventCount = events->iEvents.Count();
                IPTVLOGSTRING3_HIGH_LEVEL("CIptvVodDlEventListener:: (%d) received %d events", this, eventCount);
                TInt i;
                for(i = 0; i < eventCount; i++)
                    {
                    IPTVLOGSTRING4_LOW_LEVEL("CIptvVodDlEventListener:: (%d) service id: %d, content id: %d",
                                             this,
                                             events->iEvents[i]->iServiceId,
                                             events->iEvents[i]->iContentId);                         
                    iClient.iClientObserver.HandleVodDlDownloadEvent(*(events->iEvents[i]));
                    }
                }
            else
                {
                IPTVLOGSTRING2_HIGH_LEVEL("CIptvVodDlEventListener:: (%d) failed to receive vod dl events", this);
                }
            CleanupStack::PopAndDestroy(events); // <-1

            SendSizeReqL();
            }
            break;
                        
        default:
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvVodDlEventListener:: (%d) unknown msg received-> panic", this);
            User::Panic(KIptvVodDlEventListener, EIptvUnknownMessage);
            break;
        }

    }
    
// -----------------------------------------------------------------------------
// CIptvVodDlEventListener::DoCancel
// -----------------------------------------------------------------------------
//
void CIptvVodDlEventListener::DoCancel()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlEventListener::DoCancel()");

    //delete event manager from server side
    TBuf8<KIptvEngineVodDlDeRegisterClientSize> ipcMsg;
    ipcMsg.Zero();           
    RDesWriteStream writeStream;
    writeStream.Open(ipcMsg);
    TRAPD(err, writeStream.WriteInt32L(iClientId)); //TRAPD is just to remove Codescanner warning, this never leaves anyway
    writeStream.Close();
    if (err != KErrNone)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlEventListener:: writeStream.WriteInt32L failed, this should never happen!");
        }
        
    iClient.iSession.SendRequest(EIptvEngineVodDlDeRegisterClient, ipcMsg); //sync, server side is gone when this finishes

    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlEventListener::DoCancel() exit");
    }
    
// Destructor
CIptvVodDlEventListener::~CIptvVodDlEventListener()
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvVodDlEventListener::~CIptvVodDlEventListener() (%d)", this);

    Cancel(); //calls DoCancel if there is message pending

    delete iIpcMsg;
    IPTVLOGSTRING2_LOW_LEVEL("CIptvVodDlEventListener::~CIptvVodDlEventListener() (%d) exit", this);
    }

// -----------------------------------------------------------------------------
// CIptvVodDlEventListener::SendSizeReqL
// -----------------------------------------------------------------------------
//
void CIptvVodDlEventListener::SendSizeReqL()
    {
    iSizeRequestIpcMsg.Zero();
    RDesWriteStream writeStream;
    writeStream.Open(iSizeRequestIpcMsg);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(iClientId);
    CleanupStack::PopAndDestroy( &writeStream );

    IPTVLOGSTRING3_LOW_LEVEL("CIptvVodDlEventListener:: (%d) sending size req (client id = %d)", this, iClientId);
    iClient.iSession.SendRequest(EIptvEngineVodDlRequestEventSizeReq, iSizeRequestIpcMsg, iStatus);
    iResponseMsgId = EIptvEngineVodDlRequestEventSizeResp;
    SetActive();
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================


//  End of File  
