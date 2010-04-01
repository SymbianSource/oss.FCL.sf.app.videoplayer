/**
* Copyright	(c)	2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials	are	made available
* under	the	terms of "Eclipse Public License v1.0"
* which	accompanies	this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia	Corporation	- initial contribution.
*
* Contributors:
*
* Description:	 tester	for	methods	in VideoListView
*
*/

#include <QtTest/QtTest>
#include <QMap>
#include <hbapplication.h>
#include <hbinstance.h>
#include <qactiongroup.h>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <videocollectioncommon.h>
#include <hbstackedwidget.h>
#include <hbgroupbox.h>

#include "dummyhbmenu.h"
#include "videocollectionwrapper.h"
#include "testlistview.h"
#include "videolistwidget.h"
#include "videocollectionviewutils.h"
#include "videolistselectiondialog.h"
#include "videosortfilterproxymodel.h"
#include "videocollectionwrapper.h"
#include "dummydatamodel.h"
#include "videocollectionuiloader.h"
#include "videohintwidget.h"

#define	private	public
#include "videolistview.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int	main(int argc, char	*argv[])
{
	HbApplication app(argc,	argv);
	HbMainWindow window;

	TestListView tv;

	char *pass[3];
	pass[0]	= argv[0];
	pass[1]	= "-o";
	pass[2]	= "c:\\data\\testlistview.txt";

	int	res	= QTest::qExec(&tv,	3, pass);

	return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestListView::init(bool initTestView)
{
    VideoListWidget::mInitializeCount = 0;
    VideoListWidget::mInitializeReturnValue = 0;
    VideoListWidget::mActivateCount = 0;
    VideoListWidget::mActivateReturnValue = 0;
    VideoListWidget::mActive = false;
    VideoListWidget::mDeactivateCount = 0;
	VideoCollectionWrapper::mOpenFails	= false;
	VideoCollectionWrapper::mGetModelFails	= false;
	VideoCollectionViewUtils::mSortRole = -1;
    VideoListSelectionDialog::mSelectionCount =	0;
	VideoListSelectionDialog::mMultiSelectionLaunchCount = 0;
	VideoListSelectionDialog::mMultiSelectionItemSelectionCount	= 0;
	VideoListSelectionDialog::mExecReturnPrimary = false;
	VideoCollectionViewUtils::mSortOrder = Qt::AscendingOrder;
	VideoSortFilterProxyModel::mOpenFails =	0;
	VideoSortFilterProxyModel::mDoSortingCallCount = 0;

	mUiLoader = new VideoCollectionUiLoader();

	mUiLoader->reset();

	bool ok(false);

	QList<QObject *> objects = mUiLoader->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);

    if (!ok)
    {
    	return;
    }

    mTestView = mUiLoader->findObject<VideoListView>( DOCML_NAME_VIEW );

    QVERIFY(mTestView);
    
    mListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);

    if(initTestView)
	{
		QVERIFY(mTestView->initializeView()	== 0);
	    VideoSortFilterProxyModel::mDoSortingCallCount = 0;
        VideoSortFilterProxyModel::mSortRole = -1;
        VideoSortFilterProxyModel::mSortOrder = Qt::AscendingOrder;
	}
	mWrapper = VideoCollectionWrapper::instance();
	QVERIFY(mWrapper->open(VideoListWidget::ELevelVideos) != 0);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestListView::cleanup()
{
	delete mTestView;
	mTestView =	0;
	if(mWrapper)
	{
	    mWrapper->decreaseReferenceCount();
	    QCOMPARE(mWrapper->mReferenceCount, 0);
    }

	disconnect();
}

// ---------------------------------------------------------------------------
// visibleMenuActions
// ---------------------------------------------------------------------------
//
int	TestListView::visibleMenuActions()
{
	int	visibleActions(0);
	VideoListView::TViewActionIds key;
	HbAction *action(0);
	foreach(key,mTestView->mMenuActions.keys())
	{
		action = dynamic_cast<HbAction*>(mTestView->mMenuActions[key]);
		if(action && action->isVisible())
			visibleActions++;
	}
	return visibleActions;
}

// ---------------------------------------------------------------------------
// testCreateDelete
// ---------------------------------------------------------------------------
//
void TestListView::testCreateDelete()
{
    mWrapper = 0;

    mUiLoader = new VideoCollectionUiLoader();

	mUiLoader->reset();

	bool ok(false);

	QList<QObject *> objects = mUiLoader->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);

    if (!ok)
    {
    	return;
    }

    mTestView = mUiLoader->findObject<VideoListView>( DOCML_NAME_VIEW );

    delete mTestView;
	mTestView =	0;
}

