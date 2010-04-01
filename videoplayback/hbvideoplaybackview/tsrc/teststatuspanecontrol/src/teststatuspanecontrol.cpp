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
* Description:   tester for methods in status pane control
* 
*/

// Version : %version:  4 %


#include <hbapplication.h>
#include <hbinstance.h>
#include <hbmenu.h>
#include <QDebug>


#include "mpxvideoplaybackcontrol.hrh"
#include "mpxhelixplaybackplugindefs.h"

#include "teststatuspanecontrol.h"

#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "hbvideobaseplaybackview.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "hblabel.h"

#define private public
#define protected public
#include "mpxvideoplaybackstatuspanecontrol.h"
#undef protected
#undef private


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;
    
    TestStatusPaneControl tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestStatusPaneControl.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::init()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::init()"));
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::setup()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::setup()"));

    mBaseVideoView    = new HbVideoBasePlaybackView();
        
    mFileDetails = new QMPXVideoPlaybackViewFileDetails();  
    
    mControlsController = new QMPXVideoPlaybackControlsController( mBaseVideoView, 
                                                                   mFileDetails );
                                                                            
    mStatusPane = new QMPXVideoPlaybackStatusPaneControl( mControlsController, 
                                                          EMPXStatusPane,
                                                          NULL, 
                                                          0 );                                                                

}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::cleanup()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::cleanup()"));

    if ( mBaseVideoView )
    {
        delete mBaseVideoView;
        mBaseVideoView = NULL;    
    }
    
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
    
    if ( mStatusPane )
    {
        delete mStatusPane;
        mStatusPane = NULL;    
    }
}

// ---------------------------------------------------------------------------
// testSetMenu
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::testSetMenu()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSetMenu()"));

    setup();    
    
    mFileDetails->mTvOutConnected = false;
    mFileDetails->mVideoEnabled = true;
        
    mStatusPane->setMenu( mFileDetails );
    
    QVERIFY( mControlsController->view()->menu()->isEmpty() );
}

// ---------------------------------------------------------------------------
// testSetVisible
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::testSetVisible()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSetVisible()"));

    setup();    
    
    // 1. Set visible
    mStatusPane->setVisible( true );    
    QVERIFY( mStatusPane->isVisible() );  
    QVERIFY( mStatusPane->mTitleLabel->isVisible() ); 
    
    // 2. Set invisible
    mStatusPane->setVisible( false );    
    QVERIFY( ! mStatusPane->isVisible() );  
    QVERIFY( ! mStatusPane->mTitleLabel->isVisible() );     
    
}

// ---------------------------------------------------------------------------
// testUpdateControlsWithFileDetails
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::testUpdateControlsWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testUpdateControlsWithFileDetails()"));

    setup(); 
    
    // 1. test for full creen mode
    mControlsController->mViewMode = EFullScreenView;
    
    mFileDetails->mTvOutConnected = true;
    
    mStatusPane->updateControlsWithFileDetails( mFileDetails );
    
    QVERIFY( mControlsController->view()->mTitleFlag == HbView::TitleBarTransparent );
    
    // 2. test for detial view mode    
    mControlsController->mViewMode = EDetailsView;
    
    mFileDetails->mTvOutConnected = true;
    
    mStatusPane->updateControlsWithFileDetails( mFileDetails );

    QVERIFY( mControlsController->view()->mTitleFlag == HbView::TitleBarFlagNone );

    // 3. test for audio only mode    
    mControlsController->mViewMode = EAudioOnlyView;
    
    mFileDetails->mTvOutConnected = true;
    
    mStatusPane->updateControlsWithFileDetails( mFileDetails );

    QVERIFY( mControlsController->view()->mTitleFlag == HbView::TitleBarFlagNone );
}

// ---------------------------------------------------------------------------
// testSetVisibility
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::testSetVisibility()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSetVisibility()"));

    setup();    
    
    // 1. test for first block of cases:
    mStatusPane->setVisibility( EPbStatePaused );        
    QVERIFY( mControlsController->view()->menu()->isEmpty() ); 
    
    // 2. test for second block of cases:
    mStatusPane->setVisibility( EPbStateInitialising );        
    QVERIFY( mControlsController->view()->menu()->isEmpty() );
}

// ---------------------------------------------------------------------------
// testControlListUpdated
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::testControlListUpdated()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testControlListUpdated()"));

    setup();    
    
    mStatusPane->controlListUpdated( mFileDetails );    
        
    QVERIFY( mControlsController->view()->menu()->isEmpty() );
}

// ---------------------------------------------------------------------------
// testSlot_handleAboutToShow
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::testSlot_handleAboutToShow()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSlot_handleAboutToShow()"));

    setup();    
    
    connect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( handleAboutToShow() ) );
    
    emit commandSignal();     
    
    QVERIFY( mControlsController->mTimerAction == EMPXTimerCancel ); 
    
    disconnect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( handleAboutToShow() ) );
}

// ---------------------------------------------------------------------------
// testSlot_handleAboutToHide
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::testSlot_handleAboutToHide()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSlot_handleAboutToHide()"));

    setup();    
    
    connect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( handleAboutToHide() ) );
    
    emit commandSignal();     
    
    QVERIFY( mControlsController->mTimerAction == EMPXTimerReset ); 
    
    disconnect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( handleAboutToHide() ) );    
}

// ---------------------------------------------------------------------------
// testSlot_openFullScreenView
// ---------------------------------------------------------------------------
//
void TestStatusPaneControl::testSlot_openFullScreenView()
{
    MPX_ENTER_EXIT(_L("TestStatusPaneControl::testSlot_openFullScreenView()"));

    setup();    
    
    connect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( openFullScreenView() ) );
    
    emit commandSignal();     
    
    QVERIFY( mControlsController->mViewMode  == EFullScreenView ); 
    
    disconnect( this, SIGNAL( commandSignal() ), mStatusPane, SLOT( openFullScreenView() ) );
}

// End of file
