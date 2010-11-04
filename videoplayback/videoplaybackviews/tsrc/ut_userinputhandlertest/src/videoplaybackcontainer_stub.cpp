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
 * Description:  Test Harness for Video Playback Views
 *
 */

// Version : %version: ou1cpsw#3 %


// INCLUDES
#include <coeaui.h>
#include <aknutils.h>
#include <aknsutils.h>
#include <aknviewappui.h>
#include <aknsdrawutils.h>
#include <aknsbasicbackgroundcontrolcontext.h>
#include <RemConCoreApiTarget.h>
#include <remconinterfaceselector.h> // Side volume key

#include "videoplaybackcontainer_stub.h"
#include "mpxhelixplaybackplugindefs.h"
#include "mpxvideoplaybackuserinputhandler.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideo_debug.h"

//
// extension of TPointerEvent::TType in w32std.h
//
TInt KContainerPtrEventNull = 20;

//
// extension of TMPXVideoPlaybackViewCommandIds in
// mpxcommonvideoplaybackview.hrh
//
TInt KCmdNone = 9000;

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
void CMPXVideoPlaybackContainer::ConstructL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::ConstructL()"));

    //
    // pre-set defaults
    //
    iKeyEvent.iScanCode = EStdKeyNull;
    iEventCode = EEventNull;
    iPointerEvent.iType = (TPointerEvent::TType) KContainerPtrEventNull;
    iCommand = KCmdNone;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackContainer* CMPXVideoPlaybackContainer::NewL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::NewL()"));

    CMPXVideoPlaybackContainer* self =
            new (ELeave) CMPXVideoPlaybackContainer();

    CleanupStack::PushL(self);
    self->ConstructL();
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
}

//
// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::HandleCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::HandleCommandL( TInt aCommand,
                                                 TInt /* aValue */ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::HandleCommandL()"));
    iCommand = aCommand;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::DoHandleKeyEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::DoHandleKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::DoHandleKeyEventL()"));

    iKeyEvent = aKeyEvent;
    iEventCode = aType;

    switch ( aKeyEvent.iScanCode )
    {
        case EStdKeyDevice3:        // rocker middle key
        case EStdKeyEnter:          // enter key on a qwerty key board
        {
            if ( aKeyEvent.iCode == EKeyNull && aType == EEventKeyDown )
            {
                iCommand = EMPXPbvCmdPlayPause;
            }
            break;
        }
        case EStdKeyUpArrow:        // rocker up key
        {
            if (aType == EEventKeyDown)
            {
            iCommand = EMPXPbvCmdIncreaseVolume;
            }
            break;
        }
        case EStdKeyDownArrow:      // rocker down key
        {
            if (aType == EEventKeyDown)
            {
            iCommand = EMPXPbvCmdDecreaseVolume;
            }
            break;
        }
        case EStdKeyLeftArrow:      // rocker left key
        {
            if (aType == EEventKeyDown)
            {
                iCommand = EMPXPbvCmdSeekBackward;
            }
            else if (aType == EEventKeyUp)
            {
                iCommand = EMPXPbvCmdEndSeek;
            }
            break;
        }
        case EStdKeyRightArrow:     // rocker right key
        {
            if (aType == EEventKeyDown)
            {
                iCommand = EMPXPbvCmdSeekForward;
            }
            else if (aType == EEventKeyUp)
            {
                iCommand = EMPXPbvCmdEndSeek;
            }
            break;
        }
        case EStdKeyYes:            // green/send key
        {
            // in playback view we dont want to take any action
            // just consume the key
            iCommand = KCmdNone;
            break;
        }
        case EStdKeyDevice0:
        case EStdKeyDevice1:
        {
            if (aType == EEventKeyDown)
            {
                iCommand = EMPXControlCmdSoftKeyPressed;
            }
            break;
        }
        default:
            break;
    }

}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackContainer::DoHandlePointerEventL()
//
// Gets called from the Controls Controller
// only if if OK to handle pointer events
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackContainer::DoHandlePointerEventL(
        const TPointerEvent& aPointerEvent)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackContainer::DoHandlePointerEventL()"));

    iPointerEvent = aPointerEvent;
}

//  End of File
