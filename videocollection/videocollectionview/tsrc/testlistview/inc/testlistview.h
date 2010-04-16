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

#ifndef __TESTLISTVIEW_H__
#define __TESTLISTVIEW_H__


// INCLUDES
#include <QtTest/QtTest>
#include "hbmessagebox.h"

class VideoListView;
class VideoSortFilterProxyModel;
class VideoCollectionWrapper;
class VideoCollectionUiLoader;
class VideoListWidget;

class TestListView : public QObject
{
    Q_OBJECT

    /**
     * Will be called before each testfunction is executed.
     */
    void init(bool initTestView = true); 
     
    /**
     * Returns amount of visible menu actions in the video list view.
     */
    int visibleMenuActions();
    
    /**
     * Set row count in model.
     */
    void setRowCount(int count);
    
    // Test functions for the test framework.
    
private slots:

    /**
     * Will be called after every testfunction.
     */
    void cleanup();

	/**
	 * Tests creating and deleting the view.
	 */
	void testCreateDelete();
	
	/**
	 * Tests initializeView.
	 */
	void testInitializeView();
	
    /**
	 * Tests creating menu of the view.
	 */
    void testCreateListMenu();
    
    /**
     * Tests menus.
     */
    void testMenus();

    /**
     * Tests createAction.
     */
    void testCreateAction();
    
    /**
     * Tests activatView.
     */
    void testActivateView();

    /**
     * Tests deactivateView.
     */
    void testDeactivateView();

    /**
     * Tests back.
     */
    void testBack();
    
    /**
    * Tests modelReadySlot.
    */
   void testModelReadySlot();

    /**
     * Tests deleteItemsSlot.
     */
    void testDeleteItemsSlot();

    /**
     * Tests aboutToShowMainMenuSlot.
     */
    void testAboutToShowMainMenuSlot();
    
    /**
     * Tests openSortByMenuSlot
     */
    void testOpenSortByMenuSlot();
    
    /**
     * Tests handleStatusSlot.
     */
    void testHandleStatusSlot();

    /**
     * Tests openAllVideosViewSlot.
     */
    void testOpenAllVideosViewSlot();

    /**
     * Tests openCollectionViewSlot.
     */
    void testOpenCollectionViewSlot();
    
    /**
     * Tests openServicesViewSlot.
     */
    void testOpenServicesViewSlot();
    
    /**
     * Tests startSortingSlot.
     */
    void testStartSortingSlot();

    /**
     * Tests view when orientation changes.
     */
    void testOrientationSlots();
    
    /**
     * Tests collectionOpenedSlot.
     */
    void testCollectionOpenedSlot();

    /**
     * Tests updateSubLabel.
     */
    void testUpdateSubLabel();
    
    /**
     * Tests showHint
     */
    void testShowHint();

signals:

    // Signals needed in tests.
    void testSignal();

    void testSignal(int);

    void testLayoutChangedSignal();

    void testCollectionOpenedSignal(bool, const QString&);

    void testStatusSignal(int, QVariant&);
    
private:
    
    VideoCollectionUiLoader* mUiLoader;
    
    VideoListView* mTestView;
    
    VideoListWidget* mListWidget;
    
    VideoCollectionWrapper* mWrapper;
};


#endif  // __TESTCOLLECTIONVIEW_H__

// End of file
    
