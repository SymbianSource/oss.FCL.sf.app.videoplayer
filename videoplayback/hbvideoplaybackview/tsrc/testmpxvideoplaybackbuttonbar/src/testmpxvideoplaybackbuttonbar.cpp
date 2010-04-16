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
* Description:   tester for methods in TestMPXVideoPlaybackButtonBar
* 
*/

// Version : %version:  1 %


#include <qdebug>
#include <hbmainwindow.h>
#include <hbapplication.h>

#include "testmpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "hbpushbutton.h"

#define private public
#include "mpxvideoplaybackbuttonbar.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMPXVideoPlaybackButtonBar tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testmpxvideoplaybackbuttonbar.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::init()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::init()"));

    mController = new QMPXVideoPlaybackControlsController();   
    mButtonBar = new QMPXVideoPlaybackButtonBar( mController );

    mButtonBar->initialize();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::cleanup()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mButtonBar )
    {
        delete mButtonBar;
        mButtonBar = NULL;
    }
}

// ---------------------------------------------------------------------------
// testPlay
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testPlay()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testPlay()"));

    init();

    mButtonBar->mButtons[EMPXButtonPlay]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdPlay );

    cleanup();
}

// ---------------------------------------------------------------------------
// testStartFFSeeking
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testStartFFSeeking()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testStartFFSeeking()"));

    init();

    mButtonBar->mButtons[EMPXButtonFF]->press();

    QVERIFY( mButtonBar->mButtons[EMPXButtonFF]->mSelected == true );
    QVERIFY( mController->mTimerAction == EMPXTimerCancel );
    QVERIFY( mController->mCommand == EMPXPbvCmdSeekForward );

    cleanup();
}

// ---------------------------------------------------------------------------
// testStartRWSeeking
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testStartRWSeeking()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testStartRWSeeking()"));

    init();

    mButtonBar->mButtons[EMPXButtonRW]->press();

    QVERIFY( mButtonBar->mButtons[EMPXButtonRW]->mSelected == true );
    QVERIFY( mController->mTimerAction == EMPXTimerCancel );
    QVERIFY( mController->mCommand == EMPXPbvCmdSeekBackward );

    cleanup();
}

// ---------------------------------------------------------------------------
// testEndSeeking
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testEndSeeking()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testEndSeeking()"));

    init();

    mButtonBar->mButtons[EMPXButtonRW]->release();

    QVERIFY( mButtonBar->mButtons[EMPXButtonFF]->mSelected == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonRW]->mSelected == false );
    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdEndSeek );

    cleanup();
}

// ---------------------------------------------------------------------------
// testPause
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testPause()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testPause()"));

    init();

    mButtonBar->mButtons[EMPXButtonPause]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdPause );

    cleanup();
}

// ---------------------------------------------------------------------------
// testChangeAspectRatio
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testChangeAspectRatio()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testChangeAspectRatio()"));

    init();

    //
    // Test natural
    //
    mButtonBar->mButtons[EMPXButtonNatural]->setVisible( true );
    mButtonBar->mButtons[EMPXButtonStretch]->setVisible( false );
    mButtonBar->mButtons[EMPXButtonZoom]->setVisible( false );
    mButtonBar->mButtons[EMPXButtonNatural]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdNaturalAspectRatio );

    //
    // Test stretch
    //
    mButtonBar->mButtons[EMPXButtonNatural]->setVisible( false );
    mButtonBar->mButtons[EMPXButtonStretch]->setVisible( true );
    mButtonBar->mButtons[EMPXButtonZoom]->setVisible( false );
    mButtonBar->mButtons[EMPXButtonStretch]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdStretchAspectRatio );

    //
    // Test stretch
    //
    mButtonBar->mButtons[EMPXButtonNatural]->setVisible( false );
    mButtonBar->mButtons[EMPXButtonStretch]->setVisible( false );
    mButtonBar->mButtons[EMPXButtonZoom]->setVisible( true );
    mButtonBar->mButtons[EMPXButtonZoom]->release();

    QVERIFY( mController->mTimerAction == EMPXTimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdZoomAspectRatio );

    cleanup();
}

