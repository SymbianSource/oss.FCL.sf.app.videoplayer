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
* Description:   tester for methods in VideoSortFilterProxyModel
* 
*/

#include <QModelIndex>
#include <QModelIndexList>
#include <hbapplication.h>

#define private public
#include "videosortfilterproxymodel.h"
#undef private

#include "videocollectionwrapper.h"
#include "videocollectioncommon.h"
#include "testvideosortfilterproxymodel.h"
#include "videolistdatamodel.h"
#include "videocollectionclient.h"
#include "videocollectioncommon.h"
#include "videothumbnaildata.h"
#include "filterproxytester.h"

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    
    TestVideoSortFilterProxyModel tv;

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
        pass[2] = "c:\\data\\testvideosortfilterproxymodel.txt";
        res = QTest::qExec(&tv, 3, pass);
    }    
    
    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::init()
{
    qRegisterMetaType<TMPXItemId>("TMPXItemId");
    
	VideoListDataModel::mInitFails = false;
	VideoListDataModel::mRemoveRowsFails = false;
	VideoListDataModel::mGetMediaIdAtIndexFails = false;
	VideoCollectionClient::mFailMediaPlayback = false;
	VideoCollectionClient::mFailMediaDetails = false;
	VideoCollectionClient::mFailInit = false;
	VideoCollectionClient::mFailAddNewCollection = false;
	VideoCollectionClient::mSortOrderSetCount = 0;
	VideoCollectionClient::mAddNewCollectionName = QString();
	VideoCollectionClient::mAddNewCollectionThumb = QString();
	VideoCollectionClient::mAddNewCollectionIds = QList<TMPXItemId>();
	VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
	
    mTestObject = new FilterProxyTester(VideoCollectionCommon::EModelTypeGeneric);
    QVERIFY(mTestObject);
    
    mStubModel = new VideoListDataModel();
    QVERIFY(mStubModel);  
    
    mCollectionClient = new VideoCollectionClient();
    QVERIFY(mCollectionClient);
    
    mCollectionModel = new VideoSortFilterProxyModel(VideoCollectionCommon::EModelTypeAllVideos);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::cleanup()
{
	delete mCollectionClient;
	mCollectionClient = 0;
	
	delete mStubModel;
    mStubModel = 0;
 
    delete mTestObject;
    mTestObject = 0;
    
    delete mCollectionModel;
    mCollectionModel = 0;
}

// ---------------------------------------------------------------------------
// testInitialize
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testInitialize()
{
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
}

// ---------------------------------------------------------------------------
// testInitializeSignalConnectFail
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testInitializeSignalConnectFail()
{	
	QVERIFY(mTestObject->initialize(NULL) == -1);
}

// ---------------------------------------------------------------------------
// testSecondInitialize
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testSecondInitialize()
{
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
}

// ---------------------------------------------------------------------------
// testOpen
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testOpen()
{
    // no mCollectionClient
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelInvalid) == -1);
    
    mTestObject->initialize(mStubModel);
    
	// First open.
	QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0); 
	// Open again with same level.
	QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0); 
	// Fail open.
	VideoCollectionClient::mFailStartOpen = true;
	QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == -1);
}

// ---------------------------------------------------------------------------
// testDeleteItems
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testDeleteItems()
{
    VideoThumbnailData::mEnableBackgroundThumbnailFetchingCallCount = 0;
    VideoCollectionClient::mFailStartOpen = false;
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0);
    QString name1 = "cc";
    QString name2 = "bb";
    QString name3 = "aa";
    QString name4 = "11";
    QString name5 = "12";
   
    mStubModel->appendData(name1); // to source model index 0, proxy index after sort 4   
    mStubModel->appendData(name2); // to source model index 1, proxy index after sort 3
    mStubModel->appendData(name3); // to source model index 2, proxy index after sort 2
    mStubModel->appendData(name4); // to source model index 3, proxy index after sort 0
    mStubModel->appendData(name5); // to source model index 4, proxy index after sort 1
	
	// sort to make sure that list order is different compared to source model
	mTestObject->doSorting(VideoCollectionCommon::KeyTitle, Qt::AscendingOrder);
    // need to wait for awhile to make sure zero-counter gets 
    // processing time.
    QTest::qWait(500);
    QVERIFY(mTestObject->sortRole() == VideoCollectionCommon::KeyTitle);
    
	int count = mTestObject->rowCount();
    QModelIndexList list;
    QModelIndex index; 
    for(int i = 0; i < 5; ++i)
    {
        index = mTestObject->index(i,0);
        list.append(index);
    }
    QModelIndexList emptyList;
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
	QVERIFY(mTestObject->deleteItems(emptyList) == 0);
	QCOMPARE(VideoThumbnailData::mEnableBackgroundThumbnailFetchingCallCount, 2);
	QVERIFY(VideoThumbnailData::mBackgroundThumbnailFetchingEnabled == true);
	QVERIFY(mTestObject->deleteItems(list) == 0);
	QCOMPARE(VideoThumbnailData::mEnableBackgroundThumbnailFetchingCallCount, 4);
	QVERIFY(VideoThumbnailData::mBackgroundThumbnailFetchingEnabled == true);
	QVERIFY(mStubModel->mLastDeletedIndexRow == 0);
	QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 2);
}

// ---------------------------------------------------------------------------
// testDeleteItemsModelNull
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testDeleteItemsModelNull()
{
	mStubModel->appendData("Test");
	QVERIFY(mTestObject->initialize(NULL) == -1);

	QModelIndex index = mStubModel->index(0, 0);
	QModelIndexList list;
	list.append(index);	
	
	QVERIFY(mTestObject->deleteItems(list) == -1);
}

// ---------------------------------------------------------------------------
// testDeleteItemsRemoveRowsFail
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testDeleteItemsRemoveRowsFail()
{
	VideoListDataModel::mRemoveRowsFails = true;
	VideoCollectionClient::mFailStartOpen = false;
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0);
    
	mStubModel->appendData("Test");

	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
	QModelIndex index = mTestObject->index(0, 0);
	QModelIndexList list;
	list.append(index);
	
	QVERIFY(mTestObject->deleteItems(list) == -1);
}

