
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
* Description:   tester for methods in CVideoCollectionWrapperPrivate
* 
*/


// INCLUDES

#include "testvideocollectionclient.h"
#include "videocollectioncommon.h"
#include "stub/inc/videocollectionlistener.h"

#define private public
#include "videocollectionclient.h"
#undef private

#include "stub/inc/stubcollectionsignalclasses.h"

Q_DECLARE_METATYPE(QList<int>)

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    TestVideoCollectionClient tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideocollectionclient.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}

Q_DECLARE_METATYPE(QList<TMPXItemId>) 

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::init()
{    
    mTestObject = new VideoCollectionClient();
}
    
// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::cleanup()
{
    delete mTestObject;
    mTestObject = 0;
}

// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testInitializeCollUtilityExists()
{
    MMPXCollectionUtility *tmpUtility = new MMPXCollectionUtility();
    mTestObject->mCollectionUtility = tmpUtility;
    QVERIFY(mTestObject->initialize() == 0);
    QVERIFY(mTestObject->mCollectionUtility != 0);
    QVERIFY(mTestObject->mCollectionListener != 0);
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testInitializeCollListenerExist()
{
    VideoCollectionListener *tmpListener = new VideoCollectionListener(*mTestObject);
    mTestObject->mCollectionListener = tmpListener;
    QVERIFY(mTestObject->initialize() == 0);    
    QVERIFY(mTestObject->mCollectionUtility != 0);
    QVERIFY(mTestObject->mCollectionListener != 0);
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testInitializeUtilCreateFail()
{
    MMPXCollectionUtility::setNewLLeave(true);
    QVERIFY(mTestObject->initialize() < 0);  
    QVERIFY(mTestObject->mCollectionUtility == 0);
    QVERIFY(mTestObject->mCollectionListener == 0);
    MMPXCollectionUtility::setNewLLeave(false);
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testInitializeSucceed()
{   
    MMPXCollectionUtility::setNewLLeave(false);
    QVERIFY(mTestObject->initialize() == 0);  
    // none exists
    QVERIFY(mTestObject->initialize() == 0);  
    QVERIFY(mTestObject->mCollectionUtility != 0);
    QVERIFY(mTestObject->mCollectionListener != 0);
    
    // both items exists
    QVERIFY(mTestObject->initialize() == 0);  
    QVERIFY(mTestObject->mCollectionUtility != 0);
    QVERIFY(mTestObject->mCollectionListener != 0);
    
    MMPXCollectionUtility::resetCloseCounter();
    QPointer<VideoCollectionListener> listener = mTestObject->mCollectionListener;
    
    cleanup();
    
    QVERIFY(MMPXCollectionUtility::getCloseCounter() == 1);
    QVERIFY(listener == 0);   
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testConnectCollectionSignalReceiver()
{   
    // no collection listener or signal receiver
    QVERIFY(mTestObject->connectCollectionSignalReceiver(0) == -1);

    // no collection listener
    StubSignalReceiver stubReceiver;        
    QVERIFY(mTestObject->connectCollectionSignalReceiver(&stubReceiver) == -1);
    
    // collection listener exists, no signal receiver    
    QVERIFY(mTestObject->initialize() == 0);  
    QVERIFY(mTestObject->connectCollectionSignalReceiver(0) == -1);
    
    VideoCollectionListener *tmp = mTestObject->mCollectionListener;
    
    // no newVideoList signal
    StubListenerNoNewVideoList *noNewVideoList = 
        new StubListenerNoNewVideoList(*mTestObject);
    mTestObject->mCollectionListener = noNewVideoList;
    QVERIFY(mTestObject->connectCollectionSignalReceiver(&stubReceiver) == -1);
    delete noNewVideoList;
    
    // no videoListAppended signal
    StubListenerNoVideoListAppended *noVideoListAppended = 
        new StubListenerNoVideoListAppended(*mTestObject);
    mTestObject->mCollectionListener = noVideoListAppended;
    QVERIFY(mTestObject->connectCollectionSignalReceiver(&stubReceiver) == -1);
    delete noVideoListAppended;
    
    // no newVideoAvailable signal
    StubListenerNoNewVideoAvailable *noNewVideo = 
        new StubListenerNoNewVideoAvailable(*mTestObject);
    mTestObject->mCollectionListener = noNewVideo;
    QVERIFY(mTestObject->connectCollectionSignalReceiver(&stubReceiver) == -1);
    delete noNewVideo;
    
    // no videoDeleted signal
    StubListenerNoVideoDeleted *noVideoDeleted = 
        new StubListenerNoVideoDeleted(*mTestObject);
    mTestObject->mCollectionListener = noVideoDeleted;
    QVERIFY(mTestObject->connectCollectionSignalReceiver(&stubReceiver) == -1);
    delete noVideoDeleted;
    
    // no videoDeleteCompleted signal
    StubListenerNoVideoDeleteCompleted *noVideoDelCompl = 
        new StubListenerNoVideoDeleteCompleted(*mTestObject);
    mTestObject->mCollectionListener = noVideoDelCompl;
    QVERIFY(mTestObject->connectCollectionSignalReceiver(&stubReceiver) == -1);
    delete noVideoDelCompl;
    
    // no videoDetailsCompleted signal
    StubListenerNoVideoDetailsCompleted *noVideoDetails = 
        new StubListenerNoVideoDetailsCompleted(*mTestObject);
    mTestObject->mCollectionListener = noVideoDetails;
    QVERIFY(mTestObject->connectCollectionSignalReceiver(&stubReceiver) == -1);
    delete noVideoDetails;
    
    // all signals exists
    StubListenerAllSignals *allSignals = 
        new StubListenerAllSignals(*mTestObject);
    mTestObject->mCollectionListener = allSignals;
    QVERIFY(mTestObject->connectCollectionSignalReceiver(&stubReceiver) == 0);
    delete allSignals;
    
    mTestObject->mCollectionListener = tmp;
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testCollectionLevel()
{
    // no collection utility
    QVERIFY(mTestObject->getCollectionLevel() == -1);
    
    QVERIFY(mTestObject->initialize() == 0);  
    
    // path level not setted
    QVERIFY(mTestObject->getCollectionLevel() == -1);

    // succeeds
    mTestObject->mCollectionPathLevel = VideoCollectionClient::ELevelCategory;
    QVERIFY(mTestObject->getCollectionLevel() == VideoCollectionClient::ELevelCategory);
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testGetOpenStatus()
{
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionNotOpen);
    
    mTestObject->setOpenStatus(100);
    
    QVERIFY(mTestObject->getOpenStatus() == 100);  
}
     
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testSetOpenStatus()
{
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionNotOpen);
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionNotOpen);
    
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionOpened);
    
    mTestObject->setOpenStatus(100);
    QVERIFY(mTestObject->getOpenStatus() == 100);
    
}
    
// -----------------------------------------------------------------------------
// testInitializeCollUtilityExists
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testStartOpenCollection()
{
    // no collection utility    
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelVideos) == -1);

    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelCategory) == -1);

    QVERIFY(mTestObject->initialize() == 0); 
    // open status setted allready
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpening); 
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelVideos) == 0);
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelVideos) == 0);

    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpening); 
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelCategory) == 0);
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelCategory) == 0);
    
    // open status and level setted allready
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpening); 
    mTestObject->mCollectionPathLevel = VideoCollectionClient::ELevelVideos;
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelVideos) == 0);
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelVideos) == 0);

    mTestObject->mCollectionPathLevel = VideoCollectionClient::ELevelCategory;
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpening); 
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelCategory) == 0);
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelCategory) == 0);

    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionNotOpen);  
    // startOpenCollectionL -leaves
    MMPXCollection::setOpenLPathLeave(true);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelVideos) < 0);

    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelCategory) < 0);

    MMPXCollection::setOpenLPathLeave(false);
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelVideos) == 0);
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    int id = 0;
    int type = 0;
    mTestObject->getCategoryIds(id, type);
    QVERIFY(id == KVcxMvcCategoryIdAll);
    QVERIFY(type == 1);

    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelCategory) == 0);
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    mTestObject->getCategoryIds(id, type);
    QVERIFY(id == 0);
    QVERIFY(type == 9);

    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    QVERIFY(mTestObject->startOpenCollection(VideoCollectionClient::ELevelVideos) == 0);
    QVERIFY(mTestObject->getOpenStatus() == VideoCollectionClient::ECollectionOpening);

}

