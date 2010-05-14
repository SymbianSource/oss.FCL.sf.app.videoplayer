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

// Version : %version: 6 %

#include "videoplayerengine.h"
#include "videoservices.h"
#include "videoserviceurifetch.h"
#include "videoserviceplay.h"
#include "videoserviceview.h"
#include "videoservicebrowse.h"
#include "mpxvideo_debug.h"

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
    
    mServiceUriFetch = new VideoServiceUriFetch(this);
    mServicePlay     = new VideoServicePlay(this, engine);
    mServiceView     = new VideoServiceView(this, engine);
    mServiceBrowse   = new VideoServiceBrowse(this);
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
    
    if ( mServiceBrowse )
    {
        category = mServiceBrowse->getBrowseCategory();
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
    
    if (mCurrentService == VideoServices::EUriFetcher && mServiceUriFetch)
    {
        title = mServiceUriFetch->contextTitle();
    }
    else if (mCurrentService == VideoServices::EBrowse && mServiceBrowse)
    {
        title = mServiceBrowse->contextTitle();
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
    
    if (mCurrentService == EBrowse && mServiceBrowse)
    {
        sortRole = mServiceBrowse->sortRole();
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
    mServiceUriFetch->complete( list );
    mFetchSelected = true;
}

// ----------------------------------------------------------------------------
// browsingEnded()
// ----------------------------------------------------------------------------
//
void VideoServices::browsingEnded()
{
    MPX_ENTER_EXIT(_L("VideoServices::browsingEnded()"));
	
    if ( mServiceBrowse )
    {
        mServiceBrowse->complete();
    }
}

// End of file
