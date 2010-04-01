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
* Description:  Implementation of Video playback view
*
*/

// Version : %version:  5 %



//  Include Files
#include <aknappui.h>
#include <qgraphicssceneevent>
#include <hbinstance.h>
#include <mmf/common/mmferrors.h>

#include "../inc/hbvideobaseplaybackview.h"


//  Member Functions

// -------------------------------------------------------------------------------------------------
// HbVideoPlaybackView::HbVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoBasePlaybackView::HbVideoBasePlaybackView()
{
    mCurrentError = KErrNone;
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
// HbVideoBasePlaybackView::mousePress
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::mousePressEvent( QGraphicsSceneMouseEvent *event ) 
{
    Q_UNUSED( event );
}

// -------------------------------------------------------------------------------------------------
// HbVideoBasePlaybackView::itemChange
// -------------------------------------------------------------------------------------------------
//
QVariant HbVideoBasePlaybackView::itemChange( GraphicsItemChange change, const QVariant &value )
{
    return QGraphicsWidget::itemChange( change, value );
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::closePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::paint( QPainter *painter, 
                                     const QStyleOptionGraphicsItem *option, 
                                     QWidget *widget )
{
    Q_UNUSED( painter );
    Q_UNUSED( option );
    Q_UNUSED( widget );    
}


// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::event()
// -------------------------------------------------------------------------------------------------
//
bool HbVideoBasePlaybackView::event( QEvent *event )
{
    Q_UNUSED( event );
    bool consumed = false;

    return consumed;
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
//   HbVideoBasePlaybackView::issuePlayCommand
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::issuePlayCommand()
{
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleBufferingState
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleBufferingState()
{
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handlePluginError()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handlePluginError( int aError )
{
    mCurrentError = aError;
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
//   HbVideoBasePlaybackView::retrievePdlInformation
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::retrievePdlInformation()
{
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
//   HbVideoBasePlaybackView::getWindow()
// -------------------------------------------------------------------------------------------------
//
RWindow *HbVideoBasePlaybackView::getWindow()
{
    return static_cast<RWindow*>( hbInstance->allMainWindows()[0]->effectiveWinId()->DrawableWindow() );
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleClosePopupDialog()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleClosePopupDialog()
{
}

// EOF
