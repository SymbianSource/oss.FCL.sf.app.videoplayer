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
* Description:  Implementation of QMPXVideoPlaybackDetailsPlaybackWindow
*
*/

// Version : %version:   1 %



#ifndef MPXVIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_
#define MPXVIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_

#include <qobject>

class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackDetailsPlaybackWindow : public QObject
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackDetailsPlaybackWindow( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackDetailsPlaybackWindow();

    private:
        QMPXVideoPlaybackControlsController *mController;
};

#endif /*MPXVIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_*/

