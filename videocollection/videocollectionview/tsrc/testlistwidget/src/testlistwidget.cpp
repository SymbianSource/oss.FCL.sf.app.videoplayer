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

#include <QMap>
#include "hblistview.h"
#include "hbview.h"

#include "videoservices.h"
#include "videosortfilterproxymodel.h"
#include "dummydatamodel.h"
#include "hbscrollbar.h"
#include "hbmenu.h"
#include "hbmessagebox.h"
#include "hbinstance.h"
#include "hbmainwindow.h"
#include "mpxhbvideocommondefs.h"
#include "videothumbnaildata.h"
#include "videocollectioncommon.h"

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
    ListWidgetTester(HbView *parent) :
            VideoListWidget(parent)
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
int main(int /*argc*/, char *argv[])
{
        
    TestListWidget tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testlistwidget.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestListWidget::init()
{
    mDummyMainWnd = new HbMainWindow;
    mTempView = new HbView;
    mDummyModel = new DummyDataModel();
    mDummyProxyModel = new VideoSortFilterProxyModel();
    mDummyProxyModel->setSourceModel(mDummyModel);
    
    mTestWidget = new ListWidgetTester(mTempView);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestListWidget::cleanup()
{
    delete mTestWidget;
    mTestWidget = 0;
    
    delete mDummyProxyModel;
    mDummyProxyModel = 0;
    
    delete mDummyModel;
    mDummyModel = 0;
    
    delete mTempView;
    mTempView = 0;
    
    delete mDummyMainWnd;
    mDummyMainWnd = 0;
    
    hbInstance->mWindowses.clear();
}
 
// ---------------------------------------------------------------------------
// testInitialize
// ---------------------------------------------------------------------------
//
void TestListWidget::testInitialize()
{
    // no prototype
    HbListView::mReturnNullPrototype = true;
    QVERIFY(mTestWidget->initialize(*mDummyProxyModel) == -1);
    HbListView::mReturnNullPrototype = false;
    
    // no scroll bar
    HbListView::mVScrollBarIsNull = true;
    QVERIFY(mTestWidget->initialize(*mDummyProxyModel) == -1);

    // succeed case ( new operator cannot be stubbed)
    HbListView::mVScrollBarIsNull = false;
    HbListView::mReturnNullPrototype = false;
    QVERIFY(mTestWidget->initialize(*mDummyProxyModel) == 0);
    QCOMPARE(HbListView::mLatestrecycling, true);
    QCOMPARE(HbListView::mLatestClamping, HbScrollArea::BounceBackClamping);
    QCOMPARE(HbListView::mLatestScrolling, HbScrollArea::PanOrFlick);
    QCOMPARE(HbListView::mLatestFrictionEnabled, true);
    QCOMPARE(HbListView::mLatestUniformItemSizes, true);
    QCOMPARE(HbListView::mLatestVisibility, false);
    QCOMPARE(HbListView::mLatestEnableValue, false);    
    QCOMPARE(HbScrollBar::mInteractive, true);    
	QVERIFY(mTestWidget->mModel == mDummyProxyModel);  
	QVERIFY(mTestWidget->mVideoServices == 0);
	QVERIFY(mTestWidget->mIsService == false);
}
 
// ---------------------------------------------------------------------------
// testActivate
// ---------------------------------------------------------------------------
//
void TestListWidget::testActivate()
{
    // no  model: fails
    VideoSortFilterProxyModel *nullModel = 0;
    QVERIFY(mTestWidget->activate() == -1);
    QCOMPARE(HbMenu::mEnabledSetted, false);
    QCOMPARE(HbListView::mLatestVisibility, false);
    QCOMPARE(HbListView::mLatestModel, nullModel);
    QCOMPARE(VideoThumbnailData::mBackgroundThumbnailFetchingEnabled, 1);
    
    // no context menu, model exist: succeeds
    QVERIFY(mTestWidget->initialize(*mDummyProxyModel) == 0);
    QVERIFY(mTestWidget->activate() == 0);
    QCOMPARE(HbMenu::mEnabledSetted, false);
    QCOMPARE(HbListView::mLatestVisibility, true);
    QCOMPARE(HbListView::mLatestModel, mDummyProxyModel);
    QCOMPARE(VideoThumbnailData::mBackgroundThumbnailFetchingEnabled, 1);
    
    HbMenu::mEnabledSetted = true;
    HbListView::mLatestVisibility = false;
    HbListView::mLatestEnableValue = false;
    HbListView::mLatestModel = 0;
    VideoThumbnailData::mBackgroundThumbnailFetchingEnabled = 0;
    
    // context menu exists, succeed (signal connecting failure cannot be tested here)
    mTestWidget->mContextMenu = new HbMenu;
    QVERIFY(mTestWidget->activate() == 0);
    QCOMPARE(HbMenu::mEnabledSetted, true);
    QCOMPARE(HbListView::mLatestVisibility, true);
    QCOMPARE(HbListView::mLatestModel, mDummyProxyModel);
    QCOMPARE(VideoThumbnailData::mBackgroundThumbnailFetchingEnabled, 1);
}
 
// ---------------------------------------------------------------------------
// testDeactivate
// ---------------------------------------------------------------------------
//
void TestListWidget::testDeactivate()
{
    HbMenu::mEnabledSetted = false;
    HbListView::mLatestVisibility = false;
    HbListView::mLatestEnableValue = false;
    HbListView::mLatestModel = 0;
    VideoThumbnailData::mBackgroundThumbnailFetchingEnabled = 0;
    VideoSortFilterProxyModel *nullModel = 0;
    
    mTestWidget->activate();
    
    // no context menu and activated without model
    mTestWidget->deactivate();
    QCOMPARE(HbListView::mLatestVisibility, false);
    QCOMPARE(HbListView::mLatestModel, nullModel);
    QCOMPARE(VideoThumbnailData::mBackgroundThumbnailFetchingEnabled, 0);
    
    // context menu exists and activated with model
    mTestWidget->mContextMenu = new HbMenu;
    QVERIFY(mTestWidget->initialize(*mDummyProxyModel) == 0);
    mTestWidget->activate();
    mTestWidget->deactivate();
    QCOMPARE(HbListView::mLatestVisibility, true);
    QCOMPARE(HbListView::mLatestModel, mDummyProxyModel);
    QCOMPARE(VideoThumbnailData::mBackgroundThumbnailFetchingEnabled, 0);
   
}

// ---------------------------------------------------------------------------
// testGetLevel
// ---------------------------------------------------------------------------
//
void TestListWidget::testGetType()
{
    mTestWidget->initialize(*mDummyProxyModel);
    
    
    // level != ELevelVideos and role != ELevelCategory
    mTestWidget->mCurrentLevel = (VideoListWidget::TVideoListLevel)0;
    QCOMPARE(mTestWidget->getType(), VideoListWidget::EUnknown);
    
    // level == ELevelVideos
    mTestWidget->mCurrentLevel = VideoListWidget::ELevelVideos;
    QCOMPARE(mTestWidget->getType(), VideoListWidget::EAllVideos);
    
    // level == ELevelCategory and mLastOpenItemId == TMPXItemId::InvalidId
    mTestWidget->mCurrentLevel = VideoListWidget::ELevelCategory;
    mTestWidget->mLastOpenItemId = TMPXItemId::InvalidId();
    QCOMPARE(mTestWidget->getType(), VideoListWidget::ECollections);
    
    // level == ELevelCategory and mLastOpenItemId.iId2 == KVcxMvcCategoryIdDownloads  
    mTestWidget->mLastOpenItemId.iId2 = KVcxMvcCategoryIdDownloads;
    mTestWidget->mCurrentLevel = VideoListWidget::ELevelCategory;
    mDummyMainWnd->mSoftKeyAction = mTestWidget->mSecSkAction;
    QCOMPARE(mTestWidget->getType(), VideoListWidget::EDefaultColItems);
    
    // level == ELevelCategory and  mLastOpenItemId.iId2 == KVcxMvcCategoryIdCaptured
    mTestWidget->mLastOpenItemId.iId2 = KVcxMvcCategoryIdDownloads;
    mTestWidget->mCurrentLevel = VideoListWidget::ELevelCategory;
    mDummyMainWnd->mSoftKeyAction = mTestWidget->mSecSkAction;
    QCOMPARE(mTestWidget->getType(), VideoListWidget::EDefaultColItems);
    
    
    // level == ELevelCategory and mLastOpenItemId.iId2 is neither above
    mTestWidget->mLastOpenItemId.iId2 = 0;
    mTestWidget->mCurrentLevel = VideoListWidget::ELevelCategory;
    mDummyMainWnd->mSoftKeyAction = mTestWidget->mSecSkAction;
    QCOMPARE(mTestWidget->getType(), VideoListWidget::EUserColItems);
    

    
}
   
// ---------------------------------------------------------------------------
// testGetModel
// ---------------------------------------------------------------------------
//
void TestListWidget::testGetModel()
{
    QVERIFY(&(mTestWidget->getModel()) == 0);
    
    HbListView::mVScrollBarIsNull = false;
    HbListView::mReturnNullPrototype = false;
    mTestWidget->initialize(*mDummyProxyModel);
    
    QVERIFY(&(mTestWidget->getModel()) == mDummyProxyModel);
}
 
// ---------------------------------------------------------------------------
// testEmitActivated
// ---------------------------------------------------------------------------
//
void TestListWidget::testEmitActivated()
{
    QSignalSpy spysignal(mTestWidget, SIGNAL(collectionOpened(bool, const QString&)));
    mTestWidget->initialize(*mDummyProxyModel);
    mTestWidget->activate();
    hbInstance->mWindowses.append(mDummyMainWnd);
    
    QVariant data = QString("test");
    // correct data to index 0
    mDummyModel->setData( Qt::DisplayRole, data);
    mDummyModel->setRowCount(1);
    mDummyProxyModel->mItemIds.append(TMPXItemId(0,0));
    
    QModelIndex fetchIndex = mDummyProxyModel->index(0, 0, QModelIndex());
    
    // selection mode == HbAbstractItemView::MultiSelection
    HbListView::mSelectionMode = HbAbstractItemView::MultiSelection;
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(mDummyMainWnd->mSoftKeyAction == 0);
    QVERIFY(!VideoSortFilterProxyModel::mLastIndex.isValid());
    
    // modelIndex is not valid
    HbListView::mSelectionMode = HbAbstractItemView::NoSelection;
    fetchIndex = QModelIndex();
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(mDummyMainWnd->mSoftKeyAction == 0);
    QVERIFY(!VideoSortFilterProxyModel::mLastIndex.isValid());
    QVERIFY(mTestWidget->mLastOpenItemId == TMPXItemId::InvalidId());
    
    // current level is not ELevelCategory
    fetchIndex = mDummyProxyModel->index(0, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(VideoSortFilterProxyModel::mLastIndex.row() == 0);
    QVERIFY(mTestWidget->mLastOpenItemId == TMPXItemId::InvalidId());
    VideoSortFilterProxyModel::mLastIndex = QModelIndex();
    
    // current level is ELevelCategory
    mTestWidget->mCurrentLevel= VideoListWidget::ELevelCategory;
    mTestWidget->mLastOpenItemId = TMPXItemId();
    
    // -> getType() != ECollections
    mTestWidget->mLastOpenItemId.iId2 = 0;
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(mDummyMainWnd->mSoftKeyAction == 0);
    QVERIFY(VideoSortFilterProxyModel::mLastIndex.row() == 0);
    QVERIFY(mTestWidget->mLastOpenItemId != TMPXItemId::InvalidId());
    VideoSortFilterProxyModel::mLastIndex = QModelIndex();
    
    // -> getType() == ECollections
    mTestWidget->mLastOpenItemId = TMPXItemId::InvalidId();
    
    // --> variant is not valid (invalid data at row index 1)
    fetchIndex = mDummyProxyModel->index(1, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(mDummyMainWnd->mSoftKeyAction == 0);
    QVERIFY(VideoSortFilterProxyModel::mLastIndex.row() == 1);
    QVERIFY(mTestWidget->mLastOpenItemId == TMPXItemId::InvalidId());
    VideoSortFilterProxyModel::mLastIndex = QModelIndex();
    
    // --> variant is valid (correct data at index 0),5 collectionOpened -signal should be emitted
    mDummyMainWnd->mSoftKeyAction = 0;
    fetchIndex = mDummyProxyModel->index(0, 0, QModelIndex());
    mTestWidget->callEmiteActivated(fetchIndex);
    QVERIFY(spysignal.count() == 1);
    QVERIFY(mDummyMainWnd->mSoftKeyAction == mTestWidget->mSecSkAction);
    QVERIFY(VideoSortFilterProxyModel::mLastIndex.row() == 0);
    QVERIFY(mTestWidget->mLastOpenItemId != TMPXItemId::InvalidId());
    VideoSortFilterProxyModel::mLastIndex = QModelIndex();
  
    hbInstance->mWindowses.clear();
}
 

// ---------------------------------------------------------------------------
// testLongPressGesture
// ---------------------------------------------------------------------------
//
void TestListWidget::testLongPressGesture()
{
    QVariant data = QString("test");
    // correct data to index 0
    mDummyModel->setData( Qt::DisplayRole, data);
    mDummyModel->setRowCount(1);
    
    QPointF point(1,1);

    QSignalSpy spysignal(mTestWidget, SIGNAL(command(int)));
    mTestWidget->initialize(*mDummyProxyModel);
    hbInstance->mWindowses.append(mDummyMainWnd);
    
    HbListView::mSelectionMode = HbAbstractItemView::MultiSelection;
    // mDetailsReady is false
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 1);
    QVERIFY(HbMenu::mExecPoint != point);
    QVERIFY(HbListView::mLongPressedPoint != point);
    spysignal.clear();
    
    // multiselection is on
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenu::mExecPoint != point);
        
    // current index is invalid
    HbListView::mSelectionMode = HbAbstractItemView::NoSelection;
    HbListView::mCurrentIndex = QModelIndex();    
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenu::mExecPoint != point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    
    // current index is valid
    HbListView::mCurrentIndex = mDummyProxyModel->index(0, 0, QModelIndex());            
    
    // model is == 0
    VideoSortFilterProxyModel *tmp = mTestWidget->mModel;
    mTestWidget->mModel = 0;
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenu::mExecPoint != point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    mTestWidget->mModel = tmp;
    
    // mCurrentLevel != ELevelCategory
    mTestWidget->mCurrentLevel = VideoListWidget::ELevelVideos;
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenu::mExecPoint == point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    HbMenu::mExecPoint = QPointF();
    
    // mCurrentLevel == ELevelCategory
    mTestWidget->mCurrentLevel = VideoListWidget::ELevelCategory;
    
    // --> mpxId.iId2 != 1
    TMPXItemId itemId;
    itemId.iId2 = 0;
    mDummyProxyModel->mItemIds.append(itemId);
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenu::mExecPoint == point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    HbMenu::mExecPoint = QPointF();
    mDummyProxyModel->mItemIds.clear();
    
    // --> mpxId.iId2 == 1
    itemId.iId2 = 1;
    
    // ---> mpxId.iId1 == KVcxMvcCategoryIdDownloads
    itemId.iId1 = KVcxMvcCategoryIdDownloads;
    mDummyProxyModel->mItemIds.append(itemId);
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenu::mExecPoint == point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    HbMenu::mExecPoint = QPointF();
    mDummyProxyModel->mItemIds.clear();
    
    // ---> mpxId.iId1 == KVcxMvcCategoryIdCaptured
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    mDummyProxyModel->mItemIds.append(itemId);
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenu::mExecPoint == point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    HbMenu::mExecPoint = QPointF();
    mDummyProxyModel->mItemIds.clear();
    
    // ---> mpxId.iId1 != KVcxMvcCategoryIdDownloads and mpxId.iId1 != KVcxMvcCategoryIdCaptured
    itemId.iId1 = KVcxMvcCategoryIdAll;
    mDummyProxyModel->mItemIds.append(itemId);
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenu::mExecPoint == point);
    QVERIFY(HbListView::mLongPressedPoint == point);
    HbMenu::mExecPoint = QPointF();  
    
    // context menu setup fails, due invalid amount of correct actions
    QMap<VideoListWidget::TContextActionIds, HbAction*>::iterator iter = mTestWidget->mContextMenuActions.begin();
    iter++;
    HbAction *nullAction = 0;
    iter.value() = nullAction;
    mTestWidget->callLongPressGesture(point);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(HbMenu::mExecPoint != point);
    QVERIFY(HbListView::mLongPressedPoint == point);
   
}

// ---------------------------------------------------------------------------
// testSetContextMenu
// ---------------------------------------------------------------------------
//
void TestListWidget::testSetContextMenu()
{
    mDummyProxyModel->mItemIds.clear();
    mTestWidget->initialize(*mDummyProxyModel);
    hbInstance->mWindowses.append(mDummyMainWnd);
    
    TMPXItemId itemId;
    QPointF point(1,1);
    itemId.iId2 = 0;
    mDummyProxyModel->mItemIds.append(itemId);

    // no context menu
    delete mTestWidget->mContextMenu;
    mTestWidget->mContextMenu = 0;   
    
    // mCurrentLevel == ELevelVideos
    int visibleCount = 0;
    mTestWidget->mCurrentLevel = VideoListWidget::ELevelVideos;
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
    mTestWidget->mCurrentLevel = VideoListWidget::ELevelCategory;
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
     mDummyProxyModel->mItemIds.clear();
     itemId.iId2 = 1;
     itemId.iId1 = KVcxMvcCategoryIdAll;
     mDummyProxyModel->mItemIds.append(itemId);
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
     mTestWidget->mLastOpenItemId.iId2 = 0;
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
     mTestWidget->mCurrentLevel = (VideoListWidget::TVideoListLevel)0;
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
    // nothing to test yet
    HbMessageBox::mLatestTxt = "";
    mTestWidget->initialize(*mDummyProxyModel);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(shareItemSlot()));
    emit testSignal();
    QVERIFY(!HbMessageBox::mLatestTxt.isEmpty());
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(shareItemSlot()));
    
}
 