// ---------------------------------------------------------------------------
// testUpdateState
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testUpdateState()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testUpdateState()"));

    init();

    //
    // Playing
    //
    TMPXPlaybackState state = EPbStatePlaying;
    mButtonBar->updateState( state );

    QVERIFY( mButtonBar->mButtons[EMPXButtonPlay]->mVisible == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonPause]->mVisible == true );

    //
    // Paused
    //
    state = EPbStatePaused;
    mButtonBar->updateState( state );

    QVERIFY( mButtonBar->mButtons[EMPXButtonPlay]->mVisible == true );
    QVERIFY( mButtonBar->mButtons[EMPXButtonPause]->mVisible == false );

    //
    // Not initialized
    //
    state = EPbStateNotInitialised;
    mButtonBar->updateState( state );

    QVERIFY( mButtonBar->isEnabled() == false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testAspectRatioChanged
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testAspectRatioChanged()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testAspectRatioChanged()"));

    init();

    //
    // Natural
    //
    int aspectRatio = EMMFNatural;
    mButtonBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mButtonBar->mButtons[EMPXButtonNatural]->mVisible == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonStretch]->mVisible == true );
    QVERIFY( mButtonBar->mButtons[EMPXButtonZoom]->mVisible == false );

    //
    // Stretch
    //
    aspectRatio = EMMFStretch;
    mButtonBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mButtonBar->mButtons[EMPXButtonNatural]->mVisible == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonStretch]->mVisible == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonZoom]->mVisible == true );

    //
    // Zoom
    //
    aspectRatio = EMMFZoom;
    mButtonBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mButtonBar->mButtons[EMPXButtonNatural]->mVisible == true );
    QVERIFY( mButtonBar->mButtons[EMPXButtonStretch]->mVisible == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonZoom]->mVisible == false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testHandleButtonPressed
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testHandleButtonPressed()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testHandleButtonPressed()"));

    init();

    mButtonBar->mButtons[EMPXButtonNatural]->press();

    QVERIFY( mController->mTimerAction == EMPXTimerCancel );

    cleanup();
}

// ---------------------------------------------------------------------------
// testUpdateWithFileDetails
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testUpdateWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testUpdateWithFileDetails()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();

    details->mVideoEnabled = true;
    details->mVideoHeight = 1;
    details->mVideoWidth = 1;
    details->mTvOutConnected = false;
    details->mTvOutPlayAllowed = true;
    details->mSeekable = true;
    details->mPausableStream = true;

    mButtonBar->updateWithFileDetails( details );
    QVERIFY( mButtonBar->mButtons[EMPXButtonNatural]->mEnabled == true );
    QVERIFY( mButtonBar->mButtons[EMPXButtonStretch]->mEnabled == true );
    QVERIFY( mButtonBar->mButtons[EMPXButtonZoom]->mEnabled == true );
    QVERIFY( mButtonBar->mButtons[EMPXButtonRW]->mEnabled == true );
    QVERIFY( mButtonBar->mButtons[EMPXButtonPause]->mEnabled == true );
    QVERIFY( mButtonBar->mButtons[EMPXButtonPlay]->mEnabled == true );
    QVERIFY( mButtonBar->mButtons[EMPXButtonDetails]->mEnabled == true );

    details->mVideoEnabled = false;
    details->mVideoHeight = 0;
    details->mVideoWidth = 1;
    details->mTvOutConnected = true;
    details->mTvOutPlayAllowed = true;
    details->mSeekable = false;
    details->mPausableStream = false;

    mButtonBar->updateWithFileDetails( details );
    QVERIFY( mButtonBar->mButtons[EMPXButtonNatural]->mEnabled == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonStretch]->mEnabled == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonZoom]->mEnabled == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonRW]->mEnabled == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonPause]->mEnabled == false );
    QVERIFY( mButtonBar->mButtons[EMPXButtonPlay]->mEnabled == true );
    QVERIFY( mButtonBar->mButtons[EMPXButtonDetails]->mEnabled == false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testOpenDetailsView
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackButtonBar::testOpenDetailsView()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackButtonBar::testOpenDetailsView()"));

    init();

    QSignalSpy spy( mButtonBar, SIGNAL( openDetailsView() ) );  

    mController->mViewMode = EFullScreenView;
    mButtonBar->mButtons[EMPXButtonDetails]->release();
    QVERIFY( mController->mViewMode == EDetailsView );

    mController->mViewMode = EDetailsView;
    mButtonBar->mButtons[EMPXButtonDetails]->release();
    QVERIFY( mController->mViewMode == EDetailsView );

    mController->mViewMode = EAudioOnlyView;
    mButtonBar->mButtons[EMPXButtonDetails]->release();
    QVERIFY( mController->mViewMode == EAudioOnlyView );

    cleanup();
}

// End of file
