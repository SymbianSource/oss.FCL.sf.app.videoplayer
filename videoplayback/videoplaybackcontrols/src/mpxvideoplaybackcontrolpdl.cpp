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
* Description:  Implementation of CMPXVideoPlaybackControlPdl
*
*/

// Version : %version: 4 %



// INCLUDE FILES

#include "mpxvideoplaybackcontrolpdl.h"
#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrolscontroller.h"

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlPdl::CMPXVideoPlaybackControlPdl()
//   C++ default constructor can NOT contain any code, that might leave.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControlPdl::CMPXVideoPlaybackControlPdl(
    CMPXVideoPlaybackControlsController* aController,
    CCoeControl* aControl,
    TMPXVideoPlaybackControls aControlIndex,
    TUint aProperties )
    : CMPXVideoPlaybackControl( aController, aControl, aControlIndex, aProperties )
{
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlPdl::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControlPdl* CMPXVideoPlaybackControlPdl::NewL(
    CMPXVideoPlaybackControlsController* aController,
    CCoeControl* aControl,
    TRect aRect,
    TMPXVideoPlaybackControls aControlIndex,
    TUint aProperties )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlPdl::NewL()"));

    CMPXVideoPlaybackControlPdl* self = new (ELeave) CMPXVideoPlaybackControlPdl( aController, aControl, aControlIndex, aProperties );

    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();

    return self;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlPdl::~CMPXVideoPlaybackControlPdl()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControlPdl::~CMPXVideoPlaybackControlPdl()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlPdl::~CMPXVideoPlaybackControlPdl()"));
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlPdl::MakeVisible()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlPdl::MakeVisible( TBool aVisible )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlPdl::MakeVisible(%d)"), aVisible);

    CCoeControl::MakeVisible( EFalse );
    CCoeControl::MakeVisible( ETrue );
}

// End of File
