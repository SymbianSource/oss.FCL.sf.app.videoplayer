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
* Description:  Implementation of MPXVideoPlaybackControlsController
*
*/


// Version : %version: 45 %


// INCLUDE FILES
#include <fbs.h>
#include <coecntrl.h>
#include <gulicon.h>
#include <bautils.h>
#include <eiklabel.h>
#include <eikimage.h>
#include <AknUtils.h>
#include <barsread.h>
#include <AknIconUtils.h>
#include <StringLoader.h>
#include <AknsDrawUtils.h>
#include <AknsSkinInstance.h>
#include <AknBitmapAnimation.h>
#include <AknsBasicBackgroundControlContext.h>
#include <f32file.h>

#include <aknlayoutscalable_apps.cdl.h>
#include <data_caging_path_literals.hrh>

#include <mpxvideoplaybackcontrols.rsg>
#include <mpxvideoplaybackcontrols.mbg>

#include <avkondomainpskeys.h>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK

#include "mpxvideoplaybackcontrol.h"
#include "mpxvideoplaybackcontrolpdl.h"
#include "mpxvideoplaybackbuttonbar.h"
#include "mpxvideoplaybackvolumebar.h"
#include "mpxvideoplaybackprogressbar.h"
#include "mpxvideoplaybackcontainer.h"
#include "mpxvideoplaybackcontrolpolicy.h"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackcontrolscontroller.h"
#include "mpxvideoplaybackcontrolconfiguration.h"
#include "mpxvideoplaybackaspectratioicon.h"
#include "mpxvideoplaybackbrandinganimation.h"
#include "mpxvideoplaybackmediadetailsviewer.h"
#include "mpxcommonvideoplaybackview.hrh"

using namespace AknLayoutScalable_Apps;
using namespace AknLayoutScalable_Avkon;

const TInt KMPXControlsTimeOut = 4000000;

