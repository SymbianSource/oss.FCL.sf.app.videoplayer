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
* Description:   tester for methods in Video Playback Controls Controller
* 
*/

// Version : %version:  1 %

#ifndef __TESTMPXVIDEOPLAYBACKCONTROLSCONTROLLER_H__
#define __TESTMPXVIDEOPLAYBACKCONTROLSCONTROLLER_H__


// INCLUDES
#include <QtTest/QtTest>
#include "mpxvideoplaybackcontrol.hrh"

// FORWARD DECLARATIONS
class HbVideoBasePlaybackView;
class CMPXVideoViewWrapper;
class QMPXVideoPlaybackControlsController;
class QMPXVideoPlaybackViewFileDetails;

class TestMPXVideoPlaybackControlsController : public QObject
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
       
private:

    void verifyHandleEventStateChangedResult( int value );
    void verifyHandleEventTvOutResult( bool tvOutConnected, bool value );
    
private slots:
    
    //
    // test addFileDetails()
    //
    void testAddFileDetails();
    
    //
    // test handleEvent()
    //
    void testHandleEventSetPosition();
    void testHandleEventSetDuration();
    void testHandleEventStateChanged();
    void testHandleEventSetVolume();
    void testHandleEventSetAspectRatio();
    void testHandleEventSetDownloadSize();
    void testHandleEventDownloadUpdated();
    void testHandleEventDownloadComplete();
    void testHandleEventSetDownloadPaused();
    void testHandleEventClearDownloadPaused();
    void testHandleEventTvOutConnected();
    void testHandleEventTvOutDisconnected();
    void testHandleEventHandleErrors();
    void testHandleEventShowVolumeControls();
    
    //
    // test handleCommand()
    //
    void testHandleCommand();
    
    //
    // test handleTappedOnScreen()
    //
    void testHandleTappedOnScreen();
    
    //
    // test updateVideoRectDone()
    //
    void testUpdateVideoRectDone();

    //
    // test private slot skipToNextVideoItem()
    //
    void testslot_skipToNextVideoItem();

    //
    // test private slot skipToPreviousVideoItem()
    //
    void testslot_skipToPreviousVideoItem();    
    
signals:

    void commandSignal();
    
private:

    HbVideoBasePlaybackView*			    mBaseVideoView;
    CMPXVideoViewWrapper*			        mViewWrapper;
    QMPXVideoPlaybackControlsController*	mController;
    QMPXVideoPlaybackViewFileDetails*       mFileDetails;
        
};


#endif  // __TESTMPXVIDEOPLAYBACKCONTROLSCONTROLLER_H__

// End of file
    


