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

// Version : %version: 4 %


// INCLUDE FILES
#include <qobject>
#include <e32std.h>
#include <w32std.h> // RWindowBase
#include <e32base.h>
#include <aknutils.h>
#include <eikclbd.h>
#include <aknconsts.h>
#include <remconcoreapi.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h> // Side volume key
#include <mpxplaybackframeworkdefs.h>
#include <centralrepository.h>  // for peripheral display timeout setting
#include <settingsinternalcrkeys.h> // display timeout setting keys
#include <hwrmlightdomaincrkeys.h>
#include <mpxvideoplaybackdefs.h>

#include <HAL.h>
#include <hal_data.h>

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
// CMPXVideoPlaybackUserInputHandler::MrccatoPlay()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::MrccatoPlay( TRemConCoreApiPlaybackSpeed /*aSpeed*/,
                                                     TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED( aButtonAct );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::MrccatoCommand()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                                       TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED( aOperationId );
    Q_UNUSED( aButtonAct );
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::DoHandleMediaKey()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::DoHandleMediaKey( TRemConCoreApiOperationId aOperationId,
                                                          TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED( aOperationId );
    Q_UNUSED( aButtonAct );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::HandleFastForward()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleFastForward(TRemConCoreApiButtonAction aButtonAct)
{
    Q_UNUSED( aButtonAct );
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::HandleRewind()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleRewind(TRemConCoreApiButtonAction aButtonAct)
{
    Q_UNUSED( aButtonAct );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::HandleVolumeUp()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleVolumeUp(TRemConCoreApiButtonAction aButtonAct)
{
    Q_UNUSED( aButtonAct );
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::HandleVolumeDown()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleVolumeDown(TRemConCoreApiButtonAction aButtonAct)
{
    Q_UNUSED( aButtonAct );
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
// CMPXVideoPlaybackUserInputHandler::ProcessMediaKey()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::ProcessMediaKey(TRemConCoreApiOperationId aOperationId,
                                       					TRemConCoreApiButtonAction aButtonAct)
{
    Q_UNUSED( aOperationId );
    Q_UNUSED( aButtonAct );
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL( TAny* aPtr )
{
    Q_UNUSED( aPtr );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatL()
{
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
// CMPXVideoPlaybackUserInputHandler::DisableBacklight()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::DisableBacklight()
{

}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::EnableBacklight()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::EnableBacklight()
{
    
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::HandleTVOutEvent()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleTVOutEventL( TBool aTVOutConnected )
{
    Q_UNUSED( aTVOutConnected );
}



// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::HandleDisplayTimeout
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackUserInputHandler::HandleDisplayTimeout( TAny* aPtr )
{
    Q_UNUSED( aPtr );
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::RestartDisplayTimer
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::RestartDisplayTimer()
{
                   
}

// EOF
