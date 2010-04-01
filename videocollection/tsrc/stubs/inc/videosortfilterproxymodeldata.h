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
* Description: stub videosortfilterproxymodeldata
* 
*/

#ifndef VIDEOSORTFILTERPROXYMODELDATA_H
#define VIDEOSORTFILTERPROXYMODELDATA_H

#include <qnamespace.h>

class VideoSortFilterProxyModelData
{
public: // methods
    static void reset()
    {
        mOpenFails = false;
        mSortRole = 0;
        mSortOrder = Qt::AscendingOrder;
        mSortAsync = false;
        mDeleteItemsFails = true;
        mDoSortingCallCount = 0;
        mItemIds.clear();
        mLastIndex = QModelIndex();
        mDetailsReturnValue = 0;
    }
    
public: // data
    static bool mOpenFails;
    static int mSortRole;
    static Qt::SortOrder mSortOrder;
    static bool mSortAsync;
    static bool mDeleteItemsFails;
    static int mDoSortingCallCount;
    static QList<TMPXItemId> mItemIds;
    static QModelIndex mLastIndex;
    static TMPXItemId mLastItemId;
    static int mDetailsReturnValue;
};

#endif /* VIDEOSORTFILTERPROXYMODEL_H */
