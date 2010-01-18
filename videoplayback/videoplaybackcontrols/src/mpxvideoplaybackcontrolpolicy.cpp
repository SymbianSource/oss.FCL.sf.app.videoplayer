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
* Description:  Implementation of MPXVideoPlaybackControlPolicy
*
*/

// Version : %version: 9 %


// INCLUDE FILES

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolpolicy.h"

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlPolicy::CMPXVideoPlaybackControlPolicy()
// C++ default constructor can NOT contain any code, that might leave.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControlPolicy::CMPXVideoPlaybackControlPolicy()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlPolicy::CMPXVideoPlaybackControlPolicy()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlPolicy::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlPolicy::ConstructL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlPolicy::ConstructL()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlPolicy::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControlPolicy* CMPXVideoPlaybackControlPolicy::NewL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlPolicy::NewL()"));

    CMPXVideoPlaybackControlPolicy* self = new (ELeave) CMPXVideoPlaybackControlPolicy();

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlPolicy::~CMPXVideoPlaybackControlPolicy()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControlPolicy::~CMPXVideoPlaybackControlPolicy()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlPolicy::~CMPXVideoPlaybackControlPolicy()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlPolicy::SetControlProperties()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlPolicy::SetControlProperties(
                                         TMPXVideoPlaybackControls aControlIndex,
                                         TUint& aProperties,
                                         CMPXVideoPlaybackViewFileDetails* aDetails )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlPolicy::SetControlProperties()"));

    aProperties = 0;

    switch ( aControlIndex )
    {
        case EMPXSoftkeyBack:
        {
            //
            //  Transitory Buffering states will be filtered out by the View
            //
            aProperties = EMPXBufferingControl;

            //
            //  Do not show Back when Initializing for Local Mode
            //  It causes a flicker when playback begins
            //
            if ( aDetails->iPlaybackMode != EMPXVideoLocal )
            {
                aProperties += EMPXShownWhenInitializing;
            }
        }
        case EMPXSoftkeyDetails:
        {
            aProperties += EMPXSoftkeyControl |
                           EMPXShownWhenPlaying |
                           EMPXShownWhenPaused |
                           EMPXShownWhenStopped |
                           EMPXShownWhenSeeking;
            break;
        }
        case EMPXBufferingLabel:
        {
            aProperties = EMPXBufferingControl;
            break;
        }
        case EMPXTitleLabel:
        case EMPXArtistLabel:
        {
            aProperties = EMPXBufferingControl;
        }
        case EMPXButtonBar:
        case EMPXProgressBar:
        case EMPXVolumeBar:
        case EMPXAspectRatioIcon:
        {
            aProperties += EMPXShownWhenPlaying |
                           EMPXShownWhenPaused |
                           EMPXShownWhenStopped |
                           EMPXShownWhenSeeking;
            break;
        }
        case EMPXNoVideoBitmap:
        case EMPXRealAudioBitmap:
        case EMPXSoundBitmap:
        {
            aProperties = EMPXBufferingControl |
                          EMPXShownWhenPlaying |
                          EMPXShownWhenPaused |
                          EMPXShownWhenStopped |
                          EMPXShownWhenSeeking;
            break;
        }
        case EMPXDownloadPausedIcon:
        {
            aProperties = EMPXAllProperties;
            break;
        }
        case EMPXVideoPausedIcon:
        {
            aProperties = EMPXShownWhenPaused;
            break;
        }
        case EMPXBrandingAnimation:
        {
            aProperties = EMPXShownWhenInitializing;
            break;
        }
    }
}

// End of File
