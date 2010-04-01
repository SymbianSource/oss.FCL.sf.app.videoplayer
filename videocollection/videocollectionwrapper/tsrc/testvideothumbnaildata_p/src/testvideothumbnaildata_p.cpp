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
* Description:   Unit test for methods in VideoThumbnailDataPrivate
*
*/

// INCLUDES
#include <hbapplication.h>
#include <hbinstance.h>
#include <qicon.h>
#include <qpair.h>
#include <qpointer.h>
#include <qlist.h>
#include <qvariant.h>
#include "qmetatype.h"

#include "testvideothumbnaildata_p.h"
#include "thumbnailmanager_qt.h"
#include "videosortfilterproxymodel.h"
#include "videocollectionwrapper.h"

#include "e32std.h"

#define private public
#define protected public
#include "videothumbnaildatatester.h"
#undef private
#undef protected

#include "videothumbnaildata_p.h"

const int TB_PRIORITY = 1000;

// following constants are copied from videothumbnaildata.cpp
// Maximum thumbnails kept in memory.
const int THUMBNAIL_CACHE_SIZE = 60;
// Maximum of thumbnail fetches done at one background fetch round.
const int THUMBNAIL_BACKGROUND_FETCH_AMOUNT = 20;
// Maximum simulatenous thumbnail fetches.
const int THUMBNAIL_MAX_SIMULTANEOUS_FETCHES = THUMBNAIL_BACKGROUND_FETCH_AMOUNT * 10;
// Priority for background thumbnail fetches.
const int BACKGROUND_FETCH_PRIORITY = 3000;

Q_DECLARE_METATYPE(QList<TMPXItemId>)

