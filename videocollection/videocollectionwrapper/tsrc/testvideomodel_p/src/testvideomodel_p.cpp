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
#include "testvideomodel_p.h"

#include "videocollectionutils.h"
#include "videothumbnaildata.h"
#include "videolistdatamodel.h"

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
        pass[2] = "c:\\data\\testvideomodel_p.txt";
        res = QTest::qExec(&tv, 3, pass);
    }    

    return res;
}

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//

void TestVideoModel_p::initTestCase()
{
    qRegisterMetaType<TMPXItemId>("TMPXItemId");
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
    mTestObject->mMediaData.clear();
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
    mTestObject->mMediaData.clear();
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
    mTestObject->mMediaData.clear();
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
    mTestObject->mMediaData.clear();
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
    mTestObject->mMediaData.clear();
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
    mTestObject->mMediaData.clear();
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
    mTestObject->mMediaData.clear();
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
// testVideoMarkAndRestoreDeleted
// -----------------------------------------------------------------------------
//
void TestVideoModel_p::testVideoMarkAndRestoreDeleted()
{
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));   
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    QList<TMPXItemId> idsToRestore;
    // mark few items to be removed then restore them 
    // NOTE: since item marking decreases item count by one,
    //       we need to mark items starting from biggest index,
    //       otherwise we might end up to mark invalid items
    QModelIndex index = mStubModel->index(MEDIA_COUNT-1, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));    
    index = mStubModel->index(MEDIA_COUNT / 2, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));
    index = mStubModel->index(0, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));
    
    QVERIFY(mTestObject->getVideoCount() == (MEDIA_COUNT - 3) );
    
    mTestObject->restoreRemoved(&idsToRestore);
    QVERIFY(VideoListDataModel::mFirstInserted == MEDIA_COUNT - 3);
    QVERIFY(VideoListDataModel::mLastInserted == MEDIA_COUNT - 1);
    QVERIFY(mTestObject->getVideoCount() == MEDIA_COUNT);
    
    idsToRestore.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    
    // mark few items to be removed then restore them 
    index = mStubModel->index(MEDIA_COUNT-1, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));    
    index = mStubModel->index(MEDIA_COUNT / 2, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));
    index = mStubModel->index(0, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));
 
    QVERIFY(mTestObject->getVideoCount() == (MEDIA_COUNT - 3) );
    mTestObject->restoreRemoved(&idsToRestore);
    QVERIFY(VideoListDataModel::mFirstInserted == MEDIA_COUNT - 3);
    QVERIFY(VideoListDataModel::mLastInserted == MEDIA_COUNT - 1);
    
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    idsToRestore.clear();
    index = mStubModel->index(MEDIA_COUNT, 0, QModelIndex());
    idsToRestore.append(mTestObject->markVideoRemoved(index));    
    mTestObject->restoreRemoved(&idsToRestore);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    
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
    mTestObject->mMediaData.clear();
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
    mTestObject->mMediaData.clear();
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT, MediaDataId);    
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    path = mTestObject->getFilePathForId(TMPXItemId(0,0));
    QVERIFY(path.isNull());
    
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

    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;    
    
    QSignalSpy spyModelReady(mStubModel, SIGNAL(modelReady()));
    
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    // media-array does not exist
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());

    QVERIFY(VideoListDataModel::mFirstInserted == 0);
    QVERIFY(VideoListDataModel::mLastInserted == MEDIA_COUNT - 1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mLastRemoved == -1); 
    QCOMPARE(spyModelReady.count(), 1);
    spyModelReady.clear();
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;   
    
    QVERIFY(mTestObject->initialize() == 0);
    
    // providing null videolist
    emit signalNewVideoList(0);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mLastRemoved == -1); 
    QCOMPARE(spyModelReady.count(), 0);
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;  
    
    // empty media-array
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
  
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mLastRemoved == -1); 
    QCOMPARE(spyModelReady.count(), 1);
    spyModelReady.clear();
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;   
    
    // media-array exits
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mLastRemoved == -1); 
    QCOMPARE(spyModelReady.count(), 1);
    spyModelReady.clear();
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;   
    
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
    
    QSignalSpy spyModelChanged(mStubModel, SIGNAL(modelChanged()));
    
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    
    // append null
   emit signalAppendVideoList(0);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 0);
    QVERIFY(spyModelChanged.count() == 0);
    
    // append empty videolist
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 0);
    QVERIFY(spyModelChanged.count() == 0);
    
    // send 10 videos
    mMediaFactory->createMediaItems(10);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    
    // append videolist containing less than existing
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(3);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 10);
    QVERIFY(spyModelChanged.count() == 0);
    
    // reset data
    mTestObject->mMediaData.clear();
    
    // append items with invalid ids:
    // signals emitted (begin inserting is called), but items are really not appended
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(3, MediaDataNone);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray());
    QVERIFY(VideoListDataModel::mFirstInserted == 0);
    QVERIFY(VideoListDataModel::mLastInserted == 2);
    QVERIFY(mTestObject->getVideoCount() == 0);
    QVERIFY(spyModelChanged.count() == 1); 
    spyModelChanged.clear();

    // append items with iId2 == 1 && iId1 != KVcxMvcCategoryIdDownloads (==1) && iId1 != KVcxMvcCategoryIdCaptured (== 3)
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(0);
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(4, 1)));
    TRAP_IGNORE(mMediaFactory->mediaArray()->AppendL(mMediaFactory->getMediaWithWantedIds(5, 1)));
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 

    QVERIFY(VideoListDataModel::mFirstInserted == 0);
    QVERIFY(VideoListDataModel::mLastInserted == 1);
    QVERIFY(mTestObject->getVideoCount() == 0);
    QVERIFY(spyModelChanged.count() == 1); 
    spyModelChanged.clear();
    
    // append correct items
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(3);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 
    QVERIFY(VideoListDataModel::mFirstInserted == 0);
    QVERIFY(VideoListDataModel::mLastInserted == 2);
    QVERIFY(mTestObject->getVideoCount() == 3);
    QVERIFY(spyModelChanged.count() == 1); 
    spyModelChanged.clear();   
       
    // append more to existing
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(10);
    emit signalAppendVideoList(mMediaFactory->copyOfMediaArray()); 

    QVERIFY(VideoListDataModel::mFirstInserted == 3);
    QVERIFY(VideoListDataModel::mLastInserted == 9);

    QVERIFY(mTestObject->getVideoCount() == 10);
    QVERIFY(spyModelChanged.count() == 1); 
    spyModelChanged.clear();
  
      
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
    
    connect(this, SIGNAL(signalNewVideoAvailable(CMPXMedia*)), mTestObject, SLOT(newVideoAvailableSlot(CMPXMedia*)));
    connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))); 
    
    QSignalSpy spyModelChanged(mStubModel, SIGNAL(modelChanged()));
    
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
 
    CMPXMedia *media = mMediaFactory->getMedia(MEDIA_COUNT + 100);
    // succeed insert
    emit signalNewVideoAvailable(media);
    QVERIFY(VideoListDataModel::mFirstInserted == 0);
    QVERIFY(VideoListDataModel::mLastInserted == 0);
    QVERIFY(spyModelChanged.count() == 1);
    
    QVERIFY(mTestObject->getVideoCount() == 1);
    delete media;
    media = 0;
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    spyModelChanged.clear();
    
    // null pointer provided
    emit signalNewVideoAvailable(media);
    QVERIFY(spyModelChanged.count() == 0);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 1);
    spyModelChanged.clear();
    
    // invalid item
    media = mMediaFactory->getMedia(MEDIA_COUNT + 100, MediaDataNone);
    emit signalNewVideoAvailable(media);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 1);
    QVERIFY(spyModelChanged.count() == 0);
    delete media;
    media = 0;
    spyModelChanged.clear();
    
    // item without name
    media = mMediaFactory->getMedia(MEDIA_COUNT + 100, MediaDataId);
    emit signalNewVideoAvailable(media);
    QVERIFY(VideoListDataModel::mFirstInserted == -1);
    QVERIFY(VideoListDataModel::mLastInserted == -1);
    QVERIFY(mTestObject->getVideoCount() == 1);
    QVERIFY(spyModelChanged.count() == 0);
    delete media;
    media = 0;
    spyModelChanged.clear();
    
    delete mTestObject;
    mTestObject = new VideoListDataModelPrivate(mStubModel);
    QVERIFY(mTestObject->initialize() == 0);
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalNewVideoAvailable(CMPXMedia*)), mTestObject, SLOT(newVideoAvailableSlot(CMPXMedia*))));

    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    VideoListDataModel::mFirstInserted = -1;
    VideoListDataModel::mLastInserted = -1;
    spyModelChanged.clear();
    
    // succeed case
    CMPXMedia *media3 = mMediaFactory->getMedia(MEDIA_COUNT + 103 );
    emit signalNewVideoAvailable(media3);
    QVERIFY(VideoListDataModel::mFirstInserted == mTestObject->getVideoCount() - 1);
    QVERIFY(VideoListDataModel::mLastInserted == mTestObject->getVideoCount() - 1);
    QVERIFY(spyModelChanged.count() == 1);
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
    mTestObject->initialize();
    mMediaFactory->removeArray();
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;
    QSignalSpy spyModelChanged(mStubModel, SIGNAL(modelChanged()));
    
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalDeleteVideo(TMPXItemId)), mTestObject, SLOT(itemDeletedSlot(TMPXItemId))));  
    
    // no videos
    emit signalDeleteVideo(TMPXItemId(0,0));
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(spyModelChanged.count() == 0);
    
    // invalid id
    emit signalDeleteVideo(TMPXItemId::InvalidId());
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(spyModelChanged.count() == 0);
    
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;
    spyModelChanged.clear();
    
    // not marked as removed
    emit signalDeleteVideo(TMPXItemId(MEDIA_COUNT / 2,0));
    QVERIFY(VideoListDataModel::mFirstRemoved == MEDIA_COUNT / 2);
    QVERIFY(VideoListDataModel::mFirstRemoved == MEDIA_COUNT / 2);
    QVERIFY(spyModelChanged.count() == 1);
    
    VideoListDataModel::mFirstRemoved = -1;
    VideoListDataModel::mLastRemoved = -1;
    spyModelChanged.clear();
    
    QModelIndex index = mStubModel->index(MEDIA_COUNT / 2, 0, QModelIndex());
    TMPXItemId id = mTestObject->markVideoRemoved(index);
    
    // marked as removed
    emit signalDeleteVideo(id);
    // item already removed from container, no notifications
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(spyModelChanged.count() == 0);
    
    // no thumbnail data (tested for coverity)
    VideoThumbnailData *tmp = mTestObject->mVideoThumbnailData;
    mTestObject->mVideoThumbnailData = 0;
    
    index = mStubModel->index(mTestObject->getVideoCount() - 1, 0, QModelIndex());
    id = mTestObject->markVideoRemoved(index);
    emit signalDeleteVideo(id);
    // item already removed from container, no notifications
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(VideoListDataModel::mFirstRemoved == -1);
    QVERIFY(spyModelChanged.count() == 0);
    
    mTestObject->mVideoThumbnailData = tmp;
    
    disconnect(this, SIGNAL(signalDeleteVideo(TMPXItemId)), mTestObject, SLOT(itemDeletedSlot(TMPXItemId)));
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));
    
}

