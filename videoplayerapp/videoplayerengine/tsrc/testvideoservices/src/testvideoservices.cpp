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
#include <hbapplication.h>

#include "mpxhbvideocommondefs.h"
#include "videoplayerengine.h"
#include "testvideoservices.h"

#define private public
#include "videoservices.h"
#include "videoserviceurifetch.h"
#include "videoserviceplay.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);

    TestVideoServices tc;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestVideoServices.txt";

    return QTest::qExec(&tc, 3, pass);
}


TestVideoServices::TestVideoServices()
	: mTestObject(0),
	  mEngine(0)
{
}

TestVideoServices::~TestVideoServices()
{
    delete mTestObject;
    mTestObject = 0;
}


void TestVideoServices::init()
{
}

void TestVideoServices::cleanup()
{
    delete mEngine;
    mEngine = 0;
	
    QCOMPARE(mTestObject->mReferenceCount, 1);
	if(mTestObject)
	{
		mTestObject->decreaseReferenceCount();
	}
	mTestObject = 0;
}

void TestVideoServices::testCreateDelete()
{
	mTestObject = VideoServices::instance();

    QVERIFY(mTestObject);
    QVERIFY(mTestObject->mServicePlay);
    QVERIFY(mTestObject->mServiceUriFetch);
    QVERIFY(mTestObject->mEngine == 0);
    QVERIFY(mTestObject->mCurrentService == VideoServices::ENoService);
}

void TestVideoServices::testCreateDeleteEngine()
{
	mEngine = new QVideoPlayerEngine();
	mTestObject = VideoServices::instance(mEngine);

    QVERIFY(mTestObject);
    QVERIFY(mTestObject->mServicePlay);
    QVERIFY(mTestObject->mServiceUriFetch);
    QVERIFY(mTestObject->mEngine == mEngine);
    QVERIFY(mTestObject->mCurrentService == VideoServices::ENoService);
}


void TestVideoServices::testCurrentService()
{
	mTestObject = VideoServices::instance();

    QVERIFY(mTestObject);
    QVERIFY(mTestObject->mServicePlay);
    QVERIFY(mTestObject->mServiceUriFetch);
    QVERIFY(mTestObject->mEngine == 0);
    QVERIFY(mTestObject->mCurrentService == VideoServices::ENoService);

    mTestObject->mServicePlay->playMedia(QString());
    QVERIFY(mTestObject->mCurrentService == VideoServices::ENoService);

    mTestObject->mServiceUriFetch->fetch(QString());
    QVERIFY(mTestObject->mCurrentService == VideoServices::EUriFetcher);
    
    mTestObject->mServicePlay->playMedia(QString());
    QVERIFY(mTestObject->mCurrentService == VideoServices::EUriFetcher);
    
	cleanup();

	mEngine = new QVideoPlayerEngine();
	mTestObject = VideoServices::instance(mEngine);

	QVERIFY(mTestObject);
    QVERIFY(mTestObject->mServicePlay);
    QVERIFY(mTestObject->mServiceUriFetch);
    QVERIFY(mTestObject->mEngine == mEngine);
    QVERIFY(mTestObject->mCurrentService == VideoServices::ENoService);

    mTestObject->mServiceUriFetch->fetch(QString());
    QVERIFY(mTestObject->mCurrentService == VideoServices::EUriFetcher);
    
    mTestObject->mServicePlay->playMedia(QString());
    QVERIFY(mTestObject->mCurrentService == VideoServices::EPlayback);
}

void TestVideoServices::cleanupTestCase()
{
    // all common cleanup here
}


// End of file



