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
* Description:  Implementation of VideoPlayerEngine
*
*/

// Version : %version: 20 %


#include <qapplication>

#include <hbinstance.h>
#include <xqpluginloader.h>
#include <xqplugininfo.h>
#include <xqserviceutil.h>

#include "videoplayerengine.h"
#include "mpxvideoplaybackwrapper.h"
#include "videoservices.h"

// -------------------------------------------------------------------------------------------------
// QVideoPlayerEngine()
// -------------------------------------------------------------------------------------------------
//
QVideoPlayerEngine::QVideoPlayerEngine(bool isService)
    : mIsService( isService )
    , mEmbedded(false)
    , mCurrentViewPlugin( 0 )
    , mPlaybackViewPlugin( 0 )
    , mCollectionViewPlugin( 0 )
    , mFileDetailsViewPlugin( 0 )
    , mPlaybackWrapper( 0 )
    , mVideoServices(0)
{
}

// -------------------------------------------------------------------------------------------------
// ~QVideoPlayerEngine()
// -------------------------------------------------------------------------------------------------
//
QVideoPlayerEngine::~QVideoPlayerEngine()
{
    if ( mVideoServices )
    {
    	mVideoServices->decreaseReferenceCount();
    	mVideoServices = 0;
    }

    if ( mCollectionViewPlugin ) 
    {
        mCollectionViewPlugin->destroyView();
    }

    if ( mPlaybackViewPlugin ) 
    {
        mPlaybackViewPlugin->destroyView();
    }

    if ( mFileDetailsViewPlugin ) 
    {
        mFileDetailsViewPlugin->destroyView();
    }

    delete mPlaybackWrapper;

    // disconnect all signals 
    disconnect();
}

// -------------------------------------------------------------------------------------------------
// initialize()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::initialize()
{
    //
    // Clean up QVideoPlayerEngine when qApp try to quit
    //
    connect( qApp, SIGNAL( aboutToQuit() ), this, SLOT( handleQuit() ) );

    //
    // Create playback wrapper
    //
    if ( !mPlaybackWrapper )
    {
        mPlaybackWrapper = new QMpxVideoPlaybackWrapper();
        connect( mPlaybackWrapper,
                 SIGNAL( handlePlaybackView( int ) ),
                 this,
                 SLOT( handleCommand( int ) ) );
    }

    //
    // Get VideoServices instance
    //
    if( mIsService && !mVideoServices )
    {
		mVideoServices = VideoServices::instance(this);
		connect( mVideoServices, SIGNAL(activated(int)), this, SLOT(handleCommand(int)));
    }

    QList<XQPluginInfo> impls;
    XQPluginLoader::listImplementations("org.nokia.mmdt.MpxViewPlugin/1.0", impls);

    //
    // Pre-load collectionview and playbackview plugins
    //
    loadPlugin( MpxHbVideoCommon::CollectionView );

    if ( mCollectionViewPlugin ) 
    {
        mCollectionViewPlugin->createView();
    }
    
    loadPlugin( MpxHbVideoCommon::PlaybackView );

    if ( mPlaybackViewPlugin ) 
    {
        mPlaybackViewPlugin->createView();
    }

	loadPlugin( MpxHbVideoCommon::VideoDetailsView );

	if ( mFileDetailsViewPlugin )
	{
		mFileDetailsViewPlugin->createView();
	}

    //
    // default view in the app is the collection view.
    //
    if(!mIsService)
    {
        activateView( MpxHbVideoCommon::CollectionView );
    }            
}

// -------------------------------------------------------------------------------------------------
// handleCommand()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::handleCommand( int commandCode )
{
    switch ( commandCode )
    {
        case MpxHbVideoCommon::ActivateCollectionView:
        {
            if ( mCurrentViewPlugin != mCollectionViewPlugin )
            { 
                activateView( MpxHbVideoCommon::CollectionView );
            }
            break;
        }
        case MpxHbVideoCommon::ActivatePlaybackView:
        {
            if ( mCurrentViewPlugin != mPlaybackViewPlugin )
            {                
                activateView( MpxHbVideoCommon::PlaybackView );                
            }
            break;
        }
        case MpxHbVideoCommon::ActivateVideoDetailsView:
        {
            if ( mCurrentViewPlugin != mFileDetailsViewPlugin )
			{
				activateView( MpxHbVideoCommon::VideoDetailsView );                
            }
            break;
        }
        case MpxHbVideoCommon::LoadVideoDetailsView:
        {
			if ( mFileDetailsViewPlugin )
			{
				mFileDetailsViewPlugin->createView();
			}
			break;
		}
        
        default:
        {
            break;
        }
    }    
}

