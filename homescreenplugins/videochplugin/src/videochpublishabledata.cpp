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
* Description:    CVcxNsChPublishableData class definition*
*/


#include <liwvariant.h>
#include <e32hashtab.h>

#include <videosuiteres.rsg>
#include "videochpublishabledata.h"
#include "videochpublishabledataitem.h"
#include "iptvlastwatchedapi.h"


// CONSTANTS

// content type values for tvvideosuite
// related data
_LIT(KContentLastWatched, "lastwatchedcontent" );
_LIT(KContentIPTV,        "iptvcontent" );
_LIT(KContentMyVideos,    "myvideoscontent" );
_LIT(KContentVideoFeeds,  "videofeedcontent" );
_LIT(KContentPreloaded,   "preloadedcontent" );
_LIT(KContentService,     "servicecontent");

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::CVcxNsChPublishableData
// -----------------------------------------------------------------------------
//
CVcxNsChPublishableData::CVcxNsChPublishableData() 
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::~CVcxNsChPublishableData
// -----------------------------------------------------------------------------
//
CVcxNsChPublishableData::~CVcxNsChPublishableData()
    {
    
    delete iLastWatched;
    
    if( iData )
        {
        THashMapIter< TInt, CVcxNsPublishableDataItem* > iter( *iData );
        for( CVcxNsPublishableDataItem* const* iValue = iter.NextValue(); 
                            iValue; iValue = iter.NextValue() )
            {
            delete *iValue;
            }
    
        iData->Close();
        delete iData;
        }
    
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::NewLC
// -----------------------------------------------------------------------------
//
CVcxNsChPublishableData* CVcxNsChPublishableData::NewLC()
    {
    CVcxNsChPublishableData* self = new ( ELeave )CVcxNsChPublishableData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::NewL
// -----------------------------------------------------------------------------
//
CVcxNsChPublishableData* CVcxNsChPublishableData::NewL()
    {
    CVcxNsChPublishableData* self = CVcxNsChPublishableData::NewLC();
    CleanupStack::Pop( self ); // self;
    self->iFs.Connect();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::ConstructL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublishableData::ConstructL()
    {
    iData = new( ELeave )RHashMap< TInt, CVcxNsPublishableDataItem* >;
    
    CVcxNsPublishableDataItem* item = NULL;
            
    // Generate default values    
    // Last watched
    item = CVcxNsPublishableDataItem::NewLC( TPtrC( KContentLastWatched ) );   
    item->SetDataL( TPtrC8( KTvVideoName ), KNullDesC );
    item->SetDataL( TPtrC8( KTvVideoPath ), KNullDesC );
    item->SetDataL( TPtrC8( KTvVideoIconPath ), KNullDesC ); 
    item->SetDataL( TPtrC8( KTvVideoLastWatchedType ), 0 ); 
    // ownership transferred
    iData->InsertL( EVCxNsCPContentLastWatched, item );
    CleanupStack::Pop( item );
    
    // iptv
    item = CVcxNsPublishableDataItem::NewLC( TPtrC( KContentIPTV ) );   
    item->SetDataL( TPtrC8( KTvVideoCount ), 0 );
    // ownership transferred
    iData->InsertL( EVCxNsCPContentIPTV, item );
    CleanupStack::Pop( item );
    
    // My videos
    item = CVcxNsPublishableDataItem::NewLC( TPtrC( KContentMyVideos ) );   
    item->SetDataL( TPtrC8( KTvVideoName ), KNullDesC );
    item->SetDataL( TPtrC8( KTvVideoCount ), 0 );
    item->SetDataL( TPtrC8( KTvVideoHasNewVideos ), 0 );
    // ownership transferred
    iData->InsertL( EVCxNsCPContentMyVideos, item );
    CleanupStack::Pop( item );
    
    // video feeds
    item = CVcxNsPublishableDataItem::NewLC( TPtrC( KContentVideoFeeds ) );   
    item->SetDataL( TPtrC8( KTvVideoCount ), 0 );
    // ownership transferred
    iData->InsertL( EVCxNsCPContentFeeds, item );
    CleanupStack::Pop( item );
    
    // preloaded video
    item = CVcxNsPublishableDataItem::NewLC( TPtrC( KContentPreloaded ) );   
    item->SetDataL( TPtrC8( KTvVideoName ), KNullDesC );
    item->SetDataL( TPtrC8( KTvVideoPath ), KNullDesC );
    item->SetDataL( TPtrC8( KTvVideoIconPath ), KNullDesC ); 
    // ownership transferred
    iData->InsertL( EVCxNsCPContentPreloaded, item );
    CleanupStack::Pop( item );
    
    // preloaded service group
    item = CVcxNsPublishableDataItem::NewLC( TPtrC( KContentService ) );   
    item->SetDataL( TPtrC8( KTvVideoName ), KNullDesC );
    item->SetDataL( TPtrC8( KTvVideoPath ), KNullDesC );
    item->SetDataL( TPtrC8( KTvVideoId ), -1 );
    item->SetDataL( TPtrC8( KTvVideoIconPath ), KNullDesC ); 
    // ownership transferred
    iData->InsertL( EVCxNsCPContentServiceGroup, item );
    CleanupStack::Pop( item );

    iLastWatched = CIptvLastWatchedApi::NewL();
    
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::GetItemL
// -----------------------------------------------------------------------------
//
CVcxNsPublishableDataItem* CVcxNsChPublishableData::GetItemL( TVCxNsCPContentType aContentType )
    {
    CVcxNsPublishableDataItem** item = iData->Find( aContentType );
                          
    if( !item || !( *item ) )
        {
        User::Leave( KErrNotFound );
        }
    return *item;
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::ItemContentTypeNameL
// -----------------------------------------------------------------------------
//
TPtrC CVcxNsChPublishableData::ItemContentTypeNameL( TVCxNsCPContentType aContentType )
    {
    return GetItemL( aContentType )->ContentType();
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::SetDataL
// -----------------------------------------------------------------------------
// 
void CVcxNsChPublishableData::SetDataL(
        TVCxNsCPContentType aContentType, 
        TPtrC8 aValueKey,  
        TInt aValue )
    {
    GetItemL( aContentType )->SetDataL( aValueKey, aValue );
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::SetDataL
// -----------------------------------------------------------------------------
//    
void CVcxNsChPublishableData::SetDataL( 
        TVCxNsCPContentType aContentType, 
        TPtrC8 aValueKey,  
        const TDesC& aValue )
    {
    GetItemL( aContentType )->SetDataL( aValueKey, aValue );
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::GetIntDataL
// -----------------------------------------------------------------------------
//  
TInt CVcxNsChPublishableData::GetIntDataL( 
        TVCxNsCPContentType aContentType, 
        TPtrC8 aValueKey )
    {
    return GetItemL( aContentType )->GetIntDataL( aValueKey );
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::GetDesCDataL
// -----------------------------------------------------------------------------
//   
const TDesC& CVcxNsChPublishableData::GetDesCDataL( 
        TVCxNsCPContentType aContentType, 
        TPtrC8 aValueKey )
    {
    return GetItemL( aContentType )->GetDesCDataL( aValueKey );
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::FillCPDataMapL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublishableData::FillCPDataMapL( TVCxNsCPContentType aContentType, 
                                              CLiwDefaultMap& aDataMap )
    {
    GetItemL( aContentType )->FillCPDataMapL( aDataMap ); 
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::ReadCPDataMapL
// -----------------------------------------------------------------------------
//
void CVcxNsChPublishableData::ReadCPDataMapL( TVCxNsCPContentType aContentType, 
                                              CLiwDefaultMap& aDataMap )
    {  
    GetItemL( aContentType )->ReadCPDataMapL( aDataMap );
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::ClearData
// -----------------------------------------------------------------------------
//
void CVcxNsChPublishableData::ClearData( TVCxNsCPContentType aContentType )
    {
    CVcxNsPublishableDataItem* item( 0 );
    TRAPD( err, item = GetItemL( aContentType ) );
    if( err == KErrNone && item )
        {
        item->ClearData();
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::FileExists
// -----------------------------------------------------------------------------
//
TBool CVcxNsChPublishableData::FileExists( const TDesC& aFilePath )
    {
    if( BaflUtils::FileExists( iFs, aFilePath ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVcxNsChPublishableData::LastWatchedApi
// -----------------------------------------------------------------------------
//
CIptvLastWatchedApi& CVcxNsChPublishableData::LastWatchedApi()
    {
    return *iLastWatched;
    }

