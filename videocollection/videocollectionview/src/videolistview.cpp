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
* Description:   Videolist view class source code
*
*/

#include <xqserviceutil.h>
#include <QActionGroup>
#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hblabel.h>
#include <hbmessagebox.h>
#include <hbstackedwidget.h>
#include <hbstackedlayout.h>
#include <hblistwidget.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbgroupbox.h>
#include <hbpushbutton.h>

#include "videoservices.h"
#include "videolistselectiondialog.h"
#include "videocollectionviewutils.h"
#include "videolistwidget.h"
#include "videohintwidget.h"
#include "videolistview.h"
#include "videocollectioncommon.h"
#include "videocollectionwrapper.h"
#include "videosortfilterproxymodel.h"
#include "videocollectionuiloader.h"

// remove these
#include <QDebug>

const int VIDEO_LIST_VIEW_OPTION_MENU_COUNT = 4;
const int VIDEO_LIST_VIEW_SORT_MENU_COUNT = 5;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListView::VideoListView(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent) :
HbView(parent),
mSortMenu(0),
mUiUtils(VideoCollectionViewUtils::instance()),
mWrapper(VideoCollectionWrapper::instance()),
mUiLoader(uiLoader),
mModel(0),
mIsService(false),
mModelReady(false),
mSubLabel(0),
mOptionsMenu(0),
mVideoServices(0),
mVideoListWidget(0),
mVideoHintWidget(0),
mToolbarViewsActionGroup(0),
mToolbarCollectionActionGroup(0),
mSelectionDialog(0)
{
    // NOP
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListView::~VideoListView()
{
    qDebug() << "VideoListView::~VideoListView()";
    delete mSelectionDialog;
    // widgets' destructors are being called throught
    // orbit framework. No need to remove them here.
    mMenuActions.clear();
    mToolbarActions.clear();
    mSortingRoles.clear();

    if(mVideoServices)
    {
    	mVideoServices->decreaseReferenceCount();
    	mVideoServices = 0;
    }

    if(mWrapper)
    {
        mWrapper->decreaseReferenceCount();
        mWrapper = 0;
    }
}

// ---------------------------------------------------------------------------
// initializeView()
// ---------------------------------------------------------------------------
//
int VideoListView::initializeView()
{
	if(!mUiLoader)
	{
		return -1;
	}

    if (XQServiceUtil::isService() && !mVideoServices)
    {
    	mIsService = true;

    	mVideoServices = VideoServices::instance();

    	if (!mVideoServices)
        {
        	return -1;
		}
        else
        {
        	connect(mVideoServices, SIGNAL(titleReady(const QString&)), this, SLOT(titleReadySlot(const QString&)));
        }
	}

    // create model
    if(mWrapper)
    {
        mModel = mWrapper->getModel();
    }

    if(!mModel || mModel->open(VideoListWidget::ELevelVideos) < 0)
    {
    	cleanup();
        return -1;
    }
    
    int sortRole(VideoCollectionCommon::KeyDateTime);
    Qt::SortOrder sortOrder(Qt::AscendingOrder);
    
    // return value ignored, as in case of error the sortRole and sortOrder variables
    // stay at their predefined values, and in error cases those are the sorting values
    // that are used.
    mUiUtils.loadSortingValues(sortRole, sortOrder);
    
    mModel->doSorting(sortRole, sortOrder, false);

	mOptionsMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);

    if(!mOptionsMenu)
    {
		cleanup();
		return -1;
	}

    mSubLabel = mUiLoader->findWidget<HbGroupBox>(DOCML_NAME_VC_HEADINGBANNER);

	if(!mSubLabel)
	{
		cleanup();
		return -1;
	}

	mSubLabel->setCollapsable(false);
	
    // allocate and initialize views, menus and toolbar
    if(createVideoWidget() != 0
    || createHintWidget() != 0
    || createToolbar() != 0
    || createMainMenu() != 0)
    {
        cleanup();
        return -1;
    }

	mSubLabel->setHeading(tr("Retrieving list.."));

    mCollectionName = "";

    return 0;
}

