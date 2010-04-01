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
* Description:   VideoCollectionViewPlugin class implementation
*
*/

// INCLUDE FILES
#include <qcoreapplication.h>
#include <xqserviceutil.h>
#include <xqplugin.h>
#include <hbview.h>
#include <hbinstance.h>
#include <hbstackedwidget.h>
#include <hbmarqueeitem.h>
#include <hbpushbutton.h>
#include <hbaction.h>
#include <qabstractitemmodel.h>
#include <qdebug.h>
#include <hbmessagebox.h>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hblistviewitem.h>
#include <cmath>
#include <thumbnailmanager_qt.h>
#include "videocollectionclient.h"
#include "videofiledetailsviewplugin.h"
#include "videocollectioncommon.h"
#include "mpxhbvideocommondefs.h"
#include "videocollectionwrapper.h"
#include "videosortfilterproxymodel.h"
#include "videoservices.h"
#include "videodetailslabel.h"

const char* const VIDEO_DETAILS_DOCML = ":/xml/videofiledetails.docml";
const char* const VIDEO_DETAILS_GFX_PLAY = ":/gfx/play.png";
const char* const VIDEO_DETAILS_GFX_DEFAULT = ":/gfx/pri_large_video.svg";
const char* const VIDEO_DETAILS_VIEW = "videofiledetailsview";
const char* const VIDEO_DETAILS_TITLE = "mLblTitle";
const char* const VIDEO_DETAILS_THUMBNAIL = "mDetailsLabel";
const char* const VIDEO_DETAILS_BUTTON = "mButton";
const char* const VIDEO_DETAILS_MENUACTION_DELETE = "mOptionsDelete";
const char* const VIDEO_DETAILS_LISTWIDGET ="mDetailsList";

// Just for testing, remove this
void _DebugNotImplementedYet()
{
    HbMessageBox::information(QObject::tr("Not implemented yet"));
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoFileDetailsViewPlugin::VideoFileDetailsViewPlugin()
    : mModel(0),
      mVideoServices(0),
      mActivated(false),
      mIsService(false),
      mCreated(VideoFileDetailsViewPlugin::ENotCreated),
      mVideoId(TMPXItemId::InvalidId()),
      mDeletedIndex(-1),
      mNavKeyBackAction(0),
      mTitleAnim(0),
      mThumbLabel(0),
      mThumbnailManager(0),
      mCollectionWrapper(VideoCollectionWrapper::instance())
{

}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoFileDetailsViewPlugin::~VideoFileDetailsViewPlugin()
{
	destroyView();
}

// ---------------------------------------------------------------------------
// Create view
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::createView()
{
    if (VideoFileDetailsViewPlugin::EPreCreated == mCreated)
    {
    	finalizeCreateView();
    }
    else if (VideoFileDetailsViewPlugin::ENotCreated == mCreated)
    {
    	preCreateView();
    }
}

// ---------------------------------------------------------------------------
// preCreateView
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::preCreateView()
{
	mActivated = false;
	mCreated = VideoFileDetailsViewPlugin::EPreCreated;

	if (!mThumbnailManager)
	{
		mThumbnailManager = new ThumbnailManager();
	}
}
// ---------------------------------------------------------------------------
// finalizeCreateView
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::finalizeCreateView()
{
	mView.reset();
	mActivated = false;
	mCreated = VideoFileDetailsViewPlugin::EFinalized;
	bool ok = false;
	mView.load(VIDEO_DETAILS_DOCML, &ok);

	mModel = mCollectionWrapper.getModel(VideoCollectionWrapper::EAllVideos);

	if (!mModel)
		{
		qDebug() << "VideoFileDetailsViewPlugin::createView(): Unable to open collection wrapper. Cannot connect slots, aborting...";
		// TODO need to throw exception instead?
		return;
		}

	mIsService = XQServiceUtil::isService();

	// no deallocation needed for this since
	// stackedwidget takes ownership
	mTitleAnim = new HbMarqueeItem;
	HbFontSpec spec = mTitleAnim->fontSpec();
    spec.setRole( HbFontSpec::Primary );
    mTitleAnim->setFontSpec( spec );
    mTitleAnim->setLoopCount(-1);

	connect(mModel->sourceModel(),
			SIGNAL(shortDetailsReady(TMPXItemId)),
			this, SLOT(shortDetailsReadySlot(TMPXItemId)));

	connect(mModel,
			SIGNAL(fullDetailsReady(TMPXItemId)),
			this, SLOT(fullDetailsReadySlot(TMPXItemId)));

	connect(mModel,
			SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
			this, SLOT(rowsRemovedSlot(const QModelIndex&, int, int)));

	HbStackedWidget* thumbWidget = findWidget<HbStackedWidget>(VIDEO_DETAILS_THUMBNAIL);

	// no deallocation needed for this since
	// stackedwidget takes ownership
	mThumbLabel = new VideoDetailsLabel;

	mThumbLabel->setAlignment(Qt::AlignCenter);

	connect(mThumbLabel, SIGNAL(clicked(bool)), this, SLOT(startPlaybackSlot()));

	thumbWidget->addWidget(mThumbLabel);

	HbStackedWidget* title = findObject<HbStackedWidget>(VIDEO_DETAILS_TITLE);
	title->addWidget(mTitleAnim);

	HbAction* deleteAction = findObject<HbAction>(VIDEO_DETAILS_MENUACTION_DELETE);

	if (mIsService)
	{
		deleteAction->setVisible(false);
	}
	else
	{
		connect(deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteVideoSlot()));
	}
	
	// Create navigation keys.
	mNavKeyBackAction = new HbAction(Hb::BackNaviAction);
	
	connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap,void*,int,int)),
			this, SLOT(thumbnailReadySlot(QPixmap,void*,int,int)));
}

