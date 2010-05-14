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
* Description:  Implementation of QMPXVideoPlaybackToolBar
*
*/

// Version : %version: 2 %



#ifndef MPXVIDEOPLAYBACKTOOLBAR_H_
#define MPXVIDEOPLAYBACKTOOLBAR_H_

#include <QObject>

#include <MMFScalingCustomCommandConstants.h>
#include <mpxplaybackframeworkdefs.h>


class QTimer;
class HbIcon;
class HbAction;
class HbToolBar;
class HbToolButton;
class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;

// DATA TYPES

enum TMPXButton
{
    EMPX1stButton,
    EMPX2ndButton,
    EMPX3rdButton,
    EMPX4thButton,
    EMPX5thButton,
    EMPXButtonCount    // Should always be the last value
};

enum TMPXButtonIcon
{
    EMPXRWIcon,
    EMPXPlayIcon,
    EMPXPauseIcon,
    EMPXFFIcon,
    EMPXNaturalIcon,
    EMPXStretchIcon,
    EMPXZoomIcon,
    EMPXDetailsIcon,
    EMPXAttachIcon,
    EMPXShareIcon,
    EMPXIconCount    // Should always be the last value
};

enum TMPXSeekingState
{
    EMPXNotSeeking,
    EMPXFastForwarding,
    EMPXRewinding
};

const int KMPXFastForward = 30;
const int KMPXRewind = -10;

class QMPXVideoPlaybackToolBar : public QObject
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackToolBar( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackToolBar();
        void updateState( TMPXPlaybackState state );
        void aspectRatioChanged( int aspectRatio );
        void initialize();
        void updateWithFileDetails( QMPXVideoPlaybackViewFileDetails* details );
        void positionChanged( int position );
        void durationChanged( int duration );
        void setVisible( bool visible );

    private slots:
        void playPause();
        void changeAspectRatio();
        void handleButtonPressed();
        void handleRWButtonPressed();
        void handleFFButtonPressed();
        void ffPressing();
        void rwPressing();
        void ffReleased();
        void rwReleased();
        void openDetailsView();
        void retrieveButtons();

    private:
        QMPXVideoPlaybackControlsController *mController;
        HbToolBar                           *mToolBar;
        QTimer                              *mSeekStartTimer;
        QTimer                              *mRetrieveButtonTimer;

        QList<HbIcon*>                       mButtonIcons;
        QList<HbAction*>                     mButtonActions;
        QList<HbToolButton*>                 mButtons;

        TMPXSeekingState                     mSeekingState;

        bool    mInitialized;
        bool    mNeverVisibled;
        int     mPosition;
        int     mDuration;
        int     mAspectRatio;
        TReal32 mDisplayAspectRatio;
};

#endif /*MPXVIDEOPLAYBACKTOOLBAR_H_*/
