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
* Description:   TestVideoFileDetails class implementation
* 
*/

// INCLUDE FILES
#include "hbmessagebox.h"

#include <QtTest/QtTest>
#include <qdebug.h>
#include <hbinstance.h>
#include <hbwidget.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbapplication.h>
#include <hbscrollarea.h>
#include <hbmarqueeitem.h>

#include "hbmessagebox.h"
#include "thumbnailmanager_qt.h"
#include "videocollectioncommon.h"
#include "mpxhbvideocommondefs.h"
#include "videocollectionwrapper.h"
#include "videosortfilterproxymodel.h"
#include "testvideofiledetails.h"

// trick to get access to protected/private members.
#define private public
#include "videofiledetailsviewplugin.h"
#undef private

const char *TEST_VIDEO_DETAILS_VIEW = "videofiledetailsview";
const char *TEST_VIDEO_DETAILS_WIDGET = "mContent";

const char *TEST_VIDEO_DETAILS_TITLE = "mLblTitle";
const char *TEST_VIDEO_DETAILS_ITEM = "mLblDetail";
const char *TEST_VIDEO_DETAILS_BUTTON_PLAY = "mBtnPlay";
const char* const VIDEO_DETAILS_BUTTON_ATTACH = "mBtnAttach";
const char *TEST_VIDEO_DETAILS_MENUACTION_SHARE = "mOptionsShare";
const char *TEST_VIDEO_DETAILS_MENUACTION_DELETE = "mOptionsDelete";

const char *TEST_VIDEO_DETAILS_SCROLLAREA = "mDetailScrollArea";

// ---------------------------------------------------------------------------
// initTestCase
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::initTestCase()
{
   mWrapper = VideoCollectionWrapper::instance();
   mDummyModel = 0;
   mDummyModel = new VideoSortFilterProxyModel();
   
   connect(this, SIGNAL(shortDetailsReady(int)), mDummyModel, SIGNAL(shortDetailsReady(int)));
   connect(this, SIGNAL(fullDetailsReady(int)), mDummyModel, SIGNAL(fullDetailsReady(int)));

   connect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), 
           mDummyModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
   
   connect(this, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
           mDummyModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)));
   
   mWrapper->setModel(mDummyModel);
}

// ---------------------------------------------------------------------------
// cleanupTestCase
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::cleanupTestCase()
{
    disconnect(this, SIGNAL(shortDetailsReady(int)), mDummyModel, SIGNAL(shortDetailsReady(int)));
    disconnect(this, SIGNAL(fullDetailsReady(int)),mDummyModel, SIGNAL(fullDetailsReady(int)));
    disconnect(this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), 
            mDummyModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
    disconnect(this, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
            mDummyModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)));
    delete mDummyModel; mDummyModel = 0;
    delete mPlugin; mPlugin = 0;
    mWrapper->decreaseReferenceCount();
    QCOMPARE(mWrapper->mReferenceCount, 0);
}

// ---------------------------------------------------------------------------
// init before every test function
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::init()
{
    mDummyModel->reset();
    
    mPlugin = new VideoFileDetailsViewPlugin();
    mPlugin->createView();
    mPlugin->createView();
    mCommandReceived = false;
    mReceivedCommand = -1;
    ThumbnailManager::mRequests.clear();
}

// ---------------------------------------------------------------------------
// cleanup after every test function
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::cleanup()
{
    if(mPlugin->getView()) {
        hbInstance->allMainWindows().at(0)->removeView(mPlugin->getView());
    }
    mDummyModel->reset();
    delete mPlugin; mPlugin = 0;
}

// ---------------------------------------------------------------------------
// Helper function that activates view after init()
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::activateView()
{
    hbInstance->allMainWindows().at(0)->addView(mPlugin->getView());
    mPlugin->activateView();

    // without this the xml loaded objects don't have the correct geometry.
    mPlugin->getView()->setGeometry(hbInstance->allMainWindows().at(0)->rect());
}

