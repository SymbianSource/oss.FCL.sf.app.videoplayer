/*
* Copyright (c) 20010 Nokia Corporation and/or its subsidiary(-ies).
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

// Version : %version:3 %



// INCLUDE FILES


#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"

#include "mpxvideoplaybackcontrolconfiguration.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController()   
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController") );

    initializeController();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::initializeController()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::initializeController()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::initializeController") );

    mLoader = new QMPXVideoPlaybackDocumentLoader();		
    mFileDetails = new QMPXVideoPlaybackViewFileDetails();
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

    if ( mFileDetails )
    {
        delete mFileDetails;
        mFileDetails = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::resetDisappearingTimers()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::resetDisappearingTimers( TMPXTimerAction timerAction )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::resetDisappearingTimers") );

    mTimerAction = timerAction;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::handleCommand()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleCommand( 
        TMPXVideoPlaybackViewCommandIds command, int value )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::handleCommand") );

    mCommand = command;
    mValue = value;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::layoutLoader
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackDocumentLoader* QMPXVideoPlaybackControlsController::layoutLoader()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::layoutLoader") );

    return mLoader;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::viewMode
// -------------------------------------------------------------------------------------------------
//
TPlaybackViewMode QMPXVideoPlaybackControlsController::viewMode()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::viewMode") );

    return mViewMode;
}

// End of File
