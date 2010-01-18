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

// Version : %version: 15 %


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
const TInt KMPXVolumeBarWidth = 24;
const TInt KMPXVolumeDragEventTimeOut = 100000;
const TInt KMPXPtrEventRepeatRequestTime = 200000;

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

    //
    // Calculate icon rects
    //
    iRect = TRect(0, 0, iRect.Width(), iRect.Height() );
    TInt volumeUpDownIndicatorSize = iRect.Width() / 3;

    iVolumeControlRect = TRect( volumeUpDownIndicatorSize,
                                iRect.iTl.iY,
                                iRect.iBr.iX,
                                iRect.iTl.iY + iRect.Height() * 4 / 5 );

    iVolumeBarRect =  TRect( iVolumeControlRect.iTl.iX + iVolumeControlRect.Width() / 3,
                             iVolumeControlRect.iTl.iY + iVolumeControlRect.Height() / 20,
                             iVolumeControlRect.iTl.iX + iVolumeControlRect.Width() / 3 * 2,
                             iVolumeControlRect.iTl.iY + iVolumeControlRect.Height() );

    iVolumeBarRectTop = TRect( iRect.Width() / 2 - KMPXVolumeBarWidth / 2 ,
                               iVolumeBarRect.iTl.iY,
                               iRect.Width() / 2 + KMPXVolumeBarWidth / 2,
                               iVolumeBarRect.iTl.iY + iVolumeBarRect.Height() / 10 );

    iVolumeBarRectMiddle = TRect( iVolumeBarRectTop.iTl.iX,
                                  iVolumeBarRectTop.iBr.iY,
                                  iVolumeBarRectTop.iBr.iX,
                                  iVolumeBarRectTop.iBr.iY + iVolumeBarRectTop.Height() * 8 );

    iVolumeBarRectBottom = TRect( iVolumeBarRectTop.iTl.iX,
                                  iVolumeBarRectMiddle.iBr.iY,
                                  iVolumeBarRectTop.iBr.iX,
                                  iVolumeBarRect.iBr.iY );

    iSliderRect = TRect( iRect.Width() / 2 - KMPXSliderWidth / 2 + 3,
                         iVolumeBarRect.iBr.iY - KMPXSliderHeight,
                         iRect.Width() / 2 + KMPXSliderWidth / 2 + 3,
                         iVolumeBarRect.iBr.iY );

    iVolumeUpControlRect = TRect( iRect.iTl,
                           TSize( volumeUpDownIndicatorSize,
                                  iVolumeControlRect.Height() / 2 ) );

    iVolumeDownControlRect = TRect( TPoint ( iRect.iTl.iX, iVolumeControlRect.Height() / 2 ),
                                    iVolumeUpControlRect.Size() );

    iVolumeUpRect = TRect( iVolumeUpControlRect.iTl.iX + 2,
                           iVolumeUpControlRect.iTl.iY + 8,
                           iVolumeUpControlRect.iBr.iX + 2,
                           iVolumeUpControlRect.iTl.iY + volumeUpDownIndicatorSize );

    iVolumeDownRect = TRect( iVolumeUpRect.iTl.iX,
                             iVolumeDownControlRect.iBr.iY - volumeUpDownIndicatorSize,
                             iVolumeUpRect.iBr.iX,
                             iVolumeDownControlRect.iBr.iY  );

    iSpeakerControlRect = TRect( TPoint( iRect.iTl.iX, iVolumeControlRect.iBr.iY ),
                                 iRect.iBr );

    iSpeakerRect = TRect( iSpeakerControlRect.iTl.iX + iSpeakerControlRect.Width() / 5,
                          iSpeakerControlRect.iTl.iY + iSpeakerControlRect.Height() / 5,
                          iSpeakerControlRect.iBr.iX - iSpeakerControlRect.Width() / 5,
                          iSpeakerControlRect.iBr.iY - iSpeakerControlRect.Height() / 5 );

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
                               TSize( KMPXVolumeBarWidth, iVolumeBarRectTop.Height() ),
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
                               TSize( KMPXVolumeBarWidth, iVolumeBarRectMiddle.Height() ),
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
                               TSize( KMPXVolumeBarWidth, iVolumeBarRectBottom.Height() ),
                               EAspectRatioNotPreserved );
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
        SetVolumeL( iVolumeWhileDraggingEvent );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackVolumeBar::HandlePointerEventL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackVolumeBar::HandlePointerEventL()"));

    //
    //  Block pointer events in the following cases:
    //  - If TV-Out is connected and the clip is DRM protected
    //
    if ( iController->IsTvOutPlaybackAllowed() && iController->FileDetails()->iAudioEnabled )
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
    else
    {
        // draw a solid background so that the entire progress
        // bar is shown not just the area representing the
        // portion that has been played.
        gc.SetBrushColor( KRgbBlack );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.DrawRect( aRect );
        gc.SetBrushStyle(CGraphicsContext::ENullBrush);
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

    if ( iVolumeFrameIconTop )
    {
        gc.BitBltMasked( iVolumeBarRectTop.iTl,
                         iVolumeFrameIconTop->Bitmap(),
                         TRect(iVolumeBarRectTop.Size()),
                         iVolumeFrameIconTop->Mask(),
                         ETrue );
    }

    if ( iVolumeFrameIconMiddle )
    {
        gc.BitBltMasked( iVolumeBarRectMiddle.iTl,
                         iVolumeFrameIconMiddle->Bitmap(),
                         TRect(iVolumeBarRectMiddle.Size()),
                         iVolumeFrameIconMiddle->Mask(),
                         ETrue );
    }

    if ( iVolumeFrameIconBottom )
    {
        gc.BitBltMasked( iVolumeBarRectBottom.iTl,
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
	MPX_DEBUG(_L("CMPXVideoPlaybackVolumeBar::Reset()"));

    if ( iDragging != EVolumeNotDragging )
    {
    	iDragging = EVolumeNotDragging;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackVolumeBar::SetVolumeL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackVolumeBar::SetVolumeL( TInt aVolume )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackVolumeBar::SetVolumeL(%d)"), aVolume);
       
    iController->HandleCommandL( EMPXPbvCmdSetVolume, aVolume );
}

//  End of File
