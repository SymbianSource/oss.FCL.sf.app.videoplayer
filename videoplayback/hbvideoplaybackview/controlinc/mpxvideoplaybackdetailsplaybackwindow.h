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
* Description:  Implementation of QMPXVideoPlaybackDetailsPlaybackWindow
*
*/

// Version : %version:  7 %



#ifndef MPXVIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_
#define MPXVIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_

#include <hbwidget.h>
#include <mpxplaybackframeworkdefs.h>

class HbPushButton;
class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackDetailsPlaybackWindow : public HbWidget
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackDetailsPlaybackWindow( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackDetailsPlaybackWindow();
        void initialize();
        void updateState( TMPXPlaybackState state );

    private slots:
        void playPause();

    private:
        QMPXVideoPlaybackControlsController *mController;
        HbPushButton                        *mPlayButton;

        bool                                 mInitialized;
};

#endif /*MPXVIDEOPLAYBACKDETAILSPLAYBACKWINDOW_H_*/

