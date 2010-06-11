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
* Description: stub videocollectionviewutilsdata
* 
*/

#ifndef VIDEOCOLLECTIONVIEWUTILSDATA_H
#define VIDEOCOLLECTIONVIEWUTILSDATA_H

#include <qnamespace.h>
#include <qvariant.h>
#include "videocollectioncommon.h"

class VideoCollectionViewUtilsData
{
public: // methods
    static void reset()
    {
        mLastError = 0;
        mLoadSortingValuesFails = false;
        mGetServiceIconStringsFails = false;
        mVideoSortRole = -1;
        mCollectionSortRole = -1;
        mVideoSortOrder = Qt::AscendingOrder;
        mCollectionSortOrder = Qt::AscendingOrder;
        mIconString = "";
        mPressedString = "";
        mLastStatusAdditional = QVariant();
        mWidgetLevel = VideoCollectionCommon::ELevelInvalid;
    }   
    
public: // data
    static int mLastError;
    static bool mLoadSortingValuesFails;
    static bool mGetServiceIconStringsFails;
    static int mVideoSortRole;
    static int mCollectionSortRole;
    static Qt::SortOrder mVideoSortOrder;
    static Qt::SortOrder mCollectionSortOrder;
    static QString mIconString;
    static QString mPressedString;
    static QVariant mLastStatusAdditional;
    static VideoCollectionCommon::TCollectionLevels mWidgetLevel;
};

#endif /* VIDEOCOLLECTIONVIEWUTILSDATA_H */
