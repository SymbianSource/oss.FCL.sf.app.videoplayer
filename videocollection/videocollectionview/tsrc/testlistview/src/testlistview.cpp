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

// Version : %version: 41 %

#define private public
#include "videoservices.h"
#include "videosortfilterproxymodel.h"
#include "videolistwidget.h"
#include "videolistview.h"
#undef private

#include <qdebug.h>
#include <xqserviceutil.h>
#include <QtTest/QtTest>
#include "hbapplication.h"
#include "hbeffect.h"
#include "hbinstance.h"
#include "hbmenu.h"
#include "hbmenudata.h"
#include "hbtoolbar.h"
#include "hbaction.h"
#include "hbstackedwidget.h"
#include "hbgroupbox.h"
#include "hbdocumentloader.h"
#include "hbview.h"
#include "hblistview.h"
#include "hbinputdialog.h"
#include <qactiongroup.h>
#include <videocollectioncommon.h>
#include <qhash.h>
#include <qmap.h>
#include <mpxitemid.h>
#include <vcxmyvideosdefs.h>

#include "videocollectionuiloader.h"
#include "videocollectionwrapper.h"
#include "testlistview.h"
#include "videocollectionviewutils.h"
#include "videolistselectiondialog.h"
#include "videocollectionwrapper.h"
#include "videohintwidget.h"
#include "videohintwidgetdata.h"
#include "videolistwidgetdata.h"
#include "videocollectionwrapperdata.h"
#include "videocollectionviewutilsdata.h"
#include "videolistselectiondialogdata.h"
#include "videosortfilterproxymodeldata.h"
#include "videocollectionuiloaderdata.h"
#include "videolistdatamodeldata.h"
#include "xqserviceutilxtra.h"

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int	main(int argc, char	*argv[])
{
	HbApplication app(argc,	argv);
	HbMainWindow window;
	hbInstance->mWindowses.append(&window);

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
    XQServiceUtilXtra::service = false;
	mUiLoader = new VideoCollectionUiLoader();
	mUiLoader->reset();

	bool ok(false);
	mUiLoader->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
    if (!ok)
    {
    	return;
    }

    mTestView = mUiLoader->findWidget<VideoListView>( DOCML_NAME_VIEW );
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
	delete mUiLoader;
	mUiLoader = 0;
	
	mWrapper = 0;
	mTestView = 0;
	disconnect();

    VideoListWidgetData::reset();
    VideoCollectionWrapperData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoListSelectionDialogData::reset();
    VideoSortFilterProxyModelData::reset();
    HbDocumentLoader::cleanup();
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
	foreach (key,VideoCollectionUiLoaderData::mMenuActions.keys())
	{
		action = dynamic_cast<HbAction*>(VideoCollectionUiLoaderData::mMenuActions[key]);
		if (action && action->isVisible())
		{
            visibleActions++;
		}
	}
	return visibleActions;
}

// ---------------------------------------------------------------------------
// setRowCount
// ---------------------------------------------------------------------------
//
void TestListView::setRowCount(int count, VideoSortFilterProxyModel *model)
{
    if(!model)
    {
        if (mTestView)
        {
            if (!mTestView->mCurrentList)
            {
                mTestView->activateView(TMPXItemId::InvalidId());
            }
            model = mTestView->mCurrentList->mModel;
        }
    }

    if (model)
    {
        VideoListDataModel *sourceModel =
            qobject_cast<VideoListDataModel*>(
                model->sourceModel());
        if(sourceModel)
        {
            VideoListDataModelData::setRowCount(*sourceModel, count);
        }
    }
}

// ---------------------------------------------------------------------------
// isActionVisible
// ---------------------------------------------------------------------------
//
bool TestListView::isActionVisible(const char *name) const
{
    bool isVisible = false;
    
    HbAction *action = mUiLoader->findObject<HbAction>(name);
    if (!action)
    {
        // must be menu widget
        HbMenu *menu = mUiLoader->findWidget<HbMenu>(name);
        if (menu)
        {
            action = menu->menuAction();
        }
    }
    
    // check if action is visible
    if (action)
    {
        isVisible = action->isVisible();
    }
    
    return isVisible;
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

	mUiLoader->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);

    if (!ok)
    {
    	return;
    }

    mTestView = mUiLoader->findWidget<VideoListView>( DOCML_NAME_VIEW );

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
	// Test mUiLoader is null
    VideoCollectionUiLoader *tmp = mTestView->mUiLoader;
	mTestView->mUiLoader = 0;
	QVERIFY( mTestView->initializeView() < 0 );	
	QVERIFY( mTestView->mVideoServices == 0 );
	QVERIFY( mTestView->mIsService == false );
	mTestView->mUiLoader = tmp;
	
	// service flag is false and mVideoServices is != 0
	mTestView->mIsService = false;
	XQServiceUtilXtra::service = false;
	mTestView->mVideoServices = VideoServices::instance();
	QVERIFY( mTestView->initializeView() == 0 );    
    QVERIFY( mTestView->mVideoServices != 0 );
    QVERIFY( mTestView->mIsService == false );
	
    // servicve flag is true, mVideoServices is 0
    mTestView->mVideoServices = 0;
    XQServiceUtilXtra::service = true;
    QVERIFY( mTestView->initializeView() == 0 );    
    QVERIFY( mTestView->mVideoServices != 0 );
    QVERIFY( mTestView->mIsService == true );
    XQServiceUtilXtra::service = false;
    
	cleanup();	
}

