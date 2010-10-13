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
* Description:  Video Playback User Input Handler Test class for STIF unit testing
*
*/

// Version : %version: 1 %



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <e32std.h>
#include <w32std.h>
#include <coeaui.h>
#include <aknutils.h>
#include <aknviewappui.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "ut_collectionplugintest.h"

#include "mpxlog.h"
#include <vcxmyvideosuids.h>
#include <vcxmyvideosdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmessagegeneraldefs.h>

#include <mpxcollectionplugin.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>
#include "vcxmyvideosactivetask.h"
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideosvideocache_stub.h"

#define private public
#include "vcxmyvideoscollectionplugin.h"
#undef private

_LIT( KTestModuleName, "CollectionPluginTest" );
_LIT( KLogLocation, "In %S" );

const TInt KInvalidId( -100 );
const TInt KSelectAll( -2 );

enum EMPXMediaAttribute
    {
    ETestMediaGeneralId = 1,
    ETestMediaTitle
    };

enum EStubs
    {
    EActiveTask = 1,
    EAlbums,
    EAsyncFileOperations,
    ECategories,
    EMDSDB,
    EMessageList,
    EOpenHandler,
    EVideoCache
    };

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCollectionPluginTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::Delete()
{
    if( iCollection )
        {
        delete iCollection;
        iCollection = NULL;
        }
    
    if( iPath )
        {
        delete iPath;
        iPath = NULL;
        }
    
    if( iMessageList )
        {
        iMessageList->Reset();
        delete iMessageList;
        iMessageList = NULL;
        }
    
    if( iMedia )
        {
        delete iMedia;
        iMedia = NULL;
        }

    if( iCommand )
        {
        delete iCommand;
        iCommand = NULL;
        }
    
    iAttrs.Close();
}

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::CreateL
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::CreateL()
{
    TInt arguments( 0 );
    iCollection = CVcxMyVideosCollectionPlugin::NewL( &arguments );
    iCollection->SetObserver( *this );
    iMessageList = CMPXMediaArray::NewL();
    iPath = CMPXCollectionPath::NewL();
}


