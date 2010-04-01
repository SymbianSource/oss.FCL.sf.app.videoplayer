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
* Description:  Implementation of QMPXVideoPlaybackFullScreenControl
*
*/

// Version : %version:  1 %



#include <w32std.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackprogressbar.h"
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
    mControl->setVisible( visible );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::isVisible()
{
    return mVisible;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::setVisibility()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::setVisibility( TMPXPlaybackState /*state*/ )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::setVisibility()"));
   
    mVisible = true;
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
// QMPXVideoPlaybackFullScreenControl::volumeChanged()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::volumeChanged( int /*volume*/ )
{
    return true;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::durationChanged()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::durationChanged( int duration )
{
    bool changed = EFalse;

    if ( mControlIndex == EMPXProgressBar )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::DurationChanged() [%d]"), duration);

        static_cast<QMPXVideoPlaybackProgressBar*>(mControl)->durationChanged( duration );

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

    if ( mControlIndex == EMPXProgressBar )
    {
        MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::positionChanged() [%d]"), position);

        static_cast<QMPXVideoPlaybackProgressBar*>(mControl)->positionChanged( position );

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

    if ( mControlIndex == EMPXButtonBar )
    {
        MPX_DEBUG(
            _L("QMPXVideoPlaybackFullScreenControl::aspectRatioChanged() [%d]"), aspectRatio);

        static_cast<QMPXVideoPlaybackButtonBar*>(mControl)->aspectRatioChanged( aspectRatio );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::updateState( TMPXPlaybackState state )
{
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::updateControlsWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::updateControlsWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{

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
