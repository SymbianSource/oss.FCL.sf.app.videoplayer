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
* Description:   Implementation of Video base playback view
*
*/


// Version : %version: 83 %


//  Include Files

#include <aknViewAppUi.h>
#include <w32std.h>
#include <eikenv.h>
#include <bautils.h>
#include <AknUtils.h>
#include <coemain.h>
#include <hlplch.h>
#include <akntoolbar.h>
#include <AknUtils.h>
#include <data_caging_path_literals.hrh>

#include <mpxplaybackcommanddefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcommand.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxmediageneraldefs.h>
#include <mpxplaybackutility.h>
#include <mpxviewutility.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <mpxvideoplaybackviews.rsg>
#include <textresolver.h>
#include <mmf/common/mmferrors.h>
#include <DRMHelper.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionplaylist.h>
#include <mpxcollectionpath.h>
#include <mpxmediageneralextdefs.h>
#include <vcxmyvideosuids.h>

#include <drmuihandling.h>
#include <drmerrorhandling.h>

#include "mpxvideobaseplaybackview.h"
#include "mpxvideoplaybackcontainer.h"
#include "mpxvideoplaybackcontrol.hrh"
#include "mpxcommonvideoplaybackview.hrh"
#include "mpxvideoplaybackviewfiledetails.h"
#include "mpxvideoplaybackuids.hrh"
#include "MPFileDetailsDialog.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxmediavideodefs.h"
#include "mpxvideo_debug.h"
#include "mpxvideoplayercustomviewmsgconsts.h"

//
//  Set a constant for the information timeout of 5 seconds
//
const TInt KInformationNoteTimeout = 5000000;

// -------------------------------------------------------------------------------------------------
// CMPXVideoBasePlaybackView::CMPXVideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoBasePlaybackView::CMPXVideoBasePlaybackView()
// note, CBase initialises all member variables to zero
{
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::InitializeVideoPlaybackViewL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::InitializeVideoPlaybackViewL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::InitializeVideoPlaybackViewL()"));

    _LIT( KMPXVideoPlaybackViewsRscPath, "mpxvideoplaybackviews.rsc" );

    CCoeEnv* coeEnv = iEikonEnv;
    TParse parse;
    parse.Set( KMPXVideoPlaybackViewsRscPath, &KDC_APP_RESOURCE_DIR, NULL );
    TFileName resourceFile = parse.FullName();
    User::LeaveIfError( AddDllPath( resourceFile ) );
    BaflUtils::NearestLanguageFile( coeEnv->FsSession(), resourceFile );

    iVideoResourceOffset = coeEnv->AddResourceFileL( resourceFile );

    //
    //  Get an instance of view utility
    //
    iViewUtility = MMPXViewUtility::UtilityL();

    //
    //  Get an instance of collection utility
    //  Used for the Collection Observer to get the MediaL callbacks
    //
    iCollectionUtility = MMPXCollectionUtility::NewL( this );

    //
    //  Create Active Object for closing player
    //
    iCloseAO = CIdle::NewL( CActive::EPriorityStandard );

    BaseConstructL( R_MPX_VIDEO_PLAYBACK_VIEW );

    iAknEventMonitor = static_cast<CAknAppUiBase*>(CCoeEnv::Static()->AppUi())->EventMonitor();
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoBasePlaybackView::~CMPXVideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
CMPXVideoBasePlaybackView::~CMPXVideoBasePlaybackView()
{
    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::~CMPXVideoBasePlaybackView()"));

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }

    if ( iCloseAO )
    {
        delete iCloseAO;
        iCloseAO = NULL;
    }

    if ( iFileDetails )
    {
        delete iFileDetails;
        iFileDetails = NULL;
    }

    if ( iDisplayHandler )
    {
        delete iDisplayHandler;
        iDisplayHandler = NULL;
    }

    if ( iPlaybackUtility )
    {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        iPlaybackUtility = NULL;
    }

    if ( iViewUtility )
    {
        iViewUtility->Close();
        iViewUtility = NULL;
    }

    if ( iCollectionUtility )
    {
        iCollectionUtility->Close();
    }

    if ( iVideoResourceOffset )
    {
        iEikonEnv->DeleteResourceFile( iVideoResourceOffset );
    }

    if ( iContainer )
    {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::CreateGeneralPlaybackCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::CreateGeneralPlaybackCommandL( TMPXPlaybackCommand aCmd, TBool aDoSync )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::CreateGeneralPlaybackCommandL()"),
                   _L("aCmd = %d, aDoSync, = %d"), aCmd, aDoSync );

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, aDoSync );
    cmd->SetTObjectValueL<TBool>( KMPXCommandPlaybackGeneralNoBuffer, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXCommandIdPlaybackGeneral );
    cmd->SetTObjectValueL<TInt>( KMPXCommandPlaybackGeneralType, aCmd );

    iPlaybackUtility->CommandL( *cmd, this );

    CleanupStack::PopAndDestroy( cmd );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleCommandL( TInt aCommand )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoBasePlaybackView::HandleCommandL()"),
        _L("aCommand = %d"), aCommand );

    switch ( aCommand )
    {
        case EMPXPbvCmdPlay:
        {
            IssuePlayCommandL();
            break;
        }
        case EMPXPbvCmdPause:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdPause"));
            CreateGeneralPlaybackCommandL( EPbCmdPause );
            break;
        }
        case EAknSoftkeyClose:
        case EMPXPbvCmdClose:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdClose"));
            CreateGeneralPlaybackCommandL( EPbCmdClose );
            break;
        }
        case EMPXPbvCmdSeekForward:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdSeekForward"));
            CreateVideoSpecificCmdL( EPbCmdStartVideoSeekingForward );
            break;
        }
        case EMPXPbvCmdSeekBackward:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdSeekBackward"));
            CreateVideoSpecificCmdL( EPbCmdStartVideoSeekingBackward );
            break;
        }
        case EMPXPbvCmdEndSeek:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdEndSeek"));
            CreateVideoSpecificCmdL( EPbCmdStopVideoSeeking );
            break;
        }
        case EMPXPbvCmdPlayPause:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdPause"));
            iPlaybackUtility->CommandL( EPbCmdPlayPause );
            break;
        }
        case EAknSoftkeyBack:
        {
            ClosePlaybackViewL();
            break;
        }
        case EMPXPbvCmdStop:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdStop"));
            CreateGeneralPlaybackCommandL( EPbCmdStop );
            break;
        }
        case EMPXPbvCmdDecreaseVolume:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdDecreaseVolume"));
            HandleVolumeCmdL( EPbCmdDecreaseVolume );
            break;
        }
        case EMPXPbvCmdIncreaseVolume:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdIncreaseVolume"));
            HandleVolumeCmdL( EPbCmdIncreaseVolume );
            break;
        }
        case EMPXPbvCmdNaturalAspectRatio:
        {
            MPX_DEBUG(
                _L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdNaturalAspectRatio"));
            SetAspectRatioL( EPbCmdNaturalAspectRatio );
            break;
        }
        case EMPXPbvCmdZoomAspectRatio:
        {
            MPX_DEBUG(
                _L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdZoomAspectRatio"));
            SetAspectRatioL( EPbCmdZoomAspectRatio );
            break;
        }
        case EMPXPbvCmdStretchAspectRatio:
        {
            MPX_DEBUG(
                _L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdStretchAspectRatio"));
            SetAspectRatioL( EPbCmdStretchAspectRatio );
            break;
        }
        case EMPXPbvCmdMute:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdMute"));
            HandleVolumeCmdL( EPbCmdMuteVolume );
            break;
        }
        case EMPXPbvCmdUnMute:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdUnMute"));
            HandleVolumeCmdL( EPbCmdUnMuteVolume );
            break;
        }
        case EMPXPbvCmdShortPressBackward:
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleCommandL() EMPXPbvCmdShortPressBackward"));

            HandleShortPressBackwardL();
            break;
        }
        case EAknCmdHelp:
        {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                                 AppUi()->AppHelpContextL() );
            break;
        }
        case EMPXPbvCmdShowFileDetails:
        {
            if ( AknLayoutUtils::PenEnabled() )
            {
                iContainer->HandleEventL( EMPXControlCmdShowFileDetailsViewer );
            }
            else
            {
                ShowFileDetailsDialogL();
            }
            break;
        }
        case EMPXPbvCmdEndOfClip:
        {
            CreateVideoSpecificCmdL( EPbCmdEndofClipReached );
            break;
        }
        case EMPXPbvCmdCustomPause:
        {
            CreateVideoSpecificCmdL( EPbCmdCustomPause );
            break;
        }
        case EMPXPbvCmdCustomPlay:
        {
            CreateVideoSpecificCmdL( EPbCmdCustomPlay );
            break;
        }
        case EMPXPbvLaunchDRMDetails:
        {
            LaunchDRMDetailsL();
            break;
        }
        case KMpxVideoPlaybackPdlReloading:
        {
            iPdlReloading = ETrue;
            iMediaRequested = EFalse;
            break;
        }
        case KMpxVideoPlaybackPdlReloadComplete:
        {
            HandlePdlReloadComplete();
            break;
        }
        case EMPXPbvCmdRealOneBitmapTimeout:
        {
            HandleRealOneBitmapTimeoutL();
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::ProcessCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::ProcessCommandL( TInt aCommand )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoBasePlaybackView::ProcessCommandL()"),
        _L("aCommand = %d"), aCommand );

    if ( aCommand == EAknCmdExit )
    {
        MPX_DEBUG(_L("CMPXVideoBasePlaybackView::ProcessCommandL() EAknCmdExit"));

        if ( MenuBar() )
        {
            MenuBar()->StopDisplayingMenuBar();
        }
    }

    CAknView::ProcessCommandL( aCommand );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleStatusPaneSizeChange()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleStatusPaneSizeChange()
{
    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleStatusPaneSizeChange()"));
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::DoActivateL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DoActivateL( const TVwsViewId& /* aPrevViewId */,
                                             TUid /* aCustomMessageId */,
                                             const TDesC8& /* aCustomMessage */ )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::DoActivateL()"));

    //
    //  Get the playback utility instance from playback utility
    //
    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( EMPXCategoryVideo, KPbModeDefault );
    iPlaybackUtility->AddObserverL( *this );
    iPlaybackUtility->SetPrimaryClientL();

    //
    //  Initialize the playback state
    //
    iPlaybackState = iPlaybackUtility->StateL();

    //
    //  Observe the View Framework when the view is active
    //
    iViewUtility->AddObserverL( this );

    //
    //  Disable tool bar in playback view
    //
    AppUi()->CurrentFixedToolbar()->SetToolbarVisibility( EFalse );

    if ( ! iContainer )
    {
        AppUi()->SetOrientationL( CAknAppUiBase::EAppUiOrientationLandscape );

        iContainer = CMPXVideoPlaybackContainer::NewL( this );

        AppUi()->AddToStackL( *this, iContainer );

        iContainer->GetWindow().SetVisible( ETrue );

        //
        //  Create Video Playback Display Handler
        //
        iDisplayHandler = CMPXVideoPlaybackDisplayHandler::NewL( iPlaybackUtility, iContainer );
    }

    //
    //  Register for window visibility changes
    //
    iContainer->GetWindow().EnableVisibilityChangeEvents();

    iKeyboardInFocus = ETrue;
    iAknEventMonitor->Enable( ETrue );
    iAknEventMonitor->AddObserverL( this );

    //
    //  Deactivate the CBA set the LSK & RSK to empty
    //
    Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_EMPTY );

    //
    //  Determine if the playback is from a playlist on view activation
    //
    MMPXSource* s = iPlaybackUtility->Source();
    CMPXCollectionPlaylist* playlist = s->PlaylistL();
    iPlaylistView = (playlist) ? ETrue : EFalse;
    delete playlist;

    //
    //  Retrieve PDL information for container
    //
    RetrievePdlInformationL();

    //
    //  Request the needed Media from the Playback Plugin
    //
    RequestMediaL();
}