/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestVideoThumbnailData_p tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestVideoThumbnailData_p.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::init()
{
    VideoCollectionWrapper::mReferenceCount = 0;
    
    if(!mWrapper)
        mWrapper = VideoCollectionWrapper::instance();
    mModel = new VideoSortFilterProxyModel();
    mWrapper->setModel(mModel);
    mTestObject = new VideoThumbnailDataTester();
    mTestObject->initialize();
    ThumbnailManager::mGetThumbFails = false;
    VideoSortFilterProxyModel::mReturnInvalidIndexes = false;
    VideoSortFilterProxyModel::mRowCountCallCount = 0;
    qRegisterMetaType<QList<TMPXItemId> >("QList<TMPXItemId>");
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::cleanup()
{
    delete mTestObject; mTestObject = 0;
    delete mModel; mModel = 0;
    ThumbnailManager::mRequests.clear();
    if(mWrapper)
    {
        mWrapper->decreaseReferenceCount();
        mWrapper = 0;
    }
    QCOMPARE(mWrapper->mReferenceCount, 0);
}

// ---------------------------------------------------------------------------
// checkThumbnailReadyCount
// ---------------------------------------------------------------------------
//
bool TestVideoThumbnailData_p::checkThumbnailReadyCount(QSignalSpy *spy, int count)
{
    if(!mTestObject || !spy) 
        {
        qWarning("TestVideoThumbnailData_p::checkThumbnailReadyCount: error, mTestObject = %d, spy = %d", mTestObject, spy); 
        return false;
        }
    mTestObject->emitReportThumbnailsReady();
    if(spy->count() != 1)
    {
        qWarning("TestVideoThumbnailData_p::checkThumbnailReadyCount: error, spy->count() is %d, 1 was expected.", spy->count());
        return false;
    }
    QList<TMPXItemId> mediaIds = qvariant_cast<QList<TMPXItemId> >(spy->at(0).at(0));
    return mediaIds.count() == count;
}

// ---------------------------------------------------------------------------
// testDestructor
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testDestructor()
{
    mWrapper = 0;
    
    //User::Heap().__DbgMarkStart();
    mTestObject = new VideoThumbnailDataTester();
    delete mTestObject; mTestObject = 0;
    ulong heapCellPtr = User::Heap().__DbgMarkEnd(0);
    //QCOMPARE( heapCellPtr, (ulong)0 );

    //User::Heap().__DbgMarkStart();
    init();
    cleanup(); // should not crash...
    heapCellPtr = User::Heap().__DbgMarkEnd(0);
    //QCOMPARE( heapCellPtr, (ulong)0 );

    //User::Heap().__DbgMarkStart();
    init();
    QPointer<ThumbnailManager> tnMgrPtr(mTestObject->mThumbnailManager);
    ThumbnailManager::mRequests[0] = ThumbnailManager::TnRequest("first", 0, 0, false);
    ThumbnailManager::mRequests[1] = ThumbnailManager::TnRequest("second", 0, 0, false);
    mTestObject->mFetchList.insert(0);
    mTestObject->mFetchList.insert(1);
    mTestObject->mThumbnailData.insert(TMPXItemId(2, 0), new QIcon());
    mTestObject->mThumbnailData.insert(TMPXItemId(3, 0), new QIcon());
    cleanup(); // should not crash...
    QVERIFY( tnMgrPtr == 0 );
    ThumbnailManager::TnRequest req;
    foreach(req, ThumbnailManager::mRequests) {
        QVERIFY( req.cancelled );
    }
    heapCellPtr = User::Heap().__DbgMarkEnd(0);
    //QCOMPARE( heapCellPtr, (ulong)0 );
}

// ---------------------------------------------------------------------------
// testInitialize
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testInitialize()
{
    // Wrapper return null model
    if(!mWrapper)
        mWrapper = VideoCollectionWrapper::instance();
    mWrapper->setModel(0);

    mTestObject = new VideoThumbnailDataTester();
    QVERIFY(mTestObject->mThumbnailManager == 0);
    QVERIFY(mTestObject->mModel == 0);
    QVERIFY(mTestObject->mModel == 0);
    QVERIFY(mTestObject->mBgFetchTimer == 0);
    cleanup();

    mTestObject = new VideoThumbnailDataTester();
    mTestObject->disconnectSignals();
    delete mTestObject->mBgFetchTimer;
    mTestObject->mBgFetchTimer = 0;
    delete mTestObject->mThumbnailManager;
    mTestObject->mThumbnailManager = 0;
    mTestObject->mModel = 0;
    __UHEAP_FAILNEXT(3);
    mTestObject->initialize();
    QVERIFY(mTestObject->mThumbnailManager == 0);
    QVERIFY(mTestObject->mModel == 0);
    QVERIFY(mTestObject->mBgFetchTimer == 0);
    cleanup();

    init();
    mTestObject->initialize();
    mTestObject->initialize();
    QVERIFY(mTestObject->mThumbnailManager != 0);
    QVERIFY(mTestObject->mModel != 0);
    QVERIFY(mTestObject->mBgFetchTimer != 0);
    QCOMPARE( mTestObject->mThumbnailManager->mThumbSize, ThumbnailManager::ThumbnailMedium );
    QCOMPARE( mTestObject->mThumbnailManager->mQuality, ThumbnailManager::OptimizeForPerformance );
    cleanup();
}

// ---------------------------------------------------------------------------
// testGetThumbnail
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testGetThumbnail()
{

    int mediaid(50);
    QPixmap pixmap(100,100);
    pixmap.fill(Qt::blue);

    init();

    mTestObject->mThumbnailData.insert(TMPXItemId(mediaid, 0), new QIcon(pixmap));
    QCOMPARE( *(mTestObject->getThumbnail(TMPXItemId(mediaid, 0))), *(mTestObject->mThumbnailData[TMPXItemId(mediaid, 0)]) );
    QVERIFY( mTestObject->getThumbnail(TMPXItemId(mediaid+100, 0)) == mTestObject->mDefaultTnVideo );

    cleanup();
}

// ---------------------------------------------------------------------------
// testStartFetchingThumbnail
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testStartFetchingThumbnail()
{
    QSignalSpy* spy = 0;
    ThumbnailManager::TnRequest req;
    
    // Tests when mModel is null.
    init();
    VideoSortFilterProxyModel* backupProxyModel = mTestObject->mModel;
    mTestObject->mModel = NULL;
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    mTestObject->mModel = backupProxyModel;
    cleanup();

    // Tests when mThumbnailManager is null.
    init();
    ThumbnailManager* backupTnMgr = mTestObject->mThumbnailManager;
    mTestObject->mThumbnailManager = NULL;
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    mTestObject->mThumbnailManager = backupTnMgr;
    cleanup();

    // Tests when there's max fetches ongoing.
    init();
    for(int i = 0; i < THUMBNAIL_MAX_SIMULTANEOUS_FETCHES+10; i++)
    {
        mTestObject->mFetchList.insert(i);
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), THUMBNAIL_MAX_SIMULTANEOUS_FETCHES+10);
    delete spy;
    cleanup();

    // Thumbnail has been already fetched.
    init();
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    mTestObject->mThumbnailData.insert(TMPXItemId(10, 0), 0);
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(10, 0), TB_PRIORITY), 0);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    delete spy;
    cleanup();

    // Thumbnail fetch request to tn manager fails.
    init();
    ThumbnailManager::mGetThumbFails = true;
    mTestObject->mModel->appendData("testfile");
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    delete spy;
    cleanup();

    // Filename is empty.
    init();
    ThumbnailManager::mGetThumbFails = true;
    mTestObject->mModel->appendData("");
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    delete spy;
    cleanup();

    // Filename is null.
    init();
    ThumbnailManager::mGetThumbFails = true;
    mTestObject->mModel->appendData(QString());
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    delete spy;
    cleanup();

    // Good case.
    init();
    QString fileName("video.mp4");
    mTestObject->mModel->appendData(fileName);
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), 0);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), 1);
    req = ThumbnailManager::mRequests[0];
    QVERIFY(req.name == fileName);
    QCOMPARE(req.priority, TB_PRIORITY);
    delete spy;
    cleanup();

    // Already fetching same thumbnail.
    init();
    mTestObject->mModel->appendData(fileName);
    ThumbnailManager::mRequests[1] = ThumbnailManager::TnRequest("test1", 0, -1, false);
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnail(TMPXItemId(0, 0), TB_PRIORITY), 1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), 1);
    req = ThumbnailManager::mRequests[1];
    QVERIFY(req.name == fileName);
    QCOMPARE(req.priority, TB_PRIORITY);
    delete spy;
    cleanup();
}

