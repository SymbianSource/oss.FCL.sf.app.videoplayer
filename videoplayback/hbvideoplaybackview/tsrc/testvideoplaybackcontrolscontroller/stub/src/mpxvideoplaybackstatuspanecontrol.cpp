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
* Description:  Implementation of QMPXVideoPlaybackStatusPaneControl
*
*/

// Version : %version:  1 %


#include "mpxvideo_debug.h"
#include "mpxvideoplaybackstatuspanecontrol.h"
#include "mpxvideoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::QMPXVideoPlaybackStatusPaneControl()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackStatusPaneControl::QMPXVideoPlaybackStatusPaneControl( 
        QMPXVideoPlaybackControlsController* controller, 
        TMPXVideoPlaybackControls index, 
        HbWidget* widget, 
        TUint controlproperties )
    : QMPXVideoPlaybackFullScreenControl( controller, index, widget, controlproperties )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackStatusPaneControl::QMPXVideoPlaybackStatusPaneControl()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackStatusPaneControl::~QMPXVideoPlaybackStatusPaneControl()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackStatusPaneControl::~QMPXVideoPlaybackStatusPaneControl()
{
}


// End of file
