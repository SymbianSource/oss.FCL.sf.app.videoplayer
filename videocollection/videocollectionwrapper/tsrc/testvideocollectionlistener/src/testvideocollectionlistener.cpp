
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
* Description:   tester for methods in CVideoCollectionListener
* 
*/


// INCLUDES
#include <e32cmn.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxcollectionmessage.h>
#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>
#include "testvideocollectionlistener.h"
#include "mediaobjectfactory.h"
#include "stub/inc/stubsignalreceiver.h"
#include "stub/inc/stubcollection.h"

#include "videocollectionutils.h"

#define private public
#include "videocollectionlistener.h"
#undef private


// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    TestVideoCollectionListener tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideocollectionlistener.txt";
    
    int res = QTest::qExec(&tv, 3, pass);
    
    return res;
}

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::initTestCase()
{
    mMediaFactory = new MediaObjectFactory();
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::init()
{   
    mStubCollectionClient = new VideoCollectionClient();
    
    mTestObject = new VideoCollectionListener(*mStubCollectionClient);
    
    mStubCollection = new StubCollection(*mTestObject);
    
    qRegisterMetaType<CMPXMediaArray*>("CMPXMediaArray*");
    mSpyNewVideoList = new QSignalSpy(mTestObject, SIGNAL(newVideoList(CMPXMediaArray*)));
    
    mSpyVideoListAppended = new QSignalSpy(mTestObject, SIGNAL(videoListAppended(CMPXMediaArray*)));
    
    qRegisterMetaType<CMPXMedia*>("CMPXMedia*");
    mSpyNewVideoAvailable = new QSignalSpy(mTestObject, SIGNAL(newVideoAvailable(CMPXMedia*)));   
    
    qRegisterMetaType<TMPXItemId>("TMPXItemId");
    
    mSpyVideoDeleted = new QSignalSpy(mTestObject, SIGNAL(videoDeleted(TMPXItemId)));

    qRegisterMetaType<QList<TMPXItemId>*>("QList<TMPXItemId>*");
    mSpyVideoDeleteCompleted = new QSignalSpy(mTestObject, SIGNAL(videoDeleteCompleted(int, QList<TMPXItemId>*)));
    
    mSpyVideoDetailsCompleted = new QSignalSpy(mTestObject, SIGNAL(videoDetailsCompleted(TMPXItemId))); 
}
    
// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::cleanup()
{
    delete mTestObject;
    mTestObject = 0;
    
    delete mStubCollectionClient;
    mStubCollectionClient = 0;
    
    delete mStubCollection;
    mStubCollection = 0;
    
    delete mSpyNewVideoList;
    mSpyNewVideoList = 0;
    
    delete mSpyVideoListAppended;
    mSpyVideoListAppended = 0;
        
    delete mSpyNewVideoAvailable;   
    mSpyNewVideoAvailable = 0;
    
    delete mSpyVideoDeleted;
    mSpyVideoDeleted = 0;
    
    delete mSpyVideoDeleteCompleted;
    mSpyVideoDeleteCompleted = 0;
    
    delete mSpyVideoDetailsCompleted; 
    mSpyVideoDetailsCompleted = 0;
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::cleanupTestCase()
{
    delete mMediaFactory;
}

// -----------------------------------------------------------------------------
// testHandleCollectionMediaL
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleCollectionMediaLFunc()
{
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));
    // nothing to test here, because
    // currently HandleCollectionMediaL is empty implementation required 
    // by the inheritance of MMPXCollectionObserver.
    User::Heap().__DbgMarkStart();   
    
    CMPXMedia *media = NULL;
    TRAP_IGNORE(media = CMPXMedia::NewL());
    mStubCollection->callHandleCollectionMediaLFunc(*media, 0);
    delete media;
    
    QVERIFY(!hasSignalsEmitted());
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);   
}

