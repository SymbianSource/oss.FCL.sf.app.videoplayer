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
* Description:  Implementation of VideoServices
*
*/

#include <videoplayerengine.h>
#include <videoservices.h>
#include <videoserviceurifetch.h>
#include <videoserviceplay.h>

VideoServices *VideoServices::mInstance = 0;

// -----------------------------------------------------------------------------
// VideoServices::instance()
// -----------------------------------------------------------------------------
//
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

// -----------------------------------------------------------------------------
// VideoServices::decreaseReferenceCount()
// -----------------------------------------------------------------------------
//
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

// ----------------------------------------------------------------------------
// setEngine()
// ----------------------------------------------------------------------------
//
void VideoServices::setEngine(QVideoPlayerEngine* engine)
{
    if (mServicePlay)
    {
    	mEngine = engine;
    	mServicePlay->setEngine(engine);
    }
}

// ----------------------------------------------------------------------------
// engine()
// ----------------------------------------------------------------------------
//
QVideoPlayerEngine* VideoServices::engine()
{
	return mEngine;
}

// ----------------------------------------------------------------------------
// VideoServices()
// ----------------------------------------------------------------------------
//
VideoServices::VideoServices(QVideoPlayerEngine* engine) :
mReferenceCount(0),
mEngine(engine),
mCurrentService(VideoServices::ENoService)
{
    mServiceUriFetch = new VideoServiceUriFetch(this);
	mServicePlay     = new VideoServicePlay(this, engine);
}

// ----------------------------------------------------------------------------
// ~VideoServices()
// ----------------------------------------------------------------------------
//
VideoServices::~VideoServices()
{
	delete mServiceUriFetch;
	delete mServicePlay;
}

// ----------------------------------------------------------------------------
// currentService()
// ----------------------------------------------------------------------------
//
VideoServices::TVideoService VideoServices::currentService()
{
	return mCurrentService;
}

// ----------------------------------------------------------------------------
// setCurrentService()
// ----------------------------------------------------------------------------
//
void VideoServices::setCurrentService(VideoServices::TVideoService service)
{
	mCurrentService = service;
}
// ----------------------------------------------------------------------------
// contextTitle()
// ----------------------------------------------------------------------------
//
QString VideoServices::contextTitle() const
{
    return mServiceUriFetch->contextTitle();
}

// ----------------------------------------------------------------------------
// itemSelected()
// ----------------------------------------------------------------------------
//
void VideoServices::itemSelected(const QString& item)
{
    QStringList list;
    list.append(item);
    mServiceUriFetch->complete(list);
}