const TInt KMP4LayoutSet = 6;

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::CMPXVideoPlaybackControlsController
// C++ default constructor can NOT contain any code, that might leave.
// -------------------------------------------------------------------------------------------------
//
CMPXVideoPlaybackControlsController::CMPXVideoPlaybackControlsController(
    CMPXVideoPlaybackContainer* aContainer, TRect aRect )
    : iControls( EMPXControlsCount ),
      iRect( aRect ),
      iContainer( aContainer )
{
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::ConstructL()
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::ConstructL( CMPXVideoPlaybackViewFileDetails* aDetails )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::ConstructL()"));

    User::LeaveIfError( iFs.Connect() );

    _LIT( KMPXVideoPlaybackRscPath, "mpxvideoplaybackcontrols.rsc" );

    TParse parse;
    parse.Set( KMPXVideoPlaybackRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile = parse.FullName();

    //
    //  Find the resource file.  It must be on the same drive as the DLL is installed
    //
    User::LeaveIfError( AddDllPath( resourceFile ) );

    BaflUtils::NearestLanguageFile( iFs, resourceFile );

    CCoeEnv* coeEnv = CCoeEnv::Static();

    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::ConstructL() rsc file %S"), &resourceFile);

    iVideoResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    iFileDetails = aDetails;
    iTvOutConnected = iFileDetails->iTvOutConnected;

    iRNFormat = IsRealFormatL( iFileDetails->iClipName->Des() );

    if ( IsRealOneBitmapVisible() )
    {
        iShowControls  = EFalse;
    }
    else
    {
        iShowControls  = ETrue;
    }

    iControlsPolicy = CMPXVideoPlaybackControlPolicy::NewL();
    iControlsConfig = CMPXVideoPlaybackControlConfiguration::NewL( this );

    iControlsTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    CreateControlsL();

#ifdef RD_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif //RD_TACTILE_FEEDBACK
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::NewL()
// Two-phased constructor.
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMPXVideoPlaybackControlsController* CMPXVideoPlaybackControlsController::NewL(
        CMPXVideoPlaybackContainer* aContainer,
        TRect aRect,
        CMPXVideoPlaybackViewFileDetails* aDetails )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::NewL()"));

    CMPXVideoPlaybackControlsController* self =
        new (ELeave) CMPXVideoPlaybackControlsController( aContainer, aRect );

    CleanupStack::PushL(self);
    self->ConstructL( aDetails );
    CleanupStack::Pop();

    return self;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::~CMPXVideoPlaybackControlsController
// Destructor.
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CMPXVideoPlaybackControlsController::~CMPXVideoPlaybackControlsController()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::~CMPXVideoPlaybackControlsController"));

    iFs.Close();

#ifdef RD_TACTILE_FEEDBACK
    if ( iFeedback )
    {
        iFeedback->RemoveFeedbackForControl( iContainer );
    }
#endif //RD_TACTILE_FEEDBACK

    if ( iControlsPolicy )
    {
        delete iControlsPolicy;
        iControlsPolicy = NULL;
    }

    if ( iControlsConfig )
    {
        delete iControlsConfig;
        iControlsConfig = NULL;
    }

    if ( iControlsTimer )
    {
        iControlsTimer->Cancel();
        delete iControlsTimer;
        iControlsTimer = NULL;
    }

    iControls.ResetAndDestroy();

    if ( iRealOneBitmap )
    {
        SetRealOneBitmapVisibility( EFalse );
        delete iRealOneBitmap;
        iRealOneBitmap = NULL ;
    }

    if ( iVideoResourceOffset )
    {
        (CCoeEnv::Static())->DeleteResourceFile( iVideoResourceOffset );
    }

    if ( iMediaDetailsViewerControl )
    {
        delete iMediaDetailsViewerControl;
        iMediaDetailsViewerControl = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::AddFileDetailsL()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void CMPXVideoPlaybackControlsController::AddFileDetailsL(
    CMPXVideoPlaybackViewFileDetails* aDetails )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::AddFileDetailsL()"));

    iFileDetails = aDetails;

    iControlsConfig->UpdateControlsWithFileDetailsL();

    ControlsListUpdatedL();

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        iControls[i]->UpdateControlsWithFileDetailsL( iFileDetails );
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::HandleEventL
// -------------------------------------------------------------------------------------------------
//
EXPORT_C void CMPXVideoPlaybackControlsController::HandleEventL(
    TMPXVideoPlaybackControlCommandIds aEvent, TInt aValue )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::HandleEventL(%d)"), aEvent);

    switch ( aEvent )
    {
        case EMPXControlCmdSetPosition:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdSetPosition [%d]]"), aValue );

            PositionChangedL( (TReal)aValue / (TReal)KPbMilliMultiplier);
            break;
        }
        case EMPXControlCmdSetDuration:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdSetDuration [%d]]"), aValue );

            DurationChangedL( (TReal)aValue / (TReal)KPbMilliMultiplier );
            break;
        }
        case EMPXControlCmdStateChanged:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdStateChanged]"));

            HandleStateChange( (TMPXPlaybackState)aValue );
            break;
        }
        case EMPXControlCmdToggleVisibility:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdToggleVisibility]"));

            ToggleVisibility();
            break;
        }
        case EMPXControlCmdSetVolume:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdSetVolume [%d]]"), aValue );
            VolumeChanged( aValue );
            break;
        }
        case EMPXControlCmdSetAspectRatio:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdSetAspectRatio %d]"), aValue);
            AspectRatioChanged( aValue );
            break;
        }
        case EMPXControlCmdSetDownloadSize:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdSetDownloadSize [%d]]"), aValue );
            SetDownloadSize( aValue );
            break;
        }
        case EMPXControlCmdDownloadUpdated:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdDownloadUpdated [%d]]"), aValue );
            UpdateDownloadPosition( aValue );
            break;
        }
        case EMPXControlCmdDownloadComplete:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdDownloadComplete [%d]]"), aValue);
            UpdateDownloadPosition( aValue );
            break;
        }
        case EMPXControlCmdSetDownloadPaused:
        case EMPXControlCmdClearDownloadPaused:
        {
            iControlsConfig->UpdateControlListL( aEvent );
            ControlsListUpdatedL();
            break;
        }
        case EMPXControlCmdTvOutConnected:
        {
            iTvOutConnected = ETrue;

            //
            //  Do not show the aspect ratio icon when TV Out is connected
            //
            HandleTvOutEventL( ETrue, aEvent, EFalse );
            break;
        }
        case EMPXControlCmdTvOutDisconnected:
        {
            iTvOutConnected = EFalse;

            //
            //  Pass in the show aspect ratio flag from the event
            //
            HandleTvOutEventL( EFalse, aEvent, aValue );
            break;
        }
        case EMPXControlCmdHandleBackgroundEvent:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdHandleBackgroundEvent]"));

            TBool keylock( EFalse );
            RProperty::Get( KPSUidAvkonDomain, KAknKeyguardStatus, keylock );

            //
            //  Don't show controls when key is locked
            //
            if ( keylock )
            {
                iShowControls = EFalse;
            }

            ResetDisappearingTimers( EMPXTimerReset );

            for ( TInt i = 0 ; i < iControls.Count() ; i++ )
            {
                iControls[i]->ResetControl();
            }

            break;
        }
        case EMPXControlCmdHandleForegroundEvent:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdHandleForegroundEvent]"));

            if ( ! IsRealOneBitmapVisible() )
            {
                iShowControls = ETrue;
                UpdateControlsVisibility();
            }

            break;
        }
        case EMPXControlCmdHandleErrors:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdHandleErrors]"));
            HandleErrors();
            break;
        }
        case EMPXControlCmdShowControls:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdShowControls]"));
            ShowControls();
            break;
        }
        case EMPXControlCmdHideControls:
        {
            MPX_DEBUG(_L("    [EMPXControlCmdHideControls]"));
            HideAllControls();
            break;
        }
        case EMPXControlCmdSoftKeyPressed:
        {
            HandleSoftKeyPressedL( aValue );
            break;
        }
        case EMPXControlCmdShowFileDetailsViewer:
        {
            ShowMediaDetailsViewerL();
            break;
        }
        case EMPXControlCmdCloseFileDetailsViewer:
        {
            ShowControls();
            break;
        }
        case EMPXControlCmdSurfaceCreated:
        {
            SetRealOneBitmapVisibility( EFalse );
            break;
        }
        case EMPXControlCmdLoadingStarted:
        {
            HandleLoadingStarted();
            break;
        }
        case EMPXControlCmdCreateAspectRatioIcon:
        case EMPXControlCmdDeleteAspectRatioIcon:
        {
            //
            //  Ignore the Aspect Ration commands for non touch devices and
            //  when the TV-Out is connected.
            //
            if ( AknLayoutUtils::PenEnabled() && ! iTvOutConnected )
            {
                iControlsConfig->UpdateControlListL( aEvent );

                //
                //  Set the update visibility flag based on the visibility of the controls
                //
                ControlsListUpdatedL( IsVisible() );
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::HandleStateChange
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::HandleStateChange( TMPXPlaybackState aNewState )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::HandleStateChange()"),
                   _L("new state = %d"), aNewState );

    //
    // Somehow EPbStatePlaying, EPbStatePaused gets called twice continously
    //
    if ( aNewState != iState )
    {
        TBool hideControls( EFalse );

        iState = aNewState;

        switch ( aNewState )
        {
            case EPbStateInitialised:
            {
                //
                //  remove branding control when the state is transitioned to Initialized
                //
                if ( iFileDetails->iPlaybackMode == EMPXVideoStreaming ||
                     iFileDetails->iPlaybackMode == EMPXVideoLiveStreaming )
                {
                    TRAP_IGNORE(
                        iControlsConfig->UpdateControlListL( EMPXControlCmdPluginInitialized );
                        ControlsListUpdatedL();
                        );
                }

                break;
            }
            case EPbStatePlaying:
            {
                if ( iFileDetails->iVideoEnabled && ! IsTvOutConnected() )
                {
                    //
                    //  Hide controls when transitioning to Playing State
                    //
                    hideControls = ETrue;
                }

                //
                //  Intentionally fall through
                //
            }
            case EPbStateBuffering:
            case EPbStateNotInitialised:
            case EPbStateInitialising:
            case EPbStatePaused:
            case EPbStatePluginSeeking:
            {
                UpdateStateOnButtonBar();

                //
                //  Show all the controls
                //
                if ( hideControls )
                {
                    HideAllControls();
                }
                else
                {
                    UpdateControlsVisibility();
                }

                break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::CreateControlsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::CreateControlsL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::CreateControlsL()"));

    //
    //  Clean up old controls list and controls
    //
    iControls.ResetAndDestroy();

    RArray<TMPXVideoPlaybackControls>& controlList = iControlsConfig->ControlList();

    //
    //  Iterate thru the controls list from view-config
    //  and create controls that match items in the list
    //
    for ( TInt i = 0 ; i < controlList.Count() ; i++ )
    {
        AppendControlL( controlList[i] );
    }

    UpdateControlsVisibility();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::CreateRealOneBitmapL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::CreateRealOneBitmapL()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::CreateRealOneBitmapL()"));

    TFileName iconsPath;
    LocateBitmapFileL( iconsPath );

    iRealOneBitmap = CreateBitmapL( iconsPath,
                                    EMbmMpxvideoplaybackcontrolsQgn_graf_realplayer_splash_01,
                                    main_mp4_pane_g1(0).LayoutLine() );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::GetBitmap()
// -------------------------------------------------------------------------------------------------
//
EXPORT_C CEikImage*
CMPXVideoPlaybackControlsController::GetBitmap( TMPXVideoPlaybackControls aBitmap )
{
    CEikImage* bitmap = NULL;

    switch( aBitmap )
    {
        case EMPXRealLogoBitmap:
        {
            bitmap = iRealOneBitmap;
            break;
        }
    }

    return bitmap;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::ControlsListUpdatedL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::ControlsListUpdatedL( TBool aUpdateVisibility )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::ControlsListUpdatedL()"),
                   _L("aUpdateVisibility = %d"), aUpdateVisibility );

    RArray<TMPXVideoPlaybackControls>& updatedList = iControlsConfig->ControlList();

    //
    //  Create a copy of the control list
    //
    TMPXVideoPlaybackControls* data = (TMPXVideoPlaybackControls*)
        User::AllocL( updatedList.Count() * sizeof( TMPXVideoPlaybackControls ) );

    Mem::Copy( data, &(updatedList[0]), updatedList.Count()* sizeof(TMPXVideoPlaybackControls));

    RArray<TMPXVideoPlaybackControls> myList( sizeof(TMPXVideoPlaybackControls),
                                              data,
                                              updatedList.Count() );

    CleanupClosePushL( myList );

    TInt controlCount = iControls.Count();

    TInt i = 0 ;
    TInt index = KErrNotFound;


    for ( TInt iCnt = 0 ; iCnt < controlCount ; iCnt++ )
    {
        index = myList.Find( iControls[i]->ControlIndex() );

        if ( index == KErrNotFound )
        {
            //
            //  Delete control since it doesn't exist in new list
            //
            delete iControls[i];
            iControls.Delete( i );
        }
        else
        {
            //
            //  Control exists in new list.
            //  Remove it from the new list.
            //
            myList.Remove( index );
            i++;
        }
    }

    //
    //  The updated list will contain added controls only
    //
    for ( TInt j = 0 ; j < myList.Count() ; j++ )
    {
        AppendControlL( myList[j] );
    }

    if ( aUpdateVisibility )
    {
        UpdateControlsVisibility();
    }

    CleanupStack::PopAndDestroy();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::AppendControlL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::AppendControlL( TMPXVideoPlaybackControls aControlIndex )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::AppendControlL()"),
                   _L("control index = %d"), aControlIndex );

    TUint properties = 0;

    iControlsPolicy->SetControlProperties( aControlIndex, properties, iFileDetails );

    TFileName iconsPath;
    LocateBitmapFileL( iconsPath );

    switch ( aControlIndex )
    {
        case EMPXSoftkeyBack:
        {
            CreateFakeSoftKeyL( R_MPX_VIDEO_PLAYBACK_CONTROLS_FAKE_SK_BACK,
                                ETrue,
                                aControlIndex,
                                properties );
            break;
        }
        case EMPXButtonBar:
        {
            TAknLayoutRect buttonBarPaneRect;
            buttonBarPaneRect.LayoutRect(
                iRect,
                AknLayoutScalable_Avkon::area_side_right_pane( 0 ).LayoutLine() );

            TAknLayoutRect buttonBarRect;
            buttonBarRect.LayoutRect( buttonBarPaneRect.Rect(),
                                      grid_sctrl_middle_pane( 0 ).LayoutLine() );

            TRect btBarRect( buttonBarRect.Rect() );
            btBarRect.iBr.iY -= 25;
            btBarRect.iTl.iY += 15;
            CMPXVideoPlaybackButtonBar* buttonBar =
                CMPXVideoPlaybackButtonBar::NewL( this, btBarRect );

            CleanupStack::PushL( buttonBar );

            CMPXVideoPlaybackControl* control =
                CMPXVideoPlaybackControl::NewL( this,
                                                buttonBar,
                                                btBarRect,
                                                aControlIndex,
                                                properties );

            CleanupStack::PushL( control );

            iControls.AppendL( control );

            CleanupStack::Pop( 2 ); // buttonBar, control

            break;
        }
        case EMPXVolumeBar:
        {
            TAknLayoutRect volumeLayoutRect;
            volumeLayoutRect.LayoutRect( iRect, popup_slider_window_cp02().LayoutLine() );
            TRect rect = volumeLayoutRect.Rect();

            CMPXVideoPlaybackVolumeBar* volBar = CMPXVideoPlaybackVolumeBar::NewL( this, rect );

            CleanupStack::PushL( volBar );

            CMPXVideoPlaybackControl* control = CMPXVideoPlaybackControl::NewL( this,
                                                                                volBar,
                                                                                rect,
                                                                                aControlIndex,
                                                                                properties );

            CleanupStack::PushL( control );

            iControls.AppendL( control );

            CleanupStack::Pop( 2 ); // volBar, control

            break;
        }
        case EMPXProgressBar:
        {
            TAknLayoutRect progressPaneRect;
            progressPaneRect.LayoutRect( iRect, mp4_progress_pane( KMP4LayoutSet ).LayoutLine() );

            TRect touchPaneRect( progressPaneRect.Rect().iTl.iX,
                                 progressPaneRect.Rect().iTl.iY,
                                 progressPaneRect.Rect().iBr.iX,
                                 iRect.Height() );

            CMPXVideoPlaybackProgressBar* progBar =
                CMPXVideoPlaybackProgressBar::NewL( this, touchPaneRect );

            CleanupStack::PushL( progBar );

            CMPXVideoPlaybackControl* control = CMPXVideoPlaybackControl::NewL( this,
                                                                                progBar,
                                                                                touchPaneRect,
                                                                                aControlIndex,
                                                                                properties );

            CleanupStack::PushL( control );

            iControls.AppendL( control );

            CleanupStack::Pop( 2 ); // progBar, control

            break;
        }
        case EMPXSoftkeyDetails:
        {
            CreateFakeSoftKeyL( R_MPX_VIDEO_PLAYBACK_CONTROLS_FAKE_SK_DETAILS,
                                EFalse,
                                aControlIndex,
                                properties );
            break;
        }
        case EMPXBufferingLabel:
        {
            CreateTextLabelWithSpecificLayoutL( R_MPX_LOADING,
                                                aControlIndex,
                                                properties,
                                                main_mp4_pane_t3( KMP4LayoutSet ) );
            break;
        }
        case EMPXTitleLabel:
        {
            CreateTitleControlL( aControlIndex, properties );

            break;
        }
        case EMPXArtistLabel:
        {
            if ( iFileDetails->iArtist && iFileDetails->iArtist->Length() )
            {
                CEikLabel* label = CreateTextLabelL( *(iFileDetails->iArtist) );

                CleanupStack::PushL( label );

                AknLayoutUtils::LayoutLabel( label,
                                             iRect,
                                             main_mp3_pane_t3().LayoutLine());

                label->OverrideColorL( EColorLabelText, KRgbWhite );
                label->OverrideColorL( EColorLabelTextEmphasis, KRgbWhite );

                CMPXVideoPlaybackControl* control =
                    CMPXVideoPlaybackControl::NewL( this,
                                                    label,
                                                    label->Rect(),
                                                    aControlIndex,
                                                    properties );

                CleanupStack::PushL( control );

                iControls.AppendL( control );

                CleanupStack::Pop( 2 ); // label, control
            }
            break;
        }
        case EMPXRealAudioBitmap:
        {
            CreateAudioOnlyBitmapL( iconsPath,
                                    EMbmMpxvideoplaybackcontrolsQgn_menu_rp,
                                    EMbmMpxvideoplaybackcontrolsQgn_menu_rp_mask,
                                    aControlIndex,
                                    main_mp3_pane_g3(0).LayoutLine(),
                                    properties );

            break;
        }
        case EMPXSoundBitmap:
        {
            CreateAudioOnlyBitmapL( iconsPath,
                                    EMbmMpxvideoplaybackcontrolsQgn_indi_onimage_audio,
                                    EMbmMpxvideoplaybackcontrolsQgn_indi_onimage_audio_mask,
                                    aControlIndex,
                                    main_mp3_pane_g4(0).LayoutLine(),
                                    properties );

            break;
        }
        case EMPXNoVideoBitmap:
        {
            CreateAudioOnlyBitmapL( iconsPath,
                                    EMbmMpxvideoplaybackcontrolsQgn_prop_video_corrupted,
                                    EMbmMpxvideoplaybackcontrolsQgn_prop_video_corrupted_mask,
                                    aControlIndex,
                                    main_mp3_pane_g4(0).LayoutLine(),
                                    properties );

            break;
        }
        case EMPXDownloadPausedIcon:
        {
            //
            //  Not added to the control list since it will always be visible
            //
            CEikImage* dlPausedBitmap =
                CreateImageL( iconsPath,
                              EMbmMpxvideoplaybackcontrolsQgn_indi_mp_ongoing_interrupt,
                              EMbmMpxvideoplaybackcontrolsQgn_indi_mp_ongoing_interrupt_mask );

            CleanupStack::PushL( dlPausedBitmap );

            AknLayoutUtils::LayoutImage( dlPausedBitmap,
                                         iRect,
                                         AknLayoutScalable_Apps::main_mp4_pane_g6( KMP4LayoutSet ) );

            TRect ctrlRect = dlPausedBitmap->Rect();

            //
            //  To make it aligned with AspectRatioIcon when touch is supported.
            //
            if ( AknLayoutUtils::PenEnabled() )
            {
                ctrlRect.iBr.iY += 10;
                ctrlRect.iTl.iY += 10;
            }

            CMPXVideoPlaybackControl* control =
                CMPXVideoPlaybackControlPdl::NewL( this,
                                                   dlPausedBitmap,
                                                   ctrlRect,
                                                   aControlIndex,
                                                   properties );

            CleanupStack::PushL( control );

            iControls.AppendL( control );

            CleanupStack::Pop( 2 ); // dlPausedBitmap, control

            break;
        }
        case EMPXAspectRatioIcon:
        {

            TAknLayoutRect aspectRatioPaneRect;
            aspectRatioPaneRect.LayoutRect( iRect,
                                            main_mp4_pane_g5( KMP4LayoutSet ).LayoutLine() );

            //
            // Set the touchable area for aspect ratio
            //
            TRect touchableArea = TRect( aspectRatioPaneRect.Rect().iTl.iX - 25,
                                         aspectRatioPaneRect.Rect().iTl.iY - 10,
                                         aspectRatioPaneRect.Rect().iBr.iX + 15,
                                         aspectRatioPaneRect.Rect().iBr.iY + 30 );

            CMPXVideoPlaybackAspectRatioIcon* aspectRatioIcon =
                CMPXVideoPlaybackAspectRatioIcon::NewL( this, touchableArea );

            CleanupStack::PushL( aspectRatioIcon );

            CMPXVideoPlaybackControl* control =
                CMPXVideoPlaybackControl::NewL( this,
                                                aspectRatioIcon,
                                                touchableArea,
                                                aControlIndex,
                                                properties );

            control->MakeVisible( EFalse );

            CleanupStack::PushL( control );

            iControls.AppendL( control );

            CleanupStack::Pop( 2 ); // aspectRatioIcon, control

            break;
        }
        case EMPXBrandingAnimation:
        {
            TAknLayoutRect brandingPaneRect;
            brandingPaneRect.LayoutRect( iRect, main_mp4_pane_g3( KMP4LayoutSet ).LayoutLine() );

            // Decrease the size of BrandingAnimation to make it look better.
            TRect brandingLogoRect(
                    brandingPaneRect.Rect().iTl.iX+20,
                    brandingPaneRect.Rect().iTl.iY+50,
                    brandingPaneRect.Rect().iBr.iX-20,
                    brandingPaneRect.Rect().iBr.iY-50 );
            CMPXVideoPlaybackBrandingAnimation* brandingAnimation =
                CMPXVideoPlaybackBrandingAnimation::NewL( this,
                                                          brandingLogoRect,
                                                          iRNFormat );

            CleanupStack::PushL( brandingAnimation );

            CMPXVideoPlaybackControl* control =
                CMPXVideoPlaybackControl::NewL( this,
                                                brandingAnimation,
                                                brandingLogoRect,
                                                aControlIndex,
                                                properties );

            CleanupStack::PushL( control );

            iControls.AppendL( control );

            CleanupStack::Pop( 2 ); // brandingAnimation, control

            break;
        }
        case EMPXVideoPausedIcon:
        {
            CEikImage* pausedBitmap =
                CreateImageL( iconsPath,
                              EMbmMpxvideoplaybackcontrolsQgn_prop_image_tb_pause,
                              EMbmMpxvideoplaybackcontrolsQgn_prop_image_tb_pause_mask );

            CleanupStack::PushL( pausedBitmap );

            AknLayoutUtils::LayoutImage( pausedBitmap,
                                         iRect,
                                         AknLayoutScalable_Apps::main_mp4_pane_g4( KMP4LayoutSet ) );

            CMPXVideoPlaybackControl* control =
                CMPXVideoPlaybackControl::NewL( this,
                                                pausedBitmap,
                                                pausedBitmap->Rect(),
                                                aControlIndex,
                                                properties );

            CleanupStack::PushL( control );
            iControls.AppendL( control );
            CleanupStack::Pop( 2 ); // pausedBitmap, control
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::CreateTitleControlL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::CreateTitleControlL(
        TMPXVideoPlaybackControls aControlIndex,
        TUint aProperties )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::CreateTitleControlL()"));

    HBufC* displayedTitle;

    //
    //  If Title exists, use the title.  If title doesn't exitst, use the filename
    //  excecpt when streaming functionality is hidden
    //
    if ( iFileDetails->iTitle )

    {
        displayedTitle = HBufC::NewL( iFileDetails->iTitle->Length() );
        TPtr tempTitle( displayedTitle->Des() );
        tempTitle.Append( iFileDetails->iTitle->Des() );
    }
    else
    {
        //
        //  If title doesn't exitst, use the filename
        //  excecpt when streaming functionality is hidden
        //
        //                 ! ( iFileDetails->iMode == EStreaming &&
        //                     iFileDetails->iHideStreamingFunctionality ) )
        //
        TParse parse;
        parse.Set( iFileDetails->iClipName->Des(), NULL, NULL);
        displayedTitle = parse.Name().AllocL();
    }

    TAknTextComponentLayout layout = main_mp3_pane_t6();

    if ( iFileDetails->iArtist )
    {
        layout = main_mp3_pane_t2();
    }

    CEikLabel* label = CreateTextLabelL( *displayedTitle );

    CleanupStack::PushL( label );

    delete displayedTitle;

    AknLayoutUtils::LayoutLabel( label,
                                 iRect,
                                 layout.LayoutLine());

    label->OverrideColorL( EColorLabelText, KRgbWhite );
    label->OverrideColorL( EColorLabelTextEmphasis, KRgbWhite );

    CMPXVideoPlaybackControl* control = CMPXVideoPlaybackControl::NewL( this,
                                                                        label,
                                                                        label->Rect(),
                                                                        aControlIndex,
                                                                        aProperties );

    iControls.AppendL( control );

    CleanupStack::Pop( label );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::CreateAudioOnlyBitmapL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::CreateAudioOnlyBitmapL(
        TFileName& aIconsPath,
        TInt aBitmapIndex,
        TInt aBitmapMaskIndex,
        TMPXVideoPlaybackControls aControlIndex,
        const TAknWindowLineLayout& aLayout,
        TUint aProperties )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::CreateAudioOnlyBitmapL()"));

    CEikImage* audioOnlyBitmap = CreateImageL( aIconsPath, aBitmapIndex, aBitmapMaskIndex );

    CleanupStack::PushL( audioOnlyBitmap );

    AknLayoutUtils::LayoutImage( audioOnlyBitmap, iRect, aLayout );

    CMPXVideoPlaybackControl* control =
        CMPXVideoPlaybackControl::NewL( this,
                                        audioOnlyBitmap,
                                        audioOnlyBitmap->Rect(),
                                        aControlIndex,
                                        aProperties );

    iControls.AppendL( control );

    CleanupStack::Pop( audioOnlyBitmap );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::CreateFakeSoftKeyL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::CreateFakeSoftKeyL(
    TInt aResource,
    TBool aTop,
    TMPXVideoPlaybackControls aControlIndex,
    TUint aProperties )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::CreateFakeSoftKeyL"));

    HBufC* text = StringLoader::LoadLC( aResource );
    CEikLabel* label = CreateTextLabelL( text->Des() );
    CleanupStack::PopAndDestroy( text );

    CleanupStack::PushL( label );

    TAknTextComponentLayout SKLayout;
    TAknWindowComponentLayout softkeyPane;
    TRect skRect;

    if ( aTop )
    {
        SKLayout = main_mp4_pane_t2( KMP4LayoutSet );
    }
    else
    {
        SKLayout = main_mp4_pane_t1( KMP4LayoutSet );
    }

    AknLayoutUtils::LayoutLabel( label,
                                 iRect,
                                 SKLayout.LayoutLine() );

    if ( AknLayoutUtils::PenEnabled() )
    {
        if ( aTop )
        {
            label->SetAlignment( EHRightVCenter );
        }
        else
        {
            label->SetAlignment( EHLeftVCenter );
        }
    }
    else
    {
        if ( aTop )
        {
            //Default alignment is EHRightVTop
        }
        else
        {
            label->SetAlignment( EHRightVBottom );
        }
    }

    skRect = label->Rect();

    // Enlarge the button region to make it easy to be touched.
    skRect.iTl.iY -= 26;
    skRect.iBr.iY += 6;

    CMPXVideoPlaybackControl* control = CMPXVideoPlaybackControl::NewL( this,
                                                                        label,
                                                                        skRect,
                                                                        aControlIndex,
                                                                        aProperties );

    iControls.AppendL( control );

    CleanupStack::Pop( label );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::CreateTextLabelWithSpecificLayoutL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::CreateTextLabelWithSpecificLayoutL(
    TInt aResource,
    TMPXVideoPlaybackControls aControlIndex,
    TUint aProperties,
    TAknTextComponentLayout aLayout )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::CreateTextLabelWithSpecificLayoutL()"));

    HBufC* text = StringLoader::LoadLC( aResource );
    CEikLabel* label = CreateTextLabelL( text->Des() );
    CleanupStack::PopAndDestroy( text );

    CleanupStack::PushL( label );

    AknLayoutUtils::LayoutLabel( label,
                                 iRect,
                                 aLayout.LayoutLine());

    CMPXVideoPlaybackControl* control = CMPXVideoPlaybackControl::NewL( this,
                                                                        label,
                                                                        label->Rect(),
                                                                        aControlIndex,
                                                                        aProperties );

    iControls.AppendL( control );

    CleanupStack::Pop( label );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::CreateTextLabelL()
// -------------------------------------------------------------------------------------------------
//
CEikLabel* CMPXVideoPlaybackControlsController::CreateTextLabelL( const TDesC& aText )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::CreateTextLabelL()"));

    CEikLabel* label = new (ELeave) CEikLabel;
    label->SetContainerWindowL( *iContainer );
    label->SetTextL( aText );
    label->MakeVisible( EFalse );

    return label;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::ToggleVisibility()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::ToggleVisibility()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::ToggleVisibility()"));

    //
    //  Toggle visibility only if:
    //  (a) MediaDetailsViewer is not visble
    //  (b) TV-Out is not connected and
    //  (c) we are in playing or paused state
    //
    if ( ! IsMediaDetailsViewerVisible() &&
         ! IsTvOutConnected() &&
         ( iState == EPbStatePlaying || iState == EPbStatePaused ) )
    {
        //
        //  Controls will remain visible for audio only clips
        //
        if ( iFileDetails->iVideoEnabled )
        {
#ifdef RD_TACTILE_FEEDBACK
            if ( iFeedback )
            {
                MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::ToggleVisibility() tactile"));
                iFeedback->InstantFeedback( ETouchFeedbackBasic );
            }
#endif //RD_TACTILE_FEEDBACK

            if ( IsVisible() )
            {
                HideAllControls();
            }
            else
            {
                ShowControls();
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::ResetDisappearingTimers()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::ResetDisappearingTimers( TMPXTimerAction aTimerAction )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::ResetDisappearingTimers()"));

    if ( iControlsTimer->IsActive() )
    {
        iControlsTimer->Cancel();
    }

    if ( aTimerAction == EMPXTimerReset && iState == EPbStatePlaying )
    {
        //
        //  Do not start the time for Audio only clips or TV-Out is connected
        //
        if ( iFileDetails->iVideoEnabled && ! IsTvOutConnected() )
        {
            MPX_DEBUG(_L("    Starting Controls Timer"));

            iControlsTimer->Start(
                KMPXControlsTimeOut,
                0,
                TCallBack( CMPXVideoPlaybackControlsController::HandleControlsTimeout, this ) );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::HandleControlsTimeout
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackControlsController::HandleControlsTimeout( TAny* aPtr )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::HandleControlsTimeout()"));

    static_cast<CMPXVideoPlaybackControlsController*>(aPtr)->HideAllControls();

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::HideAllControls()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::HideAllControls()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::HideAllControls()"));

    ResetDisappearingTimers( EMPXTimerCancel );

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        iControls[i]->MakeVisible( EFalse );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::ShowControls()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::ShowControls()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::ShowControls()"),
                   _L("iState = %d"), iState );

    if ( iShowControls )
    {
        CloseMediaDetailsViewer();

        ResetDisappearingTimers( EMPXTimerReset );

        for ( TInt i = 0 ; i < iControls.Count() ; i++ )
        {
            iControls[i]->SetVisibility( iState );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::IsVisible()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControlsController::IsVisible()
{
    TBool visible = EFalse;

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->ControlIndex() == EMPXSoftkeyBack )
        {
            if ( iControls[i]->IsVisible() )
            {
                visible = ETrue;
            }

            break;
        }
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::IsVisible() [%d]"), visible);

    return visible;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::UpdateControlsVisibility()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::UpdateControlsVisibility()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::UpdateControlsVisibility()"));

    HideAllControls();
    ShowControls();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::HandleCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::HandleCommandL( TInt aCommand, TInt aValue )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::HandleCommandL(%d)"), aCommand);

    iContainer->HandleCommandL( aCommand, aValue );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::VolumeChanged()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::VolumeChanged( TInt aVolume )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::VolumeChanged() [%d]"), aVolume);

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->VolumeChanged( aVolume ) )
        {
            break;
        }
    }

    ShowControls();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::DurationChangedL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::DurationChangedL( TInt aDuration )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::DurationChangedL() [%d]"), aDuration);

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->DurationChangedL( aDuration ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::PositionChangedL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::PositionChangedL( TInt aPosition )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::PositionChangedL() [%d]"), aPosition);

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->PositionChangedL( aPosition ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::AspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::AspectRatioChanged( TInt aAspectRatio )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::AspectRatioChanged() [%d]"), aAspectRatio);

    iAspectRatio = (TMMFScalingType)aAspectRatio;

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->AspectRatioChanged( aAspectRatio ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::UpdateStateOnButtonBar()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::UpdateStateOnButtonBar()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::UpdateStateOnButtonBar() [%d]"), iState);

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->UpdateStateOnButtonBar( iState ) )
        {
            break;
        }
    }
}

// ------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::CreateBitmap()
// ------------------------------------------------------------------------------------------------
//
CEikImage*
CMPXVideoPlaybackControlsController::CreateBitmapL( TFileName& aIconsPath,
                                                    TInt aBitmapIndex,
                                                    const TAknWindowLineLayout& aLayout )
{
    CEikImage* eikImage = new (ELeave) CEikImage();

    CleanupStack::PushL( eikImage );

    CFbsBitmap* imageBitmap = AknIconUtils::CreateIconL( aIconsPath, aBitmapIndex );

    CleanupStack::PushL( imageBitmap );

    eikImage->SetBitmap( imageBitmap );
    eikImage->SetContainerWindowL( *iContainer );
    eikImage->MakeVisible( EFalse );

    AknLayoutUtils::LayoutImage( eikImage, iRect, aLayout );

    CleanupStack::Pop( 2 ); // eikImage & imageBitmap

    return eikImage;
}

// ------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::CreateImageL()
// ------------------------------------------------------------------------------------------------
//
CEikImage*
CMPXVideoPlaybackControlsController::CreateImageL( TFileName& aIconsPath,
                                                   TInt aBitmapIndex,
                                                   TInt aBitmapMaskIndex )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::CreateImageL()"));

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    CEikImage* image = new (ELeave) CEikImage();

    AknIconUtils::CreateIconLC( bitmap, mask, aIconsPath, aBitmapIndex, aBitmapMaskIndex );

    image->SetPicture( bitmap, mask );             //  Ownership transferred
    CleanupStack::Pop( 2 );                        // bitmap, mask
    image->SetContainerWindowL( *iContainer );
    image->MakeVisible( EFalse );

    return image;
}

// ------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::StopBrandingAnimation()
// ------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::StopBrandingAnimation()
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::StopBrandingAnimation()"));

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->StopBrandingAnimationTimer() )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::IsRealFormatL()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControlsController::IsRealFormatL( const TDesC& aDes )
{
    TBool realFormat = EFalse;

    if ( aDes != KNullDesC && aDes.Length() )
    {
        if ( iFileDetails->iPlaybackMode == EMPXVideoStreaming ||
             iFileDetails->iPlaybackMode == EMPXVideoLiveStreaming )
        {
            realFormat = RealFormatForStreamingL( aDes );
        }
        else
        {
            realFormat = RealFormatForLocalL();
        }
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::IsRealFormatL() [%d]"), realFormat);

    return realFormat;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::RealFormatForStreamingL()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControlsController::RealFormatForStreamingL( const TDesC& aDes )
{
    TBool realFormat = EFalse;
    TParse filePath;

    _LIT(KMPXRMEXT, ".R" );

    TInt err = filePath.Set( aDes, NULL, NULL );

    //
    // It is valid to have a "\" character in a url, but parsing fails,
    // switch these characters to "/" for the local string and try parsing again.
    //
    if ( err == KErrBadName )
    {
        TInt backsashPos = aDes.LocateF('\\');

        if( backsashPos != KErrNotFound )
        {
            HBufC* fileName = aDes.AllocL();

            TInt count( fileName->Des().Length() );

            for ( TInt j = backsashPos ; j < count; ++j )
            {
                if ( fileName->Des()[j]== '\\' )
                {
                    fileName->Des()[j]='/';
                }
            }
            err = filePath.Set( fileName->Des(), NULL, NULL );

            delete fileName;
        }
    }

    if ( err == KErrNone )
    {
        //
        // The requirement to support arguments with the extension in streaming links
        // for-ex: /video.3gp?start=10&end=40 will have to supported
        // as well. So just by doing p.Ext() would retrieve all the string
        // after "." so by doing a Left(2) would retrieve only
        // .3g or .rm and so on. This would help better
        // interpret the extension and decide the branding accordingly
        //
        filePath.Set( filePath.NameAndExt(), NULL, NULL );
        TPtrC extension = filePath.Ext().Left( 2 );

        TBuf<2> buf;
        buf.Format( extension );
        buf.UpperCase();

        // RealMedia Branding
        if ( ! buf.Compare( KMPXRMEXT ) )
        {
            realFormat = ETrue;
        }
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::RealFormatForStreamingL()[%d]"), realFormat);

    return realFormat;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::RealFormatForLocalL()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControlsController::RealFormatForLocalL()
{
    TBool realFormat = EFalse;

    _LIT(KMPXReal, "real" );
    _LIT(KMPXRN, "rn" );

    if ( iFileDetails->iMimeType )
    {
        iFileDetails->iMimeType->Des().LowerCase();

        if ( iFileDetails->iMimeType->Find( KMPXReal ) >= 0 ||
             iFileDetails->iMimeType->Find( KMPXRN ) >= 0 )
        {
            //
            // RN clip and local mode, add RN logo bitmap to show while initializing
            //
            CreateRealOneBitmapL();
            SetRealOneBitmapVisibility( ETrue );
            realFormat = ETrue;
        }
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::RealFormatForLocalL() [%d]"), realFormat);

    return realFormat;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::LocateBitmapFileL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::LocateBitmapFileL( TFileName& aFileName )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::LocateBitmapFileL()"));

    if ( ! iBitmapFileName.Length() )
    {
        TParse parse;

        //
        //  This is done to ensure upgraded file is used first.
        //  The drives from Y: to A: will be searched, then Z: last
        //
        TFindFile find( iFs );

        //
        //  Search for the MBM file
        //
        HBufC* mbmFileName = StringLoader::LoadLC( R_MPX_VIDEO_PLAYBACK_CONTROLS_ICONS_MBM_FILE );
        parse.Set( mbmFileName->Des(), &KDC_APP_BITMAP_DIR, NULL );
        TPtrC mbmPath = parse.FullName();

        MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::LocateBitmapFileL() file %S"), &mbmPath);

        TInt error = find.FindByDir( mbmPath, KNullDesC );

        if ( error == KErrNone )
        {
            parse.Set( find.File(), NULL, NULL );
        }
        else
        {
            //
            //  MBM file not found, search for the MIF file
            //
            HBufC* mifFileName = StringLoader::LoadLC( R_MPX_VIDEO_PLAYBACK_CONTROLS_ICONS_MIF_FILE );
            parse.Set( mifFileName->Des(), &KDC_APP_BITMAP_DIR, NULL );
            TPtrC mifPath = parse.FullName();
            CleanupStack::PopAndDestroy( mifFileName );

            MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::LocateBitmapFileL() file %S"), &mifPath);

            //
            //  If the MBM and the MIF file cannot be found, leave
            //
            User::LeaveIfError( find.FindByDir( mifPath, KNullDesC ) );

            parse.Set( find.File(), NULL, NULL );
        }

        //
        //  File Path must be to the MBM file
        //
        aFileName.Append( parse.Drive() );
        aFileName.Append( KDC_APP_BITMAP_DIR );
        aFileName.Append( mbmFileName->Des() );

        CleanupStack::PopAndDestroy( mbmFileName );

        //
        //  Save this bitmap file name
        //
        iBitmapFileName = aFileName;

    }
    else
    {
        aFileName.Append( iBitmapFileName );
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::LocateBitmapFileL(%S)"),&aFileName);
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::SetDownloadSize()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::SetDownloadSize( TInt aSize )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::SetDownloadSize() [%d]"), aSize);

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->SetDownloadSize( aSize ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::UpdateDownloadPosition()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::UpdateDownloadPosition( TInt aSize )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::UpdateDownloadPosition() [%d]"), aSize);

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->UpdateDownloadPosition( aSize ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::SetRealOneBitmapVisibility()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::SetRealOneBitmapVisibility( TBool aVisible )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::SetRealOneBitmapVisibility(%d)"), aVisible);

    if ( iRealOneBitmap )
    {
        iRealOneBitmap->MakeVisible( aVisible );

        if ( ! aVisible )
        {
            // HDMI/TV-out cable connected then show the controls
            if ( iTvOutConnected  && !iShowControls )
            {
                iShowControls = ETrue;
                UpdateControlsVisibility();
            }
            else
            {
                iShowControls = ETrue;
            }
        }
    }// iRealOneBitmap
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::IsRealOneBitmapVisible()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControlsController::IsRealOneBitmapVisible()
{
    TBool visible = EFalse;

    if ( iRealOneBitmap )
    {
        visible = iRealOneBitmap->IsVisible();
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::IsRealOneBitmapVisible(%d)"), visible);

    return visible;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlsController::HandleErrors
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::HandleErrors()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::HandleErrors()"),
                   _L("iState = %d"), iState );

    switch ( iState )
    {
        case EPbStateNotInitialised :
        case EPbStateInitialising :
        {
            if ( iFileDetails->iPlaybackMode == EMPXVideoStreaming )
            {
                StopBrandingAnimation();
            }

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::IsSoftKeyVisible()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControlsController::IsSoftKeyVisible( TInt aValue )
{
    TBool visible = EFalse;
    TInt softKeys;

    if ( aValue == EStdKeyDevice0 )
    {
        softKeys = EMPXSoftkeyDetails;
    }
    else//EStdKeyDevice1
    {
        softKeys = EMPXSoftkeyBack;
    }

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->ControlIndex() == softKeys )
        {
            if ( iControls[i]->IsVisible() )
            {
                visible = ETrue;
            }

            break;
        }
    }

    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::IsSoftKeyVisible() [%d]"), visible);

    return visible;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::HandleTvOutEventL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::HandleTvOutEventL(
        TBool aConnected, TMPXVideoPlaybackControlCommandIds aEvent, TBool aShowArIcon )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::HandleTvOutEventL()"));

    iFileDetails->iTvOutConnected = aConnected;
    iControlsConfig->UpdateControlListL( aEvent, aShowArIcon );
    ControlsListUpdatedL();

    for ( TInt i = 0 ; i < iControls.Count() ; i++ )
    {
        if ( iControls[i]->UpdateTVOutStatusL( aConnected ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::HandleSoftKeyPressedL( TInt aValue )
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::HandleSoftKeyPressedL( TInt aValue )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::HandleSoftKeyPressedL() [%d]"), aValue);

    if ( IsSoftKeyVisible( aValue ) )
    {
        if ( aValue == EStdKeyDevice0 )
        {
            iContainer->HandleCommandL( EMPXPbvCmdShowFileDetails );
        }
        else//EStdKeyDevice1
        {
            iContainer->HandleCommandL( EAknSoftkeyBack );
        }
    }
    else
    {
        ShowControls();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::AddDllPath
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoPlaybackControlsController::AddDllPath( TDes& aFileName )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::AddDllPath()"),
                   _L("aFileName = %S"), &aFileName);

    //
    //  Find drive for dll to search same drive for rsc
    //
    TFileName dllName;
    Dll::FileName( dllName );

    TPtrC driveAndPath = TParsePtrC( dllName ).DriveAndPath();

    TParse parse;

    TInt error = parse.Set( aFileName, &driveAndPath, NULL );

    if ( error == KErrNone )
    {
        aFileName.Zero();
        aFileName.Append( parse.FullName() );
    }

    return error;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::ShowMediaDetailsViewerL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::ShowMediaDetailsViewerL()
{
    if ( ! iMediaDetailsViewerControl )
    {
        TUint properties = 0;

        CMPXVideoPlaybackMediaDetailsViewer* detailsViewer =
                                                CMPXVideoPlaybackMediaDetailsViewer::NewL( this );

        CleanupStack::PushL( detailsViewer );

        iMediaDetailsViewerControl = CMPXVideoPlaybackControl::NewL( this,
                                                                     detailsViewer,
                                                                     detailsViewer->ViewerRect(),
                                                                     EMPXMediaDetailsViewer,
                                                                     properties );

        CleanupStack::Pop(); // detailsViewer
    }

    HideAllControls();
    iMediaDetailsViewerControl->MakeVisible(ETrue);

}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::IsMediaDetailsViewerVisible
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoPlaybackControlsController::IsMediaDetailsViewerVisible()
{
    TBool result = EFalse;

    if ( iMediaDetailsViewerControl )
    {
        result = iMediaDetailsViewerControl->IsVisible();
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::CloseMediaDetailsViewer
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::CloseMediaDetailsViewer()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::CloseMediaDetailsViewer()"));

    if ( iMediaDetailsViewerControl )
    {
        iMediaDetailsViewerControl->MakeVisible(EFalse);
        delete iMediaDetailsViewerControl;
        iMediaDetailsViewerControl = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackControlsController::HandleLoadingStarted
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackControlsController::HandleLoadingStarted()
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackControlsController::HandleLoadingStarted()"));

    if ( iState != EPbStateBuffering )
    {
        iState = EPbStateBuffering;
        UpdateStateOnButtonBar();
        UpdateControlsVisibility();
    }
}

// End of File
