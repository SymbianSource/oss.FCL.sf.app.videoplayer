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
* Description:  Implementation of Video playback view's container.
*
*/

// Version : %version: 22 %



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
#include <data_caging_path_literals.hrh> // KDC_APP_BITMAP_DIR
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <mpxvideoplaybackviews.rsg>

#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackview.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontainer.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackuserinputhandler.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideo_debug.h"

using namespace AknLayoutScalable_Apps;

// CONSTANTS

// ======== MEMBER FUNCTIONS =======================================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::CMPXVideoPlaybackContainer()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackContainer::CMPXVideoPlaybackContainer( CMPXVideoBasePlaybackView* aView )
    : iView( aView )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::NewL()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackContainer* CMPXVideoPlaybackContainer::NewL( CMPXVideoBasePlaybackView* aView )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::NewL()"));

    CMPXVideoPlaybackContainer* self = new (ELeave) CMPXVideoPlaybackContainer( aView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPNormalScreenPlaybackContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackContainer::ConstructL()"));

    SetMopParent( iView );
    CreateWindowL();

    TAknWindowLineLayout screenLayout = AknLayout::screen();
    TRect screenRect = screenLayout.Rect();

    SetRect( screenRect );

    CreateControlsL();

    iUserInputHandler =
        CMPXVideoPlaybackUserInputHandler::NewL( this, iFileDetails->iTvOutConnected );

    ActivateL();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::~CMPXVideoPlaybackContainer()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackContainer::~CMPXVideoPlaybackContainer()
{
    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }

    if ( iControlsController )
    {
        delete iControlsController;
        iControlsController = NULL;
    }

    if ( iUserInputHandler )
    {
        delete iUserInputHandler;
        iUserInputHandler = NULL;
    }

    CloseWindow();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::AddFileDetailsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::AddFileDetailsL( CMPXVideoPlaybackViewFileDetails* aDetails )
{
    iControlsController->AddFileDetailsL( aDetails );

    //
    //  Delete the temp file details since plugin initialization is complete
    //
    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::GetWindow()
// -------------------------------------------------------------------------------------------------
//
RWindow& CMPXVideoPlaybackContainer::GetWindow() const
{
    return Window();
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// -------------------------------------------------------------------------------------------------
//
TKeyResponse CMPXVideoPlaybackContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                         TEventCode aType )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoPlaybackContainer::OfferKeyEventL()"),
        _L("iCode = %d iScanCode = %d aType = %d"),
        aKeyEvent.iCode, aKeyEvent.iScanCode, aType );

    iKeyResponse = EKeyWasNotConsumed;

    iUserInputHandler->ProcessKeyEventL( aKeyEvent, aType );

    return iKeyResponse;
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::DoHandleKeyEventL()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void
CMPXVideoPlaybackContainer::DoHandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::DoHandleKeyEventL()"));

    switch ( aKeyEvent.iScanCode )
    {
        case EStdKeyDevice3:        // rocker middle key
        case EStdKeyEnter:          // enter key on a qwerty key board
        {
            HandleRockerMiddleKeyL( aKeyEvent, aType );
            iKeyResponse = EKeyWasConsumed;
            break;
        }
        case EStdKeyUpArrow:        // rocker up key
        {
            if ( aType == EEventKeyDown )
            {
                HandleCommandL( EMPXPbvCmdIncreaseVolume );
                iKeyResponse = EKeyWasConsumed;
            }
            break;
        }
        case EStdKeyDownArrow:      // rocker down key
        {
            if ( aType == EEventKeyDown )
            {
                HandleCommandL( EMPXPbvCmdDecreaseVolume );
                iKeyResponse = EKeyWasConsumed;
            }
            break;
        }
        case EStdKeyLeftArrow:      // rocker left key
        {
            HandleSeekBackL(aType);
            iKeyResponse = EKeyWasConsumed;
            break;
        }
        case EStdKeyRightArrow:     // rocker right key
        {
            HandleSeekFwdL(aType);
            iKeyResponse = EKeyWasConsumed;
            break;
        }
        case EStdKeyDevice0:
        case EStdKeyDevice1:
        {
            if ( aType == EEventKeyUp )
            {
                iControlsController->HandleEventL( EMPXControlCmdSoftKeyPressed,
                		                           aKeyEvent.iScanCode );
            }

            iKeyResponse = EKeyWasConsumed;
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// Handles rocker's middle key =>  Toggles between play & pause
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::HandleRockerMiddleKeyL(const TKeyEvent& aKeyEvent,
                                                       TEventCode aType)
{
    if ( aKeyEvent.iCode == EKeyNull && aType == EEventKeyDown  )
    {
        iView->HandleCommandL(EMPXPbvCmdPlayPause);
    }
}

// -------------------------------------------------------------------------------------------------
// Starts/Stops Seeking Forward
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::HandleSeekFwdL(TEventCode aType)
{
    if (aType == EEventKeyDown)
    {
        iView->HandleCommandL(EMPXPbvCmdSeekForward);
    }
    else if (aType == EEventKeyUp)
    {
        iView->HandleCommandL(EMPXPbvCmdEndSeek);
    }
}

// -------------------------------------------------------------------------------------------------
// Starts/Stops Seeking Backward
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::HandleSeekBackL(TEventCode aType)
{
    if (aType == EEventKeyDown)
    {
        iView->HandleCommandL(EMPXPbvCmdSeekBackward);
    }
    else if (aType == EEventKeyUp)
    {
        iView->HandleCommandL(EMPXPbvCmdEndSeek);
    }
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Handles a change to the control's resources.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::HandleResourceChange( TInt aType )
{
    CCoeControl::HandleResourceChange( aType );
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Gets the control's help context.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::GetHelpContext( TCoeHelpContext& /*aContext*/ ) const
{
    //aContext.iMajor = KAppUidMusicPlayerX;
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Gets the number of controls contained in a compound control.
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackContainer::CountComponentControls() const
{
	TInt count = 0;

	if ( iRealOneBitmap )
	{
        count = 1;
	}

	return count;
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Gets an indexed component of a compound control.
// -------------------------------------------------------------------------------------------------
//
CCoeControl* CMPXVideoPlaybackContainer::ComponentControl( TInt aIndex ) const
{
    CCoeControl* control = NULL;

    switch ( aIndex )
    {
        case 0:
        {
            control = iRealOneBitmap;
            break;
        }
    }

    return control;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::HandlePointerEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackContainer::HandlePointerEventL()"));

    iUserInputHandler->ProcessPointerEventL(this, aPointerEvent, EMpxVideoPlaybackContainer);
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::DoHandlePointerEventL()
//
// Gets called from the Controls Controller
// only if if OK to handle pointer events
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void CMPXVideoPlaybackContainer::DoHandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::DoHandlePointerEventL()"));

    if ( AknLayoutUtils::PenEnabled() && aPointerEvent.iType == TPointerEvent::EButton1Up )
    {
        iControlsController->HandleEventL( EMPXControlCmdToggleVisibility );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackContainer::Draw()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::Draw( const TRect& aRect ) const
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackContainer::Draw()"));

    CWindowGc& gc = SystemGc();
    
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );

    //
    //  Make the window transparent when a surface has been created
    //  unless TV-Out is connected
    //
    if ( iSurfaceCreated && ! iTvOutConnected )
    {
        gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );

        if ( Window().DisplayMode() == EColor16MAP )
        {
            gc.SetBrushColor( TRgb::Color16MAP(255) );
        }
        else if ( Window().DisplayMode() == EColor16MA )
        {
            gc.SetBrushColor( TRgb::Color16MA(0) );
        }

        gc.Clear( aRect );
    }
    else
    {
        //
        //  Set the background to black
        //
        gc.SetBrushColor( KRgbBlack );
        gc.DrawRect( aRect );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::HandleEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::HandleEventL( TMPXVideoPlaybackControlCommandIds aEvent,
                                               TInt aValue )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackContainer::HandleEventL()"));

    if ( aEvent == EMPXControlCmdHandleBackgroundEvent )
    {
        iUserInputHandler->SetForeground(EFalse);
    }
    else if ( aEvent == EMPXControlCmdHandleForegroundEvent )
    {
        iUserInputHandler->SetForeground(ETrue);
    }
    else if ( aEvent == EMPXControlCmdTvOutConnected )
    {
    	iTvOutConnected = ETrue;
        iUserInputHandler->HandleTVOutEvent(ETrue);
        DrawNow();
    }
    else if ( aEvent == EMPXControlCmdTvOutDisconnected )
    {
    	iTvOutConnected = EFalse;
        iUserInputHandler->HandleTVOutEvent(EFalse);
        DrawNow();
    }

    iControlsController->HandleEventL( aEvent, aValue );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackContainer::HandleCommandL()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void CMPXVideoPlaybackContainer::HandleCommandL( TInt aCommand, TInt aValue )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackContainer::HandleCommandL()"),
                   _L("aCommand = %d  aValue = %d"), aCommand, aValue );

    switch ( aCommand )
    {
        case EMPXPbvCmdSetPosition:
        {
            iView->SetPropertyL( EPbPropertyPosition, (TInt)aValue * KPbMilliMultiplier );
            break;
        }
        case EMPXPbvCmdResetControls:
        {
            //
            //  Recreate the controls with the new clip
            //
            CreateControlsL();

            //
            //  Retrieve the PDL information for container
            //
            iView->RetrievePdlInformationL();

            iControlsController->HandleEventL( EMPXControlCmdCloseFileDetailsViewer );

            break;
        }
        case EAknSoftkeyOptions:
        {
            static_cast<CAknView*>(iView)->ProcessCommandL( EAknSoftkeyOptions );
            break;
        }
        case EMPXPbvCmdSetVolume:
        {
            iView->SetPropertyL( EPbPropertyVolume, aValue );
            break;
        }
        case EMPXPbvSurfaceCreated:
        {
        	iSurfaceCreated = ETrue;
			iControlsController->HandleEventL( EMPXControlCmdSurfaceCreated );
        	DrawNow();
            break;
        }
        default:
        {
            iView->HandleCommandL( aCommand );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackContainer::CreateControlsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::CreateControlsL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackContainer::CreateControlsL()"));

    //
    //  Query playback plugin for filename and mode
    //
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    iView->RetrieveFileNameAndModeL( cmd );

    //
    //  Create a temporary file details that is populated with the
    //  file name and playback mode.  This will be delete when
    //  plugin initialization is complete
    //
    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }

    iFileDetails = CMPXVideoPlaybackViewFileDetails::NewL();

    TPtrC fileName( cmd->ValueText( KMPXMediaVideoPlaybackFileName ) );
    iFileDetails->iClipName = fileName.AllocL();

    iFileDetails->iPlaybackMode = (TMPXVideoMode) cmd->ValueTObjectL<TInt>( KMPXMediaVideoMode );

    iFileDetails->iTvOutConnected   = cmd->ValueTObjectL<TInt>( KMPXMediaVideoTvOutConnected );
    iFileDetails->iTvOutPlayAllowed = cmd->ValueTObjectL<TInt>( KMPXMediaVideoTvOutPlayAllowed );

    TPtrC mimeType( cmd->ValueText( KMPXMediaVideoRecognizedMimeType ) );
    iFileDetails->iMimeType = mimeType.AllocL();

    CleanupStack::PopAndDestroy( cmd );

    if ( iControlsController )
    {
        delete iControlsController;
        iControlsController = NULL;
    }

    iControlsController = CMPXVideoPlaybackControlsController::NewL( this, Rect(), iFileDetails );

    //
    //  Retrieve the Real One Logo bitmap
    //
    iRealOneBitmap = iControlsController->GetBitmap( EMPXRealLogoBitmap );
    iTvOutConnected = iFileDetails->iTvOutConnected;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackContainer::UserInputHandler()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMPXVideoPlaybackUserInputHandler* CMPXVideoPlaybackContainer::UserInputHandler()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::UserInputHandler()"));

    return iUserInputHandler;
}

// EOF
