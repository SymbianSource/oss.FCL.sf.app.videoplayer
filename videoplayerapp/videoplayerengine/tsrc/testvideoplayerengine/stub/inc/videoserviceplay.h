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

#ifndef VIDEOSERVICEPLAY_H
#define VIDEOSERVICEPLAY_H

#include <QObject>

#include "videoplayerengine.h"

// FORWARD DECLARATIONS
class VideoServices;

class VideoServicePlay : public QObject
    {

    Q_OBJECT

    public:
        VideoServicePlay( VideoServices* parent, QVideoPlayerEngine* engine );
        virtual ~VideoServicePlay();

    public slots:
        void playMedia( QString filePath );

    private:
        QVideoPlayerEngine* mEngine;
        VideoServices* mServiceApp;
    };

    #endif
