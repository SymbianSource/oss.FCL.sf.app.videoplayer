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

// Version : %version: da1mmcf#27 %



//  Include Files

#include <w32std.h>
#include <eikenv.h>

#include <mpxplaybackcommanddefs.h>
#include <mpxvideoplaybackdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcommand.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediavideodefs.h>
#include <mpxplaybackutility.h>
#include <mmf/common/mmferrors.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionplaylist.h>
#include <mpxmediageneralextdefs.h>

#include "mpxvideo_debug.h"
#include "mpxvideoregion.h"
#include "mpxvideoviewwrapper.h"
#include "mpxvideoplaybackuids.hrh"
#include "mpxhbvideocommondefs.h"
#include "hbvideobaseplaybackview.h"
#include "mpxvideoplaybackcontrol.hrh"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackdisplayhandler.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackuserinputhandler.h"

//  Member Functions

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::CMPXVideoViewWrapper()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoViewWrapper::CMPXVideoViewWrapper( HbVideoBasePlaybackView* aView )
    : iView( aView )
    , iControlsController( NULL )
    , iMediaRequested( false )
    , iPlaylistView( false )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::NewL()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoViewWrapper* CMPXVideoViewWrapper::NewL( HbVideoBasePlaybackView* aView )
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::NewL()"));

    CMPXVideoViewWrapper* self = new (ELeave) CMPXVideoViewWrapper( aView );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::ConstructL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::ConstructL()"));

    iPlaybackUtility = 
        MMPXPlaybackUtility::UtilityL( EMPXCategoryVideo, KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );
    iPlaybackUtility->SetPrimaryClientL();

    //
    //  Create Active Object for closing player
    //
    iCloseAO = CIdle::NewL( CActive::EPriorityStandard );   
                
    //
    //  Create Video Playback Display Handler
    //
    iDisplayHandler = CMPXVideoPlaybackDisplayHandler::NewL( iPlaybackUtility, this );

    //
    // Create control's controller
    //
    CreateControlsL();

    //
    // Create user input handler
    //
    iUserInputHandler = CMPXVideoPlaybackUserInputHandler::NewL( this, iFileDetails->mTvOutConnected );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::~CMPXVideoViewWrapper()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoViewWrapper::~CMPXVideoViewWrapper()
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::~CMPXVideoViewWrapper()"));

    //
    //  Delete the display handler when the view is deactivated
    //
    if ( iDisplayHandler )
    {
        delete iDisplayHandler;
        iDisplayHandler = NULL;
    }

    if ( iUserInputHandler )
    {
        delete iUserInputHandler;
        iUserInputHandler = NULL;
    }

    if ( iControlsController )
    {
        delete iControlsController;
        iControlsController = NULL;
    }

    if ( iCloseAO )
    {
        delete iCloseAO;
        iCloseAO = NULL;
    }

    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }

    if ( iPlaybackUtility )
    {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        iPlaybackUtility = NULL;
    }

    if ( iCollectionUtility )
    {
        iCollectionUtility->Close();
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::IsLive()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoViewWrapper::IsLive()
{
    return (iFileDetails->mPlaybackMode == EMPXVideoLiveStreaming);
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::IsPlaylist()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoViewWrapper::IsPlaylist()
{
    return iPlaylistView;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::CreateGeneralPlaybackCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::CreateGeneralPlaybackCommandL( TMPXPlaybackCommand aCmd )
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::CreateGeneralPlaybackCommandL(%d)"), aCmd );

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, aCmd );

    iPlaybackUtility->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::HandleCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandleCommandL( TInt aCommand )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoViewWrapper::HandleCommandL()"),
        _L("aCommand = %d"), aCommand );

    switch ( aCommand )
    {
        case EMPXPbvCmdPlay:
        {
            IssuePlayCommandL();
            break;
        }
        case EMPXPbvCmdPause:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdPause"));
            CreateGeneralPlaybackCommandL( EPbCmdPause );
            break;
        }
        case EMPXPbvCmdClose:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdClose"));
            
            CreateGeneralPlaybackCommandL( EPbCmdClose );
            break;
        }
        case EMPXPbvCmdSeekForward:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdSeekForward"));
            CreateVideoSpecificCmdL( EPbCmdStartVideoSeekingForward );
            break;
        }
        case EMPXPbvCmdSeekBackward:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdSeekBackward"));
            CreateVideoSpecificCmdL( EPbCmdStartVideoSeekingBackward );
            break;
        }
        case EMPXPbvCmdEndSeek:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdEndSeek"));
            CreateVideoSpecificCmdL( EPbCmdStopVideoSeeking );
            break;
        }
        case EMPXPbvCmdPlayPause:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdPause"));
            iPlaybackUtility->CommandL( EPbCmdPlayPause );
            break;
        }
        case EMPXPbvCmdStop:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdStop"));
            CreateGeneralPlaybackCommandL( EPbCmdStop );
            break;
        }
        case EMPXPbvCmdDecreaseVolume:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdDecreaseVolume"));
            HandleVolumeCmdL( EPbCmdDecreaseVolume );
            break;
        }
        case EMPXPbvCmdIncreaseVolume:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdIncreaseVolume"));
            HandleVolumeCmdL( EPbCmdIncreaseVolume );
            break;
        }
        case EMPXPbvCmdNaturalAspectRatio:
        {
            MPX_DEBUG(
                _L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdNaturalAspectRatio"));
            SetAspectRatioL( EPbCmdNaturalAspectRatio );
            break;
        }
        case EMPXPbvCmdZoomAspectRatio:
        {
            MPX_DEBUG(
                _L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdZoomAspectRatio"));
            SetAspectRatioL( EPbCmdZoomAspectRatio );
            break;
        }
        case EMPXPbvCmdStretchAspectRatio:
        {
            MPX_DEBUG(
                _L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdStretchAspectRatio"));
            SetAspectRatioL( EPbCmdStretchAspectRatio );
            break;
        }
        case EMPXPbvCmdMute:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdMute"));
            HandleVolumeCmdL( EPbCmdMuteVolume );
            break;
        }
        case EMPXPbvCmdUnMute:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdUnMute"));
            HandleVolumeCmdL( EPbCmdUnMuteVolume );
            break;
        }
        case EMPXPbvCmdShortPressBackward:
        {
            MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL() EMPXPbvCmdShortPressBackward"));

            HandleShortPressBackwardL();
            break;
        }
        case EMPXPbvCmdResetControls:
        {
            CreateControlsL();
            iView->retrievePdlInformation();
            break;
        }
        case EMPXPbvCmdNextListItem:
        {
            if ( iPlaylistView && iFileDetails->mMultiItemPlaylist )
            {
                iPlaybackUtility->CommandL( EPbCmdNext );
            }
            break;
        }
        case EMPXPbvCmdPreviousListItem:  
        {
            if ( iPlaylistView && iFileDetails->mMultiItemPlaylist )
            {
			    //
			    // the command is being sent twice on purpose
                // one EMPXPbvCmdPreviousListItem command only sets the position to 0  
                // the second cmd actually goes to the previous item in the list
				//
                iPlaybackUtility->CommandL( EPbCmdPrevious );
                iPlaybackUtility->CommandL( EPbCmdPrevious );  
            }
            break;
        }
        case EMPXPbvCmdEndOfClip:
        {
            CreateVideoSpecificCmdL( EPbCmdEndofClipReached );
            break;
        }
        case EMPXPbvCmdCustomPause:
        {
            CreateVideoSpecificCmdL( EPbCmdCustomPause );
            break;
        }
        case EMPXPbvCmdCustomPlay:
        {
            CreateVideoSpecificCmdL( EPbCmdCustomPlay );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::HandlePlaybackMessage()"),
                   _L("aError = %d"), aError );

    if ( aError )
    {
        MPX_TRAPD( err, DoHandleErrorPlaybackMessageL( aError ) );
    }
    else if ( aMessage )
    {
        MPX_TRAPD( err, DoHandlePlaybackMessageL( aMessage ) );
    }
}

