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
* Description:     Stub class for unit testing*
*/




// INCLUDE FILES

#include <centralrepository.h>

#include "VcxHgMyVideosCategoryListImpl.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryListImpl* CVcxHgMyVideosCategoryListImpl::NewL(
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
    {
    CVcxHgMyVideosCategoryListImpl* self = 
        CVcxHgMyVideosCategoryListImpl::NewLC( aModel, aView, aScroller );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryListImpl* CVcxHgMyVideosCategoryListImpl::NewLC(
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
    {
    CVcxHgMyVideosCategoryListImpl* self = 
        new (ELeave) CVcxHgMyVideosCategoryListImpl( aModel, aView, aScroller );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryListImpl::CVcxHgMyVideosCategoryListImpl( 
        CVcxHgMyVideosModel& aModel,
        CVcxHgMyVideosMainView& aView,
        CHgScroller& aScroller )
    : CVcxHgMyVideosListBase( aModel, aView, aScroller ) 
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryListImpl::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosCategoryListImpl::~CVcxHgMyVideosCategoryListImpl()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryListImpl::DoListActivateL( TInt aIndex )
    {  
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryListImpl::DoListDeactivate()
    {    
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
TInt CVcxHgMyVideosCategoryListImpl::Highlight()
    {
    return 0;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryListImpl::HandleSelectL( TInt /*aIndex*/ )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
void CVcxHgMyVideosCategoryListImpl::HandleOpenL( TInt /*aIndex*/ )
    {

    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryListImpl::SetEmptyListTextL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosCategoryListImpl::SetTitleTextL( TInt /*aIndex*/ )
    {
    }
