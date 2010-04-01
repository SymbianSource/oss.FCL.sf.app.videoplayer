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
* Description:   VideoListDataModelPrivate class definition*
*/

#ifndef __VIDEOLISTDATAMODEL_P_H__
#define __VIDEOLISTDATAMODEL_P_H__


// INCLUDES
#include <QObject>
#include <qicon.h>
#include <qdatetime.h>
#include <qset.h>
#include <qabstractitemmodel.h>
#include <e32const.h>
#include <mpxitemid.h>
#include "videodatasignalreceiver.h"
#include "videodatacontainer.h"

// FORWARD DECLARATIONS
class VideoListDataModel;
class CMPXMediaArray;
class VideoThumbnailData;

class VideoListDataModelPrivate : public VideoDataSignalReceiver
{    
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
	Q_DISABLE_COPY(VideoListDataModelPrivate) 

public: // Constructor

	/**
	 * Default constructor
     */
    VideoListDataModelPrivate(VideoListDataModel *model);	
	
	/**
     * Destructor
     */
	~VideoListDataModelPrivate();	
		
	/**
	 * Method initializes class members:
	 * Creates thumbnail data -object and connect it's signals.
	 * 
	 * - VideoThumbnailData: thumbnailsFetched
	 * ---> VideoListData: thumbnailsFetchedSlot
	 * 
	 * @return int, 0 if ok.
	 */
	int initialize();

signals:

    /**
     * This signal is emitted, when data to some video item changes.
     * Like for example new thumbnail is fetched.
     * 
     * @param startIndex
     * @param endIndex
     * 
     */
    void dataChanged(const QModelIndex &startIndex, const QModelIndex &endIndex);

    /**
     * This signal is emitted, collection notifies details for item has been
     * fetched
     * 
     * @param rowindex
     * 
     */
    void videoDetailsReady(int rowIndex);

private slots:

    /**
     * This slot is connected to VideoThumbnailData::thumbnailsFetched -signal
     * to get notified when thumbnail(s) for some particular item(s) is ready.
     * 
     * The indexes of the items are checked and videoDataChanged signal is emitted.
     * 
     * @param mediaIds: media ids of the items  
     * 
     */
    void thumbnailsFetchedSlot(QList<TMPXItemId> mediaIds);
    
private slots: //slots from VideoDataSignalReceiver    
    
    /**
     * Signaled by the collection client when there is new video list available.
     * Method emits videosInserted -signal.
     * 
     * @param newVideoList: new list of videos
     * 
     */
    void newVideoListSlot(CMPXMediaArray *newVideoList);
      
    /**
     * Signaled by the collection client when there are new videos appended
     * into the existing videolist. 
     * Checks if medias at provided array does not exist at the existing
     * array and appends them if they do not.
     * 
     * @param videoList: list of videos provided for appending
     */
    void appendVideoListSlot(CMPXMediaArray* videoList);
        
    /**
     * Signaled by the collection client when new videodata is fetched.
     * Given object is inserted into correct position in the 
     * media array and videosInserted -signal is emitted
     * 
     * @param aVideo data of the new video.
     */
    void newVideoAvailableSlot(CMPXMedia *newVideo);
    
    /**
     * Signaled by the collection client when
     * video removal has detected.
     * 
     * @param videoId id of the removed video
     */
    void videoDeletedSlot(TMPXItemId videoId);
    
    /**
     * Signaled by the collection client video deletion request
     * has completed
     * 
     * @param overallCount count of ALL videos in this delete procedure
     * @param failedMediaIds list of item ids that failed to be removed.
     */
    void videoDeleteCompletedSlot( int overallCount, QList<TMPXItemId> *failedMediaIds);
     
    /**
     * Signaled by the collection client when video details fetching
     * has completed.
     * 
     */
    void videoDetailsCompletedSlot(TMPXItemId videoId);      
    
public: // services 
    
    /**
     * Returns video count
     * 
     * @return int count of videos
     */
    int getVideoCount();
    
    /**
     * Returns the MPX media item id (TMPXItemId.id1)
     * from the given index.
     * 
     * @param index: item position where client wants the id from 
     * 
     * @return TMPXItemId item id or invalid TMPXItemId if id not found at that index
     */
    TMPXItemId getMediaIdFromIndex(int index) const;
        