// ---------------------------------------------------------------------------
// Slot: test initialize view
// ---------------------------------------------------------------------------
//
void TestListView::testInitializeView()
{
	init(false);
	// Test	videolist widget creation failure.
	VideoListWidget::mInitializeReturnValue	= -1;
	QVERIFY( mTestView->initializeView() < 0 );
	QVERIFY( mTestView->mVideoListWidget ==	0 );
	QVERIFY( mTestView->mVideoHintWidget == 0 );
	QVERIFY( mTestView->mToolbarViewsActionGroup == 0 );
	QVERIFY( mTestView->mToolbarCollectionActionGroup == 0 );
	QVERIFY( mTestView->mVideoServices == 0 );
	QVERIFY( mTestView->mIsService == false );

	cleanup();

	// Test	widget creation	succeed.
	init(false);
    VideoCollectionViewUtils::mSortRole = Qt::DisplayRole;
    VideoCollectionViewUtils::mSortOrder = Qt::DescendingOrder;
	QVERIFY( mTestView->initializeView() ==	0 );
	// Verify
	QVERIFY( mTestView->mVideoListWidget !=	0 );
    QVERIFY( mTestView->mVideoHintWidget != 0 );
	QCOMPARE( VideoListWidget::mInitializeCount, 1 );
	QVERIFY( mTestView->mToolbarViewsActionGroup != 0 );
	QVERIFY( mTestView->mToolbarCollectionActionGroup != 0 );
	QCOMPARE( mTestView->mToolbarViewsActionGroup->actions().count(), 3 );
	QCOMPARE( mTestView->mToolbarCollectionActionGroup->actions().count(), 3 );
	QCOMPARE( mTestView->toolBar()->actions().count(), 3 );
	// to my understanding the all videos action should	always be first.
    QCOMPARE( mTestView->mToolbarViewsActionGroup->actions().first()->toolTip(), tr("All videos") );
	QVERIFY( mTestView->mToolbarViewsActionGroup->actions().first()->isChecked()	);
	QCOMPARE( VideoSortFilterProxyModel::mSortRole, (int)Qt::DisplayRole );
	QCOMPARE( VideoSortFilterProxyModel::mSortOrder, Qt::DescendingOrder );
	QVERIFY( VideoSortFilterProxyModel::mSortAsync == false );
	QVERIFY( mTestView->mVideoServices == 0 );
	QVERIFY( mTestView->mIsService == false );
	cleanup();
	cleanup();

	// Test	command	signal.
	init(true);
	connect( this, SIGNAL(testSignal(int)),	mTestView->mVideoListWidget, SIGNAL(command(int)) );
	QSignalSpy spy(mTestView, SIGNAL(command(int)));
	emit testSignal(5);
	QCOMPARE( spy.count(), 1 );
	QList<QVariant>	arguments =	spy.takeFirst();
	QCOMPARE( arguments.at(0).toInt(), 5 );
	disconnect();
	cleanup();

	init(true);
	VideoListWidget* list = mTestView->mVideoListWidget;
	VideoHintWidget* hint = mTestView->mVideoHintWidget;
	QPointer<QActionGroup> toolbar = mTestView->mToolbarViewsActionGroup;
	QPointer<QActionGroup> toolbar2 = mTestView->mToolbarCollectionActionGroup;

	// Test	calling	initializeView again.
	QVERIFY( mTestView->initializeView() ==	0 );

	QVERIFY( toolbar !=	0 );
	QVERIFY( toolbar2 != 0 );
	QVERIFY( mTestView->mVideoListWidget ==	list );
	QVERIFY( mTestView->mVideoHintWidget == hint );
	QVERIFY( mTestView->mToolbarViewsActionGroup == toolbar );
	QVERIFY( mTestView->mToolbarCollectionActionGroup == toolbar2 );

	// Test	view deletion.
	cleanup();
	QVERIFY( toolbar ==	0 );
	QVERIFY( toolbar2 == 0 );

	// Test	get	model failure.
	init(false);
	QVERIFY(mTestView);
	VideoCollectionWrapper::mGetModelFails	= true;
	QVERIFY( mTestView->initializeView() < 0 );
	QVERIFY( mTestView->mVideoListWidget ==	0 );
	QVERIFY( mTestView->mVideoHintWidget	== 0 );
	QVERIFY( mTestView->mToolbarViewsActionGroup	== 0 );
	QVERIFY( mTestView->mToolbarCollectionActionGroup	== 0 );
	cleanup();

	// Test	model open failure.
	init(false);
	QVERIFY(mTestView);
	VideoSortFilterProxyModel::mOpenFails =	true;
	QVERIFY( mTestView->initializeView() < 0 );
	QVERIFY( mTestView->mVideoListWidget ==	0 );
	QVERIFY( mTestView->mVideoHintWidget == 0 );
	QVERIFY( mTestView->mToolbarViewsActionGroup == 0 );
	QVERIFY( mTestView->mToolbarCollectionActionGroup == 0 );
	cleanup();

	// Test sorting value load 'failure'.
	init(false);
    VideoCollectionViewUtils::mSortRole = Qt::DisplayRole;
    VideoCollectionViewUtils::mSortOrder = Qt::DescendingOrder;
    VideoCollectionViewUtils::mLoadSortingValuesFails = true;
    QVERIFY( mTestView->initializeView() == 0 );
    QCOMPARE( VideoSortFilterProxyModel::mSortRole, (int)VideoCollectionCommon::KeyDateTime );
    QCOMPARE( VideoSortFilterProxyModel::mSortOrder, Qt::AscendingOrder );
    QVERIFY( VideoSortFilterProxyModel::mSortAsync == false );
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test create menu
// ---------------------------------------------------------------------------
//
void TestListView::testCreateListMenu()
{
	init(false);
	VideoListWidget::mInitializeReturnValue	= -1;
	QVERIFY( mTestView->initializeView() < 0 );
	HbMenu*	menu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
	QVERIFY( menu );
	QCOMPARE( menu->actions().count(), 4 );

	cleanup();

	init(false);
	VideoListWidget::mInitializeReturnValue	= 0;
	QVERIFY( mTestView->initializeView() ==	0 );
	menu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
	QVERIFY( menu );
	QCOMPARE( menu->actions().count(), 4 );

	HbAction* sortAction = dynamic_cast<HbAction*>(menu->actions().at(3));
	QVERIFY( sortAction	);
	menu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
	QVERIFY(menu !=	0);
	QVERIFY( menu->actions().count() ==	4 );

	QVERIFY( mTestView->mMenuActions.count() ==	9 );
	QVERIFY( mTestView->mSortingRoles.count() == 5 );

	cleanup();
}

// ---------------------------------------------------------------------------
// testMenus
// ---------------------------------------------------------------------------
//
void TestListView::testMenus()
{
	init();

	HbAction* action;
	QList<QAction*> tbActions = mTestView->toolBar()->actions();
	QList<QAction*>	tbGroupActions;
	QList<QAction*>	tbGroupActions2;

	// All videos is open for default, verify.
	QVERIFY(mTestView->mToolbarViewsActionGroup != 0);
	QVERIFY(mTestView->mToolbarCollectionActionGroup != 0);
	tbGroupActions = mTestView->mToolbarViewsActionGroup->actions();
	tbGroupActions2 = mTestView->mToolbarCollectionActionGroup->actions();
	QVERIFY(mTestView->mToolbarActions.count() == 6);
	QVERIFY(tbActions.count() == 3);
	QVERIFY(tbGroupActions.count() == 3);
	QVERIFY(tbGroupActions2.count() == 3);
	QVERIFY( tbGroupActions.at(0)->isChecked() ); // First is checked.
	// visible menu items remains constant after init until
	// aboutToShowMainMenuSlot is called
	QVERIFY(visibleMenuActions() == 9);
	// Verify checkable	and	visible	toolbar	actions.
	HbAction* allVideosAction =	mTestView->mToolbarActions[VideoListView::ETBActionAllVideos];
	HbAction* collectionsAction	= mTestView->mToolbarActions[VideoListView::ETBActionCollections];
	HbAction* servicesAction = mTestView->mToolbarActions[VideoListView::ETBActionServices];
	HbAction* addVideosAction =	mTestView->mToolbarActions[VideoListView::ETBActionAddVideos];
	HbAction* removeVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos];
	HbAction* sortVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionSortVideos];
	QVERIFY( allVideosAction !=	0 && allVideosAction->isCheckable()	&& allVideosAction->isVisible() &&
			tbActions.at(0) == tbGroupActions.at(0));
	QVERIFY( collectionsAction != 0	&& collectionsAction->isCheckable()	&& collectionsAction->isVisible() &&
			tbActions.at(1) == tbGroupActions.at(1));
	QVERIFY( servicesAction	!= 0 &&	servicesAction->isCheckable() && servicesAction->isVisible() &&
			tbActions.at(2) == tbGroupActions.at(2));
	QVERIFY( addVideosAction !=	0 && !addVideosAction->isCheckable() &&
			tbActions.at(0) != tbGroupActions2.at(0));
	QVERIFY( removeVideosAction	!= 0 &&	!removeVideosAction->isCheckable() &&
			tbActions.at(1) != tbGroupActions2.at(1));
	QVERIFY( sortVideosAction != 0 && !sortVideosAction->isCheckable() &&
			tbActions.at(2) != tbGroupActions2.at(2));

	// Open	collections.
	action = dynamic_cast<HbAction*>(tbGroupActions.at(1));
	QVERIFY(action);
	action->setChecked(false);
	action->trigger();

	// Verify.
	QVERIFY(mTestView->mToolbarViewsActionGroup);
	tbGroupActions = mTestView->mToolbarViewsActionGroup->actions();
	QVERIFY(mTestView->mToolbarActions.count() == 6);
	QVERIFY(tbGroupActions.count() == 3);
	QVERIFY(tbGroupActions2.count() == 3);
	QVERIFY( tbGroupActions.at(1)->isChecked() ); // 2nd is	checked.
	// visible menu items remains constant after init until
	// aboutToShowMainMenuSlot is called
	QVERIFY(visibleMenuActions() ==	9);
	// Verify checkable	and	visible	toolbar	actions.
	allVideosAction	= mTestView->mToolbarActions[VideoListView::ETBActionAllVideos];
	collectionsAction =	mTestView->mToolbarActions[VideoListView::ETBActionCollections];
	servicesAction = mTestView->mToolbarActions[VideoListView::ETBActionServices];
	addVideosAction	= mTestView->mToolbarActions[VideoListView::ETBActionAddVideos];
	removeVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos];
	sortVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionSortVideos];
	QVERIFY( allVideosAction !=	0 && allVideosAction->isCheckable()	&& allVideosAction->isVisible()	&&
			tbActions.at(0) == tbGroupActions.at(0));
	QVERIFY( collectionsAction != 0	&& collectionsAction->isCheckable()	&& collectionsAction->isVisible() &&
			tbActions.at(1) == tbGroupActions.at(1));
	QVERIFY( servicesAction	!= 0 &&	servicesAction->isCheckable() && servicesAction->isVisible() &&
			tbActions.at(2) == tbGroupActions.at(2));
	QVERIFY( addVideosAction !=	0 && !addVideosAction->isCheckable() &&
			tbActions.at(0) != tbGroupActions2.at(0));
	QVERIFY( removeVideosAction	!= 0 &&	!removeVideosAction->isCheckable() &&
			tbActions.at(1) != tbGroupActions2.at(1));
	QVERIFY( sortVideosAction != 0 && !sortVideosAction->isCheckable() &&
			tbActions.at(2) != tbGroupActions2.at(2));

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test create toolbar
// ---------------------------------------------------------------------------
//
void TestListView::testCreateAction()
{
	init();

	QVERIFY(mTestView->mToolbarViewsActionGroup != 0);

	QList<QAction*>	actions	= mTestView->mToolbarViewsActionGroup->actions();
	actions.append(mTestView->mToolbarCollectionActionGroup->actions());
	QVERIFY(actions.count()	== 6);

	QCOMPARE( mTestView->toolBar()->actions().count(), 3 );

	HbAction* allVideos	= static_cast<HbAction*>(actions.at(0));
	QVERIFY( allVideos->isCheckable() );
    QCOMPARE( allVideos->toolTip(), tr("All videos") );
	QCOMPARE( allVideos->icon().iconName(),	QString(":/images/qtg_mono_video_all.svg")	);

	HbAction* collections =	static_cast<HbAction*>(actions.at(1));
	QVERIFY( collections->isCheckable()	);
	QCOMPARE( collections->toolTip(), tr("Collections")	);
	QCOMPARE( collections->icon().iconName(), QString(":/images/qtg_mono_video_collection.svg") );

	HbAction* services = static_cast<HbAction*>(actions.at(2));
	QVERIFY( services->isCheckable() );
	QCOMPARE( services->toolTip(), tr("Services") );
	QCOMPARE( services->icon().iconName(), QString(":/images/qtg_mono_video_services.svg") );

	HbAction* addVideos	= static_cast<HbAction*>(actions.at(3));
	QVERIFY( !addVideos->isCheckable() );
    QCOMPARE( addVideos->toolTip(), tr("Add videos") );
	QCOMPARE( addVideos->icon().iconName(),	QString(":/images/mono_video_addvideos.svg") );

	HbAction* removeVideos = static_cast<HbAction*>(actions.at(4));
	QVERIFY( !removeVideos->isCheckable() );
	QCOMPARE( removeVideos->toolTip(), tr("Remove videos") );
	QCOMPARE( removeVideos->icon().iconName(), QString(":/images/mono_video_removevideos.svg") );

	HbAction* sortVideos = static_cast<HbAction*>(actions.at(5));
	QVERIFY( !sortVideos->isCheckable()	);
	QCOMPARE( sortVideos->toolTip(), tr("Sort by") );
	QCOMPARE( sortVideos->icon().iconName(), QString(":/images/mono_video_sortvideos.svg") );

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test activate view
// ---------------------------------------------------------------------------
//
void TestListView::testActivateView()
{
	init();

	VideoListWidget* list = mTestView->mVideoListWidget;
	mTestView->mVideoListWidget = 0;

	QCOMPARE( mTestView->activateView(), -1	);
	QVERIFY( mTestView->mVideoListWidget->mActive == false );
	QCOMPARE( mTestView->mVideoListWidget->mActivateCount, 0 );
	QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 0 );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() == false );
	
	mTestView->mVideoListWidget = list;

	VideoListWidget::mActivateReturnValue =	-1;

	QCOMPARE( mTestView->activateView(), -1	);
	QVERIFY( mTestView->mVideoListWidget->mActive == false );
	QCOMPARE( mTestView->mVideoListWidget->mActivateCount, 1 );
	QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 1 );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() == false );

	VideoListWidget::mActivateReturnValue =	0;
	
	QCOMPARE( mTestView->activateView(), 0 );
	QVERIFY( mTestView->mVideoListWidget->mActive );
	QCOMPARE( mTestView->mVideoListWidget->mActivateCount, 2 );
	QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 1 );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() );

	QCOMPARE( mTestView->activateView(), 0 );
	QVERIFY( mTestView->mVideoListWidget->mActive );
	QCOMPARE( mTestView->mVideoListWidget->mActivateCount, 3 );
	QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 1 );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() );

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test deactivate view
// ---------------------------------------------------------------------------
//
void TestListView::testDeactivateView()
{
	init();

    VideoListWidget* list = mTestView->mVideoListWidget;
    mTestView->mVideoListWidget = 0;

	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() == false );
	QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 0 );

    mTestView->mVideoListWidget = list;

	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() == false );
	QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 1 );

	VideoListWidget::mActivateReturnValue =	0;
	mTestView->activateView();
	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() == false );
	QCOMPARE( mTestView->mVideoListWidget->mActivateCount, 1 );
	QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 2 );

	cleanup();
}

