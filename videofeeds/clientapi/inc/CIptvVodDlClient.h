/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    VoD download API*
*/




#ifndef CIPTVVODDLCLIENT_H
#define CIPTVVODDLCLIENT_H

#include <e32base.h>
#include "CIptvUtil.h"
#include "IptvEngineUids.h"

#include "CIptvClientBase.h"
#include "CIptvMyVideosVideoBriefDetails.h"
#include "CIptvVodDlEventListener.h"
#include "CIptvVodDlDownloadEvent.h"
#include "TIptvVodDlDownloadId.h"
#include "CIptvVodDlDownloadList.h"
#include "CIptvVodDlDownloadListItem.h"
#include "MIptvVodDlClientObserver.h"
#include "CIptvService.h"
#include "MIptvEventListenerObserver.h"

class CIptvVodDlDownloadReq;
class CIptvVodDlDownloadList;
class CIptvVodDlDownloadWatcher;
class CIptvVodDlCancelDownloadReq;
class CIptvEventListener;

/**
 *  VoD download API
 *
 *  Notice, if you set SetWaitForFlushInDestructor(ETrue), then when calling this class'
 *  destructor, the destructing might take some time and during that time dl events might arrive.
 *  Client must be able to handle events untill the destruct method has finished.
 *  This is usually achieved by placing this class as a first in destruction chain
 *  of the application. One can also use WaitForFlush() method before deleting the object to achieve
 *  similar functionality. If neither method is used, then all requests which are not sent yet
 *  to server are lost in deletion.
 *
 *  @lib IptvEngineClient.dll
 */
