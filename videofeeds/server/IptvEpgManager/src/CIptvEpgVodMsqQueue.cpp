/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
#include <s32mem.h>
#include "IptvDebug.h"
#include "CIptvTimer.h"

#include "MIptvVodContentClientObserver.h"
#include "IptvClientServerCommon.h"
#include "CIptvEpgVodMsqQueue.h"

// CONSTANTS
const TInt KIptvMaxTries = 600;
const TInt KIptvVodMsgQueueTimerInterval = 100000;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvEpgVodMsqQueue::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvEpgVodMsqQueue::ConstructL()
    {       
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::ConstructL");               

    iTimer = CIptvTimer::NewL(CActive::EPriorityUserInput, *this);    
    }

// ---------------------------------------------------------
// CIptvEpgVodMsqQueue::NewL
// Two-phased constructor.
// Create instance of concrete interface implementation
// ---------------------------------------------------------
//
CIptvEpgVodMsqQueue* CIptvEpgVodMsqQueue::NewL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::NewL");  

    CIptvEpgVodMsqQueue* self = new(ELeave) CIptvEpgVodMsqQueue();
    CleanupStack::PushL(self);

    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvEpgVodMsqQueue::~CIptvEpgVodMsqQueue
// Destructor
// ---------------------------------------------------------
//
CIptvEpgVodMsqQueue::~CIptvEpgVodMsqQueue()
    {	
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::~CIptvEpgVodMsqQueue");            

    iMsgQueue.Close();
    CancelRequest();

    if (iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }            
    }

// ---------------------------------------------------------
// CIptvEpgVodMsqQueue::CIptvEpgVodMsqQueue
// C++ default constructor
// ---------------------------------------------------------
//
CIptvEpgVodMsqQueue::CIptvEpgVodMsqQueue() :
    iRequestPending( EFalse ),
    iTimer ( NULL ),
    iCounter( 0 )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::CIptvEpgVodMsqQueue");
    }
  
// ------------------------------------------------------------------
// CIptvEpgVodMsqQueue::SendMessageToClientL
// ------------------------------------------------------------------
//
void CIptvEpgVodMsqQueue::SendMessageToClientL(TInt aMsg, TInt aInfo)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::SendMessageToClientL");
    
    if (!iRequestPending)
        {            
        TIptvEpgVodMsg msg;        
        msg.iValue = aMsg;
        msg.iInfo = aInfo;

        iMsgQueue.AppendL(msg);
        SetTimer();
        }
    else
        {
        TIptvEpgVodMsg msg;        
        msg.iValue = aMsg;
        msg.iInfo = aInfo;

        SendMessageL(msg);
        }                
    }

// ------------------------------------------------------------------
// CIptvEpgVodMsqQueue::SetTimer
// ------------------------------------------------------------------
//
void CIptvEpgVodMsqQueue::SetTimer()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::SetTimer");    			

    if (!iTimer->IsActive())        
        {
        iTimer->After(KIptvVodMsgQueueTimerInterval);
        }
    }

// ---------------------------------------------------------
// CIptvEpgVodMsqQueue::TimerExpired
// 
// ---------------------------------------------------------
//
void CIptvEpgVodMsqQueue::TimerExpired(CIptvTimer* /*aTimer*/)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::TimerExpired");    			

    TInt count = iMsgQueue.Count();
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEpgVodMsqQueue::TimerExpired --- Items in message queue %d", count);

    if (iRequestPending)
        {        
        if (count > 0)
            {
            TIptvEpgVodMsg msg = iMsgQueue[0];
            TRAPD(err, SendMessageL(msg));
            if (err != KErrNone)
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::TimerExpired SendMessageL failed");    			
                }

            iCounter = 0;
            iMsgQueue.Remove(0);
            SetTimer();
            }
        }
    else
        {
        if (count > 0)
            {
            iCounter++;            
            if (iCounter > KIptvMaxTries)
                {
                IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::TimerExpired Clearing message queue!");
                iMsgQueue.Reset();
                iCounter = 0;
                }      
            else
                {
                SetTimer();
                }      
            }
        }           
    }

// ---------------------------------------------------------
// CIptvEpgVodMsqQueue::SendMessageL
// 
// ---------------------------------------------------------
//
void CIptvEpgVodMsqQueue::SendMessageL(TIptvEpgVodMsg& aMsg)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::SendMessageL");    			

    if (iRequestPending && !iObserverMessage.IsNull())
        {
        TBuf8<KIptvVodObserverRequestSize> data;   
        RDesWriteStream writeStream;
        writeStream.Open(data);    
        CleanupClosePushL( writeStream );    
        writeStream.WriteUint32L(aMsg.iValue);
        writeStream.WriteInt32L(aMsg.iInfo);
        CleanupStack::PopAndDestroy( &writeStream );
        iObserverMessage.WriteL(0, data, 0);
        iObserverMessage.Complete(KErrNone);
        iRequestPending = EFalse;
        }
#ifdef _DEBUG
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::SendMessageL() Error! Called when no request or message!");
        }
#endif // _DEBUG
    }

// ------------------------------------------------------------------
// CIptvEpgVodMsqQueue::SetRequest
// ------------------------------------------------------------------
//
void CIptvEpgVodMsqQueue::SetRequest(const RMessage2& aMessage)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::SetRequest");

    iCounter = 0;
    iObserverMessage = aMessage;
    iRequestPending = ETrue;

    TInt count = iMsgQueue.Count();    
    if (count >= 1)
        {
        if (count == 1)
            {
            iTimer->Cancel();
            }

        TIptvEpgVodMsg msg = iMsgQueue[0];
        TRAPD(err, SendMessageL(msg));
        if (err != KErrNone)
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::SetRequest SendMessageL failed");    			
            }
        iMsgQueue.Remove(0);        
        }    
    }

// ------------------------------------------------------------------
// CIptvEpgVodMsqQueue::CancelRequest
// ------------------------------------------------------------------
//
void CIptvEpgVodMsqQueue::CancelRequest()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::CancelRequest");
        
    if (iRequestPending)
        {
        iObserverMessage.Complete(KErrCancel);    
        iRequestPending = EFalse;
        }
    }

// ------------------------------------------------------------------
// CIptvEpgVodMsqQueue::CancelMsgQueue
// ------------------------------------------------------------------
//
void CIptvEpgVodMsqQueue::CancelMsgQueue()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEpgVodMsqQueue::CancelMsgQueue");
    iMsgQueue.Close();
    }

// End of File