// ---------------------------------------------------------------------------
// testOpenItemInvalidId
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testOpenItemInvalidId()
{
	VideoListDataModel::mGetMediaIdAtIndexFails = true;
	VideoCollectionClient::mFailStartOpen = false;
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0);
    
	mStubModel->appendData("Test");

	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	QModelIndex index = mTestObject->index(0, 0);
	TMPXItemId itemId = TMPXItemId::InvalidId();
		
	QVERIFY(mTestObject->openItem(itemId) == -1);
}

// ---------------------------------------------------------------------------
// testOpenItemCollectionNull
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testOpenItemCollectionNull()
{	
	mStubModel->appendData("Test");
	
	TMPXItemId itemId(1,0);

	QVERIFY(mTestObject->openItem(itemId) == -1);
}

// ---------------------------------------------------------------------------
// testOpenItemOpenVideoFails
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testOpenItemOpenVideoFails()
{
	VideoCollectionClient::mFailMediaPlayback = true;
	
	mStubModel->appendData("Test");
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	TMPXItemId itemId(1,0);
	
	QVERIFY(mTestObject->openItem(itemId) == -1);
	
	VideoCollectionClient::mFailMediaPlayback = false;
}

// ---------------------------------------------------------------------------
// testOpenItem
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testOpenItem()
{   
    mStubModel->appendData("Test");
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    TMPXItemId itemId(1,0);
    
    // open video
    QVERIFY(mTestObject->openItem(itemId) == 0);
    
    // open category or album
    itemId = TMPXItemId(1,2);
    QVERIFY(mTestObject->openItem(itemId) == 0);
}

// ---------------------------------------------------------------------------
// testBack
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testBack()
{	
    QVERIFY(mTestObject->back() == -1);
	
    QVERIFY(mTestObject->initialize(mStubModel) == 0);

	QVERIFY(mTestObject->back() == 0);
	
	mTestObject->open(VideoCollectionCommon::ELevelAlbum);
	QVERIFY(mTestObject->back() == 0);
	
	// back fails
	VideoCollectionClient::mBackReturnValue = -1;
	QVERIFY(mTestObject->back() == -1);
	
	VideoCollectionClient::mBackReturnValue = 0;
}

// ---------------------------------------------------------------------------
// testFetchItemDetails
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testFetchItemDetails()
{
    VideoCollectionClient::mFailStartOpen = false;
	mStubModel->appendData("Test");
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	mTestObject->open(VideoCollectionCommon::ELevelVideos);
	QSignalSpy fetchSpy(mTestObject, SIGNAL(shortDetailsReady(TMPXItemId)));
	
	QModelIndex index = mTestObject->index(0, 0);	

	QList<QVariant> arguments;
	QVERIFY(mTestObject->fetchItemDetails(index) == 0);

	QVERIFY(fetchSpy.count() == 1);
	arguments = fetchSpy.takeFirst();
	QVERIFY(arguments.at(0).toInt() == 0);
	arguments.clear();
	fetchSpy.clear();
	
	// no collection client
	VideoCollectionClient *tmp = mTestObject->mCollectionClient;
	mTestObject->mCollectionClient = 0;
	QVERIFY(mTestObject->fetchItemDetails(index) == -1);
	mTestObject->mCollectionClient = tmp;
}

// ---------------------------------------------------------------------------
// testFetchItemDetails
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testFetchItemDetailsGetMediaIdFail()
{
	VideoListDataModel::mGetMediaIdAtIndexFails = true;
	QSignalSpy fetchSpy(mTestObject, SIGNAL(shortDetailsReady(TMPXItemId)));	
	mStubModel->appendData("Test");
	QModelIndex index = mTestObject->index(0, 0);

	// no model
	QVERIFY(mTestObject->fetchItemDetails(index) == -1);
	
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
	QVERIFY(mTestObject->fetchItemDetails(index) == -1);
	
	QVERIFY(fetchSpy.count() == 0);
}

// ---------------------------------------------------------------------------
// testFetchItemDetails
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testFetchItemDetailsGetVideoDetailsFails()
{
	VideoCollectionClient::mFailMediaDetails = true;
	
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	mTestObject->open(VideoCollectionCommon::ELevelVideos);
	mStubModel->appendData("Test");
	
	QSignalSpy fetchSpy(mTestObject, SIGNAL(shortDetailsReady(TMPXItemId)));
	QModelIndex index = mTestObject->index(0, 0);

	QVERIFY(mTestObject->fetchItemDetails(index) == -1);
	
	QVERIFY(fetchSpy.count() == 0);
}

