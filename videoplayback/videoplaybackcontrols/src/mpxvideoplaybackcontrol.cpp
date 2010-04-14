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
* Description:  Implementation of CMPXVideoPlaybackControl
*
*/

// Version : %version: e003sa33#20 %


// INCLUDE FILES
#include <avkon.hrh>
#include <e32std.h>
#include <AknUtils.h>

#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackcontrol.h"
#include "mpxvideoplaybackvolumebar.h"
#include "mpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackaspectratioicon.h"
#include "mpxvideoplaybackbrandinganimation.h"
#include "mpxvideoplaybackuserinputhandler.h"
#include "mpxvideoplaybackcontainer.h"
#include "mpxvideo_debug.h"

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::CMPXVideoPlaybackControl()
// C++ default constructor can NOT contain any code, that
// might leave.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControl::CMPXVideoPlaybackControl(
        CMPXVideoPlaybackControlsController* aController,
        CCoeControl* aControl,
        TMPXVideoPlaybackControls aControlIndex,
        TUint aProperties )
    : iController( aController )
    , iControl( aControl )
    , iControlIndex( aControlIndex )
    , iProperties( aProperties )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControl::ConstructL( TRect aRect )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::ConstructL()"));

    CreateWindowL();
    EnableDragEvents();

    iEikonEnv->EikAppUi()->AddToStackL( this, ECoeStackPriorityDefault );

    SetRect( aRect );

    iControl->SetContainerWindowL( *this );
    iControl->SetRect( Rect() );
    SetComponentsToInheritVisibility( ETrue );

#ifdef RD_TACTILE_FEEDBACK
    if ( iProperties & EMPXSoftkeyControl )
    {
        iFeedback = MTouchFeedback::Instance();
    }
#endif //RD_TACTILE_FEEDBACK

    Window().SetNonFading( ETrue );

    ActivateL();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControl* CMPXVideoPlaybackControl::NewL(
    CMPXVideoPlaybackControlsController* aController,
    CCoeControl* aControl,
    TRect aRect,
    TMPXVideoPlaybackControls aControlIndex,
    TUint aProperties )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::NewL()"));

    CMPXVideoPlaybackControl* self =
        new (ELeave) CMPXVideoPlaybackControl( aController, aControl, aControlIndex, aProperties );

    CleanupStack::PushL(self);
    self->ConstructL( aRect );
    CleanupStack::Pop();

    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::~CMPXVideoPlaybackControlsController()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControl::~CMPXVideoPlaybackControl()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::~CMPXVideoPlaybackControl()"));

    iEikonEnv->EikAppUi()->RemoveFromStack( this );

#ifdef RD_TACTILE_FEEDBACK
    if ( iFeedback )
    {
       iFeedback->RemoveFeedbackForControl( this );
    }
#endif //RD_TACTILE_FEEDBACK

    if ( iControl )
    {
        delete iControl;
        iControl = NULL;
    }

    CloseWindow();
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Gets the number of controls contained in a compound control.
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackControl::CountComponentControls() const
{
    return 1;
}

