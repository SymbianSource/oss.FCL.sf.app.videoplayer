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
* Description:    Implementation of My Videos collection Plugin interface*
*/




// INCLUDE FILES
#include <e32cmn.h>
#include <s32mem.h>
#include <mpxlog.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmessagecontainerdefs.h>
#include <vcxmyvideosuids.h>
#include <drmutility.h>
#include <bautils.h>
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideoscollection.hrh"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosdownloadutil.h"
#include "vcxmyvideosvideocache.h"
#include "vcxmyvideoscategories.h"
#include "vcxmyvideosmessagelist.h"
#include "vcxmyvideosasyncfileoperations.h"
#include "vcxmyvideosopenhandler.h"

const TInt KMaxFileDeleteAttempts = 4;
const TInt KFileDeleteLoopDelay = 100000;

//       Add 2000 new videos to memory card. Reboot phone, mds starts harvesting,
//       open my videos -> mds server crashes and lots of events is sent to client.
//       If one waits until all are harvested before opening my videos, it works.


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCollectionPlugin* CVcxMyVideosCollectionPlugin::NewL(
    TAny* /* aInitParams */)
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::NewL");

    CVcxMyVideosCollectionPlugin* self = new (ELeave) CVcxMyVideosCollectionPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCollectionPlugin::~CVcxMyVideosCollectionPlugin()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::~CVcxMyVideosCollectionPlugin");
    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: this = %x", this);
        
    delete iMyVideosMdsDb;
    delete iDownloadUtil;
    delete iCache;
    delete iMessageList;
    delete iCategories;
    delete iAsyncFileOperations;
    delete iActiveTask;
    delete iOpenHandler;
    iFs.Close();
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCollectionPlugin::CVcxMyVideosCollectionPlugin()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::CVcxMyVideosCollectionPlugin");
    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: this = %x", this);
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::ConstructL ()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::ConstructL");
    
    User::LeaveIfError( iFs.Connect() );
        
    iMyVideosMdsDb = CVcxMyVideosMdsDb::NewL( this, iFs );    
    iActiveTask    = CVcxMyVideosActiveTask::NewL( *this );
    iCache         = CVcxMyVideosVideoCache::NewL( *this );
    iMessageList   = CVcxMyVideosMessageList::NewL( *this );
    iOpenHandler   = CVcxMyVideosOpenHandler::NewL( *this, *iCache, *iMyVideosMdsDb );
    }

// ----------------------------------------------------------------------------
// Navigates to the given path
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::OpenL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& /* aAttrs */,
    CMPXFilter* /*aFilter*/)
    {
    iOpenHandler->OpenL( aPath );
    }

// ----------------------------------------------------------------------------
// Get the extended properties of the current file (async)
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::MediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    const TArray<TCapability>& /*aCaps*/,
    CMPXAttributeSpecs* /*aSpecs*/)
    {
    MPX_FUNC("CMPXMyVideosDbPlugin::MediaL");
    MPX_DEBUG_PATH(aPath);
    
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds ); // 1->

    if ( aPath.Selection().Count() > 1 )
        {
        // it's a container if there are multiple selections, else it's not a container
        //supportedIds.AppendL(KMPXMediaIdContainer);
        //multiple selections not supported yet
        CleanupStack::PopAndDestroy( &supportedIds ); // <-1
        iObs->HandleMedia( NULL, KErrNotSupported );
        return;
        }

    RArray<TMPXItemId> ids;
    CleanupClosePushL(ids); // 2->

    aPath.SelectionL( ids );
    
    // MPX playback server asks it by path without selecting the media.
    if ( ids.Count() == 0 && aPath.Id() != KMPXInvalidItemId )
        {
        CMPXCollectionPath* path = CMPXCollectionPath::NewL( aPath );
        CleanupStack::PushL( path );
        path->SelectL( aPath.Id() );
        path->SelectionL( ids );
        CleanupStack::PopAndDestroy( path );
        }

    if ( ids.Count() == 0 )
        {
        MPX_DEBUG1("CMPXMyVideosDbPlugin:: request didn't contain any items ids, aborting");
        
        iObs->HandleMedia( NULL, KErrArgument );
        CleanupStack::PopAndDestroy( &ids );          // <-2
        CleanupStack::PopAndDestroy( &supportedIds ); // <-1
        return;
        }
        
    TBool useCachedVideo( EFalse );

    TInt pos;
    CMPXMedia* videoInCache = iCache->FindVideoByMdsIdL( ids[0].iId1, pos );
    
    if ( videoInCache )
        {
        // 0 attributes means "get all" -> can't use cache
        MPX_DEBUG2("CMPXMyVideosDbPlugin:: client is requesting %d attributes", aAttrs.Count());
        if ( aAttrs.Count() > 0 )
            {
            TBool nonSupportedAttrCanBeFoundFromMds;
            if ( TVcxMyVideosCollectionUtil::AreSupported( *videoInCache, aAttrs,
                    nonSupportedAttrCanBeFoundFromMds ) )
                {
                MPX_DEBUG1("CMPXMyVideosDbPlugin:: all attributes found from cache");
                useCachedVideo = ETrue;
                }
            else
                {
                MPX_DEBUG1("CMPXMyVideosDbPlugin:: all attributes NOT found from cache");
                if ( !nonSupportedAttrCanBeFoundFromMds )
                    {
                    MPX_DEBUG1("CMPXMyVideosDbPlugin:: none of the non cached attrs can be found from MDS -> use cached version");
                    useCachedVideo = ETrue;
                    }
                else
                    {
                    MPX_DEBUG1("CMPXMyVideosDbPlugin:: at least one of the non cached attributes can be found from MDS");
                    }
                }
            }
        }

    CMPXMedia* video;

    if ( useCachedVideo )
        {
        MPX_DEBUG1("CMPXMyVideosDbPlugin:: using cached video");
        video = CMPXMedia::CopyL( *videoInCache );
        }
    else
        {
        MPX_DEBUG1("CMPXMyVideosDbPlugin:: fetching from MDS");
        video = iMyVideosMdsDb->CreateVideoL( ids[0].iId1, ETrue /* full details */ );    
        }
        
    iObs->HandleMedia( video, KErrNone );
    
    CleanupStack::PopAndDestroy( &ids );          // <-2
    CleanupStack::PopAndDestroy( &supportedIds ); // <-1
    }

// ----------------------------------------------------------------------------
// Cancel the pending request, this is called by mpx framework when client calls
// Cancel.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::CancelRequest()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::CancelRequest");

    iActiveTask->Cancel();
    iMyVideosMdsDb->Cancel();
    }
    
