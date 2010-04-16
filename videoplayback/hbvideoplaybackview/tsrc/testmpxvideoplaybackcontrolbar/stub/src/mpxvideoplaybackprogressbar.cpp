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
    , mDuration( -1 )
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

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::durationChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::durationChanged duration = %d"), duration );

    mDuration = duration;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::positionChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::positionChanged( int position )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::positionChanged position = %d"), position );

    mPosition = position;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::updateControlsWithFileDetails()"));

    mFileDetails = details;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::updateState() state = %d"), state );

    mState = state;
}

//End of file