// ---------------------------------------------------------------------------
// titleReadySlot()
// ---------------------------------------------------------------------------
//
void VideoListView::titleReadySlot(const QString& title)
{
	setTitle(title);
}

// ---------------------------------------------------------------------------
// activateView()
// ---------------------------------------------------------------------------
//
int VideoListView::activateView()
{
	mOptionsMenu->setEnabled(true);

    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

    if(!connect(mainWnd, SIGNAL(aboutToChangeOrientation()),
                this, SLOT( aboutToChangeOrientationSlot())) ||
       !connect(mainWnd, SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChangedSlot(Qt::Orientation))) ||
       !connect(mWrapper, SIGNAL(asyncStatus(int, QVariant&)),
                this, SLOT(handleAsyncStatusSlot(int, QVariant&))) ||

		//TODO: seems like rowsremoved is not signaled when files are removed, but layoutchanged
		//TODO: on the other hand, layoutchanged is not signaled when items are added but rowsinserted is

        !connect(mModel,
    			SIGNAL(rowsInserted(const QModelIndex&, int, int)),
    			this, SLOT(layoutChangedSlot()))  ||
    	!connect(mModel,
    			SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
    			this, SLOT(layoutChangedSlot()))  ||
        //TODO: connect to rowsremoved rowsinserted once delete results emitting removed
    	!connect(mModel,
    			SIGNAL(layoutChanged()),
    			this, SLOT(layoutChangedSlot()))  ||
    	!connect(mModel,
    			SIGNAL(modelReady()),
    			this, SLOT(modelReadySlot())))
    {
        // deactivate view so we get rid of dangling connections.
        deactivateView();
        return -1;
    }

    // activate current vidget, first make sure it's one that can be activated
    if(mVideoListWidget)
    {
        int result = mVideoListWidget->activate();
        if(result < 0) {
            // activate failed, deactivate view so we get rid of dangling connections.
            deactivateView();
        }
        else if (mSubLabel->heading() != tr("Retrieving list.."))
        {
        	updateSubLabel();
        }
        
        showHint();
        mainWnd->unsetOrientation();
        
        return result;
    }

    // deactivate view so we get rid of dangling connections.
    deactivateView();
    return -1;
}

// ---------------------------------------------------------------------------
// modelReadySlot
// ---------------------------------------------------------------------------
//
void VideoListView::modelReadySlot()
{
    mModelReady = true;
    // since the reset signal arrives after
    // layout changed, need to make sure that
    // view is updated in case needed
    layoutChangedSlot();
}

// ---------------------------------------------------------------------------
// layoutChangedSlot
// ---------------------------------------------------------------------------
//
void VideoListView::layoutChangedSlot()
{
    updateSubLabel();
    
    if(mModelReady)
    {
        showHint();
    }
}

// ---------------------------------------------------------------------------
// deactivateView()
// ---------------------------------------------------------------------------
//
void VideoListView::deactivateView()
{
    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

    disconnect(mainWnd, SIGNAL(aboutToChangeOrientation()),
               this, SLOT(aboutToChangeOrientationSlot()));

    disconnect(mainWnd, SIGNAL(orientationChanged(Qt::Orientation)),
               this, SLOT(orientationChangedSlot(Qt::Orientation)));

    disconnect(mWrapper, SIGNAL(asyncStatus(int, QVariant&)),
               this, SLOT(handleAsyncStatusSlot(int, QVariant&)));

	disconnect(mModel,
			SIGNAL(rowsInserted(const QModelIndex&, int, int)),
			this, SLOT(layoutChangedSlot()));
	disconnect(mModel,
			SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
			this, SLOT(layoutChangedSlot()));
	disconnect(mModel,
			SIGNAL(layoutChanged()),
			this, SLOT(layoutChangedSlot()));
	disconnect(mModel,
			SIGNAL(modelReady()),
			this, SLOT(modelReadySlot()));

	mOptionsMenu->setVisible(false);
    mOptionsMenu->setEnabled(false);
    
    showHint(false);
    
    if(mVideoListWidget)
    {
        mVideoListWidget->deactivate();
    }
}

// ---------------------------------------------------------------------------
// back()
// ---------------------------------------------------------------------------
//
void VideoListView::back()
{
    // NOP
}

