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
* Description:   tester for methods in VideoListWidget
*
*/

#include <qmap.h>
#include <vcxmyvideosdefs.h>
#include <mpxhbvideocommondefs.h>

#include "hbglobal.h"
#include "hblistview.h"
#include "hbview.h"
#include "videoservices.h"
#include "videosortfilterproxymodel.h"
#include "hbscrollbar.h"
#include "hbmenu.h"
#include "hbmessagebox.h"
#include "hbinstance.h"
#include "hbmainwindow.h"
#include "videothumbnaildata.h"
#include "videocollectioncommon.h"
#include "videocollectionwrapper.h"
#include "videocollectionwrapperdata.h"
#include "hbmenudata.h"
#include "videothumbnailtestdata.h"
#include "videolistdatamodel.h"
#include "videolistdatamodeldata.h"
#include "videosortfilterproxymodeldata.h"
#include "hbmessageboxdata.h"

#include "testlistwidget.h"

#define private public
#include "videolistwidget.h"
#undef private

static const int CONTEXT_MENU_COUNT = 8;

/**
 * helper class to run protected member of the test object
 */
class ListWidgetTester : public VideoListWidget
{
public:
    /**
     * constructor
     */
    ListWidgetTester(VideoCollectionUiLoader* uiLoader, HbView *parent) :
            VideoListWidget(uiLoader, parent)
    {
        
    }
    
    /**
     * calls emitActivated
     */
    void callEmiteActivated(QModelIndex index)
    {
        VideoListWidget::emitActivated(index);
    }
    
    /**
     * calls longPressGesture
     */
    void callLongPressGesture(const QPointF &point)
    {
        VideoListWidget::longPressGesture (point);
    }
};

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    TestListWidget tv;

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
        pass[2] = "c:\\data\\testlistwidget.txt";
        res = QTest::qExec(&tv, 3, pass);
    }
    
    return res;
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void TestListWidget::setRowCount(int count, int type, VideoListDataModel *model)
{
    if (!model)
    {
        VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
        VideoSortFilterProxyModel *model = wrapper.getModel(type);
        QVERIFY(model);
        
        VideoListDataModel *sourceModel = qobject_cast<VideoListDataModel*>(model->sourceModel());
        QVERIFY(sourceModel);
        
        VideoListDataModelData::setRowCount(*sourceModel, count);
        sourceModel->initialize();
    }
    else
    {
        VideoListDataModelData::setRowCount(*model, count);
        model->initialize();
    }
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestListWidget::init()
{
    mDummyMainWnd = new HbMainWindow;
    mTempView = new HbView;
    
    mTestWidget = new ListWidgetTester(0, mTempView);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestListWidget::cleanup()
{
    delete mTestWidget;
    mTestWidget = 0;
    
    delete mTempView;
    mTempView = 0;
    
    delete mDummyMainWnd;
    mDummyMainWnd = 0;
    
    hbInstance->mWindowses.clear();
    
    VideoCollectionWrapperData::reset();
    HbMenuData::reset();
    VideoThumbnailTestData::reset();
    VideoListDataModelData::reset();
    VideoSortFilterProxyModelData::reset();
    HbMessageBoxData::reset();
}
 
// ---------------------------------------------------------------------------
// testInitialize
// ---------------------------------------------------------------------------
//
void TestListWidget::testInitialize()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);
    
    // no prototype
    HbListView::mReturnNullPrototype = true;
    QVERIFY(mTestWidget->initialize(*model) == -1);
    HbListView::mReturnNullPrototype = false;
    
    // no scroll bar
    HbListView::mVScrollBarIsNull = true;
    QVERIFY(mTestWidget->initialize(*model) == -1);

    // succeed case ( new operator cannot be stubbed)
    HbListView::mVScrollBarIsNull = false;
    HbListView::mReturnNullPrototype = false;
    QVERIFY(mTestWidget->initialize(*model) == 0);
    QCOMPARE(HbListView::mLatestrecycling, true);
    QCOMPARE(HbListView::mLatestClamping, HbScrollArea::BounceBackClamping);
    QCOMPARE(HbListView::mLatestScrolling, HbScrollArea::PanOrFlick);
    QCOMPARE(HbListView::mLatestFrictionEnabled, true);
    QCOMPARE(HbListView::mLatestUniformItemSizes, true);
    QCOMPARE(HbListView::mLatestVisibility, false);
    QCOMPARE(HbListView::mLatestEnableValue, false);    
    QCOMPARE(HbScrollBar::mInteractive, true);    
	QVERIFY(mTestWidget->mModel == model);  
	QVERIFY(mTestWidget->mVideoServices == 0);
	QVERIFY(mTestWidget->mIsService == false);
}
 
