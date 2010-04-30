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

// Version : %version:   2 %



//  Include Files

#include <hbinstance.h>
#include <coecntrl.h>

#include "hbtoolbar.h"
#include "hbvideobaseplaybackview.h"

//  Member Functions

// -------------------------------------------------------------------------------------------------
// HbVideoPlaybackView::HbVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoBasePlaybackView::HbVideoBasePlaybackView()
{
     mToolBar = new HbToolBar();
}

// -------------------------------------------------------------------------------------------------
//   HbVideoPlaybackView::~HbVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoBasePlaybackView::~HbVideoBasePlaybackView()
{
    if ( mToolBar )
    {
         delete mToolBar;
         mToolBar = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::toolBar()
// -------------------------------------------------------------------------------------------------
//
HbToolBar* HbVideoBasePlaybackView::toolBar()
{
    return mToolBar;
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::hideItems()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::hideItems( int item )
{
    Q_UNUSED( item );
    mToolBar->setVisible( false );
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::hideItems()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::showItems( int item )
{
    Q_UNUSED( item );
    mToolBar->setVisible( true );
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::getWindow()
// -------------------------------------------------------------------------------------------------
//
RWindow *HbVideoBasePlaybackView::getWindow()
{
    return static_cast<RWindow*>( hbInstance->allMainWindows()[0]->effectiveWinId()->DrawableWindow() );
}

// EOF