// ---------------------------------------------------------------------------
// testMenus
// ---------------------------------------------------------------------------
//
void TestListView::testMenus()
{
    init();
    mTestView->activateView(TMPXItemId::InvalidId());

    HbAction* action = 0;
    QList<QAction*> tbActions = mTestView->toolBar()->actions();
    QList<QAction*>	tbGroupActions;
    QList<QAction*>	tbGroupActions2;

    // All videos is open for default, verify.
    QVERIFY( mTestView->mToolbarViewsActionGroup != 0 );
    QVERIFY( mTestView->mToolbarCollectionActionGroup != 0 );
    tbGroupActions = mTestView->mToolbarViewsActionGroup->actions();
    tbGroupActions2 = mTestView->mToolbarCollectionActionGroup->actions();
    QCOMPARE( mTestView->mToolbarActions.count(), 5 );
    QCOMPARE( tbActions.count(), 3 );
    QCOMPARE( tbGroupActions.count(), 3 );
    QCOMPARE( tbGroupActions2.count(), 2 );
    QVERIFY( tbGroupActions.at(0)->isChecked() ); // First is checked.

    // Verify checkable	and	visible	toolbar	actions.
    HbAction* allVideosAction =	mTestView->mToolbarActions[VideoListView::ETBActionAllVideos];
    HbAction* collectionsAction	= mTestView->mToolbarActions[VideoListView::ETBActionCollections];
    HbAction* servicesAction = mTestView->mToolbarActions[VideoListView::ETBActionServices];
    HbAction* addVideosAction =	mTestView->mToolbarActions[VideoListView::ETBActionAddVideos];
    HbAction* removeVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos];
    QVERIFY( allVideosAction !=	0 );
    QVERIFY( allVideosAction->isCheckable()	);
    QVERIFY( allVideosAction->isVisible() );
    QVERIFY( tbActions.at(0) == tbGroupActions.at(0) );
    
    QVERIFY( collectionsAction != 0	);
    QVERIFY( collectionsAction->isCheckable() );
    QVERIFY( collectionsAction->isVisible() );
    QVERIFY( tbActions.at(1) == tbGroupActions.at(1) );
    
    QVERIFY( servicesAction	!= 0 );
    QVERIFY( !servicesAction->isCheckable() );
    QVERIFY( servicesAction->isVisible() );
    QVERIFY( tbActions.at(2) == tbGroupActions.at(2) );
    
    QVERIFY( addVideosAction !=	0 );
    QVERIFY( !addVideosAction->isCheckable() );
    QVERIFY( tbActions.at(0) != tbGroupActions2.at(0) );
    
    QVERIFY( removeVideosAction	!= 0 );
    QVERIFY( !removeVideosAction->isCheckable() );
    QVERIFY( tbActions.at(1) != tbGroupActions2.at(1) );

    // Open	collections.
    action = dynamic_cast<HbAction*>(tbGroupActions.at(1));
    QVERIFY( action );
    action->setChecked(false);
    action->QAction::trigger();

    // Verify.
    QVERIFY( mTestView->mToolbarViewsActionGroup );
    tbGroupActions = mTestView->mToolbarViewsActionGroup->actions();
    QCOMPARE( mTestView->mToolbarActions.count(), 5 );
    QCOMPARE( tbGroupActions.count(), 3 );
    QCOMPARE( tbGroupActions2.count(), 2 );
    QVERIFY( tbGroupActions.at(1)->isChecked() ); // 2nd is	checked.

    // Verify checkable	and	visible	toolbar	actions.
    allVideosAction	= mTestView->mToolbarActions[VideoListView::ETBActionAllVideos];
    collectionsAction =	mTestView->mToolbarActions[VideoListView::ETBActionCollections];
    servicesAction = mTestView->mToolbarActions[VideoListView::ETBActionServices];
    addVideosAction	= mTestView->mToolbarActions[VideoListView::ETBActionAddVideos];
    removeVideosAction = mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos];
    QVERIFY( allVideosAction !=	0 );
    QVERIFY( allVideosAction->isCheckable()	);
    QVERIFY( allVideosAction->isVisible() );
    QVERIFY( tbActions.at(0) == tbGroupActions.at(0) );
    
    QVERIFY( collectionsAction != 0	);
    QVERIFY( collectionsAction->isCheckable() );
    QVERIFY( collectionsAction->isVisible() );
    QVERIFY( tbActions.at(1) == tbGroupActions.at(1) );
    
    QVERIFY( servicesAction	!= 0 );
    QVERIFY( !servicesAction->isCheckable() );
    QVERIFY( servicesAction->isVisible() );
    QVERIFY( tbActions.at(2) == tbGroupActions.at(2) );
    
    QVERIFY( addVideosAction !=	0 );
    QVERIFY( !addVideosAction->isCheckable() );
    QVERIFY( tbActions.at(0) != tbGroupActions2.at(0) );
    
    QVERIFY( removeVideosAction	!= 0 );
    QVERIFY( !removeVideosAction->isCheckable() );
    QVERIFY( tbActions.at(1) != tbGroupActions2.at(1) );

    cleanup();
}