// ---------------------------------------------------------------------------
// testStartFetchingThumbnails
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testStartFetchingThumbnails()
{
    ThumbnailManager::TnRequest req;
    QList<QModelIndex> indexes;
    QSignalSpy* spy = 0;
    
    // Tests when mModel is null.
    init();
    indexes.clear();
    indexes.append(mTestObject->mModel->index(0, 0, QModelIndex()));
    indexes.append(mTestObject->mModel->index(10, 0, QModelIndex()));
    indexes.append(mTestObject->mModel->index(20, 0, QModelIndex()));
    VideoSortFilterProxyModel* backupProxyModel = mTestObject->mModel;
    mTestObject->mModel = NULL;
    
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnails(indexes,
            BACKGROUND_FETCH_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    mTestObject->mModel = backupProxyModel;
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    cleanup();

    // Tests when mThumbnailManager is null.
    init();
    indexes.clear();
    ThumbnailManager* backupTnMgr = mTestObject->mThumbnailManager;
    mTestObject->mThumbnailManager = NULL;
    indexes.append(mTestObject->mModel->index(0, 0, QModelIndex()));
    indexes.append(mTestObject->mModel->index(10, 0, QModelIndex()));
    indexes.append(mTestObject->mModel->index(20, 0, QModelIndex()));
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnails(indexes,
            BACKGROUND_FETCH_PRIORITY), -1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    mTestObject->mThumbnailManager = backupTnMgr;
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    cleanup();

    // Tests when parameter array indexes is empty.
    init();
    indexes.clear();
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnails(indexes,
            BACKGROUND_FETCH_PRIORITY), 0);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    cleanup();

    // Already fetching some thumbnails.
    init();
    QString fileName("video.mp4");
    mTestObject->mModel->appendData(fileName);
    indexes.clear();
    ThumbnailManager::mRequests[50] = ThumbnailManager::TnRequest("test1", 0, -1, false);
    ThumbnailManager::mRequests[40] = ThumbnailManager::TnRequest("test2", 0, -1, false);
    ThumbnailManager::mRequests[30] = ThumbnailManager::TnRequest("test3", 0, -1, false);
    mTestObject->mFetchList.insert(50);
    mTestObject->mFetchList.insert(40);
    mTestObject->mFetchList.insert(30);
    indexes.append(mTestObject->mModel->index(0, 0, QModelIndex()));
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnails(indexes,
            BACKGROUND_FETCH_PRIORITY), 1);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 4);
    
    req = ThumbnailManager::mRequests[50];
    QVERIFY(req.name == "test1");
    QCOMPARE(req.priority, -1);
    
    req = ThumbnailManager::mRequests[40];
    QVERIFY(req.name == "test2");
    QCOMPARE(req.priority, -1);
    
    req = ThumbnailManager::mRequests[30];
    QVERIFY(req.name == "test3");
    QCOMPARE(req.priority, -1);
    
    req = ThumbnailManager::mRequests[3];
    QVERIFY(req.name == fileName);
    QCOMPARE(req.priority, static_cast<int>(BACKGROUND_FETCH_PRIORITY) + 4);
    
    ThumbnailManager::mRequests.clear();
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    cleanup();

    // Ok case
    init();
    mTestObject->mModel->appendData("file1");
    mTestObject->mModel->appendData("file2");
    mTestObject->mModel->appendData("file3");
    mTestObject->mModel->appendData("file4");
    mTestObject->mModel->appendData("file5");
    mTestObject->mModel->appendData("file6");
    indexes.clear();
    indexes.append(mTestObject->mModel->index(2, 0, QModelIndex()));
    indexes.append(mTestObject->mModel->index(3, 0, QModelIndex()));
    indexes.append(mTestObject->mModel->index(4, 0, QModelIndex()));
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    QCOMPARE(mTestObject->startFetchingThumbnails(indexes,
            BACKGROUND_FETCH_PRIORITY), 3);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 3);

    req = ThumbnailManager::mRequests[0];
    QVERIFY(req.name == "file3");
    QCOMPARE(req.priority, static_cast<int>(BACKGROUND_FETCH_PRIORITY) + 3);
    
    req = ThumbnailManager::mRequests[1];
    QVERIFY(req.name == "file4");
    QCOMPARE(req.priority, static_cast<int>(BACKGROUND_FETCH_PRIORITY) + 3 - 1);
    
    req = ThumbnailManager::mRequests[2];
    QVERIFY(req.name == "file5");
    QCOMPARE(req.priority, static_cast<int>(BACKGROUND_FETCH_PRIORITY) + 3 - 2);
    cleanup();
}