// ---------------------------------------------------------------------------
// Helper function that populates a qmap with dummy data.
// ---------------------------------------------------------------------------
//
QMap<QString, QVariant> TestVideoFileDetails::createDummyMetadata(int ratingStartCount)
{
    using namespace VideoCollectionCommon;
    
    QMap<QString, QVariant> map;
    
    int detailCount = sizeof(VideoDetailLabelKeys) / sizeof(int);
    QString txt;
    for(int i = 0; i<detailCount; i++) {
        txt = QString(VideoDetailLabelKeys[i]);
        if(i%2 == 0)
        {            
            map[VideoDetailLabelKeys[i]] = txt;
        }
        else
        {
            txt.append(QString("test test test test test test test test test test test test test test test test test test test"));
            map[VideoDetailLabelKeys[i]] = txt;
        }
    }
    if(ratingStartCount > -1)
    {
        map[MetaKeyStarRating] = ratingStartCount;
    }
    return map;
}

// ---------------------------------------------------------------------------
// One round for the activate view test
// ---------------------------------------------------------------------------
//
inline void TestVideoFileDetails::activateViewTestRound()
    {
    init();
    
    HbScrollArea* area = findWidget<HbScrollArea>(TEST_VIDEO_DETAILS_SCROLLAREA);
    
    // won't scroll without these
    area->contentWidget()->adjustSize();
    area->setHorizontalScrollBarPolicy(HbScrollArea::ScrollBarAutoHide);
    
    area->scrollContentsTo(QPointF(0, 200));
    
    QSignalSpy spy(area, SIGNAL(scrollPositionChanged(const QPointF)));
    
    activateView();
    
    QVERIFY( mPlugin->getView() != 0 );
    QVERIFY( mPlugin->viewPlugin() == mPlugin );
    QCOMPARE( mPlugin->mVideoIndex, -1 );
    
    verifyOrientation();
    
    // verify that actions are currently disabled.
    HbPushButton* playBtn = findWidget<HbPushButton>(TEST_VIDEO_DETAILS_BUTTON_PLAY);
    HbAction* shareAction = findObject<HbAction>(TEST_VIDEO_DETAILS_MENUACTION_SHARE);
    HbAction* deleteAction = findObject<HbAction>(TEST_VIDEO_DETAILS_MENUACTION_DELETE);
    
    QVERIFY( playBtn != 0 );
    QVERIFY( shareAction != 0 );
    QVERIFY( deleteAction != 0 );
    QVERIFY( playBtn->isEnabled() );
    QVERIFY( shareAction->isEnabled() );
    QVERIFY( deleteAction->isEnabled() );
    
    // verify that the scroll area has been scrolled to the top.
    QCOMPARE( spy.count(), 1 );
    QPointF scrollPoint = spy.takeFirst().at(0).toPointF();
    QCOMPARE( scrollPoint, QPointF(0, 0) );
    
    QVERIFY(mPlugin->mActivated);    
     // second activation should not affect
    activateView();
    QVERIFY(mPlugin->mActivated);
    QVERIFY( playBtn != 0 );
    QVERIFY( shareAction != 0 );
    QVERIFY( deleteAction != 0 );
    QVERIFY( playBtn->isEnabled() );
    QVERIFY( shareAction->isEnabled() );
    QVERIFY( deleteAction->isEnabled() );
        
    
    cleanup();
    }

// ---------------------------------------------------------------------------
// Verifies that correct orientation is loaded from xml
// ---------------------------------------------------------------------------
//
inline void TestVideoFileDetails::verifyOrientation()
{
    HbWidget* info = findWidget<HbWidget>(TEST_VIDEO_DETAILS_SCROLLAREA);
    
    // this verifies that we have correct orientation loaded from the xml.
    if (hbInstance->allMainWindows().at(0)->orientation() == Qt::Vertical)
    {
        QVERIFY( info->pos().x() < 300 );
        QVERIFY( info->pos().x() > 0 );
    } else {
        QVERIFY( info->pos().x() > 300 );
        QVERIFY( info->pos().x() < 640 );
    }
}