// -----------------------------------------------------------------------------
// testHandleOpenLMediaFunc
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleOpenLMediaFunc()
{   
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));

    User::Heap().__DbgMarkStart();
    
    mStubCollectionClient->setCollectionLevel(VideoCollectionClient::ELevelVideos);      
    CMPXMedia *media = mMediaFactory->newMedia(0);
    
    // error != KErrNone
    mStubCollection->callHandleOpenLFunc(*media, 0, true, -2 );
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(mSpyNewVideoList->count() == 0);
    QVERIFY(mSpyVideoListAppended->count() == 0);
    
    // collectionLevel != LevelVideos
    mStubCollectionClient->setCollectionLevel(-100);
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );
    
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(mSpyNewVideoList->count() == 0);
    QVERIFY(mSpyVideoListAppended->count() == 0);
    mStubCollectionClient->setCollectionLevel(VideoCollectionClient::ELevelVideos);
    
    // empty media (== media does not contain array)
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(mSpyNewVideoList->count() == 0);
    QVERIFY(mSpyVideoListAppended->count() == 0);
    
    // signal emitting: correct media-array passed
    CMPXMediaArray *array = mMediaFactory->newMediaArray();  
    
    // empty array
    mMediaFactory->putValuePtr<CMPXMediaArray>(media, KMPXMediaArrayContents, array);   
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );    
    QVERIFY(receiver.getLatestPointerAddr() != 0);
    QVERIFY(mSpyNewVideoList->count() == 1);
    QVERIFY(mSpyVideoListAppended->count() == 0);
    
    // item for invalid level, no new signals emitted
    mStubCollectionClient->setCollectionLevel(VideoCollectionClient::ELevelCategory);
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(1, 2));
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );  
    QVERIFY(receiver.getLatestPointerAddr() != 0);
    QVERIFY(mSpyNewVideoList->count() == 1);
    QVERIFY(mSpyVideoListAppended->count() == 0); 
    
    mStubCollectionClient->setCollectionLevel(VideoCollectionClient::ELevelVideos);
    delete array;
    array = mMediaFactory->newMediaArray();  
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(1));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(2));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(3));

    mMediaFactory->putValuePtr<CMPXMediaArray>(media, KMPXMediaArrayContents, array);   
    mTestObject->setRequestNewMediaArray(true);
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );    
    
    QVERIFY(receiver.getLatestPointerAddr() != 0);
    QVERIFY(mSpyNewVideoList->count() == 2);
    QVERIFY(mSpyVideoListAppended->count() == 0);
    
    mStubCollection->callHandleOpenLFunc(*media, 0, true, 0 );    
       
   QVERIFY(receiver.getLatestPointerAddr() != 0);
   QVERIFY(mSpyNewVideoList->count() == 2);
   QVERIFY(mSpyVideoListAppended->count() == 1);
    
    
    CMPXMediaArray *gottenArray = static_cast<CMPXMediaArray*>(receiver.getLatestPointerAddr());
    QVERIFY(gottenArray->Count() == 3);
    
    TMPXItemId mediaId = TMPXItemId::InvalidId();
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>((*gottenArray)[0], KMPXMediaGeneralId, mediaId );
    QVERIFY(mediaId.iId1 == 1);
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>((*gottenArray)[1], KMPXMediaGeneralId, mediaId );
    QVERIFY(mediaId.iId1 == 2);
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>((*gottenArray)[2], KMPXMediaGeneralId, mediaId );
    QVERIFY(mediaId.iId1 == 3);

    delete array;
    delete media;
    mSpyNewVideoList->clear();
    mSpyVideoListAppended->clear();
    QVERIFY(!hasSignalsEmitted());
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());  
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);    
}

// -----------------------------------------------------------------------------
// testHandleOpenLPlaylistFunc
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleOpenLPlaylistFunc()
{
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));
    
    // nothing to test here, because
    // currently HandleCollectionMediaL is empty implementation required 
    // by the inheritance of MMPXCollectionObserver.
    User::Heap().__DbgMarkStart(); 
    CMPXCollectionPlaylist *plList = 0;
    
    mStubCollection->callHandleOpenLFunc(*plList, 0);
    
    QVERIFY(!hasSignalsEmitted());
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0); 
}

