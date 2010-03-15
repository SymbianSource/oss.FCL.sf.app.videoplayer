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
* Description:  Test Harness for VideoPlaybackControls
*
*/

// Version : %version: e003sa33#14 %


// INCLUDES
#include <coeaui.h>
#include <aknutils.h>
#include <aknsutils.h> 
#include <aknviewappui.h>
#include <aknsdrawutils.h>
#include <aknsbasicbackgroundcontrolcontext.h>

#include "mpxvideoplaybackviewfiledetails.h"
#include "videoplaybackcontrolstest.h"
#include "mpxvpbc_stub.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxhelixplaybackplugindefs.h"
#include "mpxvpbuih_stub.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideo_debug.h"

// CONSTANTS

_LIT( KFileTitle, "asdfasdfasdfasdfasdfasd asdfasdfasdfasdfasdfasd" );

const TInt KMaxFileTitleLength = 2048;


// ============================ MEMBER FUNCTIONS ===================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::CMPXVideoPlaybackContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -------------------------------------------------------------------------------------------------
//

CMPXVideoPlaybackContainer::CMPXVideoPlaybackContainer()
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::ConstructL( const TDesC& aFileName )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::ConstructL()"));

    iFileDetails = CMPXVideoPlaybackViewFileDetails::NewL();    
    iFileDetails->iClipName = HBufC::NewL( aFileName.Length() );
    iFileDetails->iClipName->Des().Copy( aFileName );

    CreateWindowL();       

    iUserInputHandler = CMPXVideoPlaybackUserInputHandler::NewL( this );
    
    SetFocus( ETrue );
    
    MakeVisible( ETrue );

    ActivateL();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackContainer* CMPXVideoPlaybackContainer::NewL( const TDesC& aFileName )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::NewL( %S )"),&aFileName);

    CMPXVideoPlaybackContainer* self = new (ELeave) CMPXVideoPlaybackContainer();


    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    CleanupStack::Pop();

    return self;
}

// 
// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::~CMPXVideoPlaybackContainer()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackContainer::~CMPXVideoPlaybackContainer()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::~CMPXVideoPlaybackContainer()"));

    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }   
    
    if ( iUserInputHandler )
    {
        delete iUserInputHandler;
        iUserInputHandler = NULL;
    } 

    CloseWindow();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::CountComponentControls
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackContainer::CountComponentControls() const
{
    return 0;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::Draw
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::Draw(const TRect& aRect) const
{
    CWindowGc& gc = SystemGc();

    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbBlack );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
}

// 
// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::HandleCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::HandleCommandL( TInt aCommand, TInt aValue )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::HandleCommandL(aCommand=%d,aValue=%d)"),aCommand,aValue);

    iCommand = aCommand;
    iValue = aValue;
}

// 
// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::ExpectedResultL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::ExpectedResultL( TInt aCommand, TInt aValue )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::ExpectedResultL(aCommand=%d,aValue=%d)"),aCommand,aValue);

    if ( iCommand != aCommand || iValue != aValue )
    {
        User::Leave( KErrGeneral );
    }
}

//
// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::SetController()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::SetController(
    CMPXVideoPlaybackControlsController* aControlsController )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::SetController()"));

    iControlsController = aControlsController;
}

//
// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::AddFileDetailsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::AddFileDetailsL( TBool aHasVideoTrack )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::AddFileDetailsL() "));

    iFileDetails->iAudioEnabled = ETrue;
    iFileDetails->iVideoEnabled = aHasVideoTrack;
    iFileDetails->iSeekable = ETrue;
    
    iFileDetails->iTitle    = _L("Test Title").Alloc();

    if ( iFileDetails->iVideoEnabled )
    {        
        iFileDetails->iVideoHeight = 176;
        iFileDetails->iVideoWidth = 144;
        
        iFileDetails->iBitRate = 8000;
        iFileDetails->iMimeType = _L("video/3gp").Alloc();        
    }

    if ( iFileDetails->iPlaybackMode == EMPXVideoLiveStreaming )
    {
        iFileDetails->iSeekable = EFalse;
    }

    iControlsController->AddFileDetailsL( iFileDetails );
}

