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

// Version : %version: 9 %


// INCLUDE FILES
#include <e32std.h>
#include <w32std.h> // RWindowBase
#include <e32base.h>
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

#include <hal.h>
#include <hal_data.h>

#include "mpxvideo_debug.h"
#include "mpxvideoviewwrapper.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "videoplaybackuserinputhandler.h"


// CONSTANTS
const TInt KMPXMicroSecondsInASecond = 1000000;


// ======== MEMBER FUNCTIONS =======================================================================

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::CVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CVideoPlaybackUserInputHandler::CVideoPlaybackUserInputHandler( CMPXVideoViewWrapper* aWrapper )
   : iViewWrapper( aWrapper )
{
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::NewL()
// -------------------------------------------------------------------------------------------------
//
CVideoPlaybackUserInputHandler* CVideoPlaybackUserInputHandler::NewL(
        CMPXVideoViewWrapper* aWrapper, TBool aTvOutConnected )
{
    MPX_DEBUG(_L("CVideoPlaybackUserInputHandler::NewL()"));

    CVideoPlaybackUserInputHandler* self =
        new (ELeave) CVideoPlaybackUserInputHandler( aWrapper );
    CleanupStack::PushL( self );
    self->ConstructL( aTvOutConnected );
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::ConstructL( TBool aTvOutConnected )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::ConstructL()"));

    iVolumeRepeatTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iDisplayTimer = CPeriodic::NewL( CPeriodic::EPriorityStandard );
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );

    iTVOutConnected = aTvOutConnected;
 
    // Start the timer if TV out is connected
    if ( iTVOutConnected )
    {                
        // Get the display light time-out value from CenRep 
        CRepository* repository = CRepository::NewLC( KCRUidLightSettings  );
    
        // What's the timeout value (in seconds ) for the display light?
        repository->Get( KDisplayLightsTimeout, iDisplayTimeOut );
        MPX_DEBUG(_L("CVideoPlaybackUserInputHandler::ConstructL Display Timeout( %d )"), iDisplayTimeOut);
    
        CleanupStack::PopAndDestroy( repository );
    
        // Convert the timeout value to microseconds
        iDisplayTimeOut *= KMPXMicroSecondsInASecond;
    
        RestartDisplayTimer();
    }

    // not detrimental if Media Keys dont work - so ignore any errors here
    TRAP_IGNORE( iInterfaceSelector->OpenTargetL() );

    iProcessingInputType = EVideoNone;
    iForeground = ETrue;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::~CVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CVideoPlaybackUserInputHandler::~CVideoPlaybackUserInputHandler()
{
    if ( iVolumeRepeatTimer )
    {
        iVolumeRepeatTimer->Cancel();
        delete iVolumeRepeatTimer;
    }

    if ( iDisplayTimer )
    {
        iDisplayTimer->Cancel();
        delete iDisplayTimer;
    }

    if ( iInterfaceSelector )
    {
        delete iInterfaceSelector;
        iCoreTarget = NULL;
        iInterfaceSelector = NULL;
    }

    // make sure that backlight enabled when 
    // the view updates or deactivates
    EnableBacklight();
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::MrccatoPlay()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::MrccatoPlay( TRemConCoreApiPlaybackSpeed /*aSpeed*/,
                                                  TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(
        _L("CVideoPlaybackUserInputHandler::MrccatoPlay"),
        _L("aButtonAct = %d"), aButtonAct );

    ProcessMediaKey(ERemConCoreApiPlay, aButtonAct);
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::MrccatoCommand()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::MrccatoCommand( TRemConCoreApiOperationId aOperationId,
                                                     TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(
        _L("CVideoPlaybackUserInputHandler::MrccatoCommand"),
        _L("aButtonAct = %d"), aButtonAct );

    ProcessMediaKey(aOperationId, aButtonAct);
}


// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::DoHandleMediaKey()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::DoHandleMediaKey( TRemConCoreApiOperationId aOperationId,
                                                       TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::DoHandleMediaKey()"));

    switch ( aOperationId )
    {
        case ERemConCoreApiStop:
        {
            if ( aButtonAct == ERemConCoreApiButtonClick )
            {
                TRAP_IGNORE(iViewWrapper->HandleCommandL( EMPXPbvCmdStop ));
            }
            break;
        }
        case ERemConCoreApiRewind:
        {
            HandleRewind(aButtonAct);
            break;
        }
        case ERemConCoreApiFastForward:
        {
            HandleFastForward(aButtonAct);
            break;
        }
        case ERemConCoreApiVolumeUp:
        {
            HandleVolumeUp(aButtonAct);
            break;
        }
        case ERemConCoreApiVolumeDown:
        {
            HandleVolumeDown(aButtonAct);
            break;
        }
        case ERemConCoreApiPausePlayFunction:
        {
            if ( aButtonAct == ERemConCoreApiButtonClick )
            {
                TRAP_IGNORE(iViewWrapper->HandleCommandL(EMPXPbvCmdPlayPause));
            }
            break;
        }
        case ERemConCoreApiPause:
        {
            TRAP_IGNORE(iViewWrapper->HandleCommandL(EMPXPbvCmdPause));
            break;
        }
        case ERemConCoreApiPlay:
        {
            if ( aButtonAct == ERemConCoreApiButtonClick )
            {
                TRAP_IGNORE(iViewWrapper->HandleCommandL(EMPXPbvCmdPlay));
            }
            break;
        }
        default:
            break;
    }
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::HandleFastForward()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleFastForward( TRemConCoreApiButtonAction aButtonAct )
{
    if (aButtonAct == ERemConCoreApiButtonPress)
    {
        TRAP_IGNORE(iViewWrapper->HandleCommandL(EMPXPbvCmdSeekForward));
    }
    else if (aButtonAct == ERemConCoreApiButtonRelease)
    {
        TRAP_IGNORE(iViewWrapper->HandleCommandL(EMPXPbvCmdEndSeek));
    }
}


// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::HandleRewind()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleRewind( TRemConCoreApiButtonAction aButtonAct )
{
    if (aButtonAct == ERemConCoreApiButtonPress)
    {
        TRAP_IGNORE(iViewWrapper->HandleCommandL(EMPXPbvCmdSeekBackward));
    }
    else if (aButtonAct == ERemConCoreApiButtonRelease)
    {
        TRAP_IGNORE(iViewWrapper->HandleCommandL(EMPXPbvCmdEndSeek));
    }
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::HandleVolumeUp()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleVolumeUp( TRemConCoreApiButtonAction aButtonAct )
{
    switch ( aButtonAct )
    {
        case ERemConCoreApiButtonPress:
        {
            // Volume Up - Pressed
            if ( iVolumeRepeatTimer->IsActive() )
            {
                iVolumeRepeatTimer->Cancel();
            }

            iVolumeRepeatUp = ETrue;
            iVolumeRepeatTimer->Start(
                KAknStandardKeyboardRepeatRate,
                KAknStandardKeyboardRepeatRate,
                TCallBack(
                    CVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL,
                    this ) );

            break;
        }
        case ERemConCoreApiButtonRelease:
        {
            // Volume Up - Released
            iVolumeRepeatTimer->Cancel();
            break;
        }
        case ERemConCoreApiButtonClick:
        {
            // Volume Up - Clicked
            TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdIncreaseVolume ) );
            break;
        }
    }
}


// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::HandleVolumeDown()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleVolumeDown( TRemConCoreApiButtonAction aButtonAct )
{
    switch ( aButtonAct )
    {
        case ERemConCoreApiButtonPress:
        {
            // Volume Up - Pressed
            if ( iVolumeRepeatTimer->IsActive() )
            {
                iVolumeRepeatTimer->Cancel();
            }

            iVolumeRepeatUp = EFalse;
            iVolumeRepeatTimer->Start(
                KAknStandardKeyboardRepeatRate,
                KAknStandardKeyboardRepeatRate,
                TCallBack(
                    CVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL,
                    this ) );

            break;
        }
        case ERemConCoreApiButtonRelease:
        {
            // Volume Up - Released
            iVolumeRepeatTimer->Cancel();
            break;
        }
        case ERemConCoreApiButtonClick:
        {
            // Volume Down - Clicked
            TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdDecreaseVolume ) );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::ProcessKeyEvent()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::ProcessKeyEventL( const TKeyEvent& /*aKeyEvent*/,
                                                       TEventCode /*aType*/ )
{
    MPX_DEBUG(_L("VideoPlaybackUserInputHandler::ProcessKeyEvent"));

    /*
    switch (iProcessingInputType)
    {
        case EVideoNone:
        {
            if (aType == EEventKeyDown && iForeground)
            {
                iProcessingInputType = EVideoKeyboard;
                iLastPressedKeyCode = aKeyEvent.iCode;
                iLastPressedKeyScanCode = aKeyEvent.iScanCode;
                if ( iTVOutConnected )
                {    
                    RestartDisplayTimer();
                }
                iViewWrapper->DoHandleKeyEventL( aKeyEvent, aType );
            }
            break;
        }
        case EVideoKeyboard:
        {
            if (aType == EEventKeyUp)
            {
                // only handle up event for the key being handled
                // ignore spurious key presses
                if (aKeyEvent.iCode == iLastPressedKeyCode  &&
                    aKeyEvent.iScanCode == iLastPressedKeyScanCode)
                {
                    iViewWrapper->DoHandleKeyEventL( aKeyEvent, aType );

                    // reset the value only on key up event
                    iProcessingInputType = EVideoNone;
                }
            }
            break;
        }
        default:
        {
            // user input is disallowed
            break;
        }
    } // switch*/
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::ProcessMediaKey()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::ProcessMediaKey( TRemConCoreApiOperationId aOperationId,
                                                      TRemConCoreApiButtonAction aButtonAct )
{
    MPX_DEBUG(_L("CVideoPlaybackUserInputHandler::ProcessMediaKey"));

    switch (iProcessingInputType)
    {
        case EVideoNone:
        {
            if (aButtonAct == ERemConCoreApiButtonPress && iForeground)
            {
                iProcessingInputType = EVideoMediaKeys;
                iLastMediaKeyPressed = aOperationId;
                DoHandleMediaKey(aOperationId, aButtonAct);
                if ( iTVOutConnected )
                {    
                    RestartDisplayTimer();
                }
            }
            else if (aButtonAct == ERemConCoreApiButtonClick && iForeground)
            {
                DoHandleMediaKey(aOperationId, aButtonAct);
                if ( iTVOutConnected )
                {    
                    RestartDisplayTimer();
                }
                // reset on click AND/OR release
                iProcessingInputType = EVideoNone;
            }
            break;
        }
        case EVideoMediaKeys:
        {
            if (aButtonAct == ERemConCoreApiButtonRelease)
            {
                // handle only if this release is for media-key being currently handled
                // ignore spurious media key presses
                if (iLastMediaKeyPressed == aOperationId)
                {
                    DoHandleMediaKey(aOperationId, aButtonAct);
                    // reset on click AND/OR release
                    iProcessingInputType = EVideoNone;
                }
            }
            break;
        }
        default:
        {
            // user input is disallowed
            break;
        }
    } // switch
}


// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL()
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL( TAny* aPtr )
{
    MPX_DEBUG(_L("CVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL()"));

    static_cast<CVideoPlaybackUserInputHandler*>(aPtr)->HandleVolumeRepeatL();

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::HandleVolumeRepeatL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleVolumeRepeatL()
{
    MPX_DEBUG(_L("CVideoPlaybackUserInputHandler::HandleVolumeRepeatL()"));

    TMPXVideoPlaybackViewCommandIds command = EMPXPbvCmdDecreaseVolume;

    if ( iVolumeRepeatUp )
    {
        command = EMPXPbvCmdIncreaseVolume;
    }

    iViewWrapper->HandleCommandL( command );
}


// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::SetForeground()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::SetForeground( TBool aForeground )
{
    iForeground = aForeground;

    if ( !iForeground )
    {
        // we are in background so reset iProcessingInputType value
        iProcessingInputType = EVideoNone;
    }
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::DisableBacklight()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::DisableBacklight()
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::DisableBacklight"));
    
    // cancel the timer
    iDisplayTimer->Cancel(); 

    // disable the backlight
    HAL::Set( HALData::EBacklightState, 0 );
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::EnableBacklight()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::EnableBacklight()
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::EnableBacklight"));
 
    // enable the backlight
    HAL::Set( HALData::EBacklightState, 1 );    
}


// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::HandleTVOutEvent()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleTVOutEventL(TBool aTVOutConnected)
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::HandleTVOutEvent"));

    iTVOutConnected = aTVOutConnected;

    if ( iTVOutConnected )
    {
        // Get the display light time-out value from CenRep 
        CRepository* repository = CRepository::NewLC( KCRUidLightSettings );
    
        // What's the timeout value (in seconds ) for the display light?
        repository->Get( KDisplayLightsTimeout, iDisplayTimeOut );
        MPX_DEBUG(_L("CVideoPlaybackUserInputHandler::ConstructL Display Timeout( %d )"), iDisplayTimeOut);
    
        CleanupStack::PopAndDestroy( repository );
    
        // Convert the timeout value to microseconds
        iDisplayTimeOut *= KMPXMicroSecondsInASecond;
    
        RestartDisplayTimer();
    }
    else
    {
        iDisplayTimer->Cancel(); 
        EnableBacklight();
    }
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::HandleDisplayTimeout
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackUserInputHandler::HandleDisplayTimeout( TAny* aPtr )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::HandleDisplayTimeout"));

    static_cast<CVideoPlaybackUserInputHandler*>(aPtr)->DisableBacklight();

    return KErrNone;
}

// -----------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::RestartDisplayTimer
// -----------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::RestartDisplayTimer()
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::RestartDisplayTimer"));

    // check if the display timer is running if so cancelit
    if ( iDisplayTimer->IsActive() )
    {
        iDisplayTimer->Cancel(); 
    }
    else
    {
        // timeout has happened and the backlight is disabled
        // enable the backlight
        HAL::Set( HALData::EBacklightState, 1 );    
    }
    
    TBool backlightState;    
    TInt ret = HAL::Get( HALData::EBacklightState, backlightState );    

    // Re start the display backlight timer
    iDisplayTimer->Start( iDisplayTimeOut, iDisplayTimeOut,
                          TCallBack( CVideoPlaybackUserInputHandler::HandleDisplayTimeout, this ) );                     
}

// EOF