// ---------------------------------------------------------------------------
// testDoBackgroundFetching
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testDoBackgroundFetching()
{
    ThumbnailManager::TnRequest req;
    QSignalSpy* spy = 0;
    
    // Tests when mModel is null.
    init();
    VideoSortFilterProxyModel* backupProxyModel = mTestObject->mModel;
    mTestObject->mModel = NULL;
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    mTestObject->mModel = backupProxyModel;
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QCOMPARE(VideoSortFilterProxyModel::mRowCountCallCount, 0);
    cleanup();

    // Tests when mThumbnailManager is null.
    init();
    ThumbnailManager* backupTnMgr = mTestObject->mThumbnailManager;
    mTestObject->mThumbnailManager = NULL;
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    mTestObject->mThumbnailManager = backupTnMgr;
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QCOMPARE(VideoSortFilterProxyModel::mRowCountCallCount, 1);
    cleanup();

    // THUMBNAIL_BACKGROUND_FETCH_AMOUNT items in model and fetch index at 0
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT; i++)
    {
        mTestObject->mModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2);
    int previousPriority = -1;
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2; i++)
    {
        req = ThumbnailManager::mRequests[i];
        QVERIFY(req.name == "file" + QString::number(i));
        if(previousPriority != -1)
        {
            QVERIFY(previousPriority > req.priority);    
        }
        previousPriority = req.priority;
        QVERIFY(previousPriority != -1);
    }
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QVERIFY(mTestObject->mCurrentBackgroundFetchCount == THUMBNAIL_BACKGROUND_FETCH_AMOUNT);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);

    // Continue fetching.
    spy->clear();
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    // Now all thumbnails are being fetched.
    QCOMPARE(mTestObject->mFetchList.count(), THUMBNAIL_BACKGROUND_FETCH_AMOUNT);
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2; i++)
    {
        req = ThumbnailManager::mRequests[i];
        QVERIFY(req.name == "file" + QString::number(i));

        req = ThumbnailManager::mRequests[i+5];
        QVERIFY(req.name == "file" + QString::number(i+5));
    }

    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QVERIFY(mTestObject->mCurrentBackgroundFetchCount == THUMBNAIL_BACKGROUND_FETCH_AMOUNT * 2);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);

    delete spy;
    cleanup();

    // THUMBNAIL_BACKGROUND_FETCH_AMOUNT items in model and fetch index negative
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT; i++)
    {
        mTestObject->mModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    mTestObject->mCurrentFetchIndex = THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2*-1;
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2*-1);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == true);
    cleanup();

    // THUMBNAIL_BACKGROUND_FETCH_AMOUNT*2 items in model and fetch index at end
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT*2; i++)
    {
        mTestObject->mModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    mTestObject->mCurrentFetchIndex = THUMBNAIL_BACKGROUND_FETCH_AMOUNT*2;
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2);
    QVERIFY(mTestObject->mCurrentFetchIndex == THUMBNAIL_BACKGROUND_FETCH_AMOUNT*2);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);
    cleanup();

    // THUMBNAIL_BACKGROUND_FETCH_AMOUNT items in model and fetch index over the end
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT; i++)
    {
        mTestObject->mModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    mTestObject->mCurrentFetchIndex = THUMBNAIL_BACKGROUND_FETCH_AMOUNT + THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2;
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == THUMBNAIL_BACKGROUND_FETCH_AMOUNT + THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == true);
    cleanup();

    // no items and fetch index -5
    init();
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    mTestObject->mCurrentFetchIndex = -5;
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    delete spy;
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == -5);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);
    cleanup();

    // Model returns invalid indexes.
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT; i++)
    {
        mTestObject->mModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    VideoSortFilterProxyModel::mReturnInvalidIndexes = true;
    mTestObject->mCurrentFetchIndex = THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2;
    mTestObject->emitDoBackgroundFetching();
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2);
    QVERIFY(mTestObject->mCurrentBackgroundFetchCount == THUMBNAIL_BACKGROUND_FETCH_AMOUNT);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == true);

    // More than maximum background fetched items in model, do bg fetch until max amount of thumbnails.
    init();
    int itemAmount = THUMBNAIL_MAX_SIMULTANEOUS_FETCHES*2;
    for(int i = 0; i < itemAmount; i++)
    {
        mTestObject->mModel->appendData(QString("file") + QString::number(i));
    }
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    mTestObject->mCurrentFetchIndex = itemAmount/2;

    int fetchTimes = THUMBNAIL_MAX_SIMULTANEOUS_FETCHES/THUMBNAIL_BACKGROUND_FETCH_AMOUNT+2;
    for(int i = 0; i < fetchTimes; i++)
    {
        mTestObject->mBgFetchTimer->stop();
        QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);
        mTestObject->emitDoBackgroundFetching();
    }
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), THUMBNAIL_CACHE_SIZE);
    QCOMPARE(mTestObject->mCurrentBackgroundFetchCount, THUMBNAIL_CACHE_SIZE);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);
    cleanup();
}

