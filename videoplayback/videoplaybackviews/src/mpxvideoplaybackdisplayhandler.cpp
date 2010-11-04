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
* Description:   Implementation of video playback display handler
*
*/


// Version : %version: 18 %


#include <sysutil.h>
#include <s32file.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackutility.h>
#include <mpxvideoplaybackdefs.h>

#include "mpxvideoplaybackdisplayhandler.h"
#include "mpxvideoregion.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontainer.h"
#include "mpxcommonvideoplaybackview.hrh"


_LIT( KAspectRatioFile, "c:\\private\\200159b2\\mpxvideoplayer_aspect_ratio.dat" );


CMPXVideoPlaybackDisplayHandler::CMPXVideoPlaybackDisplayHandler(
                                     MMPXPlaybackUtility* aPlayUtil,
                                     CMPXVideoPlaybackContainer* aContainer)
    : iPlaybackUtility( aPlayUtil )
    , iContainer( aContainer )
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

    iAspectRatioArray.Close();

    if ( iVideoDisplay )
    {
        delete iVideoDisplay;
        iVideoDisplay = NULL;
    }

    iSurfaceId = TSurfaceId::CreateNullId();
}

CMPXVideoPlaybackDisplayHandler*
CMPXVideoPlaybackDisplayHandler::NewL( MMPXPlaybackUtility* aPlayUtil,
                                       CMPXVideoPlaybackContainer* aContainer )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::NewL()"));

    CMPXVideoPlaybackDisplayHandler* self =
        new(ELeave) CMPXVideoPlaybackDisplayHandler( aPlayUtil, aContainer );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::ConstructL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::ConstructL()
{
    LoadAspectRatioL();

    TRect displayRect = iContainer->Rect();
    iDisplayAspectRatio = (TReal32)displayRect.Width() / (TReal32)displayRect.Height();

    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::ConstructL() iDisplayAspectRatio = %f"),
        iDisplayAspectRatio );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL(
                                           CWsScreenDevice& aScreenDevice,
                                           RWindow& aWin,
                                           CMPXVideoPlaybackViewFileDetails* aFileDetails )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL()"));

    //
    //  Save off the video aspect ratio
    //
    iVideoHeight = aFileDetails->iVideoHeight;
    iVideoWidth  = aFileDetails->iVideoWidth;

    MPX_DEBUG(
        _L("CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL() clip size (%d,%d)"),
                iVideoHeight, iVideoWidth );

    AddDisplayWindowL( aScreenDevice, aWin, &aWin );
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
        //
        //  Surface is being removed, signal the container so it can reset the background
        //
        if ( iContainer )
        {
            MPX_TRAPD( err, iContainer->HandleCommandL( EMPXPbvSurfaceRemoved ) );
        }

        delete iVideoDisplay;
        iVideoDisplay = NULL;
    }

    iSurfaceId = TSurfaceId::CreateNullId();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL()
