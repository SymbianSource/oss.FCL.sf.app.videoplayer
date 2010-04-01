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
* Description: video collection view plugin's ui utils class
* 
*/

#include "videocollectionviewutils.h"
#include "videocollectionviewutilsdata.h"

int VideoCollectionViewUtilsData::mLastError = 0;
bool VideoCollectionViewUtilsData::mLoadSortingValuesFails = false;
bool VideoCollectionViewUtilsData::mGetServiceIconStringsFails = false;
int VideoCollectionViewUtilsData::mSortRole = 0;
Qt::SortOrder VideoCollectionViewUtilsData::mSortOrder = Qt::AscendingOrder;
QString VideoCollectionViewUtilsData::mIconString = "";
QString VideoCollectionViewUtilsData::mPressedString = "";

VideoCollectionViewUtils& VideoCollectionViewUtils::instance()
{
     static VideoCollectionViewUtils _popupInstance;
     return _popupInstance;
}

VideoCollectionViewUtils::VideoCollectionViewUtils():
    mSortRole(-1),
    mSortOrder(Qt::AscendingOrder)
{
    // not stubbed
}

VideoCollectionViewUtils::~VideoCollectionViewUtils()
{
    VideoCollectionViewUtilsData::reset();
}

int VideoCollectionViewUtils::saveSortingValues(int role, Qt::SortOrder order)
{
    VideoCollectionViewUtilsData::mSortRole = role;
    VideoCollectionViewUtilsData::mSortOrder = order;
    return 0;
}

int VideoCollectionViewUtils::loadSortingValues(int &role, Qt::SortOrder &order)
{
    if (VideoCollectionViewUtilsData::mLoadSortingValuesFails)
    {
        return -1;
    }
    
    role = VideoCollectionViewUtilsData::mSortRole;
    order = VideoCollectionViewUtilsData::mSortOrder;
    
    return 0;
}

int VideoCollectionViewUtils::getServiceIconStrings(QString& icon, 
    QString& iconPressed)
{
    if (VideoCollectionViewUtilsData::mGetServiceIconStringsFails)
    {
        return -1;
    }
    
    icon = VideoCollectionViewUtilsData::mIconString;
    iconPressed = VideoCollectionViewUtilsData::mPressedString;
    
    return 0;
}

QString VideoCollectionViewUtils::getServiceUriString()
{
    // not stubbed
    return QString();
}

void VideoCollectionViewUtils::initListView(HbListView *view)
{
    Q_UNUSED(view);
    // not stubbed
}

void VideoCollectionViewUtils::sortModel(VideoSortFilterProxyModel *model,
    bool async)
{
    Q_UNUSED(model);
    Q_UNUSED(async);
    // not stubbed
}

void VideoCollectionViewUtils::showStatusMsgSlot(int statusCode, QVariant &additional)
{
    Q_UNUSED(additional);
    VideoCollectionViewUtilsData::mLastError = statusCode;
}
