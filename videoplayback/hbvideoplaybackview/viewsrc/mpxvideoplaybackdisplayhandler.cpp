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

// Version : %version:  20 %

#include <sysutil.h>
#include <s32file.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackutility.h>
#include <mpxvideoplaybackdefs.h>
#include <alfcompositionutility.h>

#include "mpxvideocontainer.h"
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
    , iTransitionEffectCnt( 0 )
    , iViewWrapper( aViewWrapper )
    , iScaleWidth( 100.0f )
    , iScaleHeight( 100.0f )
    , iHorizontalPosition( EHorizontalAlignCenter )
    , iVerticalPosition( EVerticalAlignCenter )
    , iRotation( EVideoRotationNone )
    , iAutoScale( EAutoScaleBestFit )
{
}

CMPXVideoPlaybackDisplayHandler::~CMPXVideoPlaybackDisplayHandler()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::~CMPXVideoPlaybackDisplayHandler()"));

    MPX_TRAPD( error, SaveAspectRatioL() );

    if ( iResizingTimer )
    {
        iResizingTimer->Cancel();
        delete iResizingTimer;
        iResizingTimer = NULL;
    }

    iAspectRatioArray.Close();

    if ( iVideoDisplay )
    {
        SurfaceRemoved();
    	
        delete iVideoDisplay;
        iVideoDisplay = NULL;
    }

	if ( iVideoContainer )
    {
        delete iVideoContainer;
        iVideoContainer = NULL;
    }
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
    iResizingTimer = CPeriodic::NewL( CActive::EPriorityStandard );    
    LoadAspectRatioL();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL(
                                          RWsSession& /*aWs*/,
                                          CWsScreenDevice& aScreenDevice,
                                          RWindow& aWin,
                                          TRect aDisplayRect )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL()"));

    //
    // Workaround start for PREQ-2669
    //
    iVideoContainer = new ( ELeave ) CMPXVideoContainer();
    iVideoContainer->ConstructL();
    iVideoContainer->SetRect( aDisplayRect );

    RWindowBase *videoWindow = iVideoContainer->DrawableWindow();
    videoWindow->SetOrdinalPosition( -1 );
    (&aWin)->SetOrdinalPosition( 0 );

    MPX_DEBUG(_L("VideoWindow ordinal position is: %d"), videoWindow->OrdinalPosition());
    MPX_DEBUG(_L("UiWindow ordinal position is: %d"), (&aWin)->OrdinalPosition());

    TRAPD
    ( status,
        {
            CAlfCompositionSource* me = CAlfCompositionSource::NewL( aWin );
            me->EnableAlpha();
            delete me;
            me = NULL;
        }
    );

    MPX_DEBUG(
        _L("CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL() status : %d"), status);

    //
    // Workaround end
    //

    AddDisplayWindowL( aScreenDevice, *videoWindow, (RWindow*)videoWindow );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::RemoveDisplayWindow()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::RemoveDisplayWindow()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::RemoveDisplayWindow()"));

    if ( iVideoDisplay )
    {
        SurfaceRemoved();
        delete iVideoDisplay;
        iVideoDisplay = NULL;
    }
	
    if ( iVideoContainer )
    {
        delete iVideoContainer;
        iVideoContainer = NULL;
    }

    iSurfaceId = TSurfaceId::CreateNullId();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL( CMPXMessage* aMessage )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessage()"));

    TMPXVideoDisplayCommand message =
        ( *(aMessage->Value<TMPXVideoDisplayCommand>(KMPXMediaVideoDisplayCommand)) );

    MPX_DEBUG(
      _L("CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL() message = %d"), message );

    switch ( message )
    {
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
    }
}


// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::SetAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::SetAspectRatioL()"));

    TInt aspectRatio;

    aspectRatio = SetNgaAspectRatioL( aCmd );

    //
    //  Update the aspect ratio in the array
    //
    TInt count = iAspectRatioArray.Count();

    if ( count > 0 && count > iCurrentIndexForAspectRatio )
    {
        iAspectRatioArray[iCurrentIndexForAspectRatio].scalingType = (TMMFScalingType)aspectRatio;
    }

    return aspectRatio;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetDefaultAspectRatioL
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::SetDefaultAspectRatioL(
                                          QMPXVideoPlaybackViewFileDetails* aFileDetails,
                                          TReal32 aDisplayAspectRatio )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::SetDefaultAspectRatioL()"));

    TInt newAspectRatio = EMMFNatural;

    if ( aFileDetails->mVideoHeight > 0 && aFileDetails->mVideoWidth > 0 )
    {
        TMMFScalingType scalingType = EMMFNatural;

        TReal32 videoAspectRatio = (TReal32)aFileDetails->mVideoWidth /
                                   (TReal32)aFileDetails->mVideoHeight;

        TInt cnt = iAspectRatioArray.Count();
        TInt i = 0;

        //
        //  check whether dat file has the information about (videoRatio + screenRatio)
        //
        for ( ; i < cnt ; i++ )
        {
            if ( iAspectRatioArray[i].videoRatio == videoAspectRatio &&
                 iAspectRatioArray[i].screenRatio == aDisplayAspectRatio &&
                 ( scalingType = iAspectRatioArray[i].scalingType ) > 0 )
            {
                break;
            }
        }

        //
        //  if can't find out match aspect ratio in dat file,
        //  choose the scaling type through the rule
        //      videoAspectRatio - aDisplayAspectRatio > 0.1 ==> zoom
        //      videoAspectRatio - aDisplayAspectRatio < 0.1 ==> stretch
        //      videoAspectRatio = aDisplayAspectRatio ==> natural
        //
        if ( i == cnt )
        {
            if ( videoAspectRatio - aDisplayAspectRatio > 0.1 )
            {
                scalingType = EMMFZoom;
            }
            else if ( videoAspectRatio != aDisplayAspectRatio )
            {
                scalingType = EMMFStretch;
            }

            TMPXAspectRatio ratio;

            ratio.videoRatio = videoAspectRatio;
            ratio.screenRatio = aDisplayAspectRatio;
            ratio.scalingType = scalingType;

            iAspectRatioArray.Append( ratio );
        }

        iCurrentIndexForAspectRatio = i;

        TMPXVideoPlaybackCommand aspectRatioCmd = EPbCmdNaturalAspectRatio;

        if ( scalingType == EMMFZoom )
        {
            aspectRatioCmd = EPbCmdZoomAspectRatio;
        }
        else if ( scalingType == EMMFStretch )
        {
            aspectRatioCmd = EPbCmdStretchAspectRatio;
        }

        newAspectRatio = SetAspectRatioL( aspectRatioCmd );
    }

    return newAspectRatio;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SaveAspectRatioL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SaveAspectRatioL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::SaveAspectRatioL"));

    RFs fs;
    TInt err = fs.Connect();
    CleanupClosePushL<RFs>( fs );

    TBool canSave = EFalse;

    TRAP_IGNORE( canSave = ! SysUtil::FFSSpaceBelowCriticalLevelL(
                                 &fs, sizeof(TMPXAspectRatio) * iAspectRatioArray.Count()) );

    if ( canSave )
    {
        // save list to disk
        RFileWriteStream out;

        TInt err( out.Replace( fs, KAspectRatioFile, EFileWrite ) );

        if ( err == KErrPathNotFound )
        {
            fs.MkDirAll( KAspectRatioFile );
            err = out.Create( fs, KAspectRatioFile, EFileWrite );
        }

        if ( ! err )
        {
            TInt cnt = iAspectRatioArray.Count();

            MPX_TRAP( err,
            {
                for ( TInt i = 0 ; i < cnt ; i++ )
                {
                    //Save (videoRatio + screenRatio + scalingType)
                    out.WriteReal32L( iAspectRatioArray[i].videoRatio );
                    out.WriteReal32L( iAspectRatioArray[i].screenRatio );
                    out.WriteInt8L( iAspectRatioArray[i].scalingType );
                }
            } );

            out.Close();
        }
    }

    CleanupStack::PopAndDestroy();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::LoadAspectRatioL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::LoadAspectRatioL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::LoadAspectRatioL()"));

    RFs fs;
    RFileReadStream in;

    TInt err = fs.Connect();
    CleanupClosePushL<RFs>( fs );

    if ( ! err && in.Open( fs, KAspectRatioFile, EFileRead ) == KErrNone )
    {
        TMPXAspectRatio ratio;

        MPX_TRAP( err,
        {
            for ( err = KErrNone ; err == KErrNone ; )
            {
                //
                //  Read (videoRatio + screenRatio + scalingType)
                //
                ratio.videoRatio = in.ReadReal32L();
                ratio.screenRatio = in.ReadReal32L();
                ratio.scalingType = (TMMFScalingType)in.ReadInt8L();

                iAspectRatioArray.Append( ratio );
            }
        } );

        in.Close();
    }

    CleanupStack::PopAndDestroy();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::UpdateVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::UpdateVideoRectL( TRect aClipRect, TBool transitionEffect )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::UpdateVideoRectL()"));

    if ( transitionEffect )
    {
        iTlXDiff = (TReal32)( iWindowRect.iTl.iX - aClipRect.iTl.iX ) / KTRANSITIONEFFECTCNT;
        iTlYDiff = (TReal32)( iWindowRect.iTl.iY - aClipRect.iTl.iY ) / KTRANSITIONEFFECTCNT;
        iBrXDiff = (TReal32)( iWindowRect.iBr.iX - aClipRect.iBr.iX ) / KTRANSITIONEFFECTCNT;
        iBrYDiff = (TReal32)( iWindowRect.iBr.iY - aClipRect.iBr.iY ) / KTRANSITIONEFFECTCNT;

        if ( iResizingTimer->IsActive() )
        {
            iResizingTimer->Cancel();
        }

        iResizingTimer->Start( 
                0,
                KVIDEORESIZINGREPEATRATE,
                TCallBack( CMPXVideoPlaybackDisplayHandler::UpdateVideoRectTimeOutL, this ) );                
    }
    else
    {
        SetVideoRectL( aClipRect );

        iWindowRect = aClipRect;

        iViewWrapper->UpdateVideoRectDone();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::UpdateVideoRectTimeOutL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::UpdateVideoRectTimeOutL( TAny* aPtr )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::UpdateVideoRectTimeOutL()"));

    static_cast<CMPXVideoPlaybackDisplayHandler*>(aPtr)->CalculateVideoRectL();

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::CalculateVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::CalculateVideoRectL()
{
    iTransitionEffectCnt++;

    TRect windowRect( (TInt)( (TReal32)iWindowRect.iTl.iX - iTlXDiff * (TReal32)iTransitionEffectCnt ), 
                      (TInt)( (TReal32)iWindowRect.iTl.iY - iTlYDiff * (TReal32)iTransitionEffectCnt ), 
                      (TInt)( (TReal32)iWindowRect.iBr.iX - iBrXDiff * (TReal32)iTransitionEffectCnt ),
                      (TInt)( (TReal32)iWindowRect.iBr.iY - iBrYDiff * (TReal32)iTransitionEffectCnt ) );

    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::CalculateVideoRectL() %d %d %d %d"),
            windowRect.iTl.iX, windowRect.iTl.iY, windowRect.iBr.iX, windowRect.iBr.iY );

    SetVideoRectL( windowRect );

    if ( iTransitionEffectCnt >= KTRANSITIONEFFECTCNT )
    {
        iTransitionEffectCnt = 0;
        iWindowRect = windowRect;

        if ( iResizingTimer->IsActive() )
        {
            iResizingTimer->Cancel();
        }

        MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::CalculateVideoRectL() Done"));

        iViewWrapper->UpdateVideoRectDone();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SetVideoRectL( TRect aRect )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::SetVideoRectL()"));

    if ( iVideoDisplay )
    {
        iVideoDisplay->SetVideoExtentL( *iWindowBase, aRect, TRect( iWindowBase->Size() ) );        
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                                         RWindowBase& aWindowBase,
                                                         RWindow* aWin )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL()"));

    iWindowBase = &aWindowBase;

    TInt displayId = aScreenDevice.GetScreenNumber();

    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL() displayId %d"), displayId);

    CMediaClientVideoDisplay* tempDisplay = iVideoDisplay;

    iVideoDisplay = CMediaClientVideoDisplay::NewL( displayId );

    delete tempDisplay;

    TRect cropRect = TRect( aWin->Size() );

    //
    // If RWindow is still in potrait, rotate surface to play a video in landscape
    //
    if ( cropRect.Width() < cropRect.Height() )
    {
        iRotation = EVideoRotationClockwise90;
    }
    
    iWindowRect = cropRect;
    
    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL() cropRect (%d, %d), (%d, %d)"),
        cropRect.iTl.iX, cropRect.iTl.iY, cropRect.iBr.iX, cropRect.iBr.iY);

    MPX_TRAPD( dispError,
    iVideoDisplay->AddDisplayWindowL( iWindowBase,
                                      cropRect,
                                      cropRect,
                                      cropRect,
                                      iScaleWidth,
                                      iScaleHeight,
                                      iRotation,
                                      iAutoScale,
                                      iHorizontalPosition,
                                      iVerticalPosition,
                                      aWin );
    );

    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL() Display Added"));
    //
    //  Check if surface was created before window was ready
    //
    if ( iSurfaceCached )
    {
        iVideoDisplay->SurfaceCreated( iSurfaceId, iCropRect, iAspectRatio, iCropRect );

        iSurfaceCached = EFalse;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SurfaceCreatedL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SurfaceCreatedL( CMPXMessage* aMessage )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::SurfaceCreatedL()"));

    TSurfaceId oldSurfaceId = iSurfaceId;
    
    //
    //  Extract the surface parameters from the message
    //
    iSurfaceId = aMessage->ValueTObjectL<TSurfaceId>( KMPXMediaVideoDisplayTSurfaceId );
    iCropRect = aMessage->ValueTObjectL<TRect>( KMPXMediaVideoDisplayCropRect );
    iAspectRatio = aMessage->ValueTObjectL<TVideoAspectRatio>( KMPXMediaVideoDisplayAspectRatio );

    if ( iVideoDisplay )
    {
        //
        //  Remove old surface if one exists
        //
        if ( ! oldSurfaceId.IsNull() )
        {
            iVideoDisplay->RemoveSurface();
        }

        //
        //  Add new surface
        //
        iVideoDisplay->SurfaceCreated( iSurfaceId, iCropRect, iAspectRatio, iCropRect );
    }
    else
    {
        //
        //  Video display has not been created yet, save surface information to create
        //  the surface when the display is created
        //
        iSurfaceCached = ETrue;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SurfaceChangedL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SurfaceChangedL( CMPXMessage* aMessage )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::SurfaceChangedL()"));

    //
    //  Extract the surface parameters from the message
    //
    iSurfaceId = aMessage->ValueTObjectL<TSurfaceId>( KMPXMediaVideoDisplayTSurfaceId );
    iCropRect = aMessage->ValueTObjectL<TRect>( KMPXMediaVideoDisplayCropRect );
    iAspectRatio = aMessage->ValueTObjectL<TVideoAspectRatio>( KMPXMediaVideoDisplayAspectRatio );

    if ( iVideoDisplay )
    {
        //
        //  Add new surface
        //
        iVideoDisplay->SurfaceParametersChanged( iSurfaceId, iCropRect, iAspectRatio );

        iVideoDisplay->RedrawWindows( iCropRect );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SurfaceRemoved()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SurfaceRemoved()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::SurfaceRemoved()"));

    if ( iVideoDisplay )
    {
        iVideoDisplay->RemoveSurface();
    }

    iSurfaceId = TSurfaceId::CreateNullId();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetNgaAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackDisplayHandler::SetNgaAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::SetNgaAspectRatioL()"));

    TInt aspectRatio = EMMFNatural;

    switch ( aCmd )
    {
        case EPbCmdNaturalAspectRatio:
        {
            iAutoScale = EAutoScaleBestFit;
            aspectRatio = EMMFNatural;
            break;
        }
        case EPbCmdZoomAspectRatio:
        {
            iAutoScale = EAutoScaleClip;
            aspectRatio = EMMFZoom;
            break;
        }
        case EPbCmdStretchAspectRatio:
        {
            iAutoScale = EAutoScaleStretch;
            aspectRatio = EMMFStretch;
            break;
        }
    }

    if ( iVideoDisplay && ! iSurfaceId.IsNull() )
    {
        iVideoDisplay->SetAutoScaleL( iAutoScale,
                                      iHorizontalPosition,
                                      iVerticalPosition,
                                      iCropRect );
    }

    return aspectRatio;
}

// End of File
