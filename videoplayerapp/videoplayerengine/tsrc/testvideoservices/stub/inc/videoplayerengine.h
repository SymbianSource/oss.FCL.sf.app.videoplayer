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

// Version : %version: 1 %

#ifndef VIDEOPLAYERENGINE_H
#define VIDEOPLAYERENGINE_H

#include <qobject.h>

//FORWARD CLASS DECLARATION

/**
 *  QVideoPlayerEngine
 *
 */
class QVideoPlayerEngine: public QObject
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
        bool                      mIsService;
        bool                      mEmbedded;
};

#endif // VIDEOPLAYERENGINE_H

// EOF
