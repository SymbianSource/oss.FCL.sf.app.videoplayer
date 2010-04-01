/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

// Version : %version: 1 %



// INCLUDE FILES

#include "mpxvideo_debug.h"
#include "mpxvideoplaybackcontrolpolicy.h"
#include "mpxvideoplaybackviewfiledetails.h"

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlPolicy::CMPXVideoPlaybackControlPolicy()
// C++ default constructor can NOT contain any code, that might leave.
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlPolicy::QMPXVideoPlaybackControlPolicy()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlPolicy::QMPXVideoPlaybackControlPolicy()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlPolicy::~QMPXVideoPlaybackControlPolicy()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
QMPXVideoPlaybackControlPolicy::~QMPXVideoPlaybackControlPolicy()
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlPolicy::~QMPXVideoPlaybackControlPolicy()"));
}

// -------------------------------------------------------------------------------------------------
// QMPXVideoPlaybackControlPolicy::setControlProperties()
// -------------------------------------------------------------------------------------------------
//
void 
QMPXVideoPlaybackControlPolicy::setControlProperties( TMPXVideoPlaybackControls controlIndex,
                                                      TUint& properties,
                                                      QMPXVideoPlaybackViewFileDetails *details,
                                                      TPlaybackViewMode viewMode )
{
    MPX_DEBUG(_L("QMPXVideoPlaybackControlPolicy::setControlProperties()"));

    properties = 0;

    switch ( controlIndex )
    {
        case EMPXBufferingAnimation:
        {
            properties = EMPXShownWhenInitializing |
                         EMPXShownWhenBuffering;
            break;
        }
        case EMPXStatusPane:
        {
            if ( details->mPlaybackMode != EMPXVideoLocal ||
            	   viewMode == EAudioOnlyView ||
            	   viewMode == EDetailsView )
            {                
                properties = EMPXAllProperties;
            }
            else
            {
                properties = EMPXShownWhenPlaying |
                             EMPXShownWhenPaused |
                             EMPXShownWhenSeeking;
            }
            break;
        }
        case EMPXButtonBar:
        case EMPXProgressBar:
        {
            //
            // We don't delete ButtonBar and Progress Bar from the controlsList 
            // to update information all the time though it's not visible or activated
            // So just hide these if it is details view
            //
            if ( viewMode == EDetailsView )
            {
                properties = 0;
            }
            else if ( viewMode == EAudioOnlyView )
            {
                properties = EMPXAllProperties;
            }
            else
            {
                properties = EMPXShownWhenPlaying |
                             EMPXShownWhenPaused |
                             EMPXShownWhenSeeking;
            }
            break;
        }
        case EMPXFileDetailsWidget:
        case EMPXIndicatorBitmap:
        case EMPXDetailsViewPlaybackWindow:
        {
            //
            // Add all these controls when it is details view or audio only view
            // Once it's added to controlsList, these are visible always
            //
            properties = EMPXAllProperties;
            break;
        }
    }
}

// End of File
