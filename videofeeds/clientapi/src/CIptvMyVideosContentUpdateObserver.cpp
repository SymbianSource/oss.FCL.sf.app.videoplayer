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




#include <e32base.h>
#include <s32mem.h>
#include "IptvDebug.h"

#include "IptvClientServerCommon.h"
#include "RIptvClientSession.h"
#include "CIptvMyVideosContentUpdateObserver.h"
#include "MIptvMyVideosClientObserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIptvMyVideosContentUpdateObserver::CIptvMyVideosContentUpdateObserver()
// ---------------------------------------------------------------------------
//
CIptvMyVideosContentUpdateObserver::CIptvMyVideosContentUpdateObserver(
    MIptvMyVideosClientObserver& aClientObserver,
    RIptvClientSession& aSession )
 :  CActive( EPriorityStandard ),
    iClientObserver( aClientObserver ),
    iSession( aSession ),
    iObserverMsgPtr( static_cast<unsigned char*>( 0 ), 0 )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CIptvMyVideosContentUpdateObserver::NewL()
// ---------------------------------------------------------------------------
//
CIptvMyVideosContentUpdateObserver* CIptvMyVideosContentUpdateObserver::NewL(
    MIptvMyVideosClientObserver& aClientObserver,
    RIptvClientSession& aSession )
    {
    CIptvMyVideosContentUpdateObserver* self =
        new ( ELeave ) CIptvMyVideosContentUpdateObserver(
            aClientObserver, aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIptvMyVideosContentUpdateObserver::ConstructL()
// ---------------------------------------------------------------------------
//
void CIptvMyVideosContentUpdateObserver::ConstructL()
    {
    SendObserverRequestL();
    SetActive();
    }

// ---------------------------------------------------------------------------
// CIptvMyVideosContentUpdateObserver::~CIptvMyVideosContentUpdateObserver()
// ---------------------------------------------------------------------------
//
CIptvMyVideosContentUpdateObserver::~CIptvMyVideosContentUpdateObserver()
    {
    delete iObserverMsg;
    Cancel();
    }

// ---------------------------------------------------------------------------
// CIptvMyVideosContentUpdateObserver::RunL()
// ---------------------------------------------------------------------------
//
void CIptvMyVideosContentUpdateObserver::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        MIptvMyVideosClientObserver::TIptvContentUpdatedEvent event;

        RDesReadStream stream;
        stream.Open( iObserverMsgPtr );
        CleanupClosePushL( stream );

        event.iEventType = static_cast<MIptvMyVideosClientObserver::TIptvContentUpdatedEventType>
            ( stream.ReadUint32L() );
        event.iDrive = stream.ReadUint32L();
        event.iFileId = stream.ReadUint32L();
        CleanupStack::PopAndDestroy( &stream );

        iClientObserver.ContentsUpdated( event );

        // Only set active if previous completed without error. Otherwise
        // we make eternal loop; client sending new messages and server
        // completing them instantly with error.
        SendObserverRequestL();
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CIptvMyVideosContentUpdateObserver::SendObserverRequestL
// ---------------------------------------------------------------------------
//
void CIptvMyVideosContentUpdateObserver::SendObserverRequestL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvMyVideosContentUpdateObserver::SendObserverRequestL" );

    delete iObserverMsg;
    iObserverMsg = NULL;

    iObserverMsg = HBufC8::NewL( KIptvMyVideosObserverRequestSize );
    iObserverMsgPtr.Set( iObserverMsg->Des() );

    iSession.SendRequest(
        EIptvEngineMyVideosContentUpdateReq,
        iObserverMsgPtr,
        iStatus );
    }

// ---------------------------------------------------------------------------
// CIptvMyVideosContentUpdateObserver::DoCancel()
// ---------------------------------------------------------------------------
//
void CIptvMyVideosContentUpdateObserver::DoCancel()
    {
    }