// -----------------------------------------------------------------------------
// testHandleCommandComplete
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleCommandComplete()
{
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));    
    
    User::Heap().__DbgMarkStart(); 
    
    CMPXCommand *pCommand = mMediaFactory->newMedia(0);
    
    // error != KErrNone
    mStubCollection->callHandleCommandComplete(pCommand, -2);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSpyVideoDetailsCompleted->count() == 0);
    
    // command == NULL
    mStubCollection->callHandleCommandComplete(0, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSpyVideoDetailsCompleted->count() == 0);
    
    // null command + error != KErrNone
    mStubCollection->callHandleCommandComplete(0, -2);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSpyVideoDetailsCompleted->count() == 0);    
    
    // command without id
    delete pCommand;
    pCommand = mMediaFactory->newMedia();
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSpyVideoDetailsCompleted->count() == 0);
    
    // no command attribute
    delete pCommand;
    pCommand = mMediaFactory->newMedia(0);
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSpyVideoDetailsCompleted->count() == 0);
    
    // invalid KVcxMediaMyVideosCommandId
    int value = (KVcxCommandMyVideosGetMediaFullDetailsByMpxId + 10);
    mMediaFactory->putTValue<int>(pCommand, KVcxMediaMyVideosCommandId, value );
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSpyVideoDetailsCompleted->count() == 0);
    
    // no media object in command
    value = KVcxCommandMyVideosGetMediaFullDetailsByMpxId;
    mMediaFactory->putTValue<int>(pCommand, KVcxMediaMyVideosCommandId, value );
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(mSpyVideoDetailsCompleted->count() == 0);
    
    // NOTE! we cannot test with item without KMPXMediaGeneralId, because after creation CMPXMedia 
    //       always contain default value
    
    // correct item    
    CMPXMedia *subMedia = mMediaFactory->newMedia(10);
    mMediaFactory->putValuePtr<CMPXMedia>(pCommand, KMPXCommandColAddMedia, subMedia);
    mStubCollection->callHandleCommandComplete(pCommand, 0);
    QVERIFY(receiver.getLatestItemId().iId1  == 10);
    QVERIFY(mSpyVideoDetailsCompleted->count() == 1);
    delete subMedia;
    delete pCommand;
    mSpyVideoDetailsCompleted->clear();
    
    QVERIFY(!hasSignalsEmitted());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);   
}

// -----------------------------------------------------------------------------
// testCollectionMessageError
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testCollectionMessageError()
{
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));

    User::Heap().__DbgMarkStart(); 
      
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    
    // correct message, error != KErrNone
    mStubCollection->callHandleCollectionMessage(message, -1);
    QVERIFY(mStubCollectionClient->getOpenStatus() == -1);

    // message does not contain KMPXMessageGeneralId
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == -1);
    
    delete message;
    
    // empty message
    mStubCollection->callHandleCollectionMessage(0, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == -1);
     
    // none of above calls should not cause any signal emitting
    QVERIFY(!hasSignalsEmitted());
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(receiver.getLatestIntegerData() == -1);
         
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);    
}

// -----------------------------------------------------------------------------
// testCollectionMessageNotOpened
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testCollectionMessageNotOpened()
{
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));

    User::Heap().__DbgMarkStart(); 
    mStubCollectionClient->setOpenStatus( VideoCollectionClient::ECollectionOpening);   
    
    // collection not opened, empty message
    mStubCollection->callHandleCollectionMessage(0, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    // collection not opened, no msg id
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    TMPXMessageId msgId = 1;    
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    
    // collection not opened, invalid msg id
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    msgId = KMPXMessageGeneral;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    
    // collection not opened, correct msg id, missing all details
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
        
    int value = -1;
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralEvent, value);
    
    // collection not opened, correct msg id, missing  KMPXMessageGeneralData
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    delete message;
    message = mMediaFactory->newMedia(0);
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralData, value);
    
    // collection not opened, correct msg id, missing KMPXMessageGeneralEvent
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralEvent, value);
    
    // collection not opened, correct msg id, invalid detail values
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    value = EMcContainerOpened;
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralData, value);
    
    // collection not opened, correct msg id, invalid KMPXMessageGeneralEvent value
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
    
    value = -1;
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralData, value);
    value = TMPXCollectionMessage::EPathChanged;
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralEvent, value);
    
    // collection not opened, correct msg id, invalid KMPXMessageGeneralData value
    mStubCollection->callHandleCollectionMessage(message, 0);    
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpening);
        
    value = EMcContainerOpened;
    mMediaFactory->putTValue<int>(message, KMPXMessageGeneralData, value);
    mStubCollectionClient->setOpenStatus(-1); 
    // correct message, open status != CVideoCollectionClient::ECollectionOpening
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == -1);
    
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpening); 
    // correct message, open status == CVideoCollectionClient::ECollectionOpening
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mStubCollectionClient->getOpenStatus() == VideoCollectionClient::ECollectionOpened);
    
    // none of above calls should not cause any signal emitting
    QVERIFY(!hasSignalsEmitted());
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(receiver.getLatestPointerAddr() == 0); 
    QVERIFY(receiver.getLatestIntegerData() == -1);

    delete message;
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);     
}

