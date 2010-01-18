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
* Description:    Implements VoD download API*
*/




// INCLUDE FILES
#include <e32svr.h>

#include <s32mem.h>
#include "IptvDebug.h"
#include "TIptvUint32StreamObject.h"

#include "CIptvMyVideosVideoBriefDetails.h"
#include "CIptvVodDlDownloadList.h"
#include "IptvClientServerCommon.h"
#include "CIptvVodDlClient.h"
#include "MIptvVodDlClientObserver.h"
#include "IptvServer.pan"
#include "CIptvVodDlDownloadReq.h"
#include "CIptvVodDlResumeDownloadReq.h"
#include "TIptvVodDlDownloadId.h"
#include "CIptvVodDlClient.h"
#include "CIptvVodDlDownloadEvent.h"
#include "CIptvVodDlCancelDownloadReq.h"
#include "CIptvEventListener.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvVodDlClient::CIptvVodDlClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CIptvVodDlClient::CIptvVodDlClient( MIptvVodDlClientObserver& aClientObserver ) 
: CIptvClientBase(), iClientObserver( aClientObserver ), iIpcMsgPtr((unsigned char*)0, 0)
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvVodDlClient::ConstructL()
    {
    iPendingReq = -1;
    
    iActiveSchedulerWait = new ( ELeave ) CActiveSchedulerWait();
    
    User::LeaveIfError( iSession.Connect() );
    
    CIptvVodDlDownloadEvent* event = CIptvVodDlDownloadEvent::NewL();
    CleanupStack::PushL(event); // 1->
    iEventListener = CIptvEventListener::NewL( *this,
                                               event /* ownership moves */,
                                               EIptvEngineVodDlGetEventSizeReq,
                                               EIptvEngineVodDlGetEventData,
                                               EIptvEngineVodDlCancelGetEventSizeReq );
    CleanupStack::Pop(event); // <-1

    iReqQueue.Reset();
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodDlClient* CIptvVodDlClient::NewL(MIptvVodDlClientObserver& aClientObserver)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::NewL()");

    CIptvVodDlClient* self = new ( ELeave ) CIptvVodDlClient( aClientObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::NewL() exit");

    return self;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvVodDlClient* CIptvVodDlClient::NewLC(MIptvVodDlClientObserver& aClientObserver)
    {
    CIptvVodDlClient* self = new ( ELeave ) CIptvVodDlClient( aClientObserver );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// Destructor
EXPORT_C CIptvVodDlClient::~CIptvVodDlClient()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::~CIptvVodDlClient() start");

    if (IsActive())
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: there is a message pending");
        if (iWaitForFlushInDestructor)
            {
            WaitForFlush();
            }
        else
            {
            IPTVLOGSTRING2_LOW_LEVEL("CIptvVodDlClient:: iWaitForFlushInDestructor was EFalse, discarding %d commands", iReqQueue.Count());
            Cancel();
            }
        }

    delete iEventListener;
    delete iIpcMsg;
        
    iReqQueue.Close();
    
    delete iActiveSchedulerWait;
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::~CIptvVodDlClient exit");
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::Purchase
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodDlClient::PurchaseL(TIptvServiceId /* aServiceId */,
                                          TIptvContentId /* aContentId */)
    {
    // 1) Create Ipc message function EIptvEngineVoDDlPurchaseReq with aServiceId and aContentId
    // 2) SendReceive message synchronously (request completed at server side)
    // 3) Return status to caller
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::DownloadL
// -----------------------------------------------------------------------------
EXPORT_C TIptvDlError CIptvVodDlClient::DownloadL(TUint32 aIapId,
                                                  TIptvServiceId aServiceId,
                                                  TIptvContentId aContentId,
                                                  TUint32 aIndex,                                                    
                                                  TIptvDownloadType aDlType)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::DownloadL()");
            
    CIptvVodDlDownloadReq* dlReq;
    dlReq = CIptvVodDlDownloadReq::NewL();
    CleanupStack::PushL(dlReq); // 1->

    dlReq->iIapId     = aIapId;
    dlReq->iServiceId = aServiceId;
    dlReq->iContentId = aContentId;
    dlReq->iIndex     = aIndex;
    dlReq->iDlType    = aDlType;

    //allocate a new IPC message
    HBufC8* ipcMsg = HBufC8::NewL(dlReq->CountExternalizeSize());
    CleanupStack::PushL(ipcMsg); // 2->
    
    TPtr8 ipcMsgPtr(ipcMsg->Des());

    RDesWriteStream writeStream;
    writeStream.Open(ipcMsgPtr);
    writeStream.PushL(); // 3->        
    dlReq->ExternalizeL(writeStream); //write the download item to the stream
    writeStream.Close();
    writeStream.Pop(); // <-3
    
           
    //send download item as TPtr8 over IPC
    TInt globalError = iSession.SendRequest(EIptvEngineVodDlDownload, ipcMsgPtr);
    
    TIptvDlError result;
    
    if(globalError == KErrNone)
        {
        RDesReadStream readStream;
        readStream.Open(ipcMsgPtr);
        readStream.PushL(); // 2->
        result = static_cast<TIptvDlError>(readStream.ReadUint8L());
        readStream.Close();
        readStream.Pop(); // <-2
        }
    else
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvVodDlClient:: dl request completed with error code: %d", globalError);
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: server side leaved?");
        result = EIptvDlGeneral;
        }
    
    CleanupStack::PopAndDestroy(ipcMsg); // <-2
    CleanupStack::PopAndDestroy(dlReq); // <-1

	
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::DownloadL() exit");
    return result;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::DownloadReqL
// -----------------------------------------------------------------------------
EXPORT_C void CIptvVodDlClient::DownloadReqL( TUint32 aIapId,
                                              TIptvServiceId aServiceId,
                                              TIptvContentId aContentId,
                                              TUint32 aContentIndex,
                                              TIptvDownloadType aDlType )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::DownloadReqL()");

    CIptvVodDlDownloadReq* dlReq = CIptvVodDlDownloadReq::NewL();
    CleanupStack::PushL( dlReq ); // 1->

    dlReq->iIapId     = aIapId;
    dlReq->iServiceId = aServiceId;
    dlReq->iContentId = aContentId;
    dlReq->iIndex     = aContentIndex;
    dlReq->iDlType    = aDlType;
    
    if ( IsActive() )
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: putting dl in queue");
        iReqQueue.AppendL(dlReq);
        CleanupStack::Pop(dlReq); // <-1
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: sending immedetially");
        SendDownloadReqL( *dlReq );
        CleanupStack::PopAndDestroy( dlReq ); // <-1
        }
       
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::DownloadReqL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::CancelDownloadL
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodDlClient::CancelDownloadL( CIptvMyVideosGlobalFileId& aGlobalFileId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::CancelDownloadL()");

    TIptvVodDlDownloadId downloadId;
    
    downloadId.iServiceIdAndContentIdAreValid = EFalse;
    downloadId.iServiceId                     = 0;
    downloadId.iContentId                     = 0;
    downloadId.iFileIdAndDriveAreValid        = ETrue;
    downloadId.iFileId                        = aGlobalFileId.iFileId;
    downloadId.iDrive                         = aGlobalFileId.iDrive;

    //no need to check queue since the global file id is valid -> cant be in queue
            
    TInt resp = SendRequestL(static_cast<TUint16>(EIptvEngineVodDlCancelDownload), downloadId);

    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::CancelDownloadL()");
    return resp;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::CancelDownloadL
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodDlClient::CancelDownloadL(TUint32 aServiceId, TUint32 aContentId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::CancelDownloadL()");

    TIptvVodDlDownloadId downloadId;
    
    downloadId.iServiceIdAndContentIdAreValid = ETrue;
    downloadId.iServiceId                     = aServiceId;
    downloadId.iContentId                     = aContentId;
    downloadId.iFileIdAndDriveAreValid        = EFalse;
    downloadId.iFileId                        = 0;
    downloadId.iDrive                         = 0;

    TBool removed = CancelDlFromQueueL(downloadId.iServiceId, downloadId.iContentId, EFalse /* dont send event */);
    if (removed)
        {            
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: dl cancelled from queue");
        return KErrNone;
        }
        
    TInt resp = SendRequestL(static_cast<TUint16>(EIptvEngineVodDlCancelDownload), downloadId);

    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::CancelDownloadL()");
    return resp;
    }
        
// -----------------------------------------------------------------------------
// CIptvVodDlClient::CancelDownloadReqL
// -----------------------------------------------------------------------------
EXPORT_C void CIptvVodDlClient::CancelDownloadReqL(TIptvVodDlDownloadId& aDownloadId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::CancelDownloadReqL(aDownloadId) start");
    
    if (aDownloadId.iServiceIdAndContentIdAreValid == EFalse && 
        aDownloadId.iFileIdAndDriveAreValid == EFalse)
        {
        User::Leave(KErrArgument);
        }

    //can be in queue only if iServiceIdAndContentIdAreValid is valid.
    //Cant be in queue if global file id is valid
    if (aDownloadId.iServiceIdAndContentIdAreValid)
        {
        TBool removed = CancelDlFromQueueL(aDownloadId.iServiceId, aDownloadId.iContentId, ETrue /* send event */);
        if (removed)
            {            
            IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: dl cancelled from queue");
            return;
            }
        }
    
    CIptvVodDlCancelDownloadReq* cancelDlReq = CIptvVodDlCancelDownloadReq::NewL();
    CleanupStack::PushL(cancelDlReq); // 1->
    
    cancelDlReq->Set(aDownloadId);
        
    if (IsActive())
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: active -> putting CancelDlReq in queue");
        iReqQueue.AppendL(cancelDlReq);
        CleanupStack::Pop(cancelDlReq); // <-1
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: sending cancel dl req immedetially");
        SendCancelDownloadReqL(*cancelDlReq);
        CleanupStack::PopAndDestroy(cancelDlReq); // <-1
        }
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::CancelDownloadReqL(aDownloadId) exit");
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::PauseDownload
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodDlClient::PauseDownloadL(CIptvMyVideosGlobalFileId& aGlobalFileId)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::PauseDownloadL()");
           
    //send download item as TPtr8 over IPC
    TInt result = SendRequestL(static_cast<TUint16>(EIptvEngineVodDlPauseDownload),
                               aGlobalFileId);

    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::PauseDownloadL() exit");
    return result;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::PauseAllDownloads
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodDlClient::PauseAllDownloads()
    {
    return iSession.SendRequest(static_cast<TUint16>(EIptvEngineVodDlPauseAllDownloads));
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::ResumeDownload
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodDlClient::ResumeDownloadL(CIptvMyVideosGlobalFileId& aGlobalFileId,
                                                TBool aUseUserNameAndPassword,
                                                const TDesC& aUserName,
                                                const TDesC& aPassword,
                                                TUint32 aIapId,
                                                TBool aUseIapId)
    {
    if(aUserName.Length() > KIptvVodDlUserNameMaxLength)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: User name too long");
        return KErrArgument;        
        }
        
    if(aPassword.Length() > KIptvVodDlPasswordMaxLength)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: Password too long");
        return KErrArgument;
        }

    CIptvVodDlResumeDownloadReq* resumeDownloadReq = CIptvVodDlResumeDownloadReq::NewL();
    CleanupStack::PushL(resumeDownloadReq); // 1->

    resumeDownloadReq->iDownloadId.iFileIdAndDriveAreValid        = ETrue;
    resumeDownloadReq->iDownloadId.iFileId                        = aGlobalFileId.iFileId;
    resumeDownloadReq->iDownloadId.iDrive                         = aGlobalFileId.iDrive;
    resumeDownloadReq->iDownloadId.iServiceIdAndContentIdAreValid = EFalse;
    resumeDownloadReq->iUseUserNameAndPassword                    = aUseUserNameAndPassword;
    resumeDownloadReq->iUserName                                  = aUserName;
    resumeDownloadReq->iPassword                                  = aPassword;
    resumeDownloadReq->iIapId                                     = aIapId;
    resumeDownloadReq->iUseIapId                                  = aUseIapId;
    
    TInt resp = SendRequestL(static_cast<TUint16>(EIptvEngineVodDlResumeDownload), *resumeDownloadReq);
    CleanupStack::PopAndDestroy(resumeDownloadReq); // <-1
    return resp;    
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::ResumeDownload
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodDlClient::ResumeDownloadL(TUint32 aServiceId,
                                                TUint32 aContentId,
                                                TBool aUseUserNameAndPassword,
                                                const TDesC& aUserName,
                                                const TDesC& aPassword,
                                                TUint32 aIapId,
                                                TBool aUseIapId)
    {
    if(aUserName.Length() > KIptvVodDlUserNameMaxLength)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: User name too long");
        return KErrArgument;        
        }
        
    if(aPassword.Length() > KIptvVodDlPasswordMaxLength)
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: Password too long");
        return KErrArgument;
        }

    CIptvVodDlResumeDownloadReq* resumeDownloadReq = CIptvVodDlResumeDownloadReq::NewL();
    CleanupStack::PushL(resumeDownloadReq); // 1->
    
    resumeDownloadReq->iDownloadId.iFileIdAndDriveAreValid        = EFalse;
    resumeDownloadReq->iDownloadId.iServiceIdAndContentIdAreValid = ETrue;
    resumeDownloadReq->iDownloadId.iServiceId                     = aServiceId;
    resumeDownloadReq->iDownloadId.iContentId                     = aContentId;
    resumeDownloadReq->iUseUserNameAndPassword                    = aUseUserNameAndPassword;
    resumeDownloadReq->iUserName                                  = aUserName;
    resumeDownloadReq->iPassword                                  = aPassword;
    resumeDownloadReq->iIapId                                     = aIapId;
    resumeDownloadReq->iUseIapId                                  = aUseIapId;
    
    TInt resp = SendRequestL(static_cast<TUint16>(EIptvEngineVodDlResumeDownload), *resumeDownloadReq);
    CleanupStack::PopAndDestroy(resumeDownloadReq); // <-1
    return resp;    
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::ResumeAllDownloads
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodDlClient::ResumeAllDownloads()
    {    
    return iSession.SendRequest(static_cast<TUint16>(EIptvEngineVodDlResumeAllDownloads));
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::GetDownloadList
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodDlClient::GetDownloadListL(CIptvVodDlDownloadList& aDownloadList)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::GetDownloadListL()");

    TIptvUint32StreamObject sizeObj;
    
    TInt globalError = SendRequestL(static_cast<TUint16>(EIptvEngineVodDlGetDownloadListSize),
                       sizeObj,
                       sizeObj.CountExternalizeSize() /* = 4 */);
    
    if(globalError == KErrNone)
        {
        if(sizeObj.iData > 0)
            {
            globalError = SendRequestL(static_cast<TUint16>(EIptvEngineVodDlGetDownloadList),
                                       aDownloadList,
                                       sizeObj.iData);
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: download list was empty");
            globalError = KErrNotFound;            
            }
        }
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::GetDownloadListL() exit");
    return globalError;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::GetDownloadProgress
// -----------------------------------------------------------------------------
EXPORT_C TInt CIptvVodDlClient::GetDownloadProgressL(CIptvMyVideosGlobalFileId& aGlobalFileId)
    {
    // 1) Create Ipc message function EIptvEngineVoDDlGetDownloadProgressReq with aGlobalFileId
    // 2) SendReceive message synchronously
    // 3) Return progress and status to caller
        
    TIptvUint32StreamObject progress;

    TInt result = SendRequestL(static_cast<TUint16>(EIptvEngineVodDlGetDownloadProgressReq),
                               aGlobalFileId,
                               progress,
                               0);

    if(result != KErrNone)
        {
        return result;
        }
    else 
        {   
        return static_cast<TInt>(progress.iData);
        }

    }

// -----------------------------------------------------------------------------
// Updates existing soft notification.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlClient::UpdateDownloadNotification(
    const TDesC& aMsg )
    {
    if ( !IsActive() )
        {
        TRAP_IGNORE( SendRequestL(
            EIptvEngineVodDlUpdateDownloadNotification,
            aMsg ) );
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::RunL
// -----------------------------------------------------------------------------
void CIptvVodDlClient::RunL()
    {
    //build common parts of the download event
    CIptvVodDlDownloadEvent* dlEvent = CIptvVodDlDownloadEvent::NewL();
    CleanupStack::PushL(dlEvent); // 1->
    dlEvent->iServiceIdAndContentIdAreValid = iReqDownloadId.iServiceIdAndContentIdAreValid;
    dlEvent->iServiceId                     = iReqDownloadId.iServiceId;          
    dlEvent->iContentId                     = iReqDownloadId.iContentId;
    dlEvent->iGlobalFileIdIsValid           = iReqDownloadId.iFileIdAndDriveAreValid;
    dlEvent->iGlobalFileId->iFileId         = iReqDownloadId.iFileId;
    dlEvent->iGlobalFileId->iDrive          = iReqDownloadId.iDrive;
    
    dlEvent->iGlobalError = iStatus.Int();

    switch(iPendingReq)
        {
        //response to asynchronous dl req
        case EIptvEngineVodDlDownloadReq:
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: RunL, response to download req arrived");

            iPendingReq = -1;

            if (iStatus.Int() == KErrNone)
                {
                //VOD DL Manager generated the response
                RDesReadStream readStream;
                readStream.Open(iIpcMsgPtr);
                readStream.PushL(); // 2->
                TUint8 dlError = readStream.ReadUint8L();
                readStream.Close();
                readStream.Pop(); // <-2
                dlEvent->iError = static_cast<TIptvDlError>(dlError);
                if (dlEvent->iError != EIptvDlNoError)
                    {
                    //VOD DL Manager informs that download request failed
                    dlEvent->iEvent = CIptvVodDlDownloadEvent::EDownloadReqFailed;
                    }
                else
                    {
                    //VOD DL Manager informs that download request was ok
                    dlEvent->iEvent = CIptvVodDlDownloadEvent::EDownloadReqSucceeded;
                    }
                }
            else
                {
                //Request failed before it reached VOD DL manager
                dlEvent->iEvent = CIptvVodDlDownloadEvent::EDownloadReqFailed;
                dlEvent->iError = EIptvDlGeneral;
                }

            delete iIpcMsg;
            iIpcMsg = NULL;

            iClientObserver.HandleVodDlDownloadEvent(*dlEvent);

            HandleNextReqInQueueL();

            }
            break;
            
        case EIptvEngineVodDlCancelDownloadReq:
            {
            iPendingReq = -1;

            if (iStatus.Int() == KErrNone)
                {
                dlEvent->iEvent = CIptvVodDlDownloadEvent::ECancelReqSucceeded;
                dlEvent->iError = EIptvDlNoError;
                }
            else
                {
                dlEvent->iEvent = CIptvVodDlDownloadEvent::ECancelReqFailed;
                dlEvent->iError = EIptvDlGeneral;
                }
                
            iClientObserver.HandleVodDlDownloadEvent(*dlEvent);
            
            HandleNextReqInQueueL();
            }
            break;
        }
        
    CleanupStack::PopAndDestroy(dlEvent); // <-1
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::DoCancel
// -----------------------------------------------------------------------------
void CIptvVodDlClient::DoCancel()
    {
    switch(iPendingReq)
        {
        case EIptvEngineVodDlDownloadReq:
        case EIptvEngineVodDlCancelDownloadReq:
            iSession.SendRequest(EIptvEngineVodDlCancel);
            break;
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::HandleEvent
// -----------------------------------------------------------------------------
void CIptvVodDlClient::HandleEvent( MIptvEvent& aEvent )
    {
    iClientObserver.HandleVodDlDownloadEvent( static_cast<CIptvVodDlDownloadEvent&>(aEvent) );
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::CancelDlFromQueueL()
// -----------------------------------------------------------------------------
//
TBool CIptvVodDlClient::CancelDlFromQueueL(TUint32 aServiceId,
                                           TUint32 aContentId,
                                           TBool aSendEvent)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::CancelDlFromQueueL() start");
    
    if (iReqQueue.Count() > 0)
        {
        CIptvVodDlDownloadReq* dlReq;
        TInt i;
        for (i = 0; i < iReqQueue.Count(); i++)
            {
            if (iReqQueue[i]->iObjId == EIptvEngineVodDlDownloadReq)
                {
                dlReq = static_cast<CIptvVodDlDownloadReq*>(iReqQueue[i]);
                if (aServiceId == dlReq->iServiceId &&
                    aContentId == dlReq->iContentId)
                    {
                    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: dl was still on client queue, deleting it and sending cancel event");

                    if (aSendEvent)
                        {
                        SendCancelEventL(dlReq->iServiceId,
                                         dlReq->iContentId,
                                         ETrue /* ETrue = cancel successfull */);
                        }

                    iReqQueue.Remove(i);
                    iReqQueue.Compress();
                    return ETrue;
                    }                
                }
            }
        }
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::CancelDlFromQueueL() exit");
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::SendCancelEventL()
// -----------------------------------------------------------------------------
//
void CIptvVodDlClient::SendCancelEventL(TUint32 aServiceId,
                                        TUint32 aContentId,
                                        TBool aSuccess)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::SendCancelEventL() start");

    CIptvVodDlDownloadEvent* dlEvent = CIptvVodDlDownloadEvent::NewL();
    CleanupStack::PushL(dlEvent); // 1->
    if (aSuccess)
        {
        dlEvent->iEvent = CIptvVodDlDownloadEvent::ECancelReqSucceeded;
        }
    else
        {
        dlEvent->iEvent = CIptvVodDlDownloadEvent::ECancelReqFailed;
        }
        
    dlEvent->iServiceIdAndContentIdAreValid = iReqDownloadId.iServiceIdAndContentIdAreValid;
    dlEvent->iServiceId                     = aServiceId;          
    dlEvent->iContentId                     = aContentId;
    dlEvent->iGlobalFileIdIsValid           = EFalse;
    dlEvent->iGlobalError                   = KErrNone;
    dlEvent->iError                         = EIptvDlNoError;

    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: sending cancel download response event");
    iClientObserver.HandleVodDlDownloadEvent(*dlEvent);
    
    CleanupStack::PopAndDestroy(dlEvent); // <-1
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::SendCancelEventL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::SendCancelDownloadReqL()
// -----------------------------------------------------------------------------
//
void CIptvVodDlClient::SendCancelDownloadReqL(CIptvVodDlCancelDownloadReq& aCancelDlReq)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::SendCancelDownloadReqL() start");
    
    //store download id so we can generate response event    
    iReqDownloadId.Set(aCancelDlReq.iDownloadId);

    //delete old IPC message
    if(iIpcMsg)
        {
        delete iIpcMsg;
        iIpcMsg = NULL;
        }

    //allocate a new IPC message
    iIpcMsg = HBufC8::NewL(aCancelDlReq.CountExternalizeSize());
    
    iIpcMsgPtr.Set(iIpcMsg->Des());

    RDesWriteStream writeStream;
    writeStream.Open(iIpcMsgPtr);
    writeStream.PushL(); // 1->
    aCancelDlReq.ExternalizeL(writeStream); //write the item to the stream
    writeStream.Pop(); // <-1
    writeStream.Close();
    
    //send async message           
    iSession.SendRequest(EIptvEngineVodDlCancelDownloadReq, iIpcMsgPtr, iStatus);
    SetActive();
    iPendingReq = EIptvEngineVodDlCancelDownloadReq;

    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::SendCancelDownloadReqL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::SendDownloadReqL()
// -----------------------------------------------------------------------------
//
void CIptvVodDlClient::SendDownloadReqL(CIptvVodDlDownloadReq& aDownloadReq)
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::SendDownloadReqL() start");

    //store download id so that we can generate response event
    iReqDownloadId.iServiceIdAndContentIdAreValid = ETrue;
    iReqDownloadId.iServiceId                     = aDownloadReq.iServiceId;
    iReqDownloadId.iContentId                     = aDownloadReq.iContentId;
    iReqDownloadId.iFileIdAndDriveAreValid        = EFalse;
    iReqDownloadId.iFileId                        = 0;
    iReqDownloadId.iDrive                         = 0;
                
    //delete old IPC message
    if (iIpcMsg)
        {
        delete iIpcMsg;
        iIpcMsg = NULL;
        }

    //allocate a new IPC message
    iIpcMsg = HBufC8::NewL(aDownloadReq.CountExternalizeSize());
    
    iIpcMsgPtr.Set(iIpcMsg->Des());

    RDesWriteStream writeStream;
    writeStream.Open(iIpcMsgPtr);
    writeStream.PushL(); // 1->
    aDownloadReq.ExternalizeL(writeStream); //write the download item to the stream
    writeStream.Pop(); // <-1
    writeStream.Close();
                                
    //send download item as TPtr8 over IPC asynchronously
    iSession.SendRequest(EIptvEngineVodDlDownloadReq, iIpcMsgPtr, iStatus);

    SetActive();    
    iPendingReq = EIptvEngineVodDlDownloadReq;

    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::SendDownloadReqL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::HandleNextReqInQueueL()
// -----------------------------------------------------------------------------
//
void CIptvVodDlClient::HandleNextReqInQueueL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::HandleNextReqInQueueL() start");

    if (iReqQueue.Count() > 0)
        {
        switch(iReqQueue[0]->iObjId)
            {
            case EIptvEngineVodDlDownloadReq:
                SendDownloadReqL(*(static_cast<CIptvVodDlDownloadReq*>(iReqQueue[0])));
                delete static_cast<CIptvVodDlDownloadReq*>(iReqQueue[0]);
                break;
                
            case EIptvEngineVodDlCancelDownloadReq:
                SendCancelDownloadReqL(*(static_cast<CIptvVodDlCancelDownloadReq*>(iReqQueue[0])));
                delete static_cast<CIptvVodDlCancelDownloadReq*>(iReqQueue[0]);
                break;
            }
        iReqQueue[0] = NULL;
        iReqQueue.Remove(0);
        iReqQueue.Compress();
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: no request in queue");
        
        if ( iWaitingForFlush )
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: destructing was going on..stopping activescheduler");
            iActiveSchedulerWait->AsyncStop();
            }
        }
        
    
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::HandleNextReqInQueueL() exit");
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::SetWaitForFlushInDestructor()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlClient::SetWaitForFlushInDestructor(TBool aWaitForFlushInDestructor)
    {
    iWaitForFlushInDestructor = aWaitForFlushInDestructor;
    }

// -----------------------------------------------------------------------------
// CIptvVodDlClient::WaitForFlush()
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvVodDlClient::WaitForFlush()
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::WaitForFlush() start");
    
    if (IsActive())
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: waiting for flush");
        iWaitingForFlush = ETrue;
        if ( ! iActiveSchedulerWait->IsStarted() )
            {
            iActiveSchedulerWait->Start();
            }
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: flush finished");
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient:: there were no messages pending, continuing");
        }
        
    IPTVLOGSTRING_LOW_LEVEL("CIptvVodDlClient::WaitForFlush() exit");
    }
