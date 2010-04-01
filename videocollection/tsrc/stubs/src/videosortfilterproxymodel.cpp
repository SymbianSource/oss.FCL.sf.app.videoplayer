/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "videosortfilterproxymodel.h"
#include "videosortfilterproxymodeldata.h"
#include "videocollectioncommon.h"
#include "videolistdatamodel.h"

int VideoSortFilterProxyModelData::mSortRole = -1;
Qt::SortOrder VideoSortFilterProxyModelData::mSortOrder = Qt::AscendingOrder;
bool VideoSortFilterProxyModelData::mSortAsync = false;
bool VideoSortFilterProxyModelData::mDeleteItemsFails = false;
int VideoSortFilterProxyModelData::mDoSortingCallCount = 0;
bool VideoSortFilterProxyModelData::mOpenFails = false;
QList<TMPXItemId> VideoSortFilterProxyModelData::mItemIds = QList<TMPXItemId>();
QModelIndex VideoSortFilterProxyModelData::mLastIndex = QModelIndex();
TMPXItemId VideoSortFilterProxyModelData::mLastItemId = TMPXItemId::InvalidId();
int VideoSortFilterProxyModelData::mDetailsReturnValue = 0;


VideoSortFilterProxyModel::VideoSortFilterProxyModel(int type, QObject *parent):
    QSortFilterProxyModel(parent),
    mModel(0),
    mCollectionClient(0),
    mType(type),
    mLevel(-1),
    mIdleSortTimer(0),
    mWantedSortRole(VideoCollectionCommon::KeyDateTime)
{
    // NOP
}

VideoSortFilterProxyModel::~VideoSortFilterProxyModel()
{
    VideoSortFilterProxyModelData::reset();
}

int VideoSortFilterProxyModel::initialize(VideoListDataModel *sourceModel)
{
    setSourceModel(sourceModel);
    
    return 0;
}

bool VideoSortFilterProxyModel::connectSignals()
{
    // not stubbed
    return true;
}
   
void VideoSortFilterProxyModel::disconnectSignals()
{
    // not stubbed
}

int VideoSortFilterProxyModel::open(int level)
{
    int err = 0;
    
    mLevel = level;
    if (VideoSortFilterProxyModelData::mOpenFails)
    {
        err = -1;
    }
    
    return err;
}

void VideoSortFilterProxyModel::doSorting(int sortingRole,
    Qt::SortOrder order, 
    bool async)
{
    VideoSortFilterProxyModelData::mDoSortingCallCount++;
    VideoSortFilterProxyModelData::mSortRole = sortingRole;
    VideoSortFilterProxyModelData::mSortOrder = order;
    VideoSortFilterProxyModelData::mSortAsync = async;
}

void VideoSortFilterProxyModel::getSorting(int &sortingRole,
    Qt::SortOrder &order)
{
    sortingRole = VideoSortFilterProxyModelData::mSortRole;
    order = VideoSortFilterProxyModelData::mSortOrder;
}

int VideoSortFilterProxyModel::deleteItems(const QModelIndexList &indexList)
{
    Q_UNUSED(indexList);
    
    int err = 0;
    if (VideoSortFilterProxyModelData::mDeleteItemsFails)
    {
        err = -1;
    }
    else
    {
        if (indexList.count() > 0)
        {
            VideoSortFilterProxyModelData::mLastIndex = indexList.at(0);
        }
    }
    
    return err;
}

int VideoSortFilterProxyModel::openItem(TMPXItemId mediaId)
{
    VideoSortFilterProxyModelData::mLastItemId = mediaId;
    
    return 0;
}

int VideoSortFilterProxyModel::back()
{
    // not stubbed
    return 0;
}

int VideoSortFilterProxyModel::fetchItemDetails(const QModelIndex &index)
{
    VideoSortFilterProxyModelData::mLastIndex = index;
    
    return VideoSortFilterProxyModelData::mDetailsReturnValue;  
}

void VideoSortFilterProxyModel::processSorting()
{
    // not stubbed
}

void VideoSortFilterProxyModel::albumChangedSlot()
{
    // not stubbed
}

bool VideoSortFilterProxyModel::lessThan(const QModelIndex &left,
    const QModelIndex &right) const
{
    Q_UNUSED(left);
    Q_UNUSED(right);
    // not stubbed
    return true;
}

bool VideoSortFilterProxyModel::filterAcceptsRow (int source_row,
    const QModelIndex &source_parent) const
{
    Q_UNUSED(source_row);
    Q_UNUSED(source_parent);
    // not stubbed
    return true;
}

TMPXItemId VideoSortFilterProxyModel::getMediaIdAtIndex(
    const QModelIndex &index) const
{
    if (index.row() >= 0 &&
        index.row() < VideoSortFilterProxyModelData::mItemIds.count())
    {
        return VideoSortFilterProxyModelData::mItemIds[index.row()];
    }
    return TMPXItemId::InvalidId();
}

QString VideoSortFilterProxyModel::getMediaFilePathForId(TMPXItemId mediaId)
{
    Q_UNUSED(mediaId);
    // not stubbed
    return QString();
}

TMPXItemId VideoSortFilterProxyModel::addNewAlbum(const QString &title)
{
    Q_UNUSED(title);
    // not stubbed
    return TMPXItemId::InvalidId();
}

QString VideoSortFilterProxyModel::resolveAlbumName(
    const QString& albumName) const
{
    Q_UNUSED(albumName);
    // not stubbed
    return QString();
}

int VideoSortFilterProxyModel::addItemsInAlbum(TMPXItemId albumId, const QList<TMPXItemId> &mediaIds)
{
    Q_UNUSED(albumId);
    Q_UNUSED(mediaIds);
    // not stubbed
    return 0;
}

TMPXItemId VideoSortFilterProxyModel::getOpenItem() const
{
    // not stubbed
    return TMPXItemId();
}

// end of file
