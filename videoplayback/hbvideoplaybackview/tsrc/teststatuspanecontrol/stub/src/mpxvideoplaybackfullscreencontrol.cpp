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

// Version : %version:  3 %



#include <w32std.h>
#include <hbwidget.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackfullscreencontrol.h"
#include "mpxvideoplaybackcontrolscontroller.h"


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
// QMPXVideoPlaybackFullScreenControl::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::updateState( TMPXPlaybackState state )
{
    Q_UNUSED( state );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFullScreenControl::updateControlsWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFullScreenControl::updateControlsWithFileDetails(
        QMPXVideoPlaybackViewFileDetails* details )
{
    Q_UNUSED( details );
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
