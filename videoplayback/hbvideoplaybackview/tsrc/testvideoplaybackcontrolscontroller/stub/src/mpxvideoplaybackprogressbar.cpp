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
* Description:  Implementation of QMPXVideoPlaybackProgressBar
*
*/

// Version : %version:  1 %





#include "mpxvideo_debug.h"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxvideoplaybackcontrolscontroller.h"

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::QMPXVideoPlaybackProgressBar
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackProgressBar::QMPXVideoPlaybackProgressBar( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackProgressBar::QMPXVideoPlaybackProgressBar()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::~QMPXVideoPlaybackProgressBar
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackProgressBar::~QMPXVideoPlaybackProgressBar()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackProgressBar::~QMPXVideoPlaybackProgressBar()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::initialize
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::initialize()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackProgressBar::initialize()"));
}

//End of file
