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
* Description: VideoThumbnailDataPrivate class definition
*
*/

#ifndef __VIDEOTHUMBNAILDATAPRIVATE_H__
#define __VIDEOTHUMBNAILDATAPRIVATE_H__

// INCLUDES
#include <QObject>
#include <QPointer>
#include <QCache>
#include <QIcon>
#include <QSet>
#include <QList>
#include <e32const.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS
class ThumbnailManager;
class VideoCollectionWrapper;
class VideoSortFilterProxyModel;
class QModelIndex;
class QTimer;

class VideoThumbnailDataPrivate : public QObject
{
    /**
     * defined to be able to use signals and slots
     */
    Q_OBJECT

public:
    
    
    /**
     * Default constructor
     */
    VideoThumbnailDataPrivate();

    /**
     * Destructor
     */
    virtual ~VideoThumbnailDataPrivate();

    /**
     * Starts fetching thumbnails for medias in the model defined by the indexes 
     * parameter. Priority for the thumbnails is ascending starting from the 
     * defined priority. Uses method startFetchingThumbnail for the actual 
     * fetching.

     *
     * param @indexes indexes of the medias in the model
     * param @priority starting priority for the thumbnails
     *
     * @return int count of fetches started or -1 in case of error.
     */
    int startFetchingThumbnails(const QList<QModelIndex> &indexes, int priority);

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
    int startFetchingThumbnail(TMPXItemId mediaId, int priority);

    /**
     * Method returns a pointer to video's thumbnail.
     *
     * If thumbnail fetching is ongoing or not yet started, pointer to default thumbnail
     * is returned.
     *
     * @param mediaId media id for which to get thumbnail
     * @param tnData reference to thumbnail data where data is ocpied
     *
     * @return QIcon* thumbnail pointer
     *
     */
    const QIcon* getThumbnail(TMPXItemId mediaId);

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
     * Starts background thumbnail fetching from the given fetch index.
     *
     * @param fetchIndex index where to start the background thumbnail fetching.
     */
    void startBackgroundFetching(int fetchIndex);
    
    /**
     * Enables or disables thumbnail background fetching.
     * 
     * @param enable true enables and false disables thumbnail background fetching.
     */
    void enableBackgroundFetching(bool enable);
     
    /**
     * Frees allocated data for thumbnails and cancels ongoing fetches.
     */
    void freeThumbnailData();
    
protected:    
    
    /**
     * Method creates member objects and connects signals.
     *
     * @return int 0 initialization ok, -1 initialization failed
     *
     */
    int initialize();

    /**
     * Cleanup internal data. This must be called before UI environment is destroyed.
     */
    void cleanup();

    /**
     * Connects the signals.
     *
     * @return int 0 connecting ok, -1 connecting failed.
     */
    int connectSignals();

    /**
     * Disconnects the signals.
     *
     */
    void disconnectSignals();
    
    /**
     * Appends indexes in the source model to the list between start and end.
     *
     * @param indexes list of indexes
     * @param startIndex start index
     * @param endIndex end index
     */
    void getModelIndexes(QList<QModelIndex> &indexes, int startIndex, int endIndex);

    /**
     * Method returns default thumbnail data.
     *
     * @param mediaId media id for which to get thumbnail
     * 
     * @return QIcon: reference to default thumbnail data
     *
     */
    const QIcon* defaultThumbnail(TMPXItemId mediaId);

    /**
     * Removes given id from the fetch list. Fetch list is an internal id list containing
     * thumbnail ids currently under fetching.
     *
     * When list is emptied the background thumbnail fetching is continued.
     *
     * @param tnId thumbnail id to be removed from the list
     *
     * @return true if id was found from fetch list, otherwise false.
     */
    bool removeFromFetchList(int tnId);

    /**
     * Starts timer that continues the background thumbnail fetching at timeout.
     */
    void continueBackgroundFetch();

signals:

    /**
     * This signal is emitted from thumbnailReadySlot, when
     *
     * @param mediaIds: media ids of the videos whose thumbnail is ready.
     *
     */
    void thumbnailsFetched(QList<TMPXItemId> mediaIds);

private slots:

    /**
     * Thumbnail manager signals this slot when thumbnail is ready
     * for some item.
     *
     * @param tnData: thumbnail
     * @param internal: internal data contains pointer to the media id used in emitted signal
     * @param id: thumbnail id
     * @param error: possible error code from thumbnail manager ( 0 == ok )
     *
     */
    void thumbnailReadySlot(QPixmap tnData, void *internal , int id , int error);

    /**
     * VideoSortFilterProxyModel signals this slot when the layout of items in the model have
     * changed.
     *
     */
    void layoutChangedSlot();

    /**
     * VideoSortFilterProxyModel signals this slot when the rows have been inserted to the model.
     *
     */
    void rowsInsertedSlot(const QModelIndex & parent, int start, int end);

    /**
     * Fetches thumbnails around of mCurrentFetchIndex in batches of THUMBNAIL_BACKGROUND_FETCH_AMOUNT.
     * The fetch index is set by startFetchingThumbnails which also resets
     * mCurrentBackgroundFetchCount. Maximum of THUMBNAIL_MAX_BACKGROUND_FETCH thumbnails are
     * fetched between the count resets.
     *
     */
    void doBackgroundFetching();
    
    /**
     * Signaled when UI environment is about to be destroyed. Thumbnails must be cleaned before that.
     * 
     */
    void aboutToQuitSlot();
    
    /**
     * Emits thumbnailReady signals for list of thumbnails. 
     * 
     */
    void reportThumbnailsReadySlot();
    
private: // Data

    /**
     * Thumbnail manager object.
     */
    ThumbnailManager                *mThumbnailManager;

    /**
     * list containing thumbnails ids which data is
     * currently being fetched from the tn wrapper.
     * empty if there are no thumbnail fetching
     * ongoing
     */
    QSet<int>                       mFetchList;

    /**
     * Local data map for thumbnail data:
     * - key is the video's media id
     * - data is the actual thumbnail data.
     * If thumbnail data is not yet fetched, QIcon is default thumbnail.
     */
    QCache<TMPXItemId, QIcon>       mThumbnailData;

    /**
     * Default thumbnail for a video.
     */
    QIcon                           *mDefaultTnVideo;

    /**
     * Default thumbnail for a category.
     */
    QIcon                           *mDefaultTnCategory;

    /**
     * Wrapper to collection.
     */
    VideoCollectionWrapper          *mCollectionWrapper;

    /**
     * Model containing the media, not owned.
     */
    QPointer<VideoSortFilterProxyModel>     mModel;

    /**
     * Current index for background thumbnail fetching.
     */
    int                             mCurrentFetchIndex;

    /**
     * Current count of background thumbnails fetched around the fetch index.
     */
    int                             mCurrentBackgroundFetchCount;

    /**
     * Timer for background fetching.
     */
    QTimer                          *mBgFetchTimer;

    /**
     * Timer for thumbnail ready signaling.
     */
    QTimer                          *mTbnReportTimer;
    
    /**
     * List of media ids for thumbnails that are ready.
     */
    QList<TMPXItemId>               mReadyThumbnailMediaIds;
    
    /**
     * Flag for signal connections.
     */
    bool                            mSignalsConnected;
    
    /**
     * Indicates if background thumbnail fetching is enabled.
     */
    bool                            mBackgroundFetchingEnabled;
};

#endif  // __VIDEOTHUMBNAILDATAPRIVATE_H__

// End of file
