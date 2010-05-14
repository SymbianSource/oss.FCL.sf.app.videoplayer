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

// Version : %version: da1mmcf#30 %


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
QVideoPlayerEngine::QVideoPlayerEngine( bool isService )
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
    MPX_DEBUG(_L("QVideoPlayerEngine::QVideoPlayerEngine()"));
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
    if ( mIsService && ! mVideoServices )
    {
        mVideoServices = VideoServices::instance(this);
        connect( mVideoServices, SIGNAL(activated(int)), this, SLOT(handleCommand(int)));
    }

    QList<XQPluginInfo> impls;
    XQPluginLoader::listImplementations("org.nokia.mmdt.MpxViewPlugin/1.0", impls);
        
    if ( isPlayServiceInvoked() )
    {
        createPlayAndDetailsViews(); 
    }
    else
    {
        loadPluginAndCreateView( MpxHbVideoCommon::CollectionView );   

        if((mIsService && !(XQServiceUtil::interfaceName().contains("IVideoBrowse"))) || !mIsService)
        {
        	//Browse service will activate view once the category to be opened is informed from highway
        	activateView( MpxHbVideoCommon::CollectionView );
        }
    }
            
}

// -------------------------------------------------------------------------------------------------
// handleCommand()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::handleCommand( int commandCode )
{
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::handleCommand()"),
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
// doDelayedLoad()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::doDelayedLoad()
{
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::doDelayedLoad()"));
	
    createPlayAndDetailsViews();
	
    mDelayedLoadDone = true;
}

// -------------------------------------------------------------------------------------------------
// createPlayAndDetailsViews()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::createPlayAndDetailsViews()
{
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::createPlayAndDetailsViews()"));
    
    //
    // delayed initialization of some uiengine member variables
    // to help application startup time & improve playback start time
    //
    mPlaybackWrapper->lateInit();
    
    if ( ! mPlaybackViewPlugin )
    {
        loadPluginAndCreateView( MpxHbVideoCommon::PlaybackView );
    }

    // details view need not be created for playback via serviceFW
    if ( ! mIsPlayService && 
         ! mFileDetailsViewPlugin )
    {
        loadPluginAndCreateView( MpxHbVideoCommon::VideoDetailsView );
    }
}


// -------------------------------------------------------------------------------------------------
// activateView()
// activate view based on view type.
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::activateView( MpxHbVideoCommon::MpxHbVideoViewType viewType )
{
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::activateView() "),
                   _L("viewType = %d"), viewType );
    	  
    disconnectView();
    
    if ( mCurrentViewPlugin )
    {
        mCurrentViewPlugin->deactivateView();
        mCurrentViewPlugin = NULL;
    }

    if ( viewType == MpxHbVideoCommon::CollectionView && mCollectionViewPlugin  ) 
    {
        if ( mIsService &&  
             ( mVideoServices->currentService() == VideoServices::EPlayback ||
               mVideoServices->currentService() == VideoServices::EView || 
               mVideoServices->currentService() == VideoServices::EUriFetcher ) )
        {
            if ( mVideoServices->currentService() == VideoServices::EUriFetcher )
            {
                if ( ! mVideoServices->mFetchSelected )    
                {
                    // 
                    // view is in 'fetch' service but 'attach' operation has not been selected,
                    // therefore, go back to collection view
                    //
                    mCurrentViewPlugin = mCollectionViewPlugin;
                    setCurrentView();                    
                }
            }
            else
            {
                qApp->quit();
                XQServiceUtil::toBackground( false );
            }
        }
        else
        {
            mCurrentViewPlugin = mCollectionViewPlugin;
            setCurrentView();
        }
    }
    else if ( viewType == MpxHbVideoCommon::PlaybackView ) 
    {
        if ( ! mPlaybackViewPlugin )
    	{
            loadPluginAndCreateView( MpxHbVideoCommon::PlaybackView );
        }
		
        mCurrentViewPlugin = mPlaybackViewPlugin;
        setCurrentView();
    }
    else if ( viewType == MpxHbVideoCommon::VideoDetailsView ) 
    {
        if ( ! mFileDetailsViewPlugin )
    	{
            loadPluginAndCreateView( MpxHbVideoCommon::VideoDetailsView );	
    	}
		
        mCurrentViewPlugin = mFileDetailsViewPlugin;
        setCurrentView();
    }
}

// -------------------------------------------------------------------------------------------------
// loadPluginAndCreateView()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::loadPluginAndCreateView( MpxHbVideoCommon::MpxHbVideoViewType viewType )
{
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::loadPluginAndCreateView() "), 
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
                MPX_DEBUG(_L("QVideoPlayerEngine::loadPluginAndCreateView() : plugin not FOUND ! "));                
            }
            
            //
            // create corresponding view
            //
            if ( currentViewPlugin ) 
            {
                QGraphicsWidget *view = currentViewPlugin->getView();
          
                if ( ! view )
                {
                    MPX_DEBUG(_L("QVideoPlayerEngine::loadPluginAndCreateView() : create view "));
                    
                    currentViewPlugin->createView();
                    hbInstance->allMainWindows().value(0)->addView( currentViewPlugin->getView() );
                }
                else
                {
                    MPX_DEBUG(_L("QVideoPlayerEngine::loadPluginAndCreateView() : view has already been created! "));                    
                }
            }
            else
            {
                MPX_DEBUG(_L("QVideoPlayerEngine::loadPluginAndCreateView() : currentViewPlugin is NULL ! "));                    
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
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::playMedia()"),
                   _L("filePath = %s"), filePath.data() );  
	  	
    mPlaybackWrapper->playMedia( filePath );
}

// -------------------------------------------------------------------------------------------------
// playMedia()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::playMedia( RFile file )
{
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::playMedia( RFile )")); 
        
    mPlaybackWrapper->playMedia( file );    
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

// -------------------------------------------------------------------------------------------------
// setCurrentView()
// -------------------------------------------------------------------------------------------------
//
void QVideoPlayerEngine::setCurrentView()
{
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::setCurrentView()")); 
	
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
bool QVideoPlayerEngine::isPlayServiceInvoked()
{
    MPX_ENTER_EXIT(_L("QVideoPlayerEngine::isPlayServiceInvoked()"));       
    
    bool result = false;   
    
    if ( mIsService )
    {
        QString intface = XQServiceUtil::interfaceName();
                        
        if ( intface.contains("IVideoView") ||
             intface.contains("IFileView") )
        {
            result = true;
            mIsPlayService = true;
        }
    }
    
    return result;
}

// End of file