// -----------------------------------------------------------------------------
// testCollectionMessageMyVidCommandId
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testCollectionMessageMyVidCommandId()
{
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));

    User::Heap().__DbgMarkStart(); 
    
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    TMPXMessageId msgId = KVcxCommandIdMyVideos;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    
    // KMPXMessageCollectionId != KVcxUidMyVideosMpxCollection 
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    TUid uid = {KVcxUidMyVideosMpxCollection};
    mMediaFactory->putTValue<TUid>(message, KMPXMessageCollectionId, uid);
    
    // does not contain KVcxMediaMyVideosCommandId
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    int value = KVcxMessageMyVideosMessageArray + 1;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    
    // non KVcxMessageMyVideosMessageArray, invalid value
    // (this is tested throughoutly in testCollectionMessageNonMyVidMsgArray)
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    value = KVcxMessageMyVideosMessageArray;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    CMPXMediaArray *nullArray = 0;     
    mMediaFactory->putValuePtr<CMPXMediaArray>(message, KMPXMediaArrayContents, nullArray);
    // KVcxMessageMyVideosMessageArray, but no contents. 
    // (this is tested throughoutly in testCollectionMessageMyVidMsgArray)    
    mStubCollection->callHandleCollectionMessage(message, 0);
    delete message;         
    
    // none of above calls should not cause any signal emitting
    QVERIFY(!hasSignalsEmitted());
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(receiver.getLatestPointerAddr() == 0); 
    QVERIFY(receiver.getLatestIntegerData() == -1);

    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);     
    
}
 
