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
* Description:  Declaration of VideoServicePlay
*
*/

// Version : %version: 1 %

#ifndef __VIDEOSERVICEPLAY_H__
#define __VIDEOSERVICEPLAY_H__

#include <xqserviceprovider.h>
#include <QObject>

// FORWARD DECLARATIONS
class VideoServices;
class QVideoPlayerEngine;

class VideoServicePlay : public XQServiceProvider
    {

    Q_OBJECT

    public:
        VideoServicePlay( VideoServices* parent, QVideoPlayerEngine* engine );
        virtual ~VideoServicePlay();

        void setEngine( QVideoPlayerEngine* engine );

    public slots:
        void playMedia( QString filePath );
        void playPDLMedia( QString filePath, int downloadID );
        void closePlayer();

    private:
        QVideoPlayerEngine* mEngine;
        VideoServices* mServiceApp;


    };

    #endif //__VIDEOSERVICEPLAY_H__
