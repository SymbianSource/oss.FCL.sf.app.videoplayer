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
VideoSortFilterProxyModel::VideoSortFilterProxyModel(int type, QObject *parent) : 
 QObject(parent),
 mModel(0),
 mType(type)
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
// end of file
