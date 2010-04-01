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
* Description:  Implementation of QMpxVideoPlaybackWrapper
*
*/

// Version : %version: 3 %



#ifndef MPXVIDEOPLAYBACKWRAPPER_H_
#define MPXVIDEOPLAYBACKWRAPPER_H_

#include <QObject>
#include <QString>
#include <QFile>

#include "mpxhbvideocommondefs.h"

class CMpxVideoPlayerAppUiEngine;

class QMpxVideoPlaybackWrapper : public QObject
{
    Q_OBJECT

    public:
        QMpxVideoPlaybackWrapper();
        virtual ~QMpxVideoPlaybackWrapper();

        int playMedia( QString aFileName );
        void openPlaybackView() ;

    private:
        void initializePlugins();
 
    signals:
        void handlePlaybackView( int viewId );

    private:   //data  
        CMpxVideoPlayerAppUiEngine      *mUiEngine;
};

#endif /* MPXVIDEOPLAYBACKWRAPPER_H_ */