// ---------------------------------------------------------------------------
// testLessThanProxyModelNotInitialized
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanNoModel()
{
    // "normal" comparisation
    QString name1 = "cc";
    QString name2 = "bb";
    QString name3 = "aa";
    QString name4 = "11";
    QString name5 = "11";
    
    mStubModel->appendData(name1); // to index 0, position 4
    mStubModel->appendData(name2); // to index 1, position 3
    mStubModel->appendData(name3); // to index 2, position 2
    mStubModel->appendData(name4); // to index 3, position 0 or 1
    mStubModel->appendData(name5); // to index 4, position 0 or 1
   
    mTestObject->setSortRole(VideoCollectionCommon::KeyTitle);

    QModelIndex left = mStubModel->index(0,0);
    QModelIndex right = mStubModel->index(1,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
}
// ---------------------------------------------------------------------------
// testLessThanName
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanName()
{
    VideoCollectionClient::mFailStartOpen = false;
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0);
	
    // "normal" comparisation
    QString name1 = "cc";
    QString name2 = "bb";
    QString name3 = "aa";
    QString name4 = "11";
    QString name5 = "11";
    
    mStubModel->appendData(name1); // to index 0, position 4
    mStubModel->appendData(name2); // to index 1, position 3
    mStubModel->appendData(name3); // to index 2, position 2
    mStubModel->appendData(name4); // to index 3, position 0 or 1
    mStubModel->appendData(name5); // to index 4, position 0 or 1
   
    mTestObject->setSortRole(VideoCollectionCommon::KeyTitle);

    QModelIndex left = mStubModel->index(0,0);
    QModelIndex right = mStubModel->index(1,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(1,0);
    right = mStubModel->index(2,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(2,0);
    right = mStubModel->index(3,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(3,0);
    right = mStubModel->index(4,0);
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(4,0);
    right = mStubModel->index(0,0);
    
    QVERIFY(mTestObject->callLessThan(left, right));

    // reset model
    mStubModel->removeAll();;
        
    // case sensitive check
    name1 = "cc";
    name2 = "CC";
    name3 = "cC";
    name4 = "aa";
    name5 = "Aa";
    QString name6 = "2Aa";
    QString name7 = "1Aa";
    QString name8 = "1cc";
    mStubModel->removeAll();
    mStubModel->appendData(name1); // to index 0, position can be 5, 6 or 7
    mStubModel->appendData(name2); // to index 1, position can be 5, 6 or 7
    mStubModel->appendData(name3); // to index 2, position can be 5, 6 or 7
    mStubModel->appendData(name4); // to index 3, position can be 3 or 4
    mStubModel->appendData(name5); // to index 4, position can be 3 or 4
    mStubModel->appendData(name6); // to index 5, position is 2 
    mStubModel->appendData(name7); // to index 6, position is 0
    mStubModel->appendData(name8); // to index 7, position is 1
    
    mTestObject->setSortRole(VideoCollectionCommon::KeyTitle);

    left = mStubModel->index(0,0);  // "cc"
    right = mStubModel->index(1,0); // "CC"
        
    QVERIFY(!mTestObject->callLessThan(left, right));
        
    left = mStubModel->index(1,0);  // "CC"
    right = mStubModel->index(2,0); // "cC"
        
    QVERIFY(!mTestObject->callLessThan(left, right));
        
    left = mStubModel->index(2,0);  // "cC"
    right = mStubModel->index(3,0); // "aa"
        
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(3,0);  // "aa"
    right = mStubModel->index(2,0); // "cC"
            
    QVERIFY(mTestObject->callLessThan(left, right));
       
    left = mStubModel->index(3,0);  // "aa"
    right = mStubModel->index(4,0); // "Aa"
    QVERIFY(!mTestObject->callLessThan(left, right));
        
    left = mStubModel->index(4,0);  // "Aa"
    right = mStubModel->index(5,0); // "2Aa"
      
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(5,0);  // "2Aa"
    right = mStubModel->index(6,0); // "1Aa"
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(6,0);  // "1Aa"
    right = mStubModel->index(7,0); // "1cc"
    QVERIFY(mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(5,0);  // "2Aa"
    right = mStubModel->index(7,0); // "1cc"
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(7,0);  // "1cc"
    right = mStubModel->index(0,0); // "cc"
    QVERIFY(mTestObject->callLessThan(left, right));
}
  
// ---------------------------------------------------------------------------
// testLessThanSize
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanSize()
{
    VideoCollectionClient::mFailStartOpen = false;
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0);

    quint32 size1 = 300;
    quint32 size2 = 200;
    quint32 size3 = 100;
    quint32 size4 = 99;
    quint32 size5 = 99;
    
    mStubModel->appendData(size1); // to index 0
    mStubModel->appendData(size2); // to index 1
    mStubModel->appendData(size3); // to index 2    
    mStubModel->appendData(size4); // to index 3
    mStubModel->appendData(size5); // to index 4
 
    mTestObject->setSortRole(VideoCollectionCommon::KeySizeValue);
    
    QModelIndex left = mStubModel->index(0,0);
    QModelIndex right = mStubModel->index(1,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(1,0);
    right = mStubModel->index(2,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(2,0);
    right = mStubModel->index(3,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(3,0);
    right = mStubModel->index(4,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(4,0);
    right = mStubModel->index(0,0);
    
    QVERIFY(mTestObject->callLessThan(left, right));
}
 
// ---------------------------------------------------------------------------
// testLessThanDate
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanDateTime()
{
    VideoCollectionClient::mFailStartOpen = false;
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0);
	
	QDateTime date1(QDate(2010, 11, 5), QTime(0,0));
    QDateTime date2(QDate(2009, 9, 5), QTime(0,0));
    QDateTime date3(QDate(2009, 9, 4), QTime(0,0));
    QDateTime date4(QDate(2008, 9, 6), QTime(0,0));
    QDateTime date5(QDate(2008, 9, 6), QTime(0,0));
    
    mStubModel->appendData(date1); // to index 0
    mStubModel->appendData(date2); // to index 1
    mStubModel->appendData(date3); // to index 2    
    mStubModel->appendData(date4); // to index 3
    mStubModel->appendData(date5); // to index 4
    
    mTestObject->setSortRole(VideoCollectionCommon::KeyDateTime);
    
    QModelIndex left = mStubModel->index(0,0);
    QModelIndex right = mStubModel->index(1,0);
    
    QVERIFY(mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(1,0);
    right = mStubModel->index(2,0);
    
    QVERIFY(mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(2,0);
    right = mStubModel->index(3,0);
    
    QVERIFY(mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(3,0);
    right = mStubModel->index(4,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    left = mStubModel->index(4,0);
    right = mStubModel->index(0,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
}

// ---------------------------------------------------------------------------
// testLessThanDefaults
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanDefaults()
{
    VideoCollectionClient::mFailStartOpen = false;
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelCategory) == 0);

    // Default categories (KVcxMvcMediaTypeCategory) 
    // are always first in the following order: 
    // Recently played (missing currently 
    // Captured   (KVcxMvcCategoryIdCaptured)
    // Downloaded (KVcxMvcCategoryIdDownloads)
    // Podcasts (missing currently)     
    mStubModel->appendData(TMPXItemId(20, KVcxMvcMediaTypeAlbum)); // to source index 0
    mStubModel->appendData(TMPXItemId(KVcxMvcCategoryIdDownloads, KVcxMvcMediaTypeCategory)); // to source index 1
    mStubModel->appendData(TMPXItemId(KVcxMvcCategoryIdCaptured, KVcxMvcMediaTypeCategory)); // to source index 2    
    mStubModel->appendData(TMPXItemId(100, KVcxMvcMediaTypeCategory)); // to source index 3    
    mStubModel->appendData(TMPXItemId(101, KVcxMvcMediaTypeCategory)); // to source index 4    
    
    // left & right are KVcxMvcMediaTypeCategory
    // -> left == KVcxMvcCategoryIdCaptured
    QModelIndex left = mStubModel->index(2,0);
    QModelIndex right = mStubModel->index(1,0);
    // call doSorting to setup sorting order
    mTestObject->mWantedSortOrder = Qt::AscendingOrder;
    QVERIFY(mTestObject->callLessThan(left, right));
    // call doSorting to setup sorting order
    mTestObject->mWantedSortOrder = Qt::DescendingOrder;
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    // -> left == KVcxMvcCategoryIdDownloads
    left = mStubModel->index(1,0);
    //     -> right == KVcxMvcCategoryIdCaptured
    right = mStubModel->index(2,0);
    mTestObject->mWantedSortOrder = Qt::AscendingOrder;
    QVERIFY(!mTestObject->callLessThan(left, right));  
    mTestObject->mWantedSortOrder = Qt::DescendingOrder;
    QVERIFY(mTestObject->callLessThan(left, right));
    
    //     -> right != KVcxMvcCategoryIdCaptured
    right = mStubModel->index(3,0);
    mTestObject->mWantedSortOrder = Qt::AscendingOrder;
    QVERIFY(mTestObject->callLessThan(left, right));  
    mTestObject->mWantedSortOrder = Qt::DescendingOrder;
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    // both are KVcxMvcMediaTypeCategory but netiher KVcxMvcCategoryIdCaptured
    // nor KVcxMvcCategoryIdDownloads
    left = mStubModel->index(3,0);
    right = mStubModel->index(4,0);
    mTestObject->mWantedSortOrder = Qt::AscendingOrder;
    QVERIFY(!mTestObject->callLessThan(left, right));  
    mTestObject->mWantedSortOrder = Qt::DescendingOrder;
    QVERIFY(mTestObject->callLessThan(left, right));
        
    // Left is KVcxMvcMediaTypeCategory and right is not
    left = mStubModel->index(1,0);
    right = mStubModel->index(0,0);
    mTestObject->mWantedSortOrder = Qt::AscendingOrder;
    QVERIFY(mTestObject->callLessThan(left, right));  
    mTestObject->mWantedSortOrder = Qt::DescendingOrder;
    QVERIFY(!mTestObject->callLessThan(left, right));

    // right is KVcxMvcMediaTypeCategory and left is not
    left = mStubModel->index(0,0);
    right = mStubModel->index(1,0);
    mTestObject->mWantedSortOrder = Qt::AscendingOrder;
    QVERIFY(!mTestObject->callLessThan(left, right));  
    mTestObject->mWantedSortOrder = Qt::DescendingOrder;
    QVERIFY(mTestObject->callLessThan(left, right));
}

// ---------------------------------------------------------------------------
// testLessThanInvalid
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanInvalid()
{
    VideoCollectionClient::mFailStartOpen = false;
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0);
	
    // invalid role
	mTestObject->setSortRole(10000);
    
    QModelIndex left = mStubModel->index(0,0);
    QModelIndex right = mStubModel->index(0,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    // invalid role but valid data.
    
	QString name1 = "cc";
	QString name2 = "bb";
	QString name3 = "aa";
	
	mStubModel->appendData(name1); // to index 0, position 4
	mStubModel->appendData(name2); // to index 1, position 3
	mStubModel->appendData(name3); // to index 2, position 2
   
	mTestObject->setSortRole(INVALID_ROLE_FOR_SORTING);
	
    left = mStubModel->index(0,0);
    right = mStubModel->index(1,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
           
    // invalid left index
    mTestObject->setSortRole(VideoCollectionCommon::KeyTitle);

    left = QModelIndex();
    right = mStubModel->index(0,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));

    // invalid right index
    mTestObject->setSortRole(VideoCollectionCommon::KeyTitle);
    User::Heap().__DbgMarkStart(); 
    left =  mStubModel->index(0,0); 
    right = QModelIndex();
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    // both invalid
    mTestObject->setSortRole(VideoCollectionCommon::KeyTitle);
    User::Heap().__DbgMarkStart(); 
    left =  QModelIndex();
    right = QModelIndex();
    
    QVERIFY(!mTestObject->callLessThan(left, right));

    // both same index, use size
    quint32 size1 = 99;
    
    mStubModel->appendData(size1); // to index 0
    
    mTestObject->setSortRole(VideoCollectionCommon::KeySizeValue);
    mTestObject->setSortRole(VideoCollectionCommon::KeyTitle);
    
    left = mStubModel->index(0,0);
    right = mStubModel->index(0,0);
    QVERIFY(!mTestObject->callLessThan(left, right));
}

// ---------------------------------------------------------------------------
// testDoSorting
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testDoSorting()
{
    VideoCollectionClient::mFailStartOpen = false;
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0);
    
    // these signals are emitted during sorting procedure from the fw
    QSignalSpy spyAboutToChange(mTestObject, SIGNAL(layoutAboutToBeChanged()));
    QSignalSpy spyChanged(mTestObject, SIGNAL(layoutChanged()));
    
    QString name1 = "cc";
    QString name2 = "bb";
    QString name3 = "aa";
    QString name4 = "11";
    QString name5 = "11";
    QDateTime date1(QDate(2010, 11, 5), QTime(0,0));
    QDateTime date2(QDate(2009, 9, 5), QTime(0,0));
    QDateTime date3(QDate(2009, 9, 4), QTime(0,0));
    QDateTime date4(QDate(2008, 9, 6), QTime(0,0));
    QDateTime date5(QDate(2008, 9, 6), QTime(0,0));
    quint32 size1 = 300;
    quint32 size2 = 200;
    quint32 size3 = 100;
    quint32 size4 = 99;
    quint32 size5 = 99;
  
    mStubModel->appendData(name1); // to index 0, position 4
    mStubModel->appendData(name2); // to index 1, position 3
    mStubModel->appendData(name3); // to index 2, position 2
    mStubModel->appendData(name4); // to index 3, position 0 or 1
    mStubModel->appendData(name5); // to index 4, position 0 or 1
  
    mStubModel->appendData(date1); // to index 0
    mStubModel->appendData(date2); // to index 1
    mStubModel->appendData(date3); // to index 2    
    mStubModel->appendData(date4); // to index 3
    mStubModel->appendData(date5); // to index 4
  
    mStubModel->appendData(size1); // to index 0
    mStubModel->appendData(size2); // to index 1
    mStubModel->appendData(size3); // to index 2    
    mStubModel->appendData(size4); // to index 3
    mStubModel->appendData(size5); // to index 4
    
    int sortingRole;
    Qt::SortOrder sortingOrder;

    // first sort call, includes timer creation and setup
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle, Qt::AscendingOrder);
    // need to wait for awhile to make sure zero-counter gets 
    // processing time.
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1); 
    QCOMPARE(spyChanged.count(), 1); 
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(mTestObject->sortOrder(), Qt::AscendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    mTestObject->getSorting(sortingRole, sortingOrder);
    QCOMPARE(sortingRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(sortingOrder, Qt::AscendingOrder);
    
    // reset spys
    spyAboutToChange.clear();
    spyChanged.clear();

    // second sort call, should use same timer appropriately
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle, Qt::DescendingOrder);
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(mTestObject->sortOrder(), Qt::DescendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    mTestObject->getSorting(sortingRole, sortingOrder);
    QCOMPARE(sortingRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(sortingOrder, Qt::DescendingOrder);
    
    // reset spys
    spyAboutToChange.clear();
    spyChanged.clear();    
    
    // double call without first letting timer to timeout
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle, Qt::DescendingOrder);
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle, Qt::AscendingOrder);
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(mTestObject->sortOrder(), Qt::AscendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    mTestObject->getSorting(sortingRole, sortingOrder);
    QCOMPARE(sortingRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(sortingOrder, Qt::AscendingOrder);
    
    spyAboutToChange.clear();
    spyChanged.clear();
    
    // syncronous call checks
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle, Qt::DescendingOrder, false);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(mTestObject->sortOrder(), Qt::DescendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    mTestObject->getSorting(sortingRole, sortingOrder);
    QCOMPARE(sortingRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(sortingOrder, Qt::DescendingOrder);
    
    spyAboutToChange.clear();
    spyChanged.clear();
    
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle, Qt::AscendingOrder, false);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(mTestObject->sortOrder(), Qt::AscendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    mTestObject->getSorting(sortingRole, sortingOrder);
    QCOMPARE(sortingRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(sortingOrder, Qt::AscendingOrder);
    
    spyAboutToChange.clear();
    spyChanged.clear();
    
    // check that layout signals are not send if the sorting values don't change.
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle, Qt::AscendingOrder);
    QCOMPARE(spyAboutToChange.count(), 0);
    QCOMPARE(spyChanged.count(), 0);
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(mTestObject->sortOrder(), Qt::AscendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 0);
    mTestObject->getSorting(sortingRole, sortingOrder);
    QCOMPARE(sortingRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(sortingOrder, Qt::AscendingOrder);
    
    spyAboutToChange.clear();
    spyChanged.clear();
    
    // date role check    
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(VideoCollectionCommon::KeyDateTime, Qt::AscendingOrder);
    QTest::qWait(500);
    int count = spyAboutToChange.count();
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyDateTime);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    mTestObject->getSorting(sortingRole, sortingOrder);
    QCOMPARE(sortingRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(sortingOrder, Qt::AscendingOrder);
    
    spyAboutToChange.clear();
    spyChanged.clear();
    
    // size role check
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(VideoCollectionCommon::KeySizeValue, Qt::AscendingOrder);
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(),  (int)VideoCollectionCommon::KeySizeValue);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    mTestObject->getSorting(sortingRole, sortingOrder);
    QCOMPARE(sortingRole, (int)VideoCollectionCommon::KeySizeValue);
    QCOMPARE(sortingOrder, Qt::AscendingOrder);

    spyAboutToChange.clear();
    spyChanged.clear();

    // number of items role check
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(VideoCollectionCommon::KeyNumberOfItems, Qt::AscendingOrder);
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(),  (int)VideoCollectionCommon::KeyNumberOfItems);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    mTestObject->getSorting(sortingRole, sortingOrder);
    QCOMPARE(sortingRole, (int)VideoCollectionCommon::KeyNumberOfItems);
    QCOMPARE(sortingOrder, Qt::AscendingOrder);

    spyAboutToChange.clear();
    spyChanged.clear();    
    
    // invalid role call, sorting should be set to date
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->mType = VideoCollectionCommon::EModelTypeAllVideos;
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle - 100, Qt::AscendingOrder);
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyDateTime);
    spyAboutToChange.clear();
    spyChanged.clear();
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    
    // invalid role call, model type is categories, sorting should be set to VideoCollectionCommon::KeyTitle
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->mType = VideoCollectionCommon::EModelTypeCollections;
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle - 100, Qt::AscendingOrder);
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyTitle);
    spyAboutToChange.clear();
    spyChanged.clear();
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);   
    
    // sync sorting call for non -changing sort order (for coverity)
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->mType = VideoCollectionCommon::EModelTypeAllVideos;
    mTestObject->mIdleSortTimer = 0;
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle - 100, Qt::AscendingOrder, false);
    spyAboutToChange.clear();
    spyChanged.clear();
    mTestObject->doSorting(VideoCollectionCommon::KeyTitle - 100, Qt::AscendingOrder, false);
    QCOMPARE(spyAboutToChange.count(), 0);
    QCOMPARE(spyChanged.count(), 0);
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyDateTime);
    spyAboutToChange.clear();
    spyChanged.clear();
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 2);
}

// ---------------------------------------------------------------------------
// testFilterAcceptsRow
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testFilterAcceptsRowVideos()
{	   
    // source_parent can be anything, test model doesn't use it.
	QModelIndex source_parent = QModelIndex(); 
		
	// Not initialized: no rouce model
	QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == false);	
	
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	mTestObject->open(VideoCollectionCommon::ELevelVideos);
	mTestObject->mType = VideoCollectionCommon::EModelTypeAllVideos;
	
	// Test invalid row: below 0
	QVERIFY(mTestObject->callFilterAcceptsRow(-1, source_parent) == false);
	
	// invalid row: larger than count
	mStubModel->appendData("test");
	QVERIFY(mTestObject->callFilterAcceptsRow(2, source_parent) == false);
	
	// correct row
	QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == true);
	
	// correct row, but id not corresponds video
	mStubModel->removeAll();
	mStubModel->appendData(TMPXItemId(1, KVcxMvcMediaTypeCategory));
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == false);
	
}

