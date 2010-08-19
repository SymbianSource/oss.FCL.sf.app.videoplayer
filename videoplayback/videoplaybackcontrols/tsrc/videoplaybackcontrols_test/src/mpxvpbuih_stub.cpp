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

// Version : %version: 8 %



// INCLUDE FILES
#include <w32std.h> // RWindowBase
#include <e32base.h>
#include <stringloader.h>
#include <aknutils.h>
#include <eikclbd.h>
#include <aknconsts.h>
#include <AknUtils.h>
#include <AknIconUtils.h>  // SetSize for the icons
#include <remconcoreapi.h>
#include <RemConCoreApiTarget.h>
#include <remconinterfaceselector.h> // Side volume key
#include <mpxplaybackframeworkdefs.h>

#include "mpxvpbc_stub.h"
#include "mpxvideoplaybackcontrol.hrh"
#include "mpxvpbuih_stub.h"

#include <mpxvideoplaybackdefs.h>
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideo_debug.h"


// CONSTANTS

// ======== MEMBER FUNCTIONS =======================================================================

// -------------------------------------------------------------------------------------------------
// MPXVideoPlaybackUserInputHandler::CMPXVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackUserInputHandler::CMPXVideoPlaybackUserInputHandler(CMPXVideoPlaybackContainer* aContainer)
   : iContainer(aContainer)
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::NewL()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackUserInputHandler* CMPXVideoPlaybackUserInputHandler::NewL(
        CMPXVideoPlaybackContainer* aContainer)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::NewL()"));

    CMPXVideoPlaybackUserInputHandler* self = new (ELeave) CMPXVideoPlaybackUserInputHandler( aContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackUserInputHandler::ConstructL()"));

    iProcessingInputType = EMpxVideoNone;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::~CMPXVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackUserInputHandler::~CMPXVideoPlaybackUserInputHandler()
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::ProcessPointerEvent()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::ProcessPointerEventL( CCoeControl* aControl,
                                                              const TPointerEvent& aPointerEvent,
                                                              TMPXVideoControlType /*aMPXControl*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackUserInputHandler::ProcessPointerEvent"));

    MPX_DEBUG(_L("   iType = %d, iPosition = (%d,%d)"),
       aPointerEvent.iType, aPointerEvent.iPosition.iX, aPointerEvent.iPosition.iY );

    switch (iProcessingInputType)
    {
        case EMpxVideoNone:
        {
            if (aPointerEvent.iType == TPointerEvent::EButton1Down)
            {
                iProcessingInputType = EMpxVideoTouch;
                static_cast<CMPXVideoPlaybackControl*>(aControl)->DoHandlePointerEventL( aPointerEvent );
            }
            break;
        }
        case EMpxVideoTouch:
        {
            if (aPointerEvent.iType != TPointerEvent::EButton1Down)
            {
                static_cast<CMPXVideoPlaybackControl*>(aControl)->DoHandlePointerEventL( aPointerEvent );

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
            if (aType == EEventKeyDown)
            {
                iProcessingInputType = EMpxVideoKeyboard;
                iLastPressedKeyScanCode = aKeyEvent.iScanCode;
                iContainer->DoHandleKeyEventL(aKeyEvent, aType);
            }
            break;
        }
        case EMpxVideoKeyboard:
        {
            if ( aKeyEvent.iScanCode == iLastPressedKeyScanCode )
            {
                iContainer->DoHandleKeyEventL( aKeyEvent, aType );

                if ( aType == EEventKeyUp )
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

// EOF
