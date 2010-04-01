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
* Description:   Stub VideoCollectionClient class for VideoCollectionWrapperPrivate unit tests  
*/



// INCLUDE FILES
#include <mpxmediaarray.h>
#include "stub/inc/videocollectionclient.h"


VideoCollectionClient* VideoCollectionClient::testInstance = 0;

bool gFailInit = false;
bool gFailStartOpen = false;
bool gFailMediaPlayback = false;
bool gFailMediaDetails = false;
bool gFailSetSort = false;


TVcxMyVideosSortingOrder gSettedSortOrder = EVcxMyVideosSortingNone;


// -----------------------------------------------------------------------------
// setInitFailure
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::setInitFailure(bool fails)
{
    gFailInit = fails;
}

// -----------------------------------------------------------------------------
// setStartopenFailure
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::setStartopenFailure(bool fails)
{
    gFailStartOpen = fails;
}

// -----------------------------------------------------------------------------
// setOpenMediaplaybackFailure
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::setOpenMediaplaybackFailure(bool fails)
{
    gFailMediaPlayback = fails;
}

// -----------------------------------------------------------------------------
// setOpenMediaDetailsFailure
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::setOpenMediaDetailsFailure(bool fails)
{
    gFailMediaDetails = fails;
}

// -----------------------------------------------------------------------------
// setSortMediasFailure
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::setSortMediasFailure(bool fails)
{
    gFailSetSort = fails;
}

// -----------------------------------------------------------------------------
// getSettedSortOrder
// -----------------------------------------------------------------------------
//
TVcxMyVideosSortingOrder VideoCollectionClient::getSettedSortOrder()
{
    return gSettedSortOrder;
}

// -----------------------------------------------------------------------------
// VideoCollectionClient
// -----------------------------------------------------------------------------
//
VideoCollectionClient::VideoCollectionClient()
{
    testInstance = this;
}

// -----------------------------------------------------------------------------
// ~VideoCollectionClient
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
    if(gFailInit)
    {
        return -1;
    }
    return 0;
}


int VideoCollectionClient::startOpenCollection(int level)
{
    if(gFailStartOpen)
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
    if(gFailMediaPlayback)
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
    if(gFailMediaDetails)
    {
        return -1;
    }
    return 0;
}
