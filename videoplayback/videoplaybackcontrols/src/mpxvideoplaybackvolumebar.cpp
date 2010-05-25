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
* Description: Volume bar control
*
*/


// Version : %version: 23 %


// INCLUDE FILES
#include <eikenv.h>
#include <gulicon.h>
#include <gulfont.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <StringLoader.h>
#include <AknsDrawUtils.h>
#include <data_caging_path_literals.hrh>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <mpxvideoplaybackcontrols.mbg>

#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackvolumebar.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideo_debug.h"

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK

// CONSTANTS
const TInt KMPXSliderHeight = 30;
const TInt KMPXSliderWidth = 18;
const TInt KMPXVolumeDragEventTimeOut = 100000;
const TInt KMPXPtrEventRepeatRequestTime = 200000;

using namespace AknLayoutScalable_Apps;
using namespace AknLayoutScalable_Avkon;

// ============================ MEMBER FUNCTIONS ===================================================

CMPXVideoPlaybackVolumeBar::CMPXVideoPlaybackVolumeBar(
    CMPXVideoPlaybackControlsController* aController, TRect aRect )
    : iOneVolumeIncrementHeight(0)
    , iMuted(EFalse)
    , iRect(aRect)
    , iDragging(EVolumeNotDragging)
    , iController(aController)
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::ConstructL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackVolumeBar::ConstructL()"));

    SetLayout();
    SkinChangeL();

    iDraggingHandlerTimer = CPeriodic::NewL( CActive::EPriorityStandard );

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif //RD_TACTILE_FEEDBACK
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackVolumeBar* CMPXVideoPlaybackVolumeBar::NewL(
    CMPXVideoPlaybackControlsController* aController, TRect aRect )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackVolumeBar::NewL()"));

    CMPXVideoPlaybackVolumeBar* self =
        new ( ELeave ) CMPXVideoPlaybackVolumeBar( aController, aRect );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::~CMPXVideoPlaybackVolumeBar()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackVolumeBar::~CMPXVideoPlaybackVolumeBar()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackVolumeBar::~CMPXVideoPlaybackVolumeBar()"));

    if ( iDraggingHandlerTimer )
    {
        iDraggingHandlerTimer->Cancel();
        delete iDraggingHandlerTimer;
        iDraggingHandlerTimer = NULL;
    }

    if (iSpeakerIcon)
    {
       delete iSpeakerIcon;
       iSpeakerIcon = NULL;
    }

    if (iSpeakerMuteIcon)
    {
       delete iSpeakerMuteIcon;
       iSpeakerMuteIcon = NULL;
    }

    if (iSliderIcon)
    {
       delete iSliderIcon;
       iSliderIcon = NULL;
    }

    if (iSliderSelectedIcon)
    {
       delete iSliderSelectedIcon;
       iSliderSelectedIcon = NULL;
    }

    if (iVolumeUpIcon)
    {
       delete iVolumeUpIcon;
       iVolumeUpIcon = NULL;
    }

    if (iVolumeDownIcon)
    {
       delete iVolumeDownIcon;
       iVolumeDownIcon = NULL;
    }

    if (iVolumeFrameIconTop)
    {
        delete iVolumeFrameIconTop;
        iVolumeFrameIconTop = NULL;
    }

    if (iVolumeFrameIconMiddle)
    {
        delete iVolumeFrameIconMiddle;
        iVolumeFrameIconMiddle = NULL;
    }

    if (iVolumeFrameIconBottom)
    {
        delete iVolumeFrameIconBottom;
        iVolumeFrameIconBottom = NULL;
    }

    if (iNoAudioIcon)
    {
       delete iNoAudioIcon;
       iNoAudioIcon = NULL;
    }

#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
        iFeedback->RemoveFeedbackForControl(this);
    }
