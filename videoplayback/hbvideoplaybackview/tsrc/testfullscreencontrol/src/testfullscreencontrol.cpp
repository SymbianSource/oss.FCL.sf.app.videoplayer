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

// Version : %version:  1 %


#include <hbapplication.h>
#include <hbinstance.h>
#include <qdebug>

#include "mpxhelixplaybackplugindefs.h"

#include "testfullscreencontrol.h"

#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxvideoplaybackbuttonbar.h"


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

    mFileDetails = new QMPXVideoPlaybackViewFileDetails();  
    
    mControlsController = new QMPXVideoPlaybackControlsController( mFileDetails );
    
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
        case EMPXProgressBar:
        {            
            QMPXVideoPlaybackProgressBar *progressBar = 
                    new QMPXVideoPlaybackProgressBar( mControlsController );            

            mFullScreenControl = new QMPXVideoPlaybackFullScreenControl( mControlsController,
                                                                         control,
                                                                         progressBar,
                                                                         0 );            
        
            break; 
        }
        case EMPXButtonBar:
        {
            QMPXVideoPlaybackButtonBar *buttonBar = 
                    new QMPXVideoPlaybackButtonBar( mControlsController );            
    
            mFullScreenControl = new QMPXVideoPlaybackFullScreenControl( mControlsController,
                                                                         control,
                                                                         buttonBar,
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
// main
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::testVolumeChanged()
{  
    setup( EMPXStatusPane );
    
    QVERIFY( ! mFullScreenControl->volumeChanged( 50 ) );
        
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
    setup( EMPXProgressBar );
    QVERIFY( mFullScreenControl->durationChanged( 100 ) );      
    QVERIFY( static_cast<QMPXVideoPlaybackProgressBar*>(mFullScreenControl->mControl)->mDuration == 100 );       
    
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
    setup( EMPXProgressBar );
    QVERIFY( mFullScreenControl->positionChanged( 100 ) );      
    QVERIFY( static_cast<QMPXVideoPlaybackProgressBar*>(mFullScreenControl->mControl)->mPosition == 100 );      
    
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
    setup( EMPXButtonBar );        
    QVERIFY( mFullScreenControl->aspectRatioChanged( 3 ) );  // EMMFNatural
    QVERIFY( static_cast<QMPXVideoPlaybackButtonBar*>(mFullScreenControl->mControl)->mAspectRatio == 3 ); 
    
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
    setup( EMPXButtonBar );     
    mFullScreenControl->updateState( EPbStatePaused ); 
    QVERIFY( static_cast<QMPXVideoPlaybackButtonBar*>(mFullScreenControl->mControl)->mState == EPbStatePaused );         
}

// ---------------------------------------------------------------------------
// testUpdateControlsWithFileDetails
// ---------------------------------------------------------------------------
//
void TestFullScreenControl::testUpdateControlsWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestFullScreenControl::testUpdateControlsWithFileDetails()"));

    delete mFullScreenControl;
    mFullScreenControl = NULL;     
    setup( EMPXButtonBar );
    
    mFileDetails->mPausableStream = true;
    
    mFullScreenControl->updateControlsWithFileDetails(mFileDetails);
    
    QVERIFY( static_cast<QMPXVideoPlaybackButtonBar*>(mFullScreenControl->mControl)->mShowPauseButton );    
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
