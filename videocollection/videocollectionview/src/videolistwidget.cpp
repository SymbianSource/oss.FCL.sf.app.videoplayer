/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Videolist content widget implementation
*
*/

#include "videocollectiontrace.h"
#include "videolistwidget.h"

#include <qcoreapplication.h>
#include <qtimer.h>
#include <hbscrollbar.h>
#include <xqserviceutil.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hblistviewitem.h>
#include <hbmessagebox.h>
#include <hbinputdialog.h>
#include <vcxmyvideosdefs.h>

#include "videocollectionviewutils.h"
#include "videocollectionuiloader.h"
#include "videolistselectiondialog.h"
#include "videoservices.h"
#include "videothumbnaildata.h"
#include "videosortfilterproxymodel.h"
#include "videocollectioncommon.h"
#include "mpxhbvideocommondefs.h"

// Interval in ms to report the scroll position.
const int SCROLL_POSITION_TIMER_TIMEOUT = 100;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoListWidget::VideoListWidget(VideoCollectionUiLoader *uiLoader, HbView *parent) :
HbListView(parent),
mModel(0),
mVideoServices(0),
mCurrentLevel(VideoCollectionCommon::ELevelInvalid),
mSignalsConnected(false),
mIsService(false),
mNavKeyAction(0),
mContextMenu(0),
mSelectionMode(HbAbstractItemView::NoSelection),
mScrollPositionTimer(0),
mUiLoader(uiLoader)
{
	FUNC_LOG_ADDR(this);
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListWidget::~VideoListWidget()
{
	FUNC_LOG_ADDR(this);
    delete mScrollPositionTimer;
    mScrollPositionTimer = 0;
	mContextMenuActions.clear();
	disconnect();
    delete mContextMenu;
    mContextMenu = 0;
    delete mNavKeyAction;
    mNavKeyAction = 0;
}

// ---------------------------------------------------------------------------
// initialize
// ---------------------------------------------------------------------------
//
int VideoListWidget::initialize(VideoSortFilterProxyModel &model, VideoServices* videoServices)
{
	FUNC_LOG_ADDR(this);
    mModel = &model;

	mVideoServices = videoServices;

	if(mVideoServices)
	{
		mIsService = true;
	}

    // init list view
    VideoCollectionViewUtils::initListView(this);

	mScrollPositionTimer = new QTimer();
	mScrollPositionTimer->setSingleShot(true);

	if (mIsService)
	{
		connect(this, SIGNAL(fileUri(const QString&)), mVideoServices, SLOT(itemSelected(const QString&)));
	}

	setModel(mModel);
	
    return 0;
}

// ---------------------------------------------------------------------------
// activate
// ---------------------------------------------------------------------------
//
int VideoListWidget::activate()
{
	FUNC_LOG_ADDR(this);
    return activate(mCurrentLevel);
}

// ---------------------------------------------------------------------------
// activate
// ---------------------------------------------------------------------------
//
int VideoListWidget::activate(VideoCollectionCommon::TCollectionLevels level)
{
	FUNC_LOG_ADDR(this);
	INFO_2("VideoListWidget::activate() [0x%x]: level: %d", this, level);
	
    if(!mModel)
    {
        return -1;
    }
    
	mCurrentLevel = level;
    setVisible(true);

    setNavigationAction();

    if (connectSignals() < 0)
    {
        ERROR_1(-1, "VideoListWidget::activate() [0x%x]: connecting signals failed.", this);
        return -1;
    }

    // open model to the current level in case not in album or category
    if (level != VideoCollectionCommon::ELevelAlbum &&
        level != VideoCollectionCommon::ELevelDefaultColl)
    {
         mModel->open(level);
    }
    
    // Enable thumbnail background fetching.
    VideoThumbnailData &thumbnailData = VideoThumbnailData::instance();
    thumbnailData.enableBackgroundFetching(true);
    thumbnailData.startBackgroundFetching(mModel, 0);

    return 0;
}

// ---------------------------------------------------------------------------
// deactivate
// ---------------------------------------------------------------------------
//
void VideoListWidget::deactivate()
{
	FUNC_LOG_ADDR(this);
    if(mContextMenu) 
    {
        mContextMenu->hide();
    }

    setVisible(false);
    disConnectSignals();

    // Free allocated memory for list thumbnails and disable background fetching.
    VideoThumbnailData &thumbnailData = VideoThumbnailData::instance();
    thumbnailData.enableBackgroundFetching(false);
    thumbnailData.freeThumbnailData();
}

// ---------------------------------------------------------------------------
// getLevel
// ---------------------------------------------------------------------------
//
VideoCollectionCommon::TCollectionLevels VideoListWidget::getLevel()
{
	FUNC_LOG_ADDR(this);
    return mCurrentLevel;
}

// ---------------------------------------------------------------------------
// connectSignals
// ---------------------------------------------------------------------------
//
int VideoListWidget::connectSignals()
{
	FUNC_LOG_ADDR(this);
	
	int retval(0);
	
    if (!mSignalsConnected)
    {
        if(!connect(this, SIGNAL(scrollingStarted()), this, SLOT(scrollingStartedSlot())) ||
           !connect(this, SIGNAL(scrollingEnded()), this, SLOT(scrollingEndedSlot())) ||
           !connect(this, SIGNAL(scrollPositionChanged(const QPointF &)), 
                   this, SLOT(scrollPositionChangedSlot(const QPointF &))) ||
           !connect(mScrollPositionTimer, SIGNAL(timeout()), this, SLOT(scrollPositionTimerSlot())) || 
           !connect(this, SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)), 
                    this, SLOT(longPressedSlot(HbAbstractViewItem *, const QPointF &))))
        {
            return -1;
        }

        if (!isBrowsingService())
        {
	        if(VideoCollectionCommon::EModelTypeCollectionContent == mModel->getType())
	        {
	            if (!connect(mNavKeyAction, SIGNAL(triggered()), this, SLOT(back())))
				{
					retval = -1;
				}
	        }
	        else
	        {
	            if (!connect(mNavKeyAction, SIGNAL(triggered()), qApp, SLOT(quit())))
				{
					retval = -1;
				}
	        }
        }
        
        mSignalsConnected = true;
    }
	return retval;
}

