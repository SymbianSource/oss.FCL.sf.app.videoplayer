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
* Description:  Implementation of QMPXVideoPlaybackFileDetailsWidget
*
*/

// Version : %version:  1 %



#ifndef MPXVIDEOPLAYBACKFILEDETAILSWIDGET_H_
#define MPXVIDEOPLAYBACKFILEDETAILSWIDGET_H_

#include <qobject>

class QMPXVideoPlaybackControlsController;


class QMPXVideoPlaybackFileDetailsWidget : public QObject
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackFileDetailsWidget( QMPXVideoPlaybackControlsController* controller );
        virtual ~QMPXVideoPlaybackFileDetailsWidget();
        
    private:        
        QMPXVideoPlaybackControlsController *mController;
};

#endif /*MPXVIDEOPLAYBACKFILEDETAILSWIDGET_H_*/

