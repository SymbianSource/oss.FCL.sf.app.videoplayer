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

class VideoListDataModel;
class FilterProxyTester;
class VideoCollectionClient;

class TestVideoSortFilterProxyModel : public QObject
{
    Q_OBJECT

    // test functions for the test framework

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
     * Test when model init fails. 
     */
    void testInitializeDataModelInitFail();
    
    /**
	 * Test when collection init fails. 
	 */
    void testInitializeCollectionInitFail();
        
    /**
     * Test second call to initialize with valid data.
     */
    void testSecondInitialize();
    
    /**
	 * Test second call to initialize when model init fails. 
	 */
    void testSecondInitializeModelInitFail();
    
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
     * Test openItem with valid data.
     */
    void testOpenItem();

    /**
     * Test openItem when getMediaId fails.
     */
    void testOpenItemGetMediaIdFail();    

    /**
     * Test openItem when model is NULL.
     */
    void testOpenItemModelNull();
    
    /**
     * Test openItem when openVideo fails.
     */
    void testOpenItemOpenVideoFails();

    /**
     * Test back with valid data.
     */
    void testBack();
    
    /**
     * Test back when videocollectionclient is null.
     */
    void testBackClientNull();

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
     * Test lessThan when initialize has not been called.
     */
    void testLessThanProxyModelNotInitialized();
    
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
     * Test filterAcceptsRow when initialize has not been called.
     */
    void testFilterAcceptsRowProxyModelNotInitialized();
    
    /**
     * Calls filterAcceptsRow.
     * Tests that return values are correct with items with varying statuses. 
     * 
     */
    void testFilterAcceptsRow();

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
     * Tests calling addNewCollection where collection client returns -1.
     */
    void testAddNewCollectionFail();
    
public slots:
	
	int testShortDetailsReadySlot(int index);
    
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
};


#endif  // __TESTVIDEOSORTFILTERPROXYMODEL_H__

// End of file
    


