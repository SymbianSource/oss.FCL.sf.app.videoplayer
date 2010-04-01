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

// INCLUDES
#define private public
#include "videothumbnaildata_p.h"
#undef private

#include "videothumbnaildatatester.h"

// -----------------------------------------------------------------------------
// VideoTnDataTester::VideoTnDataTester()
// -----------------------------------------------------------------------------
//
VideoThumbnailDataTester::VideoThumbnailDataTester()
{
    connect(this, SIGNAL(doBackgroundFetchingSignal()), this, SLOT(doBackgroundFetching()));
    connect(this, SIGNAL(layoutChangedSignal()), this, SLOT(layoutChangedSlot()));
    connect(this, SIGNAL(rowsInsertedSignal(const QModelIndex &, int, int)), this, SLOT(rowsInsertedSlot(const QModelIndex &, int, int)));
    connect(this, SIGNAL(reportThumbnailsReadySignal()), this, SLOT(reportThumbnailsReadySlot()));
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::~VideoThumbnailDataTester()
// -----------------------------------------------------------------------------
//
VideoThumbnailDataTester::~VideoThumbnailDataTester()
{
    disconnect(this, SIGNAL(doBackgroundFetchingSignal()), this, SLOT(doBackgroundFetching()));
    disconnect(this, SIGNAL(layoutChangedSignal()), this, SLOT(layoutChangedSlot()));
    disconnect(this, SIGNAL(rowsInsertedSignal(const QModelIndex &, int, int)), this, SLOT(rowsInsertedSlot(const QModelIndex &, int, int)));
    disconnect(this, SIGNAL(reportThumbnailsReadySignal()), this, SLOT(reportThumbnailsReadySlot()));
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::emitDoBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::emitDoBackgroundFetching()
{
    emit doBackgroundFetchingSignal();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::emitDoBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::emitReportThumbnailsReady()
{
    emit reportThumbnailsReadySignal();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::emitLayoutChanged()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::emitLayoutChanged()
{
    emit layoutChangedSignal();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::emitRowsInserted()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::emitRowsInserted(const QModelIndex & parent, int start, int end)
{
    emit rowsInsertedSignal(parent, start, end);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::removeFromFetchList()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::removeFromFetchList(int tnId)
{
    VideoThumbnailDataPrivate::removeFromFetchList(tnId);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::initialize()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::initialize()
{
    VideoThumbnailDataPrivate::initialize();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::cleanup()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::cleanup()
{
    VideoThumbnailDataPrivate::cleanup();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::connectSignals()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::connectSignals()
{
    VideoThumbnailDataPrivate::connectSignals();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::disconnectSignals()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::disconnectSignals()
{
    VideoThumbnailDataPrivate::disconnectSignals();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::defaultThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailDataTester::defaultThumbnail(TMPXItemId mediaId)
{
    return VideoThumbnailDataPrivate::defaultThumbnail(mediaId);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::startBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::startBackgroundFetching(int fetchIndex)
{
    VideoThumbnailDataPrivate::startBackgroundFetching(fetchIndex);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::continueBackgroundFetch()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::continueBackgroundFetch()
{
    VideoThumbnailDataPrivate::continueBackgroundFetch();
}

// End of file

