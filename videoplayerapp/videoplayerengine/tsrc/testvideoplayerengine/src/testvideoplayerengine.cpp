/*
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
* Description:  Videoplayerengine test class implementation.
*
*/

// Version : %version: %

// INCLUDES
#include <QtTest/QtTest>
#include <qdebug.h>
#include "stub/inc/hbinstance.h"
#include <hbwidget.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbapplication.h>
#include <qabstractitemmodel.h>
#include <xqsharablefile.h>
#include <f32file.h>
#include <qfile.h>

#include "mpxhbvideocommondefs.h"
#include "testvideoplayerengine.h"
#include "stub/inc/mpxviewpluginqt.h"
#include "stub/inc/videoservices.h"
#include "stub/inc/xqpluginloader.h"
#include "../stub/inc/mpxvideoplaybackwrapper.h"
#include "mpxvideo_debug.h"

#define private public
#include "videoplayerengine.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);

    TestVideoPlayerEngine tc;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestVideoPlayerEngine.txt";

    return QTest::qExec(&tc, 3, pass);
}


TestVideoPlayerEngine::TestVideoPlayerEngine()
	: mTestObject(0)
	, mVideoServices(0)
    , mCurrentViewPlugin(0)
    , mPlaybackViewPlugin(0)
    , mCollectionViewPlugin(0)
    , mFileDetailsViewPlugin(0)
{
    MPX_DEBUG(_L("TestVideoPlayerEngine::TestVideoPlayerEngine()"));
}

TestVideoPlayerEngine::~TestVideoPlayerEngine()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::~TestVideoPlayerEngine()"));
    
    XQPluginLoader::cleanup();
    
    delete mTestObject;
    mTestObject = 0;
}


void TestVideoPlayerEngine::init( bool isService )
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::init()"));
    
    XQPluginLoader::mFailToLoadPluginUid  = -1; 
    
    mCurrentViewPlugin     = 0;
    mPlaybackViewPlugin    = 0;
    mCollectionViewPlugin  = 0;
    mFileDetailsViewPlugin = 0;
    mVideoServices         = 0;

    mTestObject = new QVideoPlayerEngine(isService);
}

void TestVideoPlayerEngine::cleanup()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::cleanup()"));
    
    mCurrentViewPlugin     = 0;
    mPlaybackViewPlugin    = 0;
    mCollectionViewPlugin  = 0;
    mFileDetailsViewPlugin = 0;
    mVideoServices         = 0;

    delete mTestObject;
    mTestObject = 0;
}


void TestVideoPlayerEngine::testCreateDelete()
{
    MPX_DEBUG(_L("TestVideoServices::testCreateDelete()"));
    
    //Test object creation and deletion
	init();

    QVERIFY(mTestObject);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackWrapper == 0);    
    QVERIFY(mTestObject->mVideoServices == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}