// -------------------------------------------------------------------------------------------------
//
void
CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL( CMPXMessage* aMessage )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL()"));

    TMPXVideoDisplayCommand message =
        ( *(aMessage->Value<TMPXVideoDisplayCommand>(KMPXMediaVideoDisplayCommand)) );

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
//   CMPXVideoPlaybackDisplayHandler::CalculateAspectRatioL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::CalculateAspectRatioL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::CalculateAspectRatioL()"));

    if ( iVideoHeight > 0 && iVideoWidth > 0 )
    {
        TMMFScalingType scalingType = EMMFNatural;

        TReal videoAspectRatio = CalculateVideoAspectRatio();

        MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::CalculateAspectRatioL() displayAR = %f videoAR = %f)"),
            iDisplayAspectRatio, videoAspectRatio );

        TInt cnt = iAspectRatioArray.Count();
        TInt i = 0;

        //
        //  check whether dat file has the information about (videoRatio + screenRatio)
        //
        for ( ; i < cnt ; i++ )
        {
            if ( IsAspectRatioEqual( iAspectRatioArray[i].videoRatio, videoAspectRatio ) &&
                 IsAspectRatioEqual( iAspectRatioArray[i].screenRatio, iDisplayAspectRatio ) &&
                 ( scalingType = iAspectRatioArray[i].scalingType ) > 0 )
            {
                MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::CalculateAspectRatioL() ratio found)"));
                break;
            }
        }

        //
        //  if can't find out match aspect ratio in dat file,
        //  choose the scaling type through the rule
        //      aspectRatioDiff =  videoAspectRatio - aDisplayAspectRatio
        //      aspectRatioDiff ==  0        ==> natural
        //      aspectRatioDiff > 0.1        ==> zoom
        //      aspectRatioDiff < - 0.3      ==> natural
        //      aspectRatioDiff >= - 0.3 and <= 0.1   ==> stretch
        //
        if ( i == cnt )
        {
            if ( videoAspectRatio - iDisplayAspectRatio > 0.1 )
            {
                scalingType = EMMFZoom;
            }
            else if ( ( ! IsAspectRatioEqual( videoAspectRatio, iDisplayAspectRatio ) ) &&
                      ( videoAspectRatio - iDisplayAspectRatio > (- 0.3) ) )
            {
                scalingType = EMMFStretch;
            }

            TMPXAspectRatio ratio;

            ratio.videoRatio = videoAspectRatio;
            ratio.screenRatio = iDisplayAspectRatio;
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

        SetAspectRatioL( aspectRatioCmd );

        //
        //  If the screen aspect ratio and the video aspect ratio are not equal,
        //  signal the controls controller to create the aspect ratio icon
        //
        if ( ShowAspectRatioIcon() )
        {
            iContainer->HandleEventL( EMPXControlCmdCreateAspectRatioIcon );
        }
        else
        {
            iContainer->HandleEventL( EMPXControlCmdDeleteAspectRatioIcon );
        }
    }
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
                    //
                    //  Save (videoRatio + screenRatio + scalingType)
                    //
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
//   CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                                         RWindowBase& aWindowBase,
                                                         RWindow* aWin )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL()"));

    TInt displayId = aScreenDevice.GetScreenNumber();

    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL() displayId %d"), displayId);

    CMediaClientVideoDisplay* tempDisplay = iVideoDisplay;

    iVideoDisplay = CMediaClientVideoDisplay::NewL( displayId );

    delete tempDisplay;

    TRect cropRect = TRect( aWin->Size() );

    MPX_DEBUG(
        _L("CMPXVideoPlaybackDisplayHandler::AddDisplayWindowL() cropRect (%d, %d), (%d, %d)"),
        cropRect.iTl.iX, cropRect.iTl.iY, cropRect.iBr.iX, cropRect.iBr.iY);

    MPX_TRAPD( dispError,
        iVideoDisplay->AddDisplayWindowL( &aWindowBase,
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
    if ( iSurfaceCached && ! iContainer->IsRealOneBitmapTimerActive() )
    {
        AttachNewSurfaceToWindowL();
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

    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::SurfaceCreatedL iCropRect[(%d,%d),(%d,%d)] iAspectRatio = (%d,%d)"),
        iCropRect.iTl.iX, iCropRect.iTl.iY, iCropRect.iBr.iX, iCropRect.iBr.iY,
        iAspectRatio.iNumerator, iAspectRatio.iDenominator );

    //
    //  Add the surface unless the video display hasn't been created or
    //  the Real One bitmap is being shown.
    //
    if ( iVideoDisplay && ! iContainer->IsRealOneBitmapTimerActive() )
    {
        //
        //  Remove old surface if one exists
        //
        if ( ! oldSurfaceId.IsNull() )
        {
            iVideoDisplay->RemoveSurface();
        }

        AttachNewSurfaceToWindowL();
    }
    else
    {
        //
        //  Save the surface information to add it when the display is ready
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

    TVideoAspectRatio oldAspectRatio = iAspectRatio;

    //
    //  Extract the surface parameters from the message
    //
    iSurfaceId = aMessage->ValueTObjectL<TSurfaceId>( KMPXMediaVideoDisplayTSurfaceId );
    iCropRect = aMessage->ValueTObjectL<TRect>( KMPXMediaVideoDisplayCropRect );
    iAspectRatio = aMessage->ValueTObjectL<TVideoAspectRatio>( KMPXMediaVideoDisplayAspectRatio );

    //
    //  Check if the surface has been cached
    //  If surface has been cached, exit since the surface parameters have been saved
    //
    if ( ! iSurfaceCached && iVideoDisplay )
    {
        //
        //  Add new surface
        //
        iVideoDisplay->SurfaceParametersChanged( iSurfaceId, iCropRect, iAspectRatio );
        iVideoDisplay->RedrawWindows( iCropRect );

        //
        //  Update the Aspect Ratio if it changed on the surface
        //
        if ( oldAspectRatio != iAspectRatio )
        {
            CalculateAspectRatioL();
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::RemoveSurfaceFromPlaybackPluginL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::RemoveSurfaceFromPlaybackPluginL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::RemoveSurfaceFromPlaybackPluginL()"));

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );
    cmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand,
                                                     EPbCmdSurfaceRemovedFromWindow );
    cmd->SetTObjectValueL<TSurfaceId>( KMPXMediaVideoDisplayTSurfaceId, iSurfaceId );

    iPlaybackUtility->CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
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
        if ( iContainer )
        {
            MPX_TRAPD( err, iContainer->HandleCommandL( EMPXPbvSurfaceRemoved ) );
        }

        iVideoDisplay->RemoveSurface();

        //
        // Signal the playback plugin to remove the surface from Helix
        //
        MPX_TRAPD( err2, RemoveSurfaceFromPlaybackPluginL() );
    }

    iSurfaceId = TSurfaceId::CreateNullId();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::SetAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::SetAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::SetAspectRatioL()"),
                   _L("aCmd = %d"), aCmd);

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

    //
    //  Update the controls controller with the new aspect ratio
    //
    if ( iContainer )
    {
        iContainer->HandleEventL( EMPXControlCmdSetAspectRatio, aspectRatio );
    }

    //
    //  Update the aspect ratio in the array
    //
    TInt count = iAspectRatioArray.Count();

    if ( count > 0 && count > iCurrentIndexForAspectRatio )
    {
        iAspectRatioArray[iCurrentIndexForAspectRatio].scalingType = (TMMFScalingType)aspectRatio;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::DoHandleRealOneBitmapTimeoutL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::DoHandleRealOneBitmapTimeoutL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::DoHandleRealOneBitmapTimeoutL()"));

    //
    //  Check if surface was created before window was ready
    //
    if ( iSurfaceCached && iVideoDisplay )
    {
        AttachNewSurfaceToWindowL();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::AttachNewSurfaceToWindowL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::AttachNewSurfaceToWindowL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::AttachNewSurfaceToWindowL()"));

    //
    //  Calculate the video aspect ratio and set the proper value on the video display
    //  before attaching the surface
    //
    CalculateAspectRatioL();

    iVideoDisplay->SurfaceCreated( iSurfaceId, iCropRect, iAspectRatio, iCropRect );

    if ( iContainer )
    {
        iContainer->HandleCommandL( EMPXPbvSurfaceCreated );
    }

    iSurfaceCached = EFalse;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::ShowAspectRatioIcon()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackDisplayHandler::ShowAspectRatioIcon()
{
    TBool showAspectRatio = ETrue;

    TReal videoAspectRatio = CalculateVideoAspectRatio();

    if ( IsAspectRatioEqual( videoAspectRatio, iDisplayAspectRatio ) )
    {
        showAspectRatio = EFalse;
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::ShowAspectRatioIcon(%d)"), showAspectRatio);

    return showAspectRatio;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::IsAspectRatioEqual()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackDisplayHandler::IsAspectRatioEqual( TReal aRatio1, TReal aRatio2 )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::IsAspectRatioEqual() ar1 = %f ar2 = %f)"),
            aRatio1, aRatio2 );

    TBool valuesEqual = EFalse;
    TReal arDiff = aRatio1 - aRatio2;

    if ( arDiff < 0.01 && arDiff > -0.01 )
    {
        valuesEqual = ETrue;
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::IsAspectRatioEqual(%d)"), valuesEqual);

    return valuesEqual;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::CalculateVideoAspectRatio()
// -------------------------------------------------------------------------------------------------
//
TReal CMPXVideoPlaybackDisplayHandler::CalculateVideoAspectRatio()
{
    TReal videoAspectRatio = 0.0;

    if ( iVideoHeight > 0 && iVideoWidth > 0 )
    {
        videoAspectRatio = (TReal32)iVideoWidth / (TReal32)iVideoHeight;

        //
        //  If the pixel aspect ratio is valid, use it to modify the videoAspectRatio
        //
        if ( iAspectRatio.iDenominator )
        {
            MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::CalculateVideoAspectRatio() iAspectRatio = (%d,%d)"),
                iAspectRatio.iNumerator, iAspectRatio.iDenominator );

            TReal32 par = (TReal32)iAspectRatio.iNumerator / (TReal32)iAspectRatio.iDenominator;

            videoAspectRatio *= par;
        }
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::CalculateVideoAspectRatio(%f)"), videoAspectRatio );

    return videoAspectRatio;
}

// End of File
