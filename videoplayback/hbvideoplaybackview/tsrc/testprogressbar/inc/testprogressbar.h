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

#ifndef __TESTPROGRESSBAR_H__
#define __TESTPROGRESSBAR_H__


// INCLUDES
#include <QtTest/QtTest>

class QMPXVideoPlaybackProgressBar;
class QMPXVideoPlaybackControlsController;

class TestProgressBar : public QObject
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
    
    void setup();

    // test functions for the test framework         
private slots:
        void testMousePressEvent();
        void testMouseReleaseEvent();
        void testMouseMoveEvent();
        void testUpdateWithFileDetails();
        void testUpdateState();
        void testDurationChanged();
        void testPositionChanged();	


signals:
    void commandSignal(int);

private:
    QMPXVideoPlaybackProgressBar*         mProgBar;
    QMPXVideoPlaybackControlsController*  mController;
};


#endif  // __TESTPROGRESSBAR_H__

// End of file
    


