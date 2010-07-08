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
* Description:   Implementation of video playback display handler
*
*/

// Version : %version:  8 %

#include <sysutil.h>
#include <s32file.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackutility.h>
#include <mpxvideoplaybackdefs.h>

#include "mpxvideoviewwrapper.h"
#include "mpxvideoplaybackdisplayhandler.h"
#include "mpxvideoregion.h"
#include "videoplaybackviewfiledetails.h"


CMPXVideoPlaybackDisplayHandler::CMPXVideoPlaybackDisplayHandler( MMPXPlaybackUtility* aPlayUtil,
                                                                  CMPXVideoViewWrapper* aViewWrapper )
    : iPlaybackUtility( aPlayUtil )
    , iViewWrapper( aViewWrapper )
{
}

CMPXVideoPlaybackDisplayHandler::~CMPXVideoPlaybackDisplayHandler()
{
}

CMPXVideoPlaybackDisplayHandler*
CMPXVideoPlaybackDisplayHandler::NewL( MMPXPlaybackUtility* aPlayUtil,
                                       CMPXVideoViewWrapper* aViewWrapper )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::NewL()"));

    CMPXVideoPlaybackDisplayHandler* self =
        new(ELeave) CMPXVideoPlaybackDisplayHandler( aPlayUtil, aViewWrapper );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackDisplayHandler::ConstructL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::ConstructL()
{
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL(
                                          RWsSession& aWs,
                                          CWsScreenDevice& aScreenDevice,
                                          RWindow& aWin,
                                          TRect aDisplayRect )
{
    Q_UNUSED( aWs );
    Q_UNUSED( aScreenDevice );
    Q_UNUSED( aWin );
    Q_UNUSED( aDisplayRect );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::RemoveDisplayWindow()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::RemoveDisplayWindow()
{
    if ( iVideoDisplay )
    {
        delete iVideoDisplay;
        iVideoDisplay = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL( CMPXMessage* /*aMessage*/ )
{
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::SetAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    iCommand = aCmd;
    
    switch ( aCmd )
    {
        case EPbCmdNaturalAspectRatio:
        {
            iAspectRatio = EMMFNatural;
            break;
        }
        case EPbCmdZoomAspectRatio:
        {
            iAspectRatio = EMMFZoom;
            break;
        }
        case EPbCmdStretchAspectRatio:
        {
            iAspectRatio = EMMFStretch;
            break;
        }
    }

    return iAspectRatio;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetDefaultAspectRatioL
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::SetDefaultAspectRatioL(
                                          VideoPlaybackViewFileDetails* aFileDetails,
                                          TReal32 aDisplayAspectRatio )
{
    Q_UNUSED( aFileDetails );
    Q_UNUSED( aDisplayAspectRatio );

    return iAspectRatio;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::UpdateVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::UpdateVideoRectL( TRect aClipRect, TBool transitionEffect  )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::UpdateVideoRectL()"));

    Q_UNUSED( transitionEffect );

    iRect = aClipRect;
}

// End of File
