/**
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
* Description:   tester for methods in VideoListWidget
*
*/

#include <hbicon.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <hblabel.h>
#include <hbpushbutton.h>
#include <hbapplication.h>

#include "videocollectionuiloader.h"
#include "videocollectionviewutils.h"
#include "videocollectionviewutilsdata.h"
#include "videocollectionuiloaderdata.h"
#include "testhintwidget.h"

#define private public
#include "videohintwidget.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbApplication::setKeypadNavigationEnabled(false);
    HbMainWindow mainWnd;
    hbInstance->mWindowses.append(&mainWnd);

    TestHintWidget tv;
    
    int res;
    if(argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\testhintwidget.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

    return res;
}

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
TestHintWidget::TestHintWidget():
    mTestObject(0),
    mUiLoader(0)
{
    // nop
}

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
//
TestHintWidget::~TestHintWidget()
{
    cleanup();
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestHintWidget::init(bool callInitialize)
{
    mUiLoader = new VideoCollectionUiLoader;
    
    HbDocumentLoader::mFindWidgetFails = false;
    bool ok(false);
    mUiLoader->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    if(!ok)
    {
        cleanup();
        QFAIL("docml loading failed!");
    }
    
    mTestObject = mUiLoader->findObject<VideoHintWidget>(DOCML_NAME_VC_VIDEOHINTWIDGET);
    if(!mTestObject)
    {
        mTestObject = new VideoHintWidget(mUiLoader);
    }
    
    mTestObject->mHintText = "Go get some!";
    
    if (callInitialize)
    {
        mTestObject->mHintText = "";
        mTestObject->initialize();
    }
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestHintWidget::cleanup()
{
    if (mUiLoader)
    {
        VideoHintWidget* temp = mUiLoader->findObject<VideoHintWidget>(DOCML_NAME_VC_VIDEOHINTWIDGET);
        if (!temp)
        {
            // need to delete mTestObject as it is not owned by uiloader.
            delete mTestObject;
            mTestObject = 0;
        }
        mTestObject = 0;

        delete mUiLoader;
        mUiLoader = 0;
    }

    VideoCollectionViewUtilsData::reset();
    VideoCollectionUiLoaderData::reset();    
}

// ---------------------------------------------------------------------------
// testInitialize
// ---------------------------------------------------------------------------
//
void TestHintWidget::testInitialize()
{
    init(false);

    // Fail
    mTestObject->mHintText = "";
    VideoCollectionViewUtilsData::mCenRepStringValues.append(""); 
    mTestObject->initialize();
    QVERIFY(mTestObject->mHintText.isEmpty());

    // Succeed
    VideoCollectionViewUtilsData::mCenRepStringValues.append("test"); 
    mTestObject->initialize();
    QVERIFY(!mTestObject->mHintText.isEmpty());
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testSetLevel
// ---------------------------------------------------------------------------
//
void TestHintWidget::testSetLevel()
{
    init(false);
    mTestObject->setVisible(false);
    
    mTestObject->setLevel(VideoHintWidget::Collection);
    QCOMPARE(mTestObject->mCurrentLevel, VideoHintWidget::Collection);
    
    mTestObject->setLevel(VideoHintWidget::AllVideos);
    QCOMPARE(mTestObject->mCurrentLevel, VideoHintWidget::AllVideos);
    
    mTestObject->setLevel(VideoHintWidget::AllVideos);
    QCOMPARE(mTestObject->mCurrentLevel, VideoHintWidget::AllVideos);
    
    mTestObject->setLevel(VideoHintWidget::Collection);
    QCOMPARE(mTestObject->mCurrentLevel, VideoHintWidget::Collection);
    cleanup();
}

// ---------------------------------------------------------------------------
// testActivate
// ---------------------------------------------------------------------------
//
void TestHintWidget::testActivate()
{
    init(false);

    mTestObject->setVisible(false);
    
    // successful case.
    mTestObject->mActivated = false;

    HbLabel *noVideosLabel = mUiLoader->findWidget<HbLabel>(DOCML_NAME_NO_VIDEOS_LABEL);
    QVERIFY(noVideosLabel);
    noVideosLabel->setVisible(false);
    
    mTestObject->activate();
    QVERIFY(mTestObject->isVisible());
    QVERIFY(noVideosLabel->isVisible()); // checks that updateUiComponents() has been called.
    QVERIFY(mTestObject->mActivated == true);
    
    // second call
    noVideosLabel->setVisible(false);
    mTestObject->activate();
    QVERIFY(mTestObject->isVisible());    
    QVERIFY(!noVideosLabel->isVisible()); // shouldn't call updateUiComponents() again.
    QVERIFY(mTestObject->mActivated == true);
    
    cleanup();
}
 
// ---------------------------------------------------------------------------
// testDeactivate
// ---------------------------------------------------------------------------
//
void TestHintWidget::testDeactivate()
{
    init(false);
    
    mTestObject->setVisible(true);
    mTestObject->mActivated = true;
    
    // when serviceButton does not exist
    HbDocumentLoader::mFindWidgetFails = true;
    mTestObject->deactivate();
    QVERIFY(mTestObject->mActivated == false);
    QVERIFY(mTestObject->isVisible() == false);
    
    // service button exists
    mTestObject->setVisible(true);
    mTestObject->mActivated = true;
    HbDocumentLoader::mFindWidgetFails = false;
    mTestObject->deactivate();
    QVERIFY(mTestObject->mActivated == false);
    QVERIFY(mTestObject->isVisible() == false);
    
    // second call. widget is not active.
    mTestObject->setVisible(true);
    mTestObject->deactivate();
    QVERIFY(mTestObject->mActivated == false);
    QVERIFY(mTestObject->isVisible());
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testUpdateUiComponents
// ---------------------------------------------------------------------------
//
void TestHintWidget::testUpdateUiComponents()
{
    HbMainWindow *mainWnd = hbInstance->allMainWindows()[0];
    
    init(false);
    
    // when a widget cannot be found.
    mTestObject->mActivated = true;
    mTestObject->mButtonShown = true;
    HbDocumentLoader::mFindWidgetFails = true;
    mTestObject->setLevel(VideoHintWidget::AllVideos);
    // no verification possible except that it doesn't crash
    
    HbDocumentLoader::mFindWidgetFails = false;
    HbLabel *hintLabel = mUiLoader->findWidget<HbLabel>(DOCML_NAME_NO_VIDEOS_LABEL);
    QVERIFY(hintLabel);
    HbPushButton *serviceButton = mUiLoader->findWidget<HbPushButton>(DOCML_NAME_HINT_BUTTON);
    QVERIFY(serviceButton);
    
    // current level is all videos
    serviceButton->setVisible(false);
    mTestObject->setLevel(VideoHintWidget::AllVideos);
    QVERIFY(hintLabel->isVisible());
    QVERIFY(serviceButton->isVisible());
    
    // current level is not all videos
    serviceButton->setVisible(true);
    mTestObject->setLevel(VideoHintWidget::Collection);
    QVERIFY(hintLabel->isVisible());
    QVERIFY(serviceButton->isVisible() == false);

    // no hint text
    serviceButton->setVisible(true);
    mTestObject->mHintText = "";
    mTestObject->setLevel(VideoHintWidget::AllVideos);
    QVERIFY(hintLabel->isVisible());
    QVERIFY(serviceButton->isVisible() == false);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testOrientationChangedSlot
// ---------------------------------------------------------------------------
//
void TestHintWidget::testOrientationChangedSlot()
{
    // NOTE: This also checks that the signals are connected and
    // disconnected correctly.
    
    HbMainWindow *mainWnd = hbInstance->allMainWindows()[0];
    mainWnd->setOrientation(Qt::Horizontal);
    connect(this, SIGNAL(testSignal(Qt::Orientation)), mainWnd, SIGNAL(orientationChanged(Qt::Orientation)));
    
    init(false);
    
    HbLabel *noVideosLabel = mUiLoader->findWidget<HbLabel>(DOCML_NAME_NO_VIDEOS_LABEL);
    QVERIFY(noVideosLabel);

    // test that updateUiComponents is not called when only initialize has been called,
    // ie that no activate calls have been made yet.
    // mTestObject->setVisible is needed because othervise childs cannot be set visible, ie the 
    // serviceButton->setVisible does not work as expected.
    mTestObject->setVisible(true);
    noVideosLabel->setVisible(true);
    emit testSignal(Qt::Vertical);
    QVERIFY(noVideosLabel->isVisible());
    mTestObject->setVisible(false);
        
    // tests that updateUiComponents is called when widget is visible.
    mTestObject->activate();
    noVideosLabel->setVisible(false);
    emit testSignal(Qt::Horizontal);
    QVERIFY(noVideosLabel->isVisible());    
    mTestObject->deactivate();
    
    // tests that updateUiComponents is not called when widget is not visible:
    // mTestObject->setVisible is needed because othervise childs cannot be set visible, ie the 
    // serviceButton->setVisible does not work as expected.
    mTestObject->setVisible(true);    
    noVideosLabel->setVisible(true);
    emit testSignal(Qt::Vertical);
    QVERIFY(noVideosLabel->isVisible());
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testSetButtonShown
// ---------------------------------------------------------------------------
//
void TestHintWidget::testSetButtonShown()
{
    HbMainWindow *mainWnd = hbInstance->allMainWindows()[0];
    
    init(true);
    
    HbLabel *noVideosLabel = mUiLoader->findWidget<HbLabel>(DOCML_NAME_NO_VIDEOS_LABEL);
    QVERIFY(noVideosLabel);
    
    mTestObject->mActivated = true;
    noVideosLabel->setVisible(false);
    mTestObject->setButtonShown(true);
    QVERIFY(noVideosLabel->isVisible());
    
    mTestObject->mActivated = false;
    noVideosLabel->setVisible(false);
    mTestObject->setButtonShown(false);
    QVERIFY(noVideosLabel->isVisible() == false);
}

// end of file
