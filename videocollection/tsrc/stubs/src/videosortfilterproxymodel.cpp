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

using namespace VideoCollectionCommon;

int VideoSortFilterProxyModelData::mSortRole = -1;
Qt::SortOrder VideoSortFilterProxyModelData::mSortOrder = Qt::AscendingOrder;
bool VideoSortFilterProxyModelData::mSortAsync = false;
bool VideoSortFilterProxyModelData::mDeleteItemsFails = false;
bool VideoSortFilterProxyModelData::mRemoveAlbumsFails = false;
int VideoSortFilterProxyModelData::mDoSortingCallCount = 0;
bool VideoSortFilterProxyModelData::mOpenFails = false;
QList<TMPXItemId> VideoSortFilterProxyModelData::mItemIds = QList<TMPXItemId>();
QModelIndex VideoSortFilterProxyModelData::mLastIndex = QModelIndex();
TMPXItemId VideoSortFilterProxyModelData::mLastItemId = TMPXItemId::InvalidId();
TMPXItemId VideoSortFilterProxyModelData::mOpenedItemId = TMPXItemId::InvalidId();
int VideoSortFilterProxyModelData::mDetailsReturnValue = 0;
int VideoSortFilterProxyModelData::mAddItemsInAlbumReturnValue = 0;
int VideoSortFilterProxyModelData::mRemoveItemsFromAlbumReturnValue = 0;
TMPXItemId VideoSortFilterProxyModelData::mGenericFilterId  = TMPXItemId::InvalidId();
bool VideoSortFilterProxyModelData::mGenericFilterValue = false;
TMPXItemId VideoSortFilterProxyModelData::mNewAlbumId = TMPXItemId::InvalidId();
QString VideoSortFilterProxyModelData::mLastAlbumNameInRename;
int VideoSortFilterProxyModelData::mRenameAlbumReturnValue = 0;
QString VideoSortFilterProxyModelData::mLastAddedAlbumName = "";

VideoSortFilterProxyModel::VideoSortFilterProxyModel(VideoCollectionCommon::TModelType type, QObject *parent):
    QSortFilterProxyModel(parent),
    mModel(0),
    mCollectionClient(0),
    mType(type),
    mLevel(VideoCollectionCommon::ELevelInvalid),
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

int VideoSortFilterProxyModel::open(VideoCollectionCommon::TCollectionLevels level)
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
    setSortRole(sortingRole);
    sort(0, order);
}

void VideoSortFilterProxyModel::getSorting(int &sortingRole,
    Qt::SortOrder &order)
{
    sortingRole = VideoSortFilterProxyModelData::mSortRole;
    order = VideoSortFilterProxyModelData::mSortOrder;
}

int VideoSortFilterProxyModel::deleteItems(const QModelIndexList &indexList)
{
    
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

void VideoSortFilterProxyModel::itemModifiedSlot(const TMPXItemId &itemId)
{
    Q_UNUSED(itemId);
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

QModelIndex VideoSortFilterProxyModel::indexOfId(TMPXItemId id)
{
    //VideoListDataModel *sourceModel = qobject_cast<VideoListDataModel*>(sourceModel());

    for(int i = 0; i < VideoSortFilterProxyModelData::mItemIds.count(); i++)
    {
        if(VideoSortFilterProxyModelData::mItemIds.at(i) == id)
        {
            //return sourceModel->index(i, 0);
            return index(i, 0);
        }
    }
    return QModelIndex();
}

QString VideoSortFilterProxyModel::getMediaFilePathForId(TMPXItemId mediaId)
{
    Q_UNUSED(mediaId);
    // not stubbed
    return QString();
}

TModelType VideoSortFilterProxyModel::getType()
{
    return mType;
}

TMPXItemId VideoSortFilterProxyModel::addNewAlbum(const QString &title)
{
    VideoSortFilterProxyModelData::mLastAddedAlbumName = title;
    return VideoSortFilterProxyModelData::mNewAlbumId;
}

QString VideoSortFilterProxyModel::resolveAlbumName(
    const QString& albumName) const
{
    QString name = albumName;
    return name;
}

int VideoSortFilterProxyModel::addItemsInAlbum(TMPXItemId &albumId, const QList<TMPXItemId> &mediaIds)
{
    if(VideoSortFilterProxyModelData::mAddItemsInAlbumReturnValue >= 0)
    {
        VideoSortFilterProxyModelData::mLastItemId = albumId;
        VideoSortFilterProxyModelData::mItemIds.clear();
        VideoSortFilterProxyModelData::mItemIds = mediaIds;
    }
    // not stubbed
    return VideoSortFilterProxyModelData::mAddItemsInAlbumReturnValue;
}

int VideoSortFilterProxyModel::removeAlbums(const QModelIndexList &indexList)
{
    
    int err = 0;
    if (VideoSortFilterProxyModelData::mRemoveAlbumsFails)
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

int VideoSortFilterProxyModel::removeItemsFromAlbum(class TMPXItemId &album, QList<class TMPXItemId> const &mediaIds)
{
    if(VideoSortFilterProxyModelData::mRemoveItemsFromAlbumReturnValue >= 0)
    {
        VideoSortFilterProxyModelData::mLastItemId = album;
        VideoSortFilterProxyModelData::mItemIds.clear();
        VideoSortFilterProxyModelData::mItemIds = mediaIds;
    }
    return VideoSortFilterProxyModelData::mRemoveItemsFromAlbumReturnValue;
}

void VideoSortFilterProxyModel::setAlbumInUse(TMPXItemId albumId)
{
    VideoSortFilterProxyModelData::mLastItemId = albumId;
}

int VideoSortFilterProxyModel::renameAlbum(const TMPXItemId &albumId, const QString &newTitle)
{
    if(VideoSortFilterProxyModelData::mRemoveItemsFromAlbumReturnValue >= 0)
    {
        VideoSortFilterProxyModelData::mLastItemId = albumId;
        VideoSortFilterProxyModelData::mLastAlbumNameInRename = newTitle;

    }
    return VideoSortFilterProxyModelData::mRenameAlbumReturnValue;
}

TMPXItemId VideoSortFilterProxyModel::getOpenItem() const
{
    return VideoSortFilterProxyModelData::mOpenedItemId;
}

void VideoSortFilterProxyModel::setGenericIdFilter(TMPXItemId itemId, bool filterValue)
{
    VideoSortFilterProxyModelData::mGenericFilterId  = itemId;
    VideoSortFilterProxyModelData::mGenericFilterValue = filterValue;
    return;
}

// end of file
