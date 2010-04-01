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
* Description:   tester for methods in VideoListSelectionDialog
* 
*/

#ifndef __TESTVIDEOLISTSELECTIONDIALOG_H__
#define __TESTVIDEOLISTSELECTIONDIALOG_H__


// INCLUDES
#include <QtTest/QtTest>

#include <QItemSelection>

class VideoCollectionUiLoader;
class VideoListWidget;
class DummyDataModel;
class VideoSortFilterProxyModel;
class VideoListSelectionDialog;

class TestVideoListSelectionDialog : public QObject
{
    Q_OBJECT

    // test functions for the test framework
   
signals:

    /**
     * signal for testing markAllStateChangedSlot
     */
    void signalTestMarkAll(int);
    
    /**
     * signal for testing selectionChangedSlot
     */
    void signalTestSelectionChanged(const QItemSelection&, const QItemSelection&);
    
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
     * test of setContent with empty title
     */
    void testSetContentEmptyTitle();
    
    /**
     * test of setContent with null content
     */
    void testSetContentNullcontent();
    
    /**
     * test of setContent while finding items
     */
    void tesSetContentItemFinding();
    
    /**
     * verifies exec
     */
    void testExec();
    
    /**
     * verifies markAllStateChangedSlot
     */
    void testmMarkAllStateChangedSlot();
    
    /**
     * verifies selectionChangedSlot
     */
    void testSelectionChangedSlot();
 
private:
    /**
     * object under test
     */
    VideoListSelectionDialog *mTestObject;  
    
    /**
     * dummy UI loader object
     */
    VideoCollectionUiLoader *mTestUiLoader;
    
    /**
     * dummy source model
     */
    DummyDataModel *mSourceModel;
    
    /**
     * dummy model
     */
    VideoSortFilterProxyModel *mModel;
    
    /**
     * dummy widget
     */
    VideoListWidget *mTestWidget;
};


#endif  // __TESTVIDEOCOLLECTIONVIEWUTILS_H__

// End of file
    