// ----------------------------------------------------------------------------
// Executes the given command on the collection
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::CommandL(
    CMPXCommand& aCmd)
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::CommandL 2");

    if ( !aCmd.IsSupported( KMPXCommandGeneralId ) )
        {
        User::Leave( KErrArgument );
        }

    TMPXCommandId commandId = *aCmd.Value<TMPXCommandId>(KMPXCommandGeneralId);
     
    TBool syncOp( EFalse );
    if( aCmd.IsSupported( KMPXCommandGeneralDoSync ) )
        {
        syncOp = *aCmd.Value<TBool>( KMPXCommandGeneralDoSync );
        }

    if ( !syncOp )
        {
        // async
        iActiveTask->StartL( commandId, aCmd );        
        }
    else
        {
        // sync, operations to a single media object only
        TMPXCommandId commandId = *aCmd.Value<TMPXCommandId>(KMPXCommandGeneralId);

        switch ( commandId )
            {
            case KMPXCommandIdCollectionAdd:
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: sync KMPXCommandIdCollectionAdd arrived");
                
#ifdef _DEBUG
                CMPXMedia* video = aCmd.Value<CMPXMedia>( KMPXCommandColAddMedia );                
                TUint32 mdsId( 0 );
                iMyVideosMdsDb->AddVideoL( *video, mdsId );
#else
                User::Leave( KErrNotSupported );
#endif
                }
                break;
                
            case KMPXCommandIdCollectionSet:
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: sync KMPXCommandIdCollectionSet arrived");
                
                CMPXMedia* video = aCmd.Value<CMPXMedia>( KMPXCommandColSetMedia );
                SetVideoL( *video );
                }
                break;
            
            case KVcxCommandIdMyVideos:
                {
                switch ( aCmd.ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId ) )
                    {
                    case KVcxCommandMyVideosCancelMoveOrCopy:
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: sync KVcxCommandMyVideosCancelMoveOrCopy arrived");
                        iActiveTask->Cancel();
                        }
                        break;

                    case KVcxCommandMyVideosCancelDelete:
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: sync KVcxCommandMyVideosCancelDelete arrived");
                        iActiveTask->Cancel();
                        }
                        break;
                    }
                }
                break;
                   
            default:
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: UNKNOWN SYNC COMMAND ARRIVED");
                User::Leave( KErrNotSupported );
                }
            }
        }
            
    }

// ----------------------------------------------------------------------------
// Find the items matching the media specifications
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::FindAllL(
    const CMPXMedia& /* aCriteria */,
    const TArray<TMPXAttribute>& /* aAttrs */)
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::FindAllL");
    }

// ----------------------------------------------------------------------------
// Find the items matching the media specifications
// ----------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosCollectionPlugin::FindAllSyncL(
    const CMPXMedia& /* aCriteria */,
    const TArray<TMPXAttribute>& /* aAttrs */)
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::FindAllSyncL");
    return NULL;
    }

// ----------------------------------------------------------------------------
// Get the list of supported capabilities
// ----------------------------------------------------------------------------
//
TCollectionCapability CVcxMyVideosCollectionPlugin::GetCapabilities()
    {
    // This one supports simple search
    return EMcSearch;
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SendMessages
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SendMessages( CMPXMessage& aMessages )
    {
#if _DEBUG
    TRAP_IGNORE(

    if ( aMessages.IsSupported( KMPXMessageArrayContents ) )
        {
        const CMPXMessageArray* messageArray =
            aMessages.Value<CMPXMessageArray>(KMPXMessageArrayContents);
            
        for( TInt i = 0; i < messageArray->Count(); i++ )
            {            
            MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: sending message ID: %d in array", ++iTotalMessagesSent);
            messageArray->AtL( i )->SetTObjectValueL<TUint32>( KVcxMediaMyVideosMessageId, iTotalMessagesSent );
            iMessagesInArraySent++;
            }    

        MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: total messages sent (MSG ID): %d, messages in array sent: %d",
            iTotalMessagesSent, iMessagesInArraySent);
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: NO ARRAY IN MESSAGE!!!");
        return;
        }

    );
#endif

    iObs->HandleMessage( aMessages );
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::HandleMyVideosDbEvent
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::HandleMyVideosDbEvent(
        TMPXChangeEventType aEvent,
        RArray<TUint32>& aId )
    {
    //MPX_FUNC("CVcxMyVideosCollectionPlugin::HandleMyVideosDbEvent");
    TRAPD( err, DoHandleMyVideosDbEventL( aEvent, aId ));
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() leaved with error code: %d", err);
        }
    }
    
// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL(
        TMPXChangeEventType aEvent,
        RArray<TUint32>& aId )
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL");
    
    switch ( aEvent )
        {
        case EMPXItemDeleted:
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() --------------------------------------------.");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() Items from MDS deleted, deleting from cache |" );
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() --------------------------------------------'");
                        
            iCache->RemoveL( aId );            
            }
            break;
        
        case EMPXItemInserted:
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() ------------------------------------.");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() Items added to MDS, adding to cache |");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() ------------------------------------'");
            
            if ( iMyVideosMdsDb->iVideoListFetchingIsOngoing )
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: video list fetching is ongoing, ignoring add event");
                return;
                }
                
            TBool videoListFetchingWasCancelled = EFalse;
            
            // After the call, aId will contain only items which were actually inserted to cache.
            // We receive add events for all object types. When fetching the item from MDS we use
            // video condition and only video objects are added to cache.
            iCache->AddVideosFromMdsL( aId, videoListFetchingWasCancelled );
            if ( videoListFetchingWasCancelled )
                {
                RestartVideoListFetchingL();
                }
             
            SyncWithDownloadsL( aId );
            }
            break;
        
        case EMPXItemModified:
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() --------------------------------------.");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() Items modified in MDS, updating cache |");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() --------------------------------------'");
            CMPXMedia* video;
            for ( TInt i = 0; i < aId.Count(); i++ )
                {
                video = iMyVideosMdsDb->CreateVideoL( aId[i], ETrue /* full details */ );
                
                if ( video )
                    {
                    CleanupStack::PushL( video ); // 1->
                    iCache->UpdateVideoL( *video );
                    CleanupStack::PopAndDestroy( video ); // <-1
                    }
                else
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: couldn't find the modified item from MDS");
                    }
                }
            }
            SyncWithDownloadsL( aId );
            break;
        }
        
    TInt pos;
    for ( TInt i = 0; i < aId.Count(); i++ )
        {
        if ( aEvent == EMPXItemInserted )
            {
            // add item from cache to the message if we have it.
            CMPXMedia* video = iCache->FindVideoByMdsIdL( aId[i], pos );
            TRAP_IGNORE( iMessageList->AddEventL( TMPXItemId( aId[i], 0), aEvent, 0, video ) );
            }
        else
            {
            TRAP_IGNORE( iMessageList->AddEventL( TMPXItemId( aId[i], 0), aEvent ) );
            }
        }
    
    iMessageList->SendL();
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::HandleStepL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosCollectionPlugin::HandleStepL()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::HandleStepL");

    TBool done(ETrue);

    switch ( iActiveTask->GetTask() )
        {
        case KMPXCommandIdCollectionSet:
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: async KMPXCommandIdCollectionSet in");
            
            CMPXMedia* video = CMPXMedia::NewL( *(iActiveTask->GetCommand().Value<CMPXMedia>(
                            KMPXCommandColSetMedia)) );
            
            CleanupStack::PushL( video );
            
            SetVideoL( *video );

            CleanupStack::PopAndDestroy( video );
            
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: async KMPXCommandIdCollectionSet out");
            
            done = ETrue;
            
            break;
            }
        case KVcxCommandIdMyVideos:
            {
            CMPXCommand& cmd = iActiveTask->GetCommand();
            if ( !cmd.IsSupported( KVcxMediaMyVideosCommandId ) )
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxMediaMyVideosCommandId attribute not supported in cmd, aborting");
                User::Leave( KErrArgument );
                }

            TInt myVideosCmd( cmd.ValueTObjectL<TUint>( KVcxMediaMyVideosCommandId ) );

            switch ( myVideosCmd )
                {
                case KVcxCommandMyVideosStartDownload:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: Handling KVcxCommandMyVideosStartDownload command.");
                    
                    CMPXMedia* video = CMPXMedia::NewL( *(iActiveTask->GetCommand().Value<CMPXMedia>(
                            KMPXCommandColAddMedia)) );
                    CleanupStack::PushL( video ); // 1->

                    if ( !iCache->iVideoList )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: iVideoListCache = NULL -> creating new empty iVideoListCache");
                        iCache->iVideoListIsPartial = ETrue;
                        iCache->iVideoList          = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
                        }

                    TBool resume = EFalse;
                    
                    if ( video->IsSupported( KVcxMediaMyVideosDownloadId ) )
                        {
                        TUint32 downloadId = video->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );
                        if ( downloadId != 0 )
                            {
                            MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: download id %d given by the client -> this is download resume",
                                    downloadId);
                            resume = ETrue;
                            
                            // load the existing item to cache if its not there already
                            }
                        }
                    
                    if ( !resume )
                        {    
                        if ( !video->IsSupported( KVcxMediaMyVideosRemoteUrl ) )
                            {
                            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxMediaMyVideosRemoteUrl not supported -> leaving with KErrArgument");
                            User::Leave( KErrArgument );
                            }

                        if ( video->ValueText( KVcxMediaMyVideosRemoteUrl ).Length() >
                                KVcxMvcMaxUrlLength )
                            {
                            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: url longer than 1024 -> leaving with KErrArgument");
                            User::Leave( KErrArgument );
                            }
                        
                        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: starting download for url: %S", 
                            &video->ValueText( KVcxMediaMyVideosRemoteUrl ) );

                        video->SetTObjectValueL<TUint8>( KVcxMediaMyVideosOrigin, EVcxMyVideosOriginDownloaded );    
                        video->SetTObjectValueL<TUint8>( KVcxMediaMyVideosDownloadState,
                                static_cast<TUint8>(EVcxMyVideosDlStateDownloading) );
                        video->SetTObjectValueL<TUint32>( KMPXMediaGeneralFlags, EVcxMyVideosVideoNew );
                            
                        HBufC* fileName = DownloadUtilL().CreateFilePathL( *video );
                        CleanupStack::PushL( fileName ); // 2->
                        video->SetTextValueL( KMPXMediaGeneralUri, *fileName );
                        CleanupStack::PopAndDestroy( fileName ); // <-2
                        }

                    TRAPD( err, DownloadUtilL().StartDownloadL( *video ) ); //download id is written to video object
                    
                    if ( err != KErrNone )
                        {
                        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: StartDownloadL left: %d", err);
                        User::Leave( err );
                        }
                    
                    if ( !resume )
                        {    
                        TUint32 newDownloadId = video->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId ); 
                        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: new download ID: %d",
                            newDownloadId);

                        AddVideoToMdsAndCacheL( *video );
                        }
                    else
                        {
                        // clear old error codes from the dl item
                        TInt pos;
                        
                        CMPXMedia* videoInCache = iCache->FindVideoByMdsIdL(
                                TVcxMyVideosCollectionUtil::IdL( *video ), pos );
                        if ( videoInCache )
                            {
                            videoInCache->SetTObjectValueL<TInt>( KVcxMediaMyVideosDownloadError, 0 );
                            videoInCache->SetTObjectValueL<TInt>( KVcxMediaMyVideosDownloadGlobalError, 0 );
                            }
                        }
                        
                    CleanupStack::PopAndDestroy( video ); // <-1
                    done = ETrue;
                    }
                    break;
                    
                case KVcxCommandMyVideosCancelDownload:
                    {
                    // Error code is returned to client if dl item was left to system.
                    // If file delete fails, then mds item is also left to system.
                    
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: Handling KVcxCommandMyVideosCancelDownload command.");
                    
                    CMPXMedia* video = CMPXMedia::NewL( *(iActiveTask->GetCommand().Value<CMPXMedia>(
                            KMPXCommandColAddMedia)) );
                    CleanupStack::PushL( video ); // 1->

                    if ( !video->IsSupported( KVcxMediaMyVideosDownloadId ) ||
                            !video->IsSupported( KMPXMediaGeneralId ) ||
                            !video->IsSupported( KMPXMediaGeneralUri ) )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: parameter missing, can't cancel dl, leaving with KErrArgument");
                        User::Leave( KErrArgument );
                        }

                    TUint32 downloadId = video->ValueTObjectL<TUint32>(
                            KVcxMediaMyVideosDownloadId );
                    DownloadUtilL().CancelDownload( downloadId, ETrue /* remove file */ );
 
                    if ( BaflUtils::FileExists( iFs, video->ValueText( KMPXMediaGeneralUri ) ) )
                        {
                        TMPXItemId mpxItemId = video->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
                        TInt err( KErrNone );
                        
                        for ( TInt i = 0; i < KMaxFileDeleteAttempts; i++ )
                            {
                            TRAP( err, AsyncFileOperationsL().DeleteVideoL( mpxItemId.iId1, ETrue ) );
                                        
                            if ( err == KErrInUse )
                                {
                                MPX_DEBUG1( "CVcxMyVideosCollectionPlugin:: file is already in use, waiting a moment and try again");
                                User::After( KFileDeleteLoopDelay );
                                }
                            else
                                {
                                break;
                                }
                            }
                        
                        if ( err != KErrNone && err != KErrNotFound )
                            {
#ifdef _DEBUG                        
                            if ( err == KErrInUse )
                                {
                                TVcxMyVideosCollectionUtil::PrintOpenFileHandlesL(
                                        video->ValueText( KMPXMediaGeneralUri ), iFs );
                                }
#endif
							
                            // Some error occured when cancelling download operation, dl item is however gone and file is left
                            // -> change dl id to 0 and leave mpx collection item there. Report operation to client as a success.
                            MPX_DEBUG1( "CVcxMyVideosCollectionPlugin:: dl item is gone from dl manager, file and mpx item are left, setting dl id to 0");
                            TRAP_IGNORE( SetDownloadIdToZeroL( downloadId ) );
                            }
                        }
                    
                    CleanupStack::PopAndDestroy( video ); // <-1
                    done = ETrue;
                    }
                    break;
                                        
                case KVcxCommandMyVideosPauseDownload:
                    {
                    CMPXMedia& cmd = iActiveTask->GetCommand();
                    if ( !cmd.IsSupported( KVcxMediaMyVideosDownloadId ) )
                        {
                        User::Leave( KErrArgument );
                        }
                    else
                        {
                        TInt err = DownloadUtilL().PauseDownload(
                                cmd.ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId ) );
                        User::LeaveIfError( err );
                        }
                    done = ETrue;
                    }
                    break;
                
                case KVcxCommandMyVideosGetMediaFullDetailsByMpxId:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosGetMediaFullDetailsByMpxId received");
                    CMPXMedia& cmd = iActiveTask->GetCommand();
                    TMPXItemId mpxId( TVcxMyVideosCollectionUtil::IdL( cmd ) );
                    if ( !mpxId.iId1 && !mpxId.iId2 )
                        {
                        User::Leave( KErrArgument );
                        }
                    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosGetMediaFullDetailsByMpxId cmd: MDS ID %d requested", mpxId.iId1 );

                    CMPXMedia* video = iMyVideosMdsDb->CreateVideoL(
                            mpxId.iId1, ETrue /* full details */ );

                    if ( !video )
                        {
                        User::Leave( KErrGeneral );
                        }
                    
                    CleanupStack::PushL( video ); // 1->
                    
                    TBool eventsAdded;
                    SyncVideoWithDownloadsL( *video, eventsAdded,
                            EFalse /* dont add event to iMessageList */ );
                    
                    cmd.SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, video );
                    CleanupStack::PopAndDestroy( video ); // <-1

                    cmd.SetTObjectValueL<TUid>(KMPXMessageCollectionId, TUid::Uid(
                            KVcxUidMyVideosMpxCollection));
                    
                    done = ETrue;
                    }
                    break;
                    
                case KVcxCommandMyVideosGetMediasByMpxId:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosGetMediasByMpxId received");
                    
                    // Get ids from the request
                    CMPXMedia& cmd = iActiveTask->GetCommand();
                    if ( !cmd.IsSupported( KMPXMediaArrayContents ) )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: error, no array defined");
                        User::Leave( KErrArgument );
                        }
                    
                    CMPXMediaArray* idMediaArray = cmd.Value<CMPXMediaArray>(
                            KMPXMediaArrayContents );

                    if ( idMediaArray->Count() == 0 )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: error, no items in array ");
                        User::Leave( KErrArgument );
                        }

                    TMPXItemId mpxId;
                    RArray<TUint32> mdsIds;
                    RArray<TUint32> mdsIds2;
                    mdsIds.Reset();
                    CleanupClosePushL( mdsIds );  // 1->
                    mdsIds2.Reset();
                    CleanupClosePushL( mdsIds2 ); // 2->

                    for ( TInt i = 0; i < idMediaArray->Count(); i++ )
                        {
                        mpxId = (*idMediaArray)[i]->ValueTObjectL<TMPXItemId>(
                                KMPXMessageMediaGeneralId );
                        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosGetMediasByMpxId cmd: MDS ID %d requested",
                               mpxId.iId1 );
                        mdsIds.AppendL( mpxId.iId1 );
                        mdsIds2.AppendL( mpxId.iId1 );
                        }

                    if ( !iCache->iVideoList )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: iVideoListCache = NULL -> creating new empty iCache->iVideoList");
                        iCache->iVideoListIsPartial = ETrue;
                        iCache->iVideoList = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
                        }

                    TBool videoListFetchingWasCancelled = EFalse;
                        
                    if ( iCache->iVideoListIsPartial )
                        {                            
                        // Load items to cache
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: loading requested items to iCache->iVideoList");

                        iCache->AddVideosFromMdsL( mdsIds, videoListFetchingWasCancelled );
                        if ( mdsIds.Count() > 0 )
                            {
                            SyncWithDownloadsL( mdsIds );
                            }
                        }
                    else
                        {
                        // iCache->iVideoList contains all 
                        }

                    CMPXMessage* message = iCache->GetVideosL( mdsIds2 );
                    CleanupStack::PushL( message ); // 3-> 

                    // Set message attributes
                    //
                    TMPXItemId itemId;
                    itemId.iId1 = KVcxCommandIdMyVideos;
                    message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, itemId );
                    message->SetTObjectValueL<TInt>( KVcxMediaMyVideosCommandId,
                            KVcxMessageMyVideosGetMediasByMpxIdResp );
                    
                    SetTransactionIdL( cmd, *message );
                            
                    iMessageList->AddL( message );
                    iMessageList->SendL();

                    if ( videoListFetchingWasCancelled )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: video list fetching was cancelled, restarting");
                        RestartVideoListFetchingL();
                        }

                    CleanupStack::Pop( message );            //  <-3
                    CleanupStack::PopAndDestroy( &mdsIds2 ); //  <-2
                    CleanupStack::PopAndDestroy( &mdsIds );  //  <-1
                        
                    done = ETrue;
                    }
                    break;
                
                case KVcxCommandMyVideosCopy:
                case KVcxCommandMyVideosMove:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosMove or Copy step");
                    done = AsyncFileOperationsL().HandleMoveOrCopyStepL();
                    }
                    break;
                    
                case KVcxCommandMyVideosDelete:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosDelete step");
                    done = AsyncFileOperationsL().HandleDeleteStepL();
                    }
                    break;
                }
            }
            break;

            
        case KMPXCommandIdCollectionRemoveMedia:
            {
            TMPXItemId mpxId = iActiveTask->GetCommand().ValueTObjectL<TMPXItemId>(
                    KMPXMediaGeneralId );
            AsyncFileOperationsL().DeleteVideoL( mpxId.iId1 );
            done = ETrue;
            break;
            }
            
        default:
            {
            // Should never happen!
            ASSERT(0);
            break;
            }
        }
    return done;
    }


