/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class plays local video file
 *
*/

// Version : %version: ou1cpsw#41 %


//
//  INCLUDE FILES
//
#include <eikenv.h>
#include <mpxcenrepobserver.h>
#include <mpxcenrepwatcher.h>
#include <mpxmediageneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxplaybackcommanddefs.h>
#include <mpxplaybackpluginobserver.h>
#include <mpxplaybackplugin.h>
#include <AudioPreference.h>
#include <mpxplaybackplugin.h>

#include <coreapplicationuisdomainpskeys.h>
#include <ctsydomainpskeys.h>
#include <mmf/common/mmferrors.h>

#include <hal.h>
#include <hal_data.h>
#include <e32std.h>
#include <devsoundif.h>

#include "mpxvideoregion.h"
#include "mpxvideoplaybackcontroller.h"
#include "mpxvideoplaybackstate.h"
#include "mpxvideoprivatecrkeys.h"
#include "mpxvideohelixplayback.h"
#include "mpxvideoseeker.h"
#include "mpxvideoplayerutility.h"
#include "mpxvideodrmhelper.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxmediavideodefs.h"

//
//  CONSTANTS
//
#define KMAXMIMETYPELENGTH 256

//
//  Backlight Timeout in Micro Seconds
//
#define KMPXBackLightTimeOut 3500000

#define KOneKilobyte 1024

_LIT( KTitle, "Title" );
_LIT( KLiveStream, "LiveStream" );
_LIT( KSeekable, "Seekable" );
_LIT( KArtist, "Artist" );
_LIT( KAuthor, "Author" );
_LIT( KFormat, "MimeType" );
_LIT( KTrue, "1" );
_LIT( KFalse, "0" );
_LIT (KMPStreamingPauseSupported, "StreamingPauseSupported");


// ============================ MEMBER FUNCTIONS ===================================================

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::NewL()
//  ------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackController* CMPXVideoPlaybackController::NewL( MMPXPlaybackPluginObserver& aObs )
{
   CMPXVideoPlaybackController* self = new( ELeave ) CMPXVideoPlaybackController();
   CleanupStack::PushL( self );
   self->ConstructL( aObs );
   CleanupStack::Pop();
   return self;
}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::CreatePreInitStatesL()
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::CreatePreInitStatesL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CreatePreInitStatesL()"));

    iNotIntialisedState = CMPXNotInitialisedState::NewL(this);
    iInitialisingState  = CMPXInitialisingState::NewL(this);

    // Set the default state to non initialised
    iState              = iNotIntialisedState;
}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::CreatePostInitStatesL()
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::CreatePostInitStatesL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CreatePostInitStatesL()"));

    //
    //  Check if the post init states have been created
    //
    if ( ! iInitialisedState )
    {
        iInitialisedState   = CMPXInitialisedState::NewL(this);
        iPlayingState       = CMPXPlayingState::NewL(this);
        iBufferingState     = CMPXBufferingState::NewL(this);
        iPausedState        = CMPXPausedState::NewL(this);
        iSeekingState       = CMPXSeekingState::NewL(this);
        iStoppedState       = CMPXStoppedState::NewL(this);
    }
}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::ConstructL
//  Symbian 2nd phase constructor can leave.
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::ConstructL( MMPXPlaybackPluginObserver& aObs )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::ConstructL()"));

    iMPXPluginObs = &aObs;
    iAccessPointId = -1;
    iVideoSeeker = CMPXVideoSeeker::NewL( this );

    // Initiliaze to True
    iSeekable = ETrue;

    InitVolumeWatchers();

    CreatePreInitStatesL();

    iPlayer = CMpxVideoPlayerUtility::NewL( this );

    iBackLightTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    iDrmHelper = CMpxVideoDrmHelper::NewL();
}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::CloseController()
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::CloseController()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CloseController()"));

    ChangeState( EMPXVideoNotInitialized );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::OpenFileL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::OpenFileL( const TDesC& aMediaFile,
                                             RFile& aFile,
                                             TInt aAccessPointId )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::OpenFileL()"),
                   _L("file = %S"), &aMediaFile );

    TBool fileExists = EFalse;

    ChangeState( EMPXVideoInitializing );

    iClipName = aMediaFile.AllocL();
    iAccessPointId = aAccessPointId;

    //
    //  If file handle exists, duplicate it to the controller
    //
    if ( aFile.SubSessionHandle() )
    {
        if ( iFileHandle.SubSessionHandle() )
        {
            iFileHandle.Close();
        }

        User::LeaveIfError( iFileHandle.Duplicate( aFile ) );

        fileExists = ETrue;
    }

    DetermineMediaTypeL();
    SetPlaybackModeL();

    //
    //  Create accessory monitor to search for TV-Out events
    //
    if ( ! iAccessoryMonitor )
    {
        iAccessoryMonitor = CMPXVideoAccessoryObserver::NewL( this );
    }

    if ( iAccessoryMonitor->IsTvOutPlaybackAllowed() )
    {
        if ( fileExists )
        {
            //
            //  Ensure there are rights for protected clips
            //
            TInt drmError = iDrmHelper->GetDrmRightsStatus( iFileHandle );

            if ( drmError )
            {
                //
                //  Send error to observer for handling
                //
                HandleError( drmError );
            }
            else
            {
                iState->OpenFileL( iFileHandle );
            }
        }
        else
        {
            iState->OpenFileL( iClipName->Des() );
        }
    }
    else
    {
        HandleError( KMPXVideoTvOutPlaybackNotAllowedClose );
    }
}

//  ----------------------------------------------------------------------------
//    C++ constructor
//  ----------------------------------------------------------------------------
//
CMPXVideoPlaybackController::CMPXVideoPlaybackController()
    : iAppInForeground(ETrue)
    , iForegroundPause(EFalse)
    , iAllowAutoPlay(ETrue)
    , iHelixLoadingStarted(EFalse)
{
}

