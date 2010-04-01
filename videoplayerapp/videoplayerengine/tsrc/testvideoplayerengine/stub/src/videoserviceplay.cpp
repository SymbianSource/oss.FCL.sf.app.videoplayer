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
* Description:  Implementation of QVideoPlayerEngine
*
*/

// Version : %version: 1 %


#include <videoservices.h>
#include "videoserviceplay.h"
#include "videoplayerengine.h"


VideoServicePlay::VideoServicePlay( VideoServices* parent, QVideoPlayerEngine* engine )
{
	mServiceApp = parent;
}

VideoServicePlay::~VideoServicePlay()
{
}

void VideoServicePlay::playMedia( QString filePath )
{
   	mServiceApp->setCurrentService(VideoServices::EPlayback);
}
