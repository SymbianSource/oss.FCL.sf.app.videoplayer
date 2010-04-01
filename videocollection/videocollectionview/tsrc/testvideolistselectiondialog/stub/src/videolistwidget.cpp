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
* Description:    stub videolistwidget
* 
*/

#include "videosortfilterproxymodel.h"
#include "videolistwidget.h"

// ---------------------------------------------------------------------------
// VideoListWidget
// ---------------------------------------------------------------------------
//
VideoListWidget::VideoListWidget(VideoSortFilterProxyModel &model) : 
mModel(model),
mDummySelectionModel(0),
mStartSelModeStartedCount(0),
mEndSelModelStartedCount(0),
mParentItem(0),
mParentLayout(0),
mVisibility(false),
mSelectionMode(HbAbstractItemView::NoSelection)
{
    mDummySelectionModel = new DummySelectionModel();
    mParentItem = new DummyGraphicsItem();
    mParentLayout = new DummyGraphicsLayoutItem();
    mInitiallySelected.clear();
}
 
// ---------------------------------------------------------------------------
// ~VideoListWidget
// ---------------------------------------------------------------------------
//    
VideoListWidget::~VideoListWidget()
{
    delete mParentItem;
    delete mParentLayout;
}

// ---------------------------------------------------------------------------
// getModel
// ---------------------------------------------------------------------------
// 
VideoSortFilterProxyModel& VideoListWidget::getModel()
{
    return mModel;
}

// ---------------------------------------------------------------------------
// selectAll
// ---------------------------------------------------------------------------
//
void VideoListWidget::selectAll()
{
    mInitiallySelected.select( mModel.index(0,0), mModel.index(mModel.rowCount() - 1, 0 ));
}
  
// ---------------------------------------------------------------------------
// clearSelection
// ---------------------------------------------------------------------------
//
void VideoListWidget::clearSelection()
{
    mInitiallySelected.clear();
}

// ---------------------------------------------------------------------------
// parentLayoutItem
// ---------------------------------------------------------------------------
//
QGraphicsLayoutItem* VideoListWidget::parentLayoutItem()
{
    return mParentLayout;
}
  
// ---------------------------------------------------------------------------
// parentItem
// ---------------------------------------------------------------------------
//
QGraphicsItem* VideoListWidget::parentItem()
{
    return mParentItem;
}
  
// ---------------------------------------------------------------------------
// setParentItem
// ---------------------------------------------------------------------------
//
void VideoListWidget::setParentItem(QGraphicsItem *currentParentItem)
{
    mParentItem = currentParentItem;   
}
   
// ---------------------------------------------------------------------------
// setParentLayoutItem
// ---------------------------------------------------------------------------
//
void VideoListWidget::setParentLayoutItem(QGraphicsLayoutItem *currenParentLayout)
{
    mParentLayout = currenParentLayout;
}

// ---------------------------------------------------------------------------
// setVisible
// ---------------------------------------------------------------------------
//
void VideoListWidget::setVisible(bool how)
{
    mVisibility = how;
}

// ---------------------------------------------------------------------------
// setSelectionMode
// ---------------------------------------------------------------------------
//
void VideoListWidget::setSelectionMode(HbAbstractItemView::SelectionMode mode)
{
    if(mode == HbAbstractItemView::MultiSelection)
    {
        mStartSelModeStartedCount++;
    }
    else if(mode == HbAbstractItemView::NoSelection)
    {
        mEndSelModelStartedCount++;
    }
    mSelectionMode = mode;
}

// ---------------------------------------------------------------------------
// selectionModel
// ---------------------------------------------------------------------------
//
DummySelectionModel* VideoListWidget::selectionModel()
{
    return mDummySelectionModel;
}


