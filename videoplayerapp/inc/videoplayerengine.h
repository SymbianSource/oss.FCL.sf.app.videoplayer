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
* Description:  Implementation of QVideoPlayerEngine
*
*/

// Version : %version: ou1cpsw#9 %

#ifndef VIDEOPLAYERENGINE_H
#define VIDEOPLAYERENGINE_H

#include <qobject.h>
#include <mpxviewpluginqt.h>

#include "mpxhbvideocommondefs.h"
#include "videoplayerappexport.h"

//FORWARD CLASS DECLARATION

class MpxViewPlugin;
class VideoServices;
class QMpxVideoPlaybackWrapper;

/**
 *  QVideoPlayerEngine
 * 
 */
class VIDEOPLAYERAPP_DLL_EXPORT QVideoPlayerEngine: public QObject
{
    Q_OBJECT

    public:
        /**
         * Constructor
         */
        QVideoPlayerEngine(bool isService = false);

        /**
        * Destructor.
        */
        virtual ~QVideoPlayerEngine();
    
    public:
        void initialize();
        void playMedia( QString filePath );
        void setEmbedded();

    public slots:
        void handleCommand( int commandCode );

    private slots:
        void handleQuit();

    private:
        void activateView( MpxHbVideoCommon::MpxHbVideoViewType viewType );

        void connectView();
        void disconnectView();
    
        void switchView();    
    
        void loadPlugin( MpxHbVideoCommon::MpxHbVideoViewType viewType );

    private:
        bool                      mIsService;
        bool                      mEmbedded;   

        MpxViewPlugin*            mCurrentViewPlugin;
        MpxViewPlugin*            mPlaybackViewPlugin; 
        MpxViewPlugin*            mCollectionViewPlugin; 
        MpxViewPlugin*            mFileDetailsViewPlugin;
        

        QMpxVideoPlaybackWrapper *mPlaybackWrapper;

        VideoServices*            mVideoServices;

};

#endif // VIDEOPLAYERENGINE_H

// EOF
