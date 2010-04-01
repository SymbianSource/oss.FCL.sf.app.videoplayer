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
* Description: stub VideoListData class declaration*
*/

#include <mpxmediageneraldefs.h>
#include <mpxmedia.h>
#include "videodatacontainer.h"
#include "videocollectionutils.h"

/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}


// -----------------------------------------------------------------------------
// VideoDataContainer
// -----------------------------------------------------------------------------
//
VideoDataContainer::VideoDataContainer()
{
    // NOP
}
    
// -----------------------------------------------------------------------------
// VideoDataContainer
// -----------------------------------------------------------------------------
//
VideoDataContainer::~VideoDataContainer()
{
    clear();
}

// -----------------------------------------------------------------------------
// clear
// -----------------------------------------------------------------------------
//
void VideoDataContainer::clear()
{
    QMultiHash<TMPXItemId, QPair<int, CMPXMedia*> >::iterator i = mMediaData.begin();
    while(i != mMediaData.end())
    {
        delete (*i).second;
        ++i;
    }
    mMediaData.clear();
    mMediaIds.clear();
}

// -----------------------------------------------------------------------------
// remove
// -----------------------------------------------------------------------------
//
void VideoDataContainer::remove(const TMPXItemId &id)
{   
    QMultiHash<TMPXItemId, QPair<int, CMPXMedia*> >::iterator removeIter = mMediaData.find(id);
    if(removeIter == mMediaData.end())
    {
        return;
    }
    QMultiHash<TMPXItemId, QPair<int, CMPXMedia*> >::iterator hashIter;     
    mMediaIds.removeAt(removeIter->first);    
    // sync item indexes whose ids exist in id- list after
    // recently removoved. 
    int count = mMediaIds.count();
    for(int i = removeIter->first; i < count; ++i)
    {
        hashIter = mMediaData.find(mMediaIds[i]);
        if(hashIter != mMediaData.end())
        {
            hashIter->first--;
        }
    }
    delete removeIter->second;
    mMediaData.erase(removeIter);
}   

// -----------------------------------------------------------------------------
// append
// -----------------------------------------------------------------------------
//
void VideoDataContainer::append(CMPXMedia *media)
{
    TMPXItemId mediaId = TMPXItemId::InvalidId();
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>(media, KMPXMediaGeneralId, mediaId );

    if(mediaId == TMPXItemId::InvalidId())
    {
        // could not get id or id does not match ==> NOP
        return;       
    }
    // just append item even there are duplicates
    mMediaIds.append(mediaId);
    mMediaData.insert(mediaId, qMakePair( mMediaIds.count() - 1, media));     
}


// -----------------------------------------------------------------------------
// fromIndex
// -----------------------------------------------------------------------------
//
CMPXMedia* VideoDataContainer::fromIndex(int index) const
{   
    if(index >= 0 && index < mMediaIds.count() && mMediaData.contains(mMediaIds[index]))
    {   
        return (mMediaData.find(mMediaIds[index]))->second;
    }
    return 0;  
}
   
 
// -----------------------------------------------------------------------------
// indexOfId
// -----------------------------------------------------------------------------
//
int VideoDataContainer::indexOfId(const TMPXItemId &id) const
{
    QMultiHash<TMPXItemId, QPair<int, CMPXMedia*> >::const_iterator iter = mMediaData.find(id);
    if( iter != mMediaData.constEnd())
    {
        return iter->first;
    }
    
    return -1;
}

// -----------------------------------------------------------------------------
// idFromIndex
// -----------------------------------------------------------------------------
//
TMPXItemId VideoDataContainer::idFromIndex(int index) const
{
    if(index >= 0 && index < mMediaIds.count())
    {
        return mMediaIds[index];
    }
    return TMPXItemId::InvalidId();    
}

// -----------------------------------------------------------------------------
// count
// -----------------------------------------------------------------------------
//
int VideoDataContainer::count() const
{
    return mMediaData.count();
}

// end of gile