// ---------------------------------------------------------------------------
// Slot: test create toolbar
// ---------------------------------------------------------------------------
//
void TestListView::testCreateAction()
{
	init();
	mTestView->activateView(TMPXItemId::InvalidId());

	QVERIFY(mTestView->mToolbarViewsActionGroup != 0);

	QList<QAction*>	actions	= mTestView->mToolbarViewsActionGroup->actions();
	actions.append(mTestView->mToolbarCollectionActionGroup->actions());
	QCOMPARE(actions.count(), 5);

	QCOMPARE( mTestView->toolBar()->actions().count(), 3 );

	HbAction* allVideos	= static_cast<HbAction*>(actions.at(0));
	QVERIFY( allVideos->isCheckable() );
	QCOMPARE( allVideos->icon().iconName(),	QString("qtg_mono_video") );
	QVERIFY( allVideos->isChecked() );

	HbAction* collections =	static_cast<HbAction*>(actions.at(1));
	QVERIFY( collections->isCheckable()	);
	QCOMPARE( collections->icon().iconName(), QString("qtg_mono_video_collection") );

	HbAction* services = static_cast<HbAction*>(actions.at(2));
	QVERIFY( !services->isCheckable() );
	QCOMPARE( services->icon().iconName(), QString("qtg_mono_ovistore") );

	HbAction* addVideos	= static_cast<HbAction*>(actions.at(3));
	QVERIFY( !addVideos->isCheckable() );
	QCOMPARE( addVideos->icon().iconName(),	QString("qtg_mono_add_to_video_collection") );

	HbAction* removeVideos = static_cast<HbAction*>(actions.at(4));
	QVERIFY( !removeVideos->isCheckable() );
	QCOMPARE( removeVideos->icon().iconName(), QString("qtg_mono_remove_from_video_collection") );

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
	
	HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
	mainWnd->unsetOrientation(false);
    mTestView->mCurrentList = 0;
	HbDocumentLoader::mFindWidgetFails = true;
	QCOMPARE( mTestView->activateView(TMPXItemId::InvalidId()), -1	);
    QVERIFY( VideoListWidgetData::mActive == false );
    QVERIFY( mTestView->mCurrentList == 0 );
    QCOMPARE( VideoListWidgetData::mActivateCount, 0 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 0 );
	QVERIFY( mainWnd->mOrientationSet == false );
	
    HbDocumentLoader::mFindWidgetFails = false;
    videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY(videoListWidget);
	VideoListWidgetData::mActivateReturnValue =	-1;
	QCOMPARE( mTestView->activateView(TMPXItemId::InvalidId()), -1	);
	QVERIFY( VideoListWidgetData::mActive == false );
	QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
    QVERIFY( mainWnd->mOrientationSet == false );

	VideoListWidgetData::mActivateReturnValue =	0;
	QCOMPARE( mTestView->activateView(TMPXItemId::InvalidId()), 0 );
	QVERIFY( VideoListWidgetData::mActive );
	QCOMPARE( VideoListWidgetData::mActivateCount, 2 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
    QVERIFY( mainWnd->mOrientationSet );
    QCOMPARE( mainWnd->mOrientation, Qt::Vertical );

	QCOMPARE( mTestView->activateView(TMPXItemId::InvalidId()), 0 );
	QVERIFY( VideoListWidgetData::mActive );
	QCOMPARE( VideoListWidgetData::mActivateCount, 3 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
    QVERIFY( mainWnd->mOrientationSet );
    QCOMPARE( mainWnd->mOrientation, Qt::Vertical );
    
    cleanup();

    // activate view:
    // -browse service
    // -captured category
    TMPXItemId itemId = TMPXItemId::InvalidId();
    init();
    VideoListWidgetData::reset();
    VideoSortFilterProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_captured"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoSortFilterProxyModelData::mLastItemId, itemId);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    cleanup();
    
    // activate view:
    // -browse service
    // -downloaded category
    init();
    VideoListWidgetData::reset();
    VideoSortFilterProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdDownloads;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_downloaded"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoSortFilterProxyModelData::mLastItemId, itemId);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    cleanup();

    // activate view:
    // -browse service
    // -other category
    init();
    VideoListWidgetData::reset();
    VideoSortFilterProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdOther;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName.length(), 0);
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelVideos);
    QCOMPARE(VideoSortFilterProxyModelData::mLastItemId, TMPXItemId::InvalidId());
    cleanup();

    // activate view:
    // -browse service
    // -invalid category but item id is ok
    init();
    VideoListWidgetData::reset();
    VideoSortFilterProxyModelData::reset();
    itemId.iId1 = 0;
    itemId.iId2 = KVcxMvcMediaTypeVideo;
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName.length(), 0);
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelVideos);
    QCOMPARE(VideoSortFilterProxyModelData::mLastItemId, TMPXItemId::InvalidId());
    cleanup();

    // activate view:
    // -browse service
    // -captured category
    // -sort role 0 (default, date & time)
    init(false);
    XQServiceUtilXtra *serviceUtilXtra = XQServiceUtilXtra::instance();
    serviceUtilXtra->setCurrentService(true);
    VideoServices *videoServices = VideoServices::instance(0);
    videoServices->mCurrentService = VideoServices::EBrowse;
    videoServices->mSortRole = 0;
    VideoListWidgetData::reset();
    VideoSortFilterProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->initializeView(), 0);
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_captured"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoSortFilterProxyModelData::mLastItemId, itemId);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    serviceUtilXtra->decreaseReferenceCount();
    videoServices->decreaseReferenceCount();
    cleanup();

    // activate view:
    // -browse service
    // -captured category
    // -sort role 1 (date & time)
    init(false);
    serviceUtilXtra = XQServiceUtilXtra::instance();
    serviceUtilXtra->setCurrentService(true);
    videoServices = VideoServices::instance(0);
    videoServices->mCurrentService = VideoServices::EBrowse;
    videoServices->mSortRole = 1;
    VideoListWidgetData::reset();
    VideoSortFilterProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->initializeView(), 0);
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_captured"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoSortFilterProxyModelData::mLastItemId, itemId);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    cleanup();
    serviceUtilXtra->decreaseReferenceCount();
    videoServices->decreaseReferenceCount();

    // activate view:
    // -browse service
    // -captured category
    // -sort role 2 (title)
    init(false);
    serviceUtilXtra = XQServiceUtilXtra::instance();
    serviceUtilXtra->setCurrentService(true);
    videoServices = VideoServices::instance(0);
    videoServices->mCurrentService = VideoServices::EBrowse;
    videoServices->mSortRole = 2;
    VideoListWidgetData::reset();
    VideoSortFilterProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->initializeView(), 0);
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_captured"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoSortFilterProxyModelData::mLastItemId, itemId);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    cleanup();
    serviceUtilXtra->decreaseReferenceCount();
    videoServices->decreaseReferenceCount();

    // activate view:
    // -browse service
    // -captured category
    // -sort role 3 (size) 
    init(false);
    serviceUtilXtra = XQServiceUtilXtra::instance();
    serviceUtilXtra->setCurrentService(true);
    videoServices = VideoServices::instance(0);
    videoServices->mCurrentService = VideoServices::EBrowse;
    videoServices->mSortRole = 3;
    VideoListWidgetData::reset();
    VideoSortFilterProxyModelData::reset();
    itemId.iId1 = KVcxMvcCategoryIdCaptured;
    itemId.iId2 = KVcxMvcMediaTypeCategory;
    QCOMPARE(mTestView->initializeView(), 0);
    QCOMPARE(mTestView->activateView(itemId), 0);
    QVERIFY(VideoListWidgetData::mActive);
    QCOMPARE(VideoListWidgetData::mActivateCount, 1);
    QCOMPARE(mTestView->mCollectionName, hbTrId("txt_videos_dblist_captured"));
    QCOMPARE(mTestView->mCurrentList->getLevel(), VideoCollectionCommon::ELevelDefaultColl);
    QCOMPARE(VideoSortFilterProxyModelData::mLastItemId, itemId);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeySizeValue);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    cleanup();
    serviceUtilXtra->decreaseReferenceCount();
    videoServices->decreaseReferenceCount();
}

