/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of playback view's input handler
*
*/

// Version : %version: 6 %


// INCLUDE FILES
#include <qobject>
#include <e32std.h>
#include <w32std.h> // RWindowBase
#include <e32base.h>
#include <eikclbd.h>
#include <aknconsts.h>
#include <mpxplaybackframeworkdefs.h>
#include <hwrmlightdomaincrkeys.h>
#include <mpxvideoplaybackdefs.h>

#include "mpxvideo_debug.h"
#include "mpxvideoviewwrapper.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "../inc/mpxvideoplaybackuserinputhandler.h"


// CONSTANTS
const TInt KMPXMicroSecondsInASecond = 1000000;


// ======== MEMBER FUNCTIONS =======================================================================

// -------------------------------------------------------------------------------------------------
// MPXVideoPlaybackUserInputHandler::CMPXVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackUserInputHandler::CMPXVideoPlaybackUserInputHandler( CMPXVideoViewWrapper* aWrapper )
   : iViewWrapper( aWrapper )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::NewL()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackUserInputHandler* CMPXVideoPlaybackUserInputHandler::NewL(
        CMPXVideoViewWrapper* aWrapper, TBool aTvOutConnected )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::NewL()"));

    CMPXVideoPlaybackUserInputHandler* self =
        new (ELeave) CMPXVideoPlaybackUserInputHandler( aWrapper );
    CleanupStack::PushL( self );
    self->ConstructL( aTvOutConnected );
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::ConstructL( TBool aTvOutConnected )
{
    Q_UNUSED( aTvOutConnected );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::~CMPXVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackUserInputHandler::~CMPXVideoPlaybackUserInputHandler()
{

}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::ProcessKeyEvent()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                                          TEventCode aType )
{
    Q_UNUSED( aKeyEvent );
    Q_UNUSED( aType );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::SetForeground()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::SetForeground(TBool aForeground)
{
    Q_UNUSED( aForeground );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::HandleTVOutEvent()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleTVOutEventL( TBool aTVOutConnected )
{
    Q_UNUSED( aTVOutConnected );
}

// EOF
