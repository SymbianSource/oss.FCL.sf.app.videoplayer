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
* Description: VideoThumbnailFetcher class definition
*
*/

#ifndef __VIDEOTHUMBNAILDATAFETCHER_H__
#define __VIDEOTHUMBNAILDATAFETCHER_H__

// INCLUDES
#include <qobject.h>
#include <qicon.h>
#include <qlist.h>
#include <qhash.h>
#include <qmap.h>
#include <qpixmap.h>

// FORWARD DECLARATIONS


class ThumbnailFetchData
{
    public:
        QString mFileName;
        int mPriority;
        void *mInternal;
};

class VideoThumbnailFetcher : public QObject
{
    /**
     * defined to be able to use signals and slots
     */
    Q_OBJECT

public:

    /**
     * Default constructor
     */
    VideoThumbnailFetcher();
    
    /**
     * Desctructor
     */
    ~VideoThumbnailFetcher();

    /**
     * Adds thumbnail fetch to the fetch list. pauseFetching should be called 
     * before adding fetches for performance reasons. After all fetches have
     * been added, call continueFetching to start the fetch process which passes 
     * the internal data to thumbnail manager. Signal thumbnailReady is emitted 
     * when fetch is complete. 
     * 
     * @param fileName path to the media.
     * @param internal data identifying the media.
     * @param priority priority for the fetch.
     * 
     */
    void addFetch(const QString fileName, void *internal, int priority);
    
    /**
     * Empties fetch list. This does not cancel the possible ongoing fetch on
     * thumbnail manager side.
     * 
     */
    void cancelFetches();
    
    /**
     * Returns count of the fetches.
     * 
     */
    int fetchCount();
    
    /**
     * Pauses thumbnail fetching process. This does not pause the possible 
     * ongoing fetch on thumbnail manager side. 
     * 
     */
    void pauseFetching();
    
    /**
     * Continues the fetching process. All fetches added with addFetch are 
     * started without create thumbnail flag. If there's not any of those, 
     * starts creating thumbnails for fetches that have no thumbnail yet. 
     * Signal allThumbnailsFetched is emitted if there's nothing to do.
     * 
     */
    void continueFetching();
    
    /**
     * Enables or disables the thumbnail creation for videos that do  
     * not have thumbnail already in the database.
     * 
     * @param enable true enables thumbnail creation, false disables. 
     * 
     */
    void enableThumbnailCreation(bool enable);
    
// Test helper methods
public:
    
    void emitThumbnailReady(QPixmap pixmap, void *internal, int error);
    
    void emitAllThumbnailsFetched();

signals:
    
    /**
     * Signaled after signal from thumbnail manager has been processed and
     * thumbnail fetch process is complete.
     *
     * @param tnData thumbnail
     * @param internal internal data to identify the request
     * @param error possible error code from thumbnail manager ( 0 == ok )
     * 
     */
    void thumbnailReady(QPixmap tnData, void *internal, int error);

    /**
     * Signaled when all the fetches have been done.
     * 
     */
    void allThumbnailsFetched();
    
public:

    struct TnRequest
    {
       QString name;
       void *id;
       int priority;
       bool cancelled;

       TnRequest(QString name, void *id, int priority, bool cancelled) {
           this->name = name;
           this->id = id;
           this->priority = priority;
           this->cancelled = cancelled;
       }

       TnRequest() {
           name = QString("");
           id = 0;
           priority = -1;
           cancelled = false;
       }
    };

    static QMap<int, TnRequest> mRequests;
    
public: // Data
    
    static int mConstructorCallCount;
    static int mDestructorCallCount;
    static int mAddFetchCallCount;
    static int mCancelFetchesCallCount;
    static int mFetchCountCallCount;
    static int mPauseFetchingCallCount;
    static int mContinueFetchingCallCount;
    static int mEnableThumbnailCreationCallCount;
    
    static bool mAddFetchFails;
    static int mThumbnailReadyError;
        
    /*
    static int mxxxCallCount;
    static int mxxxCallCount;
    static int mxxxCallCount;
    static int mxxxCallCount;
    static int mxxxCallCount;
    static int mxxxCallCount;*/
};

#endif // __VIDEOTHUMBNAILDATAFETCHER_H__
