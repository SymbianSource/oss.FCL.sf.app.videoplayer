/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stub collection client to be used when unit testing videocollectionsortfilterproxy 
*/



// INCLUDE FILES>
#include <mpxmediaarray.h>
#include "stub/inc/videocollectionclient.h"

VideoCollectionClient* VideoCollectionClient::testInstance = 0;

bool VideoCollectionClient::mFailInit = false;
bool VideoCollectionClient::mFailStartOpen = false;
bool VideoCollectionClient::mFailMediaPlayback = false;
bool VideoCollectionClient::mFailMediaDetails = false;
bool VideoCollectionClient::mFailSetSort = false;
bool VideoCollectionClient::mFailAddNewCollection = false;
TVcxMyVideosSortingOrder VideoCollectionClient::mSettedSortOrder = EVcxMyVideosSortingNone;
int VideoCollectionClient::mSortOrderSetCount = 0;
QString VideoCollectionClient::mAddNewCollectionName = QString();
QString VideoCollectionClient::mAddNewCollectionThumb = QString();
QList<TMPXItemId> VideoCollectionClient::mAddNewCollectionIds = QList<TMPXItemId>();

// -----------------------------------------------------------------------------
// VideoCollectionClient
// -----------------------------------------------------------------------------
//
VideoCollectionClient::VideoCollectionClient()
{
    testInstance = this;
}

// -----------------------------------------------------------------------------
// ~CVideoCollectionClient
// -----------------------------------------------------------------------------
//
VideoCollectionClient::~VideoCollectionClient()
{
}

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::initialize()
{
    if(mFailInit)
    {
        return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// startOpenCollection
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::startOpenCollection(int level)
{
    if(mFailStartOpen)
    {
        return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// openVideo
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::openVideo(int /*mpxId1*/)
{
    if(mFailMediaPlayback)
    {
        return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// deleteFile
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::getVideoDetails(int /*mpxId1*/)
{
    if(mFailMediaDetails)
    {
        return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// addNewCollection
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::addNewCollection(QString name, QString thumbnail, QList<TMPXItemId> mediaIds)
{
    mAddNewCollectionName = name;
    mAddNewCollectionThumb = thumbnail;
    mAddNewCollectionIds = mediaIds;
    if(mFailAddNewCollection) {
        return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// back
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::back()
{
    return 0;
}

