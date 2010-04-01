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
* Description: VideoThumbnailData class implementation
*
*/

// INCLUDE FILES
#include "videothumbnaildata.h"
#include "videothumbnaildata_p.h"

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoThumbnailData::VideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData &VideoThumbnailData::instance()
{
    static VideoThumbnailData _thumbnailData;
    return _thumbnailData;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::VideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData::VideoThumbnailData() :
d_ptr(new VideoThumbnailDataPrivate())
{
    connect(d_ptr, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)), 
            this, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::~VideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData::~VideoThumbnailData()
{
    disconnect(d_ptr, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)), 
            this, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    delete d_ptr;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::removeThumbnail()
// -----------------------------------------------------------------------------
//
bool VideoThumbnailData::removeThumbnail(TMPXItemId mediaId)
{
    return d_ptr->removeThumbnail(mediaId);
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::getThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailData::getThumbnail(TMPXItemId mediaId)
{
    return d_ptr->getThumbnail(mediaId);
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::startBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::startBackgroundFetching(VideoSortFilterProxyModel *model, int fetchIndex)
{
    d_ptr->startBackgroundFetching(model, fetchIndex);
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::enableBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::enableBackgroundFetching(bool enable)
{
    d_ptr->enableBackgroundFetching(enable);
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::enableThumbnailCreation()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::enableThumbnailCreation(bool enable)
{
    d_ptr->enableThumbnailCreation(enable);
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::freeThumbnailData()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::freeThumbnailData()
{
    d_ptr->freeThumbnailData();
}

// End of file