// -----------------------------------------------------------------------------
// testHandleMyVideosMessageArray
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleMyVideosMessageArray()
{
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));

    User::Heap().__DbgMarkStart(); 
        
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    TMPXMessageId msgId = KVcxCommandIdMyVideos;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    TUid uid = {KVcxUidMyVideosMpxCollection};
    mMediaFactory->putTValue<TUid>(message, KMPXMessageCollectionId, uid);
    int value = KVcxMessageMyVideosMessageArray;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
   
    // no message array
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    // CMPXMedia *subMedia = mMediaFactory->newMedia(0);
    // mMediaFactory->putValuePtr<CMPXMedia>( message, KMPXMessageArrayContents, subMedia);
    //////////////////
    // NOTE
    // We cannot test case where message containsCMPXMedia instead of CMPXMessageArray.
    // collection fw does not support type check of pointers. Whatever pointer we have it is supported as long as
    // it is saved into message with correct attribute.
    //
    // this kind of situation causes crash in application because we cannot validate type.
   
    // mStubCollection->callHandleCollectionMessage(message, 0);
    // delete subMedia;
    CMPXMediaArray *array = mMediaFactory->newMediaArray();
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);
    
    // empty message array
    mStubCollection->callHandleCollectionMessage(message, 0);  
    
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(1));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(2));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(3));
    mMediaFactory->putArrayContent(array, mMediaFactory->newMedia(4));
    
    // message array containing invalid ( == empty) items
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    delete array;
    array = mMediaFactory->newMediaArray();
    CMPXMedia *media = 0;
    
    value = 0;
    media = mMediaFactory->newMedia(1);
    mMediaFactory->putTValue<int>(media, KVcxMediaMyVideosCommandId, value );
    mMediaFactory->putArrayContent(array, media);
    value = -1;
    media = mMediaFactory->newMedia(2);
    mMediaFactory->putTValue<int>(media, KVcxMediaMyVideosCommandId, value );
    mMediaFactory->putArrayContent(array, media);
    value = -2;
    media = mMediaFactory->newMedia(3);
    mMediaFactory->putTValue<int>(media, KVcxMediaMyVideosCommandId, value );
    mMediaFactory->putArrayContent(array, media);
     
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);
    
    // message array containing KVcxMediaMyVideosCommandId -typed items
    mStubCollection->callHandleCollectionMessage(message, 0);
 
    delete array;
    array = mMediaFactory->newMediaArray(); 
    TMPXMessageId valueid = 1;
    media = mMediaFactory->newMedia(1);
    mMediaFactory->putTValue<TMPXMessageId>(media, KMPXMessageGeneralId, valueid );
    mMediaFactory->putArrayContent(array, media);
    valueid = 2;
    media = mMediaFactory->newMedia(2);
    mMediaFactory->putTValue<TMPXMessageId>(media, KMPXMessageGeneralId, valueid );
    mMediaFactory->putArrayContent(array, media);
    valueid = 3;
    media = mMediaFactory->newMedia(3);
    mMediaFactory->putTValue<TMPXMessageId>(media, KMPXMessageGeneralId, valueid );
    mMediaFactory->putArrayContent(array, media);
     
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);   
    
    // message array containing KMPXMessageGeneralId -typed items
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    delete array;
    array = mMediaFactory->newMediaArray(); 
    valueid = 0;
    media = mMediaFactory->newMedia(1);
    mMediaFactory->putTValue<TMPXMessageId>(media, KMPXMessageGeneralId, valueid );
    mMediaFactory->putArrayContent(array, media);
    value = 1;
    media = mMediaFactory->newMedia(2);
    mMediaFactory->putTValue<TMPXMessageId>(media, KVcxMediaMyVideosCommandId, valueid );
    mMediaFactory->putArrayContent(array, media);
    valueid = 2;
    media = mMediaFactory->newMedia(3);
    mMediaFactory->putTValue<TMPXMessageId>(media, KMPXMessageGeneralId, valueid );
    mMediaFactory->putArrayContent(array, media);
         
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);
    
    // message array contains both KVcxMediaMyVideosCommandId and KMPXMessageGeneralId typed items
    mStubCollection->callHandleCollectionMessage(message, 0);
    delete array;
    delete message;
    // none of above calls should cause any signal emitting
    QVERIFY(!hasSignalsEmitted());
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(receiver.getLatestPointerAddr() == 0);  
    QVERIFY(receiver.getLatestIntegerData() == -1);
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0); 
}
    
// -----------------------------------------------------------------------------
// testHandleMyVideosMPXMessage
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleMyVideosMPXMessage()
{
    // tests all other my videos msg ids than KVcxMessageMyVideosDeleteResp.
    // it is tested in testMyVideosDeleteMsgArray
    
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));

    User::Heap().__DbgMarkStart(); 
        
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    TMPXMessageId msgId = KVcxCommandIdMyVideos;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    TUid uid = {KVcxUidMyVideosMpxCollection};
    mMediaFactory->putTValue<TUid>(message, KMPXMessageCollectionId, uid);
    
    // msg ids, that should not cause any additional funtionality (verified at the end of the method)
    int value = KVcxMessageMyVideosDeleteStarted;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);        
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    value = KVcxMessageMyVideosMoveOrCopyStarted;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    value = KVcxMessageMyVideosMoveResp;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    value = KVcxMessageMyVideosCopyResp;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    mStubCollection->callHandleCollectionMessage(message, 0);
    
    // Handling of KVcxMessageMyVideosItemsAppended
    VideoCollectionClient::mStartOpenCurrentStateCallCount = 0;
    value = KVcxMessageMyVideosItemsAppended;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QCOMPARE(VideoCollectionClient::mStartOpenCurrentStateCallCount, 1);
    
    // Handling of // Handling of KVcxMessageMyVideosItemsAppended
    VideoCollectionClient::mStartOpenCurrentStateCallCount = 0;
    value = KVcxMessageMyVideosListComplete;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QCOMPARE(VideoCollectionClient::mStartOpenCurrentStateCallCount, 1);    
    
    //////   
    // KVcxMessageMyVideosGetMediasByMpxIdResp
    value = KVcxMessageMyVideosGetMediasByMpxIdResp;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);   
    
    // no contents
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSpyNewVideoAvailable->count() == 0);
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(receiver.getLatestIntegerData() == -1);
    
    CMPXMediaArray *array = mMediaFactory->newMediaArray();
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMediaArrayContents, array);
    
    // empty array 
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSpyNewVideoAvailable->count() == 0);
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(receiver.getLatestIntegerData() == -1);
    
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(10));
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(11));
    
    // item(s) exists
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(mSpyNewVideoAvailable->count() == 1);
    QVERIFY(receiver.getLatestPointerAddr() != 0);
    CMPXMedia *fetched = static_cast<CMPXMedia*>(receiver.getLatestPointerAddr());
    int fetchedId = -1;
    VideoCollectionUtils::instance().mediaValue<int>(fetched, KMPXMediaGeneralId, fetchedId );
    QVERIFY(fetchedId == 10);
            
    mSpyNewVideoAvailable->clear();
    delete array;
    delete message;
    QVERIFY(!hasSignalsEmitted());
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());  
    QVERIFY(receiver.getLatestPointerAddr() != 0);  
    QVERIFY(receiver.getLatestIntegerData() == -1);
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0); 
}

