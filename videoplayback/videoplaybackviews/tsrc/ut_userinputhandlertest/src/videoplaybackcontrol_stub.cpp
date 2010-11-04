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
* Description:  Implementation of STIF Video Playback Control
*
*/

// Version : %version: ou1cpsw#3 %


// INCLUDE FILES
#include <avkon.hrh>
#include <e32std.h>
#include <aknutils.h>

#include "videoplaybackcontrol_stub.h"
#include "mpxvideo_debug.h"

//
// extension of TPointerEvent::TType in w32std.h
//
TInt KControlPtrEventNull = 20;

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::CMPXVideoPlaybackControl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControl::CMPXVideoPlaybackControl()
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControl::ConstructL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::ConstructL()"));

    //
    // pre-set defaults
    //
    iKeyEvent.iScanCode = EStdKeyNull;
    iEventCode = EEventNull;
    iPointerEvent.iType = (TPointerEvent::TType) KControlPtrEventNull;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControl* CMPXVideoPlaybackControl::NewL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::NewL()"));

    CMPXVideoPlaybackControl* self =
        new (ELeave) CMPXVideoPlaybackControl();

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::~CMPXVideoPlaybackControl()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControl::~CMPXVideoPlaybackControl()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::~CMPXVideoPlaybackControl()"));
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Gets the number of controls contained in a compound control.
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackControl::CountComponentControls() const
{
    return 0;
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Gets an indexed component of a compound control.
// -------------------------------------------------------------------------------------------------
//
CCoeControl* CMPXVideoPlaybackControl::ComponentControl( TInt /*aIndex*/ ) const
{
    return NULL;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::Draw()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControl::Draw(const TRect& /* aRect */ ) const
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::Draw()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::DoHandlePointerEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControl::DoHandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::DoHandlePointerEventL()"));
    iPointerEvent = aPointerEvent;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::DoHandleKeyEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControl::DoHandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::DoHandleKeyEventL()"));

    iKeyEvent = aKeyEvent;
    iEventCode = aType;
}

// End of File
