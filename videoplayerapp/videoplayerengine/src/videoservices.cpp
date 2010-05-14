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
* Description:  Implementation of VideoServices
*
*/

// Version : %version: da1mmcf#8 %

#include "videoplayerengine.h"
#include "videoservices.h"
#include "videoserviceurifetch.h"
#include "videoserviceplay.h"
#include "videoserviceview.h"
#include "videoservicebrowse.h"
#include "mpxvideo_debug.h"
#include <xqaiwdecl.h>
#include <xqserviceutil.h>

VideoServices *VideoServices::mInstance = 0;

// -----------------------------------------------------------------------------
// VideoServices::instance()
// -----------------------------------------------------------------------------
//
VideoServices* VideoServices::instance(QVideoPlayerEngine* engine)
{
    MPX_ENTER_EXIT(_L("VideoServices::instance()"));
    
    if ( ! mInstance )
    {
        mInstance = new VideoServices(engine);
    }
    else if ( engine && ! mInstance->engine() )
    {
    	mInstance->setEngine(engine);
    }

    mInstance->mReferenceCount++;
    return mInstance;
}

// -----------------------------------------------------------------------------
// VideoServices::decreaseReferenceCount()
// -----------------------------------------------------------------------------
//
void VideoServices::decreaseReferenceCount()
{
    MPX_ENTER_EXIT(_L("VideoServices::decreaseReferenceCount()"));
    
    if ( mInstance )
    {
        if( --mInstance->mReferenceCount == 0 )
        {
            delete mInstance;
            mInstance = NULL;
        }
    }
}

// ----------------------------------------------------------------------------
// setEngine()
// ----------------------------------------------------------------------------
//
void VideoServices::setEngine(QVideoPlayerEngine* engine)
{
    MPX_ENTER_EXIT(_L("VideoServices::setEngine()"));
    
    if ( mServicePlay )
    {
    	mEngine = engine;
    	mServicePlay->setEngine(engine);
    }
    
    if ( mServiceView )
    {
    	mEngine = engine;
    	mServiceView->setEngine(engine);    	
    }
    		
}

// ----------------------------------------------------------------------------
// engine()
// ----------------------------------------------------------------------------
//
QVideoPlayerEngine* VideoServices::engine()
{
    MPX_DEBUG(_L("VideoServices::engine"));
	
    return mEngine;
}

// ----------------------------------------------------------------------------
// VideoServices()
// ----------------------------------------------------------------------------
//
VideoServices::VideoServices( QVideoPlayerEngine* engine ) 
    : mReferenceCount( 0 )
    , mEngine( engine )
    , mCurrentService( VideoServices::ENoService )
    , mFetchSelected( false )
{
    MPX_ENTER_EXIT(_L("VideoServices::VideoServices()"));
   
    mServicePlay     = new VideoServicePlay(this, engine, QLatin1String("videoplayer.com.nokia.symbian.IVideoView"));  //New service, new interface
    mServiceView     = new VideoServiceView(this, engine, QLatin1String("videoplayer.com.nokia.symbian.IFileView"));   //New service, new interface        
    mServiceUriFetch = new VideoServiceUriFetch(this, QLatin1String("videoplayer.com.nokia.symbian.IVideoFetch")); //New service, new interface
    mServiceBrowse   = new VideoServiceBrowse(  this, QLatin1String("videoplayer.com.nokia.symbian.IVideoBrowse")); //New service, new interface

    mServicePlayDeprecatedNewService = new VideoServicePlay(this, engine, QLatin1String("videoplayer.IVideoView")); //New service, old interface
    mServicePlayDeprecatedOldService = new VideoServicePlay(this, engine, QLatin1String("com.nokia.Videos.IVideoView")); //Old service, old interface   
    
    mServiceViewDeprecatedNewService = new VideoServiceView(this, engine, QLatin1String("videoplayer.IFileView")); //New service, old interface
    mServiceViewDeprecatedOldService = new VideoServiceView(this, engine, QLatin1String("com.nokia.Videos.IFileView")); //Old service, old interface     
    
    mServiceUriFetchDeprecatedNewService = new VideoServiceUriFetch(this, QLatin1String("videoplayer.IVideoFetch")); //New service, old interface
    mServiceUriFetchDeprecatedOldService = new VideoServiceUriFetch(this, QLatin1String("com.nokia.Videos.IVideoFetch")); //Old service, old interface    

    mServiceBrowseDeprecatedNewService   = new VideoServiceBrowse(this, QLatin1String("videoplayer.IVideoBrowse")); //New service, old interface
    mServiceBrowseDeprecatedOldService   = new VideoServiceBrowse(this, QLatin1String("com.nokia.Videos.IVideoBrowse")); //Old service, old interface
}

// ----------------------------------------------------------------------------
// ~VideoServices()
// ----------------------------------------------------------------------------
//
VideoServices::~VideoServices()
{
    MPX_ENTER_EXIT(_L("VideoServices::~VideoServices()"));
    
    delete mServiceUriFetch;
    delete mServicePlay;
    delete mServiceView;
    delete mServiceBrowse;
    delete mServicePlayDeprecatedNewService;
    delete mServicePlayDeprecatedOldService;
    delete mServiceViewDeprecatedNewService;
    delete mServiceViewDeprecatedOldService;
    delete mServiceBrowseDeprecatedNewService;
    delete mServiceUriFetchDeprecatedNewService;
    delete mServiceBrowseDeprecatedOldService;
    delete mServiceUriFetchDeprecatedOldService;
}

// ----------------------------------------------------------------------------
// currentService()
// ----------------------------------------------------------------------------
//
VideoServices::TVideoService VideoServices::currentService()
{
    MPX_DEBUG(_L("VideoServices::currentService() ret %d"), mCurrentService );
	
    return mCurrentService;
}

