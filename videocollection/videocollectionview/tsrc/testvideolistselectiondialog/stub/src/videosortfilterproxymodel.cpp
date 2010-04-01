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
* Description: dummy videosortfilterproxymodel implementation for 
*              VideoListSelectionDialog unit tests
* 
*/

#include <qstringlist.h>
#include <qdatetime.h>

#include "videocollectioncommon.h"
#include "videosortfilterproxymodel.h"


// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::VideoSortFilterProxyModel(QObject *parent) : 
 QSortFilterProxyModel(parent)
{
    reset();
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::~VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::~VideoSortFilterProxyModel() 
{
    disconnect();
}

// end of file

   