// ---------------------------------------------------------------------------
// disConnectSignals
// ---------------------------------------------------------------------------
//
void VideoListWidget::disConnectSignals()
{
    FUNC_LOG_ADDR(this);
    
    // safe to disconnect these always
    disconnect(
        this, SIGNAL(scrollingStarted()),
        this, SLOT(scrollingStartedSlot()));
    disconnect(
        this, SIGNAL(scrollingEnded()),
        this, SLOT(scrollingEndedSlot()));
    disconnect(
        this, SIGNAL(scrollPositionChanged(const QPointF&)), 
        this, SLOT(scrollPositionChangedSlot(const QPointF&)));
    disconnect(this, SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)), 
             this, SLOT(longPressedSlot(HbAbstractViewItem *, const QPointF &)));
    
    // check that scroll position timer is created
    if (mScrollPositionTimer)
    {
        disconnect(
            mScrollPositionTimer, SIGNAL(timeout()),
            this, SLOT(scrollPositionTimerSlot()));
    }
    
    // check that model and navigation action exists
    if (!isBrowsingService())
    {
	    if (mModel &&
	        mModel->getType() == VideoCollectionCommon::EModelTypeCollectionContent)
	    {
	        if (mNavKeyAction)
	        {
	            disconnect(
	                mNavKeyAction, SIGNAL(triggered()),
	                this, SLOT(back()));
	        }
	    }
	    else
	    {
	        if (mNavKeyAction)
	        {
	            disconnect(
	                mNavKeyAction, SIGNAL(triggered()),
	                qApp, SLOT(quit()));
	        }
	    }
    }

	mSignalsConnected = false;
}

// ---------------------------------------------------------------------------
// isBrowsingService
// ---------------------------------------------------------------------------
//
bool VideoListWidget::isBrowsingService() const
{
    FUNC_LOG_ADDR(this);
    
    bool isBrowsingService = false;
    
    if (mIsService &&
        mVideoServices &&
        mVideoServices->currentService() == VideoServices::EBrowse)
    {
        isBrowsingService = true;
    }
    
    return isBrowsingService;

}

