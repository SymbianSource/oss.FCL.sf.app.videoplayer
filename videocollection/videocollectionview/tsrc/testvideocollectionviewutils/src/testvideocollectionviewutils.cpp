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
#include "xqsettingsmanagerstub.h"
#include <vcxmyvideosdefs.h>
#include "centralrepository.h"
#include "testvideocollectionviewutils.h"
#include "hblabel.h"
#include "hbaction.h"
#include "videocollectionwrapper.h"
#include "videosortfilterproxymodeldata.h"
#include "videoactivitystate.h"
#include "videocollectioncommon.h"
#include "centralrepository.h"
#include "hbmessageboxdata.h"
#include "hbnotificationdialog.h"
#include "hblistview.h"
#include <hbactivitymanager.h>
#include <hbapplication.h>

#define private public
#include "videocollectionviewutils.h"
#undef private

// following consts are copied from videocollectionviewutils.cpp
const int KVideoSortingRoleKey(0x1);
const int KVideoSortingOrderKey(0x2);
const int KCollectionsSortingRoleKey(0x3);
const int KCollectionsSortingOrderKey(0x4);
static const QString KEY_WIDGET_LEVEL    = "_VideoActivity_widget_level_";

// id of the collection whose videolist is to be shown (int).
static const QString KEY_COLLECTION_ID   = "_VideoActivity_collection_id_";