// ---------------------------------------------------------------------------
// Slot: test deactivate view
// ---------------------------------------------------------------------------
//
void TestListView::testDeactivateView()
{
    VideoListWidget* videoListWidget = 0;

    init();

    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

    videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY(videoListWidget);
    mTestView->mCurrentList = 0;
    mainWnd->mOrientationSet = true;

	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 0 );
    QVERIFY( mainWnd->mOrientationSet == false );
    
    mTestView->mCurrentList = videoListWidget;
    mainWnd->mOrientationSet = true;
    
	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );
    QVERIFY( mainWnd->mOrientationSet == false );

	VideoListWidgetData::mActivateReturnValue =	0;
    mainWnd->mOrientationSet = true;
	mTestView->activateView(TMPXItemId::InvalidId());
	mTestView->deactivateView();
	QVERIFY( mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->isVisible() == false );
	QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 2 );
    QVERIFY( mainWnd->mOrientationSet == false );

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
	mTestView->activateView(TMPXItemId::InvalidId());
	
    videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY( videoListWidget );

	QList<QAction*>	actions	= mTestView->mToolbarViewsActionGroup->actions();
	HbAction* action = static_cast<HbAction*>(actions.at(0));
	
	// action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
	QCOMPARE( mTestView->mCurrentList, videoListWidget );

	mTestView->mCurrentList = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);
	QVERIFY( mTestView->mCurrentList );
    VideoListWidgetData::mActivateCount = 0;
	videoListWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    
    // action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
    QCOMPARE( mTestView->mCurrentList, videoListWidget );
    QCOMPARE( mTestView->mCurrentList->mCurrentLevel, VideoCollectionCommon::ELevelVideos );
    QVERIFY( VideoListWidgetData::mActive == true );
	QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
	QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );

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
    mTestView->activateView(TMPXItemId::InvalidId());

	QList<QAction*>	actions	= mTestView->mToolbarViewsActionGroup->actions();
	HbAction* action = static_cast<HbAction*>(actions.at(1));

	collectionWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);
    QVERIFY(collectionWidget);

	// action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
    QCOMPARE(mTestView->mCurrentList, collectionWidget);

    mTestView->mCurrentList = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY( mTestView->mCurrentList );
    VideoListWidgetData::mActivateCount = 0;
    VideoListWidgetData::mDeactivateCount = 0;
    collectionWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    
    // action wont emit	if it's	already	checked.
	action->setChecked(false);
	action->trigger();
    QCOMPARE(mTestView->mCurrentList, collectionWidget);
    QCOMPARE( mTestView->mCurrentList->mCurrentLevel, VideoCollectionCommon::ELevelCategory );
    QVERIFY( VideoListWidgetData::mActive == true );
    QCOMPARE( VideoListWidgetData::mActivateCount, 1 );
    QCOMPARE( VideoListWidgetData::mDeactivateCount, 1 );

	cleanup();
}
// ---------------------------------------------------------------------------
// Slot: test open services	view slot
// ---------------------------------------------------------------------------
//
void TestListView::testOpenServicesViewSlot()
{
    QFAIL("Feature not yet implemented!");
}

// ---------------------------------------------------------------------------
// testStartSorting
// ---------------------------------------------------------------------------
//
void TestListView::testStartSortingSlot()
{
	HbAction *action(0);
	HbMenu *sortMenu(0);
	HbAction *sortMenuAction = new HbAction("test");

	// View	is not initialized.
	init(false);
	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
	emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 0);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, -1);
	cleanup();

    // View is not activated.
    init();
    connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 0);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, -1);
    cleanup();

    // Active action is not sort by.
    init();
    mTestView->activateView(TMPXItemId::InvalidId());
    action = mUiLoader->findObject<HbAction>(DOCML_NAME_DELETE_MULTIPLE);
    QVERIFY(action != 0);
    mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(action);
    sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    sortMenuAction->setMenu(sortMenu);
    HbMenuData::mMenuAction = sortMenuAction;
    connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 0);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, -1);
    cleanup();

	// Good	cases.
	init();
    mTestView->activateView(TMPXItemId::InvalidId());
    sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    sortMenuAction->setMenu(sortMenu);
    HbMenuData::mMenuAction = sortMenuAction;
    VideoSortFilterProxyModel& model = mTestView->mCurrentList->getModel();
	QVERIFY(action != 0);
	mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(sortMenuAction);
	HbAction* sortAction = mUiLoader->findObject<HbAction>(DOCML_NAME_SORT_BY_NAME);
	QVERIFY(sortAction != 0);
	sortMenu->setActiveAction(sortAction);
	model.setSortRole(1);
	connect(this, SIGNAL(testObjectReadySignal(QObject*, const QString)), mTestView, SLOT(objectReadySlot(QObject*, const QString)));
	emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_DATE), DOCML_NAME_SORT_BY_DATE);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_NAME), DOCML_NAME_SORT_BY_NAME);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS), DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    emit testObjectReadySignal(mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_SIZE), DOCML_NAME_SORT_BY_SIZE);
    disconnect(this, SIGNAL(testObjectReadySignal(QObject*, const QString)), mTestView, SLOT(objectReadySlot(QObject*, const QString)));

	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
	emit testSignal(0);
	QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 1);
	QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyTitle);
	QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync);

    // emit test signal again, with same parameters. Sorting should be switched to
    // descending order.
    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 2);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::DescendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync);

    // on third emit, sorting should be switched back to ascending
    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 3);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyTitle);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync);

    // test that after changing the sort role, the order is also switched to ascending.
    model.doSorting(model.sortRole(), Qt::DescendingOrder);
    sortAction = mUiLoader->findObject<HbAction>(DOCML_NAME_SORT_BY_DATE);
    QVERIFY(sortAction != 0);
    sortMenu->setActiveAction(sortAction);

    emit testSignal(0);
    QCOMPARE(VideoSortFilterProxyModelData::mDoSortingCallCount, 5);
    QCOMPARE(VideoSortFilterProxyModelData::mSortRole, (int)VideoCollectionCommon::KeyDateTime);
    QCOMPARE(VideoSortFilterProxyModelData::mSortOrder, Qt::AscendingOrder);
    QVERIFY(VideoSortFilterProxyModelData::mSortAsync);
	cleanup();
	
	// start sorting slot:
	// -is service
	init(false);
	connect(this, SIGNAL(testSignal(int)), mTestView, SLOT(startSorting()));
    sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    sortMenuAction->setMenu(sortMenu);
    HbMenuData::mMenuAction = sortMenuAction;
    mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU)->setActiveAction(sortMenuAction);
    sortAction = mUiLoader->findObject<HbAction>(DOCML_NAME_SORT_BY_NAME);
    QVERIFY(sortAction != 0);
    sortMenu->setActiveAction(sortAction);
	XQServiceUtilXtra *serviceUtilXtra = XQServiceUtilXtra::instance();
	serviceUtilXtra->setCurrentService(true);
	QCOMPARE(mTestView->initializeView(), 0);
	QCOMPARE(mTestView->activateView(TMPXItemId::InvalidId()), 0);
	int sortRole = VideoCollectionViewUtilsData::mVideoSortRole;
	emit testSignal(0);
	QCOMPARE(sortRole, VideoCollectionViewUtilsData::mVideoSortRole);
	serviceUtilXtra->decreaseReferenceCount();
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
    mTestView->mViewReady = false;
    connect( this, SIGNAL(testSignal()), mTestView, SLOT(modelReadySlot()) );
    QSignalSpy spy(mTestView, SIGNAL(viewReady()));
    emit testSignal();
    QVERIFY(mTestView->mModelReady);
    QVERIFY(mTestView->mViewReady);
    QCOMPARE(spy.count(), 1);
    spy.clear();
    
    emit testSignal();
    QCOMPARE(spy.count(), 0);
    
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
    // no current list
    ////////////
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 0);
    
    mTestView->activateView(TMPXItemId::InvalidId());
    
    ////////////
    // dialog loading fails
    ////////////
    VideoCollectionUiLoaderData::mFailDialogLoad = true;
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 0);

    ////////////
    // selection dialog exists
    ////////////
    VideoCollectionUiLoaderData::mFailDialogLoad = false;
    VideoSortFilterProxyModelData::mOpenedItemId = TMPXItemId(2, 3);
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 1);
    QCOMPARE(VideoListSelectionDialogData::mSelectionType, (int)VideoListSelectionDialog::EDeleteVideos);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoSortFilterProxyModelData::mOpenedItemId);
    
    ////////////
    // test second successful activation.
    ////////////
    VideoSortFilterProxyModelData::mOpenedItemId = TMPXItemId(3, 2);
    emit testSignal();
    QCOMPARE(VideoListSelectionDialogData::mMultiSelectionLaunchCount, 2);
    QCOMPARE(VideoListSelectionDialogData::mSelectionType, (int)VideoListSelectionDialog::EDeleteVideos);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoSortFilterProxyModelData::mOpenedItemId);

    cleanup();
}