// -------------------------------------------------------------------------------------------------
// From CAknView
// View deactivation function.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DoDeactivate()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::DoDeactivate()"));

    //
    //  Close the playback plugin to release all references to previous clip
    //
    MPX_TRAPD( err, HandleCommandL( EMPXPbvCmdClose ) );

    iAknEventMonitor->Enable( EFalse );
    iAknEventMonitor->RemoveObserver( this );
    iKeyboardInFocus = EFalse;

    //
    //  Delete the display handler when the view is deactivated
    //
    if ( iDisplayHandler )
    {
        delete iDisplayHandler;
        iDisplayHandler = NULL;
    }

    if ( iPlaybackUtility )
    {
        TRAP_IGNORE( iPlaybackUtility->RemoveObserverL( *this ) );
        iPlaybackUtility->Close();
        iPlaybackUtility = NULL;
    }

    //
    //  Stop observing the view framework when this view is deactivated
    //
    iViewUtility->RemoveObserver( this );

    if ( iContainer )
    {
        iContainer->GetWindow().SetVisible( EFalse );
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
    }

    iMediaRequested = EFalse;
    iPlaybackState = EPbStateNotInitialised;

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleForegroundEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleForegroundEventL( TBool aForeground )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::HandleForegroundEventL()"),
                   _L("aForeground = %d"), aForeground );

    if ( aForeground )
    {
        iContainer->HandleEventL( EMPXControlCmdHandleForegroundEvent );
        iKeyboardInFocus = ETrue;

        SendWindowCommandL( EPbCmdHandleForeground );
    }
    else
    {
        iContainer->HandleEventL( EMPXControlCmdHandleBackgroundEvent );
        iKeyboardInFocus = EFalse;

        SendWindowCommandL( EPbCmdHandleBackground );
    }

    CAknView::HandleForegroundEventL( aForeground );
}

