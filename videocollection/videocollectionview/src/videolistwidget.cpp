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
#include <hbstyleloader.h>
#include <vcxmyvideosdefs.h>
#include "videocollectionuiloader.h"

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
mDetailsReady(false),
mIsService(false),
mNavKeyBackAction(0),
mNavKeyQuitAction(0),
mContextMenu(0),
mSelectionMode(HbAbstractItemView::NoSelection),
mScrollPositionTimer(0),
mUiLoader(uiLoader)
{
    // NOP
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListWidget::~VideoListWidget()
{
    HbStyleLoader::unregisterFilePath( ":/style/hblistviewitem.css" );
    delete mScrollPositionTimer;
    mScrollPositionTimer = 0;
	mContextMenuActions.clear();
	disconnect();
    delete mContextMenu;
    mContextMenu = 0;
    delete mNavKeyBackAction;
    mNavKeyBackAction = 0;
    delete mNavKeyQuitAction;
    mNavKeyQuitAction = 0;
}

// ---------------------------------------------------------------------------
// initialize
// ---------------------------------------------------------------------------
//
int VideoListWidget::initialize(VideoSortFilterProxyModel &model, VideoServices* videoServices)
{
    mModel = &model;

	mVideoServices = videoServices;

	if(mVideoServices)
	{
		mIsService = true;
	}

    // init list view
    VideoCollectionViewUtils::initListView(this);

    // Navigation keys.
    mNavKeyBackAction = new HbAction(Hb::BackNaviAction);
	
    mNavKeyQuitAction = new HbAction(Hb::QuitNaviAction);

    // initial setup for widget is hidden
	setVisible(false);

	mScrollPositionTimer = new QTimer();
	mScrollPositionTimer->setSingleShot(true);

	if (mIsService)
	{
		connect(this, SIGNAL(fileUri(const QString&)), mVideoServices, SLOT(itemSelected(const QString&)));
	}

    bool ret = HbStyleLoader::registerFilePath( ":/style/hblistviewitem.css" );

    if(!ret)
    {
        return -1;
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
    return activate(mCurrentLevel);
}

// ---------------------------------------------------------------------------
// activate
// ---------------------------------------------------------------------------
//
int VideoListWidget::activate(VideoCollectionCommon::TCollectionLevels level)
{
    if(!mModel)
    {
        return -1;
    }
	mCurrentLevel = level;
	setVisible(true);

    if ( connectSignals() < 0)
    {
        return -1;
    }

    HbView *currentView = hbInstance->allMainWindows().value(0)->currentView();

    // Set navigation key only when widget is not in selection mode.
    if(currentView && mSelectionMode == HbAbstractItemView::NoSelection)
    {
        if(level != VideoCollectionCommon::ELevelDefaultColl && 
           level != VideoCollectionCommon::ELevelAlbum)
        {
            if(mNavKeyQuitAction)
            {
                currentView->setNavigationAction(mNavKeyQuitAction);
            }
        }
        else if(mNavKeyBackAction)
        {
            currentView->setNavigationAction(mNavKeyBackAction);
        }
    }
    // open model to the current level in case not in album or category
    if(level != VideoCollectionCommon::ELevelAlbum &&
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
    return mCurrentLevel;
}

// ---------------------------------------------------------------------------
// connectSignals
// ---------------------------------------------------------------------------
//
int VideoListWidget::connectSignals()
{
    if (!mSignalsConnected)
    {
        if(!connect(this, SIGNAL(scrollingStarted()), this, SLOT(scrollingStartedSlot())) ||
           !connect(this, SIGNAL(scrollingEnded()), this, SLOT(scrollingEndedSlot())) ||
           !connect(this, SIGNAL(scrollPositionChanged(const QPointF &)), 
                   this, SLOT(scrollPositionChangedSlot(const QPointF &))) ||
           !connect(mScrollPositionTimer, SIGNAL(timeout()), this, SLOT(scrollPositionTimerSlot())) || 
           !connect(mNavKeyBackAction, SIGNAL(triggered()), this, SLOT(back())) ||
           !connect(mNavKeyQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()))) 
           
        {
            return -1;
        }
        mSignalsConnected = true;
    }
	return 0;
}

// ---------------------------------------------------------------------------
// disConnectSignals
// ---------------------------------------------------------------------------
//
void VideoListWidget::disConnectSignals()
{
    disconnect(this, SIGNAL(scrollingStarted()), this, SLOT(scrollingStartedSlot()));
    disconnect(this, SIGNAL(scrollingEnded()), this, SLOT(scrollingEndedSlot()));
    disconnect(this, SIGNAL(scrollPositionChanged(const QPointF&)), 
               this, SLOT(scrollPositionChangedSlot(const QPointF&)));
    disconnect(mScrollPositionTimer, SIGNAL(timeout()), this, SLOT(scrollPositionTimerSlot()));
    disconnect(mNavKeyBackAction, SIGNAL(triggered()), this, SLOT(back()));
    disconnect(mNavKeyQuitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	mSignalsConnected = false;
}

// ---------------------------------------------------------------------------
// shareItemSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::shareItemSlot()
{
    debugNotImplementedYet();
}

// ---------------------------------------------------------------------------
// deleteItemSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::deleteItemSlot()
{
    if(!mModel)
    {
        return;
    }
    
    QVariant variant;
    QModelIndex index = currentIndex();
    variant = mModel->data(index, Qt::DisplayRole);

    if (variant.isValid())
    {
        QString text = tr("Do you want to delete \"%1\"?").arg( //TODO: localisation
                variant.toStringList().first());
        if(HbMessageBox::question(text))
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
    if(mContextMenu)
    {
        return;
    }
    mContextMenu = new HbMenu();

    if (mIsService)
    {
		mContextMenuActions[EActionPlay]    = mContextMenu->addAction(hbTrId("txt_videos_menu_play"),    this, SLOT(playItemSlot())); //TODO: localisation
		mContextMenuActions[EActionDetails] = mContextMenu->addAction(hbTrId("txt_common_menu_details"), this, SLOT(openDetailsSlot()));
    }
    else
    {
        mContextMenuActions[EACtionAddToCollection]      = mContextMenu->addAction(hbTrId("txt_videos_menu_add_to_collection"), this, SLOT(addToCollectionSlot()));
        mContextMenuActions[EACtionRemoveFromCollection] = mContextMenu->addAction(hbTrId("txt_videos_menu_remove_collection"), this, SLOT(debugNotImplementedYet()));
        mContextMenuActions[EActionShare]                = mContextMenu->addAction(hbTrId("txt_videos_menu_share"), this, SLOT(shareItemSlot()));
        mContextMenuActions[EActionRename]               = mContextMenu->addAction(hbTrId("txt_common_menu_rename_item"), this, SLOT(renameSlot()));
        mContextMenuActions[EACtionRemoveCollection]     = mContextMenu->addAction(hbTrId("txt_videos_menu_remove_collection"), this, SLOT(removeCollectionSlot()));   
        mContextMenuActions[EActionDelete]               = mContextMenu->addAction(hbTrId("txt_common_menu_delete"), this, SLOT(deleteItemSlot()));
        mContextMenuActions[EActionDetails]              = mContextMenu->addAction(hbTrId("txt_common_menu_details"), this, SLOT(openDetailsSlot()));
    }
}

// -------------------------------------------------------------------------------------------------
// setContextMenu
// -------------------------------------------------------------------------------------------------
//
void VideoListWidget::setContextMenu()
{
    if(!mContextMenu)
    {
        createContextMenu();
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
    	// fatal error, some action(s) was not created before.
        delete mContextMenu;
        mContextMenu = 0;
        mContextMenuActions.clear();
    	return;
    }

    HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

    if(mCurrentLevel == VideoCollectionCommon::ELevelVideos ||
       mCurrentLevel == VideoCollectionCommon::ELevelDefaultColl)
    {
    	if (!mIsService)
    	{
			mContextMenuActions[EACtionAddToCollection]->setVisible(true);
    		mContextMenuActions[EActionShare]->setVisible(true);
    		mContextMenuActions[EActionDelete]->setVisible(true);
    	}
    	else
    	{
    		mContextMenuActions[EActionPlay]->setVisible(true);
    	}
		mContextMenuActions[EActionDetails]->setVisible(true);
    }
    else if(mCurrentLevel == VideoCollectionCommon::ELevelCategory) 
    {
		if(!mIsService)
		{
            mContextMenuActions[EActionRename]->setVisible(true);
            mContextMenuActions[EACtionRemoveCollection]->setVisible(true);
		}
    }
    else if(mCurrentLevel == VideoCollectionCommon::ELevelAlbum)
    {
    	if (!mIsService)
    	{
			mContextMenuActions[EACtionRemoveFromCollection]->setVisible(true);
            mContextMenuActions[EActionShare]->setVisible(true);
            mContextMenuActions[EActionDelete]->setVisible(true);
    	}
    	else
    	{
    		mContextMenuActions[EActionPlay]->setVisible(true);
    	}
		mContextMenuActions[EActionDetails]->setVisible(true);
    }
}


// ---------------------------------------------------------------------------
// getModel
// ---------------------------------------------------------------------------
//
VideoSortFilterProxyModel& VideoListWidget::getModel()
{ 
    return *mModel; 
}

// ---------------------------------------------------------------------------
// emitActivated
// This slot is called by the fw when viewitem is activated
// ---------------------------------------------------------------------------
//
void VideoListWidget::emitActivated (const QModelIndex &modelIndex)
{
    if(mSelectionMode != HbAbstractItemView::NoSelection)
    {
        // no custom functionality defined
        emit activated(modelIndex);
        return;
    }

    if (!mModel || !modelIndex.isValid())
    {
        return;
    }
    
    if (mCurrentLevel == VideoCollectionCommon::ELevelCategory)
    {
        QVariant variant = mModel->data(modelIndex, Qt::DisplayRole);
        if (variant.isValid())
        {
            // signal view that item has been activated
            emit(collectionOpened(true,
                variant.toStringList().first(),
                modelIndex));
            
            return;
        }
    }
    if(mIsService && (mCurrentLevel != VideoCollectionCommon::ELevelCategory))
    {
    	QVariant variant = mModel->data(modelIndex, VideoCollectionCommon::KeyFilePath);
		if ( variant.isValid()  )
		{
			QString itemPath = variant.value<QString>();
			emit(fileUri(itemPath));
		}
    }
    else
    {
    	mModel->openItem(mModel->getMediaIdAtIndex(modelIndex));
    }
}

// ---------------------------------------------------------------------------
// setSelectionMode
// called by the fw when user long presses some item
// ---------------------------------------------------------------------------
//
//
void VideoListWidget::setSelectionMode(int mode)
{
    HbAbstractItemView::SelectionMode selMode = HbAbstractItemView::NoSelection;
    if(mode >= HbAbstractItemView::NoSelection && mode <= HbAbstractItemView::ContiguousSelection)
    {
        selMode = HbAbstractItemView::SelectionMode(mode);
    }

    HbListView::setSelectionMode(selMode);
    mSelectionMode = mode;
}

// ---------------------------------------------------------------------------
// longPressGesture
// called by the fw when user long presses some item
// ---------------------------------------------------------------------------
//
//
void VideoListWidget::longPressGesture (const QPointF &point)
{
    if (!mDetailsReady)
    {
        emit command(MpxHbVideoCommon::LoadVideoDetailsView);
        mDetailsReady = true;
    }
    if(mSelectionMode != HbAbstractItemView::NoSelection)
    {
        // do not activate context menu during selection mode
        return;
    }

	QModelIndex index = currentIndex();
    if(mModel && index.isValid())
    {   
    	TMPXItemId mpxId = mModel->getMediaIdAtIndex(index);
    	// Only videos and user created albums have context menu.
    	if((mpxId.iId2 == KVcxMvcMediaTypeVideo) ||
    	   (!mIsService && mpxId.iId2 == KVcxMvcMediaTypeAlbum ))
    	{
            setContextMenu();
            // if menu not yet exists, it has been created
            // setup might fail causing menu to be removed
            if(mContextMenu)
            {
                mContextMenu->exec(point);
            }
    	}
    }
	HbListView::longPressGesture(point);
}

// ---------------------------------------------------------------------------
// playItemSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::playItemSlot()
{
    mModel->openItem(mModel->getMediaIdAtIndex(currentIndex()));
}

// ---------------------------------------------------------------------------
// openDetailsSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::openDetailsSlot()
{   
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
	debugNotImplementedYet();
}

// ---------------------------------------------------------------------------
// addToCollectionSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::addToCollectionSlot()
{
    VideoListSelectionDialog *dialog =
       mUiLoader->findWidget<VideoListSelectionDialog>(
           DOCML_NAME_DIALOG);
    if (!dialog || !mModel)
    {
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
// removeCollectionSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::removeCollectionSlot()
{
    if(!mModel)
    {
        return;
    }
    
    QVariant variant;
    QModelIndex index = currentIndex();
    variant = mModel->data(index, Qt::DisplayRole);

    if (variant.isValid())
    {
        QString text = tr("Do you want to remove collection \"%1\"?").arg( //TODO: localisation
                variant.toStringList().first());
        if(HbMessageBox::question(text))
        {
            QModelIndexList list;
            list.append(index);
        	mModel->removeAlbums(list);
        }
    }
}

// ---------------------------------------------------------------------------
// playAllSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::playAllSlot()
{
	debugNotImplementedYet();
}

// ---------------------------------------------------------------------------
// back
// ---------------------------------------------------------------------------
//
void VideoListWidget::back()
{
    if(mModel)
    {
    	emit collectionOpened(false, QString(), QModelIndex());
    }
}

// ---------------------------------------------------------------------------
// scrollingStartedSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollingStartedSlot()
{
    VideoThumbnailData::instance().enableThumbnailCreation(false);
}

// ---------------------------------------------------------------------------
// scrollingEndedSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollingEndedSlot()
{
    if(mScrollPositionTimer)
        mScrollPositionTimer->stop();	
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
    fetchThumbnailsForVisibleItems();
}

// ---------------------------------------------------------------------------
// fetchThumbnailsForVisibleItems
// ---------------------------------------------------------------------------
//
void VideoListWidget::fetchThumbnailsForVisibleItems()
{
    const QList<HbAbstractViewItem *> itemsVisible = visibleItems();

    if(itemsVisible.count() > 0)
    {
        int row = itemsVisible.value(0)->modelIndex().row();
        VideoThumbnailData::instance().startBackgroundFetching(mModel, row);
    }
}

// ---------------------------------------------------------------------------
// debugNotImplementedYet
// ---------------------------------------------------------------------------
//
void VideoListWidget::debugNotImplementedYet()
{
    HbMessageBox::information(tr("Not implemented yet"));
}

// end of file
