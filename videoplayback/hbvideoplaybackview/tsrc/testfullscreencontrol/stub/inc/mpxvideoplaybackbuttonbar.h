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
* Description:  Implementation of QMPXVideoPlaybackButtonBar
*
*/

// Version : %version: 1 %



#ifndef MPXVIDEOPLAYBACKBUTTONBAR_H_
#define MPXVIDEOPLAYBACKBUTTONBAR_H_

#include <hbwidget.h>
#include <MMFScalingCustomCommandConstants.h>
#include <mpxplaybackframeworkdefs.h>


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

        void aspectRatioChanged( int aspectRatio );

        void updateWithFileDetails( QMPXVideoPlaybackViewFileDetails* details );
        
        void updateState( TMPXPlaybackState state );

    public:
        QMPXVideoPlaybackControlsController *mController;
        bool                                 mInitialized;
        int                                  mAspectRatio;
        TMPXPlaybackState                    mState;
        bool                                 mShowPauseButton;
};

#endif /*MPXVIDEOPLAYBACKBUTTONBAR_H_*/