// ---------------------------------------------------------------------------
// testFilterAcceptsNoVideoTypes
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testFilterAcceptsNoVideoTypes()
{
    // source_parent can be anything, test model doesn't use it.
    QModelIndex source_parent = QModelIndex(); 
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    
    // only one item at ondex 0
    mStubModel->appendData(TMPXItemId(1, KVcxMvcMediaTypeVideo));
    
    // model type == VideoCollectionWrapper::ECollections    
    mTestObject->mType = VideoCollectionCommon::EModelTypeCollections;
    // mLevel == VideoCollectionCommon::ELevelCategory, id.iId1 == KVcxMvcMediaTypeVideo
    mTestObject->mLevel = VideoCollectionCommon::ELevelCategory;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == false);
    // mLevel != VideoCollectionCommon::ELevelCategory
    mTestObject->mLevel = VideoCollectionCommon::ELevelAlbum;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == false);
    
    // model type == VideoCollectionWrapper::ECollectionContent   
    mTestObject->mType = VideoCollectionCommon::EModelTypeCollectionContent;
    // item belongs to album
    VideoListDataModel::mBelongsToAlbum = true;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == true);
    // item does not belong to album
    VideoListDataModel::mBelongsToAlbum = false;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == false);
    
    // model type  == VideoCollectionWrapper::EGeneric
    mTestObject->mType = VideoCollectionCommon::EModelTypeGeneric;
    mTestObject->mGenericFilterValue = true;
    
    // mLevel != VideoCollectionCommon::ELevelVideos && id.iId2 == KVcxMvcMediaTypeVideo
    mTestObject->mLevel = VideoCollectionCommon::ELevelAlbum;
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == false);
    
    // mLevel == VideoCollectionCommon::ELevelVideos && id.iId2 != KVcxMvcMediaTypeVideo
    mTestObject->mLevel = VideoCollectionCommon::ELevelVideos;
    mStubModel->removeAll();
    mStubModel->appendData(TMPXItemId(1, KVcxMvcMediaTypeCategory));
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == false);
       
    // mLevel == VideoCollectionCommon::ELevelVideos && id.iId2 == KVcxMvcMediaTypeVideo
    mStubModel->removeAll();
    mStubModel->appendData(TMPXItemId(1, KVcxMvcMediaTypeVideo));
    // generic filter id == TMPXItemId::InvalidId()
    mTestObject->mGenericFilterId = TMPXItemId::InvalidId();
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == true);
    
    // generic filter id == (KVcxMvcCategoryIdAll, KVcxMvcMediaTypeCategory)
    mTestObject->mGenericFilterId = TMPXItemId(KVcxMvcCategoryIdAll, KVcxMvcMediaTypeCategory);
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == true);
    
    // generic filter id == (100, KVcxMvcMediaTypeCategory)
    mTestObject->mGenericFilterId = TMPXItemId(100, KVcxMvcMediaTypeCategory);
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == false);
    
    // generic filter id == (KVcxMvcCategoryIdAll, KVcxMvcMediaTypeVideo)
    mTestObject->mGenericFilterId = TMPXItemId(KVcxMvcCategoryIdAll, KVcxMvcMediaTypeVideo);
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == false);
    
    // generic filter id == (100, KVcxMvcMediaTypeCategory)
    VideoListDataModel::mBelongsToAlbum = true;
    mTestObject->mGenericFilterId = TMPXItemId(100, KVcxMvcMediaTypeCategory);
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == true);
    
    // generic filter id == (KVcxMvcCategoryIdAll, KVcxMvcMediaTypeVideo)
    VideoListDataModel::mBelongsToAlbum = true;
    mTestObject->mGenericFilterId = TMPXItemId(KVcxMvcCategoryIdAll, KVcxMvcMediaTypeVideo);
    QVERIFY(mTestObject->callFilterAcceptsRow(0, source_parent) == true);
}