void TestVideoPlayerEngine::testCreateDeleteService()
{
    MPX_DEBUG(_L("TestVideoServices::testCreateDeleteService()"));
    
    //Test object creation and deletion
	init(true);

    QVERIFY(mTestObject);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackWrapper == 0);    
    QVERIFY(mTestObject->mVideoServices == 0);
    QVERIFY(mTestObject->mIsService == true);
    QVERIFY(VideoServices::mReferenceCount == 0);

    cleanup();
    
    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testInitialize()
{
    MPX_DEBUG(_L("TestVideoServices::testInitialize()"));
    
    init();

    QVERIFY(mTestObject);
    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackWrapper == 0);
    QVERIFY(mTestObject->mVideoServices == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->initialize();

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackWrapper != 0);
    QVERIFY(mTestObject->mVideoServices == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    QVERIFY(mTestObject->mCollectionViewPlugin != mTestObject->mPlaybackViewPlugin);
    QVERIFY(mTestObject->mCollectionViewPlugin != mTestObject->mFileDetailsViewPlugin);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testInitializeService()
{
    MPX_DEBUG(_L("TestVideoServices::testInitializeService()"));
    
    init(true);

    QVERIFY(mTestObject);
    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackWrapper == 0);
    QVERIFY(mTestObject->mVideoServices == 0);
    QVERIFY(mTestObject->mIsService == true);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->initialize();

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackWrapper != 0);
    QVERIFY(mTestObject->mVideoServices != 0);
    QVERIFY(mTestObject->mIsService == true);
    QVERIFY(VideoServices::mReferenceCount == 1);

    QVERIFY(mTestObject->mCollectionViewPlugin != mTestObject->mPlaybackViewPlugin);
    QVERIFY(mTestObject->mCollectionViewPlugin != mTestObject->mFileDetailsViewPlugin);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testMultipleInitialize()
{
    MPX_DEBUG(_L("TestVideoServices::testMultipleInitialize()"));
    
	init();

    QVERIFY(mTestObject);
    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mVideoServices == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->initialize();

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mVideoServices == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mCurrentViewPlugin = mTestObject->mCurrentViewPlugin;
    mPlaybackViewPlugin = mTestObject->mPlaybackViewPlugin;
    mCollectionViewPlugin = mTestObject->mCollectionViewPlugin;
    mFileDetailsViewPlugin = mTestObject->mFileDetailsViewPlugin;

    mTestObject->initialize();

    QVERIFY(mCurrentViewPlugin == mTestObject->mCurrentViewPlugin);
    QVERIFY(mPlaybackViewPlugin == mTestObject->mPlaybackViewPlugin);
    QVERIFY(mCollectionViewPlugin == mTestObject->mCollectionViewPlugin);
    QVERIFY(mFileDetailsViewPlugin == mTestObject->mFileDetailsViewPlugin);
    QVERIFY(VideoServices::mReferenceCount == 0);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testMultipleInitializeService()
{
    MPX_DEBUG(_L("TestVideoServices::testMultipleInitializeService()"));
    
	init(true);

    QVERIFY(mTestObject);
    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mVideoServices == 0);
    QVERIFY(mTestObject->mIsService == true);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->initialize();

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mVideoServices != 0);
    QVERIFY(mTestObject->mIsService == true);
    QVERIFY(VideoServices::mReferenceCount == 1);

    mCurrentViewPlugin = mTestObject->mCurrentViewPlugin;
    mPlaybackViewPlugin = mTestObject->mPlaybackViewPlugin;
    mCollectionViewPlugin = mTestObject->mCollectionViewPlugin;
    mFileDetailsViewPlugin = mTestObject->mFileDetailsViewPlugin;
    mVideoServices = mTestObject->mVideoServices;

    mTestObject->initialize();

    QVERIFY(mCurrentViewPlugin == mTestObject->mCurrentViewPlugin);
    QVERIFY(mPlaybackViewPlugin == mTestObject->mPlaybackViewPlugin);
    QVERIFY(mCollectionViewPlugin == mTestObject->mCollectionViewPlugin);
    QVERIFY(mFileDetailsViewPlugin == mTestObject->mFileDetailsViewPlugin);
    QVERIFY(mVideoServices == mTestObject->mVideoServices);
    QVERIFY(VideoServices::mReferenceCount == 1);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}