// -------------------------------------------------------------------------------------------------
// Request for the media object
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::RequestMediaL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::RequestMediaL()"));

    if ( ! iMediaRequested && iPlaybackUtility->StateL() == EPbStateInitialised )
    {
        iMediaRequested = ETrue;

        //
        //  Request the volume for the controls
        //
        iPlaybackUtility->PropertyL( *this, EPbPropertyVolume );

        //
        //  Request the clip's meta data
        //
        MMPXSource* s = iPlaybackUtility->Source();

        if ( s )
        {
            RArray<TMPXAttribute> attrs;
            CleanupClosePushL(attrs);

            //
            //  General Media Attributes
            //
            attrs.Append( KMPXMediaGeneralUri |
                          KMPXMediaGeneralDuration |
                          KMPXMediaGeneralTitle |
                          KMPXMediaGeneralMimeType );

            //
            //  Video specific Attributes
            //
            attrs.Append( KMPXMediaVideoAll );

            // Set the attribute to always route the media call to playback plugin
            CMPXAttributeSpecs* specs = CMPXAttributeSpecs::NewL();
            CleanupStack::PushL( specs );

            specs->SetTObjectValueL<TBool>(KMPXMediaGeneralExtMediaRedirect, ETrue);

            s->MediaL( attrs.Array(), *this, specs);

            CleanupStack::PopAndDestroy( specs );
            CleanupStack::PopAndDestroy( &attrs );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::DoHandlePlaybackMessageL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::DoHandlePlaybackMessageL( CMPXMessage* aMessage )
{
    TMPXMessageId id( *(aMessage->Value<TMPXMessageId>(KMPXMessageGeneralId)) );

    MPX_ENTER_EXIT(
        _L("CMPXVideoViewWrapper::DoHandlePlaybackMessageL()"),
        _L("id = 0x%08x"), id );

    if ( KMPXMessageGeneral == id )
    {
        HandleGeneralPlaybackMessageL( aMessage );
    }
    else if ( KMPXMediaIdVideoPlayback == id )
    {
        HandleVideoPlaybackMessage( aMessage );
    }
    else if ( KMPXMediaIdVideoDisplayMessage == id )
    {
        if ( iDisplayHandler )
        {
            iDisplayHandler->HandleVideoDisplayMessageL( aMessage );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::HandleGeneralPlaybackMessageL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandleGeneralPlaybackMessageL( CMPXMessage* aMessage )
{
    TInt event( *aMessage->Value<TInt>( KMPXMessageGeneralEvent ) );
    TInt type( *aMessage->Value<TInt>( KMPXMessageGeneralType ) );
    TInt data( *aMessage->Value<TInt>( KMPXMessageGeneralData ) );

    MPX_DEBUG(
      _L("CMPXVideoViewWrapper::HandleGeneralPlaybackMessageL() event = %d type = %d  value = %d"),
      event, type, data );

    switch ( event )
    {
        case TMPXPlaybackMessage::EStateChanged:
        {
            DoHandleStateChangeL( type );

            break;
        }
        case TMPXPlaybackMessage::EPropertyChanged:
        {
            TMPXPlaybackProperty property(
                 static_cast<TMPXPlaybackProperty>( type ) );

            HandlePropertyL( property, data, KErrNone );
            break;
        }
        case TMPXPlaybackMessage::EDownloadPositionChanged:
        {
            if ( iControlsController )
            {
                iControlsController->handleEvent( EMPXControlCmdDownloadUpdated, data );
            }

            break;
        }
        case TMPXPlaybackMessage::EDownloadStateChanged:
        {
            break;
        }
        case TMPXPlaybackMessage::ECommandReceived:
        {
            break;
        }
        case TMPXPlaybackMessage::EReachedEndOfPlaylist:
        {
            iView->closePlaybackView();
            break;
        }
        case TMPXPlaybackMessage::ESongChanged:
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::HandleVideoPlaybackMessage()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandleVideoPlaybackMessage( CMPXMessage* aMessage )
{
    TMPXVideoPlaybackCommand message =
        ( *(aMessage->Value<TMPXVideoPlaybackCommand>(KMPXMediaVideoPlaybackCommand)) );

    MPX_DEBUG(
      _L("CMPXVideoViewWrapper::HandleVideoPlaybackMessage() message = %d"), message );

    switch ( message )
    {
        case EPbCmdPluginError:
        {
            TInt error( *aMessage->Value<TInt>( KMPXMediaVideoError ) );

            iView->handlePluginError( error );
            break;
        }
        case EPbCmdTvOutEvent:
        {
            TMPXVideoPlaybackControlCommandIds cmdId = EMPXControlCmdTvOutDisconnected;

            TBool tvOutConnected( *aMessage->Value<TInt>( KMPXMediaVideoTvOutConnected ) );
            TBool playbackAllowed = ETrue;

            if ( tvOutConnected )
            {
                cmdId = EMPXControlCmdTvOutConnected;

                playbackAllowed = *aMessage->Value<TInt>( KMPXMediaVideoTvOutPlayAllowed );
            }

            if ( iUserInputHandler )
            {
                TRAP_IGNORE(iUserInputHandler->HandleTVOutEventL( tvOutConnected ));     
            }

            if ( iControlsController )
            {
                iControlsController->handleEvent( cmdId, playbackAllowed );
            }
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::HandlePluginError()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandlePluginError( int aError )
{
    MPX_DEBUG(
      _L("CMPXVideoViewWrapper::HandlePluginError() aError = %d"), aError );

    iView->handlePluginError( aError );
}

// -------------------------------------------------------------------------------------------------
//   Handle playback state changes
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::DoHandleStateChangeL( TInt aNewState )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoViewWrapper::DoHandleStateChangeL()"),
        _L("aNewState = %d"), aNewState );

    if ( aNewState != iPlaybackState )
    {
        switch ( aNewState )
        {
            case EPbStateInitialising:
            {
                if ( iControlsController )
                {
                    //
                    //  If transitioning from Not Initialized to Initialising there is
                    //  no need to update the playback information that was gathered
                    //  when the container was created
                    //
                    if ( iPlaybackState != EPbStateNotInitialised )
                    {
                        iMediaRequested = EFalse;
                        HandleCommandL( EMPXPbvCmdResetControls );

                        if ( iFileDetails )
                        {    
                            iFileDetails->clearFileDetails();
                        }
                    }
                }
                break;
            }
            case EPbStateBuffering:
            {
                HandleBufferingStateL();

                break;
            }
            case EPbStatePluginSeeking:
            {
                // no-op
                break;
            }
            case EPbStateStopped:
            {
                if ( iFileDetails->mMultiItemPlaylist )
                {
                    iView->handleStoppedState();
                }
                else
                {
                    iView->closePlaybackView();  
                }

                break;
            }
            case EPbStateInitialised:
            {
                RequestMediaL();
            	break;
            }
            default:
            {
                break;
            }
        }

        iPlaybackState = (TMPXPlaybackState)aNewState;

        if ( iControlsController )
        {
            iControlsController->handleEvent( EMPXControlCmdStateChanged, aNewState );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::HandleBufferingStateL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandleBufferingStateL()
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleBufferingStateL()"));

}

// -------------------------------------------------------------------------------------------------
// Handle playback error message.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::DoHandleErrorPlaybackMessageL( TInt aError )
{
    MPX_ENTER_EXIT( _L("CMPXVideoViewWrapper::DoHandleErrorPlaybackMessageL()"),
                    _L("aError = %d"), aError );

    HandleCommandL( EMPXPbvCmdResetControls );
    iView->handlePluginError(aError);
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::HandlePlaybackCommandComplete()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandlePlaybackCommandComplete( CMPXCommand* /*aCommandResult*/,
                                                         TInt /*aError*/ )
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::HandlePlaybackCommandComplete()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::ParseMetaDataL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::ParseMetaDataL( const CMPXMessage& aMedia )
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::ParseMetaDataL()"));

    //
    //  Clip Name
    //
    if ( aMedia.IsSupported( KMPXMediaGeneralUri ) )
    {
        TPtrC uri( aMedia.ValueText( KMPXMediaGeneralUri ) );
        const QString qClipname( (QChar*)uri.Ptr(), uri.Length() );
        iFileDetails->mClipName = qClipname;
    }

    //
    //  Title
    //
    if ( aMedia.IsSupported( KMPXMediaGeneralTitle ) )
    {
        TPtrC title( aMedia.ValueText( KMPXMediaGeneralTitle ) );
        const QString qTitle( (QChar*)title.Ptr(), title.Length() );
        iFileDetails->mTitle = qTitle;
    }

    //
    //  Artist
    //
    if ( aMedia.IsSupported( KMPXMediaVideoArtist ) )
    {
        TPtrC artist( aMedia.ValueText( KMPXMediaVideoArtist ) );
        const QString qArtist( (QChar*)artist.Ptr(), artist.Length() );
        iFileDetails->mArtist = qArtist;
    }

    //
    //  Mime Type
    //
    if ( aMedia.IsSupported( KMPXMediaGeneralMimeType ) )
    {
        TPtrC mimeType( aMedia.ValueText( KMPXMediaGeneralMimeType ) );
        const QString qMimeType( (QChar*)mimeType.Ptr(), mimeType.Length() );
        iFileDetails->mMimeType = qMimeType;
    }

    //
    //  Duration
    //
    if ( aMedia.IsSupported( KMPXMediaGeneralDuration ) )
    {
        iFileDetails->mDuration = aMedia.ValueTObjectL<TInt>( KMPXMediaGeneralDuration );
    }

    //
    //  Audio Enabled
    //
    if ( aMedia.IsSupported( KMPXMediaVideoAudioEnabled ) )
    {
        iFileDetails->mAudioEnabled = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoAudioEnabled );
    }

    //
    //  Video Enabled
    //
    if ( aMedia.IsSupported( KMPXMediaVideoVideoEnabled ) )
    {
        iFileDetails->mVideoEnabled = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoVideoEnabled );
    }

    //
    //  Partial Playback
    //
    if ( aMedia.IsSupported( KMPXMediaVideoPartialPlayback ) )
    {
        iFileDetails->mPartialPlayback =
            aMedia.ValueTObjectL<TInt>( KMPXMediaVideoPartialPlayback );
    }

    //
    //  Playback Mode
    //
    if ( aMedia.IsSupported( KMPXMediaVideoPlaybackMode ) )
    {
        iFileDetails->mPlaybackMode =
            (TMPXVideoMode)aMedia.ValueTObjectL<TInt>( KMPXMediaVideoPlaybackMode );
    }

    //
    //  Seekable
    //
    if ( aMedia.IsSupported( KMPXMediaVideoSeekable ) )
    {
        iFileDetails->mSeekable = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoSeekable );
    }

    //
    //  Pausable
    //
    if ( aMedia.IsSupported( KMPXMediaVideoPausableStream ) )
    {
        iFileDetails->mPausableStream = aMedia.ValueTObjectL<TBool>( KMPXMediaVideoPausableStream );
    }

    //
    //  Video Height
    //
    if ( aMedia.IsSupported( KMPXMediaVideoHeight ) )
    {
        iFileDetails->mVideoHeight = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoHeight );
    }

    //
    //  Video Width
    //
    if ( aMedia.IsSupported( KMPXMediaVideoWidth ) )
    {
        iFileDetails->mVideoWidth = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoWidth );
    }

    //
    //  TV-Out Connected
    //
    if ( aMedia.IsSupported( KMPXMediaVideoTvOutConnected ) )
    {
        iFileDetails->mTvOutConnected = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoTvOutConnected );
    }

    //
    //  TV-Out Playback Allowed
    //
    if ( aMedia.IsSupported( KMPXMediaVideoTvOutPlayAllowed ) )
    {
        iFileDetails->mTvOutPlayAllowed =
            aMedia.ValueTObjectL<TInt>( KMPXMediaVideoTvOutPlayAllowed );
    }

    //
    //  BitRate
    //
    if ( aMedia.IsSupported( KMPXMediaVideoBitRate ) )
    {
        iFileDetails->mBitRate = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoBitRate );
    }

    //
    //  Drm Protected
    //
    if ( aMedia.IsSupported( KMPXMediaVideoDrmProtected ) )
    {
        iFileDetails->mDrmProtected = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoDrmProtected );
    }
    
    //
    //  Description
    //
    if ( aMedia.IsSupported( KMPXMediaVideoDescription ) )
    {
        TPtrC description( aMedia.ValueText( KMPXMediaVideoDescription ) );
        const QString qDescription( (QChar*)description.Ptr(), description.Length() );
        iFileDetails->mDescription = qDescription;
    }
    
    //
    //  Location
    //
    if ( aMedia.IsSupported( KMPXMediaVideoLocation ) )
    {
        TPtrC location( aMedia.ValueText( KMPXMediaVideoLocation ) );
        const QString qLocation( (QChar*)location.Ptr(), location.Length() );
        iFileDetails->mLocation = qLocation;
    }
    
    //
    //  Copyright
    //
    if ( aMedia.IsSupported( KMPXMediaVideoCopyright ) )
    {
        TPtrC copyright( aMedia.ValueText( KMPXMediaVideoCopyright ) );
        const QString qCopyright( (QChar*)copyright.Ptr(), copyright.Length() );
        iFileDetails->mCopyright = qCopyright;
    }
    
    //
    //  Language
    //
    if ( aMedia.IsSupported( KMPXMediaVideoLanguage ) )
    {
        TPtrC language( aMedia.ValueText( KMPXMediaVideoLanguage ) );
        const QString qLanguage( (QChar*)language.Ptr(), language.Length() );
        iFileDetails->mLanguage = qLanguage;
    }
    
    //
    //  Keywords
    //
    if ( aMedia.IsSupported( KMPXMediaVideoKeywords ) )
    {
        TPtrC keywords( aMedia.ValueText( KMPXMediaVideoKeywords ) );
        const QString qKeywords( (QChar*)keywords.Ptr(), keywords.Length() );
        iFileDetails->mKeywords = qKeywords;
    }
    
    //
    //  Creation date/time
    //
    if ( aMedia.IsSupported( KMPXMediaVideoCreated ) )
    {
        iFileDetails->mCreationTime = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoCreated );
    }
 
    //
    //  Last Modified date/time
    //
    if ( aMedia.IsSupported( KMPXMediaVideoLastModified ) )
    {
        iFileDetails->mModificationTime = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoLastModified );
    }
    
}