// ---------------------------------------------------------------------------
// cleanup()
// ---------------------------------------------------------------------------
//
void VideoListView::cleanup()
{
    delete mToolbarViewsActionGroup;
    mToolbarViewsActionGroup = 0;

    delete mToolbarCollectionActionGroup;
    mToolbarCollectionActionGroup = 0;
    
    // not deleted as the uiloader owns these.
    mVideoListWidget = 0;
    mVideoHintWidget = 0;
}

// ---------------------------------------------------------------------------
// createMainMenu()
// ---------------------------------------------------------------------------
//
int VideoListView::createMainMenu()
{
	if(mMenuActions.count() > 0)
	{
		// Menu is already created.
		return 0;
	}

	if(!connect(mOptionsMenu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMainMenuSlot())))
	{
	    return -1;
	}

	mMenuActions[EActionAddToCollection] = (HbAction*)mUiLoader->findObject<QObject>(DOCML_NAME_ADD_TO_COLLECTION);
    connect(mMenuActions[EActionAddToCollection], SIGNAL(triggered()), this, SLOT(debugNotImplementedYet()));

    mMenuActions[EActionNewCollection] =  (HbAction*)mUiLoader->findObject<QObject>(DOCML_NAME_CREATE_COLLECTION);
    connect(mMenuActions[EActionNewCollection], SIGNAL(triggered()), this, SLOT(createCollectionSlot()));

    mMenuActions[EActionDelete]          = (HbAction*)mUiLoader->findObject<QObject>(DOCML_NAME_DELETE_MULTIPLE);
    connect(mMenuActions[EActionDelete], SIGNAL(triggered()), this, SLOT(deleteItemsSlot()));

    mSortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);

	mMenuActions[EActionSortBy] = mSortMenu->menuAction();

    // submenu items and roles for sorting
    mMenuActions[EActionSortByDate]             = (HbAction*)mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_DATE);
    connect(mMenuActions[EActionSortByDate], SIGNAL(triggered()), this, SLOT(startSorting()));
    mSortingRoles[mMenuActions[EActionSortByDate]] = VideoCollectionCommon::KeyDateTime;

    mMenuActions[EActionSortByName]             = (HbAction*)mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_NAME);
    connect(mMenuActions[EActionSortByName], SIGNAL(triggered()), this, SLOT(startSorting()));
    mSortingRoles[mMenuActions[EActionSortByName]] = Qt::DisplayRole;

    mMenuActions[EACtionSortByItemCount]  = (HbAction*)mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    connect(mMenuActions[EACtionSortByItemCount], SIGNAL(triggered()), this, SLOT(startSorting()));
    // TODO:
    mSortingRoles[mMenuActions[EACtionSortByItemCount]] = 0;

    mMenuActions[EActionSortByLength]     = (HbAction*)mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_TOTAL_LENGTH);
    connect(mMenuActions[EActionSortByLength], SIGNAL(triggered()), this, SLOT(startSorting()));
    // TODO:
    mSortingRoles[mMenuActions[EActionSortByLength]] = 0;

    mMenuActions[EActionSortBySize]             = (HbAction*)mUiLoader->findObject<QObject>(DOCML_NAME_SORT_BY_SIZE);
    connect(mMenuActions[EActionSortBySize], SIGNAL(triggered()), this, SLOT(startSorting()));
    mSortingRoles[mMenuActions[EActionSortBySize]] = VideoCollectionCommon::KeySizeValue;

    if(mSortMenu->actions().count() != VIDEO_LIST_VIEW_SORT_MENU_COUNT
	|| mOptionsMenu->actions().count()  != VIDEO_LIST_VIEW_OPTION_MENU_COUNT)
    {
        return -1;
    }

    return 0;
}