class CIptvVodDlClient : public CIptvClientBase,
                         public MIptvEventListenerObserver
    {

public:

    /**
     * Two-phased constructor.
     */
    static IMPORT_C CIptvVodDlClient* NewL (
        MIptvVodDlClientObserver& aClientObserver );

    /**
     * Two-phased constructor.
     */
    static IMPORT_C CIptvVodDlClient* NewLC(
        MIptvVodDlClientObserver& aClientObserver );

    /**
     * Destructor.
     */
    virtual IMPORT_C ~CIptvVodDlClient();

    /**
     * Purchase content from service. Not Supported.
     * @param aServiceId Service provider ID identifier.
     * @param aContentId Content ID identifier.
     * @return Error code, KErrNone if no errors,
     * KErrCouldNotConnect if server needs permission for connection, otherwise one of the system wide error codes.
     */
    IMPORT_C TInt PurchaseL(
        TIptvServiceId aServiceId,
        TIptvContentId aContentId );

    /**
     * Download content from service.
     * @param aIapId
     * @param aServiceId Service provider ID identifier.
     * @param aContentId Content ID identifier.
     * @param aIndex     Content access index.
     * @param aDltype    Type of the requested download, currently only EIptvDownloadTypeImmediate
     *                   supported.
     * @return           Error code, indicates if download is started successfully. If return code is
     *                   EIptvDlNoError, then further download status is reported via events, on error
     *                   no further events will arrive.
     */
    IMPORT_C TIptvDlError DownloadL(
        TUint32 aIapId,
        TIptvServiceId aServiceId,
        TIptvContentId aContentId,
        TUint32 aIndex,
        TIptvDownloadType aDlType );

    /**
     * Download content from service. Download status is reported via events.
     * Download requests are buffered on client side. Use WaitForFlush() method
     * to ensure that requests have been sent to server.
     * @param aIapId.
     * @param aServiceId    Service provider ID identifier.
     * @param aContentId    Content ID identifier.
     * @param aContentIndex Content access index.
     * @param aDltype       Type of the requested download, currently only EIptvDownloadTypeImmediate
     *                      supported.
     */
    IMPORT_C void DownloadReqL(
        TUint32 aIapId,
        TIptvServiceId aServiceId,
        TIptvContentId aContentId,
        TUint32 aContentIndex,
        TIptvDownloadType aDlType );

    /**
     * Cancels download.
     * Operation includes clean up that removes files from file system,
     * deletes data related to aGlobalFileId and releases aGlobalFileId identifier.
     * @param aGlobalFileId Download ID identifier, reference to actual file under download.
     * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
     */
    IMPORT_C TInt CancelDownloadL(
        CIptvMyVideosGlobalFileId& aGlobalFileId );

    /**
     * Cancels download.
     * Operation includes clean up that removes files from file system,
     * deletes data related to FileId and releases FileId identifier.
     * @param aServiceId
     * @param aContentId
     * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
     *         KErrNotFound if download was not found from VOD DL Manager, file might still be in
     *         My Videos.
     */
    IMPORT_C TInt CancelDownloadL(
        TUint32 aServiceId,
        TUint32 aContentId );

    /**
     * Cancels download asynchronously. See CIptvVodDlDownloadEvent::TEvent for
     * response events.
     * Download cancel requests are buffered on client side. Use WaitForFlush() method
     * to ensure that requests have been sent to server.
     * Operation includes clean up that removes files from file system,
     * deletes data related to FileId and releases FileId identifier.
     * @param aDownloadId For cancel to success, either one of iServiceId/iContentId
     *                    OR iFileId&iDriveId pairs must be set.
     */
    IMPORT_C void CancelDownloadReqL(
        TIptvVodDlDownloadId& aDownloadId );

    /**
     * Pause download.
     * @param  aFileId Download ID identifier, reference to actual file under download.
     * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
     */
    IMPORT_C TInt PauseDownloadL(
        CIptvMyVideosGlobalFileId& aFileId );

    /**
     * Pauses all downloads.
     * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
     */
    IMPORT_C TInt PauseAllDownloads();

    /**
     * Resume download.
     * @param aFileId reference to file under download.
     * @param aUseUserNameAndPassword if EFalse, aUserName and aPassword are ignored.
     * @param aUserName HTTP user name
     * @param aPassword HTTP password
     * @param aIapId
     * @param aUseIapId if EFalse, aIapId is ignored
     * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
     */
    IMPORT_C TInt ResumeDownloadL(
        CIptvMyVideosGlobalFileId& aFileId,
        TBool aUseUserNameAndPassword,
        const TDesC& aUserName,
        const TDesC& aPassword,
        TUint32 aIapId,
        TBool aUseIapId );

    /**
     * Resume download. This is meant to to be used in early download phaze when global
     * file ID is not valid yet.
     * @param aServiceId
     * @param aContentId
     * @param aUseUserNameAndPassword if EFalse, aUserName and aPassword are ignored.
     * @param aUserName HTTP user name, KIptvVodDlUserNameMaxLength is the maximum length
     * @param aPassword HTTP password, KIptvVodDlPasswordMaxLength is the maximum length
     * @param aIapId
     * @param aUseIapId if EFalse, aIapId is ignored
     * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
     */
    IMPORT_C TInt ResumeDownloadL(
        TUint32 aServiceId,
        TUint32 aContentId,
        TBool aUseUserNameAndPassword,
        const TDesC& aUserName,
        const TDesC& aPassword,
        TUint32 aIapId,
        TBool aUseIapId );

    /**
     * Resumes all downloads.
     * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
     */
    IMPORT_C TInt ResumeAllDownloads();

    /**
     * Gets list of items selected for download.
     * @param aDownloadList
     * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
     */
    IMPORT_C TInt GetDownloadListL(
        CIptvVodDlDownloadList& aDownloadList );

    /**
     * Get download progess percentage for selected download file ID.
     * @since
     * @param aGlobalFileId Download file ID identifier, reference to actual file under download.
     * @return Progress in %. Decimal
     *         0 for not started,
     *         1-99 for ongoing and
     *         100 for completed download.
     *         If error, one of the system wide error codes returned.
     */
    IMPORT_C TInt GetDownloadProgressL(
        CIptvMyVideosGlobalFileId& aGlobalFileId );

    /**
     * From MIptvEventListenerObserver.
     * Event is routed to Client observer (MIptvVodDlClientObserver).
     * @param aEvent
     */
    void HandleEvent( MIptvEvent& aEvent );

    /**
     * Configures flush behaviour in destructor.
     * @param aWaitForFlushInDestructor If ETrue, then this objects destructor will block until
     *                                  all commands have been passed to server. EFalse will discard
     *                                  commands and destruct immedetially. EFalse is on as default.
     */
    IMPORT_C void SetWaitForFlushInDestructor(
        TBool aWaitForFlushInDestructor );

    /**
     * Blocks until all commands have been flushed to server.
     */
    IMPORT_C void WaitForFlush();

    /**
     * Updates existing soft notification.
     *
     * @param aMsg Notification information.
     */
    IMPORT_C void UpdateDownloadNotification( const TDesC& aMsg );

private:

    /**
     * C++ default constructor.
     */
    CIptvVodDlClient(MIptvVodDlClientObserver& aClientObserver);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * From CActive, handles an active object’s request completion event
     */
    void RunL();

    /**
     * From CActive, implements cancellation of an outstanding request.
     * This function is called as part of the active object’s Cancel().
     */
    void DoCancel();

    /**
     * Removes Dl request from the queue.
     * @param aServiceId
     * @param aContentId
     * @param aSendEvent If ETrue, (and the dl req was found from queue)
     *                   the CIptvVodDlDownloadEvent::ECancelReqSucceeded event is sent
     *                   to client. If EFalse, then no event is sent.
     * @return           ETrue if dl req was removed from the queue, EFalse otherwise.
     */
    TBool CancelDlFromQueueL(
        TUint32 aServiceId,
        TUint32 aContentId,
        TBool aSendEvent );

    /**
     * Sends cancel download response event to client.
     * @param aSuccess if ETrue then CIptvVodDlDownloadEvent::EDownloadReqSucceeded event is sent,
     *                 if EFalse then CIptvVodDlDownloadEvent::EDownloadReqFailed event is sent.
     */
    void SendCancelEventL(
        TUint32 aServiceId,
        TUint32 aContentId,
        TBool aSuccess );

#if 0
    /**
     * Gets next download request from the queue. Ownership moves.
     * Item is removed from the iReqQueue.
     * @return pointer to download req, NULL if not found.
     */
    CIptvVodDlDownloadReq* GetNextDlReqFromQueueL();

    /**
     * Extracts cancel download req object from request queue.
     * @return object extracted, NULL if none found.
     */
    CIptvVodDlCancelDownloadReq* GetNextCancelDlReqFromQueueL();
#endif

    /**
     * Sends cancel download request. Active object is set active.
     * @aParam aCancelDlReq request object to send.
     */
    void SendCancelDownloadReqL(
        CIptvVodDlCancelDownloadReq& aCancelDlReq );

    /**
     * Sends download req. Active object is set active.
     * @aParam aDownloadReq request object to send.
     */
    void SendDownloadReqL(
        CIptvVodDlDownloadReq& aDownloadReq );

    /**
     * Sends next request from the queue.
     */
    void HandleNextReqInQueueL();

public:

    MIptvVodDlClientObserver& iClientObserver;

private: // Data

    /**
     * Listens events from the server side (CIptvEventGenerator sends them).
     */
    CIptvEventListener*        iEventListener;

    HBufC8*                    iIpcMsg;
    TPtr8                      iIpcMsgPtr;

    /**
     * The last asynchronous message sent. Negative
     * value means that there is no msg pending.
     */
    TInt                       iPendingReq;

    /**
     * Used to store download id for the asynchronous request. If asynchronous
     * request fails before download object is created
     * in VOD DL Manager, this object is used to generate fail event
     * to client.
     */
    TIptvVodDlDownloadId     iReqDownloadId;

    /**
     * Asynchronous download requests which can't be sent immedetially are queued here.
     */
    RArray<MIptvStreamObject*> iReqQueue;

    /**
     * This is ETrue when this object is flush wait pending.
     */
    TBool                      iWaitingForFlush;

    /**
     * If ETrue, then destructor will block until commands have been flushed
     * to server.
     */
    TBool                      iWaitForFlushInDestructor;

    /**
     * Used to send requests on the background, allowing client to continue
     * normal activities.
     */
    CActiveSchedulerWait*      iActiveSchedulerWait;

    };

#endif // CIPTVVODDLCLIENT_H
