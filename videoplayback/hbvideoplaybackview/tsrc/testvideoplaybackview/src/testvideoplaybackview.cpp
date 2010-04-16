/**
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
* Description:   tester for methods in VideoSortFilterProxyModel
* 
*/

// Version : %version: %

#include <e32err.h>
#include <w32std.h>

#include <mmf/common/mmferrors.h>
#include <caf/caferr.h>

#include <mpxvideoplaybackdefs.h>

#include <hbapplication.h>
#include <hbinstance.h>
#include <qgraphicssceneevent>
#include <QDebug>

#include "mpxvideoplaybackviewfiledetails.h"

#include "testvideoplaybackview.h"
#include "mpxvideoviewwrapper.h"

#define private public
#define protected public
#include "hbvideoplaybackview.h"
#include "hbvideobaseplaybackview.h"
#undef protected
#undef private




// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    
    TestVideoPlaybackView tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestVideoPlaybackView.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackView::init()
{	  
	  mVideoView = new HbVideoPlaybackView();	  
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackView::cleanup()
{
    delete mVideoView;
    mVideoView = NULL;
}

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackView::setup()
{
    init();
    mVideoView->handleActivateView();
}


void TestVideoPlaybackView::testHandleActivateView()
{
    init();		
    mVideoView->handleActivateView();    
        
    QVERIFY( mVideoView->mVideoMpxWrapper->iMediaRequested == true );
    QCOMPARE( mVideoView->mVideoMpxWrapper->iFileDetails->mBitRate, 16000 );
    QCOMPARE( mVideoView->mVideoMpxWrapper->iFileDetails->mTitle, QString("Test Video Title") );
    
    cleanup();        
}

void TestVideoPlaybackView::testHandlePluginError()
{   
    //
    // playback view is closed after displaying these error notes
    //
    verifyHandlePluginError( KErrNotSupported );
    verifyHandlePluginError( KErrUnknown );
    verifyHandlePluginError( KErrMMDecoder );
    verifyHandlePluginError( KErrCorrupt );
    verifyHandlePluginError( KErrTooBig );
    verifyHandlePluginError( KErrMMInvalidProtocol );
    verifyHandlePluginError( KErrMMInvalidURL );
    verifyHandlePluginError( KErrArgument );
    verifyHandlePluginError( KErrSessionClosed ); 
    verifyHandlePluginError( KErrTimedOut );
    verifyHandlePluginError( KErrNotFound );
    verifyHandlePluginError( KErrMMNotEnoughBandwidth );   
    verifyHandlePluginError( KErrDisconnected );
    verifyHandlePluginError( KErrMMProxyServer ); 
    verifyHandlePluginError( KErrCouldNotConnect );
    verifyHandlePluginError( KErrAbort );
    verifyHandlePluginError( KErrCancel ); 
    verifyHandlePluginError( KErrMMDRMNotAuthorized );
    verifyHandlePluginError( KErrCANoRights );
    verifyHandlePluginError( KErrCANoPermission );
    verifyHandlePluginError( KMPXVideoTvOutPlaybackNotAllowedClose );
    
    //
    // playback view remains open after displaying these error notes
    //
    verifyHandlePluginError( KMPXVideoCallOngoingError, false );
    verifyHandlePluginError( KMPXVideoTvOutPlaybackNotAllowed, false );
    verifyHandlePluginError( KMPXVideoPlayOver2GDuringVoiceCallError, false );
            
    //
    // default error case, playback view is closed after displaying error note
    //
    verifyHandlePluginError( KErrGeneral );    
}

void TestVideoPlaybackView::testShowDialog()
{   
    //
    // construct and activate playback view
    //
    setup();
    
    //
    // ensure that playback view is currently activated
    //
    QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );
    
    //
    // test showDialog() method
    //
    mVideoView->showDialog( "test error msg" );
    
    //
    // connect and emit signal for handleClosePopupDialog() slot
    //
    connect( this, SIGNAL( commandSignal() ), mVideoView, SLOT( handleClosePopupDialog() ) );
    emit commandSignal();     

    //
    // verify that playback view is properly closed
    //
    QVERIFY( mVideoView->mTimerForClosingView->isActive() );

    //
    // disconnect signal for handleClosePopupDialog() slot
    //
    disconnect( this, SIGNAL( commandSignal() ), mVideoView, SLOT( handleClosePopupDialog() ) );
    
    //
    // destruct playback view
    //
    cleanup();    
}


void TestVideoPlaybackView::testGetWindow()
{      
    setup();
    
    QVERIFY( mVideoView->getWindow() != NULL );     
    
    cleanup();
}


void TestVideoPlaybackView::testIssuePlayCommand()
{      
    setup();
    
    mVideoView->issuePlayCommand();      
    
    //JAM remove
    QVERIFY( mVideoView->isEnabled() );
    
    cleanup();
}

void TestVideoPlaybackView::testEvent()
{
    setup();
    
    //
    // ensure default foreground is false
    //
    QCOMPARE( mVideoView->mVideoMpxWrapper->iForeground, 0 );

    //
    // declare foreground/background event
    //
    QEvent* windowActivate   = new QEvent( QEvent::WindowActivate );
    QEvent* windowDeactivate = new QEvent( QEvent::WindowDeactivate );
    
    //
    // verify the returned value (consumed) of HbVideoBasePlaybackView::event() 
    // for QEvent::WindowActivate is false
    //
    QVERIFY( ! mVideoView->event( windowActivate ) );  
    
    //
    // verify view is in foreground
    //
    QCOMPARE( mVideoView->mVideoMpxWrapper->iForeground, 1 );

    //
    // verify the returned value (consumed) of HbVideoBasePlaybackView::event() 
    // for QEvent::WindowDeactivate is false
    //
    QVERIFY( ! mVideoView->event( windowDeactivate ) );   
    
    //
    // verify view is in background
    //
    QCOMPARE( mVideoView->mVideoMpxWrapper->iForeground, 0 );
    
    //
    // clean up
    //
    delete windowActivate;
    windowActivate = NULL;
    delete windowDeactivate;
    windowDeactivate = NULL;
    cleanup();
}