// -----------------------------------------------------------------------------
// testStartOpenCurrentState
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testStartOpenCurrentState()
{
    // no collection utility
    QVERIFY(mTestObject->startOpenCurrentState() == -1);    
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionNotOpen);
    QVERIFY(mTestObject->startOpenCurrentState() == -1);  
    
    // collection not opened
    QVERIFY(mTestObject->initialize() == 0); 
    QVERIFY(mTestObject->startOpenCurrentState() == -1); 
    
    // OpenL -leaves
    mTestObject->setOpenStatus(VideoCollectionClient::ECollectionOpened);
    MMPXCollection::setOpenLLeave(true);
    QVERIFY(mTestObject->startOpenCurrentState() < 0); 
   
    // succeed
    MMPXCollection::setOpenLLeave(false);
    QVERIFY(mTestObject->startOpenCurrentState() == 0); 
    
}

// -----------------------------------------------------------------------------
// testDeleteVideos
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testDeleteVideos()
{
    // no collection utility
    QVERIFY(mTestObject->deleteVideos(0) == -1);       
    QList<TMPXItemId> ids;
    QVERIFY(mTestObject->deleteVideos(&ids) == -1); 
    
    QVERIFY(mTestObject->initialize() == 0); 
    // empty list
    QVERIFY(mTestObject->deleteVideos(&ids) < 0); 
    
    // one item
    ids.append(TMPXItemId(1,0));
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->deleteVideos(&ids) < 0); 
    MMPXCollection::setCommandLLeave(false);
    QVERIFY(mTestObject->deleteVideos(&ids) == 0);
    
    // more than one
    ids.append(TMPXItemId(2,0));
    ids.append(TMPXItemId(3,0));
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->deleteVideos(&ids) < 0); 
    MMPXCollection::setCommandLLeave(false);
    QVERIFY(mTestObject->deleteVideos(&ids) == 0);

}
 
