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

// Version : %version:  8 %



#include <qfileinfo>
#include <qgraphicsscenemouseevent>

#include <hblabel.h>
#include <hbpushbutton.h>

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
        HbPushButton *playButton = qobject_cast<HbPushButton*>( widget );
        playButton->setFlag( QGraphicsItem::ItemIsFocusable, false );

        connect( playButton, SIGNAL( released() ), this, SLOT( playPause() ) );
        
        QGraphicsItem *widget1 = playButton->primitive( HbStyle::P_PushButton_background );
        widget1->setVisible( false );
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

    QMPXVideoPlaybackDocumentLoader *loader = mController->layoutLoader();

    QGraphicsWidget *widget = loader->findWidget( QString( "detailsViewPlayButton" ) );

    switch ( state )
    {
        case EPbStatePaused:
        {
            widget->setVisible( true );
            break;
        }
        default:
        {
            widget->setVisible( false );
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
//   HbVideoBasePlaybackView::event()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackDetailsPlaybackWindow::event( QEvent *event )
{
    bool consumed = false;

    //
    // Ignore touch event if the gesture event was just received and is being processed
    //
    if ( event->type() == QEvent::UngrabMouse )
    {
        playPause();
        consumed = true;
    }
    else
    {
         consumed = QGraphicsWidget::event( event );
    }

    return consumed;
}

//End of file
