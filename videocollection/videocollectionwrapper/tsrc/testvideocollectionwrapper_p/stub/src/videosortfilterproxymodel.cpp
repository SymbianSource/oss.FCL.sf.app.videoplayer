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
* Description:   stub Videosortfilterproxymodel for VideoCollectionWrapperPrivate unit tests   
* 
*/

#include <qstringlist.h>
#include <qtimer.h>
#include <qdatetime.h>

#include "videocollectioncommon.h"
#include "videosortfilterproxymodel.h"
#include "videolistdatamodel.h"
#include "videocollectionclient.h"

bool gInitFails = false;

bool gFetchMediaIDFails = false;

bool gFailDeleteFile = false;

// -----------------------------------------------------------------------------
// setInitFailure
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::setInitFailure(bool fails)
{
    gInitFails = fails;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::VideoSortFilterProxyModel(QObject *parent) : 
 QObject(parent),
 mModel(0),
 mLevel(-1),
 mWantedSortRole(VideoCollectionCommon::KeyDateTime)
{
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::~VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::~VideoSortFilterProxyModel() 
{
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::initialize
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::initialize(VideoListDataModel *sourceModel)
{
    mModel = sourceModel;
    return gInitFails ? -1 : 0;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::open
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::open(int level)
{
    mLevel = level;
    return 0;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::doSorting
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::doSorting(int role, Qt::SortOrder order, bool async)
{
    Q_UNUSED(async);
    mWantedSortRole = role;
    mWantedSortOrder = order;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::deleteItems
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::deleteItems(const QModelIndexList &indexList)
{
    return -1;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::openItem
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::openItem(const QModelIndex &index)
{
    return -1;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::fetchItemDetails
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::fetchItemDetails(const QModelIndex &index)
{
    return -1;  
}

// end of file
