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
* Description:  Implementation of VideoServices
*
*/

#include <qobject.h>
#include "videoservices.h"
#include "videoserviceurifetch.h"

VideoServices *VideoServices::mInstance = 0;

VideoServices* VideoServices::instance(QVideoPlayerEngine* engine)
{
    if(!mInstance)
    {
        mInstance = new VideoServices(engine);
    }
    else if(engine && !mInstance->engine())
    {
    	mInstance->setEngine(engine);
    }
    mInstance->mReferenceCount++;
    return mInstance;
}

void VideoServices::decreaseReferenceCount()
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

void VideoServices::setEngine(QVideoPlayerEngine* engine)
{
    Q_UNUSED(engine);
    // not stubbed
}

QVideoPlayerEngine* VideoServices::engine()
{
    // not stubbed
    return 0;
}

VideoServices::VideoServices(QVideoPlayerEngine* engine):
    mReferenceCount(0),
    mEngine(engine),
    mCurrentService(VideoServices::ENoService)
{
    mServiceUriFetch = new VideoServiceUriFetch(this);
}

VideoServices::~VideoServices()
{
    delete mServiceUriFetch;
}

VideoServices::TVideoService VideoServices::currentService()
{
	return mCurrentService;
}

void VideoServices::setCurrentService(VideoServices::TVideoService service)
{
	mCurrentService = service;
}
QString VideoServices::contextTitle() const
{
    // not stubbed
    return QString();
}

void VideoServices::itemSelected(const QString& item)
{
    Q_UNUSED(item);
    // not stubbed
}

