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

// Version : %version: 97 %

// INCLUDE FILES
#include <xqserviceutil.h>
#include <qactiongroup.h>
#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbmessagebox.h>
#include <hbstackedwidget.h>
#include <hbstackedlayout.h>
#include <hblistwidget.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbgroupbox.h>
#include <hbpushbutton.h>
#include <hbinputdialog.h>
#include <vcxmyvideosdefs.h>

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
#include "mpxhbvideocommondefs.h"
#include "videocollectiontrace.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListView::VideoListView( VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent ) 
    : HbView( parent )
    , mUiUtils( VideoCollectionViewUtils::instance() )
    , mWrapper( VideoCollectionWrapper::instance() )
    , mUiLoader( uiLoader )
    , mIsService( false )
    , mModelReady( false )
    , mTransitionOngoing( false )
    , mHintLevel( VideoHintWidget::AllVideos )
    , mVideoServices( 0 )
    , mCurrentList( 0 )
    , mToolbarViewsActionGroup( 0 )
    , mToolbarCollectionActionGroup( 0 )
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListView::~VideoListView()
{
	FUNC_LOG;
    
    toolBar()->clearActions();
    
    mToolbarActions.clear();
    mSortingRoles.clear();

    if(mVideoServices)
    {
    	mVideoServices->decreaseReferenceCount();
    	mVideoServices = 0;
    }
}

// ---------------------------------------------------------------------------
// initializeView()
// ---------------------------------------------------------------------------
//
int VideoListView::initializeView()
{
	FUNC_LOG;
	if(!mUiLoader)
	{
        cleanup();
		return -1;
	}

    if (XQServiceUtil::isService() && !mVideoServices)
    {
        INFO("VideoListView::initializeView() service flag set to true.");
        mIsService = true;

    	mVideoServices = VideoServices::instance();

    	if (!mVideoServices)
        {
    	    ERROR(-1, "VideoListView::initializeView() getting services instance failed.");
            cleanup();
        	return -1;
		}
        else
        {
        	connect(mVideoServices, SIGNAL(titleReady(const QString&)), this, SLOT(titleReadySlot(const QString&)));
        }
	}
    
    // start loading objects and widgets
    QList<VideoCollectionUiLoaderParam> params;
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_VIDEOLISTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_OPTIONS_MENU,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhasePrimary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_ADD_TO_COLLECTION,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_CREATE_COLLECTION,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_DELETE_MULTIPLE,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_HEADINGBANNER,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_MENU,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_DATE,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_NAME,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_SORT_BY_SIZE,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        false,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_COLLECTIONWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_COLLECTIONCONTENTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary)); // widget
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_DIALOG,
        DOCML_VIDEOSELECTIONDIALOG_FILE,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_VIDEOHINTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_HINT,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_HINT_BUTTON,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_HINT,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_NO_VIDEOS_LABEL,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_HINT,
        true,
        VideoCollectionUiLoaderParam::LoadPhaseSecondary));
    mUiLoader->addData(params,
        this,
        SLOT(objectReadySlot(QObject*, const QString&)));
    mUiLoader->loadPhase(VideoCollectionUiLoaderParam::LoadPhasePrimary);
    params.clear();
    
    return 0;
}

// ---------------------------------------------------------------------------
// titleReadySlot()
// ---------------------------------------------------------------------------
//
void VideoListView::titleReadySlot(const QString& title)
{
	FUNC_LOG;
	setTitle(title);
}

// ---------------------------------------------------------------------------
// activateView()
// ---------------------------------------------------------------------------
//
int VideoListView::activateView(const TMPXItemId &itemId)
{
	FUNC_LOG;

    int err = 0;
    
    // activate collection to correct view
    if (itemId == TMPXItemId::InvalidId())
    {
        err = activateVideosView();
    }
    else
    {
        err = activateCollectionContentView(itemId);
    }
    
    // connect signals if everything went ok
    if (err == 0)
    {
        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
        mainWnd->setOrientation(Qt::Vertical, false);
        if (!connect(
                mainWnd, SIGNAL(aboutToChangeOrientation()),
                this, SLOT( aboutToChangeOrientationSlot())) ||
            !connect(
                mainWnd, SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChangedSlot(Qt::Orientation))) ||
            !connect(
                &mWrapper, SIGNAL(asyncStatus(int, QVariant&)),
                this, SLOT(handleAsyncStatusSlot(int, QVariant&))) ||
            !connect(
                mCurrentList->getModel().sourceModel(), SIGNAL(modelChanged()),
                this, SLOT(layoutChangedSlot())) ||
            !connect(
                mCurrentList->getModel().sourceModel(), SIGNAL(modelReady()),
                this, SLOT(modelReadySlot())))
        {
            ERROR(-1, "VideoListView::activateView() failed to connect signals.");
            // deactivate view so we get rid of dangling connections.
            deactivateView();
            err = -1;
        }
    }
    
    return err;
}