void TestVideoPlayerEngine::testHandleCommandPreInit()
{
    MPX_DEBUG(_L("TestVideoServices::testHandleCommandPreInit()"));
    
	init();

    QVERIFY(mTestObject);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView);
    mTestObject->handleCommand(MpxHbVideoCommon::ActivatePlaybackView);
    mTestObject->handleCommand(MpxHbVideoCommon::ActivateVideoDetailsView);
    
    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testHandleCommandPostInit()
{
    MPX_DEBUG(_L("TestVideoServices::testHandleCommandPostInit()"));
    
    init();

    QVERIFY(mTestObject);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->initialize();

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);
    mTestObject->handleCommand(MpxHbVideoCommon::DoDelayedLoad);

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);
    QVERIFY(mTestObject->mCollectionViewPlugin->activated());
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivatePlaybackView);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mPlaybackViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated());
    QVERIFY(mTestObject->mCollectionViewPlugin->activated() == false);
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateVideoDetailsView);

    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);
    QVERIFY(mTestObject->mCollectionViewPlugin->activated() == false);
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated());
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);
    QVERIFY(mTestObject->mCollectionViewPlugin->activated());
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);
    
    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView-1111);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testHandleCommandPreAndPostInit()
{
    MPX_DEBUG(_L("TestVideoServices::testHandleCommandPreAndPostInit()"));
    
    init();

    QVERIFY(mTestObject);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView);
    mTestObject->handleCommand(MpxHbVideoCommon::ActivatePlaybackView);
    mTestObject->handleCommand(MpxHbVideoCommon::ActivateVideoDetailsView);
    
    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->initialize();


    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);
    mTestObject->handleCommand(MpxHbVideoCommon::DoDelayedLoad);

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);
    QVERIFY(mTestObject->mCollectionViewPlugin->activated());
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivatePlaybackView);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mPlaybackViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated());
    QVERIFY(mTestObject->mCollectionViewPlugin->activated() == false);
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateVideoDetailsView);

    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);
    QVERIFY(mTestObject->mCollectionViewPlugin->activated() == false);
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated());
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);
    QVERIFY(mTestObject->mCollectionViewPlugin->activated());
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testHandleCommandPluginFail()
{
    MPX_DEBUG(_L("TestVideoServices::testHandleCommandPluginFail()"));
    
    init();

    QVERIFY(mTestObject);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    XQPluginLoader::mFailToLoadPluginUid = MpxHbVideoCommon::KMpxVideoPluginDllFileDetailsUid;

    mTestObject->initialize();

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);
    mTestObject->handleCommand(MpxHbVideoCommon::DoDelayedLoad);

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);

	QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mCollectionViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivatePlaybackView);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mPlaybackViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated());
    QVERIFY(mTestObject->mCollectionViewPlugin->activated() == false);

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateVideoDetailsView);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);

    QVERIFY(mTestObject->mCollectionViewPlugin->activated() == false);
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mCollectionViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testHandleCommandPluginFail1()
{
    MPX_DEBUG(_L("TestVideoServices::testHandleCommandPluginFail1()"));
    
    init();

    QVERIFY(mTestObject);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    XQPluginLoader::mFailToLoadPluginUid = MpxHbVideoCommon::KMpxVideoPluginDllCollectionUid;

    mTestObject->initialize();

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);
    mTestObject->handleCommand(MpxHbVideoCommon::DoDelayedLoad);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);
    
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivatePlaybackView);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mPlaybackViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated());
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView);

    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateVideoDetailsView);

    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mPlaybackViewPlugin->activated()  == false);
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated());

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView);

    QVERIFY(mTestObject->mPlaybackViewPlugin->activated() == false);
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testHandleCommandPluginFail2()
{
    MPX_DEBUG(_L("TestVideoServices::testHandleCommandPluginFail2()"));
    
    init();

    QVERIFY(mTestObject);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    XQPluginLoader::mFailToLoadPluginUid = MpxHbVideoCommon::KMpxVideoPluginDllPlaybackUid;
    
 	mTestObject->initialize();

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);
    mTestObject->handleCommand(MpxHbVideoCommon::DoDelayedLoad);

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

	QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mCollectionViewPlugin->activated());
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivatePlaybackView);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);

    QVERIFY(mTestObject->mCollectionViewPlugin->activated() == false);
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);

    QVERIFY(mTestObject->mCurrentViewPlugin == 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin);

    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mCollectionViewPlugin->activated());
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated() == false);

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

	mTestObject->handleCommand(MpxHbVideoCommon::ActivateVideoDetailsView);

    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin);

    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mCurrentViewPlugin->activated());
    QVERIFY(mTestObject->mCollectionViewPlugin->activated() == false);
    QVERIFY(mTestObject->mFileDetailsViewPlugin->activated());

    QVERIFY(mTestObject->mCurrentViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testPlayMedia()
{
    MPX_DEBUG(_L("TestVideoServices::testPlayMedia()"));
    
	  // 1. test with a string
    init();    
    mTestObject->playMedia( QString("c:\\data\\videos\\test.3gp"));
    QVERIFY(mTestObject);
    cleanup();
    
    // 2. test with a file handle
    init();
    XQSharableFile sf;
    if ( sf.open("C:\\data\\videos\\test.3gp") )
    {
        RFile rFile;
        bool handleOK = sf.getHandle( rFile );  
        if ( handleOK )
        {
            mTestObject->playMedia( rFile );
        }    
         
        QVERIFY(mTestObject);
        
        sf.close();
    }
    cleanup();

    // 3. test with a filename
    init();
    QFile qfile("C:\\data\\videos\\test.3gp");
    mTestObject->playMedia( qfile.fileName() );
    QVERIFY(mTestObject);
    cleanup();
    
}

void TestVideoPlayerEngine::testSetEmbedded()
{
    MPX_DEBUG(_L("TestVideoServices::testSetEmbedded()"));
    
    init();
    QVERIFY(mTestObject);
    mTestObject->setEmbedded();
    QVERIFY( mTestObject->mEmbedded == true );
    cleanup();
}

void TestVideoPlayerEngine::cleanupTestCase()
{
    MPX_DEBUG(_L("TestVideoServices::cleanupTestCase()"));    
    // all common cleanup here
}

// End of file
