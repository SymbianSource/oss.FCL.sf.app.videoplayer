/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VideoOperatorService class implementation.
*
*/

// Version : 

// INCLUDE FILES

#include "videooperatorservice.h"
#include "videooperatorservicedata.h"
#include "videocollectionviewutils.h"
#include "videocollectioncenrepdefs.h"

QList<QString> VideoOperatorServiceData::mTitles;
QList<QString> VideoOperatorServiceData::mIcons;
QList<QString> VideoOperatorServiceData::mUris;
QList<int> VideoOperatorServiceData::mUids;
        
int VideoOperatorServiceData::mLoadCallCount = 0;
int VideoOperatorServiceData::mTitleCallCount = 0;
int VideoOperatorServiceData::mIconResourceCallCount = 0;
int VideoOperatorServiceData::mLaunchServiceCallCount = 0;
int VideoOperatorServiceData::mLaunchApplicationLCallCount = 0;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoOperatorService::VideoOperatorService(QObject *parent) : 
    QObject(parent)
{
    mTitle = "";
    mIconResource = "";
    mServiceUri = "";
    mApplicationUid = 0;
}

// ---------------------------------------------------------------------------
// load
// ---------------------------------------------------------------------------
//
bool VideoOperatorService::load(int titleKey, int iconKey, int serviceUriKey, int appUidKey)
{
    VideoOperatorServiceData::mLoadCallCount++;
    
    if(VideoOperatorServiceData::mTitles.count() > 0)
    {
        mTitle = VideoOperatorServiceData::mTitles.takeFirst();
    }
    if(VideoOperatorServiceData::mIcons.count() > 0)
    {
        mIconResource = VideoOperatorServiceData::mIcons.takeFirst();
    }
    if(VideoOperatorServiceData::mUris.count() > 0)
    {
        mServiceUri = VideoOperatorServiceData::mUris.takeFirst();
    }
    if(VideoOperatorServiceData::mUids.count() > 0)
    {
        mApplicationUid = VideoOperatorServiceData::mUids.takeFirst();
    }

    // Icon is required, either service uri or application uid is required.
    if(mIconResource.isEmpty() && (mServiceUri.isEmpty() || mApplicationUid > 0))
    {
        return false;
    }    
    return true;
}

// ---------------------------------------------------------------------------
// title
// ---------------------------------------------------------------------------
//
const QString VideoOperatorService::title() const
{
    VideoOperatorServiceData::mTitleCallCount++;
    return mTitle;
}

// ---------------------------------------------------------------------------
// iconResource
// ---------------------------------------------------------------------------
//
const QString VideoOperatorService::iconResource() const
{
    VideoOperatorServiceData::mIconResourceCallCount++;
    return mIconResource;
}

// ---------------------------------------------------------------------------
// launchService
// ---------------------------------------------------------------------------
//
void VideoOperatorService::launchService()
{
    VideoOperatorServiceData::mLaunchServiceCallCount++;
}

// ---------------------------------------------------------------------------
// launchApplicationL
// ---------------------------------------------------------------------------
//
void VideoOperatorService::launchApplicationL(const TUid uid, TInt viewId)
{
    Q_UNUSED(uid);
    Q_UNUSED(viewId);
    VideoOperatorServiceData::mLaunchApplicationLCallCount++;
}

// End of file.
