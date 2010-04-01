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

// Version : %version:  1 %



#ifndef MPXVIDEOPLAYBACKBUTTONBAR_H_
#define MPXVIDEOPLAYBACKBUTTONBAR_H_

#include <hbwidget.h>
#include <mpxplaybackframeworkdefs.h>



class QMPXVideoPlaybackViewFileDetails;
class QMPXVideoPlaybackControlsController;


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

    public:
        QMPXVideoPlaybackControlsController *mController;
        QMPXVideoPlaybackViewFileDetails *mFileDetails;
        int mAspectRatio;
        TMPXPlaybackState mState;
};

#endif /*MPXVIDEOPLAYBACKBUTTONBAR_H_*/
