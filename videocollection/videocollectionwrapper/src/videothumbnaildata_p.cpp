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
* Description:  VideoThumbnailDataPrivate class implementation
*
*/

// INCLUDE FILES
#include <qapplication.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <mpxmediageneraldefs.h>
#include <vcxmyvideosdefs.h>

#include "videothumbnaildata_p.h"
#include "videocollectionwrapper.h"
#include "videosortfilterproxymodel.h"
#include "videothumbnailfetcher.h"

// Maximum thumbnails kept in memory.
const int THUMBNAIL_CACHE_SIZE = 60;
// Maximum of thumbnail fetches done at one background fetch round.
const int THUMBNAIL_BACKGROUND_FETCH_AMOUNT = 20;
// Milliseconds for the background fetch timer.
const int THUMBNAIL_BACKGROUND_TIMEOUT = 100;
// Milliseconds while thumbnail ready events are gathered before they 
// are signaled.
const int THUMBNAIL_READY_SIGNAL_TIMEOUT = 50;
// Priority for background thumbnail fetches.
const int BACKGROUND_FETCH_PRIORITY = 3000;

/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::VideoThumbnailDataPrivate()
// -----------------------------------------------------------------------------
//
VideoThumbnailDataPrivate::VideoThumbnailDataPrivate() :
    mThumbnailFetcher(0),
    mDefaultTnVideo(0),
    mDefaultTnCategory(0),
    mCurrentModel(0),
    mCurrentFetchIndex(0),
    mCurrentBackgroundFetchCount(0),
    mBgFetchTimer(0),
    mTbnReportTimer(0),
    mSignalsConnected(false),
    mBackgroundFetchingEnabled(true)
{
    initialize();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::~VideoThumbnailDataPrivate()
// -----------------------------------------------------------------------------
//
VideoThumbnailDataPrivate::~VideoThumbnailDataPrivate()
{
    cleanup();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::initialize()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::initialize()
{
    mThumbnailData.setMaxCost(THUMBNAIL_CACHE_SIZE);
    
    if(!mThumbnailFetcher)
    {
        mThumbnailFetcher = new VideoThumbnailFetcher();        
    }

    if(!mBgFetchTimer)
    {
        mBgFetchTimer = new QTimer();
    }
    
    if(!mTbnReportTimer)
    {
        mTbnReportTimer = new QTimer();
    }

    if(connectSignals() < 0)
    {
        cleanup();
        return -1;
    }
    
    return 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::cleanup()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::cleanup()
{
    disconnectSignals();

    delete mThumbnailFetcher;
    mThumbnailFetcher = 0;
    
    freeThumbnailData();
    
    if(mTbnReportTimer)
    {
        mTbnReportTimer->stop();
        delete mTbnReportTimer;
        mTbnReportTimer = 0;
    }
    
    if(mBgFetchTimer)
    {
        mBgFetchTimer->stop();
        delete mBgFetchTimer;
        mBgFetchTimer = 0;
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::disconnectSignals()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::disconnectSignals()
{
    if(mSignalsConnected)
    {
        VideoSortFilterProxyModel *model = 
                VideoCollectionWrapper::instance().getModel(VideoCollectionWrapper::EAllVideos);
        if(model)
            {
            disconnect(model->sourceModel(), SIGNAL(modelReady()), this, SLOT(modelChangedSlot()));
            disconnect(model->sourceModel(), SIGNAL(modelChanged()), this, SLOT(modelChangedSlot()));
            }
		disconnect(mThumbnailFetcher, SIGNAL(thumbnailReady( QPixmap , void *, int )),
                    this, SLOT(thumbnailReadySlot( QPixmap , void *, int )));
        disconnect(mThumbnailFetcher, SIGNAL(allThumbnailsFetched()),
                 this, SLOT(allThumbnailsFetchedSlot()));          
        disconnect(mBgFetchTimer, SIGNAL(timeout()), this, SLOT(doBackgroundFetching()));
        disconnect(mTbnReportTimer, SIGNAL(timeout()), this, SLOT(reportThumbnailsReadySlot()));
    }
    mSignalsConnected = false;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::connectSignals()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::connectSignals()
{
    if(!mSignalsConnected)
    {
        VideoSortFilterProxyModel *model = 
                VideoCollectionWrapper::instance().getModel(VideoCollectionWrapper::EAllVideos);
        if(!model)
            return -1;
        if(!connect(mThumbnailFetcher, SIGNAL(thumbnailReady( QPixmap , void *, int )),
                    this, SLOT(thumbnailReadySlot( QPixmap , void *, int))) ||
            !connect(mThumbnailFetcher, SIGNAL(allThumbnailsFetched()),
                     this, SLOT(allThumbnailsFetchedSlot())) ||
           !connect(model->sourceModel(), SIGNAL(modelReady()), this, SLOT(modelChangedSlot())) ||
           !connect(model->sourceModel(), SIGNAL(modelChanged()), this, SLOT(modelChangedSlot())) ||
           !connect(mBgFetchTimer, SIGNAL(timeout()), this, SLOT(doBackgroundFetching())) ||
           !connect(mTbnReportTimer, SIGNAL(timeout()), this, SLOT(reportThumbnailsReadySlot())))
        {
            return -1;
        }
        
        QApplication *app = qApp;
        if(!connect(app, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuitSlot())))
        {
            return -1;
        }
        
        mSignalsConnected = true;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::getThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailDataPrivate::getThumbnail(TMPXItemId mediaId)
{
    const QIcon *thumbnail = mThumbnailData[mediaId];
    if(!thumbnail)
    {
        return defaultThumbnail(mediaId);
    }
    return thumbnail;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::startFetchingThumbnails()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::startFetchingThumbnails(const QList<QModelIndex> &indexes, int priority)
{
    if(!mCurrentModel || !mThumbnailFetcher)
    {
        return -1;
    }
    if(indexes.count() == 0)
    {
        return 0;
    }
    
    mThumbnailFetcher->pauseFetching();

    int fetchCountBefore = mThumbnailFetcher->fetchCount();
    
    // Fetch the thumbnails
    for(int i = 0; i < indexes.count(); i++)
    {
        startFetchingThumbnail(mCurrentModel->getMediaIdAtIndex(indexes[i]), priority--);
    }
    
    int started = mThumbnailFetcher->fetchCount() - fetchCountBefore;
    
    mThumbnailFetcher->continueFetching();
    
    return started;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::startFetchingThumbnail()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::startFetchingThumbnail(TMPXItemId mediaId, int priority)
{
    if(!mCurrentModel || !mThumbnailFetcher)
    {
        return -1;
    }

    // Check that it's not fetched before.
    if(mThumbnailData.contains(mediaId))
    {
        return 0;
    }

    QString fileName = mCurrentModel->getMediaFilePathForId(mediaId);
    
    // object containing media id to be passed throught
    // thumbnail generation process.
    TMPXItemId *internal = new TMPXItemId(mediaId.iId1, mediaId.iId2);
    
    // Thumbnail fetcher signals into thumbnailReadySlot when thumbnail ready
    if(fileName.length() > 0)
    {
        mThumbnailFetcher->addFetch(fileName, internal, priority);
    }

    return 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::doBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::doBackgroundFetching()
{
    if(!mCurrentModel || !mThumbnailFetcher)
    {
        return;
    }

    if(mCurrentBackgroundFetchCount >= THUMBNAIL_CACHE_SIZE)
    {
        return;
    }
    
    int maxIndex = mCurrentModel->rowCount();
    if(maxIndex == 0)
    {
        return;
    }

    // Delta to UI index where fetch has been done already.  
    int currentDelta = mCurrentBackgroundFetchCount/2;
    
    // How many will be fetched.  
    const int fetchAmount = THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2;

    QList<QModelIndex> indexes;

    // Items after the current fetch index.
    int startIndex = mCurrentFetchIndex+currentDelta;
    int endIndex = mCurrentFetchIndex+currentDelta+fetchAmount;
    getModelIndexes(indexes, startIndex, endIndex);

    // Items before the current fetch index.
    startIndex = mCurrentFetchIndex-currentDelta-fetchAmount;
    endIndex = mCurrentFetchIndex-currentDelta;
    getModelIndexes(indexes, startIndex, endIndex);
    
    mCurrentBackgroundFetchCount += THUMBNAIL_BACKGROUND_FETCH_AMOUNT;

    int fetchesStarted = startFetchingThumbnails(indexes, BACKGROUND_FETCH_PRIORITY);
    
    // No thumbnails to fetch, start again.
    if(fetchesStarted == 0)
    {
        continueBackgroundFetch();
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::getModelIndexes()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::getModelIndexes(QList<QModelIndex> &indexes, int startIndex, int endIndex)
{
    QModelIndex index;
    for(int i = startIndex; i < endIndex; i++)
    {
        if(i >= 0)
        {
            index = mCurrentModel->index(i, 0);
            if(index.isValid())
            {
                indexes.append(index);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::thumbnailReadySlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::thumbnailReadySlot(QPixmap tnData, void *internal, int error)
{
    TMPXItemId mediaId(0, 0);
    if(internal)
    {
        mediaId = *(static_cast<TMPXItemId*>(internal));
        delete internal;
    }
    else
    {
        return;
    }

    if(!error && !tnData.isNull())
    {
        mThumbnailData.insert(mediaId, new QIcon(tnData));
        
        // Gather list of media ids and emit thumbnailReady signals in larger set
        // when timer goes off.
        if(mTbnReportTimer && !mTbnReportTimer->isActive())
        {
            mTbnReportTimer->setSingleShot(true);
            mTbnReportTimer->start(THUMBNAIL_READY_SIGNAL_TIMEOUT);
        }

        // Save the media id for the signal.  
        mReadyThumbnailMediaIds.append(mediaId);
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::reportThumbnailsReadySlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::reportThumbnailsReadySlot()
{
    emit thumbnailsFetched(mReadyThumbnailMediaIds);
    mReadyThumbnailMediaIds.clear();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::allThumbnailsFetchedSlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::allThumbnailsFetchedSlot()
{
    continueBackgroundFetch();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::modelChangedSlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::modelChangedSlot()
{
    startBackgroundFetching(mCurrentModel, mCurrentFetchIndex);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::defaultThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailDataPrivate::defaultThumbnail(TMPXItemId mediaId)
{
    // Is thumbnail for a video or a category.
    if(mediaId.iId2 == KVcxMvcMediaTypeVideo) 
    {
        if(!mDefaultTnVideo)
            mDefaultTnVideo = new QIcon(":/icons/default_thumbnail_video.svg");
        return mDefaultTnVideo;
    }
    else
    {
        if(!mDefaultTnCategory)
            mDefaultTnCategory = new QIcon(":/icons/default_thumbnail_collection.svg");
        return mDefaultTnCategory;
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::removeThumbnail()
// -----------------------------------------------------------------------------
//
bool VideoThumbnailDataPrivate::removeThumbnail(TMPXItemId mediaId)
{
    return mThumbnailData.remove(mediaId);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::enableBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::enableBackgroundFetching(bool enable)
{
    mBackgroundFetchingEnabled = enable;
    startBackgroundFetching(mCurrentModel, 0);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::enableThumbnailCreation()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::enableThumbnailCreation(bool enable)
{
    if(mThumbnailFetcher)
        mThumbnailFetcher->enableThumbnailCreation(enable);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::freeThumbnailData()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::freeThumbnailData()
{
    // Stop timers.
    if(mBgFetchTimer)
        mBgFetchTimer->stop();
    
    if(mTbnReportTimer)
        mTbnReportTimer->stop();

    if(mThumbnailFetcher)
        mThumbnailFetcher->cancelFetches();
    
    // Clear data.
    mReadyThumbnailMediaIds.clear();
    mThumbnailData.clear();
    
    delete mDefaultTnVideo;
    mDefaultTnVideo = 0;
    
    delete mDefaultTnCategory;
    mDefaultTnCategory = 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::startBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::startBackgroundFetching(VideoSortFilterProxyModel *model, int fetchIndex)
{
    if(!mBackgroundFetchingEnabled || !mThumbnailFetcher)
        return;
    
    mThumbnailFetcher->cancelFetches();
    
    // If model is null, we continue using the current one. 
    if(model)
        mCurrentModel = model;
    
    mCurrentFetchIndex = fetchIndex;
    mCurrentBackgroundFetchCount = 0;
    doBackgroundFetching();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::continueBackgroundFetch()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::continueBackgroundFetch()
{
    if(!mBackgroundFetchingEnabled)
        return;

    if(mBgFetchTimer)
    {
        mBgFetchTimer->stop();
        mBgFetchTimer->setSingleShot(true);
        mBgFetchTimer->start(THUMBNAIL_BACKGROUND_TIMEOUT);
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::aboutToQuitSlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::aboutToQuitSlot()
{
    cleanup();
}

// End of file