// ---------------------------------------------------------------------------
// testDeleteItemSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testDeleteItemSlot()
{
    HbMessageBox::mLatestTxt = "";
    VideoSortFilterProxyModel::mLastIndex = QModelIndex();
    mDummyModel->reset(); 
    mTestWidget->initialize(*mDummyProxyModel);    
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(deleteItemSlot()));    
    
    // no model
    VideoSortFilterProxyModel *tmp = mTestWidget->mModel;
    mTestWidget->mModel = 0;
    emit testSignal();
    QVERIFY(mDummyModel->dataAccessCount() == 0);
    QVERIFY(HbMessageBox::mLatestTxt.isEmpty());
    QVERIFY(!VideoSortFilterProxyModel::mLastIndex.isValid());
    mDummyModel->reset();
    mTestWidget->mModel = tmp;
    
    // current index is invalid
    mTestWidget->mCurrentIndex = QModelIndex();
    emit testSignal();
    QVERIFY(mDummyModel->dataAccessCount() == 1);
    QVERIFY(HbMessageBox::mLatestTxt.isEmpty());
    QVERIFY(!VideoSortFilterProxyModel::mLastIndex.isValid());
    mDummyModel->reset();
    
    // data fetched from item is invalid
    mDummyModel->setRowCount(1);
    mTestWidget->mCurrentIndex = mDummyProxyModel->index(0, 0, QModelIndex()); 
    emit testSignal();
    QVERIFY(mDummyModel->dataAccessCount() == 1);
    QVERIFY(HbMessageBox::mLatestTxt.isEmpty());
    QVERIFY(!VideoSortFilterProxyModel::mLastIndex.isValid());
    mDummyModel->reset(); 
    
    // data is valid
    mDummyModel->setRowCount(1);
    QVariant data = QString("test");
    mDummyModel->setData( Qt::DisplayRole, data);
    mTestWidget->mCurrentIndex = mDummyProxyModel->index(0, 0, QModelIndex());
    
    // messagebox question returns false
    HbMessageBox::mQuestionReturnValue = false;
    emit testSignal();
    QVERIFY(mDummyModel->dataAccessCount() == 1);
    QVERIFY(!HbMessageBox::mLatestTxt.isEmpty());
    QVERIFY(!VideoSortFilterProxyModel::mLastIndex.isValid());;
    HbMessageBox::mLatestTxt = "";
    mDummyModel->reset(); 
    mDummyModel->setRowCount(1);
    data = QString("test");
    mDummyModel->setData( Qt::DisplayRole, data);
    
    // messagebox question returns true
    HbMessageBox::mQuestionReturnValue = true;
    emit testSignal();
    QVERIFY(mDummyModel->dataAccessCount() == 1);
    QVERIFY(!HbMessageBox::mLatestTxt.isEmpty());
    QVERIFY(VideoSortFilterProxyModel::mLastIndex.row() == 0);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(deleteItemSlot()));
}
 
