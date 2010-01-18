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
* Description:    Class for handling mpx collection interactions*
*/




#include <bldvariant.hrh>
#include "IptvDebug.h"
#include "CIptvUtil.h"
#include <mpxcollectionutility.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionpath.h>
#include <mpxmessage2.h>
#include <mpxcollectionmessage.h>
#include <mpxlog.h>
#include <mpxitemid.h>
#include <mpxmessagecontainerdefs.h>
#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <HttpDownloadMgrCommon.h>

#include "CIptvMyVideosGlobalFileId.h"
#include "vcxnsmpxcollectionclienthandler.h"
#include "vcxnscontentclienthandler.h"
#include "vcxnsuiengine.h"
#include "vcxnseventrouter.h"
#include "vcxnscontentprovider.h"

#include "vcxnsservice.h"
#include "vcxnscontent.h"
#include "vcxnscontentaccess.h"
#include "vcxnsdownloadupdater.h"

const TInt KMpxMediaId2 = 0;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxNsMpxCollectionClientHandler::CVcxNsMpxCollectionClientHandler( CVcxNsUiEngine& aUiEngine )
  : iUiEngine( aUiEngine ),
    iMarkAsWatchedId( KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxNsMpxCollectionClientHandler* CVcxNsMpxCollectionClientHandler::NewL( 
                                                           CVcxNsUiEngine& aUiEngine )
    {
    CVcxNsMpxCollectionClientHandler* self =
        new (ELeave) CVcxNsMpxCollectionClientHandler( aUiEngine );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;    
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::ConstructL()
    {
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );

    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( KVcxUidMyVideosMpxCollection );
    iCollectionUtility->Collection().OpenL( *path );
    CleanupStack::PopAndDestroy( path );
    
    iDownloadUpdater = CVcxNsDownloadUpdater::NewL( this );

    iGetMediaDetailWait = new ( ELeave ) CActiveSchedulerWait;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxNsMpxCollectionClientHandler::~CVcxNsMpxCollectionClientHandler()
    {
    iLastFetchedMedias.ResetAndDestroy();

    iResumeArray.Close();
    delete iDownloadUpdater;
    
    if( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }

    iDownloadedContents.Reset();

    delete iGetMediaDetailWait;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
TInt CVcxNsMpxCollectionClientHandler::DownloadL( CVcxNsService& aService, 
                                                  CVcxNsContent& aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::DownloadL(s,c) IN");
    
    aContent.SetDlFailNotified( EFalse );
    aContent.SetUsernameL( aService.GetUserName() );
    aContent.SetPasswordL( aService.GetPassword() );
    
    if( aContent.PendingCommand() == CVcxNsContent::EVcxNsMpxContentCommandStartDownload ||
        aContent.PendingCommand() == CVcxNsContent::EVcxNsMpxContentCommandResumeDownload ||
        aContent.PendingCommand() == CVcxNsContent::EVcxNsMpxContentCommandWaitingForMpxItem )
        {
        // Download request is already made, but 
        // Collection hasn't processed it yet.
        return KErrAlreadyExists;
        }

    if( aContent.PendingCommand() == CVcxNsContent::EVcxNsMpxContentCommandCancelDownload )
        {
        // Download request is already made, and user has cancelled
        // it already, but Collection hasn't process the requests yet.
        // So reset just the pending cmd data.
        aContent.SetPendingCommand( CVcxNsContent::EVcxNsMpxContentCommandNone  );

        return KErrNone;
        }

    TInt ret( KErrGeneral );

    if( aContent.GetUrl( CVcxNsContent::EVcxContentTypeVideo ).Length() == 0 )
        {
        ret = KErrArgument;
        }
    else 
        {
        TUint32 transactionId( 0 );

        ret = DownloadL( aContent.GetUrl( CVcxNsContent::EVcxContentTypeVideo ),
                         aContent.GetName(),
                         aService.GetIap(),                          
                         aService.GetIsReadOnlyIap(),
                         aService.GetUserName(),
                         aService.GetPassword(),
                         transactionId );
        
        if ( ret == KErrNone )
            {
            // To the download list. Content from Mpx events
            // is resolved from the list.
            AppendToDownloadList( &aContent );

            aContent.SetPendingCommand( CVcxNsContent::EVcxNsMpxContentCommandStartDownload );
            aContent.SetCmdTransactionId( transactionId );
            
            iUiEngine.ContentProvider()->UpdateContent( &aContent );
            }
        else
            {
            aContent.SetPendingCommand( CVcxNsContent::EVcxNsMpxContentCommandNone );
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::DownloadL(s,c) OUT");

    return ret;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
TInt CVcxNsMpxCollectionClientHandler::DownloadL( const TDesC& aAddress,
                                                  const TDesC& aName,
                                                  TUint32 aIapId,
                                                  TBool aIsReadOnlyIap,
                                                  const TDesC& aUserName,
                                                  const TDesC& aPassword,
                                                  TUint32& aTransactionId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::DownloadL() start");
           
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd ); // 1->
    
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosStartDownload );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse /* = async */ );
    cmd->SetTObjectValueL( KVcxMediaMyVideosTransactionId, ++iLatestTransactionId );
    aTransactionId = iLatestTransactionId;

    TUid colId = TUid::Uid( KVcxUidMyVideosMpxCollection );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
    
    CMPXMedia* startDownloadReq = CMPXMedia::NewL();
    CleanupStack::PushL( startDownloadReq ); // 2->
    
    TUint flags = ( aIsReadOnlyIap ? EVcxMyVideosServiceHasReadOnlyIap : 0 ) | 
                        EVcxMyVideosSilent;
    
    startDownloadReq->SetTObjectValueL( KMPXMediaGeneralFlags, flags );
    
    if( aIsReadOnlyIap )
        {
        startDownloadReq->SetTObjectValueL( KVcxMediaMyVideosIapId, aIapId );
        }
    else
        {
        // not a read only iap, set iap 0 so download utility 
        // can select iap via connectionutility
        startDownloadReq->SetTObjectValueL( KVcxMediaMyVideosIapId, 0 );
        }

    startDownloadReq->SetTextValueL( KVcxMediaMyVideosRemoteUrl, aAddress );
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosUsername, aUserName );
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosPassword, aPassword );
    startDownloadReq->SetTextValueL( KMPXMediaGeneralTitle, aName );

    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, startDownloadReq );
    
    iCollectionUtility->Collection().CommandL( *cmd );
    
    CleanupStack::PopAndDestroy( startDownloadReq ); // <-2
    CleanupStack::PopAndDestroy( cmd );              // <-1

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::DownloadL() exit");
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
TInt CVcxNsMpxCollectionClientHandler::PauseDownloadL( CVcxNsContent& aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::PauseDownloadL IN");
    
    TInt ret( KErrNone );
    
    if( !aContent.GetMpxMedia() ||
             !aContent.GetMpxMedia()->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId ) )
        {
        IPTVLOGSTRING_LOW_LEVEL(
             "CVcxNsMpxCollectionClientHandler::PauseDownloadL OUT, bad argument");
        
        ret = KErrArgument;
        }
    else
        {
        CMPXMedia* media = CMPXMedia::NewL( *(aContent.GetMpxMedia()) );
        CleanupStack::PushL( media );
        
        CMPXCommand* cmd = CMPXCommand::NewL();
        CleanupStack::PushL( cmd );
        
        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosPauseDownload );
        cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse );
        
        TUid colId = TUid::Uid( KVcxUidMyVideosMpxCollection );
        cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );

        TUint32 downloadId = media->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );            
        cmd->SetTObjectValueL( KVcxMediaMyVideosDownloadId, downloadId );        
        iCollectionUtility->Collection().CommandL( *cmd );

        CleanupStack::PopAndDestroy( cmd );
        CleanupStack::PopAndDestroy( media );
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::PauseDownloadL OUT");

    return ret;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
