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

// Version : %version: 40 %


#include <QApplication>

#include <hbinstance.h>
#include <xqpluginloader.h>
#include <xqplugininfo.h>
#include <xqserviceutil.h>
#include <hbview.h>
#include <hbapplication.h>
#include <hbactivitymanager.h>

#include "videoplayerengine.h"
#include "videoactivitystate.h"
#include "videoplaybackwrapper.h"
#include "videoservices.h"
#include "mpxvideo_debug.h"

// -------------------------------------------------------------------------------------------------
// VideoPlayerEngine()
// -------------------------------------------------------------------------------------------------
//
VideoPlayerEngine::VideoPlayerEngine( bool isService )
    : mIsService( isService )
    , mEmbedded( false )
    , mDelayedLoadDone( false )
    , mIsPlayService( false )
    , mCurrentViewPlugin( 0 )
    , mPlaybackViewPlugin( 0 )
    , mCollectionViewPlugin( 0 )
    , mFileDetailsViewPlugin( 0 )
    , mPlaybackWrapper( 0 )
    , mVideoServices( 0 )
{
    MPX_DEBUG(_L("VideoPlayerEngine::VideoPlayerEngine()"));
}

// -------------------------------------------------------------------------------------------------
// ~VideoPlayerEngine()
// -------------------------------------------------------------------------------------------------
//
VideoPlayerEngine::~VideoPlayerEngine()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::~VideoPlayerEngine()"));
    	
    if ( mVideoServices )
    {
    	mVideoServices->decreaseReferenceCount();
    	mVideoServices = 0;
    }

    if ( mCollectionViewPlugin ) 
    {
        mCollectionViewPlugin->destroyView();
        delete mCollectionViewPlugin;
        mCollectionViewPlugin = 0;
    }

    if ( mPlaybackViewPlugin ) 
    {
        mPlaybackViewPlugin->destroyView();
        delete mPlaybackViewPlugin;
        mPlaybackViewPlugin = 0;
    }

    if ( mFileDetailsViewPlugin ) 
    {
        mFileDetailsViewPlugin->destroyView();
        delete mFileDetailsViewPlugin;
        mFileDetailsViewPlugin = 0;
    }

    delete mPlaybackWrapper;

    // disconnect all signals 
    disconnect();
}

// -------------------------------------------------------------------------------------------------
// initialize()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::initialize()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::initialize()"));	
	
    //
    // Clean up VideoPlayerEngine when qApp try to quit
    //
    connect( qApp, SIGNAL( aboutToQuit() ), this, SLOT( handleQuit() ) );

    //
    // Create playback wrapper
    //
    if ( ! mPlaybackWrapper )
    {
        mPlaybackWrapper = new VideoPlaybackWrapper();
        connect( mPlaybackWrapper,
                 SIGNAL( handlePlaybackView( int ) ),
                 this,
                 SLOT( handleCommand( int ) ) );
    }

    QList<XQPluginInfo> impls;
    XQPluginLoader::listImplementations("org.nokia.mmdt.MpxViewPlugin/1.0", impls);
    
    if ( mIsService )
    {
        if ( ! mVideoServices )
        {
            mVideoServices = VideoServices::instance(this);
            connect( mVideoServices, SIGNAL(activated(int)), this, SLOT(handleCommand(int)));
        }

        if ( isPlayServiceInvoked() )
        {
            createPlaybackView(); 
        }
        else
        {
            loadPluginAndCreateView( MpxHbVideoCommon::CollectionView );  
            
            //
            // Browse service will activate view once the category to be opened is informed from highway
            // since the category is not known at this point, we do not activate view for it here
            //
            if ( ! (XQServiceUtil::interfaceName().contains("IVideoBrowse") ) )
            {
                activateView( MpxHbVideoCommon::CollectionView );   
            }
        }
    }
    else
    {
        //
        // check latest plugin type from activity manager data and create + activate it 
        // CollectionView (default) and playbackview are the ones that are accepted
        //
        MpxHbVideoCommon::MpxHbVideoViewType viewType = MpxHbVideoCommon::CollectionView;
        int typeGotten = VideoActivityState::instance().getActivityData( KEY_VIEWPLUGIN_TYPE ).toInt();
        
        if ( typeGotten == MpxHbVideoCommon::PlaybackView )  
        {
            createPlaybackView(); 
            viewType = MpxHbVideoCommon::MpxHbVideoViewType(typeGotten);
            QVariant data = VideoActivityState::instance().getActivityData( KEY_LAST_PLAYED_CLIP );
            int error = mPlaybackWrapper->replayMedia( data.toString() );
            
            // if replay fails, then activate collection view instead
            if ( error != KErrNone )
            {
                loadPluginAndCreateView( MpxHbVideoCommon::CollectionView );  
                activateView( MpxHbVideoCommon::CollectionView );                 
            }            
        }
        else
        {
            loadPluginAndCreateView( viewType );  
            activateView( viewType );        
        }
    }
            
}

