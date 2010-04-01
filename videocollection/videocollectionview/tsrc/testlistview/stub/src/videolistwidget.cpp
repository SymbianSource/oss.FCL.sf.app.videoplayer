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
* Description:   Videolist content widget implementation
*
*/
#include "videolistwidget.h"
#include "videoservices.h"

#include "videosortfilterproxymodel.h"

int VideoListWidget::mInitializeCount(0);
int VideoListWidget::mInitializeReturnValue(0);
int VideoListWidget::mActivateCount(0);
int VideoListWidget::mActivateReturnValue(0);
bool VideoListWidget::mActive(false);
int VideoListWidget::mDeactivateCount(0);

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListWidget::VideoListWidget(HbView *parent) :
HbListView(parent),
mModel(0),
mCurrentLevel(VideoListWidget::ELevelVideos),
mType(VideoListWidget::EUnknown)
{
    // NOP
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListWidget::~VideoListWidget()
{
}

// ---------------------------------------------------------------------------
// initialize
// ---------------------------------------------------------------------------
//
int VideoListWidget::initialize(VideoSortFilterProxyModel &model, VideoServices* videoServices)
{
    mInitializeCount++;
    mModel = &model;
    return mInitializeReturnValue;
}

// ---------------------------------------------------------------------------
// activate
// ---------------------------------------------------------------------------
//
int VideoListWidget::activate()
{
    return activate(mCurrentLevel);
}

// ---------------------------------------------------------------------------
// activate
// ---------------------------------------------------------------------------
//
int VideoListWidget::activate(VideoListWidget::TVideoListLevel level)
{
    mActive = true;
    mActivateCount++;
    mCurrentLevel = level;
    return mActivateReturnValue;
}

// ---------------------------------------------------------------------------
// deactivate
// ---------------------------------------------------------------------------
//
void VideoListWidget::deactivate()
{
    mActive = false;
    mDeactivateCount++;
}

// -------------------------------------------------------------------------------------------------
// getLevel
// -------------------------------------------------------------------------------------------------
//
VideoListWidget::TVideoListType VideoListWidget::getType()
{
    return mType;
}

// ---------------------------------------------------------------------------
// getModel
// ---------------------------------------------------------------------------
//
VideoSortFilterProxyModel& VideoListWidget::getModel()
{ 
    return *mModel; 
}

// end of file
