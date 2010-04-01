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
* Description: 
*
*/

// INCLUDES
#include <mpxmediageneraldefs.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <vcxmyvideosdefs.h>
#include <qabstractitemmodel.h>
#include "videocollectionutils.h"
#include "videothumbnaildata.h"
#include "videolistdatamodel.h"
#include "testvideomodel_p.h"
#include "mediaobjectfactory.h"
#include "videocollectioncommon.h"
#include "videocollectionutils.h"

#define private public
#include "videolistdatamodel_p.h"
#undef private

const int MEDIA_COUNT = 10;

inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}


int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    
    TestVideoModel_p tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideomodel_p.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::initTestCase()
{
    mTestObject = 0;
    mMediaFactory = new MediaObjectFactory();
    
    mStubModel = new VideoListDataModel();
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::init()
{
    qRegisterMetaType<QModelIndex>("QModelIndex");
    mTestObject = new VideoListDataModelPrivate(mStubModel);
    QVERIFY(mTestObject);
    VideoCollectionUtils::mPrepareLengthStringReturnValue = "test duration";
    VideoCollectionUtils::mPrepareSizeStringReturnValue = "test size";    
}

// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::cleanup()
{
    if(mTestObject)
    {
        delete mTestObject;
        mTestObject = 0;
    }  
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::cleanupTestCase()
{
    delete mMediaFactory;
    delete mStubModel;
}

// -----------------------------------------------------------------------------
// testInitTNFails
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testInitialize()
{   
    // Succeeds.
    QVERIFY(mTestObject->initialize() == 0);
    
    // Initialize already called.
    QVERIFY(mTestObject->initialize() == 0);
}

// -----------------------------------------------------------------------------
// testItemCount
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testItemCount()
{    
    QVERIFY(mTestObject->getVideoCount() == 0);
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));    
    
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
        
    QVERIFY( mTestObject->getVideoCount() == MEDIA_COUNT);

    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetMediaIdFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetMediaIdFromIndex()
{
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == -1);
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == -1);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == -1);
    
    QVERIFY(mTestObject->getMediaIdFromIndex(MEDIA_COUNT) == -1);
    
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == 0);
    
    QVERIFY(mTestObject->getMediaIdFromIndex(MEDIA_COUNT - 1) == MEDIA_COUNT - 1);
    
    // no data
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataNone);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == -1);
        
    QVERIFY(mTestObject->getMediaIdFromIndex(MEDIA_COUNT) == -1);
    
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == -1);
    
    QVERIFY(mTestObject->getMediaIdFromIndex(MEDIA_COUNT - 1) == - 1);
       
              
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
}

// -----------------------------------------------------------------------------
// testGetVideoNameFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideoNameFromIndex()
{
    QString expectedName = "";
        
    QVERIFY(mTestObject->getVideoNameFromIndex(0) == expectedName);
    QVERIFY(mTestObject->getVideoNameFromIndex(-1) == expectedName);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideoNameFromIndex(-1) == expectedName);
            
    QVERIFY(mTestObject->getVideoNameFromIndex(MEDIA_COUNT) == expectedName);
    
    expectedName = gQTMediaNamePrefix + "0";
    QVERIFY(mTestObject->getVideoNameFromIndex(0) == expectedName);
    
    expectedName = gQTMediaNamePrefix + QString::number(MEDIA_COUNT - 1);
    QVERIFY(mTestObject->getVideoNameFromIndex(MEDIA_COUNT - 1) == expectedName);
    
    // no data
    expectedName = "";
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideoNameFromIndex(-1) == expectedName);
        
    QVERIFY(mTestObject->getVideoNameFromIndex(MEDIA_COUNT) == expectedName);
    
    QVERIFY(mTestObject->getVideoNameFromIndex(0) == expectedName);
    
    QVERIFY(mTestObject->getVideoNameFromIndex(MEDIA_COUNT - 1) == expectedName);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))); 
}

// -----------------------------------------------------------------------------
// testGetVideoThumbnailFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideoThumbnailFromIndex()
{
    // TODO:
    // no thumbnail data
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(0) == 0);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->initialize() == 0);
    
    // invalid indexes
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(-1) == 0);
    
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(MEDIA_COUNT) == 0);
    
    // correct indexes
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(0) != 0);
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(MEDIA_COUNT/2) != 0);
    QVERIFY(mTestObject->getVideoThumbnailFromIndex(MEDIA_COUNT - 1) != 0);
      
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))); 
}

// -----------------------------------------------------------------------------
// testGetVideoSizeFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideoSizeFromIndex()
{
    QVERIFY(mTestObject->getVideoSizeFromIndex(0) == 0);
    QVERIFY(mTestObject->getVideoSizeFromIndex(-1) == 0);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(-1) == 0);
        
    QVERIFY(mTestObject->getVideoSizeFromIndex(MEDIA_COUNT) == 0);
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(0) == 1);
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(MEDIA_COUNT - 1) == MEDIA_COUNT);
    
    // data does not exist
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(-1) == 0);
            
    QVERIFY(mTestObject->getVideoSizeFromIndex(MEDIA_COUNT) == 0);
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(0) == 0);
    
    QVERIFY(mTestObject->getVideoSizeFromIndex(MEDIA_COUNT - 1) == 0);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
}

