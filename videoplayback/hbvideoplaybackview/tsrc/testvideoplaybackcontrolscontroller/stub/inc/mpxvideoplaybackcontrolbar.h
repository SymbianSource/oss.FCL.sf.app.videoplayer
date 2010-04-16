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

// Version : %version: 1 %



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
        void initialize();

    public:
        QMPXVideoPlaybackControlsController *mController;
};

#endif /*MPXVIDEOPLAYBACKCONTROLBAR_H_*/
