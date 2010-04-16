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
* Description: VideoThumbnailFetcher class implementation
*
*/

// Version : %version: %

// INCLUDE FILES
#include <qpixmap.h>
#include <thumbnailmanager_qt.h>

#include "videothumbnailfetcher.h"
#include "videocollectiontrace.h"

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::VideoThumbnailFetcher()
// -----------------------------------------------------------------------------
//
VideoThumbnailFetcher::VideoThumbnailFetcher() 
    : mThumbnailManager( 0 )
    , mPaused( false )
    , mTbnCreationEnabled( true )
{
	FUNC_LOG;
    mThumbnailManager = new ThumbnailManager();
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailMedium);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForPerformance);
    mThumbnailManager->setMode(ThumbnailManager::CropToAspectRatio);
    
    connect(mThumbnailManager, SIGNAL(thumbnailReady( QPixmap , void * , int , int )),
                this, SLOT(thumbnailReadySlot( QPixmap , void * , int , int )));
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::~VideoThumbnailFetcher()
// -----------------------------------------------------------------------------
//
VideoThumbnailFetcher::~VideoThumbnailFetcher()
{
	FUNC_LOG;
    cancelFetches();

    disconnect(mThumbnailManager, SIGNAL(thumbnailReady( QPixmap , void * , int , int )),
                this, SLOT(thumbnailReadySlot( QPixmap , void * , int , int )));
    
    delete mThumbnailManager;
    mThumbnailManager = 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::addFetch()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::addFetch(const QString fileName, void *internal, int priority)
{
    ThumbnailFetchData *fetch = new ThumbnailFetchData;
    fetch->mFileName = fileName;
    fetch->mInternal = internal;
    fetch->mPriority = priority;
    mFetchList.append(fetch);
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::continueFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::continueFetching()
{
	FUNC_LOG;
    mPaused = false;
    
    // First fetch all thumbnails that have been created already, next
    // start thumbnail creation for one thumbnail at a time. 
    if(!mFetchList.isEmpty())
    {
        startThumbnailFetches();
    }
    else if(!mCreationList.isEmpty())
    {
        startThumbnailCreation();
    }

    // All thumbnails have been fetched, report it.
    if(mFetchList.isEmpty() && mCreationList.isEmpty() && mStartedFetchList.isEmpty())
    {
        emit allThumbnailsFetched();
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::startThumbnailFetches()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::startThumbnailFetches()
{
	FUNC_LOG;
    if(!mThumbnailManager)
    {
        return;
    }
    
    // Only fetch those thumbnails that are already been created.
    mThumbnailManager->setMode(ThumbnailManager::DoNotCreate);
    
    // Push all from thumbnail manager.
    while(!mFetchList.isEmpty())
    {
        ThumbnailFetchData *fetch = mFetchList.takeFirst();

        int requestId = mThumbnailManager->getThumbnail(fetch->mFileName,
               fetch->mInternal, fetch->mPriority);
        
        if(requestId != -1)
        {
            // Request succeed, add to list of started fetches.
            mStartedFetchList.insert(requestId, fetch);
        }
        else
        {
            // Request failed, free internal data.
            delete fetch->mInternal;
            delete fetch;
        }
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::startThumbnailCreation()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::startThumbnailCreation()
{
	FUNC_LOG;
    if(!mThumbnailManager || !mTbnCreationEnabled)
    {
        return;
    }
    
    mThumbnailManager->setMode(ThumbnailManager::CropToAspectRatio);
    
    // Do nothing if list is empty. 
    if(mCreationList.isEmpty())
    {
        return;
    }
    
    // Find fetch with highest priority. 
    int highestPriority = 0;
    int indexWithHighestPriority = 0;
    
    for(int i = 0; i < mCreationList.count(); i++)
    {
        if(mCreationList.at(i)->mPriority > highestPriority)
        {
            indexWithHighestPriority = i;
            highestPriority = mCreationList.at(i)->mPriority;
        }
    }
    
    ThumbnailFetchData *fetch = mCreationList.takeAt(indexWithHighestPriority);
    
    // Do request to thumbnail manager.
    int requestId = mThumbnailManager->getThumbnail(fetch->mFileName,
            fetch->mInternal, fetch->mPriority);
    
    // Request failed, free internal data.
    if(requestId == -1)
    {
        delete fetch->mInternal;
        delete fetch;
    }
    else 
    {
        // Don't keep track of fetches when creating thumbnails, if
        // it fails with -1 it would be only tried to create again.  
        delete fetch;
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::pauseFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::pauseFetching()
{
	FUNC_LOG;
    mPaused = true;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::cancelFetches()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::cancelFetches()
{
	FUNC_LOG;
    // Clear list of started fetches, thumbnail manager has the internal 
    // pointer.
    QList<int> keys = mStartedFetchList.keys();
    for(int i = 0; i < keys.count(); i++ )
    {
        delete mStartedFetchList.take(keys[i]);
    }
    
    // Merge lists and free data.
    mFetchList.append(mCreationList);
    mCreationList.clear();
    while(!mFetchList.isEmpty())
    {
        ThumbnailFetchData *fetch = mFetchList.takeFirst();
        delete fetch->mInternal;
        delete fetch;
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::fetchCount()
// -----------------------------------------------------------------------------
//
int VideoThumbnailFetcher::fetchCount()
{
    return mFetchList.count() + mCreationList.count() + mStartedFetchList.count();
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::enableThumbnailCreation()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::enableThumbnailCreation(bool enable)
{
	FUNC_LOG;
    mTbnCreationEnabled = enable;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::thumbnailReadySlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::thumbnailReadySlot(QPixmap tnData, void *internal, int requestId, int error)
{
    // Thumbnail has not been generated yet, put it into creation list.
    if(error == -1 && internal)
    {
        if(mStartedFetchList.contains(requestId))
        {
            ThumbnailFetchData *fetch = mStartedFetchList.take(requestId);
            mCreationList.append(fetch);
        }
        else
        {
            // Fetch data was not found, meaning cancelFetches was called.
            delete internal;
        }
    }
    else
    {
        // Report that thumbnail was fetched.
        emit thumbnailReady(tnData, internal, error);
        
        if(mStartedFetchList.contains(requestId))
        {
            delete mStartedFetchList.take(requestId);
        }
    }
    
    // Continue the fetching process.
    if(!mPaused && mStartedFetchList.isEmpty())
    {
        continueFetching();
    }
}

// End of file.
