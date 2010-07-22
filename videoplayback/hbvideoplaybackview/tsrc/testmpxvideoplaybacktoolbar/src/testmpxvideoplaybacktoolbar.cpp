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
* Description:   tester for methods in TestMPXVideoPlaybackToolBar
* 
*/

// Version : %version:  4 %


#include <qdebug>
#include <hbmainwindow.h>
#include <hbapplication.h>

#include "hbtoolbar.h"
#include "hbtoolbutton.h"
#include "testmpxvideoplaybacktoolbar.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackdocumentloader.h"

#define private public
#include "mpxvideoplaybacktoolbar.h"
#undef private

// -------------------------------------------------------------------------------------------------
// main
// -------------------------------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMPXVideoPlaybackToolBar tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testmpxvideoplaybacktoolbar.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}

// -------------------------------------------------------------------------------------------------
// init
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::init( bool attachOperation )
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::init()"));

    mController = new QMPXVideoPlaybackControlsController( attachOperation );   
    mVideoToolBar = new QMPXVideoPlaybackToolBar( mController );

    mVideoToolBar->setVisible( true );

    connect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( retrieveButtons() ) );
    emit commandSignal();
    disconnect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( retrieveButtons() ) );
}

// -------------------------------------------------------------------------------------------------
// cleanup
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::cleanup()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mVideoToolBar )
    {
        delete mVideoToolBar;
        mVideoToolBar = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// testPlay
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testPlay()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testPlay()"));

    init();

    mVideoToolBar->mButtons[EMPX3rdButton]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdPlayPause );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testLongTapOnFF
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testLongTapOnFF()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testLongTapOnFF()"));

    init();

    //
    // Start seeking
    //
    mVideoToolBar->mButtons[EMPX4thButton]->press();
    QVERIFY( mController->mTimerAction == EMPXTimerCancel );

    connect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( ffPressing() ) );
    emit commandSignal();
    disconnect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( ffPressing() ) );

    QVERIFY( mVideoToolBar->mSeekingState == EMPXFastForwarding );
    QVERIFY( mController->mCommand == EMPXPbvCmdSeekForward );

    //
    // End seeking
    //
    mVideoToolBar->mButtons[EMPX4thButton]->release();

    QVERIFY( mVideoToolBar->mSeekingState == EMPXNotSeeking );
    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdEndSeek );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testLongTapOnRW
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testLongTapOnRW()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testLongTapOnRW()"));

    init();

    //
    // Start seeking
    //
    mVideoToolBar->mButtons[EMPX2ndButton]->press();
    QVERIFY( mController->mTimerAction == EMPXTimerCancel );

    connect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( rwPressing() ) );
    emit commandSignal();
    disconnect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( rwPressing() ) );

    QVERIFY( mVideoToolBar->mSeekingState == EMPXRewinding );
    QVERIFY( mController->mCommand == EMPXPbvCmdSeekBackward );

    //
    // End seeking
    //
    mVideoToolBar->mButtons[EMPX2ndButton]->release();

    QVERIFY( mVideoToolBar->mSeekingState == EMPXNotSeeking );
    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdEndSeek );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testShortTapOnFF
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testShortTapOnFF()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testShortTapOnFF()"));

    init();

    //
    // If mPostion + KMPXFastForward < mDuration
    //
    mVideoToolBar->mPosition = 30;
    mVideoToolBar->mDuration = 70;
    mVideoToolBar->mButtons[EMPX4thButton]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetPosition );
    QVERIFY( mController->mCommandValue == mVideoToolBar->mPosition + KMPXFastForward );

    //
    // If mPostion + KMPXFastForward < mDuration
    //
    mVideoToolBar->mDuration = 50;
    mController->mCommand = EMPXPbvCmdPlay;

    mVideoToolBar->mButtons[EMPX4thButton]->release();
    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdPlay );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testShortTapOnRW
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testShortTapOnRW()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testShortTapOnRW()"));

    init();

    //
    // If mPostion + KMPXRewind > 0
    //
    mVideoToolBar->mPosition = 30;
    mVideoToolBar->mButtons[EMPX2ndButton]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetPosition );
    QVERIFY( mController->mCommandValue == mVideoToolBar->mPosition + KMPXRewind );

    //
    // If mPostion + KMPXRewind < 0
    //
    mVideoToolBar->mPosition = 4;
    mVideoToolBar->mButtons[EMPX2ndButton]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetPosition );
    QVERIFY( mController->mCommandValue == 0 );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testPause
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testPause()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testPause()"));

    init();

    mVideoToolBar->mButtons[EMPX3rdButton]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdPlayPause );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testChangeAspectRatio
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testChangeAspectRatio()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testChangeAspectRatio()"));

    init();

    //
    // Test natural
    //
    mVideoToolBar->mAspectRatio = EMMFZoom;
    mVideoToolBar->mButtons[EMPX1stButton]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdNaturalAspectRatio );

    //
    // Test stretch
    //
    mVideoToolBar->mAspectRatio = EMMFStretch;
    mVideoToolBar->mButtons[EMPX1stButton]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdZoomAspectRatio );

    //
    // Test stretch
    //
    mVideoToolBar->mAspectRatio = EMMFNatural;
    mVideoToolBar->mButtons[EMPX1stButton]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdStretchAspectRatio );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testUpdateState
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testUpdateState()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testUpdateState()"));

    init();

    //
    // Playing
    //
    TMPXPlaybackState state = EPbStatePlaying;
    mVideoToolBar->updateState( state );

    QVERIFY( mVideoToolBar->mToolBar->mEnabled == true );

    //
    // Paused
    //
    state = EPbStatePaused;
    mVideoToolBar->updateState( state );

    QVERIFY( mVideoToolBar->mToolBar->mEnabled == true );

    //
    // Not initialized
    //
    state = EPbStateNotInitialised;
    mVideoToolBar->updateState( state );

    QVERIFY( mVideoToolBar->mToolBar->mEnabled == false );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testAspectRatioChanged
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testAspectRatioChanged()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testAspectRatioChanged()"));

    init();

    //
    // Natural
    //
    int aspectRatio = EMMFNatural;
    mVideoToolBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mVideoToolBar->mAspectRatio == EMMFNatural );

    //
    // Stretch
    //
    aspectRatio = EMMFStretch;
    mVideoToolBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mVideoToolBar->mAspectRatio == EMMFStretch );

    //
    // Zoom
    //
    aspectRatio = EMMFZoom;
    mVideoToolBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mVideoToolBar->mAspectRatio == EMMFZoom );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testHandleButtonPressed
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testHandleButtonPressed()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testHandleButtonPressed()"));

    init();

    mVideoToolBar->mButtons[EMPX1stButton]->press();

    QVERIFY( mController->mTimerAction == EMPXTimerCancel );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testUpdateWithFileDetails
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testUpdateWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testUpdateWithFileDetails()"));

    //
    // 'non-attach' mode
    //
    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // video clip, view mode is full-screen
    //
    details->mVideoEnabled = true;
    details->mVideoHeight = 1;
    details->mVideoWidth = 1;
    details->mTvOutConnected = false;
    details->mSeekable = true;
    details->mPausableStream = true;
    mController->mViewMode = EFullScreenView;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[EMPX1stButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX2ndButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX3rdButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX4thButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX5thButton]->isEnabled() == true );

    //
    // video clip has same aspect ratio as display window
    //
    details->mVideoEnabled = true;
    details->mVideoHeight = 360;
    details->mVideoWidth = 640;
    details->mTvOutConnected = false;
    mController->mViewMode = EFullScreenView;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[EMPX1stButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX2ndButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX3rdButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX4thButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX5thButton]->isEnabled() == true );

    //
    // audio-only clip, view mode is full-screen
    //
    details->mVideoEnabled = false;
    details->mVideoHeight = 0;
    details->mVideoWidth = 1;
    details->mTvOutConnected = true;
    details->mSeekable = false;
    details->mPausableStream = true;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[EMPX1stButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX2ndButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX3rdButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX4thButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX5thButton]->isEnabled() == false );

    //
    // local audio-only clip, view mode is audio-only
    //
    details->mVideoEnabled = false;
    details->mVideoHeight = 0;
    details->mVideoWidth = 1;
    details->mTvOutConnected = true;
    details->mSeekable = true;
    details->mPausableStream = false;
    mController->mViewMode = EAudioOnlyView;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[EMPX1stButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX2ndButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX3rdButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX4thButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX5thButton]->isEnabled() == false );

    //
    // streaming audio-only clip, view mode is audio-only
    //
    details->mVideoEnabled = false;
    details->mPlaybackMode = EMPXVideoStreaming;
    mController->mViewMode = EAudioOnlyView;
    details->mSeekable = false;
    details->mPausableStream = false;
	
    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[EMPX1stButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX2ndButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX3rdButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX4thButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[EMPX5thButton]->isEnabled() == false );

    cleanup();

    //
	// 'attach' mode
	//
    init( true );

    //
    // video clip, view mode is full-screen
    //
    details->mVideoEnabled = true;
    details->mVideoHeight = 1;
    details->mVideoWidth = 1;
    details->mTvOutConnected = false;
    details->mSeekable = true;
    details->mPausableStream = true;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[EMPX1stButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX2ndButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX3rdButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX4thButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[EMPX5thButton]->isEnabled() == true );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testOpenDetailsView
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testOpenDetailsView()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testOpenDetailsView()"));

    init();

    QSignalSpy spy( mVideoToolBar, SIGNAL( openDetailsView() ) );  

    mController->mViewMode = EFullScreenView;
    mVideoToolBar->mButtons[EMPX5thButton]->release();
    QVERIFY( mController->mViewMode == EDetailsView );

    mController->mViewMode = EDetailsView;
    mVideoToolBar->mButtons[EMPX5thButton]->release();
    QVERIFY( mController->mViewMode == EDetailsView );

    mController->mViewMode = EAudioOnlyView;
    mVideoToolBar->mButtons[EMPX5thButton]->release();
    QVERIFY( mController->mViewMode == EAudioOnlyView );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testAttach
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testAttach()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testAttach()"));

    //
    // test when 'attach' operation is enabled
    //
    init( true );
    
    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mVideoEnabled = false;
    mVideoToolBar->updateWithFileDetails( details );

    //
    // release 'attach' button
    //
    mVideoToolBar->mButtons[EMPX1stButton]->release();

    //
    // verify the controller attachVideo() slot has been called
    //
    QVERIFY( mController->mCommand == EMPXPbvCmdClose );
    QVERIFY( mController->mAttachVideoDone == true );
    
    //
    // clean up
    //
    cleanup();
    
    //
    // test when 'attach' operation is disabled
    //
    init();

    //
    // verify the controller attachVideo() slot is not called
    //
    QVERIFY( mController->mAttachVideoDone == false );
    
    //
    // clean up
    //
    cleanup();
    
}

// -------------------------------------------------------------------------------------------------
// testSend
// -------------------------------------------------------------------------------------------------
//
void TestMPXVideoPlaybackToolBar::testSend()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackToolBar::testSend()"));

    //
    // test when 'share' operation is enabled
    //
    init( false );
    
    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mVideoEnabled = false;
    mController->mViewMode = EAudioOnlyView;
    mVideoToolBar->updateWithFileDetails( details ); 
    
    //
    // release 'share' button
    //
    mVideoToolBar->mButtons[EMPX1stButton]->release();

    //
    // verify the controller sendVideo() slot has been called
    //
    QVERIFY( mController->mCommand == EMPXPbvCmdPause );
    QVERIFY( mController->mSendVideoDone == true );
    
    //
    // clean up
    //
    cleanup();        
}

// End of file