// ---------------------------------------------------------------------------
// Destroy view
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::destroyView()
{
    if (mActivated)
    {
        deactivateView();
    }

    if(mVideoServices)
    {
    	mVideoServices->decreaseReferenceCount();
    	mVideoServices = 0;
    }

    delete mNavKeyBackAction; mNavKeyBackAction = 0;
    delete mThumbnailManager; mThumbnailManager = 0;
    disconnect();
    mView.reset();
}

// ---------------------------------------------------------------------------
// Activate view
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::activateView()
{
	if ( !mActivated && (VideoFileDetailsViewPlugin::EFinalized == mCreated))
    {
        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

        HbView *currentView = mainWnd->currentView();
        if(currentView && mNavKeyBackAction)
        {
        	if (connect(mNavKeyBackAction, SIGNAL(triggered()), this, SLOT(back())))
        	{
        		currentView->setNavigationAction(mNavKeyBackAction);
        	}
        	else
        	{
        		return;
        	}
        }
        
        mainWnd->setOrientation(Qt::Vertical, false);

        // following if is for the future implementations where we should support
        // also landscape configuration.
        Qt::Orientation orientation = mainWnd->orientation();
        if ( orientation == Qt::Vertical )
        {
    		mView.load(VIDEO_DETAILS_DOCML, "portrait");
        }
        else if ( orientation == Qt::Horizontal )
        {
    		mView.load(VIDEO_DETAILS_DOCML, "landscape");
        }

    	if (mIsService && !mVideoServices)
    	{
    		mVideoServices = VideoServices::instance();

			if (!mVideoServices)
			{
				return;
			}
    	}
    	if (mIsService && mVideoServices)
		{
			HbPushButton* attachBtn = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON);
			attachBtn->setText(tr("Attach")); //TODO: Localisation

			connect(attachBtn, SIGNAL(clicked(bool)), this, SLOT(getFileUri()));
			connect(this, SIGNAL(fileUri(const QString&)), mVideoServices, SLOT(itemSelected(const QString&)));

            HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

            mainWnd->currentView()->setTitle(mVideoServices->contextTitle());
		}
    	else if(!mIsService)
    	{
    		HbPushButton* shareBtn = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON);
			connect(shareBtn, SIGNAL(triggered(bool)), this, SLOT(sendVideoSlot()));
			shareBtn->setText(hbTrId("txt_videos_opt_share"));
    	}

        connect(mainWnd,
                SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChange(Qt::Orientation)));

        connect(&mCollectionWrapper,
               SIGNAL(asyncStatus(int, QVariant&)),
               this, SLOT(handleErrorSlot(int, QVariant&)));

        // setup title size in order for animation to be enabled if needed
        mTitleAnim->setMinimumWidth(hbInstance->allMainWindows().value(0)->width()-50);
        mTitleAnim->adjustSize();

        mActivated = true;
    }
}

