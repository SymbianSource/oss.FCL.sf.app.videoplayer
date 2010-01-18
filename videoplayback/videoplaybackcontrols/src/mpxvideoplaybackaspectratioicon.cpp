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
* Description: Aspect ratio control
*
*/

// Version : %version: 11 %


// INCLUDE FILES
#include <eikenv.h>
#include <gulicon.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <AknsDrawUtils.h>
#include <mpxvideoplaybackcontrols.mbg>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK

#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackaspectratioicon.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideo_debug.h"

// ============================ MEMBER FUNCTIONS ===================================================

CMPXVideoPlaybackAspectRatioIcon::CMPXVideoPlaybackAspectRatioIcon(
    CMPXVideoPlaybackControlsController* aController )
    : iController( aController )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackAspectRatioIcon::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackAspectRatioIcon::ConstructL( TRect aRect )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackAspectRatioIcon::ConstructL()"));

    SetRect( TRect( 0, 0, aRect.Width(), aRect.Height() ) );

    iAspectRatioRect = TRect( Rect().iTl.iX + 15,
                              Rect().iTl.iY + 15,
                              Rect().iBr.iX - 15,
                              Rect().iBr.iY - 15 );

    SkinChangeL();

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif //RD_TACTILE_FEEDBACK
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackAspectRatioIcon::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackAspectRatioIcon*
CMPXVideoPlaybackAspectRatioIcon::NewL( CMPXVideoPlaybackControlsController* aController,
                                        TRect aRect )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackAspectRatioIcon::NewL()"));

    CMPXVideoPlaybackAspectRatioIcon* self =
        new ( ELeave ) CMPXVideoPlaybackAspectRatioIcon( aController );

    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackAspectRatioIcon::~CMPXVideoPlaybackAspectRatioIcon()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackAspectRatioIcon::~CMPXVideoPlaybackAspectRatioIcon()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackAspectRatioIcon::~CMPXVideoPlaybackAspectRatioIcon()"));

    if ( iNaturalIcon )
    {
        delete iNaturalIcon;
        iNaturalIcon = NULL;
    }

    if ( iStretchIcon )
    {
        delete iStretchIcon;
        iStretchIcon = NULL;
    }

    if ( iZoomIcon )
    {
        delete iZoomIcon;
        iZoomIcon = NULL;
    }

#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->RemoveFeedbackForControl(this);
    }
