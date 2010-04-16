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

#ifndef __TESTVIDEOSORTFILTERPROXYMODEL_H__
#define __TESTVIDEOSORTFILTERPROXYMODEL_H__


// INCLUDES
#include <QtTest/QtTest>
#include <mpxitemid.h>

class VideoListDataModel;
class FilterProxyTester;
class VideoCollectionClient;
class VideoSortFilterProxyModel;

class TestVideoSortFilterProxyModel : public QObject
{
    Q_OBJECT

    // test functions for the test framework
    
signals:
    
    /**
     * test signal
     */
    void testSignal();

    /**
     * test signal
     */
    void testSignalMpxId(const TMPXItemId &id);
    
private slots:

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
     * Test initialize with valid data.
     */
    void testInitialize();
    
    /**
     * Test initialize with null model causing connect to fail.
     */
    void testInitializeSignalConnectFail();
        
    /**
     * Test second call to initialize with valid data.
     */
    void testSecondInitialize();
    
    /**
     * Test open.
     */
    void testOpen();
    
    /**
     * Test deleteItems with valid data.
     */
    void testDeleteItems();
    
    /**
     * Test deleteItems with NULL model.
     */
    void testDeleteItemsModelNull();    
    
    /**
     * Test deleteItems when removeRows fails.
     */
    void testDeleteItemsRemoveRowsFail();

    /**
     * Test openItem when getMediaId fails.
     */
    void testOpenItemInvalidId();    

    /**
     * Test openItem when collection is NULL.
     */
    void testOpenItemCollectionNull();
    
    /**
     * Test openItem when openVideo fails.
     */
    void testOpenItemOpenVideoFails();
    
    /**
    * Test openItem with valid data.
    */
    void testOpenItem();

    /**
     * Test back with valid data.
     */
    void testBack();

    /**
     * Test fetchItemDetails with valid data.
     */
    void testFetchItemDetails();

    /**
     * Test fetchItemDetails when getMediaId fails.
     */
    void testFetchItemDetailsGetMediaIdFail();    

    /**
     * Test fetchItemDetails when getVideoDetails fails.
     */
    void testFetchItemDetailsGetVideoDetailsFails();

    /**
     * Test lessThan when there's no mModel.
     */
    void testLessThanNoModel();
    
    /**
     * Calls sure lessThan with Qt::DisplayRole setted on 
     * tests that return values are correct for presetted items
     */
    void testLessThanName();
    
    /**
     * Calls sure lessThan with VideoCollectionCommon::KeySizeValue setted on 
     * tests that return values are correct for presetted items
     */
    void testLessThanSize();
    
    /**
     * Calls sure lessThan with VideoCollectionCommon::KeyDate setted on 
     * tests that return values are correct for presetted items
     */
    void testLessThanDateTime();
    
    /**
     * Calls less than with media objects of combined default / user created 
     * collections
     */
    void testLessThanDefaults();
    
    /**
     * Calls sure lessThan with invalid role,
     * same indexes and for invalid data
     * tests that return values are correct for presetted items
     */
    void testLessThanInvalid();
    
    /**
     * Calls doSorting.
     * tests that sorting is done only once
     */
    void testDoSorting();
    
    /**
     * Calls filterAcceptsRow. using type EAllVideos for model
     * Tests that return values are correct with items with varying statuses.
     * 
     */
    void testFilterAcceptsRowVideos();
    
    /**
     * Calls filterAcceptsRow. using type differ than EAllVideos for model
     * Tests that return values are correct with items with varying statuses.
     * 
     */
    void testFilterAcceptsNoVideoTypes();

    /**
     * Test getMediaFilePathForId.
     */
    void testGetMediaFilePathForId();
 
    /**
     * Tests calling addNewCollection when initialize has not been called.
     */
    void testAddNewCollectionNoCollectionClient();
    
    /**
     * Tests calling addNewCollection where collection client returns 0.
     */
    void testAddNewCollectionSucceed();
    
    /**
     * tests resolveAlbumName
     */
    void testResolveAlbumName();
    
    /**
     * tests addItemsInAlbum
     */
    void testAddItemsInAlbum();
    
    /**
     * tests removeItemsFromAlbum
     */
    void testRemoveItemsFromAlbum();
    
    /**
     * tests getOpenItem
     */
    void testGetOpenItem();
    
    /**
     * tests removeAlbums
     */
    void testRemoveAlbums();
    
    /**
     * tests albumChangedSlot
     */
    void testAlbumChangedSlot();
    
    /**
     * tests indexOfId
     */
    void testIndexOfId();
    
    /**
     * tests setGenericIdFilter
     */
    void testSetGenericIdFilter();
    
    /**
     * tests setAlbumInUse
     */
    void testSetAlbumInUse();
    
    /**
     * tests renameAlbum
     */
    void testRenameAlbum();
    
    /**
     * tests itemModifiedSlot
     */
    void testItemModifiedSlot();
    
private:
    
    /**
     * stub videolist model
     */
	VideoListDataModel   *mStubModel;

    /**
     * stub CVideoCollectionClient.
     */
    VideoCollectionClient *mCollectionClient;
    
    /**
     * tester object inherited from the actual test object
     * implemented to be able to call protected objects
     */
    FilterProxyTester *mTestObject;
    
    /**
     * dummy collections content proxy
     */
    VideoSortFilterProxyModel *mCollectionModel;
};


#endif  // __TESTVIDEOSORTFILTERPROXYMODEL_H__

// End of file
    


