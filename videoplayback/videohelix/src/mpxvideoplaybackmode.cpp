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

// Version : %version: 19 %



//
//  INCLUDE FILES
//
#include <mmf/server/mmffile.h>
#include <MMFROPCustomCommandConstants.h>
#include <mpxplaybackpluginobserver.h>

#include <etel.h>    // 3G
#include <etelmm.h>
#include <mmtsy_names.h>

#include "mpxvideoplaybackcontroller.h"
#include "mpxvideoplaybackmode.h"
#include "mpxvideofiledetails.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideodrmhelper.h"

// Constants
const TUid KUidInterfaceMMFHelixController = {0x101F855D};

_LIT8(KMMFAccessPoint,"AccessPoint:Integer = \"%d\";");

// ============================ MEMBER FUNCTIONS ===============================

//  ------------------------------------------------------------------------------------------------
//    Symbian 2nd phase constructor can leave.
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMode::ConstructL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::ConstructL()"));

    iVideoPlaybackCtlr = aVideoPlaybackCtlr;
}

//  ------------------------------------------------------------------------------------------------
//    C++ constructor
//  ------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackMode::CMPXVideoPlaybackMode()
{
}

//  ------------------------------------------------------------------------------------------------
//    Destructor
//  ------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackMode::~CMPXVideoPlaybackMode()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::~CMPXVideoPlaybackMode()"));
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::OpenFileL()
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackMode::OpenFileL( const TDesC& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::OpenFileL()"),
                   _L("file = %S"), &aMediaFile );

    iVideoPlaybackCtlr->iPlayer->OpenFileL( aMediaFile );
}

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::OpenFileL()
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackMode::OpenFileL( const RFile& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::OpenFileL()"));

    iVideoPlaybackCtlr->iPlayer->OpenFileL( aMediaFile );
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ----------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::OpenFile64L()
//  ----------------------------------------------------------------------------
void CMPXVideoPlaybackMode::OpenFile64L( const RFile64& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::OpenFile64L( RFile64 )"));

    iVideoPlaybackCtlr->iPlayer->OpenFile64L( aMediaFile );
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::ConnectToDownloadL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMode::ConnectToDownloadL( CMPXCommand& /*aCmd*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::ConnectToDownloadL()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::GetPdlStatusL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMode::GetPdlStatusL( CMPXCommand& /*aCmd*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::GetPdlStatusL()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::HandleOpenComplete()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMode::HandleOpenComplete()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::HandleOpenComplete()"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::UpdateSeekPosition()
//  ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackMode::UpdateSeekPosition( TInt64& aPosition )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::UpdateSeekPosition()"));

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
        MMPXPlaybackPluginObserver::EPPositionChanged,
        aPosition / KPbMilliMultiplier,
        KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::HandlePause()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackMode::HandlePause()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackMode::HandlePause()"));

    MPX_TRAPD( err, iVideoPlaybackCtlr->iPlayer->PauseL() );

    if ( err == KErrNone )
    {
        iVideoPlaybackCtlr->ChangeState( EMPXVideoPaused );

        iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                                                              0,
                                                              err );
    }
    else
    {
        TRAP_IGNORE( iVideoPlaybackCtlr->iState->SendErrorToViewL( err ) );
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::HandleBackground()
//  ------------------------------------------------------------------------------------------------
void CMPXVideoPlaybackMode::HandleBackground()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::HandleBackground()"));

    if ( iVideoPlaybackCtlr->iAppInForeground )
    {
        if ( iVideoPlaybackCtlr->IsAlarm() ||
             ( iVideoPlaybackCtlr->IsKeyLocked() && iVideoPlaybackCtlr->iFileDetails->iVideoEnabled ) )
        {
            iVideoPlaybackCtlr->iForegroundPause = ETrue;
            iVideoPlaybackCtlr->iState->HandlePause();
        }
        else if ( iVideoPlaybackCtlr->IsPhoneCall() || iVideoPlaybackCtlr->IsVideoCall() )
        {
            iVideoPlaybackCtlr->iForegroundPause = EFalse;
            iVideoPlaybackCtlr->iState->HandlePause();
        }
    }
    else
    {
        iVideoPlaybackCtlr->iForegroundPause = EFalse;
        iVideoPlaybackCtlr->iState->HandlePause();
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackMode::CanPlayNow
//  ------------------------------------------------------------------------------------------------
TBool CMPXVideoPlaybackMode::CanPlayNow()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::CanPlayNow"));
    TBool playAllowed = EFalse;

    if ( iVideoPlaybackCtlr->iAppInForeground && iVideoPlaybackCtlr->iAllowAutoPlay )
    {
        if ( iVideoPlaybackCtlr->IsVideoCall() )
        {
            MPX_TRAPD( err,
                iVideoPlaybackCtlr->iState->SendErrorToViewL( KMPXVideoCallOngoingError ) );
        }
        else if ( iVideoPlaybackCtlr->IsKeyLocked() &&
                  iVideoPlaybackCtlr->iFileDetails->iVideoEnabled )
        {
            iVideoPlaybackCtlr->iForegroundPause = ETrue;
        }
        else if ( ! iVideoPlaybackCtlr->iAccessoryMonitor->IsTvOutPlaybackAllowed() )
        {
            MPX_TRAPD( err,
                iVideoPlaybackCtlr->iState->SendErrorToViewL( KMPXVideoTvOutPlaybackNotAllowed ) );
        }
        else
        {
            playAllowed = ETrue;
        }
    }
    return playAllowed;
}

//  ------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackMode::IsNetworkMode2GL
//  ------------------------------------------------------------------------------------------------
TBool CMPXVideoPlaybackMode::IsNetworkMode2GL()
{
    TBool networkMode2g( EFalse );
    RMobilePhone mobilePhone;
    RTelServer telServer;          // 3G

    RTelServer::TPhoneInfo phoneInfo;
    RMobilePhone::TMobilePhoneNetworkMode networkMode;

    User::LeaveIfError( telServer.Connect() );
    User::LeaveIfError( telServer.LoadPhoneModule( KMmTsyModuleName ) );

    TInt numPhones;
    User::LeaveIfError(telServer.EnumeratePhones( numPhones ) );

    if ( numPhones <= 0 )
    {
        User::Leave( KErrCorrupt );
    }

    User::LeaveIfError( telServer.GetPhoneInfo( 0, phoneInfo ) );
    User::LeaveIfError( mobilePhone.Open( telServer, phoneInfo.iName ) );
    User::LeaveIfError( mobilePhone.Initialise() );

    User::LeaveIfError( mobilePhone.GetCurrentMode( networkMode ) );

    if ( networkMode == RMobilePhone::ENetworkModeGsm)
    {
        networkMode2g = ETrue;
    }

    mobilePhone.Close();
    telServer.Close();

    MPX_DEBUG(_L("CMPXVideoPlaybackMode::IsNetworkMode2GL(%d)"), networkMode2g);

    return networkMode2g;
}


//  ------------------------------------------------------------------------------------------------
//    CMPXVideoPlaybackMode::IsTvOutAllowedL()
//  ------------------------------------------------------------------------------------------------
TBool CMPXVideoPlaybackMode::IsTvOutAllowedL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackMode::IsTvOutAllowedL()"));

    TBool allowTvOut =
        iVideoPlaybackCtlr->iDrmHelper->IsTvOutAllowedL( iVideoPlaybackCtlr->iFileHandle );

    MPX_DEBUG(_L("CMPXVideoPlaybackMode::IsTvOutAllowedL(%d)"), allowTvOut);

    return allowTvOut;
}

//************************************************************************************************//
//          CMPXLocalPlaybackMode
//************************************************************************************************//
CMPXVideoPlaybackMode*
CMPXLocalPlaybackMode::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
   MPX_ENTER_EXIT(_L("CMPXLocalPlaybackMode::NewL()"));
   CMPXLocalPlaybackMode* self = new( ELeave ) CMPXLocalPlaybackMode();
   CleanupStack::PushL( self );
   self->ConstructL(aVideoPlaybackCtlr);
   CleanupStack::Pop();
   return self;
}

CMPXLocalPlaybackMode::~CMPXLocalPlaybackMode()
{
    MPX_DEBUG(_L("CMPXLocalPlaybackMode::~CMPXLocalPlaybackMode()"));
}

//************************************************************************************************//
//          CMPXStreamingPlaybackMode
//************************************************************************************************//
CMPXVideoPlaybackMode*
CMPXStreamingPlaybackMode::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
   MPX_ENTER_EXIT(_L("CMPXStreamingPlaybackMode::NewL()"));

   CMPXStreamingPlaybackMode* self = new( ELeave ) CMPXStreamingPlaybackMode();
   CleanupStack::PushL( self );
   self->ConstructL(aVideoPlaybackCtlr);
   CleanupStack::Pop();
   return self;
}

CMPXStreamingPlaybackMode::~CMPXStreamingPlaybackMode()
{
    MPX_DEBUG(_L("CMPXStreamingPlaybackMode::~CMPXStreamingPlaybackMode()"));
}

//  ----------------------------------------------------------------------------
//    CMPXStreamingPlaybackMode::OpenFileL()
//  ----------------------------------------------------------------------------
void CMPXStreamingPlaybackMode::OpenFileL( const TDesC& aMediaFile )
{
    MPX_ENTER_EXIT(_L("CMPXStreamingPlaybackMode::OpenFileL()"), _L("file = %S"), &aMediaFile );

    iVideoPlaybackCtlr->iPlayer->OpenUrlL( aMediaFile, iVideoPlaybackCtlr->iAccessPointId );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXStreamingPlaybackMode::HandleOpenComplete()
//  ------------------------------------------------------------------------------------------------
//
void CMPXStreamingPlaybackMode::HandleOpenComplete()
{
    MPX_ENTER_EXIT(_L("CMPXStreamingPlaybackMode::HandleOpenComplete()"));

    //
    //  There is no need to send the access point if it is set to use default.
    //  SDP files are opened as KMmfUidFileSource type, we need to set the access point for
    //  SDP files before Prepare is called on Helix for RAM files and URLs - access point is
    //  already been set at the point of adding data source
    //
    if ( iVideoPlaybackCtlr->iAccessPointId != KUseDefaultIap &&
         iVideoPlaybackCtlr->iMediaType == CMediaRecognizer::ELocalSdpFile )
    {
        const TMMFMessageDestinationPckg destinationPckg(KUidInterfaceMMFHelixController);
        const TPckgBuf<TBool> savePckg( EFalse );

        HBufC8* tempBuf = NULL;
        TInt apMaxLen = 3;

        HBufC8* accessPoint = HBufC8::NewLC( KMMFAccessPoint().Length() + apMaxLen );
        accessPoint->Des().Format( KMMFAccessPoint, iVideoPlaybackCtlr->iAccessPointId );

        tempBuf = HBufC8::NewLC( accessPoint->Length() );
        tempBuf->Des().Copy( accessPoint->Des() );

        if ( tempBuf )
        {
            iVideoPlaybackCtlr->iPlayer->CustomCommandSync( destinationPckg,
                                                            EMMFROPControllerSetApplicationConfig,
                                                            tempBuf->Des(),
                                                            savePckg );
        }

        CleanupStack::PopAndDestroy(2);   // accessPoint, tempBuf
    }
}

//  ------------------------------------------------------------------------------------------------
//  CMPXStreamingPlaybackMode::CanPlayNow
//  ------------------------------------------------------------------------------------------------
TBool CMPXStreamingPlaybackMode::CanPlayNow()
{
    MPX_DEBUG(_L("CMPXStreamingPlaybackMode::CanPlayNow"));
    TBool playAllowed = EFalse;

    if ( iVideoPlaybackCtlr->iAppInForeground && iVideoPlaybackCtlr->iAllowAutoPlay )
    {
        if ( iVideoPlaybackCtlr->IsVideoCall() )
        {
            MPX_TRAPD(err,
                      iVideoPlaybackCtlr->iState->SendErrorToViewL( KMPXVideoCallOngoingError ));
        }
        else if ( iVideoPlaybackCtlr->IsKeyLocked() && iVideoPlaybackCtlr->iFileDetails->iVideoEnabled )
        {
          //exit for live streaming
        }
        else
        {
            playAllowed = !( iVideoPlaybackCtlr->IsVoiceCall() && IsNetworkMode2GL() );

            if ( !playAllowed )
            {
                MPX_TRAPD( err,
                    iVideoPlaybackCtlr->iState->SendErrorToViewL(
                        KMPXVideoPlayOver2GDuringVoiceCallError ) );
            }
        }
    }

    return  playAllowed;
}

//  ------------------------------------------------------------------------------------------------
//    CMPXStreamingPlaybackMode::IsTvOutAllowedL()
//  ------------------------------------------------------------------------------------------------
TBool CMPXStreamingPlaybackMode::IsTvOutAllowedL()
{
    MPX_ENTER_EXIT(_L("CMPXStreamingPlaybackMode::IsTvOutAllowedL(1)"));
    return ETrue;
}
//  ------------------------------------------------------------------------------------------------
//    CMPXStreamingPlaybackMode::HandlePause()
//  ------------------------------------------------------------------------------------------------
void CMPXStreamingPlaybackMode::HandlePause()
{
    MPX_ENTER_EXIT(_L("CMPXStreamingPlaybackMode::HandlePause()"));

    if ( iVideoPlaybackCtlr->iFileDetails->iPausableStream )
    {
        MPX_TRAPD( err, iVideoPlaybackCtlr->iPlayer->PauseL() );

        if ( err == KErrNone )
        {
            iVideoPlaybackCtlr->ChangeState( EMPXVideoPaused );

            iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPPaused,
                                                                  0,
                                                                  err );
        }
        else
        {
            TRAP_IGNORE( iVideoPlaybackCtlr->iState->SendErrorToViewL( err ) );
        }
    }
    else
    {
        iVideoPlaybackCtlr->iPlayer->Stop();

        iVideoPlaybackCtlr->ChangeState( EMPXVideoStopped );

        iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPStopped,
                                                              0,
                                                              KErrNone );
    }
}

