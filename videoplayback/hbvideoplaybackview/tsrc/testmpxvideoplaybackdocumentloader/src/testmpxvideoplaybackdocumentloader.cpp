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
* Description:   tester for methods in TestMPXVideoPlaybackDocumentLoader
* 
*/

// Version : %version:  5 %


#include <qdebug>
#include <qobject>

#include <hbmainwindow.h>
#include <hbapplication.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "testmpxvideoplaybackdocumentloader.h"

#define private public
#include "mpxvideoplaybackdocumentloader.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackDocumentLoader::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMPXVideoPlaybackDocumentLoader tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testmpxvideoplaybackdocumentloader.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackDocumentLoader::init()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackDocumentLoader::init()"));

    mController = new QMPXVideoPlaybackControlsController();
    mLoader = new QMPXVideoPlaybackDocumentLoader( mController );
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackDocumentLoader::cleanup()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackDocumentLoader::cleanup()"));

    if ( mLoader )
    {
        delete mLoader;
        mLoader = NULL;
    }

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }
}

// ---------------------------------------------------------------------------
// testCreateObject
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackDocumentLoader::testCreateObject()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackDocumentLoader::testCreateObject()"));

    init();

    QObject *object = mLoader->createObject( "", "controlBarLayout" );
    QVERIFY( object->objectName() == "controlBarLayout" );

    object = mLoader->createObject( "", "progressBarLayout" );
    QVERIFY( object->objectName() == "progressBarLayout" );

    object = mLoader->createObject( "", "fileDetailsLayout" );
    QVERIFY( object->objectName() == "fileDetailsLayout" );

    object = mLoader->createObject( "", "detailsPlaybackWindow" );
    QVERIFY( object->objectName() == "detailsPlaybackWindow" );

    cleanup();
}

// End of file
