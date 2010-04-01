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
* Description:  Implementation of QMPXVideoPlaybackFileDetailsWidget
*
*/

// Version : %version:   1 %




#include "mpxvideo_debug.h"
#include "mpxvideoplaybackfiledetailswidget.h"
#include "mpxvideoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::QMPXVideoPlaybackFileDetailsWidget
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackFileDetailsWidget::QMPXVideoPlaybackFileDetailsWidget( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFileDetailsWidget::QMPXVideoPlaybackFileDetailsWidget()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::~QMPXVideoPlaybackFileDetailsWidget
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackFileDetailsWidget::~QMPXVideoPlaybackFileDetailsWidget()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFileDetailsWidget::~QMPXVideoPlaybackFileDetailsWidget()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackFileDetailsWidget::initialize
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackFileDetailsWidget::initialize()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackFileDetailsWidget::initialize"));
}


//End of file