// -------------------------------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically initialises a menu pane.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::DynInitMenuPaneL()"));

    if ( aResourceId == R_MPX_VIDEO_PLAYBACK_VIEW_MENU  )
    {
        //
        //  Make EMPXPbvCmdChangeAspectRatio menu dimmed in the followings cases:
        //  -  support touch
        //  -  a mmf controller is not prepared
        //  -  clip doesn't have a video track
        //  -  TV-out cable gets connected
        //
        if ( AknLayoutUtils::PenEnabled() ||
             ! iFileDetails ||
             ! iFileDetails->iVideoEnabled ||
             iFileDetails->iVideoHeight <= 0 ||
             iFileDetails->iVideoWidth <= 0 )
        {
            aMenuPane->SetItemDimmed( EMPXPbvCmdChangeAspectRatio, ETrue );
        }
    }
}

void CMPXVideoBasePlaybackView::HandleViewActivation( const TUid& aCurrentViewType,
                                                      const TUid& aPreviousViewType )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::HandleViewActivation()"),
                   _L("aCurrentViewType = 0x%08x, aPreviousViewType = 0x%08x"),
                   aCurrentViewType.iUid, aPreviousViewType.iUid );

    //
    //  This view is active since we are receiving the callback.
    //  Some new view is being activated so remove surface, stop playback and
    //  return to automatic orientation
    //
    iDisplayHandler->RemoveDisplayWindow();

    TRAP_IGNORE( HandleCommandL( EMPXPbvCmdStop );
                 AppUi()->SetOrientationL( CAknAppUiBase::EAppUiOrientationAutomatic ) );
}

// -------------------------------------------------------------------------------------------------
// From MMPXPlaybackObserver
// Handle playback message.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandlePlaybackMessage( CMPXMessage* aMessage, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::HandlePlaybackMessage()"),
                   _L("aError = %d"), aError );

    if ( aError )
    {
        MPX_TRAPD( err, DoHandleErrorPlaybackMessageL( aError ) );
    }
    else if ( aMessage )
    {
        MPX_TRAPD( err, DoHandlePlaybackMessageL( aMessage ) );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::IsInMemoryPlugin
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoBasePlaybackView::IsInMemoryPlugin()
{
    TBool retVal( EFalse );

    MMPXCollection& collectionPlugin = iCollectionUtility->Collection();

    TUid collectionUid;
    TUid inMemoryPluginUid = TUid::Uid( KMpxInMemoryPluginUid );

    MPX_TRAPD( error, collectionUid = collectionPlugin.UidL() );

    if ( ! error )
    {
        MPX_DEBUG(_L("CMPXVideoBasePlaybackView::IsInMemoryPlugin() collectionUid = 0x%08x"),
            collectionUid.iUid );

        if ( collectionUid == inMemoryPluginUid )
        {
            retVal = ETrue;
        }
    }

    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::IsInMemoryPlugin(%d)"), retVal );

    return retVal;
}

// -------------------------------------------------------------------------------------------------
// Request for the media object
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::RequestMediaL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::RequestMediaL()"));

    if ( ! iMediaRequested && iPlaybackState == EPbStateInitialised )
    {
        iMediaRequested = ETrue;

        if ( iPlaylistView && IsInMemoryPlugin() )
        {
            // Get the media attributes from the collection plugin
            RequestCollectionMediaL();
        }
        else
        {
            // Get the file details from playback plugin
            RequestPlaybackMediaL();
        }
    }
}

// -------------------------------------------------------------------------------------------------
// Request for the media object
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::RequestPlaybackMediaL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::RequestPlaybackMediaL()"));

    //
    //  Request the volume for the controls
    //
    iPlaybackUtility->PropertyL( *this, EPbPropertyVolume );

    //
    //  Request the clip's meta data
    //
    MMPXSource* s = iPlaybackUtility->Source();

    if ( s )
    {
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);

        //
        //  General Media Attributes
        //
        attrs.Append( KMPXMediaGeneralUri |
                      KMPXMediaGeneralDuration |
                      KMPXMediaGeneralTitle |
                      KMPXMediaGeneralMimeType );

        //
        //  Video specific Attributes
        //
        attrs.Append( KMPXMediaVideoAll );

        // Set the attribute to always route the media call to playback plugin
        CMPXAttributeSpecs* specs = CMPXAttributeSpecs::NewL();
        CleanupStack::PushL( specs );

        specs->SetTObjectValueL<TBool>(KMPXMediaGeneralExtMediaRedirect, ETrue);

        s->MediaL( attrs.Array(), *this, specs);

        CleanupStack::PopAndDestroy( specs );
        CleanupStack::PopAndDestroy( &attrs );
    }
}

