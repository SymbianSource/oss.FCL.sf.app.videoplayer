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
* Description:  Video Playback User Input Handler Test class for STIF unit testing
*
*/

// Version : %version: 6 %


// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <e32std.h>
#include <w32std.h>
#include <coeaui.h>
#include <aknutils.h>
#include <aknviewappui.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "userinputhandlertest.h"
#include "videoplaybackcontainer_stub.h"
#include "videoplaybackcontrol_stub.h"
#include "mpxvideoplaybackuserinputhandler.h"
#include "mpxvideo_debug.h"
#include "hal_stub.h"

//
// pre-define
//
_LIT( KPtrTypeParameter, "ptrtype=" );
_LIT( KKeyTypeParameter, "keytype=" );
_LIT( KControlTypeParameter, "controltype=" );
_LIT( KVideoPlaybackContainer, "EMpxVideoPlaybackContainer" );
_LIT( KVideoPlaybackControl, "EMpxVideoPlaybackControl" );
_LIT( KButtonDown, "EButton1Down" );
_LIT( KButtonUp, "EButton1Up" );
_LIT( KButtonDrag, "EDrag" );
_LIT( KPointerEventNull, "EPtrEventNull" );
_LIT( KKeyScancode, "scancode=" );
_LIT( KKeyCode, "code=" );
_LIT( KKeyDown, "EEventKeyDown" );
_LIT( KKeyUp, "EEventKeyUp" );
_LIT( KKeyNull, "EEventNull" );

//
// extension of TPointerEvent::TType in w32std.h
//
TInt KPtrEventNull = 20;

//
// extension of TMPXVideoPlaybackViewCommandIds in
// mpxcommonvideoplaybackview.hrh
//
TInt KCommandNone = 9000;

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUserinputhandlertest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CUserinputhandlertest::Delete()
{
    if ( iControl )
    {
        delete iControl;
        iControl = NULL;
    }

    if ( iUserInputHandler )
    {
        delete iUserInputHandler;
        iUserInputHandler = NULL;
    }

    if ( iContainer )
    {
        delete iContainer;
        iContainer = NULL;
    }
}

// -----------------------------------------------------------------------------
// CUserinputhandlertest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CUserinputhandlertest::RunMethodL( CStifItemParser& aItem )
{
    static TStifFunctionInfo const KFunctions[] =
    {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "CreateStubsL", CUserinputhandlertest::CreateStubsL ),
        ENTRY( "PointerEvent", CUserinputhandlertest::PointerEvent ),
        ENTRY( "KeyEvent", CUserinputhandlertest::KeyEvent ),
        ENTRY( "MediaKeyEvent", CUserinputhandlertest::MediaKeyEvent ),
        ENTRY( "SetForeground", CUserinputhandlertest::SetForeground ),
        ENTRY( "CheckExpectedResult",CUserinputhandlertest::CheckExpectedResult ),
        ENTRY( "CheckExpectedCommand",CUserinputhandlertest::CheckExpectedCommand ),
        ENTRY( "TvOutEvent",CUserinputhandlertest::TvOutEvent ),
        ENTRY( "CheckBacklightState",CUserinputhandlertest::CheckBacklightState ),        
    };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
}

// -----------------------------------------------------------------------------
//   CUserinputhandlertest::CreateStubs
// -----------------------------------------------------------------------------
//
TInt CUserinputhandlertest::CreateStubsL( CStifItemParser&  aItem )
{
    TInt tvOutConnected;
    TInt err = aItem.GetNextInt( tvOutConnected );
    //
    // Stubs
    //
    iContainer = CMPXVideoPlaybackContainer::NewL();
    iControl = CMPXVideoPlaybackControl::NewL();

    //
    // Class to be unit-tested
    //
	if ( err == KErrNone )
	{
        iUserInputHandler = CMPXVideoPlaybackUserInputHandler::NewL( iContainer, tvOutConnected );
    }
	
    return KErrNone;
}


