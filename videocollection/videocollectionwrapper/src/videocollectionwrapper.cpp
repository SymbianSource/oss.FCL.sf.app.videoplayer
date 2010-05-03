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
* Description:   VideoCollectionWrapper class implementation
* 
*/

// Version : %version: %

// INCLUDE FILES
#include <qabstractitemmodel.h>

#include "videocollectionwrapper.h"
#include "videocollectionwrapper_p.h"
#include "videosortfilterproxymodel.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::CVideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper &VideoCollectionWrapper::instance()
{
	FUNC_LOG;
    static VideoCollectionWrapper _staticWrapper;

    return _staticWrapper;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::VideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::VideoCollectionWrapper() : 
    d( new VideoCollectionWrapperPrivate ),
    mReferenceCount(0)
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::~CVideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::~VideoCollectionWrapper()
{
	FUNC_LOG;
    // NOP
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::getModel()
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel* VideoCollectionWrapper::getModel(VideoCollectionCommon::TModelType type)
{
	FUNC_LOG;
    if(d)
    {
        return d->getModel(type);
    }
    return 0;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::sendAsyncStatus()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapper::sendAsyncStatus(int statusCode,  QVariant &additional)
{
	FUNC_LOG;
    emit asyncStatus(statusCode, additional);
}

// End of file
