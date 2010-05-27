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
* Description:  Implementation of QMPXVideoPlaybackProgressBar
*
*/

// Version : %version: da1mmcf#12 %



#ifndef MPXVIDEOPLAYBACKPROGRESSBAR_H_
#define MPXVIDEOPLAYBACKPROGRESSBAR_H_

#include <hbwidget.h>
#include <mpxplaybackframeworkdefs.h>

class QTimer;
class HbProgressSlider;
class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackProgressBar : public HbWidget
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackProgressBar( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackProgressBar();
        void initialize();
        void updateWithFileDetails( QMPXVideoPlaybackViewFileDetails* details );
        void updateState( TMPXPlaybackState state );

    public:
        void durationChanged( int duration );
        void positionChanged( int position );

    private slots:
        void handleSliderPressed();
        void handleSliderMoved( int value );
        void handleSliderReleased();
        void handleSeekingTimeout();

    private:
        QString valueToReadableFormat( int value );
        void updatePostion( int position );

    private:
        QMPXVideoPlaybackControlsController *mController;
        HbProgressSlider                    *mProgressSlider;

        int     mDuration;
        int     mDraggingPosition;
        int     mSetPosition;

        bool    mNeedToResumeAfterSetPosition;
        bool    mInitialized;
        bool    mSliderDragging;
        bool    mLongTimeFormat;
        bool    mLiveStreaming;

        QTimer *mSeekingTimer;
};

#endif /*MPXVIDEOPLAYBACKPROGRESSBAR_H_*/

