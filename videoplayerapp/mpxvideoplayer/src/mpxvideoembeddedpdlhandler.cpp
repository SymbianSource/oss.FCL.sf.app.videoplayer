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
* Description:  Handles PDL commands passed in by other appilcations
*
*/


// Version : %version:  ou1cpsw#15.1.1 %


#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxplaybackutility.h>
#include <mpxmediageneraldefs.h>

#include <mpxviewutility.h>
#include <mpxviewpluginmanager.h>
#include <mpxviewplugin.h>

#include "mpxvideoembeddedpdlhandler.h"
#include "mpxvideoplayerappuiengine.h"
#include "mpxvideoplayerconstants.h"
#include "mpxvideoplayercustomviewmsgconsts.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideo_debug.h"

CMpxVideoEmbeddedPdlHandler*
CMpxVideoEmbeddedPdlHandler::NewL( CMpxVideoPlayerAppUiEngine* aAppUiEngine )
{
    MPX_ENTER_EXIT(_L("CMpxVideoEmbeddedPdlHandler::NewL()"));

    CMpxVideoEmbeddedPdlHandler* self = new ( ELeave ) CMpxVideoEmbeddedPdlHandler( aAppUiEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

// -------------------------------------------------------------------------------------------------
//   C++ constructor can NOT contain any code that might leave.
// -------------------------------------------------------------------------------------------------
//
CMpxVideoEmbeddedPdlHandler::CMpxVideoEmbeddedPdlHandler( CMpxVideoPlayerAppUiEngine* aAppUiEngine )
    : iAppUiEngine( aAppUiEngine )
{
}

// -------------------------------------------------------------------------------------------------
//   Destructor.
// -------------------------------------------------------------------------------------------------
//
CMpxVideoEmbeddedPdlHandler::~CMpxVideoEmbeddedPdlHandler()
{
    MPX_ENTER_EXIT(_L("CMpxVideoEmbeddedPdlHandler::~CMpxVideoEmbeddedPdlHandler()"));

    if ( iDownloadFileName )
    {
        delete iDownloadFileName;
        iDownloadFileName = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
//   Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoEmbeddedPdlHandler::ConstructL()
{
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoEmbeddedPdlHandler::ConnectToEmbeddedDownloadL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoEmbeddedPdlHandler::ConnectToEmbeddedDownloadL( TInt aDlId, TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoEmbeddedPdlHandler::ConnectToEmbeddedDownloadL()"),
                   _L("aDlId = %d, aFileName = %S"), aDlId, &aFileName );

    iEmbeddedPdlCase = ETrue;

    if ( iDownloadFileName )
    {
        //
        //  Check if this is the same download that is loaded
        //  If it is, issue a play command to resume playback

        if ( ( aDlId == iDownloadId ) && ( ! aFileName.Compare( *iDownloadFileName ) ) )
        {        
            iAppUiEngine->SendMpxPlaybackCmdL( EPbCmdPlay );            
            iAppUiEngine->SendMessageToPdlViewL( KMpxVideoPlaybackPdlReloadComplete );
        }
        else
        {
            //
            //  New download received, close old playback plugin
            //
            iAppUiEngine->ClosePlaybackPluginL();
            iAppUiEngine->SendMessageToPdlViewL( KMpxVideoPlaybackPdlReloading );
            StartNewDownloadL( aDlId, aFileName );
        }
    }
    else
    {
        StartNewDownloadL( aDlId, aFileName );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoEmbeddedPdlHandler::ConnectToCollectionDownloadL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoEmbeddedPdlHandler::ConnectToCollectionDownloadL( TInt aDlId, TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoEmbeddedPdlHandler::ConnectToCollectionDownloadL()"),
                   _L("aDlId = %d, aFileName = %S"), aDlId, &aFileName );

    iEmbeddedPdlCase = EFalse;

    //
    //  The playback plugin will not be loaded for collection downloads
    //
    StartNewDownloadL( aDlId, aFileName );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoEmbeddedPdlHandler::StartNewDownloadL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoEmbeddedPdlHandler::StartNewDownloadL( TInt aDlId, TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoEmbeddedPdlHandler::StartNewDownloadL()"),
                   _L("aDlId = %d, aFileName = %S"), aDlId, &aFileName );

    //
    //  Save the download parameters
    //
    iDownloadId = aDlId;
    delete iDownloadFileName;
    iDownloadFileName = NULL;
    iDownloadFileName = aFileName.AllocL();

    //
    //  Load the correct playback view
    //
    iAppUiEngine->PreLoadPdlPlaybackViewL();

    SendPdlCustomCommandL( EPbCmdStartPd, iDownloadId );

    iAppUiEngine->InitializeFileL( *iDownloadFileName );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoEmbeddedPdlHandler::SendPdlCustomCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoEmbeddedPdlHandler::SendPdlCustomCommandL( TMPXPlaybackPdCommand aCustomCmd,
                                                         TInt aData )
{
    MPX_ENTER_EXIT(_L("CMpxVideoEmbeddedPdlHandler::SendPdlCustomCommandL"),
                   _L("aCustomCmd = %d, aData = %d"), aCustomCmd, aData );

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackPD );
    cmd->SetTObjectValueL<TMPXPlaybackPdCommand>( KMPXCommandPlaybackGeneralType, aCustomCmd );
    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackPDTransactionID, aData );
    cmd->SetTextValueL( KMPXMediaVideoPlaybackFileName, *iDownloadFileName );
    cmd->SetTObjectValueL<TInt>( KMPXMediaVideoMovePdlFile, iEmbeddedPdlCase );
    
    TRAPD( err, iAppUiEngine->SendCustomMpxPlaybackUtilityCmdL( *cmd ) );

    CleanupStack::PopAndDestroy( cmd );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoEmbeddedPdlHandler::ClearPdlInformation()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoEmbeddedPdlHandler::ClearPdlInformation()
{
    MPX_ENTER_EXIT(_L("CMpxVideoEmbeddedPdlHandler::ClearPdlInformation"));

    if ( iDownloadFileName )
    {
        delete iDownloadFileName;
        iDownloadFileName = NULL;
    }

    iDownloadId = KErrNotFound;
}

// EOF
