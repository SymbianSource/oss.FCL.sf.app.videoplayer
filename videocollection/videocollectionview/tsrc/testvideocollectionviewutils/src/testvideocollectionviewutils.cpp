/**
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
* Description:   tester for methods in VideoCollectionViewUtils
* 
*/

#include <qapplication.h>
#include "centralrepository.h"
#include "testvideocollectionviewutils.h"
#include "hblabel.h"
#include "hbaction.h"
#include "videocollectioncommon.h"
#include "centralrepository.h"
#include "hbmessageboxdata.h"

#define private public
#include "videocollectionviewutils.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    TestVideoVideoCollectionViewUtils tv;

    int res;
    if(argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\testvideocollectionviewutils.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
        
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::init()
{

}
 
// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::cleanup()
{

}

// ---------------------------------------------------------------------------
// testShowErrorMsgSlot
// ---------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testShowErrorMsgSlot()
{
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    QVariant additional;
    QString txt = "__test__";      
    HbMessageBoxData::mWarningCallCount = 0;
    HbMessageBoxData::mInformationCallCount = 0;
    
    // status: VideoCollectionCommon::statusSingleDeleteFail
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mWarningCallCount == 1);
    QVERIFY(HbMessageBoxData::mInformationCallCount == 0);
    HbMessageBoxData::mWarningCallCount = 0;
    HbMessageBoxData::mInformationCallCount = 0;
    
    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mWarningCallCount == 0);
    QVERIFY(HbMessageBoxData::mInformationCallCount == 0);
    
    // status: VideoCollectionCommon::statusMultipleDeleteFail
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultipleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mWarningCallCount == 1);
    QVERIFY(HbMessageBoxData::mInformationCallCount == 0);
    HbMessageBoxData::mWarningCallCount = 0;
    HbMessageBoxData::mInformationCallCount = 0;
    
    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultipleDeleteFail, additional);
    HbMessageBoxData::mWarningCallCount = 0;
    HbMessageBoxData::mInformationCallCount = 0;
    
    // status: VideoCollectionCommon::statusMultipleRemoveFail (additional not needed)
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultiRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mWarningCallCount == 1);
    QVERIFY(HbMessageBoxData::mInformationCallCount == 0);
    HbMessageBoxData::mWarningCallCount = 0;
    HbMessageBoxData::mInformationCallCount = 0;
    
    // status: VideoCollectionCommon::statusVideosAddedToCollection
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mWarningCallCount == 0);
    QVERIFY(HbMessageBoxData::mInformationCallCount == 1);
    HbMessageBoxData::mWarningCallCount = 0;
    HbMessageBoxData::mInformationCallCount = 0;
    
    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mWarningCallCount == 0);
    QVERIFY(HbMessageBoxData::mInformationCallCount == 0);
    
    // status: VideoCollectionCommon::statusAllVideosAlreadyInCollection
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusAllVideosAlreadyInCollection, additional);
    QVERIFY(HbMessageBoxData::mWarningCallCount == 1);
    QVERIFY(HbMessageBoxData::mInformationCallCount == 0);
    HbMessageBoxData::mWarningCallCount = 0;
    HbMessageBoxData::mInformationCallCount = 0;
    
    // status: VideoCollectionCommon::statusDeleteInProgress
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusDeleteInProgress, additional);
    QVERIFY(HbMessageBoxData::mWarningCallCount == 0);
    QVERIFY(HbMessageBoxData::mInformationCallCount == 1);
    HbMessageBoxData::mWarningCallCount = 0;
    HbMessageBoxData::mInformationCallCount = 0;

    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusDeleteInProgress, additional);
    QVERIFY(HbMessageBoxData::mWarningCallCount == 0);
    QVERIFY(HbMessageBoxData::mInformationCallCount == 0);
}