// ---------------------------------------------------------------------------
// testRenameSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testRenameSlot()
{
    // nothing to test yet
    HbMessageBox::mLatestTxt = "";
    mTestWidget->initialize(*mDummyProxyModel);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(renameSlot()));
    emit testSignal();
    QVERIFY(!HbMessageBox::mLatestTxt.isEmpty());
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(renameSlot()));
}
  
// ---------------------------------------------------------------------------
// testPlayAllSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testPlayAllSlot()
{
    // nothing to test yet
    HbMessageBox::mLatestTxt = "";
    mTestWidget->initialize(*mDummyProxyModel);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(playAllSlot()));
    emit testSignal();
    QVERIFY(!HbMessageBox::mLatestTxt.isEmpty());
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(playAllSlot()));    
}
 
// ---------------------------------------------------------------------------
// testAddItemSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testAddItemSlot()
{
    // nothing to test yet
    HbMessageBox::mLatestTxt = "";
    mTestWidget->initialize(*mDummyProxyModel);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(addItemSlot()));
    emit testSignal();
    QVERIFY(!HbMessageBox::mLatestTxt.isEmpty());
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(addItemSlot()));    
}
  
// ---------------------------------------------------------------------------
// testAddToCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testAddToCollectionSlot()
{
    // nothing to test yet
    HbMessageBox::mLatestTxt = "";
    mTestWidget->initialize(*mDummyProxyModel);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(addToCollectionSlot()));
    emit testSignal();
    QVERIFY(!HbMessageBox::mLatestTxt.isEmpty());
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(addToCollectionSlot()));    
}
 
