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
#include <hbapplication.h>
#include <hbinstance.h>
#include <qactiongroup.h>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <videocollectioncommon.h>
#include <hbstackedwidget.h>
#include <hbgroupbox.h>
#include <hbdocumentloader.h>
#include <qhash.h>
#include <qmap.h>
#include <hbview.h>
#include <hblistview.h>
#include <mpxitemid.h>
#include <vcxmyvideosdefs.h>

#include "videocollectionwrapper.h"
#include "testlistview.h"
#include "videocollectionviewutils.h"
#include "videolistselectiondialog.h"
#include "videosortfilterproxymodel.h"
#include "videocollectionwrapper.h"
#include "videohintwidget.h"
#include "videolistwidgetdata.h"
#include "videocollectionwrapperdata.h"
#include "videocollectionviewutilsdata.h"
#include "videolistselectiondialogdata.h"
#include "videosortfilterproxymodeldata.h"
#include "videocollectionuiloaderdata.h"
#include "videolistdatamodeldata.h"

#define	private	public
#include "videolistwidget.h"
#include "videocollectionuiloader.h"
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

    int res;
    if(argc > 1)
    {   
        res = QTest::qExec(&tv, argc, argv);
    }
    else
    {
    	char *pass[3];
    	pass[0]	= argv[0];
    	pass[1]	= "-o";
    	pass[2]	= "c:\\data\\testlistview.txt";
        res = QTest::qExec(&tv, 3, pass);
    }

	return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestListView::init(bool initTestView)
{
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
    if(initTestView)
    {
        QVERIFY(mTestView->initializeView() == 0);
    }
    
    mListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY(mListWidget);

	mWrapper = &VideoCollectionWrapper::instance();
	QVERIFY(mWrapper);
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestListView::cleanup()
{
	delete mTestView;
	mTestView =	0;
	
	mWrapper = 0;

	disconnect();

    VideoListWidgetData::reset();
    VideoCollectionWrapperData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoListSelectionDialogData::reset();
    VideoSortFilterProxyModelData::reset();
}

// ---------------------------------------------------------------------------
// visibleMenuActions
// ---------------------------------------------------------------------------
//
int	TestListView::visibleMenuActions()
{
	int	visibleActions(0);
	VideoCollectionUiLoader::ActionIds key;
	HbAction *action(0);
	foreach(key,mUiLoader->mMenuActions.keys())
	{
		action = dynamic_cast<HbAction*>(mUiLoader->mMenuActions[key]);
		if(action && action->isVisible())
			visibleActions++;
	}
	return visibleActions;
}

// ---------------------------------------------------------------------------
// setRowCount
// ---------------------------------------------------------------------------
//
void TestListView::setRowCount(int count)
{
    if (mTestView)
    {
        if (!mTestView->mCurrentList)
        {
            mTestView->activateView();
        }
        VideoSortFilterProxyModel *model = mTestView->mCurrentList->mModel;
        if (model)
        {
            VideoListDataModel *sourceModel =
                qobject_cast<VideoListDataModel*>(
                    model->sourceModel());
            if (sourceModel)
            {
                VideoListDataModelData::setRowCount(*sourceModel, count);
            }
        }
    }
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
    VideoListWidget *videoListWidget = 0;
    VideoHintWidget *hintWidget = 0;
    
    init(false);
	// Test	videolist widget creation failure.
	VideoListWidgetData::mInitializeReturnValue	= -1;
	QVERIFY( mTestView->initializeView() < 0 );
	QVERIFY( mTestView->mToolbarViewsActionGroup == 0 );
	QVERIFY( mTestView->mToolbarCollectionActionGroup == 0 );
	QVERIFY( mTestView->mVideoServices == 0 );
	QVERIFY( mTestView->mIsService == false );

	cleanup();

	// Test	widget creation	succeed.
	init(false);
    VideoCollectionViewUtilsData::mSortRole = Qt::DisplayRole;
    VideoCollectionViewUtilsData::mSortOrder = Qt::DescendingOrder;
	QVERIFY( mTestView->initializeView() ==	0 );
	// Verify
	QCOMPARE( VideoListWidgetData::mInitializeCount, 1 );
	QVERIFY( mTestView->mToolbarViewsActionGroup != 0 );
	QVERIFY( mTestView->mToolbarCollectionActionGroup != 0 );
	QCOMPARE( mTestView->mToolbarViewsActionGroup->actions().count(), 3 );
	QCOMPARE( mTestView->mToolbarCollectionActionGroup->actions().count(), 3 );
	QCOMPARE( mTestView->toolBar()->actions().count(), 3 );
	// to my understanding the all videos action should	always be first.
	QVERIFY( mTestView->mToolbarViewsActionGroup->actions().first()->isChecked()	);
	QCOMPARE( VideoSortFilterProxyModelData::mSortRole, (int)Qt::DisplayRole );
	QCOMPARE( VideoSortFilterProxyModelData::mSortOrder, Qt::DescendingOrder );
	QVERIFY( VideoSortFilterProxyModelData::mSortAsync == false );
	QVERIFY( mTestView->mVideoServices == 0 );
	QVERIFY( mTestView->mIsService == false );
	cleanup();
	cleanup();

	// Test	command	signal.
	init(true);
	videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY(videoListWidget);
	connect( this, SIGNAL(testSignal(int)),	videoListWidget, SIGNAL(command(int)) );
	QSignalSpy spy(mTestView, SIGNAL(command(int)));
	emit testSignal(5);
	QCOMPARE( spy.count(), 1 );
	QList<QVariant>	arguments =	spy.takeFirst();
	QCOMPARE( arguments.at(0).toInt(), 5 );
	disconnect();
	cleanup();

	init(true);
	QPointer<QActionGroup> toolbar = mTestView->mToolbarViewsActionGroup;
	QPointer<QActionGroup> toolbar2 = mTestView->mToolbarCollectionActionGroup;

	// Test	calling	initializeView again.
	QVERIFY( mTestView->initializeView() ==	0 );

	QVERIFY( toolbar !=	0 );
	QVERIFY( toolbar2 != 0 );
	QVERIFY( mTestView->mToolbarViewsActionGroup == toolbar );
	QVERIFY( mTestView->mToolbarCollectionActionGroup == toolbar2 );

	// Test	view deletion.
	cleanup();
	QVERIFY( toolbar ==	0 );
	QVERIFY( toolbar2 == 0 );

	// Test	get	model failure.
	init(false);
	QVERIFY(mTestView);
	VideoCollectionWrapperData::mGetModelFails	= true;
	QVERIFY( mTestView->initializeView() < 0 );
	QVERIFY( mTestView->mToolbarViewsActionGroup	== 0 );
	QVERIFY( mTestView->mToolbarCollectionActionGroup	== 0 );
	cleanup();

	// Test	model open failure.
	init(false);
	QVERIFY(mTestView);
	VideoSortFilterProxyModelData::mOpenFails =	true;
	QVERIFY( mTestView->initializeView() < 0 );
	QVERIFY( mTestView->mToolbarViewsActionGroup == 0 );
	QVERIFY( mTestView->mToolbarCollectionActionGroup == 0 );
	cleanup();

	// Test sorting value load 'failure'.
	init(false);
    VideoCollectionViewUtilsData::mSortRole = Qt::DisplayRole;
    VideoCollectionViewUtilsData::mSortOrder = Qt::DescendingOrder;
    VideoCollectionViewUtilsData::mLoadSortingValuesFails = true;
    QVERIFY( mTestView->initializeView() == 0 );
    QCOMPARE( VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyDateTime );
    QCOMPARE( VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder );
    QVERIFY( VideoSortFilterProxyModelData::mSortAsync == false );
    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test create menu
// ---------------------------------------------------------------------------
//
void TestListView::testCreateListMenu()
{
	init(false);
	VideoListWidgetData::mInitializeReturnValue	= -1;
	QVERIFY( mTestView->initializeView() < 0 );
	HbMenu*	menu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
	QVERIFY( menu );
	QCOMPARE( menu->actions().count(), 4 );

	cleanup();

	init(false);
	VideoListWidgetData::mInitializeReturnValue	= 0;
	QVERIFY( mTestView->initializeView() ==	0 );
	menu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
	QVERIFY( menu );
	QCOMPARE( menu->actions().count(), 4 );

	HbAction* sortAction = dynamic_cast<HbAction*>(menu->actions().at(3));
	QVERIFY( sortAction	);
	menu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
	QVERIFY(menu !=	0);
	QVERIFY( menu->actions().count() ==	4 );

	QVERIFY( mUiLoader->mMenuActions.count() ==	9 );
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
	QCOMPARE( allVideos->icon().iconName(),	QString(":/images/qtg_mono_video_all.svg")	);

	HbAction* collections =	static_cast<HbAction*>(actions.at(1));
	QVERIFY( collections->isCheckable()	);
	QCOMPARE( collections->icon().iconName(), QString(":/images/qtg_mono_video_collection.svg") );

	HbAction* services = static_cast<HbAction*>(actions.at(2));
	QVERIFY( services->isCheckable() );
	QCOMPARE( services->icon().iconName(), QString(":/images/qtg_mono_video_services.svg") );

	HbAction* addVideos	= static_cast<HbAction*>(actions.at(3));
	QVERIFY( !addVideos->isCheckable() );
	QCOMPARE( addVideos->icon().iconName(),	QString(":/images/mono_video_addvideos.svg") );

	HbAction* removeVideos = static_cast<HbAction*>(actions.at(4));
	QVERIFY( !removeVideos->isCheckable() );
	QCOMPARE( removeVideos->icon().iconName(), QString(":/images/mono_video_removevideos.svg") );

	HbAction* sortVideos = static_cast<HbAction*>(actions.at(5));
	QVERIFY( !sortVideos->isCheckable()	);
	QCOMPARE( sortVideos->icon().iconName(), QString(":/images/mono_video_sortvideos.svg") );

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test activate view
// ---------------------------------------------------------------------------
//
void TestListView::testActivateView()
{
    VideoListWidget* videoListWidget = 0;
    
	init();

	videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
	QVERIFY(videoListWidget);
	mTestView->mCurrentList = 0;

	QCOMPARE( mTestView->activateView(), -1	);
    QVERIFY( VideoListWidgetData::mActive == false );
    QCOMPARE( mTestView->mCurrentList, videoListWidget );
    QCOMPARE( VideoListWidgetData::mActivateCount, 0 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 0 );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() == false );
	
	VideoListWidgetData::mActivateReturnValue =	-1;

	QCOMPARE( mTestView->activateView(), -1	);
	QVERIFY( VideoListWidgetData::mActive == false );
	QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() == false );

	VideoListWidgetData::mActivateReturnValue =	0;
	
	QCOMPARE( mTestView->activateView(), 0 );
	QVERIFY( VideoListWidgetData::mActive );
	QCOMPARE( VideoListWidgetData::mActivateCount, 2 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() );

	QCOMPARE( mTestView->activateView(), 0 );
	QVERIFY( VideoListWidgetData::mActive );
	QCOMPARE( VideoListWidgetData::mActivateCount, 3 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() );

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test deactivate view
// ---------------------------------------------------------------------------
//
void TestListView::testDeactivateView()
{
    VideoListWidget* videoListWidget = 0;

    init();

    videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY(videoListWidget);
    mTestView->mCurrentList = 0;

	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() == false );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 0 );

	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() == false );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );

	VideoListWidgetData::mActivateReturnValue =	0;
	mTestView->activateView();
	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isEnabled() == false );
	QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 2 );

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
    VideoListWidget *videoListWidget = 0;
    HbGroupBox *subLabel = 0;
    
	init();
	
    videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY(videoListWidget);
    subLabel = mUiLoader->findWidget<HbGroupBox>(DOCML_NAME_VC_HEADINGBANNER);
    QVERIFY(subLabel);

	QList<QAction*>	actions	= mTestView->mToolbarViewsActionGroup->actions();
	HbAction* action = static_cast<HbAction*>(actions.at(0));
	
	videoListWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
	subLabel->setHeading("invalid");
	
	// action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
	QCOMPARE( mTestView->mCurrentList->mCurrentLevel, VideoCollectionCommon::ELevelVideos );
    QVERIFY( VideoListWidgetData::mActive == true );
    QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
    QCOMPARE( VideoListWidgetData::mDeactivateCount, 0 );
    QCOMPARE( subLabel->heading(), tr("txt_videos_subtitle_ln_videos" ));
	QCOMPARE(mTestView->mCurrentList, videoListWidget);

	mTestView->activateView();
	
	VideoListWidgetData::mActivateCount = 0;
	mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    subLabel->setHeading("invalid");
    
    // action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
    QCOMPARE( mTestView->mCurrentList->mCurrentLevel, VideoCollectionCommon::ELevelVideos );
    QVERIFY( VideoListWidgetData::mActive == true );
	QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 0 );
    QCOMPARE( subLabel->heading(), tr("txt_videos_subtitle_ln_videos" ));
    QCOMPARE(mTestView->mCurrentList, videoListWidget);

	cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test open collections view	slot