// ---------------------------------------------------------------------------
// testActivate
// ---------------------------------------------------------------------------
//
void TestListWidget::testActivate()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    // no  model: fails
    QVERIFY(mTestWidget->activate() == -1);
    QCOMPARE(HbMenuData::mEnabledSetted, false);
    QCOMPARE(HbListView::mLatestVisibility, false);
    QVERIFY(!HbListView::mLatestModel);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 1);
    
    // no context menu, model exist: succeeds
    QVERIFY(mTestWidget->initialize(*model) == 0);
    QVERIFY(mTestWidget->activate() == 0);
    QCOMPARE(HbMenuData::mEnabledSetted, false);
    QCOMPARE(HbListView::mLatestVisibility, true);
    QCOMPARE(HbListView::mLatestModel, model);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 1);
    
    HbMenuData::mEnabledSetted = true;
    HbListView::mLatestVisibility = false;
    HbListView::mLatestEnableValue = false;
    HbListView::mLatestModel = 0;
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = 0;
    
    // context menu exists, succeed (signal connecting failure cannot be tested here)
    mTestWidget->mContextMenu = new HbMenu;
    QVERIFY(mTestWidget->activate() == 0);
    QCOMPARE(HbMenuData::mEnabledSetted, true);
    QCOMPARE(HbListView::mLatestVisibility, true);
    QCOMPARE(HbListView::mLatestModel, model);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 1);
}
 
// ---------------------------------------------------------------------------
// testDeactivate
// ---------------------------------------------------------------------------
//
void TestListWidget::testDeactivate()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    HbMenuData::mEnabledSetted = false;
    HbListView::mLatestVisibility = false;
    HbListView::mLatestEnableValue = false;
    HbListView::mLatestModel = 0;
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = 0;
    VideoSortFilterProxyModel *nullModel = 0;
    
    mTestWidget->activate();
    
    // no context menu and activated without model
    mTestWidget->deactivate();
    QCOMPARE(HbListView::mLatestVisibility, false);
    QCOMPARE(HbListView::mLatestModel, nullModel);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 0);
    
    // context menu exists and activated with model
    mTestWidget->mContextMenu = new HbMenu;
    QVERIFY(mTestWidget->initialize(*model) == 0);
    mTestWidget->activate();
    mTestWidget->deactivate();
    QCOMPARE(HbListView::mLatestVisibility, true);
    QCOMPARE(HbListView::mLatestModel, model);
    QCOMPARE(VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled, 0);
   
}

// ---------------------------------------------------------------------------
// testGetLevel
// ---------------------------------------------------------------------------
//
void TestListWidget::testGetLevel()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    mTestWidget->initialize(*model);
    
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    QCOMPARE(mTestWidget->getLevel(), VideoCollectionCommon::ELevelVideos);
}
   
// ---------------------------------------------------------------------------
// testGetModel
// ---------------------------------------------------------------------------
//
void TestListWidget::testGetModel()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    QVERIFY(&(mTestWidget->getModel()) == 0);
    
    HbListView::mVScrollBarIsNull = false;
    HbListView::mReturnNullPrototype = false;
    mTestWidget->initialize(*model);
    
    QVERIFY(&(mTestWidget->getModel()) == model);
}
 
