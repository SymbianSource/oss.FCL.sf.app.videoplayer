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
#include <httpdownloadmgrcommon.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "vcnsscheduleddownloadmpxclient.h"
#include "CIptvVodContentContentFullDetails.h"
#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvVodDlDownloadEvent.h"
#include "CIptvVodContentClient.h"
#include "CIptvMediaContent.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxNsScheduledDownloadMpxClient::CVcxNsScheduledDownloadMpxClient( 
    MVcxNsMpxClientObserver& aObserver,
    CIptvVodContentClient& aContentClient,
    TUint32 aServiceId ) :
    iObserver( aObserver ),
    iContentClient( aContentClient ),
    iServiceId( aServiceId )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxNsScheduledDownloadMpxClient* CVcxNsScheduledDownloadMpxClient::NewL( 
                                         MVcxNsMpxClientObserver& aObserver,
                                         CIptvVodContentClient& aContentClient,
                                         TUint32 aServiceId )
    {
    CVcxNsScheduledDownloadMpxClient* self = 
      new (ELeave) CVcxNsScheduledDownloadMpxClient( aObserver, aContentClient, aServiceId );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;    
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::ConstructL()
    {
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeDefault );

    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( KVcxUidMyVideosMpxCollection );
    iCollectionUtility->Collection().OpenL( *path );
    CleanupStack::PopAndDestroy( path );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxNsScheduledDownloadMpxClient::~CVcxNsScheduledDownloadMpxClient()
    {
    if( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }

    iDownloadedContents.ResetAndDestroy();
    iDownloadedMedias.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
TIptvDlError CVcxNsScheduledDownloadMpxClient::DownloadL( TUint32 aIapId,
                                                  CIptvService* aService,
                                                  TIptvContentId aContentId,
                                                  const TDesC& aName )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DownloadL(s,c) IN");
    
    CIptvMediaContent* content = NULL;  
    RPointerArray<CIptvMediaContent> calist;
    CleanupResetAndDestroyPushL( calist );
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DownloadL: get ca IN");
    
    TInt ret = iContentClient.GetContentAccessListL( aContentId, calist );
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DownloadL get ca OUT"); 
    
    TInt i( 0 );
    
    for( i = 0; i < calist.Count() && ret == KErrNone; i++ )
        {
        TUint32 type = ( calist[i] )->iDownloadType;
        TUint32 expr = ( calist[i] )->iExpression;
        
        if( expr == CIptvMediaContent::EFull && type == EIptvDownloadTypeImmediate )   
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DownloadL ca found");
            content = calist[i];
            break;
            }
        }
    
    if( ret == KErrNone && content && content->GetMediaContentUrl().Length() > 0 )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DownloadL calling dl( ph.2 )");
        
        TPtrC url ( content->GetMediaContentUrl() );
        TPtrC name( aName );
        TPtrC usr ( aService->GetUserName() );
        TPtrC pwd ( aService->GetPassword() );
 
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DownloadL url  = %S", &url );
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DownloadL name = %S", &name );
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DownloadL usr  = %S", &usr );
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DownloadL psw  = %S", &pwd );

        ret = DoDownloadL( url,
                           name,
                           aIapId,
                           ETrue,
                           usr,
                           pwd  );
  
        if ( ret == KErrNone )
            {
            // To the download list. Content from Mpx events
            // is resolved from the list.
            AppendToDownloadList( content );

            // Dl list takes ownership of content.
            calist.Remove( i );
            }
        }

    CleanupStack::PopAndDestroy( &calist );
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DownloadL(s,c) OUT");

    return ret == KErrNone ? EIptvDlNoError : EIptvDlGeneral;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
