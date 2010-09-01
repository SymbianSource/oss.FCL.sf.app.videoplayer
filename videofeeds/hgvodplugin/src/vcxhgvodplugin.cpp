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
* Description:    HG VOD UI plugin implementation*
*/



// INCLUDE FILES
#include "vcxhgvodmainview.h"
#include "vcxhgvodplugin.h"
#include <mpxviewutility.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CVcxHgVodPlugin::CVcxHgVodPlugin()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVcxHgVodPlugin::ConstructL()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CVcxHgVodPlugin* CVcxHgVodPlugin::NewL( )
    {
    CVcxHgVodPlugin* self = 
        new ( ELeave ) CVcxHgVodPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxHgVodPlugin::~CVcxHgVodPlugin()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CVcxHgVodPlugin::ConstructViewLC()
    {
    return CVcxHgVodMainView::NewLC();
    }