// ---------------------------------------------------------------------------
// testEmitActivated
// ---------------------------------------------------------------------------
//
void TestListWidget::testEmitActivated()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    QSignalSpy spysignal(mTestWidget, SIGNAL(collectionOpened(bool, const QString&)));
    mTestWidget->initialize(*model);
    mTestWidget->activate();
    hbInstance->mWindowses.append(mDummyMainWnd);
    
    QVariant data = QString("test");
    // correct data to index 0
    VideoListDataModelData::setData( Qt::DisplayRole, data);
    setRowCount(1);
    VideoSortFilterProxyModelData::mItemIds.append(TMPXItemId(0,0));
    
    QModelIndex fetchIndex = model->index(0, 0, QModelIndex());
    
    // selection mode == HbAbstractItemView::MultiSelection
    HbListView::mSelectionMode = HbAbstractItemView::MultiSelection;
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(mDummyMainWnd->mSoftKeyAction == 0);
    QVERIFY(!VideoSortFilterProxyModelData::mLastIndex.isValid());
    
    // modelIndex is not valid
    HbListView::mSelectionMode = HbAbstractItemView::NoSelection;
    fetchIndex = QModelIndex();
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(mDummyMainWnd->mSoftKeyAction == 0);
    QVERIFY(!VideoSortFilterProxyModelData::mLastIndex.isValid());

    // current level is not ELevelCategory
    fetchIndex = model->index(0, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(VideoSortFilterProxyModelData::mLastIndex.row() == 0);
    VideoSortFilterProxyModelData::mLastIndex = QModelIndex();
    
    // current level is ELevelCategory
    mTestWidget->mCurrentLevel= VideoCollectionCommon::ELevelCategory;

    // -> getType() != ECollections
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(mDummyMainWnd->mSoftKeyAction == 0);
    QVERIFY(VideoSortFilterProxyModelData::mLastIndex.row() == 0);
    VideoSortFilterProxyModelData::mLastIndex = QModelIndex();
    
    // -> getType() == ECollections

    // --> variant is not valid (invalid data at row index 1)
    fetchIndex = model->index(1, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(mDummyMainWnd->mSoftKeyAction == 0);
    QVERIFY(VideoSortFilterProxyModelData::mLastIndex.row() == 1);
    VideoSortFilterProxyModelData::mLastIndex = QModelIndex();

    // --> variant is valid (correct data at index 0),5 collectionOpened -signal should be emitted
    mDummyMainWnd->mSoftKeyAction = 0;
    fetchIndex = model->index(0, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 1);
//    QVERIFY(mDummyMainWnd->mSoftKeyAction == mTestWidget->mSecSkAction);
    QVERIFY(VideoSortFilterProxyModelData::mLastIndex.row() == 0);
    VideoSortFilterProxyModelData::mLastIndex = QModelIndex();
  
    hbInstance->mWindowses.clear();
}
 

// ---------------------------------------------------------------------------
// testLongPressGesture
// ---------------------------------------------------------------------------
//
void TestListWidget::testLongPressGesture()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    QVariant data = QString("test");
    // correct data to index 0
    VideoListDataModelData::setData( Qt::DisplayRole, data);
    setRowCount(1);
    
    QPointF point(1,1);

    QSignalSpy spysignal(mTestWidget, SIGNAL(command(int)));
    mTestWidget->initialize(*model);
    hbInstance->mWindowses.append(mDummyMainWnd);
    
    HbListView::mSelectionMode = HbAbstractItemView::MultiSelection;
    // mDetailsReady is false
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 1);
    QVERIFY(HbMenuData::mExecPoint != point);
    QVERIFY(HbListView::mLongPressedPoint != point);
    spysignal.clear();
    
    // multiselection is on
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenuData::mExecPoint != point);
        
    // current index is invalid
    HbListView::mSelectionMode = HbAbstractItemView::NoSelection;
    HbListView::mCurrentIndex = QModelIndex();    
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenuData::mExecPoint != point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    
    // current index is valid
    HbListView::mCurrentIndex = model->index(0, 0, QModelIndex());
    
    // model is == 0
    VideoSortFilterProxyModel *tmp = mTestWidget->mModel;
    mTestWidget->mModel = 0;
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenuData::mExecPoint != point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    mTestWidget->mModel = tmp;
    
    // mCurrentLevel != ELevelCategory
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenuData::mExecPoint == point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    HbMenuData::mExecPoint = QPointF();
    
    // mCurrentLevel == ELevelCategory
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    
    // --> mpxId.iId2 != 1
    TMPXItemId itemId;
    itemId.iId2 = 0;
    VideoSortFilterProxyModelData::mItemIds.append(itemId);
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenuData::mExecPoint == point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    HbMenuData::mExecPoint = QPointF();
    VideoSortFilterProxyModelData::mItemIds.clear();
    
    // --> mpxId.iId2 == 1
    itemId.iId2 = 1;
    
    // ---> mpxId.iId1 == KVcxMvcCategoryIdDownloads
    itemId.iId1 = KVcxMvcCategoryIdDownloads;
    VideoSortFilterProxyModelData::mItemIds.append(itemId);
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenuData::mExecPoint == point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    HbMenuData::mExecPoint = QPointF();
    VideoSortFilterProxyModelData::mItemIds.clear();
    
    // ---> mpxId.iId1 == KVcxMvcCategoryIdCaptured
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    VideoSortFilterProxyModelData::mItemIds.append(itemId);
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenuData::mExecPoint == point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    HbMenuData::mExecPoint = QPointF();
    VideoSortFilterProxyModelData::mItemIds.clear();
    
    // ---> mpxId.iId1 != KVcxMvcCategoryIdDownloads and mpxId.iId1 != KVcxMvcCategoryIdCaptured
    itemId.iId1 = KVcxMvcCategoryIdAll;
    VideoSortFilterProxyModelData::mItemIds.append(itemId);
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenuData::mExecPoint == point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    HbMenuData::mExecPoint = QPointF();  
    
    // context menu setup fails, due invalid amount of correct actions
    QMap<VideoListWidget::TContextActionIds, HbAction*>::iterator iter = mTestWidget->mContextMenuActions.begin();
    iter++;
    HbAction *nullAction = 0;
    iter.value() = nullAction;
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenuData::mExecPoint != point);
    QVERIFY(HbListView::mLongPressedPoint == point);
   
}

