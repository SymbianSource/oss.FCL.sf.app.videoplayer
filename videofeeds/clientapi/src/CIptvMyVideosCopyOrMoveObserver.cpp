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
* Description:    Observer for completion of copy or move requests.*
*/




// INCLUDE FILES
#include "IptvClientServerCommon.h"
#include "RIptvClientSession.h"
#include "CIptvMyVideosCopyOrMoveObserver.h"
#include "MIptvMyVideosClientObserver.h"
#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvMyVideosGlobalFolderId.h"

// CONSTANTS
const TInt KTUint32SizeInBytes = 4;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvMyVideosCopyOrMoveObserver::CIptvMyVideosCopyOrMoveObserver()
// ---------------------------------------------------------
//
CIptvMyVideosCopyOrMoveObserver::CIptvMyVideosCopyOrMoveObserver(MIptvMyVideosClientObserver& aClientObserver, RIptvClientSession& aSession) 
 :  CActive(EPriorityStandard),
    iClientObserver(aClientObserver),
    iSession(aSession),
    iMsgPtr((unsigned char*)0, 0)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CIptvMyVideosCopyOrMoveObserver::NewL()
// ---------------------------------------------------------
//
CIptvMyVideosCopyOrMoveObserver* CIptvMyVideosCopyOrMoveObserver::NewL(MIptvMyVideosClientObserver& aClientObserver, RIptvClientSession& aSession)
    {
    CIptvMyVideosCopyOrMoveObserver* self = new (ELeave) CIptvMyVideosCopyOrMoveObserver(aClientObserver, aSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosCopyOrMoveObserver::ConstructL()
// ---------------------------------------------------------
//
void CIptvMyVideosCopyOrMoveObserver::ConstructL()
    {
    }

// ---------------------------------------------------------
// CIptvMyVideosCopyOrMoveObserver::~CIptvMyVideosCopyOrMoveObserver()
// ---------------------------------------------------------
//
CIptvMyVideosCopyOrMoveObserver::~CIptvMyVideosCopyOrMoveObserver()
    {
    Cancel();

    delete iMsg;
    }

// ---------------------------------------------------------
// CIptvMyVideosCopyOrMoveObserver::SetActiveL()
// ---------------------------------------------------------
//
void CIptvMyVideosCopyOrMoveObserver::SetActiveL(TUint16 aCommand, CIptvMyVideosGlobalFileId& aSourceFileId, CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    if (! IsActive())
        {
        TInt dataSize = 0;

        delete iMsg;
        iMsg = NULL;

        dataSize  = KTUint32SizeInBytes; // aCommand
        dataSize += aSourceFileId.CountExternalizeSize();
        dataSize += aTargetFolderId.CountExternalizeSize();

        iMsg = HBufC8::NewL(dataSize > (KTUint32SizeInBytes * 2) ? dataSize : (KTUint32SizeInBytes * 2)); // Return values need 8 bytes.
        iMsgPtr.Set(iMsg->Des());      

        RDesWriteStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aSourceFileId.ExternalizeL(stream);
        aTargetFolderId.ExternalizeL(stream);
        CleanupStack::PopAndDestroy( &stream );

        iSession.SendRequest(aCommand, iMsgPtr, iStatus);
        CActive::SetActive();        
        }
    }
    
// ---------------------------------------------------------
// CIptvMyVideosCopyOrMoveObserver::SetActiveL()
// ---------------------------------------------------------
//
void CIptvMyVideosCopyOrMoveObserver::SetActiveL(TUint16 aCommand, CIptvMyVideosGlobalFolderId& aSourceFolderId, CIptvMyVideosGlobalFolderId& aTargetFolderId)
    {
    if (! IsActive())
        {
        TInt dataSize = 0;

        delete iMsg;
        iMsg = NULL;

        dataSize  = KTUint32SizeInBytes; // aCommand
        dataSize += aSourceFolderId.CountExternalizeSize();
        dataSize += aTargetFolderId.CountExternalizeSize();

        iMsg = HBufC8::NewL(dataSize > (KTUint32SizeInBytes * 2) ? dataSize : (KTUint32SizeInBytes * 2)); // Return values need 8 bytes.
        iMsgPtr.Set(iMsg->Des());      

        RDesWriteStream stream;
        stream.Open(iMsgPtr);
        CleanupClosePushL( stream );
        aSourceFolderId.ExternalizeL(stream);
        aTargetFolderId.ExternalizeL(stream);
        CleanupStack::PopAndDestroy( &stream );

        iSession.SendRequest(aCommand, iMsgPtr, iStatus);
        CActive::SetActive();        
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosCopyOrMoveObserver::RunL()
// Server has finished the copy or move request.
// -----------------------------------------------------------------------------
//
void CIptvMyVideosCopyOrMoveObserver::RunL()
    {
    TInt operationStatus = KErrGeneral;
    TInt failed          = 0;

    if (iStatus.Int() == KErrNone)
        {
        if (iMsg)
            {
            RDesReadStream stream;
            CleanupClosePushL( stream );
            stream.Open(iMsgPtr);
            operationStatus = stream.ReadUint32L();
            failed          = stream.ReadUint32L();
            CleanupStack::PopAndDestroy( &stream );
            }
        }

    delete iMsg;
    iMsg = NULL;

    // Call obsever, except in the case when server was cancelled.
    // This situation happens during shutdown so client is no more.    
    if (iStatus.Int() != KErrCancel)
        {
        iClientObserver.CopyOrMoveResponse(iStatus.Int(), operationStatus, failed);
        }
    }

// ---------------------------------------------------------
// CIptvMyVideosCopyOrMoveObserver::DoCancel()
// ---------------------------------------------------------
//
void CIptvMyVideosCopyOrMoveObserver::DoCancel()
    {
    }

// End of file.
