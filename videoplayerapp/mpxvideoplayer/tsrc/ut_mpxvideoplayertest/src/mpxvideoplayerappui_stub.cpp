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
* Description:  mpxvideoplayerappui stub for STIF testing
*
*/

// Version : %version: ou1cpsw#4 %


#include <avkon.hrh>

#include "mpxvideoplayer_appuienginetester.h"
#include "mpxvideoplayerappui_stub.h"
#include "mpxvideo_debug.h"


// ======== MEMBER FUNCTIONS ========
// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::CMpxVideoPlayerAppUi
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerAppUi::CMpxVideoPlayerAppUi( TBool aEmbedded,
                                            MAppUiEngineStifTestObserver* aObserver )
    : iEmbedded( aEmbedded ),
      iStifObserver( aObserver )
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUi::CMpxVideoPlayerAppUi(%d)"), aEmbedded);
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::~CMpxVideoPlayerAppUi
// Destructor. Frees reserved resources
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerAppUi::~CMpxVideoPlayerAppUi()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::~CMpxVideoPlayerAppUi()"));
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::HandleCommandL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::HandleCommandL( TInt aCommand )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::HandleCommandL()"),
                   _L("aCommand = %d"), aCommand );

    switch ( aCommand )
    {
        case EEikCmdExit: // fall through
        case EAknCmdExit:
        {
            TCallbackEvent* event = new TCallbackEvent;

            event->iEvent = EAppUiCmdExit;

            iStifObserver->HandleUtilityEvent( event );

            break;
        }
        case EAknSoftkeyBack:
        {
            break;
        }
        case EAknCmdHelp:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Querying whether the application is launched in embedded mode or not.
// ---------------------------------------------------------------------------
//
TBool CMpxVideoPlayerAppUi::IsEmbedded() const
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUi::IsEmbedded(%d)"), iEmbedded );

    return iEmbedded;
}

// ---------------------------------------------------------------------------
// Return active view.
// ---------------------------------------------------------------------------
CAknView* CMpxVideoPlayerAppUi::View()
{
    return NULL;
}
