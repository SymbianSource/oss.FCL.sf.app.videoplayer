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

// Version : %version: da1mmcf#24 %




#include <QTime>
#include <QTimer>
#include <QGraphicsSceneMouseEvent>

#include <hblabel.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbprogressslider.h>
#include <hbextendedlocale.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackcontrolscontroller.h"

const int KSeekingTimeOut = 250;

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::QMPXVideoPlaybackProgressBar
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackProgressBar::QMPXVideoPlaybackProgressBar(
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
    , mDraggingPosition( 0 )
    , mSetPosition( -1 )
    , mNeedToResumeAfterSetPosition( false )
    , mInitialized( false )
    , mSliderDragging( false )
    , mLongTimeFormat( false )
    , mLiveStreaming( false )
    , mSeekingTimer( NULL )
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

    if ( mSeekingTimer )
    {
        disconnect( mSeekingTimer, SIGNAL( timeout() ), this, SLOT( handleSeekingTimeout() ) );

        if ( mSeekingTimer->isActive() )
        {
            mSeekingTimer->stop();
        }

        delete mSeekingTimer;
        mSeekingTimer = NULL;
    }
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
        mLiveStreaming =
                ( mController->fileDetails()->mPlaybackMode == EMPXVideoLiveStreaming )? true:false;

        //
        // Create a timer for seeking.
        // We will issue SetPosition every KSeekingTimeOut msec to show the current frame to user
        //
        mSeekingTimer = new QTimer();
        mSeekingTimer->setSingleShot( false );
        mSeekingTimer->setInterval( KSeekingTimeOut );
        connect( mSeekingTimer, SIGNAL( timeout() ), this, SLOT( handleSeekingTimeout() ) );

        //
        // progress slider
        //
        QGraphicsWidget *widget = loader->findWidget( QString( "progressSlider" ) );
        mProgressSlider = qobject_cast<HbProgressSlider*>( widget );

        connect( mProgressSlider, SIGNAL( sliderPressed() ), this, SLOT( handleSliderPressed() ) );
        connect( mProgressSlider, SIGNAL( sliderReleased() ), this, SLOT( handleSliderReleased() ) );
        connect( mProgressSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( handleSliderMoved( int ) ) );

        //
        // If we init the progress bar after pp sends the duration informatin
        // we need to set the duration manually
        //
        durationChanged( (qreal)mController->fileDetails()->mDuration / (qreal)KPbMilliMultiplier );

        //
        // Set the position to 0 until we get position information
        //
        positionChanged( 0 );

        //
        // Set framedrawer for semi transparent background
        //
        HbFrameItem *frameItem = new HbFrameItem();
        frameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_multimedia_trans" );
        frameItem->frameDrawer().setFrameType( HbFrameDrawer::NinePieces );
        frameItem->frameDrawer().setFillWholeRect( true );
        mProgressSlider->setBackgroundItem( frameItem );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::durationChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::durationChanged duration = %d"), duration );

    if ( mLiveStreaming )
    {
        mDuration = 0;
        mProgressSlider->setMaxText( "Live" );
    }
    else
    {
        mDuration = duration;

        if ( ( mDuration / 3600 ) > 0 )
        {
            mLongTimeFormat = true;
        }
        else
        {
            mLongTimeFormat = false;
        }

        mProgressSlider->setMaxText( valueToReadableFormat( mDuration ) );
    }

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
    if ( ! mSliderDragging )
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
    else if ( position > mDuration && ! mLiveStreaming )
    {
        position = mDuration;
    }

    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::updatePostion position = %d"), position );

    mProgressSlider->setMinText( valueToReadableFormat( position ) );

    //
    // Don't need to set the slider for live streaming
    //
    if ( ! mLiveStreaming )
    {
        mProgressSlider->setSliderValue( position );
        mProgressSlider->setProgressValue( position );
    }
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

    HbExtendedLocale locale = HbExtendedLocale::system();

    if ( mLongTimeFormat )
    {
        str = locale.format( time, r_qtn_time_durat_long );
    }
    else
    {
        str = locale.format( time, r_qtn_time_durat_min_sec );
    }

    return str;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::handleSliderPressed
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::handleSliderPressed()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackProgressBar::handleSliderPressed()"));

    mSliderDragging = true;

    mController->resetDisappearingTimers( EMPXTimerCancel );

    //
    // Pause the playback if it's playing
    //
    if( mController->state() == EPbStatePlaying )
    {
        mNeedToResumeAfterSetPosition = true;
        mController->handleCommand( EMPXPbvCmdCustomPause );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::handleSliderMoved
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::handleSliderMoved( int value )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::handleSliderMoved() position = %d"), value);

    updatePostion( value );

    //
    // If the slider is dragging, start the timer and seek every KSeekingTimeOut msec
    //
    if ( mSliderDragging )
    {
        mDraggingPosition = value;
        MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::handleSliderMoved() mDraggingPosition = %d"), mDraggingPosition);

        if ( mSeekingTimer && ! mSeekingTimer->isActive() )
        {
            mSeekingTimer->start();
        }
    }
    else
    {
        if ( value >= mDuration )
        {
            MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::setPosition() reached end of the clip"));

            mController->handleCommand( EMPXPbvCmdEndOfClip );
        }
        else
        {
            value = mProgressSlider->sliderValue();

            MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::setPosition() position = %d"), value);
            mController->handleCommand( EMPXPbvCmdSetPosition, value );
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::handleSliderReleased
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::handleSliderReleased()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackProgressBar::handleSliderReleased()"));

    mSliderDragging = false;

    if ( mSeekingTimer && mSeekingTimer->isActive() )
    {
        mSeekingTimer->stop();
    }

    mController->resetDisappearingTimers( EMPXTimerReset );
    int value = mProgressSlider->sliderValue();

    if ( value >= mDuration )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::setPosition() reached end of the clip"));
        mController->handleCommand( EMPXPbvCmdEndOfClip );
    }
    else
    {
        if ( value < 0 )
        {
            value = 0;
        }

        MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::setPosition() position = %d"), value);
        mController->handleCommand( EMPXPbvCmdSetPosition, value );

        //
        // Resume if it was playing
        //
        if( mNeedToResumeAfterSetPosition )
        {
            mNeedToResumeAfterSetPosition = false;
            mController->handleCommand( EMPXPbvCmdCustomPlay );
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::updateControlsWithFileDetails()"));

    if ( ! details->mPausableStream || ! details->mSeekable )
    {
        mProgressSlider->setEnabled( false );
    }
    else if ( ! mProgressSlider->isEnabled() )
    {
        mProgressSlider->setEnabled( true );
    }

    mProgressSlider->backgroundItem()->setVisible(
            ( mController->viewMode() == EFullScreenView )? ETrue:EFalse );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::updateState() state = %d"), state );

    switch ( state )
    {
        case EPbStatePlaying:
        case EPbStatePaused:
        {
            if ( ! mProgressSlider->isEnabled() )
            {
                mProgressSlider->setEnabled( true );
            }

            break;
        }
        default:
        {
            if ( mProgressSlider->isEnabled() )
            {
                mProgressSlider->setEnabled( false );
            }
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::handleSeekingTimeout()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackProgressBar::handleSeekingTimeout()
{
    if ( mDraggingPosition >= mDuration )
    {
        mDraggingPosition = mDuration;
    }
    else if( mDraggingPosition < 0 )
    {
        mDraggingPosition = 0;
    }

    MPX_DEBUG(_L("QMPXVideoPlaybackProgressBar::handleSeekingTimeout() Dragging pos = %d, Set pos = %d")
            , mDraggingPosition, mSetPosition );

    if ( mSetPosition != mDraggingPosition )
    {
        mSetPosition = mDraggingPosition;
        mController->handleCommand( EMPXPbvCmdSetPosition, mSetPosition );
    }
}

//End of file
