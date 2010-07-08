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
* Description:  Stub-implementation of VideoPlaybackWrapper
*
*/

// Version : %version:  da1mmcf#7 %


#include "videoplaybackwrapper.h"
#include "mpxvideo_debug.h"

int mCount = 0;

VideoPlaybackWrapper::VideoPlaybackWrapper()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::VideoPlaybackWrapper()"));    
    VideoPlaybackWrapper::Increment();
}

VideoPlaybackWrapper::~VideoPlaybackWrapper()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::~VideoPlaybackWrapper()"));
    VideoPlaybackWrapper::Decrement();
}

void VideoPlaybackWrapper::openPlaybackView()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::openPlaybackView()"));    
    emit handlePlaybackView( MpxHbVideoCommon::ActivatePlaybackView );
}

void VideoPlaybackWrapper::Increment()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::Increment()"));    
    ++mCount;
}

void VideoPlaybackWrapper::Decrement()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::Decrement()"));
    
    if ( mCount > 0 )
    {   
        --mCount;
    }
}

int VideoPlaybackWrapper::GetInstanceCount()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::GetInstanceCount()"));    
    return mCount;
}

int VideoPlaybackWrapper::playMedia( QString aFileName )
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::playMedia( aFileName )"));
    
    Q_UNUSED( aFileName );    
    return 0;
}

int VideoPlaybackWrapper::playMedia( RFile aFile )
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::playMedia( aFile )"));
    
    Q_UNUSED( aFile );    
    return 0;
}

void VideoPlaybackWrapper::lateInit()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::lateInit()"));    
}

// End of File
