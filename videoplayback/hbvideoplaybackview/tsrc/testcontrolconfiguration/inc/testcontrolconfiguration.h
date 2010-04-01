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
* Description:   test module for QMPXVideoPlaybackControlConfiguration
* 
*/

// Version : %version:  1 %


#ifndef __TESTCONTROLCONFIGURATION_H__
#define __TESTCONTROLCONFIGURATION_H__


// INCLUDES
#include <QtTest/QtTest>

class HbVideoBasePlaybackView;
class CMPXVideoViewWrapper;
class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;
class QMPXVideoPlaybackControlConfiguration;



class TestControlConfiguration : public QObject
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
    
    
    void setup();
         
private slots:
	  void testControlList();	  
	  void testUpdateControlsWithFileDetails();
	  void testUpdateControlList();

    
signals:

    void commandSignal(int);
    
private:
	  HbVideoBasePlaybackView*                 mBaseVideoView;
	  CMPXVideoViewWrapper*                    mVideoViewWrapper;
	  QMPXVideoPlaybackViewFileDetails*        mFileDetails;
	  QMPXVideoPlaybackControlsController*     mControlsController;
	  QMPXVideoPlaybackControlConfiguration*   mControlConfig;
 
};


#endif  // __TESTCONTROLCONFIGURATION_H__

// End of file
