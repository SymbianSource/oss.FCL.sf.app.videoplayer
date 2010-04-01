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
* Description: stub hblistview
* 
*/

#include "videosortfilterproxymodel.h"
#include "hblistview.h"

QList<HbAbstractViewItem*> HbListView::mVisibleItems = QList<HbAbstractViewItem*>();
HbAbstractItemView::SelectionMode HbListView::mSelectionMode = HbAbstractItemView::InvalidSelection;
VideoSortFilterProxyModel* HbListView::mLatestModel = 0;
bool HbListView::mReturnNullPrototype = false; 
bool HbListView::mVScrollBarIsNull = false;
bool HbListView::mLatestrecycling = false;
QPointF HbListView::mLongPressedPoint = QPointF();
HbScrollArea::ClampingStyle HbListView::mLatestClamping = HbScrollArea::InvalidClamping;
HbScrollArea::ScrollingStyle HbListView::mLatestScrolling = HbScrollArea::InvalidScrolling;
bool HbListView::mLatestFrictionEnabled = false;
bool HbListView::mLatestUniformItemSizes = false;
bool HbListView::mLatestVisibility = false;
bool HbListView::mLatestEnableValue = false;
QModelIndex HbListView::mCurrentIndex = QModelIndex();