// ---------------------------------------------------------------------------
// testGetMediaFilePathForId
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testGetMediaFilePathForId()
{
    // Not initialized.
    VideoListDataModel::mMediaFilePathReturnValue = "testfile";
    QVERIFY(mTestObject->getMediaFilePathForId(TMPXItemId(0,0)) == "");

    // Ok case.
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    VideoListDataModel::mMediaFilePathReturnValue = "testfile";
    QVERIFY(mTestObject->getMediaFilePathForId(TMPXItemId(0,0)) == "testfile");
}

// ---------------------------------------------------------------------------
// testAddNewCollectionNoCollectionClient
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testAddNewCollectionNoCollectionClient()
{
    QString name("testname");
    VideoCollectionClient::mNewAlbumId = TMPXItemId(1,1);
    QVERIFY(mTestObject->addNewAlbum(name) == TMPXItemId::InvalidId());
    QVERIFY(VideoCollectionClient::mAddNewCollectionName != name);
}

// ---------------------------------------------------------------------------
// testAddNewCollectionSucceed
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testAddNewCollectionSucceed()
{
    QString name("testname");

    mTestObject->initialize(mStubModel);
    VideoCollectionClient::mNewAlbumId = TMPXItemId(1,1);
    QVERIFY(mTestObject->addNewAlbum(name) == TMPXItemId(1,1));
    QCOMPARE(VideoCollectionClient::mAddNewCollectionName, name);
}