// ---------------------------------------------------------------------------
// testOpenDetailsSlot
// ---------------------------------------------------------------------------
//
void TestListWidget::testOpenDetailsSlot()
{
    QSignalSpy spysignal(mTestWidget, SIGNAL(command(int)));
    HbMessageBox::mLatestTxt = "";
    VideoSortFilterProxyModel::mLastIndex = QModelIndex();
    mDummyModel->reset(); 
    mTestWidget->initialize(*mDummyProxyModel);    
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(openDetailsSlot())); 

    // no model
    VideoSortFilterProxyModel *tmp = mTestWidget->mModel;
    mTestWidget->mModel = 0;
    emit testSignal();
    QVERIFY(spysignal.count() == 0);
    QVERIFY(!VideoSortFilterProxyModel::mLastIndex.isValid());
    mTestWidget->mModel = tmp;
    
    // detail fetch fails
    mDummyModel->setRowCount(1);
    QVariant data = QString("test");
    mDummyModel->setData( Qt::DisplayRole, data);
    mTestWidget->mCurrentIndex = mDummyProxyModel->index(0, 0, QModelIndex());
    VideoSortFilterProxyModel::mDetailsReturnValue = -1;
    
    emit testSignal();
    QVERIFY(spysignal.count() == 0);
    QVERIFY(VideoSortFilterProxyModel::mLastIndex.row() == 0);
    
    // detail fetch succeeds
    VideoSortFilterProxyModel::mDetailsReturnValue = 0;
    VideoSortFilterProxyModel::mLastIndex = QModelIndex();
    emit testSignal();
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.at(0).at(0).isValid());
    QVERIFY(spysignal.at(0).at(0).toInt() == MpxHbVideoCommon::ActivateVideoDetailsView);
    QVERIFY(VideoSortFilterProxyModel::mLastIndex.row() == 0);
    
    disconnect(this, SIGNAL(testSignal()), mTestWidget, SLOT(openDetailsSlot())); 
}
 
