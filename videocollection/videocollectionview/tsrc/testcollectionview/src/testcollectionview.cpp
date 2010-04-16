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
* Description:   tester for methods in VideoSortFilterProxyModel
*
*/
#include <hbaction.h>
#include <qpointer.h>
#include <hbapplication.h>
#include <hbinstance.h>
#include <hblabel.h>

#include "videocollectionuiloader.h"
#include "testcollectionview.h"
#include "videocollectioncommon.h"
#include "videolistview.h"

#include "videolistviewdata.h"
#include "videocollectionuiloaderdata.h"

#define private public
#include "videocollectionviewplugin.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    hbInstance->mWindowses.append(&window);

    TestCollectionView tv;

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
        pass[2] = "c:\\data\\testcollectionview.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

    return res;
}


// ---------------------------------------------------------------------------
// TestCollectionView
// ---------------------------------------------------------------------------
//
TestCollectionView::TestCollectionView()
 : mTestView(0)
{
}

// ---------------------------------------------------------------------------
// initTestCase
// ---------------------------------------------------------------------------
//
void TestCollectionView::initTestCase()
{
    // nop
}

// ---------------------------------------------------------------------------
// cleanupTestCase
// ---------------------------------------------------------------------------
//
void TestCollectionView::cleanupTestCase()
{
    // nop
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestCollectionView::init()
{
    cleanup();
    VideoCollectionUiLoaderData::reset();
    VideoListViewData::reset();
        
    mTestView = new VideoCollectionViewPlugin();
    mTestView->createView();
    
    HbMainWindow *window = hbInstance->allMainWindows().value(0);
    if (window)
    {
        window->addView(mTestView->getView());
    }
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestCollectionView::cleanup()
{
    HbMainWindow *window = hbInstance->allMainWindows().value(0);
    if (window)
    {
        QList<HbView*> views = window->views(); 
        int count = views.count();
        for (int i = 0; i < count; i++)
        {
            window->removeView(views.at(i));
        }
    }
    
    delete mTestView;
    mTestView = 0;
}

// ---------------------------------------------------------------------------
// Slot: test create view
// ---------------------------------------------------------------------------
//
void TestCollectionView::testCreateView()
{
    cleanup();
    VideoCollectionUiLoaderData::reset();
    VideoListViewData::reset();
    
    VideoCollectionUiLoaderData::mFindFailure = true;
	
	mTestView = new VideoCollectionViewPlugin();

    QVERIFY( mTestView->mView == 0 );

    mTestView->createView();

    QVERIFY( mTestView->mView == 0 );
    QVERIFY( mTestView->mUiLoader != 0);

    delete mTestView;

    VideoCollectionUiLoaderData::mFindFailure = false;
	
	mTestView = new VideoCollectionViewPlugin();

    QVERIFY( mTestView->mView == 0 );

    mTestView->createView();

    QVERIFY( mTestView->mView != 0 );
    QVERIFY( mTestView->mUiLoader != 0);
    QCOMPARE( VideoListViewData::mInitializeViewCount, 1 );
    QVERIFY( mTestView->mActivated == false );

    connect( this, SIGNAL(commandSignal(int)), mTestView->mView, SIGNAL(command(int)));
    QSignalSpy commandSpy(mTestView, SIGNAL(command(int)));
    emit commandSignal(5);
    QCOMPARE( commandSpy.count(), 1 );
    QList<QVariant> arguments = commandSpy.takeFirst();
    QCOMPARE( arguments.at(0).toInt(), 5 );
    disconnect();

    VideoListView* current = mTestView->mView;

    mTestView->createView();

    QCOMPARE( mTestView->mView, current );
    
    HbMainWindow *window = hbInstance->allMainWindows().value(0);
    if (window)
    {
        window->addView(current);
    }

    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test activate view
// ---------------------------------------------------------------------------
//
void TestCollectionView::testActivateView()
{
    init();

    mTestView->activateView();
    QVERIFY( mTestView->mActivated == true );
    QCOMPARE( VideoListViewData::mActivateViewCount, 1 );

    mTestView->activateView();
    QCOMPARE( VideoListViewData::mActivateViewCount, 1 );

    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test deactivate view
// ---------------------------------------------------------------------------
//
void TestCollectionView::testDeactivateView()
{
    init();

    mTestView->deactivateView();
    QCOMPARE( VideoListViewData::mDeactivateViewCount, 0 );
    QVERIFY( mTestView->mActivated == false );

    mTestView->activateView();

    mTestView->deactivateView();
    QCOMPARE( VideoListViewData::mDeactivateViewCount, 1 );
    QVERIFY( mTestView->mActivated == false );

    mTestView->deactivateView();
    QCOMPARE( VideoListViewData::mDeactivateViewCount, 1 );
    QVERIFY( mTestView->mActivated == false );

    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test destroy view
// ---------------------------------------------------------------------------
//
void TestCollectionView::testDestroyView()
{
    init();

    // remove view from mainwindow so that we don't get hanging pointers there, as
    // the test is creating and destroying view several times.
    HbMainWindow *window = hbInstance->allMainWindows().value(0);
    QVERIFY(window);
    window->removeView(mTestView->mView);
    
    mTestView->activateView();
    
    mTestView->destroyView();

    QCOMPARE( VideoListViewData::mDeactivateViewCount, 1 );
    QVERIFY( mTestView->mActivated == false );
    QVERIFY( mTestView->mView == 0 );
    QVERIFY( mTestView->mUiLoader == 0);

    mTestView->createView();
    QVERIFY( mTestView->mView != 0 );
    QVERIFY( mTestView->mUiLoader != 0);
    
    mTestView->destroyView();
    QCOMPARE( VideoListViewData::mDeactivateViewCount, 1 );
    QVERIFY( mTestView->mActivated == false );
    QVERIFY( mTestView->mView == 0 );
    QVERIFY( mTestView->mUiLoader == 0);

    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test get view
// ---------------------------------------------------------------------------
//
void TestCollectionView::testGetView()
{
    cleanup();
    mTestView = new VideoCollectionViewPlugin();

    QVERIFY( mTestView->getView() == 0 );
    mTestView->createView();
    QVERIFY( mTestView->getView() == mTestView->mView );
    mTestView->activateView();
    QVERIFY( mTestView->getView() == mTestView->mView );
    mTestView->deactivateView();
    QVERIFY( mTestView->getView() == mTestView->mView );
    mTestView->destroyView();
    QVERIFY( mTestView->getView() == 0 );

    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test get view
// ---------------------------------------------------------------------------
//
void TestCollectionView::testBack()
{
    init();

    mTestView->back();
    QCOMPARE( VideoListViewData::mBackCount, 0 );
    mTestView->activateView();
    mTestView->back();
    QCOMPARE( VideoListViewData::mBackCount, 1 );

    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test orientation change
// ---------------------------------------------------------------------------
//
void TestCollectionView::testOrientationChange()
{
    // there's nothing to be tested for this.
    // slot is probably going to be removed. This is here just to have the
    // function decision coverage on correct level.
    init();
    mTestView->orientationChange(Qt::Horizontal);
    cleanup();
}



// End of file

