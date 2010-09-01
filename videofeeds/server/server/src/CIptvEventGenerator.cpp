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
* Description:    Generates events on server side, CIptvEventListener listens them on*
*/





// INCLUDE FILES
#include "IptvDebug.h"
#include <s32mem.h>

#include "IptvClientServerCommon.h"
#include "CIptvEvents.h"
#include "MIptvEvent.h"
#include "IptvServer.pan"
#include "CIptvEventGenerator.h"

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
// CIptvEventGenerator::CIptvEventGenerator
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvEventGenerator::CIptvEventGenerator(MIptvEvent* aEventProto)
: iEventProto(aEventProto)
    {
    }

// -----------------------------------------------------------------------------
// CIptvEventGenerator::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvEventGenerator::ConstructL()
    {
    iEventsIncoming = CIptvEvents::NewL(iEventProto->GetCopyL());
    iEventsOutgoing = CIptvEvents::NewL(iEventProto->GetCopyL());
    }

// -----------------------------------------------------------------------------
// CIptvEventGenerator::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvEventGenerator* CIptvEventGenerator::NewL(MIptvEvent* aEventProto)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvEventGenerator::NewL()");

    CIptvEventGenerator* self = new( ELeave ) CIptvEventGenerator(aEventProto);    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator:: this = %d", self);

    IPTVLOGSTRING_LOW_LEVEL("CIptvEventGenerator::NewL()");
    return self;
    }

    
// Destructor
CIptvEventGenerator::~CIptvEventGenerator()
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator::~CIptvEventGenerator() (%d)", this);

    if ( iState == ESizeMsgPending && !iMessage.IsNull() )
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator:: (%d) completing size req with KErrCancel status", this);
        iMessage.Complete(KErrCancel);
        }
    if ( iEventProto )
        {
        iEventProto->Destruct();    
        }
    iEventProto = NULL;
    delete iEventsIncoming;
    iEventsIncoming = NULL;
    delete iEventsOutgoing;
    iEventsOutgoing = NULL;

    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator::~CIptvEventGenerator() (%d) exit", this);
    }


// -----------------------------------------------------------------------------
// CIptvEventGenerator::AddEvent
// -----------------------------------------------------------------------------
//
void CIptvEventGenerator::AddEventL(MIptvEvent& aEvent)
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator::AddEvent() (%d)", this);

    MIptvEvent* event = aEvent.GetCopyL();
    
    switch(iState)
        {
        case EIdle:
            delete event;
            break;
            
        case ESizeMsgPending:
            {
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator:: (%d) in ESizeMsgPending state", this);
            iEventsIncoming->iEvents.Append(event);
            SendSizeRespL();           
            }
            break;
            
        case ESizeMsgCompleted:
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator:: (%d) in ESizeMsgCompleted state -> adding event to iEventsIncoming", this);
            iEventsIncoming->iEvents.Append(event);
            break;
            
        case EDataMsgPending:
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator:: (%d) in EDataMsgPending state", this);
            //should not happen, add event still
            iEventsIncoming->iEvents.Append(event);
            break;
            
        case EDataMsgCompleted:
            IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator:: (%d) in EDataMsgCompleted state", this);
            iEventsIncoming->iEvents.Append(event);            
            break;
            
        default:
            IPTVLOGSTRING_LOW_LEVEL("CIptvEventGenerator:: in unknown state, panicing!");
            delete event;
            User::Panic(KIptvEventGenerator, EBadState);
            break;
        }   
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator::AddEvent() (%d) exit", this);
    }

// -----------------------------------------------------------------------------
// CIptvEventGenerator::SizeRequestL
// -----------------------------------------------------------------------------
//
void CIptvEventGenerator::SizeRequestL(const RMessage2& aSizeRequest)
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator::SizeRequestL() (%d)", this);

    iState   = ESizeMsgPending;
    iMessage = aSizeRequest;

    if(iEventsIncoming->iEvents.Count() > 0)
        {
        //Event arrived in ESizeMsgCompleted or EDataMsgCompleted states
        //return them immedetially to client.
        //iState goes to ESizeMsgCompleted.
        SendSizeRespL();
        }
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator::SizeRequestL() (%d) exit", this);
    }

// -----------------------------------------------------------------------------
// CIptvEventGenerator::DataRequest
// -----------------------------------------------------------------------------
//
void CIptvEventGenerator::DataRequestL(const RMessage2& aDataRequest)
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator::DataRequestL() (%d)", this);
     
    if(iState != ESizeMsgCompleted)
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvEventGenerator:: STATEMACHINE IN WRONG STATE!!");
        iState = EDataMsgCompleted;
        aDataRequest.Complete(KErrGeneral);
        return;
        }
        
    
    if(iEventsOutgoing->iEvents.Count() == 0)
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvEventGenerator:: error, there were no events!");
        IPTVLOGSTRING_HIGH_LEVEL("CIptvEventGenerator::DataRequestL() exit");
        iState = EDataMsgCompleted;
        aDataRequest.Complete(KErrNotFound);
        return;
        }
        
    TUint32 size   = iEventsOutgoing->CountExternalizeSize();
    HBufC8* ipcMsg = HBufC8::NewL(size);
    CleanupStack::PushL(ipcMsg); // 1->
    TPtr8 ipcMsgPtr(ipcMsg->Des());
    
    RDesWriteStream writeStream;
    writeStream.Open(ipcMsgPtr);
    CleanupClosePushL( writeStream );
    iEventsOutgoing->ExternalizeL(writeStream);
    iEventsOutgoing->DeleteEvents();
    CleanupStack::PopAndDestroy( &writeStream );
    
    aDataRequest.Write(0, ipcMsgPtr, 0);
        
    CleanupStack::PopAndDestroy(ipcMsg); // <-1
    
    iState = EDataMsgCompleted;
    aDataRequest.Complete(KErrNone);

    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator::DataRequestL() (%d) exit", this);
    }

// -----------------------------------------------------------------------------
// CIptvEventGenerator::SendSizeRespL
// -----------------------------------------------------------------------------
//
void CIptvEventGenerator::SendSizeRespL()
    {
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator::SendSizeRespL() (%d)", this);
    iEventsOutgoing->SetL(*iEventsIncoming);
    iEventsIncoming->DeleteEvents();
    TUint32 size = iEventsOutgoing->CountExternalizeSize(); 

    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator:: size = %d", size);

    const TInt KIptvVodDlEventSizeRespSize = 4;
    TBuf8<KIptvVodDlEventSizeRespSize> ipcMsg;
    ipcMsg.Zero();
    RDesWriteStream writeStream;
    writeStream.Open(ipcMsg);
    CleanupClosePushL( writeStream );
    writeStream.WriteUint32L(size);
    CleanupStack::PopAndDestroy( &writeStream );
    iMessage.Write(0, ipcMsg, 0);
    iState = ESizeMsgCompleted;
    iMessage.Complete(KErrNone);
    IPTVLOGSTRING2_LOW_LEVEL("CIptvEventGenerator::SendSizeRespL() (%d) exit", this);
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
