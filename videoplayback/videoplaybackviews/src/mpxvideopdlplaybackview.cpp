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
* Description:  Implementation of Video playback view
*
*/

// Version : %version: 21 %


//  Include Files
#include <aknViewAppUi.h>
#include <mpxplaybackutility.h>
#include <mpxviewutility.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxvideoplaybackviews.rsg>
#include <mmf/common/mmferrors.h>
#include <eikmenup.h>
#include <AknTaskList.h>

#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackuids.hrh"
#include "mpxvideopdlplaybackview.h"
#include "mpxvideoplaybackcontainer.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideo_debug.h"

//  Member Functions

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::NewLC()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMPXVideoPdlPlaybackView* CMPXVideoPdlPlaybackView::NewLC()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPdlPlaybackView::NewLC()"));

    CMPXVideoPdlPlaybackView* self = new (ELeave) CMPXVideoPdlPlaybackView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::NewL()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMPXVideoPdlPlaybackView* CMPXVideoPdlPlaybackView::NewL()
{
    CMPXVideoPdlPlaybackView* self = CMPXVideoPdlPlaybackView::NewLC();
    CleanupStack::Pop( self );
    return self;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::CMPXVideoPdlPlaybackView()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPdlPlaybackView::CMPXVideoPdlPlaybackView()
// note, CBase initialises all member variables to zero
{
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::ConstructL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPdlPlaybackView::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPdlPlaybackView::ConstructL()"));

    InitializeVideoPlaybackViewL();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::~CMPXVideoPdlPlaybackView()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPdlPlaybackView::~CMPXVideoPdlPlaybackView()
{
    MPX_DEBUG(_L("CMPXVideoPdlPlaybackView::~CMPXVideoPdlPlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::Id()
// -------------------------------------------------------------------------------------------------
//
TUid CMPXVideoPdlPlaybackView::Id() const
{
    return TUid::Uid( KMPXVIDEOPDLPLAYBACKVIEWUID );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::ViewImplementationIdId()
// -------------------------------------------------------------------------------------------------
//
TUid CMPXVideoPdlPlaybackView::ViewImplementationId() const
{
    return TUid::Uid( KMPXVIDEOPDLPLAYBACKVIEWPLUGINIMPLEMENTATIONUID );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::CloseEmbeddedPlaybackViewL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPdlPlaybackView::CloseEmbeddedPlaybackViewL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPdlPlaybackView::CloseEmbeddedPlaybackViewL()"));

    //
    //  If playback state is playing, pause playback
    //  If playback state is stopped, close the plugin so file will be moved
    //
    if ( iPlaybackState == EPbStatePlaying )
    {
        HandleCommandL( EMPXPbvCmdPause );
    }
    else if ( iPlaybackState == EPbStateStopped )
    {
        //
        //  Close the playback plugin and clear out the PDL data from the PDL Handler
        //
        HandleCommandL( EMPXPbvCmdClose );
        AppUi()->HandleCommandL( EAknCmdHideInBackground );
    }

    //
    //  Embedded PDL
    //  If we are in the foreground, bring the browser to the foreground
    //
    if ( ! BringUpBrowserL() )
    {
        //
        //  Browser cannot be found, exit application
        //
        AppUi()->RunAppShutter();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::BringUpBrowserL
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPdlPlaybackView::BringUpBrowserL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPdlPlaybackView::BringUpBrowserL()"));

    TBool browserFound( EFalse );

    RWsSession& wsSession=iEikonEnv->WsSession();
    CAknTaskList* taskList = CAknTaskList::NewL( wsSession );
    TApaTask task = taskList->FindRootApp( TUid::Uid( KBROWSERUID ) );

    delete taskList;

    if ( task.Exists() )
    {
        task.BringToForeground();
        browserFound = ETrue;
    }

    return browserFound;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::RetrievePdlInformationL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPdlPlaybackView::RetrievePdlInformationL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPdlPlaybackView::RetrievePdlInformationL()"));

    CMPXCommand* cmd( CMPXCommand::NewL() );
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
    cmd->SetTObjectValueL<TMPXPlaybackPdCommand>( KMPXCommandPlaybackGeneralType,
                                                  EPbCmdGetPdStatus );

    iPlaybackUtility->CommandL( *cmd );

    //
    //  Parse the PDL information
    //
    iPdlState = cmd->ValueTObjectL<TMPXPlaybackPdDownloadState>( KMPXCommandPlaybackPDState );
    TInt downloadedBytes = cmd->ValueTObjectL<TInt>( KMPXCommandPlaybackPDDownloadedBytes );
    iDownloadSize = cmd->ValueTObjectL<TInt>( KMPXCommandPlaybackPDTotalBytes );

    MPX_DEBUG(_L("    PDL State = %d, current size = %d, total size = %d"),
        iPdlState, downloadedBytes, iDownloadSize );

    CleanupStack::PopAndDestroy( cmd );

    if ( iContainer )
    {
        //
        //  Check for a saved download.
        //  The download size and downloaded bytes will be set to 100 so
        //  the download progress bar will show 100%
        //
        if ( iPdlState == EPbDlStateNotDownloading )
        {
            iDownloadSize   = 100;
            downloadedBytes = 100;
        }

        iContainer->HandleEventL( EMPXControlCmdSetDownloadSize, iDownloadSize );
        iContainer->HandleEventL( EMPXControlCmdDownloadUpdated, downloadedBytes );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::HandlePluginErrorL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPdlPlaybackView::HandlePluginErrorL( TInt aError )
{
    MPX_DEBUG(
      _L("CMPXVideoPdlPlaybackView::HandlePluginErrorL() aError = %d"), aError );

    switch ( aError )
    {
        case KErrAbort:
        {
            //
            //  For progressive download mode, this indicates that the file
            //  could not be played until the entire file is downloaded.
            //
            DisplayErrorMessageL( R_MPX_VIDEO_PDL_WAIT_DL_COMPLETE_MSG );
            ClosePlaybackViewL();

            break;
        }
        case KErrEof:
        {
            //
            //  For progressive download mode, this indicates that the file
            //  has timed out waiting for more data to be downloaded
            //
            DisplayErrorMessageL( R_MPX_VIDEO_PDL_ALL_CONTENT_PLAYED_MSG );
            ClosePlaybackViewL();

            break;
        }
        case KErrNotSupported:
        case KErrUnknown:
        case KErrMMDecoder:
        case KErrCorrupt:
        case KErrTooBig:
        {
            //
            //  We have received an error for an invalid clip.
            //  If the download is complete, display the invalid
            //  clip message.  If the download is in progress,
            //  we do not know if the clip is invalid yet.
            //  Show message to wait for more data.
            //
            if ( iPdlState >= EPbDlStateNotDownloading )
            {
                DisplayErrorMessageL( R_MPX_PDL_MORE_DATA_NEEDED_MSG );
            }
            else
            {
                DisplayErrorMessageL( R_MPX_VIDEO_INVALID_CLIP );
            }

            ClosePlaybackViewL();

            break;
        }
        default:
        {
            CMPXVideoBasePlaybackView::HandlePluginErrorL( aError );
        }
    }
}

// -----------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::HandlePdlStateChangeL
// -----------------------------------------------------------------------------
//
void CMPXVideoPdlPlaybackView::HandlePdlStateChangeL( TInt aState )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPdlPlaybackView::HandlePdlStateChangeL()"),
                   _L("aState = %d"), aState );

    TMPXPlaybackPdDownloadState prevPdlState = iPdlState;
    iPdlState = (TMPXPlaybackPdDownloadState)aState;

    switch ( aState )
    {
        case EPbDlStateDownloadError:
        case EPbDlStateDownloadCanceled:
        {
            HandleCommandL( EMPXPbvCmdStop );
            ClosePlaybackViewL();
            break;
        }
        case EPbDlStateDownloading:
        {
            if ( prevPdlState == EPbDlStateDownloadPaused )
            {
                if ( iContainer )
                {
                    iContainer->HandleEventL( EMPXControlCmdClearDownloadPaused );
                }
            }

            break;
        }
        case EPbDlStateDownloadPaused:
        {
            //
            //  If we are buffering, display information message and pause
            //
            if ( iPlaybackState == EPbStateBuffering  )
            {
                HandleCommandL( EMPXPbvCmdPause );

                DisplayInfoMessageL( R_MPX_VIDEO_PDL_ALL_CONTENT_PLAYED_MSG, ETrue );
                HandleCommandL( EAknSoftkeyBack );
            }
            else
            {
                //
                //  Download has been paused, let container show
                //  the download paused icon
                //
                if ( iContainer )
                {
                    iContainer->HandleEventL( EMPXControlCmdSetDownloadPaused );
                }
            }

            break;
        }
        case EPbDlStateDownloadCompleted:
        {
            if ( iContainer )
            {
                iContainer->HandleEventL( EMPXControlCmdDownloadComplete, iDownloadSize );
            }

            break;
        }
        case EPbDlStateNotDownloading:
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::HandleStoppedStateL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPdlPlaybackView::HandleStoppedStateL()
{
    MPX_DEBUG(_L("CMPXVideoPdlPlaybackView::HandleStoppedStateL()"));

    if ( iPlaybackState == EPbStateStopped )
    {
        ClosePlaybackViewL();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::HandleBufferingStateL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPdlPlaybackView::HandleBufferingStateL()
{
    MPX_DEBUG(_L("CMPXVideoPdlPlaybackView::HandleBufferingStateL()"));

    //
    //  If the download is paused and the state is buffering,
    //  display information message
    //
    if ( iPdlState == EPbDlStateDownloadPaused  )
    {
        HandleCommandL( EMPXPbvCmdPause );

        DisplayInfoMessageL( R_MPX_VIDEO_PDL_ALL_CONTENT_PLAYED_MSG, ETrue );
        HandleCommandL( EAknSoftkeyBack );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPdlPlaybackView::SendPdlCustomCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPdlPlaybackView::SendPdlCustomCommandL( TMPXPlaybackPdCommand aCustomCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPdlPlaybackView::SendPdlCustomCommandL"),
                   _L("aCustomCmd = %d"), aCustomCmd );

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
    cmd->SetTObjectValueL<TMPXPlaybackPdCommand>( KMPXCommandPlaybackGeneralType, aCustomCmd );

    iPlaybackUtility->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
}

// EOF