// -----------------------------------------------------------------------------
// CCollectionPluginTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::RunMethodL( CStifItemParser& aItem )
{
    static TStifFunctionInfo const KFunctions[] =
    {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "CreateNewMedia", CCollectionPluginTest::CreateNewMediaL ),
        ENTRY( "SetMediaAttribute", CCollectionPluginTest::SetMediaAttributeL ),
        ENTRY( "AddMediaToCache", CCollectionPluginTest::AddMediaToCacheL ),
        ENTRY( "RemoveMediaFromCache", CCollectionPluginTest::RemoveMediaFromCacheL ),
        ENTRY( "CheckMediaCount", CCollectionPluginTest::CheckMediaCountL ),
        ENTRY( "OpenL", CCollectionPluginTest::OpenL ),
        ENTRY( "SelectPath", CCollectionPluginTest::SelectPathL ),
        ENTRY( "SelectPathId", CCollectionPluginTest::SelectPathIdL ),
        ENTRY( "InsertToPath", CCollectionPluginTest::InsertToPathL ),
        ENTRY( "SelectAttributes", CCollectionPluginTest::SelectAttributesL ),
        ENTRY( "MediaL", CCollectionPluginTest::MediaL ),
        ENTRY( "CancelRequest", CCollectionPluginTest::CancelRequest ),
        ENTRY( "CommandL", CCollectionPluginTest::CommandL ),
        ENTRY( "FindAllL", CCollectionPluginTest::FindAllL ),
        ENTRY( "FindAllSyncL", CCollectionPluginTest::FindAllSyncL ),
        ENTRY( "GetCapabilities", CCollectionPluginTest::GetCapabilities ),
        ENTRY( "AddL", CCollectionPluginTest::AddL ),
        ENTRY( "RemoveL", CCollectionPluginTest::RemoveL ),
        ENTRY( "SetL", CCollectionPluginTest::SetL ),
        ENTRY( "HandleStepL", CCollectionPluginTest::HandleStepL ),
        ENTRY( "CreateNewCmd", CCollectionPluginTest::CreateNewCmdL ),
        ENTRY( "CheckCurrentCmd", CCollectionPluginTest::CheckCurrentCmdL ),
        ENTRY( "SetNewCmd", CCollectionPluginTest::SetNewCmdL ),
        ENTRY( "SetVideoListFetching", CCollectionPluginTest::SetVideoListFetchingL ),
        ENTRY( "CancelCacheListFetching", CCollectionPluginTest::CancelCacheListFetchingL ),
        ENTRY( "HandleMyVideosDbEvent", CCollectionPluginTest::HandleMyVideosDbEvent ),
        ENTRY( "HandleCreateVideoListResp", CCollectionPluginTest::HandleCreateVideoListResp ),
        ENTRY( "HandleObjectPresentNotification", CCollectionPluginTest::HandleObjectPresentNotificationL ),
        ENTRY( "AddVideoToMdsAndCacheL", CCollectionPluginTest::AddVideoToMdsAndCacheL ),
        ENTRY( "SetVideoL", CCollectionPluginTest::SetVideoL ),
        ENTRY( "SendMyVideosMessageL", CCollectionPluginTest::SendMyVideosMessageL ),
        ENTRY( "SendMessages", CCollectionPluginTest::SendMessages ),
        ENTRY( "CategoriesL", CCollectionPluginTest::CategoriesL ),
        ENTRY( "NotifyNewVideosCountDecreasedL", CCollectionPluginTest::NotifyNewVideosCountDecreasedL ),
        ENTRY( "DeleteStub", CCollectionPluginTest::DeleteStubL ),
        ENTRY( "AlbumsL", CCollectionPluginTest::AlbumsL ),
    };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::LogMethod( TPtrC aMethod )
    {
    // Print to UI
    TBuf< 64 > buffer;
    buffer.Format( KLogLocation, &aMethod );
    TestModuleIf().Printf( 0, KTestModuleName, buffer );
    // Print to log file
    iLog->Log( buffer );
    }

// -----------------------------------------------------------------------------
//  CCollectionPluginTest::SetMessageResponse
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::DeleteStubL( CStifItemParser& aItem )
    {
    TInt error( KErrNone );
    TInt stub( KErrNotFound );
    aItem.GetNextInt( stub );
    
    switch( stub )
        {
        case EActiveTask:
            TRAP( error, delete iCollection->iActiveTask );
            break;
        case EAlbums:
            TRAP( error, delete iCollection->iAlbums );
            break;
        case EAsyncFileOperations:
            TRAP( error, delete iCollection->iAsyncFileOperations );
            break;
        case ECategories:
            TRAP( error, delete iCollection->iCategories );
            break;
        case EMDSDB:
            TRAP( error, delete iCollection->iMyVideosMdsDb );
            break;
        case EMessageList:
            TRAP( error, delete iCollection->iMessageList );
            break;
        case EOpenHandler:
            TRAP( error, delete iCollection->iOpenHandler );
            break;
        case EVideoCache:
            TRAP( error, delete iCollection->iCache );
            break;
        default:
            break;
        }
    
    return error;
    }