// ---------------------------------------------------------------------------
// testSetContextMenu
// ---------------------------------------------------------------------------
//
void TestListWidget::testSetContextMenu()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);
    VideoListDataModel *sourceModel = qobject_cast<VideoListDataModel*>(model->sourceModel());

    VideoSortFilterProxyModelData::mItemIds.clear();
    mTestWidget->initialize(*model);
    hbInstance->mWindowses.append(mDummyMainWnd);
    
    TMPXItemId itemId;
    QPointF point(1,1);
    itemId.iId2 = 0;
    VideoSortFilterProxyModelData::mItemIds.append(itemId);

    // no context menu
    delete mTestWidget->mContextMenu;
    mTestWidget->mContextMenu = 0;   
    
    // mCurrentLevel == ELevelVideos
    int visibleCount = 0;
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    mTestWidget->callLongPressGesture(point);
    
    QMap<VideoListWidget::TContextActionIds, HbAction*>::iterator iter = mTestWidget->mContextMenuActions.begin();    
    QVERIFY(iter != mTestWidget->mContextMenuActions.end());
    
    while(iter != mTestWidget->mContextMenuActions.end())
    {
        if(iter.value()->mVisible)
        {
            visibleCount++;   
        }
        ++iter;
    }
    QVERIFY(visibleCount == 4);
    
    // invalid amount of actions -> invalid items gets removed
    HbAction *nullAction = 0;
    iter = mTestWidget->mContextMenuActions.begin();
    iter++;
    iter.value() = nullAction;
    mTestWidget->callLongPressGesture(point);
    QVERIFY(!mTestWidget->mContextMenu);
    QVERIFY(mTestWidget->mContextMenuActions.count() == 0);            
    
    // mCurrentLevel == ELevelCategory
    mTestWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    // --> getType returns ECollections
    // ---> default collection flag is on
    mTestWidget->callLongPressGesture(point);
    iter = mTestWidget->mContextMenuActions.begin();
    QVERIFY(iter != mTestWidget->mContextMenuActions.end());
    visibleCount = 0;
    while(iter != mTestWidget->mContextMenuActions.end())
    {
        if(iter.value()->mVisible)
        {
            visibleCount++;   
         }
         ++iter;
     }
     QVERIFY(visibleCount == 0);
    
     // ---> default collection flag is off
     VideoSortFilterProxyModelData::mItemIds.clear();
     itemId.iId2 = 1;
     itemId.iId1 = KVcxMvcCategoryIdAll;
     VideoSortFilterProxyModelData::mItemIds.append(itemId);
     mTestWidget->callLongPressGesture(point);
     iter = mTestWidget->mContextMenuActions.begin();
     QVERIFY(iter != mTestWidget->mContextMenuActions.end());
     visibleCount = 0;
     while(iter != mTestWidget->mContextMenuActions.end())
     {
         if(iter.value()->mVisible)
         {
             visibleCount++;   
         }
         ++iter;
     }
     QVERIFY(visibleCount == 4);
    
    // --> getType returns EUserColItems
     mTestWidget->callLongPressGesture(point);
     iter = mTestWidget->mContextMenuActions.begin();
     QVERIFY(iter != mTestWidget->mContextMenuActions.end());
     visibleCount = 0;
     while(iter != mTestWidget->mContextMenuActions.end())
     {
         if(iter.value()->mVisible)
         {
             visibleCount++;   
         }
         ++iter;
     }
     QVERIFY(visibleCount == 4);    
     
     // --> getType returns EUnknow
     mTestWidget->mCurrentLevel = (VideoCollectionCommon::TCollectionLevels)0;
     mTestWidget->callLongPressGesture(point);
      iter = mTestWidget->mContextMenuActions.begin();
      QVERIFY(iter != mTestWidget->mContextMenuActions.end());
      visibleCount = 0;
      while(iter != mTestWidget->mContextMenuActions.end())
      {
          if(iter.value()->mVisible)
          {
              visibleCount++;   
          }
          ++iter;
      }
      QVERIFY(visibleCount == 0);    
}
  