// ---------------------------------------------------------------------------
// createToolbar()
// Creates toolbar, toolbar actions and toolbar icons
// ---------------------------------------------------------------------------
//
int VideoListView::createToolbar()
{
    // Create actiongroup and add all actions to it. This ensures that only one is
    // active at certain moment.

    if(!mToolbarViewsActionGroup && !mToolbarCollectionActionGroup)
    {
    	mToolbarViewsActionGroup = new QActionGroup(this);
        mToolbarCollectionActionGroup = new QActionGroup(this);

        if(!mToolbarViewsActionGroup || !mToolbarCollectionActionGroup)
        {
            delete mToolbarViewsActionGroup;
            mToolbarViewsActionGroup = 0;

            delete mToolbarCollectionActionGroup;
            mToolbarCollectionActionGroup = 0;

            return -1;
        }

        // create toolbar item actions

        // All Videos tab
        mToolbarActions[ETBActionAllVideos] = createAction(tr("All videos"), ":/images/qtg_mono_video_all.svg",
                mToolbarViewsActionGroup, SLOT(openAllVideosViewSlot()));

        // Collections tab
        mToolbarActions[ETBActionCollections] = createAction(tr("Collections"), ":/images/qtg_mono_video_collection.svg",
                mToolbarViewsActionGroup, SLOT(openCollectionViewSlot()));

        if (!mIsService)
        {
			// Services tab
			mToolbarActions[ETBActionServices] = createAction(tr("Services"), ":/images/qtg_mono_video_services.svg",
					mToolbarViewsActionGroup, SLOT(openServicesViewSlot()));
			// Add Videos tab
			mToolbarActions[ETBActionAddVideos] = createAction(tr("Add videos"), ":/images/mono_video_addvideos.svg",
					mToolbarCollectionActionGroup, SLOT(addVideosToCollectionSlot()));

			// Remove Videos tab
			mToolbarActions[ETBActionRemoveVideos] = createAction(tr("Remove videos"), ":/images/mono_video_removevideos.svg",
					mToolbarCollectionActionGroup, SLOT(debugNotImplementedYet()));
        }

        // Sort by tab
        mToolbarActions[ETBActionSortVideos] = createAction(tr("Sort by"), ":/images/mono_video_sortvideos.svg",
                mToolbarCollectionActionGroup, SLOT(openSortByMenuSlot()));

        HbToolBar *bar = toolBar(); // First call to toolBar() creates the object, so on failure it could return 0.

        if(   !bar
		   || !mToolbarActions[ETBActionAllVideos]
           || !mToolbarActions[ETBActionCollections]
		   || !mToolbarActions[ETBActionSortVideos]
           || (!mToolbarActions[ETBActionServices]     && !mIsService)
           || (!mToolbarActions[ETBActionAddVideos]    && !mIsService)
           || (!mToolbarActions[ETBActionRemoveVideos] && !mIsService))
        {
        	delete mToolbarActions[ETBActionAllVideos];
            delete mToolbarActions[ETBActionCollections];
            delete mToolbarActions[ETBActionServices];
            delete mToolbarActions[ETBActionAddVideos];
            delete mToolbarActions[ETBActionRemoveVideos];
            delete mToolbarActions[ETBActionSortVideos];
        	return -1;
        }

        // Collection view actions are not checkable
        mToolbarActions[ETBActionAllVideos]->setCheckable(true);
        mToolbarActions[ETBActionCollections]->setCheckable(true);

        if (!mIsService)
        {
        	mToolbarActions[ETBActionServices]->setCheckable(true);
        }

        // Allvideos is checked at creation phase
        mToolbarActions[ETBActionAllVideos]->setChecked(true);

        bar->addActions(mToolbarViewsActionGroup->actions());
    }

    return 0;
}

// ---------------------------------------------------------------------------
// createAction()
// ---------------------------------------------------------------------------
//
HbAction* VideoListView::createAction(QString tooltip, QString icon,
        QActionGroup* actionGroup, const char *slot)
{
    HbAction* action = new HbAction(actionGroup);
    if(!action) {
        return 0;
    }

    action->setToolTip(tooltip);
    HbIcon hbIcon(icon);
    action->setIcon(hbIcon);

    if(!connect(action, SIGNAL(triggered()), this, slot)) {
        // actiongroup deletion deletes this also.
        // return 0 tells that there was a problem in creation to caller.
        delete action;
        return 0;
    }

    return action;
}