// ---------------------------------------------------------------------------
// setNavigationAction
// ---------------------------------------------------------------------------
//
void VideoListWidget::setNavigationAction()
{
    FUNC_LOG_ADDR(this);
    
    // Create navigation action if not already created
    if (!mNavKeyAction)
    {
        if (isBrowsingService())
        {
            mNavKeyAction = new HbAction(Hb::QuitNaviAction);
            if (mNavKeyAction)
            {
                connect(
                    mNavKeyAction, SIGNAL(triggered()),
                    mVideoServices, SLOT(browsingEnded()));
            }
        }
        else if (mModel)
        {
            if (VideoCollectionCommon::EModelTypeCollectionContent == mModel->getType())
            {
                mNavKeyAction = new HbAction(Hb::BackNaviAction);
            }
            else
            {
                mNavKeyAction = new HbAction(Hb::QuitNaviAction);
            }
        }
    }
        
    // Set navigation action only when widget is not in selection mode
    if (mSelectionMode == HbAbstractItemView::NoSelection)
    {
        HbView *currentView = hbInstance->allMainWindows().value(0)->currentView();
        if (currentView)
        {
            currentView->setNavigationAction(mNavKeyAction);
        }
    }
}

// ---------------------------------------------------------------------------
// deleteItemSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::deleteItemSlot()
{
	FUNC_LOG_ADDR(this);
    if(!mModel)
    {
        return;
    }
    
    QVariant variant;
    QModelIndex index = currentIndex();
    variant = mModel->data(index, VideoCollectionCommon::KeyTitle);

    if (variant.isValid())
    {
        QString text = hbTrId("txt_videos_info_do_you_want_to_delete_1").arg(
                variant.toString());
        
        HbMessageBox *messageBox = new HbMessageBox(text, HbMessageBox::MessageTypeQuestion);
        messageBox->setAttribute(Qt::WA_DeleteOnClose);
        messageBox->open(this, SLOT(deleteItemDialogFinished(HbAction *)));
    }
}

// ---------------------------------------------------------------------------
// deleteItemDialogFinished
// ---------------------------------------------------------------------------
//
void VideoListWidget::deleteItemDialogFinished(HbAction *action)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(action == dlg->actions().at(0)) 
    {
        QModelIndex index = currentIndex();
        if(index.isValid())
        {
            QModelIndexList list;
            list.append(index);
            mModel->deleteItems(list);
        }
    }
}

// ---------------------------------------------------------------------------
// createContextMenu
// ---------------------------------------------------------------------------
//
void VideoListWidget::createContextMenu()
{
	FUNC_LOG_ADDR(this);
    if(mContextMenu)
    {
        return;
    }
    
    mContextMenu = new HbMenu();
    if (mContextMenu)
    {
        mContextMenu->setDismissPolicy(HbPopup::TapAnywhere);
        
        VideoServices::TVideoService service = VideoServices::ENoService;
        if (mIsService &&
            mVideoServices)
        {
            service = mVideoServices->currentService();
        }
        
        if (service == VideoServices::EUriFetcher)
        {
            mContextMenuActions[EActionAttach]    = 
                    mContextMenu->addAction(hbTrId("txt_videos_menu_attach"), this, SLOT(openItemSlot())); 
            mContextMenuActions[EActionPlay]    = 
                    mContextMenu->addAction(hbTrId("txt_videos_menu_play"), this, SLOT(playItemSlot())); 
            mContextMenuActions[EActionDetails] = 
                    mContextMenu->addAction(hbTrId("txt_common_menu_details"), this, SLOT(openDetailsSlot()));
        }
        else if (service == VideoServices::EBrowse)
        {
            mContextMenuActions[EActionPlay]    = 
                    mContextMenu->addAction(hbTrId("txt_videos_menu_play"), this, SLOT(playItemSlot())); 
            mContextMenuActions[EActionDelete]           = 
                    mContextMenu->addAction(hbTrId("txt_common_menu_delete"), this, SLOT(deleteItemSlot()));
            mContextMenuActions[EActionDetails] = 
                    mContextMenu->addAction(hbTrId("txt_common_menu_details"), this, SLOT(openDetailsSlot()));
        }
        else
        {
            mContextMenuActions[EActionPlay]    = 
                    mContextMenu->addAction(hbTrId("txt_videos_menu_play"), this, SLOT(playItemSlot())); 
            mContextMenuActions[EActionOpen]    = 
                                mContextMenu->addAction(hbTrId("txt_common_menu_open"), this, SLOT(openItemSlot()));  
            mContextMenuActions[EActionAddToCollection] = 
                    mContextMenu->addAction(hbTrId("txt_videos_menu_add_to_collection"), this, SLOT(addToCollectionSlot()));
            mContextMenuActions[EActionRemove]           = 
                    mContextMenu->addAction(hbTrId("txt_videos_menu_remove_from_collection"), this, SLOT(removeFromCollectionSlot()));
            mContextMenuActions[EActionRename]           = 
                    mContextMenu->addAction(hbTrId("txt_common_menu_rename_item"), this, SLOT(renameSlot()));
            mContextMenuActions[EActionRemoveCollection] = 
                    mContextMenu->addAction(hbTrId("txt_videos_menu_remove_collection"), this, SLOT(removeCollectionSlot()));   
            mContextMenuActions[EActionDelete]           = 
                    mContextMenu->addAction(hbTrId("txt_common_menu_delete"), this, SLOT(deleteItemSlot()));
            mContextMenuActions[EActionDetails]          = 
                    mContextMenu->addAction(hbTrId("txt_common_menu_details"), this, SLOT(openDetailsSlot()));
        }
    }
}