// ---------------------------------------------------------------------------
// testThumbnailReadySlot
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testThumbnailReadySlot()
{
    QString fileName("filename");
    QString fileName2("filename2");
    QSignalSpy* spy = 0;
    User::Heap().__DbgMarkStart();

    int mediaId1 = 1;
    int mediaId2 = 2;

    init();
    mTestObject->mModel->appendData("notusedfile0");
    mTestObject->mModel->appendData(fileName); // id 1
    mTestObject->mModel->appendData(fileName2); // id 2
    mTestObject->mModel->appendData("notusedfile1");
    mTestObject->mModel->appendData("notusedfile2");
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    int tnId1 = mTestObject->startFetchingThumbnail(TMPXItemId(mediaId1, 0), TB_PRIORITY);
    int tnId2 = mTestObject->startFetchingThumbnail(TMPXItemId(mediaId2, 0), TB_PRIORITY);
    QVERIFY( mTestObject->mThumbnailData.contains(TMPXItemId(mediaId1, 0)) == false );
    QVERIFY( mTestObject->mThumbnailData.contains(TMPXItemId(mediaId2, 0)) == false );    
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), 2);

    spy->clear();
    mTestObject->mThumbnailManager->emitThumbnailReady(10);
    QVERIFY( mTestObject->mThumbnailData.contains(TMPXItemId(mediaId1, 0)) == false );
    QVERIFY( mTestObject->mThumbnailData.contains(TMPXItemId(mediaId2, 0)) == false );
    QVERIFY( mTestObject->mFetchList.contains(tnId1) );
    QVERIFY( mTestObject->mFetchList.contains(tnId2) );
    QVERIFY(checkThumbnailReadyCount(spy, 0));

    spy->clear();
    mTestObject->mThumbnailManager->mThumbnailReadyError = -1;
    mTestObject->mThumbnailManager->emitThumbnailReady(tnId1);
    QVERIFY( mTestObject->mThumbnailData.contains(TMPXItemId(mediaId1, 0)) == false );
    QVERIFY( mTestObject->mThumbnailData.contains(TMPXItemId(mediaId2, 0)) == false );
    QVERIFY( mTestObject->mFetchList.contains(tnId1) == false );
    QVERIFY( mTestObject->mFetchList.contains(tnId2) );
    QVERIFY(checkThumbnailReadyCount(spy, 0));

    spy->clear();
    mTestObject->mThumbnailManager->mThumbnailReadyError = 0;
    mTestObject->mThumbnailManager->emitThumbnailReady(tnId2);
    QVERIFY( mTestObject->mThumbnailData.contains(TMPXItemId(mediaId1, 0)) == false );
    QVERIFY( mTestObject->mThumbnailData.contains(TMPXItemId(mediaId2, 0)) );
    QVERIFY( mTestObject->mFetchList.contains(tnId1) == false );
    QVERIFY( mTestObject->mFetchList.contains(tnId2) == false );
    QVERIFY(checkThumbnailReadyCount(spy, 1));
    delete spy;
    cleanup();

    ThumbnailManager::mRequests.clear();
    ulong heapCellPtr = User::Heap().__DbgMarkEnd(0);
    //QCOMPARE( heapCellPtr, (ulong)0 );
    
    // Test null thumbnail data.
    
    init();
    mTestObject->mModel->appendData("notusedfile0");
    mTestObject->mModel->appendData(fileName); // id 1
    mTestObject->mModel->appendData(fileName2); // id 2
    mTestObject->mModel->appendData("notusedfile1");
    mTestObject->mModel->appendData("notusedfile2");
    spy = new QSignalSpy(mTestObject, SIGNAL(thumbnailsFetched(QList<TMPXItemId>)));
    tnId1 = mTestObject->startFetchingThumbnail(TMPXItemId(mediaId1, 0), TB_PRIORITY);
    tnId2 = mTestObject->startFetchingThumbnail(TMPXItemId(mediaId2, 0), TB_PRIORITY);
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    QCOMPARE(mTestObject->mFetchList.count(), 2);

    spy->clear();
    mTestObject->mThumbnailManager->emitThumbnailReady(tnId1, true);
    QVERIFY( mTestObject->mThumbnailData.contains(TMPXItemId(mediaId1, 0)) == false );
    QVERIFY( mTestObject->mThumbnailData.contains(TMPXItemId(mediaId2, 0)) == false );
    QVERIFY( mTestObject->mFetchList.contains(tnId1) == false);
    QVERIFY( mTestObject->mFetchList.contains(tnId2) );
    QVERIFY(checkThumbnailReadyCount(spy, 0));
    cleanup();
}