// ----------------------------------------------------------------------------
// getBrowseCategory()
// ----------------------------------------------------------------------------
//
int VideoServices::getBrowseCategory() const
{
    MPX_DEBUG(_L("VideoServices::getBrowseCategory()"));
	
    int category = 0;

    if ( mServiceBrowse && (XQServiceUtil::interfaceName().contains("symbian")))
    {
        category = mServiceBrowse->getBrowseCategory();
    }
    else if ( mServiceBrowseDeprecatedNewService && mServiceBrowseDeprecatedOldService && !(XQServiceUtil::interfaceName().contains("symbian")))
    {
    	if (mServiceBrowseDeprecatedNewService->isActive())
    	{
    		category = mServiceBrowseDeprecatedNewService->getBrowseCategory();
    	}
    	else
    	{
    		category = mServiceBrowseDeprecatedOldService->getBrowseCategory();
    	}
    }

    return category;
}

// ----------------------------------------------------------------------------
// setCurrentService()
// ----------------------------------------------------------------------------
//
void VideoServices::setCurrentService(VideoServices::TVideoService service)
{
    MPX_DEBUG(_L("VideoServices::setCurrentService(%d)"), service );
	
    mCurrentService = service;	
}

// ----------------------------------------------------------------------------
// contextTitle()
// ----------------------------------------------------------------------------
//
QString VideoServices::contextTitle() const
{
    MPX_DEBUG(_L("VideoServices::contextTitle()") );
	
    QString title;

    if (mCurrentService == VideoServices::EUriFetcher)
    {
    	if(mServiceUriFetch && (XQServiceUtil::interfaceName().contains("symbian")))
    	{
    		title = mServiceUriFetch->contextTitle();
    	}
    	else if (mServiceUriFetchDeprecatedNewService && mServiceUriFetchDeprecatedOldService && !(XQServiceUtil::interfaceName().contains("symbian")))
    	{
    		if(mServiceUriFetchDeprecatedNewService->isActive())
    		{
    			title = mServiceUriFetchDeprecatedNewService->contextTitle();
    		}
    		else
    		{
    			title = mServiceUriFetchDeprecatedOldService->contextTitle();
    		}
    	}
    }
    else if (mCurrentService == VideoServices::EBrowse)
    {
    	if ( mServiceBrowse && (XQServiceUtil::interfaceName().contains("symbian")))
		{
			title = mServiceBrowse->contextTitle();
		}
    	else if ( mServiceBrowseDeprecatedNewService && mServiceBrowseDeprecatedOldService && !(XQServiceUtil::interfaceName().contains("symbian")))
		{
        	if (mServiceBrowseDeprecatedNewService->isActive())
        	{
    			title = mServiceBrowseDeprecatedNewService->contextTitle();
        	}
        	else
        	{
    			title = mServiceBrowseDeprecatedOldService->contextTitle();
        	}
		}
    }
    
    return title;
}

// ----------------------------------------------------------------------------
// sortRole()
// ----------------------------------------------------------------------------
//
int VideoServices::sortRole() const
{
    MPX_ENTER_EXIT(_L("VideoServices::sortType()"));
    
    int sortRole = 0;

    if (mCurrentService == EBrowse)
    {
    	if ( mServiceBrowse && (XQServiceUtil::interfaceName().contains("symbian")))
		{
			sortRole = mServiceBrowse->sortRole();
		}
    	else if ( mServiceBrowseDeprecatedNewService && mServiceBrowseDeprecatedOldService && !(XQServiceUtil::interfaceName().contains("symbian")))
		{
        	if (mServiceBrowseDeprecatedNewService->isActive())
        	{
        		sortRole = mServiceBrowseDeprecatedNewService->sortRole();
        	}
        	else
        	{
        		sortRole = mServiceBrowseDeprecatedOldService->sortRole();
        	}
		}
    }
    
    return sortRole;
}

// ----------------------------------------------------------------------------
// itemSelected()
// ----------------------------------------------------------------------------
//
void VideoServices::itemSelected(const QString& item)
{
    MPX_ENTER_EXIT(_L("VideoServices::itemSelected()"),
                   _L("item = %s"), item.data() );
    
    QStringList list;
    list.append( item );

    if(mServiceUriFetch && (XQServiceUtil::interfaceName().contains("symbian")))
    {
    	mServiceUriFetch->complete( list );
    }
    else if (mServiceUriFetchDeprecatedNewService && mServiceUriFetchDeprecatedOldService)
    {
    	if (mServiceUriFetchDeprecatedNewService->isActive())
    	{
        	mServiceUriFetchDeprecatedNewService->complete( list );
    	}
    	else
    	{
        	mServiceUriFetchDeprecatedOldService->complete( list );
    	}
    }
    mFetchSelected = true;
}

// ----------------------------------------------------------------------------
// browsingEnded()
// ----------------------------------------------------------------------------
//
void VideoServices::browsingEnded()
{
    MPX_ENTER_EXIT(_L("VideoServices::browsingEnded()"));

	if ( mServiceBrowse && (XQServiceUtil::interfaceName().contains("symbian")))
	{
		mServiceBrowse->complete();
	}
	else if ( mServiceBrowseDeprecatedNewService && mServiceBrowseDeprecatedOldService && !(XQServiceUtil::interfaceName().contains("symbian")))
	{
    	if (mServiceBrowseDeprecatedNewService->isActive())
    	{
    		mServiceBrowseDeprecatedNewService->complete();
    	}
    	else
    	{
    		mServiceBrowseDeprecatedOldService->complete();
    	}
	}
}

// End of file
