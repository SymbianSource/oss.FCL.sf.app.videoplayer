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
* Description:    Observer for completed total video length requests.*
*/




// INCLUDE FILES
#include <s32mem.h>
#include "IptvClientServerCommon.h"
#include "RIptvClientSession.h"
#include "CIptvMyVideosTotalVideoLengthObserver.h"
#include "MIptvMyVideosClientObserver.h"

// CONSTANTS
const TInt KTUint32SizeInBytes = 4;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIptvMyVideosTotalVideoLengthObserver::CIptvMyVideosTotalVideoLengthObserver()
// ---------------------------------------------------------
//
CIptvMyVideosTotalVideoLengthObserver::CIptvMyVideosTotalVideoLengthObserver(MIptvMyVideosClientObserver& aClientObserver, RIptvClientSession& aSession) 
 :  CActive(EPriorityStandard),
    iClientObserver(aClientObserver),
    iSession(aSession),
    iMsgPtr((unsigned char*)0, 0)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CIptvMyVideosTotalVideoLengthObserver::NewL()
// ---------------------------------------------------------
//
CIptvMyVideosTotalVideoLengthObserver* CIptvMyVideosTotalVideoLengthObserver::NewL(MIptvMyVideosClientObserver& aClientObserver, RIptvClientSession& aSession)
    {
	CIptvMyVideosTotalVideoLengthObserver* self = new (ELeave) CIptvMyVideosTotalVideoLengthObserver(aClientObserver, aSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// ---------------------------------------------------------
// CIptvMyVideosTotalVideoLengthObserver::ConstructL()
// ---------------------------------------------------------
//
void CIptvMyVideosTotalVideoLengthObserver::ConstructL()
    {
    }

// ---------------------------------------------------------
// CIptvMyVideosTotalVideoLengthObserver::~CIptvMyVideosTotalVideoLengthObserver()
// ---------------------------------------------------------
//
CIptvMyVideosTotalVideoLengthObserver::~CIptvMyVideosTotalVideoLengthObserver()
    {
    if ( IsActive() )
        {
        Cancel();
        }

    delete iMsg;
    }

// ---------------------------------------------------------
// CIptvMyVideosTotalVideoLengthObserver::SetActiveL()
// ---------------------------------------------------------
//
void CIptvMyVideosTotalVideoLengthObserver::SetActiveL()
    {
    if (! IsActive())
        {
        delete iMsg;
        iMsg = NULL;

        iMsg = HBufC8::NewL(2 * KTUint32SizeInBytes);
        iMsgPtr.Set(iMsg->Des());
        iSession.SendRequest(EIptvEngineMyVideosGetTotalVideoLenghtRequestReq, iMsgPtr, iStatus);
        SetActive();        
        }
    }

// ---------------------------------------------------------
// CIptvMyVideosTotalVideoLengthObserver::RunL()
// ---------------------------------------------------------
//
void CIptvMyVideosTotalVideoLengthObserver::RunL()
    {
    if (iStatus.Int() == KErrNone)
        {
        TIptvPlayTime  playTime = 0;
        TIptvFileSize  fileSize = 0;

        if (iMsg)
            {
            RDesReadStream stream;
            stream.Open(iMsgPtr);
            CleanupClosePushL( stream );
            playTime = stream.ReadUint32L();
            fileSize = stream.ReadUint32L();
            CleanupStack::PopAndDestroy( &stream );
            }
            
        iClientObserver.TotalVideoLenghtResponse(playTime, fileSize);
        }
    // Inform client about errors too, otherwise it might stay waiting for ever.
    else
        {
        iClientObserver.TotalVideoLenghtResponse(0, 0);
        }
 
    delete iMsg;
    iMsg = NULL;
    }

// ---------------------------------------------------------
// CIptvMyVideosTotalVideoLengthObserver::DoCancel()
// ---------------------------------------------------------
//
void CIptvMyVideosTotalVideoLengthObserver::DoCancel()
    {
    iClientObserver.TotalVideoLenghtResponse(0, 0);
    }

// End of file.