// ---------------------------------------------------------------------------
// createVideoWidget()
// ---------------------------------------------------------------------------
//
int VideoListView::createVideoWidget()
{
    if(!mModel)
    {
        return -1;
    }
    if(!mVideoListWidget)
    {
        mVideoListWidget = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
        if(!mVideoListWidget )
        {
            return -1;
        }
        if(mVideoListWidget->initialize(*mModel, mVideoServices) < 0)
        {
            return -1;
        }

        if(!connect(mVideoListWidget, SIGNAL( command(int)), this, SIGNAL(command(int))) ||
           !connect(mVideoListWidget, SIGNAL( collectionOpened(bool, const QString&)), this, SLOT(collectionOpenedSlot(bool, const QString&))))
        {
            return -1;
        }
    }

    return 0;
}

// ---------------------------------------------------------------------------
// createHintWidget()
// ---------------------------------------------------------------------------
//
int VideoListView::createHintWidget()
{
    if(!mVideoHintWidget)
    {
        mVideoHintWidget = mUiLoader->findWidget<VideoHintWidget>(DOCML_NAME_VC_VIDEOHINTWIDGET);
        if(!mVideoHintWidget )
        {
            return -1;
        }
        if(mVideoHintWidget->initialize() < 0)
        {
            return -1;
        }
        
        HbPushButton* button = mUiLoader->findWidget<HbPushButton>(DOCML_NAME_HINT_BUTTON);
        if(!connect(button, SIGNAL(clicked(bool)), this, SLOT(openServicesViewSlot())))
        {
            return -1;
        }
    }

    return 0;
}

// ---------------------------------------------------------------------------
// showHint
// ---------------------------------------------------------------------------
//
void VideoListView::showHint(bool show)
{
    if (mModel && mModelReady && (mModel->rowCount() == 0) && mVideoHintWidget)
    {
        mVideoHintWidget->setVisible(show);
    }
    else if (mVideoHintWidget)
    {
        show = false;
        mVideoHintWidget->setVisible(false);
    }

    if(show && mToolbarViewsActionGroup && mToolbarCollectionActionGroup) 
    {
    	if(!mIsService)
    	{
    		mToolbarActions[ETBActionRemoveVideos]->setEnabled(false);
    	}
        mToolbarActions[ETBActionSortVideos]->setEnabled(false);
    } 
    else 
    {
    	if(!mIsService)
    	{
			if(mToolbarActions[ETBActionRemoveVideos]->isEnabled() == false) 
			{
				mToolbarActions[ETBActionRemoveVideos]->setEnabled(true);
			}
    	}
        if(mToolbarActions[ETBActionSortVideos]->isEnabled() == false) 
        {
            mToolbarActions[ETBActionSortVideos]->setEnabled(true);
        }
    }
}

// ---------------------------------------------------------------------------
// updateSubLabel
// ---------------------------------------------------------------------------
//
void VideoListView::updateSubLabel()
{
    //TODO: reduce unnecessary updates
    if (mModel && mVideoListWidget)
    {
        int itemCount = mModel->rowCount();

        VideoListWidget::TVideoListType type = mVideoListWidget->getType();
        if (type == VideoListWidget::EAllVideos)
        {
            mSubLabel->setHeading(tr("%1 videos").arg(itemCount));
        }
        else if (type == VideoListWidget::ECollections)
        {
            mSubLabel->setHeading(tr("%1 collections").arg(itemCount));
        }  
        else if (type == VideoListWidget::EDefaultColItems || 
                 type == VideoListWidget::EUserColItems)
        {
            mSubLabel->setHeading(tr("%1 (%2)").arg(mCollectionName).arg(itemCount));
        }
    }
}

// ---------------------------------------------------------------------------
// openAllVideosViewSlot()
// ---------------------------------------------------------------------------
//
void VideoListView::openAllVideosViewSlot()
{
	mModel->open(VideoListWidget::ELevelVideos);
	mVideoListWidget->activate(VideoListWidget::ELevelVideos);
    mVideoHintWidget->setLevel(VideoHintWidget::AllVideos);
	updateSubLabel();
}

