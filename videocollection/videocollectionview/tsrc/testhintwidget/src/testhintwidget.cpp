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
    
    if (callInitialize)
    {
        QVERIFY(mTestObject->initialize() == 0);
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
    // getServiceIconStrings fails and returns -1;
    init(false);
    VideoCollectionViewUtilsData::mGetServiceIconStringsFails = true;
    QCOMPARE(mTestObject->initialize(), -1);
    cleanup();
    
    // succeed
    init(false);
    VideoCollectionViewUtilsData::mGetServiceIconStringsFails = false;
    VideoCollectionViewUtilsData::mIconString = "test";
    VideoCollectionViewUtilsData::mPressedString = "test2";
    QCOMPARE(mTestObject->initialize(), 0);
    QCOMPARE(mTestObject->mServiceIconString, VideoCollectionViewUtilsData::mIconString);
    QCOMPARE(mTestObject->mServiceIconPressedString, VideoCollectionViewUtilsData::mPressedString);
    
    // second call to initialize.
    VideoCollectionViewUtilsData::mIconString = "test3";
    VideoCollectionViewUtilsData::mPressedString = "test4";
    QCOMPARE(mTestObject->initialize(), 0);
    QCOMPARE(mTestObject->mServiceIconString, VideoCollectionViewUtilsData::mIconString);
    QCOMPARE(mTestObject->mServiceIconPressedString, VideoCollectionViewUtilsData::mPressedString);
    cleanup();
}

// ---------------------------------------------------------------------------
// testSetLevel
// ---------------------------------------------------------------------------
//
void TestHintWidget::testSetLevel()
{
    init(true);
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
    init(true);

    mTestObject->setVisible(false);
    
    // mServiceIconString empty.
    mTestObject->mServiceIconString = QString();
    mTestObject->activate();
    QVERIFY(mTestObject->mServiceIcon == 0);
    QVERIFY(mTestObject->isVisible() == true);
    QVERIFY(mTestObject->mActivated == true);
    
    // mServiceIconPressedString empty.
    mTestObject->mServiceIconString = "test";
    mTestObject->mServiceIconPressedString = QString();
    mTestObject->activate();
    QVERIFY(mTestObject->mServiceIcon == 0);
    QVERIFY(mTestObject->isVisible() == true);
    QVERIFY(mTestObject->mActivated == true);
    
    // successful case.
    mTestObject->mActivated = false;
    mTestObject->mServiceIconString = "qtg_mono_ovistore";
    mTestObject->mServiceIconPressedString = "qtg_mono_ovistore";

    HbLabel *noVideosLabel = mUiLoader->findWidget<HbLabel>(DOCML_NAME_NO_VIDEOS_LABEL);
    QVERIFY(noVideosLabel);
    noVideosLabel->setVisible(false);
    
    mTestObject->activate();
    QVERIFY(mTestObject->isVisible());
    QVERIFY(mTestObject->mServiceIcon != 0);
    QCOMPARE(mTestObject->mServiceIcon->iconName(), mTestObject->mServiceIconString);
    QCOMPARE(mTestObject->mServiceIcon->iconName(QIcon::Normal, QIcon::On), 
                mTestObject->mServiceIconPressedString);
    QVERIFY(noVideosLabel->isVisible()); // checks that updateUiComponents() has been called.
    QVERIFY(mTestObject->mActivated == true);
    
    cleanup();
}
 
// ---------------------------------------------------------------------------
// testDeactivate
// ---------------------------------------------------------------------------
//
void TestHintWidget::testDeactivate()
{
    init(true);
    
    mTestObject->setVisible(true);
    mTestObject->mActivated = true;
    
    // when serviceButton does not exist
    mTestObject->mServiceIcon = 0;
    HbDocumentLoader::mFindWidgetFails = true;
    mTestObject->deactivate();
    QVERIFY(mTestObject->mActivated == false);
    QVERIFY(mTestObject->isVisible() == false);
    
    // when icons are null, but servicebutton has non-null icon.
    mTestObject->mActivated = true;
    mTestObject->setVisible(true);
    HbDocumentLoader::mFindWidgetFails = false;
    HbPushButton *serviceButton = mUiLoader->findWidget<HbPushButton>(DOCML_NAME_HINT_BUTTON);
    QVERIFY(serviceButton);
    serviceButton->setIcon(HbIcon(QIcon(QPixmap(QSize(100,100)))));
    QVERIFY(serviceButton->icon().isNull() == false); // make sure test is valid.
    mTestObject->deactivate();
    QVERIFY(serviceButton->icon().isNull());
    QVERIFY(mTestObject->isVisible() == false);
    
    // when icons are non-null, but widget is not activated.
    mTestObject->setVisible(true);
    mTestObject->mServiceIcon = new HbIcon;
    mTestObject->deactivate();
    QVERIFY(mTestObject->mServiceIcon != 0);
    QVERIFY(mTestObject->isVisible() == true);
    
    // when icons are non-null and widget is activated.
    mTestObject->mActivated = true;
    mTestObject->deactivate();
    QVERIFY(mTestObject->mServiceIcon == 0);
    QVERIFY(mTestObject->isVisible() == false);
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testUpdateUiComponents
// ---------------------------------------------------------------------------
//
void TestHintWidget::testUpdateUiComponents()
{
    HbMainWindow *mainWnd = hbInstance->allMainWindows()[0];
    mainWnd->setOrientation(Qt::Horizontal);
    
    init(true);
    
    mTestObject->mServiceIcon = new HbIcon(QIcon(QPixmap(QSize(100,100))));
    mTestObject->mActivated = true;
    
    HbDocumentLoader::mFindWidgetFails = true;
    // when a widget cannot be found.
    mTestObject->setLevel(VideoHintWidget::AllVideos);
    // no verification possible except that it doesn't crash?
    
    HbDocumentLoader::mFindWidgetFails = false;
    HbLabel *hintLabel = mUiLoader->findWidget<HbLabel>(DOCML_NAME_HINT_LABEL);
    QVERIFY(hintLabel);
    HbPushButton *serviceButton = mUiLoader->findWidget<HbPushButton>(DOCML_NAME_HINT_BUTTON);
    QVERIFY(serviceButton);
    
    mainWnd->setOrientation(Qt::Vertical);
    mTestObject->setLevel(VideoHintWidget::AllVideos);
    QVERIFY(serviceButton->isVisible() == false);
    QVERIFY(serviceButton->icon().isNull() == false);
    QVERIFY(serviceButton->icon().qicon().cacheKey() == mTestObject->mServiceIcon->qicon().cacheKey());
    QVERIFY(hintLabel->isVisible());
    
    mainWnd->setOrientation(Qt::Horizontal);
    mTestObject->mButtonShown = true;
    mTestObject->setLevel(VideoHintWidget::Collection);
    QVERIFY(!serviceButton->isVisible());
    QVERIFY(serviceButton->icon().isNull() == false);
    QVERIFY(hintLabel->isVisible() == false);
    
    mTestObject->mButtonShown = false;
    mTestObject->setLevel(VideoHintWidget::Collection);
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
    
    init(true);
    
    HbLabel *noVideosLabel = mUiLoader->findWidget<HbLabel>(DOCML_NAME_NO_VIDEOS_LABEL);
    QVERIFY(noVideosLabel);

    mTestObject->mServiceIconString = "dummy";
    mTestObject->mServiceIconPressedString = "dummy2";
    
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
