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

// Version : %version: da1mmcf#12 %



#ifndef MPXVIDEOPLAYBACKBUTTONBAR_H_
#define MPXVIDEOPLAYBACKBUTTONBAR_H_

#include <hbwidget.h>
#include <MMFScalingCustomCommandConstants.h>
#include <mpxplaybackframeworkdefs.h>


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
    EMPXButtonAttach,
    EMPXButtonCount    // Should always be the last value
};

enum TMPXSeekingState
{
    EMPXNotSeeking,
    EMPXFastForwarding,
    EMPXRewinding
};

const int KMPXFastForward = 30;
const int KMPXRewind = -10;

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
        void positionChanged( int position );
        void durationChanged( int duration );

    private slots:
        void play();
        void pause();
        void changeAspectRatio();
        void handleButtonPressed();
        void ffPressing();
        void rwPressing();
        void ffReleased();
        void rwReleased();
        void openDetailsView();

    private:
        QMPXVideoPlaybackControlsController *mController;
        QList<HbPushButton*>                 mButtons;
        TMPXSeekingState                     mSeekingState;

        bool mInitialized;
        int  mPosition;
        int  mDuration;
};

#endif /*MPXVIDEOPLAYBACKBUTTONBAR_H_*/
