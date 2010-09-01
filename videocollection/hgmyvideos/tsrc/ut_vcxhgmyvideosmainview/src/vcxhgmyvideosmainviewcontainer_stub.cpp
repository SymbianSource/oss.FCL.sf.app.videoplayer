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

#include "VcxHgMyVideosMainViewContainer.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// 
CVcxHgMyVideosMainViewContainer::CVcxHgMyVideosMainViewContainer(
    CVcxHgMyVideosMainView& aParentView )
    : iParentView( aParentView )
	{
	}
  
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosMainViewContainer* CVcxHgMyVideosMainViewContainer::NewL(
    CVcxHgMyVideosMainView& aParentView )
	{
    CVcxHgMyVideosMainViewContainer* self = NewLC( aParentView );
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
CVcxHgMyVideosMainViewContainer* CVcxHgMyVideosMainViewContainer::NewLC(
    CVcxHgMyVideosMainView& aParentView )
	{
    CVcxHgMyVideosMainViewContainer* self = 
        new (ELeave) CVcxHgMyVideosMainViewContainer( aParentView );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosMainViewContainer::~CVcxHgMyVideosMainViewContainer()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosMainViewContainer::ConstructL()
	{
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool CVcxHgMyVideosMainViewContainer::IsActivated() const
    {
    return EFalse;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
TKeyResponse CVcxHgMyVideosMainViewContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    TKeyResponse response( EKeyWasNotConsumed );

    return response;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void CVcxHgMyVideosMainViewContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    }