// ---------------------------------------------------------------------------
// testAboutToShowMainMenuSlot
// ---------------------------------------------------------------------------
//
void TestListView::testAboutToShowMainMenuSlot()
{
    XQServiceUtilXtra *serviceUtil = XQServiceUtilXtra::instance();
    VideoServices *videoServices = VideoServices::instance();

    // View	is not initialized.
	init(false);
	connect( this, SIGNAL(testSignal(int)),	mTestView, SLOT(aboutToShowMainMenuSlot()) );
	emit testSignal(0);
	cleanup();

	HbAction *action(0);

	// Collections is triggered	from toolbar.
	init();
	connect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToShowMainMenuSlot()));
	mTestView->activateView(TMPXItemId::InvalidId());
	setRowCount(1);
	action = mTestView->mToolbarActions[VideoListView::ETBActionCollections];
	QVERIFY(action != 0);
	action->setChecked(false);
	action->trigger();
	emit testSignal();
	int visible = visibleMenuActions();
	QCOMPARE(visible, 3);
	cleanup();

	// All videos is triggered from	toolbar.
	init();
    connect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToShowMainMenuSlot()));
    mTestView->activateView(TMPXItemId::InvalidId());
    setRowCount(1);
	action = mTestView->mToolbarActions[VideoListView::ETBActionAllVideos];
	QVERIFY(action != 0);
	action->setChecked(false);
	action->trigger();
    emit testSignal();
	visible = visibleMenuActions();
	QCOMPARE(visible, 5);
	cleanup();

	// Add videos action is	visible.
	init();
    connect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToShowMainMenuSlot()));
    mTestView->activateView(TMPXItemId::InvalidId());
	mTestView->toolBar()->clearActions();
	mTestView->toolBar()->addActions( mTestView->mToolbarCollectionActionGroup->actions() );
	setRowCount(1);
	QVERIFY(action != 0);
    emit testSignal();
	visible = visibleMenuActions();
	QCOMPARE(visible, 4);
	cleanup();

	// Model has no	items.
	init();
    connect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToShowMainMenuSlot()));
    mTestView->activateView(TMPXItemId::InvalidId());
	setRowCount(0);
    emit testSignal();
	visible = visibleMenuActions();
	QCOMPARE(visible, 0);
	cleanup();

	// Toolbar action group	is null
	init();
    connect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToShowMainMenuSlot()));
    mTestView->activateView(TMPXItemId::InvalidId());
	setRowCount(1);
	QActionGroup* actionGroup =	mTestView->mToolbarViewsActionGroup;
	mTestView->mToolbarViewsActionGroup = NULL;
    emit testSignal();
	mTestView->mToolbarViewsActionGroup = actionGroup;
	cleanup();
	
	// about to show main menu:
	// -is service
	// -current service is browse
	// -no current list
    serviceUtil->setCurrentService(true);
    videoServices->mCurrentService = VideoServices::EBrowse;
    init(true);
    connect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToShowMainMenuSlot()));
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 0);
    cleanup();

    // about to show main menu:
    // -is service
    // -current service is browse
    // -no items
    serviceUtil->setCurrentService(true);
    videoServices->mCurrentService = VideoServices::EBrowse;
    init(true);
    connect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToShowMainMenuSlot()));
    mTestView->activateView(TMPXItemId::InvalidId());
    emit testSignal();
    visible = visibleMenuActions();
    QCOMPARE(visible, 0);
    cleanup();

    // about to show main menu:
    // -is service
    // -current service is browse
    serviceUtil->setCurrentService(true);
    videoServices->mCurrentService = VideoServices::EBrowse;
	init(true);
	connect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToShowMainMenuSlot()));
    mTestView->activateView(TMPXItemId::InvalidId());
    setRowCount(3, &mTestView->mCurrentList->getModel());
    emit testSignal();
    QVERIFY(isActionVisible(DOCML_NAME_DELETE_MULTIPLE));
    QVERIFY(isActionVisible(DOCML_NAME_SORT_BY_DATE));
    QVERIFY(isActionVisible(DOCML_NAME_SORT_BY_NAME));
    QVERIFY(isActionVisible(DOCML_NAME_SORT_BY_SIZE));
	cleanup();

	// final cleanup
    serviceUtil->decreaseReferenceCount();
    videoServices->decreaseReferenceCount();
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
	QString testString;

	// View	is not initialized.
	init(false);
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&, const QModelIndex&)),
	    mTestView, SLOT(collectionOpenedSlot(bool, const QString&, const QModelIndex&)));
	emit testCollectionOpenedSignal(false, testString, QModelIndex());
	cleanup();

	// No toolbar actions.
	init(true);
	mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET); // load model
	QModelIndex index = VideoCollectionWrapperData::mCollectionsModel->index(0, 0);
	// Clear actions.
	QMap<VideoListView::TViewActionIds,	HbAction*> backupActions =
		QMap<VideoListView::TViewActionIds,	HbAction*>(mTestView->mToolbarActions);
	mTestView->mToolbarActions.clear();
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&, const QModelIndex&)), 
	    mTestView, SLOT(collectionOpenedSlot(bool, const QString&, const QModelIndex&)));
	emit testCollectionOpenedSignal(true, testString, index);
	mTestView->mToolbarActions.unite(backupActions);
	cleanup();

	// index is invalid
	init();
	emit testCollectionOpenedSignal(true, testString, QModelIndex());
	cleanup();
	
	// current list is null
	init();
	VideoListWidget *tmpList = mTestView->mCurrentList;
	emit testCollectionOpenedSignal(true, testString, index);
	mTestView->mCurrentList = tmpList;
	cleanup();
	
	// current list is collectioncontentwidget
	init();
	cleanup();
	
	// index is for video item and not for collection.
	init();
	cleanup();
	
	// non-error cases:
	testString = "Test text string";

	// Collection opened with default collection.
	init();
    mTestView->mCurrentList = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);
    index = VideoCollectionWrapperData::mCollectionsModel->index(0, 0);
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&, const QModelIndex&)), 
	    mTestView, SLOT(collectionOpenedSlot(bool, const QString&, const QModelIndex&)));
	emit testCollectionOpenedSignal(true, testString, index);
	// Verify checkable	and	visible	toolbar	actions.
	cleanup();
	
	// Collection opened with userdefined album.
	init();
	cleanup();
	
	// Collection closed.
	init();
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&, const QModelIndex&)), 
	    mTestView, SLOT(collectionOpenedSlot(bool, const QString&, const QModelIndex&)));
	emit testCollectionOpenedSignal(false, testString, QModelIndex());
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
    mTestView->mModelReady = true;
	connect(this, SIGNAL(testLayoutChangedSignal()), mTestView, SLOT(layoutChangedSlot()));
	connect(this, SIGNAL(testCollectionOpenedSignal(bool, const QString&, const QModelIndex&)), 
	    mTestView, SLOT(collectionOpenedSlot(bool, const QString&, const QModelIndex&)));

    VideoListWidget *videoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);   
    VideoListWidget *collectionWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);   
    VideoListWidget *collectionContentWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);   
	
    videoListWidget->mCurrentLevel = VideoCollectionCommon::ELevelVideos;
    collectionWidget->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    collectionContentWidget->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
    
    HbGroupBox* label = mUiLoader->findWidget<HbGroupBox>(DOCML_NAME_VC_HEADINGBANNER);
    
	mTestView->mCurrentList = videoListWidget; 
	emit testLayoutChangedSignal();
	QCOMPARE( label->heading(), QString("txt_videos_subtitle_ln_videos") );

    mTestView->mCurrentList = collectionWidget; 
	emit testLayoutChangedSignal();
    QCOMPARE( label->heading(), QString("txt_videos_subtitle_ln_collections") );

    mTestView->mCurrentList = collectionContentWidget; 
	emit testLayoutChangedSignal();
    QCOMPARE( label->heading(), QString("txt_videos_subtitle_1_l2") );

    // how to test the collection name?
	
	cleanup();
}