// -----------------------------------------------------------------------------
// testGetVideoAgeProfileFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideoAgeProfileFromIndex()
{
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(0) == 0);
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(-1) == 0);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
        
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(-1) == 0);
            
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(MEDIA_COUNT) == 0);
    
    quint32 excepted = (0%5) + 1;
    
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(0) == excepted);
    
    excepted = ((MEDIA_COUNT - 1)%5) + 1;
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(MEDIA_COUNT - 1) == excepted);
    
    // data does not exist
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(-1) == 0);
                
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(MEDIA_COUNT) == 0);
    
    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(0) == 0);

    QVERIFY(mTestObject->getVideoAgeProfileFromIndex(MEDIA_COUNT - 1) == 0);
         
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetVideodurationFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideodurationFromIndex()
{
    QVERIFY(mTestObject->getVideodurationFromIndex(0) == 0);
    QVERIFY(mTestObject->getVideodurationFromIndex(-1) == 0);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideodurationFromIndex(-1) == 0);
        
    QVERIFY(mTestObject->getVideodurationFromIndex(MEDIA_COUNT) == 0);
    
    QVERIFY(mTestObject->getVideodurationFromIndex(0) == 1);
    
    QVERIFY(mTestObject->getVideodurationFromIndex(MEDIA_COUNT - 1) == MEDIA_COUNT );
    
    // data does not exist
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getVideodurationFromIndex(-1) == 0);
            
    QVERIFY(mTestObject->getVideodurationFromIndex(MEDIA_COUNT) == 0);
    
    QVERIFY(mTestObject->getVideodurationFromIndex(0) == 0);
    
    QVERIFY(mTestObject->getVideodurationFromIndex(MEDIA_COUNT - 1) == 0);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetVideoDateFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetVideoDateFromIndex()
{
    QDateTime excepted;
    QVERIFY(mTestObject->getVideoDateFromIndex(0) == excepted);
    QVERIFY(mTestObject->getVideoDateFromIndex(-1) == excepted);
  
    // test when data exists
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
      
    QVERIFY(mTestObject->getVideoDateFromIndex(-1) == excepted);
          
    QVERIFY(mTestObject->getVideoDateFromIndex(MEDIA_COUNT) == excepted);
  
    excepted = QDateTime(QDate(2009, 1, 1), QTime(1,1,1,1));
  
    QCOMPARE(mTestObject->getVideoDateFromIndex(0), excepted);
  
    excepted = QDateTime(QDate(2009, MEDIA_COUNT, MEDIA_COUNT), QTime(MEDIA_COUNT,MEDIA_COUNT,MEDIA_COUNT,MEDIA_COUNT));
    
    QCOMPARE(mTestObject->getVideoDateFromIndex(MEDIA_COUNT - 1), excepted);
  
    // data does not exist
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    excepted = QDateTime();
    QVERIFY(mTestObject->getVideoDateFromIndex(-1) == excepted);         
    QVERIFY(mTestObject->getVideoDateFromIndex(MEDIA_COUNT) == excepted);
    QVERIFY(mTestObject->getVideoDateFromIndex(0) == excepted);
    QVERIFY(mTestObject->getVideoDateFromIndex(MEDIA_COUNT - 1) == excepted);
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetMetaDataFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetMetaDataFromIndex()
{
    using namespace VideoCollectionCommon;
    QMap<QString, QVariant> map = mTestObject->getMetaDataFromIndex(0);
    QVERIFY(map.isEmpty());
    map = mTestObject->getMetaDataFromIndex(-1);
    QVERIFY(map.isEmpty());
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
        
    map = mTestObject->getMetaDataFromIndex(-1);
    QVERIFY(map.isEmpty());
    
    map = mTestObject->getMetaDataFromIndex(-1);
    QVERIFY(map.isEmpty());
    
    map = mTestObject->getMetaDataFromIndex(0);
    
    QVERIFY(map.contains(MetaKeyDate));
    QVERIFY(map.contains(MetaKeyDurationString));
    QVERIFY(map.contains(MetaKeySizeString));
    QVERIFY(map.contains(MetaKeyStarRating));
    QVERIFY(map.contains(MetaKeyDescription));
    QVERIFY(map.contains(MetaKeyModifiedDate));
    QVERIFY(map.contains(MetaKeyAuthor));
    QVERIFY(map.contains(MetaKeyCopyright));
    QVERIFY(map.contains(MetaKeyLanguageString));
    QVERIFY(map.contains(MetaKeyFormat));
    
    // one or several of these will fail, when rest of the metadata is implemented.
    QVERIFY(map.contains(MetaKeyDRMInfo) == false);
    QVERIFY(map.contains(MetaKeyServiceURL) == false);
    QVERIFY(map.contains(MetaKeyShotLocation) == false);
    QVERIFY(map.contains(MetaKeyAudioType) == false);
    QVERIFY(map.contains(MetaKeyKeywords) == false);
    QVERIFY(map.contains(MetaKeyVideoResolutionString) == false);
    QVERIFY(map.contains(MetaKeyBitRate) == false);
    
    QCOMPARE(map[MetaKeyDate].toDate(), QDate(2009, 1, 1));
    QVERIFY(map[MetaKeyDurationString].toString().isEmpty() == false);
    QVERIFY(map[MetaKeySizeString].toString().isEmpty() == false);
    QCOMPARE(map[MetaKeyStarRating].toUInt(), (uint)1);
    QCOMPARE(map[MetaKeyDescription].toString(), gQTMediaDescPrefix + "0");
    QCOMPARE(map[MetaKeyModifiedDate].toDate(), QDate::currentDate().addDays(2));
    QCOMPARE(map[MetaKeyAuthor].toString(), gQTMediaAuthorPrefix + "0");
    QCOMPARE(map[MetaKeyCopyright].toString(), gQTMediaCopyrightPrefix + "0");
    QCOMPARE(map[MetaKeyLanguageString].toString(), gQTMediaLanguagePrefix + "0");
    QCOMPARE(map[MetaKeyFormat].toString(), gQTMediaFormatPrefix + "0");
    
    // data does not exist
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    VideoCollectionUtils::mPrepareLengthStringReturnValue = "";
    VideoCollectionUtils::mPrepareSizeStringReturnValue = "";
    
    map = mTestObject->getMetaDataFromIndex(-1);
    QVERIFY(map.isEmpty());
    
    map = mTestObject->getMetaDataFromIndex(MEDIA_COUNT);
    QVERIFY(map.isEmpty());
    
    map = mTestObject->getMetaDataFromIndex(0);
    QCOMPARE(map.count(), 0); 
    
    map = mTestObject->getMetaDataFromIndex(MEDIA_COUNT - 1);
    QCOMPARE(map.count(), 0);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetFilePathFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetFilePathFromIndex()
{
    QString path = mTestObject->getFilePathFromIndex(0);
    QVERIFY(path.isNull());
    path = mTestObject->getFilePathFromIndex(-1);
    QVERIFY(path.isNull());
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    path = mTestObject->getFilePathFromIndex(-1);
    QVERIFY(path.isNull());
    
    path = mTestObject->getFilePathFromIndex(0);
    QCOMPARE(path, gQTMediaPathPrefix + "0");
    
    // data does not exist
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    path = mTestObject->getFilePathFromIndex(0);
    QVERIFY(path.isNull());
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetFilePathForId
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetFilePathForId()
{
    QString path = mTestObject->getFilePathForId(TMPXItemId(0,0));
    QVERIFY(path.isNull());
    path = mTestObject->getFilePathForId(TMPXItemId::InvalidId());
    QVERIFY(path.isNull());
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    path = mTestObject->getFilePathForId(TMPXItemId::InvalidId());
    QVERIFY(path.isNull());
    
    path = mTestObject->getFilePathForId(TMPXItemId(0,0));
    QCOMPARE(path, gQTMediaPathPrefix + "0");
    
    // data does not exist
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    path = mTestObject->getFilePathForId(TMPXItemId(0,0));
    QVERIFY(path.isNull());
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}
// -----------------------------------------------------------------------------
// testVideoStatus
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testVideoStatus()
{
    QModelIndexList indexList;

    indexList.append(mStubModel->index(0,0));
    // no videos
    mTestObject->markVideosRemoved(indexList);
    QVERIFY(mTestObject->getVideoStatusFromIndex(0) == 0);
    QVERIFY(mTestObject->getVideoStatusFromIndex(-1) == 0);
    
    mMediaFactory->removeArray();
    indexList.clear();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
        
    // videos, invalid + correct indexes
    QList<TMPXItemId> markedIds;
    indexList.append(mStubModel->index(-1,0));
    indexList.append(mStubModel->index(0,0));
    indexList.append(mStubModel->index(MEDIA_COUNT/2,10));
    indexList.append(mStubModel->index(MEDIA_COUNT,0));
    markedIds = mTestObject->markVideosRemoved(indexList);
    QVERIFY(markedIds.count() == 2);
    QVERIFY(mTestObject->getVideoStatusFromIndex(-1) == 0);
    QVERIFY(mTestObject->getVideoStatusFromIndex(MEDIA_COUNT) == 0);
    QVERIFY(mTestObject->getVideoStatusFromIndex(0) == VideoCollectionCommon::StatusDeleted);
    QVERIFY(mTestObject->getVideoStatusFromIndex(MEDIA_COUNT/2) == VideoCollectionCommon::StatusDeleted);
    QVERIFY(mTestObject->getVideoStatusFromIndex(MEDIA_COUNT-1) == 0);

    // clear list (and items as well
    indexList.clear();
    QVERIFY(connect(this, SIGNAL(signalDeleteVideo(TMPXItemId)), mTestObject, SLOT(videoDeletedSlot(TMPXItemId))));    
    emit signalDeleteVideo(TMPXItemId(0, 0));
    emit signalDeleteVideo(TMPXItemId(MEDIA_COUNT/2, 0));
    emit signalDeleteVideo(TMPXItemId(MEDIA_COUNT-1, 0));
    emit signalDeleteVideo(TMPXItemId(MEDIA_COUNT, 0));
    QVERIFY(mTestObject->getVideoCount() == (MEDIA_COUNT - 3));
        
    // videos, invalid media ids
    indexList.append(mStubModel->index(-1,0));
    indexList.append(mStubModel->index(MEDIA_COUNT+1,0));
    mTestObject->markVideosRemoved(indexList);
    for(int i = 0; i < (MEDIA_COUNT - 2); ++i)
    {
        QVERIFY(mTestObject->getVideoStatusFromIndex(i) == 0);
    }
    QVERIFY(mTestObject->getVideoStatusFromIndex(-1) == 0);
    QVERIFY(mTestObject->getVideoStatusFromIndex(MEDIA_COUNT) == 0);
    
    // correct case after removal
    indexList.append(mStubModel->index(1,0));
    mTestObject->markVideosRemoved(indexList);
    QVERIFY(mTestObject->getVideoStatusFromIndex(1) == VideoCollectionCommon::StatusDeleted);
    
    // correct case, unmarked    
    indexList.clear();
    indexList.append(mStubModel->index(1,0));
    indexList.append(mStubModel->index(2,0));
    indexList.append(mStubModel->index(3,0));
    markedIds = mTestObject->markVideosRemoved(indexList);
    QVERIFY(markedIds.count() == 3);
    mTestObject->unMarkVideosRemoved(markedIds);
    QVERIFY(mTestObject->getVideoStatusFromIndex(1) == 0);
    QVERIFY(mTestObject->getVideoStatusFromIndex(2) == 0);
    QVERIFY(mTestObject->getVideoStatusFromIndex(3) == 0);
    
    disconnect(this, SIGNAL(signalDeleteVideo(TMPXItemId)), mTestObject, SLOT(videoDeletedSlot(TMPXItemId)));
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testThumbnailsFetchedSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testThumbnailsFetchedSlot()
{
    QVERIFY(connect(this, SIGNAL(signalThumbnailsFetched(QList<TMPXItemId>)), mTestObject, SLOT(thumbnailsFetchedSlot(QList<TMPXItemId>))));      
    
    QSignalSpy spysignal(mTestObject, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
    mMediaFactory->removeArray();
    
    QList<TMPXItemId> mediaIds;
    
    // test no videos
    mediaIds.append(TMPXItemId(0, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 0);
    mediaIds.clear();

    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));           
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    // test invalid ids
    mediaIds.append(TMPXItemId::InvalidId());
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 0);
    spysignal.clear();
    mediaIds.clear();
    mediaIds.append(TMPXItemId(MEDIA_COUNT, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 0);
    mediaIds.clear();
    mediaIds.append(TMPXItemId(MEDIA_COUNT + 1, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 0);
    mediaIds.clear();
    
    // test correct ids
    mediaIds.append(TMPXItemId(0, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == 0);
    spysignal.clear();
    mediaIds.clear();
    mediaIds.append(TMPXItemId(1, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 1);
    spysignal.clear();
    mediaIds.clear();
    
    mediaIds.append(TMPXItemId(MEDIA_COUNT/2, 0));
    emit signalThumbnailsFetched(mediaIds);
    QVERIFY(spysignal.count() == 1);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
    disconnect(this, SIGNAL(signalThumbnailsFetched(QList<TMPXItemId>)), mTestObject, SLOT(thumbnailsFetchedSlot(QList<TMPXItemId>)));
}

// -----------------------------------------------------------------------------
// testNewVideoListSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testNewVideoListSlot()
{
    QVERIFY(mTestObject->initialize() == 0);
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));      

    QSignalSpy spyRowsAboutToInsert(mStubModel, SIGNAL(rowsAboutToBeInserted (const QModelIndex& , int, int))); 
    QSignalSpy spyRowsInserted(mStubModel, SIGNAL(rowsInserted (const QModelIndex& , int, int))); 
    QSignalSpy spyRowsAboutToRemove(mStubModel, SIGNAL(rowsAboutToBeRemoved (const QModelIndex& , int, int))); 
    QSignalSpy spyRowsRemoved(mStubModel, SIGNAL(rowsRemoved (const QModelIndex& , int, int)));
    QSignalSpy spyModelReady(mStubModel, SIGNAL(modelReady()));
    
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    // media-array does not exist
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());

    QVERIFY(spyRowsAboutToInsert.count() == 1);
    QVERIFY(spyRowsInserted.count() == 1);
    QVERIFY(spyRowsAboutToRemove.count() == 0);
    QVERIFY(spyRowsRemoved.count() == 0);    
    QVERIFY(spyRowsInserted.at(0).at(1).toInt() == 0);
    QVERIFY(spyRowsInserted.at(0).at(2).toInt() == MEDIA_COUNT - 1);
    QCOMPARE(spyModelReady.count(), 1);
    spyRowsAboutToInsert.clear();
    spyRowsInserted.clear();
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();
    spyModelReady.clear();
    
    QVERIFY(mTestObject->initialize() == 0);
    
    // providing null videolist
    emit signalNewVideoList(0);
    QVERIFY(spyRowsAboutToInsert.count() == 0);
    QVERIFY(spyRowsInserted.count() == 0);
    QVERIFY(spyRowsAboutToRemove.count() == 0);
    QVERIFY(spyRowsRemoved.count() == 0);
    QCOMPARE(spyModelReady.count(), 0);

    
    // empty media-array
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    QVERIFY(spyRowsAboutToInsert.count() == 1);
    QVERIFY(spyRowsInserted.count() == 1);
    QVERIFY(spyRowsAboutToRemove.count() == 1);
    QVERIFY(spyRowsRemoved.count() == 1);    
    QVERIFY(spyRowsInserted.at(0).at(1).toInt() == 0);
    QVERIFY(spyRowsInserted.at(0).at(2).toInt() == 0);
    QVERIFY(spyRowsRemoved.at(0).at(1).toInt() == 0);
    QVERIFY(spyRowsRemoved.at(0).at(2).toInt() == MEDIA_COUNT - 1);
    QCOMPARE(spyModelReady.count(), 1);
    spyRowsAboutToInsert.clear();
    spyRowsInserted.clear();
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();
    spyModelReady.clear();
    
    // media-array exits
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    QVERIFY(spyRowsAboutToInsert.count() == 1);
    QVERIFY(spyRowsInserted.count() == 1);
    QVERIFY(spyRowsAboutToRemove.count() == 0);
    QVERIFY(spyRowsRemoved.count() == 0);    
    QVERIFY(spyRowsInserted.at(0).at(1).toInt() == 0);
    QVERIFY(spyRowsInserted.at(0).at(2).toInt() == MEDIA_COUNT - 1);
    QCOMPARE(spyModelReady.count(), 1);
    spyRowsAboutToInsert.clear();
    spyRowsInserted.clear();
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();
    spyModelReady.clear();
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
}

// -----------------------------------------------------------------------------
// testAppendVideoListSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testAppendVideoListSlot()
{
    QVERIFY(mTestObject->initialize() == 0);
        
    QVERIFY(connect(this, SIGNAL(signalAppendVideoList(CMPXMediaArray*)), mTestObject, SLOT(appendVideoListSlot(CMPXMediaArray*))));      
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    
    QSignalSpy spyRowsAboutToInsert(mStubModel, SIGNAL(rowsAboutToBeInserted (const QModelIndex& , int, int))); 
    QSignalSpy spyRowsInserted(mStubModel, SIGNAL(rowsInserted (const QModelIndex& , int, int))); 
    
    // append null
    emit signalAppendVideoList(0);
    QVERIFY(spyRowsAboutToInsert.count() == 0);
    QVERIFY(spyRowsInserted.count() == 0);
    QVERIFY(mTestObject->getVideoCount() == 0);
    
    // append empty videolist
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(spyRowsAboutToInsert.count() == 0);
    QVERIFY(spyRowsInserted.count() == 0);
    QVERIFY(mTestObject->getVideoCount() == 0);
    
    // send 10 videos
    mMediaFactory->createMediaItems(10);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    spyRowsAboutToInsert.clear();
    spyRowsInserted.clear();
    
    // append videolist containing less than existing
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(3);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(spyRowsAboutToInsert.count() == 0);
    QVERIFY(spyRowsInserted.count() == 0);
    QVERIFY(mTestObject->getVideoCount() == 10);
    
    // reset data
    mTestObject->mMediaData.clear();
    
    // append items with invalid ids:
    // signals emitted, but items are really not appended
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(3, MediaDataNone);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(spyRowsAboutToInsert.count() == 1);
    QVERIFY(spyRowsInserted.count() == 1);
    QVERIFY(mTestObject->getVideoCount() == 0);
    spyRowsAboutToInsert.clear();
    spyRowsInserted.clear();  

    // append items with iId2 == 1 && iId1 != KVcxMvcCategoryIdDownloads (==1) && iId1 != KVcxMvcCategoryIdCaptured (== 3)
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(4, 1)));
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(5, 1)));
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(spyRowsAboutToInsert.count() == 1);
    QVERIFY(spyRowsInserted.count() == 1);
    QVERIFY(spyRowsInserted.at(0).at(1).toInt() == 0);
    QVERIFY(spyRowsInserted.at(0).at(2).toInt() == 1);
    QVERIFY(mTestObject->getVideoCount() == 0);
    spyRowsAboutToInsert.clear();
    spyRowsInserted.clear();
    
    // append correct items
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(3);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(spyRowsAboutToInsert.count() == 1);
    QVERIFY(spyRowsInserted.count() == 1);
    QVERIFY(spyRowsInserted.at(0).at(1).toInt() == 0);
    QVERIFY(spyRowsInserted.at(0).at(2).toInt() == 2);
    QVERIFY(mTestObject->getVideoCount() == 3);
    spyRowsAboutToInsert.clear();
    spyRowsInserted.clear();    
       
    // append more to existing
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(10);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(spyRowsAboutToInsert.count() == 1);
    QVERIFY(spyRowsInserted.count() == 1);
    QVERIFY(spyRowsInserted.at(0).at(1).toInt() == 3);
    QVERIFY(spyRowsInserted.at(0).at(2).toInt() == 9);
    QVERIFY(mTestObject->getVideoCount() == 10);
    spyRowsAboutToInsert.clear();
    spyRowsInserted.clear();   
      
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
    disconnect(this, SIGNAL(signalAppendVideoList(CMPXMediaArray*)), mTestObject, SLOT(appendVideoListSlot(CMPXMediaArray*)));  
}

