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
#include <xqserviceutil.h>
#include <xqplugin.h>
#include <hbview.h>
#include <hbinstance.h>
#include <hblabel.h>
#include <hbstackedwidget.h>
#include <hbmarqueeitem.h>
#include <hbpushbutton.h>
#include <hbaction.h>
#include <hbratingslider.h>
#include <hbscrollarea.h>
#include <qabstractitemmodel.h>
#include <qdebug.h>
#include <hbmessagebox.h>
#include <cmath>
#include <hbframedrawer.h>
#include <thumbnailmanager_qt.h>
#include "videocollectionclient.h"
#include "videofiledetailsviewplugin.h"
#include "videocollectioncommon.h"
#include "mpxhbvideocommondefs.h"
#include "videocollectionwrapper.h"
#include "videosortfilterproxymodel.h"
#include "videoservices.h"

const char* const VIDEO_DETAILS_DOCML = ":/xml/videofiledetails.docml";
const char* const VIDEO_DETAILS_GFX_PLAY = ":/gfx/play.png";
const char* const VIDEO_DETAILS_GFX_DEFAULT = ":/gfx/pri_large_video.svg";
const char* const VIDEO_DETAILS_VIEW = "videofiledetailsview";
const char* const VIDEO_DETAILS_TITLE = "mLblTitle";
const char* const VIDEO_DETAILS_RATING = "mRatingSlider";
const char* const VIDEO_DETAILS_LAYOUT_VIDEO_INFO = "mLayoutVideoInfo";
const char* const VIDEO_DETAILS_DETAIL_SCROLL_AREA = "mDetailScrollArea";
const char* const VIDEO_DETAILS_ITEM = "mLblDetail";
const char* const VIDEO_DETAILS_BUTTON_PLAY = "mBtnPlay";
const char* const VIDEO_DETAILS_BUTTON_ATTACH = "mBtnAttach";
const char* const VIDEO_DETAILS_MENUACTION_DELETE = "mOptionsDelete";
const char* const VIDEO_DETAILS_MENUACTION_SHARE = "mOptionsShare";

