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

// Version : %version: 113 %

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

// Object names.
const char* const LIST_VIEW_OBJECT_NAME_OPTIONS_MENU      = "vc::ListViewOptionsMenu";

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListView::VideoListView( VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent ) 
    : HbView( parent )
    , mUiUtils( VideoCollectionViewUtils::instance() )
    , mWrapper( VideoCollectionWrapper::instance() )
    , mUiLoader( uiLoader )
    , mModelReady( false )
    , mViewReady( false )
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
	int videoListPhase = VideoCollectionUiLoaderParam::LoadPhasePrimary;
	int collectionListPhase = VideoCollectionUiLoaderParam::LoadPhaseSecondary;
	int collectionContentListPhase = VideoCollectionUiLoaderParam::LoadPhaseSecondary;
	VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelVideos;
    if (XQServiceUtil::isService())
    {
        INFO("VideoListView::initializeView() initializing service.");
    	if (!mVideoServices)
        {
    	    mVideoServices = VideoServices::instance();
    	    connect(mVideoServices, SIGNAL(titleReady(const QString&)), 
    	            this, SLOT(titleReadySlot(const QString&)));
		}
    	 
	}
    else
    {
        if(mVideoServices)
        {
            disconnect(mVideoServices, SIGNAL(titleReady(const QString&)), 
                    this, SLOT(titleReadySlot(const QString&)));
            mVideoServices->decreaseReferenceCount();
            mVideoServices = 0;
        }
        VideoCollectionViewUtils::getActivityWidgetLevel(level);
        if(level == VideoCollectionCommon::ELevelCategory)
        {
            videoListPhase = VideoCollectionUiLoaderParam::LoadPhaseSecondary;
            collectionListPhase = VideoCollectionUiLoaderParam::LoadPhasePrimary;
            collectionContentListPhase = VideoCollectionUiLoaderParam::LoadPhaseSecondary;
        }
        else if(level == VideoCollectionCommon::ELevelDefaultColl ||
                level == VideoCollectionCommon::ELevelAlbum)
        {
            videoListPhase = VideoCollectionUiLoaderParam::LoadPhaseSecondary;
            collectionListPhase = VideoCollectionUiLoaderParam::LoadPhasePrimary;
            collectionContentListPhase = VideoCollectionUiLoaderParam::LoadPhasePrimary;
        }   
    }
    // start loading objects and widgets
    QList<VideoCollectionUiLoaderParam> params;
    
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_VIDEOLISTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        true,
        videoListPhase));
    
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_COLLECTIONWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        collectionListPhase));
    
    params.append(VideoCollectionUiLoaderParam(
        DOCML_NAME_VC_COLLECTIONCONTENTWIDGET,
        DOCML_VIDEOCOLLECTIONVIEW_FILE,
        DOCML_VIDEOCOLLECTIONVIEW_SECTION_LIST,
        true,
        collectionContentListPhase)); 
    
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
    
    // fetch current list right away for main views
    // for default and user defined collections, currentList 
    // will be setted during activation
    if(videoListPhase == VideoCollectionUiLoaderParam::LoadPhasePrimary)
    {
        mCurrentList = mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_VIDEOLISTWIDGET );
    }
    else 
    {
        mCurrentList = mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_COLLECTIONWIDGET );
    }


    
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
int VideoListView::activateView( TMPXItemId &itemId)
{
	FUNC_LOG;

    int err = -1;
           
    // activate collection to correct view
    if (itemId == TMPXItemId::InvalidId())
    {  
        
        VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelVideos;
        if(!mVideoServices)
        {
            VideoCollectionViewUtils::getActivityWidgetLevel(level);
        }
        if(level == VideoCollectionCommon::ELevelCategory ||
           level == VideoCollectionCommon::ELevelVideos)
        {
            err = activateMainView();
        }
        else if(level == VideoCollectionCommon::ELevelDefaultColl ||
                level == VideoCollectionCommon::ELevelAlbum)
        {
            // level is default or user defined collection
            // see if we have item id 
            VideoCollectionViewUtils::getCollectionActivityData(itemId, mCollectionName);     
        }
    }
    if(itemId != TMPXItemId::InvalidId())
    {
        err = activateCollectionContentView(itemId);
    }
    
    if(!err)
    {
       
       err = createToolbar();
    }
    
    // connect signals if everything went ok
    if (!err)
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
                mCurrentList->getModel()->sourceModel(), SIGNAL(modelChanged()),
                this, SLOT(layoutChangedSlot())) ||
            !connect(
                mCurrentList->getModel()->sourceModel(), SIGNAL(modelReady()),
                this, SLOT(modelReadySlot())))
        {
            ERROR(-1, "VideoListView::activateView() failed to connect signals.");
            // deactivate view so we get rid of dangling connections.
            deactivateView();
            err = -1;
        }
    }
    else
    {
        deactivateView();
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
	if(mCurrentList)
	{
	    mCurrentList->doDelayedsSlot();
	}
}

