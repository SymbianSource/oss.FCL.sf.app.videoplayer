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

// Version : %version: da1mmcf#9 %



#ifndef MPXVIDEOPLAYBACKPROGRESSBAR_H_
#define MPXVIDEOPLAYBACKPROGRESSBAR_H_

#include <hbwidget.h>
#include <mpxplaybackframeworkdefs.h>

class HbLabel;
class HbProgressBar;
class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackProgressBar : public HbWidget
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackProgressBar( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackProgressBar();
        void mousePressEvent( QGraphicsSceneMouseEvent *event );
        void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
        void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
        void initialize();
        void updateWithFileDetails( QMPXVideoPlaybackViewFileDetails* details );
        void updateState( TMPXPlaybackState state );

    public:
        void durationChanged( int duration );
        void positionChanged( int position );

    private:
        QString valueToReadableFormat( int value );
        void updatePostion( int position );

    private:
        QMPXVideoPlaybackControlsController *mController;
        HbProgressBar                       *mProgressSlider;
        HbLabel                             *mDurationLabel;
        HbLabel                             *mPositionLabel;

        int                                  mDuration;
        bool                                 mNeedToResumeAfterSetPosition;
        bool                                 mInitialized;
        bool                                 mDragging;
};

#endif /*MPXVIDEOPLAYBACKPROGRESSBAR_H_*/

