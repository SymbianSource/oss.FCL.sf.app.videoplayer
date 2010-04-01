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

int VideoSortFilterProxyModel::mRowCountCallCount = 0;
bool VideoSortFilterProxyModel::mReturnInvalidIndexes = false;

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::VideoSortFilterProxyModel(QObject *parent) :
 QSortFilterProxyModel(parent)
{
    reset();
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
// VideoSortFilterProxyModel::lessThan
// -----------------------------------------------------------------------------
//
bool VideoSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return false;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getMediaIdAtIndex
// -----------------------------------------------------------------------------
//
TMPXItemId VideoSortFilterProxyModel::getMediaIdAtIndex(QModelIndex index)
{
	return TMPXItemId(index.row(), 0);
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getMediaFilePathForId()
// -----------------------------------------------------------------------------
//
QString VideoSortFilterProxyModel::getMediaFilePathForId(TMPXItemId mediaId)
{
    if(mediaId.iId1 >= 0 && mediaId.iId1 < mFileNames.count())
    {
        return mFileNames.at(mediaId.iId1);
    }
    return "";
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::back
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::back()
{
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::reset
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::reset()
{

}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::appendData
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::appendData(QString fileName)
{
    mFileNames.append(fileName);
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::rowCount
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::rowCount(const QModelIndex &parent) const
{
    VideoSortFilterProxyModel::mRowCountCallCount++;
    return mFileNames.count();
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::index
// -----------------------------------------------------------------------------
//
QModelIndex VideoSortFilterProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if(mReturnInvalidIndexes)
        return QModelIndex();
    else
        return QModelIndex(createIndex(row, column));
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::mapToSource
// -----------------------------------------------------------------------------
//
QModelIndex VideoSortFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return QModelIndex(proxyIndex);
}

// end of file

