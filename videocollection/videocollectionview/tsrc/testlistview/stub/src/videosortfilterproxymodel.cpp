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
#include <qdatetime.h>

#include "videocollectioncommon.h"
#include "videosortfilterproxymodel.h"

int VideoSortFilterProxyModel::mSortRole = -1;
Qt::SortOrder VideoSortFilterProxyModel::mSortOrder = Qt::AscendingOrder;
bool VideoSortFilterProxyModel::mSortAsync = false;
bool VideoSortFilterProxyModel::mDeleteItemsFails = false;
int VideoSortFilterProxyModel::mDoSortingCallCount = 0;
bool VideoSortFilterProxyModel::mOpenFails = false;

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
    reset();
    disconnect();
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::doSorting
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::open(int level)
{
    mLevel = level;
	if(VideoSortFilterProxyModel::mOpenFails) 
		return -1;
	else
		return 0;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::doSorting
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::doSorting(int role, Qt::SortOrder order, bool async)
{
	mDoSortingCallCount++;
    mSortRole = role;
    mSortOrder = order;
    mSortAsync = async;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::deleteItems
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::deleteItems(const QModelIndexList &indexList)
{
	if( mDeleteItemsFails ) 
		return -1;
	else
		return 0;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::sortRole
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::sortRole() const
{
    return mSortRole;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::sortOrder
// -----------------------------------------------------------------------------
//
Qt::SortOrder VideoSortFilterProxyModel::sortOrder() const
{
    return mSortOrder;
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
    return false;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoSortFilterProxyModel::filterAcceptsRow (int source_row, const QModelIndex &source_parent) const
{
	return true;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::reset
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::reset()
{
	VideoSortFilterProxyModel::mSortRole = -1;
	VideoSortFilterProxyModel::mSortOrder = Qt::AscendingOrder;
	VideoSortFilterProxyModel::mDeleteItemsFails = false;
	VideoSortFilterProxyModel::mDoSortingCallCount = 0;
	VideoSortFilterProxyModel::mRowCount = 0;
    mData.clear();
}

// end of file

   
