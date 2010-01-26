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
* Description:    CVcxNsChPublisher class definition*
*/




#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>

#include "videochpublishabledata.h"
#include "videochpublisher.h"

// CONSTANTS

//////
// names used as keys in maps to be passed to service api:

// key name for item data
_LIT8( KVcxNsSAKeyItem,    "item" );

// key name for type data
_LIT8( KVcxNsSAKeyType,    "type" );

// key name for "add" command -data
_LIT8( KVcxNsSAKeyCmdAdd,  "Add" );

// key name for "delete" command -data
_LIT8( KVcxNsSAKeyCmdDelete,  "Delete" );

// key name for Get list command -data
_LIT8 ( KVcxNsSAKeyCmdGetList,  "GetList" );

// key name for publisher name data
_LIT8( KVcxNsSAKeyPublisher,   "publisher" );

// key name for content_type data
_LIT8( KVcxNsSAKeyContentType, "content_type" );

// key name for content id data
_LIT8( KVcxNsSAKeyContentId,   "content_id" );

// key name for actual data
_LIT8( KVcxNsSAKeyDataMap, "data_map" );

// value key name for results when reading 
// from CP database
_LIT8( KVcxNsSAKeyResults, "results" );

/////////////////
// common values used in result map:

// value name used to identify data 
// from the result liw map in homescreen's 
// contentpublishing -plugin
_LIT ( KVcxNsSapiCPData,  "cp_data" );

// publisher value name used for tvvideosuite
// related data
_LIT( KTvVideoPublisher,   "tvvideopublisher" );

// content id's
// "normal" id for content used in the matrix menu
_LIT( KContentMenuId, "menucontentid" );
// this id can be used to maintain data in CP, but 
// not show it in the matrix menu
_LIT( KContentBackupId, "backupcontentid" );



// -----------------------------------------------------------------------------
// CVcxNsChPublisher::CVcxNsChPublisher
// -----------------------------------------------------------------------------
//
CVcxNsChPublisher::CVcxNsChPublisher( MLiwInterface* iCPSInterface, 
                                      CVcxNsChPublishableData* aData ) :