// -----------------------------------------------------------------------------
// testVideoDeleteCompletedSlot
// -----------------------------------------------------------------------------
//

void TestVideoModel_p::testVideoDeleteCompletedSlot()
{
    QVERIFY(connect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*))));  
    QVERIFY(connect(this, SIGNAL(signalDeleteCompleted(int, QList<TMPXItemId>*)), mTestObject, SLOT(videoDeleteCompletedSlot(int, QList<TMPXItemId>*))));  
   
    mMediaFactory->removeArray();
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());

    QSignalSpy spysignal(mStubModel, SIGNAL(modelChanged()));
    VideoListDataModel::mStatus = -1;
    VideoListDataModel::mStatusData = QVariant();
    
    QList<TMPXItemId> idList;
    
    // null list
    emit signalDeleteCompleted(0, 0);
    QVERIFY(spysignal.count() == 0);
    QVERIFY(VideoListDataModel::mStatus == -1);
    QVERIFY(!VideoListDataModel::mStatusData.isValid());
    
    // empty list
    emit signalDeleteCompleted(0, &idList);
    int count =spysignal.count();
    QVERIFY(spysignal.count() == 1);
    QVERIFY(VideoListDataModel::mStatus == VideoCollectionCommon::statusDeleteSucceed);
    QVERIFY(!VideoListDataModel::mStatusData.isValid());
    VideoListDataModel::mStatus = -1;
    VideoListDataModel::mStatusData = QVariant();
    spysignal.clear();
    
    // one failed (no item)
    idList.append(TMPXItemId(0,0));
    emit signalDeleteCompleted(0, &idList);
    QVERIFY(spysignal.count() == 1);
    QVERIFY(VideoListDataModel::mStatus == VideoCollectionCommon::statusSingleDeleteFail);
    QVERIFY(VideoListDataModel::mStatusData.isValid());
           
    VideoListDataModel::mStatus = -1;
    VideoListDataModel::mStatusData = QVariant();
    spysignal.clear();

    idList.append(TMPXItemId(1,0));
    // multiple failed
    emit signalDeleteCompleted(0, &idList);
    QVERIFY(spysignal.count() == 1);
    QVERIFY(VideoListDataModel::mStatus == VideoCollectionCommon::statusMultipleDeleteFail);
    QVERIFY(VideoListDataModel::mStatusData.isValid());
    
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
    
    QSignalSpy spysignal(mTestObject, SIGNAL(videoDetailsReady(TMPXItemId)));

    // no videos
    emit signalVideoDetailsCompleted(TMPXItemId(0,0));
    QVERIFY(spysignal.count() == 0);        
    
    mMediaFactory->createMediaItems(MEDIA_COUNT);
    emit signalNewVideoList(mMediaFactory->copyOfMediaArray());
    
    // invalid id
    emit signalVideoDetailsCompleted(TMPXItemId::InvalidId());
    QVERIFY(spysignal.count() == 0); 
    emit signalVideoDetailsCompleted(TMPXItemId(MEDIA_COUNT,0));
    QVERIFY(spysignal.count() == 0); 
    
    // correct ids
    emit signalVideoDetailsCompleted(TMPXItemId(0,0));
    QVERIFY(spysignal.count() == 1); 
    QVERIFY(spysignal.value(0).at(0).toInt() == 0);
    spysignal.clear();
    emit signalVideoDetailsCompleted(TMPXItemId(MEDIA_COUNT - 1,0));
    QVERIFY(spysignal.count() == 1); 
    
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
    int count = mTestObject->getVideoCount();
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
    QVERIFY( mTestObject->getVideoCount() == 2);
    QVERIFY(mTestObject->getMediaIdFromIndex(-1) == TMPXItemId::InvalidId());
    TMPXItemId id = mTestObject->getMediaIdFromIndex(0);
    QVERIFY(mTestObject->getMediaIdFromIndex(0) == TMPXItemId(KVcxMvcCategoryIdDownloads, 1));
    id = mTestObject->getMediaIdFromIndex(1);
    QVERIFY(mTestObject->getMediaIdFromIndex(1) == TMPXItemId(KVcxMvcCategoryIdCaptured, 1));
    
    disconnect(this, SIGNAL(signalNewVideoList(CMPXMediaArray*)), mTestObject, SLOT(newVideoListSlot(CMPXMediaArray*)));  
}

// End of file
    