// -----------------------------------------------------------------------------
// testNewVideoAvailableSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testNewVideoAvailableSlot()
{
    QVERIFY(mTestObject->initialize() == 0);
    
    QSignalSpy spyRowsAboutToInsert(mStubModel, SIGNAL(rowsAboutToBeInserted (const QModelIndex& , int, int))); 
    QSignalSpy spyRowsInserted(mStubModel, SIGNAL(rowsInserted (const QModelIndex& , int, int)));     
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalNewVideoAvailable(CMPXMedia*)), mTestObject, SLOT(newVideoAvailableSlot(CMPXMedia*))));  
    
    CMPXMedia *media = mMediaFactory->getMedia(MEDIA_COUNT + 100);
    // no media array
    emit signalNewVideoAvailable(media);
    QVERIFY(spyRowsAboutToInsert.count() == 1);
    QVERIFY(spyRowsInserted.count() == 1);
    QVERIFY(spyRowsAboutToInsert.at(0).at(1).toInt() == 0);
    QVERIFY(spyRowsAboutToInsert.at(0).at(2).toInt() == 0);
    
    QVERIFY(mTestObject->getVideoCount() == 1);
    delete media;
    media = 0;
    spyRowsAboutToInsert.clear();
    spyRowsInserted.clear();
    
    // null pointer provided
    emit signalNewVideoAvailable(media);
    QVERIFY(spyRowsAboutToInsert.count() == 0);
    QVERIFY(spyRowsInserted.count() == 0);
    QVERIFY(mTestObject->getVideoCount() == 1);
    
    // invalid item
    media = mMediaFactory->getMedia(MEDIA_COUNT + 100, MediaDataNone);
    emit signalNewVideoAvailable(media);
    QVERIFY(spyRowsAboutToInsert.count() == 0);
    QVERIFY(spyRowsInserted.count() == 0);
    QVERIFY(mTestObject->getVideoCount() == 1);
    delete media;
    media = 0;
    
    delete mTestObject;
    mTestObject = new VideoListDataModelPrivate(mStubModel);
    QVERIFY(mTestObject->initialize() == 0);
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalNewVideoAvailable(CMPXMedia*)), mTestObject, SLOT(newVideoAvailableSlot(CMPXMedia*))));

    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    spyRowsAboutToInsert.clear();
    spyRowsInserted.clear();
    
    // succeed case
    CMPXMedia *media3 = mMediaFactory->getMedia(MEDIA_COUNT + 103 );
    emit signalNewVideoAvailable(media3);
    QVERIFY(spyRowsAboutToInsert.count() == 1);
    QVERIFY(spyRowsInserted.count() == 1);
    QVERIFY(spyRowsAboutToInsert.at(0).at(1).toInt() == mTestObject->getVideoCount() - 1);
    QVERIFY(spyRowsAboutToInsert.at(0).at(2).toInt() == mTestObject->getVideoCount() - 1);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT + 1);
    
    disconnect(this, SIGNAL(signalNewVideoAvailable(CMPXMedia*)), mTestObject, SLOT(newVideoAvailableSlot(CMPXMedia*)));
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))); 
}

