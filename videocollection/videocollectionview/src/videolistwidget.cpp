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

#include <qtimer.h>
#include <hbscrollbar.h>
#include <xqserviceutil.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hblistviewitem.h>
#include <hbmessagebox.h>
#include <vcxmyvideosdefs.h>

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
VideoListWidget::VideoListWidget(HbView *parent) :
HbListView(parent),
mModel(0),
mVideoServices(0),
mCurrentLevel(VideoListWidget::ELevelVideos),
mSignalsConnected(false),
mDetailsReady(false),
mIsService(false),
mSecSkAction(0),
mContextMenu(0),
mLastOpenItemId(TMPXItemId::InvalidId()),
mScrollPositionTimer(0)
{
    // NOP
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoListWidget::~VideoListWidget()
{
    delete mScrollPositionTimer;
    mScrollPositionTimer = 0;
	mContextMenuActions.clear();
	disconnect();
    delete mContextMenu;
    mContextMenu = 0;
    delete mSecSkAction;
    mSecSkAction = 0;
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

    // init widget
    HbListViewItem *prototype = listItemPrototype();
    if(!prototype)
    {
        return -1;
    }
    //Use image layout in prototype
    prototype->setGraphicsSize(HbListViewItem::Thumbnail);
    setItemRecycling(true);
    setClampingStyle(HbScrollArea::BounceBackClamping);
    setScrollingStyle(HbScrollArea::PanOrFlick);
    setFrictionEnabled(true);
    setUniformItemSizes(true);  
    setSelectionMode(HbAbstractItemView::NoSelection);
    
    //Use scrollbar
    HbScrollBar *scrollBar = verticalScrollBar();
    if (!scrollBar)
    {
        return -1;
    }
    scrollBar->setInteractive(true);

    mSecSkAction = new HbAction( Hb::BackAction );

	if(!mSecSkAction)
    {
        return -1;
    }
	// initial setup for widget is hidden
	setVisible(false);

	mScrollPositionTimer = new QTimer();
	mScrollPositionTimer->setSingleShot(true);

	if (mIsService)
	{
		connect(this, SIGNAL(fileUri(const QString&)), mVideoServices, SLOT(itemSelected(const QString&)));
	}

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
int VideoListWidget::activate(VideoListWidget::TVideoListLevel level)
{
    mLastOpenItemId = TMPXItemId::InvalidId();
    if(!mModel)
    {
        return -1;
    }
	mCurrentLevel = level;
	setModel(mModel);
	setVisible(true);

	if(mContextMenu)
	{
	    mContextMenu->setEnabled(true);
	}

    if ( connectSignals() < 0)
    {
        return -1;
    }

    // Enable thumbnail background fetching.
    VideoThumbnailData &thumbnailData = VideoThumbnailData::instance();
    thumbnailData.enableBackgroundFetching(true);

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
    disConnectSignals();
    // Free allocated memory for list thumbnails and disable background fetching.
    VideoThumbnailData &thumbnailData = VideoThumbnailData::instance();
    thumbnailData.enableBackgroundFetching(false);
    thumbnailData.freeThumbnailData();
}

// ---------------------------------------------------------------------------
// connectSignals
// ---------------------------------------------------------------------------
//
int VideoListWidget::connectSignals()
{

    if (!mSignalsConnected)
    {
        if(!connect(this, SIGNAL(scrollingEnded()), this, SLOT(scrollingEndedSlot())) ||
           !connect(this, SIGNAL(scrollPositionChanged(const QPointF &)), 
                   this, SLOT(scrollPositionChangedSlot(const QPointF &))) ||
           !connect(mScrollPositionTimer, SIGNAL(timeout()), this, SLOT(scrollingEndedSlot()))
        )
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
	disconnect(this, SIGNAL(scrollingEnded()), this, SLOT(scrollingEndedSlot()));
    disconnect(this, SIGNAL(scrollPositionChanged(const QPointF&)), 
               this, SLOT(scrollPositionChangedSlot(const QPointF&)));
    disconnect(mScrollPositionTimer, SIGNAL(timeout()), this, SLOT(scrollingEndedSlot()));
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
        QString text = tr("Do you want to delete \"%1\"?").arg(
                variant.toStringList().first());
        if(HbMessageBox::question(text))
        {
            QModelIndexList list;
            list.append(index);
            mModel->deleteItems(list);

            // exec filtering
            mModel->invalidate();
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
		mContextMenuActions[EActionPlay]    = mContextMenu->addAction(tr("Play"),    this, SLOT(playItemSlot()));
		mContextMenuActions[EActionDetails] = mContextMenu->addAction(tr("Details"), this, SLOT(openDetailsSlot()));

    }
    else
    {
        mContextMenuActions[EACtionAddToCollection]      = mContextMenu->addAction(tr("Add to collection"), this, SLOT(addToCollectionSlot()));
        mContextMenuActions[EActionAddVideos]            = mContextMenu->addAction(tr("Add videos..."), this, SLOT(addItemSlot()));
        mContextMenuActions[EACtionRemoveFromCollection] = mContextMenu->addAction(tr("Remove from collection"), this, SLOT(debugNotImplementedYet()));
        mContextMenuActions[EActionShare]                = mContextMenu->addAction(tr("Share"), this, SLOT(shareItemSlot()));
        mContextMenuActions[EActionRename]               = mContextMenu->addAction(tr("Rename"), this, SLOT(renameSlot()));
        mContextMenuActions[EActionSetThumb]             = mContextMenu->addAction(tr("Set thumbnail..."), this, SLOT(debugNotImplementedYet()));
        mContextMenuActions[EACtionRemoveCollection]     = mContextMenu->addAction(tr("Remove collection"), this, SLOT(debugNotImplementedYet()));   
        mContextMenuActions[EActionDelete]               = mContextMenu->addAction(tr("Delete"), this, SLOT(deleteItemSlot()));
        mContextMenuActions[EActionDetails]              = mContextMenu->addAction(tr("Details"), this, SLOT(openDetailsSlot()));
    }
}

// -------------------------------------------------------------------------------------------------
// setContextMenu
// -------------------------------------------------------------------------------------------------
//
void VideoListWidget::setContextMenu(bool isDefaultCollection)
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

    TVideoListType type = getType();
    
    if(type == EAllVideos ||
       type == EDefaultColItems)
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
    else if(type == ECollections) 
    {
		if(!mIsService)
		{
            mContextMenuActions[EActionAddVideos]->setVisible(true);
            mContextMenuActions[EActionRename]->setVisible(true);
            mContextMenuActions[EActionSetThumb]->setVisible(true);
            mContextMenuActions[EACtionRemoveCollection]->setVisible(true);
		}
    }
    else if(type == EUserColItems)
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

// -------------------------------------------------------------------------------------------------
// getLevel
// -------------------------------------------------------------------------------------------------
//
VideoListWidget::TVideoListType VideoListWidget::getType()
{
	TVideoListType type(EUnknown);
	HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

	if(mCurrentLevel == ELevelVideos)
    {
	    type = EAllVideos;
    }
    else if(mCurrentLevel == ELevelCategory)
    {
        if(mLastOpenItemId == TMPXItemId::InvalidId())
        {
            type = ECollections;
        }
        else if(mLastOpenItemId.iId2 == KVcxMvcCategoryIdDownloads ||
                mLastOpenItemId.iId2 == KVcxMvcCategoryIdCaptured)
        {
            type = EDefaultColItems;
        }
        else
        {
            type = EUserColItems;
        }
    }
    return type;
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
    if(selectionMode() == HbAbstractItemView::MultiSelection)
    {
         // do nothing in case selection mode
         return;
    }

    if (!mModel || !modelIndex.isValid())
    {
        return;
    }
    if (mCurrentLevel == ELevelCategory)
    {
        if (getType() == ECollections)
        {
            QVariant variant = mModel->data(modelIndex, Qt::DisplayRole);
            if (variant.isValid())
            {
                HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
                mainWnd->addSoftKeyAction( Hb::SecondarySoftKey, mSecSkAction );

                connect(mainWnd->softKeyAction(Hb::SecondarySoftKey),
                        SIGNAL(triggered()), this, SLOT(back()));
                mLastOpenItemId = mModel->getMediaIdAtIndex(modelIndex);
                emit(collectionOpened(true, variant.toStringList().first()));
            }
        }
    }
    if(mIsService && (mCurrentLevel == ELevelCategory) && (getType() != ECollections))
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
    	mModel->openItem(modelIndex);
    }
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
    if(selectionMode() == HbAbstractItemView::MultiSelection)
    {
        // do not activate context menu during selection mode
        return;
    }

	QModelIndex index = currentIndex();
    if(mModel && index.isValid())
    {
        
    	TMPXItemId mpxId = mModel->getMediaIdAtIndex(index);

    	bool defaultCollection(true);    	
    	if ( ((mpxId.iId2 == 1) && //Category
    		 ((mpxId.iId1 != KVcxMvcCategoryIdDownloads) &&
    		  (mpxId.iId1 != KVcxMvcCategoryIdCaptured))) ||
    		  (mCurrentLevel != ELevelCategory))
    	{
    	    defaultCollection = false;
		}
    	
      	
        setContextMenu(defaultCollection);
        // if menu not yet exists, it has been created
        // setup might fails causing menu to be removed
        if(mContextMenu)
        {
            mContextMenu->exec(point);
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
    mModel->openItem(currentIndex());
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
// addItemSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::addItemSlot()
{
	debugNotImplementedYet();
}

// ---------------------------------------------------------------------------
// addToCollectionSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::addToCollectionSlot()
{
	debugNotImplementedYet();
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
    mLastOpenItemId = TMPXItemId::InvalidId();
    if(mModel)
    {
        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
    
        disconnect(mainWnd->softKeyAction(Hb::SecondarySoftKey),
                SIGNAL(triggered()), this, SLOT(back()));
    
        mainWnd->removeSoftKeyAction(Hb::SecondarySoftKey, mSecSkAction);
            
        emit(collectionOpened(false, QString("")));
    
        mModel->back();
    }
}

// ---------------------------------------------------------------------------
// scrollingEndedSlot
// ---------------------------------------------------------------------------
//
void VideoListWidget::scrollingEndedSlot()
{
    if(mScrollPositionTimer)
        mScrollPositionTimer->stop();

    const QList<HbAbstractViewItem *> itemsVisible = visibleItems();

    if(itemsVisible.count() > 0)
    {
        int row = itemsVisible.value(0)->modelIndex().row();
        VideoThumbnailData::instance().startBackgroundFetching(row);
    }
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
// debugNotImplementedYet
// ---------------------------------------------------------------------------
//
void VideoListWidget::debugNotImplementedYet()
{
    HbMessageBox::information(tr("Not implemented yet"));
}

// end of file
