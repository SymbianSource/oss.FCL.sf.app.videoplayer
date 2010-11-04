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
* Description:  Implementation of MPXVideoPlaybackControlConfiguration
*
*/


// Version : %version: 17 %


// INCLUDE FILES
#include <e32def.h>
#include <e32base.h>
#include <AknUtils.h>

#include "mpxvideoplaybackcontrolconfiguration.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideo_debug.h"

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::CMPXVideoPlaybackControlConfiguration()
// C++ default constructor can NOT contain any code, that might leave.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControlConfiguration::CMPXVideoPlaybackControlConfiguration(
    CMPXVideoPlaybackControlsController* aController)
    : iControlsController( aController )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlConfiguration::ConstructL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlConfiguration::ConstructL()"));

    CreateControlListL();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControlConfiguration*
CMPXVideoPlaybackControlConfiguration::NewL( CMPXVideoPlaybackControlsController* aController )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlConfiguration::NewL()"));

    CMPXVideoPlaybackControlConfiguration* self =
        new (ELeave) CMPXVideoPlaybackControlConfiguration( aController );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::~CMPXVideoPlaybackControlConfiguration()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControlConfiguration::~CMPXVideoPlaybackControlConfiguration()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlConfiguration::~CMPXVideoPlaybackControlConfiguration"));

    iControlsList.Reset();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::CreateControlListL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlConfiguration::CreateControlListL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlConfiguration::CreateControlListL()"));

    CMPXVideoPlaybackViewFileDetails* fileDetails = iControlsController->FileDetails();

    if ( fileDetails->iPlaybackMode == EMPXVideoStreaming ||
         fileDetails->iPlaybackMode == EMPXVideoLiveStreaming )
    {
        //
        //  streaming mode, add branding animation control to show while initializing
        //
        iControlsList.AppendL( EMPXBrandingAnimation );
    }

    iControlsList.AppendL( EMPXSoftkeyDetails );
    iControlsList.AppendL( EMPXSoftkeyBack );
    iControlsList.AppendL( EMPXProgressBar );
    iControlsList.AppendL( EMPXVolumeBar );
    iControlsList.AppendL( EMPXBufferingLabel );

    if ( AknLayoutUtils::PenEnabled() )
    {
        iControlsList.AppendL( EMPXButtonBar );
    }
    else
    {
        iControlsList.AppendL( EMPXVideoPausedIcon );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::ControlList
// -------------------------------------------------------------------------------------------------
//
RArray<TMPXVideoPlaybackControls>& CMPXVideoPlaybackControlConfiguration::ControlList()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlConfiguration::ControlList()"));

    return iControlsList;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::UpdateControlListL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlConfiguration::UpdateControlListL(
    TMPXVideoPlaybackControlCommandIds aEvent, TBool aShowArIcon )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlConfiguration::UpdateControlListL(%d)"), aEvent);

    //
    //  This fuction will be getting called
    //    - After Initialization complete
    //    - When TV-out cable get connected/disconnected
    //
    switch ( aEvent )
    {
        case EMPXControlCmdSetDownloadPaused:
        {
            if ( ! iDownloadPausedAdded )
            {
                iControlsList.AppendL( EMPXDownloadPausedIcon );

                iDownloadPausedAdded = ETrue;
            }

            break;
        }
        case EMPXControlCmdClearDownloadPaused:
        {
            if ( iDownloadPausedAdded )
            {
                DeleteControlFromList( EMPXDownloadPausedIcon );

                iDownloadPausedAdded = EFalse;
            }

            break;
        }
        case EMPXControlCmdTvOutConnected:
        {
            if ( ! iTitleArtistIndicatorsAdded )
            {
                iControlsList.AppendL( EMPXTitleLabel );
                iControlsList.AppendL( EMPXArtistLabel );

                iTitleArtistIndicatorsAdded = ETrue;
            }

            if ( iAspectRatioIconAdded )
            {
                DeleteControlFromList( EMPXAspectRatioIcon );
                iAspectRatioIconAdded = EFalse;
            }

            break;
        }
        case EMPXControlCmdTvOutDisconnected:
        {
            if ( iControlsController->FileDetails()->iVideoEnabled )
            {
                if ( iTitleArtistIndicatorsAdded )
                {
                    DeleteControlFromList( EMPXTitleLabel );
                    DeleteControlFromList( EMPXArtistLabel );

                    iTitleArtistIndicatorsAdded = EFalse;
                }

                if ( ! iAspectRatioIconAdded && aShowArIcon )
                {
                    iControlsList.AppendL( EMPXAspectRatioIcon );
                    iAspectRatioIconAdded = ETrue;
                }
            }
            break;
        }
        case EMPXControlCmdPluginInitialized:
        {
            if ( iControlsController->FileDetails()->iPlaybackMode == EMPXVideoStreaming ||
                 iControlsController->FileDetails()->iPlaybackMode == EMPXVideoLiveStreaming )
            {
                iControlsController->StopBrandingAnimation();
            }

            break;
        }
        case EMPXControlCmdCreateAspectRatioIcon:
        {
            if ( ! iAspectRatioIconAdded )
            {
                iControlsList.AppendL( EMPXAspectRatioIcon );
                iAspectRatioIconAdded = ETrue;
            }

            break;
        }
        case EMPXControlCmdDeleteAspectRatioIcon:
        {
            if ( iAspectRatioIconAdded )
            {
                DeleteControlFromList( EMPXAspectRatioIcon );
                iAspectRatioIconAdded = EFalse;
            }

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::DeleteControlFromList
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlConfiguration::DeleteControlFromList(
        TMPXVideoPlaybackControls aControl )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlConfiguration::DeleteControlFromList(%d)"), aControl);

    TInt index = iControlsList.Find( aControl );

    if ( index != KErrNotFound )
    {
        iControlsList.Remove( index );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::UpdateControlsWithFileDetailsL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlConfiguration::UpdateControlsWithFileDetailsL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlConfiguration::UpdateControlsWithFileDetailsL()"));

    CMPXVideoPlaybackViewFileDetails* fileDetails = iControlsController->FileDetails();

    if ( fileDetails->iVideoEnabled )
    {
        if ( iAudioOnlyIndicatorsAdded )
        {
            DeleteControlFromList( EMPXRealAudioBitmap );
            DeleteControlFromList( EMPXSoundBitmap );
            DeleteControlFromList( EMPXNoVideoBitmap );

            iAudioOnlyIndicatorsAdded = EFalse;
        }

        if ( fileDetails->iTvOutConnected )
        {
            if ( ! iTitleArtistIndicatorsAdded )
            {
                iControlsList.AppendL( EMPXTitleLabel );
                iControlsList.AppendL( EMPXArtistLabel );

                iTitleArtistIndicatorsAdded = ETrue;
            }
        }
        else
        {
            if ( iTitleArtistIndicatorsAdded )
            {
                DeleteControlFromList( EMPXTitleLabel );
                DeleteControlFromList( EMPXArtistLabel );

                iTitleArtistIndicatorsAdded = EFalse;
            }
        }
    }
    else
    {
        //
        //  No video for this clip
        //

        //
        //  Remove the Real One bitmap and show the audio only bitmaps
        //
        iControlsController->SetRealOneBitmapVisibility( EFalse );

        if ( ! iAudioOnlyIndicatorsAdded )
        {
            if ( iControlsController->IsRealMediaFormat() )
            {
                iControlsList.AppendL( EMPXRealAudioBitmap );
            }

            //
            //  this media clip doesn't have a supported video stream,
            //  display "No Video" bitmap instead of
            //  the generic "Sound" bitmap
            //
            if ( fileDetails->iPartialPlayback )
            {
                iControlsList.AppendL( EMPXNoVideoBitmap );
            }
            else
            {
                iControlsList.AppendL( EMPXSoundBitmap );
            }

            iAudioOnlyIndicatorsAdded = ETrue;
        }

        if ( ! iTitleArtistIndicatorsAdded )
        {
            iControlsList.AppendL( EMPXTitleLabel );
            iControlsList.AppendL( EMPXArtistLabel );

            iTitleArtistIndicatorsAdded = ETrue;
        }
    }
}


// End of File
