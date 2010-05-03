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

// Version : %version:   4 %



//  Include Files
#include <hbinstance.h>

#include "../inc/hbvideobaseplaybackview.h"


//  Member Functions

// -------------------------------------------------------------------------------------------------
// HbVideoPlaybackView::HbVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoBasePlaybackView::HbVideoBasePlaybackView()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   HbVideoPlaybackView::~HbVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoBasePlaybackView::~HbVideoBasePlaybackView()
{
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleActivateView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleActivateView()
{
    mViewActive = true;
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleDeactivateView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleDeactivateView()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleClosePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleClosePlaybackView()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::doClosePlayer
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::doClosePlayer()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::closePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::closePlaybackView()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::startClosingPlaybackView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::startClosingPlaybackView()
{
    mViewActive = false;
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::setTitleBarFlags()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::setViewFlags( HbView::HbViewFlags flags )
{
    mFlag = flags;
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::viewFlags()
// -------------------------------------------------------------------------------------------------
//
HbView::HbViewFlags HbVideoBasePlaybackView::viewFlags()
{
    return mFlag;
}

// EOF