// -----------------------------------------------------------------------------
// testCollectionMessageNonMyVidMsgArray
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testMyVideosDeleteMsgArray()
{
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));

    User::Heap().__DbgMarkStart(); 
        
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    TMPXMessageId msgId = KVcxCommandIdMyVideos;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    TUid uid = {KVcxUidMyVideosMpxCollection};
    mMediaFactory->putTValue<TUid>(message, KMPXMessageCollectionId, uid);
    
    int value = KVcxMessageMyVideosDeleteResp;
    mMediaFactory->putTValue<int>( message, KVcxMediaMyVideosCommandId, value);  
    
    // null array  mSpyVideoDeleteCompleted->count()
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(receiver.getLatestIntegerData() == -1);
    QVERIFY(mSpyVideoDeleteCompleted->count() == 0);
    
    // empty array
    CMPXMediaArray *array = mMediaFactory->newMediaArray();
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMediaArrayContents, array);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(receiver.getLatestIntegerData() == -1);
    QVERIFY(mSpyVideoDeleteCompleted->count() == 0);
    
    // item(s) does not contain KVcxMediaMyVideosInt32Value
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(1));
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(2));
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(3));
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(4));    
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMessageArrayContents, array);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId()); 
    QVERIFY(receiver.getLatestPointerAddr() != 0);
    QVERIFY(receiver.getLatestIntegerData() == 4);
    QVERIFY(receiver.getLatestListData().count() == 0);
    QVERIFY(mSpyVideoDeleteCompleted->count() == 1);
    mSpyVideoDeleteCompleted->clear();
    
    delete array;
    array = mMediaFactory->newMediaArray();
    mMediaFactory->putValuePtr<CMPXMediaArray>( message, KMPXMediaArrayContents, array);
   
    // item(s) containing KVcxMediaMyVideosInt32Value -values, both incorrect (1) and correct (3)
    CMPXMedia *media = 0;
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(1));
    
    media = mMediaFactory->newMedia(2);
    value = 1;
    mMediaFactory->putTValue<int>( media, KVcxMediaMyVideosInt32Value, value);
    mMediaFactory->putArrayContent( array, media);
    
    media = mMediaFactory->newMedia(3); 
    value = 1;
    mMediaFactory->putTValue<int>( media, KVcxMediaMyVideosInt32Value, value);
    mMediaFactory->putArrayContent( array, media);
    
    media = mMediaFactory->newMedia(4);
    value = 0;
    mMediaFactory->putTValue<int>( media, KVcxMediaMyVideosInt32Value, value);
    mMediaFactory->putArrayContent( array, media);
    
    media = mMediaFactory->newMedia(5);
    value = 1;
    mMediaFactory->putTValue<int>( media, KVcxMediaMyVideosInt32Value, value);
    mMediaFactory->putArrayContent( array, media);
    
    mMediaFactory->putArrayContent( array, mMediaFactory->newMedia(6));  
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestIntegerData() == 6);
    QVERIFY(receiver.getLatestPointerAddr() != 0);
    QVERIFY(receiver.getLatestListData().count() == 3); 
    QVERIFY(receiver.getLatestListData().value(0).iId1 == 2); 
    QVERIFY(receiver.getLatestListData().value(1).iId1 == 3); 
    QVERIFY(receiver.getLatestListData().value(2).iId1 == 5); 
    QVERIFY(mSpyVideoDeleteCompleted->count() == 1);
   
    receiver.resetLatestItems();
    mSpyVideoDeleteCompleted->clear();
    delete array;
    delete message;
    
    QVERIFY(!hasSignalsEmitted());
    
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);     
}