// ---------------------------------------------------------------------------
// doDelayedsSlot
// ---------------------------------------------------------------------------
//
void VideoListView::doDelayedsSlot()
{
	FUNC_LOG;
    mUiLoader->loadPhase(VideoCollectionUiLoaderParam::LoadPhaseSecondary);
	emit doDelayeds();
}

// ---------------------------------------------------------------------------
// modelReadySlot
// ---------------------------------------------------------------------------
//
void VideoListView::modelReadySlot()
{
	FUNC_LOG;
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
	FUNC_LOG;
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
	FUNC_LOG;
    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
    
    mainWnd->unsetOrientation();
    
    disconnect(mainWnd, SIGNAL(aboutToChangeOrientation()),
               this, SLOT(aboutToChangeOrientationSlot()));

    disconnect(mainWnd, SIGNAL(orientationChanged(Qt::Orientation)),
               this, SLOT(orientationChangedSlot(Qt::Orientation)));

    disconnect(&mWrapper, SIGNAL(asyncStatus(int, QVariant&)),
               this, SLOT(handleAsyncStatusSlot(int, QVariant&)));
    
    HbMenu *menu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_OPTIONS_MENU);
    if (menu)
    {
        menu->hide();
    }
    
    if(mCurrentList && &(mCurrentList->getModel()) && mCurrentList->getModel().sourceModel())
    {
        disconnect(mCurrentList->getModel().sourceModel(),
                SIGNAL(modelChanged()),
                this, SLOT(layoutChangedSlot()));
        disconnect(mCurrentList->getModel().sourceModel(),
                SIGNAL(modelReady()),
                this, SLOT(modelReadySlot()));

        showHint(false);
    }
    
    if(mCurrentList)
    {
        mCurrentList->deactivate();
    }
}

// ---------------------------------------------------------------------------
// back()
// ---------------------------------------------------------------------------
//
void VideoListView::back()
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// cleanup()
// ---------------------------------------------------------------------------
//
void VideoListView::cleanup()
{
	FUNC_LOG;
    delete mToolbarViewsActionGroup;
    mToolbarViewsActionGroup = 0;

    delete mToolbarCollectionActionGroup;
    mToolbarCollectionActionGroup = 0;
    
    mCurrentList = 0;    
}

