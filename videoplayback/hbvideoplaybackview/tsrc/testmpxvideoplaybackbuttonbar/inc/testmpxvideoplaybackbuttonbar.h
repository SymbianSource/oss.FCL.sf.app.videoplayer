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
* Description:   tester for methods in TestMPXVideoPlaybackButtonBar
* 
*/

// Version : %version:  1 %

#ifndef __TESTMPXVIDEOPLAYBACKBUTTONBAR_H__
#define __TESTMPXVIDEOPLAYBACKBUTTONBAR_H__


// INCLUDES
#include <QtTest/QtTest>

class QMPXVideoPlaybackButtonBar;
class QMPXVideoPlaybackControlsController;

class TestMPXVideoPlaybackButtonBar : public QObject
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
    void testPlay();
    void testStartFFSeeking();
    void testStartRWSeeking();
    void testEndSeeking();
    void testPause();
    void testChangeAspectRatio();
    void testUpdateState();
    void testAspectRatioChanged();
    void testHandleButtonPressed();
    void testUpdateWithFileDetails();
    void testOpenDetailsView();

signals:
    void commandSignal(int);

private:
    QMPXVideoPlaybackButtonBar*           mButtonBar;
    QMPXVideoPlaybackControlsController*  mController;
};


#endif  // __TESTMPXVIDEOPLAYBACKBUTTONBAR_H__

// End of file
    


