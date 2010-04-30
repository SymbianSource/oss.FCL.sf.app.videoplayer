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

// Version : %version: 3 %

#include <hbapplication.h>

#include "videoservices.h"
#include "videoservicebrowse.h"
#include "mpxhbvideocommondefs.h"
#include "mpxvideo_debug.h"

// -------------------------------------------------------------------------------------------------
// VideoServiceBrowse()
// -------------------------------------------------------------------------------------------------
//
VideoServiceBrowse::VideoServiceBrowse( VideoServices* parent )
    : XQServiceProvider( QLatin1String("com.nokia.Videos.IVideoBrowse"), parent )
    , mRequestIndex( 0 )
    , mServiceApp( parent )
    , mCategory( 0 )
    , mTitle( "" )
    , mSortRole( 0 )
{
    MPX_ENTER_EXIT(_L("VideoServiceBrowse::VideoServiceBrowse()"));

    publishAll();
}

// -------------------------------------------------------------------------------------------------
// ~VideoServiceBrowse()
// -------------------------------------------------------------------------------------------------
//
VideoServiceBrowse::~VideoServiceBrowse()
{
    MPX_DEBUG(_L("VideoServiceBrowse::~VideoServiceBrowse()"));
}

// -------------------------------------------------------------------------------------------------
// complete()
// -------------------------------------------------------------------------------------------------
//
void VideoServiceBrowse::complete()
{
    MPX_ENTER_EXIT(_L("VideoServiceBrowse::complete()"));	

    if ( mRequestIndex )
    {
        QStringList filesList;
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        bool ok = completeRequest(mRequestIndex, filesList);
        mRequestIndex = 0;
        
        //
        // double check that request was completed succesfully, otherwise
        // videos application cannot be exited at all
        //
        if ( ! ok )
        {
            MPX_DEBUG(_L("VideoServiceBrowse::complete() : completeRequest FAILED !"));
            qApp->quit();
        }
    }
}

// -------------------------------------------------------------------------------------------------
// getBrowseCategory()
// -------------------------------------------------------------------------------------------------
//
int VideoServiceBrowse::getBrowseCategory() const
{
    MPX_DEBUG(_L("VideoServiceBrowse::getBrowseCategory() ret %d"), mCategory);
    return mCategory;
}

// -------------------------------------------------------------------------------------------------
// contextTitle()
// -------------------------------------------------------------------------------------------------
//
QString VideoServiceBrowse::contextTitle() const
{
    MPX_DEBUG(_L("VideoServiceBrowse::contextTitle() ret %s"), mTitle.data() );
    return mTitle;
}

// -------------------------------------------------------------------------------------------------
// sortRole()
// -------------------------------------------------------------------------------------------------
//
int VideoServiceBrowse::sortRole() const
{
    MPX_DEBUG(_L("VideoServiceBrowse::getBrowseCategory() ret %d"), mSortRole);
    return mSortRole;
}

// -------------------------------------------------------------------------------------------------
// browseVideos()
// -------------------------------------------------------------------------------------------------
//
void VideoServiceBrowse::browseVideos(const QString &title,
    int category,
    int sortRole)
{    
    MPX_ENTER_EXIT(_L("VideoServiceBrowse::browseVideos()"));	

    // set application title
    QString appTitle(title);
    if (appTitle.isEmpty())
    {
        appTitle = hbTrId("txt_videos_title_videos");
    }
    
    mTitle = appTitle;
    mCategory = category;
    mSortRole = sortRole;

    // start service
    mServiceApp->setCurrentService(VideoServices::EBrowse);
    emit mServiceApp->titleReady(appTitle);
    emit mServiceApp->activated(MpxHbVideoCommon::ActivateCollectionView);

    // store async request id
    mRequestIndex = setCurrentRequestAsync();
    MPX_DEBUG(_L("VideoServiceBrowse::browseVideos() : mRequestIndex = %d"), mRequestIndex );
}

// End of file
