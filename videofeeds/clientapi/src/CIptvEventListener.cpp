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
* Description:    Listens events from IPTV server*
*/





// INCLUDE FILES
#include "IptvDebug.h"
#include <s32mem.h>

#include "IptvClientServerCommon.h"
#include "CIptvEventListener.h"
#include "CIptvEvents.h"
#include "MIptvEvent.h"
#include "CIptvClientBase.h"
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
// CIptvEventListener::CIptvEventListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvEventListener::CIptvEventListener( CIptvClientBase& aClient,
                                        MIptvEvent* aEventProto,
                                        TInt aSizeReqMsgId,
                                        TInt aDataMsgId,
                                        TInt aCancelSizeReqMsgId )
: CActive(EPriorityStandard),
  iClient(aClient),
  iEventProto(aEventProto),
  iSizeReqMsgId(aSizeReqMsgId),
  iDataMsgId(aDataMsgId),
  iCancelSizeReqMsgId(aCancelSizeReqMsgId)
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvEventListener::CIptvEventListener()");
    CActiveScheduler::Add( this );
    IPTVLOGSTRING_HIGH_LEVEL("CIptvEventListener::CIptvEventListener() exit");
    }

// -----------------------------------------------------------------------------
// CIptvEventListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvEventListener::ConstructL()
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvEventListener::ConstructL()");    
    SendSizeReqL();
    IPTVLOGSTRING_HIGH_LEVEL("CIptvEventListener::ConstructL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvEventListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvEventListener* CIptvEventListener::NewL( CIptvClientBase& aClient,
                                              MIptvEvent* aEventProto,
                                              TInt aSizeReqMsgId,
                                              TInt aDataMsgId,
                                              TInt aCancelSizeReqMsgId )
    {
    IPTVLOGSTRING_HIGH_LEVEL("CIptvEventListener::NewL()");

    CIptvEventListener* self = new( ELeave ) CIptvEventListener( aClient,
                                                                 aEventProto,
                                                                 aSizeReqMsgId,
                                                                 aDataMsgId,
                                                                 aCancelSizeReqMsgId );
    CleanupStack::PushL( self );

    IPTVLOGSTRING2_HIGH_LEVEL("CIptvEventListener:: this = %d", self);
    
    self->ConstructL();
    CleanupStack::Pop(self);

    IPTVLOGSTRING_HIGH_LEVEL("CIptvEventListener::NewL() exit");
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvEventListener::RunL
// -----------------------------------------------------------------------------
//
void CIptvEventListener::RunL()
    {
    TInt error = iStatus.Int();
    
    const TInt KMaxErrors = 10;
    
    if (error != KErrNone)
        {
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvEventListener:: request completed with error code: %d", error);

        switch(error)
            {
            case KErrServerTerminated:
                {
                IPTVLOGSTRING_HIGH_LEVEL("CIptvEventListener:: server has terminated, panicing client!");
                User::Panic(KIptvEventListener, error);
                }
                break;

            case KErrCancel:
                {
                IPTVLOGSTRING_HIGH_LEVEL("CIptvEventListener:: KErrCancel received");                        
                }
                break;

            default:
                {
                iErrorCounter++;
                if (iErrorCounter < KMaxErrors)
                    {
                    IPTVLOGSTRING_HIGH_LEVEL("CIptvEventListener:: sending new size request");
                    SendSizeReqL();
                    }
                else
                    {
                    IPTVLOGSTRING_HIGH_LEVEL("CIptvEventListener:: too many errors, panicing client");
                    User::Panic(KIptvEventListener, EMaxErrorsExceeded);
                    }
                }
            }
        }
    else
        {
        iErrorCounter = 0;
        }
        
    if ( iPendingMsgId == iSizeReqMsgId )
        {
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvEventListener:: (%d) event size msg arrived", this);
        RDesReadStream readStream;
        readStream.Open(iSizeRequestIpcMsg);
        CleanupClosePushL(readStream); // 1->
        TUint32 size = readStream.ReadUint32L();
        CleanupStack::PopAndDestroy(&readStream); // <-1
        CIptvEvents* events = CIptvEvents::NewL(iEventProto->GetCopyL());
        CleanupStack::PushL(events); // 1->

        IPTVLOGSTRING2_HIGH_LEVEL("CIptvEventListener:: (%d) sending data req", this);

        TInt err = iClient.SendRequestL( static_cast<TUint16>(iDataMsgId),
                                         *events,
                                         static_cast<TInt>(size) );
        if ( err == KErrNone )
            {
            TInt eventCount = events->iEvents.Count();
            IPTVLOGSTRING3_HIGH_LEVEL("CIptvEventListener:: (%d) received %d events", this, eventCount);
            TInt i;
            for ( i = 0; i < eventCount; i++ )
                {
                iClient.HandleEvent( *(events->iEvents[i]) );
                }
            }
        else
            {
            IPTVLOGSTRING2_HIGH_LEVEL("CIptvEventListener:: (%d) failed to receive vod dl events", this);
            }
        CleanupStack::PopAndDestroy(events); // <-1

        SendSizeReqL();
        }
    else
        {
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvEventListener:: (%d) unknown msg received-> panic", this);
        User::Panic(KIptvEventListener, EIptvUnknownMessage);
        }

    }
    
// -----------------------------------------------------------------------------
// CIptvEventListener::DoCancel
// -----------------------------------------------------------------------------
//
void CIptvEventListener::DoCancel()
    {
    iClient.iSession.SendRequest(iCancelSizeReqMsgId); //sync, server side is gone when this finishes
    }
    
// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//    
CIptvEventListener::~CIptvEventListener()
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventListener::~CIptvEventListener() (%d)", this);

    Cancel(); //calls DoCancel if there is message pending

    if ( iEventProto )
        {
        iEventProto->Destruct();
        }

    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventListener::~CIptvEventListener() (%d) exit", this);
    }

// -----------------------------------------------------------------------------
// CIptvEventListener::SendSizeReqL
// -----------------------------------------------------------------------------
//
void CIptvEventListener::SendSizeReqL()
    {   
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventListener:: (%d) sending size req", this);
    iClient.iSession.SendRequest(iSizeReqMsgId, iSizeRequestIpcMsg, iStatus);
    SetActive();
    iPendingMsgId = iSizeReqMsgId;
    }