// ---------------------------------------------------------------------------
// modelReadySlot
// ---------------------------------------------------------------------------
//
void VideoListView::modelReadySlot()
{
	FUNC_LOG;
	
	// if mModelReady is false, then it means that this is the first time modelReady
	// signal fires. Signaling that view is ready.
	if(!mViewReady)
	{
	    mViewReady = true;
	    emit viewReady();
	}
	
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
	// Note that showHint should be executed before updateSubLabel as it
	// can modify the mModelReady flag.
    showHint();
    updateSubLabel();
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
    if(menu)
    {
        menu->setObjectName(LIST_VIEW_OBJECT_NAME_OPTIONS_MENU);
        menu->hide();
    }

    if(mCurrentList && mCurrentList->getModel() && mCurrentList->getModel()->sourceModel())
    {
        disconnect(mCurrentList->getModel()->sourceModel(),
                SIGNAL(modelChanged()),
                this, SLOT(layoutChangedSlot()));
        disconnect(mCurrentList->getModel()->sourceModel(),
                SIGNAL(modelReady()),
                this, SLOT(modelReadySlot()));

        showHint(false);
        if(!mVideoServices)
        {
            VideoCollectionCommon::TCollectionLevels level = mCurrentList->getLevel();
            VideoCollectionViewUtils::setWidgetActivityLevel(level);
        }
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
        mToolbarActions[ETBActionAllVideos] = createAction("qtg_mono_video",
                mToolbarViewsActionGroup, SLOT(openAllVideosViewSlot()));

        // Collections tab
        mToolbarActions[ETBActionCollections] = createAction("qtg_mono_video_collection",
                mToolbarViewsActionGroup, SLOT(openCollectionViewSlot()));

        if (!mVideoServices)
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
           || ( !mVideoServices && (!mToolbarActions[ETBActionServices] 
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

        if (!mVideoServices)
        {
        	mToolbarActions[ETBActionServices]->setCheckable(false);
        }
        VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelVideos;
        if(mCurrentList)
        {
            level = mCurrentList->getLevel();
        }
        if(level == VideoCollectionCommon::ELevelCategory)
        {
            mToolbarActions[ETBActionCollections]->setChecked(true);
            bar->addActions(mToolbarViewsActionGroup->actions());
        }
        else if(level == VideoCollectionCommon::ELevelVideos )
        {
            mToolbarActions[ETBActionAllVideos]->setChecked(true);
            bar->addActions(mToolbarViewsActionGroup->actions());
        }
        else if(level == VideoCollectionCommon::ELevelAlbum) 
        {
            bar->addActions(mToolbarCollectionActionGroup->actions());
        }
        
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

    VideoSortFilterProxyModel *model = mCurrentList->getModel();
    
    if(!model || (!mModelReady && model->rowCount() == 0))
    {
        return;
    }
    
    mModelReady = true;
    
    // decide if the hintwidget needs to be shown or not.
    show = show && model->rowCount() == 0;
    
    // If show is false, then hint widget is fetched only if it exists. If
    // show is true then hint widget is also created and prepared if it does not exists.
    VideoHintWidget *hintWidget =
        mUiLoader->findWidget<VideoHintWidget>(
            DOCML_NAME_VC_VIDEOHINTWIDGET, show);
    
    if (hintWidget)
    {
        hintWidget->setLevel(mHintLevel);
        if (show)
        {
            hintWidget->activate();
            bool showHintBtns = (mCurrentList->getLevel() != VideoCollectionCommon::ELevelDefaultColl); 
            hintWidget->setButtonShown(showHintBtns);
        }
        else
        {
            hintWidget->deactivate();
            hintWidget->setButtonShown(true);
        }
    }

    if (mToolbarViewsActionGroup && mToolbarCollectionActionGroup && !mVideoServices)
    {
        if (show)
        {
        	mToolbarActions[ETBActionRemoveVideos]->setVisible(false);
        }
        else if(mToolbarActions[ETBActionRemoveVideos]->isVisible() == false)
		{
        	mToolbarActions[ETBActionRemoveVideos]->setVisible(true);
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
        model = mCurrentList->getModel(); 
    }
    
    if (model && mModelReady)
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
// activateMainView()
// ---------------------------------------------------------------------------
//
int VideoListView::activateMainView()
{
    FUNC_LOG;

    if(!mCurrentList)
    {
        return -1;
    }

    int result = mCurrentList->activate();
    if(result < 0)
    {
        ERROR(result, "VideoListView::activateVideosView() failed to activate.");
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

    // resolve collection name if possible
    if(itemId.iId2  == KVcxMvcMediaTypeCategory && mCollectionName.isEmpty())
    {
        if (itemId.iId1 == KVcxMvcCategoryIdDownloads)
        {
            mCollectionName = hbTrId("txt_videos_dblist_downloaded");
        }
        else if(itemId.iId1 == KVcxMvcCategoryIdCaptured) 
        {
            mCollectionName = hbTrId("txt_videos_dblist_captured");
        }
        else
        {
            // only downloads and captured are supported in default collections
            ERROR(-1, "VideoListView::activateVideosView() invalid defauld collection.");
            return -1;
        }
    }
    // at this point activation will fail if there's no collection name available
    if(mCollectionName.isEmpty())
    {
        ERROR(-1, "VideoListView::activateVideosView() no collection name, cannot proceed.");
        return -1;
    }
    
    // if current list at this point is already collection content, 
    // no need to init again, just activate
    if(mCurrentList && 
       (mCurrentList->getLevel() == VideoCollectionCommon::ELevelDefaultColl ||
        mCurrentList->getLevel() == VideoCollectionCommon::ELevelAlbum))
    {
        
        return mCurrentList->activate();
    }
    // no currentlist, or currentlist differs, create and initialize all over
    collectionOpenedSlot(true, mCollectionName, itemId);
    
    if((!mCurrentList || !mCurrentList->getModel()) ||
       (mCurrentList->getLevel() != VideoCollectionCommon::ELevelDefaultColl && 
        mCurrentList->getLevel() != VideoCollectionCommon::ELevelAlbum ))
    {
        ERROR(-1, "VideoListView::activateVideosView() failed to init and activate collection.");
        return -1;
    }

    // if we're servicing, need to fetch sorting role from client
    // in normal use, sorting has set already at uiloader
   
    if (mVideoServices)
    {       
        // TODO: service sorting roles needs to be defined somewhere
        int sortRole = mVideoServices->sortRole();
        if(sortRole == 2)
        {
            sortRole = VideoCollectionCommon::KeyTitle;
        }
        else if(sortRole == 3)
        {
            sortRole = VideoCollectionCommon::KeySizeValue;
        } 
        else
        {
            // default 
            sortRole = VideoCollectionCommon::KeyDateTime;
        }
        mCurrentList->getModel()->doSorting(sortRole, Qt::AscendingOrder);
    }
    
    return 0;
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

        setHintLevel(VideoHintWidget::AllVideos);
        
        // update the sublabel, as in most cases the data is already up to date.
        updateSubLabel();
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

        // the collection view is not empty, so we should hide the hint in advance.
        showHint(false);
        
        // also update the sublabel immediatelly, as the data is up to date almost always.
        updateSubLabel();
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
	if(!mCurrentList || !mCurrentList->getModel())
	{
	    // no list or model, cannot sort
	    return;
	}
	// sort model
	Qt::SortOrder order(Qt::AscendingOrder);
	VideoSortFilterProxyModel *model = mCurrentList->getModel();
	if(model->sortRole() == role && model->sortOrder() == Qt::AscendingOrder)
	{
		order = Qt::DescendingOrder;
	}
	model->doSorting(role, order);
	
	// for video related sorting, all videos list and collection content
	// list, sorting orders are same all the time
	VideoListWidget *anotherVideosList = 0;
	VideoCollectionCommon::TCollectionLevels level = mCurrentList->getLevel();
	if (level == VideoCollectionCommon::ELevelDefaultColl ||
	    level == VideoCollectionCommon::ELevelAlbum)
	{
	    anotherVideosList = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_VIDEOLISTWIDGET);
	}
	else if(level == VideoCollectionCommon::ELevelVideos)
	{
	    anotherVideosList = mUiLoader->findWidget<VideoListWidget>(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
	}
    if(anotherVideosList && anotherVideosList->getModel())
    {
        anotherVideosList->getModel()->doSorting(role, order);
    }

    // save sorting values only if the application is not started as a service
	if (!mVideoServices)
	{
	    // save sorting values
	    mUiUtils.saveSortingValues(role, order, mCurrentList->getLevel());
	}
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

    VideoListSelectionDialog *dialog =
        mUiLoader->findWidget<VideoListSelectionDialog>(
            DOCML_NAME_DIALOG);
    if (dialog)
    {
        TMPXItemId collectionId = mCurrentList->getModel()->getOpenItem();
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
     
    VideoListSelectionDialog *dialog =
        mUiLoader->findWidget<VideoListSelectionDialog>(
            DOCML_NAME_DIALOG);
    if (!dialog)
    {
        // fatal: no selection dialog
        return;
    }
    
    dialog->setupContent(VideoListSelectionDialog::ECreateCollection, TMPXItemId::InvalidId());
    dialog->exec();
}

// -------------------------------------------------------------------------------------------------
// addVideosToCollectionSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::addVideosToCollectionSlot()
{
	FUNC_LOG;
    if(!mCurrentList || !mCurrentList->getModel())
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
        if(allVideos && allVideos->getModel())
        {
            int count = allVideos->getModel()->rowCount();
            if(count == mCurrentList->getModel()->rowCount())
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
    TMPXItemId collectionId = mCurrentList->getModel()->getOpenItem();
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
    if(!mCurrentList || !mCurrentList->getModel())
    {
        return;
    }
    // not allowed if for some reason current widget 
    // is all videos or collection or there are no items
    if(mCurrentList->getLevel() < VideoCollectionCommon::ELevelDefaultColl ||
       !mCurrentList->getModel()->rowCount())
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
    TMPXItemId collectionId = mCurrentList->getModel()->getOpenItem();
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
    if (mVideoServices &&
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
    
    VideoSortFilterProxyModel *model = mCurrentList->getModel();
    if (!model || !model->rowCount())
    {
        return;
    }
    
    // get current sorting values
    int role;
    Qt::SortOrder order;
    model->getSorting(role, order);

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

        if (!mVideoServices)
        {
            showAction(true, DOCML_NAME_ADD_TO_COLLECTION);
            showAction(true, DOCML_NAME_DELETE_MULTIPLE);
        }
    }
    else if(mToolbarViewsActionGroup->checkedAction() == mToolbarActions[ETBActionCollections] &&
    		firstAction == mToolbarActions[ETBActionAllVideos] )
    {
        if (!mVideoServices)
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

        if (!mVideoServices)
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
    
    VideoSortFilterProxyModel *model = mCurrentList->getModel();
    if (!model || !model->rowCount())
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
    model->getSorting(role, order);
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
void VideoListView::collectionOpenedSlot(bool openingCollection,
    const QString& collection,
    const TMPXItemId &collectionId)
{
	FUNC_LOG;

    // clear toolbar actions.
    toolBar()->clearActions();

    VideoListWidget *collectionContentWidget =
                mUiLoader->findWidget<VideoListWidget>(
                    DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
    
    if(!collectionContentWidget)
    {
        return;
    }
    // update collection specific information
	mCollectionName = collection;
	
	// disable collection content animations during widget change        
	HbAbstractItemView::ItemAnimations animationState = collectionContentWidget->enabledAnimations();
	collectionContentWidget->setEnabledAnimations(HbAbstractItemView::None);
	
	if(openingCollection)
    {
        // open album view        
        if (collectionId == TMPXItemId::InvalidId() || mCurrentList == collectionContentWidget)
        {
            // no currentlist or currentlist is already collection content -list 
            collectionContentWidget->setEnabledAnimations(animationState);
            return;
        }
                
        // get level from the item to be opened only default 
        // or user defined collections can be activated here
        VideoCollectionCommon::TCollectionLevels level = VideoCollectionCommon::ELevelInvalid;
        if(collectionId.iId2 == KVcxMvcMediaTypeCategory)
        {
            level = VideoCollectionCommon::ELevelDefaultColl; 
        }
        else if(collectionId.iId2 == KVcxMvcMediaTypeAlbum)
        {
            level = VideoCollectionCommon::ELevelAlbum; 
        }
        else 
        {
            collectionContentWidget->setEnabledAnimations(animationState);
            return;
        }
        
        // Start fetching content before changing.
        VideoSortFilterProxyModel *model = collectionContentWidget->getModel();
        if(!model)
        {
            // no model for content widget, cannot activate
            collectionContentWidget->setEnabledAnimations(animationState);
            return;
        }
        model->openItem(collectionId);
        
        // deactivat current widget.
        if(mCurrentList)
        {
            mCurrentList->deactivate();
        }
        
        // activate video collection content widget and set it as current list.
        mCurrentList = collectionContentWidget;
        mCurrentList->activate(level);

        updateSubLabel();

        model->invalidate();
        
        // update hint widget for correct content
        mModelReady = model->rowCount() > 0;
        setHintLevel(VideoHintWidget::Collection);

        // update toolbar for albums, default categories don't have one.
        if(level == VideoCollectionCommon::ELevelAlbum && 
           mToolbarCollectionActionGroup && mToolbarActions.contains(ETBActionCollections))
        {
            mToolbarActions[ETBActionCollections]->setChecked(false);
            toolBar()->addActions(mToolbarCollectionActionGroup->actions());
        }
    }
    else
    {
        // open collection view
        openCollectionViewSlot();
        
        // update toolbar
        if(mToolbarViewsActionGroup && mToolbarActions.contains(ETBActionCollections))
        {
            toolBar()->addActions(mToolbarViewsActionGroup->actions());
            mToolbarActions[ETBActionCollections]->setChecked(true);
        }
    }
	// restore animations for collection content widget
	collectionContentWidget->setEnabledAnimations(animationState);
	if(!mVideoServices)
	{
	    // save / clear collection related activity data
	    VideoCollectionViewUtils::setCollectionActivityData(collectionId, collection);
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
        if(mCurrentList != object)
        {
            // this widget not yet activated so it's has been created on the second phase
            // safe to call doDelayed right away
            qobject_cast<VideoListWidget*>(object)->doDelayedsSlot();
        }
    }
    else if (name.compare(DOCML_NAME_VC_COLLECTIONWIDGET) == 0)
    {
        connect(
            object, SIGNAL(collectionOpened(bool, const QString&, const TMPXItemId&)),
            this, SLOT(collectionOpenedSlot(bool, const QString&, const TMPXItemId&)));
        if(mCurrentList != object)
        {
            // this widget not yet activated so it's has been created on the second phase
            // safe to call doDelayed right away
            qobject_cast<VideoListWidget*>(object)->doDelayedsSlot();
        }
    }
    else if (name.compare(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET) == 0)
    {
        connect(object, SIGNAL(command(int)), this, SIGNAL(command(int)));
        connect(
            object, SIGNAL(collectionOpened(bool, const QString&, const TMPXItemId&)),
            this, SLOT(collectionOpenedSlot(bool, const QString&, const TMPXItemId&)));
        if(mCurrentList != object)
        {
            // this widget not yet activated so it's has been created on the second phase
            // safe to call doDelayed right away
            qobject_cast<VideoListWidget*>(object)->doDelayedsSlot();
        }        
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

// End of file
