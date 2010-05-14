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

// Version : %version:  3 %



#include <QTimer>
#include <QGraphicsLayout>

#include <hbeffect.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbinstance.h>
#include <hbtoolbutton.h>
#include <w32std.h>

#include "mpxvideo_debug.h"
#include "hbvideobaseplaybackview.h"
#include "mpxvideoplaybacktoolbar.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"

const int KSeekStartTimeOut = 700;
const int KRetrieveButtonTimeOut = 1000;

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::QMPXVideoPlaybackToolBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackToolBar::QMPXVideoPlaybackToolBar( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
    , mSeekStartTimer( NULL )
    , mRetrieveButtonTimer( NULL )
    , mSeekingState( EMPXNotSeeking )
    , mInitialized( false )
    , mNeverVisibled( true )
    , mPosition( 0 )
    , mDuration( 0 )
    , mAspectRatio( EMPXPbvCmdNaturalAspectRatio )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackToolBar::QMPXVideoPlaybackToolBar"));

    mToolBar = mController->view()->toolBar();
    mToolBar->setOrientation( Qt::Horizontal );
    HbEffect::disable( mToolBar );
    
    mController->view()->hideItems( Hb::ToolBarItem );

    initialize();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::~QMPXVideoPlaybackToolBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackToolBar::~QMPXVideoPlaybackToolBar()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::~QMPXVideoPlaybackToolBar()"));

    HbEffect::enable( mToolBar );
    mToolBar->clearActions();

    for ( int i = 0 ; i < mButtonIcons.count() ; i++ )
    {
        if ( mButtonIcons[i] )
        {
            delete mButtonIcons[i];
        }
    }
    mButtonIcons.clear();

    if ( mSeekStartTimer )
    {
        if ( mSeekStartTimer->isActive() )
        {
            mSeekStartTimer->stop();
        }

        delete mSeekStartTimer;
        mSeekStartTimer = NULL;
    }

    if ( mRetrieveButtonTimer )
    {
        if ( mRetrieveButtonTimer->isActive() )
        {
            mRetrieveButtonTimer->stop();
        }

        delete mRetrieveButtonTimer;
        mRetrieveButtonTimer = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::initialize()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::initialize()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackToolBar::initialize()"));

    //
    // Don't need to initialize buttons once it gets initialized
    //
    if ( ! mInitialized )
    {
        mInitialized = true;

        QActionGroup *actionsGroup = new QActionGroup( mToolBar );

        for ( int i = 0 ; i < EMPXButtonCount ; i++ )
        {
            mButtonActions.append( new HbAction( actionsGroup ) );
        }

        for ( int i = 0 ; i < EMPXIconCount ; i++ )
        {
            mButtonIcons.append( new HbIcon() );
        }

        //
        // RW button
        //
        mButtonIcons[EMPXRWIcon]->setIconName( "qtg_mono_previous" );
        mButtonActions[EMPX2ndButton]->setIcon( *mButtonIcons[EMPXRWIcon] );

        //
        // Play/Pause button
        //
        mButtonIcons[EMPXPlayIcon]->setIconName( "qtg_mono_play" );
        mButtonIcons[EMPXPauseIcon]->setIconName( "qtg_mono_pause" );
        mButtonActions[EMPX3rdButton]->setIcon( *mButtonIcons[EMPXPlayIcon] );

        //
        // FF button
        //
        mButtonIcons[EMPXFFIcon]->setIconName( "qtg_mono_next" );
        mButtonActions[EMPX4thButton]->setIcon( *mButtonIcons[EMPXFFIcon] );

        //
        // Details button
        //
        mButtonIcons[EMPXDetailsIcon]->setIconName( "qtg_mono_info" );
        mButtonActions[EMPX5thButton]->setIcon( *mButtonIcons[EMPXDetailsIcon] );

        //
        // Aspect ratio buttons
        //
        mButtonIcons[EMPXNaturalIcon]->setIconName( "qtg_mono_aspect_ratio_natural" );
        mButtonIcons[EMPXStretchIcon]->setIconName( "qtg_mono_aspect_ratio_stretched" );
        mButtonIcons[EMPXZoomIcon]->setIconName( "qtg_mono_aspect_ratio_zoom" );
        mButtonActions[EMPX1stButton]->setIcon( *mButtonIcons[EMPXNaturalIcon] );

        //
        // Attach/Share button 
        //
        mButtonIcons[EMPXAttachIcon]->setIconName( "qtg_mono_attach" );
        mButtonIcons[EMPXShareIcon]->setIconName( "qtg_mono_share" );

        for ( int i = 0 ; i < EMPXButtonCount ; i++ )
        {
            mButtonActions[i]->setCheckable( false );
            mToolBar->addAction( mButtonActions[i] );
        }

        mDuration = (qreal)mController->fileDetails()->mDuration / (qreal)KPbMilliMultiplier;

        //
        // Create a timer for seeking. 
        // We will issue SetPosition every KSeekingTimeOut msec to show the current frame to user
        //
        mSeekStartTimer = new QTimer();
        mSeekStartTimer->setSingleShot( true );
        mSeekStartTimer->setInterval( KSeekStartTimeOut );
        
        //
        // get window size
        //
        RWindow *window = mController->view()->getWindow();            
        TRect displayRect = TRect( TPoint( window->Position() ), TSize( window->Size() ) );
        
        //
        // get window aspect ratio
        //   if device is in portrait mode, width > height
        //   if device is in landscape mode, width < height
        //
        TReal32 width = (TReal32) displayRect.Width();
        TReal32 height = (TReal32) displayRect.Height();            
        mDisplayAspectRatio = (width > height)? (width / height) : (height / width);
        
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::playPause()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::playPause()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::playPause()"));

    mController->resetDisappearingTimers( EMPXTimerReset );
    mController->handleCommand( EMPXPbvCmdPlayPause );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::ffPressing()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::ffPressing()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackToolBar::ffPressing()"),
                   _L("mSeekingState = %d"), mSeekingState );

    if ( mSeekingState == EMPXNotSeeking )
    {
        mSeekingState = EMPXFastForwarding;

        mController->handleCommand( EMPXPbvCmdSeekForward );        
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::rwPressing()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::rwPressing()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackToolBar::rwPressing()"),
                   _L("mSeekingState = %d"), mSeekingState );

    if ( mSeekingState == EMPXNotSeeking )
    {
        mSeekingState = EMPXRewinding;

        mController->handleCommand( EMPXPbvCmdSeekBackward );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::ffReleased()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::ffReleased()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackToolBar::ffReleased()"),
                   _L("mSeekingState = %d"), mSeekingState );

    disconnect( mSeekStartTimer, SIGNAL( timeout() ), this, SLOT( ffPressing() ) );

    if ( mSeekStartTimer->isActive() )
    {
        mSeekStartTimer->stop();
    }

    if ( mSeekingState == EMPXFastForwarding )
    {
        mController->handleCommand( EMPXPbvCmdEndSeek );        
    }
    else
    {
        int temp = mPosition + KMPXFastForward;
        MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::ffReleased() temp position = %d"), temp);

        //
        // If it has the playing time which is less than KMPXFastForward, ignore
        //
        if ( temp < mDuration )
        {
            mController->handleCommand( EMPXPbvCmdSetPosition, temp );
        }
    }

    mSeekingState = EMPXNotSeeking;
    mController->resetDisappearingTimers( EMPXTimerReset );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::rwReleased()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::rwReleased()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackToolBar::rwReleased()"),
                   _L("mSeekingState = %d"), mSeekingState );

    disconnect( mSeekStartTimer, SIGNAL( timeout() ), this, SLOT( rwPressing() ) );

    if ( mSeekStartTimer->isActive() )
    {
        mSeekStartTimer->stop();
    }

    if ( mSeekingState == EMPXRewinding )
    {
        mController->handleCommand( EMPXPbvCmdEndSeek );        
    }
    else
    {
        int temp = mPosition + KMPXRewind;
        MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::rwReleased() temp position = %d"), temp);

        //
        // If it played less than KMPXRewind, jump to 0
        //
        if ( temp < 0 )
        {
            temp = 0;
        }

        mController->handleCommand( EMPXPbvCmdSetPosition, temp );
    }

    mSeekingState = EMPXNotSeeking;
    mController->resetDisappearingTimers( EMPXTimerReset );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::changeAspectRatio()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::changeAspectRatio()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::changeAspectRatio()"));

    mController->resetDisappearingTimers( EMPXTimerReset );
    
    TMPXVideoPlaybackViewCommandIds cmd = EMPXPbvCmdStretchAspectRatio;

    switch( mAspectRatio )
    {
        case EMMFZoom:
        {
            cmd = EMPXPbvCmdNaturalAspectRatio;
            break;
        }
        case EMMFStretch:
        {
            cmd = EMPXPbvCmdZoomAspectRatio;
            break;
        }
    }

    mController->handleCommand( cmd );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::updateState() state = %d"), state );

    switch ( state )
    {
        case EPbStatePlaying:
        {
            mToolBar->setEnabled( true );

            mButtonActions[EMPX3rdButton]->setIcon( *mButtonIcons[EMPXPauseIcon] );

            break;
        }
        case EPbStatePaused:
        {
            mToolBar->setEnabled( true );

            mButtonActions[EMPX3rdButton]->setIcon( *mButtonIcons[EMPXPlayIcon] );

            break;
        }
        default:
        {
            mToolBar->setEnabled( false );

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::aspectRatioChanged() aspectRatio = %d"), aspectRatio );

    mAspectRatio = aspectRatio;

    if ( ! mController->isAttachOperation() )
    {
        switch( mAspectRatio )
        {
            case EMMFNatural:
            {
                mButtonActions[EMPX1stButton]->setIcon( *mButtonIcons[EMPXStretchIcon] );
                break;
            }
            case EMMFStretch:
            {
                mButtonActions[EMPX1stButton]->setIcon( *mButtonIcons[EMPXZoomIcon] );
                break;
            }
            default:
            {
                mButtonActions[EMPX1stButton]->setIcon( *mButtonIcons[EMPXNaturalIcon] );
                break;
            }
        }    
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::handleButtonPressed()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::handleButtonPressed()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::handleButtonPressed()"));

    mController->resetDisappearingTimers( EMPXTimerCancel );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::handleRWButtonPressed()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::handleRWButtonPressed()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::handleRWButtonPressed()"));

    mController->resetDisappearingTimers( EMPXTimerCancel );

    connect( mSeekStartTimer, SIGNAL( timeout() ), this, SLOT( rwPressing() ) );

    if ( mSeekStartTimer->isActive() )
    {
        mSeekStartTimer->stop();
    }

    mSeekStartTimer->start();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::handleFFButtonPressed()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::handleFFButtonPressed()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::handleButtonPressed()"));

    mController->resetDisappearingTimers( EMPXTimerCancel );

    connect( mSeekStartTimer, SIGNAL( timeout() ), this, SLOT( ffPressing() ) );

    if ( mSeekStartTimer->isActive() )
    {
        mSeekStartTimer->stop();
    }

    mSeekStartTimer->start();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::updateWithFileDetails()"));

    if ( mController->isAttachOperation() )
    {
        //
        // Show attach button
        //
        mButtonActions[EMPX1stButton]->setIcon( *mButtonIcons[EMPXAttachIcon] );

        if ( mButtons.count() )
        {
            disconnect( mButtons[EMPX1stButton], SIGNAL( released() ) );
            connect( mButtons[EMPX1stButton], SIGNAL( released() ),
                     mController, SLOT( attachVideo() ) );
        }
    }
    else
    {
        if ( mController->viewMode() == EFullScreenView )
        {
            //
            // Show aspect ratio button
            //
            aspectRatioChanged( mAspectRatio );

            if ( ! details->mVideoEnabled ||
                   details->mVideoHeight <= 0 ||
                   details->mVideoWidth <= 0 ||
                   details->mTvOutConnected )
            {
                //
                // dim 'aspect ratio' buttons
                //
                mButtonActions[EMPX1stButton]->setEnabled( false );
            }
            else
            {
                //
                // check if video clip has same aspect ratio as display window 
                //
                TReal32 videoAspectRatio = (TReal32) details->mVideoWidth / (TReal32) details->mVideoHeight; 
                bool enabled = ( mDisplayAspectRatio == videoAspectRatio )? false : true;
                
                //
                // enable or dim 'aspect ratio' buttons accordingly
                //
                mButtonActions[EMPX1stButton]->setEnabled( enabled );

                if ( mButtons.count() )
                {
                    disconnect( mButtons[EMPX1stButton], SIGNAL( released() ) );
                    connect( mButtons[EMPX1stButton], SIGNAL( released() ),
                             this, SLOT( changeAspectRatio() ) );
                }
            }
        }                
        else if ( mController->viewMode() == EAudioOnlyView )
        {
            //
            // Show share button
            //
            mButtonActions[EMPX1stButton]->setIcon( *mButtonIcons[EMPXShareIcon] );

            if ( mController->fileDetails()->mPlaybackMode == EMPXVideoStreaming ||
                 mController->fileDetails()->mPlaybackMode == EMPXVideoLiveStreaming )
            {
                //
                // dim 'share' button for streaming
                //
                mButtonActions[EMPX1stButton]->setEnabled( false );
            }
            else
            {
                //
                // enable 'share' button
                //
                mButtonActions[EMPX1stButton]->setEnabled( true );

                if ( mButtons.count() )
                {
                    disconnect( mButtons[EMPX1stButton], SIGNAL( released() ) );
                    connect( mButtons[EMPX1stButton], SIGNAL( released() ), 
                             mController, SLOT( sendVideo() ) );
                }
            }
        }
    }

    //
    // for audio-only clips and tv-out, default view is audionOnlyView
    // therefore, dim details button
    //
    if ( ! details->mVideoEnabled || details->mTvOutConnected )
    {
        mButtonActions[EMPX5thButton]->setEnabled( false );
    }
    else
    {
        mButtonActions[EMPX5thButton]->setEnabled( true );
    }

    if ( ! details->mSeekable )
    {
        mButtonActions[EMPX2ndButton]->setEnabled( false );
        mButtonActions[EMPX4thButton]->setEnabled( false );
    }
    else
    {
        mButtonActions[EMPX2ndButton]->setEnabled( true );
        mButtonActions[EMPX4thButton]->setEnabled( true );
    }

    if ( ! details->mPausableStream )
    {
        mButtonActions[EMPX3rdButton]->setEnabled( false );
    }
    else
    {
        mButtonActions[EMPX3rdButton]->setEnabled( true );
    }        
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::openDetailsView()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::openDetailsView()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackToolBar::openDetailsView()"));

    TPlaybackViewMode viewMode = mController->viewMode();

    if ( viewMode == EFullScreenView )
    {
        mController->changeViewMode( EDetailsView );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::positionChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::positionChanged( int position )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::positionChanged position = %d"), position );

    mPosition = position;
    retrieveButtons();
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
// QMPXVideoPlaybackToolBar::setVisible
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::setVisible( bool visible )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::setVisible visible = %d"), visible );

    if ( visible )
    {
        mController->view()->showItems( Hb::ToolBarItem );

        if ( mNeverVisibled )
        {
            mNeverVisibled = false;


            //
            // toolbar creates button once it gets visible, so we don't know exact timing when toolbar 
            // creates button, so start timer to get layout information once the toolbar gets visible.
            // This is needed since we don't use toolbar in proper way.
            //
            mRetrieveButtonTimer = new QTimer();
            mRetrieveButtonTimer->setSingleShot( false );
            mRetrieveButtonTimer->setInterval( KRetrieveButtonTimeOut );
            connect( mRetrieveButtonTimer, SIGNAL( timeout() ), this, SLOT( retrieveButtons() ) );

            mRetrieveButtonTimer->start();
        }
    }
    else
    {
        mController->view()->hideItems( Hb::ToolBarItem );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackToolBar::retrieveButtons()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackToolBar::retrieveButtons()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackToolBar::retrieveButtons()"));

    if ( mButtons.count() == 0 )
    {
        QGraphicsLayout *layout = mToolBar->layout();

        if ( layout )
        {
		    disconnect( mRetrieveButtonTimer, SIGNAL( timeout() ), this, SLOT( retrieveButtons() ) );
            if ( mRetrieveButtonTimer->isActive() )
            {
                mRetrieveButtonTimer->stop();
            }

            for ( int i = 0 ; i < layout->count() ; i++ )
            {
                mButtons.append( dynamic_cast<HbToolButton*>( layout->itemAt( i ) ) );
            }

            //
            // Connect signal and slots
            //
            connect( mButtons[EMPX1stButton], SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );

            connect( mButtons[EMPX2ndButton], SIGNAL( pressed() ), this, SLOT( handleRWButtonPressed() ) );
            connect( mButtons[EMPX2ndButton], SIGNAL( released() ), this, SLOT( rwReleased() ) );

            connect( mButtons[EMPX3rdButton], SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
            connect( mButtons[EMPX3rdButton], SIGNAL( released() ), this, SLOT( playPause() ) );

            connect( mButtons[EMPX4thButton], SIGNAL( pressed() ), this, SLOT( handleFFButtonPressed() ) );
            connect( mButtons[EMPX4thButton], SIGNAL( released() ), this, SLOT( ffReleased() ) );

            connect( mButtons[EMPX5thButton], SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
            connect( mButtons[EMPX5thButton], SIGNAL( released() ), this, SLOT( openDetailsView() ) );

            if ( mController->isAttachOperation() )
            {
                connect( mButtons[EMPX1stButton], SIGNAL( released() ),
                         mController, SLOT( attachVideo() ) );
            }
            else
            {
                if ( mController->viewMode() == EFullScreenView )
                {
                    connect( mButtons[EMPX1stButton], SIGNAL( released() ),
                             this, SLOT( changeAspectRatio() ) );
                }                
                else if ( mController->viewMode() == EAudioOnlyView )
                {
                    connect( mButtons[EMPX1stButton], SIGNAL( released() ), 
                             mController, SLOT( sendVideo() ) );
                }
            }
        }
    }
}

//End of file
