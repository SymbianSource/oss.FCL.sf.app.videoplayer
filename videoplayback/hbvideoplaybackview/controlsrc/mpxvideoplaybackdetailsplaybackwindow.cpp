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

// Version : %version:  13 %



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
        frameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_multimedia_trans" );
        frameItem->frameDrawer().setFrameType( HbFrameDrawer::NinePieces );
        frameItem->frameDrawer().setFillWholeRect( true );
        
        //
        // create 'attach' button and connect corresponding signal/slot
        //
        QGraphicsWidget *detailsAttachWidget = loader->findWidget( QString( "detailsAttachButton" ) );
        HbPushButton *attachButton = qobject_cast<HbPushButton*>( detailsAttachWidget );
        connect( attachButton, SIGNAL( released() ), mController, SLOT( attachVideo() ) );

        //
        // create 'share' button
        //     signal and slot to be created when requirement for 'share' operation is confirmed
		//
        QGraphicsWidget *detailsShareWidget = loader->findWidget( QString( "detailsShareButton" ) );
        HbPushButton *shareButton = qobject_cast<HbPushButton*>( detailsShareWidget );
        
        //
        // by default in xml layout, attachButton is not visible while shareButton is visible.
        // if it's an 'attach' operation, reverse the visibility order
        //
        if ( mController->isAttachOperation() )
        {
            attachButton->setVisible( true );
            shareButton->setVisible( false );
        }    
        
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
