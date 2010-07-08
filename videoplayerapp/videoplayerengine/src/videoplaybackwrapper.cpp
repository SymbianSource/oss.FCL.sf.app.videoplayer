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
* Description:  Implementation of VideoPlaybackWrapper
*
*/

// Version : %version:  6 %


#include "mpxvideo_debug.h"
#include "videoplaybackwrapper.h"
#include "mpxvideoplayerappuiengine.h"


// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::VideoPlaybackWrapper()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackWrapper::VideoPlaybackWrapper()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::VideoPlaybackWrapper()"));

    initializePlugins();
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::~VideoPlaybackWrapper()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackWrapper::~VideoPlaybackWrapper()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::~VideoPlaybackWrapper()"));

    if ( mUiEngine ) 
    {
        delete mUiEngine;
        mUiEngine = NULL;
    }        
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::initializePlugins()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackWrapper::initializePlugins()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::initializePlugins()"));

    TRAPD( err, mUiEngine = CMpxVideoPlayerAppUiEngine::NewL( this ) );
    MPX_DEBUG(_L("VideoPlaybackWrapper::initializePlugins err = %d"), err);
    Q_UNUSED(err);
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::playMedia()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::playMedia( QString aFileName )
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::playMedia( %s )"), aFileName.data() );   
    
    TBuf<KMaxFileName> filename( aFileName.utf16() );
    int error = openFileWithNativePath( filename );

    MPX_DEBUG(_L("VideoPlaybackWrapper::playMedia err = %d"), error);

    return error;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::playMedia()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::playMedia( RFile aFile )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::playMedia( aFile )"));   
    
    TRAPD( error, mUiEngine->OpenFileL( aFile ) );

    MPX_DEBUG(_L("VideoPlaybackWrapper::playMedia err = %d"), error);

    return error;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::openPlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackWrapper::openPlaybackView()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::openPlaybackView()"));

    emit handlePlaybackView( MpxHbVideoCommon::ActivatePlaybackView );
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::postInit()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackWrapper::lateInit()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackWrapper::lateInit()"));
    
    TRAP_IGNORE( mUiEngine->LateInitL() );
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackWrapper::openFileWithNativePath()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::openFileWithNativePath(const TDesC& aFileName)
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::openFileWithNativePath()"));    
    
    int err = KErrNone;

    int fwdSlashPos = aFileName.LocateF('/');

    if( fwdSlashPos == KErrNotFound )
    {
        // no fwd slashes found => filepath is already in 
        // preferred format eg. c:\\data\\videos\\test.3gp
        //
        TRAP( err, mUiEngine->OpenFileL( aFileName ) );        
    }
    else
    {        
        HBufC* fileName = NULL;

        TRAP( err,  fileName = aFileName.AllocL() );

        if ( err == KErrNone )
        {
            int count( fileName->Des().Length() );

            for ( int j = fwdSlashPos ; j < count; ++j )
            {
                if ( fileName->Des()[j]== '/' )
                {
                    fileName->Des()[j]='\\';
                }
            }
            
        }

        TRAP( err, mUiEngine->OpenFileL( fileName->Des() ) );
        
        if ( fileName )
        {
            delete fileName;
            fileName = NULL;
        }        
    }
    
    return err;
}


// End of File