// -----------------------------------------------------------------------------
// testSaveSortingValues
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testSaveSortingValues()
{
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    CRepository::setNewLLeave(true);
    int sortRole = VideoCollectionCommon::KeyDateTime;
    Qt::SortOrder sortOrder = Qt::AscendingOrder;
    
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder) < 0);
    CRepository::setNewLLeave(false);
    
    CRepository::setSetFail(0);
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder) < 0);
    
    CRepository::setSetFail(1);
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder) < 0);
    
    CRepository::setSetFail(255);
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder) == 0);
    QCOMPARE(CRepository::getRoleValue(), sortRole);
    QCOMPARE(CRepository::getOrderValue(), (int)sortOrder);
}
 
// -----------------------------------------------------------------------------
// testLoadSortingValues
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testLoadSortingValues()
{
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    
    int sortRole(-1);
    Qt::SortOrder sortOrder(Qt::AscendingOrder);
    
    CRepository::setRoleValue(Qt::DisplayRole);
    CRepository::setOrderValue(Qt::AscendingOrder);
    
    CRepository::setNewLLeave(true);
    testObject.mSortRole = -1;
    testObject.mSortOrder = Qt::DescendingOrder;
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder) < 0);
    QCOMPARE(sortRole, -1);
    QCOMPARE(sortOrder, Qt::DescendingOrder);

    CRepository::setNewLLeave(false);
    CRepository::setGetFail(0);
    testObject.mSortRole = -1;
    testObject.mSortOrder = Qt::DescendingOrder;
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder) < 0);
    QCOMPARE(sortRole, -1);
    QCOMPARE(sortOrder, Qt::DescendingOrder);

    CRepository::setGetFail(1);
    testObject.mSortRole = -1;
    testObject.mSortOrder = Qt::DescendingOrder;
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder) < 0);
    QCOMPARE(sortRole, (int)Qt::DisplayRole);
    QCOMPARE(sortOrder, Qt::DescendingOrder);
    
    CRepository::setGetFail(255);
    testObject.mSortRole = -1;
    testObject.mSortOrder = Qt::DescendingOrder;
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder) == 0);
    QCOMPARE(sortRole, (int)Qt::DisplayRole);
    QCOMPARE(sortOrder, Qt::AscendingOrder);
}

// -----------------------------------------------------------------------------
// testGetServiceIconStrings
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testGetServiceIconStrings()
{
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    CRepository::setNewLLeave(true);
    _LIT(KExpectedTDesValue, "expected");
    CRepository::setTDesValue(KExpectedTDesValue());
    QString expected((QChar*)KExpectedTDesValue().Ptr(),KExpectedTDesValue().Length());
    
    QString icon;
    QString pressed;
    QVERIFY(testObject.getServiceIconStrings(icon, pressed) < 0);
    QVERIFY(icon.isNull());
    QVERIFY(pressed.isNull());
    
    CRepository::setNewLLeave(false);
    CRepository::setGetFail(0);
    QVERIFY(testObject.getServiceIconStrings(icon, pressed) < 0);
    QVERIFY(icon.isNull());
    QVERIFY(pressed.isNull());
    
    CRepository::setGetFail(1);
    QVERIFY(testObject.getServiceIconStrings(icon, pressed) < 0);
    QVERIFY(icon.isNull());
    QVERIFY(pressed.isNull());
    
    CRepository::setGetFail(255);
    QVERIFY(testObject.getServiceIconStrings(icon, pressed) == 0);
    QCOMPARE(icon, expected);
    QCOMPARE(pressed, expected);
}

// -----------------------------------------------------------------------------
// testGetServiceIconStrings
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testGetServiceUriString()
{
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    CRepository::setNewLLeave(true);
    _LIT(KExpectedTDesValue, "expected");
    CRepository::setTDesValue(KExpectedTDesValue());
    QString expected((QChar*)KExpectedTDesValue().Ptr(),KExpectedTDesValue().Length());
    
    QVERIFY(testObject.getServiceUriString().isNull());
    
    CRepository::setNewLLeave(false);
    CRepository::setGetFail(0);
    QVERIFY(testObject.getServiceUriString().isNull());
    
    CRepository::setGetFail(255);
    QCOMPARE(testObject.getServiceUriString(), expected);
}

// End of file
    


