/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation of HAL stub for STIF
*
*/

// Version : %version: ou1cpsw#2 %


// INCLUDE FILES
#include <e32std.h>

#include "hal_stub.h"
#include "mpxvideo_debug.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// HAL::HAL()
// C++ default constructor can NOT contain any code, that
// might leave.
// -------------------------------------------------------------------------------------------------
//
HAL::HAL()
{
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::~CMPXVideoPlaybackControl()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
HAL::~HAL()
{
    MPX_DEBUG(_L("HAL::~HAL()"));
}

// -------------------------------------------------------------------------------------------------
// Gets the attribute value.
// -------------------------------------------------------------------------------------------------
//
TInt HAL::Get( TAttribute aAttribute, TInt& aValue )
{
    switch ( aAttribute )
    {
        case EBacklightState:
        {
            aValue = iBacklightState;
            break;
        }
        default:
            break;
    }
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Gets an indexed component of a compound control.
// -------------------------------------------------------------------------------------------------
//
TInt HAL::Set( TAttribute aAttribute, TInt aValue )
{
    switch ( aAttribute )
    {
        case EBacklightState:
        {
            iBacklightState =  aValue;
            break;
        }
        default:
            break;
    }
    return KErrNone;
}


// End of File
