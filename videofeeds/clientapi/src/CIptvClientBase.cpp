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
* Description:    Base class for iptv engine client classes.*
*/




#include "MIptvStreamObject.h"
#include "IptvDebug.h"

#include "CIptvClientBase.h"
#include "CIptvMyVideosGlobalFileId.h"

const TInt KTUint16SizeInBytes( 2 );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CIptvClientBase::CIptvClientBase()
: CActive( EPriorityStandard )
    {
    }

// Destructor
CIptvClientBase::~CIptvClientBase()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvClientBase::~CIptvClientBase()");

    iSession.Close();
    Cancel();
    IPTVLOGSTRING_LOW_LEVEL("CIptvClientBase::~CIptvClientBase() exit");
    }

// ---------------------------------------------------------------------------
// CIptvClientBase::RunL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CIptvClientBase::RunL()
    {
    }

// ---------------------------------------------------------------------------
// CIptvClientBase::DoCancel
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CIptvClientBase::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// CIptvClientBase::SendRequestL
// ---------------------------------------------------------------------------
//
TInt CIptvClientBase::SendRequestL(
    TUint16 aMsgId,
    MIptvStreamObject& aReqStreamObject,
    MIptvStreamObject& aRespStreamObject,
    TInt aIpcMsgSize )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvClientBase::SendRequestL()");

    TInt size;
    HBufC8* ipcMsgInHeap = NULL;
    const TInt KMaxInStack = 100;
    TUint8 ipcMsgInStack[KMaxInStack];
    TPtr8 ipcMsgPtr((TUint8 *)0, 0, 0);

    if(aIpcMsgSize == 0)
        {
        size = aReqStreamObject.CountExternalizeSize();
        }
    else
        {
        size = aIpcMsgSize;
        }

    if(size < KMaxInStack)
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvClientBase:: size < %d -> using stack object",
            KMaxInStack);
        ipcMsgPtr.Set(
            ipcMsgInStack,
            0 /* length */,
            KMaxInStack /* max length */ );
        }
    else
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvClientBase:: size >= %d -> using heap object",
            KMaxInStack);

        ipcMsgInHeap = HBufC8::NewL(size);
        CleanupStack::PushL(ipcMsgInHeap); //1->
        ipcMsgPtr.Set(ipcMsgInHeap->Des());
        }

    RDesWriteStream writeStream;
    CleanupClosePushL( writeStream );
    //initialize stream object to use ipc msg as a data buffer
    writeStream.Open(ipcMsgPtr);
    aReqStreamObject.ExternalizeL(writeStream); //write object to the stream
    CleanupStack::PopAndDestroy( &writeStream );

    TInt result = iSession.SendRequest(aMsgId, ipcMsgPtr);

    if(result == KErrNone)
        {
        RDesReadStream readStream;
        readStream.Open(ipcMsgPtr);
        CleanupClosePushL( readStream );
        aRespStreamObject.InternalizeL(readStream); //read object from stream
        CleanupStack::PopAndDestroy( &readStream );
        }
    else
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvClientBase:: error in SendRequest: %d",
            result);
        }

    if(ipcMsgInHeap)
        {
        CleanupStack::PopAndDestroy(ipcMsgInHeap); //<-1
        }

    IPTVLOGSTRING_LOW_LEVEL("CIptvClientBase::SendRequestL() exit");
    return result;
    }

// ---------------------------------------------------------------------------
// CIptvClientBase::SendRequestL
// ---------------------------------------------------------------------------
//
TInt CIptvClientBase::SendRequestL(
    TUint16 aMsgId,
    MIptvStreamObject& aRespStreamObject,
    TInt aIpcMsgSize )
    {
    TInt result;
    const TInt KMaxSizeInStack = 100;

    if(aIpcMsgSize < KMaxSizeInStack)
        {
        //use ipc msg from stack
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvClientBase:: size < %d -> using stack object",
            KMaxSizeInStack);
        TBuf8<KMaxSizeInStack> ipcMsgInStack;

        result = iSession.SendRequest(aMsgId, ipcMsgInStack);

        if(result == KErrNone)
            {
            RDesReadStream readStream;
            readStream.Open(ipcMsgInStack);
            CleanupClosePushL( readStream );
            aRespStreamObject.InternalizeL(readStream); //read object from the stream
            CleanupStack::PopAndDestroy( &readStream );
            }
        }
    else
        {
        //allocate a new IPC message from heap
        IPTVLOGSTRING2_LOW_LEVEL(
            "CIptvClientBase:: size >= %d -> using heap object",
            KMaxSizeInStack );

        HBufC8* ipcMsgInHeap = HBufC8::NewL(aIpcMsgSize);
        CleanupStack::PushL(ipcMsgInHeap); // 1->
        TPtr8 ipcMsgPtr(ipcMsgInHeap->Des());

        result = iSession.SendRequest(aMsgId, ipcMsgPtr);

        if(result == KErrNone)
            {
            RDesReadStream readStream;
            readStream.Open(ipcMsgPtr);
            CleanupClosePushL( readStream );
            aRespStreamObject.InternalizeL(readStream); //read object from the stream
            CleanupStack::PopAndDestroy( &readStream );
            }
        CleanupStack::PopAndDestroy(ipcMsgInHeap); // <-1
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CIptvClientBase::SendRequestL
// ---------------------------------------------------------------------------
//
TInt CIptvClientBase::SendRequestL(
    TUint16 aMsgId,
    MIptvStreamObject& aReqStreamObject )
    {
    HBufC8* ipcMsg = HBufC8::NewL( aReqStreamObject.CountExternalizeSize() );
    CleanupStack::PushL(ipcMsg); // 1->

    TPtr8 ipcMsgPtr(ipcMsg->Des());
    ipcMsgPtr.Zero();

    RDesWriteStream writeStream;
    writeStream.Open(ipcMsgPtr);
    CleanupClosePushL( writeStream );
    aReqStreamObject.ExternalizeL(writeStream); //write object to the stream
    CleanupStack::PopAndDestroy( &writeStream );

    TInt err = iSession.SendRequest(aMsgId, ipcMsgPtr);
    CleanupStack::PopAndDestroy(ipcMsg); // <-1
    return err;
    }

// ---------------------------------------------------------------------------
// CIptvMyVideosClient::SendRequestL
// ---------------------------------------------------------------------------
//
TInt CIptvClientBase::SendRequestL(
    TInt aMessage,
    const TDesC& aMsg )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "My Videos Mgr ## CIptvMyVideosClient::SendRequestL" );

    TUint32 dataSize(
        KTUint16SizeInBytes + aMsg.Size() );

    HBufC8* ipcMsg = HBufC8::NewL( dataSize );
    CleanupStack::PushL( ipcMsg ); // 1->

    TPtr8 ipcMsgPtr( ipcMsg->Des() );
    ipcMsgPtr.Zero();

    RDesWriteStream stream;
    stream.Open( ipcMsgPtr );
    CleanupClosePushL( stream );
    CIptvUtil::WriteDesToStreamL( aMsg, stream );
    CleanupStack::PopAndDestroy( &stream );

    TRequestStatus status;
    iSession.SendRequest( aMessage, ipcMsgPtr, status );
    User::WaitForRequest( status );

    CleanupStack::PopAndDestroy( ipcMsg ); // <-1

    return status.Int();
    }
