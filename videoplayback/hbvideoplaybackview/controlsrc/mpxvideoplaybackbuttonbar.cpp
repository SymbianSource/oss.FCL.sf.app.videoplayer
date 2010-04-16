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
* Description:  Implementation of QMPXVideoPlaybackButtonBar
*
*/

// Version : %version: da1mmcf#25 %



#include <hbpushbutton.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::QMPXVideoPlaybackButtonBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackButtonBar::QMPXVideoPlaybackButtonBar( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
    , mSeekingState( EMPXNotSeeking )
    , mInitialized( false )
    , mPosition( 0 )
    , mDuration( 0 )
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

    QMPXVideoPlaybackDocumentLoader *loader = mController->layoutLoader();

    //
    // Don't need to initialize buttons once it gets initialized
    //
    if ( loader && ! mInitialized )
    {
        mInitialized = true;

        //
        // RW button
        //
        QGraphicsWidget *widget = loader->findWidget( QString( "rwButton" ) );
        HbPushButton *rwButton = qobject_cast<HbPushButton*>( widget );
        connect( rwButton, SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
        connect( rwButton, SIGNAL( longPress( QPointF ) ), this, SLOT( rwPressing() ) );
        connect( rwButton, SIGNAL( released() ), this, SLOT( rwReleased() ) ); 

        mButtons.append( rwButton );

        //
        // Play button
        //
        widget = loader->findWidget( QString( "playButton" ) );
        HbPushButton *playButton = qobject_cast<HbPushButton*>( widget );
        connect( playButton, SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
        connect( playButton, SIGNAL( released() ), this, SLOT( play() ) );

        mButtons.append( playButton );

        //
        // Pause button
        //
        widget = loader->findWidget( QString( "pauseButton" ) );
        HbPushButton *pauseButton = qobject_cast<HbPushButton*>( widget );
        connect( pauseButton, SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
        connect( pauseButton, SIGNAL( released() ), this, SLOT( pause() ) );

        mButtons.append( pauseButton );

        //
        // FW button
        //
        widget = loader->findWidget( QString( "ffButton" ) );
        HbPushButton *ffButton = qobject_cast<HbPushButton*>( widget );
        connect( ffButton, SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
        connect( ffButton, SIGNAL( longPress( QPointF ) ), this, SLOT( ffPressing() ) );
        connect( ffButton, SIGNAL( released() ), this, SLOT( ffReleased() ) );
        mButtons.append( ffButton );

        //
        // Aspect ratio button
        //
        widget = loader->findWidget( QString( "naturalButton" ) );
        HbPushButton *naturalButton = qobject_cast<HbPushButton*>( widget );
        connect( naturalButton, SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
        connect( naturalButton, SIGNAL( released() ), this, SLOT( changeAspectRatio() ) ); 
        mButtons.append( naturalButton );

        widget = loader->findWidget( QString( "stretchButton" ) );
        HbPushButton *stretchButton = qobject_cast<HbPushButton*>( widget );
        connect( stretchButton, SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
        connect( stretchButton, SIGNAL( released() ), this, SLOT( changeAspectRatio() ) );
        mButtons.append( stretchButton );

        widget = loader->findWidget( QString( "zoomButton" ) );
        HbPushButton *zoomButton = qobject_cast<HbPushButton*>( widget );
        connect( zoomButton, SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
        connect( zoomButton, SIGNAL( released() ), this, SLOT( changeAspectRatio() ) );
        mButtons.append( zoomButton );

        //
        // Details button
        //
        widget = loader->findWidget( QString( "detailsButton" ) );
        HbPushButton *detailsButton = qobject_cast<HbPushButton*>( widget );
        connect( detailsButton, SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
        connect( detailsButton, SIGNAL( released() ), this, SLOT( openDetailsView() ) );
        mButtons.append( detailsButton );

        //
        // Attach button 
        //
        widget = loader->findWidget( QString( "attachButton" ) );        
        HbPushButton *attachButton = qobject_cast<HbPushButton*>( widget );
        connect( attachButton, SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
        connect( attachButton, SIGNAL( released() ), mController, SLOT( attachVideo() ) ); 
        mButtons.append( attachButton );
            
        for ( int i = 0 ; i < EMPXButtonCount ; i++ )
        {
            mButtons[i]->setFlag( QGraphicsItem::ItemIsFocusable, false );
        }

        mDuration = (qreal)mController->fileDetails()->mDuration / (qreal)KPbMilliMultiplier;

        //
		// obtain layout for aspect ratio which contains all 3 aspect ratio buttons:
        //      natural, stretch, zoom
		//
        widget = loader->findWidget( QString( "aspectRatioButtons" ) );
        
        if ( mController->isAttachOperation() )
        {
            //
            // disable 3 aspect ratio buttons
            //
            widget->setVisible( false );
                
            //
            // enable "attach" button
            //
            mButtons[EMPXButtonAttach]->setVisible( true );        
       }
        else
        {
            //
            // enable 3 aspect ratio buttons
            //
            widget->setVisible( true );
        
            //
            // disable "attach" button
            //
            mButtons[EMPXButtonAttach]->setVisible( false );        
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::play()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::play()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::play()"));

    mController->resetDisappearingTimers( EMPXTimerReset );
    mController->handleCommand( EMPXPbvCmdPlay );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::ffPressing()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::ffPressing()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackButtonBar::ffPressing()"),
                   _L("mSeekingState = %d"), mSeekingState );

    if ( mSeekingState == EMPXNotSeeking )
    {
        mSeekingState = EMPXFastForwarding;
        mButtons[EMPXButtonFF]->setSelected( true );

        mController->handleCommand( EMPXPbvCmdSeekForward );        
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::rwPressing()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::rwPressing()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackButtonBar::rwPressing()"),
                   _L("mSeekingState = %d"), mSeekingState );


    if ( mSeekingState == EMPXNotSeeking )
    {
        mSeekingState = EMPXRewinding;
        mButtons[EMPXButtonRW]->setSelected( true );

        mController->handleCommand( EMPXPbvCmdSeekBackward );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::ffReleased()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::ffReleased()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackButtonBar::ffReleased()"),
                   _L("mSeekingState = %d"), mSeekingState );

    if ( mSeekingState == EMPXFastForwarding )
    {
        mSeekingState = EMPXNotSeeking;
        mController->handleCommand( EMPXPbvCmdEndSeek );        
        mButtons[EMPXButtonFF]->setSelected( false );            
    }
    else
    {
        int temp = mPosition + KMPXFastForward;
        MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::ffReleased() temp position = %d"), temp);

        //
        // If it has the playing time which is less than KMPXFastForward, ignore
        //
        if ( temp < mDuration )
        {
            mController->handleCommand( EMPXPbvCmdSetPosition, temp );
        }
    }

    mController->resetDisappearingTimers( EMPXTimerReset );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::rwReleased()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::rwReleased()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackButtonBar::rwReleased()"),
                   _L("mSeekingState = %d"), mSeekingState );

    if ( mSeekingState == EMPXRewinding )
    {
        mSeekingState = EMPXNotSeeking;
        mController->handleCommand( EMPXPbvCmdEndSeek );        
        mButtons[EMPXButtonRW]->setSelected( false );        
    }
    else
    {
        int temp = mPosition + KMPXRewind;
        MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::rwReleased() temp position = %d"), temp);

        //
        // If it played less than KMPXRewind, jump to 0
        //
        if ( temp < 0 )
        {
            temp = 0;
        }

        mController->handleCommand( EMPXPbvCmdSetPosition, temp );
    }

    mController->resetDisappearingTimers( EMPXTimerReset );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::pause()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::pause()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::pause()"));

    mController->resetDisappearingTimers( EMPXTimerReset );
    mController->handleCommand( EMPXPbvCmdPause );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::changeAspectRatio()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::changeAspectRatio()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::changeAspectRatio()"));

    if ( ! mController->isAttachOperation() )
    {    
        mController->resetDisappearingTimers( EMPXTimerReset );
    
        TMPXVideoPlaybackViewCommandIds cmd = EMPXPbvCmdNaturalAspectRatio;
    
        if ( mButtons[EMPXButtonStretch]->isVisible() )
        {
            cmd = EMPXPbvCmdStretchAspectRatio;
        }
        else if ( mButtons[EMPXButtonZoom]->isVisible() )
        {
            cmd = EMPXPbvCmdZoomAspectRatio;
        }
    
        mController->handleCommand( cmd );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::updateState() state = %d"), state );

    switch ( state )
    {
        case EPbStatePlaying:
        {
            if ( ! isEnabled() )
            {
                setEnabled( true );                
            }

            mButtons[EMPXButtonPlay]->setVisible( false );
            mButtons[EMPXButtonPause]->setVisible( true );
            break;
        }
        case EPbStatePaused:
        {
            if ( ! isEnabled() )
            {
                setEnabled( true );
            }

            mButtons[EMPXButtonPause]->setVisible( false );
            mButtons[EMPXButtonPlay]->setVisible( true );
            break;
        }
        case EPbStateNotInitialised:
        case EPbStateInitialising:
        case EPbStateBuffering:
        {
            if ( isEnabled() )
            {
                setEnabled( false );
            }

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::aspectRatioChanged() aspectRatio = %d"), aspectRatio );

    if ( ! mController->isAttachOperation() )
    {
        switch( aspectRatio )
        {
            case EMMFNatural:
            {
                mButtons[EMPXButtonNatural]->setVisible( false );
                mButtons[EMPXButtonStretch]->setVisible( true );
                mButtons[EMPXButtonZoom]->setVisible( false );
                break;
            }
            case EMMFStretch:
            {
                mButtons[EMPXButtonNatural]->setVisible( false );
                mButtons[EMPXButtonStretch]->setVisible( false );
                mButtons[EMPXButtonZoom]->setVisible( true );
                break;
            }
            default:
            {
                mButtons[EMPXButtonNatural]->setVisible( true );
                mButtons[EMPXButtonStretch]->setVisible( false );
                mButtons[EMPXButtonZoom]->setVisible( false );
                break;
            }
        }    
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::handleButtonPressed()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::handleButtonPressed()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::handleButtonPressed()"));

    mController->resetDisappearingTimers( EMPXTimerCancel );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::updateWithFileDetails()"));

    if ( ! mController->isAttachOperation() )
    {    
        if ( ! details->mVideoEnabled ||
               details->mVideoHeight <= 0 ||
               details->mVideoWidth <= 0 ||
               details->mTvOutConnected )
        {
            mButtons[EMPXButtonNatural]->setEnabled( false );
            mButtons[EMPXButtonStretch]->setEnabled( false );
            mButtons[EMPXButtonZoom]->setEnabled( false );
        }
        else
        {
            mButtons[EMPXButtonNatural]->setEnabled( true );
            mButtons[EMPXButtonStretch]->setEnabled( true );
            mButtons[EMPXButtonZoom]->setEnabled( true );
        }
    }

    //
    // for audio-only clips and tv-out, default view is audionOnlyView
    // therefore, dim details button
    //
    if ( ! details->mVideoEnabled || details->mTvOutConnected )
    {
        mButtons[EMPXButtonDetails]->setEnabled( false );
    }
    else
    {
        mButtons[EMPXButtonDetails]->setEnabled( true );
    }

    if ( ! details->mSeekable || ( details->mTvOutConnected && ! details->mTvOutPlayAllowed ) )
    {
        mButtons[EMPXButtonRW]->setEnabled( false );
        mButtons[EMPXButtonFF]->setEnabled( false );
    }
    else
    {
        mButtons[EMPXButtonRW]->setEnabled( true );
        mButtons[EMPXButtonFF]->setEnabled( true );
    }

    if ( ! details->mPausableStream || ( details->mTvOutConnected && ! details->mTvOutPlayAllowed ) )
    {
        mButtons[EMPXButtonPause]->setEnabled( false );
    }
    else
    {
        mButtons[EMPXButtonPause]->setEnabled( true );
    }

    if ( details->mTvOutConnected && ! details->mTvOutPlayAllowed )
    {
        mButtons[EMPXButtonPlay]->setEnabled( false );
    }
    else
    {
        mButtons[EMPXButtonPlay]->setEnabled( true );
    }

    //
	// If it is audio only view, show the background of button
	//
    bool backgrondVisible = false;

    if ( mController->viewMode() == EAudioOnlyView )
    {
        backgrondVisible = true;
    }

    for ( int i = 0 ; i < EMPXButtonCount ; i++ )
    {
        QGraphicsItem* widget = mButtons[i]->primitive( HbStyle::P_PushButton_background );
        widget->setVisible( backgrondVisible );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::openDetailsView()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::openDetailsView()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackButtonBar::openDetailsView()"));

    TPlaybackViewMode viewMode = mController->viewMode();

    if ( viewMode == EFullScreenView )
    {
        mController->changeViewMode( EDetailsView );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackProgressBar::positionChanged
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackButtonBar::positionChanged( int position )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackButtonBar::positionChanged position = %d"), position );

    mPosition = position;
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

//End of file
