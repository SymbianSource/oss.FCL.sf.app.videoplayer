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

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::VideoCollectionWrapperPrivate()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapperPrivate::VideoCollectionWrapperPrivate() : 
mAllVideosModel(0),
mCollectionsModel(0),
mGenericModel(0),
mSourceModel(0)
{
    // NOP 
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::~VideoCollectionWrapperPrivate()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapperPrivate::~VideoCollectionWrapperPrivate()
{
    // NOP here
}


// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::getModel()
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel* VideoCollectionWrapperPrivate::getModel(int &type)
{
    
    VideoSortFilterProxyModel *model = 0;
    if(!mSourceModel)
    {
        mSourceModel = new VideoListDataModel();
        if(!mSourceModel || mSourceModel->initialize() < 0 || 
            !connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuitSlot())) )
        {
            return 0;
        }
    }
    bool needsInitialization = false;
    if(type == VideoCollectionWrapper::EAllVideos)
    {
        if(!mAllVideosModel)
        {
            mAllVideosModel = new VideoSortFilterProxyModel(type); 
            needsInitialization = true;
        }
        model = mAllVideosModel;
    }
    else if(type == VideoCollectionWrapper::ECollections)
    {
        if(!mCollectionsModel)
        {
            mCollectionsModel = new VideoSortFilterProxyModel(type);
            needsInitialization = true;
        }
        model = mCollectionsModel;
    }
    else if(type == VideoCollectionWrapper::ECollectionContent)
    {
        if(!mCollectionContentModel)
        {
            mCollectionContentModel = new VideoSortFilterProxyModel(type);
			needsInitialization = true;   
        }
        model = mCollectionContentModel;
    }
    else if(type == VideoCollectionWrapper::EGeneric)
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
        if(model && model->initialize(mSourceModel) < 0 )
        {
            delete model;
            return 0;
        }
        if (!connect(model, SIGNAL(shortDetailsReady(TMPXItemId)), mSourceModel, SIGNAL(shortDetailsReady(TMPXItemId))))
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
    if(!mSourceModel.isNull())
    {
        delete mSourceModel;
    }
}

// End of file
