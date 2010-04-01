/*
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
* Description: CVideoThumbnailData class implementation
* 
*/

// INCLUDE FILES
#include <QDebug>
#include <QPixmap>
#include <QTimer>
#include <mpxmediageneraldefs.h>
#include <thumbnailmanager_qt.h>

#include "videothumbnaildata.h"

int VideoThumbnailData::mStartFetchingThumbnailsCallCount = 0;
int VideoThumbnailData::mStartFetchingThumbnailsThumbnailCount = 0;
int VideoThumbnailData::mInstanceCallCount = 0;
int VideoThumbnailData::mBackgroundThumbnailFetchingEnabled = true;
int VideoThumbnailData::mFreeThumbnailDataCallCount = 0;
int VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;

// -----------------------------------------------------------------------------
// VideoThumbnailData::instance()
// -----------------------------------------------------------------------------
//
VideoThumbnailData &VideoThumbnailData::instance()
{
    mInstanceCallCount++;
    static VideoThumbnailData _thumbnailData;
    return _thumbnailData;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::VideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData::VideoThumbnailData() 
{
    
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::~VideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData::~VideoThumbnailData()
{
    
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::startFetchingThumbnails()
// -----------------------------------------------------------------------------
//
int VideoThumbnailData::startFetchingThumbnails(const QList<int> &indexes, VideoThumbnailPriority /* priority */, bool /*setFetchIndex*/)
{
    mStartFetchingThumbnailsCallCount++;
    mStartFetchingThumbnailsThumbnailCount += indexes.count();
    return 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::startBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::startBackgroundFetching(int fetchIndex)
{
    mStartBackgroundFetchingCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::enableBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::enableBackgroundFetching(bool enable)
{
    mBackgroundThumbnailFetchingEnabled = enable;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::freeThumbnailData()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::freeThumbnailData()
{
    mFreeThumbnailDataCallCount++;
}

// End of file
