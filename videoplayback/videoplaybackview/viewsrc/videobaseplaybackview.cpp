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

// Version : %version: da1mmcf#47 %



//  Include Files



#include <QTimer>
#include <QGraphicsSceneEvent>

#include <hbmenu.h>
#include <hbaction.h>
#include <hbinstance.h>
#include <hbnotificationdialog.h>
#include <hblabel.h>
#include <hbtapgesture.h>
#include <hbpangesture.h>

#include <textresolver.h>
#include <mmf/common/mmferrors.h>
#include <DRMHelper.h>

#include <mpxvideoplaybackdefs.h>
#include <mpxplaybackcommanddefs.h>
#include <mpxhbvideocommondefs.h>

#include "mpxvideo_debug.h"
#include "mpxvideoviewwrapper.h"
#include "mpxvideoplaybackuids.hrh"
#include "videobaseplaybackview.h"
#include "videoactivitystate.h"
#include "videoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"


//  Member Functions

// -------------------------------------------------------------------------------------------------
// VideoBasePlaybackView::VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::VideoBasePlaybackView()
    : mVideoMpxWrapper( NULL )
    , mTimerForClosingView( NULL )
    , mActivated( false )
    , mStayPaused ( false )
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::VideoBasePlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::initializeVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::initializeVideoPlaybackView()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::initializeVideoPlaybackView()"));

    //
    // Need to set to control full screen including status pane area
    //
    setContentFullScreen();

    mTimerForClosingView = new QTimer();
    mTimerForClosingView->setSingleShot( true );
    mTimerForClosingView->setInterval( 10 );
    connect( mTimerForClosingView, SIGNAL( timeout() ), this, SIGNAL( activatePreviousView() ) );

    //
    // Read activity data ...
    // If last playback was forced to terminate due to low memory (GOOM, etc.)
    // then the clip needs to be restored to the last played position
    // and the state needs to be paused, since forced termination can only occur for
    // background apps - so if this happened Video Player must have been in background
    // which implies paused state
    //
    int lastViewType = VideoActivityState::instance().getActivityData(KEY_VIEWPLUGIN_TYPE).toInt();
    if ( lastViewType == MpxHbVideoCommon::PlaybackView )
    {
        QVariant data = VideoActivityState::instance().getActivityData(KEY_LAST_PLAY_POSITION_ID);
        mLastPlayPosition = data.toInt();

        mStayPaused = true;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoBasePlaybackView::~VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::~VideoBasePlaybackView()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::~VideoBasePlaybackView()"));

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
//   VideoBasePlaybackView::handleActivateView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleActivateView()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::handleActivateView()"));

    MPX_TRAPD( err, mVideoMpxWrapper = CMPXVideoViewWrapper::NewL( this ) );

    QCoreApplication::instance()->installEventFilter( this );

    //
    //  Request the needed Media from the Playback Plugin
    //
    MPX_TRAP( err, mVideoMpxWrapper->RequestMediaL() );

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
//   VideoBasePlaybackView::handleDeactivateView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleDeactivateView()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::handleDeactivateView()"));

    saveActivityData();

    mActivated = false;

    QCoreApplication::instance()->removeEventFilter( this );

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
//   VideoBasePlaybackView::saveActivityData()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::saveActivityData()
{
    MPX_DEBUG( _L("VideoBasePlaybackView::saveActivityData()") );

    // save the activity data
    QVariant data = QString( mVideoMpxWrapper->iFileDetails->mClipName );
    VideoActivityState::instance().setActivityData(data, KEY_LAST_PLAYED_CLIP);

    data = int( mVideoMpxWrapper->iPlayPosition );
    VideoActivityState::instance().setActivityData(data, KEY_LAST_PLAY_POSITION_ID);

    data = bool( mVideoMpxWrapper->iFileDetails->mPlaybackMode == EMPXVideoLocal );
    VideoActivityState::instance().setActivityData(data, KEY_LAST_LOCAL_PLAYBACK);    
    
    data = uint ( mVideoMpxWrapper->GetMediaId() );
    VideoActivityState::instance().setActivityData(data, KEY_LAST_PLAYED_MEDIA_ID);
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleClosePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleClosePlaybackView()
{
    MPX_DEBUG(
      _L("VideoBasePlaybackView::handleClosePlaybackView()") );

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
//   VideoBasePlaybackView::handlePluginError()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handlePluginError( int aError )
{
    MPX_DEBUG(
      _L("VideoBasePlaybackView::handlePluginError() aError = %d"), aError );

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
//   VideoBasePlaybackView::showDialog
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::showDialog( const QString& string, bool closeView )
{
    MPX_DEBUG(_L("VideoBasePlaybackView::showDialog( %s, %d )"), string.data(), closeView );

    //
    // create pop-up dialog for error notes,
    //     set its position to the middle of the screen
    //     and make sure pop-up dialog gets deleted on close.
    //
    QRectF screenRect = hbInstance->allMainWindows()[0]->rect();
    HbNotificationDialog* dlg = new HbNotificationDialog();
    dlg->setAttribute( Qt::WA_DeleteOnClose );

	if ( closeView )
	{
        //
        // connect aboutToClose() signal to handleClosePopupDialog() slot so that
        // when pop-up dialog is closed, playback view is closed also
        //
        connect( dlg, SIGNAL( aboutToClose() ), this, SLOT( handleClosePopupDialog() ) );
    }

	dlg->setTitle( string );
    dlg->show();
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::doClosePlayer
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::doClosePlayer()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::doClosePlayer()"));

    handleDeactivateView();

    qApp->quit();
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::issuePlayCommand
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::issuePlayCommand()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::issuePlayCommand()"));

    int err = KErrNone;

    TRAP( err, mVideoMpxWrapper->CreateGeneralPlaybackCommandL( EPbCmdPlay ));

    MPX_DEBUG(_L("VideoBasePlaybackView::issuePlayCommand() error = %d"), err);
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::retrievePdlInformation
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::retrievePdlInformation()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::retrievePdlInformation()"));
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleBufferingState
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleBufferingState()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::handleBufferingState()"));
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::closePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::closePlaybackView()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::closePlaybackView()"));

    mTimerForClosingView->start( 0 );
}

// ---------------------------------------------------------------------------
// VideoBasePlaybackView::eventFilter
// ---------------------------------------------------------------------------
//
bool VideoBasePlaybackView::eventFilter( QObject *object, QEvent *event )
{
    Q_UNUSED( object );

    switch ( event->type() )
    {
        case QEvent::ApplicationActivate:
        {
            if ( mActivated )
            {
                MPX_DEBUG(_L("VideoBasePlaybackView::eventFilter foreground()") );
                TRAP_IGNORE( mVideoMpxWrapper->IssueVideoAppForegroundCmdL( true ) );
            }
            break;
        }
        case QEvent::ApplicationDeactivate:
        {
            if ( mActivated )
            {
                MPX_DEBUG(_L("VideoBasePlaybackView::eventFilter background()") );
                TRAP_IGNORE( mVideoMpxWrapper->IssueVideoAppForegroundCmdL( false ) );
            }
            break;
        }
    }

    return QObject::eventFilter( object, event );
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::getWindow()
// -------------------------------------------------------------------------------------------------
//
RWindow *VideoBasePlaybackView::getWindow()
{
    return static_cast<RWindow*>( hbInstance->allMainWindows()[0]->effectiveWinId()->DrawableWindow() );
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleClosePopupDialog()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleClosePopupDialog()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::handleClosePopupDialog()") );

    handleClosePlaybackView();
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::gestureEvent()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::gestureEvent( QGestureEvent* event )
{
    if ( HbTapGesture * gesture = static_cast<HbTapGesture *>( event->gesture( Qt::TapGesture ) ) )
    {
        if ( gesture->state() == Qt::GestureFinished && mActivated )
        {
            MPX_DEBUG(_L("VideoBasePlaybackView::gestureEvent() tapGesture finished") );

            emit tappedOnScreen();
        }
    }
    else if ( HbPanGesture* gesture = static_cast<HbPanGesture*>( event->gesture( Qt::PanGesture ) ) )
    {
        if ( gesture->state() == Qt::GestureFinished && mActivated )
        {
            QPointF delta( gesture->sceneDelta() );

            if ( delta.x() > 0 )
            {
                MPX_DEBUG(_L("VideoBasePlaybackView::gestureEvent() right") );

                emit pannedToRight();
            }
            else
            {
                MPX_DEBUG(_L("VideoBasePlaybackView::gestureEvent() left") );

                emit pannedToLeft();
            }
        }
    }
}

// EOF
