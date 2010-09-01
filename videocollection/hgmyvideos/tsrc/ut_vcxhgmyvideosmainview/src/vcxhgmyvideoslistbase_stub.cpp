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


#include <vcxhgmyvideos.rsg>
#include "vcxhgmyvideoslistbase.h"


#include <coemain.h>
#include <aknappui.h>
#include <aknviewappui.h> 
#include <aknview.h>
#include "vcxviewmessageutility.h"
#include <akntitle.h>

#define private public
#include "vcxhgmyvideosmainview.h"
#undef private


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//
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
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosListBase::~CVcxHgMyVideosListBase()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::DoListActivateL( TInt aIndex )
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::DoListDeactivate()
    {
    }

// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::UpdateLayout( TRect& aRect )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::SetEmptyListTextByResourceL( TInt aResourceId )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosListBase::SetTitleTextByResourceL( TInt aResourceId )
    {
    }