// ---------------------------------------------------------------------------
// testShareItemSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testShareItemSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);
    VideoListDataModel *sourceModel = qobject_cast<VideoListDataModel*>(model->sourceModel());

    // nothing to test yet
    HbMessageBoxData::mLatestTxt = "";
    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(shareItemSlot()));
    emit testSignal();
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(shareItemSlot()));
    
}
 
// ---------------------------------------------------------------------------
// testDeleteItemSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testDeleteItemSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    HbMessageBoxData::mLatestTxt = "";
    VideoSortFilterProxyModelData::mLastIndex = QModelIndex();
    mTestWidget->initialize(*model);    
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(deleteItemSlot()));    
    
    // no model
    VideoSortFilterProxyModel *tmp = mTestWidget->mModel;
    mTestWidget->mModel = 0;
    emit testSignal();
    QVERIFY(VideoListDataModelData::dataAccessCount() == 0);
    QVERIFY(HbMessageBoxData::mLatestTxt.isEmpty());
    QVERIFY(!VideoSortFilterProxyModelData::mLastIndex.isValid());
    setRowCount(1);
    mTestWidget->mModel = tmp;
    
    // current index is invalid
    mTestWidget->mCurrentIndex = QModelIndex();
    emit testSignal();
    QVERIFY(VideoListDataModelData::dataAccessCount() == 1);
    QVERIFY(HbMessageBoxData::mLatestTxt.isEmpty());
    QVERIFY(!VideoSortFilterProxyModelData::mLastIndex.isValid());
    setRowCount(0);
    
    // data fetched from item is invalid
    setRowCount(1);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex()); 
    emit testSignal();
    QVERIFY(VideoListDataModelData::dataAccessCount() == 1);
    QVERIFY(HbMessageBoxData::mLatestTxt.isEmpty());
    QVERIFY(!VideoSortFilterProxyModelData::mLastIndex.isValid());
    setRowCount(0);
    
    // data is valid
    setRowCount(1);
    QVariant data = QString("test");
    VideoListDataModelData::setData( Qt::DisplayRole, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    
    // messagebox question returns false
    HbMessageBoxData::mQuestionReturnValue = false;
    emit testSignal();
    QVERIFY(VideoListDataModelData::dataAccessCount() == 1);
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    QVERIFY(!VideoSortFilterProxyModelData::mLastIndex.isValid());;
    HbMessageBoxData::mLatestTxt = "";
    setRowCount(1);
    data = QString("test");
    VideoListDataModelData::setData( Qt::DisplayRole, data);
    
    // messagebox question returns true
    HbMessageBoxData::mQuestionReturnValue = true;
    emit testSignal();
    QVERIFY(VideoListDataModelData::dataAccessCount() == 1);
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    QVERIFY(VideoSortFilterProxyModelData::mLastIndex.row() == 0);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(deleteItemSlot()));
}
 