// ---------------------------------------------------------------------------
// Deactivate view
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::deactivateView()
{
    if ( mActivated )
    {
        mVideoId = TMPXItemId::InvalidId();

        mDeletedIndex = -1;  // set index as invalid.

        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);

        mainWnd->unsetOrientation();

        HbView *currentView = mainWnd->currentView();
        if(currentView)
        {
       		currentView->setNavigationAction(0);
        }

        disconnect(mNavKeyBackAction, SIGNAL(triggered()), this, SLOT(back()));

        disconnect(mainWnd, SIGNAL( orientationChanged( Qt::Orientation ) ),
             this, SLOT( orientationChange( Qt::Orientation ) ));

        disconnect(&mCollectionWrapper,
                       SIGNAL(asyncStatus(int, QVariant&)),
                       this, SLOT(handleErrorSlot(int, QVariant&)));

        mTitleAnim->stopAnimation();
        mTitleAnim->setText("");

        mActivated = false;

    	mThumbLabel->setIcon(HbIcon());

   		HbPushButton* button = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON);

    	if (mIsService)
    	{
			disconnect(button, SIGNAL(clicked(bool)), this, SLOT(getFileUri()));
			disconnect(this, SIGNAL(fileUri(const QString&)), mVideoServices, SLOT(itemSelected(const QString&)));
    	}
		else
		{
			disconnect(button, SIGNAL(triggered(bool)), this, SLOT(sendVideoSlot()));
		}

    }
}

// ---------------------------------------------------------------------------
// Get view
// ---------------------------------------------------------------------------
//
QGraphicsWidget* VideoFileDetailsViewPlugin::getView()
{
    return mView.findWidget(VIDEO_DETAILS_VIEW);
}

// ---------------------------------------------------------------------------
// Slot: Orientation change
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::orientationChange( Qt::Orientation orientation )
{
    if ( orientation == Qt::Vertical )
    {
    	mView.load(VIDEO_DETAILS_DOCML, "portrait");
    }

    else if ( orientation == Qt::Horizontal )
    {
       	mView.load(VIDEO_DETAILS_DOCML, "landscape");
    }
    mTitleAnim->adjustSize();
    mTitleAnim->startAnimation();
}

// ---------------------------------------------------------------------------
// Slot: back
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::back()
{
    if ( mActivated )
    {
        emit command( MpxHbVideoCommon::ActivateCollectionView );
    }
}

// ---------------------------------------------------------------------------
// Slot: shortDetailsReadySlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::shortDetailsReadySlot(TMPXItemId id)
{
    // first clear all details, so that the view doesn't display the old data.
    findWidget<HbListWidget>(VIDEO_DETAILS_LISTWIDGET)->clear();

    QModelIndex modelIndex = mModel->indexOfId(id);

    mVideoId = id;

    QVariant variant = mModel->data(modelIndex, Qt::DisplayRole);

    if (variant.isValid() && mTitleAnim)
    {
        mTitleAnim->setText(variant.toStringList().first());
    }
    startFetchingThumbnail();
}