//  ----------------------------------------------------------------------------
//    Destructor
//  ----------------------------------------------------------------------------
//
CMPXVideoPlaybackController::~CMPXVideoPlaybackController()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::~CMPXVideoPlaybackController"));

    delete iDrmHelper;
    delete iAccessoryMonitor;

    delete iNotIntialisedState;
    delete iInitialisingState;
    delete iInitialisedState;
    delete iPlayingState;
    delete iBufferingState;
    delete iPausedState;
    delete iSeekingState;
    delete iStoppedState;

    if ( iPlaybackMode )
    {
        delete iPlaybackMode;
        iPlaybackMode = NULL;
    }

    if ( iVolumeWatcher )
    {
        delete iVolumeWatcher;
        iVolumeWatcher = NULL;
    }

    if ( iMuteWatcher )
    {
        delete iMuteWatcher;
        iMuteWatcher = NULL;
    }

    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }

    if ( iRecognizedMimeType )
    {
        delete iRecognizedMimeType;
        iRecognizedMimeType = NULL;
    }

    if ( iVideoSeeker )
    {
        delete iVideoSeeker;
        iVideoSeeker = NULL;
    }

    if ( iFileHandle.SubSessionHandle() )
    {
        iFileHandle.Close();
    }

    if ( iBackLightTimer )
    {
        iBackLightTimer->Cancel();
        delete iBackLightTimer;
        iBackLightTimer = NULL;
    }

    if ( iPlayer )
    {
        delete iPlayer;
        iPlayer = NULL;
    }

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

    if ( iFileHandle64.SubSessionHandle() )
    {
        iFileHandle64.Close();
    }

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

}

//  ----------------------------------------------------------------------------
//    Executes general playback commands supported by the playback FW
//    CMPXVideoPlaybackController::HandleGeneralPlaybackCommandL()
//  ----------------------------------------------------------------------------
//
void
CMPXVideoPlaybackController::HandleGeneralPlaybackCommandL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandleGeneralPlaybackTypeL()"));

    if ( aCmd.IsSupported( KMPXCommandPlaybackGeneralType ) )
    {
        TMPXPlaybackCommand cmd = static_cast<TMPXPlaybackCommand>
                                      (aCmd.ValueTObjectL<TInt>(KMPXCommandPlaybackGeneralType ));

        MPX_DEBUG(_L("CMPXVideoPlaybackController::HandleGeneralPlaybackCommandL() id = %d"), cmd );

        DoHandleCommandL( cmd );
    }
}

//  ----------------------------------------------------------------------------
//    Executes general playback commands supported by the playback FW
//    CMPXVideoPlaybackController::HandleCustomPlaybackCommandL()
//  ----------------------------------------------------------------------------
//
void
CMPXVideoPlaybackController::HandleCustomPlaybackCommandL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackController::HandleCustomPlaybackCommandL()") );

    if ( aCmd.IsSupported( KMPXMediaVideoPlaybackCommand ) )
    {
        TMPXVideoPlaybackCommand cmd = static_cast<TMPXVideoPlaybackCommand>
                            (aCmd.ValueTObjectL<TInt>(KMPXMediaVideoPlaybackCommand));

        MPX_DEBUG(_L("CMPXVideoPlaybackController::HandleCustomPlaybackCommandL() cmd = %d"), cmd );

        switch ( cmd )
        {
            case EPbCmdInitView:
            {
                if ( iState != iNotIntialisedState)
                {
                    aCmd.SetTextValueL( KMPXMediaVideoPlaybackFileName, *iClipName );
                    aCmd.SetTObjectValueL<TInt>( KMPXMediaVideoMode, iPlaybackMode->GetMode() );

                    aCmd.SetTObjectValueL<TInt>( KMPXMediaVideoTvOutConnected,
                                                 iAccessoryMonitor->IsTvOutConnected() );

                    aCmd.SetTObjectValueL<TInt>( KMPXMediaVideoTvOutPlayAllowed,
                                                 iAccessoryMonitor->IsTvOutPlaybackAllowed() );

                    if ( iRecognizedMimeType )
                    {
                        aCmd.SetTextValueL( KMPXMediaVideoRecognizedMimeType, *iRecognizedMimeType );
                    }
                }

                break;
            }
            case EPbCmdSetDisplayWindow:
            {
                SetDisplayWindowL( aCmd );
                break;
            }
            case EPbCmdAbortDSA:
            {
                AbortDSA();
                break;
            }
            case EPbCmdRestartDSA:
            {
                RestartDSA( aCmd );
                break;
            }
            case EPbCmdStartVideoSeekingForward:
            {
                iState->HandleStartSeekL( ETrue );
                break;
            }
            case EPbCmdStartVideoSeekingBackward:
            {
                iState->HandleStartSeekL( EFalse );
                break;
            }
            case EPbCmdStopVideoSeeking:
            {
                iState->HandleStopSeekL();
                break;
            }
            case EPbCmdHandleForeground:
            {
                iAppInForeground = ETrue;
                iState->HandleForeground();
                break;
            }
            case EPbCmdHandleBackground:
            {
                iAppInForeground = static_cast<TBool>(aCmd.ValueTObjectL<TBool>(KMPXMediaVideoAppForeground));
                iState->HandleBackground();
                break;
            }
            case EPbCmdNaturalAspectRatio:
            {
                iState->SetAspectRatioL( aCmd, EMMFNatural );
                break;
            }
            case EPbCmdZoomAspectRatio:
            {
                iState->SetAspectRatioL( aCmd, EMMFZoom );
                break;
            }
            case EPbCmdStretchAspectRatio:
            {
                iState->SetAspectRatioL( aCmd, EMMFStretch );
                break;
            }
            case EPbCmdUpdateSeekable:
            {
                iSeekable = aCmd.ValueTObjectL<TBool>(KMPXMediaGeneralExtVideoSeekable);

                if( iFileDetails )
                {
                    iFileDetails->iSeekable &= iSeekable;
                }

                break;
            }
            case EPbCmdEndofClipReached:
            {
                iState->HandleEndOfClip();
                break;
            }
            case EPbCmdHandleIncreaseVolume:
            {
                HandleVolumeL( ETrue );
                break;
            }
            case EPbCmdHandleDecreaseVolume:
            {
                HandleVolumeL( EFalse );
                break;
            }
            case EPbCmdCustomPause:
            {
                iState->HandleCustomPause();
                break;
            }
            case EPbCmdCustomPlay:
            {
                iState->HandleCustomPlay();
                break;
            }
            case EPbCmdRetrieveBufferingPercentage:
            {
                TInt bufferingPercentage = 100;

                //
                //  Check if Helix is in the loading state.  If not, return 100 since
                //  Helix believes they are fully loaded
                //
                if ( iHelixLoadingStarted )
                {
                    bufferingPercentage = iState->RetrieveBufferingPercentage();
                }

                aCmd.SetTObjectValueL<TInt>( KMPXMediaVideoBufferingPercentage,
                                             bufferingPercentage );

                break;
            }
            default:
            {
                break;
            }
        }
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::HandlePdlCommandL()
//  ------------------------------------------------------------------------------------------------
//
void
CMPXVideoPlaybackController::HandlePdlCommandL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandlePdlCommandL()"));

    TMPXPlaybackPdCommand cmd =
        static_cast<TMPXPlaybackPdCommand>(*aCmd.Value<TInt>(KMPXCommandPlaybackGeneralType));

    switch ( cmd )
    {
        case EPbCmdStartPd:
        {
            //
            //  Set the media type to PDL so the PDL mode will be created
            //
            iMediaType = CMediaRecognizer::EProgressiveDownload;
            SetPlaybackModeL();
            iPlaybackMode->ConnectToDownloadL( aCmd );

            break;
        }
        case EPbCmdGetPdStatus:
        {
            if ( iPlaybackMode )
            {
                iPlaybackMode->GetPdlStatusL( aCmd );
            }
            else
            {
                //
                //  There must be an error since the playback mode does not exist
                //  Return a state of not downloading since the error will be handled
                //  during the MediaL() call.
                //
                aCmd.SetTObjectValueL<TMPXPlaybackPdDownloadState>( KMPXCommandPlaybackPDState,
                                                                    EPbDlStateNotDownloading );
                aCmd.SetTObjectValueL<TInt>( KMPXCommandPlaybackPDDownloadedBytes, KOneKilobyte );
                aCmd.SetTObjectValueL<TInt>( KMPXCommandPlaybackPDTotalBytes, KOneKilobyte );
            }

            break;
        }
        default:
        {
            break;
        }
    }
}

