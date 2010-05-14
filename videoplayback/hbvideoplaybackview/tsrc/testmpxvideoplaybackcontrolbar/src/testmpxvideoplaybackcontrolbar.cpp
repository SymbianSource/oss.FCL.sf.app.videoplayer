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
* Description:   tester for methods in TestMPXVideoPlaybackControlBar
* 
*/

// Version : %version:  2 %


#include <qdebug>
#include <hbmainwindow.h>
#include <hbapplication.h>

#include "mpxvideoplaybacktoolbar.h"
#include "mpxvideoplaybackprogressbar.h"
#include "testmpxvideoplaybackcontrolbar.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"

#define private public
#include "mpxvideoplaybackcontrolbar.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlBar::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMPXVideoPlaybackControlBar tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testmpxvideoplaybackcontrolbar.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlBar::init()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlBar::init()"));

    mController = new QMPXVideoPlaybackControlsController();   
    mControlBar = new QMPXVideoPlaybackControlBar( mController );

    mControlBar->initialize();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlBar::cleanup()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlBar::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mControlBar )
    {
        delete mControlBar;
        mControlBar = NULL;
    }
}

// ---------------------------------------------------------------------------
// testupdateState
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlBar::testupdateState()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlBar::testupdateState()"));

    init();

    TMPXPlaybackState state = EPbStatePlaying;
    mControlBar->updateState( state );
    QVERIFY( mControlBar->mToolBar->mState == EPbStatePlaying );

    state = EPbStatePaused;
    mControlBar->updateState( state );
    QVERIFY( mControlBar->mToolBar->mState == EPbStatePaused );

    cleanup();
}

// ---------------------------------------------------------------------------
// testaspectRatioChanged
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlBar::testaspectRatioChanged()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlBar::testaspectRatioChanged()"));

    init();

    int aspectRatio = 1;
    mControlBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mControlBar->mToolBar->mAspectRatio == aspectRatio );

    cleanup();
}

// ---------------------------------------------------------------------------
// testUpdateWithFileDetails
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlBar::testUpdateWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlBar::testUpdateWithFileDetails()"));

    init();

    QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mSeekable = false;
    details->mVideoEnabled = true;

    mControlBar->updateWithFileDetails( details );

    QVERIFY( mControlBar->mToolBar->mFileDetails->mSeekable == details->mSeekable );
    QVERIFY( mControlBar->mProgressBar->mFileDetails->mVideoEnabled == details->mVideoEnabled );

    cleanup();
}

// ---------------------------------------------------------------------------
// testSetVisibleToControlBar
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlBar::testSetVisibleToControlBar()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlBar::testSetVisibleToControlBar()"));

    init();

    //
    // Make visible
    //
    mControlBar->setVisible( false );
    mControlBar->setVisibleToControlBar( true );
    QVERIFY( mControlBar->isVisible() == true );

    //
    // Make invisible
    //
    mControlBar->setVisible( true );
    mControlBar->setVisibleToControlBar( false );
    QVERIFY( mControlBar->isVisible() == false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testDurationChanged
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlBar::testDurationChanged()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlBar::testDurationChanged()"));

    init();

    int duration = 12345;
    mControlBar->durationChanged( duration );
    QVERIFY( mControlBar->mProgressBar->mDuration == duration );

    cleanup();
}

// ---------------------------------------------------------------------------
// testPositionChanged
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlBar::testPositionChanged()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlBar::testPositionChanged()"));

    init();

    int position = 54321;
    mControlBar->positionChanged( position );
    QVERIFY( mControlBar->mProgressBar->mPosition == position );

    cleanup();
}

// End of file