// -------------------------------------------------------------------------------------------------
// CUserinputhandlertest::PointerEvent
// -------------------------------------------------------------------------------------------------
//
TInt CUserinputhandlertest::PointerEvent( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CUserinputhandlertest::PointerEvent()"));

    TInt result = KErrNone;
    TMPXVideoControlType controlType;
    TPointerEvent eventType;
    TEventCode eventCode;
    TKeyEvent keyEvent;

    //
    // Parse inputs
    //
    ParseInputs( aItem, controlType, eventType, eventCode, keyEvent );

    //
    // send pointer event to UserInputHandler class
    //
    iUserInputHandler->ProcessPointerEventL( static_cast<CCoeControl*> (iControl), eventType, controlType );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CUserinputhandlertest::KeyEvent
// -------------------------------------------------------------------------------------------------
//
TInt CUserinputhandlertest::KeyEvent( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CUserinputhandlertest::KeyEvent()"));

    TInt result = KErrNone;
    TMPXVideoControlType controlType;
    TPointerEvent eventType;
    TEventCode eventCode;
    TKeyEvent keyEvent;

    //
    // Parse inputs
    //
    ParseInputs( aItem, controlType, eventType, eventCode, keyEvent );

    //
    // send key event to UserInputHandler class
    //
    iUserInputHandler->ProcessKeyEventL( keyEvent, eventCode );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CUserinputhandlertest::MediaKeyEvent
// -------------------------------------------------------------------------------------------------
//
TInt CUserinputhandlertest::MediaKeyEvent( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CUserinputhandlertest::MediaKeyEvent()"));

    _LIT( KCommandParameter, "command=" );
    _LIT( KOperationIdParameter, "operationid=" );
    _LIT( KButtonActionParameter, "buttonaction=" );
    _LIT( KMrccatoPlay, "MrccatoPlay" );
    _LIT( KMrccatoCommand, "MrccatoCommand" );

    TPtrC parameter;
    TInt value;
    TBool isMrccatoPlay;

    TRemConCoreApiButtonAction buttonAction;
    TRemConCoreApiOperationId operationId;

    TInt result = KErrNone;

    while ( aItem.GetNextString( parameter ) == KErrNone )
    {
        if ( parameter.Find( KCommandParameter ) == 0 )
        {
            //
            // get command (MrccatoPlay or MrccatoCommand)
            //
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KCommandParameter().Length() );

            if ( parameterValue.Compare(KMrccatoPlay) == 0 )
            {
                isMrccatoPlay = ETrue;
            }
            else if ( parameterValue.Compare(KMrccatoCommand) == 0 )
            {
                isMrccatoPlay = EFalse;
            }
        }
        else if ( parameter.Find( KOperationIdParameter ) == 0 )
        {
            //
            // get operation id (listed in remconcoreapi.h)
            //
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KOperationIdParameter().Length() );
            TLex parameterValueParser( parameterValue );
            if ( parameterValueParser.Val( value ) == KErrNone )
            {
                operationId = static_cast<TRemConCoreApiOperationId> (value);
            }
            else
            {
                result = KErrArgument;
            }
        }
        else if ( parameter.Find( KButtonActionParameter ) == 0 )
        {
            //
            // get button action (listed in remconcoreapi.h)
            //
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KButtonActionParameter().Length() );
            TLex parameterValueParser( parameterValue );
            if ( parameterValueParser.Val( value ) == KErrNone )
            {
                buttonAction = static_cast<TRemConCoreApiButtonAction> (value);
            }
            else
            {
                result = KErrArgument;
            }
        }
        else
        {
             result = KErrArgument;
        }
    }

    if ( isMrccatoPlay )
    {
        //
        // send Media Key Play event to UserInputHandler class
        //
		iUserInputHandler->MrccatoPlay( ERemConCoreApiPlaybackSpeedNextFrame, buttonAction );
    }
    else
    {
        //
        // send Media Key Command event to UserInputHandler class
        //
        iUserInputHandler->MrccatoCommand( operationId, buttonAction );
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
//   CUserinputhandlertest::SetForeground()
// -------------------------------------------------------------------------------------------------
//
TInt CUserinputhandlertest::SetForeground( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CUserinputhandlertest::SetForeground()"));

    _LIT( KForegroundParameter, "foreground=" );

    TInt value;
    TPtrC parameter;

    TInt result = KErrNone;

    while ( aItem.GetNextString( parameter ) == KErrNone )
    {
        if ( parameter.Find( KForegroundParameter ) == 0 )
        {
            //
            // get foreground value
            //
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KForegroundParameter().Length() );

            TLex parameterValueParser( parameterValue );
            if ( parameterValueParser.Val( value ) != KErrNone )
            {
                result = KErrArgument;
            }

        }
        else
        {
             result = KErrArgument;
        }
    }

    //
    // send foreground event to UserInputHandler class
    //
    iUserInputHandler->SetForeground( value );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CUserinputhandlertest::ParseInputs
// -------------------------------------------------------------------------------------------------
//
TInt CUserinputhandlertest::ParseInputs( CStifItemParser&  aItem,
                                         TMPXVideoControlType& aControlType,
                                         TPointerEvent& aEventType,
                                         TEventCode& aEventCode,
                                         TKeyEvent& aKeyEvent )
{
    TPtrC parameter;
    TInt result = KErrNone;

    while ( aItem.GetNextString( parameter ) == KErrNone )
    {
        if ( parameter.Find( KControlTypeParameter ) == 0 )
        {
            //
            // obtain control type (Container or Control)
            //
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KControlTypeParameter().Length() );

            if ( parameterValue.Compare(KVideoPlaybackContainer) == 0 )
            {
                aControlType = EMpxVideoPlaybackContainer;
            }
            else if ( parameterValue.Compare(KVideoPlaybackControl) == 0 )
            {
                aControlType = EMpxVideoPlaybackControl;
            }
        }
        else if ( parameter.Find( KPtrTypeParameter ) == 0 )
        {
            //
            // obtain pointer type (Button Down, Up, Drag, or Null)
            //
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KPtrTypeParameter().Length() );

            if ( parameterValue.Compare(KButtonDown) == 0 )
            {
                aEventType.iType = TPointerEvent::EButton1Down;
            }
            else if ( parameterValue.Compare(KButtonUp) == 0 )
            {
                aEventType.iType = TPointerEvent::EButton1Up;
            }
            else if ( parameterValue.Compare(KButtonDrag) == 0 )
            {
                aEventType.iType = TPointerEvent::EDrag;
            }
            else if ( parameterValue.Compare(KPointerEventNull) == 0 )
            {
                aEventType.iType = (TPointerEvent::TType) KPtrEventNull;
            }
        }
        else if ( parameter.Find( KKeyTypeParameter ) == 0 )
        {
            //
            // obtain key type (Key Down, Up, or Null)
            //
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KKeyTypeParameter().Length() );

            if ( parameterValue.Compare(KKeyDown) == 0 )
            {
                aEventCode = EEventKeyDown;
            }
            else if ( parameterValue.Compare(KKeyUp) == 0 )
            {
                aEventCode = EEventKeyUp;
            }
            else if ( parameterValue.Compare(KKeyNull) == 0 )
            {
                aEventCode = EEventNull;
            }
        }
        else if ( parameter.Find( KKeyScancode ) == 0 )
        {
            //
            // obtain iScanCode
            //
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KKeyScancode().Length() );
            TLex parameterValueParser( parameterValue );
            if ( parameterValueParser.Val( aKeyEvent.iScanCode ) != KErrNone )
            {
                result = KErrArgument;
            }
        }
        else if ( parameter.Find( KKeyCode ) == 0 )
        {
            //
            // obtain iCode
            //
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KKeyCode().Length() );
            TLex parameterValueParser( parameterValue );
            if ( parameterValueParser.Val( aKeyEvent.iCode ) != KErrNone )
            {
                result = KErrArgument;
            }
        }
        else
        {
             result = KErrArgument;
        }
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CUserinputhandlertest::CheckExpectedResult
// -------------------------------------------------------------------------------------------------
//
TInt CUserinputhandlertest::CheckExpectedResult( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CUserinputhandlertest::CheckExpectedResult()"));

    TInt result = KErrGeneral;
    TMPXVideoControlType expectedControlType;
    TPointerEvent expectedEventType;
    TEventCode expectedEventCode;
    TKeyEvent expectedKeyEvent;

    //
    // Parse expected inputs
    //
    ParseInputs( aItem,
                 expectedControlType,
                 expectedEventType,
                 expectedEventCode,
                 expectedKeyEvent );

    if ( expectedControlType == EMpxVideoPlaybackContainer )
    {
        //
        // Compare expected result and actual result in Playback Container
        //
        if ( ( expectedEventType.iType == iContainer->iPointerEvent.iType ) ||
             ( expectedEventCode  == iContainer->iEventCode &&
               expectedKeyEvent.iScanCode == iContainer->iKeyEvent.iScanCode ) )
        {
            result = KErrNone;
        }
        else
        {
            MPX_DEBUG(_L("Container FAILED: expected [type=%d,evenCode=%d,scanCode=%d]"),
                    expectedEventType.iType, expectedEventCode, expectedKeyEvent.iScanCode );
            MPX_DEBUG(_L("Container FAILED: actual   [type=%d,evenCode=%d,scanCode=%d]"),
                    iContainer->iPointerEvent.iType, iContainer->iEventCode, iContainer->iKeyEvent.iScanCode );
        }
    }
    else if ( expectedControlType == EMpxVideoPlaybackControl )
    {
        //
        // Compare expected result and actual result in Playback Control
        //
        if ( ( expectedEventType.iType == iControl->iPointerEvent.iType ) ||
             ( expectedEventCode  == iControl->iEventCode &&
               expectedKeyEvent.iScanCode == iControl->iKeyEvent.iScanCode ) )
        {
            result = KErrNone;
        }
        else
        {
            MPX_DEBUG(_L("Control FAILED: expected [type=%d,evenCode=%d,scanCode=%d]"),
                    expectedEventType.iType, expectedEventCode, expectedKeyEvent.iScanCode );
            MPX_DEBUG(_L("Control FAILED: actual   [type=%d,evenCode=%d,scanCode=%d]"),
                    iControl->iPointerEvent.iType, iControl->iEventCode, iControl->iKeyEvent.iScanCode );
        }
    }


    return result;
}