// -----------------------------------------------------------------------------
// testOpenCategory
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testOpenCategory()
{
    TMPXItemId id(0,0);
    // no collection utility    
    QVERIFY(mTestObject->openVideo(id) == -1);  
    
    // collection exists
    QVERIFY(mTestObject->initialize() == 0); 
      
    MMPXCollection::setOpenLPathLeave(true);
    QVERIFY(mTestObject->openVideo(id)< 0);
    
    MMPXCollection::setOpenLPathLeave(false);

    
    mTestObject->mCollectionPathLevel = VideoCollectionClient::ELevelCategory;
    QVERIFY(mTestObject->openVideo(id) == 0);
      
}

// -----------------------------------------------------------------------------
// testOpenVideo
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testOpenVideo()
{
    TMPXItemId id(0,0);
    // no collection utility    
    QVERIFY(mTestObject->openVideo(id) == -1);  
    
    // collection exists
    QVERIFY(mTestObject->initialize() == 0); 
    mTestObject->mCollectionPathLevel = VideoCollectionClient::ELevelVideos;  
    MMPXCollection::setOpenLPathLeave(true);
    QVERIFY(mTestObject->openVideo(id)< 0);
    MMPXCollection::setOpenLPathLeave(false);
    
    QVERIFY(mTestObject->openVideo(id) == 0);
}