// ---------------------------------------------------------------------------
// testShowHint
// ---------------------------------------------------------------------------
//
void TestListView::testShowHint()
{
    init(true);
    connect(this, SIGNAL(testLayoutChangedSignal()), mTestView, SLOT(layoutChangedSlot()));
    mTestView->mModelReady = true;
    
    // current list is null. (cannot be verified, run for coverity    
    emit testLayoutChangedSignal();   
    mTestView->activateView(TMPXItemId::InvalidId());
  
    // hint widget cannot be loaded. (cannot be tested, run for coverity)
    VideoCollectionUiLoaderData::mFindFailureNameList.append(DOCML_NAME_VC_VIDEOHINTWIDGET);
    emit testLayoutChangedSignal();
    VideoCollectionUiLoaderData::mFindFailureNameList.clear();
    
    VideoHintWidget *hintWidget = mUiLoader->findWidget<VideoHintWidget>(DOCML_NAME_VC_VIDEOHINTWIDGET);    
    
    /////
    // hint widget showing
    // model not ready, row count zero.
    mTestView->mModelReady = false;
    setRowCount(0);
    mTestView->mCurrentList =  mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONWIDGET);
    emit testLayoutChangedSignal();
    QVERIFY(hintWidget->isVisible() == false );
    QVERIFY(VideoHintWidgetData::mSettedButtonShowLevel);
    
    // model not ready, row count not zero.
    setRowCount(1);
    emit testLayoutChangedSignal();    
    QVERIFY(hintWidget->isVisible() == false );
    QVERIFY(VideoHintWidgetData::mSettedButtonShowLevel);
    QVERIFY(mTestView->mModelReady);
    
    // model ready, row count not zero
    mTestView->mModelReady = true;
    emit testLayoutChangedSignal();    
    QVERIFY(hintWidget->isVisible() == false );
    QVERIFY(VideoHintWidgetData::mSettedButtonShowLevel);

    // model ready, row count is zero
    setRowCount(0);
    emit testLayoutChangedSignal();    
    QVERIFY(hintWidget->isVisible() == true );
    QVERIFY(VideoHintWidgetData::mSettedButtonShowLevel);

    // model ready, row count is zero, show to be false
    connect(this, SIGNAL(testSignal()), mTestView, SLOT(openCollectionViewSlot()));
    emit testSignal();
    QVERIFY(hintWidget->isVisible() == false );
    QVERIFY(VideoHintWidgetData::mSettedButtonShowLevel);
    disconnect(this, SIGNAL(testSignal()), mTestView, SLOT(openCollectionViewSlot()));
   
    mTestView->mCurrentList =  mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
    
    ////////
    // toolbar setup
    mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos]->setVisible(true);

    // mToolbarViewsActionGroup is null
    QActionGroup *tmp = mTestView->mToolbarViewsActionGroup;
    mTestView->mToolbarViewsActionGroup = 0;
    emit testLayoutChangedSignal();    
    QVERIFY(mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos]->isVisible());
    mTestView->mToolbarViewsActionGroup = tmp;
    
    //  mToolbarCollectionActionGroup is null
    tmp = mTestView->mToolbarCollectionActionGroup;
    mTestView->mToolbarCollectionActionGroup = 0;
    emit testLayoutChangedSignal();    
    QVERIFY(mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos]->isVisible());
    mTestView->mToolbarCollectionActionGroup = tmp;
    
    // mIsService is true
    mTestView->mIsService = true;
    emit testLayoutChangedSignal();    
    QVERIFY(mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos]->isVisible());
    mTestView->mIsService = false;
    
    // show -flag is true, currentlist level != VideoCollectionCommon::ELevelDefaultColl
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelAlbum);
    emit testLayoutChangedSignal();    
    QVERIFY(!mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos]->isVisible());
    
    mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos]->setVisible(true);
    // show -flag is true, currentlist level == VideoCollectionCommon::ELevelDefaultColl
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelDefaultColl);
    emit testLayoutChangedSignal();    
    QVERIFY(!mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos]->isVisible());
       
    // show -flag is false
    setRowCount(1);
    emit testLayoutChangedSignal();   
    QVERIFY(mTestView->mToolbarActions[VideoListView::ETBActionRemoveVideos]->isVisible());
    
    ///////
    // sub label
    
    // sub label cannot be found (cannot be verified, run for coverity)
    VideoCollectionUiLoaderData::mFindFailureNameList.append(DOCML_NAME_VC_HEADINGBANNER);
    emit testLayoutChangedSignal();
    VideoCollectionUiLoaderData::mFindFailureNameList.clear();
    
    HbGroupBox *subLabel =
            mUiLoader->findWidget<HbGroupBox>(
                DOCML_NAME_VC_HEADINGBANNER);
    
    // show -flag is false
    setRowCount(1);
    emit testLayoutChangedSignal();  
    QVERIFY(subLabel->isVisible());
    
    // show -flag is true, current list's level != ELevelVideos
    setRowCount(0);
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelAlbum);
    emit testLayoutChangedSignal();  
    QVERIFY(subLabel->isVisible());
    
    // show -flag is true, current list's level == ELevelVideos
    setRowCount(0);
    mTestView->mCurrentList->activate(VideoCollectionCommon::ELevelVideos);
    emit testLayoutChangedSignal();  
    QVERIFY(!subLabel->isVisible());
}

