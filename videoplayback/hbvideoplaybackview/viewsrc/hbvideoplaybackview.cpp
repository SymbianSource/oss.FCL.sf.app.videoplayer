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
* Description:  Implementation of Video playback view
*
*/

// Version : %version:  da1mmcf#10 %



//  Include Files
#include <hbinstance.h>

#include <mmf/common/mmferrors.h>
#include <mpxvideoplaybackdefs.h>

#include "mpxvideo_debug.h"
#include "hbvideoplaybackview.h"
#include "mpxvideoviewwrapper.h"
#include "mpxvideoplaybackuids.hrh"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"


//  Member Functions

// -------------------------------------------------------------------------------------------------
// HbVideoPlaybackView::HbVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoPlaybackView::HbVideoPlaybackView()
{
    MPX_ENTER_EXIT(_L("HbVideoPlaybackView::HbVideoPlaybackView()"));

    initializeVideoPlaybackView();
}

// -------------------------------------------------------------------------------------------------
//   HbVideoPlaybackView::~HbVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
HbVideoPlaybackView::~HbVideoPlaybackView()
{
    MPX_DEBUG(_L("HbVideoPlaybackView::~HbVideoPlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
//   HbVideoPlaybackView::handleActivateView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoPlaybackView::handleActivateView()
{
    MPX_ENTER_EXIT(_L("HbVideoPlaybackView::handleActivateView()"));

    HbVideoBasePlaybackView::handleActivateView();
}

// -------------------------------------------------------------------------------------------------
//   HbVideoPlaybackView::handleDeactivateView()
// -------------------------------------------------------------------------------------------------
//
void HbVideoPlaybackView::handleDeactivateView()
{
    MPX_ENTER_EXIT(_L("HbVideoPlaybackView::handleDeactivateView()"));

    HbVideoBasePlaybackView::handleDeactivateView();
}

// -------------------------------------------------------------------------------------------------
//   HbVideoPlaybackView::handleBack()
// -------------------------------------------------------------------------------------------------
//
void HbVideoPlaybackView::handleBack()
{   
    MPX_ENTER_EXIT(_L("HbVideoPlaybackView::handleBack()"));

    closePlaybackView();
}

// -------------------------------------------------------------------------------------------------
//   HbVideoPlaybackView::handleSoftkeyBack()
// -------------------------------------------------------------------------------------------------
//
void HbVideoPlaybackView::handleSoftkeyBack()
{
    MPX_ENTER_EXIT(_L("HbVideoPlaybackView::handleSoftkeyBack()"));

    closePlaybackView();
}

// -------------------------------------------------------------------------------------------------
//   HbVideoPlaybackView::handlePluginError()
// -------------------------------------------------------------------------------------------------
//
void HbVideoPlaybackView::handlePluginError( int aError )
{
    MPX_DEBUG(_L("HbVideoPlaybackView::handlePluginError() aError = %d"), aError );
   
    switch( aError )
    {
        case KErrNotSupported:
        case KErrUnknown:
        case KErrMMDecoder:
        case KErrCorrupt:
        case KErrTooBig:
        {
            showDialog( hbTrId( "txt_videos_info_invalid_clip_operation_canceled" ) );
            break;
        }

        case KErrMMInvalidProtocol:
        case KErrMMInvalidURL:
        case KErrArgument:
        {
            showDialog( hbTrId( "txt_videos_info_unable_to_connect_invalid_url" ) );
            break;
        }

        case KErrSessionClosed:
        {
            if ( ! mVideoMpxWrapper->IsLive() )
            {
                showDialog( hbTrId( "txt_videos_info_resource_lost" ) );
            }
            else
            {
                handleClosePlaybackView();            
            } 
            break;
        }
        case KErrTimedOut:
        {
            showDialog( hbTrId( "txt_videos_info_unable_to_connect_connection_time" ) );
            break;
        }
        case KErrNotFound:
        {
            showDialog( hbTrId( "txt_videos_info_unable_to_connect_file_not_found" ) );
            break;
        }
        case KErrMMNotEnoughBandwidth:
        {
            showDialog( hbTrId( "txt_videos_info_unable_to_connect_not_enough_band" ) );
            break;
        }
        case KErrDisconnected:
        case KErrMMProxyServer:
        case KErrCouldNotConnect:
        case KErrAbort:
        {
            showDialog( hbTrId( "txt_videos_info_unable_to_connect" ) );
            break;
        }
        case KMPXVideoPlayOver2GDuringVoiceCallError:
        {
            showDialog( hbTrId( "txt_videos_info_video_play_not_allowed_during_voic" ), false );
            break;
        }
        default:
        {
            HbVideoBasePlaybackView::handlePluginError( aError );

            break;
        }
    }
}

// -----------------------------------------------------------------------------
//   HbVideoPlaybackView::handlePdlStateChange
// -----------------------------------------------------------------------------
//
void HbVideoPlaybackView::handlePdlStateChange( int /*aState*/ )
{
    MPX_DEBUG(_L("HbVideoPlaybackView::handlePdlStateChange()"));
}

// -----------------------------------------------------------------------------
//   HbVideoPlaybackView::handleStoppedState
// -----------------------------------------------------------------------------
//
void HbVideoPlaybackView::handleStoppedState()
{
    MPX_DEBUG(_L("HbVideoPlaybackView::HandleStoppedState()"));

    if ( ! mVideoMpxWrapper->IsPlaylist() )
    {        
        handleBack();
    }
}

// EOF
