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

// Version : %version:  5 %



#include <hbframeitem.h>
#include <hbframedrawer.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackbuttonbar.h"
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
    , mButtonBar( NULL )
    , mProgressBar( NULL )
    , mFrameItem( NULL )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlBar::QMPXVideoPlaybackControlBar"));

    HbEffect::add( this, ":/hbvideoplaybackview/effects/controlbar_appear.fxml", "appear" );
    HbEffect::add( this, ":/hbvideoplaybackview/effects/controlbar_disappear.fxml", "disappear" );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()"));

    HbEffect::remove( this, ":/hbvideoplaybackview/effects/controlbar_appear.fxml", "appear" );
    HbEffect::remove( this, ":/hbvideoplaybackview/effects/controlbar_disappear.fxml", "disappear" );
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
    if ( mButtonBar == NULL && mProgressBar == NULL )
    {
        //
        // button bar
        //
        QGraphicsWidget *widget = loader->findWidget( QString( "buttonBarLayout" ) );
        mButtonBar = qobject_cast<QMPXVideoPlaybackButtonBar*>( widget );

        if ( mButtonBar )
        {
            mButtonBar->initialize();
        }
        
        //
        // progress bar
        //
        widget = loader->findWidget( QString( "progressBarLayout" ) );
        mProgressBar = qobject_cast<QMPXVideoPlaybackProgressBar*>( widget );

        if ( mProgressBar )
        {
            mProgressBar->initialize();
        }

        //
        // Set framedrawer for semi transparent background
        //
        mFrameItem = new HbFrameItem ( this );
        mFrameItem->setGeometry( boundingRect() );
        mFrameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_multimedia_trans" );
        mFrameItem->frameDrawer().setFrameType( HbFrameDrawer::NinePieces );
        mFrameItem->frameDrawer().setFillWholeRect( true );
        mFrameItem->setVisible( false );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::updateState() state = %d"), state );

    if ( mButtonBar )
    {
        mButtonBar->updateState( state );
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

    if ( mButtonBar )
    {
        mButtonBar->aspectRatioChanged( aspectRatio );
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

    if ( mButtonBar )
    {
        mButtonBar->updateWithFileDetails( details );
    }

    if ( mProgressBar )
    {
        mProgressBar->updateWithFileDetails( details );
    }

    mFrameItem->setVisible( ( mController->viewMode() == EFullScreenView )? ETrue:EFalse ); 
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::setVisibleToControlBar()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::setVisibleToControlBar( bool visible )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlBar::setVisibleToControlBar()"),
                   _L("visible = %d, current visibility = %d"), visible, isVisible() );

    //
    // Change the visibility if the following condition meet:
    // - visible is true
    // - appear effect is not going on
    // - disappear effect is going on (assume current visiblity is false)
    //
    if ( visible && 
         ! HbEffect::effectRunning( this, "appear" ) &&
         ( ! isVisible() || HbEffect::effectRunning( this, "disappear" ) ) )
    {
        //
        // If disappear effect is running on this, cancel
        //
        if ( HbEffect::effectRunning( this, "disappear" ) )
        {
            HbEffect::cancel( this );
        }

        if ( ! isEnabled() )
        {
            setEnabled( true );
        }

        setVisible( true );

        HbEffect::start( this, "appear", this, "appeared" );
    }
    else if ( ! visible && isVisible()&& ! HbEffect::effectRunning( this, "disappear" ) )
    {
        //
        // If appear effect is running on this, cancel
        //
        if( HbEffect::effectRunning( this, "appear" ) )
        {
            HbEffect::cancel( this );
        }

        if ( isEnabled() )
		{
            setEnabled( false );
		}

        HbEffect::start( this, "disappear", this, "disappeared" );
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

    if ( mButtonBar )
    {
        mButtonBar->durationChanged( duration );
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
    
    if ( mButtonBar )
    {
        mButtonBar->positionChanged( position );
    }
}

//End of file
