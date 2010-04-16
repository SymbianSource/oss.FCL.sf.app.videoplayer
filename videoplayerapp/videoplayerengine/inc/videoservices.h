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
* Description:   VideoServices class definition
*
*/

#ifndef __VIDEOSERVICES_H__
#define __VIDEOSERVICES_H__

#include <QStringList>
#include "videoplayerappexport.h"

//FORWARD CLASS DECLARATION
class VideoServiceUriFetch;
class VideoServicePlay;
class QVideoPlayerEngine;

class VIDEOPLAYERAPP_DLL_EXPORT VideoServices : public QObject
{
    Q_OBJECT

public:

    /**
     * Returns singleton instance for this class.
     *
     * WARNING! Not safe to call this from destructor of another function scope static object!
     *
     * @return The singleton instance.
     */
    static VideoServices *instance(QVideoPlayerEngine* engine = 0);

    /**
     * Decreases the reference count, when count reaches zero cleanup is done.
     *
     */
    void decreaseReferenceCount();

    /**
     * Returns the context title set by service requestee
     *
     * @return QString the title
     *
     */
    QString contextTitle() const;

	/*
	 * Enum reflecting the services provided 
	 */
    enum TVideoService
    {
        ENoService,
        EUriFetcher,
        EPlayback
    };

    /**
     * Returns service active status
     *
     * @return bool true if active, false if not active
     *
     */
    VideoServices::TVideoService currentService();

public slots:
    void itemSelected(const QString& item);

signals:
	/*
	 * Emitted when service user has set the title
	 */    
	void titleReady(const QString& title);

	/*
	 * Emitted to acticate requested plugin
	 */
	void activated(int command);

private:

    /**
     * Constructor
     */
    VideoServices();

    /**
     * Constructor
     */
    VideoServices(QVideoPlayerEngine* engine);

    /**
     * Destructor.
     */
    virtual ~VideoServices();

    void setEngine(QVideoPlayerEngine* engine);

    /**
     * Sets the active service
     *
     * @param service
     *
     */
    void setCurrentService(VideoServices::TVideoService service);

    /**
     * Returns the current engine
     *
     * @return engine
     *
     */
    QVideoPlayerEngine* engine();

    Q_DISABLE_COPY(VideoServices)

private:

    /**
     * Reference count.
     */
    int mReferenceCount;

    /**
     * Singleton instance.
     */
    static VideoServices* mInstance;

    /**
     * VideoServiceUriFetch service instance.
     */
    VideoServiceUriFetch* mServiceUriFetch;

    /**
     * VideoServicePlay service instance.
     */
    VideoServicePlay* mServicePlay;

    /**
     * Pointer of QVideoPlayerEngine.
     */
    QVideoPlayerEngine* mEngine;

	/*
	 * Current service
	 */
    VideoServices::TVideoService mCurrentService;

    friend class VideoServiceUriFetch;

    friend class VideoServicePlay;
    };

#endif //__VIDEOSERVICES_H__