    /**
     * Returns name of the video from the given index.
     * If there is no item at that index, empty string is returned.
     * 
     * @param index: item position where client wants the name from 
     * 
     * @return QString name of the item
     */
    const QString getVideoNameFromIndex(int index)  const;
      
    /**
     * Returns the thumbnail of the item from given index.
     * If there is no item at that index or thumbnail is not yet fetched,
     * default thumbnail is returned.
     * 
     * @param index: item position where client wants the tn from 
     * 
     * @return QIcon thumbnail of the item
     */
    const QIcon* getVideoThumbnailFromIndex(int index)  const;
    
    /**
     * Returns the size of the video from given index.
     * If there is no item at that index, 0 is returned
     * 
     * @param index: item position where client wants the size from 
     * 
     * @return guint32 size in bytes
     */
    quint32 getVideoSizeFromIndex(int index) const;
    
    /**
     * Returns the age profile of the video from given index
     * If there is no item at that index, 0 is returned
     * 
     * @param index: item position where client wants the size from 
     * 
     * @return quint32 age profile in bytes
     */
    quint32 getVideoAgeProfileFromIndex(int index) const;
    
    /**
     * Returns the duration of the video from given index.
     * If there is no item at that index, 0 is returned
     * 
     * @param index: item position where client wants the duration from 
     * 
     * @return guint32 duration
     */
    quint32 getVideodurationFromIndex(int index) const;
    
    /**
     * Returns the creation/download date of the video.
     * 
     * @param index: item position where client wants the date from.
     * 
     * @return QDate date
     */
    QDateTime getVideoDateFromIndex(int index) const;
    
    /**
     * Returns the metadata for the video.
     * 
     * @param index: item position where client wants the data from.
     * 
     * @return QMap map of the QVariants that hold the data. Keys defined 
     *      in VideoCollectionCommon
     */
    QMap<QString, QVariant> getMetaDataFromIndex(int index) const;
    
    /**
     * Returns video status
     * 
     * @param index: item position where client wants the status from.
     * 
     * @return int status code
     */
    int getVideoStatusFromIndex(int index) const;
    
    /**
     * marks videos to be removed: it's id and index are saved to 
     * mItemsUnderDeletion
     * 
     * @param itemIndexes: indexes of items to be removed
     * @return QList: list of ids about removed items
     *
     */
    QList<TMPXItemId> markVideosRemoved(const QModelIndexList &itemIndexes);
    
    /**
     * Removes provided ids from the remove -list
     * 
     * @param itemIds ids of the items to be removed
     */
    void unMarkVideosRemoved(QList<TMPXItemId> &itemIds);
	
	/**
     * Returns the file path of the video.
     * 
     * @param index: item position where client wants the file path from.
     */
    const QString getFilePathFromIndex(int index) const;
    
	/**
     * Returns the file path of the video.
     * 
     * @param mediaId: id for the item
     */
    const QString getFilePathForId(TMPXItemId mediaId) const;
    
private: // private methods
  
    /**
     * private default constructor definition
     */
    VideoListDataModelPrivate();
    
    /**
     * Method clears map containing media objects
     */
    void clearVideoData();
    
    /**
     * Method used to parse media id from given object.
     * Id id data is not available or given object is null.
     * method returns -1. 
     * The id is CMPXMedia's TMPXItemId.iId1
     * 
     * @param media: where to get id from 
     * 
     * @return TMPXItemId: id of the item
     */
    TMPXItemId getMediaId(const CMPXMedia *media ) const;
    
    /**
     * Leaving helper method for appending data from media array into 
     * data container for reducing amount of TRAPs.
     * 
     * Leaves if CMPXMedia object creation fails.
     * 
     * @param videoArray array from where to take data
     * @param startIndex startindex from where start appending.
     *  
     */
    void appendDataToContainerL(CMPXMediaArray *videoArray, unsigned int startIndex = 0);
      
private:
    
    /**
     * Pointer to the public class
     * Not own.
     */
    VideoListDataModel *q_ptr;
    
   /**
    * media object container
    */
    VideoDataContainer mMediaData;  
      
    /**
    * set containing item(s)' ids whose, removal 
    * is underway. 
    */
    QSet<TMPXItemId>      mItemsUnderDeletion;  
    
    /**
     * Thumbnail handler object.
     */
   VideoThumbnailData &mVideoThumbnailData;
   
	/**
	 * Object is initialized.
	 */
	bool mInitialized;
   
};

#endif  // __VIDEOLISTDATAMODEL_P_H__

// End of file
