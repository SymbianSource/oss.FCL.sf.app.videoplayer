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
* Description:   tester for methods in TestMPXVideoPlaybackControlPolicy
* 
*/

// Version : %version:  1 %

#ifndef __TESTMPXVIDEOPLAYBACKCONTROLPOLICY_H__
#define __TESTMPXVIDEOPLAYBACKCONTROLPOLICY_H__


// INCLUDES
#include <QtTest/QtTest>

class QMPXVideoPlaybackControlPolicy;
class QMPXVideoPlaybackViewFileDetails;

class TestMPXVideoPlaybackControlPolicy : public QObject
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
    void testBufferingAnimationPolicy();
    void testStatusPanePolicy();
    void testControlBarPolicy();
    void testDetailsWidgetPolicy();

signals:
    void commandSignal();

private:
    QMPXVideoPlaybackControlPolicy*   mPolicy;
    QMPXVideoPlaybackViewFileDetails* mDetails;
};


#endif  // __TESTMPXVIDEOPLAYBACKCONTROLPOLICY_H__

// End of file
