/**
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   helper class to test VideoThumbnailData
*
*/

#ifndef _VIDEOTHUMBNAILDATATESTER_H_
#define _VIDEOTHUMBNAILDATATESTER_H_

// INCLUDES
#include <QObject>
#include "videothumbnaildata_p.h"

class VideoThumbnailDataTester : public VideoThumbnailDataPrivate
{
    Q_OBJECT

public:

    /**
     * Default constructor
     */
    VideoThumbnailDataTester();

    /**
     * Destructor
     */
    virtual ~VideoThumbnailDataTester();

    //
    // Test methods for VideoThumbnailData's signals.
    //
    
    /**
     * Emits doBackgroundFetching signal to itself.
     */
    void emitDoBackgroundFetching();
    
    /**
     * Emits reportThumbnailsReadySignal to itself.
     */
    void emitReportThumbnailsReady();    
    
    /**
     * Emits layoutChanged signal to itself.
     */
    void emitLayoutChanged();
    
    /**
     * Emits rowsInserted signal to itself.
     *
     * @param parent parent
     * @param start start index
     * @param end end index
     */
    void emitRowsInserted(const QModelIndex & parent, int start, int end);
    
    //
    // Test methods for VideoThumbnailData's methods.
    //
    
    /**
     * Calls CVideoThumbnailDataPrivate::removeFromFetchList
     *
     * @param tnId id of thumbnail to be removed.
     */
    void removeFromFetchList(int tnId);

    /**
     * Calls CVideoThumbnailDataPrivate::initialize
     */
    void initialize();

    /**
     * Calls CVideoThumbnailDataPrivate::cleanup
     */
    void cleanup();
    
    /**
     * Calls CVideoThumbnailDataPrivate::connectSignals
     */
    void connectSignals();

    /**
     * Calls CVideoThumbnailDataPrivate::disconnectSignals
     */
    void disconnectSignals();
    
    /**
     * Calls CVideoThumbnailDataPrivate::defaultThumbnail
     */
    const QIcon* defaultThumbnail(TMPXItemId mediaId);
    
    /**
     * Calls CVideoThumbnailDataPrivate::startBackgroundFetching
     */
    void startBackgroundFetching(int fetchIndex);    
    
    /**
     * Calls CVideoThumbnailDataPrivate::continueBackgroundFetch
     */
    void continueBackgroundFetch();
    
signals:

    /**
     * Signal.
     */
    void doBackgroundFetchingSignal();

    /**
     * Signal.
     */
    void layoutChangedSignal();

    /**
     * Signal.
     */
    void rowsInsertedSignal(const QModelIndex & parent, int start, int end);

    /**
     * Signal.
     */
    void reportThumbnailsReadySignal();    

private:

};

#endif // _VIDEOTHUMBNAILDATATESTER_H_

// End of file

