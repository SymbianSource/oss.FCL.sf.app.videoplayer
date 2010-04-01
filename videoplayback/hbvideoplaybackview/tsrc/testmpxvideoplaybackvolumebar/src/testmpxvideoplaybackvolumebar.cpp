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
* Description:   tester for methods in TestMPXVideoPlaybackVolumeBar
* 
*/

// Version : %version:  2 %


#include <qdebug>
#include <qtimer>
#include <qgraphicsscenemouseevent>

#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hbvolumeslider.h>

#include "mpxvideo_debug.h"
#include "testmpxvideoplaybackvolumebar.h"

#define private public
#include "mpxvideoplaybacknontouchvolumebar.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackVolumeBar::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMPXVideoPlaybackVolumeBar tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testmpxvideoplaybackvolumebar.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackVolumeBar::init()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackVolumeBar::init()"));

    mVolumeBar = new QMPXVideoPlaybackNonTouchVolumeBar();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackVolumeBar::cleanup()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackVolumeBar::cleanup()"));

    if ( mVolumeBar )
    {
        delete mVolumeBar;
        mVolumeBar = NULL;
    }
}

// ---------------------------------------------------------------------------
// testSetVisible
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackVolumeBar::testSetVisible()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackVolumeBar::testSetVisible()"));

    init();

    mVolumeBar->setVisible( false );

    QVERIFY( mVolumeBar->mVolumeTimer->isActive() == false );
    QVERIFY( mVolumeBar->isVisible() == false );

    mVolumeBar->setVisible( true );

    QVERIFY( mVolumeBar->mVolumeTimer->isActive() == true );
    QVERIFY( mVolumeBar->isVisible() == true );

    cleanup();
}

// ---------------------------------------------------------------------------
// testHideVolumeControl
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackVolumeBar::testHideVolumeControl()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackVolumeBar::testHideVolumeControl()"));

    init();

    connect( this, SIGNAL( commandSignal() ), mVolumeBar, SLOT( hideVolumeControl() ) );
    emit commandSignal();
    disconnect( this, SIGNAL( commandSignal() ), mVolumeBar, SLOT( hideVolumeControl() ) );

    cleanup();
}

// ---------------------------------------------------------------------------
// testMousePressEvent
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackVolumeBar::testMousePressEvent()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackVolumeBar::testMousePressEvent()"));

    init();

    QGraphicsSceneMouseEvent* event = 
            new QGraphicsSceneMouseEvent( QEvent::GraphicsSceneMousePress );
    event->setAccepted( true );

    mVolumeBar->mousePressEvent( event );

    QVERIFY( ! event->isAccepted() );

    delete event;
    event = NULL;

    cleanup();
}

// ---------------------------------------------------------------------------
// testMouseReleaseEvent
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackVolumeBar::testMouseReleaseEvent()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackVolumeBar::testMouseReleaseEvent()"));

    init();

    QGraphicsSceneMouseEvent* event = 
            new QGraphicsSceneMouseEvent( QEvent::GraphicsSceneMouseRelease );
    event->setAccepted( true );

    mVolumeBar->mouseReleaseEvent( event );
    QVERIFY( ! event->isAccepted() );

    delete event;
    event = NULL;

    cleanup();
}

// End of file
