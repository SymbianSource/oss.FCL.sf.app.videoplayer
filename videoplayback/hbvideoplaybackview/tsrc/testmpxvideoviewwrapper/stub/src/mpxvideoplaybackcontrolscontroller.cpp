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
* Description:  Implementation of MPXVideoPlaybackControlsController
*
*/

// Version : %version: 5 %



// INCLUDE FILES

#include "mpxvideoviewwrapper.h"
#include "hbvideobaseplaybackview.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "../inc/mpxvideoplaybackcontrolscontroller.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController(
        HbVideoBasePlaybackView *view,
        CMPXVideoViewWrapper *viewWrapper,
        QMPXVideoPlaybackViewFileDetails *details )
    : mView( view )
    , mViewWrapper( viewWrapper )
    , mFileDetails( details )   
{
    initializeController();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::initializeController()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::initializeController()
{
    connect( mView, SIGNAL( tappedOnScreen() ), this, SLOT( handleTappedOnScreen() ) );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::addFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::addFileDetails(
    QMPXVideoPlaybackViewFileDetails* details )
{
    Q_UNUSED( details );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController"));

    disconnect( mView, SIGNAL( tappedOnScreen() ), this, SLOT( handleTappedOnScreen() ) );

}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::handleEvent
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleEvent(
    TMPXVideoPlaybackControlCommandIds event, int value )
{
    switch ( event )
    {
        case EMPXControlCmdTvOutConnected:
        {
            handleTvOutEvent( true, event, value );
            break;
        }
        case EMPXControlCmdTvOutDisconnected:
        {
            handleTvOutEvent( false, event, value );
            break;
        }
        case EMPXControlCmdSetVolume:
        {
            mFileDetails->mAudioEnabled = false;
            break;
        }
        case EMPXControlCmdSetDuration:
        {
            mFileDetails->mDuration = value;
            break;
        }
        case EMPXControlCmdShowVolumeControls:
        {
            mFileDetails->mAudioEnabled = true;   
            break;
        }
        case EMPXControlCmdSetPosition:
        {
            mFileDetails->mSeekable = true;
            break;    
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::hideAllControls()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::hideAllControls()
{

}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::handleTvOutEvent
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleTvOutEvent(
        bool connected, TMPXVideoPlaybackControlCommandIds event, int value )
{
    Q_UNUSED( event );
    Q_UNUSED( value );
    mFileDetails->mTvOutConnected = connected;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::updateVideoRectDone
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::updateVideoRectDone()
{
    mViewTransitionIsGoingOn = false;
    
}


// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::skipToPreviousVideoItem
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::skipToPreviousVideoItem()
{
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::skipToNextVideoItem
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::skipToNextVideoItem()
{

}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::handleTappedOnScreen()
// -------------------------------------------------------------------------------------------------
//

void QMPXVideoPlaybackControlsController::handleTappedOnScreen()
{
}


// End of File
