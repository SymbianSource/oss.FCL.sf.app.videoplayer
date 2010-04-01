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

#ifndef __TESTLISTWIDGET_H__
#define __TESTLISTWIDGET_H__


// INCLUDES
#include <QtTest/QtTest>

class VideoListWidget;
class DummyDataModel;
class HbAbstractViewItem;
class HbView;
class VideoSortFilterProxyModel;
class VideoCollectionWrapper;
class HbMainWindow;
class ListWidgetTester;
class VideoServices;
class VideoListDataModel;

class TestListWidget : public QObject
{
    Q_OBJECT

public:

    void setRowCount(int count, int type = 0, VideoListDataModel *model = 0);
    
    // test functions for the test framework
private slots:

    /**
     * will be called automaticallybefore each testfunction is executed.
     *
     */
    void init();

    /**
     * will be called automaticallybefore after every testfunction.
     *
     */
    void cleanup();

	
	/**
	 * verifies initialize
	 */
	void testInitialize();
	
	/**
	 * verifies activate()
	 */
    void testActivate();
    
    /**
     * verifies deactivate()
     */
    void testDeactivate();

    /**
     * verifies getLevel
     */
    void testGetLevel();
    
    /**
     * verifies getModel
     */
    void testGetModel();
    
    /**
     * verifies emitActivated
     */
    void testEmitActivated();
    
    /**
     * verifies longPressGesture
     */
    void testLongPressGesture();
    
    /**
     * verifies setContextMenu
     */
    void testSetContextMenu();
    
    /**
     * verifies shareItemSlot
     */
    void testShareItemSlot();
    
    /**
     * verifies deleteItemSlot
     */
    void testDeleteItemSlot();
    
    /**
     * verifies renameSlot
     */
    void testRenameSlot();
    
    /**
     * verifies playAllSlot
     */
    void testPlayAllSlot();
    
    /**
     * verifies addItemSlot
     */
    void testAddItemSlot();
    
    /**
     * verifies addToCollectionSlot()
     */
    void testAddToCollectionSlot();
    
    /**
     * verifies openDetailsSlot()
     */
    void testOpenDetailsSlot();
    
    /**
     * verifies back()
     */
    void testBack();
    
    /**
     * verifies scrollingEndedSlot();
     */
    void testScrollingEndedSlot();

    /**
     * verifies scrollPositionChangedSlot();
     */
    void testScrollPositionChangedSlot();

signals:
    
    /**
     * test signal
     */
    void testSignal();

    /**
     * another test signal
     */
    void testSignal(const QPointF&);
    
private:
    /**
     * object under test, ListWidgetTester is inherited from VideoListWidget
     */
    ListWidgetTester* mTestWidget;

    /**
     * dummy view
     */
    HbView *mTempView;
    
    /**
     * dummy main wnd
     */
    HbMainWindow *mDummyMainWnd;

};


#endif  // __TESTLISTWIDGET_H__

// End of file
