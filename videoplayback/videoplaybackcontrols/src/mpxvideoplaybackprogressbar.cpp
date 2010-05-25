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
* Description: Progress bar control
*
*/


// Version : %version: 23 %


// INCLUDE FILES
#include <eikenv.h>
#include <gulicon.h>
#include <AknUtils.h>
#include <eiklabel.h>
#include <StringLoader.h>
#include <AknsDrawUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <data_caging_path_literals.hrh>

#include <mpxvideoplaybackcontrols.rsg>
#include <mpxvideoplaybackcontrols.mbg>

#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideo_debug.h"

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK

using namespace AknLayoutScalable_Apps;

// CONSTANTS
const TInt KMPXProgressSliderWidth = 20;
const TInt KMPXSliderHeightOverProgresBar = 5;
const TInt64 KMPXMicroSeconds = 1000000;
const TInt KMPXOneHourInSeconds = 3600;

const TInt KMPXProgressBarHeight = 41;

// ============================ MEMBER FUNCTIONS ===================================================

CMPXVideoPlaybackProgressBar::CMPXVideoPlaybackProgressBar(
    CMPXVideoPlaybackControlsController* aController )
    : iController( aController )
    , iPlaybackRatio( 0.0 )
    , iDownloadRatio( 0.0 )
    , iDragging( EFalse )
    , iPointerEventStarted( EFalse )
    , iWasPlaying ( EFalse )
    , iOldDLSize(0)
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::ConstructL( TRect aRect )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::ConstructL()"));

    SetRect( TRect(0, 0, aRect.Width(), aRect.Height()) );

    SetLayoutL();
    SkinChangeL();

    //
    //  Read time format strings from AVKON resource
    //
    iMinSecFormatString = iEikonEnv->AllocReadResourceL(R_QTN_TIME_DURAT_MIN_SEC);
    iHourMinSecFormatString = iEikonEnv->AllocReadResourceL(R_QTN_TIME_DURAT_LONG);

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif //RD_TACTILE_FEEDBACK
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackProgressBar* CMPXVideoPlaybackProgressBar::NewL(
    CMPXVideoPlaybackControlsController* aController,
    TRect aRect )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::NewL()"));

    CMPXVideoPlaybackProgressBar* self =
        new ( ELeave ) CMPXVideoPlaybackProgressBar( aController );

    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::~CMPXVideoPlaybackProgressBar()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackProgressBar::~CMPXVideoPlaybackProgressBar()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::~CMPXVideoPlaybackProgressBar()"));

    if ( iMinSecFormatString )
    {
        delete iMinSecFormatString;
        iMinSecFormatString = NULL;
    }

    if ( iHourMinSecFormatString )
    {
        delete iHourMinSecFormatString;
        iHourMinSecFormatString = NULL;
    }

    if ( iLiveStreamingString )
    {
        delete iLiveStreamingString;
        iLiveStreamingString = NULL;
    }

    if ( iFrameIcon )
    {
       delete iFrameIcon;
       iFrameIcon = NULL;
    }

    if ( iPlaybackIcon )
    {
       delete iPlaybackIcon;
       iPlaybackIcon = NULL;
    }

    if ( iDownloadIcon )
    {
       delete iDownloadIcon;
       iDownloadIcon = NULL;
    }

    if ( iSliderIcon )
    {
       delete iSliderIcon;
       iSliderIcon = NULL;
    }

    if ( iSelectedSliderIcon )
    {
       delete iSelectedSliderIcon;
       iSelectedSliderIcon = NULL;
    }

    if ( iPositionLabel )
    {
        delete iPositionLabel;
        iPositionLabel = NULL;
    }

    if ( iDurationLabel )
    {
        delete iDurationLabel;
        iDurationLabel = NULL;
    }

#ifdef RD_TACTILE_FEEDBACK
    if ( iFeedback )
    {
        iFeedback->RemoveFeedbackForControl( this );
    }
