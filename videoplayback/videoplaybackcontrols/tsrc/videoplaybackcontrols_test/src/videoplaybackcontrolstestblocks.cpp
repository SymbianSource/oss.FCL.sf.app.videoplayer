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
* Description:  Test Harness for VideoPlayControls
*
*/

// Version : %version: e003sa33#11 %


// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <e32std.h>
#include <w32std.h>
#include <coeaui.h>
#include <aknutils.h>
#include <aknviewappui.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "mpxvideoplaybackviewfiledetails.h"
#include "videoplaybackcontrolstest.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackcontrol.hrh"
#include "mpxvideo_debug.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackControlsTest::Delete()
{
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::RunMethodL( CStifItemParser& aItem )
{
    static TStifFunctionInfo const KFunctions[] =
    {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "CreateControls", CVideoPlaybackControlsTest::CreateControls ),
        ENTRY( "PointerEvent", CVideoPlaybackControlsTest::PointerEvent ),
        ENTRY( "KeyEvent", CVideoPlaybackControlsTest::KeyEvent ),
        ENTRY( "ExpectedResult",CVideoPlaybackControlsTest::ExpectedResult ),
        ENTRY( "SetAspectRatio", CVideoPlaybackControlsTest::SetAspectRatio ),
        ENTRY( "SetVolume", CVideoPlaybackControlsTest::SetVolume ),
        ENTRY( "SetState", CVideoPlaybackControlsTest::SetState ),
        ENTRY( "AddDefaultFileDetails", CVideoPlaybackControlsTest::AddDefaultFileDetails ),
        ENTRY( "AddLongFileDetails", CVideoPlaybackControlsTest::AddLongFileDetails ),
        ENTRY( "AddAudioOnlyFileDetails", CVideoPlaybackControlsTest::AddAudioOnlyFileDetails ),
        ENTRY( "AddSameARFileDetails", CVideoPlaybackControlsTest::AddSameARFileDetails ),
        ENTRY( "SetDuration", CVideoPlaybackControlsTest::SetDuration ),
        ENTRY( "SoftkeyPressed", CVideoPlaybackControlsTest::SoftkeyPressed ),
        ENTRY( "ShowControls", CVideoPlaybackControlsTest::ShowControls ),
        ENTRY( "ToggleVisibility", CVideoPlaybackControlsTest::ToggleVisibility ),
        ENTRY( "HandleBackgroundEvnet", CVideoPlaybackControlsTest::HandleBackgroundEvnet ),
        ENTRY( "DeleteControls", CVideoPlaybackControlsTest::DeleteControls ),

        // PDL test cases
        ENTRY( "SetDownLoadSize", CVideoPlaybackControlsTest::SetDownLoadSize ),
        ENTRY( "SetDownLoadPostion", CVideoPlaybackControlsTest::SetDownLoadPostion ),
        ENTRY( "UpdateDownloadPosition", CVideoPlaybackControlsTest::UpdateDownloadPosition ),
        ENTRY( "SetDownLoadPaused", CVideoPlaybackControlsTest::SetDownLoadPaused ),
        ENTRY( "ClearDownLoadPaused", CVideoPlaybackControlsTest::ClearDownLoadPaused ),

        // TV out cases
        ENTRY( "TvOutConnected", CVideoPlaybackControlsTest::TvOutConnected ),
        ENTRY( "TvOutDisconnected", CVideoPlaybackControlsTest::TvOutDisconnected ),

        ENTRY( "HandleErrors", CVideoPlaybackControlsTest::HandleErrors ),
        
        // Media Details Viewer
        ENTRY( "ShowMediaDetailsViewer", CVideoPlaybackControlsTest::ShowMediaDetailsViewer ),
        ENTRY( "CloseMediaDetailsViewer", CVideoPlaybackControlsTest::CloseMediaDetailsViewer )
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
    };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::CreateController()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackControlsTest::CreateController( const TDesC& aFileName, TInt aMode )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::CreateController( %S, %d )"), &aFileName, aMode );

    TInt result = KErrGeneral;

    if ( iAvkonViewAppUi->OrientationCanBeChanged() )
    {
        TRAPD( err, iAvkonViewAppUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationLandscape ) );
        MPX_DEBUG(_L("CVideoPlaybackControlsTest::CreateController() err = [%d]"), err);
    }

    TRect rect = iAvkonViewAppUi->ApplicationRect();

    TRAP
    (
        result,
        {
            iContainer = CMPXVideoPlaybackContainer::NewL( aFileName );

            iContainer->SetRect( rect );

            CCoeEnv::Static()->AppUi()->AddToStackL( iContainer );

            iController =
                CMPXVideoPlaybackControlsController::NewL( iContainer,
                                                           rect,
                                                           iContainer->FileDetails( aMode ) );

            iContainer->SetController( iController );
        }
    );

    User::ResetInactivityTime();
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::DeleteControls()
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::DeleteControls()
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::DeleteControls()"));

    TInt err = KErrNone;

    TRAP( err, iAvkonViewAppUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationAutomatic ) );

    if ( iController )
    {
        delete iController;
        iController = NULL;
    }

    CCoeEnv::Static()->AppUi()->RemoveFromStack( iContainer );

    if ( iContainer )
    {
        delete iContainer;
        iContainer = NULL;
    }

    return err;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::CreateControls
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::CreateControls( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::CreateControls()"));

    TInt mode = 0;
    TInt result = KErrNotFound;
    TPtrC filename;

    //
    // get file name first
    //
    result = aItem.GetNextString( filename );

    //
    // get mode next
    //
    if ( result == KErrNone )
    {
        result = aItem.GetNextInt( mode );
    }

    //
    // create controller
    //
    if ( result == KErrNone )
    {
        CreateController( GetFileNameAndPath( filename ), mode );
        iContainer->DrawNow();
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::SetAspectRatio
// Test creation of a general playback command.
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::SetAspectRatio( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::SetAspectRatio()"));

    TInt aspectRatio = 0;
    TInt result = aItem.GetNextInt( aspectRatio );

    if ( result == KErrNone )
    {
        MPX_DEBUG(_L("CVideoPlaybackControlsTest::SetAspectRatio() [%d]"), aspectRatio);

        TRAP( result, iController->HandleEventL( EMPXControlCmdSetAspectRatio, aspectRatio ) );
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::SetVolume
// Test creation of a general playback command.
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::SetVolume( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::SetVolume()"));

    TInt volume = 0;
    TInt result = aItem.GetNextInt( volume );

    if ( result == KErrNone )
    {
        MPX_DEBUG(_L("CVideoPlaybackControlsTest::SetVolume() [%d]"), volume);

        TRAP( result, iController->HandleEventL( EMPXControlCmdSetVolume,
                                                 (TInt64)volume ) );
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::SetDuration
// Test creation of a general playback command.
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::SetDuration( CStifItemParser&  aItem )
{
    TInt duration = 0;
    TInt result = aItem.GetNextInt( duration );

    MPX_DEBUG(_L("CVideoPlaybackControlsTest::SetDuration() [%d]"), duration);

    if ( result == KErrNone )
    {
         TRAP( result, iController->HandleEventL( EMPXControlCmdSetDuration,
                                                   (TInt64)duration ) );
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::PointerEvent
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::PointerEvent( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::PointerEvent()"));

    _LIT( KEventTypeParameter, "eventtype=" );
    _LIT( KPointerXParameter, "x=" );
    _LIT( KPointerYParameter, "y=" );

    TPointerEvent::TType type;
    TPoint position;

    TPtrC parameter;

    TInt result = KErrNone;

    while ( aItem.GetNextString( parameter ) == KErrNone )
    {
        if ( parameter.Find( KEventTypeParameter ) == 0 )
        {
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KEventTypeParameter().Length() );

            if ( ParseType( parameterValue, type ) != KErrNone )
            {
                result = KErrArgument;
            }
        }
        else if ( parameter.Find( KPointerXParameter ) == 0 )
        {
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KPointerXParameter().Length() );
            TLex parameterValueParser( parameterValue );
            if ( parameterValueParser.Val( position.iX ) != KErrNone )
            {
                result = KErrArgument;
            }
        }
        else if ( parameter.Find( KPointerYParameter ) == 0 )
        {
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KPointerYParameter().Length() );
            TLex parameterValueParser( parameterValue );
            if ( parameterValueParser.Val( position.iY ) != KErrNone )
            {
                result = KErrArgument;
            }
        }
        else
        {
             result = KErrArgument;
        }
    }

    TapOnScreen( type, position );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::KeyEvent
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::KeyEvent( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::KeyEvent()"));

    _LIT( KEventTypeParameter, "eventtype=" );
    _LIT( KKeyScancode, "scancode=" );

    _LIT( KKeyDown, "EEventKeyDown" );
    _LIT( KKeyUp, "EEventKeyUp" );

    TInt type = 0;
    TInt scanCode = 0;
    TPtrC parameter;

    TInt result = KErrNone;

    while ( aItem.GetNextString( parameter ) == KErrNone )
    {
        if ( parameter.Find( KEventTypeParameter ) == 0 )
        {
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KEventTypeParameter().Length() );

            if ( parameterValue.Compare(KKeyDown) == 0 )
            {
                type = EEventKeyDown;
            }
            else if ( parameterValue.Compare(KKeyUp) == 0 )
            {
                type = EEventKeyUp;
            }

        }
        else if ( parameter.Find( KKeyScancode ) == 0 )
        {
            TPtrC parameterValue =
                parameter.Right( parameter.Length() - KKeyScancode().Length() );
            TLex parameterValueParser( parameterValue );
            if ( parameterValueParser.Val( scanCode ) != KErrNone )
            {
                result = KErrArgument;
            }
        }
        else
        {
             result = KErrArgument;
        }
    }


    // Now send the key event

    TRawEvent rawEvent;

    if ( type == EEventKeyDown )
    {
        rawEvent.Set( TRawEvent::EKeyDown , scanCode );

        iWsSession.SimulateRawEvent( rawEvent );
        iWsSession.Flush();
    }
    else if ( type == EEventKeyUp )
    {
        rawEvent.Set( TRawEvent::EKeyUp, scanCode );

        iWsSession.SimulateRawEvent( rawEvent );
        iWsSession.Flush();
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::ExpectedResult
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::ExpectedResult( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::ExpectedResult()"));

    TInt command = 0;
    TInt value = 0 ;
    TInt result = aItem.GetNextInt( command );

    if ( result == KErrNone )
    {
        result = aItem.GetNextInt ( value );

        if ( result == KErrNone  )
        {
            TRAP( result, iContainer->ExpectedResultL( command, value ) );
        }
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::SetState
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::SetState( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::SetState()"));

    TInt state = 0;
    TInt result = aItem.GetNextInt( state );

    if ( result == KErrNone )
    {
        TRAP( result, iController->HandleEventL( EMPXControlCmdStateChanged, state ) );
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::AddDefaultFileDetails
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::AddDefaultFileDetails()
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::AddDefaultFileDetails()"));

    TRAPD( result, iContainer->AddFileDetailsL() );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::AddAudioOnlyFileDetails
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::AddAudioOnlyFileDetails()
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::AddAudioOnlyFileDetails()"));

    TRAPD( result, iContainer->AddFileDetailsL( EFalse ) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::AddLongFileDetails
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::AddLongFileDetails()
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::AddLongFileDetails()"));

    TRAPD( result, iContainer->AddLongFileDetailsL() );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::AddSameARFileDetails
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::AddSameARFileDetails()
{
	MPX_ENTER_EXIT( _L( "CVideoPlaybackControlsTest::AddSameARFileDetails()" ) );

    MPX_TRAPD( result, iContainer->AddSameARFileDetailsL() );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::ParseType
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::ParseType( TDesC& aTypeName,
                                            TPointerEvent::TType& aType )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::ParseType()"));

    TBool err = KErrNone;

    _LIT( KButtonDown, "EButton1Down" );
    _LIT( KButtonDrag, "EDrag" );
    _LIT( KButtonUp, "EButton1Up" );

    if ( aTypeName.Compare(KButtonDown) == 0 )
    {
        aType = TPointerEvent::EButton1Down;
    }
    else if( aTypeName.Compare(KButtonUp) == 0 )
    {
        aType = TPointerEvent::EButton1Up;
    }
    else if( aTypeName.Compare(KButtonDrag) == 0 )
    {
        aType = TPointerEvent::EDrag;
    }
    else
    {
        err = KErrNotFound;
    }

    return err;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::TapOnScreen
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackControlsTest::TapOnScreen( TPointerEvent::TType aType,
                                              TPoint aPosition )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::TapOnScreen()"));

    MPX_DEBUG(_L("   aType = %d, aPosition = (%d,%d)"), aType, aPosition.iX, aPosition.iY);

    TPoint tapPoint( aPosition );

#if !defined(__WINS__)

    //
    //  On the hardware, the window server is performing a rotation.
    //  This calculation will adjust the points so the same touch point is received
    //  on both the emulator and hardware
    //
    //  ix = ( ScreenWidth - 1 ) - oldPosition.iY - YOffset
    //      I am not sure why they subtract 1 from the screen width, but they do
    //      The YOffset can be read from the wsini.ini file
    //
    //  iy = oldPosition.iX
    //  
    tapPoint.iX = ( 360 - 1 ) - aPosition.iY - 12;
    tapPoint.iY = aPosition.iX;

#endif
    
    MPX_DEBUG(_L("   tapPosition = (%d,%d)"), tapPoint.iX, tapPoint.iY);

    TRawEvent pointer;

    if ( aType == TPointerEvent::EButton1Down )
    {
        pointer.Set( TRawEvent::EButton1Down, tapPoint.iX, tapPoint.iY );

        iWsSession.SimulateRawEvent( pointer );
        iWsSession.Flush();
    }
    else if ( aType == TPointerEvent::EButton1Up )
    {
        pointer.Set( TRawEvent::EButton1Up, tapPoint.iX, tapPoint.iY );

        iWsSession.SimulateRawEvent( pointer );
        iWsSession.Flush();
    }
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::SetDownLoadSize
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::SetDownLoadSize( CStifItemParser& aItem )
{
	MPX_DEBUG(_L("CVideoPlaybackControlsTest::SetExpectedResult()"));

    TInt size = 0;
    TInt result = aItem.GetNextInt( size );

    if ( result == KErrNone )
    {
        TRAP( result, iController->HandleEventL( EMPXControlCmdSetDownloadSize, size ) );
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::SetDownLoadPostion
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::SetDownLoadPostion( CStifItemParser&  aItem )
{
	MPX_DEBUG(_L("CVideoPlaybackControlsTest::SetExpectedResult()"));

    TInt position = 0;
    TInt result = aItem.GetNextInt( position );

    if ( result == KErrNone )
    {
        TRAP( result, iController->HandleEventL( EMPXControlCmdSetPosition, position ) );
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::UpdateDownloadPosition
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::UpdateDownloadPosition( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::UpdateDownloadPosition()"));

    TInt position = 0;
    TInt result = aItem.GetNextInt( position );

    if ( result == KErrNone )
    {
        TRAP( result, iController->HandleEventL( EMPXControlCmdDownloadUpdated, position ) );
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::SetDownLoadPaused
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::SetDownLoadPaused()
{
	MPX_DEBUG(_L("CVideoPlaybackControlsTest::SetDownLoadPaused()"));

    TRAPD( result, iController->HandleEventL( EMPXControlCmdSetDownloadPaused) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::ClearDownLoadPaused
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::ClearDownLoadPaused()
{
	MPX_DEBUG(_L("CVideoPlaybackControlsTest::ClearDownLoadPaused()"));

    TRAPD( result, iController->HandleEventL( EMPXControlCmdClearDownloadPaused) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::TvOutConnected
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::TvOutConnected()
{
	MPX_DEBUG(_L("CVideoPlaybackControlsTest::TvOutConnected()"));

    TRAPD( result, iController->HandleEventL( EMPXControlCmdTvOutConnected ) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::TvOutDisconnected
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::TvOutDisconnected()
{
	MPX_DEBUG(_L("CVideoPlaybackControlsTest::TvOutDisconnected()"));

    TRAPD( result, iController->HandleEventL( EMPXControlCmdTvOutDisconnected ) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::HandleErrors
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::HandleErrors()
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::HandleErrors()"));

    TRAPD( result, iController->HandleEventL( EMPXControlCmdHandleErrors ) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::SoftkeyPressed
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::SoftkeyPressed( CStifItemParser&  aItem )
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::SoftkeyPressed()"));

    TInt key = 0;

    TInt result = aItem.GetNextInt( key );

    TRAP( result, iController->HandleEventL( EMPXControlCmdSoftKeyPressed, key ) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::ShowControls
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::ShowControls()
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::ShowControls()"));

    TRAPD( result, iController->HandleEventL( EMPXControlCmdShowControls ) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::ToggleVisibility
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::ToggleVisibility()
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::ToggleVisibility()"));

    TRAPD( result, iController->HandleEventL( EMPXControlCmdToggleVisibility ) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::HandleBackgroundEvnet
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::HandleBackgroundEvnet()
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::HandleBackgroundEvnet()"));

    TRAPD( result, iController->HandleEventL( EMPXControlCmdHandleBackgroundEvent ) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::GetFileNameAndPath
// -------------------------------------------------------------------------------------------------
//
TFileName CVideoPlaybackControlsTest::GetFileNameAndPath( TDesC& aFileName )
{
    MPX_DEBUG(_L("CMpxVideoPlayer_AppUiEngineTester::GetFileNameAndPath()"));

    TFileName fullPath;

#ifdef __WINSCW__
    _LIT( KVideoTestPath, "c:\\data\\Videos\\" );
#else
    _LIT( KVideoTestPath, "e:\\testing\\data\\" );
#endif

    _LIT( KStreamingPrefix, "rtsp:" );

    if ( aFileName.Find( KStreamingPrefix ) < 0 )
    {
        fullPath.Append( KVideoTestPath );
    }

    fullPath.Append( aFileName );

    return fullPath;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::ShowMediaDetailsViewer
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::ShowMediaDetailsViewer()
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::ShowMediaDetailsViewer()"));

    TRAPD( result, iController->HandleEventL( EMPXControlCmdShowFileDetailsViewer ) );

    return result;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackControlsTest::CloseMediaDetailsViewer
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackControlsTest::CloseMediaDetailsViewer()
{
    MPX_DEBUG(_L("CVideoPlaybackControlsTest::CloseMediaDetailsViewer()"));

    TRAPD( result, iController->HandleEventL( EMPXControlCmdCloseFileDetailsViewer ) );

    return result;
}

// ========================== OTHER EXPORTED FUNCTIONS =============================================
// None

//  [End of File] - Do not remove
