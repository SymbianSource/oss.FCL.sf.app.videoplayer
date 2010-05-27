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

// Version : %version: da1mmcf#40 %



//  Include Files



#include <QTimer>
#include <QGraphicsSceneEvent>

#include <hbmenu.h>
#include <hbaction.h>
#include <hbinstance.h>
#include <hbnotificationdialog.h>
#include <hblabel.h>

#include <textresolver.h>
#include <mmf/common/mmferrors.h>
#include <DRMHelper.h>

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

    if ( mTimerForClosingView )
    {        
        disconnect( mTimerForClosingView, SIGNAL( timeout() ), this, SIGNAL( activatePreviousView() ) );
        
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

    TRAP_IGNORE( mVideoMpxWrapper = CMPXVideoViewWrapper::NewL( this ) ); 

    QCoreApplication::instance()->installEventFilter( this );

    //
    //  Request the needed Media from the Playback Plugin
    //
    TRAP_IGNORE( mVideoMpxWrapper->RequestMediaL() ); 
    
    menu()->close();

    //
    // Landscape orientation
    // Workaround: Disable orientation transition effect 
	// since orbit couldn't emit the orientationChanged signal with transition effect
	// 
    hbInstance->allMainWindows()[0]->setOrientation( Qt::Horizontal, false );  

    mActivated = true;   
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleDeactivateView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleDeactivateView()
{
    MPX_ENTER_EXIT(_L("HbVideoBasePlaybackView::handleDeactivateView()"));

    QCoreApplication::instance()->removeEventFilter( this );

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
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleClosePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleClosePlaybackView()
{
    MPX_DEBUG(
      _L("HbVideoBasePlaybackView::handleClosePlaybackView()") );

    if ( mVideoMpxWrapper && mVideoMpxWrapper->IsMultiItemPlaylist() )
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
            showDialog( hbTrId( "txt_videos_info_license_has_expired_or_it_is_missi" ) );
            break;
        }
        case KMPXVideoCallOngoingError:
        {
            showDialog( hbTrId( "txt_videos_info_video_playback_is_not_allowed_duri" ), false );
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
                
                //
                // clean up textresolver
                //
                CleanupStack::PopAndDestroy( textresolver );

                //
                // display error and close playback view
                //
                showDialog( qString );
    
            );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::showDialog
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::showDialog( const QString& qString, bool closeView )
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::showDialog( %s, %d )"), qString.data(), closeView );
        
    //
    // create pop-up dialog for error notes, 
    //     set its position to the middle of the screen
    //     and make sure pop-up dialog gets deleted on close.
    //
    QRectF screenRect = hbInstance->allMainWindows()[0]->rect();
    HbNotificationDialog* dlg = new HbNotificationDialog();    
    dlg->setAttribute( Qt::WA_DeleteOnClose );
    dlg->setMinimumSize( QSizeF(200, 100) );    
    dlg->setPreferredPos( QPointF( screenRect.height()/2, screenRect.width()/2 ), 
                          HbPopup::Center );
    
	if ( closeView )
	{
        //
        // connect aboutToClose() signal to handleClosePopupDialog() slot so that
        // when pop-up dialog is closed, playback view is closed also
        //
        connect( dlg, SIGNAL( aboutToClose() ), this, SLOT( handleClosePopupDialog() ) );        
    }
	
    //
    // convert string to label so text alignment can be set
    //
    HbLabel *label = new HbLabel( qString );
    label->setAlignment( Qt::AlignCenter );
    
    //
    // set label as content widget and show pop-up dialog
    //
    dlg->setContentWidget( label );
    dlg->show();    
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

    int err = KErrNone;

    TRAP( err, mVideoMpxWrapper->CreateGeneralPlaybackCommandL( EPbCmdPlay ));

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

// ---------------------------------------------------------------------------
// HbVideoBasePlaybackView::eventFilter
// ---------------------------------------------------------------------------
//
bool HbVideoBasePlaybackView::eventFilter( QObject *object, QEvent *event )
{
    Q_UNUSED( object );

    switch ( event->type() )
    {
        case QEvent::ApplicationActivate:
        {
            if ( mActivated )
            {
                MPX_DEBUG(_L("HbVideoBasePlaybackView::eventFilter foreground()") );
                TRAP_IGNORE( mVideoMpxWrapper->IssueVideoAppForegroundCmdL( true ) );
            }
            break;
        }
        case QEvent::ApplicationDeactivate:
        {
            if ( mActivated )
            {
                MPX_DEBUG(_L("HbVideoBasePlaybackView::eventFilter background()") );
                TRAP_IGNORE( mVideoMpxWrapper->IssueVideoAppForegroundCmdL( false ) );
            }
            break;
        }
    }

    return QObject::eventFilter( object, event );
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

// -------------------------------------------------------------------------------------------------
//   HbVideoBasePlaybackView::handleClosePopupDialog()
// -------------------------------------------------------------------------------------------------
//
void HbVideoBasePlaybackView::handleClosePopupDialog()
{
    MPX_DEBUG(_L("HbVideoBasePlaybackView::handleClosePopupDialog()") );

    handleClosePlaybackView();
}

// EOF