// -------------------------------------------------------------------------------------------------
// setContextMenu
// -------------------------------------------------------------------------------------------------
//
void VideoListWidget::setContextMenu()
{
	FUNC_LOG_ADDR(this);
    if(!mContextMenu)
    {
        createContextMenu();
    }
    
    if (!mContextMenu)
    {
        ERROR_1(-1, "VideoListWidget::setContextMenu() [0x%x]: failed to create context menu.", this);
        return;
    }
    
	int menuActionCount = 0;
    HbAction *action = 0;
    foreach(action, mContextMenuActions.values())
    {
        if(action)
        {
			++menuActionCount;
			action->setVisible(false);
        }
    }
    if(menuActionCount != mContextMenuActions.values().count() || mContextMenuActions.values().count() == 0)
    {
        ERROR_1(-1, "VideoListWidget::setContextMenu() [0x%x]: all actions have not been created.", this);
        delete mContextMenu;
        mContextMenu = 0;
        mContextMenuActions.clear();
    	return;
    }

    if (isBrowsingService())
    {
        setBrowsingServiceContextMenu();
        return;
    }
    

    if(mCurrentLevel == VideoCollectionCommon::ELevelVideos ||
       mCurrentLevel == VideoCollectionCommon::ELevelDefaultColl)
    {        
    	if (!mIsService)
    	{
			mContextMenuActions[EActionAddToCollection]->setVisible(true);
    		mContextMenuActions[EActionDelete]->setVisible(true);
    	} else {
            mContextMenuActions[EActionAttach]->setVisible(true);
    	}
    	mContextMenuActions[EActionPlay]->setVisible(true);
		mContextMenuActions[EActionDetails]->setVisible(true);
    }
    else if(mCurrentLevel == VideoCollectionCommon::ELevelCategory) 
    {
        mContextMenuActions[EActionOpen]->setVisible(true);
        TMPXItemId mpxId = mModel->getMediaIdAtIndex(currentIndex());
		if(!mIsService && mpxId.iId2 == KVcxMvcMediaTypeAlbum)
		{
            mContextMenuActions[EActionRename]->setVisible(true);
            mContextMenuActions[EActionRemoveCollection]->setVisible(true);
		}
    }
    else if(mCurrentLevel == VideoCollectionCommon::ELevelAlbum)
    {
    	if (!mIsService)
    	{
			mContextMenuActions[EActionRemove]->setVisible(true);
            mContextMenuActions[EActionDelete]->setVisible(true);
        } else {
            mContextMenuActions[EActionAttach]->setVisible(true);
        }
    	mContextMenuActions[EActionPlay]->setVisible(true);
		mContextMenuActions[EActionDetails]->setVisible(true);
    }
}

