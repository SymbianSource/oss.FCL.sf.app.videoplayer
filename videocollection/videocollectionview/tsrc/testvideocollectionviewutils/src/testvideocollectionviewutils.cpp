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
#include "stub/inc/centralrepository.h"
#include "testvideocollectionviewutils.h"
#include "hblabel.h"
#include "hbaction.h"
#include "videocollectioncommon.h"
#include "centralrepository.h"
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

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideocollectionviewutils.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
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
    additional = txt;
    HbMessageBox::mWarningCallCount = 0;
    HbMessageBox::mInformationCallCount = 0;
    
    // status: VideoCollectionCommon::statusSingleDeleteFail
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleDeleteFail, additional);
    QVERIFY(HbMessageBox::mWarningCallCount == 1);
    QVERIFY(HbMessageBox::mInformationCallCount == 0);
    HbMessageBox::mWarningCallCount = 0;
    HbMessageBox::mInformationCallCount = 0;
    additional = QVariant();
    
    // - invalid additional
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleDeleteFail, additional);
    QVERIFY(HbMessageBox::mWarningCallCount == 0);
    QVERIFY(HbMessageBox::mInformationCallCount == 0);
    
    // status: VideoCollectionCommon::statusMultipleDeleteFail (additional not needed)
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultipleDeleteFail, additional);
    QVERIFY(HbMessageBox::mWarningCallCount == 1);
    QVERIFY(HbMessageBox::mInformationCallCount == 0);
    HbMessageBox::mWarningCallCount = 0;
    HbMessageBox::mInformationCallCount = 0;
    additional = QVariant();
    
    additional = 5;
    // status: VideoCollectionCommon::statusMultipleDeleteSucceed
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultipleDeleteSucceed, additional);
    QVERIFY(HbMessageBox::mWarningCallCount == 0);
    QVERIFY(HbMessageBox::mInformationCallCount == 1);
    HbMessageBox::mWarningCallCount = 0;
    HbMessageBox::mInformationCallCount = 0;
    additional = QVariant();
    
    // -> invalid additional
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultipleDeleteSucceed, additional);
    QVERIFY(HbMessageBox::mWarningCallCount == 0);
    QVERIFY(HbMessageBox::mInformationCallCount == 0);
    
    // status: invalid
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultipleDeleteSucceed + 1, additional);
    QVERIFY(HbMessageBox::mWarningCallCount == 0);
    QVERIFY(HbMessageBox::mInformationCallCount == 0);
    
    
/*    
    
    QString txt = "__test__";      
    // error code 0, no additional
    testObject.showErrorMsgSlot(0, additional);
    QVERIFY(!HbMessageBox::staticInstance);

    // error code 0, additional 
    additional = txt;
    testObject.showErrorMsgSlot(0, additional);
    QVERIFY(!HbMessageBox::staticInstance);
    
    // error code VideoCollectionCommon::errorCollectionSingleDelete, no additional
    additional.clear();
    testObject.showErrorMsgSlot(VideoCollectionCommon::errorCollectionSingleDelete, additional);
    QVERIFY(!HbMessageBox::staticInstance);
    
    // error code VideoCollectionCommon::errorCollectionSingleDelete, additional
    additional = txt;
    testObject.showErrorMsgSlot(VideoCollectionCommon::errorCollectionSingleDelete, additional);
    QVERIFY(HbMessageBox::staticInstance);
    QVERIFY(HbMessageBox::staticInstance->mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(!HbMessageBox::staticInstance->mBGFaded);
    QVERIFY(!HbMessageBox::staticInstance->mModal);
    QVERIFY(HbMessageBox::staticInstance->mText.count() > 0);
    QVERIFY(HbMessageBox::staticInstance->mText.contains(txt));
    QVERIFY(HbMessageBox::staticInstance->mAttribute == Qt::WA_DeleteOnClose);
    delete HbMessageBox::staticInstance;
    HbMessageBox::staticInstance = 0;
    
    // error code VideoCollectionCommon::errorCollectionMultipleDelete, no additional
    additional.clear();
    testObject.showErrorMsgSlot(VideoCollectionCommon::errorCollectionMultipleDelete, additional);
    QVERIFY(HbMessageBox::staticInstance);
    QVERIFY(HbMessageBox::staticInstance->mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(!HbMessageBox::staticInstance->mBGFaded);
    QVERIFY(!HbMessageBox::staticInstance->mModal);
    QVERIFY(HbMessageBox::staticInstance->mText.count() > 0);
    QVERIFY(!HbMessageBox::staticInstance->mText.contains(txt));
    QVERIFY(HbMessageBox::staticInstance->mAttribute == Qt::WA_DeleteOnClose);
    delete HbMessageBox::staticInstance;
    HbMessageBox::staticInstance = 0;
    
    // error code VideoCollectionCommon::errorCollectionMultipleDelete, additional
    additional = txt;
    testObject.showErrorMsgSlot(VideoCollectionCommon::errorCollectionMultipleDelete, additional);
    QVERIFY(HbMessageBox::staticInstance);
    QVERIFY(HbMessageBox::staticInstance->mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(!HbMessageBox::staticInstance->mBGFaded);
    QVERIFY(!HbMessageBox::staticInstance->mModal);
    QVERIFY(HbMessageBox::staticInstance->mText.count() > 0);
    QVERIFY(!HbMessageBox::staticInstance->mText.contains(txt));
    QVERIFY(HbMessageBox::staticInstance->mAttribute == Qt::WA_DeleteOnClose);
    delete HbMessageBox::staticInstance;
*/
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
    CRepository::setNewLLeave(true);
    CRepository::setRoleValue(Qt::DisplayRole);
    CRepository::setOrderValue(Qt::DescendingOrder);
    
    int sortRole(-1);
    Qt::SortOrder sortOrder(Qt::AscendingOrder);
    
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder) < 0);
    QCOMPARE(sortRole, -1);
    QCOMPARE(sortOrder, Qt::AscendingOrder);
    
    CRepository::setNewLLeave(false);
    CRepository::setGetFail(0);
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder) < 0);
    QCOMPARE(sortRole, -1);
    QCOMPARE(sortOrder, Qt::AscendingOrder);

    CRepository::setGetFail(1);
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder) < 0);
    QCOMPARE(sortRole, -1);
    QCOMPARE(sortOrder, Qt::AscendingOrder);
    
    CRepository::setGetFail(255);
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder) == 0);
    QCOMPARE(sortRole, (int)Qt::DisplayRole);
    QCOMPARE(sortOrder, Qt::DescendingOrder);
}

// End of file
    


