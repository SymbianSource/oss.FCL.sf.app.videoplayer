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
* Description:  Implementation of MPXVideoPlaybackControlsController
*
*/

// Version : %version: da1mmcf#25 %



// INCLUDE FILES
#include <coecntrl.h>
#include <bautils.h>
#include <barsread.h>
#include <stringloader.h>
#include <f32file.h>

#include <qtimer>
#include <qfileinfo>
#include <thumbnailmanager_qt.h>

#include <hblabel.h>
#include <hbaction.h>
#include <hbiconitem.h>
#include <hbratingslider.h>
#include <hbiconanimator.h>
#include <hbtransparentwindow.h>

#include "mpxvideoviewwrapper.h"
#include "hbvideobaseplaybackview.h"
#include "mpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxvideoplaybackcontrolpolicy.h"
#include "mpxvideoplaybackdocumentloader.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackstatuspanecontrol.h"
#include "mpxvideoplaybackfiledetailswidget.h"
#include "mpxvideoplaybacknontouchvolumebar.h"
#include "mpxvideoplaybackfullscreencontrol.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackcontrolconfiguration.h"
#include "mpxvideoplaybackdetailsplaybackwindow.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController(
        HbVideoBasePlaybackView *view,
        CMPXVideoViewWrapper *viewWrapper,
        QMPXVideoPlaybackViewFileDetails *details )
    : mView( view )
    , mViewWrapper( viewWrapper )
    , mFileDetails( details )
    , mControlsPolicy( NULL )
    , mControlsConfig( NULL )
    , mControlsTimer( NULL )
    , mLoader( NULL )
    , mVolumeControl( NULL )
    , mThumbnailManager( NULL )
    , mViewTransitionIsGoingOn( false )
    , mThumbNailState( EThumbNailEmpty )
    , mState( EPbStateNotInitialised )
    , mViewMode( EFullScreenView )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::QMPXVideoPlaybackControlsController()"));

    initializeController();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::initializeController()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::initializeController()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::initializeController()"));

    setParent( mView );

    mControlsPolicy = new QMPXVideoPlaybackControlPolicy();
    mControlsConfig = new QMPXVideoPlaybackControlConfiguration( this );

    connect( mControlsConfig, SIGNAL( controlListUpdated() ), this, SLOT( controlsListUpdated() ) );
    
    mControlsTimer = new QTimer( this );
    mControlsTimer->setInterval( KMPXControlsTimeOut );
    mControlsTimer->setSingleShot( false );
    connect( mControlsTimer, SIGNAL( timeout() ), this, SLOT( hideAllControls() ) );

    connect( mView, SIGNAL( tappedOnScreen() ), this, SLOT( handleTappedOnScreen() ) );

    //
    // Create layout loader
    //
    bool ok = false;
    mLoader = new QMPXVideoPlaybackDocumentLoader( this );
    mLoader->load( KMPXPLAYBACKVIEW_XML, &ok );

    if ( ok )
    {
        QGraphicsWidget *widget = mLoader->findWidget( QString( "content" ) );
        mView->setWidget( widget );

        //
        // Update controls list based on configuration + volume
        //
        controlsListUpdated();

        widget = mLoader->findWidget( QString( "volumeSlider" ) );
        mVolumeControl = qobject_cast<QMPXVideoPlaybackNonTouchVolumeBar*>( widget );
    }
    else
    {
        MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::initializeController()- can't find xml"));

        //
        // Can't find xml for layout. Delete mLoader
        //
        delete mLoader;
        mLoader = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::~QMPXVideoPlaybackControlsController"));

    disconnect( mControlsConfig, SIGNAL( controlListUpdated() ), this, SLOT( controlsListUpdated() ) );

    disconnect( mView, SIGNAL( tappedOnScreen() ), this, SLOT( handleTappedOnScreen() ) );
    disconnect( mControlsTimer, SIGNAL( timeout() ), this, SLOT( hideAllControls() ) );

    mView->setWidget( NULL );

    mControls.clear();

    if ( mControlsTimer )
    {
        delete mControlsTimer;
        mControlsTimer = NULL;
    }

    if ( mControlsPolicy )
    {
        delete mControlsPolicy;
        mControlsPolicy = NULL;
    }

    if ( mControlsConfig )
    {
        delete mControlsConfig;
        mControlsConfig = NULL;
    }

    if ( mLoader )
    {
        delete mLoader;
        mLoader = NULL;
    }

    if ( mThumbnailManager )
    {
        delete mThumbnailManager;
        mThumbnailManager = 0;                    
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::addFileDetails()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::addFileDetails(
    QMPXVideoPlaybackViewFileDetails* details )
{
    //
    // If it is not local, hide the star rating
    //
    if ( details->mPlaybackMode != EMPXVideoLocal )
    {
        QString sectionName = "nonLocalPlayback";

        bool ok = false;
        mLoader->load( KMPXPLAYBACKVIEW_XML, sectionName, &ok );
    }

    mFileDetails = details;

    mFileDetails->mRNFormat = realFormat( mFileDetails->mClipName );

    mControlsConfig->updateControlsWithFileDetails();

    //
    // for audio-only clips and tv-out, default view is flip view
    //
    if ( ! details->mVideoEnabled )
    {
        changeViewMode( EAudioOnlyView, false );
    }

    if ( details->mTvOutConnected )
    {
        handleEvent( EMPXControlCmdTvOutConnected, details->mTvOutPlayAllowed );
    }

    //
    // Dimmed the volume control if it is video only
    //
    if ( ! mFileDetails->mAudioEnabled )
    {
        mVolumeControl->setValue( 0 );
        mVolumeControl->setEnabled( false );
    }

    //
    // If title is not available, show clip name
    //
    QString title = mFileDetails->mTitle;

    if ( title.count() == 0 )
    {
        QFileInfo fileInfo( mFileDetails->mClipName );
        title = fileInfo.baseName ();
    }

    QGraphicsWidget *widget = mLoader->findWidget( QString( "title" ) );
    HbLabel *titleLabel = qobject_cast<HbLabel*>( widget );
    titleLabel->setPlainText( title );
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::handleEvent
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleEvent(
    TMPXVideoPlaybackControlCommandIds event, int value )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::handleEvent(%d)"), event);

    switch ( event )
    {
        case EMPXControlCmdSetPosition:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdSetPosition [%d]]"), value );

            positionChanged( (qreal)value / (qreal)KPbMilliMultiplier);
            break;
        }
        case EMPXControlCmdSetDuration:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdSetDuration [%d]]"), value );

            durationChanged( (qreal)value / (qreal)KPbMilliMultiplier );
            break;
        }
        case EMPXControlCmdStateChanged:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdStateChanged]"));

            handleStateChange( (TMPXPlaybackState)value );
            break;
        }
        case EMPXControlCmdSetVolume:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdSetVolume [%d]]"), value );

            volumeChanged( value );
            break;
        }
        case EMPXControlCmdSetAspectRatio:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdSetAspectRatio %d]"), value );

            aspectRatioChanged( value );

            break;
        }
        case EMPXControlCmdSetDownloadSize:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdSetDownloadSize [%d]]"), value );
            setDownloadSize( value );
            break;
        }
        case EMPXControlCmdDownloadUpdated:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdDownloadUpdated [%d]]"), value );
            updateDownloadPosition( value );
            break;
        }
        case EMPXControlCmdDownloadComplete:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdDownloadComplete [%d]]"), value);
            updateDownloadPosition( value );
            break;
        }
        case EMPXControlCmdSetDownloadPaused:
        case EMPXControlCmdClearDownloadPaused:
        {
            mControlsConfig->updateControlList( event );
            break;
        }
        case EMPXControlCmdTvOutConnected:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdTvOutConnected]"));

            handleTvOutEvent( true, event, value );
            break;
        }
        case EMPXControlCmdTvOutDisconnected:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdTvOutDisConnected]"));

            handleTvOutEvent( false, event, value );
            break;
        }
        case EMPXControlCmdHandleErrors:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdHandleErrors]"));

            handleErrors();

            break;
        }
        case EMPXControlCmdShowVolumeControls:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdShowVolumeControls]"));

            showVolumeControls();
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::handleStateChange
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleStateChange( TMPXPlaybackState newState )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::handleStateChange()"),
                   _L("new state = %d"), newState );

    //
    // Somehow EPbStatePlaying, EPbStatePaused gets called twice continously
    //
    if ( newState != mState )
    {
        mState = newState;

        switch ( newState )
        {
            case EPbStateInitialised:
            {
                //
                //  remove branding control when the state is transitioned to Initialized
                //
                if ( mFileDetails->mPlaybackMode == EMPXVideoStreaming ||
                     mFileDetails->mPlaybackMode == EMPXVideoLiveStreaming )
                {
                    mControlsConfig->updateControlList( EMPXControlCmdPluginInitialized ); 
                }
                
                break;                
            }
            case EPbStatePlaying:
            case EPbStateInitialising:
            case EPbStateBuffering:
            case EPbStatePaused:
            case EPbStateNotInitialised:
            {
                updateState();

                //
                //  Show all the controls
                //
                showControls();

                break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::controlsListUpdated()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::controlsListUpdated()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::controlsListUpdated()"));

    hideAllControls();

    QList<TMPXVideoPlaybackControls>& updatedList = mControlsConfig->controlList();

    QList<TMPXVideoPlaybackControls> myList = updatedList;

    int controlCount = mControls.count();

    int i = 0 ;
    int index = KErrNotFound;


    for ( int iCnt = 0 ; iCnt < controlCount ; iCnt++ )
    {
        index = myList.indexOf( mControls[i]->controlIndex() );

        if ( index == KErrNotFound )
        {
            //
            //  Delete control since it doesn't exist in new list
            //
            mControls.removeAt( i );
        }
        else
        {
            //
            //  Control exists in new list.
            //  Update the policy property based on file details and view mode to the controls
            //
            TUint properties = 0;
            mControlsPolicy->setControlProperties( 
                    mControls[i]->controlIndex(), properties, mFileDetails, mViewMode );
            mControls[i]->updateControlProperties( properties );

            //
            //  Control exists in new list.
            //
            myList.removeAt( index );
            i++;            
        }
    }

    //
    //  The updated list will contain added controls only
    //
    for ( int j = 0 ; j < myList.count() ; j++ )
    {
        appendControl( myList[j] );
    }

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        mControls[i]->updateControlsWithFileDetails( mFileDetails );
    }

    showControls();
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::appendControl()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::appendControl( TMPXVideoPlaybackControls controlIndex )
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::appendControl()"),
                   _L("control index = %d"), controlIndex );

    TUint properties = 0;

    mControlsPolicy->setControlProperties( controlIndex, properties, mFileDetails, mViewMode );
    QMPXVideoPlaybackFullScreenControl* control;

    switch ( controlIndex )
    {
        case EMPXBufferingAnimation:
        {
            //
            // Buffering animation icon
            //
            QGraphicsWidget *widget = mLoader->findWidget( QString( "bufferingIcon" ) );
            HbLabel *bufferingAnim = qobject_cast<HbLabel*>( widget );
            HbIconItem *iconItem = new HbIconItem( bufferingAnim->icon() );
            iconItem->animator().startAnimation();

            control = new QMPXVideoPlaybackFullScreenControl( this,
                                                              controlIndex,
                                                              bufferingAnim,
                                                              properties );
            mControls.append( control );

            break;
        }
        case EMPXStatusPane:
        {
            //
            // Status key (signal + title + back key)
            //
            control = new QMPXVideoPlaybackStatusPaneControl( this,
                                                              controlIndex, 
                                                              NULL,
                                                              properties );
            mControls.append( control );

            break;
        }
        case EMPXButtonBar:
        {
            //
            // Button bar
            //
            QGraphicsWidget *widget = mLoader->findWidget( QString( "buttonBarLayout" ) );
            QMPXVideoPlaybackButtonBar *buttonBar = 
                qobject_cast<QMPXVideoPlaybackButtonBar*>( widget );
            buttonBar->initialize();

            control = new QMPXVideoPlaybackFullScreenControl( this,
                                                              controlIndex,
                                                              buttonBar,
                                                              properties );
            mControls.append( control );

            break;
        }
        case EMPXProgressBar:
        {
            //
            // Progress bar
            //
            QGraphicsWidget *widget = mLoader->findWidget( QString( "progressBarLayout" ) );
            QMPXVideoPlaybackProgressBar *progressBar = 
                qobject_cast<QMPXVideoPlaybackProgressBar*>( widget );
            progressBar->initialize();

            control = new QMPXVideoPlaybackFullScreenControl( this,
                                                              controlIndex,
                                                              progressBar,
                                                              properties );

            mControls.append( control );

            break;
        }
        case EMPXFileDetailsWidget:
        {
            QGraphicsWidget *widget = mLoader->findWidget( QString( "fileDetailsLayout" ) );
            QMPXVideoPlaybackFileDetailsWidget *fileDetails = 
                qobject_cast<QMPXVideoPlaybackFileDetailsWidget*>( widget );
            fileDetails->initialize();

            control = new QMPXVideoPlaybackFullScreenControl( this,
                                                              controlIndex,
                                                              fileDetails,
                                                              properties );

            mControls.append( control );
            break;
        }
        case EMPXIndicatorBitmap:
        {
            QGraphicsWidget *widget = mLoader->findWidget( QString( "bitmapLayout" ) );
            HbWidget *bitmapWidget = qobject_cast<HbWidget*>( widget );

            setDefaultBitmap();
            control = new QMPXVideoPlaybackFullScreenControl( this,
                                                              controlIndex,
                                                              bitmapWidget,
                                                              properties );
            mControls.append( control );

            break;
        }
        case EMPXDetailsViewPlaybackWindow:
        {
            QGraphicsWidget *widget = mLoader->findWidget( QString( "detailsPlaybackWindow" ) );
            QMPXVideoPlaybackDetailsPlaybackWindow *detailsPlaybackWindow = 
                    qobject_cast<QMPXVideoPlaybackDetailsPlaybackWindow*>( widget );
            detailsPlaybackWindow->initialize();

            control = new QMPXVideoPlaybackFullScreenControl( this,
                                                              controlIndex,
                                                              detailsPlaybackWindow,
                                                              properties );
            mControls.append( control );

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::handleTappedOnScreen()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleTappedOnScreen()

{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::handleTappedOnScreen()"));

    //
    //  Toggle visibility only if the followings are true 
    //  - TV-Out is not connected
    //  - Video is available
    //  - We are in playing or paused state
    //
    if ( ! isTvOutConnected() &&
         mFileDetails->mVideoEnabled &&
         ( mState == EPbStatePlaying || mState == EPbStatePaused ) )
    {
        if ( mViewMode == EFullScreenView )
        {
            if ( isVisible() )
            {
                //
                // If the volume control is visible, hide it
                //
                if ( mVolumeControl->isVisible() )
                {
                    mVolumeControl->setVisible( false );
                }

                hideAllControls();
            }
            else
            {
                showControls();
            }            
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::resetDisappearingTimers()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::resetDisappearingTimers( TMPXTimerAction timerAction )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::resetDisappearingTimers()"));

    if ( mControlsTimer->isActive() )
    {
        mControlsTimer->stop();
    }

    //
    //  Do not start the time for DetailsView and AudioOnlyView
    //
    if ( timerAction == EMPXTimerReset &&
         mState == EPbStatePlaying &&
         mViewMode == EFullScreenView )
    {
        MPX_DEBUG(_L("    Starting Controls Timer"));

        mControlsTimer->start();
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::hideAllControls()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::hideAllControls()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::hideControls()"));

    resetDisappearingTimers( EMPXTimerCancel );

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        mControls[i]->setVisible( false );
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::showControls()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::showControls()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::showControls()"));

    resetDisappearingTimers( EMPXTimerReset );

    if ( ! mViewTransitionIsGoingOn )
    {
        for ( int i = 0 ; i < mControls.count() ; i++ )
        {
            mControls[i]->setVisibility( mState );
        }
    }        
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::isVisible()
{
    bool visible = EFalse;

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->controlIndex() == EMPXButtonBar )
        {
            if ( mControls[i]->isVisible() )
            {
                visible = ETrue;
            }

            break;
        }
    }

    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::isVisible() [%d]"), visible);

    return visible;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::handleCommand()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleCommand( 
        TMPXVideoPlaybackViewCommandIds command, int value )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::handleCommand(%d)"), command);

    switch( command )
    {
        case EMPXPbvCmdSetPosition:
        {
            TRAP_IGNORE( mViewWrapper->SetPropertyL( EPbPropertyPosition, 
                                                     value * KPbMilliMultiplier ) );
            break;
        }
        case EMPXPbvCmdSetVolume:
        {
            TRAP_IGNORE( mViewWrapper->SetPropertyL( EPbPropertyVolume, value ) );
            break;            
        }
        default:
        {
            TRAP_IGNORE( mViewWrapper->HandleCommandL( command ) );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::volumeChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::volumeChanged( int volume )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::volumeChanged() [%d]"), volume);

    if ( mVolumeControl )
    {
        mVolumeControl->setValue( volume );
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::showVolumeControls()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::showVolumeControls()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::showVolumeControls()"));

    if ( mVolumeControl )
    {
        mVolumeControl->setVisible( true );
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::durationChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::durationChanged( int duration )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::durationChanged() [%d]"), duration);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->durationChanged( duration ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::positionChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::positionChanged( int position )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::positionChanged() [%d]"), position);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->positionChanged( position ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::aspectRatioChanged() [%d]"), aspectRatio);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->aspectRatioChanged( aspectRatio ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::updateState()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::updateState()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::updateState() [%d]"), mState);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        mControls[i]->updateState( mState );
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::realFormat()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::realFormat( QString filename )
{
    bool realFormat = EFalse;

    if ( !filename.isNull() && !filename.isEmpty() )
    {
        if ( mFileDetails->mPlaybackMode == EMPXVideoStreaming ||
             mFileDetails->mPlaybackMode == EMPXVideoLiveStreaming )
        {
            TBufC<255> file(filename.utf16());
            realFormat = realFormatForStreaming( file );
        }
        else
        {
            realFormat = realFormatForLocal();
        }
    }

    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::realFormat() [%d]"), realFormat);

    return realFormat;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::realFormatForStreaming()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::realFormatForStreaming( const TDesC& des )
{
    bool realFormat = EFalse;
    TParse filePath;

    _LIT(KMPXRMEXT, ".R" );

    int err = filePath.Set( des, NULL, NULL );

    //
    // It is valid to have a "\" character in a url, but parsing fails,
    // switch these characters to "/" for the local string and try parsing again.
    //
    if ( err == KErrBadName )
    {
        int backsashPos = des.LocateF('\\');

        if( backsashPos != KErrNotFound )
        {
            HBufC* fileName = NULL;

            TRAP( err,  fileName = des.AllocL() );

            if ( err == KErrNone )
            {
                int count( fileName->Des().Length() );

                for ( int j = backsashPos ; j < count; ++j )
                {
                    if ( fileName->Des()[j]== '\\' )
                    {
                        fileName->Des()[j]='/';
                    }
                }
                err = filePath.Set( fileName->Des(), NULL, NULL );                
            }

            if ( fileName )
            {
                delete fileName;
                fileName = NULL;
            }
        }
    }

    if ( err == KErrNone )
    {
        //
        // The requirement to support arguments with the extension in streaming links
        // for-ex: /video.3gp?start=10&end=40 will have to supported
        // as well. So just by doing p.Ext() would retrieve all the string
        // after "." so by doing a Left(2) would retrieve only
        // .3g or .rm and so on. This would help better
        // interpret the extension and decide the branding accordingly
        //
        filePath.Set( filePath.NameAndExt(), NULL, NULL );
        TPtrC extension = filePath.Ext().Left( 2 );

        TBuf<2> buf;
        buf.Format( extension );
        buf.UpperCase();

        // RealMedia Branding
        if ( ! buf.Compare( KMPXRMEXT ) )
        {
            realFormat = ETrue;
        }
    }

    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::realFormatForStreaming()[%d]"), realFormat);

    return realFormat;
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::realFormatForLocal()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::realFormatForLocal()
{
    bool realFormat = false;
    
    QString real( "real" );
    QString rn( "rn" );
    
    if ( mFileDetails->mMimeType.contains( real, Qt::CaseInsensitive ) ||
         mFileDetails->mMimeType.contains( rn, Qt::CaseInsensitive ) )
    {
        realFormat = true;
    }
        
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::realFormatForLocal() [%d]"), realFormat);

    return realFormat;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::setDownloadSize()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::setDownloadSize( int size )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::setDownloadSize() [%d]"), size);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->setDownloadSize( size ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::updateDownloadPosition()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::updateDownloadPosition( int size )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::updateDownloadPosition() [%d]"), size);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->updateDownloadPosition( size ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::handleErrors
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleErrors()
{
    MPX_ENTER_EXIT(_L("QMPXVideoPlaybackControlsController::handleErrors()"),
                   _L("mState = %d"), mState );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::isSoftKeyVisible()
// -------------------------------------------------------------------------------------------------
//
bool QMPXVideoPlaybackControlsController::isSoftKeyVisible( int /*value*/ )
{
    bool visible = EFalse;

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->controlIndex() == EMPXStatusPane )
        {
            if ( mControls[i]->isVisible() )
            {
                visible = ETrue;
            }

            break;
        }
    }

    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::isSoftKeyVisible() [%d]"), visible);

    return visible;
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::handleTvOutEvent
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleTvOutEvent(
        bool connected, TMPXVideoPlaybackControlCommandIds event, int value )
{
    Q_UNUSED( event );

    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::handleTvOutEvent()"));

    mFileDetails->mTvOutConnected = connected;

    setDefaultBitmap();

    if ( mFileDetails->mTvOutConnected )
    {
        generateThumbNail();
        mFileDetails->mTvOutPlayAllowed = value;
    }
    else
    {
        mFileDetails->mTvOutPlayAllowed = ETrue;
    }

    //
    // Change the view. 
    // If Tv-out is connected, go to AudioOnlyView.
    // If not, go back to default view.
    //
    TPlaybackViewMode viewMode = EFullScreenView;

    if ( mFileDetails->mTvOutConnected || ! mFileDetails->mVideoEnabled )
    {
        viewMode = EAudioOnlyView;
    }

    changeViewMode( viewMode, false );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::changeViewMode
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::changeViewMode( 
        TPlaybackViewMode viewMode, bool transitionEffect )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::changeViewMode()"));

    if ( viewMode != mViewMode )
    {
        switch ( viewMode )
        {
            case EFullScreenView:
            case EDetailsView:
            {
                if ( mFileDetails->mVideoEnabled && ! mFileDetails->mTvOutConnected )
                {
                    mViewMode = viewMode;

                    //
                    // Hack to clean up the screen before transition. We may not need it in NGA env
                    //
                    hideAllControls();

                    updateVideoRect( transitionEffect );
                }

                break;
            }
            case EAudioOnlyView:
            {
                if ( ! mFileDetails->mVideoEnabled || mFileDetails->mTvOutConnected )
                {
                    mViewMode = viewMode;

                    mControlsConfig->updateControlList( EMPXControlCmdAudionOnlyViewOpened );
                }

                break;
            }
            default:
            {
                break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::updateVideoRectDone
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::updateVideoRectDone()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::updateVideoRectDone()"));

    mViewTransitionIsGoingOn = false;

    TMPXVideoPlaybackControlCommandIds event = EMPXControlCmdFullScreenViewOpened;

    if ( mViewMode == EDetailsView )
    {
        event = EMPXControlCmdDetailsViewOpened;
    }

    mControlsConfig->updateControlList( event );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::skipToPreviousVideoItem
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::skipToPreviousVideoItem()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::skipToPreviousVideoItem()"));

    handleCommand( EMPXPbvCmdPreviousListItem );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::skipToNextVideoItem
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::skipToNextVideoItem()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::skipToNextVideoItem()"));

    handleCommand( EMPXPbvCmdNextListItem );
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::updateVideoRect()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::updateVideoRect( bool transitionEffect )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::updateVideoRect()"));

    if ( mFileDetails->mVideoEnabled && ! mFileDetails->mTvOutConnected )
    {
        mViewTransitionIsGoingOn = true;

        QRectF rect;

        if ( mViewMode == EDetailsView )
        {
            QGraphicsWidget *parent = mLoader->findWidget( QString( "detailsPlaybackWindow" ) );
            QGraphicsWidget *child = mLoader->findWidget( QString( "small_transparentWindow" ) );

            rect = parent->mapRectToScene( child->geometry() );
        }
        else if ( mViewMode == EFullScreenView )
        {
            QGraphicsWidget *widget = mLoader->findWidget( QString( "content" ) );
            rect = widget->geometry();
        }

        mViewWrapper->UpdateVideoRect( 
                rect.x(), rect.y(), rect.width(), rect.height(), transitionEffect );
    }
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::setDefaultBitmap()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::setDefaultBitmap()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::setDefaultBitmap()"));

    //
    // set specific bitmaps for audio only and tv out cases
    // If we already requested a thumbnail, we will set the thumbnail after we receive the thumbnail
    //
    if ( mLoader && mThumbNailState != EThumbNailRequsted )
    {
        QGraphicsWidget *tvOutBitmap = mLoader->findWidget( "tvOutBitmap" );
        tvOutBitmap->setVisible( false );

        QGraphicsWidget *realAudioOnlyBitmap = mLoader->findWidget( "realAudioOnlyBitmap" );
        realAudioOnlyBitmap->setVisible( false );

        QGraphicsWidget *partialAudioOnlyBitmap = mLoader->findWidget( "partialAudioOnlyBitmap" );
        partialAudioOnlyBitmap->setVisible( false );

        QGraphicsWidget *audioOnlyBitmap = mLoader->findWidget( "audioOnlyBitmap" );
        audioOnlyBitmap->setVisible( false );

        if ( mFileDetails->mTvOutConnected )
        {
            tvOutBitmap->setVisible( true );
        }
        else if ( mFileDetails->mRNFormat ) 
        {
            realAudioOnlyBitmap->setVisible( true );
        }
        else if ( mFileDetails->mPartialPlayback )
        {
            partialAudioOnlyBitmap->setVisible( true );
        }
        else
        {
            audioOnlyBitmap->setVisible( true );
        }
    }    
}

// -------------------------------------------------------------------------------------------------
//   QMPXVideoPlaybackControlsController::generateThumbNail()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::generateThumbNail()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlsController::generateThumbNail()"));

    //
    // Generate thumbnail if it is local playback
    //
    if ( mFileDetails->mPlaybackMode == EMPXVideoLocal )
    {
        if ( mThumbNailState == EThumbNailEmpty )
        {
            mThumbnailManager = new ThumbnailManager(); ; 

            mThumbnailManager->setThumbnailSize( ThumbnailManager::ThumbnailLarge );
            mThumbnailManager->setQualityPreference( ThumbnailManager::OptimizeForPerformance );
         
            if ( connect( mThumbnailManager, SIGNAL( thumbnailReady( QPixmap , void * , int , int ) ), 
                          this, SLOT( handleThumbnailReady( QPixmap , void * , int , int ) ) ) )
            {
                mThumbnailManager->getThumbnail( mFileDetails->mClipName );
                mThumbNailState = EThumbNailRequsted;
            }
        }
    }
    else
    {
        mThumbNailState = EThumbNailNotAvailable;
    }
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlsController::handleThumbnailReady()
// -------------------------------------------------------------------------------------------------
//
void QMPXVideoPlaybackControlsController::handleThumbnailReady( 
        QPixmap tnData, void *internal , int id, int error )
{
    Q_UNUSED( internal );
    Q_UNUSED( id );

    if( ! error && mLoader )
    {
        QGraphicsWidget *tvOutBitmap = mLoader->findWidget( "tvOutBitmap" );
        HbLabel *tvOutLabel = qobject_cast<HbLabel*>( tvOutBitmap );

        QIcon *qicon = new QIcon( tnData );

        HbIcon *hbIcon = new HbIcon( *qicon );
        hbIcon->setSize( tvOutBitmap->size() );
        tvOutLabel->setIcon( *hbIcon );

        if ( qicon )
        {
            delete qicon;
        }

        if ( hbIcon )
        {
            delete hbIcon;
        }

        mThumbNailState = EThumbNailSet;
    }
    else
    {
        mThumbNailState = EThumbNailNotAvailable;
    }

    setDefaultBitmap();

    disconnect( mThumbnailManager, SIGNAL( thumbnailReady( QPixmap , void * , int , int ) ), 
                this, SLOT( handleThumbnailReady( QPixmap , void * , int , int ) ) );
}

// End of File
