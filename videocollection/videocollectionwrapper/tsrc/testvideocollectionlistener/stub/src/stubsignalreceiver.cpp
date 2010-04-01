/**
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
* Description:   stub signal receiver for testing CVideoCollectionListener
* 
*/

#include "stubsignalreceiver.h"

// -----------------------------------------------------------------------------
// StubSignalReceiver
// -----------------------------------------------------------------------------
//
StubSignalReceiver::StubSignalReceiver() :
mLatestPtr(0),
mLatestItemId(TMPXItemId::InvalidId()),
mLatestInteger(-1)
{
    
}

// -----------------------------------------------------------------------------
// ~StubSignalReceiver
// -----------------------------------------------------------------------------
//
StubSignalReceiver::~StubSignalReceiver()
{
    
}
 
// -----------------------------------------------------------------------------
// resetLatestItems
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::resetLatestItems()
{
    mLatestPtr = 0;
    mLatestItemId = TMPXItemId::InvalidId();
    mLatesListData.clear();
    mLatestInteger = -1;
}

// -----------------------------------------------------------------------------
// getLatestPointerAddr
// -----------------------------------------------------------------------------
//
void* StubSignalReceiver::getLatestPointerAddr()
{
    return mLatestPtr;
}
 
// -----------------------------------------------------------------------------
// getLatestInt
// -----------------------------------------------------------------------------
//
TMPXItemId StubSignalReceiver::getLatestItemId()
{
    return mLatestItemId;
}

// -----------------------------------------------------------------------------
// getLatestListData
// -----------------------------------------------------------------------------
//
QList<TMPXItemId>& StubSignalReceiver::getLatestListData()
{
    return mLatesListData;
}

// -----------------------------------------------------------------------------
// getLatestListData
// -----------------------------------------------------------------------------
//
int StubSignalReceiver::getLatestIntegerData()
{
    return mLatestInteger;
}

// -----------------------------------------------------------------------------
// newVideoListSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::newVideoListSlot(CMPXMediaArray* aVideoList)
{
    mLatestPtr = aVideoList;
}

// -----------------------------------------------------------------------------
// videoListAppendedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoListAppendedSlot(CMPXMediaArray* aVideoList)
{
    mLatestPtr = aVideoList;
}
     
  
// -----------------------------------------------------------------------------
// newVideoAvailableSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::newVideoAvailableSlot(CMPXMedia* aVideo)
{
    mLatestPtr = aVideo;
}
 
// -----------------------------------------------------------------------------
// videoDeletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoDeletedSlot(TMPXItemId videoId)
{
    mLatestItemId = videoId;
}
  
// -----------------------------------------------------------------------------
// videoDeleteCompletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoDeleteCompletedSlot(int count, QList<TMPXItemId> *failedMediaIds)
{
    mLatestInteger = count;
    mLatestPtr = failedMediaIds;
    
    if(!failedMediaIds)
    {
        return;
    }
    mLatesListData.clear();
    TMPXItemId data = 0;
    foreach(data, (*failedMediaIds))
    {
        mLatesListData.append(data);
    }
}

// -----------------------------------------------------------------------------
// videoDetailsCompletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoDetailsCompletedSlot(TMPXItemId videoId)
{
    mLatestItemId = videoId;
}
