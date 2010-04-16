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

// Version : %version:  da1mmcf#4 %


#include "mpxvideoplaybackwrapper.h"

int mCount = 0;

QMpxVideoPlaybackWrapper::QMpxVideoPlaybackWrapper()
{
    QMpxVideoPlaybackWrapper::Increment();
}

QMpxVideoPlaybackWrapper::~QMpxVideoPlaybackWrapper()
{
    QMpxVideoPlaybackWrapper::Decrement();
}

void QMpxVideoPlaybackWrapper::openPlaybackView()
{
    emit handlePlaybackView( MpxHbVideoCommon::ActivatePlaybackView );
}

void QMpxVideoPlaybackWrapper::Increment()
{
    ++mCount;
}

void QMpxVideoPlaybackWrapper::Decrement()
{
    if(mCount > 0)
    {   
        --mCount;
    }
}

int QMpxVideoPlaybackWrapper::GetInstanceCount()
{
    return mCount;
}

int QMpxVideoPlaybackWrapper::playMedia( QString aFileName )
{
    return 0;
}

void QMpxVideoPlaybackWrapper::lateInit()
{
}

// End of File
