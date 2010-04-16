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

// Version : %version:  4 %

#include <sysutil.h>
#include <s32file.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackutility.h>
#include <mpxvideoplaybackdefs.h>

#include "mpxvideoviewwrapper.h"
#include "mpxvideoplaybackdisplayhandler.h"
#include "mpxvideoregion.h"
#include "mpxvideoplaybackviewfiledetails.h"

const TInt KVIDEORESIZINGREPEATRATE = 50000;
const TReal32 KTRANSITIONEFFECTCNT = 8;

_LIT( KAspectRatioFile, "c:\\private\\200159b2\\mpxvideoplayer_aspect_ratio.dat" );


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
    LoadAspectRatioL();
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
#ifdef SYMBIAN_BUILD_GCE
    if ( iVideoDisplay )
    {
        delete iVideoDisplay;
        iVideoDisplay = NULL;
    }
#else
    if ( iDirectScreenAccess )
    {
        delete iDirectScreenAccess;
        iDirectScreenAccess = NULL;
    }
#endif
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL( CMPXMessage* aMessage )
{

    TMPXVideoDisplayCommand message =
        ( *(aMessage->Value<TMPXVideoDisplayCommand>(KMPXMediaVideoDisplayCommand)) );

    switch ( message )
    {
#ifdef SYMBIAN_BUILD_GCE    	
        case EPbMsgVideoSurfaceCreated:
        {
            SurfaceCreatedL( aMessage );
            break;
        }
        case EPbMsgVideoSurfaceChanged:
        {
            SurfaceChangedL( aMessage );
            break;
        }
        case EPbMsgVideoSurfaceRemoved:
        {
            SurfaceRemoved();
            break;
        }
#endif        
    }
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::SetAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    Q_UNUSED( aCmd );
    TInt aspectRatio = 0;

    iCurrentIndexForAspectRatio = 1;
    
    return aspectRatio;
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetDefaultAspectRatioL
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::SetDefaultAspectRatioL(
                                          QMPXVideoPlaybackViewFileDetails* aFileDetails,
                                          TReal aDisplayAspectRatio )
{
    Q_UNUSED( aFileDetails );
    Q_UNUSED( aDisplayAspectRatio );
    TInt newAspectRatio = EMMFNatural;

    return newAspectRatio;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SaveAspectRatioL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SaveAspectRatioL()
{

}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::LoadAspectRatioL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::LoadAspectRatioL()
{
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::UpdateVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::UpdateVideoRectL(  TRect aClipRect, TBool transitionEffect  )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::UpdateVideoRectL()"));
    Q_UNUSED( transitionEffect );
    
    iTlXDiff = (TReal32)( aClipRect.iTl.iX );
    iTlYDiff = (TReal32)( aClipRect.iTl.iY );
    iBrXDiff = (TReal32)( aClipRect.iBr.iX );
    iBrYDiff = (TReal32)( aClipRect.iBr.iY );    

}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::UpdateVideoRectTimeOutL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::UpdateVideoRectTimeOutL( TAny* aPtr )
{
    Q_UNUSED( aPtr );
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::CalculateVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::CalculateVideoRectL()
{

}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SetVideoRectL( TRect aRect )
{
    Q_UNUSED( aRect );
}

#ifdef SYMBIAN_BUILD_GCE

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                                         RWindowBase& aWindowBase,
                                                         RWindow* aWin )
{
    Q_UNUSED( aScreenDevice );
    Q_UNUSED( aWindowBase );
    Q_UNUSED( aWin );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SurfaceCreatedL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SurfaceCreatedL( CMPXMessage* aMessage )
{
    Q_UNUSED( aMessage );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SurfaceChangedL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SurfaceChangedL( CMPXMessage* aMessage )
{
    Q_UNUSED( aMessage );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SurfaceRemoved()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SurfaceRemoved()
{
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetNgaAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::SetNgaAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    Q_UNUSED( aCmd );
    return KErrNone;
}


#else

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetDisplayWindowL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SetDisplayWindowL( RWsSession& aWs,
                                                         CWsScreenDevice& aScreenDevice,
                                                         RWindowBase& aWin,
                                                         TRect aClipRect )
{
    Q_UNUSED( aWs );
    Q_UNUSED( aScreenDevice );
    Q_UNUSED( aWin );
    Q_UNUSED( aClipRect );
}

// -------------------------------------------------------------------------------------------------
//  CMPXVideoPlaybackDisplayHandler::Restart()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::Restart( RDirectScreenAccess::TTerminationReasons aReason )
{
    Q_UNUSED( aReason );
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::RestartDsaL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::RestartDsaL()
{

}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::AbortNow()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::AbortNow( RDirectScreenAccess::TTerminationReasons aReason )
{
    Q_UNUSED( aReason );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::CreateAbortDsaCmdL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::CreateAbortDsaCmdL()
{

}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::CreateAspectRatioCommandL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::CreateAspectRatioCommandL( TMPXVideoPlaybackCommand aCmd )
{
    Q_UNUSED( aCmd );
}

#endif

// End of File