// ---------------------------------------------------------------------------
// createToolbar()
// Creates toolbar, toolbar actions and toolbar icons
// ---------------------------------------------------------------------------
//
int VideoListView::createToolbar()
{
	FUNC_LOG;
    // Create actiongroup and add all actions to it. This ensures that only one is
    // active at certain moment.

    if(!mToolbarViewsActionGroup && !mToolbarCollectionActionGroup)
    {
    	mToolbarViewsActionGroup = new QActionGroup(this);
        mToolbarCollectionActionGroup = new QActionGroup(this);

        // create toolbar item actions

        // All Videos tab
        mToolbarActions[ETBActionAllVideos] = createAction(":/images/qtg_mono_video_all.svg",
                mToolbarViewsActionGroup, SLOT(openAllVideosViewSlot()));

        // Collections tab
        mToolbarActions[ETBActionCollections] = createAction("qtg_mono_video_collection",
                mToolbarViewsActionGroup, SLOT(openCollectionViewSlot()));

        if (!mIsService)
        {
			// Services tab
			mToolbarActions[ETBActionServices] = createAction("qtg_mono_ovistore",
					mToolbarViewsActionGroup, SLOT(openServicesViewSlot()));
			// Add Videos tab
			mToolbarActions[ETBActionAddVideos] = 
			        createAction("qtg_mono_add_to_video_collection",
					mToolbarCollectionActionGroup, SLOT(addVideosToCollectionSlot()));

			// Remove Videos tab
			mToolbarActions[ETBActionRemoveVideos] = 
			        createAction("qtg_mono_remove_from_video_collection",
					mToolbarCollectionActionGroup, SLOT(removeVideosFromCollectionSlot()));
        }

        HbToolBar *bar = toolBar(); // First call to toolBar() creates the object, so on failure it could return 0.

        if(   !bar
		   || !mToolbarActions[ETBActionAllVideos]
           || !mToolbarActions[ETBActionCollections]
           || ( !mIsService && (!mToolbarActions[ETBActionServices] 
              || !mToolbarActions[ETBActionAddVideos]
              || !mToolbarActions[ETBActionRemoveVideos])))
        {
            ERROR(-1, "VideoListView::createToolbar() failed to create all actions or the toolbar.");
        	delete mToolbarActions[ETBActionAllVideos];
            delete mToolbarActions[ETBActionCollections];
            delete mToolbarActions[ETBActionServices];
            delete mToolbarActions[ETBActionAddVideos];
            delete mToolbarActions[ETBActionRemoveVideos];
        	return -1;
        }

        // Collection view actions are not checkable
        mToolbarActions[ETBActionAllVideos]->setCheckable(true);
        mToolbarActions[ETBActionCollections]->setCheckable(true);

        if (!mIsService)
        {
        	mToolbarActions[ETBActionServices]->setCheckable(false);
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
HbAction* VideoListView::createAction(QString icon,
        QActionGroup* actionGroup, const char *slot)
{
	FUNC_LOG;
    HbAction* action = new HbAction(actionGroup);

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
// showHint
// ---------------------------------------------------------------------------
//
void VideoListView::showHint(bool show)
{
	FUNC_LOG;
    if(!mCurrentList)
    {
        return;
    }

    VideoSortFilterProxyModel &model = mCurrentList->getModel();
    
    // prepare hint widget
    VideoHintWidget *hintWidget =
        mUiLoader->findWidget<VideoHintWidget>(
            DOCML_NAME_VC_VIDEOHINTWIDGET);
    if (hintWidget)
    {
        hintWidget->setLevel(mHintLevel);
        if (mModelReady &&
            model.rowCount() == 0)
        {
            show ? hintWidget->activate() : hintWidget->deactivate();
        }
        else
        {
            show = false;
            hintWidget->deactivate();
        }
        if(show)
        {
            bool showHintBtns = (mCurrentList->getLevel() != VideoCollectionCommon::ELevelDefaultColl); 
            hintWidget->setButtonShown(showHintBtns);
        }
        else
        {
            hintWidget->setButtonShown(true);
        }
    }

    if (mToolbarViewsActionGroup && mToolbarCollectionActionGroup && !mIsService)
    {
        if (show)
        {
            mToolbarActions[ETBActionRemoveVideos]->setVisible(false);

            if(mCurrentList->getLevel() == VideoCollectionCommon::ELevelDefaultColl) 
            {
                mToolbarActions[ETBActionAddVideos]->setVisible(false);
            } 
        }
        else
        {
            if(mToolbarActions[ETBActionRemoveVideos]->isVisible() == false)
            {
                mToolbarActions[ETBActionRemoveVideos]->setVisible(true);
            }
            if(mToolbarActions[ETBActionAddVideos]->isVisible() == false) 
            {
                mToolbarActions[ETBActionAddVideos]->setVisible(true);
            }
        }
    }

    // prepare sublabel
    HbGroupBox *subLabel =
        mUiLoader->findWidget<HbGroupBox>(
            DOCML_NAME_VC_HEADINGBANNER);
    if (subLabel)
    {
        if (show &&
            mCurrentList->getLevel() == VideoCollectionCommon::ELevelVideos)
        {
            subLabel->hide();
        }
        else
        {
            subLabel->show();
        }
    }
}

// ---------------------------------------------------------------------------
// setHintLevel
// ---------------------------------------------------------------------------
//
void VideoListView::setHintLevel(VideoHintWidget::HintLevel level)
{
	FUNC_LOG;
	mHintLevel = level;
}

// ---------------------------------------------------------------------------
// updateSubLabel
// ---------------------------------------------------------------------------
//
void VideoListView::updateSubLabel()
{
	FUNC_LOG;
    VideoSortFilterProxyModel *model = 0;
    if(mCurrentList)
    {
        model = &mCurrentList->getModel(); 
    }
    
    if (model)
    {
        int itemCount = model->rowCount();
        
        HbGroupBox *subLabel = 
            mUiLoader->findWidget<HbGroupBox>(
                DOCML_NAME_VC_HEADINGBANNER);

        if(subLabel && mCurrentList)
        {
			if (mCurrentList->getLevel() == VideoCollectionCommon::ELevelVideos)
			{
			    // no need to update sublabel if there are no items in videolist
			    // hint widget is shown instead
			    if (itemCount)
			    {
	                subLabel->setHeading(hbTrId("txt_videos_subtitle_ln_videos", itemCount));
			    }
			}
			else if (mCurrentList->getLevel() == VideoCollectionCommon::ELevelCategory)
			{
				subLabel->setHeading(hbTrId("txt_videos_subtitle_ln_collections", itemCount));
			}
			else
			{
				subLabel->setHeading(hbTrId("txt_videos_subtitle_1_l2").arg(mCollectionName).arg(itemCount));
			}
        }
    }
}

// ---------------------------------------------------------------------------
// showAction()
// ---------------------------------------------------------------------------
//
void VideoListView::showAction(bool show, const QString &name)
{
	FUNC_LOG;
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

    // hide or show action
    if (action)
    {
        action->setVisible(show);
    }
}

// ---------------------------------------------------------------------------
// activateVideosView()
// ---------------------------------------------------------------------------
//
int VideoListView::activateVideosView()
{
    FUNC_LOG;
    VideoListWidget *videoList =
        mUiLoader->findWidget<VideoListWidget>(
            DOCML_NAME_VC_VIDEOLISTWIDGET);
    if (videoList)
    {
        VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelVideos;
        if (mCurrentList)
        {
            level = mCurrentList->getLevel();
        }
        else
        {
            mCurrentList = videoList;
        }
        
        int result = mCurrentList->activate(level);
        if(result < 0)
        {
            ERROR(result, "VideoListView::activateVideosView() failed to activate.");
            // activate failed, deactivate view so we get rid of dangling connections.
            deactivateView();
            return -1;
        }
        
        if (createToolbar() != 0)
        {
            ERROR(result, "VideoListView::activateVideosView() failed to create toolbar.");
            deactivateView();
            return -1;
        }
    }
    else
    {
        return -1;
    }
    
    return 0;
}

// ---------------------------------------------------------------------------
// activateCollectionContentView()
// ---------------------------------------------------------------------------
//
int VideoListView::activateCollectionContentView(const TMPXItemId &itemId)
{
    FUNC_LOG;
    int err = 0;
    
    if (itemId.iId2 == KVcxMvcMediaTypeCategory ||
        itemId.iId2 == KVcxMvcMediaTypeAlbum)
    {
        // currently only captured and downloaded categories are supported
        switch (itemId.iId1)
        {
            case KVcxMvcCategoryIdDownloads:
            case KVcxMvcCategoryIdCaptured:
            {
                VideoListWidget *collectionContentWidget =
                    mUiLoader->findWidget<VideoListWidget>(
                        DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
                if (collectionContentWidget)
                {
                    // no need to deactivate since there cannot be previous widget
                    mCurrentList = collectionContentWidget;
                    
                    // hide toolbar
                    HbToolBar *toolBar = this->toolBar();
                    if (toolBar)
                    {
                        toolBar->hide();
                    }
                    
                    // resolve collection name
                    if (itemId.iId1 == KVcxMvcCategoryIdDownloads)
                    {
                        mCollectionName = hbTrId("txt_videos_dblist_downloaded");
                    }
                    else if (itemId.iId1 == KVcxMvcCategoryIdCaptured)
                    {
                        mCollectionName = hbTrId("txt_videos_dblist_captured");
                    }
                    
                    // activate collection content widget
                    mCurrentList->activate(VideoCollectionCommon::ELevelDefaultColl);
                    
                    // open the model
                    VideoSortFilterProxyModel &model = mCurrentList->getModel();
                    model.openItem(itemId);
                    
                    // sort model
                    model.doSorting(VideoCollectionCommon::KeyDateTime, Qt::AscendingOrder);
                    
                    // set hint level to collections
                    setHintLevel(VideoHintWidget::Collection);
                }
                else
                {
                    ERROR(-1, "VideoListView::activateVideosView() failed to get collection content widget.");
                    err = -1;
                }
                break;
            }
            default:
            {
                // by default open videos view
                err = activateVideosView();
                break;
            }
        }
        
    }
    else
    {
        // open videos view
        err = activateVideosView();
    }

    return err;
}

// ---------------------------------------------------------------------------
// openAllVideosViewSlot()
// ---------------------------------------------------------------------------
//
void VideoListView::openAllVideosViewSlot()
{
	FUNC_LOG;
    VideoListWidget *videoListWidget =
        mUiLoader->findWidget<VideoListWidget>(
            DOCML_NAME_VC_VIDEOLISTWIDGET);
    
    if (mCurrentList &&
        videoListWidget &&
        mCurrentList != videoListWidget)
    {
        // deactivate old list
        mCurrentList->deactivate();
        
        // activate all videos list
        mCurrentList = videoListWidget;
        mCurrentList->activate(VideoCollectionCommon::ELevelVideos);

        // since collection is not to be opened at this point,
        // we do not receive lauoutChanged for updating the hind -widget
        // if needed, need to show it here is needed
        setHintLevel(VideoHintWidget::AllVideos);
        showHint();
    }
}

// ---------------------------------------------------------------------------
// openCollectionViewSlot()
// ---------------------------------------------------------------------------
//
void VideoListView::openCollectionViewSlot()
{
	FUNC_LOG;
    VideoListWidget *collectionWidget =
        mUiLoader->findWidget<VideoListWidget>(
            DOCML_NAME_VC_COLLECTIONWIDGET);
    
    if (mCurrentList &&
        mCurrentList != collectionWidget)
    {
        // deactivate all videos widget
        mCurrentList->deactivate();
        
        // activate video collection widget
        mCurrentList = collectionWidget;
        mCurrentList->activate(VideoCollectionCommon::ELevelCategory);
        
        VideoSortFilterProxyModel &model = mCurrentList->getModel(); 

        VideoCollectionViewUtils::sortModel(&model, false, mCurrentList->getLevel());

        // the collection view is not empty, so we can hide the hint in advance.
        showHint(false);
    }
}

// ---------------------------------------------------------------------------
// openNewAlbumSlot()
// ---------------------------------------------------------------------------
//
void VideoListView::openNewAlbumSlot(const QModelIndex &parent,
    int start,
    int end)
{
	FUNC_LOG;
    Q_UNUSED(end);
    if(!mCurrentList)
    {
        return;
    }
    // invalidate model
    VideoSortFilterProxyModel &model = mCurrentList->getModel();
    model.invalidate();
        
    // activate new index
    QModelIndex index = model.index(start, 0, parent);
    if (index.isValid())
    {
        // disconnect rowsInserted signal to prevent obsolete slot calls
        disconnect( &model, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                    this, SLOT(openNewAlbumSlot(const QModelIndex&, int, int)));

        mCurrentList->emitActivated(index);        
    }
}

// ---------------------------------------------------------------------------
// openservicesViewSlot()
// ---------------------------------------------------------------------------
//
void VideoListView::openServicesViewSlot()
{
	FUNC_LOG;
    debugNotImplementedYet();
}

// ---------------------------------------------------------------------------
// startSorting()
// ---------------------------------------------------------------------------
//
void VideoListView::startSorting()
{
	FUNC_LOG;
    HbMenu *optionsMenu =
        mUiLoader->findWidget<HbMenu>(
            DOCML_NAME_OPTIONS_MENU);
    if (optionsMenu && mCurrentList)
    {
        // get sorting role from active action
        HbAction* action = optionsMenu->activeAction();
        HbMenu* sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
        if(action == sortMenu->menuAction()) // make sure that active action is the sort menu. 
        {
            HbAction* action = sortMenu->activeAction();
            if(action)
            {
                doSorting(mSortingRoles[action]);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// doSorting()
// ---------------------------------------------------------------------------
//
void VideoListView::doSorting(int role)
{
	FUNC_LOG;
	// sort model
	Qt::SortOrder order(Qt::AscendingOrder);
	VideoSortFilterProxyModel &model = mCurrentList->getModel();
	if(model.sortRole() == role && model.sortOrder() == Qt::AscendingOrder)
	{
		order = Qt::DescendingOrder;
	}
	model.doSorting(role, order);

	if (mCurrentList == mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET))
	{
		mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET)->getModel().doSorting(role, order);	
	}

	// save sorting values
	mUiUtils.saveSortingValues(role, order, mCurrentList->getLevel());
}

// -------------------------------------------------------------------------------------------------
// aboutToChangeOrientationSlot()
// hide all items in the window during orientation change
// -------------------------------------------------------------------------------------------------
//
void VideoListView::aboutToChangeOrientationSlot()
{
	FUNC_LOG;
    // this method is required for changing from to mediawall
}

// -------------------------------------------------------------------------------------------------
// orientationChangedSlot()
// orientation changed, items can be put back to visible after view has handled the change
// -------------------------------------------------------------------------------------------------
//
void VideoListView::orientationChangedSlot( Qt::Orientation orientation )
{
	FUNC_LOG;
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
	FUNC_LOG;
    if(!mCurrentList)
    {
        return;
    }

    VideoListSelectionDialog *dialog =
        mUiLoader->findWidget<VideoListSelectionDialog>(
            DOCML_NAME_DIALOG);
    if (dialog)
    {
        TMPXItemId collectionId = mCurrentList->getModel().getOpenItem();
        dialog->setupContent(VideoListSelectionDialog::EDeleteVideos, collectionId); 
        dialog->exec();
    }
}

// -------------------------------------------------------------------------------------------------
// createCollectionSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::createCollectionSlot()
{
	FUNC_LOG;
    if(!mCurrentList)
    {
        return;
    }
    
    VideoSortFilterProxyModel &model = mCurrentList->getModel();
    
    bool ok = false;
    // query a name for the collection
    QString label(hbTrId("txt_videos_title_enter_name"));
    QString text(hbTrId("txt_videos_dialog_entry_new_collection"));
    text = HbInputDialog::getText(label, text, &ok);
    if (ok && text.length())
    {
        // resolve collection true name and add new album
        text = model.resolveAlbumName(text);
        
        // when collection reports about new collection, we open it right away,
        // for that, connect to rowsInserted so that the new album can be opened
        if(!connect(&model, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                    this, SLOT(openNewAlbumSlot(const QModelIndex&, int, int))))
        {
            return;
        }
        
        model.addNewAlbum(text);
    }
}

// -------------------------------------------------------------------------------------------------
// addVideosToCollectionSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::addVideosToCollectionSlot()
{
	FUNC_LOG;
    if(!mCurrentList)
    {
        return;
    }

    VideoListSelectionDialog *dialog =
        mUiLoader->findWidget<VideoListSelectionDialog>(
            DOCML_NAME_DIALOG);
    if (!dialog)
    {
        // fatal: no selection dialog
        return;
    }
    if(mCurrentList->getLevel() == VideoCollectionCommon::ELevelAlbum)
    {
        // album is opened, do not proceed in case it already have same amount
        // of videos than all videos view.
        VideoListWidget *allVideos = mUiLoader->findWidget<VideoListWidget>(
                    DOCML_NAME_VC_VIDEOLISTWIDGET);
        if(allVideos)
        {
            int count = allVideos->getModel().rowCount();
            if(count == mCurrentList->getModel().rowCount())
            {
                if(count)
                {
                    QVariant emptyAdditional;
                    mUiUtils.showStatusMsgSlot(
                            VideoCollectionCommon::statusAllVideosAlreadyInCollection,
                            emptyAdditional);
                }
                return;
            }  
        }
    }
    TMPXItemId collectionId = mCurrentList->getModel().getOpenItem();
    dialog->setupContent(VideoListSelectionDialog::EAddToCollection, collectionId);
    dialog->exec();
}

// -------------------------------------------------------------------------------------------------
// removeVideosFromCollectionSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::removeVideosFromCollectionSlot()
{
	FUNC_LOG;
    if(!mCurrentList)
    {
        return;
    }
    // not allowed if for some reason current widget 
    // is all videos or collection or there are no items
    if(mCurrentList->getLevel() < VideoCollectionCommon::ELevelDefaultColl ||
       !mCurrentList->getModel().rowCount())
    {
        return;
    }

    VideoListSelectionDialog *dialog =
            mUiLoader->findWidget<VideoListSelectionDialog>(
                        DOCML_NAME_DIALOG);
    if (!dialog)
    {
        ERROR(-1, "VideoListView::removeVideosFromCollectionSlot() failed to load selection dialog.");
        return;
    }
    TMPXItemId collectionId = mCurrentList->getModel().getOpenItem();
    if(collectionId != TMPXItemId::InvalidId() && collectionId.iId2 != KVcxMvcMediaTypeVideo)
    {
        dialog->setupContent(VideoListSelectionDialog::ERemoveFromCollection, collectionId);
        dialog->exec();
    }
}

// -------------------------------------------------------------------------------------------------
// aboutToShowMainMenuSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::aboutToShowMainMenuSlot()
{
    if (mIsService &&
        mVideoServices &&
        mVideoServices->currentService() == VideoServices::EBrowse)
    {
        prepareBrowseServiceMenu();
        return;
    }
    
	if (!mCurrentList ||
	    !mToolbarViewsActionGroup ||
	    !mToolbarCollectionActionGroup)
	{
		return;
	}
	
	// hide all actions by default
    showAction(false, DOCML_NAME_ADD_TO_COLLECTION);
    showAction(false, DOCML_NAME_CREATE_COLLECTION);
    showAction(false, DOCML_NAME_DELETE_MULTIPLE);
    showAction(false, DOCML_NAME_SORT_BY_DATE);
    showAction(false, DOCML_NAME_SORT_BY_NAME);
    showAction(false, DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    showAction(false, DOCML_NAME_SORT_BY_SIZE);
    showAction(false, DOCML_NAME_SORT_MENU);
    
    VideoSortFilterProxyModel &model = mCurrentList->getModel();
    if (!model.rowCount())
    {
        return;
    }
    
    // get current sorting values
    int role;
    Qt::SortOrder order;
    model.getSorting(role, order);

    HbAction *firstAction = (HbAction*)(toolBar()->actions().first());

    if(mToolbarViewsActionGroup->checkedAction() == mToolbarActions[ETBActionAllVideos] &&
       firstAction == mToolbarActions[ETBActionAllVideos])
    {
        showAction(true, DOCML_NAME_SORT_MENU);

        showAction(true, DOCML_NAME_SORT_BY_DATE);
        showAction(true, DOCML_NAME_SORT_BY_NAME);
        showAction(true, DOCML_NAME_SORT_BY_SIZE);

        HbAction* action = mSortingRoles.key(role);
		if (action)
		{
			action->setChecked(true);
		}

        if (!mIsService)
        {
            showAction(true, DOCML_NAME_ADD_TO_COLLECTION);
            showAction(true, DOCML_NAME_DELETE_MULTIPLE);
        }
    }
    else if(mToolbarViewsActionGroup->checkedAction() == mToolbarActions[ETBActionCollections] &&
    		firstAction == mToolbarActions[ETBActionAllVideos] )
    {
        if (!mIsService)
        {
            showAction(true, DOCML_NAME_CREATE_COLLECTION);
        }
        showAction(true, DOCML_NAME_SORT_MENU);
        showAction(true, DOCML_NAME_SORT_BY_NAME);
        showAction(true, DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    	
		HbAction* action = mSortingRoles.key(role);
		if (action)
		{
			action->setChecked(true);
		}
    }
    else if(firstAction != mToolbarActions[ETBActionAllVideos])
    {
        showAction(true, DOCML_NAME_SORT_MENU);
        showAction(true, DOCML_NAME_SORT_BY_DATE);
        showAction(true, DOCML_NAME_SORT_BY_NAME);
        showAction(true, DOCML_NAME_SORT_BY_SIZE);

        HbAction* action = mSortingRoles.key(role);
		if (action)
		{
			action->setChecked(true);
		}

        if (!mIsService)
        {
            showAction(true, DOCML_NAME_DELETE_MULTIPLE);
        }
    }
}

// -------------------------------------------------------------------------------------------------
// prepareBrowseServiceMenu
// -------------------------------------------------------------------------------------------------
//
void VideoListView::prepareBrowseServiceMenu()
{
    if (!mCurrentList)
    {
        return;
    }
    
    // hide all actions by default
    showAction(false, DOCML_NAME_ADD_TO_COLLECTION);
    showAction(false, DOCML_NAME_CREATE_COLLECTION);
    showAction(false, DOCML_NAME_DELETE_MULTIPLE);
    showAction(false, DOCML_NAME_SORT_BY_DATE);
    showAction(false, DOCML_NAME_SORT_BY_NAME);
    showAction(false, DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    showAction(false, DOCML_NAME_SORT_BY_SIZE);
    showAction(false, DOCML_NAME_SORT_MENU);
    
    VideoSortFilterProxyModel &model = mCurrentList->getModel();
    if (!model.rowCount())
    {
        return;
    }
    
    // show delete action
    showAction(true, DOCML_NAME_DELETE_MULTIPLE);
    
    // show sort actions
    showAction(true, DOCML_NAME_SORT_MENU);
    showAction(true, DOCML_NAME_SORT_BY_DATE);
    showAction(true, DOCML_NAME_SORT_BY_NAME);
    showAction(true, DOCML_NAME_SORT_BY_SIZE);
    
    // set current sort action selected
    int role;
    Qt::SortOrder order;
    model.getSorting(role, order);
    HbAction* action = mSortingRoles.key(role);
    if (action)
    {
        action->setChecked(true);
    }
}

// -------------------------------------------------------------------------------------------------
// handleAsyncStatusSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::handleAsyncStatusSlot(int statusCode, QVariant &additional)
{
	FUNC_LOG;
    // show msg from status
    mUiUtils.showStatusMsgSlot(statusCode, additional);
}

// -------------------------------------------------------------------------------------------------
// collectionOpenedSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::collectionOpenedSlot(bool collectionOpened,
    const QString& collection,
    const QModelIndex &index)
{
	FUNC_LOG;
	if(mTransitionOngoing)
	{
		return;
	}
	else
	{
	    // clear toolbar actions.
	    toolBar()->clearActions();
	}

	if(collectionOpened)
    {
        if(!index.isValid())
        {   
            return;
        }
        
        // open album view
        VideoListWidget *collectionContentWidget =
            mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
        
        if (!mCurrentList || mCurrentList == collectionContentWidget)
        {
            // no currentlist or currentlist is already list 
            // we're about to transit
            return;
        }

        // get item id before deactivating
        TMPXItemId itemId = TMPXItemId::InvalidId();
        itemId = mCurrentList->getModel().getMediaIdAtIndex(index);
        
        // only category or album can be activated here
        if(itemId == TMPXItemId::InvalidId() ||
           (itemId.iId2 != KVcxMvcMediaTypeCategory && 
            itemId.iId2 != KVcxMvcMediaTypeAlbum))
        {
            return;
        }
        
        // Send level that is opened with the effect.
        QVariant nextLevel;
        if(itemId.iId2 ==  KVcxMvcMediaTypeCategory)
        {
            nextLevel = QVariant(VideoCollectionCommon::ELevelDefaultColl);
        }
        else if(itemId.iId2 == KVcxMvcMediaTypeAlbum)
        {
            nextLevel = QVariant(VideoCollectionCommon::ELevelAlbum);
        }            
        
        // Start fetching content.
        collectionContentWidget->getModel().openItem(itemId);
        
        HbEffect::start(collectionContentWidget,
                        EFFECT_SLIDE_IN_TO_LEFT,
                        this,
                        "finishCollectionOpenedSlot",
                        nextLevel);
        
        mTransitionOngoing = true;
    }
    else
    {
        // open categories view.
        VideoListWidget *collectionContentWidget =
            mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
        
        HbEffect::start(collectionContentWidget,
                        EFFECT_SLIDE_OUT_TO_LEFT,
                        this,
                        "finishCollectionClosedSlot");
        
        mTransitionOngoing = true;
    }
	// update collection specific information
	mCollectionName = collection;
}

// -------------------------------------------------------------------------------------------------
// finishCollectionOpenedSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::finishCollectionOpenedSlot(const HbEffect::EffectStatus &status)
{
	FUNC_LOG;
    Q_UNUSED(status);
    
    mTransitionOngoing = false;
    
    // deactivat current widget.
    mCurrentList->deactivate();
    
    // activate video collection content widget.
    mCurrentList = mUiLoader->findWidget<VideoListWidget>(
                    DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);

    if(!mCurrentList || !status.userData.isValid())
    {
        return;
    }
    
    updateSubLabel();

    VideoCollectionCommon::TCollectionLevels level =
            static_cast<VideoCollectionCommon::TCollectionLevels>(status.userData.toInt());
	mCurrentList->activate(level);

    VideoCollectionViewUtils::sortModel(&mCurrentList->getModel(), false, VideoCollectionCommon::ELevelVideos);
    mCurrentList->getModel().invalidate();

    // update hint widget
    setHintLevel(VideoHintWidget::Collection);
    showHint();

    // update toolbar for albums, default categories don't have one.
    if(level == VideoCollectionCommon::ELevelAlbum && 
       mToolbarCollectionActionGroup && mToolbarActions.contains(ETBActionCollections))
    {
        mToolbarActions[ETBActionCollections]->setChecked(false);
        toolBar()->addActions(mToolbarCollectionActionGroup->actions());
    }
}

// -------------------------------------------------------------------------------------------------
// finishCollectionClosedSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::finishCollectionClosedSlot(const HbEffect::EffectStatus &status)
{
	FUNC_LOG;
    Q_UNUSED(status);
    
    mTransitionOngoing = false;
    
    // open collection view
    openCollectionViewSlot();
    
    // update toolbar
    if(mToolbarViewsActionGroup && mToolbarActions.contains(ETBActionCollections))
    {
        toolBar()->addActions(mToolbarViewsActionGroup->actions());
        mToolbarActions[ETBActionCollections]->setChecked(true);
    }
}

// -------------------------------------------------------------------------------------------------
// objectReadySlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::objectReadySlot(QObject *object, const QString &name)
{
	FUNC_LOG;
    if (name.compare(DOCML_NAME_VC_VIDEOLISTWIDGET) == 0)
    {
        connect(object, SIGNAL(command(int)), this, SIGNAL(command(int)));
        connect(this, SIGNAL(doDelayeds()), object, SLOT(doDelayedsSlot()));
    }
    else if (name.compare(DOCML_NAME_VC_COLLECTIONWIDGET) == 0)
    {
        connect(
            object, SIGNAL(collectionOpened(bool, const QString&, const QModelIndex&)),
            this, SLOT(collectionOpenedSlot(bool, const QString&, const QModelIndex&)));
            connect(this, SIGNAL(doDelayeds()), object, SLOT(doDelayedsSlot()));
            emit(doDelayeds());
    }
    else if (name.compare(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET) == 0)
    {
        connect(object, SIGNAL(command(int)), this, SIGNAL(command(int)));
        connect(
            object, SIGNAL(collectionOpened(bool, const QString&, const QModelIndex&)),
            this, SLOT(collectionOpenedSlot(bool, const QString&, const QModelIndex&)));
            connect(this, SIGNAL(doDelayeds()), object, SLOT(doDelayedsSlot()));
            emit(doDelayeds());
    }
    else if (name.compare(DOCML_NAME_OPTIONS_MENU) == 0)
    {
        connect(
            object, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMainMenuSlot()));
    }
    else if (name.compare(DOCML_NAME_HINT_BUTTON) == 0)
    {
        connect(object, SIGNAL(clicked(bool)), this, SLOT(openServicesViewSlot()));
    }
    else if (name.compare(DOCML_NAME_SORT_BY_DATE) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
        	connect(action, SIGNAL(triggered()), this, SLOT(startSorting()));
            mSortingRoles[action] = VideoCollectionCommon::KeyDateTime;
        }
    }
    else if (name.compare(DOCML_NAME_SORT_BY_NAME) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
        	connect(action, SIGNAL(triggered()), this, SLOT(startSorting()));
            mSortingRoles[action] = VideoCollectionCommon::KeyTitle;
        }
    }
    else if (name.compare(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
            connect(action, SIGNAL(triggered()), this, SLOT(startSorting()));
            mSortingRoles[action] = VideoCollectionCommon::KeyNumberOfItems;
        }
    }
    else if (name.compare(DOCML_NAME_SORT_BY_SIZE) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
        	connect(action, SIGNAL(triggered()), this, SLOT(startSorting()));
            mSortingRoles[action] = VideoCollectionCommon::KeySizeValue;
        }
    }
    else if (name.compare(DOCML_NAME_ADD_TO_COLLECTION) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
            connect(action, SIGNAL(triggered()), this, SLOT(addVideosToCollectionSlot()));
        }
    }
    else if (name.compare(DOCML_NAME_CREATE_COLLECTION) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
            connect(action, SIGNAL(triggered()), this, SLOT(createCollectionSlot()));
        }
    }
    else if (name.compare(DOCML_NAME_DELETE_MULTIPLE) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
            connect(action, SIGNAL(triggered()), this, SLOT(deleteItemsSlot()));
        }
    }
}

// Just for testing, remove this
void VideoListView::debugNotImplementedYet()
{
	FUNC_LOG;
    HbMessageBox::information(tr("Not implemented yet"));
}

// end of file
