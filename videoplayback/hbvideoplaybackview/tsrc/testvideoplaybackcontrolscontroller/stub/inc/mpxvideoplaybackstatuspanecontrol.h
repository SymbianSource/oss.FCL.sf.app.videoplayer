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
* Description:  Implementation of QMPXVideoPlaybackStatusPaneControl
*
*/

// Version : %version:  1 %



#ifndef MPXVIDEOPLAYBACKSTATUSPANECONTROL_H_
#define MPXVIDEOPLAYBACKSTATUSPANECONTROL_H_


#include "mpxvideoplaybackfullscreencontrol.h"


class QMPXVideoPlaybackControlsController;

class QMPXVideoPlaybackStatusPaneControl : public QMPXVideoPlaybackFullScreenControl
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackStatusPaneControl( QMPXVideoPlaybackControlsController* controller, 
                                            TMPXVideoPlaybackControls index,
                                            HbWidget* widget, 
                                            TUint controlproperties );

        virtual ~QMPXVideoPlaybackStatusPaneControl();

};

#endif /*MPXVIDEOPLAYBACKSTATUSPANECONTROL_H_*/

