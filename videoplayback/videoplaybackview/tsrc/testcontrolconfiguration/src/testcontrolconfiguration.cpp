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
* Description:   tester for methods in videoplaybackcontrolconfiguration
*
*/

// Version : %version: 6 %

#include <hbapplication.h>
#include <hbinstance.h>
#include <qdebug>
#include <qgraphicswidget>

#include "videoplaybackcontrol.hrh"
#include "mpxhelixplaybackplugindefs.h"

#include "testcontrolconfiguration.h"

#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"
#include "videoplaybackdocumentloader.h"


#define private public
#include "videoplaybackcontrolconfiguration.h"
#undef private


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestControlConfiguration::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestControlConfiguration tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testcontrolconfiguration.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestControlConfiguration::init()
{
    MPX_ENTER_EXIT(_L("TestControlConfiguration::init()"));
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestControlConfiguration::setup()
{
    MPX_ENTER_EXIT(_L("TestControlConfiguration::setup()"));

    mFileDetails = new VideoPlaybackViewFileDetails();

    mControlsController = new VideoPlaybackControlsController( mFileDetails );

    mControlConfig = new VideoPlaybackControlConfiguration( mControlsController );
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestControlConfiguration::cleanup()
{
    MPX_ENTER_EXIT(_L("TestControlConfiguration::cleanup()"));

    if ( mFileDetails )
    {
        delete mFileDetails;
        mFileDetails = NULL;
    }

    if ( mControlsController )
    {
        delete mControlsController;
        mControlsController = NULL;
    }

    if ( mControlConfig )
    {
        delete mControlConfig;
        mControlConfig = NULL;
    }
}

// ---------------------------------------------------------------------------
// testControlList
// ---------------------------------------------------------------------------
//
void TestControlConfiguration::testControlList()
{
    MPX_ENTER_EXIT(_L("TestControlConfiguration::testControlList()"));

    setup();

    //
    // Streaming case
    //
    mFileDetails->mPlaybackMode = EMPXVideoStreaming;

    mControlConfig->createControlList();
    QList<TVideoPlaybackControls> controlsList = mControlConfig->controlList();

    QVERIFY( controlsList.contains( EStatusPane ) );
    QVERIFY( ! controlsList.contains( ERealLogoBitmap ) );
    QVERIFY( controlsList.contains( EBufferingAnimation ) );

    cleanup();

    //
    // local + RN
    //
    setup();

    mFileDetails->mPlaybackMode = EMPXVideoLocal;
    mFileDetails->mRNFormat = true;

    mControlConfig->createControlList();
    controlsList = mControlConfig->controlList();

    QVERIFY( controlsList.contains( EStatusPane ) );
    QVERIFY( controlsList.contains( ERealLogoBitmap ) );
    QVERIFY( ! controlsList.contains( EBufferingAnimation ) );

    cleanup();

    //
    // local + non RN
    //
    setup();

    mFileDetails->mPlaybackMode = EMPXVideoLocal;
    mFileDetails->mRNFormat = false;

    mControlConfig->createControlList();
    controlsList = mControlConfig->controlList();

    QVERIFY( controlsList.contains( EStatusPane ) );
    QVERIFY( ! controlsList.contains( ERealLogoBitmap ) );
    QVERIFY( ! controlsList.contains( EBufferingAnimation ) );

    cleanup();
}

// ---------------------------------------------------------------------------
// testUpdateControlsWithFileDetails
// ---------------------------------------------------------------------------
//
void TestControlConfiguration::testUpdateControlsWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestControlConfiguration::testUpdateControlsWithFileDetails()"));

    setup();
    mControlConfig->createControlList();

    //
    // 1. test with mVideoEnabled = false
    //
    mControlsController->mFileDetails->mVideoEnabled = false;

    mControlConfig->updateControlsWithFileDetails();

    QList<TVideoPlaybackControls> controlsList = mControlConfig->controlList();

    QVERIFY( controlsList.contains( EControlBar ) );

    //
    // 2. test with mVideoEnabled = true
    //
    mControlsController->mFileDetails->mVideoEnabled = false;

    mControlConfig->updateControlsWithFileDetails();

    QVERIFY( controlsList.contains( EControlBar ) );

    QGraphicsWidget *widget =
            mControlsController->layoutLoader()->findWidget( QString( "transparentWindow" ) );

    QVERIFY( widget->isVisible() );

    cleanup();
}

// ---------------------------------------------------------------------------
// testUpdateControlList
// ---------------------------------------------------------------------------
//
void TestControlConfiguration::testUpdateControlList()
{
    MPX_ENTER_EXIT(_L("TestControlConfiguration::testUpdateControlList()"));

    setup();
    mControlConfig->createControlList();

    QGraphicsWidget *widget =
            mControlConfig->mControlsController->layoutLoader()->findWidget( QString( "transparentWindow" ) );
    //
    // 1.  Test for Details View
    //
    mControlConfig->updateControlList( EControlCmdDetailsViewOpened );
    QList<TVideoPlaybackControls> controlsList = mControlConfig->controlList();

    QVERIFY( ! widget->isVisible() );
    QVERIFY( controlsList.contains( EFileDetailsWidget ) );
    QVERIFY( controlsList.contains( EDetailsViewPlaybackWindow ) );
    QVERIFY( ! controlsList.contains( EIndicatorBitmap ) );

    //
    // 2-1. Test for Fullscreen View
    //
    mControlConfig->mSurfaceAttached = false;
    mControlConfig->updateControlList( EControlCmdFullScreenViewOpened );
    controlsList = mControlConfig->controlList();

    QVERIFY( ! widget->isVisible() );
    QVERIFY( ! controlsList.contains( EFileDetailsWidget ) );
    QVERIFY( ! controlsList.contains( EDetailsViewPlaybackWindow ) );
    QVERIFY( ! controlsList.contains( EIndicatorBitmap ) );

    //
    // 2-2. Test for Fullscreen View
    //
    mControlConfig->mSurfaceAttached = true;
    mControlConfig->updateControlList( EControlCmdFullScreenViewOpened );
    controlsList = mControlConfig->controlList();

    QVERIFY( widget->isVisible() );
    QVERIFY( ! controlsList.contains( EFileDetailsWidget ) );
    QVERIFY( ! controlsList.contains( EDetailsViewPlaybackWindow ) );
    QVERIFY( ! controlsList.contains( EIndicatorBitmap ) );

    //
    // 3. Test for Audio Only View
    //
    mControlConfig->updateControlList( EControlCmdAudionOnlyViewOpened );
    controlsList = mControlConfig->controlList();

    QVERIFY( ! widget->isVisible() );
    QVERIFY( ! controlsList.contains( EDetailsViewPlaybackWindow ) );
    QVERIFY( controlsList.contains( EIndicatorBitmap ) );
    QVERIFY( controlsList.contains( EFileDetailsWidget ) );

    //
    // 4. RN log gets removed
    //
    mControlConfig->updateControlList( EControlCmdAudionOnlyViewOpened );
    controlsList = mControlConfig->controlList();

    QVERIFY( ! controlsList.contains( ERealLogoBitmap ) );

    //
    // 5. Surface attached
    //
    mControlConfig->updateControlList( EControlCmdSurfaceAttached );
    controlsList = mControlConfig->controlList();

    QVERIFY( widget->isVisible() );

    //
    // 6. Surface detached
    //
    mControlConfig->updateControlList( EControlCmdSurfaceDetached );
    controlsList = mControlConfig->controlList();

    QVERIFY( ! widget->isVisible() );
    cleanup();
}

// End of file