// -------------------------------------------------------------------------------------------------
// handleCommand()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::handleCommand( int commandCode )
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::handleCommand()"),
                   _L("commandCode = %d"), commandCode );
    
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
            if ( ! mDelayedLoadDone )
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
// viewReadySlot()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::viewReadySlot()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::viewReady()"));
    emit applicationReady();
    // since we need to emit applicationReady only once at startup,
    // disconnect the viewReady -signal from this object
    disconnect(hbInstance->allMainWindows().value(0), SIGNAL(viewReady()), 
               this, SLOT(viewReadySlot()));
}

// -------------------------------------------------------------------------------------------------
// doDelayedLoad()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::doDelayedLoad()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::doDelayedLoad()"));
	
    createMissingViews();
	
    mDelayedLoadDone = true;
}

// -------------------------------------------------------------------------------------------------
// createPlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::createPlaybackView()
{
    mPlaybackWrapper->lateInit();
       
    if ( ! mPlaybackViewPlugin )
    {
       loadPluginAndCreateView( MpxHbVideoCommon::PlaybackView );
    }
}

// -------------------------------------------------------------------------------------------------
// createMissingViews()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::createMissingViews()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::createMissingViews()"));
    
    //
    // delayed initialization of some uiengine member variables
    // to help application startup time & improve playback start time
    //
    createPlaybackView();

    if ( ! mFileDetailsViewPlugin )
    {
        loadPluginAndCreateView( MpxHbVideoCommon::VideoDetailsView );
    }

    if ( ! mCollectionViewPlugin )
    {
        loadPluginAndCreateView( MpxHbVideoCommon::CollectionView );
    }
}


// -------------------------------------------------------------------------------------------------
// activateView()
// activate view based on view type.
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::activateView( MpxHbVideoCommon::MpxHbVideoViewType viewType )
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::activateView() "),
                   _L("viewType = %d"), viewType );
    	  
    disconnectView();
    
    if ( mCurrentViewPlugin )
    {
        mCurrentViewPlugin->deactivateView();
        mCurrentViewPlugin = NULL;
    }

    switch ( viewType )
    {
        case MpxHbVideoCommon::CollectionView:
        {
            if ( shouldExit() )
            {
                qApp->quit();
                XQServiceUtil::toBackground( false );             
            }
            else if ( shouldActivateCollectionView()  )
            {
                if ( ! mCollectionViewPlugin )
                {
                    loadPluginAndCreateView( MpxHbVideoCommon::CollectionView );
                }            
                mCurrentViewPlugin = mCollectionViewPlugin;
                setCurrentView();                    
            }
            
            break;    
        }
        case MpxHbVideoCommon::PlaybackView:
        {
            if ( ! mPlaybackViewPlugin )
            {
                loadPluginAndCreateView( MpxHbVideoCommon::PlaybackView );
            }
            
            mCurrentViewPlugin = mPlaybackViewPlugin;
            setCurrentView();     
            
            break;    
        }
        case MpxHbVideoCommon::VideoDetailsView:
        {
            if ( ! mFileDetailsViewPlugin )
            {
                loadPluginAndCreateView( MpxHbVideoCommon::VideoDetailsView );  
            }
            
            mCurrentViewPlugin = mFileDetailsViewPlugin;
            setCurrentView();       
            
            break;    
        }        
    }    
}

