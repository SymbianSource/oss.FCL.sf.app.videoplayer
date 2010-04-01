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
* Description:  Implementation of QMPXVideoPlaybackProgressBar
*
*/

// Version : %version:   1 %




#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackdetailsplaybackwindow.h"

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDetailsPlaybackWindow::QMPXVideoPlaybackDetailsPlaybackWindow
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackDetailsPlaybackWindow::QMPXVideoPlaybackDetailsPlaybackWindow( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
    , mInitialized( false )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackDetailsPlaybackWindow::QMPXVideoPlaybackDetailsPlaybackWindow()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDetailsPlaybackWindow::~QMPXVideoPlaybackDetailsPlaybackWindow
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackDetailsPlaybackWindow::~QMPXVideoPlaybackDetailsPlaybackWindow()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackDetailsPlaybackWindow::~QMPXVideoPlaybackDetailsPlaybackWindow()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDetailsPlaybackWindow::initialize
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackDetailsPlaybackWindow::initialize()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackDetailsPlaybackWindow::initialize()"));

}


//End of file
