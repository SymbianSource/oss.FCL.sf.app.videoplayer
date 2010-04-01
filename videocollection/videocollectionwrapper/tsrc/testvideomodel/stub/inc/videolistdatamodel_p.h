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
* Description:  stubclass of videolistdata for testing VideoListDataModel class methods*
*/


#ifndef __VIDEOLISTDATA_H__
#define __VIDEOLISTDATA_H__


// INCLUDES
#include <QObject>
#include <QDate>
#include <QIcon>
#include <QSet>
#include <qabstractitemmodel.h>
#include "videodatasignalreceiver.h"


// FORWARD DECLARATIONS
class CMPXMediaArray;
class VideoThumbnailData;


class VideoListDataModel;
class CMPXMediaArray;

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

public:

    /**
     * Default constructor
     */
    VideoListDataModelPrivate(VideoListDataModel *model); 
    
    /**
     * Destructor
     */
    ~VideoListDataModelPrivate();  
	
	/**
	 * Set global init failure flag value
	 */
	static void setInitFailureStatus(bool fail);

	/**
	 * if override is true returns given size when 
	 * getvideosizeFromIndex is called
	 * 
	 * @param override if true value is overridden
	 * @value value to override
	 */
	static void overrideSizeValue(bool override, quint32 value);
	
	/**
     * if override is true returns given size when 
     * getvideoDurationFromIndex is called
     * 
     * @param override if true value is overridden
     * @param value value to override
     */
	static void overrideDurationValue(bool override, float value);
	
	/**
     * Set counter which is decreased after every getVideoCount -call
     * When counter reaches zero, getVideoCount -call returns 0 
     * 
     * @param override if true value is overridden
     * @param value value to override
     */
    static void setGetVideoCountFailAfterNCall(int counter);
    
 

	/**
     * returns 0 or -1 based on global init failure flag
     * if flag is true, return 0
     * 
     * @return int 0 or -1
     */
	int initialize();
	

signals:

    /**
     * Not emitted from here but had to be defined in order to 
     * get stub into use to testable object 
     */
    void dataChanged(const QModelIndex &startIndex, const QModelIndex &endIndex);

    /**
     * Not emitted from here but had to be defined in order to 
     * get stub into use to testable object 
     */
    void videoDetailsReady(int index);

    
public: // services

    /**
     * return count of mMediaArray
     * 
     * @rerurn int
     */
    int getVideoCount();
   
    /**
     * return video id of the item at given index
     * 
     * @param index
     * @return int
     */
    int getMediaIdFromIndex(int index) const;
    
    /**
    * return video path of the item with given media id
    * 
    * @param mediaId
    * @return int
    */
    const QString getFilePathForId(TMPXItemId mediaId) const;
           
    /**
     * return name of the item at given index
     * 
     * @param index
     * @return QString
     */
    const QString getVideoNameFromIndex( int index )  const;
   
    /**
     * return default tn always
     * 
     * @param index
     * @return QIcon
     */
    const QIcon* getVideoThumbnailFromIndex( int index ) const;
   
    /**
     * return video size of the item at given index
     * 
     * @param index
     * @return quint32
     */
    quint32 getVideoSizeFromIndex( int index ) const;
   
    /**
     * return video age profile of the item at given index
     * 
     * @param index
     * @return quint32
     */
    quint32 getVideoAgeProfileFromIndex( int index ) const;
   
    /**
     * return video duration of the item at given index
     * 
     * @param index
     * @return quint32
     */
    quint32 getVideodurationFromIndex( int index ) const;
   
    /**
     * return video creation datetime of the item at given index
     * 
     * @param index
     * @return QDateTime
     */
    QDateTime getVideoDateFromIndex( int index ) const;
   
    /**
     * Returns the metadata for the video.
     * 
     * @param index: item position where client wants the date from.
     * @return QMap map of the QVariants that hold the data. Keys defined 
     *      in VideoCollectionCommon
     */
    QMap<QString, QVariant> getMetaDataFromIndex(int index) const;
      
    /**
     * Returns video status
     * 
     * @param index: item position where client wants the date from.
     * 
     * @return int status code
     */
    int getVideoStatusFromIndex(int index) const;
    
    /**
     * marks video to be removed: it's id and index are saved to 
     * mItemsUnderDeletion
     * 
     * @param indexes: indexes of videos to be removes
     *
     */
    QList<TMPXItemId> markVideosRemoved(const QModelIndexList &indexes);

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
   
private: // private methods
    
    /**
     * return id of given item
     * 
     * @param CMPXMedia
     * @return TMPXItemId
     */
    TMPXItemId getMediaId(const CMPXMedia *media ) const;
    
    /**
     * return item from given index of given item
     * 
     * @param index
     * @return CMPXMedia
     */
    CMPXMedia* getMediaFromIndex( int index ) const;
 
    /**
     * return index of item of given id
     * 
     * @param mediaId
     * @return int
     */
    int indexOfMediaId(TMPXItemId mediaId) const;
    
      
public slots: 
    
    /**
    * ssaves given list to mMediaArray
    * 
    * @param aVideoList: video list
    * 
    */
    void newVideoListSlot(CMPXMediaArray* aVideoList);
    
    /**
     * not used in stub
     * 
     */
    void appendVideoListSlot(CMPXMediaArray * videoList); 
    
    /**
     * not used in stub
     * 
     */
    void newVideoAvailableSlot(CMPXMedia* aVideo);

    /**
     * not used in stub
     * 
     */
    void videoDeletedSlot(TMPXItemId videoId);
           
    /**
     * used to clear mItemsUnderDeletion for this stub
     * 
     */
    void videoDeleteCompletedSlot(int overallCount,QList<TMPXItemId> *failedMediaIds);
    
    /**
     * not used in stub
     * 
     */
    void videoDetailsCompletedSlot(TMPXItemId videoId);
     
private:
    
    /**
    * Pointer to the public class
    * Not own.
    */
    VideoListDataModel *q_ptr;
 
    /**
     * pointer to shared-array of medias
     */
    CMPXMediaArray *mMediaArray;
    
    /**
     * default icon
     */
    QIcon          mIcon; 
    
    /**
     * marked as removed -map
     */
    QSet<TMPXItemId> mItemsUnderDeletion;
    
};
#endif  // __VIDEOLISTDATA_H__

// End of file
    