// ---------------------------------------------------------------------------
// testDefaultThumbnail
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testDefaultThumbnail()
{
    init();

    QVERIFY( mTestObject->mDefaultTnVideo == 0 );
    QVERIFY( mTestObject->mDefaultTnCategory == 0 );
    
    // Tn for video
    const QIcon* tn = mTestObject->defaultThumbnail(TMPXItemId(1, 0));
    QVERIFY( tn != 0 );
    QVERIFY( tn->isNull() == false );
    QCOMPARE( tn->cacheKey(), mTestObject->mDefaultTnVideo->cacheKey() );

    // Tn for category
    const QIcon* tn2 = mTestObject->defaultThumbnail(TMPXItemId(0, 1));
    QVERIFY( tn2 != 0 );
    QVERIFY( tn2->isNull() == false );
    QCOMPARE( tn2->cacheKey(), mTestObject->mDefaultTnCategory->cacheKey() );

    QVERIFY(tn2->cacheKey() != tn->cacheKey());
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testRemoveThumbnail
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testRemoveThumbnail()
{
    init();

    TMPXItemId id1(1, 0);
    TMPXItemId id2(2, 0);
    TMPXItemId id3(3, 0);
    TMPXItemId id4(4, 0);
    
    mTestObject->mThumbnailData.insert(id2, new QIcon());
    mTestObject->mThumbnailData.insert(id3, new QIcon());

    QCOMPARE( mTestObject->removeThumbnail(id1), 0 );
    QCOMPARE( mTestObject->removeThumbnail(id2), 1 );
    QCOMPARE( mTestObject->removeThumbnail(id3), 1 );
    QCOMPARE( mTestObject->removeThumbnail(id4), 0 );

    QVERIFY( mTestObject->mThumbnailData.contains(id2) == false );
    QVERIFY( mTestObject->mThumbnailData.contains(id3) == false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testLayoutChangedSlot
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testLayoutChangedSlot()
{
    // Empty list.
    init();
    mTestObject->emitLayoutChanged();
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QVERIFY(mTestObject->mCurrentBackgroundFetchCount == 0);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);
    cleanup();

    // THUMBNAIL_BACKGROUND_FETCH_AMOUNT items in model and fetch index at 0
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT; i++)
    {
        mTestObject->mModel->appendData(QString("file") + QString::number(i));
    }
    mTestObject->emitLayoutChanged();
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QCOMPARE(mTestObject->mFetchList.count(), THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2);
    QVERIFY(mTestObject->mCurrentBackgroundFetchCount == THUMBNAIL_BACKGROUND_FETCH_AMOUNT);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);
    cleanup();
}

