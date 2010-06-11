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
int VideoCollectionViewUtilsData::mVideoSortRole = -1;
int VideoCollectionViewUtilsData::mCollectionSortRole = -1;
Qt::SortOrder VideoCollectionViewUtilsData::mVideoSortOrder = Qt::AscendingOrder;
Qt::SortOrder VideoCollectionViewUtilsData::mCollectionSortOrder = Qt::AscendingOrder;
QString VideoCollectionViewUtilsData::mIconString = "";
QString VideoCollectionViewUtilsData::mPressedString = "";
QVariant VideoCollectionViewUtilsData::mLastStatusAdditional = QVariant();
VideoCollectionCommon::TCollectionLevels VideoCollectionViewUtilsData::mWidgetLevel = VideoCollectionCommon::ELevelInvalid;

VideoCollectionViewUtils& VideoCollectionViewUtils::instance()
{
     static VideoCollectionViewUtils _popupInstance;
     return _popupInstance;
}

VideoCollectionViewUtils::VideoCollectionViewUtils():
mVideosSortRole(-1),
mCollectionsSortRole(-1),
mVideosSortOrder(Qt::AscendingOrder),
mCollectionsSortOrder(Qt::AscendingOrder)
{
    // not stubbed
}

VideoCollectionViewUtils::~VideoCollectionViewUtils()
{
    VideoCollectionViewUtilsData::reset();
}

int VideoCollectionViewUtils::saveSortingValues(int role, Qt::SortOrder order, VideoCollectionCommon::TCollectionLevels target)
{
    int status(0);
    if(target == VideoCollectionCommon::ELevelCategory )
    {
        VideoCollectionViewUtilsData::mCollectionSortRole = role;
        mCollectionsSortRole = role;
        VideoCollectionViewUtilsData::mCollectionSortOrder = order;
        mCollectionsSortOrder = order;
    }
    else if(target > VideoCollectionCommon::ELevelCategory && 
            target <= VideoCollectionCommon::ELevelAlbum)
    {
        VideoCollectionViewUtilsData::mVideoSortRole = role;
        mVideosSortRole = role;
        VideoCollectionViewUtilsData::mVideoSortOrder = order;
        mVideosSortOrder = order;
    }
    else
    {
        status = -1;
    }
    return status;
}

int VideoCollectionViewUtils::loadSortingValues(int &role, Qt::SortOrder &order,  VideoCollectionCommon::TCollectionLevels target)
{
    if (VideoCollectionViewUtilsData::mLoadSortingValuesFails)
    {
        return -1;
    }
    if(target == VideoCollectionCommon::ELevelCategory )
    {
        role = VideoCollectionViewUtilsData::mCollectionSortRole;
        order = VideoCollectionViewUtilsData::mCollectionSortOrder;
    }
    else if(target > VideoCollectionCommon::ELevelCategory && 
            target <= VideoCollectionCommon::ELevelAlbum)
    {
        role = VideoCollectionViewUtilsData::mVideoSortRole;
        order = VideoCollectionViewUtilsData::mVideoSortOrder;
    }
    else
    {
        return -1;
    }
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

void VideoCollectionViewUtils::saveWidgetLevel(VideoCollectionCommon::TCollectionLevels &level)
{
    Q_UNUSED(level);
    // not stubbed
}
  
VideoCollectionCommon::TCollectionLevels VideoCollectionViewUtils::loadWidgetLevel()
{
    return VideoCollectionViewUtilsData::mWidgetLevel;
}

void VideoCollectionViewUtils::sortModel(VideoSortFilterProxyModel *model,
    bool async, VideoCollectionCommon::TCollectionLevels target)
{
    Q_UNUSED(model);
    Q_UNUSED(async);
    Q_UNUSED(target);
    // not stubbed
}

void VideoCollectionViewUtils::showStatusMsgSlot(int statusCode, QVariant &additional)
{
    VideoCollectionViewUtilsData::mLastStatusAdditional = additional;
    VideoCollectionViewUtilsData::mLastError = statusCode;
}
