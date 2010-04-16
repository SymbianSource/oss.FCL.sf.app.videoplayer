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

// Version : %version: 23 %


#include <QApplication>

#include <hbinstance.h>
#include <xqpluginloader.h>
#include <xqplugininfo.h>
#include <xqserviceutil.h>
#include <hbview.h>

#include "videoplayerengine.h"
#include "mpxvideoplaybackwrapper.h"
#include "videoservices.h"
#include "mpxvideo_debug.h"

// -------------------------------------------------------------------------------------------------
// QVideoPlayerEngine()
// -------------------------------------------------------------------------------------------------
//
QVideoPlayerEngine::QVideoPlayerEngine(bool isService)
    : mIsService( isService )
    , mEmbedded(false)
    , mDelayedLoadDone(false)
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
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::~QVideoPlayerEngine()"));
    	
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
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::initialize()"));	
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
        hbInstance->allMainWindows().value(0)->addView( mCollectionViewPlugin->getView() );
    }
    
    //
    // default view in the app is the collection view.
    //
    if(!mIsService)
    {
        activateView( MpxHbVideoCommon::CollectionView );
    } 
    
    // delayed initialization of some uiengine member variables
    // to help application startup time & improve playback start time
    //
    mPlaybackWrapper->lateInit();        
    
}

// -------------------------------------------------------------------------------------------------
// handleCommand()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::handleCommand( int commandCode )
{
	  MPX_ENTER_EXIT(_L("QVideoPlayerEngine::handleCommand()"));	
	  
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
        case MpxHbVideoCommon::DoDelayedLoad:
        {
			if ( !mDelayedLoadDone )
			{
				doDelayedLoad();
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
// doDelayedLoad()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::doDelayedLoad()
{
    if ( !mPlaybackViewPlugin )
	{
	    loadPlugin( MpxHbVideoCommon::PlaybackView );

	    if ( mPlaybackViewPlugin ) 
	    {
	        mPlaybackViewPlugin->createView();
	        hbInstance->allMainWindows().value(0)->addView( mPlaybackViewPlugin->getView() );
	    }
	}

    if ( !mFileDetailsViewPlugin )
	{
	    loadPlugin( MpxHbVideoCommon::VideoDetailsView );

		if ( mFileDetailsViewPlugin )
		{
			mFileDetailsViewPlugin->createView();
			hbInstance->allMainWindows().value(0)->addView( mFileDetailsViewPlugin->getView() );
		}
	}
	
	mDelayedLoadDone = true;
}

// -------------------------------------------------------------------------------------------------
// activateView()
// activate view based on view type.
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::activateView( MpxHbVideoCommon::MpxHbVideoViewType viewType )
{
	  MPX_ENTER_EXIT(_L("QVideoPlayerEngine::activateView()"));
	  
    disconnectView();
    
    if ( mCurrentViewPlugin )
    {
        mCurrentViewPlugin->deactivateView();
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
    else if ( viewType == MpxHbVideoCommon::PlaybackView ) 
    {
		if(!mPlaybackViewPlugin)
    	{
    		loadPlugin( MpxHbVideoCommon::PlaybackView );
    	    if ( mPlaybackViewPlugin ) 
    	    {
    	        mPlaybackViewPlugin->createView();
    	        hbInstance->allMainWindows().value(0)->addView( mPlaybackViewPlugin->getView() );
    	    }
    		else
    		{
    			return;
    		}
    	}
        mCurrentViewPlugin = mPlaybackViewPlugin;
    }
    else if ( viewType == MpxHbVideoCommon::VideoDetailsView ) 
    {
		if(!mFileDetailsViewPlugin)
    	{
    		loadPlugin( MpxHbVideoCommon::VideoDetailsView );	
    		if ( mFileDetailsViewPlugin )
    		{
    			mFileDetailsViewPlugin->createView();
    			hbInstance->allMainWindows().value(0)->addView( mFileDetailsViewPlugin->getView() );
    		}
    		else
    		{
    			return;
    		}
    	}
        mCurrentViewPlugin = mFileDetailsViewPlugin;
    }
    else
    {
        // invalid plugin activation request, do nothing
        return;
    }
    
    hbInstance->allMainWindows().value(0)->setCurrentView( static_cast<HbView*>( mCurrentViewPlugin->getView() ), false );
    connectView();
    mCurrentViewPlugin->activateView();
}

// -------------------------------------------------------------------------------------------------
// loadPlugin()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::loadPlugin( MpxHbVideoCommon::MpxHbVideoViewType viewType )
{
	  MPX_ENTER_EXIT(_L("QVideoPlayerEngine::loadPlugin()"));
	  
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
	  MPX_ENTER_EXIT(_L("QVideoPlayerEngine::connectView()"));
	  
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
	  MPX_ENTER_EXIT(_L("QVideoPlayerEngine::disconnectView()"));
	  
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
	  MPX_ENTER_EXIT(_L("QVideoPlayerEngine::handleQuit()"));
	  
    delete this;
}


// -------------------------------------------------------------------------------------------------
// playMedia()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::playMedia( QString filePath )
{
	  MPX_ENTER_EXIT(_L("QVideoPlayerEngine::playMedia()")); 
	  	
    mPlaybackWrapper->playMedia( filePath );
}

// -------------------------------------------------------------------------------------------------
// setEmbedded()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::setEmbedded()
{
	  MPX_ENTER_EXIT(_L("QVideoPlayerEngine::setEmbedded()")); 
	  
    mEmbedded = true;
}

// End of file