//  ----------------------------------------------------------------------------
//    Set the DataSource and DataSink to the IMMFController
//
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleCommandL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandleCommandL() - CMPXCommand"));

    if ( aCmd.IsSupported( KMPXCommandGeneralId ) )
    {
        TInt cmdId = *aCmd.Value<TInt>(KMPXCommandGeneralId);

        MPX_DEBUG(_L("CMPXVideoPlaybackController::HandleCommandL() id = 0x%08x"), cmdId );

        switch ( cmdId )
        {
            case KMPXCommandIdPlaybackGeneral:
            {
                HandleGeneralPlaybackCommandL( aCmd );
                break;
            }
            case KMPXMediaIdVideoPlayback:
            {
                HandleCustomPlaybackCommandL( aCmd );
                break;
            }
            case KMPXCommandIdPlaybackPD:
            {
                HandlePdlCommandL( aCmd );
                break;
            }
        }
    }
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackController::HandleCommandL
//
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleCommandL(TMPXPlaybackCommand aCmd, TInt /*aData*/)
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandleCommandL() - TMPXPlaybackCommand"));

    DoHandleCommandL( aCmd );
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackController::DoHandleCommandL
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackController::DoHandleCommandL(TInt aCmd)
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackController::DoHandleCommandL()"),
        _L("Command = %d"), aCmd );

    switch (aCmd)
    {
        case EPbCmdPlay:
        {
            iState->HandlePlay();
            break;
        }
        case EPbCmdPause:
        {
            iState->HandlePause();
            break;
        }
        case EPbCmdPlayPause:
        {
            iState->HandlePlayPause();
            break;
        }
        case EPbCmdStartSeekForward:
        {
            iState->HandleStartSeekL( ETrue );
            break;
        }
        case EPbCmdStartSeekBackward:
        {
            iState->HandleStartSeekL( EFalse );
            break;
        }
        case EPbCmdStopSeeking:
        {
            iState->HandleStopSeekL();
            break;
        }
        case EPbCmdClose:
        {
            iState->HandleClose();
            break;
        }
        case EPbCmdStop:
        {
            iState->HandleStop();
            break;
        }
        default:
        {
            break;
        }
    }
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackController::HandleMMFEvent
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleMMFEvent( const TMMFEvent& aEvent )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackController::HandleMMFEvent()"),
        _L("EventType = 0x%08x, ErrorCode = %d"), aEvent.iEventType, aEvent.iErrorCode );

    if ( aEvent.iEventType == KMMFEventCategoryVideoOpenComplete )
    {
        iState->HandleOpenComplete( aEvent.iErrorCode );
    }
    else if ( aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete )
    {
        iState->HandlePrepareComplete( aEvent.iErrorCode );
    }
    else if ( aEvent.iEventType == KMMFEventCategoryPlaybackComplete )
    {
        iState->HandlePlaybackComplete( aEvent.iErrorCode );
    }
    else if ( aEvent.iEventType == KMMFEventCategoryVideoLoadingStarted )
    {
        //
        //  Set the flag to retrieve the Buffering percentage from Helix
        //
        iHelixLoadingStarted = ETrue;

        iState->HandleLoadingStarted();
    }
    else if ( aEvent.iEventType == KMMFEventCategoryVideoLoadingComplete )
    {
        //
        //  Set the flag to retrieve the Buffering percentage from Helix
        //
        iHelixLoadingStarted = EFalse;

        iState->HandleLoadingComplete( aEvent.iErrorCode );
    }
#ifdef SYMBIAN_BUILD_GCE
    else if ( aEvent.iEventType == KMMFEventCategoryVideoSurfaceCreated )
    {
        TInt error = iPlayer->VideoSurfaceCreated();

        if ( error != KErrNone )
        {
            MPX_TRAPD( err, iState->SendErrorToViewL( error ) );
        }
    }
    else if ( aEvent.iEventType == KMMFEventCategoryVideoSurfaceParametersChanged )
    {
        TInt error = iPlayer->SurfaceParametersChanged();

        if ( error != KErrNone )
        {
            MPX_TRAPD( err, iState->SendErrorToViewL( error ) );
        }
    }
    else if ( aEvent.iEventType == KMMFEventCategoryVideoRemoveSurface )
    {
        TInt error = iPlayer->RemoveSurface();

        if ( error != KErrNone )
        {
            MPX_TRAPD( err, iState->SendErrorToViewL( error ) );
        }
    }