// ---------------------------------------------------------------------------
// testRowsInsertedSlot
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testRowsInsertedSlot()
{
    init();

    // Empty list.
    mTestObject->emitRowsInserted(QModelIndex(), 0, 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QVERIFY(mTestObject->mCurrentBackgroundFetchCount == 0);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);
    cleanup();

    init();
    // With data.
    // THUMBNAIL_BACKGROUND_FETCH_AMOUNT items in model and fetch index at 0
    init();
    for(int i = 0; i < THUMBNAIL_BACKGROUND_FETCH_AMOUNT; i++)
    {
        mTestObject->mModel->appendData(QString("file") + QString::number(i));
    }
    mTestObject->emitRowsInserted(QModelIndex(), 0, 0);
    QVERIFY(mTestObject->mCurrentFetchIndex == 0);
    QCOMPARE(mTestObject->mFetchList.count(), THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2);
    QVERIFY(mTestObject->mCurrentBackgroundFetchCount == THUMBNAIL_BACKGROUND_FETCH_AMOUNT);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);

    cleanup();
}

// ---------------------------------------------------------------------------
// testRemoveFromFetchList
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testRemoveFromFetchList()
{
    init();

    // Empty list.
    mTestObject->removeFromFetchList(0);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);

    // Invalid index.
    mTestObject->removeFromFetchList(-5);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);

    // With data.
    mTestObject->mFetchList.insert(0);
    mTestObject->removeFromFetchList(0);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == true);
    cleanup();

    init();

    // With more data.
    mTestObject->mFetchList.insert(0);
    mTestObject->mFetchList.insert(1);
    mTestObject->mFetchList.insert(2);
    mTestObject->removeFromFetchList(0);
    QVERIFY(mTestObject->mBgFetchTimer->isActive() == false);

    cleanup();
}