// ---------------------------------------------------------------------------
// testBack
// ---------------------------------------------------------------------------
//
void TestListView::testBack()
{
	init();

	mTestView->back();

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test open all videos slot
// ---------------------------------------------------------------------------
//
void TestListView::testOpenAllVideosViewSlot()
{
	init();

	QList<QAction*>	actions	= mTestView->mToolbarViewsActionGroup->actions();
	HbAction* action = static_cast<HbAction*>(actions.at(0));
	
	mTestView->mVideoListWidget->mType = VideoListWidget::EAllVideos;
	mTestView->mVideoListWidget->mCurrentLevel = VideoListWidget::ELevelCategory;
	mTestView->mSubLabel->setHeading("invalid");
	mTestView->mModel->mLevel = -1;
	
	// action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
	QCOMPARE( mTestView->mVideoListWidget->mCurrentLevel, VideoListWidget::ELevelVideos );
	QVERIFY( mTestView->mVideoListWidget->mActive == true );
	QCOMPARE( mTestView->mVideoListWidget->mActivateCount, 1 );
	QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 0 );
	QCOMPARE( mTestView->mSubLabel->heading(), tr("%1 videos").arg(mTestView->mModel->rowCount()) );
	QCOMPARE( mTestView->mModel->mLevel, (int)VideoListWidget::ELevelVideos );

	mTestView->activateView();
	
	mTestView->mVideoListWidget->mActivateCount = 0;
    mTestView->mVideoListWidget->mType = VideoListWidget::EAllVideos;
    mTestView->mVideoListWidget->mCurrentLevel = VideoListWidget::ELevelCategory;
    mTestView->mSubLabel->setHeading("invalid");
    mTestView->mModel->mLevel = -1;
    
    // action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
    QCOMPARE( mTestView->mVideoListWidget->mCurrentLevel, VideoListWidget::ELevelVideos );
    QVERIFY( mTestView->mVideoListWidget->mActive == true );
	QCOMPARE( mTestView->mVideoListWidget->mActivateCount, 1 );
	QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 0 );
    QCOMPARE( mTestView->mSubLabel->heading(), tr("%1 videos").arg(mTestView->mModel->rowCount()) );
    QCOMPARE( mTestView->mModel->mLevel, (int)VideoListWidget::ELevelVideos );

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test open collections view	slot
// ---------------------------------------------------------------------------
//
void TestListView::testOpenCollectionViewSlot()
{
	init();

	QList<QAction*>	actions	= mTestView->mToolbarViewsActionGroup->actions();
	HbAction* action = static_cast<HbAction*>(actions.at(1));

    mTestView->mVideoListWidget->mType = VideoListWidget::ECollections;
    mTestView->mVideoListWidget->mCurrentLevel = VideoListWidget::ELevelVideos;
    mTestView->mSubLabel->setHeading("invalid");
    mTestView->mModel->mLevel = -1;

	// action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
    QCOMPARE( mTestView->mVideoListWidget->mCurrentLevel, VideoListWidget::ELevelCategory );
    QVERIFY( mTestView->mVideoListWidget->mActive == true );
    QCOMPARE( mTestView->mVideoListWidget->mActivateCount, 1 );
    QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 0 );
    QCOMPARE( mTestView->mSubLabel->heading(), tr("%1 collections").arg(mTestView->mModel->rowCount()) );
    QCOMPARE( mTestView->mModel->mLevel, (int)VideoListWidget::ELevelCategory );

	mTestView->activateView();

    mTestView->mVideoListWidget->mActivateCount = 0;
    mTestView->mVideoListWidget->mType = VideoListWidget::ECollections;
    mTestView->mVideoListWidget->mCurrentLevel = VideoListWidget::ELevelVideos;
    mTestView->mSubLabel->setHeading("invalid");
    mTestView->mModel->mLevel = -1;
    
    // action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
    QCOMPARE( mTestView->mVideoListWidget->mCurrentLevel, VideoListWidget::ELevelCategory );
    QVERIFY( mTestView->mVideoListWidget->mActive == true );
    QCOMPARE( mTestView->mVideoListWidget->mActivateCount, 1 );
    QCOMPARE( mTestView->mVideoListWidget->mDeactivateCount, 0 );
    QCOMPARE( mTestView->mSubLabel->heading(), tr("%1 collections").arg(mTestView->mModel->rowCount()) );
    QCOMPARE( mTestView->mModel->mLevel, (int)VideoListWidget::ELevelCategory );

	cleanup();
}
// ---------------------------------------------------------------------------
// Slot: test open services	view slot
// ---------------------------------------------------------------------------
//
void TestListView::testOpenServicesViewSlot()
{
	init();

	QList<QAction*>	actions	= mTestView->mToolbarViewsActionGroup->actions();
	HbAction* action = static_cast<HbAction*>(actions.at(2));

	// action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
	
	cleanup();
}

