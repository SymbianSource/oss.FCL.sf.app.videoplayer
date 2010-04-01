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
* Description:   CVideoCollectionWrapper dummy class implementation
*
*/

// INCLUDE FILES
#include <qabstractitemmodel.h>
#include <QDebug>
#include <QtTest/QtTest>
#include "videocollectionwrapper.h"

int VideoCollectionWrapper::mReferenceCount = 0;
VideoCollectionWrapper *VideoCollectionWrapper::mInstance = 0;

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::instance()
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
// VideoCollectionWrapper::decreaseReferenceCount()
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
mModel(0)
{
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::~VideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::~VideoCollectionWrapper()
{
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::open()
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel* VideoCollectionWrapper::getModel()
{
    return mModel;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::reset()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapper::reset()
{

}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::setModel()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapper::setModel(VideoSortFilterProxyModel* model)
{
    mModel = model;
}

// End of file
