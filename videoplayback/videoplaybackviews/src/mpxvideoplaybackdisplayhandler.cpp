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


// Version : %version: 15 %


#include <sysutil.h>
#include <s32file.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxplaybackutility.h>

#include "mpxvideoplaybackdisplayhandler.h"
#include "mpxvideoregion.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontainer.h"
#include "mpxcommonvideoplaybackview.hrh"
#include <mpxvideoplaybackdefs.h>


_LIT( KAspectRatioFile, "c:\\private\\200159b2\\mpxvideoplayer_aspect_ratio.dat" );


CMPXVideoPlaybackDisplayHandler::CMPXVideoPlaybackDisplayHandler(
                                     MMPXPlaybackUtility* aPlayUtil,
                                     CMPXVideoPlaybackContainer* aContainer)
    : iPlaybackUtility( aPlayUtil )
    , iContainer( aContainer )
#ifdef SYMBIAN_BUILD_GCE
    , iScaleWidth( 100.0f )
    , iScaleHeight( 100.0f )
    , iHorizontalPosition( EHorizontalAlignCenter )
    , iVerticalPosition( EVerticalAlignCenter )
    , iRotation( EVideoRotationNone )
    , iAutoScale( EAutoScaleBestFit )
#endif
{
}

CMPXVideoPlaybackDisplayHandler::~CMPXVideoPlaybackDisplayHandler()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::~CMPXVideoPlaybackDisplayHandler()"));

    MPX_TRAPD( error, SaveAspectRatioL() );

    iAspectRatioArray.Close();

#ifdef SYMBIAN_BUILD_GCE
    if ( iVideoDisplay )
    {
        delete iVideoDisplay;
        iVideoDisplay = NULL;
    }
#endif

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
void CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL( CWsScreenDevice& aScreenDevice,
                                                            RWindow& aWin )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::CreateDisplayWindowL()"));

#ifdef SYMBIAN_BUILD_GCE
    AddDisplayWindowL( aScreenDevice, aWin, &aWin );
#endif
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackDisplayHandler::RemoveDisplayWindow()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackDisplayHandler::RemoveDisplayWindow()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::RemoveDisplayWindow()"));

#ifdef SYMBIAN_BUILD_GCE
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
#endif

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
#ifdef SYMBIAN_BUILD_GCE
        case EPbMsgVideoSurfaceCreated:
        {
            MPX_DEBUG(_L(" message = EPbMsgVideoSurfaceCreated"));
            SurfaceCreatedL( aMessage );
            break;
        }
        case EPbMsgVideoSurfaceChanged:
        {
            MPX_DEBUG(_L(" message = EPbMsgVideoSurfaceChanged"));
            SurfaceChangedL( aMessage );
            break;
        }
        case EPbMsgVideoSurfaceRemoved:
        {
            MPX_DEBUG(_L(" message = EPbMsgVideoSurfaceRemoved"));
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
    MPX_DEBUG(_L("CMPXVideoPlaybackDisplayHandler::SetAspectRatioL()"));

    TInt aspectRatio;

#ifdef SYMBIAN_BUILD_GCE
    aspectRatio = SetNgaAspectRatioL( aCmd );
#endif

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
                                          CMPXVideoPlaybackViewFileDetails* aFileDetails )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackDisplayHandler::SetDefaultAspectRatioL()"));

    TRect displayRect = iContainer->Rect();
    TReal displayAspectRatio = (TReal32)displayRect.Width() / (TReal32)displayRect.Height();

    TInt newAspectRatio = EMMFNatural;

    if ( aFileDetails->iVideoHeight > 0 && aFileDetails->iVideoWidth > 0 )
    {
        TMMFScalingType scalingType = EMMFNatural;

        TReal32 videoAspectRatio = (TReal32)aFileDetails->iVideoWidth /
                                   (TReal32)aFileDetails->iVideoHeight;

        TInt cnt = iAspectRatioArray.Count();
        TInt i = 0;

        //
        //  check whether dat file has the information about (videoRatio + screenRatio)
        //
        for ( ; i < cnt ; i++ )
        {
            if ( iAspectRatioArray[i].videoRatio == videoAspectRatio &&
                 iAspectRatioArray[i].screenRatio == displayAspectRatio &&
                 ( scalingType = iAspectRatioArray[i].scalingType ) > 0 )
            {
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
            if ( videoAspectRatio - displayAspectRatio > 0.1 )
            {
                scalingType = EMMFZoom;
            }
            else if ( ( videoAspectRatio != displayAspectRatio ) &&
                      ( videoAspectRatio - displayAspectRatio > (- 0.3) ) )
            {
                scalingType = EMMFStretch;
            }

            TMPXAspectRatio ratio;

            ratio.videoRatio = videoAspectRatio;
            ratio.screenRatio = displayAspectRatio;
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

#ifdef SYMBIAN_BUILD_GCE

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
        iVideoDisplay->SurfaceCreated( iSurfaceId, iCropRect, iAspectRatio, iCropRect );

        if ( iContainer )
        {
            iContainer->HandleCommandL( EMPXPbvSurfaceCreated );
        }

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

        //
        //  Add new surface
        //
        iVideoDisplay->SurfaceCreated( iSurfaceId, iCropRect, iAspectRatio, iCropRect );

        if ( iContainer )
        {
            iContainer->HandleCommandL( EMPXPbvSurfaceCreated );
        }
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
        if ( iContainer )
        {
            MPX_TRAPD( err, iContainer->HandleCommandL( EMPXPbvSurfaceRemoved ) );
        }

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

#endif

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
        iVideoDisplay->SurfaceCreated( iSurfaceId, iCropRect, iAspectRatio, iCropRect );

        if ( iContainer )
        {
            iContainer->HandleCommandL( EMPXPbvSurfaceCreated );
        }

        iSurfaceCached = EFalse;
    }
}


// End of File