TInt CVcxNsScheduledDownloadMpxClient::DoDownloadL( const TDesC& aAddress,
                                                    const TDesC& aName,
                                                    TUint32 aIapId,
                                                    TBool aIsReadOnlyIap,
                                                    const TDesC& aUserName,
                                                    const TDesC& aPassword )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DoDownloadL() start");
           
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd ); // 1->
    
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosStartDownload );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse /* = async */ );

    TUid colId = TUid::Uid( KVcxUidMyVideosMpxCollection );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
    
    CMPXMedia* startDownloadReq = CMPXMedia::NewL();
    CleanupStack::PushL( startDownloadReq ); // 2->
    
    TUint flags = ( aIsReadOnlyIap ? EVcxMyVideosServiceHasReadOnlyIap : 0 ) | EVcxMyVideosSilent;
    
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

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DoDownloadL() exit");
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
TInt CVcxNsScheduledDownloadMpxClient::CancelDownloadL( TUint32 aServiceId,
                                                        TUint32 aContentId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::CancelDownloadL IN");

    TInt ret( KErrNone );
    
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosCancelDownload );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse /* = async */ );
    TUid colId = TUid::Uid( KVcxUidMyVideosMpxCollection );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, colId.iUid );
    
    CMPXMedia* cancelDownloadReq = CMPXMedia::NewL();
    CleanupStack::PushL( cancelDownloadReq );
    
    CIptvMediaContent* content = SearchFromDownloadList( aServiceId, aContentId );
    
    if( content )
        {
        CMPXMedia* media = GetMpxMediaFromList( content->iFileId );
        
        if( media )
            {
            TUint32 downloadId = media->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );
            cancelDownloadReq->SetTObjectValueL( KVcxMediaMyVideosDownloadId, downloadId );
        
            TUint32 mediaId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1;
            cancelDownloadReq->SetTObjectValueL( KMPXMediaGeneralId, mediaId );
        
            cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, cancelDownloadReq );
            
            iCollectionUtility->Collection().CommandL( *cmd );
            }
        }   
    
    CleanupStack::PopAndDestroy( cancelDownloadReq );
    CleanupStack::PopAndDestroy( cmd );

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::CancelDownloadL OUT");
    
    return ret;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsScheduledDownloadMpxClient::HandleCollectionMediaL( const CMPXMedia& /*aMedia*/, 
                                                               TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::HandleCollectionMediaL");
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::HandleCollectionMessage
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::HandleCollectionMessage( CMPXMessage* aMessage,
                                                                TInt aError )
    {
    TRAPD( err, ExplodeCollectionMessagesL( aMessage, aError ) );
    
    if ( err != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::ExplodeCollectionMessagesL() leaved with error code %d", err);
        }
    }
    
// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::ExplodeCollectionMessagesL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::ExplodeCollectionMessagesL( CMPXMessage* aMessage,
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
                IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient:: ----------------------.");
                IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient:: MESSAGE ARRAY ARRIVED |");
                IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient:: ----------------------'");
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
void CVcxNsScheduledDownloadMpxClient::HandleSingleCollectionMessageL( CMPXMessage* aMsg, 
                                                                       TInt aErr )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::HandleSingleCollectionMessageL IN");

#ifdef _DEBUG
    
    TMPXMessageId msgId = aMsg->ValueTObjectL<TMPXMessageId>(KMPXMessageGeneralId);
    TUid colId = aMsg->ValueTObjectL<TUid>(KMPXMessageCollectionId);
    TMPXChangeEventType type = aMsg->ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType);
    TMPXItemId itemId = aMsg->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);
    TInt cmdId = aMsg->ValueTObjectL<TInt>( KVcxMediaMyVideosCommandId );
    
    IPTVLOGSTRING2_HIGH_LEVEL( "UI Engine ## HandleSingleCollectionMessageL() : msgId = %d", msgId );
    IPTVLOGSTRING2_HIGH_LEVEL( "UI Engine ## HandleSingleCollectionMessageL() : colId = %d", colId );
    IPTVLOGSTRING2_HIGH_LEVEL( "UI Engine ## HandleSingleCollectionMessageL() : type = %d", type );
    IPTVLOGSTRING2_HIGH_LEVEL( "UI Engine ## HandleSingleCollectionMessageL() : item Id = %d", itemId.iId1 );
    IPTVLOGSTRING2_HIGH_LEVEL( "UI Engine ## HandleSingleCollectionMessageL() : aErr = %d", aErr );
    IPTVLOGSTRING2_HIGH_LEVEL( "UI Engine ## HandleSingleCollectionMessageL() : cmd Id = %d", cmdId );

