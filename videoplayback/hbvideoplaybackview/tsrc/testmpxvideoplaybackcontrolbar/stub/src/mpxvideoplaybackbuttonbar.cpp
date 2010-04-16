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
* Description:  Implementation of QMPXVideoPlaybackButtonBar
*
*/

// Version : %version: 2 %



#include <hbpushbutton.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::QMPXVideoPlaybackButtonBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackButtonBar::QMPXVideoPlaybackButtonBar( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackButtonBar::QMPXVideoPlaybackButtonBar"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::~QMPXVideoPlaybackButtonBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackButtonBar::~QMPXVideoPlaybackButtonBar()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::~QMPXVideoPlaybackButtonBar()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::initialize()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::initialize()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackButtonBar::initialize()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::updateState() state = %d"), state );

    mState = state;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::aspectRatioChanged() aspectRatio = %d"), aspectRatio );

    mAspectRatio = aspectRatio;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::updateWithFileDetails()"));

    mFileDetails = details;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::durationChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::durationChanged duration = %d"), duration );

    mDuration = duration;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::positionChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::positionChanged( int position )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::positionChanged position = %d"), position );

    mPosition = position;
}

//End of file