// -----------------------------------------------------------------------------
//  CCollectionPluginTest::CheckMessageResponse
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::CheckCurrentCmdL( CStifItemParser& aItem )
    {
    TInt error( KErrNone );
    
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    
    // CMD ID
    TUint32 cmdId( 0 );
    TPtrC cmdIdStr;
    if( aItem.GetNextString( cmdIdStr ) == KErrNone )
        {
        TLex lex( cmdIdStr );
        lex.Val( cmdId, EHex );
        }
    
    // MV CMD ID
    TInt cmdMV( KErrNotFound );
    if( cmdId == KVcxCommandIdMyVideos )
        {
        aItem.GetNextInt( cmdMV );
        }
    
    if( iCollection->iActiveTask->iCurCommand == NULL && !cmdId )
        {
        return KErrNone;
        }
    
    TMPXCommandId commandId = iCollection->iActiveTask->iCurCommand->ValueTObjectL<TMPXCommandId>( KMPXCommandGeneralId );
    if( commandId != cmdId )
        {
        error = KErrCorrupt;
        }

    if( commandId == KVcxCommandIdMyVideos )
        {
        TInt currCmdMV = iCollection->iActiveTask->iCurCommand->ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId );
        if( currCmdMV != cmdMV )
            {
            error = KErrCorrupt;
            }
        }
    
    return error;
    }

// -----------------------------------------------------------------------------
//  CCollectionPluginTest::CreateNewMediaL
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::CreateNewMediaL( CStifItemParser& /*aItem*/ )
    {    
    delete iMedia;
    iMedia = NULL;
    iMedia = CMPXMedia::NewL(); // NULL;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//  CCollectionPluginTest::CreateNewMediaL
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::SetMediaAttributeL( CStifItemParser& aItem )
    {
    CleanupStack::PushL( iMedia );
    
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    TInt attribute( KErrNotFound );
    aItem.GetNextInt( attribute );
    switch( attribute )
        {
        case ETestMediaGeneralId:
            {
            TInt mediaId( 0 );
            aItem.GetNextInt( mediaId );
            TInt mediaType( KVcxMvcMediaTypeVideo );
            aItem.GetNextInt( mediaType );
            TMPXItemId mpxId( mediaId, mediaType );
            iMedia->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxId );
            }
            break;
            
        case ETestMediaTitle:
            {
            TPtrC value;
            aItem.GetNextString( value );
            iMedia->SetTextValueL( KMPXMediaGeneralTitle, value );                
            }
            break;
        default:
            break;
        }
    CleanupStack::Pop( iMedia );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//  CCollectionPluginTest::CreateNewCmdL
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::CreateNewCmdL( CStifItemParser& aItem )
    {
    TInt error( KErrNone );
    
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    // COMMAND SYNC
    TInt paramSync( 0 );
    aItem.GetNextInt( paramSync );
    
    // CMD ID
    TUint32 cmdId( 0 );
    TPtrC cmdIdStr;
    if( aItem.GetNextString( cmdIdStr ) == KErrNone )
        {
        TLex lex( cmdIdStr );
        User::LeaveIfError( lex.Val( cmdId, EHex ) );
        }
    
    // MV CMD ID
    TInt cmdMV( KErrNotFound );
    if( cmdId == KVcxCommandIdMyVideos )
        {
        aItem.GetNextInt( cmdMV );
        }

    delete iCommand;
    iCommand = NULL;
    iCommand = CreateMpxCommandLC( cmdId, cmdMV, paramSync );
    
    switch( cmdId )
        {
        case KMPXCommandIdCollectionAdd: // SYNC
            {
            iCommand->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, iMedia );
            }
            break;
        
        case KMPXCommandIdCollectionSet: // SYNC
            {
            iCommand->SetCObjectValueL<CMPXMedia>( KMPXCommandColSetMedia, iMedia );
            }
            break;
        
        case KMPXCommandIdCollectionRemoveMedia:
            {
            TMPXItemId itemId = iMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            iCommand->SetTObjectValueL( KMPXMediaGeneralId, itemId );
            }
            break;
        default:
            break;
        }

    switch( cmdMV )
        {
        case KVcxCommandMyVideosGetMediaFullDetailsByMpxId:
            {
            TMPXItemId itemId = iMedia->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            iCommand->SetTObjectValueL( KMPXMediaGeneralId, itemId );
            }
            break;
        case KVcxCommandMyVideosGetMediasByMpxId:
            {
            CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
            CleanupStack::PushL( mediaArray );

            TInt id( 0 );
            
            while( aItem.GetNextInt( id ) == KErrNone )
                {
                CMPXMedia* item = CMPXMedia::NewL();
                CleanupStack::PushL( item ); 
                TMPXItemId itemId;
                itemId.iId1 = id;
                itemId.iId2 = KVcxMvcMediaTypeVideo;
                item->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, itemId );
                mediaArray->AppendL( *item );
                CleanupStack::PopAndDestroy( item );
                }
            if( id != KErrNotFound )
                {
                iCommand->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, mediaArray );
                }
            CleanupStack::Pop( mediaArray );
            }
            break;
        case KVcxCommandMyVideosMove:
        case KVcxCommandMyVideosCopy:
        case KVcxCommandMyVideosDelete:
        case KVcxCommandMyVideosCancelMoveOrCopy: // SYNC
        case KVcxCommandMyVideosCancelDelete: // SYNC
        case KVcxCommandMyVideosAddAlbum: // SYNC
        case KVcxCommandMyVideosAddToAlbum: // ASYNC
        case KVcxCommandMyVideosRemoveFromAlbum: // ASYNC
            {
            if( iMedia )
                {
                iCommand->SetCObjectValueL<CMPXMedia>( KMPXCommandColSetMedia, iMedia );
                }
            }
            break;
        default:
            break;
        }
    
    CleanupStack::Pop( iCommand );
    
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::SetNewCmdL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("SetNewCmdL"));
    TRAPD( error, iCollection->iActiveTask->SetNewCommandL( *iCommand ) );
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::SetListCacheFetching
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::CancelCacheListFetchingL( CStifItemParser& aItem )
    {
    LogMethod(_L("CancelCacheListFetchingL"));
    TInt value( KErrNotFound );
    aItem.GetNextInt( value );
    TBool enabled( value );
    TRAPD( error, iCollection->iCache->iCancelListFetching = enabled );
    return error;
    }