// ----------------------------------------------------------------------------
// Handler for async operations completed
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::HandleOperationCompleted(
    TInt aErr )
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::HandleOperationCompleted");
    
    if ( aErr != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: Leave or cancel happened during the operation: %d", aErr);
        TRAPD( err, AsyncFileOperationsL().CancelOperationL( aErr ) ); // generates resp message for move,copy or delete operations
        if ( err != KErrNone )
            {
            MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: failed to generate resp msg: %d", err);
            }
        }

    

    CMPXCommand& cmd = iActiveTask->GetCommand();
    TRAP_IGNORE( cmd.SetTObjectValueL<TInt32>( KVcxMediaMyVideosInt32Value, aErr ) );

    iObs->HandleCommandComplete( &cmd, KErrNone );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::HandleDlEvent
// From CVcxMyVideosDownloadUtilObserver
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::HandleDlEvent( TVcxMyVideosDownloadState aState,
                TUint32 aDownloadId,
                TInt aProgress,
                TInt64 aDownloaded,
                TInt32 aError,
                TInt32 aGlobalError )
    {
    TRAPD( err, DoHandleDlEventL( aState, aDownloadId, aProgress,
            aDownloaded, aError, aGlobalError ) );
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: DoHandleDlEventL left with error code: %d", err);
        }
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::DoHandleDlEventL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::DoHandleDlEventL( TVcxMyVideosDownloadState aState,
                TUint32 aDownloadId,
                TInt aProgress,
                TInt64 aDownloaded,
                TInt32 aError,
                TInt32 aGlobalError )
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::DoHandleDlEventL");
    
    CMPXMedia* video = iCache->FindVideoByDownloadIdL( aDownloadId );

    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: dl event for download ID %d, pointer = %x) arrived.", aDownloadId, video);
    
    TBool sendEvent = EFalse;
    if ( video )
        {
        TMPXItemId mpxId( TVcxMyVideosCollectionUtil::IdL( *video ) );
        MPX_DEBUG4("CVcxMyVideosCollectionPlugin:: MPX item (MDS ID %d) (DL ID %d) %S",
                mpxId.iId1, aDownloadId, &TVcxMyVideosCollectionUtil::Title( *video ) );
                
        TUint8 currentState = TVcxMyVideosCollectionUtil::DownloadStateL( *video );
        
        if ( currentState == EVcxMyVideosDlStateDownloaded )
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: already in Downloaded state, discarding event");
            return;
            }
            
        if ( currentState != aState )
            {
            MPX_DEBUG5("CVcxMyVideosCollectionPlugin:: updating (mds id: %d) (dl id: %d) state: %S -> %S",
                        mpxId.iId1, aDownloadId, &DownloadState( currentState ), &DownloadState( aState ) );
            video->SetTObjectValueL<TUint8>( KVcxMediaMyVideosDownloadState, static_cast<TUint8>(aState) );
            sendEvent = ETrue;

            if ( aState == EVcxMyVideosDlStateDownloaded )
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: downloaded state received -> setting download id to 0");
                
                //1. set download id to 0
                video->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, 0 );
                
                //2. update drm flag
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                RFile64 dlFile;
#else
                RFile dlFile;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                TInt err = dlFile.Open( iFs, video->ValueText( KMPXMediaGeneralUri ), EFileRead );
                if ( err == KErrNone )
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: file opened ok for drm reading");
                    CleanupClosePushL( dlFile ); // 1->
                    DRM::CDrmUtility* drmUtil = DRM::CDrmUtility::NewLC(); // 2->
                    if ( drmUtil->IsProtectedL( dlFile ) )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: file is DRM protected, setting the property flag");
                        TUint32 flags = video->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
                        flags |= EVcxMyVideosVideoDrmProtected;
                        video->SetTObjectValueL<TUint32>( KMPXMediaGeneralFlags, flags );
                        }
                    else
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: file is not DRM protected");
                        }
                    CleanupStack::PopAndDestroy( drmUtil ); // <-2
                    CleanupStack::PopAndDestroy( &dlFile ); // <-1
                    }
                else
                    {
                    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: file didnt open for drm reading, %d", err);
                    }
                NotifyDownloadCompletedL( *video );
                    
                //3. Update file size using iCache->UpdateVideoL function since it changes item position and
                //   sends category modified events if necessarry.
                CMPXMedia* updateObject = CMPXMedia::NewL();
                CleanupStack::PushL( updateObject ); // 1->
                updateObject->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxId );
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                updateObject->SetTObjectValueL<TInt64>( KMPXMediaGeneralExtSizeInt64,
                        static_cast<TInt64>( aDownloaded ) );
                // set current value to 0 to force event sending and video list position updating    
                video->SetTObjectValueL<TInt64>( KMPXMediaGeneralExtSizeInt64, 0 );                