#endif // SYMBIAN_BUILD_GCE
    else if ( aEvent.iEventType == KMMFEventCategoryVideoPlayerGeneralError )
    {
        switch ( aEvent.iErrorCode )
        {
            case KErrDisconnected:
            case KErrTimedOut:
            {
                iState->ResolveTimeoutError(aEvent.iErrorCode);
                break;
            }
            case KErrHardwareNotAvailable:
            case KErrMMAudioDevice:
            case KErrMMVideoDevice:
            {
                // Resource lost; go to pause state
                iState->HandlePause();
                break;
            }
            default:
            {
                iState->HandleUnexpectedError( aEvent.iErrorCode );
                break;
            }
        }
    }
    else if ( aEvent.iEventType == KMMFEventCategoryAudioResourceAvailable )
    {
      // ?????
    }
    else
    {
        //
        //  unexpected MMF event
        //
    }
}

//  ----------------------------------------------------------------------------
//    CMPXVideoHelixPlayback::RestartDSA()
//
//    This is not state dependent so the ctlr can handle this
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackController::RestartDSA( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoHelixPlayback::RestartDSA()"));

    CMPXVideoRegion* videoRegion =
        aCmd.ValueCObjectL<CMPXVideoRegion>(KMPXMediaVideoPlaybackDSARegion);

    iPlayer->RestartDsa( videoRegion->GetRegion() );

    delete videoRegion;
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackController::SetDisplayWindowL()
//
//    This is not state dependent .
//    Note: The view can control the when it sends this command
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackController::SetDisplayWindowL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::SetDisplayWindowL()"));

    TRect rect =
        static_cast<TRect>( aCmd.ValueTObjectL<TRect>(KMPXMediaVideoPlaybackTRect) );

    MPX_DEBUG(_L("CMPXVideoHelixPlayback::SetDisplayWindowL() region sent"));

    CMPXVideoRegion* videoRegion =
        aCmd.ValueCObjectL<CMPXVideoRegion>(KMPXMediaVideoPlaybackDSARegion);

    CleanupStack::PushL( videoRegion );

    iPlayer->SetDisplayWindowL( rect, rect, videoRegion->GetRegion() );

    CleanupStack::PopAndDestroy( videoRegion );
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetVolumeCenRepL
//  Sets the volume level in Central Repository
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SetVolumeCenRepL( TInt aVolume )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackController::SetVolumeCenRepL()"),
        _L("aVolume = %d"), aVolume );

    if ( iFileDetails && iFileDetails->iAudioEnabled )
    {
        TInt volume( 0 );

        if ( aVolume > KPbPlaybackVolumeLevelMax )
        {
            volume = KPbPlaybackVolumeLevelMax;
        }
        else if ( aVolume < KPbPlaybackVolumeLevelMin )
        {
            volume = KPbPlaybackVolumeLevelMin;
        }
        else
        {
            volume = aVolume;
        }

        MPX_DEBUG(
            _L("CMPXVideoPlaybackController::SetVolumeCenRepL(): Setting volume = %d"), volume );

        if ( volume > 0 )
        {
            //
            // Media player's CenRep volume range : 0 - number of volume steps
            // MPX framework volume range : 0-100
            //
            iVolumeWatcher->SetValueL( volume / iVolumeNormalizer );
            SetMuteCenRepL( EFalse );
        }
        else
        {
            //
            // save both mute and current volume values in CenRep
            //
            iVolumeWatcher->SetValueL( 0 );
            SetMuteCenRepL( ETrue );
        }
    }
    else
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackController::SetVolumeCenRepL(): Audio isn't enabled"));
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetMuteCenRepL
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackController::SetMuteCenRepL( TBool aMute )
{
    if ( iFileDetails && iFileDetails->iAudioEnabled )
    {
        iMuteWatcher->SetValueL( aMute );

        MPX_DEBUG(_L("CMPXVideoPlaybackController::SetMuteCenRepL(): Mute = %d"), aMute );
    }
    else
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackController::SetMuteCenRepL() : Audio isn't enabled"));
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetVolumeMMFL
//
//  Sets the volume level in MMF
//
//  NOTE: This is done only after the HandleSettingChange callback from CenRep
//        has been received as a result of a prior iVolumeWatcher->SetValueL call
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SetVolumeMMFL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::SetVolumeMMFL()"));

    TInt volume = iVolumeWatcher->CurrentValueL();
    TBool mute = iMuteWatcher->CurrentValueL();

    MPX_DEBUG(_L("CMPXVideoPlaybackController::SetVolumeMMFL() volume = %d, mute = %d")
            , volume, mute );

    TInt newVolume = 0;

    if ( ! mute )
    {
        //
        // If it was muted and previous volume level was 0, set the volume to 1
        //
        if ( volume == 0 )
        {
            volume++;
        }

        newVolume =
            volume  * iVolumeNormalizer * iFileDetails->iMaxVolume / KPbPlaybackVolumeLevelMax;
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackController::SetVolumeMMFL() new volume = %d"), newVolume );

    //  Change MMF Audio player's volume
    iPlayer->SetVolumeL( newVolume );

    if ( ! mute )
    {
        iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPVolumeChanged,
                                          newVolume,
                                          KErrNone );
    }
}