// -----------------------------------------------------------------------------
// testVideoDeletedSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testVideoDeletedSlot()
{
    mMediaFactory->removeArray();
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalDeleteVideo(TMPXItemId)), mTestObject, SLOT(videoDeletedSlot(TMPXItemId))));  
    QSignalSpy spyRowsAboutToRemove(mStubModel, SIGNAL(rowsAboutToBeRemoved (const QModelIndex& , int, int))); 
    QSignalSpy spyRowsRemoved(mStubModel, SIGNAL(rowsRemoved (const QModelIndex& , int, int))); 
   
    // no videos
    emit signalDeleteVideo(TMPXItemId(0,0));
    QVERIFY(spyRowsAboutToRemove.count() == 0);
    QVERIFY(spyRowsRemoved.count() == 0);
    
    QVERIFY(mTestObject->initialize() == 0);
    emit signalDeleteVideo(TMPXItemId(0,0));
    QVERIFY(spyRowsAboutToRemove.count() == 0);
    QVERIFY(spyRowsRemoved.count() == 0);
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();
    
    // external delete - item not in delete -list
    emit signalDeleteVideo(TMPXItemId::InvalidId());
    QVERIFY(spyRowsAboutToRemove.count() == 0);
    QVERIFY(spyRowsRemoved.count() == 0);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT);

    
    VideoThumbnailData::resetRemoveCount();
    emit signalDeleteVideo(TMPXItemId(0,0));
    QVERIFY(spyRowsAboutToRemove.count() == 1);
    QVERIFY(spyRowsRemoved.count() == 1);
    QVERIFY(spyRowsRemoved.at(0).at(1).toInt() == 0);
    QVERIFY(spyRowsRemoved.at(0).at(2).toInt() == 0);
    
    QVERIFY(VideoThumbnailData::getRemoveCount() == 1);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT -1);
    
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();
    
    emit signalDeleteVideo(TMPXItemId(MEDIA_COUNT-1, 0));
    QVERIFY(spyRowsAboutToRemove.count() == 1);
    QVERIFY(spyRowsRemoved.count() == 1);
    QVERIFY(spyRowsRemoved.at(0).at(1).toInt() == MEDIA_COUNT-2);
    QVERIFY(spyRowsRemoved.at(0).at(2).toInt() == MEDIA_COUNT-2);
    
    QVERIFY(VideoThumbnailData::getRemoveCount() == 2);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT -2);
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();
    
    // reset medias
    VideoThumbnailData::resetRemoveCount();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();
    
    QModelIndexList indexList;
    // internal delete - item in delete -list 
    indexList.append(mStubModel->index(-1,0));
    mTestObject->markVideosRemoved(indexList);
    emit signalDeleteVideo(TMPXItemId::InvalidId());
    QVERIFY(spyRowsRemoved.count() == 0);
    QVERIFY(spyRowsRemoved.count() == 0);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT);
   
    indexList.append(mStubModel->index(MEDIA_COUNT,0));
    mTestObject->markVideosRemoved(indexList);
    emit signalDeleteVideo(TMPXItemId::InvalidId());
    QVERIFY(spyRowsRemoved.count() == 0);
    QVERIFY(spyRowsRemoved.count() == 0);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT);
   
    indexList.append(mStubModel->index(0,0));
    mTestObject->markVideosRemoved(indexList);
    emit signalDeleteVideo(TMPXItemId(0,0));
    QVERIFY(spyRowsRemoved.count() == 1);
    QVERIFY(spyRowsRemoved.count() == 1);
    QVERIFY(spyRowsRemoved.at(0).at(1).toInt() == 0);
    QVERIFY(spyRowsRemoved.at(0).at(2).toInt() == 0);
    QVERIFY(VideoThumbnailData::getRemoveCount() == 1);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT -1);
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();
   
    indexList.append(mStubModel->index(MEDIA_COUNT-1,0));
    mTestObject->markVideosRemoved(indexList);
    emit signalDeleteVideo(TMPXItemId(MEDIA_COUNT-1, 0));
    QVERIFY(spyRowsRemoved.count() == 1);
    QVERIFY(spyRowsRemoved.count() == 1);
    QVERIFY(spyRowsRemoved.at(0).at(1).toInt() == MEDIA_COUNT-2);
    QVERIFY(spyRowsRemoved.at(0).at(2).toInt() == MEDIA_COUNT-2);
    QVERIFY(VideoThumbnailData::getRemoveCount() == 2);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT -2);
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();
    
    // reset medias
    VideoThumbnailData::resetRemoveCount();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();
    
    // gotten index same as video count ( only possible in internal delete )
    mTestObject->mItemsUnderDeletion.insert(TMPXItemId(MEDIA_COUNT,0));
    
    emit signalDeleteVideo(TMPXItemId(MEDIA_COUNT, 0));
    QVERIFY(spyRowsRemoved.count() == 0);
    QVERIFY(spyRowsRemoved.count() == 0);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT);
    
    // gotten index larger than video count ( only possible in internal delete )
    mTestObject->mItemsUnderDeletion.insert(TMPXItemId(MEDIA_COUNT + 1, 0));

    emit signalDeleteVideo(TMPXItemId(MEDIA_COUNT + 1,0));
    QVERIFY(spyRowsRemoved.count() == 0);
    QVERIFY(spyRowsRemoved.count() == 0);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT);


    emit signalDeleteVideo(TMPXItemId(1,0));
    QVERIFY(spyRowsRemoved.count() == 1);
    QVERIFY(spyRowsRemoved.count() == 1);
    QVERIFY(spyRowsRemoved.at(0).at(1).toInt() == 1);
    QVERIFY(spyRowsRemoved.at(0).at(2).toInt() == 1);
    QVERIFY(VideoThumbnailData::getRemoveCount() == 1);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT - 1);
    spyRowsAboutToRemove.clear();
    spyRowsRemoved.clear();

    disconnect(this, SIGNAL(signalDeleteVideo(TMPXItemId)), mTestObject, SLOT(videoDeletedSlot(TMPXItemId)));
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testVideoDeleteCompletedSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testVideoDeleteCompletedSlot()
{
    mMediaFactory->removeArray();
     
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalDeleteCompleted(int, QList<TMPXItemId>*)), mTestObject, SLOT(videoDeleteCompletedSlot(int, QList<TMPXItemId>*))));  
    QSignalSpy spysignal(mTestObject, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));
    
    // null list provided
    emit signalDeleteCompleted(0, 0);
    QVERIFY(spysignal.count() == 0);
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    QModelIndexList indexList;
    indexList.append(mStubModel->index(0,0));
    indexList.append(mStubModel->index(MEDIA_COUNT/2,0));
    indexList.append(mStubModel->index(MEDIA_COUNT-1,0));
    mTestObject->markVideosRemoved(indexList);

    emit signalDeleteCompleted(0, 0);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == -1);
    QVERIFY(!VideoCollectionWrapper::mLatestAdditional.isValid());
    

    // provided list contains nothing and we have none deleted
    QList<TMPXItemId> testIds;
    emit signalDeleteCompleted(0, &testIds);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == -1);
    QVERIFY(!VideoCollectionWrapper::mLatestAdditional.isValid());
    
    // provided list contains nothing and all provided count is more than one
    // (No status code sent from completely succeed delete)
    emit signalDeleteCompleted(5, &testIds);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == -1);
    QVERIFY(!VideoCollectionWrapper::mLatestAdditional.isValid());
    
    VideoCollectionWrapper::mLatestStatusCode = -1;
    VideoCollectionWrapper::mLatestAdditional = QVariant();
    
    // provided list does not contain ids that are marked
    // (No status code sent from completely succeed delete)
    testIds.append(TMPXItemId(1, 0));
    testIds.append(TMPXItemId(MEDIA_COUNT - 2,0));
    emit signalDeleteCompleted(2, &testIds);
    QVERIFY(spysignal.count() == 0);      
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == -1);
    QVERIFY(!VideoCollectionWrapper::mLatestAdditional.isValid());
    
    VideoCollectionWrapper::mLatestStatusCode = -1;
    VideoCollectionWrapper::mLatestAdditional = QVariant();
    
    // provided list contains partly items that are marked
    testIds.append(TMPXItemId(MEDIA_COUNT/2, 0));
    emit signalDeleteCompleted(3, &testIds);
    QVERIFY(spysignal.count() == 1);
    spysignal.clear();
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == VideoCollectionCommon::statusSingleDeleteFail);
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.isValid());
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.toString().count() > 0);
    
    VideoCollectionWrapper::mLatestStatusCode = -1;
    VideoCollectionWrapper::mLatestAdditional = QVariant();
    
    // provided list contains all marked items
    testIds.clear();
    testIds.append(TMPXItemId(0,0));
    testIds.append(TMPXItemId(MEDIA_COUNT - 1, 0));
    emit signalDeleteCompleted(MEDIA_COUNT, &testIds);
    QVERIFY(spysignal.count() == 2);
    QVERIFY(VideoCollectionWrapper::mLatestStatusCode == VideoCollectionCommon::statusMultipleDeleteFail);
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.isValid());
    QVERIFY(VideoCollectionWrapper::mLatestAdditional.toInt() == testIds.count());
    
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
    disconnect(this, SIGNAL(signalDeleteCompleted(int, QList<TMPXItemId>*)), mTestObject, SLOT(videoDeleteCompletedSlot(int, QList<TMPXItemId>*)));
}