// -----------------------------------------------------------------------------
//  CCollectionPluginTest::AddMediaToCacheL
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::AddMediaToCacheL( CStifItemParser& /*aItem*/ )
    {
    CMPXMediaArray* cache = iCollection->iCache->iVideoList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    TRAPD( error, cache->AppendL( *iMedia ) );
    iCollection->iCache->iVideoList->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, cache);
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
/*
TInt CCollectionPluginTest::AddMediaToCacheL( CStifItemParser& aItem )
    {
    LogMethod(_L("AddMediaToCache"));
    
    TInt err( 0 );
    
    CMPXMedia* media = CMPXMedia::NewL();
    TInt mediaId( 0 );
    aItem.GetNextInt( mediaId );
    TMPXItemId mpxId( mediaId, 0 );
    media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxId );
    
    CMPXMediaArray* medias = iCollection->iCache->iVideoList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    medias->AppendL(*media);
    delete media;
    
    return err;
    }*/

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::RemoveMediaFromCacheL( CStifItemParser& aItem )
    {
    LogMethod(_L("RemoveMediaFromCache"));
    
    TInt err = ( -1 );
    
    TInt mediaId( 0 );
    aItem.GetNextInt( mediaId );
    
    CMPXMediaArray* medias = iCollection->iCache->iVideoList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    for ( TInt i = 0; i < medias->Count(); i++ )
        {
        CMPXMedia* media = (*medias)[i];    
        TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
        if( itemId.iId1 == mediaId )
            {
            medias->Remove(i);
            err = KErrNone;
            break;
            }
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::OpenL( CStifItemParser& aItem )
    {
    LogMethod(_L("OpenL"));

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    TInt32 collectionId( KErrNotFound );
    TPtrC collectionIdStr;
    if( aItem.GetNextString( collectionIdStr ) == KErrNone )
        {
        TLex lex( collectionIdStr );
        lex.Val( collectionId, EHex );
        }
    
    CleanupStack::PushL( iPath );
    
    iPath->Reset();
    
    switch( collectionId )
        {
        case KErrNotFound:
            break;
        case KInvalidId:
            iPath->AppendL( TMPXItemId::InvalidId() );
            break;
        case 0:
            iPath->AppendL( KVcxUidMyVideosMpxCollection );
            break;
        default:
            iPath->AppendL( collectionId );
            break;
        }
    
    TMPXItemId item(1, 1);
    iPath->AppendL(item);

    RArray<TMPXAttribute> attrs;
    CMPXFilter* filter = CMPXFilter::NewL();
    
    TRAPD( error, iCollection->OpenL( *iPath, attrs.Array(), filter ) );
    
    CleanupStack::Pop( iPath );
    attrs.Reset();
    attrs.Close();
    delete filter;
    return error; // error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::InsertToPathL
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::InsertToPathL( CStifItemParser& aItem )
    {
    LogMethod(_L("InsertToPathL"));
    
    //aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    TInt id( KErrNotFound );
    TInt type( KErrNotFound );

    aItem.GetNextInt( id );
    aItem.GetNextInt( type );
    TMPXItemId itemId( id, type );
    CleanupStack::PushL( iPath );
    TRAPD( error, iPath->InsertL(itemId, iPath->Count()-1) );   
    CleanupStack::Pop( iPath );
    
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::SelectPathL
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::SelectPathL( CStifItemParser& aItem )
    {
    LogMethod(_L("SelectPathL"));
    
    CleanupStack::PushL( iPath );
    
    //aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    TInt index( KErrNotFound );
    aItem.GetNextInt( index );
    
    TInt pathcount = iPath->Count();
    
    TInt error( KErrNotFound );
    switch( index )
        {
        case KSelectAll:
            TRAP( error, iPath->SelectAllL() );
            break;
        case KErrNotFound:
            TRAP( error, iPath->DeselectAll() );
            break;
        case KInvalidId:
            TRAP( error, iPath->SelectL( TMPXItemId::InvalidId() ) );
            break;
        default:
            TRAP( error, iPath->SelectL( index ) );
            break;
        }
    
    CleanupStack::Pop( iPath );
    
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::SelectPathL
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::SelectPathIdL( CStifItemParser& aItem )
    {
    LogMethod(_L("SelectPathL"));
    CleanupStack::PushL( iPath );
    TInt id( KErrNotFound );
    aItem.GetNextInt( id );
    TInt pathcount = iPath->Count();
    TMPXItemId itemId( id, KVcxMvcMediaTypeVideo );
    TRAPD( error, iPath->SelectL( itemId ) );
    CleanupStack::Pop( iPath );
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::SelectPathL
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::SelectAttributesL( CStifItemParser& aItem )
    {
    LogMethod(_L("SelectAttributesL"));
    
    //aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    iAttrs.Reset();
    TInt attr( KErrNotFound );
    aItem.GetNextInt( attr );
    switch( attr )
        {
        case ETestMediaGeneralId:
            iAttrs.AppendL( KMPXMediaGeneralId );
            break;
        default:
            break;
        }        
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::MediaL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("MediaL"));
    
    RArray<TCapability> caps;
    CleanupClosePushL(caps);
    caps.AppendL(ECapability_None);
    /*if (aCaps.HasCapability(ECapabilityDRM))
        {
        caps.AppendL(ECapabilityDRM);
        }*/
    // else add further "relevant" capabilities here

    // Add "none" capability, if no caps listed
    /*if (caps.Count() == 0)
        {
        caps.AppendL(ECapability_None);
        }*/
    
    CMPXMedia* specs = CMPXMedia::NewL();
    
    TRAPD( error, iCollection->MediaL( *iPath, iAttrs.Array(), caps.Array(), specs ) );
    delete specs;
    CleanupStack::PopAndDestroy(&caps);
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::CheckMediaCountL( CStifItemParser& aItem )
    {
    LogMethod(_L("CheckMediaCount"));
    
    TInt err( 0 );
    
    TInt expected( 0 );
    aItem.GetNextInt( expected );
    
    CMPXMediaArray* medias = iCollection->iCache->iVideoList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    if( medias->Count() != expected )
        {
        err = KErrCorrupt;
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::CancelRequest( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("CancelRequest"));
	
    iCollection->CancelRequest();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::CommandL( CStifItemParser& aItem )
    {
    LogMethod(_L("CommandL"));
    CreateNewCmdL( aItem );
    TRAPD( error, iCollection->CommandL( *iCommand ) );
    
#ifndef _DEBUG
    // KMPXCommandIdCollectionAdd leaves with KErrNotSupported if the build is not udeb
    // and it cannot be checked with STIF, so here's a solution 
    
    TMPXCommandId cmdId = iCollection->iActiveTask->iCurCommand->ValueTObjectL<TMPXCommandId>( KMPXCommandGeneralId );
    if( cmdId == KMPXCommandIdCollectionAdd && error == KErrNotSupported )
        {
        error = KErrNone;
        }
#endif
    
    return error;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::CreateMpxCommandLC
// -----------------------------------------------------------------------------
//
CMPXCommand* CCollectionPluginTest::CreateMpxCommandLC( TInt aCommandGeneralId, 
                                                        TInt aMyVideosCommandId, 
                                                        TInt aSync )
    {
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    if( aCommandGeneralId != 0 )
        {
        cmd->SetTObjectValueL( KMPXCommandGeneralId, aCommandGeneralId );
        if( aCommandGeneralId == KVcxCommandIdMyVideos && aMyVideosCommandId != KErrNotFound )
            {
            cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, aMyVideosCommandId );
            }
        }
    
    if( aSync != KErrNotFound )
        {
        TBool asyncBool( aSync );
        cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, asyncBool );
        }

    /*
    if( !aSync )
        {
        iTransactions->NextTransactionId();

        if( aCommandGeneralId == KVcxCommandIdMyVideos )
            {
            cmd->SetTObjectValueL( KVcxMediaMyVideosTransactionId, iTransactions->TransactionId() );
            iTransactions->AddTransactionId( aMyVideosCommandId );
            }
        }

    VCXLOGLO2("CVCXMyVideosCollectionPluginTester::CreateMpxCommandLC: collectionId: 0x%x", iCollectionUid.iUid);
    */
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, iCollectionUid.iUid );

    return cmd;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::FindAllL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("FindAllL"));
    TInt error( 0 );
    CMPXMedia* media = CMPXMedia::NewL();
    RArray<TMPXAttribute> array;
    TRAP( error, iCollection->FindAllL(*media, array.Array()) );
    delete media;
    array.Close();
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::FindAllSyncL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("FindAllSyncL"));
    TInt error( 0 );
    CMPXMedia* media = CMPXMedia::NewL();
    RArray<TMPXAttribute> array;
    TRAP( error, iCollection->FindAllSyncL( *media, array.Array() ) );
    delete media;
    array.Close();
    
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::GetCapabilities( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("GetCapabilities"));

    TInt error( 0 );
    TCollectionCapability cap;
    cap = iCollection->GetCapabilities();
    if( cap != EMcSearch )
        {
        error = KErrCorrupt;
        }
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::AddL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("AddL"));
    CMPXMedia* media = CMPXMedia::NewL();
    TRAPD( error, iCollection->AddL( *media ) );
    delete media;
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::RemoveL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("RemoveL"));
    CMPXMedia* media = CMPXMedia::NewL();
    TRAPD( error, iCollection->RemoveL( *media ) );
    delete media;
    return error;;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::SetL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("SetL"));
    CMPXMedia* media = CMPXMedia::NewL();
    TRAPD( error, iCollection->SetL( *media ) ); // SetL is obsolete, so just set a null media
    delete media;
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::HandleStepL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("HandleStepL"));
    TRAPD( error, iCollection->iActiveTask->RunL() );
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::SetVideoListFetchingL( CStifItemParser& aItem )
    {
    TInt value( KErrNotFound );
    aItem.GetNextInt( value );
    TBool enabled( value );
    TRAPD( error, iCollection->iMyVideosMdsDb->iVideoListFetchingIsOngoing = enabled );
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::HandleMyVideosDbEvent( CStifItemParser& aItem )
    {
    LogMethod(_L("HandleMyVideosDbEvent"));

    //CVcxMyVideosCollectionPlugin::HandleMyVideosDbEvent(
    //        TMPXChangeEventType aEvent,
    //        RArray<TUint32>& aId )
    
    TMPXChangeEventType event;
    
    TInt eventEnum( 0 );
    aItem.GetNextInt( eventEnum );
    switch( eventEnum )
        {
        case 1:
            event = EMPXItemDeleted;
            break;
        case 2:
            event = EMPXItemInserted;
            break;
        case 3:
            event = EMPXItemModified;
            break;
        default:
            break;
        }

    //RArray<TUint32> ids;
    RArray<TItemId> ids;
    TInt id( 0 );
    while( aItem.GetNextInt( id ) == KErrNone )
        {
        ids.Append( id );
        }

    TRAPD( error, iCollection->HandleMyVideosDbEvent( event, ids, 0 ) );
    
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::HandleCreateVideoListResp( CStifItemParser& aItem )
    {
    LogMethod(_L("HandleCreateVideoListResp"));

    TInt complete( 0 );
    aItem.GetNextInt( complete );

    TBool isComplete( (complete > 0 ) ? ETrue : EFalse );
    
    TInt startIndex( 0 );
    aItem.GetNextInt( startIndex );
    
    CMPXMedia* media = CMPXMedia::NewL();
    TRAPD( error, iCollection->HandleCreateVideoListResp(media, startIndex, isComplete) );
    delete media;
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::HandleObjectPresentNotificationL( CStifItemParser& aItem )
    {
    LogMethod(_L("HandleObjectPresentNotification"));

    TInt cacheLeave( 0 );
    aItem.GetNextInt( cacheLeave );
    iCollection->iCache->iLeaveOnReset = (cacheLeave > 0) ? ETrue : EFalse;    
    TRAPD( error, iCollection->HandleObjectPresentNotification() );
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::AddVideoToMdsAndCacheL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("AddVideoToMdsAndCacheL"));
    TRAPD( error, iCollection->AddVideoToMdsAndCacheL( *iMedia ) );
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::SetVideoL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetVideoL"));

    TInt mpxid( 0 );
    aItem.GetNextInt( mpxid );

    TInt type( 0 );
    aItem.GetNextInt( type );
    
    TMPXItemId mpxId( mpxid, type );
    
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxId );
    TRAPD( error, iCollection->SetVideoL( *media ) );
    CleanupStack::PopAndDestroy( media );
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::SendMyVideosMessageL( CStifItemParser& aItem )
    {
    LogMethod(_L("SendMyVideosMessageL"));

    //void CVcxMyVideosCollectionPlugin::SendMyVideosMessageL(
    //        TUint32 aMessageId, CMPXCommand* aCommand )
    
    TInt msgId( 0 );
    aItem.GetNextInt( msgId );
    
    CMPXCommand* cmd = NULL;
    TInt createCommand( KErrNone );
    aItem.GetNextInt( createCommand );
    if( createCommand != KErrNone )
        {
        cmd = CMPXCommand::NewL();
        }
    
    iCollection->SendMyVideosMessageL( (TUint32)msgId, cmd );
    
    if( cmd )
        {
        delete cmd;
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::SendMessages( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("SendMessages"));
    
    CMPXMessage* msg = CMPXMessage::NewL();
    /*
    CleanupStack::PushL( msg );

    CMPXMediaArray* mediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL( mediaArray );

    TInt msgId( 0 );
    
    while( aItem.GetNextInt( msgId ) == KErrNone )
        {
        CMPXMessage* message = CMPXMessage::NewL();
        CleanupStack::PushL( message ); 
    
        // Set message attributes
        //
        TMPXItemId itemId;
        itemId.iId1 = KVcxCommandIdMyVideos;
        message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, itemId );
        message->SetTObjectValueL<TInt>( KVcxMediaMyVideosCommandId, msgId );
        mediaArray->AppendL( *message );
        CleanupStack::PopAndDestroy( message );
        }
    //msg->SetCObjectValueL( KMPXMediaArrayContents, mediaArray );
    msg->SetCObjectValueL( KMPXMessageArrayContents, mediaArray );
    CleanupStack::PopAndDestroy( mediaArray );
    CleanupStack::Pop( msg );
    */
    iCollection->SendMessages( *msg );
    delete msg;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::CategoriesL( CStifItemParser& aItem )
    {
    LogMethod(_L("CategoriesL"));

    TInt error( 0 );
    TInt deleteCategories( 0 );
    aItem.GetNextInt( deleteCategories );
    if( deleteCategories )
        {
        delete iCollection->iCategories;
        }
    TRAP( error, iCollection->CategoriesL() );
    
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::NotifyNewVideosCountDecreasedL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("NotifyNewVideosCountDecreasedL"));

    TInt error( 0 );
    CMPXMedia* media = CMPXMedia::NewL();
    TRAP( error, iCollection->NotifyNewVideosCountDecreasedL( *media ) );
    delete media;
    return error;
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::HandleMessage(CMPXMessage* aMsg, TInt /*aError*/)
    {
    // LOOK! WE HANDLE!
    // TODO: lisää messagearray
    
    if ( aMsg->IsSupported( KMPXMessageArrayContents ) )
        {
        const CMPXMessageArray* messageArray = 
                aMsg->Value<CMPXMessageArray>(KMPXMessageArrayContents);
            
        for( TInt i = 0; i < messageArray->Count(); i++ )
            {
            iMessageList->AppendL(*(messageArray->AtL( i )));
            }
        }
    else
        {
        iMessageList->AppendL( *aMsg );
        }
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::HandleOpen( CMPXMedia* /*aMedia*/, TInt /*aErr*/ )
    {
    // LOOK! WE HANDLE!
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::HandleOpen(CMPXCollectionPath* /*aPath*/, TInt /*aErr*/ )
    {
    // LOOK! WE HANDLE!
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------o------------
//
void CCollectionPluginTest::HandleOpen(
        CMPXMedia* /*aMedia*/, 
        const CMPXCollectionPath* /*aPath*/, 
        TInt /*aErr*/)
    {
    TInt a = 0;
    a = a+1;
    // LOOK! WE HANDLE!
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::HandleMedia( CMPXMedia* /*aMedia*/, TInt /*aError*/ )
    {
    TInt a = 0;
    a = a+1;
    // LOOK! WE HANDLE!
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::HandleCommandComplete( CMPXCommand* /*aCommandResult*/, 
                                                   TInt /*aError*/)
    {
    // LOOK! WE HANDLE!
    Signal();
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::HandleRemove( const CDesCArray& /*aUriArray*/, TInt /*aError*/ )
    {
    // LOOK! WE HANDLE!
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::HandleFindAll( CMPXMedia* /*aMedia*/, TInt /*aError*/ )
    {
    // LOOK! WE HANDLE!
    }

// -----------------------------------------------------------------------------
//   CCollectionPluginTest::
// -----------------------------------------------------------------------------
//
TInt CCollectionPluginTest::AlbumsL( CStifItemParser& aItem )
    {
    LogMethod(_L("AlbumsL"));
    
    TInt error( 0 );
    TInt deleteAlbums( 0 );
    aItem.GetNextInt( deleteAlbums );
    if( deleteAlbums )
        {
        delete iCollection->iAlbums;
        }

    // Just check that iCollection->AlbumsL() doesn't leave
    TRAP( error, iCollection->AlbumsL() );
    
    return error;
    }

//  [End of File] - Do not remove
