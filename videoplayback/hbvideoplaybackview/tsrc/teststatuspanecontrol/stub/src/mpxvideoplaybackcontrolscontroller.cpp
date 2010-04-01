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
* Description:  Implementation of MPXVideoPlaybackControlsController
*
*/

// Version : %version:  3 %



// INCLUDE FILES
#include <coecntrl.h>
#include <bautils.h>
#include <barsread.h>
#include <f32file.h>

#include "../inc/hbvideobaseplaybackview.h"
#include "../inc/mpxvideoplaybackviewfiledetails.h"
#include "../inc/mpxvideoplaybackcontrolscontroller.h"
#include "../inc/mpxvideoplaybackdocumentloader.h"



// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController(
        HbVideoBasePlaybackView *view,
        QMPXVideoPlaybackViewFileDetails *details )
    : mView( view )
    , mFileDetails( details )   
{
    initializeController();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::initializeController()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::initializeController()
{
    mLoader = new QMPXVideoPlaybackDocumentLoader();		
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController"));

    if ( mLoader )
    {
        delete mLoader;
        mLoader = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::resetDisappearingTimers()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::resetDisappearingTimers( TMPXTimerAction timerAction )
{
    mTimerAction = timerAction;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::changeViewMode
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::changeViewMode( 
        TPlaybackViewMode viewMode, bool transitionEffect )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::changeViewMode()"));
    
    Q_UNUSED( transitionEffect );
    mViewMode = viewMode;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::layoutLoader
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackDocumentLoader* QMPXVideoPlaybackControlsController::layoutLoader()
{
    return mLoader;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::viewMode
// -------------------------------------------------------------------------------------------------
//
TPlaybackViewMode QMPXVideoPlaybackControlsController::viewMode()
{
    return mViewMode;
}


// End of File
