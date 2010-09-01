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



#include "vcxnscategory.h"

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
CVcxNsCategory::CVcxNsCategory()
    : iContentIndex( 0 )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CVcxNsCategory::~CVcxNsCategory( )
    {
    delete iName;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CVcxNsCategory* CVcxNsCategory::NewL( )
    {
    CVcxNsCategory* self = new ( ELeave ) CVcxNsCategory;
    return self;
    }

// -----------------------------------------------------------------------------
// Get category name
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CVcxNsCategory::GetName()
    {
    if( iName && iName->Length() > 0 )
        {
        return *iName;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// Get category id
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CVcxNsCategory::GetCategoryId()
    {
    return iCategoryId;
    }

// -----------------------------------------------------------------------------
// Get video count
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVcxNsCategory::GetVideoCount()
    {
    return iVideoCount;
    }

// -----------------------------------------------------------------------------
// Set category name
// -----------------------------------------------------------------------------
//
void CVcxNsCategory::SetNameL( const TDesC& aName )
    {
    delete iName;
    iName = NULL;
    
    if( aName.Length() > 0 )
        {
        iName = aName.AllocL();
        } 
    }

// -----------------------------------------------------------------------------
// Set category id
// -----------------------------------------------------------------------------
//
void CVcxNsCategory::SetCategoryId( const TUint32 aCategoryId )
    {
    iCategoryId = aCategoryId;
    }

// -----------------------------------------------------------------------------
// Set video count
// -----------------------------------------------------------------------------
//
void CVcxNsCategory::SetVideoCount( const TInt aVideoCount )
    {
    iVideoCount = aVideoCount;
    }

// -----------------------------------------------------------------------------
// Set content index
// -----------------------------------------------------------------------------
//
void CVcxNsCategory::SetHighlightContentIndex( TInt aIndex )
    {
    iContentIndex = aIndex;
    }

// -----------------------------------------------------------------------------
// CVcxNsCategory::ContentIndex()
// -----------------------------------------------------------------------------
//
TInt CVcxNsCategory::HighlightContentIndex( )
    {
    return iContentIndex;
    }

