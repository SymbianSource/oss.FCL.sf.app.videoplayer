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
* Description:  Implementation of QMPXVideoPlaybackControlBar
*
*/

// Version : %version: 2 %



#ifndef MPXVIDEOPLAYBACKCONTROLBAR_H_
#define MPXVIDEOPLAYBACKCONTROLBAR_H_



#include <hbwidget.h>
#include <mpxplaybackframeworkdefs.h>

class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackControlBar : public HbWidget
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackControlBar( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackControlBar();
        void updateState( TMPXPlaybackState state );
        void aspectRatioChanged( int aspectRatio );
        void updateWithFileDetails( QMPXVideoPlaybackViewFileDetails* details );
        void setVisibleToControlBar( bool visible );
        void durationChanged( int duration );
        void positionChanged( int position );

    public:
        QMPXVideoPlaybackControlsController *mController;
        int  mPosition;
        int  mDuration;
        int  mAspectRatio;
        bool mVisibility;
        TMPXPlaybackState mState;
        QMPXVideoPlaybackViewFileDetails *mFileDetails;
};

#endif /*MPXVIDEOPLAYBACKCONTROLBAR_H_*/
