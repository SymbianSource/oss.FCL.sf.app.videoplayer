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
* Description:  Implementation of Video playback view
*
*/


// Version : %version: 18 %


//  Include Files
#include <aknViewAppUi.h>
#include <mpxplaybackutility.h>
#include <mpxviewutility.h>
#include <mpxcollectionutility.h>
#include <mpxvideoplaybackviews.rsg>
#include <eikmenup.h>
#include <mmf/common/mmferrors.h>

#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackuids.hrh"
#include "mpxvideoplaybackview.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontainer.h"
#include "mpxvideo_debug.h"

//  Member Functions

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackView::NewLC()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMPXVideoPlaybackView* CMPXVideoPlaybackView::NewLC()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackView::NewLC()"));

    CMPXVideoPlaybackView* self = new (ELeave) CMPXVideoPlaybackView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackView::NewL()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMPXVideoPlaybackView* CMPXVideoPlaybackView::NewL()
{
    CMPXVideoPlaybackView* self = CMPXVideoPlaybackView::NewLC();
    CleanupStack::Pop( self );
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackView::CMPXVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackView::CMPXVideoPlaybackView()
// note, CBase initialises all member variables to zero
{
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackView::ConstructL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackView::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackView::ConstructL()"));

    InitializeVideoPlaybackViewL();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackView::~CMPXVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackView::~CMPXVideoPlaybackView()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackView::~CMPXVideoPlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
//   From CAknView
//   CMPXVideoPlaybackView::Id() const
// -------------------------------------------------------------------------------------------------
//
TUid CMPXVideoPlaybackView::Id() const
{
    return TUid::Uid( KMPXVIDEOPLAYBACKVIEWUID );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackView::ViewImplementationIdId()
// -------------------------------------------------------------------------------------------------
//
TUid CMPXVideoPlaybackView::ViewImplementationId() const
{
    return TUid::Uid( KMPXVIDEOPLAYBACKVIEWPLUGINIMPLEMENTATIONUID );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackView::CloseEmbeddedPlaybackViewL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackView::CloseEmbeddedPlaybackViewL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackView::CloseEmbeddedPlaybackViewL()"));

    ActivateClosePlayerActiveObject();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackView::HandlePluginErrorL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackView::HandlePluginErrorL( TInt aError )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackView::HandlePluginErrorL() aError = %d"), aError );

    //
    // error, stop branding animation
    //
    if ( iContainer )
    {
        iContainer->HandleEventL( EMPXControlCmdHandleErrors, iPlaybackState );
    }

    switch( aError )
    {
        case KErrNotSupported:
        case KErrUnknown:
        case KErrMMDecoder:
        case KErrCorrupt:
        case KErrTooBig:
        {
            DisplayErrorMessageL( R_MPX_VIDEO_INVALID_CLIP );
            HandleClosePlaybackViewL();
            break;
        }

        case KErrMMInvalidProtocol:
        case KErrMMInvalidURL:
        case KErrArgument:
        {
            DisplayErrorMessageL(R_MPX_INVALID_URL);
            HandleClosePlaybackViewL();
            break;
        }

        case KErrSessionClosed:
        {
            if ( iFileDetails->iPlaybackMode != EMPXVideoLiveStreaming )
            {
                DisplayErrorMessageL( R_MPX_RESOURCE_LOST );
            }
            HandleClosePlaybackViewL();
            break;
        }
        case KErrTimedOut:
        {
            DisplayErrorMessageL( R_MPX_IN_CONNECTION_TIMEOUT );
            HandleClosePlaybackViewL();
            break;
        }
        case KErrMMNotEnoughBandwidth:
        {
            DisplayErrorMessageL( R_MPX_LOW_BANDWIDTH );
            HandleClosePlaybackViewL();
            break;
        }
        case KErrDisconnected:
        case KErrMMProxyServer:
        case KErrCouldNotConnect:
        case KErrAbort:
        {
            DisplayErrorMessageL( R_MPX_COULD_NOT_CONNECT );
            HandleClosePlaybackViewL();
            break;
        }
        case KMPXVideoPlayOver2GDuringVoiceCallError:
        {
            DisplayInfoMessageL(R_MPX_VIDEO_PLAY_OVER_2G_DURING_VOICE_CALL);
            break;
        }
        default:
        {
            CMPXVideoBasePlaybackView::HandlePluginErrorL( aError );

            break;
        }
    }
}

// -----------------------------------------------------------------------------
//   CMPXVideoPlaybackView::HandlePdlStateChangeL
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackView::HandlePdlStateChangeL( TInt /*aState*/ )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackView::HandlePdlStateChangeL()"));
}

// -----------------------------------------------------------------------------
//   CMPXVideoPlaybackView::HandleStoppedStateL
// -----------------------------------------------------------------------------
//
void CMPXVideoPlaybackView::HandleStoppedStateL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackView::HandleStoppedStateL()"));

    if ( ! iPlaylistView || ( ! IsAppInFrontL() && ! iFileDetails->iPausableStream ) )
    {
        HandleCommandL( EAknSoftkeyBack );
    }
    else if ( iPlaylistView )
    {
        //
        //  New item from the playlist clean up the view
        //
        RemoveBackgroundSurfaceL();
    }
}


// EOF