// -------------------------------------------------------------------------------------------------
// Request for the media object
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::RequestCollectionMediaL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::RequestCollectionMediaL()"));

    //  Request the clip's meta data
    MMPXSource* s = iPlaybackUtility->Source();

    if ( s )
    {
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);

        //  General Media Attributes
        attrs.Append( KMPXMediaGeneralExtVideoSeekable );

        s->MediaL( attrs.Array(), *this );

        // Set the falg to indicate that media was reuqested from collection FW
        iCollectionMediaRequested = ETrue;

        CleanupStack::PopAndDestroy( &attrs );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::DoHandlePlaybackMessageL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DoHandlePlaybackMessageL( CMPXMessage* aMessage )
{
    TMPXMessageId id( *(aMessage->Value<TMPXMessageId>(KMPXMessageGeneralId)) );

    MPX_ENTER_EXIT(
        _L("CMPXVideoBasePlaybackView::DoHandlePlaybackMessageL()"),
        _L("id = 0x%08x"), id );

    if ( KMPXMessageGeneral == id )
    {
        HandleGeneralPlaybackMessageL( aMessage );
    }
    else if ( KMPXMediaIdVideoPlayback == id )
    {
        HandleVideoPlaybackMessage( aMessage );
    }
    else if ( KMPXMediaIdVideoDisplayMessage == id )
    {
        if ( iDisplayHandler )
        {
            iDisplayHandler->HandleVideoDisplayMessageL( aMessage );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleGeneralPlaybackMessageL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleGeneralPlaybackMessageL( CMPXMessage* aMessage )
{
    TInt event( *aMessage->Value<TInt>( KMPXMessageGeneralEvent ) );
    TInt type( *aMessage->Value<TInt>( KMPXMessageGeneralType ) );
    TInt data( *aMessage->Value<TInt>( KMPXMessageGeneralData ) );

    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::HandleGeneralPlaybackMessageL()"),
                   _L("event = %d type = %d value = %d"), event, type, data );

    switch ( event )
    {
        case TMPXPlaybackMessage::EStateChanged:
        {
            DoHandleStateChangeL( type );
            break;
        }
        case TMPXPlaybackMessage::EPropertyChanged:
        {
            TMPXPlaybackProperty property( static_cast<TMPXPlaybackProperty>( type ) );

            HandlePropertyL( property, data, KErrNone );
            break;
        }
        case TMPXPlaybackMessage::EDownloadPositionChanged:
        {
            if ( iContainer )
            {
                iContainer->HandleEventL( EMPXControlCmdDownloadUpdated, data );
            }

            break;
        }
        case TMPXPlaybackMessage::EDownloadStateChanged:
        {
            HandlePdlStateChangeL( data );

            break;
        }
        case TMPXPlaybackMessage::EReachedEndOfPlaylist:
        {
            HandleCommandL( EAknSoftkeyBack );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleVideoPlaybackMessage()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleVideoPlaybackMessage( CMPXMessage* aMessage )
{
    TMPXVideoPlaybackCommand message =
        ( *(aMessage->Value<TMPXVideoPlaybackCommand>(KMPXMediaVideoPlaybackCommand)) );

    MPX_DEBUG(
      _L("CMPXVideoBasePlaybackView::HandleVideoPlaybackMessage() message = %d"), message );

    switch ( message )
    {
        case EPbCmdPluginError:
        {
            TInt error( *aMessage->Value<TInt>( KMPXMediaVideoError ) );

            MPX_TRAPD( err, HandlePluginErrorL( error ) );
            break;
        }
        case EPbCmdTvOutEvent:
        {
            TMPXVideoPlaybackControlCommandIds cmdId = EMPXControlCmdTvOutDisconnected;

            TBool tvOutConnected( *aMessage->Value<TInt>( KMPXMediaVideoTvOutConnected ) );

            if ( tvOutConnected )
            {
                cmdId = EMPXControlCmdTvOutConnected;
            }

            if ( iContainer )
            {
                MPX_TRAPD( err, iContainer->HandleEventL( cmdId ) );
            }
            break;
        }
        case EPbCmdLoadingStarted:
        {
            if ( iContainer )
            {
                MPX_TRAPD( err, iContainer->HandleEventL( EMPXControlCmdLoadingStarted ) );
            }

            break;
        }
        case EPbCmdHideControls:
        {
            if ( iContainer )
            {
                MPX_TRAPD( err, iContainer->HandleEventL( EMPXControlCmdHideControls ) );
            }
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleClosePlaybackViewL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleClosePlaybackViewL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::HandleClosePlaybackViewL()"));

    iPlaybackState = EPbStateStopped;

    if ( IsMultiItemPlaylist() )
    {
        iPlaybackUtility->CommandL( EPbCmdNext );
    }
    else
    {
        ClosePlaybackViewL();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandlePluginErrorL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandlePluginErrorL( TInt aError )
{
    MPX_DEBUG(
      _L("CMPXVideoBasePlaybackView::HandlePluginErrorL() aError = %d"), aError );

    switch ( aError )
    {
        case KErrPathNotFound:
        case KErrNotFound:
        {
            DisplayErrorMessageL( R_MPX_CLIP_NOT_FOUND_IN_SERVER );
            HandleClosePlaybackViewL();
            break;
        }
        case KErrCancel:
        {
            HandleClosePlaybackViewL();
            break;
        }
        case KErrMMDRMNotAuthorized:
        case KErrCANoRights:
        case KErrCANoPermission:
        case KErrCAPendingRights:
        {
            HandleDrmErrorsL( aError );
            break;
        }
        case KMPXVideoCallOngoingError:
        {
            DisplayInfoMessageL( R_MPX_VIDEO_CALL_ONGOING );
            break;
        }
        default:
        {
            //
            //  TextResolver instance for error resolving.
            //
            CTextResolver* textresolver = CTextResolver::NewLC();

            //
            //  Resolve the error text
            //
            TPtrC text;
            text.Set( textresolver->ResolveErrorString( aError ) );

            CAknErrorNote* dlg = new (ELeave) CAknErrorNote( ETrue );

            dlg->ExecuteLD( text );

            CleanupStack::PopAndDestroy( textresolver );

            HandleClosePlaybackViewL();
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   Handle playback state changes
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DoHandleStateChangeL( TInt aNewState )
{
    MPX_ENTER_EXIT(
        _L("CMPXVideoBasePlaybackView::DoHandleStateChangeL()"),
        _L("aNewState = %d, currentState = %d"), aNewState, iPlaybackState );

    if ( aNewState != iPlaybackState )
    {
        TBool stateChange( ETrue );

        TMPXPlaybackState oldState = iPlaybackState;
        iPlaybackState = (TMPXPlaybackState)aNewState;

        switch ( aNewState )
        {
            case EPbStateInitialising:
            {
                HandleInitializingStateL( oldState );
                break;
            }
            case EPbStateBuffering:
            {
                TInt bufferingPercentage( 0 );

                //
                //  If buffering percentage is 100, buffering is a transition state anc
                //  can be ignored
                //
                MPX_TRAPD( err, bufferingPercentage = RetrieveBufferingPercentageL() );

                if ( bufferingPercentage == 100 )
                {
                    stateChange = EFalse;
                }
                else
                {
                    HandleBufferingStateL();
                }

                break;
            }
            case EPbStatePluginSeeking:
            {
                // no-op
                break;
            }
            case EPbStateStopped:
            {
                HandleStoppedStateL();
                break;
            }
            case EPbStateInitialised:
            {
                RequestMediaL();
                break;
            }
            default:
            {
                break;
            }
        }

        if ( iContainer && stateChange )
        {
            iContainer->HandleEventL( EMPXControlCmdStateChanged, aNewState );
        }
    }
}

// -------------------------------------------------------------------------------------------------
// Handle playback error message.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DoHandleErrorPlaybackMessageL( TInt aError )
{
    MPX_ENTER_EXIT( _L("CMPXVideoBasePlaybackView::DoHandleErrorPlaybackMessageL()"),
                    _L("aError = %d"), aError );

    iContainer->HandleCommandL( EMPXPbvCmdResetControls );
    HandlePluginErrorL( aError );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoBasePlaybackView::HandlePlaybackCommandComplete()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandlePlaybackCommandComplete( CMPXCommand* /*aCommandResult*/,
                                                               TInt /*aError*/ )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::HandlePlaybackCommandComplete()"));
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoBasePlaybackView::ParseMetaDataL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::ParseMetaDataL( const CMPXMessage& aMedia )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::ParseMetaDataL()"));

    //
    //  Clip Name
    //
    if ( aMedia.IsSupported( KMPXMediaGeneralUri ) )
    {
        TPtrC uri( aMedia.ValueText( KMPXMediaGeneralUri ) );
        iFileDetails->iClipName = uri.AllocL();
    }

    //
    //  Title
    //
    if ( aMedia.IsSupported( KMPXMediaGeneralTitle ) )
    {
        TPtrC title( aMedia.ValueText( KMPXMediaGeneralTitle ) );
        iFileDetails->iTitle = title.AllocL();
    }

    //
    //  Artist
    //
    if ( aMedia.IsSupported( KMPXMediaVideoArtist ) )
    {
        TPtrC artist( aMedia.ValueText( KMPXMediaVideoArtist ) );
        iFileDetails->iArtist = artist.AllocL();
    }

    //
    //  Mime Type
    //
    if ( aMedia.IsSupported( KMPXMediaGeneralMimeType ) )
    {
        TPtrC mimeType( aMedia.ValueText( KMPXMediaGeneralMimeType ) );
        iFileDetails->iMimeType = mimeType.AllocL();
    }

    //
    //  Duration
    //
    if ( aMedia.IsSupported( KMPXMediaGeneralDuration ) )
    {
        iFileDetails->iDuration = aMedia.ValueTObjectL<TInt>( KMPXMediaGeneralDuration );
    }

    //
    //  Audio Enabled
    //
    if ( aMedia.IsSupported( KMPXMediaVideoAudioEnabled ) )
    {
        iFileDetails->iAudioEnabled = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoAudioEnabled );
    }

    //
    //  Video Enabled
    //
    if ( aMedia.IsSupported( KMPXMediaVideoVideoEnabled ) )
    {
        iFileDetails->iVideoEnabled = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoVideoEnabled );
    }

    //
    //  Partial Playback
    //
    if ( aMedia.IsSupported( KMPXMediaVideoPartialPlayback ) )
    {
        iFileDetails->iPartialPlayback =
            aMedia.ValueTObjectL<TInt>( KMPXMediaVideoPartialPlayback );
    }

    //
    //  Playback Mode
    //
    if ( aMedia.IsSupported( KMPXMediaVideoPlaybackMode ) )
    {
        iFileDetails->iPlaybackMode =
            (TMPXVideoMode)aMedia.ValueTObjectL<TInt>( KMPXMediaVideoPlaybackMode );
    }

    //
    //  Seekable
    //
    if ( aMedia.IsSupported( KMPXMediaVideoSeekable ) )
    {
        iFileDetails->iSeekable = aMedia.ValueTObjectL<TBool>( KMPXMediaVideoSeekable );
    }

    //
    //  Pausable
    //
    if ( aMedia.IsSupported( KMPXMediaVideoPausableStream ) )
    {
        iFileDetails->iPausableStream = aMedia.ValueTObjectL<TBool>( KMPXMediaVideoPausableStream );
    }

    //
    //  Video Height
    //
    if ( aMedia.IsSupported( KMPXMediaVideoHeight ) )
    {
        iFileDetails->iVideoHeight = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoHeight );
    }

    //
    //  Video Width
    //
    if ( aMedia.IsSupported( KMPXMediaVideoWidth ) )
    {
        iFileDetails->iVideoWidth = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoWidth );
    }

    //
    //  TV-Out Connected
    //
    if ( aMedia.IsSupported( KMPXMediaVideoTvOutConnected ) )
    {
        iFileDetails->iTvOutConnected = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoTvOutConnected );
    }

    //
    //  Video BitRate
    //
    if ( aMedia.IsSupported( KMPXMediaVideoBitRate ) )
    {
        iFileDetails->iBitRate = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoBitRate );
    }

    //
    //  ForCC Code
    //
    if ( aMedia.IsSupported( KMPXMediaFourCCCode ) )
    {
        iFileDetails->iFourCCCode = aMedia.ValueTObjectL<TUint32>( KMPXMediaFourCCCode );
    }

    //
    //  Drm Protected
    //
    if ( aMedia.IsSupported( KMPXMediaVideoDrmProtected ) )
    {
        iFileDetails->iDrmProtected = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoDrmProtected );
    }
}

// -------------------------------------------------------------------------------------------------
// Handle media properties.
// Notes: The client is responsible for delete the object of aProperties.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DoHandleMediaL( const CMPXMessage& aMedia, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::DoHandleMediaL()"),
                   _L("aError = %d"), aError );

    if ( aError == KErrNone )
    {
        if ( iFileDetails )
        {
            iFileDetails->ClearFileDetails();
        }
        else
        {
            iFileDetails = CMPXVideoPlaybackViewFileDetails::NewL();
        }

        //
        //  Read in the media data
        //
        ParseMetaDataL( aMedia );

        //
        //  Create controls since file details are available
        //
        iContainer->AddFileDetailsL( iFileDetails );

        if ( iFileDetails->iVideoEnabled )
        {
            //
            //  Calculate the aspect ratio and setup the display hanlder with the display window
            //  Aspect ratio should be calculated 1st so the auto scale will be set when
            //  the display window is created.
            //
            TInt newAspectRatio = iDisplayHandler->SetDefaultAspectRatioL( iFileDetails );

            iDisplayHandler->CreateDisplayWindowL( *(CCoeEnv::Static()->ScreenDevice()),
                                                   iContainer->GetWindow() );

            iContainer->HandleEventL( EMPXControlCmdSetAspectRatio, newAspectRatio );
        }
        else
        {
            //
            //  Remove the display window so the surface can be released
            //
            iDisplayHandler->RemoveDisplayWindow();
        }

        //
        //  Delay the play command while the Real One Bitmap is being shown
        //
        if ( iContainer->IsRealOneBitmapTimerActive() )
        {
            iRealOneDelayedPlay = ETrue;
        }
        else
        {
            CreateGeneralPlaybackCommandL( EPbCmdPlay );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::UpdatePbPluginMedia()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::UpdatePbPluginMediaL( TBool aSeek)
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::UpdatePbPluginMediaL()"),
                   _L("aSeek = %d"), aSeek );

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );

    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );

    cmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand,
                                                     EPbCmdUpdateSeekable );

    cmd->SetTObjectValueL<TBool>( KMPXMediaGeneralExtVideoSeekable, aSeek );

    iPlaybackUtility->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
}

// -------------------------------------------------------------------------------------------------
// From MMPXPlaybackCallback
// Handle media event.
// Notes: The client is responsible for delete the object of aProperties.
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleMediaL( const CMPXMedia& aMedia, TInt aError)
{
    MPX_ENTER_EXIT(_L( "CMPXVideoBasePlaybackView::HandleMediaL()" ));

    if ( iCollectionMediaRequested )
    {
        TBool seek;
        iCollectionMediaRequested = EFalse;

        if ( aMedia.IsSupported( KMPXMediaGeneralExtVideoSeekable ) )
        {
            seek = aMedia.ValueTObjectL<TBool>( KMPXMediaGeneralExtVideoSeekable );
            UpdatePbPluginMediaL( seek );
        }

        // request for media from playbackplugin
        RequestPlaybackMediaL();
    }
    else
    {
        //
        //  Retreive the clip name for all use cases
        //
        if ( aMedia.IsSupported( KMPXMediaGeneralUri ) )
        {
            if ( iClipName )
            {
                delete iClipName;
                iClipName = NULL;
            }

            TPtrC uri( aMedia.ValueText( KMPXMediaGeneralUri ) );
            iClipName = uri.AllocL();
        }

        if ( aMedia.IsSupported( KMPXMediaVideoError ) )
        {
            TInt error = aMedia.ValueTObjectL<TInt>( KMPXMediaVideoError );

            iMediaRequested = EFalse;
            HandlePluginErrorL( error );
        }
        else
        {
            DoHandleMediaL( aMedia, aError );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::SetPropertyL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::SetPropertyL( TMPXPlaybackProperty aProperty, TInt aValue )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::SetPropertyL"),
                   _L("aProperty = %d, aValue = %d"), aProperty, aValue );

    iPlaybackUtility->SetL( aProperty, aValue );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandlePropertyL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandlePropertyL( TMPXPlaybackProperty aProperty,
                                                 TInt aValue,
                                                 TInt aError )
{
    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandlePropertyL - Error(%d)"), aError );

    if ( aError == KErrNone )
    {
        switch ( aProperty  )
        {
            case EPbPropertyPosition:
            {
                MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandlePropertyL position(%d)"), aValue );

                if ( iContainer )
                {
                    iContainer->HandleEventL( EMPXControlCmdSetPosition, aValue );
                }

                break;
            }
            case EPbPropertyDuration:
            {
                MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandlePropertyL duration(%d)"), aValue );

                if ( iContainer )
                {
                    iContainer->HandleEventL( EMPXControlCmdSetDuration, aValue );
                }

                break;
            }
            case EPbPropertyMaxVolume:
            {
                MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandlePropertyL max volume(%d)"), aValue );

                break;
            }
            case EPbPropertyVolume:
            {
                MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandlePropertyL volume(%d)"), aValue );

                if ( iContainer )
                {
                    iContainer->HandleEventL( EMPXControlCmdSetVolume, aValue );
                }

                break;
            }
            case EPbPropertyMute:
            {
                MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandlePropertyL muted(%d)"), aValue );

                if ( iContainer && aValue )
                {
                    iContainer->HandleEventL( EMPXControlCmdSetVolume, 0 );
                }

                break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::DisplayInfoMessageL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DisplayInfoMessageL( TInt aResourceId, TBool aWaitingDialog )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::DisplayInfoMessageL()"),
                   _L("resource = %08x"), aResourceId );

    HBufC* text = StringLoader::LoadLC( aResourceId );

    CAknInformationNote* dlg = new (ELeave) CAknInformationNote( aWaitingDialog );
    dlg->SetTimeout( (CAknNoteDialog::TTimeout) KInformationNoteTimeout );
    dlg->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::DisplayErrorMessageL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DisplayErrorMessageL( TInt aResourceId )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::DisplayErrorMessageL()"),
                   _L("resource = %08x"), aResourceId );

    HBufC* text = StringLoader::LoadLC( aResourceId );

    CAknErrorNote* eDlg = new (ELeave) CAknErrorNote( ETrue );
    eDlg->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::RetrieveFileNameAndModeL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::RetrieveFileNameAndModeL( CMPXCommand* aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::RetrieveFileNameAndModeL()"));

    //
    //  set attributes on the command
    //
    aCmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    aCmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );
    aCmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand,
                                                      EPbCmdInitView );

    iPlaybackUtility->CommandL( *aCmd );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::ActivateClosePlayerActiveObject
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::ActivateClosePlayerActiveObject()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::ActivateClosePlayerActiveObject()"));

    if ( ! iCloseAO->IsActive() )
    {
        iCloseAO->Start( TCallBack( CMPXVideoBasePlaybackView::ClosePlayerL, this ) );
    }
}

// -----------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::ClosePlayerL
// -----------------------------------------------------------------------------
//
TInt CMPXVideoBasePlaybackView::ClosePlayerL( TAny* aPtr )
{
    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::ClosePlayerL()"));

    static_cast<CMPXVideoBasePlaybackView*>(aPtr)->DoClosePlayerL();
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::DoClosePlayerL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DoClosePlayerL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::DoClosePlayerL()"));
    AppUi()->HandleCommandL( EAknSoftkeyBack );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::IssuePlayCommandL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::IssuePlayCommandL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::IssuePlayCommandL()"));
    CreateGeneralPlaybackCommandL( EPbCmdPlay );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::RetrievePdlInformationL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::RetrievePdlInformationL()
{
    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::RetrievePdlInformationL()"));
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleBufferingStateL
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleBufferingStateL()
{
    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleBufferingStateL()"));

}

// -------------------------------------------------------------------------------------------------
// CMPXVideoBasePlaybackView::CreateVideoSpecificCmdL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::CreateVideoSpecificCmdL( TMPXVideoPlaybackCommand aCmd )
{
    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::CreateVideoSpecificCmdL(%d)"), aCmd );

    //
    //  create command to pass to playback plugin
    //
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );

    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );

    cmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand, aCmd );

    iPlaybackUtility->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoBasePlaybackView::SetAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::SetAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::SetAspectRatioL()"));

    TInt newAspectRatio = iDisplayHandler->SetAspectRatioL( aCmd );

    if ( iContainer )
    {
        iContainer->HandleEventL( EMPXControlCmdSetAspectRatio, newAspectRatio );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::IsAppInFrontL()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoBasePlaybackView::IsAppInFrontL()
{
    TBool ret = EFalse;
    RWsSession wsSession;

    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL( wsSession );

    if ( wsSession.Handle() )
    {
        CArrayFixFlat<TInt>* wgList =
            new (ELeave) CArrayFixFlat<TInt>( wsSession.NumWindowGroups() );

        if ( wsSession.WindowGroupList( 0, wgList ) == KErrNone )
        {
            //
            //  If this window group is at the start of the window group array,
            //  this application is in the front
            //
            ret = ( iCoeEnv->RootWin().Identifier() == wgList->At(0) );
        }

        delete wgList;
    }

    CleanupStack::PopAndDestroy(); //wsSession

    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::IsAppInFrontL (%d)" ), ret);

    return ret;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::ClosePlaybackViewL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::ClosePlaybackViewL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::ClosePlaybackViewL()"));

    if ( iFileDetailsDialog )
    {
        iFileDetailsDialog->Close();
    }

    //
    //  The View Utility may have deactivated this view already.
    //  Check if we are the active view
    //
    if ( iViewUtility->ActiveViewImplementationUid() == ViewImplementationId() )
    {
        if ( iViewUtility->ViewHistoryDepth() > 1 )
        {
            iViewUtility->ActivatePreviousViewL();

            //
            //  Hide controls and window since view will be deactivated.
            //  This prevents black boxes on the view switching
            //
            iContainer->HandleEventL( EMPXControlCmdHideControls );
        }
        else
        {
            CloseEmbeddedPlaybackViewL();
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleVolumeCmdL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleVolumeCmdL( TMPXPlaybackCommand aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::HandleVolumeCmdL()"));

    //
    // In case user try to change the volume via media key, rocker key or etc
    // We need to show the controls though the volume level doesn't get changed
    // For examples : - try to change the volume with a clip without audio track
    //                - try to reduce the volume with volume level 0
    //                - try to increase the volume with max volume level
    //
    iContainer->HandleEventL( EMPXControlCmdShowControls );

    switch( aCmd )
    {
        case EPbCmdDecreaseVolume:
        {
            CreateVideoSpecificCmdL( EPbCmdHandleDecreaseVolume );
            break;
        }
        case EPbCmdIncreaseVolume:
        {
            CreateVideoSpecificCmdL( EPbCmdHandleIncreaseVolume );
            break;
        }
        default:
        {
            iPlaybackUtility->CommandL( aCmd );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleShortPressBackwardL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleShortPressBackwardL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::HandleShortPressBackwardL()"));

    SetPropertyL( EPbPropertyPosition, 0);
}

TInt CMPXVideoBasePlaybackView::AddDllPath( TDes& aFileName )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::AddDllPath()"),
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

void CMPXVideoBasePlaybackView::ShowFileDetailsDialogL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::ShowFileDetailsDialogL()"));

    iFileDetailsDialog = CMPFileDetailsDialog::NewL();

    CMPFileDetails* fileDetailsDialogParams = new (ELeave) CMPFileDetails();
    CleanupStack::PushL( fileDetailsDialogParams );

    HBufC* fileName = iFileDetails->GenerateFileNameL();

    if ( fileName && fileName->Length() )
    {
        fileDetailsDialogParams->iFileName = fileName;
    }

    if ( iFileDetails->iTitle && iFileDetails->iTitle->Length() )
    {
        fileDetailsDialogParams->iTitle = iFileDetails->iTitle->AllocL();
    }

    if ( iFileDetails->iArtist && iFileDetails->iArtist->Length() )
    {
        fileDetailsDialogParams->iArtist = iFileDetails->iArtist->AllocL();
    }

    if ( iFileDetails->iMimeType && iFileDetails->iMimeType->Length() )
    {
        fileDetailsDialogParams->iFormat = iFileDetails->iMimeType->AllocL();
    }

    if ( iFileDetails->iVideoHeight && iFileDetails->iVideoWidth )
    {
        fileDetailsDialogParams->iResolutionHeight = iFileDetails->iVideoHeight;
        fileDetailsDialogParams->iResolutionWidth = iFileDetails->iVideoWidth;
    }

    if ( iFileDetails->iDuration > 0 )
    {
        fileDetailsDialogParams->iDurationInSeconds = iFileDetails->iDuration / KPbMilliMultiplier;
    }

    if (iFileDetails->iBitRate > 0 )
    {
        fileDetailsDialogParams->iBitrate = iFileDetails->iBitRate;
    }

    if ( iFileDetails->iDrmProtected )
    {
        fileDetailsDialogParams->iDrmProtected = iFileDetails->iDrmProtected;
        fileDetailsDialogParams->iFilePath = iFileDetails->iClipName->AllocL();

        TInt openError = KErrNone;

        //
        // based on SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API flag,
        // fileDetailsDialogParams->iDetailsFileHandle uses either RFile64 or RFile handle,
        // therefore, we need to handle it in the same manner here
        //

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        RFile64 tempFileHandle;
        CleanupClosePushL( tempFileHandle );
        openError = OpenDrmFileHandle64L( tempFileHandle );

#else // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        RFile tempFileHandle;
        CleanupClosePushL( tempFileHandle );
        openError = OpenDrmFileHandleL( tempFileHandle );

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        if ( ! openError )
        {
            fileDetailsDialogParams->iDetailsFileHandle.Duplicate( tempFileHandle );
        }

        CleanupStack::PopAndDestroy();  // tempFileHandle
    }

    TRAP_IGNORE( iFileDetailsDialog->ExecuteLD( fileDetailsDialogParams ) );

    CleanupStack::PopAndDestroy(); // fileDetailsDialogParams;
    iFileDetailsDialog = NULL;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleDrmErrorsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleDrmErrorsL( TInt aError )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::HandleDrmErrorsL()"));

    //
    //  If we receive an error when we are initialized, let the DRM UI Handler
    //  process the error.  If we have been playing, display the error message
    //  and close the playback
    //
    if ( IsMultiItemPlaylist() || iPlaybackState != EPbStateInitialised )

    {
        DisplayErrorMessageL( R_MPX_DRM_RIGHTS_MISSING  );
    }
    else
    {
        TInt openError = KErrNotFound;
        RFile fileHandle;
        CleanupClosePushL( fileHandle );

        MPX_TRAPD( err, openError = OpenDrmFileHandleL( fileHandle ) );

        if ( err == KErrNone && openError == KErrNone )
        {
            DRM::CDrmUiHandling* drmUiHandling = DRM::CDrmUiHandling::NewL( iCoeEnv );
            CleanupStack::PushL( drmUiHandling );

            drmUiHandling->GetErrorHandler().HandleErrorL( fileHandle,
                                                           ContentAccess::EPlay,
                                                           aError,
                                                           NULL );

            CleanupStack::PopAndDestroy( drmUiHandling );
        }
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        else if ( err == KErrNone && openError == KErrTooBig )
        {
            RFile64 fileHandle64;
            CleanupClosePushL( fileHandle64 );

            MPX_TRAPD( err, openError = OpenDrmFileHandle64L( fileHandle64 ) );

            if ( err == KErrNone && openError == KErrNone )
            {
                DRM::CDrmUiHandling* drmUiHandling = DRM::CDrmUiHandling::NewL( iCoeEnv );
                CleanupStack::PushL( drmUiHandling );

                drmUiHandling->GetErrorHandler().HandleErrorL( fileHandle64,
                                                               ContentAccess::EPlay,
                                                               aError,
                                                               NULL );
                CleanupStack::PopAndDestroy( drmUiHandling );
            }
            else
            {
                DisplayErrorMessageL( R_MPX_DRM_RIGHTS_MISSING  );
            }

            CleanupStack::PopAndDestroy();  // fileHandle64
        }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        else
        {
            DisplayErrorMessageL( R_MPX_DRM_RIGHTS_MISSING  );
        }

        CleanupStack::PopAndDestroy();  // fileHandle
    }

    HandleClosePlaybackViewL();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::IsMultiItemPlaylist()
// -------------------------------------------------------------------------------------------------
//
TBool CMPXVideoBasePlaybackView::IsMultiItemPlaylist()
{
    TInt numItems = 1;
    MMPXSource* s = iPlaybackUtility->Source();

    if ( s )
    {
        CMPXCollectionPlaylist* playlist = NULL;

        MPX_TRAPD( err, playlist = s->PlaylistL() );

        if ( err == KErrNone && playlist )
        {
            numItems = playlist->Count();
            delete playlist;
        }
    }

    TBool retVal = ( numItems > 1 );

    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::IsMultiItemPlaylist(%d)"), retVal);

    return retVal;
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::OpenDrmFileHandleL()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoBasePlaybackView::OpenDrmFileHandleL( RFile& aFile )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::OpenDrmFileHandleL()"));

    TInt openError = KErrNotFound;
    RFs& fs = iCoeEnv->FsSession();

    if ( iPlaylistView && iClipName )
    {
        openError = aFile.Open( fs,
                                iClipName->Des(),
                                EFileRead | EFileShareReadersOrWriters );
    }
    else
    {
        MMPXSource* s = iPlaybackUtility->Source();

        if ( s )
        {
            RFile* tempFile = s->FileL();

            if ( tempFile && tempFile->SubSessionHandle() )
            {
                //
                //  File Handle from the framework is valid, use it
                //
                openError = aFile.Duplicate( *tempFile );
            }
            else
            {
                //
                //  File Handle not valid, get URI and open file
                //
                HBufC* tempFileName = s->UriL();

                if ( tempFileName )
                {
                    openError = aFile.Open( fs,
                                            tempFileName->Des(),
                                            EFileRead | EFileShareReadersOrWriters );

                    delete tempFileName;
                }
            }
        }
    }

    return openError;
}

// -------------------------------------------------------------------------------------------------
// CMPXVideoBasePlaybackView::LaunchDRMDetailsL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::LaunchDRMDetailsL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::LaunchDRMDetailsL()"));

    TInt openError = KErrNotFound;
    RFile fileHandle;
    CleanupClosePushL( fileHandle );

    MPX_TRAPD( err, openError = OpenDrmFileHandleL( fileHandle ) );

    if ( err == KErrNone )
    {
        DRM::CDrmUiHandling* drmUiHandling = DRM::CDrmUiHandling::NewL();
        CleanupStack::PushL( drmUiHandling );

        if ( openError == KErrNone )
        {
            MPX_TRAPD( err, drmUiHandling->ShowDetailsViewL( fileHandle ) );

            if ( KLeaveExit == err )
            {
                User::Leave( err );
            }
        }
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
        else if ( openError == KErrTooBig )
        {
            RFile64 fileHandle64;
            CleanupClosePushL( fileHandle64 );
            MPX_TRAPD( err, openError = OpenDrmFileHandle64L( fileHandle64 ) );

            if ( err == KErrNone && openError == KErrNone )
            {
                MPX_TRAPD( err, drmUiHandling->ShowDetailsViewL( fileHandle64 ) );

                if ( KLeaveExit == err )
                {
                    User::Leave( err );
                }
            }
            CleanupStack::PopAndDestroy(); // fileHandle64
        }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        CleanupStack::PopAndDestroy( drmUiHandling );
    }

    CleanupStack::PopAndDestroy();  // fileHandle

    //
    //  User inputs should not be blocked when user close DRM details view
    //
    ResetPdlUserInputs();
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::RetrieveBufferingPercentageL()
// -------------------------------------------------------------------------------------------------
//
TInt
CMPXVideoBasePlaybackView::RetrieveBufferingPercentageL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::RetrieveBufferingPercentageL()"));

    //
    //  create command to pass to playback plugin
    //
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );
    cmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand,
                                                     EPbCmdRetrieveBufferingPercentage );

    iPlaybackUtility->CommandL( *cmd );

    TInt buffPercentage = cmd->ValueTObjectL<TInt>( KMPXMediaVideoBufferingPercentage );

    CleanupStack::PopAndDestroy( cmd );

    return buffPercentage;
}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::OpenDrmFileHandle64L()
// -------------------------------------------------------------------------------------------------
//
TInt CMPXVideoBasePlaybackView::OpenDrmFileHandle64L( RFile64& aFile )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::OpenDrmFileHandle64L()"));

    TInt openError = KErrNotFound;
    RFs& fs = iCoeEnv->FsSession();

    if ( iPlaylistView && iClipName )
    {
        openError = aFile.Open( fs,
                                iClipName->Des(),
                                EFileRead | EFileShareReadersOrWriters );
    }
    else
    {
        MMPXSource* s = iPlaybackUtility->Source();

        if ( s )
        {

            RFile64* tempFile = s->File64L();

            if ( tempFile && tempFile->SubSessionHandle() )
            {
                //
                //  File Handle from the framework is valid, use it
                //
                openError = aFile.Duplicate( *tempFile );
            }
            else
            {
                //
                //  File Handle not valid, get URI and open file
                //
                HBufC* tempFileName = s->UriL();

                if ( tempFileName )
                {
                    openError = aFile.Open( fs,
                                            tempFileName->Des(),
                                            EFileRead | EFileShareReadersOrWriters );

                    delete tempFileName;
                }
            }
        }
    }

    return openError;
}

#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleRealOneBitmapTimeoutL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleRealOneBitmapTimeoutL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::HandleRealOneBitmapTimeoutL()"));

    //
    //  Check if this clip has video
    //  If video exists, setup the display window
    //  If the video is not enabled, redraw container to remove Real One Bitmap
    //
    if ( iFileDetails && iFileDetails->iVideoEnabled )
    {
        if ( iDisplayHandler )
        {
            iDisplayHandler->DoHandleRealOneBitmapTimeoutL();
        }
    }
    else
    {
        iContainer->DrawNow();
    }

    if ( iRealOneDelayedPlay )
    {
        CreateGeneralPlaybackCommandL( EPbCmdPlay );
        iRealOneDelayedPlay = EFalse;
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandleWsEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandleWsEventL( const TWsEvent& aEvent,
                                                CCoeControl* /*aDestination*/ )
{
    //
    //  Fg/Bg events aren't received when the view has lost keyboard focus
    //  If we are sent to full background, sent a background command to the playback plugin
    //
    if ( ! iKeyboardInFocus && aEvent.Type() == EEventWindowVisibilityChanged )
    {
        MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleWsEventL() EEventWindowVisibilityChanged"));

        TUint visible = aEvent.VisibilityChanged()->iFlags;

        if ( visible & TWsVisibilityChangedEvent::ENotVisible )
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleWsEventL() ENotVisible"));
            SendWindowCommandL( EPbCmdHandleBackground );
        }
        else if ( ! IsAppInFrontL() )
        {
            MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandleWsEventL() App in Background"));
            SendWindowCommandL( EPbCmdHandleBackground );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::SendWindowCommandL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::SendWindowCommandL( TMPXVideoPlaybackCommand aCmd )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::SendWindowCommandL()"),
                   _L("aCmd = %d"), aCmd );

    //
    //  create command to pass to playback plugin
    //
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );
    cmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand, aCmd );
    cmd->SetTObjectValueL<TBool>( KMPXMediaVideoAppForeground, IsAppInFrontL() );

    iPlaybackUtility->CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::DoHandleInitializingStateL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::DoHandleInitializingStateL( TMPXPlaybackState aLastState )
{
    MPX_ENTER_EXIT(_L("CMPXVideoBasePlaybackView::DoHandleInitializingStateL()"),
                   _L("aLastState = %d"), aLastState );

    if ( iContainer )
    {
        //
        //  If transitioning from Not Initialized to Initialising there is
        //  no need to update the playback information that was gathered
        //  when the container was created
        //
        if ( aLastState != EPbStateNotInitialised )
        {
            iMediaRequested = EFalse;

            //
            //  Refresh the screen by removing the surface and resetting the controls
            //
            iDisplayHandler->RemoveDisplayWindow();
            iContainer->HandleCommandL( EMPXPbvCmdResetControls );

            if ( iFileDetails )
            {
                if ( iFileDetailsDialog )
                {
                    iFileDetailsDialog->Close();
                }

                iFileDetails->ClearFileDetails();
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::HandlePdlReloadComplete()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::HandlePdlReloadComplete()
{
    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::HandlePdlReloadComplete()"));
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::ResetPdlUserInputs()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoBasePlaybackView::ResetPdlUserInputs()
{
    MPX_DEBUG(_L("CMPXVideoBasePlaybackView::ResetPdlUserInputs()"));
}

// EOF
