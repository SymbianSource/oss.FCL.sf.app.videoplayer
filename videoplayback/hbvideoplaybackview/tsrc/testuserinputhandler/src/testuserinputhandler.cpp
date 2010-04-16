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
* Description:   tester for methods in User Input Handler
* 
*/

#include <e32err.h>
#include <w32std.h>
#include <eikenv.h>

#include <hbapplication.h>
#include <hbinstance.h>
#include <QDebug>

#include "testuserinputhandler.h"
#include "../stub/inc/mpxvideoviewwrapper.h"
#include "../stub/inc/hbvideobaseplaybackview.h"

#define private public
#include "mpxvideoplaybackuserinputhandler.h"
#undef private


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    
    TestUserInputHandler tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestUserInputHandler.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestUserInputHandler::init()
{	     
    mBaseVideoView    = new HbVideoBasePlaybackView();
    mVideoViewWrapper = CMPXVideoViewWrapper::NewL( mBaseVideoView );   
        
    mUserInputHdlr = CMPXVideoPlaybackUserInputHandler::NewL(mVideoViewWrapper, false);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestUserInputHandler::cleanup()
{           
    delete mUserInputHdlr;
    mUserInputHdlr = NULL;  
    
    delete mVideoViewWrapper;
    mVideoViewWrapper = NULL;
        
    delete mBaseVideoView;
    mBaseVideoView = NULL;              
}

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------
//
void TestUserInputHandler::setup()
{    
}

void TestUserInputHandler::testProcessMediaKeyPlay()
{    
    init();
    
    mUserInputHdlr->iProcessingInputType = EMpxVideoNone;    
    
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonPress);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoMediaKeys );
    
    // this event should be ignored as the 1st event is still being processed
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPause, ERemConCoreApiButtonPress);
    
    // verify that "Play" is still being processed and not "Pause"
    QVERIFY( mUserInputHdlr->iLastMediaKeyPressed == ERemConCoreApiPlay );
    
    // send the release event for "Play"
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonRelease);
    
    // verify that no input is now being processed
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );
    
    cleanup();
}

void TestUserInputHandler::testProcessMediaKeyPause()
{    
    init();
    
    mUserInputHdlr->iProcessingInputType = EMpxVideoNone;    
    
    // Issue Play
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonPress);
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonRelease);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );

    // Initiate Pause
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPause, ERemConCoreApiButtonPress);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoMediaKeys );    
    QVERIFY( mUserInputHdlr->iLastMediaKeyPressed == ERemConCoreApiPause );    
    
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPause, ERemConCoreApiButtonRelease); 
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );
    
    cleanup();    
    
}

void TestUserInputHandler::testProcessMediaKeyPlayPause()
{    
    init();
    
    mUserInputHdlr->iProcessingInputType = EMpxVideoNone;    
    
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPausePlayFunction, ERemConCoreApiButtonClick);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );      
    
    // set to background
    mUserInputHdlr->iForeground = false;
    
    mUserInputHdlr->iLastMediaKeyPressed = ENop;
    // this event should be ignored as iForeground is false
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPausePlayFunction, ERemConCoreApiButtonPress);
    QVERIFY( mUserInputHdlr->iLastMediaKeyPressed == ENop );
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );     
    
    cleanup();       
}

void TestUserInputHandler::testProcessMediaKeyStop()
{    
    init();
    
    mUserInputHdlr->iProcessingInputType = EMpxVideoNone;    
    
    // Issue Play
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonPress);
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonRelease);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );

    // Stop
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiStop, ERemConCoreApiButtonPress);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoMediaKeys );    
    QVERIFY( mUserInputHdlr->iLastMediaKeyPressed == ERemConCoreApiStop );    
    
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiStop, ERemConCoreApiButtonRelease); 
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );
    
    cleanup();     
}

