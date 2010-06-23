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
* Description:  Implementation of QMPXVideoPlaybackDetailsPlaybackWindow
*
*/

// Version : %version:  17 %



#include <QGraphicsSceneMouseEvent>

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

        //
        // Set framedrawer for semi transparent background
        //
        HbFrameDrawer *drawer = mPlayButton->frameBackground();

        if( drawer == NULL )
        {
            drawer = new HbFrameDrawer();
        }

        drawer->setFillWholeRect( true );
        drawer->setFrameType( HbFrameDrawer::NinePieces );
        drawer->setFrameGraphicsName( "qtg_fr_multimedia_trans" );
        mPlayButton->setFrameBackground( drawer );

        //
        // create 'attach' button and connect corresponding signal/slot
        //
        QGraphicsWidget *detailsAttachWidget = loader->findWidget( QString( "detailsAttachButton" ) );
        HbPushButton *attachButton = qobject_cast<HbPushButton*>( detailsAttachWidget );
        connect( attachButton, SIGNAL( released() ), mController, SLOT( attachVideo() ) );

        //
        // create 'share' button and connect corresponding signal/slot
        //
        QGraphicsWidget *detailsShareWidget = loader->findWidget( QString( "detailsShareButton" ) );
        HbPushButton *shareButton = qobject_cast<HbPushButton*>( detailsShareWidget );
        connect( shareButton, SIGNAL( released() ), mController, SLOT( sendVideo() ) );

        //
        // by default in xml layout, attachButton is not visible while shareButton is visible.
        // if it's an 'attach' operation, reverse the visibility order
        //
        if ( mController->isAttachOperation() )
        {
            attachButton->setVisible( true );
            shareButton->setVisible( false );
        }
        else
        {
            //
            // dim "share" button for streaming
            //
            if ( mController->fileDetails()->mPlaybackMode == EMPXVideoStreaming ||
                 mController->fileDetails()->mPlaybackMode == EMPXVideoLiveStreaming )
            {
                shareButton->setEnabled( false );                  
            }            
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

//End of file