#endif // _DEBUG
    
    DoHandleSingleCollectionMessageL( aMsg, aErr );
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::HandleSingleCollectionMessage OUT");
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsScheduledDownloadMpxClient::HandleOpenL( const CMPXCollectionPlaylist&
                                                    /*aPlaylist*/, TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::HandleOpenL( playlist )");
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsScheduledDownloadMpxClient::HandleOpenL( const CMPXMedia& /*aEntries*/,
                  TInt /*aIndex*/, TBool /*aComplete*/, TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::HandleOpenL( aEntries )");
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//  
void CVcxNsScheduledDownloadMpxClient::HandleCommandComplete( CMPXCommand* aCommandResult,
                                                              TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::HandleCommandComplete IN");
    
    if( aError )
        {
        IPTVLOGSTRING2_HIGH_LEVEL( "UI Engine ## HandleCommandComplete() aError = %d ", aError);
        }
    
    if( !aCommandResult )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "CVcxNsScheduledDownloadMpxClient::HandleCommandComplete OUT, aResult == NULL");
        return;
        }
    
    if( aCommandResult->IsSupported( KVcxMediaMyVideosTransactionId ) )
        {
        TInt tid = aCommandResult->ValueTObjectL<TInt>( KVcxMediaMyVideosTransactionId );
        
        IPTVLOGSTRING2_LOW_LEVEL(
            "CVcxNsScheduledDownloadMpxClient::HandleCommandComplete , trid == %d", tid );
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::HandleCommandComplete OUT");
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::GetMediaByMpxIdReqL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::GetMediaByMpxIdReqL( RArray<TUint32>& aMpxIds )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::GetMediaByMpxIdReqL IN");
    
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
        mpxId.iId2 = 0; // Id2: 0 is a media object, 1 is a category.
        
        media->SetTObjectValueL( KMPXMessageMediaGeneralId, mpxId );
        idMediaArray->AppendL( *media );
        CleanupStack::PopAndDestroy( media );
        }
                            
    cmd->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, idMediaArray );
    
    cmd->SetTObjectValueL( KMPXMediaArrayCount, idMediaArray->Count() );

    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( idMediaArray );
    CleanupStack::PopAndDestroy( cmd ); 
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::GetMediaByMpxIdReqL OUT");
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::LinkMediaObjectsL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::LinkMediaObjectsL( CMPXMediaArray* aArray  )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::LinkMediaObjectsL IN");
    
    if ( aArray )
        {
        for ( TInt i = aArray->Count()-1; i >= 0; i-- )
            {
            SearchAndAddMpxMediaL( (*aArray)[i] );
            }
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::LinkMediaObjectsL OUT");
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::SearchAndAddMpxMediaL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::SearchAndAddMpxMediaL( CMPXMedia* aMedia )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchAndAddMpxMediaL IN");
    CIptvMediaContent* content = NULL;
    TBool newMedia( EFalse );
    TUint32 mpxId = aMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1;
    TPtrC uri = aMedia->ValueText( KVcxMediaMyVideosRemoteUrl );
    
    // search from dl first by mpx id
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchAndAddMpxMediaL: SearchFromDownloadList( mpxId )");
    
    content = SearchFromDownloadList( mpxId );
    
    if( !content )
        {
        IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchAndAddMpxMediaL: SearchFromDownloadList( uri ) ");
        // Not found from dl list, this
        // is new media. search by uri.        
        content = SearchFromDownloadList( uri );
        newMedia = ETrue;
        }
    
    if( !content )
        {
        return;
        }

    if( newMedia )
        {
        //implement better way to store this.
        content->iFileId = mpxId;
        
        iDownloadedMedias.Append( CMPXMedia::NewL( *aMedia ) );

        iContentClient.SetMpxIdL( content->iContentKey, content->iIndex, mpxId );

        CopyEcgDataToMpxColL( content, aMedia );
        }
    
    CheckMediaStateL( mpxId );
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchAndAddMpxMediaL OUT");
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::CheckMediaStateL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::CheckMediaStateL( TUint32 aMediaId )
    {
    CMPXMedia* media = GetMpxMediaFromList( aMediaId );
    
    if( !media || !media->IsSupported( KVcxMediaMyVideosDownloadState ) )
        {
        return;
        }
    
    CIptvMediaContent* content = SearchFromDownloadList( aMediaId );
    
    if( !content )
        {
        return;
        }
    
    TUint8 state = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );
    
    CIptvVodDlDownloadEvent* event = CIptvVodDlDownloadEvent::NewL();
    CleanupStack::PushL( event );
    
    event->iEvent = CIptvVodDlDownloadEvent::EDownloadState;
    event->iServiceId = iServiceId;
    event->iContentId = content->iContentKey;
    
    switch ( state )
        {
        case EVcxMyVideosDlStateDownloading:
            {
            event->iState = KIptvVodDlDownloadRunning;
            iObserver.HandleVodDlDownloadEvent( *event );
            break;
            }
        case EVcxMyVideosDlStateFailed:
            {
            event->iState = KIptvVodDlDownloadFailed;
            iObserver.HandleVodDlDownloadEvent( *event );
            RemoveFromDownloadList( content );
            break;
            }
        case EVcxMyVideosDlStatePaused:
            {
            event->iState = KIptvVodDlDownloadPaused;
            iObserver.HandleVodDlDownloadEvent( *event );
            RemoveFromDownloadList( content );
            break;
            }
        case EVcxMyVideosDlStateDownloaded:
            {
            event->iState = KIptvVodDlDownloadSucceeded;
            iObserver.HandleVodDlDownloadEvent( *event );
            RemoveFromDownloadList( content );
            break;
            }
        default:
            {
            break;
            }
        }
    
    CleanupStack::PopAndDestroy( event );
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::AppendToDownloadList
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::AppendToDownloadList( CIptvMediaContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::AppendToDownloadList");

    iDownloadedContents.Append( aContent );
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::RemoveFromDownloadList
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::RemoveFromDownloadList( CIptvMediaContent* aContent )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::RemoveFromDownloadList");
    
    TInt index = iDownloadedContents.Find( aContent );
    
    if( index >= 0 )
        {
        CIptvMediaContent* p = iDownloadedContents[ index ];
        iDownloadedContents.Remove( index );
        delete p;
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList
// -----------------------------------------------------------------------------
//
CIptvMediaContent* CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList( const TDesC& aUri )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList (aUri) IN");
    
    for( TInt i = iDownloadedContents.Count()-1; i >= 0 ; i-- )
        {
        TPtrC contentUri = TPtrC( (iDownloadedContents[i])->GetMediaContentUrl() );

        if ( contentUri.CompareF( aUri ) == 0 )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList (aUri) OUT, content found");
            return iDownloadedContents[i];
            }
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList (aUri) OUT, not found");
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList
// -----------------------------------------------------------------------------
//
CIptvMediaContent* CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList( const TUint32 aMpxId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList (aId) IN");
    
    for( TInt i = iDownloadedContents.Count()-1; i >= 0 ; i-- )
        {
        //should we store mpx ids to own array?
        TUint32 mpxId = ( iDownloadedContents[i] )->iFileId;

        if ( mpxId == aMpxId )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList (aId) OUT, found");
            return iDownloadedContents[i];
            }
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList (aId) OUT, not found");
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList
// -----------------------------------------------------------------------------
//
CIptvMediaContent* CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList( const TUint32 aServiceId, 
                                                                             const TUint32 aContentId )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList (aId) IN");
    
    for( TInt i = iDownloadedContents.Count()-1; i >= 0 ; i-- )
        {
        if ( aServiceId == iServiceId &&
             aContentId == iDownloadedContents[i]->iContentKey )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList (aId) OUT, found");
            return iDownloadedContents[i];
            }
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::SearchFromDownloadList (aId) OUT, not found");
    
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxNsScheduledDownloadMpxClient::DoHandleSingleCollectionMessageL
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::DoHandleSingleCollectionMessageL( CMPXMessage* aMsg, TInt aErr )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DoHandleSingleCollectionMessageL IN");
    
    if ( aMsg && aErr == KErrNone )
        {
        TMPXMessageId mpxMsgId = *( aMsg->Value<TMPXMessageId>( KMPXMessageGeneralId ) );
        TInt mpxCmdId = aMsg->ValueTObjectL<TInt>( KVcxMediaMyVideosCommandId );

        if ( mpxMsgId == KMPXMessageGeneral )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DoHandleSingleCollectionMessageL KMPXMessageGeneral");
            }
        else if ( mpxMsgId == KMPXMessageIdItemChanged )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DoHandleSingleCollectionMessageL: KMPXMessageIdItemChanged");
           
            HandleMpxItemChangedL( aMsg, aErr );
            }

        else if ( mpxCmdId == KVcxMessageMyVideosGetMediasByMpxIdResp )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DoHandleSingleCollectionMessageL KVcxMessageMyVideosGetMediasByMpxIdResp");
            CMPXMedia* entries = CMPXMedia::NewL( *aMsg );
            CleanupStack::PushL( entries );

            CMPXMediaArray* array = entries->Value<CMPXMediaArray>( KMPXMediaArrayContents );
            
            LinkMediaObjectsL( array );

            CleanupStack::PopAndDestroy( entries );
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::DoHandleSingleCollectionMessageL OUT");
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::HandleMpxItemChangedL( CMPXMessage* aMsg, TInt /*aErr*/ )
    {
    TUid colId = aMsg->ValueTObjectL<TUid>(KMPXMessageCollectionId);
    TMPXChangeEventType type = aMsg->ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType);
    
    if( colId.iUid == KVcxUidMyVideosMpxCollection )
        {
        TMPXItemId itemId = aMsg->ValueTObjectL<TMPXItemId>( KMPXMessageMediaGeneralId );
    
        if ( type == EMPXItemModified )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::HandleCollectionMessageL EMPXItemModified");
            
            CheckMediaStateL( itemId.iId1 );
            }
        else if ( type == EMPXItemInserted )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::HandleCollectionMessageL EMPXItemInserted");
            TMPXItemId mpxId(*( aMsg->Value<TMPXItemId>( KMPXMessageMediaGeneralId )));
            RArray<TUint32> id;
            CleanupClosePushL( id );
            id.Append( mpxId.iId1 );
            GetMediaByMpxIdReqL( id );
            CleanupStack::PopAndDestroy( &id );
            }
        else if( type == EMPXItemDeleted )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsScheduledDownloadMpxClient::HandleCollectionMessageL EMPXItemDeleted");
            
            CIptvMediaContent* content = SearchFromDownloadList( itemId.iId1 );

            if ( content )
                {
                RemoveFromDownloadList( content );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsScheduledDownloadMpxClient::CopyEcgDataToMpxColL( CIptvMediaContent *aContent, CMPXMedia *aMedia )
    {
    if( !aMedia || !aContent )
        {
        return;
        }
    
    CIptvVodContentContentFullDetails* details = CIptvVodContentContentFullDetails::NewL();
    
    CleanupStack::PushL( details );

    TInt ret = iContentClient.GetContentDetailsL( aContent->iContentKey, *details );
    
    if( ret == KErrNone )
        {
        // Language
        if ( aContent->iLanguage.Length() > 0 )
            {
            aMedia->SetTextValueL( KVcxMediaMyVideosAudioLanguage, aContent->iLanguage ) ;
            }
    
        // Length
        if ( aContent->iDuration )
            {
            aMedia->SetTObjectValueL<TReal32>( KVcxMediaMyVideosDuration, aContent->iDuration );
            }
    
        // Description
        if ( details->iDescription.Length() > 0 )
            {
            aMedia->SetTextValueL( KMPXMediaGeneralComment, details->iDescription );
            }
    
        // Author
        if ( details->iAuthor.Length() > 0 )
            {
            aMedia->SetTextValueL( KVcxMediaMyVideosAuthor, details->iAuthor );
            }
        
        // Copyright
        if ( details->iCopyright.Length() > 0 )
            {
            aMedia->SetTextValueL( KMPXMediaGeneralCopyright, details->iCopyright );
            }

        // Age profile
        if ( details->iRatingAge > 0 )
            {
            aMedia->SetTObjectValueL<TInt32>( KVcxMediaMyVideosAgeProfile, details->iRatingAge );
            }
        }

    CleanupStack::PopAndDestroy( details );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CMPXMedia* CVcxNsScheduledDownloadMpxClient::GetMpxMediaFromList( TUint32 aMediaId )
    {
    for ( TInt i = iDownloadedMedias.Count()-1; i >= 0; i-- )
        {
        TUint32 id = iDownloadedMedias[i]->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1;
        
        if( id == aMediaId )
            {
            return iDownloadedMedias[i];
            }
        }
    
    return NULL;
    }