// ---------------------------------------------------------------------------
// testStartSorting
// ---------------------------------------------------------------------------
//
void TestListView::testStartSortingSlot()
{
	HbAction *action(0);

	// Active action is	not	sort by.
	init(true);
	action = mTestView->mMenuActions[VideoListView::EActionDelete];
	QVERIFY(action != 0);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(action);
	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
	emit testSignal(0);
	QCOMPARE(VideoSortFilterProxyModel::mDoSortingCallCount, 0);
	QCOMPARE(VideoSortFilterProxyModel::mSortRole, -1);
	cleanup();

	// Sort	by action has no menu.
	init(true);

	// Change sort by action to	delete action which	has	no menu.
	HbAction *sortByAction = mTestView->mMenuActions[VideoListView::EActionSortBy];
	HbAction *deleteAction = mTestView->mMenuActions[VideoListView::EActionDelete];
	mTestView->mMenuActions[VideoListView::EActionSortBy] =	deleteAction;
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(deleteAction);

	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
	emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModel::mDoSortingCallCount, 0);
    QCOMPARE(VideoSortFilterProxyModel::mSortRole, -1);

	// Change the action back.
	mTestView->mMenuActions[VideoListView::EActionSortBy] =	sortByAction;

	cleanup();

	// View	is not initialized.
	init(false);
	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
	emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModel::mDoSortingCallCount, 0);
    QCOMPARE(VideoSortFilterProxyModel::mSortRole, -1);
	cleanup();

	// Good	case.
	init();
	action = mTestView->mMenuActions[VideoListView::EActionSortBy];
	QVERIFY(action != 0);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(action);
	HbAction* sortAction = mTestView->mMenuActions[VideoListView::EActionSortByName];
	QVERIFY(sortAction != 0);
	action->menu()->setActiveAction(sortAction);

	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
	emit testSignal(0);
	QCOMPARE(VideoSortFilterProxyModel::mDoSortingCallCount, 1);
	QCOMPARE(VideoSortFilterProxyModel::mSortRole, (int)Qt::DisplayRole);
	QCOMPARE(VideoSortFilterProxyModel::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoSortFilterProxyModel::mSortAsync);

    // emit test signal again, with same parameters. Sorting should be switched to
    // descending order.
    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModel::mDoSortingCallCount, 2);
    QCOMPARE(VideoSortFilterProxyModel::mSortRole, (int)Qt::DisplayRole);
    QCOMPARE(VideoSortFilterProxyModel::mSortOrder, Qt::DescendingOrder);
    QVERIFY(VideoSortFilterProxyModel::mSortAsync);

    // on third emit, sorting should be switched back to ascending
    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModel::mDoSortingCallCount, 3);
    QCOMPARE(VideoSortFilterProxyModel::mSortRole, (int)Qt::DisplayRole);
    QCOMPARE(VideoSortFilterProxyModel::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoSortFilterProxyModel::mSortAsync);

    // test that after changing the sort role, the order is also switched to ascending.
    VideoSortFilterProxyModel::mSortOrder = Qt::DescendingOrder;
    sortAction = mTestView->mMenuActions[VideoListView::EActionSortByDate];
    QVERIFY(sortAction != 0);
    action->menu()->setActiveAction(sortAction);

    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModel::mDoSortingCallCount, 4);
    QCOMPARE(VideoSortFilterProxyModel::mSortRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(VideoSortFilterProxyModel::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoSortFilterProxyModel::mSortAsync);

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test orientation related slots
// ---------------------------------------------------------------------------
//
void TestListView::testOrientationSlots()
{
	init();

    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
	mainWnd->addView(mTestView);
	mainWnd->setOrientation(Qt::Horizontal);
	mainWnd->setOrientation(Qt::Vertical);
	
	cleanup();
}

// ---------------------------------------------------------------------------
// testModelReadySlot
// ---------------------------------------------------------------------------
//
void TestListView::testModelReadySlot()
{
    init();
    mTestView->mModelReady = false;
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(modelReadySlot()) );
    emit testSignal();
    QVERIFY(mTestView->mModelReady);
    cleanup();
}