#endif //RD_TACTILE_FEEDBACK

}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::HandleResourceChange()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::HandleResourceChange( TInt aType )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::HandleResourceChange(0x%X)"), aType);

    if ( aType == KAknsMessageSkinChange )
    {
        TRAP_IGNORE( SkinChangeL() );
    }

    CCoeControl::HandleResourceChange( aType );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::SetLayout()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::SetLayoutL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::SetLayoutL()"));

    //
    // Calculate icon rects
    //
    TRect progressRect = Rect();
    TInt topMarginHeight = ( progressRect.iBr.iY - KMPXProgressBarHeight ) / 2 - 10;
    progressRect.iTl.iY += topMarginHeight;
    progressRect.iBr.iY = progressRect.iTl.iY + KMPXProgressBarHeight;

    TAknLayoutRect seekBarFrameRect;
    seekBarFrameRect.LayoutRect( progressRect, mup_progress_pane_cp04().LayoutLine() );

    iFrameIconRect = seekBarFrameRect.Rect();

    iSliderRect.iTl.iY = iFrameIconRect.iTl.iY - KMPXSliderHeightOverProgresBar;
    iSliderRect.iBr.iY = iFrameIconRect.iBr.iY + KMPXSliderHeightOverProgresBar;

    //
    // Create labels
    //
    if ( iPositionLabel )
    {
        delete iPositionLabel;
        iPositionLabel = NULL;
    }

    iPositionLabel = new (ELeave) CEikLabel;
    AknLayoutUtils::LayoutLabel( iPositionLabel,
                                 progressRect,
                                 mp4_progress_pane_t1().LayoutLine() ) ;

    iPositionLabel->SetContainerWindowL( *this );
    iPositionLabel->SetTextL( KNullDesC );
    iPositionLabel->MakeVisible( ETrue );

    if ( iDurationLabel )
    {
        delete iDurationLabel;
        iDurationLabel = NULL;
    }

    iDurationLabel = new (ELeave) CEikLabel;

    AknLayoutUtils::LayoutLabel( iDurationLabel,
                                 progressRect,
                                 mp4_progress_pane_t2().LayoutLine() );

    iDurationLabel->SetContainerWindowL( *this );
    iDurationLabel->SetTextL( KNullDesC );
    iDurationLabel->MakeVisible( ETrue );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::SkinChangeL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::SkinChangeL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::SkinChangeL()"));

    //
    // Create icons
    //
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TFileName iconsPath;
    iController->LocateBitmapFileL( iconsPath );

    delete iFrameIcon;
    iFrameIcon = NULL;

    iFrameIcon = AknsUtils::CreateGulIconL(
        skin,
        KAknsIIDQgnGrafMup2BarFrame,
        iconsPath,
        EMbmMpxvideoplaybackcontrolsQgn_graf_mup2_bar_frame,
        EMbmMpxvideoplaybackcontrolsQgn_graf_mup2_bar_frame_mask);

    if ( iFrameIcon )
    {
        AknIconUtils::SetSize( iFrameIcon->Bitmap(),
                               iFrameIconRect.Size(),
                               EAspectRatioNotPreserved );
    }

    delete iPlaybackIcon;
    iPlaybackIcon = NULL;

    iPlaybackIcon = AknsUtils::CreateGulIconL(
        skin,
        KAknsIIDQgnGrafMup2BarProgress2,
        iconsPath,
        EMbmMpxvideoplaybackcontrolsQgn_graf_mup2_bar_progress_2,
        EMbmMpxvideoplaybackcontrolsQgn_graf_mup2_bar_progress_2_mask);

    if ( iPlaybackIcon )
    {
        AknIconUtils::SetSize( iPlaybackIcon->Bitmap(),
                               iFrameIconRect.Size(),
                               EAspectRatioNotPreserved );
    }

    UpdateProgressBarStateL( iController->FileDetails() );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::HandlePointerEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::HandlePointerEventL()"));

    TMPXPlaybackState state = iController->State();

    //
    //  Block pointer events in the following cases:
    //  - The clip is not seekable
    //  - The state is not playing/paused
    //  - If TV-Out is connected and the clip is DRM protected
    //
    if ( iController->FileDetails()->iSeekable &&
         ( state == EPbStatePlaying || state == EPbStatePaused ) )
    {
        TReal ratio = (TReal) ( aPointerEvent.iPosition.iX - iFrameIconRect.iTl.iX )/
                      (TReal)( iFrameIconRect.Width() - KMPXProgressSliderWidth );

        if ( ratio > 1.0 )
        {
            ratio = 1.0;
        }
        else if ( ratio < 0.0 )
        {
            ratio = 0.0;
        }

        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
            HandleButtonDownEventL( ratio );
        }
        else if ( aPointerEvent.iType == TPointerEvent::EDrag )
        {
            HandleDraggingEventL( ratio );
        }
        else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
            HandleButtonUpEventL( ratio );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackProgressBar::HandleButtonDownEventL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::HandleButtonDownEventL( TReal aRatio )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackProgressBar::HandleButtonDownEventL()"),
                   _L("ratio = %f"), aRatio );

    iPointerEventStarted = ETrue;
    iWasPlaying = iController->State() == EPbStatePlaying ? ETrue : EFalse;

    if ( iWasPlaying )
    {
        iController->HandleCommandL( EMPXPbvCmdCustomPause );
    }

