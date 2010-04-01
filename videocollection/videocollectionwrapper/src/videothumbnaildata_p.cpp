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
#include <QApplication>
#include <QPixmap>
#include <QTimer>
#include <mpxmediageneraldefs.h>
#include <thumbnailmanager_qt.h>

#include "videothumbnaildata_p.h"
#include "videocollectionwrapper.h"
#include "videosortfilterproxymodel.h"

// Maximum thumbnails kept in memory.
const int THUMBNAIL_CACHE_SIZE = 60;
// Maximum of thumbnail fetches done at one background fetch round.
const int THUMBNAIL_BACKGROUND_FETCH_AMOUNT = 20;
// Milliseconds for the background fetch timer.
const int THUMBNAIL_BACKGROUND_TIMEOUT = 100;
// Maximum simultaneous thumbnail fetches.
const int THUMBNAIL_MAX_SIMULTANEOUS_FETCHES = THUMBNAIL_BACKGROUND_FETCH_AMOUNT * 10;
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
    mThumbnailManager(0),
    mDefaultTnVideo(0),
    mDefaultTnCategory(0),
    mCollectionWrapper(VideoCollectionWrapper::instance()),
    mModel(0),
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
    
    if(!mCollectionWrapper)
    {
        return -1;
    }   
    
    if(!mThumbnailManager)
    {
        mThumbnailManager = new ThumbnailManager();
        mThumbnailManager->setThumbnailSize( ThumbnailManager::ThumbnailMedium );
        mThumbnailManager->setQualityPreference( ThumbnailManager::OptimizeForPerformance );
    }

    mModel = mCollectionWrapper->getModel();
    if(!mModel)
    {
        cleanup();
        return -1;
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
    if(mCollectionWrapper)
    {
        mCollectionWrapper->decreaseReferenceCount();
        mCollectionWrapper = 0;
    }
    
    disconnectSignals();

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

    delete mThumbnailManager;
    mThumbnailManager = 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::disconnectSignals()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::disconnectSignals()
{
    if(mSignalsConnected)
    {
        disconnect(mThumbnailManager, SIGNAL(thumbnailReady( QPixmap , void * , int , int )),
                    this, SLOT(thumbnailReadySlot( QPixmap , void * , int , int )));
        disconnect(mModel, SIGNAL(layoutChanged()), this, SLOT(layoutChangedSlot()));
        disconnect(mModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
                    this, SLOT(rowsInsertedSlot(const QModelIndex &, int, int)));
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
        if(!connect(mThumbnailManager, SIGNAL(thumbnailReady( QPixmap , void * , int , int )),
                    this, SLOT(thumbnailReadySlot( QPixmap , void * , int , int ))) ||
           !connect(mModel, SIGNAL(layoutChanged()), this, SLOT(layoutChangedSlot())) ||
           !connect(mModel, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
                    this, SLOT(rowsInsertedSlot(const QModelIndex &, int, int))) ||
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
    if(!mModel || !mThumbnailManager)
    {
        return -1;
    }
    if(indexes.count() == 0)
    {
        return 0;
    }
    
    // Now we fetch the requested thumbnails with higher priority than any of the current fetches.
    // TODO: Better would be to cancel the current fetches but it causes crashes in 10.1.   
    
    int fetchCountBefore = mFetchList.count();
    
    // Fetch the thumbnails in ascending priority.

    int startPriority = fetchCountBefore + priority + indexes.count();
    
    for(int i = 0; i < indexes.count(); i++)
    {
        startFetchingThumbnail(mModel->getMediaIdAtIndex(indexes[i]), startPriority-i);
    }
    
    return mFetchList.count() - fetchCountBefore;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::startFetchingThumbnail()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::startFetchingThumbnail(TMPXItemId mediaId, int priority)
{
    if(!mModel || !mThumbnailManager)
    {
        return -1;
    }
    if(mFetchList.count() >= THUMBNAIL_MAX_SIMULTANEOUS_FETCHES)
    {
        return -1;
    }

    // Check that it's not fetched before.
    if(mThumbnailData.contains(mediaId))
    {
        return 0;
    }

    QString fileName = mModel->getMediaFilePathForId(mediaId);

    // object containing media id to be passed throught
    // thumbnail generation process.
    TMPXItemId *internal = new TMPXItemId(mediaId.iId1, mediaId.iId2);
    
    int tnId = -1;
    // mThumbnailManager signals into thumbnailReadySlot when thumbnail ready
    if(fileName.length() > 0)
    {
        tnId = mThumbnailManager->getThumbnail(fileName, internal, priority);
    }

    if( tnId != -1 )
    {
        // add to fetching list to indicate we're fetching this tn.
        mFetchList.insert(tnId);
    }
    else
    {
        // tn getting starting failed for some reason
        delete internal;
    }
    return tnId;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::doBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::doBackgroundFetching()
{
    if(!mModel)
    {
        return;
    }

    if(mCurrentBackgroundFetchCount >= THUMBNAIL_CACHE_SIZE)
    {
        return;
    }
    
    int maxIndex = mModel->rowCount();
    if(maxIndex == 0)
    {
        return;
    }

    // Delta to UI index where fetch has been done already.  
    int currentDelta = mCurrentBackgroundFetchCount/2;
    
    // How many will be fetched.  
    const int fetchAmount = THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2;

    QList<QModelIndex> indexes;

    // Items before the current fetch index.
    int startIndex = mCurrentFetchIndex-currentDelta-fetchAmount;
    int endIndex = mCurrentFetchIndex-currentDelta;
    getModelIndexes(indexes, startIndex, endIndex);

    // Items after the current fetch index.
    startIndex = mCurrentFetchIndex+currentDelta;
    endIndex = mCurrentFetchIndex+currentDelta+fetchAmount;
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
            index = mModel->index(i, 0);
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
void VideoThumbnailDataPrivate::thumbnailReadySlot(QPixmap tnData, void *internal , int id, int error)
{
    // Tn ready, either failed or not it must be removed from the fetch list.
    // It's not stored if it's not found from the list. 
    if(!removeFromFetchList(id))
    {
        if(internal)
            delete internal;
        return;
    }
    
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
// VideoThumbnailDataPrivate::layoutChangedSlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::layoutChangedSlot()
{
    startBackgroundFetching(mCurrentFetchIndex);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::rowsInsertedSlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::rowsInsertedSlot(const QModelIndex & /* parent */, int /* start */, int /* end */)
{
    startBackgroundFetching(mCurrentFetchIndex);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::defaultThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailDataPrivate::defaultThumbnail(TMPXItemId mediaId)
{
    // Is thumbnail for a video or a category.
    if(mediaId.iId2 == 0)
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
    startBackgroundFetching(0);
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

    // Clear data.
    mFetchList.clear();
    mReadyThumbnailMediaIds.clear();
    mThumbnailData.clear();
    
    delete mDefaultTnVideo;
    mDefaultTnVideo = 0;
    
    delete mDefaultTnCategory;
    mDefaultTnCategory = 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::removeFromFetchList()
// -----------------------------------------------------------------------------
//
bool VideoThumbnailDataPrivate::removeFromFetchList(int tnId)
{
    if(mFetchList.isEmpty())
    {
        return false;
    }
    bool removed = mFetchList.remove(tnId);
    if(mFetchList.isEmpty())
    {
        continueBackgroundFetch();
    }
    return removed;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::startBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::startBackgroundFetching(int fetchIndex)
{
    if(!mBackgroundFetchingEnabled)
        return;

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
