/**
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
* Description:   tester for methods in Video Playback Controls Controller
* 
*/

// Version : %version:  9 %

#include <e32err.h>
#include <w32std.h>
#include <qobject>

#include <hbapplication.h>
#include <hbinstance.h>
#include <hblabel.h>
#include <MMFScalingCustomCommandConstants.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrol.hrh"
#include "mpxcommonvideoplaybackview.hrh"
#include "testmpxvideoplaybackcontrolscontroller.h"
#include "hbvideobaseplaybackview.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoviewwrapper.h"
#include "mpxvideoplaybackfullscreencontrol.h"
#include "mpxvideoplaybackcontrolconfiguration.h"
#include "thumbnailmanager_qt.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "hbvolumesliderpopup.h"
#include "videoservices.h"
#include "xqserviceutilxtra.h"

#define private public
#include "mpxvideoplaybackcontrolscontroller.h"
#undef private

const TInt KControlListsUpdated = 12; // additional extension of TMPXPlaybackState

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestMPXVideoPlaybackControlsController tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestMPXVideoPlaybackControlsController.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::init()
{	  
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlsController::init()"));
    
    mBaseVideoView = new HbVideoBasePlaybackView();

    if ( ! mFileDetails )
    {
        mFileDetails   = new QMPXVideoPlaybackViewFileDetails();        
    }
    mViewWrapper   = CMPXVideoViewWrapper::NewL( mBaseVideoView );	  
    mController    = new QMPXVideoPlaybackControlsController( mBaseVideoView,
                                                              mViewWrapper,
                                                              mFileDetails );       
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::cleanup()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlsController::cleanup()"));
    
    delete mController;
    mController = NULL;

    delete mFileDetails;
    mFileDetails = NULL;

    delete mViewWrapper;
    mViewWrapper = NULL;
    
    delete mBaseVideoView;
    mBaseVideoView = NULL;
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testAddFileDetails
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testAddFileDetails()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testAddFileDetails()") );

    //
    // local real media with no video
    //
    mFileDetails = new QMPXVideoPlaybackViewFileDetails();
    mFileDetails->mClipName = QString( "testClip.rm" );
    mFileDetails->mMimeType = QString( "video/x-pn-realvideo" );

    init();

    mController->addFileDetails( mFileDetails );    
    
    QVERIFY( mController->mFileDetails->mRNFormat == true );
    QVERIFY( mController->mViewMode == EAudioOnlyView );
    QVERIFY( mController->mControlsConfig->mUpdateControlsWithFileDetails == true );
    QVERIFY( mController->mControlsConfig->mState == EMPXControlCmdTvOutConnected );

    cleanup();    

    //
    // local 3GPP media with video-only
    //
    mFileDetails = new QMPXVideoPlaybackViewFileDetails();

    mFileDetails->mClipName = QString("testClip.3gp");
    mFileDetails->mMimeType = QString("video/3gpp");
    mFileDetails->mVideoEnabled = true;
    mFileDetails->mTvOutConnected = true;

    init();

    mController->mViewMode = EFullScreenView;
    QFileInfo videoOnlyFile( mFileDetails->mClipName );

    mController->addFileDetails( mFileDetails );    
    
    QVERIFY( mController->mFileDetails->mRNFormat == false );
    QVERIFY( mController->mControlsConfig->mUpdateControlsWithFileDetails == true ); 
    
    //
    // validate 'TvOutConnected'
    //
    verifyHandleEventTvOutResult(true, true);

    //
    // video-only, validate volume control is dimmed
    //
    QVERIFY( mController->mVolumeControl->mValue == 0 ); 
    QVERIFY( mController->mVolumeControl->isEnabled() == false ); 
    
    //
    // verify 'title' (via mClipName) is set properly
    //
    for ( int i=0 ; i < mController->mLoader->mWidgets.count() ; i++ )
    {        
        if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "title" ) )
        {        
            HbLabel *titleLabel = qobject_cast<HbLabel*>( mController->mLoader->mWidgets[i] );        
            QVERIFY( titleLabel->plainText() == videoOnlyFile.baseName() ); 
            break;
        }
    }    
    cleanup();    

    //
    // non-local audio-video media, TvOut is not connected
    //
    mFileDetails = new QMPXVideoPlaybackViewFileDetails();

    mFileDetails->mClipName = QString("rtsp:://www.youtube.com/testClip.rm");
    mFileDetails->mPlaybackMode = EMPXVideoStreaming;
    mFileDetails->mTvOutConnected = false;
    mFileDetails->mAudioEnabled = true;
    mFileDetails->mTitle = QString("Clip Title");

    init();

    mController->addFileDetails( mFileDetails );    
    
    QVERIFY( mController->mFileDetails->mRNFormat == true );
    QVERIFY( mController->mControlsConfig->mUpdateControlsWithFileDetails == true ); 
    
    //
    // verify 'title' (via mTitle) is set properly
    //
    for ( int i=0 ; i < mController->mLoader->mWidgets.count() ; i++ )
    {        
        if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "title" ) )
        {        
            HbLabel *titleLabel = qobject_cast<HbLabel*>( mController->mLoader->mWidgets[i] );        
            QVERIFY( titleLabel->plainText() == mFileDetails->mTitle ); 
            break;
        }
    }    
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventSetPosition
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventSetPosition()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventSetPosition()"));  
    
    init();    
    int value = 20000;    
    
    mController->handleEvent( EMPXControlCmdSetPosition, value );  

    QVERIFY( mController->mControls[0]->mPosition == ( value / KPbMilliMultiplier ) );
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventSetDuration
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventSetDuration()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventSetDuration()"));  
    
    init();    
    int value = 30000;    
    
    mController->handleEvent( EMPXControlCmdSetDuration, value );  

    QVERIFY( mController->mControls[0]->mDuration == ( value / KPbMilliMultiplier ) );
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventStateChanged
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventStateChanged()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventStateChanged()"));  
    
    init();    
    TMPXVideoPlaybackControlCommandIds event = EMPXControlCmdStateChanged;
    mController->mOrientation = Qt::Horizontal;

    //
    // state change (EPbStateInitialising)
    //
    int value = EPbStateInitialising;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );
        
    //
    // state change (EPbStateBuffering)
    //
    value = EPbStateBuffering;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );
    
    //
    // state change (EPbStatePlaying)
    //
    value = EPbStatePlaying;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );
    
    //
    // state change (EPbStatePaused)
    //
    value = EPbStatePaused;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStateNotInitialised)
    //
    value = EPbStateNotInitialised;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStateStopped)
    //
    value = EPbStateStopped;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStateSeekingForward)
    //
    value = EPbStateSeekingForward;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );
    
    //
    // state change (EPbStateSeekingBackward)
    //
    value = EPbStateSeekingBackward;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );
    
    //
    // state change (EPbStateShuttingDown)
    //
    value = EPbStateShuttingDown;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );
    
    //
    // state change (EPbStateDownloading)
    //
    value = EPbStateDownloading;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );
    
    //
    // state change (EPbStatePluginSeeking)
    //
    value = EPbStatePluginSeeking;    
    mController->handleEvent( event, value );    
    verifyHandleEventStateChangedResult( value );
    
    cleanup();        
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::verifyHandleEventStateChangedResult
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::verifyHandleEventStateChangedResult( int value )
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::verifyHandleEventStateChangedResult()"));  

    mController->mOrientation = Qt::Horizontal;

    if ( value == EPbStateInitialised && 
            ( mController->mFileDetails->mPlaybackMode == EMPXVideoStreaming ||
              mController->mFileDetails->mPlaybackMode == EMPXVideoLiveStreaming ) )
    {
        QVERIFY( mController->mControlsConfig->mState == EMPXControlCmdPluginInitialized );                    
    }
    else if ( value == EPbStatePlaying || value == EPbStateInitialising || 
              value == EPbStateBuffering || value == EPbStatePaused || 
              value == EPbStateNotInitialised )
    {
        for ( int i = 0 ; i < mController->mControls.count() ; i++ )
        {
            QVERIFY( mController->mControls[i]->mState == value );    
            QVERIFY( mController->mControls[i]->mVisibilityState == value );    
        }
    }
    else
    {
        QVERIFY( mController->mState == value );                
    }
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventSetVolume
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventSetVolume()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventSetVolume()"));  
    
    init();    
    int value = 40;    
    
    mController->handleEvent( EMPXControlCmdSetVolume, value );    
    QVERIFY( mController->mVolumeControl->mValue == value );  
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventSetAspectRatio
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventSetAspectRatio()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventSetAspectRatio()"));  
    
    init();    
    int numScalingType = 5;
    
    for ( int i = 0; i < numScalingType; i++ )
    {    
        mController->handleEvent( EMPXControlCmdSetAspectRatio, i );   
    
        QVERIFY( mController->mControls[0]->mAspectRatio == i );  
    }
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventSetDownloadSize
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventSetDownloadSize()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventSetDownloadSize()") );
    
    init();    
    int value = 50;    
    
    mController->handleEvent( EMPXControlCmdSetDownloadSize, value );    
    
    QVERIFY( mController->mControls[0]->mDownloadSize == value );  
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventDownloadUpdated
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventDownloadUpdated()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventDownloadUpdated()") );
    
    init();    
    int value = 55;
    
    mController->handleEvent( EMPXControlCmdDownloadUpdated, value );    

    QVERIFY( mController->mControls[0]->mDownloadPosition == value );  
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventDownloadComplete
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventDownloadComplete()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventDownloadComplete()") );
    
    init();    
    int value = 60;
    
    mController->handleEvent( EMPXControlCmdDownloadComplete, value );    

    QVERIFY( mController->mControls[0]->mDownloadPosition == value );  
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventSetDownloadPaused
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventSetDownloadPaused()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventSetDownloadPaused()") );
    
    init();    
    int value = 0;
    
    mController->handleEvent( EMPXControlCmdSetDownloadPaused, value );    
    
    QVERIFY( mController->mControlsConfig->mState == KControlListsUpdated );                    
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventClearDownloadPaused
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventClearDownloadPaused()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventClearDownloadPaused()") );
    
    init();    
    int value = 0;
    
    mController->handleEvent( EMPXControlCmdClearDownloadPaused, value );    
    
    QVERIFY( mController->mControlsConfig->mState == KControlListsUpdated );                    
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventTvOutConnected
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventTvOutConnected()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventTvOutConnected()") );
    
    init();    
    
    //
    // TV-Out Connected, value = true, EMPXVideoLocal
    //
    mController->handleEvent( EMPXControlCmdTvOutConnected, true );    
    verifyHandleEventTvOutResult( true, true );
        
    //
    // TV-Out Connected, value = false, EMPXVideoLocal
    //
    mController->handleEvent( EMPXControlCmdTvOutConnected, false );    
    verifyHandleEventTvOutResult( true, false );
        
    //
    // TV-Out Connected, value = false, non-EMPXVideoLocal
    //
    mController->mFileDetails->mPlaybackMode = EMPXVideoStreaming;
    mController->handleEvent( EMPXControlCmdTvOutConnected, false );    
    verifyHandleEventTvOutResult( true, false );
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::verifyHandleEventTvOutResult
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::verifyHandleEventTvOutResult( bool tvOutConnected,
        bool value )
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::verifyHandleEventTvOutResult(%d,%d)"),
            tvOutConnected, value ); 
    
    QVERIFY( mController->mFileDetails->mTvOutConnected == tvOutConnected );   
    
    //
    // validate setDefaultBitmap()
    //
    for ( int i=0 ; i < mController->mLoader->mWidgets.count() ; i++ )
    {        
        bool val = mController->mLoader->isVisible(i);
        
        if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "tvOutBitmap" ) )
        {
            QVERIFY( tvOutConnected? val == true : val == false ); 
        }
        else if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "realAudioOnlyBitmap" )  )
        {
            QVERIFY( mFileDetails->mRNFormat? val == true : val == false ); 
        }
        else if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "partialAudioOnlyBitmap" )  )
        {
            QVERIFY( mFileDetails->mPartialPlayback? val == true : val == false ); 
        }
        else if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "audioOnlyBitmap" )  )
        {
            QVERIFY( tvOutConnected? val == false : val == true ); 
        }
    }
        
    //
    // validate generateThumbNail()
    //
    if ( tvOutConnected )
    {
        QVERIFY( ( mController->mFileDetails->mPlaybackMode == EMPXVideoLocal )? 
                   mController->mThumbNailState == EThumbNailRequsted : 
                   mController->mThumbNailState == EThumbNailNotAvailable ); 
        QVERIFY( mController->mViewMode == EAudioOnlyView );          
        QVERIFY( mController->mControlsConfig->mState == EMPXControlCmdTvOutConnected );          
        QVERIFY( mController->mThumbnailManager->mThumbSize == ThumbnailManager::ThumbnailLarge );          
        QVERIFY( mController->mThumbnailManager->mQuality == ThumbnailManager::OptimizeForPerformance );          
        QVERIFY( mController->mViewTransitionIsGoingOn == false );          
    }
    else if ( mController->mFileDetails->mVideoEnabled )
    {
        QVERIFY( mController->mViewTransitionIsGoingOn == true );          
        QVERIFY( mController->mViewMode == EFullScreenView );          
    }
    else if ( ! mController->mFileDetails->mVideoEnabled )
    {
        QVERIFY( mController->mViewTransitionIsGoingOn == false );          
        QVERIFY( mController->mViewMode == EAudioOnlyView );          
    }

}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventTvOutDisconnected
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventTvOutDisconnected()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventTvOutDisconnected()") );
    
    init();    
    
    //
    // TV-Out Disconnected, value = true
    //
    mController->handleEvent( EMPXControlCmdTvOutDisconnected, true );    
    verifyHandleEventTvOutResult( false, true );
        
    //
    // TV-Out Disconnected, value = false
    //
    mController->handleEvent( EMPXControlCmdTvOutDisconnected, false );    
    verifyHandleEventTvOutResult( false, false );
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventHandleErrors
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventHandleErrors()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventHandleErrors()") );
    
    init();    
    
    //
    // handle error
    //
    mController->handleEvent( EMPXControlCmdHandleErrors, 0 );    
    
    QVERIFY( mController->mThumbNailState == EThumbNailEmpty );          
    QVERIFY( mController->mState == EPbStateNotInitialised );      
    QVERIFY( mController->mViewMode == EFullScreenView );          
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleEventShowVolumeControls
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleEventShowVolumeControls()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleEventShowVolumeControls()") );
    
    init();    
    
    mController->handleEvent( EMPXControlCmdShowVolumeControls, 0 );    
    
    QVERIFY( mController->mVolumeControl->mVisible == true );          
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleCommand
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleCommand()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleCommand()") );
    
    init();    
    
    mController->handleCommand( EMPXPbvCmdSetPosition, 0 );    
    QVERIFY( mViewWrapper->mProperty == EPbPropertyPosition );          
    
    mController->handleCommand( EMPXPbvCmdSetVolume, 0 );    
    QVERIFY( mViewWrapper->mProperty == EPbPropertyVolume );          
    
    mController->handleCommand( EMPXPbvCmdPlay, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPlay );          
    
    mController->handleCommand( EMPXPbvCmdPause, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPause );          
    
    mController->handleCommand( EMPXPbvCmdPlayPause, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPlayPause );          
    
    mController->handleCommand( EMPXPbvCmdStop, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdStop );          
    
    mController->handleCommand( EMPXPbvCmdClose, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdClose );          
    
    mController->handleCommand( EMPXPbvCmdMute, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdMute );          
    
    mController->handleCommand( EMPXPbvCmdUnMute, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdUnMute );          
    
    mController->handleCommand( EMPXPbvCmdChangeAspectRatio, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdChangeAspectRatio );          
    
    mController->handleCommand( EMPXPbvCmdNaturalAspectRatio, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdNaturalAspectRatio );          
    
    mController->handleCommand( EMPXPbvCmdZoomAspectRatio, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdZoomAspectRatio );          
    
    mController->handleCommand( EMPXPbvCmdStretchAspectRatio, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdStretchAspectRatio );          
    
    mController->handleCommand( EMPXPbvCmdSeekForward, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdSeekForward );          
    
    mController->handleCommand( EMPXPbvCmdSeekBackward, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdSeekBackward );          
    
    mController->handleCommand( EMPXPbvCmdEndSeek, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdEndSeek );          
    
    mController->handleCommand( EMPXPbvCmdNextListItem, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdNextListItem );          
    
    mController->handleCommand( EMPXPbvCmdPreviousListItem, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPreviousListItem );          
    
    mController->handleCommand( EMPXPbvCmdDecreaseVolume, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdDecreaseVolume );          
    
    mController->handleCommand( EMPXPbvCmdIncreaseVolume, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdIncreaseVolume );          
    
    mController->handleCommand( EMPXPbvCmdPosition, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPosition );          
    
    mController->handleCommand( EMPXPbvCmdSave, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdSave );          
    
    mController->handleCommand( EMPXPbvCmdResetControls, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdResetControls );          
    
    mController->handleCommand( EMPXPbvCmdShortPressForward, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdShortPressForward );          
    
    mController->handleCommand( EMPXPbvCmdShortPressBackward, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdShortPressBackward );          
    
    mController->handleCommand( EMPXPbvCmdShowFileDetails, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdShowFileDetails );          
    
    mController->handleCommand( EMPXPbvCmdEndOfClip, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdEndOfClip );          
    
    mController->handleCommand( EMPXPbvCmdCustomPause, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdCustomPause );          
    
    mController->handleCommand( EMPXPbvCmdCustomPlay, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdCustomPlay );          
    
    mController->handleCommand( EMPXPbvCmdExit, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdExit );          
    
    mController->handleCommand( EMPXPbvLaunchDRMDetails, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvLaunchDRMDetails );          
    
    mController->handleCommand( EMPXPbvSurfaceCreated, 0 );    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvSurfaceCreated );          
        
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testHandleTappedOnScreen
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testHandleTappedOnScreen()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testHandleTappedOnScreen()") );
    
    init();    
    mController->mOrientation = Qt::Horizontal;

    //
    // preset variables
    //
    mFileDetails->mVideoEnabled = true;
    mController->mViewMode = EFullScreenView;
    
    //
    // playing state
    //
    mController->mState = EPbStatePlaying;
    mBaseVideoView->mouseReleaseEvent(0);
    QVERIFY( mController->mControlsTimer->isActive() == true );
    for ( int i = 0 ; i < mController->mControls.count() ; i++ )
    {
        QVERIFY( mController->mControls[i]->mVisibilityState == mController->mState );
    }
    
    //
    // pause state
    //
    mController->mState = EPbStatePaused;
    mBaseVideoView->mouseReleaseEvent(0);
    QVERIFY( mController->mControlsTimer->isActive() == false );
    for ( int i = 0 ; i < mController->mControls.count() ; i++ )
    {
        QVERIFY( mController->mControls[i]->mVisibilityState == mController->mState );    
    }
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testUpdateVideoRectDone
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testUpdateVideoRectDone()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testUpdateVideoRectDone()") );
    
    init();    
    
    mController->updateVideoRectDone();    
    
    //
    // default is full-screen
    //
    QVERIFY( mController->mControlsConfig->mState == EMPXControlCmdFullScreenViewOpened );                    
    
    //
    // validate Details view
    //
    mController->mViewMode = EDetailsView;
    
    mController->updateVideoRectDone(); 
    
    QVERIFY( mController->mControlsConfig->mState == EMPXControlCmdDetailsViewOpened );                    
            
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testIsAttachOperation
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testIsAttachOperation()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testIsAttachOperation()") );
    
    //
    // get access to XQServiceUtilXtra stub 
    //
    XQServiceUtilXtra* extraUtil = XQServiceUtilXtra::instance();

    //
    // set service enable
    //
    extraUtil->setCurrentService( true );
        
    //
    // initialize controlscontroller
    //
    init();    
             
    //
    // verify 'mIsAttachOperation' flag is enabled
    //
    QVERIFY( mController->mIsAttachOperation == true ); 
        
    //
    // clean up
    //
    cleanup();    
    
    //
    // set service disable
    //
    extraUtil->setCurrentService( false );
        
    //
    // initialize controlscontroller
    //
    init();    
             
    //
    // verify 'mIsAttachOperation' flag is disabled
    //
    QVERIFY( mController->mIsAttachOperation == false ); 
            
    //
    // clean up
    //
    cleanup();    
    
    //
    // dereference extraUtil count
    //
    extraUtil->decreaseReferenceCount();
    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testslot_skipToNextVideoItem
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testslot_skipToNextVideoItem()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testslot_skipToNextVideoItem()") );

    init();    
    
    connect( this, SIGNAL( commandSignal() ), mController, SLOT( skipToNextVideoItem() ) );
    
    emit commandSignal();     
    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdNextListItem ); 
    
    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( skipToNextVideoItem() ) );
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testslot_skipToPreviousVideoItem
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testslot_skipToPreviousVideoItem()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testslot_skipToPreviousVideoItem()") );

    init();    
    
    connect( this, SIGNAL( commandSignal() ), mController, SLOT( skipToPreviousVideoItem() ) );
    
    emit commandSignal();     
    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPreviousListItem ); 
    
    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( skipToPreviousVideoItem() ) );
    
    cleanup();    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testslot_attachVideo
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testslot_attachVideo()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testslot_attachVideo()") );

    //
    // get access to XQServiceUtilXtra stub to enable XQServiceUtil::isService() method 
    //
    XQServiceUtilXtra* extraUtil = XQServiceUtilXtra::instance();

    //
    // set service enable
    //
    extraUtil->setCurrentService( true );
        
    //
    // initialize controlscontroller
    //
    init();    
        
    //
    // verify view closePlaybackView() slot is not yet called
    //
    QVERIFY( mController->mView->mViewClosed == false ); 

    //
    // connect signal with controller attachVideo() slot
    //
    bool res = connect( this, SIGNAL( commandSignal() ), mController, SLOT( attachVideo() ) );
    
    //
    // emit signal, this will in turns invoke videoservices itemSelected() slot
    //
    emit commandSignal();     
    
    //
    // verify view closePlaybackView() slot is called
    //
    QVERIFY( mController->mView->mViewClosed == true ); 

    //
    // verify videoservices itemSelected() slot is called
    //
    QVERIFY( mController->mVideoServices->mItemSelected == true ); 
    
    //
    // disconnect signal
    //
    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( attachVideo() ) );
    
    //
    // clean up
    //
    cleanup();  
    
    //
    // dereference extraUtil count
    //
    extraUtil->decreaseReferenceCount();
    
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testslot_sendVideo
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testslot_sendVideo()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testslot_sendVideo()") );
        
    //
    // initialize controlscontroller
    //
    init();    
        
    //
    // connect signal with controller sendVideoo() slot
    //
    bool res = connect( this, SIGNAL( commandSignal() ), mController, SLOT( sendVideo() ) );
    
    //
    // emit signal, this will in turns invoke mController sendVideo() slot
    //
    emit commandSignal();     
    
    //
    // verify command EMPXPbvCmdClose has been issued
    //
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPause ); 
        
    //
    // disconnect signal
    //
    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( sendVideo() ) );
    
    //
    // clean up
    //
    cleanup();      
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testslot_handleOrientationChanged
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testslot_handleOrientationChanged()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testslot_handleOrientationChanged()") );
        
    //
    // initialize controlscontroller
    //
    init();    
        
    //
    // connect signal with controller handleOrientationChanged() slot
    //
    bool res = connect( this, SIGNAL( commandSignal( Qt::Orientation ) ),
                        mController, SLOT( handleOrientationChanged( Qt::Orientation ) ) );
    
    //
    // emit signal, this will in turns invoke mController handleOrientationChanged() slot
    //
    mController->mOrientation = Qt::Vertical;
    mController->mState = EPbStatePlaying;                

    emit commandSignal( Qt::Horizontal );     

    QVERIFY( mController->mOrientation == Qt::Horizontal ); 

    for ( int i = 0 ; i < mController->mControls.count() ; i++ )
    {
        QVERIFY( mController->mControls[i]->mVisibilityState == mController->mState );    
    }

    //
    // disconnect signal
    //
    disconnect( this, SIGNAL( commandSignal( Qt::Orientation ) ), 
                mController, SLOT( handleOrientationChanged( Qt::Orientation ) ) );
    
    //
    // clean up
    //
    cleanup();      
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testslot_handleRNLogoVisibleChanged
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testslot_handleRNLogoVisibleChanged()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testslot_handleRNLogoVisibleChanged()") );
        
    //
    // initialize controlscontroller
    //
    init();    
        
    //
    // connect signal with controller handleRNLogoVisibleChanged() slot
    //
    bool res = connect( this, SIGNAL( commandSignal() ),
                        mController, SLOT( handleRNLogoVisibleChanged() ) );
    
    //
    // emit signal, this will in turns invoke mController handleRNLogoVisibleChanged() slot
    //
    emit commandSignal();     

    QVERIFY( mController->mRNLogoTimer->isActive() ); 
    QVERIFY( mController->mRNLogoTimer->interval() == KMPXRNLogoTimeOut ); 

    //
    // disconnect signal
    //
    disconnect( this, SIGNAL( commandSignal() ), 
                mController, SLOT( handleRNLogoVisibleChanged() ) );
    
    //
    // clean up
    //
    cleanup();      
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testslot_handleRNLogoTimeout
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testslot_handleRNLogoTimeout()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testslot_handleRNLogoTimeout()") );
        
    //
    // initialize controlscontroller
    //
    init();    

    if ( ! mController->mRNLogoTimer )
    {
        mController->mRNLogoTimer = new QTimer( mController );
    }

    //
    // connect signal with controller handleRNLogoTimeout() slot
    //
    bool res = connect( this, SIGNAL( commandSignal() ),
                        mController, SLOT( handleRNLogoTimeout() ) );
    
    //
    // emit signal, this will in turns invoke mController handleRNLogoTimeout() slot
    //
    emit commandSignal();     

    QVERIFY( ! mController->mRNLogoTimer->isActive() ); 
    QVERIFY( mController->mControlsConfig->mState == KControlListsUpdated );                    
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdRealOneBitmapTimeout ); 

    //
    // disconnect signal
    //
    disconnect( this, SIGNAL( commandSignal() ), 
                mController, SLOT( handleRNLogoTimeout() ) );
    
    //
    // clean up
    //
    cleanup();      
}

// -------------------------------------------------------------------------------------------------
// TestMPXVideoPlaybackControlsController::testIsRNLogoBitmapVisible
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlsController::testIsRNLogoBitmapVisible()
{
    MPX_DEBUG(_L("TestMPXVideoPlaybackControlsController::testIsRNLogoBitmapVisible()"));  

    init();

    QVERIFY( mController->isRNLogoBitmapInControlList() );

    //
    // Remove RN logo from the list
    //
    for ( int i = 0 ; i < mController->mControls.count() ; i++ )
    {
        if( mController->mControls[i]->mControlIndex == EMPXRealLogoBitmap )
        {
            mController->mControls.removeAt( i );
        }
    }

    QVERIFY( ! mController->isRNLogoBitmapInControlList() );

    cleanup();    
}

// End of file
