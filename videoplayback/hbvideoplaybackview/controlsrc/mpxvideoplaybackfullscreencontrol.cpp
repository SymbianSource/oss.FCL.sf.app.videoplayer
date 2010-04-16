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
* Description:  Implementation of QMPXVideoPlaybackFullScreenControl
*
*/

// Version : %version: da1mmcf#9 %



#include <w32std.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrolbar.h"
#include "mpxvideoplaybackfullscreencontrol.h"
#include "mpxvideoplaybackfiledetailswidget.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackdetailsplaybackwindow.h"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::QMPXVideoPlaybackFullScreenControl()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackFullScreenControl::QMPXVideoPlaybackFullScreenControl( 
        QMPXVideoPlaybackControlsController* controller, 
        TMPXVideoPlaybackControls index, 
        HbWidget* widget, 
        TUint controlproperties )
    : mController( controller )
    , mControl( widget )
    , mControlIndex( index )
    , mProperties( controlproperties )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFullScreenControl::QMPXVideoPlaybackFullScreenControl()"));

    setParent( mController );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::~QMPXVideoPlaybackFullScreenControl()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackFullScreenControl::~QMPXVideoPlaybackFullScreenControl()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackFullScreenControl"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::setVisible()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::setVisible( bool visible )
{
    switch ( mControlIndex )
    {
        case EMPXControlBar:
        {
            static_cast<QMPXVideoPlaybackControlBar*>(mControl)->setVisibleToControlBar( visible );
            break;
        }
        default:
        {
            mControl->setVisible( visible );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::isVisible()
{
    return mControl->isVisible();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::setVisibility()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::setVisibility( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::setVisibility()"));

    bool visible = false;

    switch( state )
    {
        case EPbStatePlaying:
        {
            if ( mProperties & EMPXShownWhenPlaying )
            {
                visible = true;
            }
            break;
        }
        case EPbStatePaused:
        {
            if ( mProperties & EMPXShownWhenPaused )
            {
                visible = true;
            }
            break;
        }
        case EPbStatePluginSeeking:
        {
            if ( mProperties & EMPXShownWhenSeeking )
            {
                visible = true;
            }
            break;
        }
        case EPbStateBuffering:
        {
            if ( mProperties & EMPXShownWhenBuffering )
            {
                visible = true;
            }
            break;
        }
        case EPbStateNotInitialised:
        case EPbStateInitialising:
        {
            if ( mProperties & EMPXShownWhenInitializing )
            {
                visible = true;
            }
            break;
        }
    }

    setVisible( visible );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::controlIndex()
// -------------------------------------------------------------------------------------------------
//
TMPXVideoPlaybackControls QMPXVideoPlaybackFullScreenControl::controlIndex()
{
    return mControlIndex;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::durationChanged()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::durationChanged( int duration )
{
    bool changed = EFalse;

    if ( mControlIndex == EMPXControlBar )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::DurationChanged() [%d]"), duration);

        static_cast<QMPXVideoPlaybackControlBar*>(mControl)->durationChanged( duration );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::positionChanged()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::positionChanged( int position )
{
    bool changed = EFalse;

    if ( mControlIndex == EMPXControlBar )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::positionChanged() [%d]"), position);

        static_cast<QMPXVideoPlaybackControlBar*>(mControl)->positionChanged( position );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::aspectRatioChanged( int aspectRatio )
{
    bool changed = EFalse;

    if ( mControlIndex == EMPXControlBar )
    {
        MPX_DEBUG(
            _L("QMPXVideoPlaybackFullScreenControl::aspectRatioChanged() [%d]"), aspectRatio);

        static_cast<QMPXVideoPlaybackControlBar*>(mControl)->aspectRatioChanged( aspectRatio );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackFullScreenControl::setDownloadSize()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::setDownloadSize( int /*size*/ )
{
    bool changed = EFalse;

    /*
    if ( mControlIndex == EMPXProgressBar )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::SetDownloadSize() [%d]"), size);

        //static_cast<CMPXVideoPlaybackProgressBar*>(mControl)->SetDownloadSize( size );

        changed = ETrue;
    }
*/
    return changed;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackFullScreenControl::updateDownloadPosition()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::updateDownloadPosition( int /*size*/ )
{
    bool changed = EFalse;

    /*
    if ( mControlIndex == EMPXProgressBar )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::UpdateDownloadPosition() [%d]"), size);

        //static_cast<CMPXVideoPlaybackProgressBar*>(mControl)->updateDownloadPosition( size );

        changed = ETrue;
    }*/

    return changed;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::updateState( TMPXPlaybackState state )
{
    switch ( mControlIndex )
    {
        case EMPXControlBar:
        {
            static_cast<QMPXVideoPlaybackControlBar*>(mControl)->updateState( state );
            break;
        }
        case EMPXDetailsViewPlaybackWindow:
        {
            static_cast<QMPXVideoPlaybackDetailsPlaybackWindow*>(mControl)->updateState( state );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::updateControlsWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::updateControlsWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    switch ( mControlIndex )
    {
        case EMPXControlBar:
        {
            static_cast<QMPXVideoPlaybackControlBar*>(mControl)->updateWithFileDetails( details );
            break;
        }
        case EMPXFileDetailsWidget:
        {
            static_cast<QMPXVideoPlaybackFileDetailsWidget*>(mControl)->updateWithFileDetails( details );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::updateControlProperties()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::updateControlProperties( TUint properties )
{
    mProperties = properties;
}

// End of file
