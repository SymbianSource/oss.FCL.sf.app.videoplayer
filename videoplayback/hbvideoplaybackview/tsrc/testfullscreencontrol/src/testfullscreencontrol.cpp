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
* Description:   tester for methods in QMPXVideoPlaybackFullScreenControl
* 
*/

// Version : %version:  3 %


#include <hbapplication.h>
#include <hbinstance.h>
#include <qdebug>

#include "mpxhelixplaybackplugindefs.h"

#include "testfullscreencontrol.h"

#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackcontrolbar.h"


#define private public
#define protected public
#include "mpxvideoplaybackfullscreencontrol.h"
#undef protected
#undef private


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;
    
    TestFullScreenControl tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestFullScreenControl.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::init()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::init()"));
}

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::setup(TMPXVideoPlaybackControls control)
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::setup()"));


    mControlsController = new QMPXVideoPlaybackControlsController();
    
    createControl( control );                                                                                                                                           
}

// ---------------------------------------------------------------------------
// createControl
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::createControl(TMPXVideoPlaybackControls control)
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::createControl()"));

    switch ( control )
    {
        case EMPXStatusPane:
        {
            mFullScreenControl = new QMPXVideoPlaybackFullScreenControl( mControlsController, 
                                                                         control,
                                                                         NULL, 
                                                                         0 );         
            break;    
        }
        case EMPXControlBar:
        {
            QMPXVideoPlaybackControlBar *controlBar = 
                    new QMPXVideoPlaybackControlBar( mControlsController );            
    
            mFullScreenControl = new QMPXVideoPlaybackFullScreenControl( mControlsController,
                                                                         control,
                                                                         controlBar,
                                                                         0 );            
             
            break;
        }
        case EMPXDetailsViewPlaybackWindow:
        {
            break;    
        }
    }    
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::cleanup()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::cleanup()"));

    if ( mControlsController )
    {
        delete mControlsController;
        mControlsController = NULL;    
    }
    
    if ( mFullScreenControl )
    {
        delete mFullScreenControl;
        mFullScreenControl = NULL;    
    }
}

// ---------------------------------------------------------------------------
// testControlIndex
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::testControlIndex()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testControlIndex()"));

    setup( EMPXStatusPane );
    
    QVERIFY( mFullScreenControl->controlIndex() == EMPXStatusPane );
}

// ---------------------------------------------------------------------------
// testDurationChanged
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::testDurationChanged()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testDurationChanged()"));

    // test for progress bar
    delete mFullScreenControl;
    mFullScreenControl = NULL;    
    setup( EMPXControlBar );
    QVERIFY( mFullScreenControl->durationChanged( 100 ) );      
    QVERIFY( static_cast<QMPXVideoPlaybackControlBar*>(mFullScreenControl->mControl)->mDuration == 100 );       
    
    // test for something other than progress bar
    delete mFullScreenControl;
    mFullScreenControl = NULL;
    setup( EMPXStatusPane );    
    QVERIFY( ! mFullScreenControl->durationChanged( 100 ) );    
            
}

// ---------------------------------------------------------------------------
// testPositionChanged
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::testPositionChanged()
{ 
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testPositionChanged()"));

    // test for progress bar
    delete mFullScreenControl;
    mFullScreenControl = NULL;    
    setup( EMPXControlBar );
    QVERIFY( mFullScreenControl->positionChanged( 100 ) );      
    QVERIFY( static_cast<QMPXVideoPlaybackControlBar*>(mFullScreenControl->mControl)->mPosition == 100 );      
    
    // test for something other than progress bar
    delete mFullScreenControl;
    mFullScreenControl = NULL;
    setup( EMPXStatusPane );    
    QVERIFY( ! mFullScreenControl->positionChanged( 100 ) );
}

// ---------------------------------------------------------------------------
// testAspectRatioChanged
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::testAspectRatioChanged()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testAspectRatioChanged()"));

    // test for button bar
    delete mFullScreenControl;
    mFullScreenControl = NULL;    
    setup( EMPXControlBar );        
    QVERIFY( mFullScreenControl->aspectRatioChanged( 3 ) );  // EMMFNatural
    QVERIFY( static_cast<QMPXVideoPlaybackControlBar*>(mFullScreenControl->mControl)->mAspectRatio == 3 ); 
    
    // test for something other than button bar
    delete mFullScreenControl;
    mFullScreenControl = NULL;    
    setup( EMPXStatusPane );        
    QVERIFY( ! mFullScreenControl->aspectRatioChanged( 3 ) );  // EMMFNatural
}

// ---------------------------------------------------------------------------
// testUpdateState
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::testUpdateState()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testUpdateState()"));

    delete mFullScreenControl;
    mFullScreenControl = NULL;    
    setup( EMPXControlBar );     
    mFullScreenControl->updateState( EPbStatePaused ); 
    QVERIFY( static_cast<QMPXVideoPlaybackControlBar*>(mFullScreenControl->mControl)->mState == EPbStatePaused );         
}

// ---------------------------------------------------------------------------
// testUpdateControlsWithFileDetails
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::testUpdateControlsWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testUpdateControlsWithFileDetails()"));

    QMPXVideoPlaybackViewFileDetails *fileDetails = new QMPXVideoPlaybackViewFileDetails(); 
    
    setup( EMPXControlBar );
    
    fileDetails->mPausableStream = true;
    
    mFullScreenControl->updateControlsWithFileDetails( fileDetails );
    
    QVERIFY( static_cast<QMPXVideoPlaybackControlBar*>(mFullScreenControl->mControl)->mFileDetails->mPausableStream );    

    if ( fileDetails )
    {
        delete fileDetails;
        fileDetails = NULL;    
    }
}

// ---------------------------------------------------------------------------
// testUpdateControlProperties
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::testUpdateControlProperties()
{  
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testUpdateControlProperties()"));

    setup(EMPXStatusPane);
    
    mFullScreenControl->updateControlProperties( 1 );
    
    QVERIFY( mFullScreenControl != NULL );
    
    QVERIFY( mFullScreenControl->mProperties == 1 );
}
        
// End of file