const int VIDEO_DETAILS_DETAIL_AMOUNT = 6;

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
      mVideoIndex(-1),
      mSecSkAction(0),
      mTitleAnim(0),
      mThumbnailManager(0)
{
    mCollectionWrapper = VideoCollectionWrapper::instance();
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

	if(mCollectionWrapper)
	{
		mModel = mCollectionWrapper->getModel();
	}

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
	mTitleAnim->setLoopCount(-1);

	connect(mModel,
			SIGNAL(shortDetailsReady(int)),
			this, SLOT(shortDetailsReadySlot(int)));

	connect(mModel,
			SIGNAL(fullDetailsReady(int)),
			this, SLOT(fullDetailsReadySlot(int)));

	connect(mModel,
			SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
			this, SLOT(rowsRemovedSlot(const QModelIndex&, int, int)));

	HbPushButton* playBtn = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON_PLAY);
	HbStackedWidget* title = findObject<HbStackedWidget>(VIDEO_DETAILS_TITLE);

	title->addWidget(mTitleAnim);

	HbFrameDrawer* drawer = new HbFrameDrawer("VideoDetailsFrameGraphic", HbFrameDrawer::OnePiece);
	drawer->setFillWholeRect(true);
	playBtn->setFrameBackground(drawer);

	connect(playBtn, SIGNAL(clicked(bool)), this, SLOT(startPlaybackSlot()));

	HbAction* deleteAction = findObject<HbAction>(VIDEO_DETAILS_MENUACTION_DELETE);
	HbAction* shareAction = findObject<HbAction>(VIDEO_DETAILS_MENUACTION_SHARE);

	if (mIsService)
	{
		deleteAction->setVisible(false);
		shareAction->setVisible(false);
	}
	else
	{
		connect(deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteVideoSlot()));
		connect(shareAction, SIGNAL(triggered(bool)), this, SLOT(sendVideoSlot()));
	}
	mSecSkAction = new HbAction( Hb::BackAction );
	
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

    if(mCollectionWrapper)
    {
        mCollectionWrapper->decreaseReferenceCount();
        mCollectionWrapper = 0;
    }
    
    delete mSecSkAction; mSecSkAction = 0;
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
        mainWnd->addSoftKeyAction( Hb::SecondarySoftKey, mSecSkAction );

        Qt::Orientation orientation = mainWnd->orientation();
        if ( orientation == Qt::Vertical )
        {
        	if (mIsService)
        	{
            	mView.load(VIDEO_DETAILS_DOCML, "portrait_fetch");
        	}
        	else
        	{
        		mView.load(VIDEO_DETAILS_DOCML, "portrait");
        	}
        }
        else if ( orientation == Qt::Horizontal )
        {
        	if (mIsService)
        	{
        		mView.load(VIDEO_DETAILS_DOCML, "landscape_fetch");
        	}
        	else
        	{
        		mView.load(VIDEO_DETAILS_DOCML, "landscape");
        	}
        }

    	if (mIsService && !mVideoServices)
    	{
    		mVideoServices = VideoServices::instance();

			if (!mVideoServices)
			{
				return;
			}
			else
			{
	    		HbPushButton* attachBtn = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON_ATTACH);
	    		connect(attachBtn, SIGNAL(clicked(bool)), this, SLOT(getFileUri()));
				connect(this, SIGNAL(fileUri(const QString&)), mVideoServices, SLOT(itemSelected(const QString&)));
			}
    	}

        connect(mainWnd->softKeyAction(Hb::SecondarySoftKey),
                SIGNAL(triggered()), this, SLOT(back()));
        connect(mainWnd,
                SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChange(Qt::Orientation)));
        connect(mCollectionWrapper, 
               SIGNAL(asyncStatus(int, QVariant&)), 
               this, SLOT(handleErrorSlot(int, QVariant&)));

        // scroll the scrollarea back to top.
        findWidget<HbScrollArea>(VIDEO_DETAILS_DETAIL_SCROLL_AREA)->scrollContentsTo(QPointF(0, 0));
        
        // setup title size in order for animation to be enabled if needed
        mTitleAnim->setPreferredSize(findObject<HbStackedWidget>(VIDEO_DETAILS_TITLE)->preferredSize());
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
        mVideoIndex = -1; // set video index as invalid.
        
        HbMainWindow *mainWnd = hbInstance->allMainWindows().value(0);
        
        disconnect(mainWnd->softKeyAction(Hb::SecondarySoftKey),
                        SIGNAL(triggered()), this, SLOT(back()));

        disconnect(mainWnd, SIGNAL( orientationChanged( Qt::Orientation ) ),
             this, SLOT( orientationChange( Qt::Orientation ) ));

        disconnect(mCollectionWrapper, 
                       SIGNAL(asyncStatus(int, QVariant&)), 
                       this, SLOT(handleErrorSlot(int, QVariant&)));

        mainWnd->removeSoftKeyAction(Hb::SecondarySoftKey, mSecSkAction);
        
        mTitleAnim->stopAnimation();
        mTitleAnim->setText("");
        
        mActivated = false;

        findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON_PLAY)->setIcon(QIcon());

    	if (mIsService)
    	{
    		HbPushButton* attachBtn = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON_ATTACH);
			disconnect(attachBtn, SIGNAL(clicked(bool)), this, SLOT(getFileUri()));
			disconnect(this, SIGNAL(fileUri(const QString&)), mVideoServices, SLOT(itemSelected(const QString&)));
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
      	if (mIsService)
       	{
           	mView.load(VIDEO_DETAILS_DOCML, "portrait_fetch");
       	}
       	else
       	{
       		mView.load(VIDEO_DETAILS_DOCML, "portrait");
       	}
    }
      		   
    else if ( orientation == Qt::Horizontal ) 
    {
       	if (mIsService)
       	{
       		mView.load(VIDEO_DETAILS_DOCML, "landscape_fetch");
       	}
       	else
       	{
       		mView.load(VIDEO_DETAILS_DOCML, "landscape");
       	}
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
void VideoFileDetailsViewPlugin::shortDetailsReadySlot(int index)
{
    // first clear all details, so that the view doesn't display the old data.
    int detailCount = sizeof(VideoCollectionCommon::VideoDetailLabelKeys) / sizeof(int);
    for(int i = 0; i<detailCount; i++) {
        HbLabel* detail = findWidget<HbLabel>(VIDEO_DETAILS_ITEM + 
                QString::number(i+1));
        detail->setPlainText(QString());
    }

    QModelIndex modelIndex = mModel->index(index, 0);

    mVideoIndex = index;
    
    // index assumed to come from source model, so data will be fetched from there
    QVariant variant = mModel->sourceModel()->data(modelIndex, Qt::DisplayRole);

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
void VideoFileDetailsViewPlugin::fullDetailsReadySlot(int index)
{
    using namespace VideoCollectionCommon;
    
    int detailCount = sizeof(VideoDetailLabelKeys) / sizeof(int);

    QModelIndex modelIndex = mModel->index(index, 0);
    int detailAmount = 1;
    int heightOfVisibleItems = 0;
    
    // index received comes from source model so we need to fetch data
    // directly from there
    QVariant variant = mModel->sourceModel()->data(modelIndex, KeyMetaData);
    QMap<QString, QVariant> metadata = variant.toMap();
    
    for(int i = 0; i< detailCount; i++) {
        if (metadata.contains(VideoDetailLabelKeys[i])) {
            HbLabel* detail = findWidget<HbLabel>(VIDEO_DETAILS_ITEM + 
                    QString::number(detailAmount));
            detail->setTextWrapping(Hb::TextWordWrap);
            detail->setElideMode(Qt::ElideNone);
            detail->setPlainText(tr(VideoDetailLabels[i]).arg(
                    metadata[VideoDetailLabelKeys[i]].toString()));

            detailAmount++;
            heightOfVisibleItems += detail->preferredHeight(); 
        }
    }

    for(; detailAmount <= VIDEO_DETAILS_DETAIL_AMOUNT; detailAmount++) {
        HbLabel* detail = findWidget<HbLabel>(VIDEO_DETAILS_ITEM + 
                QString::number(detailAmount));
        detail->hide();
    }

    findWidget<HbWidget>(VIDEO_DETAILS_LAYOUT_VIDEO_INFO)->setPreferredHeight(heightOfVisibleItems);
    findWidget<HbWidget>(VIDEO_DETAILS_LAYOUT_VIDEO_INFO)->adjustSize();
 
    int rating = 0;
    if (metadata.contains(MetaKeyStarRating)) {
        rating = metadata[MetaKeyStarRating].toInt();
    }
    findWidget<HbRatingSlider>(VIDEO_DETAILS_RATING)->setCurrentRating(rating); 

    // start title animation
    mTitleAnim->startAnimation();
}

// ---------------------------------------------------------------------------
// Slot: getFileUri
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::getFileUri()
{
    if (mVideoIndex >= 0)
    {
		QVariant variant = mModel->data(mModel->index(mVideoIndex, 0), VideoCollectionCommon::KeyFilePath);
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
    if (mVideoIndex >= 0) {
        mModel->openItem(mModel->index(mVideoIndex, 0));
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
    if (mVideoIndex > -1)
        {
        QModelIndex modelIndex = mModel->index(mVideoIndex, 0);
        QVariant variant = mModel->data(modelIndex, Qt::DisplayRole);

        if (variant.isValid()) {
            QString text = tr("Do you want to delete \"%1\"?").arg(variant.toStringList().first());

            if (HbMessageBox::question(text)){
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
    QModelIndexList list;
    list.append(index);
    mModel->deleteItems(list);
}


// ---------------------------------------------------------------------------
// Slot: rowsRemovedSlot
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::rowsRemovedSlot(const QModelIndex& /*parent*/,
                                                 int first, int last)
{
    if(mActivated && mVideoIndex > -1 &&
       (mVideoIndex >= first && mVideoIndex <= last))
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
        QString format = tr("Unable to delete item %1. It is currently open.");
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

	HbPushButton* playBtn = findWidget<HbPushButton>(VIDEO_DETAILS_BUTTON_PLAY);
	QSize size(playBtn->size().toSize());
	QImage play(VIDEO_DETAILS_GFX_PLAY);

	if (!errorCode) {

		QImage sourceImage = pixmap.toImage();

		if ((sourceImage.size().height() > size.height()) || (sourceImage.size().width() > size.width()))
		{
			// Smooth scaling is very expensive (size^2). Therefore we reduce the size
			// to 2x of the destination size and using fast transformation before doing final smooth scaling.
			if (sourceImage.size().width() > (4*size.width()) || sourceImage.size().height() > (4*size.height()))
			{
				QSize intermediate_size = QSize( size.width() * 2, size.height() * 2 );
				sourceImage = sourceImage.scaled(intermediate_size, Qt::KeepAspectRatio, Qt::FastTransformation );
			}
			sourceImage = sourceImage.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		}

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
	    playBtn->setIcon(compsedIcon);
	}
    else {
		playBtn->setIcon(HbIcon(VIDEO_DETAILS_GFX_DEFAULT));
	}
}

// ---------------------------------------------------------------------------
// startFetchingThumbnail
// ---------------------------------------------------------------------------
//
void VideoFileDetailsViewPlugin::startFetchingThumbnail()
{
    int tnId = -1;
    
    if(mModel && mThumbnailManager) {
        QVariant variant = mModel->data(mModel->index(mVideoIndex, 0), VideoCollectionCommon::KeyFilePath);
        if(variant.isValid()) {
            mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
            tnId = mThumbnailManager->getThumbnail(variant.toString(), 0, 5000);
        }
    } else {
        qWarning() << "Tried to start fetching thumbnail when either mModel or mThumbnailManager is NULL!";
    }
    
    if(tnId == -1) {
        // TODO set default thumbnail here
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
