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
* Description:  Implementation of QMPXVideoPlaybackNonTouchVolumeBar
*
*/

// Version : %version:  4 %



#ifndef MPXVIDEOPLAYBACKNONTOUCHVOLUMEBAR_H_
#define MPXVIDEOPLAYBACKNONTOUCHVOLUMEBAR_H_


#include <hbvolumesliderpopup.h>

class QTimer;
class QGraphicsSceneMouseEvent;

class QMPXVideoPlaybackNonTouchVolumeBar : public HbVolumeSliderPopup
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackNonTouchVolumeBar();

        virtual ~QMPXVideoPlaybackNonTouchVolumeBar();

    public:
        void setVisible( bool visible );
        virtual void mousePressEvent( QGraphicsSceneMouseEvent *event );
        virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

    private slots:
        void hideVolumeControl();

    private:
        QTimer  *mVolumeTimer;
};

#endif /*MPXVIDEOPLAYBACKNONTOUCHVOLUMEBAR_H_*/

