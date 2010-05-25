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
* Description:   CVcxHgMyVideosVideoListItem class implementation*
*/




// INCLUDE FILES
#include <mpxmedia.h>
#include <collate.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include "vcxhgmyvideosvideolistitem.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListItem::NewL()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListItem* CVcxHgMyVideosVideoListItem::NewL( CMPXMedia* aMPXMedia )
    {
    CVcxHgMyVideosVideoListItem* self = 
        CVcxHgMyVideosVideoListItem::NewLC( aMPXMedia );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListItem::NewLC()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListItem* CVcxHgMyVideosVideoListItem::NewLC( CMPXMedia* aMPXMedia )
    {
    CVcxHgMyVideosVideoListItem* self = 
        new (ELeave) CVcxHgMyVideosVideoListItem( aMPXMedia );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListItem::CVcxHgMyVideosVideoListItem()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListItem::CVcxHgMyVideosVideoListItem( CMPXMedia* aMPXMedia ) : 
    iMedia( aMPXMedia )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListItem::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoListItem::ConstructL()
    {
    // No implementation required.
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListItem::~CVcxHgMyVideosVideoListItem()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosVideoListItem::~CVcxHgMyVideosVideoListItem()
    {
    delete iMedia;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListItem::CompareBySize()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoListItem::CompareBySize( 
    const CVcxHgMyVideosVideoListItem& aNewVideo,
    const CVcxHgMyVideosVideoListItem& aVideoInArray )
    {
    TInt result( 0 );
     
    TInt64 newItemsSize = aNewVideo.iMedia->ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 );
    TInt64 arrayItemsSize = aVideoInArray.iMedia->ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 );

    if ( arrayItemsSize > newItemsSize )
        {
        result = -1;
        }
    else if ( arrayItemsSize < newItemsSize )
        {
        result = 1;    
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListItem::CompareByName()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoListItem::CompareByName( 
    const CVcxHgMyVideosVideoListItem& aNewVideo,
    const CVcxHgMyVideosVideoListItem& aVideoInArray )
    {        
    TPtrC newVideoName( aNewVideo.iMedia->ValueText( KMPXMediaGeneralTitle ) );
    TPtrC videoInArrayName( aVideoInArray.iMedia->ValueText( KMPXMediaGeneralTitle ));
    
    TCollationMethod collationMethod = *Mem::CollationMethodByIndex( 0 ); // get the standard method
    collationMethod.iFlags |= TCollationMethod::EFoldCase;
    
    return newVideoName.CompareC( videoInArrayName, 3, &collationMethod );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListItem::CompareByDate()
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosVideoListItem::CompareByDate( 
    const CVcxHgMyVideosVideoListItem& aNewVideo,
    const CVcxHgMyVideosVideoListItem& aVideoInArray )
    {
    TInt result( 0 );
    
    TInt64 newItemsDate = aNewVideo.iMedia->ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
    TInt64 arrayItemsDate = aVideoInArray.iMedia->ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
    
    if ( arrayItemsDate > newItemsDate )
        {
        result = 1;
        }
    else if ( arrayItemsDate < newItemsDate )
        {
        result = -1; 
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoListItem::Media()
// -----------------------------------------------------------------------------
//
CMPXMedia* CVcxHgMyVideosVideoListItem::Media()
    {
    return iMedia;
    }
