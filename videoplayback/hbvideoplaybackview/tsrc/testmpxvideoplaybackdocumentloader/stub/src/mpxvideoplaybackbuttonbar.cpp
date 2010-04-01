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
* Description:  Implementation of QMPXVideoPlaybackButtonBar
*
*/

// Version : %version:  1 %


#include "mpxvideo_debug.h"
#include "mpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackcontrolscontroller.h"

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackButtonBar::QMPXVideoPlaybackButtonBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackButtonBar::QMPXVideoPlaybackButtonBar( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
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

//End of file
