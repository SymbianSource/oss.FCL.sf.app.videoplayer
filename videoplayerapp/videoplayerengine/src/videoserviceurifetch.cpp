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

#include <hbapplication.h>
#include <videoservices.h>
#include <videoserviceurifetch.h>

#include "mpxhbvideocommondefs.h"


// ----------------------------------------------------------------------------
// VideoServiceUriFetch()
// ----------------------------------------------------------------------------
//
VideoServiceUriFetch::VideoServiceUriFetch(VideoServices* parent)
: XQServiceProvider(QLatin1String("com.nokia.Videos.IVideoFetch"),parent),
  mRequestIndex(0),
  mServiceApp(parent)
{
	publishAll();
}

// ----------------------------------------------------------------------------
// ~VideoServiceUriFetch()
// ----------------------------------------------------------------------------
//
VideoServiceUriFetch::~VideoServiceUriFetch()
{

}

// ----------------------------------------------------------------------------
// fetchFailed()
// ----------------------------------------------------------------------------
//
void VideoServiceUriFetch::fetchFailed( int errorCode )
{
    QStringList filesList;
    filesList.insert(0, QString::number( errorCode ));//result
    doComplete(filesList);
}

// ----------------------------------------------------------------------------
// complete()
// ----------------------------------------------------------------------------
//
void VideoServiceUriFetch::complete( QStringList filesList )
{
        doComplete(filesList);
}

// ----------------------------------------------------------------------------
// doComplete()
// ----------------------------------------------------------------------------
//
void VideoServiceUriFetch::doComplete( QStringList filesList)
{
    if ( isActive() )
    {
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        completeRequest(mRequestIndex, filesList);
        mRequestIndex = 0;
    }
}

// ----------------------------------------------------------------------------
// isActive()
// ----------------------------------------------------------------------------
//
bool VideoServiceUriFetch::isActive()
{
    return (mRequestIndex > 0);
}

// ----------------------------------------------------------------------------
// VideoServiceUriFetch()
// ----------------------------------------------------------------------------
//
QString VideoServiceUriFetch::contextTitle() const
{
    return mTitle;
}

// ----------------------------------------------------------------------------
// fetch()
// ----------------------------------------------------------------------------
//
void VideoServiceUriFetch::fetch(const QString& title)
{
    mTitle = title;
    emit mServiceApp->activated(MpxHbVideoCommon::ActivateCollectionView);
    emit mServiceApp->titleReady(title);

    mServiceApp->setCurrentService(VideoServices::EUriFetcher);

    mRequestIndex = setCurrentRequestAsync();
}