//************************************************************************************************//
//          CMPXLiveStreamingPlaybackMode
//************************************************************************************************//
CMPXVideoPlaybackMode*
CMPXLiveStreamingPlaybackMode::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
   MPX_ENTER_EXIT(_L("CMPXLiveStreamingPlaybackMode::NewL()"));

   CMPXLiveStreamingPlaybackMode* self =
            new( ELeave ) CMPXLiveStreamingPlaybackMode();
   CleanupStack::PushL( self );
   self->ConstructL(aVideoPlaybackCtlr);
   CleanupStack::Pop();
   return self;
}

CMPXLiveStreamingPlaybackMode::~CMPXLiveStreamingPlaybackMode()
{
    MPX_DEBUG(_L("CMPXLiveStreamingPlaybackMode::~CMPXLiveStreamingPlaybackMode"));
}

//  ------------------------------------------------------------------------------------------------
//    CMPXLiveStreamingPlaybackMode::HandlePause()
//  ------------------------------------------------------------------------------------------------
void CMPXLiveStreamingPlaybackMode::HandlePause()
{
    MPX_ENTER_EXIT(_L("CMPXLiveStreamingPlaybackMode::HandlePause()"));

    iVideoPlaybackCtlr->iPlayer->Stop();

    iVideoPlaybackCtlr->ChangeState( EMPXVideoStopped );

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPStopped,
                                                          0,
                                                          KErrNone );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXLiveStreamingPlaybackMode::HandleBackground()
