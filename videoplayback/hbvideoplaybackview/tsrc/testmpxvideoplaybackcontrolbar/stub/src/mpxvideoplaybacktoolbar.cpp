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
* Description:  Implementation of QMPXVideoPlaybackToolBar
*
*/

// Version : %version: 1 %



#include <hbpushbutton.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybacktoolbar.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::QMPXVideoPlaybackToolBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackToolBar::QMPXVideoPlaybackToolBar( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackToolBar::QMPXVideoPlaybackToolBar"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::~QMPXVideoPlaybackToolBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackToolBar::~QMPXVideoPlaybackToolBar()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::~QMPXVideoPlaybackToolBar()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::initialize()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::initialize()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackToolBar::initialize()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::updateState() state = %d"), state );

    mState = state;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::aspectRatioChanged() aspectRatio = %d"), aspectRatio );

    mAspectRatio = aspectRatio;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::updateWithFileDetails()"));

    mFileDetails = details;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::durationChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::durationChanged duration = %d"), duration );

    mDuration = duration;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::positionChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::positionChanged( int position )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::positionChanged position = %d"), position );

    mPosition = position;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::setVisible
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::setVisible( bool visible )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::setVisible position = %d"), visible );

    mVisible = visible;
}

//End of file
