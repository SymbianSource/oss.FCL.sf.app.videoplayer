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

#include "videocollectioncommon.h"
#include "testvideosortfilterproxymodel.h"
#include "filterproxytester.h"
#include "videolistdatamodel.h"
#include "videocollectionclient.h"
#include "videocollectioncommon.h"
#include "videothumbnaildata.h"

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    
    TestVideoSortFilterProxyModel tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideosortfilterproxymodel.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::init()
{
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
	
    mTestObject = new FilterProxyTester();
    QVERIFY(mTestObject);
    
    mStubModel = new VideoListDataModel();
    QVERIFY(mStubModel);  
    
    mCollectionClient = new VideoCollectionClient();
    QVERIFY(mCollectionClient);
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
// testInitializeDataModelInitFail
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testInitializeDataModelInitFail()
{
	VideoListDataModel::mInitFails = true; 
	
	QVERIFY(mTestObject->initialize(mStubModel) == -1);
}

// ---------------------------------------------------------------------------
// testInitializeCollectionInitFail
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testInitializeCollectionInitFail()
{
	VideoCollectionClient::mFailInit = true; 
	
	QVERIFY(mTestObject->initialize(mStubModel) == -1);
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
// testSecondInitializeModelInitFail
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testSecondInitializeModelInitFail()
{
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	VideoListDataModel::mInitFails = true; 
	QVERIFY(mTestObject->initialize(mStubModel) == -1);
}

// ---------------------------------------------------------------------------
// testOpen
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testOpen()
{
	// First open.
	QVERIFY(mTestObject->open(1) == 0); 
	// Open again with same level.
	QVERIFY(mTestObject->open(1) == 0); 
	// Fail open.
	VideoCollectionClient::mFailStartOpen = true;
	QVERIFY(mTestObject->open(20) == -1);
}

// ---------------------------------------------------------------------------
// testDeleteItems
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testDeleteItems()
{
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

	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
	mStubModel->setUsedRowCount(5);
	
	// sort to make sure that list order is different compared to source model
	mTestObject->doSorting(Qt::DisplayRole, Qt::AscendingOrder);
    // need to wait for awhile to make sure zero-counter gets 
    // processing time.
    QTest::qWait(500);
    QVERIFY(mTestObject->sortRole() == Qt::DisplayRole);
	
    QModelIndexList list;
    QModelIndex index; 
    for(int i = 0; i < 5; ++i)
    {
        index = mTestObject->index(i,0);
        list.append(index);
    }
    QModelIndexList emptyList;
	User::Heap().__DbgMarkStart();
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
	QVERIFY(mTestObject->deleteItems(emptyList) == 0);
	QVERIFY(mTestObject->deleteItems(list) == 0);
	QVERIFY(mStubModel->mLastDeletedIndexRow == 0);
	QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 2);
	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testDeleteItemsModelNull
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testDeleteItemsModelNull()
{
	mStubModel->appendData("Test");
	mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(NULL) == -1);

	QModelIndex index = mStubModel->index(0, 0);
	QModelIndexList list;
	list.append(index);	

	User::Heap().__DbgMarkStart();
	
	QVERIFY(mTestObject->deleteItems(list) == -1);

	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testDeleteItemsRemoveRowsFail
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testDeleteItemsRemoveRowsFail()
{
	VideoListDataModel::mRemoveRowsFails = true;
	
	mStubModel->appendData("Test");
	mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
	QModelIndex index = mTestObject->index(0, 0);
	QModelIndexList list;
	list.append(index);

	User::Heap().__DbgMarkStart();
	
	QVERIFY(mTestObject->deleteItems(list) == -1);

	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testOpenItem
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testOpenItem()
{	
	mStubModel->appendData("Test");
	mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	QModelIndex index = mTestObject->index(0, 0);
	
	User::Heap().__DbgMarkStart();

	QVERIFY(mTestObject->openItem(index) == 0);
	
	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testOpenItemGetMediaIdFail
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testOpenItemGetMediaIdFail()
{
	VideoListDataModel::mGetMediaIdAtIndexFails = true;
	
	mStubModel->appendData("Test");
	mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	QModelIndex index = mTestObject->index(0, 0);
	
	User::Heap().__DbgMarkStart();
		
	QVERIFY(mTestObject->openItem(index) == -1);
	
	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testOpenItemModelNull
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testOpenItemModelNull()
{	
	mStubModel->appendData("Test");
	mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(NULL) == -1); 
	
	QModelIndex index = mTestObject->index(0, 0);
	User::Heap().__DbgMarkStart();
		
	QVERIFY(mTestObject->openItem(index) == -1);
	
	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}
// ---------------------------------------------------------------------------
// testOpenItemOpenVideoFails
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testOpenItemOpenVideoFails()
{
	VideoCollectionClient::mFailMediaPlayback = true;
	
	mStubModel->appendData("Test");
	mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
	QModelIndex index = mTestObject->index(0, 0);
	User::Heap().__DbgMarkStart();
		
	QVERIFY(mTestObject->openItem(index) == -1);
	
	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testBack
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testBack()
{	
	//mStubModel->appendData("Test");
	//mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	//QModelIndex index = mTestObject->index(0, 0);
	
	User::Heap().__DbgMarkStart();

	QVERIFY(mTestObject->back() == 0);
	
	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testBack
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testBackClientNull()
{	
	//mStubModel->appendData("Test");
	//mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	//QModelIndex index = mTestObject->index(0, 0);
	VideoCollectionClient *tmpCollectionClient = mTestObject->getClient();
	mTestObject->setClient(0);
	
	User::Heap().__DbgMarkStart();

	QVERIFY(mTestObject->back() == -1);
	
	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);

	mTestObject->setClient(tmpCollectionClient);
}

// ---------------------------------------------------------------------------
// testFetchItemDetails
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testFetchItemDetails()
{
	mStubModel->appendData("Test");
	mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
	QSignalSpy fetchSpy(mTestObject, SIGNAL(shortDetailsReady(int)));
	
	QModelIndex index = mTestObject->index(0, 0);	

	QList<QVariant> arguments;
	
	User::Heap().__DbgMarkStart();
	QVERIFY(mTestObject->fetchItemDetails(index) == 0);

	QVERIFY(fetchSpy.count() == 1);
	arguments = fetchSpy.takeFirst();
	QVERIFY(arguments.at(0).toInt() == 0);
	arguments.clear();
	fetchSpy.clear();
	
	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testFetchItemDetails
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testFetchItemDetailsGetMediaIdFail()
{
	VideoListDataModel::mGetMediaIdAtIndexFails = true;
	
	mStubModel->appendData("Test");
	mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
	QSignalSpy fetchSpy(mTestObject, SIGNAL(shortDetailsReady(int)));
	
	QModelIndex index = mTestObject->index(0, 0);
	
	User::Heap().__DbgMarkStart();
	
	QVERIFY(mTestObject->fetchItemDetails(index) == -1);
	
	QVERIFY(fetchSpy.count() == 0);
	
	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testFetchItemDetails
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testFetchItemDetailsGetVideoDetailsFails()
{
	VideoCollectionClient::mFailMediaDetails = true;
	
	mStubModel->appendData("Test");
	mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
	QSignalSpy fetchSpy(mTestObject, SIGNAL(shortDetailsReady(int)));
	QModelIndex index = mTestObject->index(0, 0);
	
	User::Heap().__DbgMarkStart();
		
	QVERIFY(mTestObject->fetchItemDetails(index) == -1);
	
	QVERIFY(fetchSpy.count() == 0);

	int remHeap = User::Heap().__DbgMarkEnd(0);
	QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testLessThanProxyModelNotInitialized
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanProxyModelNotInitialized()
{
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
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
    mStubModel->setUsedRowCount(5);
   
    mTestObject->setSortRole(Qt::DisplayRole);
    
    User::Heap().__DbgMarkStart();  

    QModelIndex left = mStubModel->index(0,0);
    QModelIndex right = mStubModel->index(1,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
}
// ---------------------------------------------------------------------------
// testLessThanName
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanName()
{
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
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
    mStubModel->setUsedRowCount(5);
   
    mTestObject->setSortRole(Qt::DisplayRole);
    
    User::Heap().__DbgMarkStart();  

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
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
    // reset model
    delete mStubModel;
    mStubModel = 0;
    
    mStubModel = new VideoListDataModel();
    QVERIFY(mStubModel);
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    
    // case sensitive check
    name1 = "cc";
    name2 = "CC";
    name3 = "cC";
    name4 = "aa";
    name5 = "Aa";
    QString name6 = "2Aa";
    QString name7 = "1Aa";
    QString name8 = "1cc";
    
    mStubModel->appendData(name1); // to index 0, position can be 5, 6 or 7
    mStubModel->appendData(name2); // to index 1, position can be 5, 6 or 7
    mStubModel->appendData(name3); // to index 2, position can be 5, 6 or 7
    mStubModel->appendData(name4); // to index 3, position can be 3 or 4
    mStubModel->appendData(name5); // to index 4, position can be 3 or 4
    mStubModel->appendData(name6); // to index 5, position is 2 
    mStubModel->appendData(name7); // to index 6, position is 0
    mStubModel->appendData(name8); // to index 7, position is 1
    
    mStubModel->setUsedRowCount(8);
    
    mTestObject->setSortRole(Qt::DisplayRole);
    
    User::Heap().__DbgMarkStart();  

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
    
    remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
    
}
  
// ---------------------------------------------------------------------------
// testLessThanSize
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanSize()
{
    QVERIFY(mTestObject->initialize(mStubModel) == 0);

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
    mStubModel->setUsedRowCount(5);
    
    mTestObject->setSortRole(VideoCollectionCommon::KeySizeValue);

    User::Heap().__DbgMarkStart();  
    
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
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
}
 
// ---------------------------------------------------------------------------
// testLessThanDate
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanDateTime()
{
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
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
    mStubModel->setUsedRowCount(5);
    
    mTestObject->setSortRole(VideoCollectionCommon::KeyDateTime);

    User::Heap().__DbgMarkStart();  
    
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
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
}

// ---------------------------------------------------------------------------
// testLessThanInvalid
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testLessThanInvalid()
{
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
    // invalid role
	mTestObject->setSortRole(10000);
    
    User::Heap().__DbgMarkStart();  
    
    QModelIndex left = mStubModel->index(0,0);
    QModelIndex right = mStubModel->index(0,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
    
    // invalid role but valid data.
    
	QString name1 = "cc";
	QString name2 = "bb";
	QString name3 = "aa";
	
	
	mStubModel->appendData(name1); // to index 0, position 4
	mStubModel->appendData(name2); // to index 1, position 3
	mStubModel->appendData(name3); // to index 2, position 2
	mStubModel->setUsedRowCount(3);
   
	mTestObject->setSortRole(INVALID_ROLE_FOR_SORTING);
	
    left = mStubModel->index(0,0);
    right = mStubModel->index(1,0);
    
    User::Heap().__DbgMarkStart();
    
    QVERIFY(!mTestObject->callLessThan(left, right));

    remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
    
    // reset model
    delete mStubModel;
    mStubModel = 0;  
    
    mStubModel = new VideoListDataModel();
    QVERIFY(mStubModel);
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    
    // invalid left index
    mTestObject->setSortRole(Qt::DisplayRole);
    User::Heap().__DbgMarkStart(); 
    left = QModelIndex();
    right = mStubModel->index(0,0);
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
    
    // invalid right index
    mTestObject->setSortRole(Qt::DisplayRole);
    User::Heap().__DbgMarkStart(); 
    left =  mStubModel->index(0,0); 
    right = QModelIndex();
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
    
    // both invalid
    mTestObject->setSortRole(Qt::DisplayRole);
    User::Heap().__DbgMarkStart(); 
    left =  QModelIndex();
    right = QModelIndex();
    
    QVERIFY(!mTestObject->callLessThan(left, right));
    remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
    
    // both same index, use size
    quint32 size1 = 99;
    
    mStubModel->appendData(size1); // to index 0
    
    mTestObject->setSortRole(VideoCollectionCommon::KeySizeValue);
    mTestObject->setSortRole(Qt::DisplayRole); 
    
    User::Heap().__DbgMarkStart(); 
    
    left = mStubModel->index(0,0);
    right = mStubModel->index(0,0);
    QVERIFY(!mTestObject->callLessThan(left, right));
    remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);
 
}

// ---------------------------------------------------------------------------
// testDoSorting
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testDoSorting()
{
    QWARN("Waiting for resolving of potential memory leak in QSortFilterProxy before adding heap allocation checks here");
    
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
    
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
    
    mStubModel->setUsedRowCount(5);

    // User::Heap().__DbgMarkStart();  

    // first sort call, includes timer creation and setup
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(Qt::DisplayRole, Qt::AscendingOrder);
    // need to wait for awhile to make sure zero-counter gets 
    // processing time.
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)Qt::DisplayRole);
    QCOMPARE(mTestObject->sortOrder(), Qt::AscendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
   
    // reset spys
    spyAboutToChange.clear();
    spyChanged.clear();
      
    // second sort call, should use same timer appropriately
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(Qt::DisplayRole, Qt::DescendingOrder);
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)Qt::DisplayRole);
    QCOMPARE(mTestObject->sortOrder(), Qt::DescendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    
    // reset spys
    spyAboutToChange.clear();
    spyChanged.clear();    
    
    // double call without first letting timer to timeout
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(Qt::DisplayRole, Qt::DescendingOrder);
    mTestObject->doSorting(Qt::DisplayRole, Qt::AscendingOrder);
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)Qt::DisplayRole);
    QCOMPARE(mTestObject->sortOrder(), Qt::AscendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    
    spyAboutToChange.clear();
    spyChanged.clear();
    
    // syncronous call checks
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(Qt::DisplayRole, Qt::DescendingOrder, false);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)Qt::DisplayRole);
    QCOMPARE(mTestObject->sortOrder(), Qt::DescendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    
    spyAboutToChange.clear();
    spyChanged.clear();
    
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(Qt::DisplayRole, Qt::AscendingOrder, false);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)Qt::DisplayRole);
    QCOMPARE(mTestObject->sortOrder(), Qt::AscendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    
    spyAboutToChange.clear();
    spyChanged.clear();
    
    // check that layout signals are not send if the sorting values don't change.
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(Qt::DisplayRole, Qt::AscendingOrder);
    QCOMPARE(spyAboutToChange.count(), 0);
    QCOMPARE(spyChanged.count(), 0);
    QCOMPARE(mTestObject->sortRole(), (int)Qt::DisplayRole);
    QCOMPARE(mTestObject->sortOrder(), Qt::AscendingOrder);
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 0);
    
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
    
    spyAboutToChange.clear();
    spyChanged.clear();
    
    // invalid role call, sorting should be set to date
    VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;
    mTestObject->doSorting(Qt::DisplayRole - 100, Qt::AscendingOrder);
    QTest::qWait(500);
    QCOMPARE(spyAboutToChange.count(), 1);
    QCOMPARE(spyChanged.count(), 1);
    QCOMPARE(mTestObject->sortRole(), (int)VideoCollectionCommon::KeyDateTime);
    spyAboutToChange.clear();
    spyChanged.clear();
    QVERIFY(VideoThumbnailData::mStartBackgroundFetchingCallCount == 1);
    
   // cleanup();
   // int remHeap = User::Heap().__DbgMarkEnd(0);
   // QVERIFY(remHeap == 0);  
}

// ---------------------------------------------------------------------------
// testFilterAcceptsRow
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testFilterAcceptsRowProxyModelNotInitialized()
{	
	QModelIndex index = QModelIndex(); // index can be anything, test model doesn't use it.
    QVERIFY(mTestObject->callFilterAcceptsRow(0, index) == false);    
}

// ---------------------------------------------------------------------------
// testFilterAcceptsRow
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testFilterAcceptsRow()
{	
	QModelIndex index = QModelIndex(); // index can be anything, test model doesn't use it.
		
	// Not initialized.
	QVERIFY(mTestObject->callFilterAcceptsRow(0, index) == false);	
	
	QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
	// Test invalid row.
	QVERIFY(mTestObject->callFilterAcceptsRow(-1, index) == false);
	
	// Model data has no status role.
	mStubModel->appendData("test");
	mStubModel->setUsedRowCount(1);
	QVERIFY(mTestObject->callFilterAcceptsRow(0, index) == true);
	
    // reset model
    delete mStubModel;
    mStubModel = 0;
    
    mStubModel = new VideoListDataModel();
    QVERIFY(mStubModel);
    QVERIFY(mTestObject->initialize(mStubModel) == 0);
	
    // add data with status roles.  
	mStubModel->appendStatus(VideoCollectionCommon::StatusNone);
    mStubModel->appendStatus(VideoCollectionCommon::StatusDeleted);
    mStubModel->setUsedRowCount(2);
    
    // Test when status is not deleted.
    QVERIFY(mTestObject->callFilterAcceptsRow(0, index) == true);
    
    // Test when status is deleted.
    index = mStubModel->index(0,0);
    QVERIFY(mTestObject->callFilterAcceptsRow(1, index) == false);
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
    QString thumb("testthumb");
    QList<TMPXItemId> ids;
    
    QCOMPARE(mTestObject->addNewCollection(name, thumb, ids), -1);
    QVERIFY(VideoCollectionClient::mAddNewCollectionName != name);
}

// ---------------------------------------------------------------------------
// testAddNewCollectionSucceed
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testAddNewCollectionSucceed()
{
    QString name("testname");
    QString thumb("testthumb");
    QList<TMPXItemId> ids;
    ids.append(TMPXItemId(5, 0));
    ids.append(TMPXItemId(9, 0));
    
    mTestObject->initialize(mStubModel);
    
    QCOMPARE(mTestObject->addNewCollection(name, thumb, ids), 0);
    QCOMPARE(VideoCollectionClient::mAddNewCollectionName, name);
    QCOMPARE(VideoCollectionClient::mAddNewCollectionThumb, thumb);
    QCOMPARE(VideoCollectionClient::mAddNewCollectionIds, ids);
}

// ---------------------------------------------------------------------------
// testAddNewCollectionFail
// ---------------------------------------------------------------------------
//
void TestVideoSortFilterProxyModel::testAddNewCollectionFail()
{
    QString name("testname");
    QString thumb("testthumb");
    QList<TMPXItemId> ids;
    ids.append(TMPXItemId(5,0));
    ids.append(TMPXItemId(9,0));
    
    mTestObject->initialize(mStubModel);
    
    VideoCollectionClient::mFailAddNewCollection = true;
    
    QCOMPARE(mTestObject->addNewCollection(name, thumb, ids), -1);
    
    // this makes sure that the collection client has been called.
    QCOMPARE(VideoCollectionClient::mAddNewCollectionName, name);
    QCOMPARE(VideoCollectionClient::mAddNewCollectionThumb, thumb);
    QCOMPARE(VideoCollectionClient::mAddNewCollectionIds, ids);
}

// ---------------------------------------------------------------------------
// testShortDetailsReadySlot
// ---------------------------------------------------------------------------
//
int TestVideoSortFilterProxyModel::testShortDetailsReadySlot(int index)
{
	return 1;
}


// End of file
