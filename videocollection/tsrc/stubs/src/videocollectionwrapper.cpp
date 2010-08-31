/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   VideoCollectionWrapper class implementation
* 
*/

#include "videocollectionwrapper.h"
#include "videocollectionwrapperdata.h"

bool VideoCollectionWrapperData::mGetModelFails = false;
VideoListDataModel *VideoCollectionWrapperData::mSourceModel = 0;
VideoSortFilterProxyModel *VideoCollectionWrapperData::mAllVideosModel = 0;
VideoSortFilterProxyModel *VideoCollectionWrapperData::mCollectionsModel = 0;
VideoSortFilterProxyModel *VideoCollectionWrapperData::mCollectionContentModel = 0;
VideoSortFilterProxyModel *VideoCollectionWrapperData::mGenericModel = 0;

VideoCollectionWrapper &VideoCollectionWrapper::instance()
{
    static VideoCollectionWrapper _staticWrapper;
    return _staticWrapper;
}

VideoCollectionWrapper::VideoCollectionWrapper() 
{
    // nop
}

VideoCollectionWrapper::~VideoCollectionWrapper()
{
    VideoCollectionWrapperData::reset();
}

VideoSortFilterProxyModel* VideoCollectionWrapper::getModel(VideoCollectionCommon::TModelType type)
{
    VideoSortFilterProxyModel *model = 0;
    if (!VideoCollectionWrapperData::mGetModelFails)
    {
        VideoListDataModel *sourceModel = VideoCollectionWrapperData::mSourceModel;
        if (!sourceModel)
        {
            sourceModel = new VideoListDataModel;
            if (sourceModel)
            {
                sourceModel->initialize();
                VideoCollectionWrapperData::mSourceModel = sourceModel;
            }
        }
        
        switch (type)
        {
            case VideoCollectionCommon::EModelTypeAllVideos:
            {
                model = VideoCollectionWrapperData::mAllVideosModel;
                if (!model)
                {
                    model = new VideoSortFilterProxyModel(type);
                    if (model)
                    {
                        model->initialize(sourceModel);
                        VideoCollectionWrapperData::mAllVideosModel = model;
                    }
                }
                break;
            }
            case VideoCollectionCommon::EModelTypeCollections:
            {
                model = VideoCollectionWrapperData::mCollectionsModel;
                if (!model)
                {
                    model = new VideoSortFilterProxyModel(type);
                    if (model)
                    {
                        model->initialize(sourceModel);
                        VideoCollectionWrapperData::mCollectionsModel = model;
                    }
                }
                break;
            }
            case VideoCollectionCommon::EModelTypeCollectionContent:
            {
                model = VideoCollectionWrapperData::mCollectionContentModel;
                if (!model)
                {
                    model = new VideoSortFilterProxyModel(type);
                    if (model)
                    {
                        model->initialize(sourceModel);
                        VideoCollectionWrapperData::mCollectionContentModel = model;
                    }
                }
                break;
            }
            case VideoCollectionCommon::EModelTypeGeneric:
            {
                model = VideoCollectionWrapperData::mGenericModel;
                if (!model)
                {
                    model = new VideoSortFilterProxyModel(type);
                    if (model)
                    {
                        model->initialize(sourceModel);
                        VideoCollectionWrapperData::mGenericModel = model;
                    }
                }
                break;
            }
            default:
            {
                // invalid model type
                break;
            }
        }
    }
    
    return model;
}

void VideoCollectionWrapper::sendAsyncStatus(int statusCode,
    QVariant &additional)
{
    Q_UNUSED(statusCode);
    Q_UNUSED(additional);
    // not stubbed
}

// end of file
