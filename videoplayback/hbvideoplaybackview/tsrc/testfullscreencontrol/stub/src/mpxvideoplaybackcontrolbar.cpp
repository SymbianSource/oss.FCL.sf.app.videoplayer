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
* Description:  Implementation of QMPXVideoPlaybackControlBar
*
*/

// Version : %version:  2 %



#include <hbframeitem.h>
#include <hbframedrawer.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrolbar.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::QMPXVideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlBar::QMPXVideoPlaybackControlBar( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlBar::QMPXVideoPlaybackControlBar"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::updateState() state = %d"), state );

    mState = state;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::aspectRatioChanged() aspectRatio = %d"), aspectRatio );

    mAspectRatio = aspectRatio;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::updateWithFileDetails()"));

    mFileDetails = details;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::setVisibleToControlBar()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::setVisibleToControlBar( bool visible )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::setVisibleToControlBar()"));

    mVisibility = visible;
}


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::durationChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::durationChanged()"));

    mDuration = duration;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::positionChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::positionChanged( int position )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::positionChanged()"));

    mPosition = position;
}

//End of file
