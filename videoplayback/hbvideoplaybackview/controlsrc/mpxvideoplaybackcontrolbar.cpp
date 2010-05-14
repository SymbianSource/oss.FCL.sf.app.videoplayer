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

// Version : %version:  6 %



#include "mpxvideo_debug.h"
#include "mpxvideoplaybacktoolbar.h"
#include "mpxvideoplaybackcontrolbar.h"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::QMPXVideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlBar::QMPXVideoPlaybackControlBar( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
    , mProgressBar( NULL )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlBar::QMPXVideoPlaybackControlBar"));

    //
    // button bar
    //
    mToolBar = new QMPXVideoPlaybackToolBar( mController );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()"));

    if ( mToolBar )
    {
        delete mToolBar;
        mToolBar = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::initialize()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::initialize()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlBar::initialize()"));

    QMPXVideoPlaybackDocumentLoader *loader = mController->layoutLoader();

    //
    // Don't need to initialize buttons once it gets initialized
    //
    if ( mProgressBar == NULL )
    {
        //
        // progress bar
        //
        QGraphicsWidget *widget = loader->findWidget( QString( "progressBarLayout" ) );
        mProgressBar = qobject_cast<QMPXVideoPlaybackProgressBar*>( widget );

        if ( mProgressBar )
        {
            mProgressBar->initialize();
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::updateState() state = %d"), state );

    if ( mToolBar )
    {
        mToolBar->updateState( state );
    }

    if ( mProgressBar )
    {
        mProgressBar->updateState( state );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::aspectRatioChanged() aspectRatio = %d"), aspectRatio );

    if ( mToolBar )
    {
        mToolBar->aspectRatioChanged( aspectRatio );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::updateWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::updateWithFileDetails()"));

    if ( mToolBar )
    {
        mToolBar->updateWithFileDetails( details );
    }

    if ( mProgressBar )
    {
        mProgressBar->updateWithFileDetails( details );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::setVisibleToControlBar()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::setVisibleToControlBar( bool visible )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlBar::setVisibleToControlBar()"),
                   _L("visible = %d, current visibility = %d"), visible, isVisible() );

    if ( visible != isVisible() )
    {
        setVisible( visible );
        mToolBar->setVisible( visible );        
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::appeared()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::appeared( const HbEffect::EffectStatus &status )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::appeared()"));

    if ( status.reason == Hb::EffectFinished )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::appeared() successful"));
    }
    else
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::appeared() NOT successful"));
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::disappeared()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::disappeared( const HbEffect::EffectStatus &status )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::disappeared()"));

    if ( status.reason == Hb::EffectFinished )
    {
        setVisible( false );

        MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::disappeared() successful"));
    }
    else
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::disappeared() NOT successful"));
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::durationChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::durationChanged()"));

    if ( mProgressBar )
    {
        mProgressBar->durationChanged( duration );
    }

    if ( mToolBar )
    {
        mToolBar->durationChanged( duration );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::positionChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::positionChanged( int position )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::positionChanged()"));

    if ( mProgressBar )
    {
        mProgressBar->positionChanged( position );
    }
    
    if ( mToolBar )
    {
        mToolBar->positionChanged( position );
    }
}

//End of file
