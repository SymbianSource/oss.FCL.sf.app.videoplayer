/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CVcxHgMyVideosPlugin class implementation*
*/



// INCLUDE FILES
#include "vcxhgmyvideosmainview.h"
#include "vcxhgmyvideosplugin.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosPlugin::CVcxHgMyVideosPlugin()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosPlugin::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosPlugin* CVcxHgMyVideosPlugin::NewL()
    {
    CVcxHgMyVideosPlugin* self = new ( ELeave ) CVcxHgMyVideosPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosPlugin::~CVcxHgMyVideosPlugin()
    {
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin. Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CVcxHgMyVideosPlugin::ConstructViewLC()
    {
    return CVcxHgMyVideosMainView::NewLC();
    }
