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
* Description:   tester for methods in Video Display Handler
* 
*/

#include <e32err.h>
#include <w32std.h>
#include <eikenv.h>

#include <hbapplication.h>
#include <hbinstance.h>
#include <QDebug>

#include <mpxplaybackutility.h>

#include "testvideodisplayhandler.h"
#include "../stub/inc/hbvideobaseplaybackview.h"
#include "../stub/inc/mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoviewwrapper.h"

#define private public
#include "mpxvideoplaybackdisplayhandler.h"
#undef private


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    
    TestVideoDisplayHandler tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestVideoDisplayHandler.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoDisplayHandler::init()
{	  
    mPlaybackUtility = MMPXPlaybackUtility::UtilityL( KPbModeDefault );
    
	  mBaseVideoView    = new HbVideoBasePlaybackView();
	  mVideoViewWrapper = CMPXVideoViewWrapper::NewL( mBaseVideoView );   
    
    mDispHdlr = CMPXVideoPlaybackDisplayHandler::NewL(mPlaybackUtility, mVideoViewWrapper);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoDisplayHandler::cleanup()
{    
    if ( mPlaybackUtility )
    {
        mPlaybackUtility->Close();
        mPlaybackUtility = NULL;
    }    
    
    delete mDispHdlr;
    mDispHdlr = NULL;  
    
    delete mVideoViewWrapper;
    mVideoViewWrapper = NULL;
        
    delete mBaseVideoView;
    mBaseVideoView = NULL;              
}

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------
//
void TestVideoDisplayHandler::setup()
{    
    init();
    
    RWindow *window = mBaseVideoView->getWindow();

    TRect displayRect = TRect( window->Position().iX,
                               window->Position().iY,
                               window->Position().iX + window->Size().iWidth,                  
                               window->Position().iY + window->Size().iHeight );    

    mDispHdlr->CreateDisplayWindowL( CCoeEnv::Static()->WsSession(),
                                     *(CCoeEnv::Static()->ScreenDevice()),
                                     *window,
                                     displayRect);
    
    QCOMPARE( mDispHdlr->iWindowRect, displayRect);    
    
}


void TestVideoDisplayHandler::testCreateDisplayWindowL()
{
    setup();        
    
    cleanup();
}

void TestVideoDisplayHandler::testRemoveDisplayWindow()
{
    setup();
    
    mDispHdlr->RemoveDisplayWindow(); 
        
    QVERIFY( mDispHdlr->iVideoContainer == NULL );
    
    cleanup();        
}

void TestVideoDisplayHandler::testHandleSurfaceCreatedMessageL()
{
    setup();
    
    CMPXMessage* message = NULL;
    TRAP_IGNORE
    ( 
        message = CMPXMessage::NewL();    
        message->SetTObjectValueL<TMPXVideoDisplayCommand>
                    ( KMPXMediaVideoDisplayCommand, EPbMsgVideoSurfaceCreated );
    );    
    
    mDispHdlr->HandleVideoDisplayMessageL( message );
    
    QVERIFY( ! mDispHdlr->iSurfaceId.IsNull() );
           
    mDispHdlr->RemoveDisplayWindow(); 
    cleanup();    
}

void TestVideoDisplayHandler::testHandleSurfaceChangedMessageL()
{
    setup();
    
    CMPXMessage* message = NULL;
    TRAP_IGNORE
    ( 
        message = CMPXMessage::NewL();    
        message->SetTObjectValueL<TMPXVideoDisplayCommand>
                    ( KMPXMediaVideoDisplayCommand, EPbMsgVideoSurfaceChanged );
    );    
    
    mDispHdlr->HandleVideoDisplayMessageL( message );
    
    QVERIFY( ! mDispHdlr->iSurfaceId.IsNull() );
           
    mDispHdlr->RemoveDisplayWindow(); 
    cleanup();    
}

void TestVideoDisplayHandler::testHandleVideoRemovedMessageL()
{
    setup();

    CMPXMessage* message = NULL;
    TRAP_IGNORE
    ( 
        message = CMPXMessage::NewL();    
        message->SetTObjectValueL<TMPXVideoDisplayCommand>
                    ( KMPXMediaVideoDisplayCommand, EPbMsgVideoSurfaceRemoved );
    );    
    
    mDispHdlr->HandleVideoDisplayMessageL( message );
    
    QVERIFY( mDispHdlr->iSurfaceId.IsNull() );
           
    mDispHdlr->RemoveDisplayWindow(); 
    cleanup();    
}

void TestVideoDisplayHandler::testSetAspectRatioL()
{
    setup();
   
    int aspectRatio = mDispHdlr->SetAspectRatioL( EPbCmdNaturalAspectRatio );    
    
    QVERIFY( aspectRatio == EMMFNatural ); 
    
    mDispHdlr->RemoveDisplayWindow();    
    cleanup(); 
}

void TestVideoDisplayHandler::testSetDefaultAspectRatioL()
{
    setup();
        
    //
    // get window size
    //
    RWindow *window = mBaseVideoView->getWindow();
    TRect displayRect = TRect( TPoint( window->Position() ), TSize( window->Size() ) );

    //
    // get window aspect ratio
    //   if device is in landscape mode, width > height
    //   if device is in portrait mode, width < height
    //
    TReal32 width = (TReal32) displayRect.Width();
    TReal32 height = (TReal32) displayRect.Height();
    TReal32 displayAspectRatio = (width > height)? (width / height) : (height / width);
    
    //
    // aspect ratio zoom
    //
    mFileDetails = new QMPXVideoPlaybackViewFileDetails(); 
    mFileDetails->mVideoHeight = 280;
    mFileDetails->mVideoWidth  = 600;

    int aspectRatio = mDispHdlr->SetDefaultAspectRatioL( mFileDetails, displayAspectRatio );
    
    QVERIFY( aspectRatio == EMMFZoom ); 
       
    //
    // aspect ratio stretch
    //
    mFileDetails->mVideoHeight = 144;
    mFileDetails->mVideoWidth  = 220;

    aspectRatio = mDispHdlr->SetDefaultAspectRatioL( mFileDetails, displayAspectRatio );
    
    QVERIFY( aspectRatio == EMMFStretch ); 
 
    mDispHdlr->RemoveDisplayWindow();   

    cleanup(); 
}

void TestVideoDisplayHandler::testUpdateVideoRectL()
{
    setup();

    RWindow *window = mBaseVideoView->getWindow();

    TRect displayRect = TRect( 0, 0, 200, 300 );
    
    mDispHdlr->UpdateVideoRectL( displayRect, false );
    
    QCOMPARE( mDispHdlr->iWindowRect, displayRect);

    mDispHdlr->RemoveDisplayWindow();    
    cleanup(); 
}



// End of file
    


