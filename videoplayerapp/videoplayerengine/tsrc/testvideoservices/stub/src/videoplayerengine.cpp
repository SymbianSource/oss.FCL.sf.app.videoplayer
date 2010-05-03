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
* Description:  Implementation of VideoPlayerEngine
*
*/

// Version : %version: da1mmcf#3 %


#include <qapplication>

#include <hbinstance.h>
#include <xqpluginloader.h>
#include <xqplugininfo.h>
#include <xqserviceutil.h>

#include "videoplayerengine.h"
#include "mpxvideoplaybackwrapper.h"
#include "mpxvideo_debug.h"

// -------------------------------------------------------------------------------------------------
// QVideoPlayerEngine()
// -------------------------------------------------------------------------------------------------
//
QVideoPlayerEngine::QVideoPlayerEngine(bool isService)
    : mIsService( isService )
    , mEmbedded( false )
{
    MPX_DEBUG(_L("QVideoPlayerEngine::QVideoPlayerEngine()"));
}

// -------------------------------------------------------------------------------------------------
// ~QVideoPlayerEngine()
// -------------------------------------------------------------------------------------------------
//
QVideoPlayerEngine::~QVideoPlayerEngine()
{
    MPX_DEBUG(_L("QVideoPlayerEngine::~QVideoPlayerEngine()"));
}

// -------------------------------------------------------------------------------------------------
// initialize()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::initialize()
{
    MPX_DEBUG(_L("QVideoPlayerEngine::initialize()"));
}

// -------------------------------------------------------------------------------------------------
// handleCommand()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::handleCommand( int commandCode )
{
    MPX_DEBUG(_L("QVideoPlayerEngine::initialize()"));
    Q_UNUSED( commandCode );
}


// -------------------------------------------------------------------------------------------------
// handleQuit()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::handleQuit()
{
    MPX_DEBUG(_L("QVideoPlayerEngine::handleQuit()"));
    delete this;
}


// -------------------------------------------------------------------------------------------------
// playMedia()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::playMedia( QString filePath )
{
    MPX_DEBUG(_L("QVideoPlayerEngine::playMedia()"));
    Q_UNUSED( filePath );
}

// -------------------------------------------------------------------------------------------------
// setEmbedded()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::setEmbedded()
{
    MPX_DEBUG(_L("QVideoPlayerEngine::setEmbedded()"));
    mEmbedded = true;
}

// -------------------------------------------------------------------------------------------------
// playMedia()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::playMedia( RFile file )
{    
    MPX_DEBUG(_L("QVideoPlayerEngine::playMedia( file )"));
    Q_UNUSED( file );
}

// End of file