#else
                updateObject->SetTObjectValueL<TInt>( KMPXMediaGeneralSize,
                        static_cast<TInt>( aDownloaded ) );
                // set current value to 0 to force event sending and video list position updating    
                video->SetTObjectValueL<TInt>( KMPXMediaGeneralSize, 0 );                
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
                iCache->UpdateVideoL( *updateObject );
                CleanupStack::PopAndDestroy( updateObject ); // <-1                
                // find video again since it might have been deleted in iCache->UpdateVideoL
                TInt pos;
                video = iCache->FindVideoByMdsIdL( mpxId.iId1, pos );

                //file size and download id are saved to database
                iMyVideosMdsDb->UpdateVideoL( *video );
                sendEvent = EFalse; // MDS will send the event, this avoids duplicate
                }
                
            if ( aState == EVcxMyVideosDlStateFailed )
                {
                video->SetTObjectValueL<TInt32>( KVcxMediaMyVideosDownloadError, aError );
                video->SetTObjectValueL<TInt32>( KVcxMediaMyVideosDownloadGlobalError,
                        aGlobalError );
                }            
            }
        else
            {
            MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: state was already same(%S), skipping state update.", &DownloadState( currentState ));
            }
 
        TInt8 currentProgress = video->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress );
        if ( currentProgress != aProgress )
            {
            MPX_DEBUG4("CVcxMyVideosCollectionPlugin:: (dl id: %d) progress: %d -> %d",
                        aDownloadId, currentProgress, aProgress );

            video->SetTObjectValueL<TInt8>( KVcxMediaMyVideosDownloadProgress,
                    static_cast<TInt8>( aProgress ) );
            // Don't send the update event for progress.
            //sendEvent = ETrue;
            }
        else
            {
            MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: progress was already same(%d), skipping progress update.", currentProgress);
            }
            
        TInt64 currentFileSize = 0;
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        if ( video->IsSupported( KMPXMediaGeneralExtSizeInt64 ) )
            {
            currentFileSize = video->ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 );
            }