// ---------------------------------------------------------------------------
// Slot: fullDetailsReadySlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::fullDetailsReadySlot(TMPXItemId id)
{
    using namespace VideoCollectionCommon;

    int detailCount = sizeof(VideoDetailLabelKeys) / sizeof(int);

    QModelIndex modelIndex = mModel->indexOfId(id);

    QVariant variant = mModel->data(modelIndex, KeyMetaData);

    QMap<QString, QVariant> metadata = variant.toMap();

    HbListWidget* list = findWidget<HbListWidget>(VIDEO_DETAILS_LISTWIDGET);

    if(list->count())
    {
        list->clear();
    }

    //TODO: define maximum line count once >3 supported
    HbListViewItem *prototype = list->listItemPrototype();
    prototype->setSecondaryTextRowCount(1, 3);

    for(int i = 0; i< detailCount; i++) {
        if (metadata.contains(VideoDetailLabelKeys[i]))
        {
            HbListWidgetItem* listWidgetItem = new HbListWidgetItem();
            listWidgetItem->setEnabled(false);

            listWidgetItem->setText( hbTrId(VideoDetailLabels[i]) );
            listWidgetItem->setSecondaryText( metadata[VideoDetailLabelKeys[i]].toString() );
            list->addItem( listWidgetItem );
        }
    }

    // start title animation
    mTitleAnim->startAnimation();
}

// ---------------------------------------------------------------------------
// Slot: getFileUri
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::getFileUri()
{
	if (mVideoId != TMPXItemId::InvalidId())
    {
        QModelIndex modelIndex = mModel->indexOfId(mVideoId);
		QVariant variant = mModel->data(modelIndex, VideoCollectionCommon::KeyFilePath);
		if ( variant.isValid()  )
		{
			QString itemPath = variant.value<QString>();
    		emit(fileUri(itemPath));
		}
    }
}

// ---------------------------------------------------------------------------
// Slot: startPlaybackSlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::startPlaybackSlot()
{
	if (mVideoId != TMPXItemId::InvalidId())
	{
    	mModel->openItem(mVideoId);
	}
}

// ---------------------------------------------------------------------------
// Slot: sendVideo
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::sendVideoSlot()
{
    _DebugNotImplementedYet();
}

