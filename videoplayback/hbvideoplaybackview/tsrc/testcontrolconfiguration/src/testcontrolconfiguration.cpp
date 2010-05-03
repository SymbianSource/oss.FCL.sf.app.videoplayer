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
* Description:   tester for methods in VideoSortFilterProxyModel
* 
*/

// Version : %version: %

#include <hbapplication.h>
#include <hbinstance.h>
#include <qdebug>
#include <qgraphicswidget>

#include "mpxvideoplaybackcontrol.hrh"
#include "mpxhelixplaybackplugindefs.h"

#include "testcontrolconfiguration.h"

#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackdocumentloader.h"


#define private public
#include "mpxvideoplaybackcontrolconfiguration.h"
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
    pass[2] = "c:\\data\\TestControlConfiguration.txt";
    
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

    mFileDetails = new QMPXVideoPlaybackViewFileDetails();  
    
    mControlsController = new QMPXVideoPlaybackControlsController( mFileDetails );
                                                                     
    mControlConfig = new QMPXVideoPlaybackControlConfiguration( mControlsController );
    mControlConfig->createControlList();
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
    
    QList<TMPXVideoPlaybackControls> controlsList = mControlConfig->controlList();
    
    QVERIFY( controlsList.contains( EMPXStatusPane ) );
        
}

// ---------------------------------------------------------------------------
// testUpdateControlsWithFileDetails
// ---------------------------------------------------------------------------
//
void TestControlConfiguration::testUpdateControlsWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestControlConfiguration::testUpdateControlsWithFileDetails()"));

    setup();
    
    // 1. test with mVideoEnabled = false
    mControlsController->mFileDetails->mVideoEnabled = false;
    
    mControlConfig->updateControlsWithFileDetails();
    
    QList<TMPXVideoPlaybackControls> controlsList = mControlConfig->controlList();
    
    QVERIFY( controlsList.contains( EMPXControlBar ) );
    
    // 2. test with mVideoEnabled = true
    mControlsController->mFileDetails->mVideoEnabled = false;
    
    mControlConfig->updateControlsWithFileDetails(); 
    
    QVERIFY( controlsList.contains( EMPXControlBar ) );
    
    QGraphicsWidget *widget = 
            mControlsController->layoutLoader()->findWidget( QString( "transparentWindow" ) );

    QVERIFY( widget->isVisible() );
}

// ---------------------------------------------------------------------------
// testUpdateControlList
// ---------------------------------------------------------------------------
//
void TestControlConfiguration::testUpdateControlList()
{
    MPX_ENTER_EXIT(_L("TestControlConfiguration::testUpdateControlList()"));

    setup();    
    
    // 1.  Test for Details View
    mControlConfig->updateControlList( EMPXControlCmdDetailsViewOpened );

    QList<TMPXVideoPlaybackControls> controlsList = mControlConfig->controlList();

    QVERIFY( controlsList.contains( EMPXFileDetailsWidget ) );    


    // 2. Test for Fullscreen View
    mControlConfig->updateControlList( EMPXControlCmdFullScreenViewOpened );

    controlsList = mControlConfig->controlList();
    
    QVERIFY( ! controlsList.contains( EMPXFileDetailsWidget ) );    
    
    
    // 3. Test for Audio Only View
    mControlConfig->updateControlList( EMPXControlCmdAudionOnlyViewOpened );

    controlsList = mControlConfig->controlList();
    
    QVERIFY( ! controlsList.contains( EMPXDetailsViewPlaybackWindow ) ); 
    QVERIFY( controlsList.contains( EMPXIndicatorBitmap ) );        
}

// End of file
