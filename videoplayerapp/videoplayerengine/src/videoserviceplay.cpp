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
* Description:  Implementation of VideoServicePlay
*
*/


#include <videoservices.h>
#include "videoserviceplay.h"
#include "videoplayerengine.h"


VideoServicePlay::VideoServicePlay( VideoServices* parent, QVideoPlayerEngine* engine )
: XQServiceProvider(QLatin1String("com.nokia.Videos.IVideoView"), parent),
  mEngine(engine),
  mServiceApp(parent)
{
    publishAll();
}

VideoServicePlay::~VideoServicePlay()
{
}

void VideoServicePlay::setEngine( QVideoPlayerEngine* engine )
{
	mEngine = engine;
}

void VideoServicePlay::playMedia( QString filePath )
{
    if(mEngine)
    {
    	mServiceApp->setCurrentService(VideoServices::EPlayback);
    	mEngine->playMedia( filePath );
    }
}

void VideoServicePlay::playPDLMedia( QString filePath, int downloadID  )
{
}

void VideoServicePlay::closePlayer()
{
}