//  ------------------------------------------------------------------------------------------------
void CMPXLiveStreamingPlaybackMode::HandleBackground()
{
    MPX_DEBUG(_L("CMPXLiveStreamingPlaybackMode::HandleBackground()"));

    if ( iVideoPlaybackCtlr->iAppInForeground )
    {
        if ( iVideoPlaybackCtlr->IsPhoneCall() ||
             iVideoPlaybackCtlr->IsVideoCall() ||
             ( iVideoPlaybackCtlr->IsKeyLocked() && iVideoPlaybackCtlr->iFileDetails->iVideoEnabled ))
        {
            iVideoPlaybackCtlr->iState->HandlePause();
        }
    }
    else
    {
        iVideoPlaybackCtlr->iState->HandlePause();
    }
}

//************************************************************************************************//
//          CMPXProgressiveDLPlaybackMode
//************************************************************************************************//
CMPXVideoPlaybackMode*
CMPXProgressiveDLPlaybackMode::NewL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
   MPX_ENTER_EXIT(_L("CMPXProgressiveDLPlaybackMode::NewL()"));

   CMPXProgressiveDLPlaybackMode* self = new( ELeave ) CMPXProgressiveDLPlaybackMode();
   CleanupStack::PushL( self );
   self->ConstructL( aVideoPlaybackCtlr );
   CleanupStack::Pop();
   return self;
}

