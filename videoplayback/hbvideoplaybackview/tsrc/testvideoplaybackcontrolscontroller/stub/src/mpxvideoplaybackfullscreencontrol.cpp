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
    //mControl->setVisible( visible );
    mVisible = visible;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::isVisible()
{
    //return mControl->isVisible();
    return mVisible;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::setVisibility()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::setVisibility( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::setVisibility(%d)"), state);
    
    mVisibilityState = state;
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
bool QMPXVideoPlaybackFullScreenControl::volumeChanged( int volume )
{
    mVolume = volume;
    return true;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::durationChanged()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::durationChanged( int duration )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::durationChanged() [%d]"), duration );
    mDuration = duration;    
    return true;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::positionChanged()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::positionChanged( int position )
{   
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::positionChanged() [%d]"), position );
    mPosition = position;    
    return true;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::aspectRatioChanged() [%d]"), aspectRatio );
    mAspectRatio = aspectRatio;
    return true;    
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackFullScreenControl::setDownloadSize()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::setDownloadSize( int size )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::setDownloadSize() [%d]"), size );
    mDownloadSize = size;
    return true;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackFullScreenControl::updateDownloadPosition()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackFullScreenControl::updateDownloadPosition( int size )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::updateDownloadPosition() [%d]"), size );
    mDownloadPosition = size;
    return true;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::updateState() [%d]"), state );
    mState = state; 
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::updateControlsWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::updateControlsWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::updateControlsWithFileDetails()") );
    Q_UNUSED( details );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::updateControlProperties()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::updateControlProperties( TUint properties )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackFullScreenControl::updateControlProperties()") );
    mProperties = properties;
}

// End of file
