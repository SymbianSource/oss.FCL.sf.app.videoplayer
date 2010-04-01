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
* Description:  Implementation of QMPXVideoPlaybackButtonBar
*
*/

// Version : %version: da1mmcf#8 %



#ifndef MPXVIDEOPLAYBACKBUTTONBAR_H_
#define MPXVIDEOPLAYBACKBUTTONBAR_H_

#include <hbwidget.h>
#include <MMFScalingCustomCommandConstants.h>
#include <mpxplaybackframeworkdefs.h>


class HbFrameItem;
class HbPushButton;
class QActionGroup;
class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;

// DATA TYPES
enum TMPXButton
{
    EMPXButtonRW,
    EMPXButtonPlay,
    EMPXButtonPause,
    EMPXButtonFF,
    EMPXButtonNatural,
    EMPXButtonStretch,
    EMPXButtonZoom,
    EMPXButtonDetails,
    EMPXButtonCount    // Should always be the last value
};


class QMPXVideoPlaybackButtonBar : public HbWidget
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackButtonBar( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackButtonBar();
        void updateState( TMPXPlaybackState state );
        void aspectRatioChanged( int aspectRatio );
        void initialize();
        void updateWithFileDetails( QMPXVideoPlaybackViewFileDetails* details );

    private slots:
        void play();
        void pause();
        void changeAspectRatio();
        void handleButtonPressed();
        void startFFSeeking();
        void startRWSeeking();
        void endSeeking();
        void openDetailsView();

    private:
        QMPXVideoPlaybackControlsController *mController;

        QList<HbPushButton*>                 mButtons;
        bool                                 mInitialized;

        HbFrameItem                         *mFrameItem;
};

#endif /*MPXVIDEOPLAYBACKBUTTONBAR_H_*/
