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

// INCLUDE FILES
#include <qabstractitemmodel.h>
#include <QDebug>
#include "videocollectionwrapper.h"
#include "videocollectionwrapper_p.h"
#include "videosortfilterproxymodel.h"

VideoCollectionWrapper *VideoCollectionWrapper::mInstance = 0;

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::CVideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper *VideoCollectionWrapper::instance()
{
    if(!mInstance)
    {
        mInstance = new VideoCollectionWrapper();
    }
    mInstance->mReferenceCount++;
    return mInstance;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::cleanup()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapper::decreaseReferenceCount()
{
    if(mInstance)
    {
        if(--mInstance->mReferenceCount == 0)
        {
            delete mInstance;
            mInstance = NULL;
        }
    }
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::VideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::VideoCollectionWrapper() : 
    d( new VideoCollectionWrapperPrivate ),
    mReferenceCount(0)
{
    qDebug() << "CVideoCollectionWrapper::CVideoCollectionWrapper(): Constructing singleton";
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::~CVideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::~VideoCollectionWrapper()
{
    delete d;
    d = 0;
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::getModel()
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel* VideoCollectionWrapper::getModel()
{
    if(d)
    {
        return d->getModel();
    }
    return 0;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::sendAsyncStatus()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapper::sendAsyncStatus(int statusCode,  QVariant &additional)
{
    emit asyncStatus(statusCode, additional);
}

// End of file