// -----------------------------------------------------------------------------
// testCollectionMessageNonMyVidCommandId
// -----------------------------------------------------------------------------
//
void TestVideoCollectionListener::testHandleMPXMessage()
{
    StubSignalReceiver receiver;
    QVERIFY(connectSignals(&receiver));

    User::Heap().__DbgMarkStart(); 
           
    mStubCollectionClient->setOpenStatus(VideoCollectionClient::ECollectionOpened);  
    CMPXMessage *message = mMediaFactory->newMedia(0); 
    TMPXMessageId msgId = KMPXMessageGeneral; 
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    TUid uid = {KVcxUidMyVideosMpxCollection};
    mMediaFactory->putTValue<TUid>(message, KMPXMessageCollectionId, uid);
    
    // NOTE: 
    // - we're testing only for KMPXMessageGeneral only once, because handleGeneralMPXMessage is already verified
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(!hasSignalsEmitted()); 
    
    
    // invalid
    msgId = KMPXMessageIdItemChanged + 1;
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(!hasSignalsEmitted());
    
    // KMPXMessageIdItemChanged with no content 
    mStubCollectionClient->setCollectionLevel(VideoCollectionClient::ELevelVideos);
    msgId = KMPXMessageIdItemChanged;
    mMediaFactory->putTValue<TMPXMessageId>(message, KMPXMessageGeneralId, msgId);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(!hasSignalsEmitted());
    
    // KMPXMessageIdItemChanged with level != CVideoCollectionClient::ELevelVideos
    mStubCollectionClient->setCollectionLevel(VideoCollectionClient::ELevelVideos - 1);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(!hasSignalsEmitted());
             
    // KMPXMessageIdItemChanged with existing, not handled event type with missing KMPXMessageMediaGeneralId
    mStubCollectionClient->setCollectionLevel(VideoCollectionClient::ELevelVideos);
    TMPXChangeEventType eventType = EMPXItemModified;
    mMediaFactory->putTValue<TMPXChangeEventType>(message, KMPXMessageChangeEventType, eventType);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(!hasSignalsEmitted());
    
    // KMPXMessageIdItemChanged with existing, not handled event type 
    TMPXItemId eventId;
    eventId.iId1 = 10;
    mMediaFactory->putTValue<TMPXItemId>(message, KMPXMessageMediaGeneralId, eventId);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(!hasSignalsEmitted());
    
    // KMPXMessageIdItemChanged with EMPXItemDeleted event type 
    eventType = EMPXItemDeleted;
    mMediaFactory->putTValue<TMPXChangeEventType>(message, KMPXMessageChangeEventType, eventType);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId().iId1 == 10);
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(mSpyVideoDeleted->count() == 1);
    mSpyVideoDeleted->clear();
    QVERIFY(!hasSignalsEmitted());
    
    receiver.resetLatestItems();
    // KMPXMessageIdItemChanged with EMPXItemInserted event type with no correct id2
    eventType = EMPXItemInserted;
    eventId.iId2 = 2;
    mMediaFactory->putTValue<TMPXItemId>(message, KMPXMessageMediaGeneralId, eventId);
    mMediaFactory->putTValue<TMPXChangeEventType>(message, KMPXMessageChangeEventType, eventType);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    // mpx id should have not changed into 10
    QVERIFY(mStubCollectionClient->getLatestMPXId() == 0);
    QVERIFY(!hasSignalsEmitted());
    
    // KMPXMessageIdItemChanged with EMPXItemInserted event type with correct id2 no media object
    eventId.iId2 = 1;
    mMediaFactory->putTValue<TMPXItemId>(message, KMPXMessageMediaGeneralId, eventId);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() == 0);
    QVERIFY(mStubCollectionClient->getLatestMPXId() == eventId);
    QVERIFY(!hasSignalsEmitted());
    
    // TODO: need to check album support
    // KMPXMessageIdItemChanged with EMPXItemInserted event type with correct id2 with media object
    CMPXMedia *media = mMediaFactory->newMedia(11);
    mMediaFactory->putValuePtr<CMPXMedia>(message, KMPXCommandColAddMedia, media);
    mStubCollection->callHandleCollectionMessage(message, 0);
    QVERIFY(receiver.getLatestItemId() == TMPXItemId::InvalidId());
    QVERIFY(receiver.getLatestPointerAddr() != 0);
    QVERIFY(mSpyNewVideoAvailable->count() == 1);
    CMPXMedia *gotten = static_cast<CMPXMedia*>(receiver.getLatestPointerAddr());
    int fetchedId = -1;
    VideoCollectionUtils::instance().mediaValue<int>(gotten, KMPXMediaGeneralId, fetchedId );
    QVERIFY(fetchedId == 11);
    mSpyNewVideoAvailable->clear();
    QVERIFY(!hasSignalsEmitted());
        
    delete media;
    delete message;
    int remHeap = User::Heap().__DbgMarkEnd(0);
    QVERIFY(remHeap == 0);    
}

