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
* Description:   tester for methods in TestMPXVideoPlaybackControlPolicy
* 
*/

// Version : %version:  2 %


#include <qdebug>

#include <hbmainwindow.h>
#include <hbapplication.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "testmpxvideoplaybackcontrolpolicy.h"

#define private public
#include "mpxvideoplaybackcontrolpolicy.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlPolicy::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMPXVideoPlaybackControlPolicy tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testmpxvideoplaybackcontrolpolicy.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlPolicy::init()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlPolicy::init()"));

    mPolicy = new QMPXVideoPlaybackControlPolicy();
    mDetails = new QMPXVideoPlaybackViewFileDetails();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlPolicy::cleanup()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlPolicy::cleanup()"));

    if ( mPolicy )
    {
        delete mPolicy;
        mPolicy = NULL;
    }

    if ( mDetails )
    {
        delete mDetails;
        mDetails = NULL;
    }
}

// ---------------------------------------------------------------------------
// testBufferingAnimationPolicy
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlPolicy::testBufferingAnimationPolicy()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlPolicy::testBufferingAnimationPolicy()"));

    init();

    TUint properties = 0;

    mPolicy->setControlProperties( EMPXBufferingAnimation, properties, mDetails, EFullScreenView );

    QVERIFY( properties == ( EMPXShownWhenInitializing | EMPXShownWhenBuffering ) );

    cleanup();
}

// ---------------------------------------------------------------------------
// testStatusPanePolicy
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlPolicy::testStatusPanePolicy()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlPolicy::testStatusPanePolicy()"));

    init();

    TUint properties = 0;

    mDetails->mPlaybackMode = EMPXVideoLocal;

    mPolicy->setControlProperties( EMPXStatusPane, properties, mDetails, EFullScreenView );
    QVERIFY( properties == ( EMPXShownWhenPlaying | EMPXShownWhenPaused | EMPXShownWhenSeeking ) );

    mPolicy->setControlProperties( EMPXStatusPane, properties, mDetails, EDetailsView );
    QVERIFY( properties == EMPXAllProperties );

    cleanup();
}

// ---------------------------------------------------------------------------
// testControlBarPolicy
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlPolicy::testControlBarPolicy()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlPolicy::testControlBarPolicy()"));

    init();

    TUint properties = 0;

    mPolicy->setControlProperties( EMPXControlBar, properties, mDetails, EDetailsView );
    QVERIFY( properties == 0 );

    mPolicy->setControlProperties( EMPXControlBar, properties, mDetails, EAudioOnlyView );
    QVERIFY( properties == EMPXAllProperties );

    mPolicy->setControlProperties( EMPXControlBar, properties, mDetails, EFullScreenView );
    QVERIFY( properties == ( EMPXShownWhenPlaying | EMPXShownWhenPaused | EMPXShownWhenSeeking ) );

    cleanup();
}

// ---------------------------------------------------------------------------
// testDetailsWidgetPolicy
// ---------------------------------------------------------------------------
//
void TestMPXVideoPlaybackControlPolicy::testDetailsWidgetPolicy()
{
    MPX_ENTER_EXIT(_L("TestMPXVideoPlaybackControlPolicy::testDetailsWidgetPolicy()"));

    init();

    TUint properties = 0;

    mPolicy->setControlProperties( EMPXFileDetailsWidget, properties, mDetails, EAudioOnlyView );
    QVERIFY( properties == EMPXAllProperties );

    cleanup();
}

// End of file
