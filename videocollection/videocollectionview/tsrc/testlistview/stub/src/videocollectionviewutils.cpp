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
* Description: stub video collection view plugin's ui utils class
* 
*/

#include "videocollectionviewutils.h"
#include <QItemSelection>
#include "videocollectionwrapper.h"
#include "videosortfilterproxymodel.h"
#include "dummydatamodel.h"

int VideoCollectionViewUtils::mLastError = -6666;
bool VideoCollectionViewUtils::mLoadSortingValuesFails = false;
int VideoCollectionViewUtils::mSortRole = -1;
Qt::SortOrder VideoCollectionViewUtils::mSortOrder = Qt::AscendingOrder;

// ---------------------------------------------------------------------------
// instance
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils& VideoCollectionViewUtils::instance()
{
     static VideoCollectionViewUtils _popupInstance;
     return _popupInstance;
}

// ---------------------------------------------------------------------------
// VideoCollectionViewUtils
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils::VideoCollectionViewUtils()
{
}

// ---------------------------------------------------------------------------
// ~VideoCollectionViewUtils
// ---------------------------------------------------------------------------
//
VideoCollectionViewUtils::~VideoCollectionViewUtils()
{
}

// ---------------------------------------------------------------------------
// showStatusMsgSlot
// ---------------------------------------------------------------------------
//
void VideoCollectionViewUtils::showStatusMsgSlot(int statusCode, QVariant& /* additional */)
{
	mLastError = statusCode;
}

// ---------------------------------------------------------------------------
// showErrorMsgSlot
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::saveSortingValues(int role, Qt::SortOrder order)
{
    mSortRole = role;
    mSortOrder = order;
    return 0;
}

// ---------------------------------------------------------------------------
// showErrorMsgSlot
// ---------------------------------------------------------------------------
//
int VideoCollectionViewUtils::loadSortingValues(int& role, Qt::SortOrder& order)
{
    if(mLoadSortingValuesFails) {
        return -1;
    }
    
    role = mSortRole;
    order = mSortOrder;
}