// -------------------------------------------------------------------------------------------------
// Handle media properties.
// Notes: The client is responsible for delete the object of aProperties.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::DoHandleMediaL( const CMPXMessage& aMedia, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::DoHandleMediaL()"),
                   _L("aError = %d"), aError );

    if ( aError == KErrNone )
    {
        if ( iFileDetails )
        {
            iFileDetails->clearFileDetails();
        }
        else
        {
            iFileDetails = new QMPXVideoPlaybackViewFileDetails();
        }       
        
        //
        //  Read in the media data
        //
        ParseMetaDataL( aMedia );

        //
        //  Create controls since file details are available
        //
        if ( iControlsController )
        {
            iControlsController->addFileDetails( iFileDetails );            
        }

        if ( iFileDetails->mVideoEnabled )
        {
            RWindow *window = iView->getWindow();

            TRect displayRect = TRect( window->Position().iX,
                                       window->Position().iY,
                                       window->Position().iX + window->Size().iWidth,                  
                                       window->Position().iY + window->Size().iHeight );

            TReal displayAspectRatio = (TReal32)displayRect.Width() / (TReal32)displayRect.Height();

            TInt newAspectRatio = 
                iDisplayHandler->SetDefaultAspectRatioL( iFileDetails, displayAspectRatio );

            //
            //  Setup the display window and issue play command
            //
            iDisplayHandler->CreateDisplayWindowL( CCoeEnv::Static()->WsSession(),
                                                   *(CCoeEnv::Static()->ScreenDevice()),
                                                   *window,
                                                   displayRect );

            if ( iControlsController )
            {
                iControlsController->handleEvent( EMPXControlCmdSetAspectRatio, newAspectRatio );
            }
        }

        CreateGeneralPlaybackCommandL( EPbCmdPlay );
    }
}