// ---------------------------------------------------------------------------
// testBack
// ---------------------------------------------------------------------------
//
void TestListWidget::testBack()
{
    QSignalSpy spysignal(mTestWidget, SIGNAL(collectionOpened(bool, const QString&)));
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(back()));
    // no model
    hbInstance->mWindowses.append(mDummyMainWnd);
    emit testSignal();        
    QVERIFY(spysignal.count() == 0);
        
    // model exist
    mTestWidget->initialize(*mDummyProxyModel);   
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
    mTestWidget->initialize(*mDummyProxyModel);
    connect(this, SIGNAL(testSignal()), mTestWidget, SLOT(scrollingEndedSlot()));
    
    // no visible items
    HbListView::mVisibleItems.clear();
    emit testSignal();
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 0);
    QVERIFY(!mTestWidget->mScrollPositionTimer->isActive());
    
    // setup few "visible" items and make sure item count match at thumbnail data
    int count = 10;
    mDummyModel->setRowCount(count);
    HbAbstractViewItem *item = 0;
    for(int i = 0; i < count; ++i)
    {
        item = new HbAbstractViewItem();
        item->mModelIndex = mDummyModel->index(i, 0, QModelIndex());
        HbListView::mVisibleItems.append(item);
    }
    
    // Test
    emit testSignal();
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    QVERIFY(!mTestWidget->mScrollPositionTimer->isActive());

    // Test again when timer is null.
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    QTimer *backup = mTestWidget->mScrollPositionTimer;
    mTestWidget->mScrollPositionTimer = 0;
    VideoThumbnailData::mStartFetchingThumbnailsThumbnailCount = 0;
    emit testSignal();
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
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
    mTestWidget->initialize(*mDummyProxyModel);
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