// ---------------------------------------------------------------------------
// testDeleteItemsSlot
// ---------------------------------------------------------------------------
//
void TestListView::testDeleteItemsSlot()
{
    init();
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(deleteItemsSlot()) );
    ////////////
    // no model
    ////////////
    VideoSortFilterProxyModel *tmpModel = mTestView->mModel;
    mTestView->mModel = 0;
    emit testSignal();
    QVERIFY(VideoListSelectionDialog::mMultiSelectionLaunchCount == 0);
    QVERIFY(VideoListSelectionDialog::mMultiSelectionItemSelectionCount == 0);
    mTestView->mModel = tmpModel;

    cleanup();
    init();
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(deleteItemsSlot()) );

    ////////////
    // no selection dialog
    ////////////
    emit testSignal();
    QVERIFY(VideoListSelectionDialog::mMultiSelectionLaunchCount == 1);
    QVERIFY(VideoListSelectionDialog::mMultiSelectionItemSelectionCount == 0);

    cleanup();
    init();
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(deleteItemsSlot()) );

    ////////////
    // dialog loading fails
    ////////////
    VideoCollectionUiLoader::mFailDialogLoad = true;
    emit testSignal();
    QVERIFY(VideoListSelectionDialog::mMultiSelectionLaunchCount == 0);
    QVERIFY(VideoListSelectionDialog::mMultiSelectionItemSelectionCount == 0);
    VideoCollectionUiLoader::mFailDialogLoad = false;
    //////////
    // document loader deletes all objects in case of error, so we cannot cleanup all
    if(mWrapper)
    {
        mWrapper->decreaseReferenceCount();
        QCOMPARE(mWrapper->mReferenceCount, 0);
    }
    disconnect();
    ///////////////
    init();
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(deleteItemsSlot()) );

    ////////////
    // selection dialog exists
    ////////////
    emit testSignal();
    QVERIFY(VideoListSelectionDialog::mMultiSelectionLaunchCount == 1);
    QVERIFY(VideoListSelectionDialog::mMultiSelectionItemSelectionCount == 0);
    emit testSignal();
    QVERIFY(VideoListSelectionDialog::mMultiSelectionLaunchCount == 2);
    QVERIFY(VideoListSelectionDialog::mMultiSelectionItemSelectionCount == 0);

    cleanup();
    init();
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(deleteItemsSlot()) );

    ////////////
    // exec returns primary key (just for the coverity's sake)
    ////////////
    VideoListSelectionDialog:: mExecReturnPrimary = true;
    VideoListSelectionDialog::mSelectionCount = 5;
    emit testSignal();
    QVERIFY(VideoListSelectionDialog::mMultiSelectionLaunchCount == 1);
    QVERIFY(VideoListSelectionDialog::mMultiSelectionItemSelectionCount == 5);

    cleanup();
}