// -------------------------------------------------------------------------------------------------
// loadPluginAndCreateView()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::loadPluginAndCreateView( MpxHbVideoCommon::MpxHbVideoViewType viewType )
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::loadPluginAndCreateView() "), 
                   _L("viewType = %d"), viewType );
	  
    int viewTypeUid( 0 );

    if ( viewType == MpxHbVideoCommon::CollectionView ) 
    {
        viewTypeUid = MpxHbVideoCommon::KMpxVideoPluginDllCollectionUid;
    }
    else if ( viewType == MpxHbVideoCommon::PlaybackView ) 
    {
        viewTypeUid = MpxHbVideoCommon::KMpxVideoPluginDllPlaybackUid;
    }
    else if ( viewType == MpxHbVideoCommon::VideoDetailsView )
    {
        viewTypeUid = MpxHbVideoCommon::KMpxVideoPluginDllFileDetailsUid;
    }

    if ( viewTypeUid )
    {
        // 
        // load corresponding plug-in
        //
        XQPluginLoader pluginLoader( viewTypeUid );
        QObject* instance = pluginLoader.instance();

        if ( instance )
        {
            MpxViewPlugin *currentViewPlugin = NULL;

            if ( viewType == MpxHbVideoCommon::CollectionView ) 
            {
                mCollectionViewPlugin = qobject_cast<MpxViewPlugin*>( instance )->viewPlugin();
                currentViewPlugin = mCollectionViewPlugin;
            }
            else if ( viewType == MpxHbVideoCommon::PlaybackView )
            {
                mPlaybackViewPlugin = qobject_cast<MpxViewPlugin*>( instance )->viewPlugin();
                currentViewPlugin = mPlaybackViewPlugin;
            }
            else if ( viewType == MpxHbVideoCommon::VideoDetailsView )
            {
                mFileDetailsViewPlugin = qobject_cast<MpxViewPlugin*>( instance )->viewPlugin();
                currentViewPlugin = mFileDetailsViewPlugin;
            }
            else
            {
                MPX_DEBUG(_L("VideoPlayerEngine::loadPluginAndCreateView() : plugin not FOUND ! "));                
            }
            
            //
            // create corresponding view
            //
            if ( currentViewPlugin ) 
            {
                QGraphicsWidget *view = currentViewPlugin->getView();
          
                if ( ! view )
                {
                    MPX_DEBUG(_L("VideoPlayerEngine::loadPluginAndCreateView() : create view "));
                    
                    currentViewPlugin->createView();
                    hbInstance->allMainWindows().value(0)->addView( currentViewPlugin->getView() );
                }
                else
                {
                    MPX_DEBUG(_L("VideoPlayerEngine::loadPluginAndCreateView() : view has already been created! "));                    
                }
            }
            else
            {
                MPX_DEBUG(_L("VideoPlayerEngine::loadPluginAndCreateView() : currentViewPlugin is NULL! "));                    
            }            
        }
    }
}

