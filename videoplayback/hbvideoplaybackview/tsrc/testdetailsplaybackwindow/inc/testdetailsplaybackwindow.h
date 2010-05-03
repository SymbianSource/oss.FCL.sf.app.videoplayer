/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   tester for methods in TestDetailsPlaybackWindow
* 
*/

// Version : %version:  3 %


#ifndef __TESTDETAILSPLAYBACKWINDOW_H__
#define __TESTDETAILSPLAYBACKWINDOW_H__


// INCLUDES
#include <QtTest/QtTest>

class QEvent;
class QMPXVideoPlaybackDetailsPlaybackWindow;
class QMPXVideoPlaybackControlsController;

class TestDetailsPlaybackWindow : public QObject
{
    Q_OBJECT

public:

    /**
     * will be called before each testfunction is executed.
     *
     */
    void init( bool attachOperation = false ); 
    
    /**
     * will be called after every testfunction.
     *
     */
    void cleanup();
    
    // test functions for the test framework         
private slots:

    // the order in which these testXX methods are declared is important
    // changing this order will affect the test results
    void testUpdateState();
    void testPlayPause();
    void testMousePressEvent();
    void testMouseReleaseEvent();
    void testAttach();

signals:
    void commandSignal();

private:
    QMPXVideoPlaybackDetailsPlaybackWindow*  mWindow;
    QMPXVideoPlaybackControlsController*  mController;
};


#endif  // __TESTDETAILSPLAYBACKWINDOW_H__

// End of file
    