TInt CVcxNsMpxCollectionClientHandler::ResumeDownloadL( CVcxNsContent& aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::ResumeDownloadL IN");
    
    aContent.SetDlFailNotified( EFalse );
    
    TInt ret( KErrNone );
     
    if( !aContent.GetMpxMedia() || 
             !aContent.GetMpxMedia()->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId ) )
        {
        ret = KErrArgument;
        IPTVLOGSTRING_LOW_LEVEL(
            "CVcxNsMpxCollectionClientHandler::ResumeDownloadL OUT, bad argument");
        }
    else
        {
        CMPXMedia* media = CMPXMedia::NewL( *(aContent.GetMpxMedia()) );
        CleanupStack::PushL( media );
        
        CMPXCommand* cmd = CMPXCommand::NewL();
        CleanupStack::PushL( cmd );
        
        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosStartDownload );
        cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse );
        
        TUid colId = TUid::Uid( KVcxUidMyVideosMpxCollection );
        cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
        cmd->SetTObjectValueL( KVcxMediaMyVideosTransactionId, ++iLatestTransactionId );

        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:: TR ID = %d", iLatestTransactionId);

        TUint32 transactionId = iLatestTransactionId;

        media->SetTObjectValueL( KVcxMediaMyVideosIapId, 0 );
        media->SetTextValueL( KVcxMediaMyVideosUsername, aContent.GetUsername() );
        media->SetTextValueL( KVcxMediaMyVideosPassword, aContent.GetPassword() );
        
        cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, media );
        iCollectionUtility->Collection().CommandL( *cmd );

        CleanupStack::PopAndDestroy( cmd );
        CleanupStack::PopAndDestroy( media );
        
        aContent.SetPendingCommand( CVcxNsContent::EVcxNsMpxContentCommandResumeDownload );
        aContent.SetCmdTransactionId( transactionId );
        
        AppendToDownloadList( &aContent );
        iUiEngine.ContentProvider()->UpdateContent( &aContent );
        
        TUint32 mpxId = aContent.GetMpxId();
		if ( iResumeArray.Find( mpxId ) == KErrNotFound )
		    { 
            iResumeArray.AppendL( mpxId );
			}
        
        IPTVLOGSTRING_LOW_LEVEL(
            "CVcxNsMpxCollectionClientHandler::ResumeDownloadL OUT, cmd OK.");
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
TInt CVcxNsMpxCollectionClientHandler::CancelDownloadL( CVcxNsContent& aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::CancelDownloadL IN");

    aContent.SetDlFailNotified( EFalse );
    
    TInt ret( KErrNone );
    
    if( !aContent.GetMpxMedia(CVcxNsContent::EVcxContentTypeVideo) )
        {
        // Even though cancel cannot be called now, it must be called later
        // So mark the content to be pending the cancel.
        aContent.SetPendingCommand ( CVcxNsContent::EVcxNsMpxContentCommandCancelDownload );
        
        // Incremet cmd id, so the cmd is not reset when this 
        // content's previous cmd's complete msg arrives.
        aContent.SetCmdTransactionId ( ++iLatestTransactionId );
        
        // Redraw the fake state to user.
        iUiEngine.ContentProvider()->UpdateContent( &aContent );
        }
    else
        {
        CMPXCommand* cmd = CMPXCommand::NewL();
        CleanupStack::PushL( cmd );
        
        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, 
                               KVcxCommandMyVideosCancelDownload );
        
        cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse /* = async */ );
        TUid colId = TUid::Uid( KVcxUidMyVideosMpxCollection );
        cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
        
        CMPXMedia* media = aContent.GetMpxMedia( CVcxNsContent::EVcxContentTypeVideo );
        
        if ( media )
            {
            cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, media );
            cmd->SetTObjectValueL( KVcxMediaMyVideosTransactionId, ++iLatestTransactionId );
            
            iCollectionUtility->Collection().CommandL( *cmd );
            }
        
        CleanupStack::PopAndDestroy( cmd );
        
        aContent.SetPendingCommand ( CVcxNsContent::EVcxNsMpxContentCommandCancelDownload );
        aContent.SetCmdTransactionId ( iLatestTransactionId );
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::CancelDownloadL OUT");
    
    return ret;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsMpxCollectionClientHandler::LoadMpxMediaObjectsL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::LoadMpxMediaObjectsL IN");

    RArray<TUint32> ids;
    CleanupClosePushL( ids );

    for( TInt i = 0; i < iDownloadedContents.Count(); i++ )
        {
        if( (iDownloadedContents[i])->GetMpxId() )
            {
            ids.Append( (iDownloadedContents[i])->GetMpxId() );
            }
        }

    GetMediaByMpxIdReqL( ids );
    CleanupStack::PopAndDestroy( &ids );
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::LoadMpxMediaObjectsL OUT");
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsMpxCollectionClientHandler::LoadMpxMediaObjectL( TUint32 aMpxId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::LoadMpxMediaObjectL IN");

    RArray<TUint32> id;
    CleanupClosePushL( id );
    id.Append( aMpxId );
    GetMediaByMpxIdReqL( id );
    CleanupStack::PopAndDestroy( &id );
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::LoadMpxMediaObjectL OUT");
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsMpxCollectionClientHandler::HandleCollectionMediaL( const CMPXMedia& aMedia, 
                             TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::HandleCollectionMediaL");
    
    if ( aError == KErrNone )
        {
        SearchAndAddMpxMediaL( & const_cast<CMPXMedia&>( aMedia ) );

        if ( aMedia.IsSupported( KMPXMediaGeneralId ) )
            {
            iLastFetchedMedias.ResetAndDestroy();
            iLastFetchedMedias.Append( CMPXMedia::NewL( aMedia ) );
            }
        }
    
    if ( iGetMediaDetailWait->IsStarted() )
        {
        iGetMediaDetailWait->AsyncStop();
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::HandleCollectionMessage
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::HandleCollectionMessage(CMPXMessage* aMessage, 
                                                               TInt aError )
    {
    TRAPD( err, ExplodeCollectionMessagesL( aMessage, aError ) );
    
    if ( err != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
"CVcxNsMpxCollectionClientHandler::ExplodeCollectionMessagesL() leaved with error code %d", err);
        }
    }
    
// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::ExplodeCollectionMessagesL
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::ExplodeCollectionMessagesL(CMPXMessage* aMessage, 
                                                                  TInt aError )
    {
    if ( !aMessage )
        {
        return;
        }
    
    TInt cmdId = aMessage->ValueTObjectL<TMPXMessageId>( KMPXMessageGeneralId );
    TInt myVideosCmd = -1; 

    if ( cmdId == KVcxCommandIdMyVideos )
        {
        myVideosCmd = aMessage->ValueTObjectL<TInt>( KVcxMediaMyVideosCommandId );
        }
    
    if ( myVideosCmd != KVcxMessageMyVideosMessageArray )
        {
        HandleSingleCollectionMessageL( aMessage, aError );
        }
    else
        {
        if ( aMessage->IsSupported( KMPXMessageArrayContents ) )
            {
            const CMPXMessageArray* messageArray =
                aMessage->Value<CMPXMessageArray>(KMPXMessageArrayContents);

            if ( messageArray->Count() > 0 )
                {
                IPTVLOGSTRING_LOW_LEVEL(
                   "CVcxNsMpxCollectionClientHandler:: ----------------------.");
                IPTVLOGSTRING_LOW_LEVEL(
                   "CVcxNsMpxCollectionClientHandler:: MESSAGE ARRAY ARRIVED |");
                IPTVLOGSTRING_LOW_LEVEL(
                   "CVcxNsMpxCollectionClientHandler:: ----------------------'");
                }
                
            for( TInt i = 0; i < messageArray->Count(); i++ )
                {
                HandleSingleCollectionMessageL( messageArray->AtL( i ), aError );
                }    
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsMpxCollectionClientHandler::HandleSingleCollectionMessageL( CMPXMessage* aMsg, 
                                                                       TInt aErr )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::HandleSingleCollectionMessageL IN");

#ifdef _DEBUG
    
    TMPXMessageId msgId = 0;    
    if ( aMsg->IsSupported( KMPXMessageGeneralId ) )
        {
        msgId = aMsg->ValueTObjectL<TMPXMessageId>(KMPXMessageGeneralId);
        }
    
    TUid colId;
    colId.iUid = 0;
    if ( aMsg->IsSupported( KMPXMessageCollectionId ) )
        {
        colId = aMsg->ValueTObjectL<TUid>(KMPXMessageCollectionId);
        }
            
    TMPXChangeEventType type = static_cast<TMPXChangeEventType>( -1 );
    if ( aMsg->IsSupported( KMPXMessageChangeEventType ) )
        { 
        type = aMsg->ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType);
        }
        
    TMPXItemId itemId(0,0);
    if ( aMsg->IsSupported( KMPXMessageMediaGeneralId ) )
        {
        itemId = aMsg->ValueTObjectL<TMPXItemId>( KMPXMessageMediaGeneralId );
        }
    else
        {
        IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## HandleSingleCollectionMessageL() : KMPXMessageMediaGeneralId not supported!");
        }
        
    TInt cmdId = -1;
    if ( aMsg->IsSupported( KVcxMediaMyVideosCommandId ) )
        {
        cmdId = aMsg->ValueTObjectL<TInt>( KVcxMediaMyVideosCommandId );
        }
                
    IPTVLOGSTRING2_HIGH_LEVEL( 
          "UI Engine ## HandleSingleCollectionMessageL() : msgId = %d", msgId );
    IPTVLOGSTRING2_HIGH_LEVEL( 
          "UI Engine ## HandleSingleCollectionMessageL() : colId = %x", colId );
    
    switch ( type )
        {
        case EMPXItemDeleted:
            IPTVLOGSTRING_HIGH_LEVEL(
                  "UI Engine ## HandleSingleCollectionMessageL() : type = EMPXItemDeleted" );
            break;
        case EMPXItemInserted:
            IPTVLOGSTRING_HIGH_LEVEL(
                "UI Engine ## HandleSingleCollectionMessageL() : type = EMPXItemInserted" );
            break;
        case EMPXItemModified:
            IPTVLOGSTRING_HIGH_LEVEL(
                "UI Engine ## HandleSingleCollectionMessageL() : type = EMPXItemModified" );
            break;
        }
        
    IPTVLOGSTRING3_HIGH_LEVEL( 
          "UI Engine ## HandleSingleCollectionMessageL() : KMPXMessageMediaGeneralId = %d,%d", itemId.iId1, itemId.iId2 );
    IPTVLOGSTRING2_HIGH_LEVEL( 
          "UI Engine ## HandleSingleCollectionMessageL() : aErr = %d", aErr );

    switch ( cmdId )
        {
        case KVcxMessageMyVideosGetMediasByMpxIdResp:
            IPTVLOGSTRING_HIGH_LEVEL(
                  "UI Engine ## HandleSingleCollectionMessageL() : cmd Id = KVcxMessageMyVideosGetMediasByMpxIdResp" );
            
            if ( aMsg->IsSupported( KMPXMediaArrayContents ) )
                {
                const CMPXMessageArray* videoArray =
                    aMsg->Value<CMPXMessageArray>(KMPXMediaArrayContents);
                
                TInt count = videoArray->Count();
                CMPXMedia* video;
                TMPXItemId mpxId;
                for ( TInt i = 0; i < count; i++ )
                    {
                    video = videoArray->AtL( i );
                    mpxId = video->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                    IPTVLOGSTRING3_HIGH_LEVEL("UI Engine ## HandleSingleCollectionMessageL() :     contains video MPXID: %d,%d", mpxId.iId1, mpxId.iId2 );
                    }
                }
            else
                {
                IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## HandleSingleCollectionMessageL() : EMPTY KVcxMessageMyVideosGetMediasByMpxIdResp, SHOULD NOT HAPPEN");
                }
            
            break;

        case KVcxMessageMyVideosItemsAppended:
            IPTVLOGSTRING_HIGH_LEVEL(
                  "UI Engine ## HandleSingleCollectionMessageL() : cmd Id = KVcxMessageMyVideosItemsAppended" );
            break;

        case KVcxMessageMyVideosListComplete:
            IPTVLOGSTRING_HIGH_LEVEL(
                  "UI Engine ## HandleSingleCollectionMessageL() : cmd Id = KVcxMessageMyVideosListComplete" );
            break;

        case KVcxMessageMyVideosMoveOrCopyStarted:
            IPTVLOGSTRING_HIGH_LEVEL(
                  "UI Engine ## HandleSingleCollectionMessageL() : cmd Id = KVcxMessageMyVideosMoveOrCopyStarted" );
            break;

        case KVcxMessageMyVideosDeleteStarted:
            IPTVLOGSTRING_HIGH_LEVEL(
                  "UI Engine ## HandleSingleCollectionMessageL() : cmd Id = KVcxMessageMyVideosDeleteStarted" );
            break;

        case KVcxMessageMyVideosMoveResp:
            IPTVLOGSTRING_HIGH_LEVEL(
                  "UI Engine ## HandleSingleCollectionMessageL() : cmd Id = KVcxMessageMyVideosMoveResp" );
            break;

        case KVcxMessageMyVideosCopyResp:
            IPTVLOGSTRING_HIGH_LEVEL(
                  "UI Engine ## HandleSingleCollectionMessageL() : cmd Id = KVcxMessageMyVideosCopyResp" );
            break;

        case KVcxMessageMyVideosDeleteResp:
            IPTVLOGSTRING_HIGH_LEVEL(
                  "UI Engine ## HandleSingleCollectionMessageL() : cmd Id = KVcxMessageMyVideosDeleteResp" );
            break;

        }
        

#endif // _DEBUG
    
    DoHandleSingleCollectionMessageL( aMsg, aErr );
    
    IPTVLOGSTRING_LOW_LEVEL(
          "CVcxNsMpxCollectionClientHandler::HandleSingleCollectionMessage OUT");
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsMpxCollectionClientHandler::HandleOpenL( const CMPXCollectionPlaylist&
                                                    /*aPlaylist*/, TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::HandleOpenL( playlist )");
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsMpxCollectionClientHandler::HandleOpenL( const CMPXMedia& /*aEntries*/,
                  TInt /*aIndex*/, TBool /*aComplete*/, TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::HandleOpenL( aEntries )");
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsMpxCollectionClientHandler::HandleCommandComplete( CMPXCommand* aCommandResult,
                                                              TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::HandleCommandComplete IN");

    TInt32 error( KErrNone );
                            
    if ( aCommandResult->IsSupported( KVcxMediaMyVideosInt32Value ) )
        {
        error = aCommandResult->ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value );
        }

    TInt tid = -1;
    
    if( aCommandResult->IsSupported( KVcxMediaMyVideosTransactionId ) )
        {
        tid = aCommandResult->ValueTObjectL<TInt>( KVcxMediaMyVideosTransactionId );
        }

    TMPXItemId mpxId( 0, 0 );
        
    if ( aCommandResult->IsSupported( KMPXMediaGeneralId ) )
        {
        mpxId = aCommandResult->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
        }
        
    if( error != KErrNone )
        {    
        for( TInt i = iDownloadedContents.Count() - 1; i >= 0; i-- )
            {
            if( (iDownloadedContents[i])->CmdTransactionId() == tid )
                {
                IPTVLOGSTRING2_HIGH_LEVEL( "UI Engine ## clearing failed (TR ID = %d) command", tid); 
                (iDownloadedContents[i])->SetPendingCommand( 
                        CVcxNsContent::EVcxNsMpxContentCommandNone );
                iUiEngine.ContentProvider()->UpdateContent( iDownloadedContents[i] );
                break;
                }
            }        
        IPTVLOGSTRING2_HIGH_LEVEL( "UI Engine ## HandleCommandComplete() OUT, error = %d ",
                                   error);
        return;
        }
    
    if( !aCommandResult )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "CVcxNsMpxCollectionClientHandler::HandleCommandComplete OUT, aResult == NULL");
        return;
        }
    
    if( aCommandResult->IsSupported( KVcxMediaMyVideosTransactionId ) )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
                 "CVcxNsMpxCollectionClientHandler::HandleCommandComplete , trid == %d", tid );

        // See which content's request is processed.
        for( TInt i = iDownloadedContents.Count() - 1; i >= 0 ; i-- )
            {
            if( (iDownloadedContents[i])->CmdTransactionId() == tid )
                {
                switch ( (iDownloadedContents[i])->PendingCommand() )
                    {
                    case CVcxNsContent::EVcxNsMpxContentCommandCancelDownload:
                        {
                        IPTVLOGSTRING_LOW_LEVEL("UI Engine ## command complete EVcxNsMpxContentCommandCancelDownload OK");
                        (iDownloadedContents[i])->SetPendingCommand( CVcxNsContent::EVcxNsMpxContentCommandNone );
                        TMPXItemId mpxId = aCommandResult->Value<CMPXMedia>( KMPXCommandColAddMedia )
                            ->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                        DeleteDownload( mpxId );
                        }
                        break;
                        
                    case CVcxNsContent::EVcxNsMpxContentCommandStartDownload:
                        (iDownloadedContents[i])->SetPendingCommand( CVcxNsContent::EVcxNsMpxContentCommandWaitingForMpxItem );
                        break;                    
                
                    default:
                        (iDownloadedContents[i])->SetPendingCommand( CVcxNsContent::EVcxNsMpxContentCommandNone );
                        iUiEngine.ContentProvider()->UpdateContent( iDownloadedContents[i] );
                        break;
                    }
                break;
                }
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::HandleCommandComplete OUT");
    }

// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::GetMediaByMpxIdReqL
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::GetMediaByMpxIdReqL( RArray<TUint32>& aMpxIds )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::GetMediaByMpxIdReqL IN");
    
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosGetMediasByMpxId ); 
    TUid colId = TUid::Uid( KVcxUidMyVideosMpxCollection );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
    
    CMPXMediaArray* idMediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL( idMediaArray );

    CMPXMedia* media;
    TMPXItemId mpxId;
    for ( TInt i = 0; i < aMpxIds.Count(); i++ )
        {
        media = CMPXMedia::NewL();
        CleanupStack::PushL( media );
        
        mpxId.iId1 = aMpxIds[i];
        mpxId.iId2 = KMpxMediaId2;
        
        media->SetTObjectValueL( KMPXMessageMediaGeneralId, mpxId );
        idMediaArray->AppendL( *media );
        CleanupStack::PopAndDestroy( media );
        }
                            
    cmd->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, idMediaArray );
    
    cmd->SetTObjectValueL( KMPXMediaArrayCount, idMediaArray->Count() );

    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( idMediaArray );
    CleanupStack::PopAndDestroy( cmd ); 
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::GetMediaByMpxIdReqL OUT");
    }

// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::LinkMediaObjectsL
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::LinkMediaObjectsL( CMPXMediaArray* aArray  )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::LinkMediaObjectsL IN");
    
    if ( aArray )
        {
        for ( TInt i = aArray->Count()-1; i >= 0; i-- )
            {
            SearchAndAddMpxMediaL( (*aArray)[i] );
            }
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::LinkMediaObjectsL OUT");
    }

// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::SearchAndAddMpxMediaL
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::SearchAndAddMpxMediaL( CMPXMedia* aMedia )
    {
    //NOTE: It takes a while before this is called after download has been started.
    //      If application is closed before the media item has arrived, metadata is lost
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::SearchAndAddMpxMediaL IN");
    CVcxNsContent* content = NULL;
    TUint32 mpxId = aMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1;
    TPtrC uri = aMedia->ValueText( KVcxMediaMyVideosRemoteUrl );
    
    // search from dl first by mpx id
    IPTVLOGSTRING_LOW_LEVEL(
         "CVcxNsMpxCollectionClientHandler::SearchAndAddMpxMediaL: SearchFromDownloadList( mpxId )");
    content = SearchFromDownloadList( mpxId );
    
    if( !content )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "CVcxNsMpxCollectionClientHandler::SearchAndAddMpxMediaL: SearchFromDownloadList( uri ) ");
        // Not found from dl list, this
        // is new media. search by uri.        
        content = SearchFromDownloadList( uri );
        }
    
    // Cancel can be called before the media object
    // has arrived. If so, cancel it now.
    if( content && 
        content->PendingCommand()==CVcxNsContent::EVcxNsMpxContentCommandCancelDownload )
        {
        content->SetMpxMediaL( aMedia );
        CancelDownloadL( *content );
        // Dont link, object will be removed.
        return;
        }

    if( !content )
        {
        IPTVLOGSTRING_LOW_LEVEL(
             "CVcxNsMpxCollectionClientHandler::SearchAndAddMpxMediaL: SearchContentByUri( uri ) ");

        // Still not found from dl list, dl is not currently visible
        //   -> search from all contents.

        if( iUiEngine.GetContentProviderL() )
            {
	        content = iUiEngine.ContentProvider()->SearchContentByUri( uri );
            }

        // Content was found, but the download was not started directly
		// from Vod. Add to download list.
        if ( content && content->GetServiceId() == iUiEngine.ActiveService() )
            {
            AppendToDownloadList( content );
            }
        }

    if ( content )
        {
        CVcxNsContentAccess* contentAccess = 
            content->GetContentAccess( CVcxNsContent::EVcxContentTypeVideo );
        if ( contentAccess )
            {
            content->SetMpxMediaL( aMedia );
            
            if( !content->GetMpxId() || content->GetMpxId() != mpxId )
                {
                content->SetMpxId( mpxId );
                iUiEngine.GetContentClientHandlerL()->
                      StoreMpxIdToEcgDbL( content->GetServiceId(),
                                          content->GetContentId(),
                                          contentAccess->iIndex,
                                          mpxId );
    
                CopyEcgDataToMpxColL( content, mpxId );
                }
    
            if ( !content->GetMpxMedia()->IsSupported( KMPXMediaGeneralIcon ) &&
                  content->GetIconPath().Length() > 0 )
                {
                TIconInfo icon;
                icon.bmpfile = content->GetIconPath();
                content->GetMpxMedia()->SetTObjectValueL( KMPXMediaGeneralIcon, icon );
                }
            
            iUiEngine.ContentProvider()->UpdateContent( content );
            
            CVcxNsContent::TVcxContentDlStatus status = content->GetDlStatus();
            
            if( status == CVcxNsContent::EVcxContentDlStateDownloading )
                {
                iDownloadUpdater->AddDownload( content );
                }
            }
        }
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::SearchAndAddMpxMediaL OUT");
    }

// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::AppendToDownloadList
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::AppendToDownloadList( CVcxNsContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::AppendToDownloadList");
    
    if( iDownloadedContents.Find( aContent ) == KErrNotFound )
        {
        iDownloadedContents.Append( aContent );
        }
    
    if( aContent->GetMpxMedia() )
        {
        CVcxNsContent::TVcxContentDlStatus status = aContent->GetDlStatus();
        
        if( status == CVcxNsContent::EVcxContentDlStateDownloading )
            {
	        iDownloadUpdater->AddDownload( aContent );
	        }
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::RemoveFromDownloadList
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::RemoveFromDownloadList( CVcxNsContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::RemoveFromDownloadList");
    
    TInt index = iDownloadedContents.Find( aContent );
    
    if( index >= 0 )
        {
        iDownloadedContents.Remove( index );
        
        if( aContent->GetMpxMedia() )
            {
            iDownloadUpdater->RemoveDownload( aContent );
            }
        }

    index = iResumeArray.Find( aContent->GetMpxId() );
    if ( index >= 0 )
        {
        iResumeArray.Remove( index );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::ResetDownloadList
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::ResetDownloadList( TBool aResetCmdsFromContentItems )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::ResetDownloadList");

    if ( aResetCmdsFromContentItems )
        {    
        for( TInt i = iDownloadedContents.Count()-1; i >= 0 ; i-- )
            {
            // Reset the pending cmds before removing from the list.
            (iDownloadedContents[i])->SetPendingCommand( CVcxNsContent::EVcxNsMpxContentCommandNone );
            }
        }


    iDownloadedContents.Reset();
    iDownloadUpdater->RemoveAllDownloads();
    
    iResumeArray.Reset();
    }

// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::SearchFromDownloadList
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsMpxCollectionClientHandler::SearchFromDownloadList( const TDesC& aUri )
    {
    IPTVLOGSTRING_LOW_LEVEL(
         "CVcxNsMpxCollectionClientHandler::SearchFromDownloadList (aUri) IN");
    
    for( TInt i = iDownloadedContents.Count()-1; i >= 0 ; i-- )
        {
        TPtrC contentUri = TPtrC( (iDownloadedContents[i])->GetUrl( CVcxNsContent::EVcxContentTypeVideo ) );

        if ( contentUri.CompareF( aUri ) == 0 )
            {
            IPTVLOGSTRING_LOW_LEVEL(
                 "CVcxNsMpxCollectionClientHandler::SearchFromDownloadList (aUri) OUT, content found");
            return iDownloadedContents[i];
            }
        }

    IPTVLOGSTRING_LOW_LEVEL(
        "CVcxNsMpxCollectionClientHandler::SearchFromDownloadList (aUri) OUT, not found");
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::SearchFromDownloadList
// -----------------------------------------------------------------------------
//
CVcxNsContent* CVcxNsMpxCollectionClientHandler::SearchFromDownloadList( const TUint32 aMpxId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::SearchFromDownloadList (aId) IN");
    
    for( TInt i = iDownloadedContents.Count()-1; i >= 0 ; i-- )
        {
        TUint32 mpxId = (iDownloadedContents[i])->GetMpxId();

        if ( mpxId == aMpxId )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::SearchFromDownloadList (aId) OUT, found");
            return iDownloadedContents[i];
            }
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::SearchFromDownloadList (aId) OUT, not found");
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsMpxCollectionClientHandler::DoHandleSingleCollectionMessageL
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::DoHandleSingleCollectionMessageL( CMPXMessage* aMsg, TInt aErr )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::DoHandleSingleCollectionMessageL IN");
    
    if ( aMsg && aErr == KErrNone )
        {
        TMPXMessageId mpxMsgId = *( aMsg->Value<TMPXMessageId>( KMPXMessageGeneralId ) );
        
        TInt mpxCmdId( -1 );
        if ( aMsg->IsSupported( KVcxMediaMyVideosCommandId ) )
            {
            mpxCmdId = aMsg->ValueTObjectL<TInt>( KVcxMediaMyVideosCommandId );
            }
            
        if ( mpxMsgId == KMPXMessageGeneral )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::DoHandleSingleCollectionMessageL KMPXMessageGeneral");
            }
        else if ( mpxMsgId == KMPXMessageIdItemChanged )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::DoHandleSingleCollectionMessageL: KMPXMessageIdItemChanged");
            
            TMPXItemId id( 0, 0 );                            
            if ( aMsg->IsSupported( KMPXMessageMediaGeneralId ) )
                {
                id = aMsg->ValueTObjectL<TMPXItemId>( KMPXMessageMediaGeneralId );
                }

            if ( id.iId1 == KVcxUidMyVideosMpxCollection )
                {
                // the whole collection changed
                IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::DoHandleSingleCollectionMessageL: The whole collection changed, fetching all items.");
                LoadMpxMediaObjectsL();
                }
            else
                {
                iUiEngine.GetEventRouter()->HandleCollectionMessageL( aMsg, aErr );  
                HandleMpxItemChangedL( aMsg, aErr );
                }
            }

        else if ( mpxCmdId == KVcxMessageMyVideosGetMediasByMpxIdResp )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::DoHandleSingleCollectionMessageL KVcxMessageMyVideosGetMediasByMpxIdResp");
            CMPXMedia* entries = CMPXMedia::NewL( *aMsg );
            CleanupStack::PushL( entries );

            CMPXMediaArray* array = entries->Value<CMPXMediaArray>( KMPXMediaArrayContents );
            
            if ( array && 0 < array->Count() )
                {
                iLastFetchedMedias.ResetAndDestroy();
                
                for ( TInt i = 0; i < array->Count(); i++ )
                    {
                    iLastFetchedMedias.Append( CMPXMedia::NewL( *(*array)[i]) );
                    }
                }
            
            SearchAndMarkItemAsWatchedL( array );

            LinkMediaObjectsL( array );
            
            CleanupStack::PopAndDestroy( entries );
            }
        else if ( mpxCmdId ==  KVcxMessageMyVideosListComplete )
            {
            LoadMpxMediaObjectsL();
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::DoHandleSingleCollectionMessageL OUT");
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::HandleMpxItemChangedL( CMPXMessage* aMsg, TInt /*aErr*/ )
    {
    if( !iUiEngine.ContentProvider() )
        {
        return;
        }
    
    TUid colId = aMsg->ValueTObjectL<TUid>(KMPXMessageCollectionId);
    TMPXChangeEventType type = aMsg->ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType);
    
    if( colId.iUid == KVcxUidMyVideosMpxCollection )
        {
        TMPXItemId itemId = aMsg->ValueTObjectL<TMPXItemId>( KMPXMessageMediaGeneralId );
    
        if ( type == EMPXItemModified )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::HandleCollectionMessageL EMPXItemModified");
            CVcxNsContent* content = SearchFromDownloadList( itemId );
            if( content )
                {
                if ( content->GetMpxMedia() && !content->GetMpxMedia()->IsSupported( KVcxMediaMyVideosDownloadState ) )
                    {
                    // Media can be invalid, refetch
                    LoadMpxMediaObjectL( aMsg->Value<TMPXItemId>( KMPXMessageMediaGeneralId )->iId1 );
                    }
                
                if( iUiEngine.VcAppState() == EStateContentView )
                    {
			        CVcxNsContent::TVcxContentDlStatus status = content->GetDlStatus();
			        
			        if( status == CVcxNsContent::EVcxContentDlStateDownloading )
                        {
                        iDownloadUpdater->AddDownload( content );
                        }
                    else
                        {
                        iDownloadUpdater->RemoveDownload( content );
                        }
			        
			        if( status == CVcxNsContent::EVcxContentDlStateFailed && 
			            ! content->DlFailNotified() )
			            {
			            NotifyDownloadErrorL( content->GetMpxMedia(), *content );
			            }
                    }
                }
            }
        else if ( type == EMPXItemInserted )
            {
            LoadMpxMediaObjectL( aMsg->Value<TMPXItemId>( KMPXMessageMediaGeneralId )->iId1 );
            }
        else if( type == EMPXItemDeleted )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler::HandleCollectionMessageL EMPXItemDeleted");
            DeleteDownload( itemId );
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::DeleteDownload( TMPXItemId itemId )
    {            
    CVcxNsContent* content = iUiEngine.ContentProvider()->SearchContentByMpxId( itemId );
    if ( content )
        {
        iDownloadUpdater->RemoveDownload( content );
        content->SetMpxId( 0, CVcxNsContent::EVcxContentTypeVideo );
        TRAP_IGNORE( 
            content->SetMpxMediaL( NULL, CVcxNsContent::EVcxContentTypeVideo );
            RemoveFromDownloadList( content );
       
            CVcxNsContentAccess* contentAccess =
                content->GetContentAccess( CVcxNsContent::EVcxContentTypeVideo );

            if ( contentAccess )
                {
                iUiEngine.GetContentClientHandlerL()->StoreMpxIdToEcgDbL(
                            content->GetServiceId(),
                            content->GetContentId(),
                            contentAccess->iIndex,
                            0 /*mpx id*/ );
                }
            )
        iUiEngine.ContentProvider()->UpdateContent( content );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::UpdateVideoObject( TUint32 aMpxId )
    {
    if( iUiEngine.ContentProvider() )
        {    
        CVcxNsContent* content = SearchFromDownloadList( aMpxId );

        if( content )
            {
            iUiEngine.ContentProvider()->UpdateContent( content );
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::ResumeStartedFromBeginningL( TUint32 aMpxId )
    {
    TInt index = iResumeArray.Find( aMpxId );
    if ( index >= 0 )
        {
        iResumeArray.Remove( index );

        CVcxNsContent* content = SearchFromDownloadList( aMpxId );
        if ( content )
            {
            iUiEngine.GetEventRouter()->HandleDlErrorL( EIptvResumingFromBeginning, *content );
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::CopyEcgDataToMpxColL( CVcxNsContent *aContent, 
                                                             TUint32 aMpxId )
    {
    if ( !aContent->GetDescription().Length() &&
         !aContent->GetAuthor().Length() &&
         !aContent->GetCopyright().Length()  )
        {
        iUiEngine.ContentProvider()->FetchFullDetailsL( aContent );
        }
    
    CMPXMedia* msg = CMPXMedia::NewL();
    CleanupStack::PushL( msg );
    
    msg->SetTObjectValueL( KMPXMediaGeneralId, TMPXItemId( aMpxId, KMpxMediaId2 ) );
    
    //Language
    if ( aContent->GetLanguage().Length() > 0 )
        {
        msg->SetTextValueL( KVcxMediaMyVideosAudioLanguage, aContent->GetLanguage() ) ;
        }

    //Length
    if ( aContent->GetLength() )
        {
        msg->SetTObjectValueL<TReal32>( KVcxMediaMyVideosDuration, aContent->GetLength() );
        }
    
    //Description
    if ( aContent->GetDescription().Length() > 0 )
        {
        msg->SetTextValueL( KMPXMediaGeneralComment, aContent->GetDescription() );
        }

    //Author
    if ( aContent->GetAuthor().Length() > 0 )
        {
        msg->SetTextValueL( KVcxMediaMyVideosAuthor, aContent->GetAuthor() );
        }
    
    //Copyright
    if ( aContent->GetCopyright().Length() > 0 )
        {
        msg->SetTextValueL( KMPXMediaGeneralCopyright, aContent->GetCopyright() );
        }

    // Age profile
    if ( aContent->GetAgeProfile() > 0 )
        {
        msg->SetTObjectValueL<TInt32>( KVcxMediaMyVideosAgeProfile, aContent->GetAgeProfile() );
        }
    
    WriteMsgToColL( msg );

    CleanupStack::PopAndDestroy( msg );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::NotifyDownloadErrorL( 
    CMPXMedia* aMedia,
    CVcxNsContent& aContent )
    {
    if( !aMedia )
        {
        return;
        }
    
    TIptvDlError error = GetDownloadErrorL( aMedia );
    
    aContent.SetDlFailNotified( ETrue );
    
    iUiEngine.GetEventRouter()->HandleDlErrorL( error, aContent );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TIptvDlError CVcxNsMpxCollectionClientHandler::GetDownloadErrorL( CMPXMedia* aMedia )
    {
    TInt32 errorId = aMedia->ValueTObjectL<TInt32>( KVcxMediaMyVideosDownloadError );
    TInt32 globalErrorId = aMedia->ValueTObjectL<TInt32>( KVcxMediaMyVideosDownloadGlobalError );
    TIptvDlError vcxError;

    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcxNsMpxCollectionClientHandler: DL error ID: %d", errorId );
    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcxNsMpxCollectionClientHandler: DL global error ID: %d", globalErrorId );

    switch ( errorId )
        {
        case EConnectionFailed:
            vcxError = EIptvDlConnectionFailed;
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:  EIptvDlConnectionFailed");
            if ( globalErrorId == KErrServerBusy )
                {
                IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler: EIptvDlGeneral");
                vcxError = EIptvDlGeneral;
                }
            break;
        case EHttpAuthenticationFailed:
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:  EIptvDlAuthFailed");
            vcxError = EIptvDlAuthFailed;
            // Add download authentication error handling
            break; 
        case EProxyAuthenticationFailed:
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:  EIptvDlProxyAuthFailed");
            vcxError = EIptvDlProxyAuthFailed;
            break;
        case EDestFileInUse:
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:  EIptvDlDestFileInUse");
            vcxError = EIptvDlDestFileInUse;
            break;
        case EBadUrl:
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:  EIptvDlBadUrl");
            vcxError = EIptvDlBadUrl;
            break;
        case EMMCRemoved:
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:  EIptvDlMmcRemoved");
            vcxError = EIptvDlMmcRemoved;
            break;
        case EDiskFull:
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:  EIptvDlDiskFull");
            vcxError = EIptvDlDiskFull;
            break;
        case EObjectNotFound:
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:  EIptvDlContentNotFound");
            vcxError = EIptvDlContentNotFound;
            break;
        case ETransactionFailed:
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:  EIptvDlInvalidIap");
            vcxError = EIptvDlInvalidIap;
            break;
        default:
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsMpxCollectionClientHandler:  EIptvDlGeneral");
            vcxError = EIptvDlGeneral;
            break;
        }
    
    return vcxError;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::MarkVideoAsWatchedL( const TUint32 aMpxId )
    {
    TBool found( EFalse );
    
    for ( TInt i = 0; i < iLastFetchedMedias.Count(); i++ )
        {
        if ( iLastFetchedMedias[i]->IsSupported( KMPXMediaGeneralId ) &&
             iLastFetchedMedias[i]->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1 == aMpxId )
            {
            MarkItemAsWatchedL( iLastFetchedMedias[i] );
            found = ETrue;
            break;
            }
        }
    
    if ( !found )
        {
	    RArray<TUint32> mpxIds;
	    CleanupClosePushL( mpxIds );
	    mpxIds.Append( aMpxId );
	    iMarkAsWatchedId = aMpxId;
	    GetMediaByMpxIdReqL( mpxIds );
	    CleanupStack::PopAndDestroy( &mpxIds );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::SetlastPlayPosL( const TUint32 aMpxId, TInt32 aPos )
    {
    if ( CIptvUtil::LastPlaybackPositionFeatureSupported() )
        {
        WriteAttrToColL( KMPXMediaGeneralLastPlaybackPosition,
                         aPos,
                         aMpxId );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::SearchAndMarkItemAsWatchedL( CMPXMediaArray* aArray )
    {
    if( !aArray || iMarkAsWatchedId == KErrNotFound )
        {
        return;
        }

    CMPXMedia* pMedia;
            
    for( TInt i = 0; i < aArray->Count(); i++ )
        {
        pMedia = ( *aArray )[i];
                       
        TUint32 mpxid = pMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
        
        if ( iMarkAsWatchedId == mpxid )
            {
            iMarkAsWatchedId = KErrNotFound;
            MarkItemAsWatchedL( pMedia );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::MarkItemAsWatchedL( CMPXMedia* aMedia )
    {
    if( !aMedia || !aMedia->IsSupported( KMPXMediaGeneralFlags ) )
        {
        return;
        }

    TUint32 flags = aMedia->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
    
    if ( flags & EVcxMyVideosVideoNew )
        {
        flags &= ~EVcxMyVideosVideoNew;

        TUint32 mpxid = aMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
        
        TInt32 val( flags );

        WriteAttrToColL( KMPXMediaGeneralFlags, val, mpxid );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::WriteAttrToColL( TMPXAttributeData aAttribute, 
                                                        TInt32 aValue, 
                                                        TUint32 aMpxId )
    {
    CMPXMedia* msg = CMPXMedia::NewL();
    CleanupStack::PushL( msg );

    msg->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, TMPXItemId( aMpxId, KMpxMediaId2 ) );
    msg->SetTObjectValueL<TInt32>( aAttribute, aValue );

    WriteMsgToColL( msg );

    CleanupStack::PopAndDestroy( msg );
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::WriteAttrToColL( TMPXAttributeData aAttribute, 
                                                        TReal32 aValue, 
                                                        TUint32 aMpxId )
    {
    CMPXMedia* msg = CMPXMedia::NewL();
    CleanupStack::PushL( msg );

    msg->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, TMPXItemId( aMpxId, KMpxMediaId2 ) );
    msg->SetTObjectValueL<TReal32>( aAttribute, aValue );

    WriteMsgToColL( msg );

    CleanupStack::PopAndDestroy( msg );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::WriteMsgToColL( CMPXMedia* aMsg )
    {
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL( KMPXCommandGeneralId, KMPXCommandIdCollectionSet );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetCObjectValueL( KMPXCommandColSetMedia, aMsg );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, KVcxUidMyVideosMpxCollection );

    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::GetMediaDetailsL( TUint32 aMpxId )
    {
    if ( iGetMediaDetailWait->IsStarted() )
        {
        User::Leave( KErrNotReady );
        }
    
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    
    attrs.Append( KMPXMediaGeneralId );
    attrs.Append( KMPXMediaGeneralFlags );
    attrs.Append( KMPXMediaGeneralLastPlaybackPosition );

    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );

    path->AppendL( KVcxUidMyVideosMpxCollection );
    path->AppendL( KVcxMvcCategoryIdAll );
    path->AppendL( TMPXItemId( aMpxId, KMpxMediaId2 ) );
    path->SelectL( TMPXItemId( aMpxId, KMpxMediaId2 ) );

    iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
    
    CleanupStack::PopAndDestroy( path );
    
    CleanupStack::PopAndDestroy( &attrs );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::GetMediaDetailsSyncL( TUint32 aMpxId )
    {
    if ( !iGetMediaDetailWait->IsStarted() )
        {
        GetMediaDetailsL( aMpxId );
        iGetMediaDetailWait->Start();
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsMpxCollectionClientHandler::PlayVideoL( TUint32 aMpxId )
    {
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    
    path->AppendL( KVcxUidMyVideosMpxCollection );
    path->AppendL( KVcxMvcCategoryIdAll );
    path->AppendL( TMPXItemId( aMpxId, KMpxMediaId2 ) );
    path->SelectL( TMPXItemId( aMpxId, KMpxMediaId2 ) );
    
    iCollectionUtility->Collection().OpenL( *path );
    CleanupStack::PopAndDestroy(path);
    }
