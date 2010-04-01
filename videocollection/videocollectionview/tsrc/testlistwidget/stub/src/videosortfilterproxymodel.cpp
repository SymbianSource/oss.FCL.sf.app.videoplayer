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
* Description:  Videolist sort filter proxy stub implementation
* 
*/

#include <qstringlist.h>
#include <qdatetime.h>


#include "videocollectioncommon.h"
#include "videosortfilterproxymodel.h"

QList<TMPXItemId> VideoSortFilterProxyModel::mItemIds = QList<TMPXItemId>();
int VideoSortFilterProxyModel::mSortOrder(-1);
QModelIndex VideoSortFilterProxyModel::mLastIndex(QModelIndex());
int VideoSortFilterProxyModel::mDetailsReturnValue(0);

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::VideoSortFilterProxyModel(QObject *parent) : 
 QSortFilterProxyModel(parent)
{
    reset();
    mItemIds.clear();
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::~VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::~VideoSortFilterProxyModel() 
{
    disconnect();
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::doSorting
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::doSorting(int sortingOrder)
{
    mSortOrder = sortingOrder;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::deleteItems
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::deleteItems(const QModelIndexList &indexList)
{
    if(indexList.count() > 0)
    {
        mLastIndex = indexList.at(0);
    }
    return 0;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::openItem
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::openItem(const QModelIndex &index)
{
    mLastIndex = index;
    return 0;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::fetchItemDetails
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::fetchItemDetails(const QModelIndex &index)
{
    mLastIndex = index;
    return mDetailsReturnValue;  
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::processSorting
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::processSorting()
{
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
        QString left = leftData.toStringList().at(0);
        QString right = rightData.toStringList().at(0);
        return QString::localeAwareCompare(left, right) < 0;
    }
    else if(sRole == VideoCollectionCommon::KeySizeValue)
    {
        quint32 left = leftData.toUInt();
        quint32 right = rightData.toUInt();
        return left < right;
    }
    else if(sRole == VideoCollectionCommon::KeyDateTime)
    {
        QDate left = leftData.toDate();
        QDate right = rightData.toDate();
        return left < right;
    }  
    return false;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getMediaIdAtIndex
// -----------------------------------------------------------------------------
//
TMPXItemId VideoSortFilterProxyModel::getMediaIdAtIndex(QModelIndex index)
{
    if(index.row() >= 0 && index.row() < mItemIds.count())
    {
        return mItemIds[index.row()];
    }
	return TMPXItemId::InvalidId();
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::back
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::back()
{
}

void VideoSortFilterProxyModel::reset()
{
    mSortOrder = -1;
}

// end of file