// ---------------------------------------------------------------------------
// Slot: create view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::handleCommand(int command)
{
    mCommandReceived = true;
    mReceivedCommand = command;
}

// ---------------------------------------------------------------------------
// Slot: create view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testCreateView()
{
    init();
    // actual create view is done in the init.
    QVERIFY( mPlugin->mModel == mDummyModel );
    QVERIFY( mPlugin->mIsService == false);
    QVERIFY( mPlugin->mVideoServices == 0);
    // testing the special case where the model is null.
    cleanup();
    mWrapper->setModel(0);
    init();
    QVERIFY( mPlugin->mModel == 0 );
    QVERIFY( mPlugin->mIsService == false);
    QVERIFY( mPlugin->mVideoServices == 0);
    // TODO if the plugin is changed to throw exception in case of null model,
    //      then it needs to be added here.
    
    // restoring the proper model.
    mWrapper->setModel(mDummyModel);
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: destroy view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testDestroyView()
{
    init();
    activateView();
    mPlugin->destroyView();
    QVERIFY( mPlugin->getView() == 0 );
    // plugin is still alive
    QVERIFY( mPlugin->viewPlugin() == mPlugin );
    QVERIFY( mPlugin->mSecSkAction == 0 );
    QVERIFY( mPlugin->mActivated == false );
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: activate view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testActivateViewPortrait()
{
    hbInstance->allMainWindows().at(0)->setOrientation(Qt::Vertical);
    activateViewTestRound();  
}

// ---------------------------------------------------------------------------
// Slot: activate view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testActivateViewLandscape()
{
    hbInstance->allMainWindows().at(0)->setOrientation(Qt::Horizontal);
    activateViewTestRound();
}

// ---------------------------------------------------------------------------
// Slot: activate view
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testDeactivateView()
{
    init();
    activateView();
    mPlugin->deactivateView();
    QVERIFY( mPlugin->mActivated == false );
    QCOMPARE( mPlugin->mVideoIndex, -1 );
    mPlugin->deactivateView();
    QVERIFY( mPlugin->mActivated == false );
    QCOMPARE( mPlugin->mVideoIndex, -1 );
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: data for orientation change test
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testOrientationChange_data()
{
    QTest::addColumn<int>("start");
    QTest::addColumn<int>("switchTo");
    
    QTest::newRow("from portrait to landscape") 
            << static_cast<int>(Qt::Vertical) 
            << static_cast<int>(Qt::Horizontal);
    QTest::newRow("from landscape to portrait") 
            << static_cast<int>(Qt::Horizontal) 
            << static_cast<int>(Qt::Vertical);
}

// ---------------------------------------------------------------------------
// Slot: test for the orientationChange slot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testOrientationChange()
{
    QFETCH(int, start);
    QFETCH(int, switchTo);
    
    init();
    hbInstance->allMainWindows().at(0)->setOrientation(static_cast<Qt::Orientation>(start));
    activateView();
    
    hbInstance->allMainWindows().at(0)->setOrientation(static_cast<Qt::Orientation>(switchTo));
    
    // without this the view doesn't have the correct geometry.
    mPlugin->getView()->setGeometry(hbInstance->allMainWindows().at(0)->rect());
    // give fw some time to update content
    QTest::qWait(100);
    
    verifyOrientation();
    
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the back slot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testBack()
{
    init();
    connect(this, SIGNAL(testSignal()), mPlugin, SLOT(back()));
    connect(mPlugin, SIGNAL(command(int)), this, SLOT(handleCommand(int)));
    
    emit testSignal();
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    mPlugin->mSecSkAction->trigger();
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    activateView();
    
    emit testSignal();
    QCOMPARE( mCommandReceived, true );
    QCOMPARE( mReceivedCommand, static_cast<int>(MpxHbVideoCommon::ActivateCollectionView) );
    
    mCommandReceived = false;
    mReceivedCommand = -1;

    mPlugin->mSecSkAction->trigger();
    QCOMPARE( mCommandReceived, true );
    QCOMPARE( mReceivedCommand, static_cast<int>(MpxHbVideoCommon::ActivateCollectionView) );
    
    mCommandReceived = false;
    mReceivedCommand = -1;
    
    mPlugin->deactivateView();
    
    emit testSignal();
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    mPlugin->mSecSkAction->trigger();
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );

    disconnect(mPlugin, SIGNAL(command(int)), this, SLOT(handleCommand(int)));
    disconnect(this, SIGNAL(testSignal()), mPlugin, SLOT(back()));
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the shortDetailsReadySlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testShortDetailsReadySlot()
{
    const int testIndex = 5;
    QStringList display;
    display.append("first row");
    display.append("second row");
    QString filepath("filepath");
    
    // TODO need to add the default thumbnail checking.
    
    init();
    activateView();
    
     // no data 
    emit shortDetailsReady(testIndex);
    
    QCOMPARE( mPlugin->mVideoIndex, testIndex );
    QVERIFY( mPlugin->mTitleAnim->text().isEmpty() );
    QCOMPARE( mPlugin->mThumbnailManager->mRequests.count(), 0 );
    QVERIFY( findWidget<HbPushButton>(TEST_VIDEO_DETAILS_BUTTON_PLAY)->primitive(HbStyle::P_PushButton_background)->isVisible() == false );
    
    mDummyModel->setData(Qt::DisplayRole, display);
    mDummyModel->setData(VideoCollectionCommon::KeyFilePath, filepath);
    
    // no tn manager
    ThumbnailManager *tmpTnManager = mPlugin->mThumbnailManager;
    mPlugin->mThumbnailManager = 0;
    emit shortDetailsReady(testIndex);
    QCOMPARE( mPlugin->mVideoIndex, testIndex );
    QCOMPARE( mPlugin->mTitleAnim->text(), display.at(0) );
    QVERIFY( findWidget<HbPushButton>(TEST_VIDEO_DETAILS_BUTTON_PLAY)->primitive(HbStyle::P_PushButton_background)->isVisible() == false );    
    mPlugin->mThumbnailManager = tmpTnManager;

    // data exists
    emit shortDetailsReady(testIndex);
    
    QCOMPARE( mDummyModel->lastIndex().row(), testIndex );
    QCOMPARE( mPlugin->mTitleAnim->text(), display.at(0) );
    QVERIFY( ThumbnailManager::mRequests.contains(0) );
    ThumbnailManager::TnRequest request = ThumbnailManager::mRequests[0];
    QCOMPARE( request.name, filepath );
    // 20 == priorityHight in this case
    QCOMPARE( request.priority, 5000 );
    QVERIFY( findWidget<HbPushButton>(TEST_VIDEO_DETAILS_BUTTON_PLAY)->primitive(HbStyle::P_PushButton_background)->isVisible() == false );

    
    mDummyModel->setData(VideoCollectionCommon::KeyMetaData, createDummyMetadata() );
    emit fullDetailsReady(testIndex);
             
    emit shortDetailsReady(testIndex);
    QVERIFY( findWidget<HbPushButton>(TEST_VIDEO_DETAILS_BUTTON_PLAY)->primitive(HbStyle::P_PushButton_background)->isVisible() == false );
    
   int detailCount = sizeof(VideoCollectionCommon::VideoDetailLabelKeys) / sizeof(int);
    for(int i = 1; i<=detailCount; i++) {
        HbLabel* detail = findWidget<HbLabel>(TEST_VIDEO_DETAILS_ITEM + QString::number(i));
        QVERIFY( detail->text().isEmpty() );
    }
   
    QVERIFY( findWidget<HbPushButton>(TEST_VIDEO_DETAILS_BUTTON_PLAY)->icon().isNull() );
            
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the fullDetailsReadySlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testFullDetailsReadySlot()
{
    const int testIndex = 6;
    init();
    activateView();
    
    int detailCount = sizeof(VideoCollectionCommon::VideoDetailLabelKeys) / sizeof(int);
    for(int i = 1; i<=detailCount; i++) {
        HbLabel* detail = findWidget<HbLabel>(TEST_VIDEO_DETAILS_ITEM + QString::number(i));
        if(detail == 0)
        {
            QFAIL(QString("Found a null label at %1, check that the xml is "
                "correct, and that the resource files have been regenerated (by running abld "
                "reallyclean before build)").arg(TEST_VIDEO_DETAILS_ITEM + QString::number(i)).
                toStdString().data());
        }
    }
    
    mDummyModel->setData(VideoCollectionCommon::KeyMetaData, createDummyMetadata() );
       
    emit fullDetailsReady(testIndex);
    
    // verify that actions are currently enabled.
    HbPushButton* playBtn = findWidget<HbPushButton>(TEST_VIDEO_DETAILS_BUTTON_PLAY);
    HbAction* shareAction = findObject<HbAction>(TEST_VIDEO_DETAILS_MENUACTION_SHARE);
    HbAction* deleteAction = findObject<HbAction>(TEST_VIDEO_DETAILS_MENUACTION_DELETE);
    
    QVERIFY( playBtn != 0 );
    QVERIFY( shareAction != 0 );
    QVERIFY( deleteAction != 0 );
    QVERIFY( playBtn->isEnabled() );
    QVERIFY( shareAction->isEnabled() );
    QVERIFY( deleteAction->isEnabled() );
    
    for(int i = 0; i<detailCount; i++) {        
        QString expected = tr(VideoCollectionCommon::VideoDetailLabels[i]).arg(
                VideoCollectionCommon::VideoDetailLabelKeys[i]);
        HbLabel* detail = findWidget<HbLabel>(TEST_VIDEO_DETAILS_ITEM + QString::number(i+1));
        QVERIFY( detail != 0 );
        QVERIFY( detail->text().contains(expected) );
    }
    
    // for coverity sake, retest without star-rating
    mDummyModel->reset();
    mDummyModel->setData(VideoCollectionCommon::KeyMetaData, createDummyMetadata(1) );
    emit fullDetailsReady(testIndex);
    for(int i = 0; i<detailCount; i++) 
    {   
        if(VideoCollectionCommon::VideoDetailLabelKeys[i] != VideoCollectionCommon::MetaKeyStarRating)
        {
            QString expected = tr(VideoCollectionCommon::VideoDetailLabels[i]).arg(
                    VideoCollectionCommon::VideoDetailLabelKeys[i]);
            HbLabel* detail = findWidget<HbLabel>(TEST_VIDEO_DETAILS_ITEM + QString::number(i+1));
            QVERIFY( detail != 0 );
            QVERIFY( detail->text().contains(expected) );
        }
    }
    
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the testStartPlaybackSlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testStartPlaybackSlot()
{
    const int testIndex = 6;
    mDummyModel->reset();
    init();
    activateView();
    
    // Note that if the details view has not received signal in it's 
    // fullDetailsReadySlot, the button is disabled and should not do anything.
    
    HbPushButton* playBtn = findWidget<HbPushButton>(TEST_VIDEO_DETAILS_BUTTON_PLAY);
    QVERIFY( playBtn != 0 );
    playBtn->click();
    
    QCOMPARE( mDummyModel->startPlaybackIndex(), -1 );
    
    emit fullDetailsReady(testIndex);
    mPlugin->mVideoIndex = testIndex;
    
    playBtn->click();
    
    QCOMPARE( mDummyModel->startPlaybackIndex(), testIndex );
    
    // invalid index
    mPlugin->mVideoIndex = -1;
    playBtn->click();
    // startplayback index has not changed since previous
    QCOMPARE( mDummyModel->startPlaybackIndex(), testIndex );
    
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the sendVideoSlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testSendVideoSlot()
{
    // TODO add test after send video has been implemented.
}

// ---------------------------------------------------------------------------
// Slot: test for the deleteVideoSlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testDeleteVideoSlot()
{
    mDummyModel->reset();
    init();
    
    HbAction* deleteAction = findObject<HbAction>(TEST_VIDEO_DETAILS_MENUACTION_DELETE);
    
    QVERIFY( deleteAction != 0 ); 
    deleteAction->trigger();
    QCOMPARE( mDummyModel->dataAccessCount(), 0 );    
    mPlugin->mVideoIndex = 0;
    
    mDummyModel->setDataReturnInvalid(true);
    QModelIndex expected = mDummyModel->index(0, 0);
    deleteAction->trigger();
    QCOMPARE( mDummyModel->dataAccessCount(), 1 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( mDummyModel->deleteFileIndex(), -1 ); // verify that no file was deleted.
    mDummyModel->setDataReturnInvalid(false);

    mPlugin->mVideoIndex = 0;
    expected = mDummyModel->index(0, 0);
    deleteAction->trigger();
    QCOMPARE( mDummyModel->dataAccessCount(), 2 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( mDummyModel->deleteFileIndex(), -1 ); // verify that no file was deleted.
    
    QStringList display;
    display.append("first row");
    display.append("second row");
    mDummyModel->setData(Qt::DisplayRole, display);
    
    QString expectedText = tr("Do you want to delete \"%1\"?").arg(
            display.at(0));
    
    HbMessageBox::mQuestionReturnValue = true;
    deleteAction->trigger();
    QCOMPARE( mDummyModel->dataAccessCount(), 3 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( mDummyModel->deleteFileIndex(), expected.row() );
    QCOMPARE( HbMessageBox::mLatestTxt, expectedText );
    
    HbMessageBox::mQuestionReturnValue = false;
    deleteAction->trigger();
    QCOMPARE( mDummyModel->dataAccessCount(), 4 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( mDummyModel->deleteFileIndex(), expected.row() );
    QCOMPARE( HbMessageBox::mLatestTxt, expectedText );
    

    mDummyModel->reset();
    HbMessageBox::mQuestionReturnValue = false;
    deleteAction->trigger();
    QCOMPARE( mDummyModel->dataAccessCount(), 1 );
    QVERIFY( mDummyModel->lastIndex() == expected );
    QCOMPARE( mDummyModel->deleteFileIndex(), -1 ); // verify that no file was deleted.
    QCOMPARE( HbMessageBox::mLatestTxt, expectedText );
    
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test for the testRowsRemovedSlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testRowsRemovedSlot()
{
    const int testIndex = 9;
    init();
    connect(mPlugin, SIGNAL(command(int)), this, SLOT(handleCommand(int)));
    
    emit rowsRemoved(QModelIndex(), testIndex, testIndex);
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    activateView();
    
    emit rowsRemoved(QModelIndex(), testIndex, testIndex);
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    emit fullDetailsReady(testIndex);
    mPlugin->mVideoIndex = testIndex;
    
    emit rowsRemoved(QModelIndex(), testIndex+1, testIndex-1);
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );

    emit rowsRemoved(QModelIndex(), testIndex-3, testIndex-1);
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    emit rowsRemoved(QModelIndex(), testIndex+1, testIndex+3);
    QCOMPARE( mCommandReceived, false );
    QCOMPARE( mReceivedCommand, -1 );
    
    emit rowsRemoved(QModelIndex(), testIndex, testIndex);
    QCOMPARE( mCommandReceived, true );
    QCOMPARE( mReceivedCommand, static_cast<int>(MpxHbVideoCommon::ActivateCollectionView) );
    
    disconnect(mPlugin, SIGNAL(command(int)), this, SLOT(handleCommand(int)));
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: testHandleErrorSlot test error messages
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testHandleErrorSlot()
{
    init();
    connect(this, SIGNAL(testErrorSignal(int, QVariant&)), mPlugin, SLOT(handleErrorSlot(int, QVariant&)));

    HbMessageBox::mLatestTxt = "";
    QVariant additional =  QVariant();
    // invalid msg,
    emit testErrorSignal( VideoCollectionCommon::statusMultipleDeleteFail + 1, additional);
    
    QVERIFY(HbMessageBox::mLatestTxt.isEmpty());
     
    // VideoCollectionCommon::statusSingleDeleteFail
    // - without additional data
    emit testErrorSignal( VideoCollectionCommon::statusSingleDeleteFail , additional);
    QVERIFY(HbMessageBox::mLatestTxt.isEmpty());
    
    // - with additional data
    QString txt = "testdata";
    additional = txt;
    emit testErrorSignal( VideoCollectionCommon::statusSingleDeleteFail , additional);
    QCOMPARE(HbMessageBox::mLatestTxt, QString("Unable to delete item %1. It is currently open.").arg(txt));
    
    HbMessageBox::mLatestTxt = "";
    // VideoCollectionCommon::statusMultipleDeleteFail
    emit testErrorSignal( VideoCollectionCommon::statusMultipleDeleteFail , additional);
    QVERIFY(HbMessageBox::mLatestTxt.isEmpty());
    
    disconnect(this, SIGNAL(testErrorSignal(int, QVariant&)), mPlugin, SLOT(handleErrorSlot(int, QVariant&)));
    
    cleanup();
}


// ---------------------------------------------------------------------------
// Slot: test for the testThumbnailReadySlot
// ---------------------------------------------------------------------------
//
void TestVideoFileDetails::testThumbnailReadySlot()
{
    int testIndex = 9;
    QStringList display;
    display.append("first row");
    display.append("second row");
    QString filepath("filepath");
    
    init();
    activateView();
    
    mDummyModel->setData(Qt::DisplayRole, display);
    mDummyModel->setData(VideoCollectionCommon::KeyFilePath, filepath);

    emit shortDetailsReady(testIndex);
    
    mPlugin->mThumbnailManager->mThumbnailReadyError = -1;
    mPlugin->mThumbnailManager->emitThumbnailReady(0);
    
    QVERIFY( findWidget<HbPushButton>(TEST_VIDEO_DETAILS_BUTTON_PLAY)->icon().qicon().isNull() == false );
    
    mPlugin->mThumbnailManager->mThumbnailReadyError = 0;
    emit shortDetailsReady(testIndex);
    mPlugin->mThumbnailManager->emitThumbnailReady(0);
    
    QVERIFY( findWidget<HbPushButton>(TEST_VIDEO_DETAILS_BUTTON_PLAY)->icon().qicon().isNull() == false );
    
    cleanup();
}


// ---------------------------------------------------------------------------
// findWidget
// ---------------------------------------------------------------------------
//
template<class T> 
T* TestVideoFileDetails::findWidget(QString name)
{
    return qobject_cast<T *>(mPlugin->mView.findWidget(name));
}

// ---------------------------------------------------------------------------
// findObject
// ---------------------------------------------------------------------------
//
template<class T> 
T* TestVideoFileDetails::findObject(QString name)
{
    return qobject_cast<T *>(mPlugin->mView.findObject(name));
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbApplication::setKeypadNavigationEnabled(false);
    HbMainWindow mainWindow;
    mainWindow.show();
    
    TestVideoFileDetails tc;
    
    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideofiledetailsviewplugin.txt";

    return QTest::qExec(&tc, 3, pass);
}

// end of file