// ---------------------------------------------------------------------------
// testAboutToShowMainMenuSlot
// ---------------------------------------------------------------------------
//
void TestListView::testAboutToShowMainMenuSlot()
{
	// All ok.
	init();
	mWrapper->getDataModel()->setRowCount(1);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	QVERIFY(visibleMenuActions() ==	6);
	cleanup();

	// View	is not initialized.
	init(false);
	mWrapper->getDataModel()->setRowCount(1);
	connect( this, SIGNAL(testSignal(int)),	mTestView, SLOT(aboutToShowMainMenuSlot()) );
	emit testSignal(0);
	cleanup();

	HbAction *action(0);

	// Collections is triggered	from toolbar.
	init();
	mWrapper->getDataModel()->setRowCount(1);
	action = mTestView->mToolbarActions[VideoListView::ETBActionCollections];
	QVERIFY(action != 0);
	action->setChecked(false);
	action->trigger();
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	int visible = visibleMenuActions();
	QCOMPARE(visible,	5);
	cleanup();

	// All videos is triggered from	toolbar.
	init();
	mWrapper->getDataModel()->setRowCount(1);
	action = mTestView->mToolbarActions[VideoListView::ETBActionAllVideos];
	QVERIFY(action != 0);
	action->setChecked(false);
	action->trigger();
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	visible = visibleMenuActions();
	QCOMPARE(visible,	6);
	cleanup();

	// Services	is triggered from toolbar.
	init();
	mWrapper->getDataModel()->setRowCount(1);
	action = mTestView->mToolbarActions[VideoListView::ETBActionServices];
	QVERIFY(action != 0);
	action->setChecked(false);
	action->trigger();
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	visible = visibleMenuActions();
	QCOMPARE(visible,	0);
	cleanup();

	// Add videos action is	visible.
	init();
	mTestView->toolBar()->clearActions();
	mTestView->toolBar()->addActions( mTestView->mToolbarCollectionActionGroup->actions() );

	mWrapper->getDataModel()->setRowCount(1);
	QVERIFY(action != 0);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	visible = visibleMenuActions();
	QCOMPARE(visible,	1);
	cleanup();

	// Menu	action is null
	init();
	mWrapper->getDataModel()->setRowCount(1);
	action = mTestView->mMenuActions.take(VideoListView::EActionSortBy);
	delete action;
	action = NULL;
	mTestView->mMenuActions[VideoListView::EActionSortBy] =	action;
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	visible = visibleMenuActions();
	QCOMPARE(visible,	0);
	cleanup();

	// No menu actions.
	init();
	mWrapper->getDataModel()->setRowCount(1);
	QMap<VideoListView::TViewActionIds,	HbAction*> backupActions =
		QMap<VideoListView::TViewActionIds,	HbAction*>(mTestView->mMenuActions);
	mTestView->mMenuActions.clear();
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	mTestView->mMenuActions.unite(backupActions);
	cleanup();

	// Model has no	items.
	init();
	mWrapper->getDataModel()->setRowCount(0);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	visible = visibleMenuActions();
	QCOMPARE(visible,	0);
	cleanup();

	// Toolbar action group	is null
	init();
	mWrapper->getDataModel()->setRowCount(1);
	QActionGroup* actionGroup =	mTestView->mToolbarViewsActionGroup;
	mTestView->mToolbarViewsActionGroup = NULL;
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	mTestView->mToolbarViewsActionGroup = actionGroup;
	cleanup();
}