//
// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::AddFileDetailsL()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackViewFileDetails*
CMPXVideoPlaybackContainer::FileDetails( TInt aMode )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::AddFileDetailsL() "));

    iFileDetails->iPlaybackMode = (TMPXVideoMode)aMode;

    return iFileDetails;
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Handles key events.
// -------------------------------------------------------------------------------------------------
//
TKeyResponse CMPXVideoPlaybackContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                         TEventCode aType )
{
    iKeyResponse = EKeyWasNotConsumed;
    
    iUserInputHandler->ProcessKeyEventL( aKeyEvent, aType );
    
    return iKeyResponse;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::DoHandleKeyEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::DoHandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::DoHandleKeyEventL()"));
    
    switch ( aKeyEvent.iScanCode )
    {
        case EStdKeyDevice3:        // rocker middle key
        case EStdKeyEnter:          // enter key on a qwerty key board
        {
            HandleCommandL(EMPXPbvCmdPlayPause);
            iKeyResponse = EKeyWasConsumed;
            break;
        }
        case EStdKeyUpArrow:        // rocker up key
        {
            HandleCommandL(EMPXPbvCmdIncreaseVolume);
            iKeyResponse = EKeyWasConsumed;
            break;
        }
        case EStdKeyDownArrow:      // rocker down key
        {
            HandleCommandL( EMPXPbvCmdDecreaseVolume );
            iKeyResponse = EKeyWasConsumed;
            break;
        }
        case EStdKeyLeftArrow:      // rocker left key
        {
            if (aType == EEventKeyDown)
            {
                HandleCommandL(EMPXPbvCmdSeekBackward);
            }
            else if (aType == EEventKeyUp)
            {
                HandleCommandL(EMPXPbvCmdEndSeek);
            }
            
            iKeyResponse = EKeyWasConsumed;
            break;
        }
        case EStdKeyRightArrow:     // rocker right key
        {
            if (aType == EEventKeyDown)
            {
                HandleCommandL(EMPXPbvCmdSeekForward);
            }
            else if (aType == EEventKeyUp)
            {
                HandleCommandL(EMPXPbvCmdEndSeek);
            }
            
            iKeyResponse = EKeyWasConsumed;
            break;
        }
        case EStdKeyYes:            // green/send key
        {
            // in playback view we dont want to take any action
            // just consume the key
            iKeyResponse = EKeyWasConsumed;
            break;
        }
        default:
        {
            break;
        }
    }    
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::UserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackUserInputHandler* CMPXVideoPlaybackContainer::UserInputHandler()
{
    return iUserInputHandler;
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::AddLongFileDetailsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::AddLongFileDetailsL( TBool aHasVideoTrack )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::AddFileDetailsL() "));

    iFileDetails->iAudioEnabled = ETrue;
    iFileDetails->iVideoEnabled = aHasVideoTrack;
    iFileDetails->iSeekable = ETrue;

    HBufC* temp = HBufC::NewLC(KMaxFileTitleLength);
    TPtr fileTitlePtr = temp->Des();

    const int loop = 10;
    for ( int i=0; i<loop; i++ )
    {
        fileTitlePtr.Append( KFileTitle() );
    }
    iFileDetails->iTitle  = fileTitlePtr.Alloc();
    CleanupStack::PopAndDestroy();  //temp

    if ( iFileDetails->iVideoEnabled )
    {
        iFileDetails->iVideoHeight = 176;
        iFileDetails->iVideoWidth = 144;

        iFileDetails->iBitRate = 8000;
        iFileDetails->iMimeType = _L("video/3gp").Alloc();
    }

    if ( iFileDetails->iPlaybackMode == EMPXVideoLiveStreaming )
    {
        iFileDetails->iSeekable = EFalse;
    }

    iControlsController->AddFileDetailsL( iFileDetails );
}


void CMPXVideoPlaybackContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackContainer::HandlePointerEventL()"));

    MPX_DEBUG(_L("   iType = %d, iPosition = (%d,%d)"), 
       aPointerEvent.iType, aPointerEvent.iPosition.iX, aPointerEvent.iPosition.iY );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::AddSameARFileDetailsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::AddSameARFileDetailsL( TBool aHasVideoTrack )
{
	MPX_ENTER_EXIT( _L( "CMPXVideoPlaybackContainer::AddSameARFileDetailsL() ") );

    iFileDetails->iAudioEnabled = ETrue;
    iFileDetails->iVideoEnabled = aHasVideoTrack;
    iFileDetails->iSeekable = ETrue;

    if ( iFileDetails->iVideoEnabled )
    {        
        iFileDetails->iVideoHeight = this->Rect().Height();
        iFileDetails->iVideoWidth = this->Rect().Width();
        
        iFileDetails->iBitRate = 8000;
        iFileDetails->iMimeType = _L("video/3gp").Alloc();        
    }

    if ( iFileDetails->iPlaybackMode == EMPXVideoLiveStreaming )
    {
        iFileDetails->iSeekable = EFalse;
    }

    iControlsController->AddFileDetailsL( iFileDetails );
}

//  End of File
