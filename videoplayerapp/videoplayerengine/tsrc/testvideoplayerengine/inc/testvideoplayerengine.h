/*
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
* Description:  Videoplayerengine test class declaration.
*
*/

// TODO: at the moment we are not able to get access to all private methods
//       to the class under test. We need to declare some own framework in order
//       to declare testclasses into friends of classes under test etc.
//
//      missing test functions:
//      - how to setup construction to fail for testing purposes


#ifndef __TESTVIDEOPLAYERENGINE_H__
#define __TESTVIDEOPLAYERENGINE_H__


// INCLUDES
#include <QtTest/QtTest>

class MpxViewPlugin;
class QMpxVideoPlaybackWrapper;
class QVideoPlayerEngine;
class VideoServices;

class TestVideoPlayerEngine : public QObject
	{

    Q_OBJECT

public: // Constructor / destructor

    /**
     * Contructor.
     *
     */
	TestVideoPlayerEngine();

    /**
     * Destructor.
     *
     */
    virtual ~TestVideoPlayerEngine();

private slots:

    void init(bool isService = false);
    void cleanup();

    void testCreateDelete();
    void testCreateDeleteService();

    void testInitialize();
    void testInitializeService();
    
    void testMultipleInitialize();
    void testMultipleInitializeService();

    void testHandleCommandPreInit();
    void testHandleCommandPostInit();
    void testHandleCommandPreAndPostInit();

    void testHandleCommandPluginFail();
    void testHandleCommandPluginFail1();
    void testHandleCommandPluginFail2();
    
    void testPlayMedia();
    void testSetEmbedded();

    // called after last test case executed
    void cleanupTestCase();

signals:

	void aboutToQuit();

private:

	QVideoPlayerEngine*       mTestObject;

    MpxViewPlugin*            mCurrentViewPlugin;
    MpxViewPlugin*            mPlaybackViewPlugin;
    MpxViewPlugin*            mCollectionViewPlugin;
    MpxViewPlugin*            mFileDetailsViewPlugin;

    VideoServices*            mVideoServices;
    
    QMpxVideoPlaybackWrapper *mPlaybackWrapper;

};

#endif  // __TESTVIDEOPLAYERENGINE_H__

// End of file



