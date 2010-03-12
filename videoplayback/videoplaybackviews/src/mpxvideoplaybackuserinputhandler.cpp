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
* Description:  Implementation of playback view's input handler
*
*/

// Version : %version: 15 %


// INCLUDE FILES
#include <w32std.h> // RWindowBase
#include <e32base.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <eikclbd.h>
#include <aknconsts.h>
#include <AknUtils.h>
#include <AknIconUtils.h>  // SetSize for the icons
#include <remconcoreapi.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h> // Side volume key
#include <mpxplaybackframeworkdefs.h>
#include <centralrepository.h>  // for peripheral display timeout setting
#include <settingsinternalcrkeys.h> // display timeout setting keys
#include <hwrmlightdomaincrkeys.h>
#include <e32std.h>

#ifdef __USERINPUTHANDLERSTIF__
#include "videoplaybackcontainer_stub.h"
#include "videoplaybackcontrol_stub.h"
#include "hal_stub.h"
#else
#include "mpxvideoplaybackcontainer.h"
#include "mpxvideoplaybackcontrol.h"
#include <hwrmlight.h> //light status
#endif

#include "mpxvideoplaybackcontrol.hrh"
#include "mpxvideoplaybackuserinputhandler.h"

#include <mpxvideoplaybackdefs.h>
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideo_debug.h"


// CONSTANTS
const TInt KMPXMicroSecondsInASecond = 1000000;


// ======== MEMBER FUNCTIONS =======================================================================

