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
* Description:  Stub-implementation of QMpxVideoPlaybackWrapper
*
*/

// Version : %version:  da1mmcf#6 %


#include "mpxvideoplaybackwrapper.h"
#include "mpxvideo_debug.h"

int mCount = 0;

QMpxVideoPlaybackWrapper::QMpxVideoPlaybackWrapper()
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::QMpxVideoPlaybackWrapper()"));    
    QMpxVideoPlaybackWrapper::Increment();
}

QMpxVideoPlaybackWrapper::~QMpxVideoPlaybackWrapper()
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::~QMpxVideoPlaybackWrapper()"));
    QMpxVideoPlaybackWrapper::Decrement();
}

void QMpxVideoPlaybackWrapper::openPlaybackView()
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::openPlaybackView()"));    
    emit handlePlaybackView( MpxHbVideoCommon::ActivatePlaybackView );
}

void QMpxVideoPlaybackWrapper::Increment()
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::Increment()"));    
    ++mCount;
}

void QMpxVideoPlaybackWrapper::Decrement()
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::Decrement()"));
    
    if ( mCount > 0 )
    {   
        --mCount;
    }
}

int QMpxVideoPlaybackWrapper::GetInstanceCount()
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::GetInstanceCount()"));    
    return mCount;
}

int QMpxVideoPlaybackWrapper::playMedia( QString aFileName )
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::playMedia( aFileName )"));
    
    Q_UNUSED( aFileName );    
    return 0;
}

int QMpxVideoPlaybackWrapper::playMedia( RFile aFile )
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::playMedia( aFile )"));
    
    Q_UNUSED( aFile );    
    return 0;
}

void QMpxVideoPlaybackWrapper::lateInit()
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::lateInit()"));    
}

// End of File
