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
* Description:   VideoThumbnailData singleton class definition
*/

#ifndef __VIDEOTHUMBNAILDATA_H__
#define __VIDEOTHUMBNAILDATA_H__

// INCLUDES
#include <QObject>
#include <QHash>
#include <QPair>
#include <QIcon>
#include <QSet>
#include <QPointer>
#include <mpxitemid.h>

#include "videocollectionexport.h"

// FORWARD DECLARATIONS
class VideoThumbnailDataPrivate;

// CLASS DECLARATIONS

class VIDEOCOLLECTION_DLL_EXPORT VideoThumbnailData : public QObject
{
    /**
     * Defined to be able to use signals and slots.
     */
    Q_OBJECT

    /**
     * disable copy-constructor and assignment operator
     */
	Q_DISABLE_COPY(VideoThumbnailData)

public:

    /*
     * Thumbnail fetch priorities.
     */
    enum VideoThumbnailPriority
    {
        VideoThumbnailPriorityLow = 2000,
        VideoThumbnailPriorityMedium = 3000,
        VideoThumbnailPriorityHigh = 5000
    };    
    
    /**
     * Returns singleton instance for this class.
     *
     * @return The singleton instance.
     *
     */
    static VideoThumbnailData &instance();

    /**
     * Method starts fetching thumbnail for the video specified by given mediaId
     * Media IDs are used as a key identification in the thumbnail map where data is
     * collected. Id is passed to thumbnail manager as internal data and when
     * thumbnail is fetched and thumbnailReadySlot is signaled, id is used to notify
     * client about which item's thumbnail is ready.
     *
     * If thumbnail fetching is started succesfully, method saves thumbnail item
     * into local repository with default thumbnail data.
     *
     * @param mediaId media id for which to start fetching thumbnail
     * @param priority priority for the fetch
     *
     * @return int: thumbnail id or -1 if fetch starting fails.
     *
     */
    int startFetchingThumbnail(int mediaId, int priority);
    
    /**
     * Method removes thumbnail data from the local repository.
     *
     * @param mediaId id for media whose thumbnail is to be removed
     *
     * @return bool: true if item removed correctly, false if not.
     *
     */
    bool removeThumbnail(TMPXItemId mediaId);
    
    /**
     * Method returns a pointer to video's thumbnail.
     *
     * If thumbnail fetching is ongoing or not yet started, pointer to default 
     * thumbnail is returned.
     *
     * @param mediaId media id for which to get thumbnail
     * @param tnData reference to thumbnail data where data is occupied
     *
     * @return QIcon* thumbnail pointer
     *
     */
    const QIcon* getThumbnail(TMPXItemId mediaId);

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
     * 
     */
    void freeThumbnailData();

signals:

    /**
     * This signal is emitted from thumbnailReadySlot, when
     *
     * @param mediaIds: media ids of the videos whose thumbnail is ready.
     *
     */
    void thumbnailsFetched(QList<TMPXItemId> mediaIds);
    
private:

    /**
     * Default constructor
     */
    VideoThumbnailData();

    /**
     * Destructor
     */
    ~VideoThumbnailData();
    
    /**
     * Private implementation.
     */
    QPointer<VideoThumbnailDataPrivate> const d_ptr;
};

#endif  // __VIDEOTHUMBNAILDATA_H__

// End of file