// -----------------------------------------------------------------------------
// testVideoDetailsCompletedSlot
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testVideoDetailsCompletedSlot()
{
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalVideoDetailsCompleted(TMPXItemId)), mTestObject, SLOT(videoDetailsCompletedSlot(TMPXItemId))));      
    
    QSignalSpy spysignal(mTestObject, SIGNAL(videoDetailsReady(int)));

    // no videos
    emit signalVideoDetailsCompleted(TMPXItemId(0,0));
    QVERIFY(spysignal.count() == 0);        
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    // invalid id
    emit signalVideoDetailsCompleted(TMPXItemId::InvalidId());
    QVERIFY(spysignal.count() == 0); 
    emit signalVideoDetailsCompleted(MEDIA_COUNT);
    QVERIFY(spysignal.count() == 0); 
    
    // correct ids
    emit signalVideoDetailsCompleted(TMPXItemId(0,0));
    QVERIFY(spysignal.count() == 1); 
    QVERIFY(spysignal.value(0).at(0).toInt() == 0);
    spysignal.clear();
    emit signalVideoDetailsCompleted(TMPXItemId(MEDIA_COUNT - 1,0));
    QVERIFY(spysignal.count() == 1); 
    QVERIFY(spysignal.value(0).at(0).toInt() == MEDIA_COUNT - 1);
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
    disconnect(this, SIGNAL(signalVideoDetailsCompleted(TMPXItemId)), mTestObject, SLOT(videoDetailsCompletedSlot(TMPXItemId)));
}

// -----------------------------------------------------------------------------
// testCollectionItemCount
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testCollectionItemCount()
{    
    QVERIFY(mTestObject->getVideoCount() == 0);
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));    
    
    mMediaFactory->removeArray();
    mMediaFactory->createCollectionItems();
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
        
    QVERIFY( mTestObject->getVideoCount() == 2);

    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
}

// -----------------------------------------------------------------------------
// testGetCollectionIdFromIndex
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testGetCollectionIdFromIndex()
{
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == -1);
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == -1);
    
    mMediaFactory->removeArray();
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));          
    mMediaFactory->createCollectionItems();
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == TMPXItemId::InvalidId());
    
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == TMPXItemId(KVcxMvcCategoryIdDownloads, 1));
    
    QVERIFY(mTestObject->getMediaIdFromIndex(1) == TMPXItemId(KVcxMvcCategoryIdCaptured, 1));
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
}

// End of file
    