void CMPXProgressiveDLPlaybackMode::ConstructL( CMPXVideoPlaybackController* aVideoPlaybackCtlr )
{
    iVideoPlaybackCtlr = aVideoPlaybackCtlr;

    //
    //  Create the Download Mgr Interface
    //
    iDlMgrIf = CMPXVideoDlMgrIf::NewL( iVideoPlaybackCtlr );
}

CMPXProgressiveDLPlaybackMode::~CMPXProgressiveDLPlaybackMode()
{
    MPX_DEBUG(_L("CMPXProgressiveDLPlaybackMode::~CMPXProgressiveDLPlaybackMode()"));

    if ( iDlMgrIf )
    {
        delete iDlMgrIf;
        iDlMgrIf = NULL;
    }
}

//  ------------------------------------------------------------------------------------------------
//    CMPXProgressiveDLPlaybackMode::ConnectToDownloadL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXProgressiveDLPlaybackMode::ConnectToDownloadL( CMPXCommand& aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXProgressiveDLPlaybackMode::ConnectToDownloadL()"));

    iDlMgrIf->ConnectToDownloadL( aCmd );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXProgressiveDLPlaybackMode::HandleOpenComplete()
//  ------------------------------------------------------------------------------------------------
//
void CMPXProgressiveDLPlaybackMode::HandleOpenComplete()
{
    MPX_ENTER_EXIT(_L("CMPXProgressiveDLPlaybackMode::HandleOpenComplete()"));

    //
    //  Helix needs the download id before the prepare command has been sent
    //  to enable PDL
    //
    iDlMgrIf->DoHandleOpenComplete();
}

