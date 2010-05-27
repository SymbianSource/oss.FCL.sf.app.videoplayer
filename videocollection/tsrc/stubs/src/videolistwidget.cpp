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
#include "videolistwidgetdata.h"
#include "videocollectioncommon.h"

int VideoListWidgetData::mInitializeCount = 0;
int VideoListWidgetData::mInitializeReturnValue = 0;
int VideoListWidgetData::mActivateCount = 0;
int VideoListWidgetData::mActivateReturnValue = 0;
bool VideoListWidgetData::mActive = false;
int VideoListWidgetData::mDeactivateCount = 0;

VideoListWidget::VideoListWidget(VideoCollectionUiLoader* uiLoader, HbView *parent):
    HbListView(parent),
    mModel(0),
    mVideoServices(0),
    mCurrentLevel(VideoCollectionCommon::ELevelInvalid),
    mSignalsConnected(false),
    mIsService(false),
    mNavKeyAction(0),
    mContextMenu(0),
    mSelectionMode(HbAbstractItemView::NoSelection),
    mScrollPositionTimer(0),
    mUiLoader(uiLoader)
{
    // nop
}

VideoListWidget::~VideoListWidget()
{
    VideoListWidgetData::reset();
}

int VideoListWidget::initialize(VideoSortFilterProxyModel &model,
    VideoServices *videoServices)
{
    Q_UNUSED(videoServices);

    VideoListWidgetData::mInitializeCount++;
    mModel = &model;
    
    return VideoListWidgetData::mInitializeReturnValue;
}

int VideoListWidget::activate()
{
    return activate(mCurrentLevel);
}

int VideoListWidget::activate(VideoCollectionCommon::TCollectionLevels level)
{
    VideoListWidgetData::mActive = true;
    VideoListWidgetData::mActivateCount++;
    mCurrentLevel = level;
    
    return VideoListWidgetData::mActivateReturnValue;
}

void VideoListWidget::deactivate()
{
    VideoListWidgetData::mActive = false;
    VideoListWidgetData::mDeactivateCount++;
}

int VideoListWidget::connectSignals()
{
    // not stubbed
	return 0;
}

void VideoListWidget::disConnectSignals()
{
    // not stubbed
}

void VideoListWidget::deleteItemSlot()
{
    // not stubbed
}

void VideoListWidget::createContextMenu()
{
    // not stubbed
}

void VideoListWidget::setContextMenu()
{
    // not stubbed
}

VideoCollectionCommon::TCollectionLevels VideoListWidget::getLevel()
{
    return mCurrentLevel;
}

VideoSortFilterProxyModel& VideoListWidget::getModel()
{ 
    return *mModel;
}

void VideoListWidget::emitActivated (const QModelIndex &modelIndex)
{
    Q_UNUSED(modelIndex);
    // not stubbed
}

void VideoListWidget::setSelectionMode(int mode)
{
    Q_UNUSED(mode);
    // not stubbed
}

void VideoListWidget::longPressedSlot(HbAbstractViewItem *item, const QPointF &point)
{
    Q_UNUSED(item);
    Q_UNUSED(point);
    // not stubbed
}

void VideoListWidget::openItemSlot()
{
    // not stubbed
}

void VideoListWidget::playItemSlot()
{
    // not stubbed
}

void VideoListWidget::doDelayedsSlot()
{
    // not stubbed    
}

void VideoListWidget::openDetailsSlot()
{   
    // not stubbed
}

void VideoListWidget::renameSlot()
{
    // not stubbed
}

void VideoListWidget::addToCollectionSlot()
{
    // not stubbed
}

void VideoListWidget::removeCollectionSlot()
{
    // not stubbed
}

void VideoListWidget::removeFromCollectionSlot()
{
    // not stubbed
}

void VideoListWidget::back()
{
    // not stubbed
}

void VideoListWidget::scrollingStartedSlot()
{
    // not stubbed
}

void VideoListWidget::scrollingEndedSlot()
{
    // not stubbed
}

void VideoListWidget::scrollPositionChangedSlot(const QPointF &newPosition)
{
    Q_UNUSED(newPosition);
    // not stubbed
}

void VideoListWidget::scrollPositionTimerSlot()
{
    // not stubbed
}

void VideoListWidget::fetchThumbnailsForVisibleItems()
{
    // not stubbed
}

void VideoListWidget::setNavigationAction()
{
    // not stubbed
}

void VideoListWidget::renameDialogFinished(HbAction *action)
{
    Q_UNUSED(action);
    // not stubbed
}

void VideoListWidget::deleteItemDialogFinished(HbAction *action)
{
    Q_UNUSED(action);
    // not stubbed
}

void VideoListWidget::removeCollectionDialogFinished(HbAction *action)
{
    Q_UNUSED(action);
    // not stubbed
}

// end of file
