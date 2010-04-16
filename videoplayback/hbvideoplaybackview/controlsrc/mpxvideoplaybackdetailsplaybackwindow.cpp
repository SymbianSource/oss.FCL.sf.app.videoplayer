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
* Description:  Implementation of QMPXVideoPlaybackProgressBar
*
*/

// Version : %version:  11 %



#include <QFileInfo>
#include <QGraphicsSceneMouseEvent>


#include <hblabel.h>
#include <hbframeitem.h>
#include <hbpushbutton.h>
#include <hbframedrawer.h>

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackdetailsplaybackwindow.h"

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDetailsPlaybackWindow::QMPXVideoPlaybackDetailsPlaybackWindow
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackDetailsPlaybackWindow::QMPXVideoPlaybackDetailsPlaybackWindow( 
        QMPXVideoPlaybackControlsController* controller )
    : mController( controller )
    , mInitialized( false )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackDetailsPlaybackWindow::QMPXVideoPlaybackDetailsPlaybackWindow()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDetailsPlaybackWindow::~QMPXVideoPlaybackDetailsPlaybackWindow
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackDetailsPlaybackWindow::~QMPXVideoPlaybackDetailsPlaybackWindow()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackDetailsPlaybackWindow::~QMPXVideoPlaybackDetailsPlaybackWindow()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDetailsPlaybackWindow::initialize
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackDetailsPlaybackWindow::initialize()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackDetailsPlaybackWindow::initialize()"));

    QMPXVideoPlaybackDocumentLoader *loader = mController->layoutLoader();

    //
    // Don't need to initialize buttons once it gets initialized
    //
    if ( loader && ! mInitialized )
    {
        mInitialized = true;

        QMPXVideoPlaybackViewFileDetails *details = mController->fileDetails();

        //
        // Connect signal and slot for play button
        //
        QGraphicsWidget *widget = loader->findWidget( QString( "detailsViewPlayButton" ) );
        mPlayButton = qobject_cast<HbPushButton*>( widget );
        mPlayButton->setFlag( QGraphicsItem::ItemIsFocusable, false );

        connect( mPlayButton, SIGNAL( released() ), this, SLOT( playPause() ) );
        
        QGraphicsItem *widget1 = mPlayButton->primitive( HbStyle::P_PushButton_background );
        widget1->setVisible( false );

        //
        // Set framedrawer for semi transparent background
        //
        HbFrameItem *frameItem = new HbFrameItem ( mPlayButton );
        frameItem->setGeometry( mPlayButton->boundingRect() );
        frameItem->frameDrawer().setFrameType( HbFrameDrawer::OnePiece );
        frameItem->frameDrawer().setFillWholeRect( true );
        frameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_status_trans_normal_c" );
    }

    updateState( mController->state() );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDetailsPlaybackWindow::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackDetailsPlaybackWindow::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackDetailsPlaybackWindow::updateState() state = %d"), state );

    switch ( state )
    {
        case EPbStatePaused:
        {
            mPlayButton->setVisible( true );
            break;
        }
        default:
        {
            mPlayButton->setVisible( false );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDetailsPlaybackWindow::playPause()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackDetailsPlaybackWindow::playPause()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackDetailsPlaybackWindow::playPause"));

    mController->handleCommand( EMPXPbvCmdPlayPause );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDetailsPlaybackWindow::mousePressEvent
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackDetailsPlaybackWindow::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackDetailsPlaybackWindow::mousePressEvent"));

    event->accept();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackDetailsPlaybackWindow::mouseReleaseEvent
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackDetailsPlaybackWindow::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackDetailsPlaybackWindow::mouseReleaseEvent"));

    Q_UNUSED( event );

    playPause();
}

//End of file
