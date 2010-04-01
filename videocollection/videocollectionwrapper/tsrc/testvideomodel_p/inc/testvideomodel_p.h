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



#ifndef __TESTVIDEOMODEL_P_H__
#define __TESTVIDEOMODEL_P_H__


// INCLUDES
#include <QtTest/QtTest>
#include <QList>
#include <e32const.h>
#include <mpxitemid.h>

class MediaObjectFactory;
class CMPXMedia;
class CMPXMediaArray;
class VideoListDataModelPrivate;
class VideoListDataModel;

class TestVideoModel_p: public QObject
{
    Q_OBJECT
    
signals:
    
    /**
     * emitted to test thumbnailFetchedSlot
     */
    void signalThumbnailsFetched(QList<TMPXItemId>);
    
    /**
     * emitted to pass new media-array into testable object and
     * to test newVideoListSlot
     */
    void signalNewVideoList(CMPXMediaArray*);
    
    /**
     * emitted to pass more medias using media-array into testable object and
     * to test appendVideoListSlot
     */
    void signalAppendVideoList(CMPXMediaArray*);
    
    /**
     * emitted to test newVideoAvailableSlot
     */
    void signalNewVideoAvailable(CMPXMedia*);
    
    /**
     * emitted to get deleteVideoSlot to be called.
     */
    void signalDeleteVideo(TMPXItemId);
    
    /**
    * emitted to get videoDeleteCompletedSlot to be called.
    */
   void signalDeleteCompleted(int, QList<TMPXItemId>*);
   
   /**
    * emitted to get videoDetailsCompletedSlot to be called.
    */
   void signalVideoDetailsCompleted(TMPXItemId);
   
    // test functions for the test framework
private slots:
    /**
     * called before anything else when test starts
     */
    void initTestCase();

    /**
     * will be called before each testfunction is executed.
     *
     */
    void init(); 
    
    /**
     * will be called after every testfunction.
     *
     */
    void cleanup();
    
    /**
     * will be called after testing ends
     *
     */
    void cleanupTestCase();
    
    /**
     * Tests initialize.
     *
     */
    void testInitialize();
        
    /**
     * verifies getVideoCount
     */
    void testItemCount();

    /**
     * verifies getMediaIdFromIndex
     */
    void testGetMediaIdFromIndex();
    
    /**
     * verifies getVideoNameFromIndex
     */
    void testGetVideoNameFromIndex();
    
    /**
     * verifies getVideoThumbnailFromIndex
     */
    void testGetVideoThumbnailFromIndex();
    
    /**
     * verifies getVideoSizeFromIndex
     */
    void testGetVideoSizeFromIndex();
    
    /**
     * verifies getVideoAgeProfileFromIndex
     */
    void testGetVideoAgeProfileFromIndex();
        
    /**
     * verifies getVideodurationFromIndex
     */
    void testGetVideodurationFromIndex();

    /**
     * verifies getVideoDateFromIndex
     */
    void testGetVideoDateFromIndex();
        
    /**
     * verifies getMetaDataFromIndex
     */
    void testGetMetaDataFromIndex();
    
    /**
     * verifies markVideoRemoved and restoreRemoved
     */
    void testVideoMarkAndRestoreDeleted();
    
    /**
     * verifies getFilePathFromIndex
     */
    void testGetFilePathFromIndex();
    
    /**
     * verifies getFilePathForId
     */
    void testGetFilePathForId();
         
    /**
     * verifies thumbnailsFetchedSlot
     */
    void testThumbnailsFetchedSlot();
    
    /**
     * verifies newVideoListSlot
     */
    void testNewVideoListSlot();
    
    /**
     * verifies appendVideoListSlot
     */
    void testAppendVideoListSlot();
        
    /**
     * verifies newVideoAvailableSlot
     */
    void testNewVideoAvailableSlot();
     
    /**
     * verifies videoDeletedSlot
     */
    void testVideoDeletedSlot();
    
    /**
     * verifies videoDeleteCompletedSlot
     */
    void testVideoDeleteCompletedSlot();
      
    /**
     * verifies videoDetailsCompletedSlot
     */
    void testVideoDetailsCompletedSlot();

    /**
     * verifies getVideoCount when fetching collections
     */
     void testCollectionItemCount();

    /**
     * verifies getMediaIdFromIndex when fetching collections
     */
    void testGetCollectionIdFromIndex();

private:
  
    /**
     * object under test
     */  
    VideoListDataModelPrivate *mTestObject;
    
    /**
     * CMPXMedia -object factory.
     */
    MediaObjectFactory *mMediaFactory;
    
    /**
     * stubmodel, used to create QModelIndeces
     */
    VideoListDataModel *mStubModel;
};


#endif  // __TESTVIDEOMODEL_P_H__

// End of file
    