// -----------------------------------------------------------------------------
// testFetchMpxMediaByMpxId
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testFetchMpxMediaByMpxId()
{
    // no collection utility 
    TMPXItemId id;
    QVERIFY(mTestObject->fetchMpxMediaByMpxId(id) == -1); 
    
    // collection exists
    QVERIFY(mTestObject->initialize() == 0); 
    
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->fetchMpxMediaByMpxId(id) < 0);
    
    MMPXCollection::setCommandLLeave(false);
    QVERIFY(mTestObject->fetchMpxMediaByMpxId(id) == 0);
    
}

// -----------------------------------------------------------------------------
// testGetVideoDetails
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testGetVideoDetails()
{
    // no collection utility 
    TMPXItemId id(0,0); 
    QVERIFY(mTestObject->getVideoDetails(id) == -1); 
    
    // collection exists
    QVERIFY(mTestObject->initialize() == 0); 
    
    MMPXCollection::setCommandLLeave(true);
    QVERIFY(mTestObject->getVideoDetails(id) < 0);
    
    MMPXCollection::setCommandLLeave(false);
    QVERIFY(mTestObject->getVideoDetails(id) == 0);
    
}

// -----------------------------------------------------------------------------
// testAddNewCollection_data
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testAddNewCollection_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("thumb");
    QTest::addColumn<QList<TMPXItemId> >("ids");
    
    
    
    QList<TMPXItemId> testIds;
    testIds.append(TMPXItemId(5,0));
    testIds.append(TMPXItemId(9,0));
    
    QTest::newRow("All params empty")
        << QString()
        << QString()
        << QList<TMPXItemId>();
    
    QTest::newRow("Only name")
        << QString("testname")
        << QString()
        << QList<TMPXItemId>();
    
    QTest::newRow("Only thumb")
        << QString()
        << QString("testthumb")
        << QList<TMPXItemId>();
    
    QTest::newRow("Name and thumb")
        << QString("testname")
        << QString("testthumb")
        << QList<TMPXItemId>();
    
    QTest::newRow("Only ids")
        << QString()
        << QString()
        << testIds;
    
    QTest::newRow("Name and ids")
        << QString("testname")
        << QString()
        << testIds;
    
    QTest::newRow("Thumb and ids")
        << QString()
        << QString("testthumb")
        << testIds;
    
    QTest::newRow("All params")
        << QString("testname")
        << QString("testthumb")
        << testIds;
    
}

// -----------------------------------------------------------------------------
// testAddNewCollection
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testAddNewCollection()
{

    QFETCH(QString, name);
    QFETCH(QString, thumb);
    QFETCH(QList<TMPXItemId>, ids);
    
    QCOMPARE(mTestObject->addNewCollection(name, thumb, ids), -1);
    
    mTestObject->initialize();
    
    QCOMPARE(mTestObject->addNewCollection(name, thumb, ids), 0);
    
    // TODO needs proper verification after the method has been fully implemented.
}

// -----------------------------------------------------------------------------
// testBack
// -----------------------------------------------------------------------------
//
void TestVideoCollectionClient::testBack()
{

    CMPXCollectionPath::setLevel(3);
    // not initialized    
    QVERIFY(mTestObject->back() == -1);  
    
    QVERIFY(mTestObject->initialize() == 0); 
    mTestObject->mCollectionPathLevel = VideoCollectionClient::ELevelVideos;   
    MMPXCollection::setBackLLeave(true);
    QVERIFY(mTestObject->back()< 0);
    MMPXCollection::setBackLLeave(false);
    QVERIFY(mTestObject->back() == 0); 
    
    mTestObject->mCollectionPathLevel = VideoCollectionClient::ELevelCategory;
    QVERIFY(mTestObject->back() == 0); 
}

// End of file