// ---------------------------------------------------------------------------
//
void TestListView::testOpenCollectionViewSlot()
{
    VideoListWidget *collectionWidget = 0;
    HbGroupBox *subLabel = 0;

    init();

	QList<QAction*>	actions	= mTestView->mToolbarViewsActionGroup->actions();
	HbAction* action = static_cast<HbAction*>(actions.at(1));

	collectionWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);
    QVERIFY(collectionWidget);
    subLabel = mUiLoader->findWidget<HbGroupBox>(DOCML_NAME_VC_HEADINGBANNER);
    QVERIFY(subLabel);

	mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    subLabel->setHeading("invalid");

	// action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
    QCOMPARE( mTestView->mCurrentList->mCurrentLevel, VideoCollectionCommon::ELevelCategory );
    QVERIFY( VideoListWidgetData::mActive == true );
    QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
    QCOMPARE( VideoListWidgetData::mDeactivateCount, 0 );
    QCOMPARE( subLabel->heading(), tr("txt_videos_subtitle_l1_collections" ));
    QCOMPARE(mTestView->mCurrentList, collectionWidget);

	mTestView->activateView();

    VideoListWidgetData::mActivateCount = 0;
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    subLabel->setHeading("invalid");
    
    // action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
    QCOMPARE( mTestView->mCurrentList->mCurrentLevel, VideoCollectionCommon::ELevelCategory );
    QVERIFY( VideoListWidgetData::mActive == true );
    QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
    QCOMPARE( VideoListWidgetData::mDeactivateCount, 0 );
    QCOMPARE( subLabel->heading(), tr("txt_videos_subtitle_l1_collections" ));
    QCOMPARE(mTestView->mCurrentList, collectionWidget);

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
	action = mUiLoader->mMenuActions[VideoCollectionUiLoader::EActionDelete];
	QVERIFY(action != 0);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(action);
	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
	emit testSignal(0);
	QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 0);
	QCOMPARE(VideoSortFilterProxyModelData::mSortRole, -1);
	cleanup();

	// Sort	by action has no menu.
	init(true);

	// Change sort by action to	delete action which	has	no menu.
	HbAction *sortByAction = mUiLoader->mMenuActions[VideoCollectionUiLoader::EActionSortBy];
	HbAction *deleteAction = mUiLoader->mMenuActions[VideoCollectionUiLoader::EActionDelete];
	mUiLoader->mMenuActions[VideoCollectionUiLoader::EActionSortBy] =	deleteAction;
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(deleteAction);

	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
	emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 0);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, -1);

	// Change the action back.
	mUiLoader->mMenuActions[VideoCollectionUiLoader::EActionSortBy] =	sortByAction;

	cleanup();

	// View	is not initialized.
	init(false);
	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
	emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 0);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, -1);
	cleanup();

	// Good	case.
	init();
	action = mUiLoader->mMenuActions[VideoCollectionUiLoader::EActionSortBy];
	QVERIFY(action != 0);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(action);
	HbAction* sortAction = mUiLoader->mMenuActions[VideoCollectionUiLoader::EActionSortByName];
	QVERIFY(sortAction != 0);
	action->menu()->setActiveAction(sortAction);

	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
	emit testSignal(0);
	QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 1);
	QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)Qt::DisplayRole);
	QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync);

    // emit test signal again, with same parameters. Sorting should be switched to
    // descending order.
    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 2);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)Qt::DisplayRole);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::DescendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync);

    // on third emit, sorting should be switched back to ascending
    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 3);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)Qt::DisplayRole);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync);

    // test that after changing the sort role, the order is also switched to ascending.
    VideoSortFilterProxyModelData::mSortOrder = Qt::DescendingOrder;
    sortAction = mUiLoader->mMenuActions[VideoCollectionUiLoader::EActionSortByDate];
    QVERIFY(sortAction != 0);
    action->menu()->setActiveAction(sortAction);

    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 4);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync);

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
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionLaunchCount == 0);
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionItemSelectionCount == 0);

    cleanup();
    init();
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(deleteItemsSlot()) );

    ////////////
    // no selection dialog
    ////////////
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionLaunchCount == 1);
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionItemSelectionCount == 0);

    cleanup();
    init();
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(deleteItemsSlot()) );

    ////////////
    // dialog loading fails
    ////////////
    VideoCollectionUiLoaderData::mFailDialogLoad = true;
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionLaunchCount == 0);
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionItemSelectionCount == 0);
    VideoCollectionUiLoaderData::mFailDialogLoad = false;
    //////////
    // document loader deletes all objects in case of error, so we cannot cleanup all
    disconnect();
    ///////////////
    init();
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(deleteItemsSlot()) );

    ////////////
    // selection dialog exists
    ////////////
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionLaunchCount == 1);
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionItemSelectionCount == 0);
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionLaunchCount == 2);
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionItemSelectionCount == 0);

    cleanup();
    init();
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(deleteItemsSlot()) );

    ////////////
    // exec returns primary key (just for the coverity's sake)
    ////////////
    VideoListSelectionDialogData::mExecReturnPrimary = true;
    VideoListSelectionDialogData::mSelectionCount = 5;
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionLaunchCount == 1);
    QVERIFY(VideoListSelectionDialogData::mMultiSelectionItemSelectionCount == 5);

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
	setRowCount(1);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	QVERIFY(visibleMenuActions() ==	6);
	cleanup();

	// View	is not initialized.
	init(false);
	connect( this, SIGNAL(testSignal(int)),	mTestView, SLOT(aboutToShowMainMenuSlot()) );
	emit testSignal(0);
	cleanup();

	HbAction *action(0);

	// Collections is triggered	from toolbar.
	init();
	setRowCount(1);
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
    setRowCount(1);
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
	setRowCount(1);
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
    mTestView->activateView();
	mTestView->toolBar()->clearActions();
	mTestView->toolBar()->addActions( mTestView->mToolbarCollectionActionGroup->actions() );
	setRowCount(1);
	QVERIFY(action != 0);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	visible = visibleMenuActions();
	QCOMPARE(visible,	1);
	cleanup();

	// Menu	action is null
	init();
    setRowCount(1);
	action = mUiLoader->mMenuActions.take(VideoCollectionUiLoader::EActionSortBy);
	delete action;
	action = NULL;
	mUiLoader->mMenuActions[VideoCollectionUiLoader::EActionSortBy] =	action;
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	visible = visibleMenuActions();
	QCOMPARE(visible,	0);
	cleanup();

	// No menu actions.
	init();
	setRowCount(1);
	QMap<VideoCollectionUiLoader::ActionIds, HbAction*> backupActions =
		QMap<VideoCollectionUiLoader::ActionIds, HbAction*>(mUiLoader->mMenuActions);
	mUiLoader->mMenuActions.clear();
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	mUiLoader->mMenuActions.unite(backupActions);
	cleanup();

	// Model has no	items.
	init();
	setRowCount(0);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->show();
	visible = visibleMenuActions();
	QCOMPARE(visible,	0);
	cleanup();

	// Toolbar action group	is null
	init();
	setRowCount(1);
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
    HbMenu *optionsMenu = 0;
    HbMenu *sortMenu = 0;

    // All ok.
    init();
    sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    connect( this, SIGNAL(testSignal(int)), mTestView, SLOT(openSortByMenuSlot()) );
    setRowCount(1);
    mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->hide();
    emit testSignal(0);
    // sorting menu should be visible
    QVERIFY(visibleMenuActions() == 4);
    cleanup();

    // View is not initialized.
    init(false);
    sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    optionsMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
    connect( this, SIGNAL(testSignal(int)), mTestView, SLOT(openSortByMenuSlot()) );
    setRowCount(1);
    optionsMenu->hide();
    emit testSignal(0);
    QCOMPARE(visibleMenuActions(),   0);
    cleanup();

    // no items
    init();
    sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    optionsMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
    connect( this, SIGNAL(testSignal(int)), mTestView, SLOT(openSortByMenuSlot()) );
    setRowCount(0);
    mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->hide();
    emit testSignal(0);
    QCOMPARE(visibleMenuActions(),   0);
    cleanup();
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
	QVERIFY(VideoCollectionViewUtilsData::mLastError ==	0);
	cleanup();

	// Multiple	deletion error.
	init();
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(VideoCollectionCommon::statusMultipleDeleteFail, *variant);
	QVERIFY(VideoCollectionViewUtilsData::mLastError ==	VideoCollectionCommon::statusMultipleDeleteFail);
	cleanup();

	// Deletion	error.
	init();
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(VideoCollectionCommon::statusSingleDeleteFail, *variant);
	QVERIFY(VideoCollectionViewUtilsData::mLastError ==	VideoCollectionCommon::statusSingleDeleteFail);
	cleanup();

	// status multidelete succeed
    init();
    connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
    emit testStatusSignal(VideoCollectionCommon::statusDeleteSucceed, *variant);
    QVERIFY(VideoCollectionViewUtilsData::mLastError == VideoCollectionCommon::statusDeleteSucceed);
    cleanup();

	// No model.
	init(false);
	connect(this, SIGNAL(testStatusSignal(int, QVariant&)), mTestView, SLOT(handleAsyncStatusSlot(int, QVariant&)));
	emit testStatusSignal(1,	*variant);
	QVERIFY(VideoCollectionViewUtilsData::mLastError ==	1);
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
    setRowCount(1);
	connect(this, SIGNAL(testLayoutChangedSignal()), mTestView, SLOT(layoutChangedSlot()));
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&)), mTestView, SLOT(collectionOpenedSlot(bool, const QString&)));

    VideoListWidget *videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);   
    VideoListWidget *collectionWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);   
    VideoListWidget *collectionContentWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);   
	
	mTestView->mCurrentList = videoListWidget; 
	mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
	emit testLayoutChangedSignal();
	HbGroupBox* label = mUiLoader->findWidget<HbGroupBox>(DOCML_NAME_VC_HEADINGBANNER);
	QString returnString = label->heading();
	QString expectedString = "txt_videos_subtitle_ln_videos";
	QCOMPARE(returnString, expectedString);

    mTestView->mCurrentList = 0; 
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
	emit testLayoutChangedSignal();
	returnString = label->heading();
	expectedString = "txt_videos_subtitle_ln_videos";
	QCOMPARE(returnString, expectedString);

    mTestView->mCurrentList = collectionWidget; 
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
	emit testLayoutChangedSignal();
	returnString = label->heading();
	expectedString = "txt_videos_subtitle_l1_collections";
	QCOMPARE(returnString, expectedString);

    mTestView->mCurrentList = 0; 
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
	emit testLayoutChangedSignal();
	returnString = label->heading();
	expectedString = "txt_videos_subtitle_l1_collections";
	QCOMPARE(returnString, expectedString);

    mTestView->mCurrentList = collectionContentWidget; 
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
	emit testLayoutChangedSignal();
	returnString = label->heading();
	expectedString = "txt_videos_subtitle_1_l2";
	QCOMPARE(returnString, expectedString);

    mTestView->mCurrentList = 0; 
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
	expectedString = "txt_videos_subtitle_1_l2";
	emit testLayoutChangedSignal();
	returnString = label->heading();
	QCOMPARE(returnString, expectedString);
	
    mTestView->mCurrentList = collectionContentWidget; 
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
	emit testCollectionOpenedSignal(true, QString("NAME"));
	emit testLayoutChangedSignal();
	returnString = label->heading();
	expectedString = "txt_videos_subtitle_1_l2";
	QCOMPARE(returnString, expectedString);
}

// ---------------------------------------------------------------------------
// testShowHint
// ---------------------------------------------------------------------------
//
void TestListView::testShowHint()
{
    VideoHintWidget *hintWidget = 0;
    
    init(true);
    connect(this, SIGNAL(testLayoutChangedSignal()), mTestView, SLOT(layoutChangedSlot()));
    
    setRowCount(1);    
    hintWidget = mUiLoader->findWidget<VideoHintWidget>(DOCML_NAME_VC_VIDEOHINTWIDGET);    
    mTestView->mModelReady = false;
    hintWidget->setVisible(true);    
    emit testLayoutChangedSignal();
    QVERIFY(hintWidget->isVisible());
    
    VideoSortFilterProxyModel* model = mTestView->mCurrentList->mModel;
    mTestView->mCurrentList->mModel = 0;
    mTestView->mModelReady = true;    
    emit testLayoutChangedSignal();
    QVERIFY(hintWidget->isVisible() == false);

    setRowCount(1);
    hintWidget->setVisible(true);
    emit testLayoutChangedSignal();
    QVERIFY(hintWidget->isVisible() == false);
}

// End of file
