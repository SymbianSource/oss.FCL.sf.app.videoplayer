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



#include <qtimer>
#include <qgraphicsscenemouseevent>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybacknontouchvolumebar.h"


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackNonTouchVolumeBar::QMPXVideoPlaybackNonTouchVolumeBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackNonTouchVolumeBar::QMPXVideoPlaybackNonTouchVolumeBar(
    bool currentValueVisible,QGraphicsItem *parent )
    : HbVolumeSlider( currentValueVisible, parent )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackNonTouchVolumeBar::QMPXVideoPlaybackNonTouchVolumeBar()"));

}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackNonTouchVolumeBar::~QMPXVideoPlaybackNonTouchVolumeBar()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackNonTouchVolumeBar::~QMPXVideoPlaybackNonTouchVolumeBar()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackNonTouchVolumeBar::QMPXVideoPlaybackNonTouchVolumeBar"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackNonTouchVolumeBar::setVisible()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackNonTouchVolumeBar::setVisible( bool visible )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackNonTouchVolumeBar::setVisible"));

    mVisible = visible;
    
    QGraphicsItem::setVisible( visible );
}


// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackNonTouchVolumeBar::setValue()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackNonTouchVolumeBar::setValue( int value )
{
    mValue = value;
}

// End of file