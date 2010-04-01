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
* Description:  Implementation of MPXVideoPlaybackControlsController
*
*/

// Version : %version:  2 %



// INCLUDE FILES
#include <coecntrl.h>
#include <bautils.h>
#include <barsread.h>
#include <stringloader.h>
#include <f32file.h>

#include <qtimer>

#include <hblabel.h>
#include <hbaction.h>
#include <hbiconanimator.h>

#include "../inc/mpxvideoplaybackviewfiledetails.h"
#include "../inc/mpxvideoplaybackcontrolscontroller.h"

#include "mpxvideoplaybackcontrolconfiguration.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController(
        QMPXVideoPlaybackViewFileDetails *details )
    : mFileDetails( details )   
{
    initializeController();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::initializeController()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::initializeController()
{	
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
// QMPXVideoPlaybackControlsController::handleStateChange
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleStateChange( TMPXPlaybackState newState )
{
    Q_UNUSED( newState );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::controlsListUpdated()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::controlsListUpdated()
{

}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::appendControl()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::appendControl( TMPXVideoPlaybackControls controlIndex )
{
    Q_UNUSED( controlIndex );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::resetDisappearingTimers()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::resetDisappearingTimers( TMPXTimerAction timerAction )
{
    Q_UNUSED( timerAction );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::hideAllControls()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::hideAllControls()
{

}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::showControls()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::showControls()
{
        
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::isVisible()
{
    return false;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::handleCommand()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleCommand( 
        TMPXVideoPlaybackViewCommandIds command, int value )
{
    Q_UNUSED( command );
    Q_UNUSED( value );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::volumeChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::volumeChanged( int volume )
{
    Q_UNUSED( volume );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::showVolumeControls()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::showVolumeControls()
{

}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::durationChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::durationChanged( int duration )
{
    Q_UNUSED( duration );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::positionChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::positionChanged( int position )
{
    Q_UNUSED( position );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::aspectRatioChanged( int aspectRatio )
{
    Q_UNUSED( aspectRatio );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::realFormat()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::realFormat( QString filename )
{
    Q_UNUSED( filename );
    return false;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::realFormatForStreaming()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::realFormatForStreaming( const TDesC& des )
{
    Q_UNUSED( des );
	bool realFormat = false;
    return realFormat;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::realFormatForLocal()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::realFormatForLocal()
{
    bool realFormat = false;
    
    return realFormat;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::setDownloadSize()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::setDownloadSize( int size )
{
    Q_UNUSED( size );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::updateDownloadPosition()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::updateDownloadPosition( int size )
{
    Q_UNUSED( size );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::handleErrors
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleErrors()
{

}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::isSoftKeyVisible()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::isSoftKeyVisible( int /*value*/ )
{
    return false;
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
//   QMPXVideoPlaybackControlsController::updateVideoRect()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::updateVideoRect( bool transitionEffect )
{
    Q_UNUSED( transitionEffect );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::handleTappedOnScreen()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleTappedOnScreen()
{

}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::changeViewMode
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::changeViewMode( 
        TPlaybackViewMode viewMode, bool transitionEffect )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::changeViewMode()"));
    Q_UNUSED( transitionEffect );
    
    mViewMode = viewMode;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::viewMode
// -------------------------------------------------------------------------------------------------
//
TPlaybackViewMode QMPXVideoPlaybackControlsController::viewMode()
{
    return mViewMode;
}


// End of File