// -------------------------------------------------------------------------------------------------
// CUserinputhandlertest::CheckExpectedCommand
// -------------------------------------------------------------------------------------------------
//
TInt CUserinputhandlertest::CheckExpectedCommand( CStifItemParser& aItem )
{
    MPX_DEBUG(_L("CUserinputhandlertest::CheckExpectedCommand()"));

    TInt command = 0;
    TInt result = aItem.GetNextInt( command );

    if ( result == KErrNone && command == iContainer->iCommand )
    {
        result = KErrNone;
    }
    else
    {
        MPX_DEBUG(_L("Command FAILED: expectedCommand=%d, actualCommand=%d]"),
                          command, iContainer->iCommand );

        result = KErrArgument;
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CUserinputhandlertest::TvOutEvent
// -------------------------------------------------------------------------------------------------
//
TInt CUserinputhandlertest::TvOutEvent( CStifItemParser& aItem )
{
    MPX_DEBUG(_L("CUserinputhandlertest::CheckBacklightState()"));

    TInt tvOutEvent;
    
    TInt result = aItem.GetNextInt( tvOutEvent );

    iUserInputHandler->HandleTVOutEvent( tvOutEvent );
    
    return result;
}

// -------------------------------------------------------------------------------------------------
// CUserinputhandlertest::CheckBacklightState
// -------------------------------------------------------------------------------------------------
//
TInt CUserinputhandlertest::CheckBacklightState( CStifItemParser& aItem )
{
    MPX_DEBUG(_L("CUserinputhandlertest::CheckBacklightState()"));

    TLightStatus backlightState = ELightStatusUnknown;
    TInt status;
    
    CHWRMLight* light = NULL;
    MPX_TRAPD( err, light = CHWRMLight::NewL() );
    backlightState = light->LightStatus( CHWRMLight::EPrimaryDisplay );
    delete light;

    TInt result = aItem.GetNextInt( status );

    if ( result == KErrNone && status == backlightState )
    {
        result = KErrNone;
    }
    else
    {
        MPX_DEBUG(_L("CheckBacklightState FAILED: expectedState=%d, actualState=%d]"),
                backlightState, status );

        result = KErrArgument;
    }

    return result;
}

//  [End of File] - Do not remove