// name of the collection whose videolist is to be shown (QString)
static const QString KEY_COLLECTION_NAME = "_VideoActivity_collection_name_";

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    //HbMainWindow window;
    //QApplication app(argc, argv);
    
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
// testShowStatusMsgSlot
// ---------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testShowStatusMsgSlot()
{
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    QVariant additional;
    QList<QVariant> dataList;
    QString txt = "__test__";      
    HbNotificationDialog::mNotifDialogTitle = "";
    HbNotificationDialog::mTitleTextWRapping = Hb::TextNoWrap;
    HbNotificationDialog::mAttribute = Qt::WA_Disabled;
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;
    
    // status: VideoCollectionCommon::statusSingleDeleteFail
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;
    HbMessageBoxData::mInformationCallCount = 0;
    
    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;
    
    // status: VideoCollectionCommon::statusMultipleDeleteFail
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultipleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;

    // - invalid additional (no change, since additional not excepted)
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultipleDeleteFail, additional);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;
    
    // status: VideoCollectionCommon::statusMultipleRemoveFail (additional not needed)
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultiRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mType = -1;
    HbMessageBoxData::mShowCallCount = 0;
    
    // status: VideoCollectionCommon::statusVideosAddedToCollection
    // notification dialog shown
    dataList.clear();
    dataList.append(QVariant(1)); // count
    dataList.append(QVariant(txt)); // name
    additional = dataList;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() > 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextWordWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_DeleteOnClose);
    HbNotificationDialog::mNotifDialogTitle = "";
    HbNotificationDialog::mTitleTextWRapping = Hb::TextNoWrap;
    HbNotificationDialog::mAttribute = Qt::WA_Disabled;
        
    // - too few additional
    dataList.clear();
    dataList.append(QVariant(txt)); // name
    additional = dataList;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    
    // - empty message
    dataList.clear();
    dataList.append(QVariant(1)); // count
    dataList.append(QVariant("")); // name
    additional = dataList;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    
    // - with zero video count
    dataList.clear();
    dataList.append(QVariant(0)); // count
    dataList.append(QVariant(txt)); // name
    additional = dataList;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);    

    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusVideosAddedToCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    
    // status: VideoCollectionCommon::statusAllVideosAlreadyInCollection
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusAllVideosAlreadyInCollection, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mShowCallCount = 0;
    HbMessageBoxData::mType = -1;
    
    // status: VideoCollectionCommon::statusDeleteInProgress
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusDeleteInProgress, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() > 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextWordWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_DeleteOnClose);
    HbNotificationDialog::mNotifDialogTitle = "";
    HbNotificationDialog::mTitleTextWRapping = Hb::TextNoWrap;
    HbNotificationDialog::mAttribute = Qt::WA_Disabled;

    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusDeleteInProgress, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    
    // status: VideoCollectionCommon::statusSingleRemoveFail
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mShowCallCount = 0;
    HbMessageBoxData::mType = -1;
    
    // - invalid additional
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusSingleRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    
    // status: statusMultiRemoveFail
    additional = txt;
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultiRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mShowCallCount = 0;
    HbMessageBoxData::mType = -1;
    
    // invalid additional (no change, since additional not excepted)
    additional = QVariant();
    testObject.showStatusMsgSlot(VideoCollectionCommon::statusMultiRemoveFail, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 1);
    QVERIFY(HbMessageBoxData::mType == HbMessageBox::MessageTypeWarning);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
    HbMessageBoxData::mShowCallCount = 0;
    HbMessageBoxData::mType = -1;
    
    // invalid state
    additional = txt;
    testObject.showStatusMsgSlot(0, additional);
    QVERIFY(HbMessageBoxData::mShowCallCount == 0);
    QVERIFY(HbMessageBoxData::mType == -1);
    QVERIFY(HbNotificationDialog::mNotifDialogTitle.count() == 0);
    QVERIFY(HbNotificationDialog::mTitleTextWRapping == Hb::TextNoWrap);
    QVERIFY(HbNotificationDialog::mAttribute == Qt::WA_Disabled);
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
    VideoCollectionCommon::TCollectionLevels target = VideoCollectionCommon::ELevelInvalid;
    CRepository::mSortValues.clear();
    
    // target invalid
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) < 0);
    
    // target == VideoCollectionCommon::ELevelCategory
    target = VideoCollectionCommon::ELevelCategory;    
    
    // cenrep creation fails
    CRepository::setNewLLeave(true);
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) < 0);
    QCOMPARE(CRepository::mSortValues.count(), 0);

    // set fails
    CRepository::setNewLLeave(false);
    CRepository::setSetFail(0);
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) < 0);
    QCOMPARE(CRepository::mSortValues.count(), 0);
    CRepository::setSetFail(2);
    
    // succeed
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) == 0);
    QCOMPARE(CRepository::mSortValues.count(), 2);
    QVERIFY(CRepository::mSortValues.values().contains(sortRole));
    QVERIFY(CRepository::mSortValues.values().contains(sortOrder));
    
    // target != VideoCollectionCommon::ELevelCategory
    CRepository::mSortValues.clear();
    target = VideoCollectionCommon::ELevelVideos;    
    
    // cenrep creation fails
    CRepository::setNewLLeave(true);
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) < 0);
    QCOMPARE(CRepository::mSortValues.count(), 0);
    
    // set fails
    CRepository::setNewLLeave(false);
    CRepository::setSetFail(0);
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) < 0);
    QCOMPARE(CRepository::mSortValues.count(), 0);
    CRepository::setSetFail(2);
    // succeed
    QVERIFY(testObject.saveSortingValues(sortRole, sortOrder, target) == 0);
    QCOMPARE(CRepository::mSortValues.count(), 2);
    QVERIFY(CRepository::mSortValues.values().contains(sortRole));
    QVERIFY(CRepository::mSortValues.values().contains(sortOrder));
}
 
