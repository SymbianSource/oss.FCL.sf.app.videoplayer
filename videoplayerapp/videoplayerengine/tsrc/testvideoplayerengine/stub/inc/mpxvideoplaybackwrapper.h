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
* Description:  Stub implementation of QMpxVideoPlaybackWrapper
*
*/

// Version : %version: da1mmcf#3 %



#ifndef MPXVIDEOPLAYBACKWRAPPER_H_
#define MPXVIDEOPLAYBACKWRAPPER_H_

#include <qobject>
#include <qstring>
#include <qfile>

#include <mpxhbvideocommondefs.h>

class QMpxVideoPlaybackWrapper : public QObject
{
    Q_OBJECT

    public:
        QMpxVideoPlaybackWrapper();
        virtual ~QMpxVideoPlaybackWrapper();
        int playMedia( QString aFileName );

    public:
        void openPlaybackView() ;
        static int GetInstanceCount();

    signals:
        void handlePlaybackView( int viewId );

    private:
        static void Increment();
        static void Decrement();
};

#endif /* STUBMPXVIDEOPLAYBACKWRAPPER_H_ */
