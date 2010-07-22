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
* Description: VideoCollectionWrapperPrivate class implementation
* 
*/

// Version : %version: 27 %

// INCLUDE FILES
#include <qapplication.h>
#include <qabstractitemmodel.h>
#include <vcxmyvideosdefs.h>

#include "videocollectionwrapper.h"
#include "videocollectionwrapper_p.h"
#include "videolistdatamodel.h"
#include "videosortfilterproxymodel.h"
#include "videocollectionclient.h"
#include "videocollectioncommon.h"
#include "videocollectiontrace.h"

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::VideoCollectionWrapperPrivate()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapperPrivate::VideoCollectionWrapperPrivate() 
    : mAllVideosModel( 0 )
    , mCollectionsModel( 0 )
    , mCollectionContentModel( 0 )
    , mGenericModel( 0 )
    , mSourceModel( 0 )
    , mAboutToClose( false )
{
	FUNC_LOG;
    // NOP 
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::~VideoCollectionWrapperPrivate()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapperPrivate::~VideoCollectionWrapperPrivate()
{
	FUNC_LOG;
}


// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::getModel()
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel* VideoCollectionWrapperPrivate::getModel(VideoCollectionCommon::TModelType &type)
{
	FUNC_LOG;
    INFO_1("VideoCollectionWrapperPrivate::getModel() type: %d", type);
	
    if(mAboutToClose)
    {
        return 0;
    }
    
    VideoSortFilterProxyModel *model = 0;
    if(!mSourceModel)
    {
        mSourceModel = new VideoListDataModel();
        if(!mSourceModel || mSourceModel->initialize() < 0 || 
            !connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuitSlot())) )
        {
            ERROR(-1, "VideoCollectionWrapperPrivate::getModel() sourceModel setup failed.");
            return 0;
        }
    }
    bool needsInitialization = false;
    if(type == VideoCollectionCommon::EModelTypeAllVideos)
    {
        if(!mAllVideosModel)
        {
            mAllVideosModel = new VideoSortFilterProxyModel(type);
            needsInitialization = true;
        }
        model = mAllVideosModel;
    }
    else if(type == VideoCollectionCommon::EModelTypeCollections)
    {
        if(!mCollectionsModel)
        {
            mCollectionsModel = new VideoSortFilterProxyModel(type);
            needsInitialization = true;
        }
        model = mCollectionsModel;
    }
    else if(type == VideoCollectionCommon::EModelTypeCollectionContent)
    {
        if(!mCollectionContentModel)
        {
            mCollectionContentModel = new VideoSortFilterProxyModel(type);
			needsInitialization = true;   
        }
        model = mCollectionContentModel;
    }
    else if(type == VideoCollectionCommon::EModelTypeGeneric)
    {
        if(!mGenericModel)
        {
            mGenericModel = new VideoSortFilterProxyModel(type);
            needsInitialization = true;
        }
        model = mGenericModel;
    }
    
    if(needsInitialization)
    {        
        if(model && model->initialize(mSourceModel) < 0)
        {
            ERROR(-1, "VideoCollectionWrapperPrivate::getModel() no model or init failed.");
            delete model;
            return 0;
        }
        if(!connect(model, SIGNAL(shortDetailsReady(TMPXItemId)), mSourceModel, SIGNAL(shortDetailsReady(TMPXItemId))))
        {
        	delete model;
        	return 0;
        }
    }   
    return model;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::aboutToQuitSlot()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapperPrivate::aboutToQuitSlot()
{    
	FUNC_LOG;
    if(!mSourceModel.isNull())
    {
        delete mSourceModel;
        delete mAllVideosModel;
        delete mCollectionsModel;
        delete mCollectionContentModel;
        delete mGenericModel;
        mAboutToClose = true;
    }
}

// End of file
