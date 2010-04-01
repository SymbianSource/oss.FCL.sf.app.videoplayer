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

class VideoCollectionViewUtilsData
{
public: // methods
    static void reset()
    {
        mLastError = 0;
        mLoadSortingValuesFails = false;
        mGetServiceIconStringsFails = false;
        mSortRole = 0;
        mSortOrder = Qt::AscendingOrder;
        mIconString = "";
        mPressedString = "";
    }   
    
public: // data
    static int mLastError;
    static bool mLoadSortingValuesFails;
    static bool mGetServiceIconStringsFails;
    static int mSortRole;
    static Qt::SortOrder mSortOrder;
    static QString mIconString;
    static QString mPressedString;
};

#endif /* VIDEOCOLLECTIONVIEWUTILSDATA_H */
