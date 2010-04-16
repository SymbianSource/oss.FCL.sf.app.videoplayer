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
* Description:  Implementation of QMPXVideoPlaybackNonTouchVolumeBar
*
*/

// Version : %version:  1 %



#ifndef MPXVIDEOPLAYBACKNONTOUCHVOLUMEBAR_H_
#define MPXVIDEOPLAYBACKNONTOUCHVOLUMEBAR_H_


#include <hbvolumeslider.h>

class QTimer;
class QGraphicsSceneMouseEvent;

class QMPXVideoPlaybackNonTouchVolumeBar : public HbVolumeSlider
{
    Q_OBJECT

    public:
        QMPXVideoPlaybackNonTouchVolumeBar( 
                bool currentValueVisible = false, QGraphicsItem *parent = 0 );

        virtual ~QMPXVideoPlaybackNonTouchVolumeBar();

    public:
        void setVisible( bool visible );
        
        void setValue( int value );

    public:

        bool    mVisible;
        int     mValue;
        
};

#endif /*MPXVIDEOPLAYBACKNONTOUCHVOLUMEBAR_H_*/

