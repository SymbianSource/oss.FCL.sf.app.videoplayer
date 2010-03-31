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
* Description: Button bar control
*
*/

// Version : %version: e003sa33#17 %



// INCLUDE FILES
#include <eikenv.h>
#include <barsread.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <StringLoader.h>
#include <AknsDrawUtils.h>

#include <mpxvideoplaybackcontrols.mbg>

#include "mpxvideoplaybackbutton.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackcontrolscontroller.h"

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK

// CONSTANTS
const TInt KMPXLongPressInterval = 800000;
const TInt KMPXTactileInterval = 200000;

// ============================ MEMBER FUNCTIONS ===================================================

CMPXVideoPlaybackButtonBar::CMPXVideoPlaybackButtonBar(
    CMPXVideoPlaybackControlsController* aController )
    : iButtons( EMPXButtonCount )
    , iController( aController )
    , iSeeking( EFalse )
    , iButtonPressed( EMPXNotPressed )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::ConstructL( TRect aRect )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::ConstructL()"));

    SetRect( TRect(0, 0, aRect.Width(), aRect.Height()) );

    CreateButtonsL();

    iSeekingTimer = CPeriodic::NewL( CActive::EPriorityStandard );

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif //RD_TACTILE_FEEDBACK
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackButtonBar* CMPXVideoPlaybackButtonBar::NewL(
    CMPXVideoPlaybackControlsController* aController, TRect aRect )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::NewL()"));

    CMPXVideoPlaybackButtonBar* self = new ( ELeave ) CMPXVideoPlaybackButtonBar( aController );

    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::~CMPXVideoPlaybackButtonBar()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackButtonBar::~CMPXVideoPlaybackButtonBar()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::~CMPXVideoPlaybackButtonBar()"));

    if ( iSeekingTimer )
    {
        if ( iSeekingTimer->IsActive() )
        {
            iSeekingTimer->Cancel();
        }

        delete iSeekingTimer;
        iSeekingTimer = NULL;
    }

    iButtons.ResetAndDestroy();

#ifdef RD_TACTILE_FEEDBACK
    if ( iFeedback )
    {
        iFeedback->RemoveFeedbackForControl(this);
    }
