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
* Description:  Implementation of VideoServiceUriFetch
*
*/

// Version : %version: %

#include <hbapplication.h>

#include "videoservices.h"
#include "videoserviceurifetch.h"
#include "mpxhbvideocommondefs.h"
#include "mpxvideo_debug.h"


// ----------------------------------------------------------------------------
// VideoServiceUriFetch()
// ----------------------------------------------------------------------------
//
VideoServiceUriFetch::VideoServiceUriFetch(VideoServices* parent)
    : XQServiceProvider( QLatin1String("com.nokia.Videos.IVideoFetch"), parent )
    , mRequestIndex( 0 )
    , mServiceApp( parent )
{
    MPX_ENTER_EXIT(_L("VideoServiceUriFetch::VideoServiceUriFetch()"));
	publishAll();
}

// ----------------------------------------------------------------------------
// ~VideoServiceUriFetch()
// ----------------------------------------------------------------------------
//
VideoServiceUriFetch::~VideoServiceUriFetch()
{
    MPX_DEBUG(_L("VideoServiceUriFetch::~VideoServiceUriFetch()"));
}

// ----------------------------------------------------------------------------
// fetchFailed()
// ----------------------------------------------------------------------------
//
void VideoServiceUriFetch::fetchFailed( int errorCode )
{
    MPX_ENTER_EXIT(_L("VideoServiceUriFetch::fetchFailed()"),
                   _L("errorCode = %d"), errorCode );
    
    QStringList filesList;
    filesList.insert( 0, QString::number( errorCode ) ); //result
    doComplete( filesList );
}

// ----------------------------------------------------------------------------
// complete()
// ----------------------------------------------------------------------------
//
void VideoServiceUriFetch::complete( QStringList filesList )
{
    MPX_ENTER_EXIT(_L("VideoServiceUriFetch::complete()"));
	
    doComplete( filesList );
}

// ----------------------------------------------------------------------------
// doComplete()
// ----------------------------------------------------------------------------
//
void VideoServiceUriFetch::doComplete( QStringList filesList)
{
    MPX_ENTER_EXIT(_L("VideoServiceUriFetch::doComplete()"));
        
    if ( isActive() )
    {
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        bool ok = completeRequest(mRequestIndex, filesList);
        mRequestIndex = 0;
        
		//
        // double check that request was completed succesfully, otherwise
        // videos application cannot be exited at all
		//
        if ( ! ok )
        {
            MPX_DEBUG(_L("VideoServiceUriFetch::doComplete() : completeRequest FAILED !"));  
			              
            qApp->quit();
        }
    }
}

// ----------------------------------------------------------------------------
// isActive()
// ----------------------------------------------------------------------------
//
bool VideoServiceUriFetch::isActive()
{
    MPX_DEBUG(_L("VideoServiceUriFetch::isActive() ret %d"), mRequestIndex );
	
    return (mRequestIndex > 0);
}

// ----------------------------------------------------------------------------
// VideoServiceUriFetch()
// ----------------------------------------------------------------------------
//
QString VideoServiceUriFetch::contextTitle() const
{
    MPX_DEBUG(_L("VideoServiceUriFetch::contextTitle() ret %s"), mTitle.data() );
	
    return mTitle;
}

// ----------------------------------------------------------------------------
// fetch()
// ----------------------------------------------------------------------------
//
void VideoServiceUriFetch::fetch(const QString& title)
{
    MPX_ENTER_EXIT(_L("VideoServiceUriFetch::fetch()"),
                   _L("title = %s"), title.data() );
    
    mTitle = title;
    emit mServiceApp->activated(MpxHbVideoCommon::ActivateCollectionView);
    emit mServiceApp->titleReady(title);

    mServiceApp->setCurrentService(VideoServices::EUriFetcher);

    mRequestIndex = setCurrentRequestAsync();
    MPX_DEBUG(_L("VideoServiceUriFetch::fetch() : mRequestIndex(%d)"), mRequestIndex );
}

