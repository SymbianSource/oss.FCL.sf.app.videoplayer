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
* Description:    My Videos Client API*
*/




#ifndef MIPTVMYVIDEOSCLIENTOBSERVER_H
#define MIPTVMYVIDEOSCLIENTOBSERVER_H

#include "CIptvUtil.h"

/**
 * Observer for My Videos API that needs to be implemented by application.
 *
 * @lib IptvClientApi.dll
 */
class MIptvMyVideosClientObserver
    {

public:

    /**
     * The event type for ContentsUpdated callback method.
     */
    enum TIptvContentUpdatedEventType
        {
        EContentUpdated,    // Major changes, all content should be updated
        EAdded,             // Individual video added
        EUpdated,           // Individual video updated
        ERemoved            // Individual video deleted
        };

    /**
     * Helper event class for ContentsUpdated callback method.
     */
    class TIptvContentUpdatedEvent
        {

    public:

        /**
         * The type of the change
         */
        TIptvContentUpdatedEventType    iEventType;

        /**
         * The drive number, this can be 0 in some cases.
         */
        TInt iDrive;

        /**
         * Drive specific file ID identifier, this can be 0 in some cases.
         */
        TIptvFileId iFileId;
        };

// New functions

    /**
     * Asynchronous callback observer function signaling application that
     * My Videos video contents has changed.
     */
    virtual void ContentsUpdated( TIptvContentUpdatedEvent aEvent ) = 0;

    /**
     * Asynchronous response callback function to video GetTotalVideoLenght().
     * Result is provided asynchronously because calculation may take some
     * time.
     *
     * @param aTotalPlayTime Total play time in seconds.
     * @param aTotalFileSize Total file size in kilo bytes.
     */
    virtual void TotalVideoLenghtResponse(
        TIptvPlayTime aTotalPlayTime,
        TIptvFileSize aTotalFileSize ) = 0;

    /**
     * Asynchronous callback function signaling that copy or move operation
     * has been finished.
     *
     * @param aServerStatus    Request status.
     *                           KErrNone if no error,
     *                           KErrCancel if cancelled, etc.
     * @param aOperationStatus Status of actual copy or move operation.
     *                           KErrNone if no error,
     *                           KErrDiskFull if disk full, etc.
     * @param aFailed          If several items was copied or moved and some
     *                         failed, the count of failed items.
     */
    virtual void CopyOrMoveResponse(
        TInt aServerStatus,
        TInt aOperationStatus,
        TInt aFailed ) = 0;

    /**
     * Asynchronous callback function signaling that delete operation has
     * been finished.
     *
     * @param aStatus Status of delete operation.
     *                  KErrNone if no error,
     *                  KErrDiskFull if disk full, etc.
     */
    virtual void DeleteResponse( TInt aStatus ) = 0;

    };

#endif // MIPTVMYVIDEOSCLIENTOBSERVER_H