// ---------------------------------------------------------------------------
// Slot: deleteVideo
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::deleteVideoSlot()
{
	if (mVideoId != TMPXItemId::InvalidId())
        {
		QModelIndex modelIndex = mModel->indexOfId(mVideoId);
		QVariant variant = mModel->data(modelIndex, Qt::DisplayRole);

        if (variant.isValid())
        {
            QString text = tr("Do you want to delete \"%1\"?").arg(variant.toStringList().first()); //TODO: Localisation: txt_common_menu_delete

            if (HbMessageBox::question(text))
            {
                deleteItem(modelIndex);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// deleteItem
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::deleteItem(QModelIndex index)
{
    mDeletedIndex = index.row();

    QModelIndexList list;
    list.append(index);
    mModel->deleteItems(list);
}


// ---------------------------------------------------------------------------
// Slot: rowsRemovedSlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::rowsRemovedSlot(const QModelIndex& parent,
                                                 int first, int last)
{
	Q_UNUSED(parent);

	if(mActivated && mDeletedIndex > -1 &&
       (mDeletedIndex >= first && mDeletedIndex <= last))
    {
        // item is withing the group of removed items, deactivate view
        emit command(MpxHbVideoCommon::ActivateCollectionView);
    }
}

// -------------------------------------------------------------------------------------------------
// Slot: handleErrorSlot()
// error signal received from collectionwrapper
// -------------------------------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::handleErrorSlot(int errorCode, QVariant &additional)
{
    QString msg("");
    if(errorCode == VideoCollectionCommon::statusSingleDeleteFail)
    {
        QString format = tr("Unable to delete item %1. It is currently open."); //TODO: Localisation
        if(additional.isValid())
        {
           msg = format.arg(additional.toString());
        }
    }
    if(msg.count() > 0)
    {
        // show msg box if there's something to show
        HbMessageBox::warning(msg);
    }
}

// ---------------------------------------------------------------------------
// Slot: thumbnailReadySlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::thumbnailReadySlot(QPixmap pixmap,
        void * clientData, int id, int errorCode)
{
    Q_UNUSED(clientData);
    Q_UNUSED(id);

	QSize size(mThumbLabel->size().toSize());
	QImage play(VIDEO_DETAILS_GFX_PLAY);

	if (!errorCode)
	{
		QImage sourceImage = pixmap.toImage();

		if ((sourceImage.size().height() != size.height()) || (sourceImage.size().width() != size.width()))
		{
			// Smooth scaling is very expensive (size^2). Therefore we reduce the size
			// to 2x of the destination size and using fast transformation before doing final smooth scaling.
			if (sourceImage.size().width() > (4*size.width()) || sourceImage.size().height() > (4*size.height()))
			{
				QSize intermediate_size = QSize( size.width() * 2, size.height() * 2 );
				sourceImage = sourceImage.scaled(intermediate_size, Qt::KeepAspectRatioByExpanding, Qt::FastTransformation );
			}
			sourceImage = sourceImage.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
		}

		int difference(0);
		QRect rect = mThumbLabel->rect().toRect();

		if(sourceImage.width() > size.width())
		{
			difference = sourceImage.width() - size.width();
			difference = (difference/2)+1;
			rect.moveLeft(rect.left()+difference);
		}
		else if(sourceImage.height() > size.height())
		{
			difference = sourceImage.height() - size.height();
			difference = (difference/2)+1;
			rect.moveBottom(rect.bottom()+difference);
		}

		sourceImage = sourceImage.copy(rect);
		QImage resultImage = QImage(sourceImage.size(), QImage::Format_ARGB32_Premultiplied);

		QPainter painter(&resultImage);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		painter.fillRect(resultImage.rect(), Qt::transparent);
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		painter.drawImage( (int)(sourceImage.width() - play.width())/2,
				           (int)(sourceImage.height() - play.height())/2,
				            play );
		painter.setCompositionMode(QPainter::CompositionMode_Screen);
		painter.drawImage(0, 0, sourceImage);
		painter.end();

		HbIcon compsedIcon(QPixmap::fromImage(resultImage));
	    mThumbLabel->setIcon(compsedIcon);
	}
    else
    {
		mThumbLabel->setIcon(HbIcon(VIDEO_DETAILS_GFX_DEFAULT));
	}
}

// ---------------------------------------------------------------------------
// startFetchingThumbnail
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::startFetchingThumbnail()
{
    int tnId = -1;

    if(mModel && mThumbnailManager)
    {
    	QModelIndex modelIndex = mModel->indexOfId(mVideoId);
		QVariant variant = mModel->data(modelIndex, VideoCollectionCommon::KeyFilePath);
        if(variant.isValid())
        {
            mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
            tnId = mThumbnailManager->getThumbnail(variant.toString(), 0, 5000);
        }
    }
    else
    {
        qWarning() << "Tried to start fetching thumbnail when either mModel or mThumbnailManager is NULL!";
    }

    if(tnId == -1)
    {
    	mThumbLabel->setIcon(HbIcon(VIDEO_DETAILS_GFX_DEFAULT));
    }
}

// ---------------------------------------------------------------------------
// findWidget
// ---------------------------------------------------------------------------
//
template<class T>
T* VideoFileDetailsViewPlugin::findWidget(QString name)
{
    return qobject_cast<T *>(mView.findWidget(name));
}

// ---------------------------------------------------------------------------
// findObject
// ---------------------------------------------------------------------------
//
template<class T>
T* VideoFileDetailsViewPlugin::findObject(QString name)
{
    return qobject_cast<T *>(mView.findObject(name));
}

XQ_EXPORT_PLUGIN2( videofiledetailsview, VideoFileDetailsViewPlugin );

// end of file
