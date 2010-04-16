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

// Version : %version:  1 %



#include <hbframeitem.h>
#include <hbframedrawer.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrolbar.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::QMPXVideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlBar::QMPXVideoPlaybackControlBar( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlBar::QMPXVideoPlaybackControlBar"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlBar::~QMPXVideoPlaybackControlBar()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlBar::initialize()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlBar::initialize()
{
}



//End of file