//  ------------------------------------------------------------------------------------------------
//    Handle a change in a setting value.
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleSettingChange( const TUid& aRepositoryUid,
                                                       TUint32 aSettingId )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::HandleSettingChange()"));

    if ( aRepositoryUid == KCRUidMPXVideoSettings )
    {
        switch ( aSettingId )
        {
            case KMPXVideoPlaybackMute:
            {
                iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPMuteChanged,
                                                  iMuteWatcher->CurrentValueL(),
                                                  KErrNone );
                // fall through
            }
            case KMPXVideoPlaybackVolume:
            {
                //
                //  this is the callback from cen-rep
                //  let the state decide if it needs to do something
                //
                iState->HandleVolumeChange();
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetPropertyL()
//
//  Sets the property of the plugin
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SetPropertyL( TMPXPlaybackProperty aProperty, TInt aValue )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::SetPropertyL()"),
                   _L("prop %d, val %d"), aProperty, aValue );

    switch ( aProperty )
    {
        case EPbPropertyPosition:
        {
            iState->HandleSetPositionL( aValue );

            //
            // Set Position is a synchronous call. It needs not wait for any callback
            // so HandlePluginEvent can be issued at this time.
            //
            iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPSetComplete,
                                              aProperty,
                                              KErrNone );
            break;
        }
        case EPbPropertyVolume:
        {
            //
            // Volume is an asynchronous call. After a new volume value is saved
            // in CenRep, it needs to wait for a callback. At that time, HandlePluginEvent
            // for volume will be issued.
            //
            SetVolumeCenRepL( aValue );
            break;
        }
        case EPbPropertyMute:
        {
            //
            // Mute is an asynchronous call. After a new mute value is saved
            // in CenRep, it needs to wait for a callback. At that time, HandlePluginEvent
            // for mute will be issued.
            //
            SetMuteCenRepL( aValue );
            break;
        }
        default:
        {
            User::Leave( KErrNotSupported );
        }
    }

}

//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::GetPropertyL
//
//    Gets a property of the plugin (async)
//  ----------------------------------------------------------------------------

void CMPXVideoPlaybackController::GetPropertyL( TMPXPlaybackProperty aProperty ) const
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::GetPropertyL()"),
                   _L("aProperty = %d"), aProperty );

    TInt value = 0;
    TInt err = KErrNone;

    switch( aProperty )
    {
        case EPbPropertyPosition:
        {
            TTimeIntervalMicroSeconds pos = iPlayer->PositionL();

            value = pos.Int64() / KPbMilliMultiplier;

            break;
        }
        case EPbPropertyDuration:
        {
            if ( iFileDetails )
            {
                value = iFileDetails->iDuration.Int64() / KPbMilliMultiplier;
            }

            break;
        }
        case EPbPropertyVolume:
        {
            TInt volume = 0;

            if ( ! iMuteWatcher->CurrentValueL() )
            {
                volume = iVolumeWatcher->CurrentValueL() * iVolumeNormalizer;
            }

            value = volume;

            break;
        }
        case EPbPropertyMaxVolume:
        {
            if ( iFileDetails )
            {
                value = iFileDetails->iMaxVolume;
            }
            break;
        }
        case EPbPropertyMute:
        {
            TInt volume = iPlayer->Volume();

            value = volume ? 0 : 1;

            break;
        }
        case EPbPropertySupportedFeatures:
        {
            value = 0;
            break;
        }
        default:
        {
            User::Leave( KErrNotSupported );
        }
    }

    if ( iMPXPluginObs )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackController::GetPropertyL() value = %d"), value);

        iMPXPluginObs->HandleProperty( aProperty, value, err );
    }
}

//
//  ----------------------------------------------------------------------------
//    Sends the media properties
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SendMediaL( const TArray<TMPXAttribute>& aAttrs )
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::SendMediaL()"));

    TInt retError = KErrNone;

    RArray<TInt> suppIds;
    CleanupClosePushL( suppIds );

    suppIds.AppendL( KMPXMediaIdGeneral );
    suppIds.AppendL( KMPXMediaIdVideo );

    CMPXMedia* media = CMPXMedia::NewL( suppIds.Array() );

    CleanupStack::PopAndDestroy( &suppIds );
    CleanupStack::PushL( media );

    retError = iState->RetrieveFileDetailsL( aAttrs, media );

    iMPXPluginObs->HandleMedia( *media, retError );

    CleanupStack::PopAndDestroy( media );
}

//
//  ----------------------------------------------------------------------------
//  CMPXVideoPlaybackController::ChangeState
//  ----------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::ChangeState(TMPXVideoPlaybackState aChangeToState)
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::ChangeState()"),
                    _L("newState = %d"), aChangeToState );

    if ( aChangeToState != iState->GetState() )
    {
        switch ( aChangeToState )
        {
            case EMPXVideoPlaying:
            {
                iState = iPlayingState;
                
                //
                //  If clip is audio only, stop the backlight timer and break switch
                //  If clip has audio and video, proceed to the next case which will
                //  start the backlight timer if needed
                //
                if ( iFileDetails->iVideoEnabled )
                {
                    StartBackLightTimer();
                }
                else
                {
                    CancelBackLightTimer();
                }
                
                break;
            }
            case EMPXVideoPaused:
            {
                iState = iPausedState;
                CancelBackLightTimer();
                break;
            }
            case EMPXVideoInitializing:
            {
                iState = iInitialisingState;
                StartBackLightTimer();
                break;
            }
            case EMPXVideoInitialized:
            {
                iState = iInitialisedState;
                break;
            }
            case EMPXVideoBuffering:
            {
                iState = iBufferingState;
                StartBackLightTimer();
                break;
            }
            case EMPXVideoSeeking:
            {
                iState = iSeekingState;
                break;
            }
            case EMPXVideoStopped:
            {
                iState = iStoppedState;
                CancelBackLightTimer();
                break;
            }
            case EMPXVideoNotInitialized:
            {
                ResetMemberVariables();
                iState = iNotIntialisedState;
                CancelBackLightTimer();
                break;
            }
        }
    }
}

