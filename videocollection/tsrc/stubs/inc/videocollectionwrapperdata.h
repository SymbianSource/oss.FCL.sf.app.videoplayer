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
* Description: stub videocollectionwrapperdata
* 
*/

#ifndef VIDEOCOLLECTIONWRAPPERDATA_H
#define VIDEOCOLLECTIONWRAPPERDATA_H

#include "videosortfilterproxymodel.h"
#include "videolistdatamodel.h"

class VideoCollectionWrapperData
{
public: // methods
    static void reset()
    {
        mGetModelFails = false;
        
        delete mAllVideosModel;
        mAllVideosModel = 0;
        
        delete mCollectionsModel;
        mCollectionsModel = 0;
        
        delete mCollectionContentModel;
        mCollectionContentModel = 0;
        
        delete mGenericModel;
        mGenericModel = 0;
        
        delete mSourceModel;
        mSourceModel = 0;        
    }
    
public: // data
    static bool mGetModelFails;
    static VideoListDataModel *mSourceModel;
    static VideoSortFilterProxyModel *mAllVideosModel;
    static VideoSortFilterProxyModel *mCollectionsModel;
    static VideoSortFilterProxyModel *mCollectionContentModel;
    static VideoSortFilterProxyModel *mGenericModel;    
};

#endif /* VIDEOSORTFILTERPROXYMODEL_H */