// ---------------------------------------------------------------------------
// testStartBackgroundFetching
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testStartBackgroundFetching()
{
    init();

    // Bg fetch enabled
    mTestObject->mBackgroundFetchingEnabled = true;
    mTestObject->mCurrentFetchIndex = -5;
    mTestObject->startBackgroundFetching(10);
    QVERIFY(mTestObject->mCurrentFetchIndex == 10);

    // Bg fetch disabled
    mTestObject->mBackgroundFetchingEnabled = false;
    mTestObject->mCurrentFetchIndex = -5;
    mTestObject->startBackgroundFetching(10);
    QVERIFY(mTestObject->mCurrentFetchIndex == -5);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testEnableBackgroundFetching
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testEnableBackgroundFetching()
{
    init();

    // Default
    QVERIFY(mTestObject->mBackgroundFetchingEnabled == true);
    
    // Disable
    mTestObject->mCurrentFetchIndex = -1;
    mTestObject->enableBackgroundFetching(false);
    QVERIFY(mTestObject->mBackgroundFetchingEnabled == false);
    QCOMPARE(mTestObject->mCurrentFetchIndex, -1);
    
    mTestObject->mBgFetchTimer->setSingleShot(false);
    mTestObject->continueBackgroundFetch();
    QVERIFY(mTestObject->mBgFetchTimer->isSingleShot() == false);
    
    // Enabled
    mTestObject->mCurrentFetchIndex = -1;

    mTestObject->enableBackgroundFetching(true);
    QVERIFY(mTestObject->mBackgroundFetchingEnabled == true);    
    QCOMPARE(mTestObject->mCurrentFetchIndex, 0);
    
    mTestObject->mBgFetchTimer->setSingleShot(false);
    mTestObject->continueBackgroundFetch();
    QVERIFY(mTestObject->mBgFetchTimer->isSingleShot() == true);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testFreeThumbnailData
// ---------------------------------------------------------------------------
//
void TestVideoThumbnailData_p::testFreeThumbnailData()
{
    init();

    QVERIFY( mTestObject->getThumbnail(TMPXItemId(1, 0)) == mTestObject->mDefaultTnVideo );
    QVERIFY( mTestObject->getThumbnail(TMPXItemId(0, 1)) == mTestObject->mDefaultTnCategory );
    mTestObject->freeThumbnailData();
    
    QVERIFY(!mTestObject->mBgFetchTimer->isActive());
    QVERIFY(!mTestObject->mTbnReportTimer->isActive());
    QCOMPARE(mTestObject->mFetchList.count(), 0);
    QCOMPARE(mTestObject->mReadyThumbnailMediaIds.count(), 0);
    QCOMPARE(mTestObject->mThumbnailData.count(), 0);
    
    QVERIFY(mTestObject->mDefaultTnVideo == 0);
    QVERIFY(mTestObject->mDefaultTnVideo == 0);

    // Call again.
    mTestObject->freeThumbnailData();

    cleanup();
}
// End of file
