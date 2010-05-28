/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Version : %version: 8 %



// INCLUDE FILES

#include "mpxvideoviewwrapper.h"
#include "hbvideobaseplaybackview.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "../inc/mpxvideoplaybackcontrolscontroller.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController(
        HbVideoBasePlaybackView *view,
        CMPXVideoViewWrapper *viewWrapper,
        QMPXVideoPlaybackViewFileDetails *details )
    : mView( view )
    , mViewWrapper( viewWrapper )
    , mFileDetails( details ) 
    , mFileDetailsAdded( false )
{
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::addFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::addFileDetails(
    QMPXVideoPlaybackViewFileDetails* details )
{
    Q_UNUSED( details );

    mFileDetailsAdded = true;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::handleEvent
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleEvent(
    TMPXVideoPlaybackControlCommandIds event, int value )
{
    mReceivedEvent = event;
    mValue = value;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::updateVideoRectDone
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::updateVideoRectDone()
{
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::isRNLogoBitmapVisible()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::isRNLogoBitmapInControlList()
{
    return mRNLogoVisible;
}

// End of File
