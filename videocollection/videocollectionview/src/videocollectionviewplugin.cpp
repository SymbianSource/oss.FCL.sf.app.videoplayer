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
#include <xqplugin.h>
#include <xqserviceutil.h>
#include <hbaction.h>
#include <hbapplication.h>
#include <hbinstance.h>

#include "videocollectionviewplugin.h"
#include "videolistview.h"
#include "videocollectionuiloader.h"
#include "mpxhbvideocommondefs.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoCollectionViewPlugin::VideoCollectionViewPlugin()
    : mUiLoader(0),
      mView(0),
      mActivated(false),
      mIsService(false)
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoCollectionViewPlugin::~VideoCollectionViewPlugin()
{
    destroyView();
}

// ---------------------------------------------------------------------------
// Create view
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::createView()
{
	mActivated = false;
    if ( !mView ) {

        if(!mUiLoader)
        {
            mUiLoader = new VideoCollectionUiLoader();
        }

        mUiLoader->reset();

        if (XQServiceUtil::isService())
        {
        	mIsService = true;
        }
        
        mUiLoader->setIsService(mIsService);

		bool ok(false);

		QList<QObject *> objects = mUiLoader->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);

	    if (!ok)
	    {
	    	return;
	    }

	    mView = mUiLoader->findObject<VideoListView>( DOCML_NAME_VIEW );

        if(!mView)
        {
            // TODO: handle error: creating view
            return;
        }

        if(!connect( mView, SIGNAL(command(int)), this, SIGNAL(command(int)) ) ||
           !connect( this, SIGNAL(doDelayeds()), mView, SLOT(doDelayedsSlot()) )) {
            // TODO: handle error: connecting signal
            delete mView;
            mView = 0;
            return;
        }
        mTimerId = startTimer(DELAYED_LOAD_TIMEOUT);
        mView->initializeView();
    }
}


// ---------------------------------------------------------------------------
// timerEvent
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::timerEvent(QTimerEvent *event)
{
    if (event)
    {
        if (event->timerId() == mTimerId)
        {
        	killTimer(mTimerId);
        	mTimerId = 0;

        	emit command(MpxHbVideoCommon::DoDelayedLoad);
        	emit doDelayeds();
        }
    }
}

// ---------------------------------------------------------------------------
// Destroy view
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::destroyView()
{
    deactivateView();
    disconnect();

    delete mView; 
    mView = 0;
    delete mUiLoader;
    mUiLoader = 0;
}

// ---------------------------------------------------------------------------
// Activate view
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::activateView()
{
    if ( !mActivated ) {
        HbMainWindow *wnd = mView->mainWindow();
        if(wnd)
        {
            mView->activateView();
            mActivated = true;
        }
    }
}

// ---------------------------------------------------------------------------
// Deactivate view
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::deactivateView()
{
    if ( mActivated ) {
        mView->deactivateView();
        mActivated = false;
    }
}

// ---------------------------------------------------------------------------
// Get view
// ---------------------------------------------------------------------------
//
QGraphicsWidget* VideoCollectionViewPlugin::getView()
{
    return mView;
}

// ---------------------------------------------------------------------------
// Slot: Orientation change
// // TODO: can be removed
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::orientationChange( Qt::Orientation /* orientation */ )
{
    // view handles orientation individually
}

// ---------------------------------------------------------------------------
// Slot: back
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::back()
{
    if ( mActivated ) {
        mView->back();
    }
}

XQ_EXPORT_PLUGIN2( videocollectionview, VideoCollectionViewPlugin );

// end of file
