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
* Description:      Client class for My Videos MPX Collection.*
*/




// INCLUDE FILES
#include <bldvariant.hrh>
#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessage2.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <vcxmyvideosdefs.h>
#include <vcxhgmyvideos.rsg>
#include <vcxmyvideosuids.h>
#include "IptvDebug.h"
#include "vcxhgmyvideoscollectionclient.h"
#include "vcxhgmyvideosdownloadclient.h"
#include "vcxhgmyvideoscategorymodelobserver.h"
#include "vcxhgmyvideosvideomodelobserver.h"


// CONSTANTS
const TInt KVcxHgEventLevelRoot     = 1;
const TInt KVcxHgEventLevelCategory = 2;
const TInt KVcxHgEventLevelVideo    = 3;

const TInt KVcxMpxLevelCategories = 2;
const TInt KVcxMpxLevelVideos     = 3;

const TUint32 KVcxHgMyVideosTransactionId = 5;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::CVcxHgMyVideosCollectionClient()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCollectionClient::CVcxHgMyVideosCollectionClient()
  : iCollectionOpenStatus( EVcxHgCollectionNotOpen ),
    iPendingCommand( EVcxHgMyVideosCollectionCommandNone ),
    iCollectionLevel( KErrNotFound ) 
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCollectionClient* CVcxHgMyVideosCollectionClient::NewL()
    {
    CVcxHgMyVideosCollectionClient* self = 
    CVcxHgMyVideosCollectionClient::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCollectionClient* CVcxHgMyVideosCollectionClient::NewLC()
    {
    CVcxHgMyVideosCollectionClient* self = 
        new (ELeave) CVcxHgMyVideosCollectionClient();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::ConstructL()
    {
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );
    iDownloadClient = CVcxHgMyVideosDownloadClient::NewL( *this, *iCollectionUtility );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::~CVcxHgMyVideosCollectionClient()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCollectionClient::~CVcxHgMyVideosCollectionClient()
    {
    delete iDownloadClient;
    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::DownloadClient()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosDownloadClient& CVcxHgMyVideosCollectionClient::DownloadClient()
    {
    return *iDownloadClient;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HasPendingCommand()
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosCollectionClient::HasPendingCommand()
    {
    return iPendingCommand != EVcxHgMyVideosCollectionCommandNone;
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::SetCategoryModelObserver()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::SetCategoryModelObserver(
        MVcxHgMyVideosCategoryModelObserver* aCategoryModelObserver )
    {
    iCategoryModelObserver = aCategoryModelObserver;
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::SetVideoModelObserver()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::SetVideoModelObserver(
        MVcxHgMyVideosVideoModelObserver* aVideoModelObserver )
    {
    iVideoModelObserver = aVideoModelObserver;
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::CollectionLevelL()
// ---------------------------------------------------------------------------
//
TInt CVcxHgMyVideosCollectionClient::CollectionLevelL()
    {
    CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( path );
    TInt collectionLevel = path->Levels();
    CleanupStack::PopAndDestroy( path );

    return collectionLevel;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::GetCategoryListL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::GetCategoryListL()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # GetCategoryListL()" );

    if ( iCollectionOpenStatus == EVcxHgCollectionNotOpen )
        {
        CMPXCollectionPath* collectionPath = CMPXCollectionPath::NewL();
        CleanupStack::PushL( collectionPath );     
        collectionPath->AppendL( KVcxUidMyVideosMpxCollection );
        iCollectionUtility->Collection().OpenL( *collectionPath );
        CleanupStack::PopAndDestroy( collectionPath );  

        iCollectionOpenStatus = EVcxHgCollectionOpening;
        }
    else
        {
        if ( iCollectionLevel == KErrNotFound )
            {
            iCollectionLevel = CollectionLevelL();
            }
        if ( iCollectionLevel == KVcxMpxLevelVideos )
            {
            iCollectionUtility->Collection().BackL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::GetVideoListL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::GetVideoListL( TInt aIndex )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # GetVideoListL(%d)", aIndex );

    if ( iCollectionLevel == KErrNotFound )
        {
        iCollectionLevel = CollectionLevelL();
        }
    if ( iCollectionLevel == KVcxMpxLevelCategories )
        {    
        iCollectionUtility->Collection().OpenL( aIndex );
        }
    else if ( iCollectionLevel == KVcxMpxLevelVideos )
        {
        iCollectionUtility->Collection().BackL();
        iCollectionUtility->Collection().OpenL( aIndex );
        }
    else
        {
        // NOP
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::ArrayIndexToMpxItemIdL()
// -----------------------------------------------------------------------------
//
TMPXItemId CVcxHgMyVideosCollectionClient::ArrayIndexToMpxItemIdL( TInt aArrayIndex,
                                                                   CMPXMediaArray* aArray )
    {
    if ( aArrayIndex < 0 || aArrayIndex >= aArray->Count() )
        {
        User::Leave( KErrArgument );
        }

    return (*aArray)[aArrayIndex]->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::DeleteVideosL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::DeleteVideosL( CMPXMediaArray* aMediasToDelete )
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosCollectionClient::DeleteVideosL()" );

    if ( ! HasPendingCommand() && ! iDownloadClient->HasPendingCommand() )
        {
        CMPXCommand* cmd = CMPXMedia::NewL();
        CleanupStack::PushL( cmd );

        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosDelete );    
        cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse );
        cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, 
                               TUid::Uid( KVcxUidMyVideosMpxCollection ) );
        cmd->SetCObjectValueL( KMPXMediaArrayContents, aMediasToDelete );

        iCollectionUtility->Collection().CommandL( *cmd );

        iPendingCommand = EVcxHgMyVideosCollectionCommandMultiDelete_Starting;
        CleanupStack::PopAndDestroy( cmd );    
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # CVcxHgMyVideosCollectionClient::DeleteVideosL() LEAVE: KErrNotReady" );
        User::Leave( KErrNotReady );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::CancelDeleteVideosL()
// -----------------------------------------------------------------------------
//    
void CVcxHgMyVideosCollectionClient::CancelDeleteVideosL()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosCollectionClient::CancelDeleteVideosL()" );

    // Deletion can be cancelled right away if deletion has already started. If we
    // are only waiting for it, we modify the pending command to indicate cancellation.
    if ( iPendingCommand == EVcxHgMyVideosCollectionCommandMultiDelete_Started ||
         iPendingCommand == EVcxHgMyVideosCollectionCommandMultiDelete_Finished )
        {
        CMPXCommand* cmd = CMPXMedia::NewL();
        CleanupStack::PushL( cmd );
        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosCancelDelete );
        cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, ETrue );
        cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, 
                               TUid::Uid( KVcxUidMyVideosMpxCollection ) );
        iCollectionUtility->Collection().CommandL( *cmd );
        CleanupStack::PopAndDestroy( cmd );

        iPendingCommand = EVcxHgMyVideosCollectionCommandCancelDelete_Completed;
        }
    else if ( iPendingCommand == EVcxHgMyVideosCollectionCommandMultiDelete_Starting )
        {
        iPendingCommand = EVcxHgMyVideosCollectionCommandCancelDelete_Requested;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::MoveOrCopyVideosL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::MoveOrCopyVideosL( CMPXMediaArray* mediasToMoveOrCopy,
                                                        TInt aTargetDrive,
                                                        TBool aCopy )
    {
    CMPXCommand* cmd = CMPXMedia::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    if ( aCopy )
        {
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosCopy );
        }
    else
        {
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosMove );
        }
    cmd->SetTObjectValueL( KVcxMediaMyVideosInt32Value, aTargetDrive );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, 
                           TUid::Uid( KVcxUidMyVideosMpxCollection ) );
    cmd->SetCObjectValueL( KMPXMediaArrayContents, mediasToMoveOrCopy );

    iCollectionUtility->Collection().CommandL(*cmd);
    iPendingCommand = EVcxHgMyVideosCollectionCommandMoveCopy_Starting;
    CleanupStack::PopAndDestroy( cmd );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::CancelMoveOrCopyVideosL()
// -----------------------------------------------------------------------------
//    
void CVcxHgMyVideosCollectionClient::CancelMoveOrCopyVideosL()
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # CVcxHgMyVideosCollectionClient::CancelMoveOrCopyVideosL()" );

    if ( iPendingCommand == EVcxHgMyVideosCollectionCommandMoveCopy_Started ||
         iPendingCommand == EVcxHgMyVideosCollectionCommandMoveCopy_Finished )
        {
        CMPXCommand* cmd = CMPXMedia::NewL();
        CleanupStack::PushL( cmd );
        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosCancelMoveOrCopy );
        cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, ETrue );
        cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, 
                               TUid::Uid( KVcxUidMyVideosMpxCollection ) );
        iCollectionUtility->Collection().CommandL( *cmd );
        CleanupStack::PopAndDestroy( cmd );

        iPendingCommand = EVcxHgMyVideosCollectionCommandCancelMoveCopy_Completed;
        }
    else if ( iPendingCommand == EVcxHgMyVideosCollectionCommandMoveCopy_Starting )
        {
        iPendingCommand = EVcxHgMyVideosCollectionCommandCancelMoveCopy_Requested;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::PlayVideoL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::PlayVideoL( TMPXItemId aMpxItemId )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
                "CVcxHgMyVideosCollectionClient::PlayVideoL: Play video at collection index %d", aMpxItemId.iId1 );
    
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    
    path->AppendL( KVcxUidMyVideosMpxCollection );
    path->AppendL( KVcxMvcCategoryIdAll );
    path->AppendL( aMpxItemId );
    path->SelectL( aMpxItemId );
    
    iCollectionUtility->Collection().OpenL( *path );
    CleanupStack::PopAndDestroy(path);
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::FetchMpxMediaByMpxIdL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::FetchMpxMediaByMpxIdL( TMPXItemId& aMpxId )
    {
    if ( ! iDownloadClient->HasPendingCommand() )
        {    
        CMPXCommand* cmd = CMPXCommand::NewL();
        CleanupStack::PushL( cmd );
    
        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosGetMediasByMpxId );
        cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId,
                               TUid::Uid( KVcxUidMyVideosMpxCollection ) );
        cmd->SetTObjectValueL( KVcxMediaMyVideosTransactionId, KVcxHgMyVideosTransactionId );        
        CMPXMediaArray* idMediaArray = CMPXMediaArray::NewL();
        CleanupStack::PushL( idMediaArray );
        CMPXMedia* media = CMPXMedia::NewL();
        CleanupStack::PushL( media );
        media->SetTObjectValueL( KMPXMessageMediaGeneralId, aMpxId );
        idMediaArray->AppendL( *media );
        CleanupStack::PopAndDestroy( media );
        cmd->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, idMediaArray );
        cmd->SetTObjectValueL( KMPXMediaArrayCount, idMediaArray->Count() );

        iCollectionUtility->Collection().CommandL( *cmd );

        CleanupStack::PopAndDestroy( idMediaArray );
        CleanupStack::PopAndDestroy( cmd );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::GetVideoDetailsL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::GetVideoDetailsL( TMPXItemId& aMpxId )
    {
    if ( ! HasPendingCommand() && ! iDownloadClient->HasPendingCommand() )
        {
        CMPXCommand* cmd = CMPXCommand::NewL();
        CleanupStack::PushL( cmd );
        
        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosGetMediaFullDetailsByMpxId );
        cmd->SetTObjectValueL( KMPXMediaGeneralId, aMpxId );
        
        iCollectionUtility->Collection().CommandL( *cmd );
        iPendingCommand = EVcxHgMyVideosCollectionCommandVideoDetails_Requested;
        CleanupStack::PopAndDestroy( cmd );
        }
	}

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleCollectionMediaL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleCollectionMediaL( 
        const CMPXMedia& /*aMedia*/,
        TInt /*aError*/ )
    {
    // No implemetation required.
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleCollectionMessage()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleCollectionMessage( 
        CMPXMessage* aMessage,
        TInt aError )
    {
    TRAP_IGNORE( HandleCollectionMessageL( aMessage, aError ) );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleCollectionMessageL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleCollectionMessageL( 
        CMPXMessage* aMessage,
        TInt aError )
    {
    TInt myVideosCmd( KErrNotFound ); 
    if ( aMessage->ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId ) == KVcxCommandIdMyVideos )
        {
        myVideosCmd = aMessage->ValueTObjectL<TInt>( KVcxMediaMyVideosCommandId );
        }

    if ( myVideosCmd == KVcxMessageMyVideosMessageArray )
        {
        if ( aMessage->IsSupported( KMPXMessageArrayContents ) )
            {
            const CMPXMessageArray* messageArray =
                aMessage->Value<CMPXMessageArray>(KMPXMessageArrayContents);
            for ( TInt i = 0; i < messageArray->Count(); i++ )
                {
                HandleSingleCollectionMessageL( (*messageArray)[i], KErrNone );
                }    
            }
        }
    else
        {
        HandleSingleCollectionMessageL( aMessage, aError );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleSingleCollectionMessageL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleSingleCollectionMessageL( 
        CMPXMessage* aMessage,
        TInt aError )
    {
#ifdef _DEBUG
    if ( aMessage->IsSupported( KVcxMediaMyVideosMessageId ) )
        {
        IPTVLOGSTRING3_LOW_LEVEL(
            "MPX My Videos UI # HandleSingleCollectionMessageL(xxx,%d) MessageId: %d",
            aError,
            static_cast<TUint32>( *( aMessage->Value<TUint32>( KVcxMediaMyVideosMessageId ) ) ) );
        }
    else
#endif // _DEBUG
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "MPX My Videos UI # HandleSingleCollectionMessageL(xxx,%d)", aError );
        }

    if ( aMessage && aError == KErrNone )
        {
        switch ( *( aMessage->Value<TMPXMessageId>( KMPXMessageGeneralId ) ) )
            {
            case KVcxCommandIdMyVideos:
                {
                if ( aMessage->IsSupported( KVcxMediaMyVideosCommandId ) )
                    {
                    TInt mvMsgId = *(aMessage->Value<TInt>( KVcxMediaMyVideosCommandId ) );

                    switch ( mvMsgId )
                        {
                        case KVcxMessageMyVideosGetMediasByMpxIdResp:
                            HandleGetMediasByMpxIdRespL( aMessage, aError );
                            break;
                        case KVcxMessageMyVideosItemsAppended:
                            HandleMyVideosItemsAppendedL( aMessage, aError );
                            break;
                        case KVcxMessageMyVideosListComplete:
                            HandleMyVideosListCompleteL( aMessage, aError );
                            break;
                        case KVcxMessageMyVideosDeleteStarted:
                        case KVcxMessageMyVideosDeleteResp:
                            HandleMyVideosDeleteMessageL( aMessage, aError );
                            break;
                        case KVcxMessageMyVideosMoveOrCopyStarted:
                        case KVcxMessageMyVideosMoveResp:
                        case KVcxMessageMyVideosCopyResp:
                            HandleMyVideosMoveOrCopyMessageL( aMessage, aError );
                            break;
                        default:
                            break;
                        }
                    }
                }
                break;
            case KMPXMessageGeneral:
                {
                IPTVLOGSTRING_LOW_LEVEL( 
                    "MPX My Videos UI # KMPXMessageGeneral" );
                
                TInt event( *(aMessage->Value<TInt>( KMPXMessageGeneralEvent )) );
                TInt data( *(aMessage->Value<TInt>( KMPXMessageGeneralData )) );

                switch ( event )
                    {
                    case TMPXCollectionMessage::EPathChanged:
                        {
                        if ( data == EMcContainerOpened  )
                            {
                            if ( iCollectionOpenStatus == EVcxHgCollectionOpening )
                                {
                                iCollectionOpenStatus = EVcxHgCollectionOpened;
                                }
                            iCollectionUtility->Collection().OpenL();
                            }
                        iCollectionLevel = CollectionLevelL();
                        }
                        break;                        
                    case TMPXCollectionMessage::ECollectionChanged:
                        {
                        // NOP, called when My Videos collection initially opened.
                        }
                        break;                        
                    case TMPXCollectionMessage::EBroadcastEvent:
                    case TMPXCollectionMessage::ENoEvent:
                    case TMPXCollectionMessage::EError:
                    case TMPXCollectionMessage::EMediaChanged:
                    case TMPXCollectionMessage::EItemChanged:
                    case TMPXCollectionMessage::EFocusChanged:
                    case TMPXCollectionMessage::EAsyncOpComplete:
                    case TMPXCollectionMessage::EExtendedMessage:
                    default:
                        break;                        
                    }
                }
                break;
            case KMPXMessageIdItemChanged:
                {
                IPTVLOGSTRING_LOW_LEVEL( 
                    "MPX My Videos UI # HandleSingleCollectionMessageL() KMPXMessageIdItemChanged" );                
                // Handle messages from our plugin only, this check skips also array messages (KMPXMessageArrayContents) 
                if ( aMessage->IsSupported( KMPXMessageCollectionId ) )
                    {
                    TUid collectionUid( *(aMessage->Value<TUid>( KMPXMessageCollectionId )) );
                    IPTVLOGSTRING2_LOW_LEVEL( 
                        "MPX My Videos UI # HandleMyVideosItemsChangedL() CollectionUid=%X", collectionUid.iUid );                
                    if ( collectionUid.iUid == KVcxUidMyVideosMpxCollection )
                        {
                        HandleMyVideosItemsChangedL( aMessage, aError );
                        }
                    }
                }
                break;
            default:
                {
                IPTVLOGSTRING_LOW_LEVEL( 
                    "MPX My Videos UI # HandleSingleCollectionMessageL() ???" );                
                }
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleOpenL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleOpenL( 
        const CMPXMedia& aEntries,
        TInt /*aIndex*/,
        TBool /*aComplete*/,
        TInt aError )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # HandleOpenL(aError=%d)", aError );

    if ( aError == KErrNone )
        {
        TInt levels = CollectionLevelL();

        if ( levels == KVcxMpxLevelCategories )
            {
            // Check that entry has an array of contents.
            if ( aEntries.IsSupported( KMPXMediaArrayContents ) )
                {
                CMPXMediaArray* pointerArray = 
                    aEntries.Value<CMPXMediaArray>(KMPXMediaArrayContents);
                CMPXMediaArray* categoryArray = CMPXMediaArray::NewL( *pointerArray );
                CleanupStack::PushL( categoryArray );

                if ( iCategoryModelObserver )
                    {
                    TBool isPartial( EFalse );
                    if ( aEntries.IsSupported( KVcxMediaMyVideosVideoListIsPartial ) )
                        {
                        isPartial = aEntries.ValueTObjectL<TBool>( KVcxMediaMyVideosVideoListIsPartial );
                        }
                    iCategoryModelObserver->NewCategoryListL( categoryArray, isPartial );
                    CleanupStack::Pop( categoryArray );
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( categoryArray );
                    }
                }
            }
        else if ( levels == KVcxMpxLevelVideos )
            {
            // Check that entry has an array of contents.
            if ( aEntries.IsSupported( KMPXMediaArrayContents ) )
                {
                CMPXMediaArray* pointerArray = 
                    aEntries.Value<CMPXMediaArray>(KMPXMediaArrayContents);
                CMPXMediaArray* videoArray = CMPXMediaArray::NewL( *pointerArray );
                CleanupStack::PushL( videoArray );

#if defined(_DEBUG) && IPTV_LOGGING_METHOD != 0
                DebugPrintVideoListL( videoArray );
#endif // _DEBUG && IPTV_LOGGING_METHOD

                if ( iVideoModelObserver )
                    {
                    iVideoModelObserver->NewVideoListL( *videoArray );
                    }
                CleanupStack::PopAndDestroy( videoArray );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleOpenL()
// -----------------------------------------------------------------------------
//                      
void CVcxHgMyVideosCollectionClient::HandleOpenL( 
        const CMPXCollectionPlaylist& /*aPlaylist*/,
        TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleCommandComplete()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleCommandComplete( 
        CMPXCommand* aCommandResult,
        TInt aError )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # HandleCommandComplete(xxx,%d)", aError );

    if ( iDownloadClient->HasPendingCommand() )
        {
        TRAP_IGNORE( iDownloadClient->CommandCompleteL( aError ) );
        }
    else if ( HasPendingCommand() )
        {
        // If "HandleCommandComplete" is received before our state has moved to
        // "Operation_Finished", MPX has mixed the events asynchronously. In those
        // cases we don't clear the pending command yet, but instead we mark what
        // happened and the "Operation_Finished" handler will clear the command.
        if ( iPendingCommand == EVcxHgMyVideosCollectionCommandMoveCopy_Started )
            {
            iPendingCommand = EVcxHgMyVideosCollectionCommandMoveCopy_Finished;
            }
        else if ( iPendingCommand == EVcxHgMyVideosCollectionCommandMultiDelete_Started )
            {
            iPendingCommand = EVcxHgMyVideosCollectionCommandMultiDelete_Finished;
            }
        else if ( iPendingCommand == EVcxHgMyVideosCollectionCommandVideoDetails_Requested )
            {
            TRAP_IGNORE( HandleGetVideoDetailsRespL( aCommandResult, aError ) );
            iPendingCommand = EVcxHgMyVideosCollectionCommandNone;
            }
        else if ( iPendingCommand != EVcxHgMyVideosCollectionCommandCancelDelete_Completed &&
		          iPendingCommand != EVcxHgMyVideosCollectionCommandCancelMoveCopy_Completed )
            {
            iPendingCommand = EVcxHgMyVideosCollectionCommandNone;
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleMyVideosItemsAppendedL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleMyVideosItemsAppendedL( CMPXMessage* aMessage,
                                                                   TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # HandleMyVideosItemsAppendedL()" );

    TInt levels = CollectionLevelL();
    TBool categoryEvent( EFalse );

    if ( aMessage->IsSupported( KMPXMessageGeneralId ) )
        {
        categoryEvent = ( aMessage->Value<TMPXItemId>( KMPXMessageGeneralId )->iId2 == 1 );
        }

    // Check that necessary events are not filtered out.
    
    if ( levels == KVcxMpxLevelCategories )
        {
        if ( iCategoryModelObserver && categoryEvent )
            {
            iCategoryModelObserver->CategoryListModifiedL();
            }        
        }
    else if ( levels == KVcxMpxLevelVideos )
        {
        if ( iVideoModelObserver && ! categoryEvent )
            {
            iVideoModelObserver->VideoListModifiedL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleMyVideosItemsChangedL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleMyVideosItemsChangedL( CMPXMessage* aMessage,
                                                                   TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # HandleMyVideosItemsChangedL()" );

    TMPXChangeEventType eventType( 
        *aMessage->Value<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
    TMPXItemId eventData( 
        *aMessage->Value<TMPXItemId>( KMPXMessageMediaGeneralId ) );

    TInt levels = CollectionLevelL();
    TInt eventlevel( KVcxHgEventLevelVideo );

    if ( eventData.iId2 == KVcxUidMyVideosMpxCollection )
        {
        eventlevel = KVcxHgEventLevelRoot;
        }
    else if ( eventData.iId2 == 1 )
        {
        eventlevel = KVcxHgEventLevelCategory;
        }
    else
        {
        eventlevel = KVcxHgEventLevelVideo;
        }

    // Check that necessary events are not filtered out.

    // If root level is modified, it means that MMC has been plugged / 
    // unplugged. For video view the changed video list will be retrieved
    // based on change of parent category, but for category view we need
    // to re-open the list.
    if ( eventlevel == KVcxHgEventLevelRoot )
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # HandleMyVideosItemsChangedL() - MMC plugged/unplugged." );
        
        if ( levels == KVcxMpxLevelCategories )
            {
            iCollectionUtility->Collection().OpenL();
            }
        }
    else
        {
        if ( levels == KVcxMpxLevelCategories )
            {
            if ( iCategoryModelObserver )
                {
                iCategoryModelObserver->CategoryModifiedL( eventType, eventData );
                }        
            }
        else if ( levels == KVcxMpxLevelVideos )
            {
            TInt32 extraInfo( EVcxMyVideosListNoInfo );
            if ( aMessage->IsSupported( KVcxMediaMyVideosInt32Value ) )
                {
                extraInfo = aMessage->ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value );
                }

            if ( iVideoModelObserver && 
                 ( eventlevel == KVcxHgEventLevelVideo ||
                   extraInfo == EVcxMyVideosVideoListOrderChanged ) ) 
                {
                iVideoModelObserver->VideoModifiedL( eventType, eventData, extraInfo );
                }
            }
        }   
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleMyVideosDeleteMessageL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleMyVideosDeleteMessageL( CMPXMessage* aMessage,
                                                                   TInt aError )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # HandleMyVideosDeleteMessageL() aError = %d", aError );

    TInt mvMsgId( aMessage->ValueTObjectL<TInt>( KVcxMediaMyVideosCommandId ) );

    // CancelDelete_Requested is used to indicate that user wanted to cancel the deletion
    // when we still hadn't received KVcxMessageMyVideosDeleteStarted message. Therefore
    // we needed to wait until now to cancel the deletion.
    if ( iPendingCommand == EVcxHgMyVideosCollectionCommandCancelDelete_Requested )
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # HandleMyVideosDeleteMessageL() - Cancelling." );

        // Use convenient value so that CancelDeleteVideosL() will perform immediate cancel.
        iPendingCommand = EVcxHgMyVideosCollectionCommandMultiDelete_Started;

        CancelDeleteVideosL();
        return;
        }
    // CancelDelete_Completed is used to indicate that cancellation of deletion is now
    // completed. Cancel never shows error notes, that's why special handling is needed.
    else if ( iPendingCommand == EVcxHgMyVideosCollectionCommandCancelDelete_Completed )
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # HandleMyVideosDeleteMessageL() - Cancelled." );

        if ( iVideoModelObserver )
            {
            TMPXItemId emptyMpxItemId;
            iVideoModelObserver->VideoDeletionCompletedL( 0, emptyMpxItemId );
            }
        iPendingCommand = EVcxHgMyVideosCollectionCommandNone;
        return;
        }

    if ( mvMsgId == KVcxMessageMyVideosDeleteStarted )
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # HandleMyVideosDeleteMessageL() - Delete started." );

        iPendingCommand = EVcxHgMyVideosCollectionCommandMultiDelete_Started;
        }
    else // aMessage == KVcxMessageMyVideosDeleteResp
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # HandleMyVideosDeleteMessageL() - Delete Resp (finished)." );

        // See HandleCommandComplete().
        if ( iPendingCommand == EVcxHgMyVideosCollectionCommandMultiDelete_Finished )
            {
            iPendingCommand = EVcxHgMyVideosCollectionCommandNone;
            }
        else
            {
            iPendingCommand = EVcxHgMyVideosCollectionCommandMultiDelete_Finished;
            }        

        CMPXMediaArray* messageArray = aMessage->Value<CMPXMediaArray>(
                KMPXMediaArrayContents );

        TInt32 failedCount( 0 );
        TMPXItemId latestMpxItemId;

        for ( TInt i = 0; i < messageArray->Count(); i++ )
            {
            if ( messageArray->AtL( i )->ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value )
                 != KErrNone )
                {
                failedCount++;
                latestMpxItemId = 
                    messageArray->AtL( i )->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                }
            else
                {
                if ( iVideoModelObserver )
                    {
                    // Sending remove request to video list model. This ensures that
                    // the deleted videos has removed from a video list before the 
                    // progress bar disappears.
                    TMPXItemId removedItemId = 
                        messageArray->AtL( i )->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                    iVideoModelObserver->VideoModifiedL( 
                        EMPXItemDeleted, removedItemId, EVcxMyVideosListNoInfo );
                    }
                }
            }

        if ( iVideoModelObserver )
            {
            iVideoModelObserver->VideoDeletionCompletedL( failedCount, latestMpxItemId );
            }        
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleMyVideosMoveOrCopyMessageL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleMyVideosMoveOrCopyMessageL( CMPXMessage* aMessage,
                                                                       TInt aError )
    {
    IPTVLOGSTRING2_LOW_LEVEL( 
        "MPX My Videos UI # HandleMyVideosMoveOrCopyMessageL() aError = %d", aError );

    TInt mvMsgId( aMessage->ValueTObjectL<TInt>( KVcxMediaMyVideosCommandId ) );

    if ( iPendingCommand == EVcxHgMyVideosCollectionCommandCancelMoveCopy_Requested )
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # HandleMyVideosMoveOrCopyMessageL() - Cancelling." );
        iPendingCommand = EVcxHgMyVideosCollectionCommandMoveCopy_Started;
        CancelMoveOrCopyVideosL();
        return;        
        }
    else if ( iPendingCommand == EVcxHgMyVideosCollectionCommandCancelMoveCopy_Completed )
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # HandleMyVideosMoveOrCopyMessageL() - Cancelled." );
        if ( iVideoModelObserver )
            {
            TMPXItemId emptyMpxItemId;
            iVideoModelObserver->VideoMoveOrCopyCompletedL( 0, emptyMpxItemId );
            }
        iPendingCommand = EVcxHgMyVideosCollectionCommandNone; 
        return;
        }

    if ( mvMsgId == KVcxMessageMyVideosMoveOrCopyStarted )
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # HandleMyVideosMoveOrCopyMessageL() - Move/copy started." );

        iPendingCommand = EVcxHgMyVideosCollectionCommandMoveCopy_Started;
        }
    else // KVcxMessageMyVideosMoveResp || KVcxMessageMyVideosCopyResp
        {
        IPTVLOGSTRING_LOW_LEVEL( 
            "MPX My Videos UI # HandleMyVideosMoveOrCopyMessageL() - Move/copy finished." );        

        // See HandleCommandComplete().
        if ( iPendingCommand == EVcxHgMyVideosCollectionCommandMoveCopy_Finished )
            {
            iPendingCommand = EVcxHgMyVideosCollectionCommandNone;
            }
        else if ( iPendingCommand != EVcxHgMyVideosCollectionCommandNone )
            {
            iPendingCommand = EVcxHgMyVideosCollectionCommandMoveCopy_Finished;
            }

        CMPXMediaArray* messageArray = aMessage->Value<CMPXMediaArray>(
                KMPXMediaArrayContents );

        TInt32 failedCount( 0 );
        TMPXItemId latestMpxItemId;

        for ( TInt i = 0; i < messageArray->Count(); i++ )
            {
            TInt err = messageArray->AtL( i )->ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value );
            if ( err != KErrNone && err != KErrAlreadyExists )
                {
                failedCount++;

                latestMpxItemId = 
                    messageArray->AtL( i )->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                }
            }
        
        if ( iVideoModelObserver )
            {
            iVideoModelObserver->VideoMoveOrCopyCompletedL( failedCount, latestMpxItemId );
            }        
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleGetMediasByMpxIdRespL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleGetMediasByMpxIdRespL( CMPXMessage* aMessage,
                                                                  TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # HandleGetMediasByMpxIdRespL()" );

    if ( aError == KErrNone )
        {
        CMPXMedia* entries = CMPXMedia::NewL( *aMessage );
        CleanupStack::PushL( entries );
        CMPXMediaArray* array = entries->Value<CMPXMediaArray>( KMPXMediaArrayContents );
        if ( array->Count() >= 1 )
            {
            TUint32 transactionId( 0 );
            
            if ( aMessage->IsSupported( KVcxMediaMyVideosTransactionId ) )
                {
                transactionId = aMessage->ValueTObjectL<TUint32>( KVcxMediaMyVideosTransactionId );
                }
            
            if ( transactionId == KVcxHgMyVideosTransactionId )
                {
                if ( iVideoModelObserver )
            	    {
            	    CMPXMedia* media = CMPXMedia::NewL( *( ( *array )[0] ) );
                    // Ownership is transferred.
            	    iVideoModelObserver->VideoFetchingCompletedL( media );  
            	    }
                
                else if ( iCategoryModelObserver )
                    {
                    CMPXMedia* media = CMPXMedia::NewL( *( ( *array )[0] ) );
                    // Ownership is transferred.
                    iCategoryModelObserver->VideoFetchingCompletedL( media );  
                    }
                }
			}
        CleanupStack::PopAndDestroy( entries );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleMyVideosListCompleteL( CMPXMessage* /*aMessage*/,
                                                                  TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # HandleMyVideosListCompleteL()" );
    if ( aError == KErrNone )
        {
        if ( iCategoryModelObserver )
            {
            iCategoryModelObserver->CategoryListFetchingCompletedL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::HandleGetVideoDetailsRespL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::HandleGetVideoDetailsRespL( CMPXMessage* aMessage,
                                                                 TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL( "MPX My Videos UI # HandleGetVideoDetailsRespL()" );

    CMPXMedia* media( NULL );
    
    if ( aError == KErrNone && aMessage && 
            aMessage->IsSupported( KMPXCommandColAddMedia ) )
        {
        media = aMessage->Value<CMPXMedia>( KMPXCommandColAddMedia );
        }
    
    if ( iVideoModelObserver && media )
        {
        iVideoModelObserver->VideoDetailsCompletedL( *media );
        }
    }

#if defined(_DEBUG) && IPTV_LOGGING_METHOD != 0
// -----------------------------------------------------------------------------
// CVcxHgMyVideosCollectionClient::DebugPrintVideoListL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCollectionClient::DebugPrintVideoListL( CMPXMediaArray* aVideoArray )
    {
    CMPXMedia* media = NULL;
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # --------------------------------------------------------" );
    for ( TInt i = 0; i < aVideoArray->Count(); i++ )
        {
        media = (*aVideoArray)[i]; 
        
        if ( media->IsSupported( KMPXMediaGeneralTitle ) && 
             media->IsSupported( KMPXMediaGeneralUri ) &&
             media->IsSupported( KMPXMediaGeneralId ) )
            {
            IPTVLOGSTRING4_LOW_LEVEL( 
                "Video: %S / %S / %d",
                &media->ValueText( KMPXMediaGeneralUri ),
                &media->ValueText( KMPXMediaGeneralTitle ),
                ( media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) ).iId1 ); 
            }
        }
    IPTVLOGSTRING_LOW_LEVEL( 
        "MPX My Videos UI # --------------------------------------------------------" );
    }
#endif // _DEBUG && IPTV_LOGGING_METHOD
