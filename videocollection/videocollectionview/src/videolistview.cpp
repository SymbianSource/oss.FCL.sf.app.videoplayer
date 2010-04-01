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

// remove these
#include <QDebug>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListView::VideoListView(VideoCollectionUiLoader *uiLoader, QGraphicsItem *parent) :
HbView(parent),
mUiUtils(VideoCollectionViewUtils::instance()),
mWrapper(VideoCollectionWrapper::instance()),
mUiLoader(uiLoader),
mIsService(false),
mModelReady(false),
mVideoServices(0),
mCurrentList(0),
mToolbarViewsActionGroup(0),
mToolbarCollectionActionGroup(0)
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
	if(!mUiLoader)
	{
        cleanup();
		return -1;
	}

    if (XQServiceUtil::isService() && !mVideoServices)
    {
    	mIsService = true;

    	mVideoServices = VideoServices::instance();

    	if (!mVideoServices)
        {
            cleanup();
        	return -1;
		}
        else
        {
        	connect(mVideoServices, SIGNAL(titleReady(const QString&)), this, SLOT(titleReadySlot(const QString&)));
        }
	}
    mUiLoader->setIsService(mIsService);
    
    // start open all videos model
    VideoSortFilterProxyModel *model =
        mWrapper.getModel(VideoCollectionWrapper::EAllVideos);
    if (model)
    {
        model->open(VideoCollectionCommon::ELevelVideos);
    }
    else
    {
        cleanup();
        return -1;
    }
    
    // start loading widgets
    QSet<QString> uiItems;
    uiItems.insert(DOCML_NAME_VC_VIDEOLISTWIDGET);
    uiItems.insert(DOCML_NAME_OPTIONS_MENU);
    uiItems.insert(DOCML_NAME_ADD_TO_COLLECTION);
    uiItems.insert(DOCML_NAME_CREATE_COLLECTION);
    uiItems.insert(DOCML_NAME_DELETE_MULTIPLE);
    uiItems.insert(DOCML_NAME_VC_HEADINGBANNER);
    uiItems.insert(DOCML_NAME_VC_VIDEOHINTWIDGET);
    uiItems.insert(DOCML_NAME_HINT_BUTTON);
    uiItems.insert(DOCML_NAME_NO_VIDEOS_LABEL);
    uiItems.insert(DOCML_NAME_SORT_MENU);
    uiItems.insert(DOCML_NAME_SORT_BY_DATE);
    uiItems.insert(DOCML_NAME_SORT_BY_NAME);
    uiItems.insert(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    uiItems.insert(DOCML_NAME_SORT_BY_TOTAL_LENGTH);
    uiItems.insert(DOCML_NAME_SORT_BY_RATING);
    uiItems.insert(DOCML_NAME_SORT_BY_SIZE);
    uiItems.insert(DOCML_NAME_VC_COLLECTIONWIDGET);
    uiItems.insert(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
    uiItems.insert(DOCML_NAME_DIALOG);
    mUiLoader->startLoading(uiItems,
        this,
        SLOT(widgetReadySlot(QGraphicsWidget*, const QString&)),
        SLOT(objectReadySlot(QObject*, const QString&)));
    uiItems.clear();
    
    // TODO: create toolbar temporarily here until it has been moved to docml
    if (createToolbar() != 0)
    {
        cleanup();
        return -1;
    }

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
            // activate failed, deactivate view so we get rid of dangling connections.
            deactivateView();
            return -1;
        }
        
        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
        if (mainWnd)
        {
        mainWnd->setOrientation(Qt::Vertical, false);
            if(!connect(
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
                // deactivate view so we get rid of dangling connections.
                deactivateView();
                return -1;
            }
        }
        else
        {
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
    
    mCurrentList = 0;    
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
        mToolbarActions[ETBActionAllVideos] = createAction(":/images/qtg_mono_video_all.svg",
                mToolbarViewsActionGroup, SLOT(openAllVideosViewSlot()));

        // Collections tab
        mToolbarActions[ETBActionCollections] = createAction(":/images/qtg_mono_video_collection.svg",
                mToolbarViewsActionGroup, SLOT(openCollectionViewSlot()));

        if (!mIsService)
        {
			// Services tab
			mToolbarActions[ETBActionServices] = createAction(":/images/qtg_mono_video_services.svg",
					mToolbarViewsActionGroup, SLOT(openServicesViewSlot()));
			// Add Videos tab
			mToolbarActions[ETBActionAddVideos] = createAction(":/images/mono_video_addvideos.svg",
					mToolbarCollectionActionGroup, SLOT(addVideosToCollectionSlot()));

			// Remove Videos tab
			mToolbarActions[ETBActionRemoveVideos] = createAction(":/images/mono_video_removevideos.svg",
					mToolbarCollectionActionGroup, SLOT(debugNotImplementedYet()));
        }

        // Sort by tab
        mToolbarActions[ETBActionSortVideos] = createAction(":/images/mono_video_sortvideos.svg",
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
        bar->setVisible(true);
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
    HbAction* action = new HbAction(actionGroup);
    if(!action) {
        return 0;
    }

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
    if(!mCurrentList)
    {
        return;
    }

    VideoSortFilterProxyModel &model = mCurrentList->getModel();
    VideoHintWidget *hintWidget =
        mUiLoader->findWidget<VideoHintWidget>(
            DOCML_NAME_VC_VIDEOHINTWIDGET);
    if (mModelReady &&
        model.rowCount() == 0 &&
        hintWidget)
    {
        show ? hintWidget->activate() : hintWidget->deactivate();
    }
    else if (hintWidget)
    {
        show = false;
        hintWidget->deactivate();
    }

    if(show && mToolbarViewsActionGroup && mToolbarCollectionActionGroup) 
    {
    	if(!mIsService)
    	{
    		mToolbarActions[ETBActionRemoveVideos]->setVisible(false);
    	}
        mToolbarActions[ETBActionSortVideos]->setVisible(false);

        if(mCurrentList->getLevel() == VideoCollectionCommon::ELevelDefaultColl) 
        {
        	if(!mIsService)
        	{
        		mToolbarActions[ETBActionAddVideos]->setVisible(false);
        	}
       		hintWidget->setButtonShown(false);
        } 
        else 
        {
            hintWidget->setButtonShown(true);
        }
    } 
    else if(mToolbarViewsActionGroup && mToolbarCollectionActionGroup)
    {
    	if(!mIsService)
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
        if(mToolbarActions[ETBActionSortVideos]->isVisible() == false) 
        {
            mToolbarActions[ETBActionSortVideos]->setVisible(true);
        }
        hintWidget->setButtonShown(true);
    }
    
    HbGroupBox *subLabel =
        mUiLoader->findWidget<HbGroupBox>(DOCML_NAME_VC_HEADINGBANNER);
    if (subLabel)
    {
        if (show &&
            subLabel &&
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
    VideoHintWidget *hintWidget =
        mUiLoader->findWidget<VideoHintWidget>(
            DOCML_NAME_VC_VIDEOHINTWIDGET);
    if (hintWidget)
    {
        hintWidget->setLevel(level);
    }
}

// ---------------------------------------------------------------------------
// updateSubLabel
// ---------------------------------------------------------------------------
//
void VideoListView::updateSubLabel()
{
    VideoSortFilterProxyModel *model = 0;
    if(mCurrentList)
    {
        model = &mCurrentList->getModel(); 
    }
    
    if (model)
    {
        int itemCount = model->rowCount();
        VideoListWidget *videoListWidget =
            mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_VIDEOLISTWIDGET);
        
        VideoListWidget *collectionWidget =
            mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_COLLECTIONWIDGET);
        
        VideoListWidget *collectionContentWidget =
            mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
        
        HbGroupBox *subLabel = 
            mUiLoader->findWidget<HbGroupBox>(
                DOCML_NAME_VC_HEADINGBANNER);
        
        if (mCurrentList == videoListWidget)
        {
            subLabel->setHeading(hbTrId("txt_videos_subtitle_ln_videos", itemCount));
        }
        else if (mCurrentList == collectionWidget)
        {
            subLabel->setHeading(hbTrId("txt_videos_subtitle_l1_collections", itemCount));
        }
        else if(mCurrentList == collectionContentWidget)
        {
            subLabel->setHeading(hbTrId("txt_videos_subtitle_1_l2").arg(mCollectionName).arg(itemCount));
        }  
    }
}

// ---------------------------------------------------------------------------
// showAction()
// ---------------------------------------------------------------------------
//
void VideoListView::showAction(bool show, const QString &name)
{
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
// isActionChecked()
// ---------------------------------------------------------------------------
//
bool VideoListView::isActionChecked(const QString &name)
{
    bool isChecked = false;
    
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

    // check if action is checked
    if (action)
    {
        isChecked = action->isChecked();
    }
    
    return isChecked;
}

// ---------------------------------------------------------------------------
// isActionChecked()
// ---------------------------------------------------------------------------
//
void VideoListView::setActionChecked(bool setChecked, const QString &name)
{
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

    // update action check state
    if (action)
    {
        action->setChecked(setChecked);
    }
}

// ---------------------------------------------------------------------------
// openAllVideosViewSlot()
// ---------------------------------------------------------------------------
//
void VideoListView::openAllVideosViewSlot()
{
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
        VideoCollectionViewUtils::sortModel(&model, false);

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
    debugNotImplementedYet();
}

// ---------------------------------------------------------------------------
// startSorting()
// ---------------------------------------------------------------------------
//
void VideoListView::startSorting()
{
    HbMenu *optionsMenu =
        mUiLoader->findWidget<HbMenu>(
            DOCML_NAME_OPTIONS_MENU);
    if (optionsMenu)
    {
        // get sorting role from active action
        HbAction *action = optionsMenu->activeAction()->menu()->activeAction();
        int role = mSortingRoles[action];

        // sort model
        Qt::SortOrder order(Qt::AscendingOrder);
        VideoSortFilterProxyModel &model = mCurrentList->getModel();
        if(model.sortRole() == role && model.sortOrder() == Qt::AscendingOrder)
        {
            order = Qt::DescendingOrder;
        }
        model.doSorting(role, order);

        // save sorting values
        mUiUtils.saveSortingValues(role, order);
    }    
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
    if(!mCurrentList)
    {
        return;
    }
    
    VideoSortFilterProxyModel &model = mCurrentList->getModel();

    
    bool ok = false;
    // query a name for the collection
    QString label(tr("Enter name:")); // TODO: localization missing!
    QString text(tr("New collection")); // TODO: localization missing!
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
    if(!mCurrentList)
    {
        return;
    }
    
    VideoListSelectionDialog *dialog =
        mUiLoader->findWidget<VideoListSelectionDialog>(
            DOCML_NAME_DIALOG);
    if (dialog)
    {
        if(mCurrentList->getLevel() == VideoCollectionCommon::ELevelAlbum)
        {
            // album is opened, do not proceed in case it already have same amount
            // of videos than all videos view.
            VideoListWidget *allVideos = mUiLoader->findWidget<VideoListWidget>(
                        DOCML_NAME_VC_VIDEOLISTWIDGET);
            if(allVideos)
            {
                if(allVideos->getModel().rowCount() == mCurrentList->getModel().rowCount())
                {
                    QVariant emptyAdditional;
                    mUiUtils.showStatusMsgSlot(
                            VideoCollectionCommon::statusAllVideosAlreadyInCollection,
                            emptyAdditional);
                    return;
                }  
            }
        }
        TMPXItemId collectionId = mCurrentList->getModel().getOpenItem();
        dialog->setupContent(VideoListSelectionDialog::EAddToCollection, collectionId);
        dialog->exec();
    }
}

// -------------------------------------------------------------------------------------------------
// aboutToShowMainMenuSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::aboutToShowMainMenuSlot()
{
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
    showAction(false, DOCML_NAME_SORT_BY_TOTAL_LENGTH);
    showAction(false, DOCML_NAME_SORT_BY_RATING);
    showAction(false, DOCML_NAME_SORT_BY_SIZE);
    showAction(false, DOCML_NAME_SORT_MENU);
    
    VideoSortFilterProxyModel &model = mCurrentList->getModel();
    if (!model.rowCount(QModelIndex()))
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
        if (isActionChecked(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS) ||
            isActionChecked(DOCML_NAME_SORT_BY_TOTAL_LENGTH)) 
        {
            //TODO: when changing between videos and categories, sorting order needs to be changed, if new
            //      view does not have the previously active sorting order supported
            showAction(true, DOCML_NAME_SORT_BY_DATE);
        }
        else
        {
            HbAction* action = mSortingRoles.key(role);
            if (action)
            {
                action->setChecked(true);
            }
        }
        
        showAction(true, DOCML_NAME_SORT_BY_NAME);
        showAction(true, DOCML_NAME_SORT_BY_SIZE);

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
    	
        if (isActionChecked(DOCML_NAME_SORT_BY_DATE) ||
            isActionChecked(DOCML_NAME_SORT_BY_SIZE))
    	{
            //TODO: when changing between videos and categories, sorting order needs to be changed, if new
            //      view does not have the previously active sorting order supported
            setActionChecked(true, DOCML_NAME_SORT_BY_NAME);
    	}
        else
        {
            HbAction* action = mSortingRoles.key(role);
            if (action)
            {
                action->setChecked(true);
            }
        }
        
        showAction(true, DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
        showAction(true, DOCML_NAME_SORT_BY_TOTAL_LENGTH);
    }
    else if(firstAction != mToolbarActions[ETBActionAllVideos])
    {
        //Add, Remove and Sort by will be launched from toolbar
        if (!mIsService)
        {
            showAction(true, DOCML_NAME_DELETE_MULTIPLE);
        }
    }
}

// -------------------------------------------------------------------------------------------------
// handleAsyncStatusSlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::handleAsyncStatusSlot(int statusCode, QVariant &additional)
{
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
    if(!mToolbarCollectionActionGroup || !mToolbarViewsActionGroup || !mToolbarActions.contains(ETBActionCollections))
    {
        return;
    }
    
    // update collection specific information
    mCollectionName = collection;

    HbToolBar* bar = toolBar();
    bar->clearActions();

    if (collectionOpened)
    {
        if(!index.isValid())
        {   
            return;
        }
		
        // open album view
        VideoListWidget *collectionContentWidget =
            mUiLoader->findWidget<VideoListWidget>(
                DOCML_NAME_VC_COLLECTIONCONTENTWIDGET);
        if (mCurrentList &&
            mCurrentList != collectionContentWidget)
        {
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
            
            // deactivat current
            mCurrentList->deactivate();
			
            // activate video collection content widget
            mCurrentList = collectionContentWidget;
            
            if(itemId.iId2 ==  KVcxMvcMediaTypeCategory)
            {
                mCurrentList->activate(VideoCollectionCommon::ELevelDefaultColl);
            }
            else if(itemId.iId2 == KVcxMvcMediaTypeAlbum)
            {
                mCurrentList->activate(VideoCollectionCommon::ELevelAlbum);
            }
            mCurrentList->getModel().openItem(itemId);
            
            // update hint widget, but don't show yet
            setHintLevel(VideoHintWidget::Collection);
            showHint(false);
            
            // update toolbar
            mToolbarActions[ETBActionCollections]->setChecked(false);
            bar->addActions(mToolbarCollectionActionGroup->actions());
        }
    }
    else
    {
        // open collection view
        openCollectionViewSlot();
        
        // update toolbar
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
	if(!mCurrentList)
	{
		return;
	}
	
    VideoSortFilterProxyModel &model = mCurrentList->getModel();
    if (!model.rowCount(QModelIndex()))
    {
        // no items, no menu
        return;
    }
    
    // hide all actions by default
    showAction(false, DOCML_NAME_ADD_TO_COLLECTION);
    showAction(false, DOCML_NAME_CREATE_COLLECTION);
    showAction(false, DOCML_NAME_DELETE_MULTIPLE);
    showAction(false, DOCML_NAME_SORT_BY_DATE);
    showAction(false, DOCML_NAME_SORT_BY_NAME);
    showAction(false, DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS);
    showAction(false, DOCML_NAME_SORT_BY_TOTAL_LENGTH);
    showAction(false, DOCML_NAME_SORT_BY_RATING);
    showAction(false, DOCML_NAME_SORT_BY_SIZE);
    showAction(false, DOCML_NAME_SORT_MENU);
    
    // if sort menu found, show all sort items
    HbMenu *sortMenu = mUiLoader->findWidget<HbMenu>(DOCML_NAME_SORT_MENU);
    if (sortMenu)
    {
        // show actions
        showAction(true, DOCML_NAME_SORT_MENU);
        showAction(true, DOCML_NAME_SORT_BY_DATE);
        showAction(true, DOCML_NAME_SORT_BY_NAME);
        showAction(true, DOCML_NAME_SORT_BY_SIZE);

        // show sort menu
        sortMenu->show();
        
        // execute sort menu
        QPointF coords((size()/2).width(), (size()/3).height());
        sortMenu->exec(coords);
    }
}

// -------------------------------------------------------------------------------------------------
// widgetReadySlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::widgetReadySlot(QGraphicsWidget *widget, const QString &name)
{
    if (name.compare(DOCML_NAME_VC_VIDEOLISTWIDGET) == 0)
    {
        connect(widget, SIGNAL(command(int)), this, SIGNAL(command(int)));
    }
    else if (name.compare(DOCML_NAME_VC_COLLECTIONWIDGET) == 0)
    {
        connect(
            widget, SIGNAL(collectionOpened(bool, const QString&, const QModelIndex&)),
            this, SLOT(collectionOpenedSlot(bool, const QString&, const QModelIndex&)));
    }
    else if (name.compare(DOCML_NAME_VC_COLLECTIONCONTENTWIDGET) == 0)
    {
        connect(widget, SIGNAL(command(int)), this, SIGNAL(command(int)));
        connect(
            widget, SIGNAL(collectionOpened(bool, const QString&, const QModelIndex&)),
            this, SLOT(collectionOpenedSlot(bool, const QString&, const QModelIndex&)));
    }
    else if (name.compare(DOCML_NAME_OPTIONS_MENU) == 0)
    {
        connect(
            widget, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMainMenuSlot()));
    }
    else if (name.compare(DOCML_NAME_HINT_BUTTON) == 0)
    {
        connect(widget, SIGNAL(clicked(bool)), this, SLOT(openServicesViewSlot()));
    }
}

// -------------------------------------------------------------------------------------------------
// objectReadySlot
// -------------------------------------------------------------------------------------------------
//
void VideoListView::objectReadySlot(QObject *object, const QString &name)
{
	if (name.compare(DOCML_NAME_SORT_BY_DATE) == 0)
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
            mSortingRoles[action] = Qt::DisplayRole;
        }
    }
    else if (name.compare(DOCML_NAME_SORT_BY_NUMBER_OF_ITEMS) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
        	// TODO: implement
            connect(action, SIGNAL(triggered()), this, SLOT(debugNotImplementedYet()));
            mSortingRoles[action] = 0;
        }
    }
    else if (name.compare(DOCML_NAME_SORT_BY_TOTAL_LENGTH) == 0)
    {
        HbAction *action = qobject_cast<HbAction*>(object);
        if (action)
        {
        	// TODO: implement
            connect(action, SIGNAL(triggered()), this, SLOT(debugNotImplementedYet()));
            mSortingRoles[action] = 0;
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
    HbMessageBox::information(tr("Not implemented yet"));
}

// end of file