void TestUserInputHandler::testProcessMediaKeyForward()
{    
    init();
    
    mUserInputHdlr->iProcessingInputType = EMpxVideoNone;    
    
    // Issue Play
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonPress);
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonRelease);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );

    // Forward
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiFastForward, ERemConCoreApiButtonPress);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoMediaKeys );    
    QVERIFY( mUserInputHdlr->iLastMediaKeyPressed == ERemConCoreApiFastForward );    
    
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiFastForward, ERemConCoreApiButtonRelease); 
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );
    
    cleanup();     
}

void TestUserInputHandler::testProcessMediaKeyRewind()
{    
    init();
    
    mUserInputHdlr->iProcessingInputType = EMpxVideoNone;    
    
    // Issue Play
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonPress);
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonRelease);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );

    // Forward
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiFastForward, ERemConCoreApiButtonPress);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoMediaKeys );    
    QVERIFY( mUserInputHdlr->iLastMediaKeyPressed == ERemConCoreApiFastForward );    
    
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiFastForward, ERemConCoreApiButtonRelease); 
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );
    
    // Rewind
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiRewind, ERemConCoreApiButtonPress);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoMediaKeys );    
    QVERIFY( mUserInputHdlr->iLastMediaKeyPressed == ERemConCoreApiRewind );    
    
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiRewind, ERemConCoreApiButtonRelease); 
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );
    
    cleanup();     
}

void TestUserInputHandler::testProcessMediaKeyVolumeUp()
{    
    init();
    
    mUserInputHdlr->iProcessingInputType = EMpxVideoNone;    
    
    // Issue Play
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonPress);
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonRelease);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );
    
    // Volume Up
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiVolumeUp, ERemConCoreApiButtonPress);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoMediaKeys );    
    QVERIFY( mUserInputHdlr->iLastMediaKeyPressed == ERemConCoreApiVolumeUp );   
    QVERIFY( mUserInputHdlr->iVolumeRepeatTimer->IsActive() );
    QVERIFY( mUserInputHdlr->iVolumeRepeatUp );
    
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiVolumeUp, ERemConCoreApiButtonRelease); 
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );   
    QVERIFY( ! mUserInputHdlr->iVolumeRepeatTimer->IsActive() );
    
    cleanup();        
}

void TestUserInputHandler::testProcessMediaKeyVolumeDown()
{    
    init();
    
    mUserInputHdlr->iProcessingInputType = EMpxVideoNone;    
    
    // Issue Play
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonPress);
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiPlay, ERemConCoreApiButtonRelease);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );
    
    // Volume Down
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiVolumeDown, ERemConCoreApiButtonPress);
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoMediaKeys );    
    QVERIFY( mUserInputHdlr->iLastMediaKeyPressed == ERemConCoreApiVolumeDown );   
    QVERIFY( mUserInputHdlr->iVolumeRepeatTimer->IsActive() );
    QVERIFY( ! mUserInputHdlr->iVolumeRepeatUp );
    
    mUserInputHdlr->ProcessMediaKey(ERemConCoreApiVolumeDown, ERemConCoreApiButtonRelease); 
    QVERIFY( mUserInputHdlr->iProcessingInputType == EMpxVideoNone );   
    QVERIFY( ! mUserInputHdlr->iVolumeRepeatTimer->IsActive() );    
    
    cleanup();     
}

void TestUserInputHandler::testHandleTVOutEventL()
{   
    
    ///////////////////////////////
    // 1. HandleTVOutEventL(true)
    ///////////////////////////////
    init();
    
    mUserInputHdlr->iDisplayTimeOut = 0;
    
    mUserInputHdlr->HandleTVOutEventL(true);
    
    QVERIFY( mUserInputHdlr->iDisplayTimeOut != 0 ); 
    QVERIFY( mUserInputHdlr->iDisplayTimer->IsActive() );
        
    cleanup();
    
    
    ///////////////////////////////
    // 2. HandleTVOutEventL(false)
    ///////////////////////////////
    init();
    
    mUserInputHdlr->iDisplayTimeOut = 0;
    
    mUserInputHdlr->HandleTVOutEventL(false);   
    QVERIFY( ! mUserInputHdlr->iDisplayTimer->IsActive() );    
        
    cleanup();    
        
}




// End of file
    


