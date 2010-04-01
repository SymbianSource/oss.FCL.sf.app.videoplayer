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
* Description: stub videodatacontainer class definition*
*/

#ifndef __VIDEODATACONTAINER_H__
#define __VIDEODATACONTAINER_H__

// INCLUDES
#include <qhash.h>
#include <qlist.h>
#include <qpair.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS
class CMPXMedia;

class VideoDataContainer
{
public:
    
    /**
     * contructor
     */
    VideoDataContainer();
    
    /**
     * destructor
     */
    virtual ~VideoDataContainer();
    
public:  // from QHash
    
    /**
     * Deallocates all CMPXMedia objects and clear containers
     */
    void clear();

    /**
     * Deallocates and removes item of provided id.
     * 
     * @param id media item id
     */
    void remove(const TMPXItemId &id);
     
    /**
     * Appends provided media object to container.
     * If item with same id exists allready, old item is being
     * removed and replaced by the new item
     * 
     * @param media - media object to append
     */
    void append(CMPXMedia *media);

    /**
     * Returns a media object from provided index
     * 
     * @param index position of wanted object
     * 
     * @return CMPXMedia pointer to media object or null if 
     *                   object is not found from wanted index
     * 
     */
    CMPXMedia* fromIndex(int index) const;
  
    /**
     * Returns an index of id.
     * 
     * @param id item id
     * 
     * @return int index of item or -1 if item with provided id is not found
     */
    int indexOfId(const TMPXItemId &id) const;

    /**
     * Returns id of item from provided index
     * 
     * @param index potisiotn where to look for item
     * 
     * @return id of item or invalid TMPXItemId if item is not found from provided index
     */
    TMPXItemId idFromIndex(int index) const;

    /**
     * returns count of items
     * 
     * @return int 
     */
    int count() const;

private: // data
    
    /**
     * list of media ids used to fetch item thought index.
     */
    QList<TMPXItemId> mMediaIds;
    
    /**
     * lookup hash for fetching correct item based on id.
     * Multihash needed in stub to be able to make sure appending 
     * works ok
     * Key: item id
     * value: pair, where first is item index and second is item data
     */
    QMultiHash<TMPXItemId, QPair<int, CMPXMedia*> > mMediaData;

};

#endif  // __VIDEODATACONTAINER_H__


    