// ---------------------------------------------------------------------------
// testOpenSortByMenuSlot
// ---------------------------------------------------------------------------
//
void TestListView::testOpenSortByMenuSlot()
{
    DummyHbMenu *dummyMenu = new DummyHbMenu;
    HbMenu *tmpMenu;

    // All ok.
    init();
    tmpMenu =  mTestView->mSortMenu;
    mTestView->mSortMenu = dummyMenu;
    connect( this, SIGNAL(testSignal(int)), mTestView, SLOT(openSortByMenuSlot()) );
    mWrapper->getDataModel()->setRowCount(1);
    mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->hide();
    emit testSignal(0);
    // sorting menu should be visible
    QVERIFY(visibleMenuActions() == 4);
    mTestView->mSortMenu = tmpMenu;
    cleanup();

    // View is not initialized.
    init(false);
    tmpMenu =  mTestView->mSortMenu;
    mTestView->mSortMenu = dummyMenu;
    connect( this, SIGNAL(testSignal(int)), mTestView, SLOT(openSortByMenuSlot()) );
    mWrapper->getDataModel()->setRowCount(1);
    mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->hide();
    emit testSignal(0);
    QCOMPARE(visibleMenuActions(),   0);
    mTestView->mSortMenu = tmpMenu;
    cleanup();

    // no items
    init();
    tmpMenu =  mTestView->mSortMenu;
    mTestView->mSortMenu = dummyMenu;
    connect( this, SIGNAL(testSignal(int)), mTestView, SLOT(openSortByMenuSlot()) );
    mWrapper->getDataModel()->setRowCount(0);
    mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->hide();
    emit testSignal(0);
    QCOMPARE(visibleMenuActions(),   0);
    mTestView->mSortMenu = tmpMenu;
    cleanup();

    delete dummyMenu;

}

// ---------------------------------------------------------------------------
// testHandleErrorSlot
// ---------------------------------------------------------------------------
//
void TestListView::testHandleStatusSlot()
{
	QVariant *variant =	new	QVariant();

	// Unknown error code.
	init();
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(0, *variant);
	QVERIFY(VideoCollectionViewUtils::mLastError ==	0);
	cleanup();

	// Multiple	deletion error.
	init();
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(VideoCollectionCommon::statusMultipleDeleteFail, *variant);
	QVERIFY(VideoCollectionViewUtils::mLastError ==	VideoCollectionCommon::statusMultipleDeleteFail);
	cleanup();

	// Deletion	error.
	init();
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(VideoCollectionCommon::statusSingleDeleteFail, *variant);
	QVERIFY(VideoCollectionViewUtils::mLastError ==	VideoCollectionCommon::statusSingleDeleteFail);
	cleanup();

	// status multidelete succeed
    init();
    connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
    emit testStatusSignal(VideoCollectionCommon::statusMultipleDeleteSucceed, *variant);
    QVERIFY(VideoCollectionViewUtils::mLastError == VideoCollectionCommon::statusMultipleDeleteSucceed);
    cleanup();

	// No model.
	init(false);
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(1,	*variant);
	QVERIFY(VideoCollectionViewUtils::mLastError ==	1);
	cleanup();

	delete variant;
}

// ---------------------------------------------------------------------------
// testCollectionOpenedSlot
// ---------------------------------------------------------------------------
//
void TestListView::testCollectionOpenedSlot()
{
	HbAction* removeVideosAction;
	HbAction* allVideosAction;
	HbAction* collectionsAction;
	HbAction* servicesAction;
	HbAction* addVideosAction;
	HbAction* sortVideosAction;

	QString testString;

	// View	is not initialized.
	init(false);
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&)),	mTestView, SLOT(collectionOpenedSlot(bool, const QString&)));
	emit testCollectionOpenedSignal(false, testString);
	cleanup();

	// No toolbar actions.
	init(true);
	// Clear actions.
	QMap<VideoListView::TViewActionIds,	HbAction*> backupActions =
		QMap<VideoListView::TViewActionIds,	HbAction*>(mTestView->mToolbarActions);
	mTestView->mToolbarActions.clear();
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&)),	mTestView, SLOT(collectionOpenedSlot(bool, const QString&)));
	emit testCollectionOpenedSignal(true, testString);
	mTestView->mToolbarActions.unite(backupActions);
	cleanup();

	// One of the toolbar actions is null.
	init();
	removeVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos];
	mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos] = NULL;
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&)),	mTestView, SLOT(collectionOpenedSlot(bool, const QString&)));
	emit testCollectionOpenedSignal(true, testString);
	// Verify checkable	and	visible	toolbar	actions.
	mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos] = removeVideosAction;
	allVideosAction	= mTestView->mToolbarActions[VideoListView::ETBActionAllVideos];
	collectionsAction =	mTestView->mToolbarActions[VideoListView::ETBActionCollections];
	servicesAction = mTestView->mToolbarActions[VideoListView::ETBActionServices];
	addVideosAction	= mTestView->mToolbarActions[VideoListView::ETBActionAddVideos];
	sortVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionSortVideos];
	// Remove videos is	not	checked	because	it was used	as null	action.
	QCOMPARE( mTestView->mCollectionName, testString );
	QCOMPARE( mTestView->toolBar()->actions().count(), 3 );
	QVERIFY( allVideosAction !=	0 && allVideosAction->isCheckable());
	QVERIFY( collectionsAction != 0	&& collectionsAction->isCheckable());
	QVERIFY( servicesAction	!= 0 &&	servicesAction->isCheckable());
	QVERIFY( addVideosAction !=	0 && !addVideosAction->isCheckable() );
	QVERIFY( sortVideosAction != 0 && !sortVideosAction->isCheckable() );
	cleanup();


	testString = "Test text string";

	// Collection opened.
	init();
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&)),	mTestView, SLOT(collectionOpenedSlot(bool, const QString&)));
	emit testCollectionOpenedSignal(true, testString);
	// Verify checkable	and	visible	toolbar	actions.
	allVideosAction	= mTestView->mToolbarActions[VideoListView::ETBActionAllVideos];
	collectionsAction =	mTestView->mToolbarActions[VideoListView::ETBActionCollections];
	servicesAction = mTestView->mToolbarActions[VideoListView::ETBActionServices];
	addVideosAction	= mTestView->mToolbarActions[VideoListView::ETBActionAddVideos];
	removeVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos];
	sortVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionSortVideos];
	QCOMPARE(mTestView->mCollectionName, testString);
	QCOMPARE( mTestView->toolBar()->actions().count(), 3 );
	QVERIFY( allVideosAction !=	0 && allVideosAction->isCheckable());
	QVERIFY( collectionsAction != 0	&& collectionsAction->isCheckable());
	QVERIFY( servicesAction	!= 0 &&	servicesAction->isCheckable());
	QVERIFY( addVideosAction !=	0 && !addVideosAction->isCheckable() );
	QVERIFY( removeVideosAction	!= 0 &&	!removeVideosAction->isCheckable() );
	QVERIFY( sortVideosAction != 0 && !sortVideosAction->isCheckable() );
	cleanup();

	testString = "Test text string 2";

	// Collection not opened.
	init();
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&)),	mTestView, SLOT(collectionOpenedSlot(bool, const QString&)));
	emit testCollectionOpenedSignal(false, testString);
	// Verify checkable	and	visible	toolbar	actions.
	allVideosAction	= mTestView->mToolbarActions[VideoListView::ETBActionAllVideos];
	collectionsAction =	mTestView->mToolbarActions[VideoListView::ETBActionCollections];
	servicesAction = mTestView->mToolbarActions[VideoListView::ETBActionServices];
	addVideosAction	= mTestView->mToolbarActions[VideoListView::ETBActionAddVideos];
	removeVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos];
	sortVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionSortVideos];
	QCOMPARE(mTestView->mCollectionName, testString);
	QCOMPARE( mTestView->toolBar()->actions().count(), 3 );
	QVERIFY( allVideosAction !=	0 && allVideosAction->isCheckable());
	QVERIFY( collectionsAction != 0	&& collectionsAction->isCheckable());
	QVERIFY( servicesAction	!= 0 &&	servicesAction->isCheckable());
	QVERIFY( addVideosAction !=	0 && !addVideosAction->isCheckable() );
	QVERIFY( removeVideosAction	!= 0 &&	!removeVideosAction->isCheckable() );
	QVERIFY( sortVideosAction != 0 && !sortVideosAction->isCheckable() );
	cleanup();
}