// ---------------------------------------------------------------------------
// testRenameSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testRenameSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    // nothing to test yet
    HbMessageBoxData::mLatestTxt = "";
    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(renameSlot()));
    emit testSignal();
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(renameSlot()));
}
  
// ---------------------------------------------------------------------------
// testPlayAllSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testPlayAllSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    // nothing to test yet
    HbMessageBoxData::mLatestTxt = "";
    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(playAllSlot()));
    emit testSignal();
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(playAllSlot()));    
}
 
// ---------------------------------------------------------------------------
// testAddItemSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testAddItemSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    // nothing to test yet
    HbMessageBoxData::mLatestTxt = "";
    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(addItemSlot()));
    emit testSignal();
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(addItemSlot()));    
}
  
// ---------------------------------------------------------------------------
// testAddToCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testAddToCollectionSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    // nothing to test yet
    HbMessageBoxData::mLatestTxt = "";
    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(addToCollectionSlot()));
    emit testSignal();
    QVERIFY(!HbMessageBoxData::mLatestTxt.isEmpty());
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(addToCollectionSlot()));    
}
 
// ---------------------------------------------------------------------------
// testOpenDetailsSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testOpenDetailsSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    QSignalSpy spysignal(mTestWidget, SIGNAL(command(int)));
    HbMessageBoxData::mLatestTxt = "";
    VideoSortFilterProxyModelData::mLastIndex = QModelIndex();
    setRowCount(0);
    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(openDetailsSlot())); 

    // no model
    VideoSortFilterProxyModel *tmp = mTestWidget->mModel;
    mTestWidget->mModel = 0;
    emit testSignal();
    QVERIFY(spysignal.count() == 0);
    QVERIFY(!VideoSortFilterProxyModelData::mLastIndex.isValid());
    mTestWidget->mModel = tmp;
    
    // detail fetch fails
    setRowCount(1);
    QVariant data = QString("test");
    VideoListDataModelData::setData( Qt::DisplayRole, data);
    mTestWidget->mCurrentIndex = model->index(0, 0, QModelIndex());
    VideoSortFilterProxyModelData::mDetailsReturnValue = -1;
    
    emit testSignal();
    QVERIFY(spysignal.count() == 0);
    QVERIFY(VideoSortFilterProxyModelData::mLastIndex.row() == 0);
    
    // detail fetch succeeds
    VideoSortFilterProxyModelData::mDetailsReturnValue = 0;
    VideoSortFilterProxyModelData::mLastIndex = QModelIndex();
    emit testSignal();
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.at(0).at(0).isValid());
    QVERIFY(spysignal.at(0).at(0).toInt() == MpxHbVideoCommon::ActivateVideoDetailsView);
    QVERIFY(VideoSortFilterProxyModelData::mLastIndex.row() == 0);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(openDetailsSlot())); 
}
 
