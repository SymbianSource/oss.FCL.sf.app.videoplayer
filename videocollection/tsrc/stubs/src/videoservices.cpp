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
#include "videoplayerappexport.h"
#include "videoservices.h"
#include "videoserviceurifetch.h"
#include "videoservicebrowse.h"
#include "xqserviceutilxtra.h"

VideoServices* g_Instance = 0;

VideoServices* VideoServices::instance(QVideoPlayerEngine* engine)
{
    static VideoServices* g_Instance = 0;
    
    if(!g_Instance)
    {
        g_Instance = new VideoServices(engine);
    }
    else if(engine && !g_Instance->engine())
    {
    	g_Instance->setEngine(engine);
    }
    g_Instance->mReferenceCount++;
    return g_Instance;
}

void VideoServices::decreaseReferenceCount()
{
    if(g_Instance)
    {
        if(--g_Instance->mReferenceCount == 0)
        {
            delete g_Instance;
            g_Instance = NULL;
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
    mServiceBrowse = new VideoServiceBrowse(this);
}

VideoServices::~VideoServices()
{
    delete mServiceUriFetch;
    delete mServiceBrowse;
}

VideoServices::TVideoService VideoServices::currentService()
{
	return mCurrentService;
}

int VideoServices::getBrowseCategory() const
{
    int category = 0;
    
    if (mServiceBrowse)
    {
        category = mServiceBrowse->getBrowseCategory();
    }
    
    return category;
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

void VideoServices::browsingEnded()
{
    // not stubbed
}

// End of file