// -----------------------------------------------------------------------------
// testLoadSortingValues
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testLoadSortingValues()
{      
    CRepository::mSortValues[KVideoSortingRoleKey] = VideoCollectionCommon::KeyDateTime;
    CRepository::mSortValues[KVideoSortingOrderKey] = Qt::DescendingOrder;
    CRepository::mSortValues[KCollectionsSortingRoleKey] = VideoCollectionCommon::KeyTitle;
    CRepository::mSortValues[KCollectionsSortingOrderKey] = Qt::DescendingOrder;
    
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    
    int sortRole(-1);
    Qt::SortOrder sortOrder(Qt::AscendingOrder);   
    VideoCollectionCommon::TCollectionLevels target = VideoCollectionCommon::ELevelInvalid;
    
    testObject.mVideosSortRole = -1;
    testObject.mVideosSortOrder = Qt::AscendingOrder;
    testObject.mCollectionsSortRole = -1;
    testObject.mCollectionsSortOrder = Qt::AscendingOrder;
    
    // target is invalid
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    
    // target is VideoCollectionCommon::ELevelCategory
    target = VideoCollectionCommon::ELevelCategory;
    
    // cerep creation fails 
    CRepository::setNewLLeave(true);
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    CRepository::setNewLLeave(false);
    
    // getting from cenrep fails from first
    CRepository::setGetFail(0);
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    
    // getiing from cenrep fails from the second
    CRepository::setGetFail(1);
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    CRepository::setGetFail(2);
    
    // succeeds
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(sortOrder == Qt::DescendingOrder);
    sortRole = -1;
    sortOrder = Qt::AscendingOrder;

    // another call used previously saved values
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(sortOrder == Qt::DescendingOrder);
    sortRole = -1;
    sortOrder = Qt::AscendingOrder;
    
    // target is different than VideoCollectionCommon::ELevelCategory
    target = VideoCollectionCommon::ELevelVideos;
    
    // cerep creation fails 
    CRepository::setNewLLeave(true);
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    CRepository::setNewLLeave(false);
    
    CRepository::setGetFail(0);
    // getting from cenrep fails from first
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    
    CRepository::setGetFail(1);
    // getting from cenrep fails from second
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) < 0);
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == -1);
    QVERIFY(sortOrder == Qt::AscendingOrder);
    
    CRepository::setGetFail(2);
    
    // succeeds
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(testObject.mVideosSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(testObject.mVideosSortOrder == Qt::DescendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(sortOrder == Qt::DescendingOrder);
    
    sortRole = -1;
    sortOrder = Qt::AscendingOrder;
    
    // another call used previously saved values
    QVERIFY(testObject.loadSortingValues(sortRole, sortOrder, target) == 0);
    QVERIFY(testObject.mVideosSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(testObject.mVideosSortOrder == Qt::DescendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
    QVERIFY(sortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(sortOrder == Qt::DescendingOrder);

}

// -----------------------------------------------------------------------------
// testGetCenRepStringValue
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testGetCenRepStringValue()
{  
    
}

// -----------------------------------------------------------------------------
// testGetCenRepIntValue
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testGetCenRepIntValue()
{   
    
}

// -----------------------------------------------------------------------------
// testInitListView
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testInitListView()
{    
    // empty view
    VideoCollectionViewUtils::initListView(0);
    QVERIFY(HbListView::mLatestrecycling == false);
    QVERIFY(HbListView::mLatestClamping == HbScrollArea::InvalidClamping);
    QVERIFY(HbListView::mLatestScrolling == HbScrollArea::InvalidScrolling);
    QVERIFY(HbListView::mLatestFrictionEnabled == false);
    QVERIFY(HbListView::mLatestUniformItemSizes == false);
    QVERIFY(HbListView::mSelectionMode == HbAbstractItemView::InvalidSelection);
    
    HbListView view;
    VideoCollectionViewUtils::initListView(&view);
    QVERIFY(HbListView::mLatestrecycling == true);
    QVERIFY(HbListView::mLatestClamping == HbScrollArea::BounceBackClamping);
    QVERIFY(HbListView::mLatestScrolling == HbScrollArea::PanWithFollowOn);
    QVERIFY(HbListView::mLatestFrictionEnabled == true);
    QVERIFY(HbListView::mLatestUniformItemSizes == true);
    QVERIFY(HbListView::mSelectionMode == HbAbstractItemView::NoSelection);
    
    HbListView::mLatestrecycling = false;
    HbListView::mLatestClamping = HbScrollArea::InvalidClamping;
    HbListView::mLatestScrolling = HbScrollArea::InvalidScrolling;
    HbListView::mLatestFrictionEnabled = false;
    HbListView::mLatestUniformItemSizes = false;
    HbListView::mSelectionMode = HbAbstractItemView::InvalidSelection;
    
    // no prototype from view (for coverity)
    HbListView::mReturnNullPrototype = true;
    VideoCollectionViewUtils::initListView(&view);
    QVERIFY(HbListView::mLatestrecycling == true);
    QVERIFY(HbListView::mLatestClamping == HbScrollArea::BounceBackClamping);
    QVERIFY(HbListView::mLatestScrolling == HbScrollArea::PanWithFollowOn);
    QVERIFY(HbListView::mLatestFrictionEnabled == true);
    QVERIFY(HbListView::mLatestUniformItemSizes == true);
    QVERIFY(HbListView::mSelectionMode == HbAbstractItemView::NoSelection);
    
    HbListView::mLatestrecycling = false;
    HbListView::mLatestClamping = HbScrollArea::InvalidClamping;
    HbListView::mLatestScrolling = HbScrollArea::InvalidScrolling;
    HbListView::mLatestFrictionEnabled = false;
    HbListView::mLatestUniformItemSizes = false;
    HbListView::mSelectionMode = HbAbstractItemView::InvalidSelection;
    HbListView::mReturnNullPrototype = false;
    
    // no vertical scrollbar from view (for coverity)
    HbListView::mVScrollBarIsNull = true;
    VideoCollectionViewUtils::initListView(&view);
    QVERIFY(HbListView::mLatestrecycling == true);
    QVERIFY(HbListView::mLatestClamping == HbScrollArea::BounceBackClamping);
    QVERIFY(HbListView::mLatestScrolling == HbScrollArea::PanWithFollowOn);
    QVERIFY(HbListView::mLatestFrictionEnabled == true);
    QVERIFY(HbListView::mLatestUniformItemSizes == true);
    QVERIFY(HbListView::mSelectionMode == HbAbstractItemView::NoSelection);

}

// -----------------------------------------------------------------------------
// testSortModel
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testSortModel()
{
    VideoCollectionViewUtils &testObject(VideoCollectionViewUtils::instance());
    testObject.mVideosSortRole = -1;
    testObject.mVideosSortOrder = Qt::AscendingOrder;
    testObject.mCollectionsSortRole = -1;
    testObject.mCollectionsSortOrder = Qt::AscendingOrder;
    
    CRepository::mSortValues[KVideoSortingRoleKey] = VideoCollectionCommon::KeyDateTime;
    CRepository::mSortValues[KVideoSortingOrderKey] = Qt::DescendingOrder;
    CRepository::mSortValues[KCollectionsSortingRoleKey] = VideoCollectionCommon::KeyTitle;
    CRepository::mSortValues[KCollectionsSortingOrderKey] = Qt::DescendingOrder;
    
    VideoCollectionCommon::TCollectionLevels target = VideoCollectionCommon::ELevelInvalid;
    
    // null model
    VideoCollectionViewUtils::sortModel(0, false, target);
    
    VideoSortFilterProxyModel *model = 
        VideoCollectionWrapper::instance().getModel(VideoCollectionCommon::EModelTypeGeneric);
    
    // target == invalid -> default values used as sort -call 
    VideoCollectionViewUtils::sortModel(model, false, target);
    
    QVERIFY(VideoSortFilterProxyModelData::mDoSortingCallCount == 1);
    QVERIFY(VideoSortFilterProxyModelData::mSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(VideoSortFilterProxyModelData::mSortOrder == Qt::AscendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync == false); 
    QVERIFY(testObject.mVideosSortRole == -1);
    QVERIFY(testObject.mVideosSortOrder == Qt::AscendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    
    VideoSortFilterProxyModelData::mDoSortingCallCount = 0;
    VideoSortFilterProxyModelData::mSortRole = -1;
    VideoSortFilterProxyModelData::mSortOrder = Qt::AscendingOrder;
    VideoSortFilterProxyModelData::mSortAsync = true;
    
    // target == VideoCollectionCommon::ELevelVideos
    target = VideoCollectionCommon::ELevelVideos;
    VideoCollectionViewUtils::sortModel(model, false, target);
    
    QVERIFY(VideoSortFilterProxyModelData::mDoSortingCallCount == 1);
    QVERIFY(VideoSortFilterProxyModelData::mSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(VideoSortFilterProxyModelData::mSortOrder == Qt::DescendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync == false); 
    QVERIFY(testObject.mVideosSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(testObject.mVideosSortOrder == Qt::DescendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == -1);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::AscendingOrder);
    
    
    VideoSortFilterProxyModelData::mDoSortingCallCount = 0;
    VideoSortFilterProxyModelData::mSortRole = -1;
    VideoSortFilterProxyModelData::mSortOrder = Qt::AscendingOrder;
    VideoSortFilterProxyModelData::mSortAsync = true;
       
    // target == VideoCollectionCommon::ELevelCategory
    target = VideoCollectionCommon::ELevelCategory;
    VideoCollectionViewUtils::sortModel(model, false, target);
    
    QVERIFY(VideoSortFilterProxyModelData::mDoSortingCallCount == 1);
    QVERIFY(VideoSortFilterProxyModelData::mSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(VideoSortFilterProxyModelData::mSortOrder == Qt::DescendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync == false); 
    QVERIFY(testObject.mVideosSortRole == VideoCollectionCommon::KeyDateTime);
    QVERIFY(testObject.mVideosSortOrder == Qt::DescendingOrder);
    QVERIFY(testObject.mCollectionsSortRole == VideoCollectionCommon::KeyTitle);
    QVERIFY(testObject.mCollectionsSortOrder == Qt::DescendingOrder);
}

// -----------------------------------------------------------------------------
// testSetAndGetWidgetLevel
// -----------------------------------------------------------------------------
//
void TestVideoVideoCollectionViewUtils::testSetAndGetWidgetLevel()
{
    static const char* ACTIVITY_VIDEOS_MAINVIEW = "VideosMainView";
    
    VideoCollectionCommon::TCollectionLevels level;
    
    // Invalid level
    level = VideoCollectionCommon::ELevelInvalid;
    VideoCollectionViewUtils::setWidgetActivityLevel(level);
    VideoCollectionViewUtils::getActivityWidgetLevel( level);
    QVERIFY(level == VideoCollectionCommon::ELevelVideos);
    
    // Category level.
    level = VideoCollectionCommon::ELevelCategory;
    VideoCollectionViewUtils::setWidgetActivityLevel(level);
    VideoCollectionViewUtils::getActivityWidgetLevel( level);
    QVERIFY(level == VideoCollectionCommon::ELevelCategory);
    
    // make sure unneeded data is cleared
    TMPXItemId id = TMPXItemId(1,1);
    QString name = "name";
    VideoCollectionViewUtils::setCollectionActivityData(id, name);
    level = VideoCollectionCommon::ELevelVideos;
    VideoCollectionViewUtils::setWidgetActivityLevel(level);

    QVERIFY(level == VideoCollectionCommon::ELevelVideos);
    QVariant data = VideoActivityState::instance().getActivityData(KEY_COLLECTION_ID);
    QVERIFY(data.toInt() == 0);
    data = VideoActivityState::instance().getActivityData(KEY_COLLECTION_NAME);
    QVERIFY(data.toString().isEmpty());
    
    id = TMPXItemId(1,1);
    name = "name";
    VideoCollectionViewUtils::setCollectionActivityData(id, name);
    level = VideoCollectionCommon::ELevelAlbum;
    VideoCollectionViewUtils::setWidgetActivityLevel(level);

    QVERIFY(level == VideoCollectionCommon::ELevelAlbum);
    data = VideoActivityState::instance().getActivityData(KEY_COLLECTION_ID);
    QVERIFY(data.toInt() == 1);
    data = VideoActivityState::instance().getActivityData(KEY_COLLECTION_NAME);
    QVERIFY(data.toString() == "name");

}

void TestVideoVideoCollectionViewUtils::testSetAndGetCollectionActivityData()
{
    TMPXItemId id = TMPXItemId(1,1);
    QString name = "name";   
    
    // no default collection nor user defined collection
    VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelVideos;
    VideoCollectionViewUtils::setWidgetActivityLevel(level); 
    VideoCollectionViewUtils::setCollectionActivityData(id, name);
    VideoCollectionViewUtils::getCollectionActivityData(id, name);
    QVERIFY(id == TMPXItemId::InvalidId());
    QVERIFY(name.isEmpty());
    
    // default collection
    level = VideoCollectionCommon::ELevelDefaultColl;
    id = TMPXItemId(1,1);
    name = "name"; 
    VideoCollectionViewUtils::setWidgetActivityLevel(level); 
    VideoCollectionViewUtils::setCollectionActivityData(id, name);
    VideoCollectionViewUtils::getCollectionActivityData(id, name);
   
    QVERIFY(id == TMPXItemId(1, KVcxMvcMediaTypeCategory));
    QVERIFY(name == "name");
   
    // user defined collection
    level = VideoCollectionCommon::ELevelAlbum;
    id = TMPXItemId(1,1);
    name = "name"; 
    VideoCollectionViewUtils::setWidgetActivityLevel(level); 
    VideoCollectionViewUtils::setCollectionActivityData(id, name);
    VideoCollectionViewUtils::getCollectionActivityData(id, name);
    
    QVERIFY(id == TMPXItemId(1, KVcxMvcMediaTypeAlbum));
    QVERIFY(name == "name");
   
}

// End of file
    


