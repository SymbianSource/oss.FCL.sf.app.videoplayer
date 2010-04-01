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
* Description:  Implementation of QMPXVideoPlaybackNonTouchVolumeBar
*
*/

// Version : %version:  7 %



#include <QTimer>
#include <QGraphicsSceneMouseEvent>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybacknontouchvolumebar.h"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackNonTouchVolumeBar::QMPXVideoPlaybackNonTouchVolumeBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackNonTouchVolumeBar::QMPXVideoPlaybackNonTouchVolumeBar()
    : HbVolumeSliderPopup()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackNonTouchVolumeBar::QMPXVideoPlaybackNonTouchVolumeBar()"));

    mVolumeTimer = new QTimer( this );
    mVolumeTimer->setInterval( KMPXControlsTimeOut );
    mVolumeTimer->setSingleShot( false );
    connect( mVolumeTimer, SIGNAL( timeout() ), this, SLOT( hideVolumeControl() ) );

    setTickPosition( Hb::NoSliderTicks );

    setRange( KPbPlaybackVolumeLevelMin, KPbPlaybackVolumeLevelMax );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackNonTouchVolumeBar::~QMPXVideoPlaybackNonTouchVolumeBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackNonTouchVolumeBar::~QMPXVideoPlaybackNonTouchVolumeBar()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackNonTouchVolumeBar::QMPXVideoPlaybackNonTouchVolumeBar"));

    disconnect( mVolumeTimer, SIGNAL( timeout() ), this, SLOT( hideVolumeControl() ) );

    if ( mVolumeTimer )
    {
        if ( mVolumeTimer->isActive() )
        {
            mVolumeTimer->stop();
        }

        delete mVolumeTimer;
        mVolumeTimer = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackNonTouchVolumeBar::setVisible()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackNonTouchVolumeBar::setVisible( bool visible )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackNonTouchVolumeBar::setVisible"));

    if ( mVolumeTimer->isActive() )
    {
        mVolumeTimer->stop();
    }

    if ( visible )
    {
        mVolumeTimer->start();
    }

    HbVolumeSliderPopup::setVisible( visible );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackNonTouchVolumeBar::hideVolumeControl()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackNonTouchVolumeBar::hideVolumeControl()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackNonTouchVolumeBar::hideVolumeControl"));

    setVisible( false );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackNonTouchVolumeBar::mousePressEvent()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackNonTouchVolumeBar::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    event->ignore();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackNonTouchVolumeBar::mouseReleaseEvent()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackNonTouchVolumeBar::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    event->ignore();
}

// End of file
