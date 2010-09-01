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
* Description:      Base class for Hg based video list implementations.*
*/




// INCLUDE FILES
#include <aknViewAppUi.h>
#include <StringLoader.h>
#include <utf.h>

#include <ganes/HgScroller.h>

#include <vcxhgmyvideos.rsg>
#include "vcxhgmyvideoslistbase.h"
#include "vcxhgmyvideosmainview.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosListBase::CVcxHgMyVideosListBase()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosListBase::CVcxHgMyVideosListBase( 
    CVcxHgMyVideosModel& aModel,
    CVcxHgMyVideosMainView& aView,
    CHgScroller& aScroller )
    : iModel( aModel ),
      iView( aView ),
      iScroller( &aScroller )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosListBase::ConstructL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::ConstructL()
    {
    iScroller->MakeVisible( EFalse );
    iScroller->SetMopParent( &iView );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosListBase::~CVcxHgMyVideosListBase()
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosListBase::~CVcxHgMyVideosListBase()
    {
    iView.AppUi()->RemoveFromStack( iScroller );
    delete iScroller;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosListBase::DoListActivateL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::DoListActivateL( TInt aIndex )
    {
    iView.AppUi()->AddToStackL( iScroller );
    iScroller->MakeVisible( ETrue );
    SetEmptyListTextL();
    SetTitleTextL( aIndex );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosListBase::DoListDeactivate() 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::DoListDeactivate()
    {
    iView.AppUi()->RemoveFromStack( iScroller );
    iScroller->MakeVisible( EFalse );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosListBase::UpdateLayout()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::UpdateLayout( TRect& aRect )
    {
    if ( aRect != iScroller->Rect() )
        {
        TRAP_IGNORE( iScroller->InitScreenL( aRect ) );
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosListBase::SetEmptyListTextByResourceL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::SetEmptyListTextByResourceL( TInt aResourceId )
    {
    HBufC* textFromResourceFile = StringLoader::LoadLC( aResourceId );
    iScroller->SetEmptyTextL( *textFromResourceFile );
    CleanupStack::PopAndDestroy( textFromResourceFile );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosListBase::SetTitleTextByResourceL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::SetTitleTextByResourceL( TInt aResourceId )
    {
    HBufC* title = StringLoader::LoadLC( aResourceId );
    iView.TitlePaneL()->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
    }
