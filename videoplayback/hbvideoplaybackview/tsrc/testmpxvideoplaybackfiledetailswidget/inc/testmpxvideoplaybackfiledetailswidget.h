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
* Description:   tester for methods in TestMPXVideoPlaybackFileDetailsWidget
* 
*/

// Version : %version:  2 %

#ifndef __TESTMPXVIDEOPLAYBACKFILEDETAILSWIDGET_H__
#define __TESTMPXVIDEOPLAYBACKFILEDETAILSWIDGET_H__


// INCLUDES
#include <QtTest/QtTest>

class QMPXVideoPlaybackFileDetailsWidget;
class QMPXVideoPlaybackControlsController;
class QDateTime;

class TestMPXVideoPlaybackFileDetailsWidget : public QObject
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
    void testTitle();
    void testDescription();
    void testDuration();
    void testDate();
    void testLocation();
    void testAuthor();
    void testCopyright();
    void testLanguage();
    void testKeywords();
    void testSize();
    void testResolution();
    void testFormat();
    void testBitrate();
    void testFolder();

private:
    void verifyResult( QString primaryText,
                       bool exist,
                       QString expectedSecondaryText = "",
                       bool needToBeCompared = true );

    QString dateTimeStringFormat( QDateTime dateTime );
    
signals:
    void commandSignal(int);

private:
    QMPXVideoPlaybackFileDetailsWidget*   mWidget;
    QMPXVideoPlaybackControlsController*  mController;
};


#endif  // __TESTMPXVIDEOPLAYBACKFILEDETAILSWIDGET_H__

// End of file