// -------------------------------------------------------------------------------------------------
// connectView()
// connect application to view by setting up the signals and slots.
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::connectView()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::connectView()"));
	  
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
void VideoPlayerEngine::disconnectView()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::disconnectView()"));
	  
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
void VideoPlayerEngine::handleQuit()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::handleQuit()"));
	
    if ( ! mIsService )
    {
        VideoActivityState &localActivity(VideoActivityState::instance());
            
        QVariant data = QVariant();
        HbActivityManager *actManager = qobject_cast<HbApplication*>(qApp)->activityManager();
        
        //
        // deactivate is the final point for current plugin to save it's activity data into 
        // VideoActivityState before they are saved to to activity manager
        //
        mCurrentViewPlugin->deactivateView();        
        
        //
        // get and save recent view type: either playback or collection view plugins are currently used.
        // activity will not be saved from the details plugin
        //
        int viewType = MpxHbVideoCommon::CollectionView;

        if ( mCurrentViewPlugin == mPlaybackViewPlugin )
        {
            // for playback view, the state preservation and restoration should only be done
            // for loacl clips (non-streaming) - so set KEY_VIEWPLUGIN_TYPE to PlaybackView only
            // for local clips. Otherwise the default value CollectionView should be set.
            //
            QVariant playdata = VideoActivityState::instance().getActivityData( KEY_LAST_LOCAL_PLAYBACK );

            if  ( playdata.toBool() )
            {
                viewType = MpxHbVideoCommon::PlaybackView;
            }            
        }
        
        data = viewType;
        localActivity.setActivityData( data, KEY_VIEWPLUGIN_TYPE );
        
        // save data to activity manager
        actManager->addActivity( ACTIVITY_VIDEOPLAYER_MAINVIEW, 
                                 localActivity.getActivityData(),
                                 QVariantHash() );                
    }
    
    delete this;
}


// -------------------------------------------------------------------------------------------------
// playMedia()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::playMedia( QString filePath )
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::playMedia()"),
                   _L("filePath = %s"), filePath.data() );  
	  	
    mPlaybackWrapper->playMedia( filePath );
}

// -------------------------------------------------------------------------------------------------
// playMedia()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::playMedia( RFile file )
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::playMedia( RFile )")); 
        
    mPlaybackWrapper->playMedia( file );    
}

// -------------------------------------------------------------------------------------------------
// setEmbedded()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::setEmbedded()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::setEmbedded()")); 
	  
    mEmbedded = true;
}

// -------------------------------------------------------------------------------------------------
// setCurrentView()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::setCurrentView()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::setCurrentView()")); 
	
    if ( mCurrentViewPlugin )
    {
        //
        // set current view
        //
        hbInstance->allMainWindows().value(0)->setCurrentView( 
		                            qobject_cast<HbView*>( mCurrentViewPlugin->getView() ), false );

        //
        // connect view
        //
        connectView();

        // 
        // activate view
        //
        mCurrentViewPlugin->activateView();
    }
}

// -------------------------------------------------------------------------------------------------
// isPlayServiceInvoked()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlayerEngine::isPlayServiceInvoked()
{
    MPX_ENTER_EXIT(_L("VideoPlayerEngine::isPlayServiceInvoked()"));       
    
    bool result = false;   
    
    if ( mIsService )
    {
        QString intface = XQServiceUtil::interfaceName();
        MPX_DEBUG(_L("VideoPlayerEngine::isPlayServiceInvoked() : interfaceName(%s)"), intface.data() );     
                        
        if ( intface.contains("IVideoView") ||
             intface.contains("IFileView") )
        {
            result = true;
            mIsPlayService = true;
        }
    }
    
    MPX_DEBUG(_L("VideoPlayerEngine::isPlayServiceInvoked() return %d"), result);     
    return result;
}

// -------------------------------------------------------------------------------------------------
// shouldExit()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlayerEngine::shouldExit()
{            
    bool result = false;
    
    if ( mIsPlayService )  // play or view service
    {
        result = true;               
    }
    
    MPX_DEBUG(_L("VideoPlayerEngine::shouldExit() return %d"), result);     
    
    return result;
}


// -------------------------------------------------------------------------------------------------
// shouldActivateCollectionView()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlayerEngine::shouldActivateCollectionView()
{            
    bool result = true;  
    
    //
    // the only case where collection view should NOT be activated is ...
    // if we are in service and that service is fetch and if fetch is selected, 
    // in all other cases collection view should be activated
    //
    if ( mIsService &&
         mVideoServices->currentService() == VideoServices::EUriFetcher &&
         mVideoServices->mFetchSelected  )
    {
        result = false;                    
    }  
        
    MPX_DEBUG(_L("VideoPlayerEngine::shouldActivateCollectionView() return %d"), result);     
    
    return result;
}

// End of file
