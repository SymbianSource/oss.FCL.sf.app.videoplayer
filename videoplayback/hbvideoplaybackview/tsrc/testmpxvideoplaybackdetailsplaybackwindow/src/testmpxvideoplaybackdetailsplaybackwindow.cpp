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
* Description:   tester for methods in TestMPXVideoPlaybackDetailsPlaybackWindow
* 
*/

// Version : %version:  1 %


#include <qdebug>
#include <qgraphicsscenemouseevent>

#include <hbmainwindow.h>
#include <hbapplication.h>

#include "hbpushbutton.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "testmpxvideoplaybackdetailsplaybackwindow.h"


#define private public
#include "mpxvideoplaybackdetailsplaybackwindow.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackDetailsPlaybackWindow::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMPXVideoPlaybackDetailsPlaybackWindow tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testmpxvideoplaybackdetailsplaybackwindow.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackDetailsPlaybackWindow::init()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackDetailsPlaybackWindow::init()"));

    mController = new QMPXVideoPlaybackControlsController();   
    mWindow = new QMPXVideoPlaybackDetailsPlaybackWindow( mController );

    mWindow->initialize();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackDetailsPlaybackWindow::cleanup()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackDetailsPlaybackWindow::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mWindow )
    {
        delete mWindow;
        mWindow = NULL;
    }
}

// ---------------------------------------------------------------------------
// testUpdateState
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackDetailsPlaybackWindow::testUpdateState()
{
    MPX_ENTER_EXIT(
            _L("TestMPXVideoPlaybackDetailsPlaybackWindow::testUpdateState()"));

    init();

    TMPXPlaybackState state = EPbStatePlaying;
    mWindow->updateState( state );
    QVERIFY( mWindow->mPlayButton->isVisible() == false );
    
    state = EPbStatePaused;
    mWindow->updateState( state );
    QVERIFY( mWindow->mPlayButton->isVisible() == true );

    cleanup();
}

// ---------------------------------------------------------------------------
// testMousePressEvent
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackDetailsPlaybackWindow::testMousePressEvent()
{
    MPX_ENTER_EXIT(
            _L("TestMPXVideoPlaybackDetailsPlaybackWindow::testMousePressEvent()"));

    init();

    QGraphicsSceneMouseEvent* event = 
            new QGraphicsSceneMouseEvent( QEvent::GraphicsSceneMousePress );
    event->setAccepted( false );

    mWindow->mousePressEvent( event );

    QVERIFY( event->isAccepted() );

    cleanup();
}

// ---------------------------------------------------------------------------
// testMouseReleaseEvent
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackDetailsPlaybackWindow::testMouseReleaseEvent()
{
    MPX_ENTER_EXIT(
            _L("TestMPXVideoPlaybackDetailsPlaybackWindow::testMouseReleaseEvent()"));

    init();

    QGraphicsSceneMouseEvent* event = 
            new QGraphicsSceneMouseEvent( QEvent::GraphicsSceneMouseRelease );

    mWindow->mouseReleaseEvent( event );
    QVERIFY( mController->mCommand == EMPXPbvCmdPlayPause );

    delete event;
    event = NULL;

    cleanup();
}

// ---------------------------------------------------------------------------
// testPlayPause
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackDetailsPlaybackWindow::testPlayPause()
{
    MPX_ENTER_EXIT(
            _L("TestMPXVideoPlaybackDetailsPlaybackWindow::testPlayPause()"));

    init();

    mWindow->mPlayButton->release();
    QVERIFY( mController->mCommand == EMPXPbvCmdPlayPause );

    cleanup();
}

// End of file
