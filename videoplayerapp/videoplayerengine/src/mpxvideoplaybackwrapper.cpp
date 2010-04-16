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
* Description:  Implimentation of QMpxVideoPlaybackWrapper
*
*/

// Version : %version:  4 %


#include "mpxvideo_debug.h"
#include "mpxvideoplaybackwrapper.h"
#include "mpxvideoplayerappuiengine.h"


// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::QMpxVideoPlaybackWrapper()
// -------------------------------------------------------------------------------------------------
//
QMpxVideoPlaybackWrapper::QMpxVideoPlaybackWrapper()
{
    MPX_ENTER_EXIT(_L("QMpxVideoPlaybackWrapper::QMpxVideoPlaybackWrapper()"));

    initializePlugins();
}

// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::~QMpxVideoPlaybackWrapper()
// -------------------------------------------------------------------------------------------------
//
QMpxVideoPlaybackWrapper::~QMpxVideoPlaybackWrapper()
{
    MPX_ENTER_EXIT(_L("QMpxVideoPlaybackWrapper::~QMpxVideoPlaybackWrapper()"));

    if ( mUiEngine ) 
    {
        delete mUiEngine;
        mUiEngine = NULL;
    }        
}

// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::initializePlugins()
// -------------------------------------------------------------------------------------------------
//
void QMpxVideoPlaybackWrapper::initializePlugins()
{
    MPX_ENTER_EXIT(_L("QMpxVideoPlaybackWrapper::initializePlugins()"));

    TRAPD( err, mUiEngine = CMpxVideoPlayerAppUiEngine::NewL( this ) );
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::initializePlugins err = %d"), err);
}

// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::playMedia()
// -------------------------------------------------------------------------------------------------
//
int QMpxVideoPlaybackWrapper::playMedia( QString aFileName )
{
    MPX_ENTER_EXIT(_L("QMpxVideoPlaybackWrapper::playMedia"));   
    
    TBuf<KMaxFileName> filename( aFileName.utf16() );
    int error = openFileWithNativePath( filename );

    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::playMedia err = %d"), error);

    return error;
}

// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::playMedia()
// -------------------------------------------------------------------------------------------------
//
int QMpxVideoPlaybackWrapper::playMedia( RFile aFile )
{
    MPX_ENTER_EXIT(_L("QMpxVideoPlaybackWrapper::playMedia"));   
    
    TRAPD( error, mUiEngine->OpenFileL( aFile ) );

    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::playMedia err = %d"), error);

    return error;
}

// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::openPlaybackView()
// -------------------------------------------------------------------------------------------------
//
void QMpxVideoPlaybackWrapper::openPlaybackView()
{
    MPX_ENTER_EXIT(_L("QMpxVideoPlaybackWrapper::openPlaybackView()"));

    emit handlePlaybackView( MpxHbVideoCommon::ActivatePlaybackView );
}

// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::postInit()
// -------------------------------------------------------------------------------------------------
//
void QMpxVideoPlaybackWrapper::lateInit()
{
    MPX_ENTER_EXIT(_L("QMpxVideoPlaybackWrapper::postInit()"));
    
    TRAP_IGNORE( mUiEngine->LateInitL() );
}

// -------------------------------------------------------------------------------------------------
//   QMpxVideoPlaybackWrapper::openFileWithNativePath()
// -------------------------------------------------------------------------------------------------
//
int QMpxVideoPlaybackWrapper::openFileWithNativePath(const TDesC& aFileName)
{
    MPX_DEBUG(_L("QMpxVideoPlaybackWrapper::openFileWithNativePath()"));    
    
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
