/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   CVcxHgMyVideosDownloadClient implementation*
*/




// INCLUDE FILES
#include <bldvariant.hrh>
#include <mpxcollectionutility.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <vcxmyvideosuids.h>
#include <mpxmediageneraldefs.h>
#include <vcxmyvideosdefs.h>
#include <vcxhgmyvideos.rsg>
#include <StringLoader.h>
#include <aknnotewrappers.h> 

#include "IptvDebug.h"
#include "vcxhgmyvideosdownloadclient.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadClient::CVcxHgMyVideosDownloadClient()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosDownloadClient::CVcxHgMyVideosDownloadClient( 
        CVcxHgMyVideosCollectionClient& aCollectionClient,
        MMPXCollectionUtility& aCollectionUtility )
  : iCollectionClient( aCollectionClient ),
    iCollectionUtility( aCollectionUtility ),
    iPendingCommand( EVcxHgMyVideosDownloadCommandNone )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadClient::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosDownloadClient* CVcxHgMyVideosDownloadClient::NewL(
        CVcxHgMyVideosCollectionClient& aCollectionClient,
        MMPXCollectionUtility& aCollectionUtility )
    {
    CVcxHgMyVideosDownloadClient* self = 
    CVcxHgMyVideosDownloadClient::NewLC( aCollectionClient, aCollectionUtility );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadClient::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosDownloadClient* CVcxHgMyVideosDownloadClient::NewLC( 
        CVcxHgMyVideosCollectionClient& aCollectionClient,
        MMPXCollectionUtility& aCollectionUtility )
    {
    CVcxHgMyVideosDownloadClient* self = 
        new (ELeave) CVcxHgMyVideosDownloadClient( aCollectionClient, aCollectionUtility );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadClient::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadClient::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadClient::~CVcxHgMyVideosDownloadClient()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosDownloadClient::~CVcxHgMyVideosDownloadClient()
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadClient::HasPendingCommand()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosDownloadClient::HasPendingCommand()
    {
    return iPendingCommand != EVcxHgMyVideosDownloadCommandNone;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadClient::ResumeDownloadL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadClient::ResumeDownloadL( const CMPXMedia& aMpxMedia )
    {
    if ( ! HasPendingCommand() && ! iCollectionClient.HasPendingCommand() )
        {    
        CMPXCommand* cmd = CMPXCommand::NewL();
        CleanupStack::PushL( cmd );
    
        CMPXMedia* media = CMPXMedia::NewL( aMpxMedia );
        CleanupStack::PushL( media );
        media->SetTObjectValueL( KVcxMediaMyVideosIapId, 0 );

        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosStartDownload );
        cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse ); // asynch
        cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId,
                               TUid::Uid( KVcxUidMyVideosMpxCollection ) );
        cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, media );
    
        iCollectionUtility.Collection().CommandL( *cmd );
        iPendingCommand = EVcxHgMyVideosDownloadCommandResume;
        
        CleanupStack::PopAndDestroy( media );
        CleanupStack::PopAndDestroy( cmd );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadClient::CancelDownloadL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadClient::CancelDownloadL( const CMPXMedia& aMpxMedia )
    {
    if ( ! HasPendingCommand() && ! iCollectionClient.HasPendingCommand() )
        {
        CMPXCommand* cmd = CMPXCommand::NewL();
        CleanupStack::PushL( cmd );

        CMPXMedia* cancelDownloadReq = CMPXMedia::NewL( aMpxMedia );
        CleanupStack::PushL( cancelDownloadReq );

        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosCancelDownload ); 
        cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse ); // asynch
        cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, 
                               TUid::Uid( KVcxUidMyVideosMpxCollection ) );
        cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, cancelDownloadReq );    

        iCollectionUtility.Collection().CommandL( *cmd );
        iPendingCommand = EVcxHgMyVideosDownloadCommandCancel;

        CleanupStack::PopAndDestroy( cancelDownloadReq );
        CleanupStack::PopAndDestroy( cmd );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosDownloadClient::CommandCompleteL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosDownloadClient::CommandCompleteL( TInt aError )
    {
    if ( iPendingCommand == EVcxHgMyVideosDownloadCommandCancel && aError == KErrNone )
    	{
    	// Display cancel confirmation note. 
        HBufC* text = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DOWNLOAD_CANCELED );
        CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );  
    	}
    else
    	{
    	// Is there need to handle failed command?
    	}
    iPendingCommand = EVcxHgMyVideosDownloadCommandNone;
    }