// ---------------------------------------------------------------------------
// openCollectionViewSlot()
// ---------------------------------------------------------------------------
//
void VideoListView::openCollectionViewSlot()
{
    mModel->open(VideoListWidget::ELevelCategory);
	mVideoListWidget->activate(VideoListWidget::ELevelCategory);
	mVideoHintWidget->setLevel(VideoHintWidget::Collection);
	
	// the collection view is not empty, so we can hide the hint in advance.
	showHint(false);
	
	updateSubLabel();
}

// ---------------------------------------------------------------------------
// openservicesViewSlot()
// ---------------------------------------------------------------------------
//
void VideoListView::openServicesViewSlot()
{
    debugNotImplementedYet();
}

// ---------------------------------------------------------------------------
// startSorting()
// ---------------------------------------------------------------------------
//
void VideoListView::startSorting()
{
	if(!mModel || !mOptionsMenu)
    {
        return;
    }
	// Check that action is sort by and it has a sub menu.
    if(mOptionsMenu->activeAction() != mMenuActions[EActionSortBy] || !mOptionsMenu->activeAction()->menu())
	{
		return;
	}

    int role = mSortingRoles[mOptionsMenu->activeAction()->menu()->activeAction()];

    Qt::SortOrder order(Qt::AscendingOrder);
    
    if(mModel->sortRole() == role && mModel->sortOrder() == Qt::AscendingOrder)
    {
        order = Qt::DescendingOrder;
    }
    
    mModel->doSorting(role, order);
    
    mUiUtils.saveSortingValues(role, order);
}

// -------------------------------------------------------------------------------------------------
// aboutToChangeOrientationSlot()
// hide all items in the window during orientation change
// -------------------------------------------------------------------------------------------------
//
void VideoListView::aboutToChangeOrientationSlot()
{
    // this method is required for changing from to mediawall
}

// -------------------------------------------------------------------------------------------------
// orientationChangedSlot()
// orientation changed, items can be put back to visible after view has handled the change
// -------------------------------------------------------------------------------------------------
//
void VideoListView::orientationChangedSlot( Qt::Orientation orientation )
{
    // this method is required for changing from to mediawall
    
    // in landscape we need to hide the title and the toolbar. (also required for the mediawall?)
    // TODO: how about the feature where the toolbar comes visible if screen is tapped?
    this->setItemVisible(Hb::AllItems, orientation == Qt::Vertical);
}

// -------------------------------------------------------------------------------------------------
// deleteItemsSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::deleteItemsSlot()
{
    if(!mModel)
    {
        return;
    }
    if(!mSelectionDialog)
    {
        bool ok(false);
        mUiLoader->load(DOCML_VIDEOSELECTIONDIALOG_FILE, &ok);
        if(!ok)
        {
            return;
        }
        mSelectionDialog = mUiLoader->findObject<VideoListSelectionDialog>( DOCML_NAME_DIALOG );
    }
    mSelectionDialog->setContent(tr("Delete items"), mVideoListWidget);
    if(mSelectionDialog->exec() == mSelectionDialog->primaryAction())
    {
        // get selection and remove
        QItemSelection selection = mSelectionDialog->getSelection();
        mModel->deleteItems(selection.indexes());
        // invalid model to get UI updated
        mModel->invalidate();
    }
}

// -------------------------------------------------------------------------------------------------
// createCollectionSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::createCollectionSlot()
{
    debugNotImplementedYet();
}

// -------------------------------------------------------------------------------------------------
// addVideosToCollectionSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::addVideosToCollectionSlot()
{
    debugNotImplementedYet();
}