// ---------------------------------------------------------------------------
// setBrowsingServiceContextMenu
// ---------------------------------------------------------------------------
//
void VideoListWidget::setBrowsingServiceContextMenu()
{
    FUNC_LOG_ADDR(this);
    mContextMenuActions[EActionPlay]->setVisible(true);
    mContextMenuActions[EActionDelete]->setVisible(true);
    mContextMenuActions[EActionDetails]->setVisible(true);
}

// ---------------------------------------------------------------------------
// getModel
// ---------------------------------------------------------------------------
//
VideoSortFilterProxyModel& VideoListWidget::getModel()
{ 
	FUNC_LOG_ADDR(this);
    return *mModel; 
}

// ---------------------------------------------------------------------------
// emitActivated
// This slot is called by the fw when viewitem is activated
// ---------------------------------------------------------------------------
//
void VideoListWidget::emitActivated (const QModelIndex &modelIndex)
{
	FUNC_LOG_ADDR(this);
    // surprisingly interesting feature: after long press also single press
    // is executed. as a workaround the following hack check is needed.
    // otherwise after the context menu is shown also single press action
    // is executed.
    if (mContextMenu &&
        mContextMenu->isVisible())
    {
        // do not activate context menu if it is already visible
        return;
    }
    if(mSelectionMode != HbAbstractItemView::NoSelection)
    {
        // no custom functionality defined
        emit activated(modelIndex);
        return;
    }
    doEmitActivated(modelIndex);
}

// ---------------------------------------------------------------------------
// doEmitActivated
// ---------------------------------------------------------------------------
//
void VideoListWidget::doEmitActivated (const QModelIndex &index)
{
    if(mIsService &&
       mVideoServices &&
       mVideoServices->currentService() == VideoServices::EUriFetcher &&
       mCurrentLevel != VideoCollectionCommon::ELevelCategory)
    {
        QVariant variant = mModel->data(index, VideoCollectionCommon::KeyFilePath);
        if ( variant.isValid()  )
        {
            QString itemPath = variant.value<QString>();
            emit(fileUri(itemPath));
        }
    }
    else
    {
        doActivateItem(index);
    }
}

// ---------------------------------------------------------------------------
// doActivateItem
// ---------------------------------------------------------------------------
//
void VideoListWidget::doActivateItem(const QModelIndex &index)
{
    FUNC_LOG_ADDR(this);
    if (!mModel || !index.isValid())
    {
        return;
    }
    
    if (mCurrentLevel == VideoCollectionCommon::ELevelCategory)
    {
        QVariant variant = mModel->data(index, VideoCollectionCommon::KeyTitle);
        if (variant.isValid())
        {
            // signal view that item has been activated
            emit(collectionOpened(true,
                variant.toString(),
                index));                       
        }
        return;
    }
    else
    {
    	mModel->openItem(mModel->getMediaIdAtIndex(index));
    }
}

// ---------------------------------------------------------------------------
// setSelectionMode
// called by the fw when user long presses some item
// ---------------------------------------------------------------------------
//
void VideoListWidget::setSelectionMode(int mode)
{
	FUNC_LOG_ADDR(this);
    HbAbstractItemView::SelectionMode selMode = HbAbstractItemView::NoSelection;
    if(mode >= HbAbstractItemView::NoSelection && mode <= HbAbstractItemView::MultiSelection)
    {
        selMode = HbAbstractItemView::SelectionMode(mode);
    }

    HbListView::setSelectionMode(selMode);
    mSelectionMode = mode;
}

// ---------------------------------------------------------------------------
// longPressedSlot
// called by the fw when user long presses some item
// ---------------------------------------------------------------------------
//
void VideoListWidget::longPressedSlot(HbAbstractViewItem *item, const QPointF &point)
{
    FUNC_LOG_ADDR(this);
    if(mSelectionMode != HbAbstractItemView::NoSelection)
    {
        // do not activate context menu during selection mode
        return;
    }

    if(item)
    {
        QModelIndex index = item->modelIndex();
        if(mModel && index.isValid())
        {
            setContextMenu();
            // if menu not yet exists, it has been created
            // setup might fail causing menu to be removed
            if(mContextMenu)
            {
                mContextMenu->setPreferredPos(point);
                mContextMenu->show();
            }
        }
    }
}