// -------------------------------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media event.
// Notes: The client is responsible for delete the object of aProperties.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandleMediaL( const CMPXMedia& aMedia, TInt aError)
{
    MPX_ENTER_EXIT(_L( "CMPXVideoViewWrapper::HandleMediaL()" ));
    if ( aMedia.IsSupported( KMPXMediaVideoError ) )
    {
        TInt error = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoError );
        // Reset the controls
        HandleCommandL( EMPXPbvCmdResetControls );
        // Set the iMediaRequested flag to false
        iMediaRequested = EFalse;
        // Reset the playback state to stopped
        iPlaybackState = EPbStateStopped;
        // Handle the plugin error
        iView->handlePluginError( error );
    }
    else
    {
        DoHandleMediaL( aMedia, aError );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::SetPropertyL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::SetPropertyL( TMPXPlaybackProperty aProperty, TInt aValue )
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::SetPropertyL"),
                   _L("aProperty = %d, aValue = %d"), aProperty, aValue );

    iPlaybackUtility->SetL( aProperty, aValue );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::HandlePropertyL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandlePropertyL( TMPXPlaybackProperty aProperty,
                                                 TInt aValue,
                                                 TInt aError )
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::DoHandlePropertyL - Error(%d)"), aError );

    if ( aError == KErrNone )
    {
        switch ( aProperty  )
        {
            case EPbPropertyPosition:
            {
                MPX_DEBUG(_L("CMPXVideoViewWrapper::DoHandlePropertyL position(%d)"), aValue );

                if ( iControlsController )
                {
                    iControlsController->handleEvent( EMPXControlCmdSetPosition, aValue );
                }

                break;
            }
            case EPbPropertyDuration:
            {
                MPX_DEBUG(_L("CMPXVideoViewWrapper::DoHandlePropertyL duration(%d)"), aValue );

                if ( iControlsController )
                {
                    iControlsController->handleEvent( EMPXControlCmdSetDuration, aValue );
                }

                break;
            }
            case EPbPropertyMaxVolume:
            {
                MPX_DEBUG(_L("CMPXVideoViewWrapper::DoHandlePropertyL max volume(%d)"), aValue );

                break;
            }
            case EPbPropertyVolume:
            {
                MPX_DEBUG(_L("CMPXVideoViewWrapper::DoHandlePropertyL volume(%d)"), aValue );

                if ( iControlsController )
                {
                    iControlsController->handleEvent( EMPXControlCmdSetVolume, aValue );
                }

                break;
            }
            case EPbPropertyMute:
            {
                MPX_DEBUG(_L("CMPXVideoViewWrapper::DoHandlePropertyL mute(%d)"), aValue );

                if ( iControlsController && aValue )
                {
                    iControlsController->handleEvent( EMPXControlCmdSetVolume, 0 );
                }

                break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::RetrieveFileNameAndModeL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::RetrieveFileNameAndModeL( CMPXCommand* aCmd )
{
    //
    //  set attributes on the command
    //
    aCmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );

    aCmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );

    aCmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand,
                                                      EPbCmdInitView );

    iPlaybackUtility->CommandL( *aCmd );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::ActivateClosePlayerActiveObject
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::ActivateClosePlayerActiveObject()
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::ActivateClosePlayerActiveObject()"));

    if ( ! iCloseAO->IsActive() )
    {
        iCloseAO->Start( TCallBack( CMPXVideoViewWrapper::ClosePlayerL, this ) );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::ClosePlayerL
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoViewWrapper::ClosePlayerL( TAny* aPtr )
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::ClosePlayerL()"));

    static_cast<CMPXVideoViewWrapper*>(aPtr)->DoClosePlayerL();
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::DoClosePlayerL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::DoClosePlayerL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::DoClosePlayerL()"));

    iView->doClosePlayer();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::IssuePlayCommandL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::IssuePlayCommandL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::IssuePlayCommandL()"));
    CreateGeneralPlaybackCommandL( EPbCmdPlay );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::RetrievePdlInformationL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::RetrievePdlInformationL()
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::RetrievePdlInformationL()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::CreateVideoSpecificCmdL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::CreateVideoSpecificCmdL( TMPXVideoPlaybackCommand aCmd )
{
    //
    //  create command to pass to playback plugin
    //
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );

    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );

    cmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand, aCmd );

    iPlaybackUtility->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::SetAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::SetAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::SetAspectRatioL()"));

    TInt newAspectRatio = iDisplayHandler->SetAspectRatioL( aCmd );

    if ( iControlsController )
    {
        iControlsController->handleEvent( EMPXControlCmdSetAspectRatio, newAspectRatio );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoViewWrapper::IsAppInFrontL()
// Returns true if app is foreground. Uses windowgroup id
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoViewWrapper::IsAppInFrontL()
{
    TBool ret = EFalse;
    RWsSession wsSession;

    User::LeaveIfError( wsSession.Connect() );

    if( wsSession.Handle() )
    {
        CArrayFixFlat<TInt>* wgList =
            new (ELeave) CArrayFixFlat<TInt>( wsSession.NumWindowGroups() );

        // check if our window is front or not
        if ( wsSession.WindowGroupList( 0, wgList ) == KErrNone )
        {
            ret = ( CEikonEnv::Static()->RootWin().Identifier() == wgList->At(0) );			        
        }
        else
        {
            ret = EFalse;
        }

        delete wgList;
    }

    wsSession.Close();

    MPX_DEBUG(_L("CMPXVideoViewWrapper::IsAppInFrontL (%d)" ), ret);

    return ret;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::ClosePlaybackViewL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::ClosePlaybackViewL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::closePlaybackView()"));
        
    iView->closePlaybackView();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::HandleVolumeCmdL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandleVolumeCmdL( TMPXPlaybackCommand aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::HandleVolumeCmdL()"));

    //
    // In case user try to change the volume via media key, rocker key or etc
    // We need to show the controls though the volume level doesn't get changed
    // For examples : - try to change the volume with a clip without audio track
    //                - try to reduce the volume with volume level 0
    //                - try to increase the volume with max volume level
    //
    iControlsController->handleEvent( EMPXControlCmdShowVolumeControls );

    switch( aCmd )
    {
        case EPbCmdDecreaseVolume:
        {
            CreateVideoSpecificCmdL( EPbCmdHandleDecreaseVolume );
            break;
        }
        case EPbCmdIncreaseVolume:
        {
            CreateVideoSpecificCmdL( EPbCmdHandleIncreaseVolume );
            break;
        }
        default:
        {
            iPlaybackUtility->CommandL( aCmd );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::HandleShortPressBackwardL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::HandleShortPressBackwardL()
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::HandleCommandL()"));

    if( !iPlaylistView )
    {
        SetPropertyL( EPbPropertyPosition, 0 );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::IssueVideoAppForegroundCmd()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::IssueVideoAppForegroundCmdL( TBool aForeground )
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::HandleForegroundEventL()"),
                   _L("aForeground = %d"), aForeground );

    TMPXVideoPlaybackCommand videoCmd = EPbCmdHandleBackground;

    if ( aForeground )
    {
        videoCmd = EPbCmdHandleForeground;
    }

    //
    //  create command to pass to playback plugin
    //
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );
    cmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand, videoCmd );
    cmd->SetTObjectValueL<TBool>( KMPXMediaVideoAppForeground, IsAppInFrontL() );

    iPlaybackUtility->CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::CreateControlsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::CreateControlsL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoViewWrapper::CreateControlsL()"));

    //
    //  Query playback plugin for filename and mode
    //
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    RetrieveFileNameAndModeL( cmd );

    //
    //  Create a temporary file details that is populated with the
    //  file name and playback mode.  This will be delete when
    //  plugin initialization is complete
    //
    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }

    iFileDetails = new QMPXVideoPlaybackViewFileDetails();

    TPtrC fileName( cmd->ValueText( KMPXMediaVideoPlaybackFileName ) );    
    const QString qFilename( (QChar*)fileName.Ptr(), fileName.Length() );
    iFileDetails->mClipName = qFilename;

    iFileDetails->mPlaybackMode = (TMPXVideoMode) cmd->ValueTObjectL<TInt>( KMPXMediaVideoMode );

    iFileDetails->mTvOutConnected   = cmd->ValueTObjectL<TInt>( KMPXMediaVideoTvOutConnected );
    iFileDetails->mTvOutPlayAllowed = cmd->ValueTObjectL<TInt>( KMPXMediaVideoTvOutPlayAllowed );

    TPtrC mimeType( cmd->ValueText( KMPXMediaVideoRecognizedMimeType ) );    
    const QString qMimeType( (QChar*)mimeType.Ptr(), mimeType.Length() );
    iFileDetails->mMimeType = qMimeType;

    //
    // get playlist information and set mMultiItemPlaylist flag
    //
    TInt numItems = 1;    
    MMPXSource* s = iPlaybackUtility->Source();

    if ( s )
    {
        CMPXCollectionPlaylist* playlist = NULL;

        MPX_TRAPD( err, playlist = s->PlaylistL() );
        
        if ( err == KErrNone && playlist )
        {
            iPlaylistView = ETrue;
            numItems = playlist->Count();
            delete playlist;
        }
    }

    iFileDetails->mMultiItemPlaylist = ( numItems > 1 );
    
    CleanupStack::PopAndDestroy( cmd );

    if ( iControlsController )
    {
        delete iControlsController;
        iControlsController = NULL;
    }

    iControlsController = new QMPXVideoPlaybackControlsController( iView, this, iFileDetails );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::IsMultiItemPlaylist()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoViewWrapper::IsMultiItemPlaylist()
{
    
    bool multiLinks( false );
    
    if ( iFileDetails )
    {
        multiLinks = iFileDetails->mMultiItemPlaylist;
    }
    
    MPX_DEBUG(_L("CMPXVideoViewWrapper::IsMultiItemPlaylist() ret %d"), multiLinks );
	
    return multiLinks;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::UpdateVideoRect()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::UpdateVideoRect( 
        TInt aX, TInt aY, TInt aWidth, TInt aHeight, TBool transitionEffect )
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::UpdateVideoRect()"));

    TRect rect( TPoint( aX, aY ), TSize( aWidth, aHeight ) );
    TRAP_IGNORE( iDisplayHandler->UpdateVideoRectL( rect, transitionEffect ) );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoViewWrapper::UpdateVideoRectDone()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoViewWrapper::UpdateVideoRectDone()
{
    MPX_DEBUG(_L("CMPXVideoViewWrapper::UpdateVideoRectDone()"));

    iControlsController->updateVideoRectDone();
}

// EOF
