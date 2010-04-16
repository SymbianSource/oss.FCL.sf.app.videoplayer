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

#ifndef __TESTHINTWIDGET_H__
#define __TESTHINTWIDGET_H__


// INCLUDES
#include <QtTest/QtTest>

class VideoHintWidget;
class VideoCollectionUiLoader;

class TestHintWidget : public QObject
{
    Q_OBJECT

public:
    TestHintWidget();
    ~TestHintWidget();

    void init(bool callInitialize);
    void cleanup();

    // test functions for the test framework
private slots:

	void testInitialize();
	void testSetLevel();
    void testActivate();
    void testDeactivate();
    void testUpdateUiComponents();
    void testOrientationChangedSlot();

signals:
    
    /**
     * test signal
     */
    void testSignal(Qt::Orientation);


private:

    
    /**
     * object under test
     */
    VideoHintWidget* mTestObject;
    
    /**
     * Ui loader
     */
    VideoCollectionUiLoader* mUiLoader;
    
};


#endif  // __TESTLISTWIDGET_H__

// End of file