#endif //RD_TACTILE_FEEDBACK
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::CreateButtonsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::CreateButtonsL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::CreateButtonsL()"));

    TFileName iconsPath;
    iController->LocateBitmapFileL( iconsPath );

    //
    // Since button bar has 3 visible buttons - divide the height of aRect by 3
    //
    TInt iconSize = Rect().Height() / 3;
    TInt leftOffset = ( Rect().Width() - iconSize ) / 2 ;

    //
    // Create 3 buttons
    //
    for ( TInt i = 0 ; i < EMPXButtonCount -1 ; i++ )
    {
        CMPXVideoPlaybackButton* button =
            CMPXVideoPlaybackButton::NewL( iController,
                    TRect( leftOffset, iconSize * i, leftOffset + iconSize, iconSize * ( i + 1 ) ),
                    iconsPath );

        CleanupStack::PushL( button );
        iButtons.AppendL( button );
        CleanupStack::Pop( button );
    }

    //
    // create pause button in same rect as paly button's
    //
    CMPXVideoPlaybackButton* button =
        CMPXVideoPlaybackButton::NewL( iController, iButtons[EMPXButtonPlay]->Rect(), iconsPath );

    CleanupStack::PushL( button );
    iButtons.AppendL( button );
    CleanupStack::Pop( button );

    //
    // Set touchable rect for top, middle, bottom button
    //
    iTopButtonRect =  iButtons[EMPXButtonFastForward]->Rect();
    iMiddleButtonRect = iButtons[EMPXButtonPlay]->Rect();
    iBottomButtonRect =  iButtons[EMPXButtonRewind]->Rect();

    SkinChangeL();

    UpdateStateOnButtonBar( iController->State() );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::SkinChangeL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::SkinChangeL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::SkinChangeL()"));

    //
    // FF button
    //
    iButtons[EMPXButtonFastForward]->CreateDefaultButtonL(
            KAknsIIDQgnPropImageTbNext,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_next,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_next_mask );

    iButtons[EMPXButtonFastForward]->CreateDimmedButtonL(
            KAknsIIDQgnPropImageTbNextDimmed,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_next_dimmed,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_next_dimmed_mask );

    iButtons[EMPXButtonFastForward]->CreatePressedButtonL(
            KAknsIIDQgnPropImageTbNextFocused,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_next_focused,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_next_focused_mask );

    //
    // Play button
    //
    iButtons[EMPXButtonPlay]->CreateDefaultButtonL(
            KAknsIIDQgnPropImageTbPlay2,
            EMbmMpxvideoplaybackcontrolsQgn_prop_image_tb_play2,
            EMbmMpxvideoplaybackcontrolsQgn_prop_image_tb_play2_mask );

    //
    // Pause button
    //
    iButtons[EMPXButtonPause]->CreateDefaultButtonL(
            KAknsIIDQgnPropImageTbPause,
            EMbmMpxvideoplaybackcontrolsQgn_prop_image_tb_pause,
            EMbmMpxvideoplaybackcontrolsQgn_prop_image_tb_pause_mask );

    iButtons[EMPXButtonPause]->CreateDimmedButtonL(
            KAknsIIDQgnPropImageTbPauseDimmed,
            EMbmMpxvideoplaybackcontrolsQgn_prop_image_tb_pause_dimmed,
            EMbmMpxvideoplaybackcontrolsQgn_prop_image_tb_pause_dimmed_mask );

    //
    // RW button
    //
    iButtons[EMPXButtonRewind]->CreateDefaultButtonL(
            KAknsIIDQgnPropImageTbPrev,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_prev,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_prev_mask );

    iButtons[EMPXButtonRewind]->CreateDimmedButtonL(
            KAknsIIDQgnPropImageTbPrevDimmed,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_prev_dimmed,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_prev_dimmed_mask );

    iButtons[EMPXButtonRewind]->CreatePressedButtonL(
            KAknsIIDQgnPropImageTbPrevFocused,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_prev_focused,
            EMbmMpxvideoplaybackcontrolsQgn_prop_fusionplayer_tb_prev_focused_mask );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::HandleResourceChange()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::HandleResourceChange( TInt aType )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackProgressBar::HandleResourceChange(0x%X)"), aType);

    if ( aType == KAknsMessageSkinChange )
    {
        TRAP_IGNORE( SkinChangeL() );
    }

    CCoeControl::HandleResourceChange( aType );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::SetContainerWindowL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::SetContainerWindowL( const CCoeControl& aContainer )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::SetContainerWindowL()"));

    for ( TInt i = 0 ; i < EMPXButtonCount ; i++ )
    {
        iButtons[i]->SetContainerWindowL( aContainer );
    }

    CCoeControl::SetContainerWindowL( aContainer );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::HandlePointerEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::HandlePointerEventL()"));

    switch ( aPointerEvent.iType )
    {
        case TPointerEvent::EButton1Down:
        {
            HandleButtonDownEventL( aPointerEvent );

            break;
        }
        case TPointerEvent::EButton1Up:
        {
            if ( iButtonPressed == EMPXTopButtonPressed ||
                 ( iTopButtonRect.Contains( aPointerEvent.iPosition ) &&
                   iButtonPressed == EMPXNotPressed ) )
            {
                HandleTopButtonUpEventL( aPointerEvent );
            }
            else if ( iButtonPressed == EMPXBottomButtonPressed ||
                      ( iBottomButtonRect.Contains( aPointerEvent.iPosition ) &&
                        iButtonPressed == EMPXNotPressed ) )
            {
                HandleBottomButtonUpEventL( aPointerEvent );
            }
            else if ( iButtonPressed == EMPXMiddleButtonPressed ||
                      ( iMiddleButtonRect.Contains( aPointerEvent.iPosition ) &&
                        iButtonPressed == EMPXNotPressed ) )
            {
                HandleMiddleButtonUpEventL();
            }
            else
            {
                //Pass an event to controller to call toggle visibility
            }

            iButtonPressed = EMPXNotPressed;
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::HandleButtonDownEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::HandleButtonDownEventL( const TPointerEvent& aPointerEvent )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::HandleButtonDownEventL()"));

    if ( iController->IsTvOutPlaybackAllowed() )
    {
        if ( iSeekingTimer->IsActive() )
        {
            iSeekingTimer->Cancel();
        }

        if ( iMiddleButtonRect.Contains( aPointerEvent.iPosition ) )
        {
            iButtonPressed = EMPXMiddleButtonPressed;
        }
        else if ( iTopButtonRect.Contains( aPointerEvent.iPosition ) &&
                  iButtons[EMPXButtonFastForward]->IsVisible() &&
                  ! iButtons[EMPXButtonFastForward]->IsDimmed() )
        {
            iButtons[EMPXButtonFastForward]->SetPressed( ETrue );

            iSeekingTimer->Start(
                KMPXLongPressInterval,
                KMPXTactileInterval,
                TCallBack( CMPXVideoPlaybackButtonBar::StartSeekingL, this ) );

            iButtonPressed = EMPXTopButtonPressed;
        }
        else if( iBottomButtonRect.Contains( aPointerEvent.iPosition ) &&
                 iButtons[EMPXButtonRewind]->IsVisible() &&
                 ! iButtons[EMPXButtonRewind]->IsDimmed() )
        {
            iButtons[EMPXButtonRewind]->SetPressed( ETrue );

            iSeekingTimer->Start(
                KMPXLongPressInterval,
                KMPXTactileInterval,
                TCallBack( CMPXVideoPlaybackButtonBar::StartSeekingL, this ) );

            iButtonPressed = EMPXBottomButtonPressed;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::HandleTopButtonUpEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::HandleTopButtonUpEventL( const TPointerEvent& aPointerEvent )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::HandleTopButtonUpEventL()"));

    if ( iSeekingTimer->IsActive() )
    {
        iSeekingTimer->Cancel();
    }

    if ( iButtons[EMPXButtonFastForward]->IsVisible() &&
         ! iButtons[EMPXButtonFastForward]->IsDimmed() )
    {
        iButtons[EMPXButtonFastForward]->SetPressed( EFalse );

        if ( iSeeking )
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

            iController->HandleCommandL( EMPXPbvCmdEndSeek );
            iSeeking = EFalse;
        }
        // short press
        else
        {
            iController->HandleCommandL( EMPXPbvCmdShortPressForward );
        }

    }

    iButtons[EMPXButtonFastForward]->HandlePointerEventL( aPointerEvent );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::HandleMiddleButtonUpEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::HandleMiddleButtonUpEventL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::HandleMiddleButtonUpEventL()"));

    switch ( iController->State() )
    {
        case EPbStatePlaying:
        {
            if ( iButtons[EMPXButtonPause]->IsVisible() && ! iButtons[EMPXButtonPause]->IsDimmed() )
            {
#ifdef RD_TACTILE_FEEDBACK
                if (iFeedback)
                {
#ifdef SYMBIAN_BUILD_GCE
                    iFeedback->InstantFeedback( ETouchFeedbackBasicButton );
#else
                    iFeedback->InstantFeedback( ETouchFeedbackBasic );
#endif //SYMBIAN_BUILD_GCE                    
                }
#endif //RD_TACTILE_FEEDBACK
                iController->HandleCommandL( EMPXPbvCmdPause );
            }
            break;
        }
        case EPbStatePaused:
        case EPbStateStopped:
        {
            if ( iButtons[EMPXButtonPlay]->IsVisible() && ! iButtons[EMPXButtonPlay]->IsDimmed() )
            {
#ifdef RD_TACTILE_FEEDBACK
                if (iFeedback)
                {
#ifdef SYMBIAN_BUILD_GCE
                    iFeedback->InstantFeedback( ETouchFeedbackBasicButton );
#else
                    iFeedback->InstantFeedback( ETouchFeedbackBasic );
#endif //SYMBIAN_BUILD_GCE                    
                }
#endif //RD_TACTILE_FEEDBACK
                iController->HandleCommandL( EMPXPbvCmdPlay );
            }
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::HandleBottomButtonUpEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::HandleBottomButtonUpEventL( const TPointerEvent& aPointerEvent )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::HandleBottomButtonUpEventL()"));

    if ( iSeekingTimer->IsActive() )
    {
        iSeekingTimer->Cancel();
    }

    if ( iButtons[EMPXButtonRewind]->IsVisible() && ! iButtons[EMPXButtonRewind]->IsDimmed() )
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

        iButtons[EMPXButtonRewind]->SetPressed( EFalse );

        if ( iSeeking )
        {
            iController->HandleCommandL( EMPXPbvCmdEndSeek );
            iSeeking = EFalse;
        }
        else
        {
			// Short press backward:
            iController->HandleCommandL( EMPXPbvCmdShortPressBackward );
        }
    }

    iButtons[EMPXButtonRewind]->HandlePointerEventL( aPointerEvent );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackButtonBar::StartSeekingL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackButtonBar::StartSeekingL( TAny* aPtr )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::StartSeekingL()"));

    static_cast<CMPXVideoPlaybackButtonBar*>(aPtr)->DoStartSeekingL();

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackButtonBar::DoStartSeekingL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::DoStartSeekingL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::DoStartSeekingL()"));

#ifdef RD_TACTILE_FEEDBACK
    if (iFeedback)
    {
#ifdef SYMBIAN_BUILD_GCE
        iFeedback->InstantFeedback( ETouchFeedbackBasicButton );
#else
        iFeedback->InstantFeedback( ETouchFeedbackSensitive );
#endif //SYMBIAN_BUILD_GCE        
    }
#endif //RD_TACTILE_FEEDBACK

    if ( ! iSeeking )
    {
        if ( iButtonPressed == EMPXTopButtonPressed )
        {
            iController->HandleCommandL( EMPXPbvCmdSeekForward );
        }
        else if ( iButtonPressed == EMPXBottomButtonPressed )
        {
            iController->HandleCommandL( EMPXPbvCmdSeekBackward );
        }
        iSeeking = ETrue;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackButtonBar::Draw()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::Draw( const TRect& aRect ) const
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::Draw()"));

    CWindowGc& gc = SystemGc();
    gc.SetClippingRect( aRect );

    if ( iController->SetBackgroundBlack() )
    {
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
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::CountComponentControls()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackButtonBar::CountComponentControls() const
{
    return EMPXButtonCount;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::ComponentControl
// -------------------------------------------------------------------------------------------------
//
CCoeControl* CMPXVideoPlaybackButtonBar::ComponentControl(TInt aIndex) const
{
    CCoeControl* control = NULL;

    control = iButtons[ aIndex ];

    return control;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::StateChanged()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::UpdateStateOnButtonBar( TMPXPlaybackState aState )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::StateChanged(%d)"), aState);

    switch ( aState )
    {
        case EPbStatePlaying:
        {
            iButtons[EMPXButtonPlay]->MakeVisible( EFalse );
            iButtons[EMPXButtonPause]->MakeVisible( ETrue );

            break;
        }
        case EPbStatePaused:
        {
            iButtons[EMPXButtonPause]->MakeVisible( EFalse );
            iButtons[EMPXButtonPlay]->MakeVisible( ETrue );

            break;
        }
        case EPbStateNotInitialised:
        case EPbStateInitialising:
        {
            iButtons[EMPXButtonFastForward]->MakeVisible( ETrue );
            iButtons[EMPXButtonFastForward]->SetDimmed( ETrue );

            iButtons[EMPXButtonRewind]->MakeVisible( ETrue );
            iButtons[EMPXButtonRewind]->SetDimmed( ETrue );

            iButtons[EMPXButtonPause]->MakeVisible( EFalse );
            iButtons[EMPXButtonPlay]->MakeVisible( ETrue );

            break;
        }
        case EPbStateStopped:
        {
            iButtons[EMPXButtonRewind]->SetDimmed( ETrue );
            iButtons[EMPXButtonFastForward]->SetDimmed( ETrue );
            break;
        }
    }
}


// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::UpdateButtonBarState
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::UpdateButtonBarState( CMPXVideoPlaybackViewFileDetails* aDetails )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackButtonBar::UpdateButtonBarState"));

    if ( aDetails->iSeekable )
    {
        iButtons[EMPXButtonRewind]->SetDimmed( EFalse );
        iButtons[EMPXButtonFastForward]->SetDimmed( EFalse );
    }

    if ( !aDetails->iPausableStream )
    {
        iButtons[EMPXButtonPause]->SetDimmed( ETrue );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackButtonBar::Reset
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackButtonBar::Reset()
{
    if ( iSeeking )
    {
        // Abandon the ongoing seek operation
        TPointerEvent event;
        event.iType = TPointerEvent::EButton1Up;
        MPX_TRAPD( err, HandlePointerEventL(event) );
    }
}


//  End of File