//  ------------------------------------------------------------------------------------------------
//    CMPXProgressiveDLPlaybackMode::GetPdlStatusL()
//  ------------------------------------------------------------------------------------------------
//
void CMPXProgressiveDLPlaybackMode::GetPdlStatusL( CMPXCommand& aCmd )
{
    MPX_DEBUG(_L("CMPXProgressiveDLPlaybackMode::GetPdlStatusL()"));

    iDlMgrIf->GetPdlStatusL( aCmd );
}

//  ------------------------------------------------------------------------------------------------
//    CMPXProgressiveDLPlaybackMode::UpdateSeekPosition()
//  ------------------------------------------------------------------------------------------------
//
void CMPXProgressiveDLPlaybackMode::UpdateSeekPosition( TInt64& aPosition )
{
    MPX_DEBUG(_L("CMPXProgressiveDLPlaybackMode::UpdateSeekPosition()"));

    TReal duration = (TReal)iVideoPlaybackCtlr->iFileDetails->iDuration.Int64();
    TReal playbackRatio = (TReal)aPosition / duration;
    TReal downloadRatio = iDlMgrIf->GetDownloadRatio();

    //
    //  Limit the seek position to the downloaded part of the file
    //
    if ( playbackRatio > downloadRatio )
    {
        aPosition = downloadRatio * duration;
    }

    iVideoPlaybackCtlr->iMPXPluginObs->HandlePluginEvent(
        MMPXPlaybackPluginObserver::EPPositionChanged,
        aPosition / KPbMilliMultiplier,
        KErrNone );
}

// End of file