// ---------------------------------------------------------------------------
// testUpdateSubLabel
// ---------------------------------------------------------------------------
//
void TestListView::testUpdateSubLabel()
{
	init(true);
	connect(this, SIGNAL(testLayoutChangedSignal()), mTestView, SLOT(layoutChangedSlot()));
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&)), mTestView, SLOT(collectionOpenedSlot(bool, const QString&)));
	mWrapper->getDataModel()->setRowCount(1);

	mTestView->mVideoListWidget->mType = VideoListWidget::EAllVideos;
	
	emit testLayoutChangedSignal();

	HbGroupBox* label = mUiLoader->findWidget<HbGroupBox>(DOCML_NAME_VC_HEADINGBANNER);

	QString returnString = label->heading();
	QString expectedString = "1 videos";

	QCOMPARE(returnString, expectedString);

	mTestView->mVideoListWidget->mType = VideoListWidget::EUnknown;

	emit testLayoutChangedSignal();

	returnString = label->heading();
	expectedString = "1 videos";

	QCOMPARE(returnString, expectedString);

	mTestView->mVideoListWidget->mType = VideoListWidget::ECollections;

	emit testLayoutChangedSignal();

	returnString = label->heading();
	expectedString = "1 collections";

	QCOMPARE(returnString, expectedString);

	mTestView->mVideoListWidget->mType = VideoListWidget::EUnknown;

	emit testLayoutChangedSignal();

	returnString = label->heading();
	expectedString = "1 collections";

	QCOMPARE(returnString, expectedString);

	mTestView->mVideoListWidget->mType = VideoListWidget::EDefaultColItems;

	emit testLayoutChangedSignal();

	returnString = label->heading();
	expectedString = " (1)";

	QCOMPARE(returnString, expectedString);

	mTestView->mVideoListWidget->mType = VideoListWidget::EUnknown;
	expectedString = " (1)";

	emit testLayoutChangedSignal();

	returnString = label->heading();

	QCOMPARE(returnString, expectedString);

	mTestView->mVideoListWidget->mType = VideoListWidget::EUserColItems;

	emit testCollectionOpenedSignal(true, QString("NAME"));
	emit testLayoutChangedSignal();

	returnString = label->heading();
	expectedString = "NAME (1)";

	QCOMPARE(returnString, expectedString);
	
    mTestView->mVideoListWidget->mType = VideoListWidget::EAllVideos;
    VideoSortFilterProxyModel* model = mTestView->mModel;
    mTestView->mModel = 0;
    expectedString = "testtesttesttest";
    label->setHeading(expectedString);
    
    // should not crash.
    emit testLayoutChangedSignal();
    
    // make sure that the heading has not changed.
    QCOMPARE(label->heading(), expectedString);
}

// ---------------------------------------------------------------------------
// testShowHint
// ---------------------------------------------------------------------------
//
void TestListView::testShowHint()
{
    init(true);
    connect(this, SIGNAL(testLayoutChangedSignal()), mTestView, SLOT(layoutChangedSlot()));
    mWrapper->getDataModel()->setRowCount(1);
    
    mTestView->mModelReady = false;
    // tests that the showhint is not called, and currently only way of doing that
    // is to set hint widget visible and check that it is not hidden.
    mTestView->mVideoHintWidget->setVisible(true);
    
    emit testLayoutChangedSignal();

    QVERIFY(mTestView->mVideoHintWidget->isVisible());
    
    VideoSortFilterProxyModel* model = mTestView->mModel;
    mTestView->mModel = 0;
    mTestView->mModelReady = true;
    
    emit testLayoutChangedSignal();

    QVERIFY(mTestView->mVideoHintWidget->isVisible() == false);
    
    mTestView->mModel = model;
    mWrapper->getDataModel()->setRowCount(0);
    mTestView->mVideoHintWidget->setVisible(true);
    VideoHintWidget* hint = mTestView->mVideoHintWidget;
    mTestView->mVideoHintWidget = 0;
    
    // should not crash. No other verification possible.
    emit testLayoutChangedSignal();
    
    mTestView->mVideoHintWidget = hint;
    mTestView->mVideoHintWidget->setVisible(false);
    
    emit testLayoutChangedSignal();
    
    QVERIFY(mTestView->mVideoHintWidget->isVisible());
    
    mWrapper->getDataModel()->setRowCount(1);
    mTestView->mVideoHintWidget->setVisible(true);
    
    emit testLayoutChangedSignal();

    QVERIFY(mTestView->mVideoHintWidget->isVisible() == false);
}

// End of file