// -------------------------------------------------------------------------------------------------
// MPXVideoPlaybackUserInputHandler::CMPXVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackUserInputHandler::CMPXVideoPlaybackUserInputHandler(
        CMPXVideoPlaybackContainer* aContainer)
   : iContainer(aContainer)
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::NewL()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackUserInputHandler* CMPXVideoPlaybackUserInputHandler::NewL(
        CMPXVideoPlaybackContainer* aContainer, TBool aTvOutConnected)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::NewL()"));

    CMPXVideoPlaybackUserInputHandler* self =
        new (ELeave) CMPXVideoPlaybackUserInputHandler( aContainer );
    CleanupStack::PushL( self );
    self->ConstructL(aTvOutConnected);
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
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackUserInputHandler::ConstructL()"));

    iVolumeRepeatTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    iDisplayTimer = CPeriodic::NewL( CPeriodic::EPriorityStandard );
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );

    iTVOutConnected = aTvOutConnected;

    iLight = CHWRMLight::NewL();

    // Start the timer if TV out is connected
    if ( iTVOutConnected )
    {
        // Get the display light time-out value from CenRep
        CRepository* repository = CRepository::NewLC( KCRUidLightSettings  );

        // What's the timeout value (in seconds ) for the display light?
        repository->Get( KDisplayLightsTimeout, iDisplayTimeOut );
        MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::ConstructL Display Timeout( %d )"), iDisplayTimeOut);

        CleanupStack::PopAndDestroy( repository );

        // Convert the timeout value to microseconds
        iDisplayTimeOut *= KMPXMicroSecondsInASecond;

        RestartDisplayTimer();
    }

    // not detrimental if Media Keys dont work - so ignore any errors here
    TRAP_IGNORE( iInterfaceSelector->OpenTargetL() );

    iProcessingInputType = EMpxVideoNone;
    iForeground = ETrue;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::~CMPXVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackUserInputHandler::~CMPXVideoPlaybackUserInputHandler()
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

    if ( iLight )
    {
        delete iLight;
        iLight = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::MrccatoPlay()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::MrccatoPlay( TRemConCoreApiPlaybackSpeed /*aSpeed*/,
                                                     TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackUserInputHandler::MrccatoPlay"),
        _L("aButtonAct = %d"), aButtonAct );

    ProcessMediaKey(ERemConCoreApiPlay, aButtonAct);
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::MrccatoCommand()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                                       TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackUserInputHandler::MrccatoCommand"),
        _L("aButtonAct = %d"), aButtonAct );

    ProcessMediaKey(aOperationId, aButtonAct);
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::DoHandleMediaKey()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::DoHandleMediaKey( TRemConCoreApiOperationId aOperationId,
                                                          TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackUserInputHandler::DoHandleMediaKey()"));

    switch ( aOperationId )
    {
        case ERemConCoreApiStop:
        {
            if ( aButtonAct == ERemConCoreApiButtonClick )
            {
                //
                //  When the stop key is pressed from the remote control interface,
                //  issue a softkey back so all views will exit the playback view.
                //
                TRAP_IGNORE( iContainer->HandleCommandL( EAknSoftkeyBack ) );
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
                TRAP_IGNORE(iContainer->HandleCommandL(EMPXPbvCmdPlayPause));
            }
            break;
        }
        case ERemConCoreApiPause:
        {
            TRAP_IGNORE(iContainer->HandleCommandL(EMPXPbvCmdPause));
            break;
        }
        case ERemConCoreApiPlay:
        {
            if ( aButtonAct == ERemConCoreApiButtonClick )
            {
                TRAP_IGNORE(iContainer->HandleCommandL(EMPXPbvCmdPlay));
            }
            break;
        }
        default:
            break;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::HandleFastForward()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleFastForward(TRemConCoreApiButtonAction aButtonAct)
{
    if (aButtonAct == ERemConCoreApiButtonPress)
    {
        TRAP_IGNORE(iContainer->HandleCommandL(EMPXPbvCmdSeekForward));
    }
    else if (aButtonAct == ERemConCoreApiButtonRelease)
    {
        TRAP_IGNORE(iContainer->HandleCommandL(EMPXPbvCmdEndSeek));
    }
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::HandleRewind()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleRewind(TRemConCoreApiButtonAction aButtonAct)
{
    if (aButtonAct == ERemConCoreApiButtonPress)
    {
        TRAP_IGNORE(iContainer->HandleCommandL(EMPXPbvCmdSeekBackward));
    }
    else if (aButtonAct == ERemConCoreApiButtonRelease)
    {
        TRAP_IGNORE(iContainer->HandleCommandL(EMPXPbvCmdEndSeek));
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::HandleVolumeUp()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleVolumeUp(TRemConCoreApiButtonAction aButtonAct)
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
                    CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL,
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
            TRAP_IGNORE( iContainer->HandleCommandL( EMPXPbvCmdIncreaseVolume ) );
            break;
        }
    }
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::HandleVolumeDown()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleVolumeDown(TRemConCoreApiButtonAction aButtonAct)
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
                    CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL,
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
            TRAP_IGNORE( iContainer->HandleCommandL( EMPXPbvCmdDecreaseVolume ) );
            break;
        }
    }
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::ProcessPointerEvent()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void
CMPXVideoPlaybackUserInputHandler::ProcessPointerEventL( CCoeControl* aControl,
                                                         const TPointerEvent& aPointerEvent,
                                                         TMPXVideoControlType aMPXControl )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::ProcessPointerEvent"));

    switch (iProcessingInputType)
    {
        case EMpxVideoNone:
        {
            if (aPointerEvent.iType == TPointerEvent::EButton1Down && iForeground)
            {
                iProcessingInputType = EMpxVideoTouch;
                if ( iTVOutConnected )
                {
                    RestartDisplayTimer();
                }
                ReRoutePointerEventL(aControl, aPointerEvent, aMPXControl);
            }
            break;
        }
        case EMpxVideoTouch:
        {
            if (aPointerEvent.iType != TPointerEvent::EButton1Down)
            {
                ReRoutePointerEventL(aControl, aPointerEvent, aMPXControl);

                // reset the value only on pointer up event - but not on drag
                if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                {
                    iProcessingInputType = EMpxVideoNone;
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
// CMPXVideoPlaybackUserInputHandler::ProcessKeyEvent()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                                          TEventCode aType )
{
    MPX_DEBUG(_L("MPXVideoPlaybackUserInputHandler::ProcessKeyEvent"));

    switch (iProcessingInputType)
    {
        case EMpxVideoNone:
        {
            if (aType == EEventKeyDown && iForeground)
            {
                iProcessingInputType = EMpxVideoKeyboard;
                iLastPressedKeyCode = aKeyEvent.iCode;
                iLastPressedKeyScanCode = aKeyEvent.iScanCode;
                if ( iTVOutConnected )
                {
                    RestartDisplayTimer();
                }
                iContainer->DoHandleKeyEventL(aKeyEvent, aType);
            }
            break;
        }
        case EMpxVideoKeyboard:
        {
            if (aType == EEventKeyUp)
            {
                // only handle up event for the key being handled
                // ignore spurious key presses
                if (aKeyEvent.iCode == iLastPressedKeyCode  &&
                    aKeyEvent.iScanCode == iLastPressedKeyScanCode)
                {
                    iContainer->DoHandleKeyEventL(aKeyEvent, aType);

                    // reset the value only on key up event
                    iProcessingInputType = EMpxVideoNone;
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
// CMPXVideoPlaybackUserInputHandler::ProcessMediaKey()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::ProcessMediaKey( TRemConCoreApiOperationId aOperationId,
                                                         TRemConCoreApiButtonAction aButtonAct )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::ProcessMediaKey"));

    switch ( iProcessingInputType )
    {
        case EMpxVideoNone:
        {
            if ( aButtonAct == ERemConCoreApiButtonPress && iForeground )
            {
                iProcessingInputType = EMpxVideoMediaKeys;
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
                iProcessingInputType = EMpxVideoNone;
            }
            break;
        }
        case EMpxVideoMediaKeys:
        {
            if (aButtonAct == ERemConCoreApiButtonRelease)
            {
                // handle only if this release is for media-key being currently handled
                // ignore spurious media key presses
                if (iLastMediaKeyPressed == aOperationId)
                {
                    DoHandleMediaKey(aOperationId, aButtonAct);
                    // reset on click AND/OR release
                    iProcessingInputType = EMpxVideoNone;
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
//   CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL( TAny* aPtr )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL()"));

    static_cast<CMPXVideoPlaybackUserInputHandler*>(aPtr)->HandleVolumeRepeatL();

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::HandleVolumeRepeatL()"));

    TMPXVideoPlaybackViewCommandIds command = EMPXPbvCmdDecreaseVolume;

    if ( iVolumeRepeatUp )
    {
        command = EMPXPbvCmdIncreaseVolume;
    }

    iContainer->HandleCommandL( command );
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::ReRoutePointerEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::ReRoutePointerEventL(CCoeControl* aControl,
                                                             const TPointerEvent& aPointerEvent,
                                                             TMPXVideoControlType aMPXControl)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::ReRoutePointerEventL()"));

    if ( aMPXControl == EMpxVideoPlaybackContainer )
    {
        iContainer->DoHandlePointerEventL(aPointerEvent);
    }
    else if ( aMPXControl == EMpxVideoPlaybackControl )
    {
        static_cast<CMPXVideoPlaybackControl*>(aControl)->DoHandlePointerEventL(aPointerEvent);
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::SetForeground()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::SetForeground(TBool aForeground)
{
    iForeground = aForeground;

    if ( !iForeground )
    {
        // we are in background so reset iProcessingInputType value
        iProcessingInputType = EMpxVideoNone;
    }
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::DisableBacklight()
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::DisableBacklight()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackUserInputHandler::DisableBacklight"));

    // cancel the timer
    iDisplayTimer->Cancel();

    // disable the backlight
    MPX_TRAPD( err, 
    {
        iLight->ReserveLightL( CHWRMLight::EPrimaryDisplay );
        iLight->LightOffL( CHWRMLight::EPrimaryDisplay );
    } );
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::EnableBacklight()
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::EnableBacklight()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackUserInputHandler::EnableBacklight"));

    // enable the backlight
    MPX_TRAPD( err,
    {
        iLight->LightOnL( CHWRMLight::EPrimaryDisplay );
        iLight->ReleaseLight(CHWRMLight::EPrimaryDisplay );
    } );
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::HandleTVOutEvent()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::HandleTVOutEvent(TBool aTVOutConnected)
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackUserInputHandler::HandleTVOutEvent"));

    iTVOutConnected = aTVOutConnected;

    if ( iTVOutConnected )
    {
        // Get the display light time-out value from CenRep
        CRepository* repository = CRepository::NewLC( KCRUidLightSettings );

        // What's the timeout value (in seconds ) for the display light?
        repository->Get( KDisplayLightsTimeout, iDisplayTimeOut );
        MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::ConstructL Display Timeout( %d )"), iDisplayTimeOut);

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
//   CMPXVideoPlaybackUserInputHandler::HandleDisplayTimeout
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackUserInputHandler::HandleDisplayTimeout( TAny* aPtr )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackUserInputHandler::HandleDisplayTimeout"));

    static_cast<CMPXVideoPlaybackUserInputHandler*>(aPtr)->DisableBacklight();

    return KErrNone;
}

// -----------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::RestartDisplayTimer
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::RestartDisplayTimer()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackUserInputHandler::RestartDisplayTimer"));

    // check if the display timer is running if so cancelit
    if ( iDisplayTimer->IsActive() )
    {
        iDisplayTimer->Cancel();
    }
    else
    {
        // timeout has happened and the backlight is disabled
        // enable the backlight
        EnableBacklight();
    }

    // Re start the display backlight timer
    iDisplayTimer->Start( iDisplayTimeOut, iDisplayTimeOut,
                          TCallBack( CMPXVideoPlaybackUserInputHandler::HandleDisplayTimeout, this ) );
}

// EOF
