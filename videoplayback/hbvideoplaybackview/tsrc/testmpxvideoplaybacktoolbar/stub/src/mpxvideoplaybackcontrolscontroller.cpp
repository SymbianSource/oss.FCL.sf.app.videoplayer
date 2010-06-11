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

// Version : %version: 1 %



// INCLUDE FILES

#include "hbvideobaseplaybackview.h"
#include "mpxvideoplaybackcontrolscontroller.h"

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController( bool attachOperation )
    : mViewMode( EFullScreenView )
    , mIsAttachOperation( attachOperation )
    , mAttachVideoDone( false )
    , mSendVideoDone( false )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController()"));

    mFileDetails = new QMPXVideoPlaybackViewFileDetails();
    mView = new HbVideoBasePlaybackView();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController"));

    if ( mFileDetails )
    {
        delete mFileDetails;
        mFileDetails = NULL;
    }

    if ( mView )
    {
        delete mView;
        mView = NULL;       
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::handleCommand()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleCommand( 
        TMPXVideoPlaybackViewCommandIds command, int value )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::handleCommand(%d)"), command);

    mCommand = command;
    mCommandValue = value;
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
// QMPXVideoPlaybackControlsController::resetDisappearingTimers()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::resetDisappearingTimers( TMPXTimerAction timerAction )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::resetDisappearingTimers()"));

    mTimerAction = timerAction;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::isAttachOperation()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::isAttachOperation()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::isAttachOperation() ret %d"), 
            mIsAttachOperation);

    return mIsAttachOperation;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::attachVideo()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::attachVideo()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::attachVideo()"));
    
    handleCommand( EMPXPbvCmdClose );    
    mAttachVideoDone = true;
}    

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::sendVideo()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::sendVideo()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::sendVideo()"));
    
    handleCommand( EMPXPbvCmdPause );    
    mSendVideoDone = true;
}    

// End of File