// ---------------------------------------------------------------------------
// panGesture
// ---------------------------------------------------------------------------
//
void VideoListWidget::panGesture(const QPointF &point)
{
	FUNC_LOG_ADDR(this);
    if (mContextMenu)
    {
        if (!mContextMenu->isVisible())
        {
            HbListView::panGesture(point);
        }
    }
    else
    {
        HbListView::panGesture(point);
    }
}

// ---------------------------------------------------------------------------
// doDelayedsSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::doDelayedsSlot()
{
	FUNC_LOG_ADDR(this);
	if (!mContextMenu)
	{
		createContextMenu();
	}
}

// ---------------------------------------------------------------------------
// openItemSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::openItemSlot()
{
    FUNC_LOG_ADDR(this);
    doEmitActivated(currentIndex());
}

// ---------------------------------------------------------------------------
// playItemSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::playItemSlot()
{
    FUNC_LOG_ADDR(this);
    doActivateItem(currentIndex());
}

// ---------------------------------------------------------------------------
// openDetailsSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::openDetailsSlot()
{   
	FUNC_LOG_ADDR(this);
    if(mModel && mModel->fetchItemDetails(currentIndex()) == 0 ) 
    {
        emit command(MpxHbVideoCommon::ActivateVideoDetailsView);
    }
}

// ---------------------------------------------------------------------------
// renameSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::renameSlot()
{
	FUNC_LOG_ADDR(this);
    if(!mModel)
    {
        return;
    }
    
    QModelIndex index = currentIndex();
    QVariant variant = mModel->data(index, VideoCollectionCommon::KeyTitle);
    TMPXItemId itemId = mModel->getMediaIdAtIndex(index);

    if(variant.isValid() && itemId.iId2 == KVcxMvcMediaTypeAlbum)
    {
        QString label(hbTrId("txt_videos_title_enter_name"));
        QString albumName = variant.toString();
        
        HbInputDialog *dialog = new HbInputDialog();
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->getText(label, this, SLOT(renameDialogFinished(HbAction *)), albumName);
    }
}

// -------------------------------------------------------------------------------------------------
// renameDialogFinished
// -------------------------------------------------------------------------------------------------
//
void VideoListWidget::renameDialogFinished(HbAction *action)
{
    Q_UNUSED(action);

    HbInputDialog *dialog = static_cast<HbInputDialog*>(sender());
    
    QModelIndex index = currentIndex();
    TMPXItemId itemId = mModel->getMediaIdAtIndex(index);
    QVariant newNameVariant = dialog->value();
    QVariant oldNameVariant = mModel->data(index, VideoCollectionCommon::KeyTitle);
    
    if(dialog->actions().first() == action &&
       oldNameVariant.isValid() && newNameVariant.isValid() && itemId.iId2 == KVcxMvcMediaTypeAlbum)
    {
        QString newAlbumName = newNameVariant.toString();
        QString oldAlbumName = oldNameVariant.toString();
        
        if(newAlbumName.length() && newAlbumName.trimmed() != oldAlbumName)
        {
            // Resolve collection true name and rename the album
            newAlbumName = mModel->resolveAlbumName(newAlbumName);
            mModel->renameAlbum(itemId, newAlbumName);
        }
    }
}
// ---------------------------------------------------------------------------
// addToCollectionSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::addToCollectionSlot()
{
	FUNC_LOG_ADDR(this);
    VideoListSelectionDialog *dialog =
       mUiLoader->findWidget<VideoListSelectionDialog>(
           DOCML_NAME_DIALOG);
    if (!dialog || !mModel)
    {
        ERROR_1(-1, "VideoListWidget::addToCollectionSlot() [0x%x]: failed to get selection dialog or model is null.", this);
        return;
    }
    TMPXItemId itemId = mModel->getMediaIdAtIndex(currentIndex());
    if(itemId != TMPXItemId::InvalidId())
    {
        dialog->setupContent(VideoListSelectionDialog::ESelectCollection, itemId);
        dialog->exec();
    }
}