//  ------------------------------------------------------------------------------------------------
//    Reads the file details from the media properties
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::ReadFileDetailsL()
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::ReadFileDetailsL()"));

    CMPXVideoFileDetails* temp = iFileDetails;

    //
    //  Create the file details class and delete old data
    //
    iFileDetails = CMPXVideoFileDetails::NewL();
    delete temp;

    iFileDetails->iClipName = iClipName->AllocL();

    //
    //  Resolution
    //
    TSize size;
    iPlayer->VideoFrameSizeL( size );
    iFileDetails->iResolutionHeight = size.iHeight;
    iFileDetails->iResolutionWidth = size.iWidth;

    //
    //  Video Track
    //
    TInt videoBitRate = iPlayer->VideoBitRateL();
    iFileDetails->iVideoEnabled = ( videoBitRate != 0 );

    iFileDetails->iBitRate = videoBitRate + iPlayer->AudioBitRateL();

    //
    // Drm Protected
    //
    if ( iFileHandle.SubSessionHandle() )
    {
        iFileDetails->iDrmProtected = iDrmHelper->IsProtected( iFileHandle );
    }
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    else if ( iFileHandle64.SubSessionHandle() )
    {
        iFileDetails->iDrmProtected = iDrmHelper->IsProtected64( iFileHandle64 );
    }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API


    //
    //  Audio Track
    //
    iFileDetails->iAudioEnabled = iPlayer->AudioEnabledL();

    //
    //  Duration
    //
    iFileDetails->iDuration = iPlayer->DurationL();

    //
    //  Max Volume
    //
    iFileDetails->iMaxVolume = iPlayer->MaxVolume();

    // 
    //  FourCC Code
    //
    iFileDetails->iFourCCCode  = iPlayer->FourCCCode();    
    
    //
    //  Mime Type
    //
    HBufC8* mimeType = HBufC8::NewL( KMAXMIMETYPELENGTH );
    CleanupStack::PushL( mimeType );

    TPtr8 des = mimeType->Des();

    TInt err = iPlayer->VideoFormatMimeType( des );

    if ( ! err )
    {
        HBufC* mimeType16 = HBufC::NewLC( des.Length() );
        TPtr des16( mimeType16->Des() );

        des16.Copy( des );
        iFileDetails->iMimeType = des16.AllocL();

        CleanupStack::PopAndDestroy( mimeType16 );
    }

    CleanupStack::PopAndDestroy( mimeType );

    //
    //  Loop through the meta data entries
    //
    TInt metaCount = 0;

    MPX_TRAPD( error, metaCount = iPlayer->NumberOfMetaDataEntriesL() );

    if ( ! error )
    {
        CMMFMetaDataEntry* metaData = NULL;

        for ( TInt i = 0 ; i < metaCount ; ++i )
        {
            metaData = iPlayer->MetaDataEntryL(i);

            CleanupStack::PushL( metaData );

            if ( ! metaData->Name().CompareF( KSeekable ) )
            {
                if ( ! metaData->Value().CompareF( KTrue ) )
                {
                    iFileDetails->iSeekable = ETrue;
                }
            }
            else if ( ! metaData->Name().CompareF( KLiveStream ) )
            {
                if ( ! metaData->Value().CompareF( KTrue ) )
                {
                    iFileDetails->iLiveStream = ETrue;
                }
            }
            else if ( ! metaData->Name().CompareF( KTitle ) )
            {
                iFileDetails->iTitle = metaData->Value().AllocL();
            }
            else if ( ( ! metaData->Name().CompareF( KArtist ) ) ||
                      ( ! metaData->Name().CompareF( KAuthor ) ) )
            {
                iFileDetails->iArtist = metaData->Value().AllocL();
            }
            else if ( ! metaData->Name().CompareF( KFormat ) )
            {
                //
                //  Do not overwrite the mime type
                //
                if ( ! iFileDetails->iMimeType )
                {
                    iFileDetails->iMimeType = metaData->Value().AllocL();
                }
            }
            else if ( !metaData->Name().CompareF(KMPStreamingPauseSupported) )
            {
                if ( ! metaData->Value().CompareF( KFalse ) )
                {
                    // Streaming Pause is not supported by server
                    iFileDetails->iPausableStream = EFalse;
                }
            }

            CleanupStack::PopAndDestroy( metaData );
        }

        // Update the seek value for asx clips with the value
        // received from the engine ( single item )
        iFileDetails->iSeekable &= iSeekable;

        //
        // If it is live streaming, seekable and PausableStream should be EFalse
        // Helix sends ETrue for "Seekable" and "PausableStream" in case of live streaming
        //
        if ( iFileDetails->iLiveStream || !iFileDetails->iPausableStream )
        {
            iFileDetails->iSeekable = EFalse;
            iFileDetails->iPausableStream = EFalse;
        }
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::GetModeToSet
//  ------------------------------------------------------------------------------------------------
//
TMPXVideoMode CMPXVideoPlaybackController::GetModeToSet()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackController::GetModeToSet()"));

    TMPXVideoMode changeToMode = EMPXVideoLocal;   // default

    //
    //  CMediaRecognizer can not recognize live media - as there isn't
    //  sufficient info available from file name or file handle to make that
    //  determination - so first check the file details for LIVE
    //
    if ( iFileDetails && iFileDetails->iLiveStream )
    {
        changeToMode = EMPXVideoLiveStreaming;
    }
    else
    {
        switch ( iMediaType )
        {
            case CMediaRecognizer::ELocalVideoFile: //intentional fall through
            case CMediaRecognizer::ELocalAudioFile: //intentional fall through
            case CMediaRecognizer::ELocalAudioPlaylist:
            {
                // changeToMode initialized to EMPXVideoLocal
                // re-assignment not needed
                break;
            }
            case CMediaRecognizer::EProgressiveDownload:
            {
                changeToMode = EMPXVideoProgressiveDownload;
                break;
            }
            case CMediaRecognizer::ELocalAsxFile:    //intentional fall through
            case CMediaRecognizer::ELocalRamFile:    //intentional fall through
            case CMediaRecognizer::ELocalSdpFile:    //intentional fall through
            case CMediaRecognizer::EUrl:
            {
                changeToMode = EMPXVideoStreaming;
                break;
            }
            case CMediaRecognizer::EUnidentified:   //intentional fall through
            default:
            {
                changeToMode = EMPXVideoLocal;
                break;
            }
        }
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackController::GetModeToSet() mode = %d"), changeToMode);

    return changeToMode;
}


//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetPlaybackModeL
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SetPlaybackModeL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::SetPlaybackModeL()"));

    //
    //  Do not allow the mode to be switched from PDL once it is set
    //
    if ( ! iPlaybackMode || iPlaybackMode->GetMode() != EMPXVideoProgressiveDownload )
    {
        //
        //  determine what mode the controller needs to be in
        //
        TMPXVideoMode mode = GetModeToSet();

        //
        //  Set new mode when no mode has been set or this is a new mode
        //
        if ( ! iPlaybackMode || mode != iPlaybackMode->GetMode() )
        {
            CMPXVideoPlaybackMode* temp = iPlaybackMode;

            switch ( mode )
            {
                case EMPXVideoLocal:
                    iPlaybackMode = CMPXLocalPlaybackMode::NewL( this );
                    break;
                case EMPXVideoProgressiveDownload:
                    iPlaybackMode = CMPXProgressiveDLPlaybackMode::NewL( this );
                    break;
                case EMPXVideoStreaming:
                    iPlaybackMode = CMPXStreamingPlaybackMode::NewL( this );
                    break;
                case EMPXVideoLiveStreaming:
                    iPlaybackMode = CMPXLiveStreamingPlaybackMode::NewL( this );
                    break;
                default:
                    iPlaybackMode = CMPXLocalPlaybackMode::NewL( this );
                    break;
            }

            delete temp;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::DetermineMediaTypeL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::DetermineMediaTypeL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::DetermineMediaTypeL()"));

    CMediaRecognizer::TMediaType mediaType = CMediaRecognizer::EUnidentified;

    CMediaRecognizer* recognizer = CMediaRecognizer::NewL();
    CleanupStack::PushL(recognizer);

    if ( iFileHandle.SubSessionHandle() )
    {
        TInt size(0);
        iFileHandle.Size( size );

        if ( size > 0 )
        {
            mediaType = recognizer->IdentifyMediaTypeL( iClipName->Des(), iFileHandle, ETrue );

            iRecognizedMimeType = recognizer->MimeTypeL( iFileHandle ).AllocL();
        }
    }
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    else if ( iFileHandle64.SubSessionHandle() )
    {
        TInt64 size(0);
        iFileHandle64.Size( size );

        if ( size > 0 )
        {
            mediaType = recognizer->IdentifyMediaTypeL( iClipName->Des(), iFileHandle64, ETrue );

            iRecognizedMimeType = recognizer->MimeTypeL( iFileHandle64 ).AllocL();
        }
    }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    else
    {
        if ( recognizer->IsValidStreamingPrefix( iClipName->Des() ) )
        {
            //
            //   check the descriptor to determine if it has a valid streaming prefix
            //
            mediaType = CMediaRecognizer::EUrl;
        }
    }

    CleanupStack::PopAndDestroy( recognizer );

    iMediaType = mediaType;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsPhoneCall
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsPhoneCall()
{
    TInt state = 0;
    TBool call;
    RProperty SystemState;

    SystemState.Attach( KPSUidCtsyCallInformation, KCTsyCallState );

    TInt error = SystemState.Get( state );

    if ( error || ( state == EPSCTsyCallStateNone || state == EPSCTsyCallStateUninitialized ) )
    {
        call = EFalse;
    }
    else
    {
        call = ETrue;
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsPhoneCall(%d)"), call);

    SystemState.Close();
    return call;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsActivePhoneCall
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsActivePhoneCall()
{
    TBool retVal = EFalse;
    TInt state = 0;
    RProperty SystemState;

    SystemState.Attach( KPSUidCtsyCallInformation, KCTsyCallState );

    if ( !SystemState.Get( state ) && state == EPSCTsyCallStateConnected )
    {
        retVal = ETrue;
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsActivePhoneCall(%d)"), retVal);

    SystemState.Close();

    return retVal;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsVideoCall
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsVideoCall()
{
    TInt callType;

    RProperty::Get( KPSUidCtsyCallInformation,
                    KCTsyCallType,
                    callType );// Ignore errors

    TBool videoCall = ( callType == EPSCTsyCallTypeH324Multimedia );

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsVideoCall(%d)"), videoCall );

    return videoCall;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsVoiceCall
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsVoiceCall()
{
    TBool call;
    TInt type = 0;
    RProperty SystemState;

    SystemState.Attach( KPSUidCtsyCallInformation, KCTsyCallState );

    TInt error = SystemState.Get( KPSUidCtsyCallInformation, KCTsyCallType, type );

    if ( ! error && ( (type == EPSCTsyCallTypeCSVoice) || (type == EPSCTsyCallTypeVoIP) ) )
    {
        call = ETrue;
    }
    else
    {
        call = EFalse;
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsVoiceCall(%d)"), call);

    SystemState.Close();
    return call;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsAlarm
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsAlarm()
{
    TBool alarm( EFalse );

    RProperty::Get( KPSUidCoreApplicationUIs, KLightsAlarmLightActive, alarm );

    alarm = ( alarm == ELightsBlinking ) ? ETrue : EFalse; // Ignore errors

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsAlarm(%d)"), alarm);

    return alarm;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackController::IsDisplayOff
// -----------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackController::IsDisplayOff()
{
    TBool displayState;
    HAL::Get( HALData::EDisplayState, displayState );

    MPX_DEBUG(_L("CMPXVideoPlaybackController::IsDisplayOff(%d)"), !displayState);

    return !displayState;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::HandleTvOutEventL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleTvOutEventL( TBool aConnected )
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::HandleTvOutEventL()"),
                    _L("aConnected = %d"), aConnected );

    TBool playbackAllowed = ETrue;

    if ( aConnected )
    {
        //
        //  TV-Out accessory connected
        //
        if ( ! iAccessoryMonitor->IsTvOutPlaybackAllowed() )
        {
            //
            //  Clip has DRM protection and TV-Out is connected
            //  Pause playback and display info note
            //
            DoHandleCommandL( EPbCmdPause );

            playbackAllowed = EFalse;

            iState->SendErrorToViewL( KMPXVideoTvOutPlaybackNotAllowed );
        }
    }

    //
    //  Send notice to the playback view with TV-Out connection status
    //  and TV-Out playback allowed flag
    //
    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL( message );

    message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMediaIdVideoPlayback );
    message->SetTObjectValueL<TMPXVideoPlaybackCommand>
        ( KMPXMediaVideoPlaybackCommand, EPbCmdTvOutEvent );
    message->SetTObjectValueL<TInt>( KMPXMediaVideoTvOutConnected, aConnected );
    message->SetTObjectValueL<TInt>( KMPXMediaVideoTvOutPlayAllowed, playbackAllowed );

    iMPXPluginObs->HandlePlaybackMessage( message, KErrNone );

    CleanupStack::PopAndDestroy( message );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::CancelBackLightTimer
//  ------------------------------------------------------------------------------------------------
void  CMPXVideoPlaybackController::CancelBackLightTimer()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::CancelBackLightTimer()"));

    if ( iBackLightTimer->IsActive() )
    {
        iBackLightTimer->Cancel();
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackState::StartBackLightTimer
//  ------------------------------------------------------------------------------------------------
void  CMPXVideoPlaybackController::StartBackLightTimer()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::StartBackLightTimer()"));

    if ( !iBackLightTimer->IsActive() )
    {
        iBackLightTimer->Start(
            0,
            KMPXBackLightTimeOut,
            TCallBack( CMPXVideoPlaybackController::HandleBackLightTimout, this ));
    }
}

// -------------------------------------------------------------------------------------------------
// Handle back light timer timeout callback
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackController::HandleBackLightTimout( TAny* aPtr )
{
    static_cast<CMPXVideoPlaybackController*>(aPtr)->DoHandleBackLightTimout();
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
// Handle back light timer timeout
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::DoHandleBackLightTimout()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::DoHandleBackLightTimout()"));

    User::ResetInactivityTime();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackController::InitVolumeWatchers()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::InitVolumeWatchers()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::InitVolumeWatchers()"));

    if ( ! iVolumeWatcher )
    {
        MPX_TRAPD( err,
                iVolumeWatcher = CMPXCenRepWatcher::NewL( KCRUidMPXVideoSettings,
                                                          KMPXVideoPlaybackVolume,
                                                          this ) );
    }

    if ( ! iMuteWatcher )
    {
        MPX_TRAPD( err,
                iMuteWatcher = CMPXCenRepWatcher::NewL( KCRUidMPXVideoSettings,
                                                        KMPXVideoPlaybackMute,
                                                        this ) );
    }

    //
    // MPX framework volume range : 0-100
    // Media player volume range : 0-10
    // MPX video player volume range : 0 - volume steps (defined in PlaybackHelper)
    // For IAD, need to manipulate volume to save in cenrep
    // MPX Framework volume / iVolumeNormalizer => CenRep
    //
    CDevSoundIf* devSoundIf = CDevSoundIf::NewL();
    TInt volumeSteps =  devSoundIf->GetNumberOfVolumeSteps();
    delete devSoundIf;

    //
    // MPX issues GetPropertyL(EPbPropertyVolume) before MMF gets called.
    // Hence, set iVolumeNormalizer value during construction time.
    //
    SetVolumeSteps( volumeSteps );
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::HandleError
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleError( TInt error )
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::HandleError()"),
                    _L("error = %d"), error );

    ChangeState( EMPXVideoNotInitialized );

    //
    // Move the FW state to Initialized so that it can request for Media
    //
    iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPInitialised, 0, KErrNone );

    iPBPluginError = error;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::ResetMemberVariables
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::ResetMemberVariables()
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::ResetMemberVariables()"));

    iPlayer->Reset();

    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }

    if ( iRecognizedMimeType )
    {
        delete iRecognizedMimeType;
        iRecognizedMimeType = NULL;
    }

    if ( iFileHandle.SubSessionHandle() )
    {
        iFileHandle.Close();
    }

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    if ( iFileHandle64.SubSessionHandle() )
    {
        iFileHandle64.Close();
    }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    
    // reset to True
    iSeekable = ETrue;

    //
    //  Needs to be last thing destroyed since PDL requires the
    //  controller to be unloaded to move clip
    //
    delete iPlaybackMode;
    iPlaybackMode = NULL;

    //
    //  Reset the flag to retrieve the Buffering percentage from Helix
    //
    iHelixLoadingStarted = EFalse;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::HandleVolumeL
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::HandleVolumeL( TBool aIncrease )
{
    MPX_ENTER_EXIT( _L("CMPXVideoPlaybackController::HandleVolumeL()"),
                    _L("aIncrease = %d"), aIncrease );

    //
    // Media player's CenRep volume range : 0 - number of volume steps
    // MPX framework volume range : 0-100
    //
    TInt volume = iVolumeWatcher->CurrentValueL();

    if ( aIncrease )
    {
        //
        // increase the current volume
        //
        volume++;
    }
    else if ( volume > 0 )
    {
        //
        // decrease the current volume
        //
        volume--;
    }

    //
    // save the current volume level in CenRep
    //
    SetVolumeCenRepL( volume * iVolumeNormalizer );
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackController::SetVolumeSteps
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::SetVolumeSteps( TInt aVolumeSteps )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackController::SetVolumeSteps(%d)"), aVolumeSteps );

    iVolumeNormalizer = KPbPlaybackVolumeLevelMax / aVolumeSteps;
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackController::OpenFile64L()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackController::OpenFile64L( const TDesC& aMediaFile,
                                               RFile64& aFile,
                                               TInt aAccessPointId )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::OpenFile64L( RFile64 )"),
                   _L("file = %S"), &aMediaFile );

    TBool fileExists = EFalse;

    ChangeState( EMPXVideoInitializing );

    iClipName = aMediaFile.AllocL();
    iAccessPointId = aAccessPointId;

    //
    //  If file handle exists, duplicate it to the controller
    //
    if ( aFile.SubSessionHandle() )
    {
        if ( iFileHandle64.SubSessionHandle() )
        {
            iFileHandle64.Close();
        }

        User::LeaveIfError( iFileHandle64.Duplicate( aFile ) );

        fileExists = ETrue;
    }

    DetermineMediaTypeL();
    SetPlaybackModeL();

    //
    //  Create accessory monitor to search for TV-Out events
    //
    if ( ! iAccessoryMonitor )
    {
        iAccessoryMonitor = CMPXVideoAccessoryObserver::NewL( this );
    }

    if ( iAccessoryMonitor->IsTvOutPlaybackAllowed() )
    {
        if ( fileExists )
        {
            //
            //  Ensure there are rights for protected clips
            //
            TInt drmError = iDrmHelper->GetDrmRightsStatus64( iFileHandle64 );

            if ( drmError )
            {
                //
                //  Send error to observer for handling
                //
                HandleError( drmError );
            }
            else
            {
                iState->OpenFile64L( iFileHandle64 );
            }
        }
        else
        {
            iState->OpenFileL( iClipName->Des() );
        }
    }
    else
    {
        HandleError( KMPXVideoTvOutPlaybackNotAllowedClose );
    }
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

// End of file