#ifdef RD_TACTILE_FEEDBACK
    if ( iFeedback )
    {
#ifdef SYMBIAN_BUILD_GCE
        iFeedback->InstantFeedback( ETouchFeedbackSlider );
#else
        iFeedback->InstantFeedback( ETouchFeedbackBasic );
#endif //SYMBIAN_BUILD_GCE
    }
#endif //RD_TACTILE_FEEDBACK

    iDragging = EFalse;

    //
    //  Check for PDL using the download icon
    //
    if ( iDownloadIcon && ( aRatio > iDownloadRatio ) )
    {
        aRatio = iDownloadRatio;
    }

    PositionChangedL( aRatio * iDuration, ETrue );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackProgressBar::HandleDraggingEventL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::HandleDraggingEventL( TReal aRatio )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackProgressBar::HandleDraggingEventL()"),
                   _L("ratio = %f"), aRatio );

#ifdef RD_TACTILE_FEEDBACK
    if ( iFeedback )
    {
#ifdef SYMBIAN_BUILD_GCE
        iFeedback->InstantFeedback( ETouchFeedbackSlider );
#else
        iFeedback->InstantFeedback( ETouchFeedbackSensitive );
#endif //SYMBIAN_BUILD_GCE
    }
#endif //RD_TACTILE_FEEDBACK

    iDragging = ETrue;

    if ( iDownloadIcon && ( aRatio > iDownloadRatio ) )
    {
        aRatio = iDownloadRatio;
    }

    PositionChangedL( aRatio * iDuration, ETrue );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackProgressBar::HandleButtonUpEventL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::HandleButtonUpEventL( TReal aRatio )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackProgressBar::HandleButtonUpEventL()"),
                   _L("ratio = %f"), aRatio );

    iDragging = EFalse;

    if ( ( ! iDownloadIcon && aRatio == 1.0 ) ||
         ( iDownloadIcon && aRatio == 1.0 && iDownloadRatio == 1.0 ) )
    {
        iController->HandleCommandL( EMPXPbvCmdEndOfClip );
    }
    else
    {
        if ( iDownloadIcon && ( aRatio >= iDownloadRatio ) )
        {
            aRatio = iDownloadRatio;
        }

        TInt position = (TInt)( aRatio * iDuration );

        PositionChangedL( position, ETrue );

        iController->HandleCommandL( EMPXPbvCmdSetPosition, position );

        if ( iWasPlaying )
        {
            iWasPlaying = EFalse;
            iController->HandleCommandL( EMPXPbvCmdCustomPlay );
        }
    }

    iPointerEventStarted = EFalse;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackProgressBar::Draw()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::Draw( const TRect& aRect ) const
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::Draw()"));

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

    //
    // If it is live streaming, don't need to draw the progress bar
    //
    if ( ! iLiveStreamingString )
    {
        if ( iFrameIcon )
        {
            gc.BitBltMasked( iFrameIconRect.iTl,
                             iFrameIcon->Bitmap(),
                             TRect( iFrameIconRect.Size() ),
                             iFrameIcon->Mask(),
                             ETrue );
        }

        if ( iDownloadIcon && iDownloadRatio )
        {
            gc.BitBltMasked( iDownloadRect.iTl,
                             iDownloadIcon->Bitmap(),
                             TRect( iDownloadRect.Size() ),
                             iDownloadIcon->Mask(),
                             ETrue );
        }

        if ( iPlaybackIcon && iPlaybackRatio )
        {
            gc.BitBltMasked( iPlaybackRect.iTl,
                             iPlaybackIcon->Bitmap(),
                             TRect( iPlaybackRect.Size() ),
                             iPlaybackIcon->Mask(),
                             ETrue );
        }

        if ( iSliderIcon && iSelectedSliderIcon && iController->State() != EPbStateStopped )
        {
            if ( iDragging )
            {
                gc.BitBltMasked( iSliderRect.iTl,
                                 iSelectedSliderIcon->Bitmap(),
                                 TRect( iSliderRect.Size() ),
                                 iSelectedSliderIcon->Mask(),
                                 ETrue );
            }
            else
            {
                gc.BitBltMasked( iSliderRect.iTl,
                                 iSliderIcon->Bitmap(),
                                 TRect( iSliderRect.Size() ),
                                 iSliderIcon->Mask(),
                                 ETrue );
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::CountComponentControls()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackProgressBar::CountComponentControls() const
{
    return 2;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::ComponentControl()
// -------------------------------------------------------------------------------------------------
//
CCoeControl* CMPXVideoPlaybackProgressBar::ComponentControl( TInt aIndex ) const
{
    CCoeControl* control = NULL;

    switch ( aIndex )
    {
        case 0:
            control = iPositionLabel;
            break;
        case 1:
            control = iDurationLabel;
            break;
        default:
            break;
    }

    return control;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::DurationChangedL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::DurationChangedL( TInt aDuration )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::DurationChangedL() [%d]"), aDuration);

    iDuration = aDuration;

    //
    //  In live streaming case,
    //  Will show "live streaming" text instead of remaining time
    //
    if ( iLiveStreamingString )
    {
        iDurationLabel->SetTextL( iLiveStreamingString->Des() );
    }
    else
    {
        if ( iDuration > 0 )
        {
            SetTextLWithReadableTimeL( iDuration, iDurationLabel );
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::PositionChangedL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::PositionChangedL( TInt aPosition, TBool aUserChanged )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackProgressBar::PositionChangedL()"),
                   _L("aPosition = %d, aUserChanged = %d"), aPosition, aUserChanged );

    //
    //  We will ignore position information which engine sent
    //  after we issue SetPosition()
    //
    if ( aPosition == 0 ||
         aUserChanged   ||
         (! iPointerEventStarted && iController->State() != EPbStatePaused ) )
    {
        TReal position = aPosition;

        if ( iDuration > 0 && position > iDuration )
        {
            position = iDuration;
        }

        SetTextLWithReadableTimeL( (TInt)position, iPositionLabel );

        if ( ! iLiveStreamingString )
        {
            iPlaybackRatio = position / iDuration;
            iPlaybackRect = iFrameIconRect;

            if ( iSliderIcon )
            {
                iPlaybackRect.iBr.iX =
                    KMPXProgressSliderWidth / 2 + iPlaybackRatio *
                    ( iFrameIconRect.Width() - KMPXProgressSliderWidth );

                iSliderRect.iTl.iX =
                    iPlaybackRect.iBr.iX - KMPXProgressSliderWidth/2;
                iSliderRect.iBr.iX =
                    iPlaybackRect.iBr.iX + KMPXProgressSliderWidth/2;
            }
            else
            {
                iPlaybackRect.iBr.iX = iPlaybackRatio * iFrameIconRect.Width();
            }
        }

        if ( IsVisible() )
        {
            DrawNow();
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackProgressBar::SetDownloadSize()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::SetDownloadSize( TInt aSize )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::SetDownloadSize() [%d]"), aSize );

    iDownloadSize = aSize;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackProgressBar::UpdateDownloadPosition()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::UpdateDownloadPosition( TInt aSize )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::UpdateDownloadPosition() [%d]"), aSize );

    if ( iDownloadSize  && ( aSize > iOldDLSize ) )
    {
        iOldDLSize = aSize;
        iDownloadRatio = (TReal)aSize/(TReal)iDownloadSize;

        iDownloadRect= iFrameIconRect;

        if ( iDownloadIcon && iDownloadRatio > 0.0 )
        {
            iDownloadRect.iBr.iX = iDownloadRatio * iFrameIconRect.Width();
        }

        if ( IsVisible() )
        {
            DrawNow();
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackProgressBar::SetTextLWithReadableTimeL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::SetTextLWithReadableTimeL( TInt aTime, CEikLabel* aLabel )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackProgressBar::SetTextLWithReadableTimeL()"),
                   _L("aTime = %d"), aTime );

    TBuf<16> text;

    TTime time = TTime( (TInt64)aTime * KMPXMicroSeconds );

    if ( iDuration > 0 && iDuration < KMPXOneHourInSeconds )
    {
        // Format time to user readable format. (min:sec)
        time.FormatL( text, *iMinSecFormatString );
    }
    else
    {
        // Format time to user readable format. (hour:min:sec)
        time.FormatL( text, *iHourMinSecFormatString );
    }

    if ( AknTextUtils::DigitModeQuery( AknTextUtils::EDigitModeShownToUser ) )
    {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( text );
    }

    aLabel->SetTextL( text );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackProgressBar::UpdateProgressBarStateL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::UpdateProgressBarStateL( CMPXVideoPlaybackViewFileDetails* aDetails )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::UpdateProgressBarStateL()"));

    //
    //  Update the duration in case it has not been initialized yet
    //
    DurationChangedL( (TReal)aDetails->iDuration / (TReal)KPbMilliMultiplier );

    TFileName iconsPath;
    iController->LocateBitmapFileL( iconsPath );
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    if ( aDetails->iPlaybackMode == EMPXVideoProgressiveDownload )
    {
        delete iDownloadIcon;
        iDownloadIcon = NULL;

        iDownloadIcon = AknsUtils::CreateGulIconL(
            skin,
            KAknsIIDQgnGrafMup2BarProgress,
            iconsPath,
            EMbmMpxvideoplaybackcontrolsQgn_graf_mup2_bar_progress,
            EMbmMpxvideoplaybackcontrolsQgn_graf_mup2_bar_progress_mask );

        if ( iDownloadIcon )
        {
            AknIconUtils::SetSize( iDownloadIcon->Bitmap(),
                                   iFrameIconRect.Size(),
                                   EAspectRatioNotPreserved );
        }
    }
    else if ( aDetails->iPlaybackMode == EMPXVideoLiveStreaming )
    {
        iLiveStreamingString = iEikonEnv->AllocReadResourceL( R_MPX_LIVE_STREAMING );

        //
        //  update labels to show live streaming text
        //
        iDurationLabel->SetTextL( iLiveStreamingString->Des() );
    }

    if ( AknLayoutUtils::PenEnabled() && aDetails->iSeekable )
    {
        delete iSliderIcon;
        iSliderIcon = NULL;

        iSliderIcon = AknsUtils::CreateGulIconL(
            skin,
            KAknsIIDQgnGrafNsliderMarker,
            iconsPath,
            EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_marker,
            EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_marker_mask );

        delete iSelectedSliderIcon;
        iSelectedSliderIcon = NULL;

        iSelectedSliderIcon = AknsUtils::CreateGulIconL(
            skin,
            KAknsIIDQgnGrafNsliderMarkerSelected,
            iconsPath,
            EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_marker_selected,
            EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_marker_selected_mask );

        if ( iSliderIcon )
        {
            AknIconUtils::SetSize(
                iSliderIcon->Bitmap(),
                TSize( KMPXProgressSliderWidth,
                       iFrameIconRect.Height() + KMPXSliderHeightOverProgresBar*2 ),
                EAspectRatioNotPreserved );
        }

        if ( iSelectedSliderIcon )
        {
            AknIconUtils::SetSize(
                iSelectedSliderIcon->Bitmap(),
                TSize( KMPXProgressSliderWidth,
                       iFrameIconRect.Height() + KMPXSliderHeightOverProgresBar*2 ),
                EAspectRatioNotPreserved );
        }
    }
    else
    {
        //
        // for non-seekable media, disable iSliderIcon and iSelectedSliderIcon
        //

        if ( iSliderIcon )
        {
            delete iSliderIcon;
            iSliderIcon = NULL;
        }

        if ( iSelectedSliderIcon )
        {
            delete iSelectedSliderIcon;
            iSelectedSliderIcon = NULL;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackProgressBar::Reset
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackProgressBar::Reset()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackProgressBar::Reset()"));

    if ( iPointerEventStarted || iDragging )
    {
        TPointerEvent event;
        event.iType = TPointerEvent::EButton1Up;
        event.iPosition.iX = iSliderRect.iTl.iX;

        MPX_TRAPD( err, HandlePointerEventL(event) );
    }
}

//  End of File
