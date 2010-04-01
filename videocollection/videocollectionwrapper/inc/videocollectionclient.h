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
* Description:   VideoCollectionClient class definition*
*/

#ifndef __VIDEOCOLLECTIONCLIENT_H
#define __VIDEOCOLLECTIONCLIENT_H

#include <vcxmyvideosdefs.h>
#include <QList>

// FORWARD DECLARATIONS

class MMPXCollectionUtility;
class VideoDataSignalReceiver;
class VideoCollectionListener;


// CLASS DECLARATION

/**
 * Client class for My Videos MPX Collection.
 *
 * @lib videocollectionwrapper.dll
 */
class VideoCollectionClient 
{
        
public: 
    
    /**
     * Constructor
     */
    VideoCollectionClient();

    /**
     * Destructor.
     */
    virtual ~VideoCollectionClient();
    
    /**
     * Initializes object and allocates members
     * 
     * @return 0 if ok, < 0 in case of error
     */
    int initialize();

public:
   
    /**
     * Collection level app browsed to
     */
    enum TCollectionLevels
    {
        ELevelInvalid = -1,
    	ELevelCategory = 2,
        ELevelVideos   = 3
    };
        
    /**
     * Collection opening status
     */
    enum TCollectionOpenStatus
    {        
        ECollectionNotOpen = 0,
        ECollectionOpening,
        ECollectionOpened            
    };  
        


    /**
     * Connects videodata signals to provided object
     * If method fails, client should deallocate it's 
     * VideoDataSignalReceiver object to make sure thee are
     * no unused signal connections.
     * 
     * @param aVideoModelObserver Observer.
     * 
     * @return int -1 in case of failure
     */
    int connectCollectionSignalReceiver(
            VideoDataSignalReceiver *signalReceiver);
    
    /**
     * Returns the current level of collection.
     * 
     * @return some type of TCollectionLevels or -1 in case of error
     */
    int getCollectionLevel();
    
    /**
     * Returns the current category
     * 
     * @return int
     */
    void getCategoryIds(int& id, int& type);

    /**
     * Returns reference to collection open status. Client can use
     * this to get and change the status
     * 
     * @return some type of TCollectionLevels or -1 in case of error
     */
    int getOpenStatus();
    
    /**
     * Sets open status, if given status is ECollectionOpened calls 
     * startOpenCurrentState to start collection into current state. 
     * 
     */
    void setOpenStatus(int status);
        
    /**
     * Starts opening of collection to the all videos category
     * 
     * @return 0 startup ok
     */
    int startOpenCollection(int level);     
    
    /**
     * Calls open to collection to get current
     * state to be opened. 
     * 
     * @return 0 startup ok
     */
    int startOpenCurrentState();
    
    /** 
     * calls collection to deleteFile(s) mpx media object defined by the given ids
     *
     * @param mediaIds pointerr to the list of items to be removed
     * @return 0 if ok
     */
    int deleteVideos(QList<TMPXItemId> *mediaIds);
    
    /**
     * calls collection to open video object defined by the given id
     * 
     * @param aMediaId id of the item
     * @return KErrNone if ok  
     */
    int openVideo(TMPXItemId &mediaId);

    /**
     * calls collection to go back to collection level
     * 
     */
    int back();

    /**
     * Fetches MPX Media object based on MPX ID. Object can be requested later
     * with method GetLatestFetchedMpxMediaL(). Note, there is no indication
     * when the object becomes available.
     * 
     * @param aMpxId MPX ID.
     * 
     * @return 0 media fetchingstarted ok 
     */
    int fetchMpxMediaByMpxId(TMPXItemId &aMpxId);    
    
    /**
     * Gets all details for the selected MPX Media object.
     * Result will be returned in CVideoCollectionListener::HandleCollectionMediaL().
     * 
     * @param aMediaId meida id of the item (TMPXItemId.iId1) 
     * 
     * @return 0 if detail fetching started ok
     */
    int getVideoDetails(TMPXItemId& aMediaId);
    
    /**
     * Creates a new collection (album).
     * 
     * @param name Name for the collection.
     * @param thumbnail Path for the image file to use as a thumbnail for the
     *      collection. Can be empty.
     * @param mediaIds Media ids of the videos that are added to the collection 
     *      at creation time. Size can be zero.
     * @return 0, if collection creation was success, below 0 if there was an error.
     */
    int addNewCollection(QString name, QString thumbnail, QList<TMPXItemId> mediaIds);

private:
    
    /**
     * Private implementation to handle leaving code of collection opening.
     * 
     */
    void startOpenCollectionL(int level);
    
    /**
     * Private implementation to handle leaving code of video deletion.
     * 
     * @param mediaIds list of ides to remove
     */
    void deleteVideosL(QList<TMPXItemId> &mediaIds);
    
    /**
     * Private implementation to handle leaving code of video opening.
     * 
     * @param videoId id of the video to open
     */
    void openVideoL(TMPXItemId &videoId);

    /**
     * Private implementation to handle leaving code of collection opening.
     * 
     * @param id id of the collection to open
     */
    void openCategoryL(TMPXItemId &id);

    /**
     * Private implementation to handle leaving code of collection back stepping.
     * 
     */
    void backL();

    /**
     * Private implementation to handle leaving code while starting to get video 
     * details
     * 
     * @param videoId id of the video to get details from 
     */
    void getVideoDetailsL(TMPXItemId &videoId);
    
    /**
     * Creates a new collection (album).
     * 
     * @param name Name for the collection.
     * @param mediaIds Media ids of the videos that are added to the collection 
     *      at creation time. Size can be zero.
     */
    void addNewCollectionL(QString name, QString thumbnail, QList<TMPXItemId> mediaIds);
    
    /**
     * Private implementation to handle leaving code while starting to fetch video 
     * object from collection
     * 
     * @param mpxId mpxif for the video object to fetch from collection
     */
    void fetchMpxMediaByMpxIdL(TMPXItemId &mpxId);    
    
private:
    

    /**
     * Pointer to MPX Collection utility.
     * Own.
     */
    MMPXCollectionUtility* mCollectionUtility;
    
    /**
     * Variable for storing My Videos collection opening status.
     */
    int mCollectionOpenStatus;     

    /**
     * Collectionlistener
     */
    VideoCollectionListener *mCollectionListener;
    
    /**
     * Variable for storing opened My Videos category and album ids.
     */
	TMPXItemId mOpenCategoryAlbum;

    /**
     * Variable for storing My Videos collection path level.
     */
    TCollectionLevels mCollectionPathLevel;
 };

#endif // __VIDEOCOLLECTIONCLIENT_H