// ---------------------------------------------------------------------------
// testResolveAlbumName
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testResolveAlbumName()
{
    mCollectionModel->mType = VideoCollectionCommon::EModelTypeCollections;
    
    QString name("test");
    QString resolved("");
    // no model, same name can be used
    resolved = mTestObject->resolveAlbumName(name);
    QVERIFY(resolved.length());
    QVERIFY(resolved == name);
    
    mTestObject->initialize(mStubModel);
    
    // no collections proxy model, same name    
    resolved = mTestObject->resolveAlbumName(name);
    QVERIFY(resolved.length());
    QVERIFY(resolved == name);  
   
    mCollectionModel->initialize(mStubModel);
    mCollectionModel->open(VideoCollectionCommon::ELevelCategory);
    VideoCollectionWrapper::instance().mProxyModel = mCollectionModel;
    
    // collections proxy model exists, no source model
    VideoListDataModel *temp = mTestObject->mModel;
    mTestObject->mModel = 0;
    
    resolved = mTestObject->resolveAlbumName(name);
    QVERIFY(resolved.length());
    QVERIFY(resolved == name); 
    mTestObject->mModel = temp;
    
    mStubModel->appendData(TMPXItemId(1,2)); 
    mStubModel->appendData(name);
    
    int count = mCollectionModel->rowCount();
    
    // invalid data from model, same name can be used
    VideoListDataModel::mReturnInvalid = true;
    resolved = mTestObject->resolveAlbumName(name);
    QVERIFY(resolved.length());
    QVERIFY(resolved == name);  
    VideoListDataModel::mReturnInvalid = false;
            
    // same name, name to be different
    resolved = mTestObject->resolveAlbumName(name);
    QVERIFY(resolved.length());
    QVERIFY(resolved != name);
    QVERIFY(resolved.contains("1"));
    
    mStubModel->appendData(TMPXItemId(2,2)); 
    mStubModel->appendData(resolved);
    
    resolved = mTestObject->resolveAlbumName(name);
    QVERIFY(resolved.length());
    QVERIFY(resolved != name);
    QVERIFY(resolved.contains("2"));

    // different name, no changes
    name = "Another";
    resolved = mTestObject->resolveAlbumName(name);
    QVERIFY(resolved == name);
    
    mCollectionModel->mType = VideoCollectionCommon::EModelTypeAllVideos;
}

