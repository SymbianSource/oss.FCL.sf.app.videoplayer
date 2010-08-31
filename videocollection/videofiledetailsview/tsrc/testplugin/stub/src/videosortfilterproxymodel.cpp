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
* Description: Dummy VideoSortFilterProxyModel class implementation
* 
*/
// INCLUDE FILES
#include "videosortfilterproxymodel.h"

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel()
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::VideoSortFilterProxyModel() :
mStartPlaybackIndex(TMPXItemId::InvalidId()),
mDeleteFileIndex(-1)
{
    reset();
}

// -----------------------------------------------------------------------------
// ~VideoSortFilterProxyModel()
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::~VideoSortFilterProxyModel()
{
    reset();
}

// -----------------------------------------------------------------------------
// lastIndex()
// -----------------------------------------------------------------------------
//
QModelIndex VideoSortFilterProxyModel::lastIndex()
{
    return mLastIndex;
}

// -----------------------------------------------------------------------------
// lastIndex()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoSortFilterProxyModel::lastId()
{
    return mLastId;
}

// -----------------------------------------------------------------------------
// dataAccessCount()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::dataAccessCount()
{
    return mDataAccessCount;
}

// -----------------------------------------------------------------------------
// reset()
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::reset()
{
    mLastIndex = QModelIndex();
    mLastId    = TMPXItemId::InvalidId();
    mDataAccessCount = 0;
    mRowCount = 0;
    mData.clear();
    mStartPlaybackIndex = TMPXItemId::InvalidId();
    mDeleteFileIndex = -1;
    mDatareturnsInvalid = false;
}

// -----------------------------------------------------------------------------
// setDataReturnInvalid()
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::setDataReturnInvalid(bool setInvalid)
{
    mDatareturnsInvalid = setInvalid;
}

// -----------------------------------------------------------------------------
// setData()
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::setData(int role, QVariant data)
{
    mData.insert(role, data);
}

// -----------------------------------------------------------------------------
// setRowCount()
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::setRowCount(int count)
{
    if ( count == mRowCount ) return;
    
    if ( count > mRowCount ) {
        beginInsertRows(QModelIndex(), mRowCount, count);
        mRowCount = count;
        endInsertRows();
    } else {
        beginRemoveRows(QModelIndex(), count, mRowCount);
        mRowCount = count;
        endRemoveRows();
    }
}

// -----------------------------------------------------------------------------
// rowCount()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::rowCount(const QModelIndex &parent ) const
{
    // according to Qt documentation if parent is valid this should return 0 if
    // implementing a table based implementation like this.
    if (parent.isValid())
    {
        return 0;
    }
    
    return mRowCount;
}

// -----------------------------------------------------------------------------
// itemData()
// -----------------------------------------------------------------------------
//
QMap<int, QVariant> VideoSortFilterProxyModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> itemData;
    if (index.isValid()) 
    {
        // returns only basic data of the item
        itemData.insert(Qt::DisplayRole, data(index, Qt::DisplayRole));
        itemData.insert(Qt::DecorationRole, data(index, Qt::DecorationRole)); 
        itemData.insert(Qt::BackgroundRole, data(index, Qt::BackgroundRole)); 
    }
    return itemData;

}
// -----------------------------------------------------------------------------
// data()
// -----------------------------------------------------------------------------
//
QVariant VideoSortFilterProxyModel::data(const QModelIndex & index, int role) const
{
    QVariant returnValue = QVariant();
    mLastIndex = index;
    mDataAccessCount++;
    if (index.isValid() && !mDatareturnsInvalid) 
    {
        returnValue = mData.value(role);
    }
    
    return returnValue;
}

// -----------------------------------------------------------------------------
// columnCount()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::columnCount(const QModelIndex & parent) const
{
    // according to Qt documentation if parent is valid this should return 0 if
    // implementing a table based implementation like this.
    if (parent.isValid())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// -----------------------------------------------------------------------------
// index()
// -----------------------------------------------------------------------------
//
QModelIndex VideoSortFilterProxyModel::index(int row, int column, const QModelIndex & /*parent*/) const
{
    return createIndex(row, column);
}

// -----------------------------------------------------------------------------
// parent()
// -----------------------------------------------------------------------------
//
QModelIndex VideoSortFilterProxyModel::parent(const QModelIndex & /*index*/) const
{
    return QModelIndex();
}

// -----------------------------------------------------------------------------
// openItem()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::openItem(const TMPXItemId &index)
{
    mStartPlaybackIndex = index;
    return 0;
}
    
// -----------------------------------------------------------------------------
// deleteItems()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::deleteItems(const QModelIndexList &indexList)
{
    if(indexList.count() > 0)
    {
        mDeleteFileIndex = indexList.at(0).row();
        return 0;
    }
    else
    {
        mDeleteFileIndex = -1;
        return -1;
    }
    
}

// -----------------------------------------------------------------------------
// startPlaybackIndex()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoSortFilterProxyModel::startPlaybackIndex()
{
    return mStartPlaybackIndex;
}     

// -----------------------------------------------------------------------------
// deleteFileIndex()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::deleteFileIndex()
{
    return mDeleteFileIndex;
}

// -----------------------------------------------------------------------------
// sourceModel()
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel* VideoSortFilterProxyModel::sourceModel()
{
    return this;
}

// -----------------------------------------------------------------------------
// getMediaIdAtIndex()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoSortFilterProxyModel::getMediaIdAtIndex(const QModelIndex &index)
{
	TMPXItemId id = TMPXItemId::InvalidId();
	id.iId1 = mLastIndex.row();
	id.iId2 = 0;
	return id;
}

// -----------------------------------------------------------------------------
//  VideoSortFilterProxyModel::indexOfId()
// -----------------------------------------------------------------------------
//
QModelIndex VideoSortFilterProxyModel::indexOfId(TMPXItemId id)
{    
	mLastId = id;
	mLastIndex = createIndex(id.iId1, 0);
	return mLastIndex; //rikki! createIndex(row, column)
}


// End of file