// ---------------------------------------------------------------------------
// testBack
// ---------------------------------------------------------------------------
//
void TestListWidget::testBack()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    QSignalSpy spysignal(mTestWidget, SIGNAL(collectionOpened(bool, const QString&)));
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(back()));
    // no model
    hbInstance->mWindowses.append(mDummyMainWnd);
    emit testSignal();        
    QVERIFY(spysignal.count() == 0);
        
    // model exist
    mTestWidget->initialize(*model);   
    emit testSignal();    
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).isValid());
    QVERIFY(spysignal.value(0).at(0).toBool() == false);   
    QVERIFY(spysignal.value(0).at(1).isValid());
    QVERIFY(spysignal.value(0).at(1).toString().isEmpty());   
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(back()));
}
  
// ---------------------------------------------------------------------------
// testScrollingEndedSlot
// ---------------------------------------------------------------------------
// 
void TestListWidget::testScrollingEndedSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    mTestWidget->initialize(*model);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(scrollingEndedSlot()));
    
    // no visible items
    HbListView::mVisibleItems.clear();
    emit testSignal();
    QVERIFY(VideoThumbnailTestData::mStartBackgroundFetchingCallCount == 0);
    QVERIFY(!mTestWidget->mScrollPositionTimer->isActive());
    
    // setup few "visible" items and make sure item count match at thumbnail data
    int count = 10;
    setRowCount(count);
    HbAbstractViewItem *item = 0;
    for(int i = 0; i < count; ++i)
    {
        item = new HbAbstractViewItem();
        item->mModelIndex = model->index(i, 0, QModelIndex());
        HbListView::mVisibleItems.append(item);
    }
    
    // Test
    emit testSignal();
    QVERIFY(VideoThumbnailTestData::mStartBackgroundFetchingCallCount == 1);
    QVERIFY(!mTestWidget->mScrollPositionTimer->isActive());

    // Test again when timer is null.
    VideoThumbnailTestData::mStartBackgroundFetchingCallCount = 0;
    QTimer *backup = mTestWidget->mScrollPositionTimer;
    mTestWidget->mScrollPositionTimer = 0;
    emit testSignal();
    QVERIFY(VideoThumbnailTestData::mStartBackgroundFetchingCallCount == 1);
    mTestWidget->mScrollPositionTimer = backup;
    backup = 0;
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(scrollingEndedSlot()));
}

// ---------------------------------------------------------------------------
// testScrollingEndedSlot
// ---------------------------------------------------------------------------
// 
void TestListWidget::testScrollPositionChangedSlot()
{
    VideoCollectionWrapper &wrapper = VideoCollectionWrapper::instance();
    VideoSortFilterProxyModel *model = wrapper.getModel(VideoCollectionWrapper::EAllVideos);

    mTestWidget->initialize(*model);
    const QPointF point;
    
    connect(this, SIGNAL(testSignal(const QPointF&)), mTestWidget, SLOT(scrollPositionChangedSlot(const QPointF&)));
    
    // Test
    emit testSignal(point);
    
    // Test again when timer is running.
    mTestWidget->mScrollPositionTimer->start(1000*30);
    emit testSignal(point);
    
    // Test when timer is null.
    QTimer *backup = mTestWidget->mScrollPositionTimer;
    mTestWidget->mScrollPositionTimer = 0;
    emit testSignal(point);
    mTestWidget->mScrollPositionTimer = backup;
    backup = 0;
    
    disconnect(this, SIGNAL(testSignal(const QPointF&)), mTestWidget, SLOT(scrollPositionChangedSlot(const QPointF&)));
}

// end of file
