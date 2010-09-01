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

#include "MIptvVodContentClientObserver.h"
#include "IptvClientServerCommon.h"
#include "CIptvVodContentUpdateObserver.h"
#include "RIptvClientSession.h"

// CONSTANTS
const TUint32 KIptvVodObserverData = 0;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvVodContentUpdateObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CIptvVodContentUpdateObserver::ConstructL()
    {    
    SendObserverRequestL();
    SetActive();
    }

// ---------------------------------------------------------
// CIptvVodContentUpdateObserver::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
EXPORT_C CIptvVodContentUpdateObserver* CIptvVodContentUpdateObserver::NewL(MIptvVodContentClientObserver& aObserver, 
    RIptvClientSession& aSession,
    TIptvServiceId aServiceId)
    {
    CIptvVodContentUpdateObserver* self = new(ELeave) CIptvVodContentUpdateObserver(aObserver, aSession, aServiceId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvVodContentUpdateObserver::~CIptvVodContentUpdateObserver
// Destructor
// -----------------------------------------------------------------------------
//
CIptvVodContentUpdateObserver::~CIptvVodContentUpdateObserver()
    {
    delete iObserverMsg;
    Cancel();        
    }

// ---------------------------------------------------------
// CIptvVodContentUpdateObserver::CIptvVodContentUpdateObserver
// C++ default constructor
// ---------------------------------------------------------
//
CIptvVodContentUpdateObserver::CIptvVodContentUpdateObserver(MIptvVodContentClientObserver& aObserver, 
    RIptvClientSession& aSession, 
    TIptvServiceId aServiceId) : 
    CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iSession( aSession ),
    iObserverMsg( NULL ),
    iObserverMsgPtr( (unsigned char*)0, 0 ),
    iServiceId( aServiceId )
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CIptvVodContentUpdateObserver::DoCancel
// 
// ---------------------------------------------------------
//
void CIptvVodContentUpdateObserver::DoCancel()
    {
    }

// ---------------------------------------------------------
// CIptvVodContentUpdateObserver::RunL
// 
// ---------------------------------------------------------
//
void CIptvVodContentUpdateObserver::RunL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentUpdateObserver::RunL");

    RDesReadStream stream;
    stream.Open(iObserverMsgPtr);
    CleanupClosePushL( stream );    
    TUint32 msg = stream.ReadUint32L();
    TInt info = stream.ReadInt32L();
    CleanupStack::PopAndDestroy( &stream );

    TRAP_IGNORE( iObserver.HandleEpgManagerMsgL( msg, info, iServiceId ) );
            
    SendObserverRequestL();
    SetActive();
    }

// -----------------------------------------------------------------------------
// CIptvVodContentClient::SendObserverRequestL
// -----------------------------------------------------------------------------
void CIptvVodContentUpdateObserver::SendObserverRequestL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodContentUpdateObserver::SendObserverRequestL");

    delete iObserverMsg;
    iObserverMsg = NULL;
    
    iObserverMsg = HBufC8::NewL(KIptvVodObserverRequestSize);
    iObserverMsgPtr.Set(iObserverMsg->Des());

    RDesWriteStream stream;
    stream.Open(iObserverMsgPtr);
    CleanupClosePushL( stream );
    stream.WriteUint32L(KIptvVodObserverData);
    CleanupStack::PopAndDestroy( &stream );
    
    iSession.SendRequest(EIptvEngineVodContentObserver, iObserverMsgPtr, iStatus);
    }
