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
* Description:   ?Description
*
*/

#include <videoplayerengine.h>
#include <videoservices.h>
#include <videoserviceurifetch.h>
#include <videoserviceplay.h>


VideoServices *VideoServices::mInstance = 0;
int  VideoServices::mReferenceCount = 0;

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
// VideoServices()
// ----------------------------------------------------------------------------
//
VideoServices::VideoServices(QVideoPlayerEngine* engine) :
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

