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
* Description:   tester for methods in TestMPXVideoPlaybackVolumeBar
* 
*/

// Version : %version:  1 %

#ifndef __TESTMPXVIDEOPLAYBACKVOLUMEBAR_H__
#define __TESTMPXVIDEOPLAYBACKVOLUMEBAR_H__


// INCLUDES
#include <QtTest/QtTest>

class QMPXVideoPlaybackNonTouchVolumeBar;

class TestMPXVideoPlaybackVolumeBar : public QObject
{
    Q_OBJECT

public:

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

    // test functions for the test framework         
private slots:

    // the order in which these testXX methods are declared is important
    // changing this order will affect the test results
    void testSetVisible();
    void testHideVolumeControl();
    void testMousePressEvent();
    void testMouseReleaseEvent();


signals:
    void commandSignal();

private:
    QMPXVideoPlaybackNonTouchVolumeBar*   mVolumeBar;
};


#endif  // __TESTMPXVIDEOPLAYBACKVOLUMEBAR_H__

// End of file