// ---------------------------------------------------------------------------
// testResolveAlbumName
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testAddItemsInAlbum()
{
    TMPXItemId albumId(1,2);
    QList<TMPXItemId> items;
    items.append(TMPXItemId(1,0));
    // no collection client
    QVERIFY(mTestObject->addItemsInAlbum(albumId, items) == -1);
    
    mTestObject->initialize(mStubModel);
       
    QVERIFY(mTestObject->addItemsInAlbum(albumId, items) == 0);
}

// ---------------------------------------------------------------------------
// testRemoveItemsFromAlbum
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testRemoveItemsFromAlbum()
{
    VideoListDataModel::mRemoveFrAlbumReturn = 1;
    TMPXItemId albumId(1,2);
    QList<TMPXItemId> items;
    items.append(TMPXItemId(1,0));
    VideoThumbnailData::mEnableBackgroundThumbnailFetchingCallCount = 0;
    // no model
    
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, items) == -1);
    QCOMPARE(VideoThumbnailData::mEnableBackgroundThumbnailFetchingCallCount, 2);
    QVERIFY(VideoThumbnailData::mBackgroundThumbnailFetchingEnabled == true);
    mTestObject->initialize(mStubModel);
    
    // model returns < 0
    VideoListDataModel::mRemoveFrAlbumReturn = -1;
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, items) == -1);
    QCOMPARE(VideoThumbnailData::mEnableBackgroundThumbnailFetchingCallCount, 4);
    QVERIFY(VideoThumbnailData::mBackgroundThumbnailFetchingEnabled == true);
    
    VideoListDataModel::mRemoveFrAlbumReturn = 11;
    // "succeed"
    QVERIFY(mTestObject->removeItemsFromAlbum(albumId, items) == 0);
    QCOMPARE(VideoThumbnailData::mEnableBackgroundThumbnailFetchingCallCount, 6);
    QVERIFY(VideoThumbnailData::mBackgroundThumbnailFetchingEnabled == true);
}

// ---------------------------------------------------------------------------
// testGetOpenItem
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testGetOpenItem()
{
    // no model nor collection client
    QVERIFY(mTestObject->getOpenItem() == TMPXItemId::InvalidId());
    
    mTestObject->initialize(mStubModel);
    
    // model exist, no collection client
    VideoCollectionClient *tmpClient = mTestObject->mCollectionClient;
    mTestObject->mCollectionClient = 0;
    QVERIFY(mTestObject->getOpenItem() == TMPXItemId::InvalidId());
    mTestObject->mCollectionClient = tmpClient;
    
    // no model, collection client exists
    VideoListDataModel *tmpModel = mTestObject->mModel;
    mTestObject->mModel = 0;
    QVERIFY(mTestObject->getOpenItem() == TMPXItemId::InvalidId());
    mTestObject->mModel = tmpModel;
    
    // type neither EAllVideos or ECollectionContent
    QVERIFY(mTestObject->getOpenItem() == TMPXItemId::InvalidId());
    
    TMPXItemId id;
    // type EAllVideos
    delete mTestObject;    
    mTestObject = new FilterProxyTester(VideoCollectionCommon::EModelTypeAllVideos);
    mTestObject->initialize(mStubModel);
    id = mTestObject->getOpenItem();
    QVERIFY(id != TMPXItemId::InvalidId());
    QVERIFY(id.iId1 == KVcxMvcCategoryIdAll);
    QVERIFY(id.iId2 == KVcxMvcMediaTypeCategory);

    // type ECollectionContent
    delete mTestObject;    
    mTestObject = new FilterProxyTester(VideoCollectionCommon::EModelTypeCollectionContent);
    mTestObject->initialize(mStubModel);
    id.iId1 = 1;
    id.iId2 = KVcxMvcMediaTypeAlbum;
    mTestObject->openItem(id);
    id = TMPXItemId::InvalidId();
    id = mTestObject->getOpenItem();
    QVERIFY(id != TMPXItemId::InvalidId());
    QVERIFY(id.iId1 == 1);
    QVERIFY(id.iId2 == KVcxMvcMediaTypeAlbum);
}