// -------------------------------------------------------------------------------------------------
// aboutToShowMainMenuSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::aboutToShowMainMenuSlot()
{
	if(!mModel || !mToolbarViewsActionGroup || !mToolbarCollectionActionGroup)
	{
		return;
	}

	int menuActionCount = 0;
    HbAction *action = 0;
    foreach(action, mMenuActions.values())
    {
        if(action)
        {
        	++menuActionCount;
        	action->setVisible(false);
        }
    }
    if(menuActionCount != mMenuActions.values().count() || mMenuActions.values().count() == 0)
    {
    	// fatal error, some action(s) was not created.
    	return;
    }

    if(!mModel->rowCount(QModelIndex()))
    {
        // no items, no menu
        return;
    }

    HbAction *firstAction = (HbAction*)(toolBar()->actions().first());

    if(mToolbarViewsActionGroup->checkedAction() == mToolbarActions[ETBActionAllVideos] &&
       firstAction == mToolbarActions[ETBActionAllVideos])
    {
        mMenuActions[EActionSortBy]->setVisible(true);
        mMenuActions[EActionSortByDate]->setVisible(true);
        mMenuActions[EActionSortByName]->setVisible(true);
        mMenuActions[EActionSortBySize]->setVisible(true);

        if (!mIsService)
        {
        	mMenuActions[EActionAddToCollection]->setVisible(true);
        	mMenuActions[EActionDelete]->setVisible(true);
        }
    }
    else if(mToolbarViewsActionGroup->checkedAction() == mToolbarActions[ETBActionCollections] &&
    		firstAction == mToolbarActions[ETBActionAllVideos] )
    {
        if (!mIsService)
        {
        	mMenuActions[EActionNewCollection]->setVisible(true);
        }
    	mMenuActions[EActionSortBy]->setVisible(true);
    	mMenuActions[EActionSortByName]->setVisible(true);
        mMenuActions[EACtionSortByItemCount]->setVisible(true);
        mMenuActions[EActionSortByLength]->setVisible(true);
    }
    else if(firstAction != mToolbarActions[ETBActionAllVideos])
    {
        //Add, Remove and Sort by will be launched from toolbar
        if (!mIsService)
        {
        	mMenuActions[EActionDelete]->setVisible(true);
        }
    }
}

// -------------------------------------------------------------------------------------------------
// handleAsyncStatusSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::handleAsyncStatusSlot(int statusCode, QVariant &additional)
{
    // in case of error concerns delete, need to reset filtering
    if(statusCode == VideoCollectionCommon::statusSingleDeleteFail ||
       statusCode == VideoCollectionCommon::statusMultipleDeleteFail)
    {
        if(mModel)
        {
            mModel->clear();
        }
    }
    // show msg from status
    mUiUtils.showStatusMsgSlot(statusCode, additional);
}

// -------------------------------------------------------------------------------------------------
// collectionOpenedSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::collectionOpenedSlot(bool collectionOpened, const QString& collection)
{
    if(!mToolbarCollectionActionGroup || !mToolbarViewsActionGroup || !mToolbarActions.contains(ETBActionCollections))
    {
        return;
    }

   	mCollectionName = collection;

	HbToolBar* bar = toolBar();
	bar->clearActions();

	if (collectionOpened)
    {
        mToolbarActions[ETBActionCollections]->setChecked(false);
    	bar->addActions(mToolbarCollectionActionGroup->actions());
    }
    else
    {
    	bar->addActions(mToolbarViewsActionGroup->actions());
        mToolbarActions[ETBActionCollections]->setChecked(true);
    }
}

// -------------------------------------------------------------------------------------------------
// openSortByMenuSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::openSortByMenuSlot()
{
	if(!mModel)
	{
		return;
	}

	int menuActionCount = 0;
    HbAction *action = 0;
    foreach(action, mMenuActions.values())
    {
        if(action)
        {
        	++menuActionCount;
            action->setVisible(false);
        }
    }
    if(menuActionCount != mMenuActions.values().count() || mMenuActions.values().count() == 0)
    {
    	// fatal error, some action(s) was not created.
    	return;
    }

    if(!mModel->rowCount(QModelIndex()))
    {
        // no items, no menu
        return;
    }

    mSortMenu->setVisible(true);
    mMenuActions[EActionSortBy]->setVisible(true);
	mMenuActions[EActionSortByDate]->setVisible(true);
	mMenuActions[EActionSortByName]->setVisible(true);
	mMenuActions[EActionSortBySize]->setVisible(true);

	QPointF coords((size()/2).width(), (size()/3).height());
    mSortMenu->exec(coords);
}

// Just for testing, remove this
void VideoListView::debugNotImplementedYet()
{
    HbMessageBox::information(tr("Not implemented yet"));
}

// end of file