// ---------------------------------------------------------------------------
// removeFromCollectionSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::removeFromCollectionSlot()
{
	FUNC_LOG_ADDR(this);
    if(!mModel)
    {
        return;
    }
    TMPXItemId collectionId = mModel->getOpenItem();
    TMPXItemId itemId = mModel->getMediaIdAtIndex(currentIndex());
    if(collectionId != TMPXItemId::InvalidId() && 
       itemId != TMPXItemId::InvalidId())
    {
        QList<TMPXItemId> ids;
        ids.append(itemId);
        mModel->removeItemsFromAlbum(collectionId, ids);
    }
}

// ---------------------------------------------------------------------------
// removeCollectionSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::removeCollectionSlot()
{
	FUNC_LOG_ADDR(this);
    if(!mModel)
    {
        return;
    }
    
    QVariant variant;
    QModelIndex index = currentIndex();
    variant = mModel->data(index, VideoCollectionCommon::KeyTitle);

    if (variant.isValid())
    {
        QString text = hbTrId("txt_videos_info_do_you_want_to_remove_collection").arg(
                variant.toString());
        HbMessageBox *messageBox = new HbMessageBox(text, HbMessageBox::MessageTypeQuestion);
        messageBox->setAttribute(Qt::WA_DeleteOnClose);
        messageBox->open(this, SLOT(removeCollectionDialogFinished(HbAction *)));
    }
}

// ---------------------------------------------------------------------------
// removeCollectionDialogFinished
// ---------------------------------------------------------------------------
//
void VideoListWidget::removeCollectionDialogFinished(HbAction *action)
{
    HbMessageBox *dlg = static_cast<HbMessageBox*>(sender());
    if(action == dlg->actions().at(0)) 
    {
        QModelIndex index = currentIndex();
        if(index.isValid())
        {
            QModelIndexList list;
            list.append(index);
            mModel->removeAlbums(list);
        }
    }
}

// ---------------------------------------------------------------------------
// back
// ---------------------------------------------------------------------------
//
void VideoListWidget::back()
{
	FUNC_LOG_ADDR(this);

    if(mModel)
    {
    	// Empty the proxy model causing the items to be removed from list widget.
        mModel->setAlbumInUse(TMPXItemId::InvalidId());
		emit collectionOpened(false, QString(), QModelIndex());
    }
}

// ---------------------------------------------------------------------------
// scrollingStartedSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollingStartedSlot()
{
	FUNC_LOG_ADDR(this);
    VideoThumbnailData::instance().enableThumbnailCreation(false);
}

// ---------------------------------------------------------------------------
// scrollingEndedSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollingEndedSlot()
{
	FUNC_LOG_ADDR(this);
    if(mScrollPositionTimer)
    {
        mScrollPositionTimer->stop();
    }
    VideoThumbnailData::instance().enableThumbnailCreation(true);
    fetchThumbnailsForVisibleItems();
}

// ---------------------------------------------------------------------------
// scrollPositionChangedSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollPositionChangedSlot(const QPointF &newPosition)
{
    Q_UNUSED(newPosition);
	if(mScrollPositionTimer && !mScrollPositionTimer->isActive())
	    mScrollPositionTimer->start(SCROLL_POSITION_TIMER_TIMEOUT);
}

// ---------------------------------------------------------------------------
// scrollPositionTimerSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollPositionTimerSlot()
{
	FUNC_LOG_ADDR(this);
    fetchThumbnailsForVisibleItems();
}

// ---------------------------------------------------------------------------
// fetchThumbnailsForVisibleItems
// ---------------------------------------------------------------------------
//
void VideoListWidget::fetchThumbnailsForVisibleItems()
{
	FUNC_LOG_ADDR(this);
    const QList<HbAbstractViewItem *> itemsVisible = visibleItems();

    if(itemsVisible.count() > 0)
    {
        int row = itemsVisible.value(0)->modelIndex().row();
        VideoThumbnailData::instance().startBackgroundFetching(mModel, row);
    }
}

// end of file
