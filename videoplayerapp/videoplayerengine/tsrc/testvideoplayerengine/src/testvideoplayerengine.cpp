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

#include "mpxhbvideocommondefs.h"
#include "testvideoplayerengine.h"
#include "stub/inc/mpxviewpluginqt.h"
#include "stub/inc/videoservices.h"
#include "stub/inc/xqpluginloader.h"
#include "../stub/inc/mpxvideoplaybackwrapper.h"
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
}

TestVideoPlayerEngine::~TestVideoPlayerEngine()
{
    XQPluginLoader::cleanup();
    
    delete mTestObject;
    mTestObject = 0;
}


void TestVideoPlayerEngine::init(bool isService)
{
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
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
    QVERIFY(mTestObject->mPlaybackWrapper != 0);
    QVERIFY(mTestObject->mVideoServices == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    QVERIFY(mTestObject->mCollectionViewPlugin != mTestObject->mPlaybackViewPlugin);
    QVERIFY(mTestObject->mCollectionViewPlugin != mTestObject->mFileDetailsViewPlugin);
    QVERIFY(mTestObject->mPlaybackViewPlugin != mTestObject->mFileDetailsViewPlugin);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testInitializeService()
{
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
    QVERIFY(mTestObject->mPlaybackViewPlugin != mTestObject->mFileDetailsViewPlugin);

    cleanup();

    QVERIFY(VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testMultipleInitialize()
{
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
    QVERIFY(mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY(mTestObject->mCollectionViewPlugin != 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin != 0);
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
    
    mTestObject->handleCommand(MpxHbVideoCommon::LoadVideoDetailsView);

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
    
    mTestObject->handleCommand(MpxHbVideoCommon::LoadVideoDetailsView);

    QVERIFY(mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin);
    QVERIFY(mTestObject->mPlaybackViewPlugin == 0);
    QVERIFY(mTestObject->mCollectionViewPlugin == 0);
    QVERIFY(mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY(mTestObject->mIsService == false);
    QVERIFY(VideoServices::mReferenceCount == 0);

    mTestObject->initialize();

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
    init();    
    mTestObject->playMedia( QString("c:\\data\\videos\\test.3gp"));
    QVERIFY(mTestObject);
    cleanup();
}

void TestVideoPlayerEngine::testSetEmbedded()
{
    init();
    QVERIFY(mTestObject);
    mTestObject->setEmbedded();
    QVERIFY( mTestObject->mEmbedded == true );
    cleanup();
}

void TestVideoPlayerEngine::cleanupTestCase()
{
    // all common cleanup here
}


// End of file