// ---------------------------------------------------------------------------
// testTitleReadySlot
// ---------------------------------------------------------------------------
//
void TestListView::testTitleReadySlot()
{
    init(true);
    
    QVERIFY(connect(this, SIGNAL(testSignal(const QString &)), mTestView, SLOT(titleReadySlot(const QString &))));
    
    QString title("TEST");
    
    emit testSignal(title);
    
    QVERIFY(mTestView->mTitle == title);
    
    disconnect(this, SIGNAL(testSignal(const QString &)), mTestView, SLOT(titleReadySlot(const QString &)));
    cleanup();
}

// ---------------------------------------------------------------------------
// testDoDelayedSlot
// ---------------------------------------------------------------------------
//
void TestListView::testDoDelayedsSlot()
{
    init(true);
    
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestView, SLOT(doDelayedsSlot())));
    
    emit testSignal();
    
    QCOMPARE(VideoCollectionUiLoaderData::mLastLoadPhasedData, (int)VideoCollectionUiLoaderParam::LoadPhaseSecondary); 
    
    disconnect(this, SIGNAL(testSignal()), mTestView, SLOT(doDelayedsSlot()));
    cleanup();
}

// ---------------------------------------------------------------------------
// testOpenNewAlbumSlot
// ---------------------------------------------------------------------------
//
void TestListView::testOpenNewAlbumSlot()
{
    init(false);
    
    QVERIFY(connect(this, SIGNAL(testSignal(const QModelIndex &, int, int)), mTestView, SLOT(openNewAlbumSlot(const QModelIndex &, int, int))));

    QModelIndex index;
    
    // Not initialized, no mCurrentList
    emit testSignal(index, 0, 0);
    // TODO verify
    
    // Good case
    QVERIFY(mTestView->initializeView() == 0);
    mTestView->activateView(TMPXItemId::InvalidId());
    setRowCount(1);
    emit testSignal(index, 0, 0);
    // TODO verify
    
    // Invalid index
    emit testSignal(index, -1, 0);
    // TODO verify
    
    disconnect(this, SIGNAL(testSignal()), mTestView, SLOT(doDelayedsSlot()));
    cleanup();
}

// ---------------------------------------------------------------------------
// testAboutToChangeOrientationSlot
// ---------------------------------------------------------------------------
//
void TestListView::testAboutToChangeOrientationSlot()
{
    init(false);
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToChangeOrientationSlot())));

    emit testSignal();
    
    // nothing to verify
    
    disconnect(this, SIGNAL(testSignal()), mTestView, SLOT(aboutToChangeOrientationSlot()));
    cleanup();
}

// ---------------------------------------------------------------------------
// testOrientationChangedSlot
// ---------------------------------------------------------------------------
//
void TestListView::testOrientationChangedSlot()
{
    init(false);
    QVERIFY(connect(this, SIGNAL(testSignal(Qt::Orientation)), mTestView, 
            SLOT(orientationChangedSlot(Qt::Orientation))));

    HbView::mSetItemVisibleLast = false;
    
    emit testSignal(Qt::Vertical);
    QVERIFY(HbView::mSetItemVisibleLast == true);

    emit testSignal(Qt::Horizontal);
    QVERIFY(HbView::mSetItemVisibleLast == false);
    
    disconnect(this, SIGNAL(testSignal(Qt::Orientation)), mTestView, 
            SLOT(orientationChangedSlot(Qt::Orientation)));    
    cleanup();
}

