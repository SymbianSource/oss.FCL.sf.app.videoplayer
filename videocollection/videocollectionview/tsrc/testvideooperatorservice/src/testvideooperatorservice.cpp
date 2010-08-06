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
* Description: tests for VideoOperatorService
*
*/

#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <qdebug.h>

#include "testvideooperatorservice.h"
#include "videocollectionviewutilsdata.h"

#define private public
#include "videooperatorservice.h"
#include "videooperatorservicedata.h"
#undef private

int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestVideoOperatorService tv;
    int res;
    if (argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\testvideooperatorservice.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}

TestVideoOperatorService::TestVideoOperatorService():
    mTestObject(0)
{
    
}

TestVideoOperatorService::~TestVideoOperatorService()
{
    cleanup();
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoOperatorService::init()
{
    // create test object
    if (!mTestObject)
    {
        mTestObject = new VideoOperatorService;
    }
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoOperatorService::cleanup()
{
    // delete test object
    delete mTestObject;
    mTestObject = 0;
}

// TEST CASES START ----------------------------------------------------------

// ---------------------------------------------------------------------------
// testLoad
// ---------------------------------------------------------------------------
//
void TestVideoOperatorService::testLoad()
{
    // Good case
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test title");
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test icon");
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test uri");
    VideoCollectionViewUtilsData::mCenRepIntValues.append(5050);
    QVERIFY(mTestObject->load(0, 0, 0, 0) == true);
    QVERIFY(mTestObject->mTitle == "test title");
    QVERIFY(mTestObject->mIconResource == "test icon");
    QVERIFY(mTestObject->mServiceUri == "test uri");
    QCOMPARE(mTestObject->mApplicationUid, 5050);
    
    // Only icon is defined for service.
    VideoCollectionViewUtilsData::mCenRepStringValues.append(CENREP_NO_STRING);
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test icon");
    QVERIFY(mTestObject->load(0, 0, 0, 0) == false);
    QVERIFY(mTestObject->mTitle == "");
    QVERIFY(mTestObject->mIconResource == "test icon");
    QVERIFY(mTestObject->mServiceUri == "");
    QCOMPARE(mTestObject->mApplicationUid, -1);

    // Only icon and uri are defined for service.
    VideoCollectionViewUtilsData::mCenRepStringValues.append(CENREP_NO_STRING);
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test icon");
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test uri");
    QVERIFY(mTestObject->load(0, 0, 0, 0) == true);
    QVERIFY(mTestObject->mTitle == "");
    QVERIFY(mTestObject->mIconResource == "test icon");
    QVERIFY(mTestObject->mServiceUri == "test uri");
    QCOMPARE(mTestObject->mApplicationUid, -1);
    
    // Only icon and app uid are defined for service.
    VideoCollectionViewUtilsData::mCenRepStringValues.append(CENREP_NO_STRING);
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test icon");
    VideoCollectionViewUtilsData::mCenRepIntValues.append(5050);
    QVERIFY(mTestObject->load(0, 0, 0, 0) == true);
    QVERIFY(mTestObject->mTitle == "");
    QVERIFY(mTestObject->mIconResource == "test icon");
    QVERIFY(mTestObject->mServiceUri == "");
    QCOMPARE(mTestObject->mApplicationUid, 5050);
}

// ---------------------------------------------------------------------------
// testTitle
// ---------------------------------------------------------------------------
//
void TestVideoOperatorService::testTitle()
{
    mTestObject->mTitle = "test title";
    QVERIFY(mTestObject->title() == "test title");
}

// ---------------------------------------------------------------------------
// testIconResource
// ---------------------------------------------------------------------------
//
void TestVideoOperatorService::testIconResource()
{
    mTestObject->mIconResource = "test icon";
    QVERIFY(mTestObject->iconResource() == "test icon");
}

// ---------------------------------------------------------------------------
// test
// ---------------------------------------------------------------------------
//
void TestVideoOperatorService::testLaunchService()
{
    mTestObject->mApplicationUid = 0;
    mTestObject->launchService();
}

// end of file
