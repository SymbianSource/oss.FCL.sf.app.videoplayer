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
#include "vcxnsscheduleview.h"
#include "vcxnsscheduleplugin.h"
#include <mpxviewutility.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CVcxNsSchedulePlugin::CVcxNsSchedulePlugin()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVcxNsSchedulePlugin::ConstructL()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CVcxNsSchedulePlugin* CVcxNsSchedulePlugin::NewL( )
    {
    CVcxNsSchedulePlugin* self = 
        new ( ELeave ) CVcxNsSchedulePlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxNsSchedulePlugin::~CVcxNsSchedulePlugin()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// From CMPXAknViewPlugin
// Construct Avkon view.
// ---------------------------------------------------------------------------
//
CAknView* CVcxNsSchedulePlugin::ConstructViewLC()
    {
    return CVcxNsScheduleView::NewLC();
    }
