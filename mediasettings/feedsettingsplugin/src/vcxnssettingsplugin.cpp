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
* Description:    NSeries VOD UI plugin implementation*
*/



// INCLUDE FILES
#include "feedsettingsview.h"
#include "vcxnssettingsplugin.h"
#include <mpxviewutility.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CVcxNsSettingsPlugin::CVcxNsSettingsPlugin()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVcxNsSettingsPlugin::ConstructL()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CVcxNsSettingsPlugin* CVcxNsSettingsPlugin::NewL( )
    {
    CVcxNsSettingsPlugin* self = 
        new ( ELeave ) CVcxNsSettingsPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxNsSettingsPlugin::~CVcxNsSettingsPlugin()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CVcxNsSettingsPlugin::ConstructViewLC()
    {
    return CVcxNsSettingsView::NewLC();
    }
