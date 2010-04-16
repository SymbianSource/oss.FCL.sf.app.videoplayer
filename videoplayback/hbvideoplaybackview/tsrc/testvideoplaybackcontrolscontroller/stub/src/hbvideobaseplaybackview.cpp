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
* Description:  Implementation of Video playback view
*
*/

// Version : %version: 1 %



//  Include Files
#include <aknappui.h>
#include <qgraphicssceneevent>
#include <hbinstance.h>
#include <mmf/common/mmferrors.h>

#include "../inc/hbvideobaseplaybackview.h"
#include "mpxvideo_debug.h"


//  Member Functions

// -------------------------------------------------------------------------------------------------
// HbVideoPlaybackView::HbVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoBasePlaybackView::HbVideoBasePlaybackView()
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::HbVideoBasePlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
//   HbVideoPlaybackView::~HbVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoBasePlaybackView::~HbVideoBasePlaybackView()
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::~HbVideoBasePlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
// HbVideoBasePlaybackView::mouseReleaseEvent
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::mouseReleaseEvent()"));
    Q_UNUSED( event );
    emit tappedOnScreen();
}

// EOF
