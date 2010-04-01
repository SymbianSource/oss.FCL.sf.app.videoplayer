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
* Description:   test module for QMPXVideoPlaybackFullScreenControl
* 
*/

#ifndef __TESTFULLSCREENCONTROL_H__
#define __TESTFULLSCREENCONTROL_H__


// INCLUDES
#include <QtTest/QtTest>

#include "mpxvideoplaybackcontrol.hrh"


class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;
class QMPXVideoPlaybackFullScreenControl;



class TestFullScreenControl : public QObject
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
    
    
    void setup(TMPXVideoPlaybackControls control);
    
    void createControl(TMPXVideoPlaybackControls control);
         
private slots:
        
        void testControlIndex();
        
        void testVolumeChanged();

        void testDurationChanged();

        void testPositionChanged();

        void testAspectRatioChanged();

        void testUpdateState();
        
        void testUpdateControlsWithFileDetails();

        void testUpdateControlProperties();    
    
signals:

    void commandSignal(int);
    
private:
    QMPXVideoPlaybackViewFileDetails*        mFileDetails;
    QMPXVideoPlaybackControlsController*     mControlsController;
    QMPXVideoPlaybackFullScreenControl*      mFullScreenControl;
 
};


#endif  // __TESTFULLSCREENCONTROL_H__

// End of file