// ---------------------------------------------------------------------------
// testCreateCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListView::testCreateCollectionSlot()
{
    init(false);
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestView, SLOT(createCollectionSlot())));

    HbInputDialog *dialog = new HbInputDialog(); 
    
    // not initialized, no mCurrentList
    emit testSignal();
    QCOMPARE(HbInputDialog::mGetTextCallCount, 0);
    QVERIFY(VideoSortFilterProxyModelData::mLastAddedAlbumName == "");

    QVERIFY(mTestView->initializeView() == 0);
    mTestView->activateView(TMPXItemId::InvalidId());
    
    // dialog canceled
    HbInputDialog::mValueCallCount = 0;
    emit testSignal();
    dialog->emitDialogFinished(mTestView, SLOT(createCollectionDialogFinished(HbAction *)), 1);
    QCOMPARE(HbInputDialog::mGetTextCallCount, 1);
    QVERIFY(VideoSortFilterProxyModelData::mLastAddedAlbumName == "");
    QVERIFY(HbInputDialog::mValueCallCount == 1);
    
    // empty name
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mValueReturnValue = "";
    HbInputDialog::mGetTextCallCount = 0;
    emit testSignal();
    dialog->emitDialogFinished(mTestView, SLOT(createCollectionDialogFinished(HbAction *)), 0);
    QCOMPARE(HbInputDialog::mGetTextCallCount, 1);
    QVERIFY(VideoSortFilterProxyModelData::mLastAddedAlbumName == "");
    QVERIFY(HbInputDialog::mValueCallCount == 1);
    
    // Good case.
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mValueReturnValue = "testAlbum";
    HbInputDialog::mGetTextCallCount = 0;
    emit testSignal();
    dialog->emitDialogFinished(mTestView, SLOT(createCollectionDialogFinished(HbAction *)), 0);
    QCOMPARE(HbInputDialog::mGetTextCallCount, 1);
    QVERIFY(VideoSortFilterProxyModelData::mLastAddedAlbumName == "testAlbum");
    QVERIFY(HbInputDialog::mValueCallCount == 1);
    
    HbInputDialog::mValueCallCount = 0;
    HbInputDialog::mValueReturnValue = "";
    HbInputDialog::mGetTextCallCount = 0;
    VideoSortFilterProxyModelData::mLastAddedAlbumName = "";
    
    disconnect(this, SIGNAL(testSignal()), mTestView, SLOT(createCollectionSlot()));
    cleanup();
}

// ---------------------------------------------------------------------------
// testAddVideosToCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListView::testAddVideosToCollectionSlot()
{
    init(false);
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestView, SLOT(addVideosToCollectionSlot())));

    VideoSortFilterProxyModelData::mOpenedItemId = TMPXItemId(0, 2);
    
    // Not initalized, no mCurrentList
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);
    
    QVERIFY(mTestView->initializeView() == 0);
    mTestView->activateView(TMPXItemId::InvalidId());
    
    // Selection dialog widget loading fails.
    VideoCollectionUiLoaderData::mFindFailure = true;
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);
    
    // Current level is not album
    VideoListSelectionDialogData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoCollectionUiLoaderData::mFindFailure = false;
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == VideoListSelectionDialog::EAddToCollection);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoSortFilterProxyModelData::mOpenedItemId);
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);

    // Current level is album, video list widget load fails.
    VideoListSelectionDialogData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoCollectionUiLoaderData::reset();
    HbDocumentLoader::mVideoListWidgetFailure = true;
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == VideoListSelectionDialog::EAddToCollection);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoSortFilterProxyModelData::mOpenedItemId);
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);
    
    // Current level is album, but model is empty.
    HbDocumentLoader::mVideoListWidgetFailure = false;
    VideoListSelectionDialogData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoCollectionUiLoaderData::reset();
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);

    // Current level is album, all videos are in the album 
    VideoListSelectionDialogData::reset();
    VideoListDataModelData::reset();
    VideoCollectionViewUtilsData::reset();
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
    VideoListWidget *allVideos = mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_VIDEOLISTWIDGET);
    QVERIFY(allVideos);
    setRowCount(3, allVideos->mModel);
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == -1);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == TMPXItemId::InvalidId());
    QVERIFY(VideoCollectionViewUtilsData::mLastError == 
            VideoCollectionCommon::statusAllVideosAlreadyInCollection);
    
    // Current level is album, all videos are not in the album
    VideoListSelectionDialogData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoListDataModelData::reset();
    QVERIFY(allVideos);
    setRowCount(3, allVideos->mModel);
    // Must have different model than selection dialog has. Otherwise Qt optimizes rowCount calls 
    // to source model and VideoListDataModelData::mRowCountDecrement hack doesn't work.
    QVERIFY(connect(this, SIGNAL(testSignal2()), mTestView, SLOT(openCollectionViewSlot())));
    emit testSignal2();
    disconnect(this, SIGNAL(testSignal2()), mTestView, SLOT(openCollectionViewSlot()));
    QVERIFY(mTestView->initializeView() == 0);
    mTestView->activateView(TMPXItemId::InvalidId());
    VideoListDataModelData::mRowCountDecrement = 1;
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelAlbum;
    setRowCount(1);
    emit testSignal();
    QVERIFY(VideoListSelectionDialogData::mSelectionType == VideoListSelectionDialog::EAddToCollection);
    QVERIFY(VideoListSelectionDialogData::mSettedMpxId == VideoSortFilterProxyModelData::mOpenedItemId);
    QCOMPARE(VideoCollectionViewUtilsData::mLastError, 0);
    
    disconnect(this, SIGNAL(testSignal()), mTestView, SLOT(addVideosToCollectionSlot()));
    
    cleanup();
}

// ---------------------------------------------------------------------------
// testRemoveVideosFromCollectionSlot
// ---------------------------------------------------------------------------
//
void TestListView::testRemoveVideosFromCollectionSlot()
{
    VideoListSelectionDialogData::reset();
    VideoCollectionViewUtilsData::reset();
    VideoCollectionUiLoaderData::reset();
    HbDocumentLoader::mVideoListWidgetFailure = false;
    
    init(false);
    
    QVERIFY(connect(this, SIGNAL(testSignal()), mTestView, SLOT(removeVideosFromCollectionSlot())));
    
    // Not initialized, no current list
    emit testSignal();
    
    QVERIFY(mTestView->initializeView() == 0);
    mTestView->activateView(TMPXItemId::InvalidId());
    setRowCount(1);
    
    // Wrong level.
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelCategory;
    emit testSignal();
    
    // No items in model.
    mTestView->mCurrentList->mCurrentLevel = VideoCollectionCommon::ELevelDefaultColl;
    emit testSignal();

    // Fail to load selection dialog.
    VideoCollectionUiLoaderData::mFindFailure = true;
    emit testSignal();
    
    // Open item is invalid
    VideoCollectionUiLoaderData::mFindFailure = false;
    VideoSortFilterProxyModelData::mOpenedItemId = TMPXItemId::InvalidId();
    emit testSignal();
    
    // Open item is video!
    VideoSortFilterProxyModelData::mOpenedItemId = TMPXItemId(0, KVcxMvcMediaTypeVideo);
    emit testSignal();
    
    // Good case.
    VideoSortFilterProxyModelData::mOpenedItemId = TMPXItemId(0, KVcxMvcMediaTypeAlbum);
    emit testSignal();
    
    disconnect(this, SIGNAL(testSignal()), mTestView, SLOT(removeVideosFromCollectionSlot()));
    
    cleanup();
}

// End of file