// ---------------------------------------------------------------------------
// testRemoveAlbums
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testRemoveAlbums()
{
    QModelIndexList indexList;
    // no collection 
    QVERIFY(mTestObject->removeAlbums(indexList) == -1);
    
    mTestObject->initialize(mStubModel);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelVideos) == 0);
    
    // empty list provided
    QVERIFY(mTestObject->removeAlbums(indexList) == -1);
    
    mStubModel->appendData(TMPXItemId(1,0));
    mStubModel->appendData(TMPXItemId(2,0));
    mStubModel->appendData(TMPXItemId(3,0));
    
    indexList.append(mTestObject->index(0,0));
    indexList.append(mTestObject->index(1,0));
    indexList.append(mTestObject->index(2,0));
    
    // no albums in model
    QVERIFY(mTestObject->removeAlbums(indexList) == -1);
    
    indexList.clear();
    mStubModel->removeAll();
    delete mTestObject;
    mTestObject = new FilterProxyTester(VideoCollectionCommon::EModelTypeCollections);
    QVERIFY(mTestObject);
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    QVERIFY(mTestObject->open(VideoCollectionCommon::ELevelCategory) == 0);
    mStubModel->appendData(TMPXItemId(1,2));
    mStubModel->appendData(TMPXItemId(2,2));
    mStubModel->appendData(TMPXItemId(3,2));
    indexList.append(mTestObject->index(0,0));
    indexList.append(mTestObject->index(1,0));
    indexList.append(mTestObject->index(2,0));

    // succeed
    QVERIFY(mTestObject->removeAlbums(indexList) == 0);
}

// ---------------------------------------------------------------------------
// testAlbumChangedSlot
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testAlbumChangedSlot()
{
    connect(this, SIGNAL(testSignal()), mTestObject, SLOT(albumChangedSlot()));
    // cannot test anything here, just for the coverage
    // level incorrect
    mTestObject->initialize(mStubModel);
    emit testSignal();
    delete mTestObject;
    mTestObject = 0;
    mTestObject = new FilterProxyTester(VideoCollectionCommon::EModelTypeCollectionContent);
    connect(this, SIGNAL(testSignal()), mTestObject, SLOT(albumChangedSlot()));
    QVERIFY(mTestObject);    
    mTestObject->initialize(mStubModel);
    emit testSignal();      
    disconnect(this, SIGNAL(testSignal()), mTestObject, SLOT(albumChangedSlot()));
}

// ---------------------------------------------------------------------------
// testIndexOfId
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testIndexOfId()
{
    QModelIndex index;
    // no model
    TMPXItemId id(1,0);
    index = mTestObject->indexOfId(id);
    QVERIFY(!index.isValid());
    
    mTestObject->initialize(mStubModel);
    mTestObject->open(VideoCollectionCommon::ELevelVideos);
    
    // fetching invalid
    id = TMPXItemId::InvalidId();
    index = mTestObject->indexOfId(id);
    QVERIFY(!index.isValid());
    
    mStubModel->appendData(TMPXItemId(1,0));
    mStubModel->appendData(TMPXItemId(2,0));
    mStubModel->appendData(TMPXItemId(3,0));
    
    id = TMPXItemId(2,0);
    index = mTestObject->indexOfId(id);
    QVERIFY(index.isValid());
    QVERIFY(index.row() == 1);
}
 
// ---------------------------------------------------------------------------
// testSetGenericIdFilter
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testSetGenericIdFilter()
{
    // type VideoCollectionWrapper::EGeneric    
    mTestObject->initialize(mStubModel);   
    mTestObject->open(VideoCollectionCommon::ELevelVideos);
    mTestObject->setGenericIdFilter(TMPXItemId(0,1), true);
    
    mStubModel->appendData(TMPXItemId(1,0));
    mStubModel->appendData(TMPXItemId(2,0));
    mStubModel->appendData(TMPXItemId(3,0));
    mTestObject->invalidate();
    QModelIndex index;
    index = mTestObject->indexOfId(TMPXItemId(1,0));
    QVERIFY(index.isValid());
    QVERIFY(index.row() == 0);        
    
    // other type
    delete mTestObject;
    mTestObject = new FilterProxyTester(VideoCollectionCommon::EModelTypeAllVideos);
    mTestObject->initialize(mStubModel);   
    mTestObject->open(VideoCollectionCommon::ELevelVideos);
    
    mTestObject->setGenericIdFilter(TMPXItemId(1,1), true);
    mTestObject->invalidate();
    index = mTestObject->indexOfId(TMPXItemId(1,0));
    QVERIFY(index.isValid());
    QVERIFY(index.row() == 0);
}

// ---------------------------------------------------------------------------
// testRenameAlbum
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testSetAlbumInUse()
{
    mStubModel->mAlbumInUse = TMPXItemId::InvalidId();
    
    // no model
    mTestObject->setAlbumInUse(TMPXItemId(1,2));
    QVERIFY(mStubModel->mAlbumInUse == TMPXItemId::InvalidId());
    
    // model exists
    mTestObject->initialize(mStubModel);       
    mTestObject->setAlbumInUse(TMPXItemId(1,2));
    QVERIFY(mStubModel->mAlbumInUse == TMPXItemId(1,2));
}

// ---------------------------------------------------------------------------
// testRenameItem
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testRenameItem()
{
    delete mTestObject;
    mTestObject = new FilterProxyTester(VideoCollectionCommon::EModelTypeAllVideos);

    // Not initialized.
    TMPXItemId id = TMPXItemId::InvalidId();
    QString name = "";
    QVERIFY(mTestObject->renameItem(id, name) == -1);
    
    // Initialized.
    mTestObject->initialize(mStubModel);
    QVERIFY(mTestObject->renameItem(id, name) == 0);
}

// End of file
