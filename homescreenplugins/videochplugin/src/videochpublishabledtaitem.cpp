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
* Description: 
*
*/




#include <e32std.h>
#include <e32base.h>
#include <e32hashtab.h>
#include <LiwVariant.h>

#include "videochpublishabledataitem.h"
#include "videochpublishabledata.h"


// -----------------------------------------------------------------------------
// TPtrC8Hash()
// Local function used to call hash function for creating hash value to
// the key provided.
// -----------------------------------------------------------------------------
//
TUint32 TPtrC8Hash(const TBuf8< KTvVideoMaxValueKeyLen >& aPtr)
    {
    return DefaultHash::Des8(aPtr);
    }

// -----------------------------------------------------------------------------
// TPtrC8Ident()
// Local function used to call indentification function for comparing items
// -----------------------------------------------------------------------------
//
TBool TPtrC8Ident(const TBuf8< KTvVideoMaxValueKeyLen >& aL, 
                  const TBuf8< KTvVideoMaxValueKeyLen >& aR)
    {
    return DefaultIdentity::Des8(aL, aR);
    }


// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::NewLC
// -----------------------------------------------------------------------------
//
CVcxNsPublishableDataItem* CVcxNsPublishableDataItem::NewLC( TPtrC aContentType )
    {
    CVcxNsPublishableDataItem* self = new ( ELeave )CVcxNsPublishableDataItem( aContentType );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::NewL
// -----------------------------------------------------------------------------
//
CVcxNsPublishableDataItem* CVcxNsPublishableDataItem::NewL( TPtrC aContentType )
    {
    CVcxNsPublishableDataItem* self = CVcxNsPublishableDataItem::NewLC( aContentType );
    CleanupStack::Pop( self ); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::CVcxNsPublishableDataItem
// -----------------------------------------------------------------------------
//
CVcxNsPublishableDataItem::CVcxNsPublishableDataItem( TPtrC aContentType ) :
iContentType( aContentType )
    {
    // NOP
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::~CVcxNsPublishableDataItem
// -----------------------------------------------------------------------------
//
CVcxNsPublishableDataItem::~CVcxNsPublishableDataItem()
    {
    if( iIntData )
        {
        iIntData->Close();
        delete iIntData;
        }
    
    if( iDescData )
        {
        THashMapIter< TBuf8< KTvVideoMaxValueKeyLen >, HBufC* > iter( *iDescData );
        for( HBufC* const* value = iter.NextValue(); value; value = iter.NextValue() )
            {
            delete *value;
            }  
        iDescData->Close();
        delete iDescData; 
        }
    
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::ConstructL
// -----------------------------------------------------------------------------
//
void CVcxNsPublishableDataItem::ConstructL()
    {
        
    iIntData  = 
        new( ELeave )RHashMap< TBuf8< KTvVideoMaxValueKeyLen >, TInt >
                (&TPtrC8Hash, &TPtrC8Ident);
    
    iDescData = 
        new( ELeave )RHashMap< TBuf8< KTvVideoMaxValueKeyLen >, HBufC* >
            (&TPtrC8Hash, &TPtrC8Ident); 
    }
 
// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::ContentType
// -----------------------------------------------------------------------------
//
TPtrC CVcxNsPublishableDataItem::ContentType() const
    {
    return iContentType;
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::SetDataL
// -----------------------------------------------------------------------------
//
void CVcxNsPublishableDataItem::SetDataL( TPtrC8 aKey, TInt aValue )
    {
    iIntData->InsertL( aKey, aValue );
    }
 
// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::SetDataL
// -----------------------------------------------------------------------------
//
void CVcxNsPublishableDataItem::SetDataL( TPtrC8 aKey, const TDesC& aData )
    {
    HBufC** item = iDescData->Find( aKey );
    if( item && *item )
        {
        delete *item;
        }
    
    iDescData->InsertL( aKey, aData.AllocL() );
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::GetIntDataL
// -----------------------------------------------------------------------------
//
TInt CVcxNsPublishableDataItem::GetIntDataL( TPtrC8 aKey ) const 
    {
    TInt* item = iIntData->Find( aKey );
    if( !item )
        {
        User::Leave( KErrNotFound );
        }
    return *item;
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::GetDesCDataL
// -----------------------------------------------------------------------------
//
const TDesC& CVcxNsPublishableDataItem::GetDesCDataL( TPtrC8 aKey ) const
    {
    HBufC** item = iDescData->Find( aKey );
    
    if( !item || !( *item ) )
        {
        User::Leave( KErrNotFound );
        }
    return  **item;
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::ClearData
// -----------------------------------------------------------------------------
//
void CVcxNsPublishableDataItem::ClearData()
    {

    THashMapIter< TBuf8< KTvVideoMaxValueKeyLen >, HBufC* > descIter( *iDescData );
    for( HBufC* const* value = descIter.NextValue(); value; value = descIter.NextValue() )
        {        
        TRAP_IGNORE( SetDataL( *( descIter.CurrentKey() ), KNullDesC ) );    
        }
    THashMapIter< TBuf8< KTvVideoMaxValueKeyLen >, TInt > intIter( *iIntData );
    for( const TInt* intValue = intIter.NextValue(); intValue; intValue = intIter.NextValue() )
        {
        TRAP_IGNORE( SetDataL( *( intIter.CurrentKey() ), 0 ) );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::FillCPDataMapL
// -----------------------------------------------------------------------------
//
void CVcxNsPublishableDataItem::FillCPDataMapL( CLiwDefaultMap& aDataMap )
    {
    // fill descriptor data
    THashMapIter< TBuf8< KTvVideoMaxValueKeyLen >, HBufC* > descIter( *iDescData );
    for( HBufC* const* value = descIter.NextValue(); value; value = descIter.NextValue() )
        {
        aDataMap.InsertL( *( descIter.CurrentKey() ) , TLiwVariant( **value ) );
        }
    
    THashMapIter< TBuf8< KTvVideoMaxValueKeyLen >, TInt > intIter( *iIntData );
    TInt32 value;
    for( const TInt* intValue = intIter.NextValue(); intValue; intValue = intIter.NextValue() )
        {
        value = *intValue;
        aDataMap.InsertL( *( intIter.CurrentKey() ) , TLiwVariant( value ) );
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::ReadCPDataMapL
// -----------------------------------------------------------------------------
//
void CVcxNsPublishableDataItem::ReadCPDataMapL( CLiwDefaultMap& aDataMap )
    {
    // read all data keys saved to this item, extract the corresponding 
    // data from the liwdefaultmap and save it to this object
    
    // clear first
    ClearData();
    
    TInt count = aDataMap.Count();
    
    for( TInt i = 0; i < count; i++ )
        {
        TPtrC8 key = aDataMap.AtL( i );

        TLiwVariant variant;
        variant.PushL();
        ExtractVariant( key, aDataMap, variant );
    
        if( variant.TypeId() == LIW::EVariantTypeDesC )
            {
            SetDataL( key, variant.AsDes() );
            }
        else if(  variant.TypeId() == LIW::EVariantTypeTInt32 )
            {
            SetDataL( key, variant.AsTInt32() );
            }
        CleanupStack::PopAndDestroy( &variant );   
        }
    }

// -----------------------------------------------------------------------------
// CVcxNsPublishableDataItem::ExtractVariant
// -----------------------------------------------------------------------------
//
TInt CVcxNsPublishableDataItem::ExtractVariant( const TDesC8& aKey,  
                                                CLiwDefaultMap& aDataMap, 
                                                TLiwVariant& aVariant )
    {
    TInt err( KErrNotFound );
    TBool found( EFalse );
    TRAP( err, found = aDataMap.FindL( aKey, aVariant ) );
    if( err == KErrNone && found )
        {
        return KErrNone;        
        }
    return err;
    }

