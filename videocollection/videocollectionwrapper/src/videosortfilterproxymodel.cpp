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
* Description:   Videosortfilterproxymodel implementation
*
*/

#include <qstringlist.h>
#include <qtimer.h>
#include <qdatetime.h>

#include "videothumbnaildata.h"
#include "videocollectioncommon.h"
#include "videosortfilterproxymodel.h"
#include "videolistdatamodel.h"
#include "videocollectionclient.h"

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::VideoSortFilterProxyModel(QObject *parent) :
 QSortFilterProxyModel(parent),
 mModel(0),
 mCollectionClient(0),
 mLevel(-1),
 mIdleSortTimer(0),
 mWantedSortRole(VideoCollectionCommon::KeyDateTime)
{
    // NOP
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::~VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::~VideoSortFilterProxyModel()
{
    delete mCollectionClient;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::initialize
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::initialize(VideoListDataModel *sourceModel)
{
    if(!mCollectionClient) {
        if(!connect(sourceModel, SIGNAL(fullVideoDetailsReady(int)),
                this, SIGNAL(fullDetailsReady(int)))) {
            return -1;
        }
        
        if(!connect(sourceModel, SIGNAL(modelReady()),
                this, SIGNAL(modelReady()))) {
            return -1;
        }

        mCollectionClient = new VideoCollectionClient();
        if(!mCollectionClient ||
           mCollectionClient->initialize() < 0 ||
           sourceModel->initialize(mCollectionClient) < 0) {
            delete mCollectionClient;
            mCollectionClient = 0;
            disconnect(sourceModel, SIGNAL(fullVideoDetailsReady(int)),
                this, SIGNAL(fullDetailsReady(int)));
            return -1;
        }

    } else {
        if(sourceModel->initialize(mCollectionClient) < 0) {
            return -1;
        }
    }

    mModel = sourceModel;
    setSourceModel(sourceModel);
    return 0;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::open
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::open(int level)
{
    if(mLevel != level) {
        mLevel = level;
        return mCollectionClient->startOpenCollection(level);
    }

    return 0;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::doSorting
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::doSorting(int sortingRole, Qt::SortOrder order, 
        bool async)
{
    if((sortingRole != Qt::DisplayRole) &&
       (sortingRole != VideoCollectionCommon::KeySizeValue) &&
       (sortingRole != VideoCollectionCommon::KeyDateTime))
    {
        // default sorting order is by date
        mWantedSortRole = VideoCollectionCommon::KeyDateTime;
    }
    else
    {
        mWantedSortRole = sortingRole;
    }
    
    mWantedSortOrder = order;
    
    if(async)
    {
        if(mIdleSortTimer)
        {
            if(mIdleSortTimer->isActive())
            {
                mIdleSortTimer->stop();
            }
        }
        else
        {
            mIdleSortTimer = new QTimer(this);
            connect(mIdleSortTimer, SIGNAL(timeout()), this, SLOT(processSorting()));
        }
        mIdleSortTimer->start(0);
    } else {
        processSorting();
    }
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::deleteItems
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::deleteItems(const QModelIndexList &indexList)
{
    if(mModel)
    {
        QModelIndexList mappedList;
        for(int i = 0; i < indexList.count(); ++i)
        {
            mappedList.append(mapToSource(indexList.at(i)));
        }
        
        if(mModel->removeRows(mappedList))
        {
            // Start fetching thumbnails at start of the model.
            VideoThumbnailData::instance().startBackgroundFetching(0);
            
            return 0;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::openItem
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::openItem(const QModelIndex &index)
{
    // getMediaIdAtIndex maps index to source
    TMPXItemId mpxId1 = getMediaIdAtIndex(index);
    if(mpxId1 != TMPXItemId::InvalidId() && mCollectionClient)
    {
        return mCollectionClient->openVideo(mpxId1);
    }
    return -1;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::back
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::back()
{
    if(mCollectionClient)
    {
        return mCollectionClient->back();
    }
    return -1;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::fetchItemDetails
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::fetchItemDetails(const QModelIndex &index)
{
    // no need to map index beforehand, because 
	// getMediaIdAtIndex maps it
    TMPXItemId mpxId1 = getMediaIdAtIndex(index);
    if(mpxId1 != TMPXItemId::InvalidId() && mCollectionClient)
    {
        if(mCollectionClient->getVideoDetails(mpxId1) == 0)
        {
			// because full details comes from model, map index gotten from UI
			// into sourcemodel index as well for consistency's sake
			QModelIndex sourceIndex = mapToSource(index);
            emit shortDetailsReady(sourceIndex.row());
            return 0;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::processSorting
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::processSorting()
{
    if(mIdleSortTimer)
    {
        mIdleSortTimer->stop();
    }

    if(sortRole() != mWantedSortRole)
    {
        setSortRole(mWantedSortRole);
    }
    
    if(sortColumn() == -1 || sortOrder() != mWantedSortOrder)
    {
        // if sorting column is not yet setted, it indicates
        // that we've setted the sorting order for the first time
        // and sorting has not yet been done. After we have called sort once,
        // the sort columnt gets updated and following sorts gets executed
        // by setSortRole -call as long as role changes.
    
        // Another case is when we need to change the sort order, as there is
        // no other way of setting the sort order than calling sort().
        sort(0, mWantedSortOrder);
    }
    
    // Start fetching thumbnails at start of the model.
    VideoThumbnailData::instance().startBackgroundFetching(0);
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::lessThan
// -----------------------------------------------------------------------------
//
bool VideoSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // do comparisation based on the role, if role is different than Qt::DisplayRole,
    // VideoCollectionCommon::KeySizeValue or VideoCollectionCommon::KeyDateValue, method does not sort at all

    int sRole = sortRole();
    QVariant leftData = sourceModel()->data(left, sRole);
    QVariant rightData = sourceModel()->data(right, sRole);

    if(!leftData.isValid() || !rightData.isValid())
    {
        return false;
    }
    if(sRole == Qt::DisplayRole)
    {
        QString leftString = leftData.toStringList().first().toUpper();
        QString rightString = rightData.toStringList().first().toUpper();
        return QString::localeAwareCompare(leftString, rightString) < 0;
    }
    else if(sRole == VideoCollectionCommon::KeySizeValue)
    {
        quint32 leftSize = leftData.toUInt();
        quint32 rightSize = rightData.toUInt();
        return leftSize < rightSize;
    }
    else if(sRole == VideoCollectionCommon::KeyDateTime)
    {
        QDateTime leftDateTime = leftData.toDateTime();
        QDateTime rightDateTime = rightData.toDateTime();
        // datetime sorting role has inverted sorting order compared to other roles 
        return rightDateTime < leftDateTime;
    }
    return false;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoSortFilterProxyModel::filterAcceptsRow (int source_row, const QModelIndex &source_parent) const
{
    if(!sourceModel())
        return false;

    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    if(index.isValid())
    {
        QVariant data = sourceModel()->data(index, VideoCollectionCommon::KeyStatus);
        if(data.isValid() && data.toInt() == VideoCollectionCommon::StatusDeleted)
        {
            return false;
        }
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getMediaIdAtIndex()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoSortFilterProxyModel::getMediaIdAtIndex(const QModelIndex &index)
{
    QModelIndex sourceIndex = mapToSource(index);
    TMPXItemId mpxId = TMPXItemId::InvalidId();
    if(mModel && sourceIndex.isValid())
    {
        mpxId = mModel->mediaIdAtIndex(sourceIndex.row());
    }
    return mpxId;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getMediaFilePathForId()
// -----------------------------------------------------------------------------
//
QString VideoSortFilterProxyModel::getMediaFilePathForId(TMPXItemId mediaId)
{
    QString filePath;
    if(mModel)
    {
        filePath = mModel->mediaFilePathForId(mediaId);
    }
    return filePath;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::addNewCollection()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::addNewCollection(QString name, QString thumbnail, QList<TMPXItemId> mediaIds)
{
    int error = -1;
    if(mCollectionClient) {
        error = mCollectionClient->addNewCollection(name, thumbnail, mediaIds);
    }
    return error;
}

// end of file

