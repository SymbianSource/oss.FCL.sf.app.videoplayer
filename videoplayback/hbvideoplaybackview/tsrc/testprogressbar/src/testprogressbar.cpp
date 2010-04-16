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
#include <qgraphicssceneevent>
#include <hbprogressbar.h>


#include "testprogressbar.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "hblabel.h"

#define private public
#include "mpxvideoplaybackprogressbar.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestProgressBar::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestProgressBar tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestProgressBar.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestProgressBar::init()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::init()"));
}

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------
//
void TestProgressBar::setup()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::setup()"));

    mController = new QMPXVideoPlaybackControlsController();   
    mProgBar = new QMPXVideoPlaybackProgressBar( mController );

    mProgBar->initialize();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestProgressBar::cleanup()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mProgBar )
    {
        delete mProgBar;
        mProgBar = NULL;
    }
}

// ---------------------------------------------------------------------------
// testMousePressEvent
// ---------------------------------------------------------------------------
//
void TestProgressBar::testMousePressEvent()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testMousePressEvent()"));

    setup();    
    mController->mState = EPbStatePlaying;        
    QGraphicsSceneMouseEvent* event = new QGraphicsSceneMouseEvent( QEvent::GraphicsSceneMousePress );       
    event->setAccepted( false );    

    mProgBar->mousePressEvent(event);
    
    QVERIFY( mController->mTimerAction == EMPXTimerCancel );        
    QVERIFY( mProgBar->mNeedToResumeAfterSetPosition );    
    QVERIFY( event->isAccepted() );      
    
    delete event;
    event = NULL;     
}

// ---------------------------------------------------------------------------
// testMouseReleaseEvent
// ---------------------------------------------------------------------------
//
void TestProgressBar::testMouseReleaseEvent()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testMouseReleaseEvent()"));

    setup();
    QGraphicsSceneMouseEvent* event = new QGraphicsSceneMouseEvent( QEvent::GraphicsSceneMouseRelease );       
    event->setAccepted( false );     
    
    mProgBar->mouseReleaseEvent(event);
    
    QVERIFY( mController->mTimerAction == EMPXTimerReset );    
    QVERIFY( event->isAccepted() );      
    
    delete event;
    event = NULL; 
}

// ---------------------------------------------------------------------------
// testMouseMoveEvent
// ---------------------------------------------------------------------------
//
void TestProgressBar::testMouseMoveEvent()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testMouseMoveEvent()"));

    setup();
    QGraphicsSceneMouseEvent* event = new QGraphicsSceneMouseEvent( QEvent::GraphicsSceneMouseMove );       
    event->setAccepted( false );
    mProgBar->mDuration = 0;
    
    mProgBar->mouseMoveEvent(event);
        
    QVERIFY( mProgBar->mPositionLabel->mTextSet );
    QVERIFY( event->isAccepted() );      
    
    delete event;
    event = NULL; 
}

// ---------------------------------------------------------------------------
// testUpdateWithFileDetails
// ---------------------------------------------------------------------------
//
void TestProgressBar::testUpdateWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testUpdateWithFileDetails()"));

    setup();
    
    // test for live
    mController->mFileDetails->mPlaybackMode = EMPXVideoLiveStreaming;    
    mProgBar->updateWithFileDetails( mController->mFileDetails );    
    QVERIFY( mProgBar->mDurationLabel->mLabelText == "Live" );
    
    // test for tv-out
    mController->mFileDetails->mPlaybackMode = EMPXVideoLocal; 
    mController->mFileDetails->mTvOutConnected = true;
    mController->mFileDetails->mTvOutPlayAllowed = false;
    mProgBar->updateWithFileDetails( mController->mFileDetails );
    QVERIFY( ! mProgBar->isEnabled() );
        
}

// ---------------------------------------------------------------------------
// testUpdateState
// ---------------------------------------------------------------------------
//
void TestProgressBar::testUpdateState()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testUpdateState()"));

    setup();
    
    // test for 1st block of cases
    mController->mFileDetails->mPlaybackMode = EMPXVideoLiveStreaming; 
    mController->mViewMode =  EAudioOnlyView;
    mProgBar->updateState(EPbStatePlaying);
    QVERIFY( mProgBar->mDurationLabel->mLabelText == "Live" );    
    
    // test for 2nd block of cases
    mProgBar->updateState(EPbStateBuffering);
    QVERIFY( ! mProgBar->isEnabled() );
    
}

// ---------------------------------------------------------------------------
// testDurationChanged
// ---------------------------------------------------------------------------
//
void TestProgressBar::testDurationChanged()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testDurationChanged()"));

    setup();
    
    mProgBar->durationChanged( 120 );
    
    QVERIFY( mProgBar->mDuration == 120 );
    QVERIFY( mProgBar->mProgressSlider->maximum() == 120 );
    
}

// ---------------------------------------------------------------------------
// testPositionChanged
// ---------------------------------------------------------------------------
//
void TestProgressBar::testPositionChanged()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testPositionChanged()"));

    setup();
    mProgBar->mDragging = false;
    
    mProgBar->positionChanged( 60 );
    QVERIFY( mProgBar->mPositionLabel->mTextSet );
        
}

// End of file