#endif //RD_TACTILE_FEEDBACK
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackAspectRatioIcon::SkinChangeL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackAspectRatioIcon::SkinChangeL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackAspectRatioIcon::SkinChangeL()"));

    //
    // Create icons
    //
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TFileName iconsPath;
    iController->LocateBitmapFileL( iconsPath );

    if ( iNaturalIcon )
    {
        delete iNaturalIcon;
        iNaturalIcon = NULL;
    }

    iNaturalIcon = AknsUtils::CreateGulIconL(
        skin,
        KAknsIIDQgnIndiCamsNight,
        iconsPath,
        EMbmMpxvideoplaybackcontrolsQgn_indi_mp_ratio_orig,
        EMbmMpxvideoplaybackcontrolsQgn_indi_mp_ratio_orig_mask );

    if ( iNaturalIcon )
    {
        AknIconUtils::SetSize( iNaturalIcon->Bitmap(),
                               iAspectRatioRect.Size(),
                               EAspectRatioNotPreserved );
    }

    if ( iStretchIcon )
    {
        delete iStretchIcon;
        iStretchIcon = NULL;
    }

    iStretchIcon = AknsUtils::CreateGulIconL(
        skin,
        KAknsIIDQgnIndiCamsBright,
        iconsPath,
        EMbmMpxvideoplaybackcontrolsQgn_indi_mp_ratio_stretch,
        EMbmMpxvideoplaybackcontrolsQgn_indi_mp_ratio_stretch_mask );

    if ( iStretchIcon )
    {
        AknIconUtils::SetSize( iStretchIcon->Bitmap(),
                               iAspectRatioRect.Size(),
                               EAspectRatioNotPreserved );
    }

    if ( iZoomIcon )
    {
        delete iZoomIcon;
        iZoomIcon = NULL;
    }

    iZoomIcon = AknsUtils::CreateGulIconL(
        skin,
        KAknsIIDQgnIndiCamsCloudy,
        iconsPath,
        EMbmMpxvideoplaybackcontrolsQgn_indi_mp_ratio_zoom,
        EMbmMpxvideoplaybackcontrolsQgn_indi_mp_ratio_zoom_mask );

    if ( iZoomIcon )
    {
        AknIconUtils::SetSize( iZoomIcon->Bitmap(),
                               iAspectRatioRect.Size(),
                               EAspectRatioNotPreserved );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackAspectRatioIcon::CountComponentControls()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackAspectRatioIcon::CountComponentControls() const
{
    return 0;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackAspectRatioIcon::Draw()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackAspectRatioIcon::Draw( const TRect& aRect ) const
{
    MPX_DEBUG(_L("CMPXVideoPlaybackAspectRatioIcon::Draw()"));

    CWindowGc& gc = SystemGc();
    gc.SetClippingRect( aRect );

    if ( Window().DisplayMode() == EColor16MAP )
    {
        gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        gc.SetBrushColor( TRgb::Color16MAP( 255 ) );
        gc.Clear( aRect );
    }
    else if ( Window().DisplayMode() == EColor16MA )
    {
        gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        gc.SetBrushColor( TRgb::Color16MA( 0 ) );
        gc.Clear( aRect );
    }
    else
    {
        // draw a solid background so that the entire progress
        // bar is shown not just the area representing the
        // portion that has been played.
        gc.SetBrushColor( KRgbBlack );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.DrawRect( aRect );
        gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    }

    switch( iAspectRatio )
    {
        case EMMFStretch:
        {
            gc.BitBltMasked( iAspectRatioRect.iTl,
                             iZoomIcon->Bitmap(),
                             TRect( iAspectRatioRect.Size() ),
                             iZoomIcon->Mask(),
                             ETrue );
            break;
        }
        case EMMFZoom:
        {
            gc.BitBltMasked( iAspectRatioRect.iTl,
                             iNaturalIcon->Bitmap(),
                             TRect( iAspectRatioRect.Size() ),
                             iNaturalIcon->Mask(),
                             ETrue );
            break;
        }
        default:
        {
            gc.BitBltMasked( iAspectRatioRect.iTl,
                             iStretchIcon->Bitmap(),
                             TRect( iAspectRatioRect.Size() ),
                             iStretchIcon->Mask(),
                             ETrue );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackAspectRatioIcon::HandlePointerEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackAspectRatioIcon::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackAspectRatioIcon::HandlePointerEventL()"));

    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
    {
#ifdef RD_TACTILE_FEEDBACK
        if ( iFeedback )
        {
#ifdef SYMBIAN_BUILD_GCE
            iFeedback->InstantFeedback( ETouchFeedbackBasicButton );
#else
            iFeedback->InstantFeedback( ETouchFeedbackBasic );
#endif //SYMBIAN_BUILD_GCE
        }
#endif //RD_TACTILE_FEEDBACK
    }
    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
    {
        TMPXVideoPlaybackViewCommandIds cmd = EMPXPbvCmdNaturalAspectRatio;

        switch ( iAspectRatio )
        {
            case EMMFStretch:
            {
                cmd = EMPXPbvCmdZoomAspectRatio;
                break;
            }
            case EMMFNatural:
            {
                cmd = EMPXPbvCmdStretchAspectRatio;
                break;
            }
        }

        iController->HandleCommandL( cmd );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackAspectRatioIcon::AspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackAspectRatioIcon::AspectRatioChanged( TInt aAspectRatio )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackAspectRatioIcon::AspectRatioChanged() (%d)"), aAspectRatio);

    iAspectRatio = (TMMFScalingType)aAspectRatio;

    if ( IsVisible() )
    {
        DrawNow();
    }
}

//  End of File