// -------------------------------------------------------------------------------------------------
// From CCoeControl
// Gets an indexed component of a compound control.
// -------------------------------------------------------------------------------------------------
//
CCoeControl* CMPXVideoPlaybackControl::ComponentControl( TInt /*aIndex*/ ) const
{
    return iControl;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::Draw()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControl::Draw( const TRect& aRect ) const
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::Draw()"));

    CWindowGc& gc = SystemGc();
    gc.SetClippingRect( aRect );

    if ( iController->SetBackgroundBlack() )
    {
        if ( Window().DisplayMode() == EColor16MAP )
        {
            gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
            gc.SetBrushColor( TRgb::Color16MAP( 255 ) );
        }
        else if ( Window().DisplayMode() == EColor16MA )
        {
            gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
            gc.SetBrushColor( TRgb::Color16MA( 0 ) );
        }
    }
    else
    {
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.SetBrushColor( KRgbBlack );
    }

    gc.Clear( aRect );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::HandlePointerEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControl::HandlePointerEventL( const TPointerEvent& aPointerEvent )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::HandlePointerEventL()"));

    iController->iContainer->UserInputHandler()->ProcessPointerEventL( this,
                                                                       aPointerEvent,
                                                                       EMpxVideoPlaybackControl );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::DoHandlePointerEventL()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void CMPXVideoPlaybackControl::DoHandlePointerEventL( const TPointerEvent& aPointerEvent)
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControl::DoHandlePointerEventL()"));

    if ( AknLayoutUtils::PenEnabled() )
    {
        switch( iControlIndex )
        {
            case EMPXProgressBar:
            case EMPXVolumeBar:
            case EMPXButtonBar:
            case EMPXAspectRatioIcon:
            case EMPXMediaDetailsViewer:
            {
                if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                {
                    iController->ResetDisappearingTimers( EMPXTimerCancel );

                    iControl->SetPointerCapture( ETrue );
                    iControl->ClaimPointerGrab( ETrue );
                }
                else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                {
                    iController->ResetDisappearingTimers( EMPXTimerReset );

                    iControl->SetPointerCapture( EFalse );
                    iControl->ClaimPointerGrab( EFalse );
                }

                //
                // Pass an event to controller
                //
                iControl->HandlePointerEventL( aPointerEvent );
                break;
            }
            case EMPXSoftkeyDetails:
            {
                if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                {
#ifdef RD_TACTILE_FEEDBACK
                    if ( iFeedback )
                    {
                        iFeedback->InstantFeedback( ETouchFeedbackBasic );
                    }
#endif //RD_TACTILE_FEEDBACK

                    iController->HandleCommandL( EMPXPbvCmdShowFileDetails );
                }
                break;
            }
            case EMPXSoftkeyBack:
            {
                if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                {
#ifdef RD_TACTILE_FEEDBACK
                    if ( iFeedback )
                    {
                        iFeedback->InstantFeedback( ETouchFeedbackBasic );
                    }
#endif //RD_TACTILE_FEEDBACK

                    iController->HandleCommandL( EAknSoftkeyBack );
                }
                break;
            }
            case EMPXNoVideoBitmap:
            case EMPXRealAudioBitmap:
            case EMPXSoundBitmap:
            case EMPXTitleLabel:
            case EMPXArtistLabel:
            case EMPXBrandingAnimation:
            default:
            {
                //
                //  Ignore pointer events for these controls
                //
                break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::SetVisibility()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControl::SetVisibility( TMPXPlaybackState aState )
{
    TBool visible = EFalse;

    switch( aState )
    {
        case EPbStatePlaying:
        {
            if ( iProperties & EMPXShownWhenPlaying )
            {
                visible = ETrue;
            }
            break;
        }
        case EPbStatePaused:
        {
            if ( iProperties & EMPXShownWhenPaused )
            {
                visible = ETrue;
            }
            break;
        }
        case EPbStatePluginSeeking:
        {
            if ( iProperties & EMPXShownWhenSeeking )
            {
                visible = ETrue;
            }
            break;
        }
        case EPbStateBuffering:
        {
            if ( iProperties & EMPXBufferingControl )
            {
                visible = ETrue;
            }
            break;
        }
        case EPbStateStopped:
        {
            if ( iProperties & EMPXShownWhenStopped )
            {
                visible = ETrue;
            }
            break;
        }
        case EPbStateNotInitialised:
        case EPbStateInitialising:
        {
            if ( iProperties & EMPXShownWhenInitializing )
            {
                visible = ETrue;
            }
            break;
        }
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackControl::SetVisibility(%d)"), visible);

    MakeVisible( visible );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::ControlIndex()
// -------------------------------------------------------------------------------------------------
//
TMPXVideoPlaybackControls CMPXVideoPlaybackControl::ControlIndex()
{
    return iControlIndex;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::VolumeChanged()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControl::VolumeChanged( TInt aVolume )
{
    TBool changed = EFalse;

    if ( iControlIndex == EMPXVolumeBar )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::VolumeChanged() [%d]"), aVolume);

        static_cast<CMPXVideoPlaybackVolumeBar*>(iControl)->VolumeChanged( aVolume );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::DurationChangedL()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControl::DurationChangedL( TInt aDuration )
{
    TBool changed = EFalse;

    if ( iControlIndex == EMPXProgressBar )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::DurationChangedL() [%d]"), aDuration);

        static_cast<CMPXVideoPlaybackProgressBar*>(iControl)->DurationChangedL( aDuration );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::PositionChangedL()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControl::PositionChangedL( TInt aPosition )
{
    TBool changed = EFalse;

    if ( iControlIndex == EMPXProgressBar )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::PositionChangedL() [%d]"), aPosition);

        static_cast<CMPXVideoPlaybackProgressBar*>(iControl)->PositionChangedL( aPosition );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::AspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControl::AspectRatioChanged( TInt aAspectRatio )
{
    TBool changed = EFalse;

    if ( iControlIndex == EMPXAspectRatioIcon )
    {
        MPX_DEBUG(
            _L("CMPXVideoPlaybackControlsController::AspectRatioChanged() [%d]"), aAspectRatio);

        static_cast<CMPXVideoPlaybackAspectRatioIcon*>(iControl)->
            AspectRatioChanged( aAspectRatio );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControl::SetDownloadSize()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControl::SetDownloadSize( TInt aSize )
{
    TBool changed = EFalse;

    if ( iControlIndex == EMPXProgressBar )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::SetDownloadSize() [%d]"), aSize);

        static_cast<CMPXVideoPlaybackProgressBar*>(iControl)->SetDownloadSize( aSize );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControl::UpdateDownloadPosition()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControl::UpdateDownloadPosition( TInt aSize )
{
    TBool changed = EFalse;

    if ( iControlIndex == EMPXProgressBar )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::UpdateDownloadPosition() [%d]"), aSize);

        static_cast<CMPXVideoPlaybackProgressBar*>(iControl)->UpdateDownloadPosition( aSize );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::UpdateStateOnButtonBar()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControl::UpdateStateOnButtonBar( TMPXPlaybackState aState )
{
    TBool changed = EFalse;

    if ( iControlIndex == EMPXButtonBar )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::UpdateStateOnButtonBar() [%d]"), aState);

        static_cast<CMPXVideoPlaybackButtonBar*>(iControl)->UpdateStateOnButtonBar( aState );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::UpdateControlsWithFileDetailsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControl::UpdateControlsWithFileDetailsL(
        CMPXVideoPlaybackViewFileDetails* aDetails )
{
    iPlaybackMode = aDetails->iPlaybackMode;

    //
    //  Controls should be shown when if there is no video or the TV-Out is connected
    //
    if ( iControlIndex == EMPXButtonBar )
    {
        static_cast<CMPXVideoPlaybackButtonBar*>(iControl)->UpdateButtonBarState( aDetails );
    }
    else if ( iControlIndex == EMPXProgressBar )
    {
        static_cast<CMPXVideoPlaybackProgressBar*>(iControl)->UpdateProgressBarStateL( aDetails );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::StopBrandingAnimationTimer()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControl::StopBrandingAnimationTimer()
{
    TBool changed = EFalse;

    if ( iControlIndex == EMPXBrandingAnimation )
    {
        MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::StopBrandingAnimationTimer()"));

        static_cast<CMPXVideoPlaybackBrandingAnimation*>(iControl)->CancelBrandingTimer();

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControl::ResetControl()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControl::ResetControl()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::ResetControl()"));

    iControl->SetPointerCapture( EFalse );
    iControl->ClaimPointerGrab( EFalse );
    
    switch( iControlIndex )
    {
        case EMPXButtonBar:
        {
            static_cast<CMPXVideoPlaybackButtonBar*>(iControl)->Reset();
            break;
        }
        case EMPXProgressBar:
        {
            static_cast<CMPXVideoPlaybackProgressBar*>(iControl)->Reset();
            break;
        }
        case EMPXVolumeBar:
        {
             static_cast<CMPXVideoPlaybackVolumeBar*>(iControl)->Reset();
             break;
        }
        case EMPXAspectRatioIcon:
        default:
            break;
    }
}

// End of File