#endif //RD_TACTILE_FEEDBACK

}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::HandleResourceChange()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::HandleResourceChange( TInt aType )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackVolumeBar::HandleResourceChange(0x%X)"), aType);

    if ( aType == KAknsMessageSkinChange )
    {
        TRAP_IGNORE( SkinChangeL() );
    }

    CCoeControl::HandleResourceChange( aType );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::SetLayout()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::SetLayout()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackVolumeBar::SetLayout()"));

    TAknLayoutRect tmpLayoutRect;

    iRect = TRect( 0, 0, iRect.Width(), iRect.Height() );

    tmpLayoutRect.LayoutRect( iRect, bg_popup_window_pane_cp19( 9 ).LayoutLine() );
    iVolumeControlRect = tmpLayoutRect.Rect();


    tmpLayoutRect.LayoutRect( iRect, aid_touch_area_slider( 6 ).LayoutLine() );
    iVolumeBarRect = tmpLayoutRect.Rect();

    tmpLayoutRect.LayoutRect( iRect, slider_pane( 6 ).LayoutLine() );
    TRect sliderBarRect( tmpLayoutRect.Rect() );
    TInt volumeBarEndHeight = sliderBarRect.Height() / 10;
    iVolumeBarRectTop = TRect( sliderBarRect.iTl.iX,
                               sliderBarRect.iTl.iY,
                               sliderBarRect.iBr.iX,
                               sliderBarRect.iTl.iY + volumeBarEndHeight );

    iVolumeBarRectBottom = TRect( sliderBarRect.iTl.iX,
                                  sliderBarRect.iBr.iY - volumeBarEndHeight,
                                  sliderBarRect.iBr.iX,
                                  sliderBarRect.iBr.iY );

    iVolumeBarRectMiddle = TRect( iVolumeBarRectTop.iTl.iX,
                                  iVolumeBarRectTop.iBr.iY,
                                  iVolumeBarRectTop.iBr.iX,
                                  iVolumeBarRectBottom.iTl.iY );

    TInt volumeBarMiddleX = sliderBarRect.iTl.iX + sliderBarRect.Width() / 2;
    iSliderRect = TRect( volumeBarMiddleX - KMPXSliderWidth / 2,
                         sliderBarRect.iBr.iY - KMPXSliderHeight,
                         volumeBarMiddleX + KMPXSliderWidth / 2,
                         sliderBarRect.iBr.iY );

    tmpLayoutRect.LayoutRect( iRect, aid_touch_area_increase( 6 ).LayoutLine() );
    iVolumeUpControlRect = tmpLayoutRect.Rect();

    tmpLayoutRect.LayoutRect( iRect, aid_touch_area_decrease( 6 ).LayoutLine() );
    iVolumeDownControlRect = tmpLayoutRect.Rect();

    tmpLayoutRect.LayoutRect( iRect, popup_slider_window_g4( 6 ).LayoutLine() );
    iVolumeUpRect = tmpLayoutRect.Rect();

    tmpLayoutRect.LayoutRect( iRect, popup_slider_window_g5( 6 ).LayoutLine() );
    iVolumeDownRect = tmpLayoutRect.Rect();

    tmpLayoutRect.LayoutRect( iRect, aid_touch_area_mute( 4 ).LayoutLine() );
    iSpeakerControlRect = tmpLayoutRect.Rect();

    tmpLayoutRect.LayoutRect( iRect, popup_slider_window_g6( 2 ).LayoutLine() );
    iSpeakerRect = tmpLayoutRect.Rect();

    iOneVolumeIncrementHeight = (TReal)( iVolumeBarRect.Height() - KMPXSliderHeight )
                                / (TReal)KPbPlaybackVolumeLevelMax;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::SkinChangeL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::SkinChangeL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackVolumeBar::SkinChangeL()"));

    //
    // Create icons
    //
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TFileName iconsPath;
    iController->LocateBitmapFileL( iconsPath );

    delete iSpeakerIcon;
    iSpeakerIcon = NULL;
    iSpeakerIcon = AknsUtils::CreateGulIconL(
            skin,
		    KAknsIIDQgnIndiNsliderUnmuted,
		    iconsPath,
		    EMbmMpxvideoplaybackcontrolsQgn_indi_nslider_unmuted,
		    EMbmMpxvideoplaybackcontrolsQgn_indi_nslider_unmuted_mask );

    if ( iSpeakerIcon )
    {
        AknIconUtils::SetSize( iSpeakerIcon->Bitmap(),
                               iSpeakerRect.Size(),
                               EAspectRatioPreserved );
    }

    delete iSpeakerMuteIcon;
    iSpeakerMuteIcon = NULL;
    iSpeakerMuteIcon = AknsUtils::CreateGulIconL(
            skin,
		    KAknsIIDQgnIndiNsliderMuted,
		    iconsPath,
		    EMbmMpxvideoplaybackcontrolsQgn_indi_nslider_muted,
		    EMbmMpxvideoplaybackcontrolsQgn_indi_nslider_muted_mask );


    if ( iSpeakerMuteIcon )
    {
        AknIconUtils::SetSize( iSpeakerMuteIcon->Bitmap(),
                               iSpeakerRect.Size(),
                               EAspectRatioPreserved );
    }

    delete iNoAudioIcon;
    iNoAudioIcon = NULL;
    iNoAudioIcon = AknsUtils::CreateGulIconL(
            skin,
            KAknsIIDQgnGrafMupLstCorrupttrack,
            iconsPath,
            EMbmMpxvideoplaybackcontrolsQgn_graf_mup_lst_corrupttrack,
            EMbmMpxvideoplaybackcontrolsQgn_graf_mup_lst_corrupttrack_mask );


    if ( iNoAudioIcon )
    {
        AknIconUtils::SetSize( iNoAudioIcon->Bitmap(),
                               iSpeakerRect.Size(),
                               EAspectRatioPreserved );
    }

    delete iSliderIcon;
    iSliderIcon = NULL;
    iSliderIcon = AknsUtils::CreateGulIconL(
            skin,
		    KAknsIIDQgnGrafNsliderVerticalMarker,
		    iconsPath,
		    EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_vertical_marker,
		    EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_vertical_marker_mask );

    if ( iSliderIcon )
    {
        AknIconUtils::SetSize( iSliderIcon->Bitmap(),
                               TSize(KMPXSliderWidth, KMPXSliderHeight),
                               EAspectRatioNotPreserved );
    }

    delete iSliderSelectedIcon;
    iSliderSelectedIcon = NULL;
    iSliderSelectedIcon = AknsUtils::CreateGulIconL(
            skin,
		    KAknsIIDQgnGrafNsliderVerticalMarkerSelected,
		    iconsPath,
		    EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_vertical_marker_selected,
		    EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_vertical_marker_selected_mask );

    if ( iSliderSelectedIcon )
    {
        AknIconUtils::SetSize( iSliderSelectedIcon->Bitmap(),
                               TSize(KMPXSliderWidth, KMPXSliderHeight),
                               EAspectRatioNotPreserved );
    }

    delete iVolumeUpIcon;
    iVolumeUpIcon = NULL;
    iVolumeUpIcon = AknsUtils::CreateGulIconL(
            skin,
		    KAknsIIDQgnIndiCam4ZoomMax,
		    iconsPath,
		    EMbmMpxvideoplaybackcontrolsQgn_indi_nslider_level_increase,
		    EMbmMpxvideoplaybackcontrolsQgn_indi_nslider_level_increase_mask );

    if ( iVolumeUpIcon )
    {
        AknIconUtils::SetSize( iVolumeUpIcon->Bitmap(),
                               iVolumeUpRect.Size(),
                               EAspectRatioPreserved );
    }

    delete iVolumeDownIcon;
    iVolumeDownIcon = NULL;
    iVolumeDownIcon = AknsUtils::CreateGulIconL(
            skin,
		    KAknsIIDQgnIndiCam4ZoomMin,
		    iconsPath,
		    EMbmMpxvideoplaybackcontrolsQgn_indi_nslider_level_decrease,
		    EMbmMpxvideoplaybackcontrolsQgn_indi_nslider_level_decrease_mask );

    if ( iVolumeDownIcon )
    {
        AknIconUtils::SetSize( iVolumeDownIcon->Bitmap(),
                               iVolumeDownRect.Size(),
                               EAspectRatioPreserved );
    }

    delete iVolumeFrameIconTop;
    iVolumeFrameIconTop = NULL;
    iVolumeFrameIconTop = AknsUtils::CreateGulIconL(
            skin,
		    KAknsIIDQgnIndiCam4ZoomTop,
		    iconsPath,
		    EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_vertical_top,
		    EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_vertical_top_mask );

    if ( iVolumeFrameIconTop )
    {
        AknIconUtils::SetSize( iVolumeFrameIconTop->Bitmap(),
                               TSize( iVolumeBarRectTop.Width(), iVolumeBarRectTop.Height() ),
                               EAspectRatioNotPreserved );
    }

    delete iVolumeFrameIconMiddle;
    iVolumeFrameIconMiddle = NULL;
    iVolumeFrameIconMiddle = AknsUtils::CreateGulIconL(
            skin,
		    KAknsIIDQgnIndiCam4ZoomMiddle,
		    iconsPath,
		    EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_vertical_middle,
		    EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_vertical_middle_mask );

    if ( iVolumeFrameIconMiddle )
    {
        AknIconUtils::SetSize( iVolumeFrameIconMiddle->Bitmap(),
                               TSize( iVolumeBarRectMiddle.Width(), iVolumeBarRectMiddle.Height() ),
                               EAspectRatioNotPreserved );
    }

    delete iVolumeFrameIconBottom;
    iVolumeFrameIconBottom = NULL;
    iVolumeFrameIconBottom = AknsUtils::CreateGulIconL(
            skin,
		    KAknsIIDQgnIndiCam4ZoomBottom,
		    iconsPath,
		    EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_vertical_bottom,
		    EMbmMpxvideoplaybackcontrolsQgn_graf_nslider_vertical_bottom_mask );

    if ( iVolumeFrameIconBottom )
    {
        AknIconUtils::SetSize( iVolumeFrameIconBottom->Bitmap(),
                               TSize( iVolumeBarRectBottom.Width(), iVolumeBarRectBottom.Height() ),
                               EAspectRatioNotPreserved );
    }

    // Calculate the X coordinate to center the icon of sliderbar
    TRect volumeBarRectPart;
    CGulIcon* volumeFrameIconPart = NULL;
    if ( iVolumeFrameIconTop )
    {
        volumeFrameIconPart = iVolumeFrameIconTop;
        volumeBarRectPart = iVolumeBarRectTop;
    }
    else if ( iVolumeFrameIconMiddle )
    {
        volumeFrameIconPart = iVolumeFrameIconMiddle;
        volumeBarRectPart = iVolumeBarRectMiddle;
    }
    else if ( iVolumeFrameIconBottom )
    {
        volumeFrameIconPart = iVolumeFrameIconBottom;
        volumeBarRectPart = iVolumeBarRectBottom;
    }

    if ( volumeFrameIconPart )
    {
        iCenteredSliderbarIconX = volumeBarRectPart.iTl.iX +
            ( volumeBarRectPart.Width() -
              volumeFrameIconPart->Bitmap()->SizeInPixels().iWidth ) / 2;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackVolumeBar::HandleVolumeIncreaseL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::HandleVolumeIncreaseL( const TPointerEvent& aPointerEvent )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackVolumeBar::HandleVolumeIncreaseL()"));

    switch ( aPointerEvent.iType )
    {
        case TPointerEvent::EButtonRepeat:
        {
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
            iController->HandleCommandL( EMPXPbvCmdIncreaseVolume );
            iDragging = EVolumeIncreaseDragging;

        }
        case TPointerEvent::EButton1Down:
        {
            Window().RequestPointerRepeatEvent(
                         TTimeIntervalMicroSeconds32(KMPXPtrEventRepeatRequestTime)
                         ,iVolumeUpControlRect);

            break;
        }
        case TPointerEvent::EDrag:
        {
            iDragging = EVolumeIncreaseDragging;

            break;
        }
        case TPointerEvent::EButton1Up:
        {
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

            iController->HandleCommandL( EMPXPbvCmdIncreaseVolume );

            iDragging = EVolumeNotDragging;

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackVolumeBar::HandleVolumeDecreaseL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::HandleVolumeDecreaseL( const TPointerEvent& aPointerEvent )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackVolumeBar::HandleVolumeDecreaseL()"));

    switch ( aPointerEvent.iType )
    {
        case TPointerEvent::EButtonRepeat:
        {
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
            iController->HandleCommandL( EMPXPbvCmdDecreaseVolume );
            iDragging = EVolumeDecreaseDragging;

        }
        case TPointerEvent::EButton1Down:
        {
            Window().RequestPointerRepeatEvent(
                         TTimeIntervalMicroSeconds32(KMPXPtrEventRepeatRequestTime)
                         ,iVolumeDownControlRect);

            break;
        }
        case TPointerEvent::EDrag:
        {
            iDragging = EVolumeDecreaseDragging;
            break;
        }
        case TPointerEvent::EButton1Up:
        {
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

            iController->HandleCommandL( EMPXPbvCmdDecreaseVolume );

            iDragging = EVolumeNotDragging;

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackVolumeBar::HandleSpeakerControlEventL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::HandleSpeakerControlEventL( const TPointerEvent& aPointerEvent )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackVolumeBar::HandleSpeakerControlEventL()"));

    switch ( aPointerEvent.iType )
    {
        case TPointerEvent::EButton1Up:
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

            if ( iMuted )
            {
                iController->HandleCommandL( EMPXPbvCmdUnMute );
            }
            else
            {
                iController->HandleCommandL( EMPXPbvCmdMute );
            }

            iDragging = EVolumeNotDragging;

            break;
        }
        case TPointerEvent::EDrag:
        {
            iDragging = EVolumeSpeakerDragging;

            break;
        }
        case TPointerEvent::EButton1Down:
        {
            iDragging = EVolumeNotDragging;

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackVolumeBar::HandleVolumeBarEventL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::HandleVolumeBarEventL( const TPointerEvent& aPointerEvent )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackVolumeBar::HandleVolumeBarEventL()"));

    TInt vol = 0;

    if ( aPointerEvent.iPosition.iY < iVolumeBarRect.iTl.iY + KMPXSliderHeight / 2 )
    {
        vol = KPbPlaybackVolumeLevelMax;
    }
    else if ( aPointerEvent.iPosition.iY > iVolumeBarRect.iBr.iY - KMPXSliderHeight / 2 )
    {
        vol = 0;
    }
    else
    {
        vol = ( iVolumeBarRect.iBr.iY - KMPXSliderHeight / 2 - aPointerEvent.iPosition.iY ) /
              iOneVolumeIncrementHeight;
    }

    switch ( aPointerEvent.iType )
    {
        case TPointerEvent::EButton1Down:
        {
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

            if ( ! iDraggingHandlerTimer->IsActive() )
            {
                iDraggingHandlerTimer->Start(
                    KMPXVolumeDragEventTimeOut,
                    KMPXVolumeDragEventTimeOut,
                    TCallBack( CMPXVideoPlaybackVolumeBar::HandleVolumeDragEventTimeOut, this ) );
            }

            iDragging = EVolumeNotDragging;

            break;
        }
        case TPointerEvent::EDrag:
        {
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

            iVolumeWhileDraggingEvent = vol;
            iDragging = EVolumeBarDragging;

            break;
        }
        case TPointerEvent::EButton1Up:
        {
            if ( iDraggingHandlerTimer->IsActive() )
            {
                iDraggingHandlerTimer->Cancel();
            }

            SetVolumeL( vol );

            if ( iDragging == EVolumeBarDragging && vol == 0 )
            {
                //
                // volume & mute levels are kept track in playbackplugin.
                //
                iController->HandleCommandL( EMPXPbvCmdMute );
            }

            iDragging = EVolumeNotDragging;

            if ( IsVisible() )
            {
                DrawNow();
            }

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::HandleVolumeDragEventTimeOut
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackVolumeBar::HandleVolumeDragEventTimeOut( TAny* aPtr )
{
    static_cast<CMPXVideoPlaybackVolumeBar*>(aPtr)->DoHandleVolumeDragEventTimeOut();
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::DoHandleVolumeDragEventTimeOut
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::DoHandleVolumeDragEventTimeOut()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackController::DoHandleVolumeDragEventTimeOut()"));

    if ( iDragging == EVolumeBarDragging )
    {
        MPX_TRAPD( err, SetVolumeL( iVolumeWhileDraggingEvent ) );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackVolumeBar::HandlePointerEventL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackVolumeBar::HandlePointerEventL()"));

    if ( iController->FileDetails()->iAudioEnabled )
    {
        //
        //  Volume up
        //
        if ( iDragging == EVolumeIncreaseDragging ||
             ( iVolumeUpControlRect.Contains( aPointerEvent.iPosition ) &&
               iDragging == EVolumeNotDragging ) )
        {
            HandleVolumeIncreaseL( aPointerEvent );
        }
        //
        // Volume down
        //
        else if ( iDragging == EVolumeDecreaseDragging ||
                  ( iVolumeDownControlRect.Contains( aPointerEvent.iPosition ) &&
                    iDragging == EVolumeNotDragging ) )
        {
            HandleVolumeDecreaseL( aPointerEvent );
        }
        //
        // Speaker/Mute
        //
        else if ( iDragging == EVolumeSpeakerDragging ||
                  ( iSpeakerControlRect.Contains( aPointerEvent.iPosition ) &&
                    iDragging == EVolumeNotDragging ) )
        {
            HandleSpeakerControlEventL( aPointerEvent );
        }
        //
        // Volume Bar
        //
        else
        {
            HandleVolumeBarEventL( aPointerEvent );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackVolumeBar::Draw()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::Draw( const TRect& aRect ) const
{
    MPX_DEBUG(_L("CMPXVideoPlaybackVolumeBar::Draw()"));

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

    if ( ! iController->FileDetails()->iAudioEnabled || iMuted )
    {
        if ( iController->FileDetails()->iAudioEnabled )
        {
            //
            // use "Mute" icon for media clip with audio supported
            //
            gc.BitBltMasked( iSpeakerRect.iTl,
                             iSpeakerMuteIcon->Bitmap(),
                             TRect(iSpeakerRect.Size()),
                             iSpeakerMuteIcon->Mask(),
                             ETrue );
        }
        else
        {
            //
            // use "No Audio" icon instead of the generic "Mute" icon
            // for media clip with audio not supported
            //
            gc.BitBltMasked( iSpeakerRect.iTl,
                             iNoAudioIcon->Bitmap(),
                             TRect(iSpeakerRect.Size()),
                             iNoAudioIcon->Mask(),
                             ETrue );
        }
    }
    else
    {
        gc.BitBltMasked( iSpeakerRect.iTl,
                         iSpeakerIcon->Bitmap(),
                         TRect(iSpeakerRect.Size()),
                         iSpeakerIcon->Mask(),
                         ETrue );
    }

    if ( iVolumeUpIcon )
    {
        gc.BitBltMasked( iVolumeUpRect.iTl,
                         iVolumeUpIcon->Bitmap(),
                         TRect(iVolumeUpRect.Size()),
                         iVolumeUpIcon->Mask(),
                         ETrue );
    }

    if ( iVolumeDownIcon )
    {
        gc.BitBltMasked( iVolumeDownRect.iTl,
                         iVolumeDownIcon->Bitmap(),
                         TRect(iVolumeDownRect.Size()),
                         iVolumeDownIcon->Mask(),
                         ETrue );
    }

    TPoint srcPoint( iCenteredSliderbarIconX, 0 );
    if ( iVolumeFrameIconTop )
    {
        srcPoint.iY = iVolumeBarRectTop.iTl.iY;
        gc.BitBltMasked( srcPoint,
                         iVolumeFrameIconTop->Bitmap(),
                         TRect(iVolumeBarRectTop.Size()),
                         iVolumeFrameIconTop->Mask(),
                         ETrue );
    }

    if ( iVolumeFrameIconMiddle )
    {
        srcPoint.iY = iVolumeBarRectMiddle.iTl.iY;
        gc.BitBltMasked( srcPoint,
                         iVolumeFrameIconMiddle->Bitmap(),
                         TRect(iVolumeBarRectMiddle.Size()),
                         iVolumeFrameIconMiddle->Mask(),
                         ETrue );
    }

    if ( iVolumeFrameIconBottom )
    {
        srcPoint.iY = iVolumeBarRectBottom.iTl.iY;
        gc.BitBltMasked( srcPoint,
                         iVolumeFrameIconBottom->Bitmap(),
                         TRect(iVolumeBarRectBottom.Size()),
                         iVolumeFrameIconBottom->Mask(),
                         ETrue );
    }

    if ( iController->FileDetails() &&
         iController->FileDetails()->iAudioEnabled &&
         iSliderIcon &&
         iSliderSelectedIcon )
    {
        if ( iDragging == EVolumeBarDragging )
        {
            gc.BitBltMasked( iSliderRect.iTl,
                             iSliderSelectedIcon->Bitmap(),
                             TRect(iSliderRect.Size()),
                             iSliderSelectedIcon->Mask(),
                             ETrue );
        }
        else
        {
            gc.BitBltMasked( iSliderRect.iTl,
                             iSliderIcon->Bitmap(),
                             TRect(iSliderRect.Size()),
                             iSliderIcon->Mask(),
                             ETrue );
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::CountComponentControls()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackVolumeBar::CountComponentControls() const
{
    return 0;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::VolumeChanged()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::VolumeChanged( TInt aVolume )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackVolumeBar::VolumeChanged() [%d]"), aVolume);

    iMuted = (aVolume == 0)? ETrue:EFalse;

    TReal volumeBarHeight = (TReal)iVolumeBarRect.iBr.iY -
                            iOneVolumeIncrementHeight * (TReal)aVolume;

    iSliderRect.iTl.iY = volumeBarHeight - KMPXSliderHeight;
    iSliderRect.iBr.iY = volumeBarHeight;

    if ( IsVisible() )
    {
        DrawNow();
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::Reset
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::Reset()
{
	MPX_ENTER_EXIT(_L("CMPXVideoPlaybackVolumeBar::Reset()"));

    if ( iDragging != EVolumeNotDragging )
    {
        TPointerEvent event;
        event.iType = TPointerEvent::EButton1Up;
        event.iPosition.iY = ( iSliderRect.iTl.iY + iSliderRect.iBr.iY ) / 2;

        MPX_TRAPD( err, HandlePointerEventL(event) );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::SetVolumeL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::SetVolumeL( TInt aVolume )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackVolumeBar::SetVolumeL()"),
                   _L("aVolume = %d"), aVolume );

    iController->HandleCommandL( EMPXPbvCmdSetVolume, aVolume );
}

//  End of File
