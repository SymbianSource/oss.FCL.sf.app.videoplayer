/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of QMPXVideoPlaybackProgressBar
*
*/

// Version : %version:  1 %



#ifndef MPXVIDEOPLAYBACKPROGRESSBAR_H_
#define MPXVIDEOPLAYBACKPROGRESSBAR_H_

#include <qobject>

class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackProgressBar : public QObject
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackProgressBar( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackProgressBar();

    public:
        QMPXVideoPlaybackControlsController *mController;
};

#endif /*MPXVIDEOPLAYBACKPROGRESSBAR_H_*/

