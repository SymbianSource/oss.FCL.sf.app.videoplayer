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
* Description:   CVideoThumbnailData class definition*
*/

#ifndef __VIDEOTHUMBNAILDATA_H__
#define __VIDEOTHUMBNAILDATA_H__

// INCLUDES
#include <QObject>
#include <QHash>
#include <QPair>
#include <QIcon>
#include <QSet>

// FORWARD DECLARATIONS

class VideoThumbnailData : public QObject
{
    /**
     * defined to be able to use signals and slots
     */
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
	Q_DISABLE_COPY(VideoThumbnailData) 

public: // Constructor

    /* 
     * Thumbnail fetch priorities
     */
    enum VideoThumbnailPriority
    {
        VideoThumbnailPriorityBackground = 2000,
        VideoThumbnailPriorityLow = 5000,
        VideoThumbnailPriorityMedium = 10000,
        VideoThumbnailPriorityHigh = 2000 
    };
    
    /**
     * Returns singleton instance for this class.
     *
     * @return The singleton instance.
     * 
     */
    static VideoThumbnailData &instance();
 	
    /**
     * Starts fetching thumbnails for medias in the model defined by the indexes parameter.
     * Priority for the thumbnails is ascending starting from the defined priority. Uses method 
     * startFetchingThumbnail for the actual fetching.
     * 
     * Clears all other thumbnail fetches.
     * 
     * param @indexes indexes of the medias in the model
     * param @priority starting priority for the thumbnails
     * param @setFetchIndex whether index for background thumbnail fetcing is set 
     *   
     * @return int count of fetches started or -1 in case of error. 
     */
    int startFetchingThumbnails(const QList<int> &indexes, VideoThumbnailPriority priority, bool setFetchIndex = true);
    
    /**
     * Starts background thumbnail fetching.
     *
     * @param fetchIndex index where to start the background thumbnail fetching.
     *  
     */
    void startBackgroundFetching(int fetchIndex);
    
    /**
     * Enables or disables thumbnail background fetching. Default is enabled.
     * 
     * @param enable true enables and false disables thumbnail background fetching.
     */
    void enableBackgroundFetching(bool enable);
    
    /**
     * Frees allocated data for thumbnails and cancels ongoing fetches.
     */
    void freeThumbnailData();
    
private:
    
    /**
     * Default constructor
     */
    VideoThumbnailData();  
    
    /**
     * Destructor
     */
    ~VideoThumbnailData(); 
    
public:
    static int mStartFetchingThumbnailsCallCount;
    static int mStartFetchingThumbnailsThumbnailCount;
    static int mInstanceCallCount;
    static int mFreeThumbnailDataCallCount;
    static int mBackgroundThumbnailFetchingEnabled;
    static int mStartBackgroundFetchingCallCount;
    
private: // data
    
    
};
#endif  // __VIDEOTHUMBNAILDATA_H__

// End of file