iCPSInterface( iCPSInterface ),                                      
iPublishableData( aData ),
iContentId( 0, 0 )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::~CVcxNsChPublisher
// -----------------------------------------------------------------------------
//
CVcxNsChPublisher::~CVcxNsChPublisher()
    {
    // No implementation required 
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::NewLC
// -----------------------------------------------------------------------------
//
CVcxNsChPublisher* CVcxNsChPublisher::NewLC( MLiwInterface* aCPInterface,
                                             CVcxNsChPublishableData* aData )
    {
    if( !aCPInterface || !aData )
        {
        User::Leave( KErrNotFound );
        }
    
    CVcxNsChPublisher* self = new (ELeave)CVcxNsChPublisher( aCPInterface, aData );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::NewL
// -----------------------------------------------------------------------------
//
CVcxNsChPublisher* CVcxNsChPublisher::NewL( MLiwInterface* aCPInterface, 
                                            CVcxNsChPublishableData* aData )
    {
    CVcxNsChPublisher* self = CVcxNsChPublisher::NewLC( aCPInterface, aData );
    CleanupStack::Pop( self ); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::ConstructL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublisher::ConstructL()
    {
    iContentId.Set( KContentMenuId );
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::PublishAllL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublisher::PublishAllL()
    {
    PublishContentL( EVCxNsCPContentLastWatched );
    PublishContentL( EVCxNsCPContentLiveTV );
    PublishContentL( EVCxNsCPContentIPTV );
    PublishContentL( EVCxNsCPContentMyVideos );
    PublishContentL( EVCxNsCPContentFeeds );
    PublishContentL( EVCxNsCPContentPreloaded );
    }
 
// -----------------------------------------------------------------------------
// CVcxNsChPublisher::PublishContentL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublisher::PublishContentL( TVCxNsCPContentType aContentType )
    {
    CLiwGenericParamList* inParam = CLiwGenericParamList::NewLC();
    CLiwGenericParamList* outParam = CLiwGenericParamList::NewLC();
    ExecCPCommandL( aContentType, KVcxNsSAKeyCmdAdd,
                    inParam, outParam);
    CleanupStack::PopAndDestroy( outParam );
    CleanupStack::PopAndDestroy( inParam );   
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::DeleteContentL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublisher::DeleteContentL( TVCxNsCPContentType aContentType )    
    {
    CLiwGenericParamList* inParam = CLiwGenericParamList::NewLC();
    CLiwGenericParamList* outParam = CLiwGenericParamList::NewLC();
    ExecCPCommandL( aContentType, KVcxNsSAKeyCmdDelete,
                    inParam, outParam );    
    CleanupStack::PopAndDestroy( outParam );
    CleanupStack::PopAndDestroy( inParam ); 
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::ReadContentL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublisher::ReadContentL( TVCxNsCPContentType aContentType )
    {
    TInt pos( 0 );
    CLiwGenericParamList* inParam = CLiwGenericParamList::NewLC();
    CLiwGenericParamList* outParam = CLiwGenericParamList::NewLC();
    
    ExecCPCommandL( aContentType, KVcxNsSAKeyCmdGetList, inParam, outParam );
    
    TLiwVariant variant;
    variant.PushL();

    outParam->FindFirst( pos, KVcxNsSAKeyResults );
    if( pos != KErrNotFound )
        {
        variant = ( *outParam )[pos].Value();
        // get the first iterator data. In tvvideo suite context
        // we have only one level of data
        CLiwIterable* iterable = variant.AsIterable();
        iterable->Reset();
        iterable->NextL( variant );
        
        CLiwDefaultMap* cpDataRootMap = CLiwDefaultMap::NewLC();
        if( variant.Get( *cpDataRootMap ) )
            {
            ReadCPResultDataRootL( aContentType, cpDataRootMap );
            }

       CleanupStack::PopAndDestroy( cpDataRootMap );     
       }                       
    CleanupStack::PopAndDestroy( &variant );
    CleanupStack::PopAndDestroy(outParam);
    CleanupStack::PopAndDestroy(inParam); 
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::BackupContentL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublisher::BackupContentL( TVCxNsCPContentType aContentType, TBool aRefreshFrDB )
    {
    if( aRefreshFrDB )
        {
        // read content data from default to backup        
        iContentId.Set( KContentMenuId );
        ReadContentL( aContentType );
        }
    iContentId.Set( KContentBackupId );
    // trap to make sure the default id is switched back
    // delete old backup
    TRAPD( err, DeleteContentL( aContentType ) );
    if( err == KErrNone )
        {
        TRAP( err, PublishContentL( aContentType ) );
        }
    iContentId.Set( KContentMenuId );
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::RestoreBackupL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublisher::RestoreBackupL( TVCxNsCPContentType aContentType )
    {
    // read content data from backup to default        
    iContentId.Set( KContentBackupId );
    // trap to make sure the default id is swithced back
    TRAPD( err, ReadContentL( aContentType ) );
    if( err == KErrNone )
        {
        // delete old backup
        TRAP( err, DeleteContentL( aContentType ) );
        }
    iContentId.Set( KContentMenuId );
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::ReadCPResultDataRootL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublisher::ReadCPResultDataRootL( TVCxNsCPContentType& aContentType, 
                                              CLiwDefaultMap* aDataRootMap )
    {
    TLiwVariant variant;
    variant.PushL();
    if( aDataRootMap->FindL( KVcxNsSAKeyDataMap, variant ) )
        {
        CLiwDefaultMap* cpDataMap = CLiwDefaultMap::NewLC();
        if( variant.Get( *cpDataMap ) )
            {
            iPublishableData->ReadCPDataMapL( aContentType, *cpDataMap );
            }
        CleanupStack::PopAndDestroy( cpDataMap );        
        }
    CleanupStack::PopAndDestroy( &variant );

    }

// -----------------------------------------------------------------------------
// CVcxNsChPublisher::ExecCPCommandL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublisher::ExecCPCommandL( TVCxNsCPContentType& aContentType,   
                                        const TDesC8& aCmd,
                                        CLiwGenericParamList* aInParam,
                                        CLiwGenericParamList* aOutParam )
    {
    // inParammlist -map organisation:
    // results
    //  |- KVcxNsSAKeyType, KVcxNsSapiCPData
    //  |- KVcxNsSAKeyItem, cpDataRootMap
    //                          |- KVcxNsSAKeyPublisher, KTvVideoPublisher
    //                          |- KVcxNsSAKeyContentType, < resolved content name >
    //                          |- KVcxNsSAKeyContentId, KContentMenuId (for visible content) OR
    //                                                   KContentBackupId (for backed up content)
    //                          |- KVcxNsSAKeyDataMap, cpDataMap
    //                                                   |- contains data map generated 
    //                                                      in CVcxNsChPublishableData
    
    
        
    // setup basic data type value
    TLiwGenericParam cptype( KVcxNsSAKeyType, TLiwVariant( KVcxNsSapiCPData ));
    aInParam->AppendL( cptype );
    
    CLiwDefaultMap* cpDataRootMap = CLiwDefaultMap::NewLC();
    
    // set up publisher 
    cpDataRootMap->InsertL( KVcxNsSAKeyPublisher,   
                            TLiwVariant( KTvVideoPublisher ));
    // set up content_type 
    TPtrC cType = iPublishableData->ItemContentTypeNameL( aContentType );
    cpDataRootMap->InsertL( KVcxNsSAKeyContentType, 
                            TLiwVariant( cType ) );
    
    // content_id, just use default value
    cpDataRootMap->InsertL( KVcxNsSAKeyContentId,   
                            TLiwVariant( iContentId ));
    
    CLiwDefaultMap* cpDataMap = CLiwDefaultMap::NewLC();
    
    // set the actual data from our data container
    iPublishableData->FillCPDataMapL( aContentType, *cpDataMap );
    
    // save it to root
    cpDataRootMap->InsertL( KVcxNsSAKeyDataMap, TLiwVariant( cpDataMap ) );
        
    // data ready, fill item and save it to input param
    TLiwGenericParam item( KVcxNsSAKeyItem, TLiwVariant( cpDataRootMap ));        
    aInParam->AppendL( item );

    CleanupStack::PopAndDestroy( cpDataMap );
    CleanupStack::PopAndDestroy( cpDataRootMap );
    item.Reset();
    cptype.Reset();

    iCPSInterface->ExecuteCmdL( aCmd, *aInParam, *aOutParam );    
 
    }