#else
        if ( video->IsSupported( KMPXMediaGeneralSize ) )
            {
            currentFileSize = video->ValueTObjectL<TInt>( KMPXMediaGeneralSize );
            }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        if ( currentFileSize != aDownloaded )
            {
            MPX_DEBUG4("CVcxMyVideosCollectionPlugin:: updating (dl id: %d) size: %ld -> %ld",
                        aDownloadId, currentFileSize, aDownloaded );
                        
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
            video->SetTObjectValueL<TInt64>( KMPXMediaGeneralExtSizeInt64, aDownloaded );
#else
            TInt newFileSize( aDownloaded );
            video->SetTObjectValueL<TInt>( KMPXMediaGeneralSize, newFileSize );
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
            //sendEvent = ETrue;
            }
        
        if ( sendEvent )
            {
            iMessageList->AddEventL( mpxId, EMPXItemModified );
            iMessageList->SendL();
            }
        }
    else
        {
        if ( (aState != EVcxMyVideosDlStateDownloaded) && (aProgress < 100) &&
                !iCache->iVideoListIsPartial )
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: -----------------------------------------------------------------------.");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: Event for progressing download arrived, but the MPX/MDS item not found!|");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: -> deleting download.                                                  |");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: -----------------------------------------------------------------------'");
            
            RHttpDownload* download = DownloadUtilL().Download( aDownloadId );
            if ( download )
                {
                MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: Download ID (%d) not found from MPX/MDS, deleting download!",
                        aDownloadId );                
                DownloadUtilL().DeleteDownloadAsync( aDownloadId, ETrue );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SyncWithDownloadsL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SyncWithDownloadsL(
        RArray<TUint32>& aItemsInCache )
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::SyncWithDownloadsL()");
    
    TBool eventsAdded = EFalse;
    for ( TInt i = 0; i < aItemsInCache.Count(); i++ )
        {
        TInt pos;
        CMPXMedia* video = iCache->FindVideoByMdsIdL( aItemsInCache[i], pos );
        if ( video )
            {
            SyncVideoWithDownloadsL( *video, eventsAdded );
            }
        }
    if ( eventsAdded )
        {
        iMessageList->SendL();
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SyncVideoWithDownloadsL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SyncVideoWithDownloadsL( CMPXMedia& aVideo,
        TBool& aEventAdded, TBool aAddEvent )
    {    
    TInt downloadId( TVcxMyVideosCollectionUtil::DownloadIdL( aVideo ) );
    
    if ( downloadId )
        {
        RHttpDownload* download( DownloadUtilL().Download( downloadId ) );

        if ( download )
            {
            MPX_DEBUG2("CVcxMyVideosCollectionPlugin::SyncVideoWithDownloadsL() item (DL ID: %d) found from dl manager", downloadId);
    
            TBool modified = EFalse;
            SyncVideoAndDownloadL( aVideo, *download, modified );
            if ( modified && aAddEvent )
                {
                iMessageList->AddEventL( TVcxMyVideosCollectionUtil::IdL( aVideo ),
                        EMPXItemModified );
                aEventAdded = ETrue;
                }
            }
        else
            {
            //download id != 0 and it is not found from download manager -> we set download id to 0
            MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: download id %d != 0 and no corresponding download found from Download Manager",
                    downloadId);
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: -> setting download id to 0");
            aVideo.SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, 0 );
            iMyVideosMdsDb->UpdateVideoL( aVideo ); // if video list fetching is ongoing, this will leave with KErrNotReady
            }
        }
    }
 
// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SyncVideoAndDownloadL
// ----------------------------------------------------------------------------
//
void  CVcxMyVideosCollectionPlugin::SyncVideoAndDownloadL(
        CMPXMedia& aVideo,
        RHttpDownload& aDownload,
        TBool& aModified )
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::SyncVideoAndDownloadL()");
        
    aModified = EFalse;
         
    TBuf<KMaxUrlLength> downloadUrl;
    aDownload.GetStringAttribute( EDlAttrReqUrl, downloadUrl );
    if ( aVideo.ValueText( KVcxMediaMyVideosRemoteUrl ) 
            != downloadUrl )
        {
        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: urls in MPX and DL Manager differ!");
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: remote url in MPX: %S",
                &(aVideo.ValueText( KVcxMediaMyVideosRemoteUrl )));
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: remote url in DL Manager: %S",
                &downloadUrl);
        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: -> updating MPX cache");
        
        aVideo.SetTextValueL( KVcxMediaMyVideosRemoteUrl, downloadUrl );
        aModified = ETrue;
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: remote urls already same in dl manager and mpx");
        }

    // KVcxMediaMyVideosDownloadState
    TVcxMyVideosDownloadState dlStateInDlManager;
    DownloadUtilL().GetDownloadState( aDownload, dlStateInDlManager );
    
    TUint8 dlStateInMpxCache; 
    if ( aVideo.IsSupported( KVcxMediaMyVideosDownloadState ))
        {
        dlStateInMpxCache = aVideo.ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );
        }
    else
        {
        dlStateInMpxCache = static_cast<TUint8>( EVcxMyVideosDlStateNone );
        }

    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: dl state in dl manager: %S", &DownloadState( dlStateInDlManager ));
    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: dl state in mpx cache: %S", &DownloadState( dlStateInMpxCache ));
    
    if ( static_cast<TUint8>( dlStateInDlManager ) != dlStateInMpxCache )
        {        
        if ( dlStateInDlManager == EVcxMyVideosDlStateDownloaded )
            {
            // Download finished event has arrived when we weren't around, call event handler to get things right.
            // Collection is updated and download is deleted from Download Manager.
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: Download is in Finished state and collection has download id != 0");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: -> we have missed download finished event, lets generate it by ourselves.");
            
            TUint64 downloaded( 0 );
            TUint8 progress( DownloadUtilL().DownloadProgress( aDownload, downloaded, EFalse ) );            
            TUint32 downloadId( aVideo.ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId ) );
            HandleDlEvent( dlStateInDlManager, downloadId,
                    progress, downloaded, KErrNone, KErrNone );
            DownloadUtilL().DeleteDownloadAsync( downloadId, EFalse /* don't delete content */ );
            }
        else
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: dl state in dl manager differs of mpx cache-> updating mpx cache");
            aVideo.SetTObjectValueL<TUint8>( KVcxMediaMyVideosDownloadState,
                    static_cast<TUint8>( dlStateInDlManager ) );
            aModified = ETrue;
            }
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: download state already same in dl manager and mds");
        }
    
    // KVcxMediaMyVideosDownloadProgress
    TUint64 downloaded = 0;
    TInt8 dlProgressInDlManager = DownloadUtilL().DownloadProgress(
            aDownload, downloaded, EFalse );
    
    TInt8 dlProgressInMpxCache;
    if ( aVideo.IsSupported( KVcxMediaMyVideosDownloadProgress ) )
        {
        dlProgressInMpxCache = aVideo.ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress );
        }
    else
        {
        aVideo.SetTObjectValueL<TInt8>( KVcxMediaMyVideosDownloadProgress, 0 );
        dlProgressInMpxCache = 0;
        }

    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: dl progress in dl manager: %d", dlProgressInDlManager);
    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: dl progress in mpx cache: %d", dlProgressInMpxCache);

    if ( dlProgressInDlManager != dlProgressInMpxCache )
        {
        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: dl progress in dl manager differs of mpx cache-> updating mpx cache");
        aVideo.SetTObjectValueL<TInt8>( KVcxMediaMyVideosDownloadProgress,
                static_cast<TInt8>( dlProgressInDlManager ) );
        aModified = ETrue;
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: download progress already same in dl manager and mds");
        }        
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SyncVideoListWithDownloadsL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SyncVideoListWithDownloadsL( CMPXMedia& aVideoList,
        TBool aSendEvents, TInt aStartPos )
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::SyncVideoListWithDownloadsL");

    CMPXMediaArray* videoArray = aVideoList.Value<CMPXMediaArray>(
                                KMPXMediaArrayContents);    

    CMPXMedia* video;
    
    TBool eventsAdded = EFalse;
    for ( TInt i = aStartPos; i < videoArray->Count(); i++ )
        {
        video = (*videoArray)[i];        
        SyncVideoWithDownloadsL( *video, eventsAdded, aSendEvents );        
        }
    if ( eventsAdded )
        {
        iMessageList->SendL();
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::DownloadUtil
// ----------------------------------------------------------------------------
//
CVcxMyVideosDownloadUtil& CVcxMyVideosCollectionPlugin::DownloadUtilL()
    {
    if ( !iDownloadUtil )
        {
        iDownloadUtil = CVcxMyVideosDownloadUtil::NewL( *this, iFs );        
        }
    
    if ( !iOrphanDownloadsCleared )
        {
        if ( !iCache->iVideoListIsPartial )
            {
            iOrphanDownloadsCleared = ETrue;
            iDownloadUtil->ClearOrphanDownloadsL( *iCache->iVideoList );
            }
        }
    
    return *iDownloadUtil;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::CategoriesL
// ----------------------------------------------------------------------------
//
CVcxMyVideosCategories& CVcxMyVideosCollectionPlugin::CategoriesL()
    {
    if ( !iCategories )
        {
        iCategories = CVcxMyVideosCategories::NewL( *this );
        }
    return *iCategories;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::HandleCreateVideoListResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::HandleCreateVideoListResp(
        CMPXMedia* aVideoList, TInt aNewItemsStartIndex, TBool aComplete )
    {
    TRAPD( err, iOpenHandler->DoHandleCreateVideoListRespL( aVideoList, aNewItemsStartIndex, aComplete ));
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: DoHandleCreateVideoListRespL() left with error code: %d", err );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::RestartVideoListFetchingL
// Called when video list fetching is interrupted due to "get item by mpx id"
// request, or sorting order change. Or by some other reason that requires
// new video list fetching. iCache->iVideoList media array has been reset, Ie lists are the
// the same but items have been deleted.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::RestartVideoListFetchingL()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::RestartVideoListFetchingL()");
    
    // Client already had something on the list.
    // -> tell client to fetch everything from scratch again.
    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: open was not pending, client had something on the list");
    MPX_DEBUG1("                               -> telling client to fetch everything from the scratch again");
    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: adding modify event for path root, extra info = %d",
            EVcxMyVideosVideoListOrderChanged );
    
    iCache->ResetVideoListL();
    
    iMessageList->AddEventL( TMPXItemId( KVcxUidMyVideosMpxCollection,
            KVcxUidMyVideosMpxCollection ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );
    iMessageList->SendL();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SetVideoL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SetVideoL( CMPXMedia& aVideo )
    {
    TRAPD( leave, iCache->UpdateVideoL( aVideo ) );
	
    if ( leave == KErrNone )
        {
        TMPXItemId mpxId( TVcxMyVideosCollectionUtil::IdL( aVideo ) );

        TInt pos;
        CMPXMedia* videoInCache = iCache->FindVideoByMdsIdL( mpxId, pos );
		
        if ( videoInCache )
            {
            iMyVideosMdsDb->UpdateVideoL( *videoInCache );
            }
        }
    else if ( leave == KErrNotFound )
        {
        iMyVideosMdsDb->UpdateVideoL( aVideo );
        }
	else
	    {
		User::Leave( leave );
		}
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::NotifyDownloadCompletedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::NotifyDownloadCompletedL( CMPXMedia& aVideo )
    {
    const TInt KMediaExtSize( 8 );
    
    HBufC8* buffer = HBufC8::NewL( KMediaExtSize );
    CleanupStack::PushL( buffer );
    TPtr8 des = buffer->Des();
    
    RDesWriteStream stream;
    CleanupClosePushL( stream );
    stream.Open( des );
    
    aVideo.ExternalizeL( stream );
    
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream );    
    
    DownloadUtilL().NotifyDownloadCompleted( *buffer );
    
    CleanupStack::PopAndDestroy( buffer );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::NotifyNewVideosCountDecreasedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::NotifyNewVideosCountDecreasedL( CMPXMedia& aVideo )
    {    
    const TInt KMediaExtSize( 8 );
    
    HBufC8* buffer = HBufC8::NewL( KMediaExtSize );
    CleanupStack::PushL( buffer );
    TPtr8 des = buffer->Des();
    
    RDesWriteStream stream;
    CleanupClosePushL( stream );
    stream.Open( des );
    
    aVideo.ExternalizeL( stream );
    
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream );    
    
    DownloadUtilL().NotifyNewVideosCountDecreased( *buffer );
    
    CleanupStack::PopAndDestroy( buffer );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::AddVideoToMdsAndCacheL
// ----------------------------------------------------------------------------
//    
void CVcxMyVideosCollectionPlugin::AddVideoToMdsAndCacheL( CMPXMedia& aVideo )
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::AddVideoToMdsAndCacheL");
    
    TMPXItemId mpxId;
    iMyVideosMdsDb->AddVideoL( aVideo, mpxId.iId1 );

    aVideo.SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxId );                        
    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: new MDS ID: %d", mpxId.iId1 );

    RArray<TUint32> ids;
    ids.Reset();
    CleanupClosePushL( ids ); // 1->
    ids.AppendL( mpxId.iId1 );
    HandleMyVideosDbEvent( EMPXItemInserted, ids ); //this will fetch from mds to cache and sync with downloads
    CleanupStack::PopAndDestroy( &ids ); // <-1
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SetTransactionIdL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SetTransactionIdL( CMPXMedia& aRequest, CMPXMedia& aResp )
    {
    if ( aRequest.IsSupported( KVcxMediaMyVideosTransactionId ) )
        {
        TUint32 transactionId( aRequest.ValueTObjectL<TUint32>( KVcxMediaMyVideosTransactionId ));
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: transaction ID: %d", transactionId );
        aResp.SetTObjectValueL<TUint32>( KVcxMediaMyVideosTransactionId, transactionId );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SendMyVideosMessageL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SendMyVideosMessageL(
        TUint32 aMessageId, CMPXCommand* aCommand )
    {
    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL( message ); // 1->
    TMPXItemId itemId;
    itemId.iId1 = KVcxCommandIdMyVideos;
    message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, itemId );
    message->SetTObjectValueL<TInt>( KVcxMediaMyVideosCommandId,
            aMessageId );
    
    if ( aCommand )
        {
        SetTransactionIdL( *aCommand, *message );
        }
    else
        {
        message->SetTObjectValueL<TUint32>( KVcxMediaMyVideosTransactionId, 0 );
        }
            
    iMessageList->AddL( message );
    CleanupStack::Pop( message ); // <-1
    iMessageList->SendL();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::AsyncFileOperationsL
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations& CVcxMyVideosCollectionPlugin::AsyncFileOperationsL()
    {
    if ( !iAsyncFileOperations )
        {
        iAsyncFileOperations = CVcxMyVideosAsyncFileOperations::NewL( *this );
        }
    return *iAsyncFileOperations;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::HandleObjectPresentNotification
// From MVcxMyVideosMdsDbObserver. Called when media is inserted/removed. 
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::HandleObjectPresentNotification()
    {
    TRAPD( err, DoHandleObjectPresentNotificationL() );

    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin::DoHandleObjectPresentNotificationL() left with code %d", err);
        }
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::DoHandleObjectPresentNotificationL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::DoHandleObjectPresentNotificationL()
    {
    iCache->ResetVideoListL();

    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: adding modify event for path root, extra info = %d",
            EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxUidMyVideosMpxCollection,
            KVcxUidMyVideosMpxCollection ), EMPXItemModified, EVcxMyVideosVideoListOrderChanged );
            
    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: adding modify event for category[%d], extra info = %d",
            KVcxMvcCategoryIdAll, EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdAll, 1 ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );

    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: adding modify event for category[%d], extra info = %d",
            KVcxMvcCategoryIdDownloads, EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdDownloads, 1 ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );

    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: adding modify event for category[%d], extra info = %d",
            KVcxMvcCategoryIdTvRecordings, EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdTvRecordings, 1 ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );

    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: adding modify event for category[%d], extra info = %d",
            KVcxMvcCategoryIdCaptured, EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdCaptured, 1 ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );

    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: adding modify event for category[%d], extra info = %d",
            KVcxMvcCategoryIdOther, EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdOther, 1 ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );

    iMessageList->SendL();    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SetDownloadIdToZeroL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SetDownloadIdToZeroL( TUint aDownloadId )
    {
    CMPXMedia* video = iCache->FindVideoByDownloadIdL( aDownloadId );
    if ( video )
        {
        video->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, 0 );
        video->SetTObjectValueL<TUint8>( KVcxMediaMyVideosDownloadState,
                static_cast<TUint8>( EVcxMyVideosDlStateNone ) );
        iMyVideosMdsDb->UpdateVideoL( *video );
        iMessageList->AddEventL( TVcxMyVideosCollectionUtil::IdL(
                *video ), EMPXItemModified );
        iMessageList->SendL();
        }
    }
    
#ifdef _DEBUG
// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::DownloadState
// ----------------------------------------------------------------------------
//
const TDesC& CVcxMyVideosCollectionPlugin::DownloadState( TUint8 aDlState )
    {
    _LIT(KDlStateNoneDes, "None");
    _LIT(KDlStateDownloadingDes, "Downloading");
    _LIT(KDlStateFailedDes, "Failed");
    _LIT(KDlStatePausedDes, "Paused");
    _LIT(KDlStateDownloadedDes, "Downloaded");

    switch ( aDlState )
        {
        case EVcxMyVideosDlStateNone:
            return KDlStateNoneDes;
        case EVcxMyVideosDlStateDownloading:
            return KDlStateDownloadingDes;
        case EVcxMyVideosDlStateFailed:
            return KDlStateFailedDes;
        case EVcxMyVideosDlStatePaused:
            return KDlStatePausedDes;
        case EVcxMyVideosDlStateDownloaded:
            return KDlStateDownloadedDes;
        default:
            return KNullDesC;
        }
    }
#endif