// -------------------------------------------------------------------------------------------------
// activateView()
// activate view based on view type.
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::activateView( MpxHbVideoCommon::MpxHbVideoViewType viewType )
{
    disconnectView();
    
    if ( mCurrentViewPlugin )
    {
        mCurrentViewPlugin->deactivateView();
        hbInstance->allMainWindows().value(0)->removeView( mCurrentViewPlugin->getView() );
        mCurrentViewPlugin = NULL;
    }

    if ( viewType == MpxHbVideoCommon::CollectionView && mCollectionViewPlugin  ) 
    {
        if ( mIsService &&  (VideoServices::EPlayback == mVideoServices->currentService()) )
            {
                qApp->quit();
                XQServiceUtil::toBackground(false);
                return;
            }
            else
            {
                mCurrentViewPlugin = mCollectionViewPlugin;
            }
    }
    else if ( viewType == MpxHbVideoCommon::PlaybackView && mPlaybackViewPlugin ) 
    {
        mCurrentViewPlugin = mPlaybackViewPlugin;
    }
    else if ( viewType == MpxHbVideoCommon::VideoDetailsView && mFileDetailsViewPlugin ) 
    {
        mCurrentViewPlugin = mFileDetailsViewPlugin;
    }
    else
    {
        // invalid plugin activation request, do nothing
        return;
    }

    hbInstance->allMainWindows().value(0)->addView( mCurrentViewPlugin->getView() );
    connectView();
    mCurrentViewPlugin->activateView();
}

// -------------------------------------------------------------------------------------------------
// loadPlugin()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::loadPlugin( MpxHbVideoCommon::MpxHbVideoViewType viewType )
{
    int viewTypeUid( 0 );

    if ( viewType == MpxHbVideoCommon::CollectionView ) 
    {
        viewTypeUid = MpxHbVideoCommon::KMpxVideoPluginDllCollectionUid;
    }
    else if ( viewType == MpxHbVideoCommon::PlaybackView ) 
    {
        viewTypeUid = MpxHbVideoCommon::KMpxVideoPluginDllPlaybackUid;
    }
    else if( viewType == MpxHbVideoCommon::VideoDetailsView )
    {
        viewTypeUid = MpxHbVideoCommon::KMpxVideoPluginDllFileDetailsUid;
    }

    if ( viewTypeUid )
    {
        // activate the collection view
        XQPluginLoader pluginLoader( viewTypeUid );
        QObject* instance = pluginLoader.instance();

        if ( instance )
        {
            if ( viewType == MpxHbVideoCommon::CollectionView ) 
            {
                mCollectionViewPlugin = qobject_cast<MpxViewPlugin*>( instance )->viewPlugin();
            }
            else if ( viewType == MpxHbVideoCommon::PlaybackView )
            {
                mPlaybackViewPlugin = qobject_cast<MpxViewPlugin*>( instance )->viewPlugin();
            }
            else if ( viewType == MpxHbVideoCommon::VideoDetailsView )
            {
                mFileDetailsViewPlugin = qobject_cast<MpxViewPlugin*>( instance )->viewPlugin();
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// connectView()
// connect application to view by setting up the signals and slots.
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::connectView()
{
    connect( mCurrentViewPlugin,
             SIGNAL( command( int ) ),
             this,
             SLOT( handleCommand( int ) ) );
}

// -------------------------------------------------------------------------------------------------
// disconnectView()
// connect application to view by setting up the signals and slots.
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::disconnectView()
{
    if ( mCurrentViewPlugin )
    {
        disconnect( mCurrentViewPlugin,
                    SIGNAL( command( int ) ),
                    this,
                    SLOT( handleCommand( int ) ) );   
    }
}

// -------------------------------------------------------------------------------------------------
// handleQuit()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::handleQuit()
{
    delete this;
}


// -------------------------------------------------------------------------------------------------
// playMedia()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::playMedia( QString filePath )
{
    mPlaybackWrapper->playMedia( filePath );
}

// -------------------------------------------------------------------------------------------------
// setEmbedded()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::setEmbedded()
{
    mEmbedded = true;
}

// End of file
