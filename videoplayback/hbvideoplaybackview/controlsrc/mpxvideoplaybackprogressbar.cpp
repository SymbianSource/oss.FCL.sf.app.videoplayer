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
* Description:  Implementation of QMPXVideoPlaybackProgressBar
*
*/

// Version : %version: da1mmcf#14 %




#include <QTime>
#include <QGraphicsSceneMouseEvent>

#include <hblabel.h>
#include <hbprogressbar.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackcontrolscontroller.h"

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::QMPXVideoPlaybackProgressBar
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackProgressBar::QMPXVideoPlaybackProgressBar( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
    , mDuration( -1 )
    , mNeedToResumeAfterSetPosition( false )
    , mInitialized( false )
    , mDragging( false )
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

    QMPXVideoPlaybackDocumentLoader *loader = mController->layoutLoader();

    //
    // Don't need to initialize buttons once it gets initialized
    //
    if ( loader && ! mInitialized )
    {
        mInitialized = true;

        //
        // progress slider
        //
        QGraphicsWidget *widget = loader->findWidget( QString( "progressSlider" ) );
        mProgressSlider = qobject_cast<HbProgressBar*>( widget );

        //
        // position label
        //
        widget = loader->findWidget( QString( "positionLabel" ) );
        mPositionLabel= qobject_cast<HbLabel*>( widget );

        //
        // duration label
        //
        widget = loader->findWidget( QString( "durationLabel" ) );
        mDurationLabel= qobject_cast<HbLabel*>( widget );

        //
        // If we init the progress bar after pp sends the duration informatin
        // we need to set the duration manually 
        //
        durationChanged( (qreal)mController->fileDetails()->mDuration / (qreal)KPbMilliMultiplier );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::durationChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::durationChanged duration = %d"), duration );

    mDuration = duration;

    mDurationLabel->setPlainText( valueToReadableFormat( mDuration ) );
    mProgressSlider->setRange( 0, mDuration );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::positionChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::positionChanged( int position )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::positionChanged position = %d"), position );

    //
    // While dragging event, don't update old position information from mpx framework
    //
    if ( ! mDragging )
    {
        updatePostion( position );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::updatePostion
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::updatePostion( int position )
{
    if ( position < 0 )
    {
        position = 0;
    }
    else if ( position > mDuration )
    {
        position = mDuration;
    }

    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::updatePostion position = %d"), position );

    mPositionLabel->setPlainText( valueToReadableFormat( position ) );
    mProgressSlider->setProgressValue( position );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::valueToReadableFormat
// -------------------------------------------------------------------------------------------------
//
QString QMPXVideoPlaybackProgressBar::valueToReadableFormat( int value )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::valueToReadableFormat value = %d"), value);

    int hour = value / 3600;
    value = value % 3600;
    int minutes = value / 60;
    value = value % 60;
    int second = value;

    QTime time( hour ,minutes ,second );
    QString str;

    if ( hour == 0 )
    {
        str = time.toString("mm:ss");
    }
    else
    {
        str = time.toString("hh:mm:ss");
    }

    return str;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::mousePressEvent
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::mousePressEvent( QGraphicsSceneMouseEvent *event ) 
{
    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::mousePressEvent()"));

    mDragging = true;

    mController->resetDisappearingTimers( EMPXTimerCancel );

    //
    // Pause the playback if it's playing
    //
    if( mController->state() == EPbStatePlaying )
    {
        mNeedToResumeAfterSetPosition = true;
        mController->handleCommand( EMPXPbvCmdCustomPause );   
    }

    event->accept();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::mouseReleaseEvent
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::mouseReleaseEvent()"));

    mDragging = false;

    mController->resetDisappearingTimers( EMPXTimerReset );

    int position = 
        (int)( ( event->scenePos().x() - mProgressSlider->geometry().x() ) / 
        mProgressSlider->geometry().width() * (qreal)mDuration );

    if ( position > mDuration )
    {
        mController->handleCommand( EMPXPbvCmdEndOfClip );
    }
    else
    {
        if ( position < 0 )
        {
            position = 0;
        }

        mController->handleCommand( EMPXPbvCmdSetPosition, position );

        //
        // Resume if it was playing
        //
        if( mNeedToResumeAfterSetPosition )
        {
            mNeedToResumeAfterSetPosition = false;
            mController->handleCommand( EMPXPbvCmdCustomPlay );
        }
    }

    event->accept();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::mouseMoveEvent
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    qreal result = 
        ( event->scenePos().x() - mProgressSlider->geometry().x() ) / 
        mProgressSlider->geometry().width() * (qreal)mDuration;

    updatePostion( result );

    event->accept();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::updateControlsWithFileDetails()"));

    if ( details->mPlaybackMode == EMPXVideoLiveStreaming )
    {
        setEnabled( false );
        mDurationLabel->setPlainText( "Live" );
    }
    else if ( details->mTvOutConnected && ! details->mTvOutPlayAllowed )
    {
        setEnabled( false );
    }
    else
    {
        setEnabled( true );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::updateState() state = %d"), state );

    if ( mController->viewMode() == EAudioOnlyView )
    {
        switch ( state )
        {
            case EPbStatePlaying:
            case EPbStatePaused:
            {
                updateWithFileDetails( mController->fileDetails() );
                break;
            }
            case EPbStateNotInitialised:
            case EPbStateInitialising:
            case EPbStateBuffering:
            {
                setEnabled( false );
                break;
            }
        }
    }
}

//End of file