void TestVideoPlaybackView::testClosePlaybackView()
{
    setup();
    
    QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );

    mVideoView->closePlaybackView();
    
    QVERIFY( mVideoView->mTimerForClosingView->isActive() );

    cleanup();
}

void TestVideoPlaybackView::testMousePressEvent()
{
    setup();
    
    //
    // declare QEvent::GraphicsSceneMousePress
    //
    QGraphicsSceneMouseEvent* event = new QGraphicsSceneMouseEvent( QEvent::GraphicsSceneMousePress );
        
    //
    // set m_accept value to false
    //        
    event->setAccepted( false );
    
    // 
    // simulate QEvent::GraphicsSceneMousePress event
    //
    mVideoView->mousePressEvent( event );  
    
    //
    // verify event has been accepted
    //        
    QVERIFY( event->isAccepted() );
    
    //
    // clean up
    //    
    delete event;
    event = NULL;    
    cleanup();
}

void TestVideoPlaybackView::testMouseReleaseEvent()
{
    setup();
    
    //
    // declare QEvent::GraphicsSceneMouseRelease
    //
    QGraphicsSceneMouseEvent* event = new QGraphicsSceneMouseEvent( QEvent::GraphicsSceneMouseRelease );
    
    //
    // connect to tappedOnScreen() signal and record its emission
    //
    QSignalSpy spy( mVideoView, SIGNAL(tappedOnScreen()) );  
    
    //
    // ensure signal has not been emitted yet
    //
    QCOMPARE(spy.count(), 0);     
    
    // 
    // simulate QEvent::GraphicsSceneMousePress event
    //
    mVideoView->mouseReleaseEvent( event );        
    
    //
    // ensure signal has been emitted and its emission count is incremented
    //    
    QCOMPARE(spy.count(), 1);  
    
    //
    // clean up
    //
    delete event;
    event = NULL;    
    spy.clear();
    cleanup();    
}


void TestVideoPlaybackView::testHandlePdlStateChange()
{
    setup();

    QCOMPARE( mVideoView->mVideoMpxWrapper->iFileDetails->mClipName, QString("testClip.3gp") );
    
    mVideoView->handlePdlStateChange( 0 );
    
    QCOMPARE( mVideoView->mVideoMpxWrapper->iFileDetails->mClipName, QString("testClip.3gp") );
    
    cleanup();
}

void TestVideoPlaybackView::testHandleStoppedStateNoPrevView()
{    
    setup();

    QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );        
    mVideoView->handleStoppedState();

    QVERIFY( mVideoView->mTimerForClosingView->isActive() );    

    cleanup();   
}


void TestVideoPlaybackView::testHandleStoppedStatePrevView()
{ 
    setup();
           
    mVideoView->handleStoppedState();
    
    QVERIFY( mVideoView->mTimerForClosingView->isActive() );
    
    cleanup();
    
}

void TestVideoPlaybackView::testHandleBackNoPrevView()
{    
    setup();
       
    QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );
    mVideoView->handleBack();

    QVERIFY( mVideoView->mTimerForClosingView->isActive() );  
  
    cleanup();
    
}

void TestVideoPlaybackView::testHandleBackPrevView()
{  
    setup();
           
    mVideoView->handleBack();
    
    QVERIFY( mVideoView->mTimerForClosingView->isActive() ); 
    
    cleanup();
}

void TestVideoPlaybackView::testHandleSoftkeyback()
{        
    setup();
           
    mVideoView->handleSoftkeyBack();
    
    QVERIFY( mVideoView->mTimerForClosingView->isActive() ); 
    
    cleanup();    
}



void TestVideoPlaybackView::testDoClosePlayer()
{    
    // quits app - which can be problematic for the qvptestrunner app    

//    setup();
//    
//    mVideoView->handleActivateView();
//
//    mVideoView->doClosePlayer();
//    
//    cleanup();
    
}

void TestVideoPlaybackView::testHandleDeactivateView()
{    
    setup();
    
    mVideoView->handleDeactivateView();
    
    QVERIFY( mVideoView->mVideoMpxWrapper == NULL );   
    
    cleanup();        
}

void TestVideoPlaybackView::verifyHandlePluginError( TInt error, bool closeView )
{       
    //
    // construct and activate playback view
    //
    setup();
    
    //
    // ensure that playback view is currently open
    //
    QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );
    
    //
    // test handlePluginError() method
    //
    mVideoView->handlePluginError( error );   
    
    //
    // close playback view after error note has been launched
    //
    if ( closeView )
    {
        //
        // connect and emit signal for handleClosePopupDialog() slot
        //
        connect( this, SIGNAL( commandSignal() ), mVideoView, SLOT( handleClosePopupDialog() ) );
        emit commandSignal();  

        //
        // verify that playback view is properly closed
        //
        QVERIFY( mVideoView->mTimerForClosingView->isActive() );
    
        //
        // disconnect signal for handleClosePopupDialog() slot
        //
        disconnect( this, SIGNAL( commandSignal() ), mVideoView, SLOT( handleClosePopupDialog() ) );
    }
    else
    {    
        //
        // verify that playback view is still open after error note has been launched
        //
        QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );
    }        
    
    //
    // destruct playback view
    //
    cleanup();
}

// End of file
    


