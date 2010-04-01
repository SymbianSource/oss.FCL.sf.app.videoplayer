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
* Description:   Implementation of Video base playback view
*
*/

// Version : %version: da1mmcf#28 %



//  Include Files



#include <qtimer>
#include <qgraphicssceneevent>

#include <hbmenu.h>
#include <hbaction.h>
#include <hbgesture.h>
#include <hbinstance.h>
#include <hbnotificationdialog.h>

#include <textresolver.h>
#include <mmf/common/mmferrors.h>
#include <drmhelper.h>

#include <mpxvideoplaybackdefs.h>
#include <mpxplaybackcommanddefs.h>

#include "mpxvideo_debug.h"
#include "mpxvideoviewwrapper.h"
#include "mpxvideoplaybackuids.hrh"
#include "hbvideobaseplaybackview.h"
#include "mpxcommonvideoplaybackview.hrh"


//  Member Functions

// -------------------------------------------------------------------------------------------------
// HbVideoBasePlaybackView::HbVideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoBasePlaybackView::HbVideoBasePlaybackView()
    : mVideoMpxWrapper( NULL )
    , mTimerForClosingView( NULL )
    , mActivated( false )
{
    MPX_ENTER_EXIT(_L("HbVideoBasePlaybackView::HbVideoBasePlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::initializeVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::initializeVideoPlaybackView()
{
    MPX_ENTER_EXIT(_L("HbVideoBasePlaybackView::initializeVideoPlaybackView()"));

    //
    // Need to set to control full screen including status pane area
    //
    setContentFullScreen();

    mTimerForClosingView = new QTimer();
    mTimerForClosingView->setSingleShot( true );
    mTimerForClosingView->setInterval( 10 );
    connect( mTimerForClosingView, SIGNAL( timeout() ), this, SIGNAL( activatePreviousView() ) );
}

// -------------------------------------------------------------------------------------------------
// HbVideoBasePlaybackView::~HbVideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoBasePlaybackView::~HbVideoBasePlaybackView()
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::~HbVideoBasePlaybackView()"));

    disconnect( mTimerForClosingView, SIGNAL( timeout() ), this, SIGNAL( activatePreviousView() ) );

    if ( mTimerForClosingView )
    {
        if ( mTimerForClosingView->isActive() )
        {
            mTimerForClosingView->stop();
        }

        delete mTimerForClosingView;
        mTimerForClosingView = NULL;
    }

    if ( mVideoMpxWrapper )
    {
        delete mVideoMpxWrapper;
        mVideoMpxWrapper = NULL;
    }

    setParentItem( 0 );
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleActivateView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleActivateView()
{
    MPX_ENTER_EXIT(_L("HbVideoBasePlaybackView::handleActivateView()"));

    menu()->close();

    hideItems( Hb::AllItems );

    //
    // Landscape orientation
    //
    hbInstance->allMainWindows()[0]->setOrientation( Qt::Horizontal );

    TRAP_IGNORE( mVideoMpxWrapper = CMPXVideoViewWrapper::NewL( this ) );

    mActivated = true;

    //
    //  Retrieve PDL information for container
    //
    retrievePdlInformation();

    //
    //  Request the needed Media from the Playback Plugin
    //
    TRAP_IGNORE( mVideoMpxWrapper->RequestMediaL() );
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleDeactivateView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleDeactivateView()
{
    MPX_ENTER_EXIT(_L("HbVideoBasePlaybackView::handleDeactivateView()"));

    mActivated = false;

    // 
    //  Close the playback plugin to release all references to previous clip
    //
    MPX_TRAPD( err, mVideoMpxWrapper->HandleCommandL( EMPXPbvCmdClose ) );

    if ( mVideoMpxWrapper )
    {
        delete mVideoMpxWrapper;
        mVideoMpxWrapper = NULL;
    }

    //
    // go back to device orientation
    //
    //hbInstance->allMainWindows()[0]->unsetOrientation();

    showItems( Hb::AllItems );
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleClosePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleClosePlaybackView()
{
    MPX_DEBUG(
      _L("HbVideoBasePlaybackView::handleClosePlaybackView()") );

    if ( mVideoMpxWrapper->IsMultiItemPlaylist() )
    {
        TRAP_IGNORE( mVideoMpxWrapper->HandleCommandL( EMPXPbvCmdNextListItem ) );
    }
    else
    {
        closePlaybackView();
    }
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handlePluginError()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handlePluginError( int aError )
{
    MPX_DEBUG(
      _L("HbVideoBasePlaybackView::handlePluginError() aError = %d"), aError );

    switch ( aError )
    {
        case KErrCancel:
		{
            handleClosePlaybackView();
            break;
        }
        case KErrMMDRMNotAuthorized:
        case KErrCANoRights:
        case KErrCANoPermission:
        {
            const QString qString = "License has expired or it is missing";
            displayErrorMessage( qString );
            handleClosePlaybackView();
            break;
        }
        case KMPXVideoCallOngoingError:
        {
            const QString qString = "FMP cannot be used during video call";
            displayInfoMessage( qString );
            break;
        }
        case KMPXVideoTvOutPlaybackNotAllowed:
        {
            const QString qString = "Protected clip, can not be played through TV-Out";
            displayInfoMessage( qString );
            break;
        }
        case KMPXVideoTvOutPlaybackNotAllowedClose:
        {
            const QString qString = "Protected clip, can not be played through TV-Out";
            displayInfoMessage( qString );
            handleClosePlaybackView();
            break;
        }
        default:
        {
            TRAP_IGNORE(  

                //
                //  TextResolver instance for error resolving.
                //
                CTextResolver* textresolver = CTextResolver::NewLC();
                
                //
                //  Resolve the error text
                //
                TPtrC text;
                text.Set( textresolver->ResolveErrorString( aError ) );
    
                //
                // convert to QString
                //
                const QString qString( (QChar*)text.Ptr(), text.Length() );
                displayErrorMessage( qString );
    
                CleanupStack::PopAndDestroy( textresolver );
    
                handleClosePlaybackView();
            
            );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::displayInfoMessage
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::displayInfoMessage(  const QString& qString )
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::displayInfoMessage()"));

    HbNotificationDialog::launchDialog( qString );             
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::displayErrorMessage
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::displayErrorMessage( const QString& qString )
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::displayErrorMessage()"));

    HbNotificationDialog::launchDialog( qString );
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::doClosePlayer
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::doClosePlayer()
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::doClosePlayer()"));

    handleDeactivateView();

    qApp->quit();
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::issuePlayCommand
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::issuePlayCommand()
{
    MPX_ENTER_EXIT(_L("HbVideoBasePlaybackView::issuePlayCommand()"));
    TRAPD(err, mVideoMpxWrapper->CreateGeneralPlaybackCommandL( EPbCmdPlay ));
    MPX_DEBUG(_L("HbVideoBasePlaybackView::issuePlayCommand() error = %d"), err);
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::retrievePdlInformation
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::retrievePdlInformation()
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::retrievePdlInformation()"));
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleBufferingState
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleBufferingState()
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::handleBufferingState()"));

}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::closePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::closePlaybackView()
{
    MPX_ENTER_EXIT(_L("HbVideoBasePlaybackView::closePlaybackView()"));

    mTimerForClosingView->start( 0 );    
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::event()
// -------------------------------------------------------------------------------------------------
//
bool HbVideoBasePlaybackView::event( QEvent *event )
{
    bool consumed = false;

    if ( event->type() == QEvent::WindowActivate && mActivated )
    {
        TRAP_IGNORE( mVideoMpxWrapper->IssueVideoAppForegroundCmdL( true ) );
    }
    else if ( event->type() == QEvent::WindowDeactivate && mActivated )
    {
        TRAP_IGNORE( mVideoMpxWrapper->IssueVideoAppForegroundCmdL( false ) );
    }
    else
    {
         consumed = QGraphicsWidget::event( event );
    }

    return consumed;
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::closePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::paint( QPainter *painter,
                                     const QStyleOptionGraphicsItem *option,
                                     QWidget *widget )
{
    Q_UNUSED( widget );

    painter->fillRect( option->exposedRect, Qt::black );
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
// HbVideoBasePlaybackView::mousePress
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    //
    // Needed for gesture framework
    //
    event->accept();
}

// -------------------------------------------------------------------------------------------------
// HbVideoBasePlaybackView::mouseReleaseEvent
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    MPX_ENTER_EXIT(_L("HbVideoBasePlaybackView::mouseReleaseEvent"));

    Q_UNUSED( event );

    emit tappedOnScreen();
}

// EOF