// -----------------------------------------------------------------------------
// connectSignals
// -----------------------------------------------------------------------------
//
bool TestVideoCollectionListener::connectSignals(StubSignalReceiver *receiver)
{
    if(!QObject::connect(mTestObject, SIGNAL(newVideoList(CMPXMediaArray*)),
            receiver, SLOT(newVideoListSlot(CMPXMediaArray*)),
            Qt::DirectConnection))
    {
        return false;
    }
    
    if(!QObject::connect(mTestObject, SIGNAL(videoListAppended(CMPXMediaArray*)),
                receiver, SLOT(videoListAppendedSlot(CMPXMediaArray*)),
                Qt::DirectConnection))
    {
        return false;
    }
    
    if(!QObject::connect(mTestObject, SIGNAL(newVideoAvailable(CMPXMedia*)),
            receiver, SLOT(newVideoAvailableSlot(CMPXMedia*)),
            Qt::DirectConnection))
    {
        return false;
    }
    
    if(!QObject::connect(mTestObject, SIGNAL( videoListAppended(CMPXMediaArray*)),
                receiver, SLOT(videoListAppendedSlot(CMPXMediaArray*)),
                Qt::DirectConnection))
        {
            return false;
        }
    
    if(!QObject::connect(mTestObject, SIGNAL(videoDeleted(TMPXItemId)),
            receiver, SLOT(videoDeletedSlot(TMPXItemId)),
            Qt::DirectConnection))
    {
        return false;
    }
    
    if(!QObject::connect(mTestObject, SIGNAL(videoDeleteCompleted(int, QList<TMPXItemId>*)),
            receiver, SLOT(videoDeleteCompletedSlot(int, QList<TMPXItemId>*)),
            Qt::DirectConnection))
    {
        return false;
    }
    
    if(!QObject::connect(mTestObject, SIGNAL(videoDetailsCompleted(TMPXItemId)),
            receiver, SLOT(videoDetailsCompletedSlot(TMPXItemId)),
            Qt::DirectConnection))
    {
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// hasSignalsEmitted
// -----------------------------------------------------------------------------
//
bool TestVideoCollectionListener::hasSignalsEmitted()
{
    if(mSpyNewVideoList->count() > 0)
    {
        return true;
    }
    if(mSpyVideoListAppended->count() > 0)
    {
        return true;
    }
    if(mSpyNewVideoAvailable->count() > 0)
    {
        return true;
    }
    if(mSpyVideoDeleted->count() > 0)
    {
        return true;
    }
    if(mSpyVideoDeleteCompleted->count() > 0)
    {
        return true;
    }
    if(mSpyVideoDetailsCompleted->count() > 0)
    {
        return true;
    }
    return false;
}


// End of file
    


