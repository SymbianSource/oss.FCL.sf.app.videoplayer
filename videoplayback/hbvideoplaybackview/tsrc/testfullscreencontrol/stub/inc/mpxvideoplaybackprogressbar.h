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

#include <hbwidget.h>
#include <mpxplaybackframeworkdefs.h>

class HbLabel;
class QPainter;
class HbProgressBar;
class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackProgressBar : public HbWidget
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackProgressBar( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackProgressBar();

        void updateWithFileDetails( QMPXVideoPlaybackViewFileDetails* details );

        void durationChanged( int duration );
        void positionChanged( int position );
        void updateState( TMPXPlaybackState state );

    public:
        QMPXVideoPlaybackControlsController *mController;
        int                                  mDuration;
        bool                                 mNeedToResumeAfterSetPosition;
        bool                                 mInitialized;
        bool                                 mDragging;  
        int                                  mPosition;

};

#endif /*MPXVIDEOPLAYBACKPROGRESSBAR_H_*/

