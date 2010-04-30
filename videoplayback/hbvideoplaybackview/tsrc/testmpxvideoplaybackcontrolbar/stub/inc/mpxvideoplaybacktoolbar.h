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
* Description:  Implementation of QMPXVideoPlaybackToolBar
*
*/

// Version : %version:  1 %



#ifndef MPXVIDEOPLAYBACKTOOLBAR_H_
#define MPXVIDEOPLAYBACKTOOLBAR_H_

#include <QWidget>
#include <mpxplaybackframeworkdefs.h>



class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;


class QMPXVideoPlaybackToolBar : public QWidget
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackToolBar( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackToolBar();
        void updateState( TMPXPlaybackState state );
        void aspectRatioChanged( int aspectRatio );
        void initialize();
        void updateWithFileDetails( QMPXVideoPlaybackViewFileDetails* details );
        void durationChanged( int duration );
        void positionChanged( int position );
        void setVisible( bool visible );

    public:
        QMPXVideoPlaybackControlsController *mController;
        QMPXVideoPlaybackViewFileDetails *mFileDetails;
        int mAspectRatio;
        int mPosition;
        int mDuration;
        bool mVisible;
        TMPXPlaybackState mState;
};

#endif /*MPXVIDEOPLAYBACKTOOLBAR_H_*/
