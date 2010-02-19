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
* Description:  AppUI engine implementation
*
*/

// Version : %version: da1mmcf#61 %


#include <eikon.hrh>
#include <avkon.hrh>
#include <aknview.h>
#include <mpxviewutility.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxplaybackutility.h>
#include <mpxcollectionmessage.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>
#include <mpxcollectionpath.h>
#include <videoplayeractivationmessage.h>
#include <AiwGenericParam.h>
#include <mpxcollectionuihelper.h>
#include <mpxcollectionhelperfactory.h>
#include <mpxcollectionplugin.hrh>
#include <mpxmediageneralextdefs.h>

#include <streaminglinkmodel.h>
#include <mpsettingsmodel.h>
#include <coeutils.h>
#include <videoplaylistutility.h>
#include <cmmanagerext.h>
#include <cmpluginwlandef.h>
#include <commdb.h>             // CMDBSession
#include <commsdattypesv1_1.h>  // CCDWAPIPBearerRecord
#include <es_sock.h>
#include <commdbconnpref.h>     // TCommDbConnPref
#include <extendedconnpref.h>

#include "mpxvideoplayerappuiengine.h"
#include "mpxvideoplayerlogger.h"
#include "mpxvideoplayerconstants.h"
#include "mpxvideoembeddedpdlhandler.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideo_debug.h"
#include "mpxvideoplayercustomviewmsgconsts.h"

_LIT( KMpxVideoPlayerQueryIap, "IAP\\Id" );

const TInt KMpxPlaybackPluginTypeUid = 0x101FFCA0;

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::CMpxVideoPlayerAppUiEngine
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerAppUiEngine::CMpxVideoPlayerAppUiEngine( CMpxVideoPlayerAppUi* aAppUi )
    : iAppUi( aAppUi ),
      iPlaybackUtility( NULL ),
      iViewUtility( NULL ),
      iCollectionUtility( NULL ),
      iExitAo( NULL ),
      iRecognizer( NULL ),
      iExtAccessPointId( KErrUnknown ),
      iMultilinkPlaylist(EFalse),
      iSeekable(ETrue),
      iUpdateSeekInfo(EFalse),
      iExitToMatrixMenu(ETrue)
{
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::NewL
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerAppUiEngine* CMpxVideoPlayerAppUiEngine::NewL( CMpxVideoPlayerAppUi* aAppUi )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::NewL()"));

    CMpxVideoPlayerAppUiEngine* self = new( ELeave) CMpxVideoPlayerAppUiEngine( aAppUi );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::ConstructL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ConstructL()"));

    iRecognizer = CMediaRecognizer::NewL();

    //
    //  Create the View Utility
    //
    iViewUtility = MMPXViewUtility::UtilityL();
    iViewUtility->AddObserverL( this );
    iViewUtility->ConstructDefaultViewHistoryL();

    //
    //  Create the Collection Utility
    //
    if ( ! iAppUi->IsEmbedded() )
    {
        TUid collectionMode( KUidMpxVideoPlayerApplication );

        iCollectionUtility = MMPXCollectionUtility::NewL( this, collectionMode );
    }

    //
    //  Create Active Object for exiting the application
    //
    iExitAo = CIdle::NewL( CActive::EPriorityStandard );
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::CreateCollectionUtilityMemberVariablesL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::CreateCollectionUtilityMemberVariablesL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::CreateCollectionUtilityMemberVariablesL()"));

    if ( ! iCollectionUtility )
    {
        //  PLAYLIST default mode
        iCollectionUtility = MMPXCollectionUtility::NewL( this );
    }

    if ( ! iCollectionUiHelper )
    {
        iCollectionUiHelper = CMPXCollectionHelperFactory::NewCollectionUiHelperL();
    }
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::CreatePlaybackUtilityMemberVariablesL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::CreatePlaybackUtilityMemberVariablesL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::CreatePlaybackUtilityMemberVariablesL()"));

    if ( ! iPlaybackUtility )
    {
        //
        //  Create VideoHelix playback plugin
        //
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( EMPXCategoryVideo, KPbModeNewPlayer );
        MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
        manager.SelectPlayerL( KVideoHelixPlaybackPluginUid );
        iPlaybackUtility->CommandL( EPbCmdSetAutoResume, EFalse );
    }
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::CreateEmbeddedPdlPlaybackUtilityMemberVariablesL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::CreateEmbeddedPdlPlaybackUtilityMemberVariablesL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::CreateEmbeddedPdlPlaybackUtilityMemberVariablesL()"));

    iViewUtility->PreLoadViewL( KVideoPdlPlaybackViewUid );

    if ( ! iPlaybackUtility )
    {
        iPlaybackUtility = MMPXPlaybackUtility::UtilityL( EMPXCategoryVideo, KPbModeNewPlayer );
        MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
        manager.SelectPlayerL( KVideoHelixPlaybackPluginUid );
        iPlaybackUtility->CommandL( EPbCmdSetAutoResume, EFalse );
    }
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::~CMpxVideoPlayerAppUiEngine
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerAppUiEngine::~CMpxVideoPlayerAppUiEngine()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::~CMpxVideoPlayerAppUiEngine()"));

    if ( iExitAo )
    {
        delete iExitAo;
        iExitAo = NULL;
    }

    if ( iRecognizer )
    {
        delete iRecognizer;
        iRecognizer = NULL;
    }

    if ( iPdlHandler )
    {
        delete iPdlHandler;
        iPdlHandler = NULL;
    }

    if ( iCollectionUtility )
    {
        iCollectionUtility->Close();
    }

    if ( iCollectionUiHelper )
    {
        iCollectionUiHelper->Close();
    }

    if ( iViewUtility )
    {
        iViewUtility->RemoveObserver( this );
        iViewUtility->Close();
    }

    if ( iPlaybackUtility )
    {
        TRAP_IGNORE( iPlaybackUtility->CommandL( EPbCmdClose ) );
        iPlaybackUtility->Close();
    }
}

// ---------------------------------------------------------------------------
// From MMPXViewActivationObserver.
// Handle view activation.
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleViewActivation( const TUid& /*aCurrentViewType*/,
                                                       const TUid& /*aPreviousViewType*/ )
{
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleAiwGenericParamListL()
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayerAppUiEngine::HandleAiwGenericParamListL( const CAiwGenericParamList* aParams )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleAiwGenericParamListL()"));

    TInt retVal = KErrNone;

    TInt index = 0;

    //
    //  Check if there is a terminate message.  If it exists, exit the application.
    //
    TInt32 terminateID = 0;

    const TAiwGenericParam* paramTerminate =
        aParams->FindFirst( index, EGenericParamTerminate, EVariantTypeTInt32 );

    if ( paramTerminate )
    {
        paramTerminate->Value().Get( terminateID );
    }

    if ( terminateID )
    {
        MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::HandleAiwGenericParamListL()  Terminate message received"));

        //
        //  Embedded app is being closed
        //
        iAppUi->HandleCommandL( EEikCmdExit );

        retVal = KErrCancel;
    }
    else
    {
        //
        //  Determine if these parameters are for PDL
        //
        index = 0;
        TInt32 dlId = KErrNotFound;

        const TAiwGenericParam* genParamDlId =
            aParams->FindFirst( index, EGenericParamDownloadId, EVariantTypeTInt32 );

        if ( genParamDlId )
        {
            genParamDlId->Value().Get( dlId );

            index = 0;

            const TAiwGenericParam* paramFileName =
                aParams->FindFirst( index, EGenericParamFile, EVariantTypeDesC );

            TPtrC fileName;

            //
            //  Set the filename.  The LWPlayerAppUi checks for the
            //  filename and exits if it doesn't exist
            //
            fileName.Set( paramFileName->Value().AsDes() );

            if ( ! iPdlHandler )
            {
                iPdlHandler = CMpxVideoEmbeddedPdlHandler::NewL( this );
            }

            iPdlHandler->ConnectToEmbeddedDownloadL( dlId, fileName );
        }
        else
        {
            //
            //  Check Access Point
            //
            index = 0;

            const TAiwGenericParam* genParamAccessPoint =
                aParams->FindFirst( index, EGenericParamAccessPoint, EVariantTypeTInt32 );

            if ( index >= 0 && genParamAccessPoint )
            {
                TInt32 apId = KErrUnknown;
                genParamAccessPoint->Value().Get( apId );
                iExtAccessPointId = apId;
            }
        }
    }

    return retVal;
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::OpenFileL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::OpenFileL( RFile& aFile, const CAiwGenericParamList* aParams )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::OpenFileL(RFile)"));

    TInt err = KErrNone;

    if ( aParams )
    {
        err = HandleAiwGenericParamListL( aParams );
    }

    if ( KErrNone == err && ! iPdlHandler )
    {
        //
        //  Create member variables for embedded use cases that are not PDL
        //
        CreatePlaybackUtilityMemberVariablesL();

        TFileName filename;
        aFile.FullName(filename);

        CMediaRecognizer::TMediaType mediaType = iRecognizer->IdentifyMediaTypeL( filename, aFile );

        if ( mediaType == CMediaRecognizer::ELocalRamFile ||
             mediaType == CMediaRecognizer::ELocalAsxFile )
        {
            HandleMultiLinksFileL( aFile, mediaType );
        }
        else if ( mediaType == CMediaRecognizer::ELocalSdpFile )
        {
            SetAccessPointL();
            iPlaybackUtility->InitStreamingL( aFile, iAccessPointId );
            ActivatePlaybackViewL();
        }
        else
        {
            iViewUtility->PreLoadViewL( KVideoPlaybackViewUid );
            iPlaybackUtility->InitL( aFile );
            ActivatePlaybackViewL();
        }
    }

    iRecognizer->FreeFilehandle();

    aFile.Close();
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::OpenFileL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::OpenFileL( const TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::OpenFileL()"),
                   _L("aFileName = %S"), &aFileName);

    //
    //  Create member variables for embedded use cases that are not PDL
    //
    CreatePlaybackUtilityMemberVariablesL();

    // pre load the view and initialize the playback framework
    iViewUtility->PreLoadViewL( KVideoPlaybackViewUid );

    CMediaRecognizer::TMediaType mediaType = iRecognizer->IdentifyMediaTypeL(aFileName);

    if ( mediaType == CMediaRecognizer::ELocalVideoFile )
    {
        InitializeFileL( aFileName );
    }
    // check if aFileName is a path to a ram or asx file
    //          eg. c:\\data\\videos\\ramfile.ram
    else if ( mediaType == CMediaRecognizer::ELocalRamFile ||
              mediaType == CMediaRecognizer::ELocalAsxFile )
    {
        HandleMultiLinksFileL( aFileName, mediaType );
    }
    // check if this is a url eg. rtsp://someaddress/file.3gp
    //
    else if ( iRecognizer->IsValidStreamingPrefix( aFileName ) ||
              mediaType == CMediaRecognizer::ELocalSdpFile )
    {
        InitializeStreamingLinkL( aFileName );
    }
    else
    {
        InitializeFileL( aFileName );
    }

    iRecognizer->FreeFilehandle();
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::OpenMediaL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::OpenMediaL( const CMPXMedia& aMedia )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::OpenMediaL()"));

    // Assume MPX medias are always local clips
    CMPXCollectionPath* mediaPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL( mediaPath );

    mediaPath->AppendL( KVcxUidMyVideosMpxCollection );
    mediaPath->AppendL( KVcxMvcCategoryIdAll );

    mediaPath->AppendL( aMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    mediaPath->SelectL( aMedia.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );

    CMPXCollectionPlaylist* playList = CMPXCollectionPlaylist::NewL( *mediaPath );
    CleanupStack::PushL( playList );
    playList->SetSingleItemPlaylist();
    playList->SetToFirst();

    CreatePlaybackUtilityMemberVariablesL();

    iViewUtility->PreLoadViewL( KVideoPlaybackViewUid );

    InitializePlaylistL( *playList, ETrue );

    CleanupStack::PopAndDestroy( playList );
    CleanupStack::PopAndDestroy( mediaPath );
}

// ---------------------------------------------------------------------------
//   Activate the proper playback view
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::ActivatePlaybackViewL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ActivatePlaybackViewL()"));

    if ( iUpdateSeekInfo )
    {
        //
        //  The plugin has been instantiated, update the media
        //
        UpdatePbPluginMediaL();
        iUpdateSeekInfo = EFalse;
    }

    MMPXPlayer* player = iPlaybackUtility->PlayerManager().CurrentPlayer();

    TUid pluginUid( KNullUid );
    RArray<TUid> array;

    CleanupClosePushL( array );

    if ( iPdlHandler )
    {
        array.AppendL( KVideoPdlPlaybackViewUid );
    }
    else
    {
        if ( player )
        {
            pluginUid = player->UidL();
            array.AppendL( pluginUid );
        }
    }

    iViewUtility->ActivateViewL( array );

    CleanupStack::PopAndDestroy( &array );
}

// ---------------------------------------------------------------------------
// Closed mpx components and readies to application takedown.
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::PrepareCloseMpxL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::PrepareCloseMpxL()"));

    if ( iPlaybackUtility )
    {
        ClosePlaybackPluginL();

        MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
        TRAP_IGNORE( manager.ClearSelectPlayersL() );
    }
}

// ---------------------------------------------------------------------------
// Sets AppUiEngine in stand alone "mode"
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::StartStandAloneL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::StartStandAloneL()"));

    iExitToMatrixMenu = EFalse;

    //
    //  Create the utilities for the stand alone player
    //
    CreatePlaybackUtilityMemberVariablesL();

    // Fetch the video collection UID
    RArray<TUid> uid;
    CleanupClosePushL( uid );

    uid.AppendL( TUid::Uid( KVcxMediaIdMyVideos ) );

    iVideoCollectionId = iCollectionUtility->CollectionIDL( uid.Array() );

    CleanupStack::PopAndDestroy( &uid );

    iViewUtility->SetAsDefaultViewL( KUidMyVideosViewType );
}

// -----------------------------------------------------------------------------
// Handle collection message
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL( CMPXMessage* aMessage )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL()"));

    TMPXMessageId id = *(aMessage->Value<TMPXMessageId>( KMPXMessageGeneralId ));

    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL() TMPXMessageId = 0x%08x"), id );

    if ( id == KMPXMessageGeneral )
    {
        TInt event( *( aMessage->Value<TInt> ( KMPXMessageGeneralEvent )) );
        TInt type( *( aMessage->Value<TInt> ( KMPXMessageGeneralType )) );
        TInt data( *( aMessage->Value<TInt> ( KMPXMessageGeneralData )) );

        MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL event %d, type %d, data %d"), event, type, data );

        if ( event == TMPXCollectionMessage::EPathChanged && type == EMcPathChangedByOpen )
        {
            if ( data == EMcItemOpened )
            {
                // An item was opened on the collection. Get the media attributes
                // on the item so we can initiatiate playback
                MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL: EMcItemOpened received. Can open video"));

                CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();

                CleanupStack::PushL( cPath );

                if ( cPath->Count() > 0 )
                {
                    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL: Call MediaL() on collection."));

                    RArray<TMPXAttribute> attribs;
                    CleanupClosePushL( attribs );

                    attribs.Append( KMPXMediaGeneralUri );
                    attribs.Append( KVcxMediaMyVideosDownloadId );
                    iCollectionUtility->Collection().MediaL( *cPath, attribs.Array() );

                    CleanupStack::PopAndDestroy( &attribs );
                }

                CleanupStack::PopAndDestroy( cPath );
            }
            else if ( data == EMcContainerOpened && iMultilinkPlaylist )
            {
                // start the playback for streaming playlists
                // once the playlist is opened
                iMultilinkPlaylist = EFalse;

                // The playlist was opened, initiatiate playback
                MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL: EMcContainerOpened received. initiatiate playback"));

                CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
                CleanupStack::PushL( cPath );

                if ( cPath->Count() > 0 )
                {
                    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMessageL: Call Playback utility with the playlist."));

                    CMPXCollectionPlaylist* playlist = CMPXCollectionPlaylist::NewL( *cPath );
                    CleanupStack::PushL( playlist );

                    // Set the AutoPlay mode to false
                    playlist->SetAutoPlay(EFalse);
                    // set pre init plugin to false
                    playlist->SetPreInitPlugin(EFalse);

                    InitializePlaylistL( *playlist, ETrue );
                    CleanupStack::PopAndDestroy( playlist );
                }
                CleanupStack::PopAndDestroy( cPath );
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::DoHandleCollectionMediaL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::DoHandleCollectionMediaL( const CMPXMedia& aMedia )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::DoHandleCollectionMediaL()"));

    const TDesC& fileUri = aMedia.ValueText(KMPXMediaGeneralUri);

    MPX_DEBUG(_L("Video URI: %S"), &fileUri );

    if ( aMedia.IsSupported( KVcxMediaMyVideosDownloadId ) &&
         aMedia.ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId ) != 0 )
    {
        TUint32 dlId = aMedia.ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );

        if ( ! iPdlHandler )
        {
            iPdlHandler = CMpxVideoEmbeddedPdlHandler::NewL( this );
        }

        iPdlHandler->ConnectToCollectionDownloadL( dlId, const_cast<TDesC&>( fileUri ) );
    }
    else
    {
        OpenMediaL( aMedia ); // Initialize and initiate playback of a single video
    }
}

// ---------------------------------------------------------------------------
// Steps back one level back in collection path. Will activate previous view if level exists
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::StepBackCollectionPathL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::StepBackCollectionPathL()"));

    // Back the collection path one level...

    CMPXCollectionPath* cPath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( cPath );

    // Don't back up if there are no more levels or will panic
    if ( cPath->Levels() > 1 )
    {
        cPath->Back();
        iCollectionUtility->Collection().OpenL( *cPath );
        // ... and then activate the previous view.
        iViewUtility->ActivatePreviousViewL();
    }

    CleanupStack::PopAndDestroy( cPath );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL( const TDesC& aFileName,
                                                        CMediaRecognizer::TMediaType aMediaType )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL()"),
                   _L("aFileName = %S, type = %d"), &aFileName, aMediaType );

    TBool singleLink;
    TBool localFile;

    CVideoPlaylistUtility* playlistUtil = CVideoPlaylistUtility::NewL();
    CleanupStack::PushL(playlistUtil);

    playlistUtil->GetFileInfoL( aFileName, aMediaType, singleLink, localFile );

    DoHandleMultiLinksFileL( playlistUtil, singleLink, localFile );

    CleanupStack::PopAndDestroy( playlistUtil );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL( RFile& aFile,
                                                        CMediaRecognizer::TMediaType aMediaType )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleMultiLinksFileL(RFile)"));

    // playlist
    TBool singleLink;
    TBool localFile;

    CVideoPlaylistUtility* playlistUtil = CVideoPlaylistUtility::NewL();
    CleanupStack::PushL(playlistUtil);

    playlistUtil->GetFileInfoL( aFile, aMediaType, singleLink, localFile );

    DoHandleMultiLinksFileL( playlistUtil, singleLink, localFile );

    CleanupStack::PopAndDestroy( playlistUtil );
}


void CMpxVideoPlayerAppUiEngine::DoHandleMultiLinksFileL( CVideoPlaylistUtility* aPlaylistUtil,
                                                          TBool aSingleLink,
                                                          TBool aLocalFile )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::DoHandleMultiLinksFileL()"),
                   _L("aSingleLink = %d aLocalFile %d "), aSingleLink, aLocalFile);

    if ( aSingleLink )
    {
        TPtrC link;
        link.Set( aPlaylistUtil->GetLinkLC() );
        iUpdateSeekInfo = ETrue;
        iSeekable = aPlaylistUtil->IsSeekable();

        if ( aLocalFile )
        {
            CMediaRecognizer::TMediaType mediaType = iRecognizer->IdentifyMediaTypeL( link );

            if ( mediaType == CMediaRecognizer::EUnidentified )
            {
                User::Leave( KErrNotSupported );
            }
            else
            {
                InitializeFileL( link );
            }
        }
        else
        {
            InitializeStreamingLinkL( link );
        }
        CleanupStack::PopAndDestroy();  // link
    }
    else
    {
        if ( ! aLocalFile )
        {
            SetAccessPointL();
        }

        CMPXMedia* playlist = aPlaylistUtil->GetPlayListL( iAccessPointId );
        CleanupStack::PushL( playlist );

        //  Create FW utility member variables for playlist embedded use case
        CreateCollectionUtilityMemberVariablesL();

        // Set the flag to true
        iMultilinkPlaylist = ETrue;

        // load the in memory plugin
        iCollectionUiHelper->OpenL( KUidMpxVideoPlayerApplication,
                                    *playlist,
                                    this,
                                    EMPXCollectionPluginGallery );

        CleanupStack::PopAndDestroy( playlist );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::SetAccessPointL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::SetAccessPointL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::SetAccessPointL()"));

    //
    //  a value was passed in for argument aAPId
    //
    if ( ( iExtAccessPointId != KErrUnknown ) && IsWLANAccessPointL( iExtAccessPointId ) )
    {
        //
        //  An external WLAN access point was passed in by an embedding app, use it
        //
        iAccessPointId = iExtAccessPointId;
    }
    else
    {
        //
        //  No access point was passed in or it's not WLAN, retrieve the default access point
        //
        TInt defaultAPId(0);

        MPX_TRAPD( err, defaultAPId = GetDefaultAccessPointL() );

        if ( ( err == KErrNone ) && ( defaultAPId != 0 ))
        {
            //
            //  Use the default access point
            //
            iAccessPointId = defaultAPId;
        }
        else
        {
            //
            //  Valid default access point was not found
            //
            if ( iExtAccessPointId != KErrUnknown )
            {
                //
                //  Use the AP passed in by embedding app, regardless of bearer type
                //
                iAccessPointId = iExtAccessPointId;
            }
            else
            {
                //
                //  Try to fetch AP ID and open connection selection dialog if required
                //
                TUint32 apUid = TryToGetAccessPointL();

                //
                //  Use selected access point
                //
                if ( apUid != 0 )
                {
                    //
                    //  convert the AccessPoint-Uid to AccessPoint-Id
                    //
                    iAccessPointId = GetAccessPointIdForUIDL( apUid );
                }
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::IsWLANAccessPointL
// -------------------------------------------------------------------------------------------------
//
TBool CMpxVideoPlayerAppUiEngine::IsWLANAccessPointL( TInt aAPId )
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::IsWLANAccessPointL(%d)"), aAPId);

    TBool wlanAP(EFalse);
    TUint32 bearer( 0 );

    RCmManagerExt cmManager;
    CleanupClosePushL( cmManager );
    cmManager.OpenL();

    bearer = cmManager.ConnectionMethodL( aAPId ).GetIntAttributeL( CMManager::ECmBearerType );
    CleanupStack::PopAndDestroy( &cmManager );

    if ( bearer == KUidWlanBearerType )
    {
        wlanAP = ETrue;
    }

    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::IsWLANAccessPointL() return %d"), wlanAP);

    return wlanAP;
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::GetDefaultAccessPointL
// -----------------------------------------------------------------------------
//
TInt CMpxVideoPlayerAppUiEngine::GetDefaultAccessPointL()
{
    TInt defaultAP(0);
    TUint32 iap;

    CMPSettingsModel* ropSettings = CMPSettingsModel::NewL( KSettingsModelForROPUid );

    CleanupStack::PushL( ropSettings );

    ropSettings->LoadSettingsL( EConfigDefault );

    User::LeaveIfError( ropSettings->GetDefaultAp( iap ) );

    CleanupStack::PopAndDestroy(); // ropSettings

    defaultAP = GetAccessPointIdForUIDL( iap );

    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::GetDefaultAccessPointL(%d)"), defaultAP);

    return defaultAP;
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::GetAccessPointIdForUIDL
// -----------------------------------------------------------------------------
//
TInt CMpxVideoPlayerAppUiEngine::GetAccessPointIdForUIDL( TUint32 aAPUid )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::GetAccessPointIdForUIDL()"),
                   _L(" aAPUid = %d"), aAPUid);

    TInt apId(0);

    CMDBSession* db = CMDBSession::NewL( CMDBSession::LatestVersion() );
    CleanupStack::PushL( db );

    //
    //  WapIpBearer table contains the mapping between wap and iap id's.
    //
    CCDWAPIPBearerRecord* wapBearerRecord =
        static_cast<CCDWAPIPBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWAPIPBearerRecord));

    CleanupStack::PushL( wapBearerRecord );

    wapBearerRecord->iWAPAccessPointId = aAPUid;

    TBool found = wapBearerRecord->FindL( *db );

    if ( ! found )
    {
        User::Leave( KErrNotFound );
    }

    apId = static_cast<TUint32>( wapBearerRecord->iWAPIAP );

    CleanupStack::PopAndDestroy( wapBearerRecord );
    CleanupStack::PopAndDestroy( db );

    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::GetAccessPointIdForUIDL(%d)"), apId);

    return apId;
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::TryToGetAccessPointL
// -----------------------------------------------------------------------------
//
TUint32 CMpxVideoPlayerAppUiEngine::TryToGetAccessPointL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::TryToGetAccessPointL()"));

    TUint32 returnVal(0);

    RSocketServ ss;
    CleanupClosePushL( ss );

    User::LeaveIfError( ss.Connect() );

    RConnection conn;
    CleanupClosePushL( conn );

    User::LeaveIfError( conn.Open( ss ) );

    TConnPrefList prefList;
    TExtendedConnPref prefs;
    prefs.SetSnapPurpose( CMManager::ESnapPurposeInternet  );
    prefs.SetNoteBehaviour( TExtendedConnPref::ENoteBehaviourConnSilent );
    prefList.AppendL( &prefs );

    TInt err = conn.Start( prefList );

    if ( err == KErrNone )
    {
        returnVal = QueryIap( conn );
    }
    else if ( err == KErrNotFound )
    {
        //
        //  SNAP is empty or no WLAN was available.
        //  Try again with connection selection dialog.
        //
        MPX_DEBUG(_L("    connection start returned KErrNotFound"));

        TConnPrefList prefList2;
        TExtendedConnPref prefs2;

        prefs2.SetConnSelectionDialog( ETrue );
        prefList2.AppendL( &prefs2 );

        err = conn.Start( prefList2 );

        if ( err == KErrNone )
        {
            returnVal = QueryIap( conn );
        }
        else
        {
            User::Leave( err );
        }
    }
    else
    {
        User::Leave( err );
    }

    CleanupStack::PopAndDestroy( &conn );

    CleanupStack::PopAndDestroy( &ss );

    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::TryToGetAccessPointL() AP ID: %d"), returnVal);

    return returnVal;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::QueryIap()
// -------------------------------------------------------------------------------------------------
//
TUint32 CMpxVideoPlayerAppUiEngine::QueryIap( RConnection& aConn )
{
    TUint32 iap( 0 );
    aConn.GetIntSetting( KMpxVideoPlayerQueryIap, iap );
    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::QueryIap(%u)"), iap);
    return iap;
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::ProcessActivationMessageL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::ProcessActivationMessageL( const TDesC8 &aMsg )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ProcessActivationMessageL()"));

    if ( aMsg.Length() < KVideoPlayerActivationMessageMinLength )
    {
        User::Leave( KErrNotSupported );
    }

    TVideoPlayerActivationMessage msgHandler;
    TPckg<TVideoPlayerActivationMessage> paramsPckg( msgHandler );
    paramsPckg.Copy( aMsg );

    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::ProcessActivationMessageL() sender = %d, type = %d"),
        msgHandler.iMsgSender, msgHandler.iMsgType );

    if ( msgHandler.iMsgSender == TVideoPlayerActivationMessage::EMatrixMenu )
    {
        // Clear the view history, so app exits back to matrix.
        iViewUtility->PushDefaultHistoryL();
    }

    if ( msgHandler.iMsgType == TVideoPlayerActivationMessage::EOpenInternetVideos )
    {
        iViewUtility->ActivateViewL( TUid::Uid( KMpxVideoPlayerVodViewPluginTypeId ) );
    }
    else if ( msgHandler.iMsgType == TVideoPlayerActivationMessage::EOpenVideoStorage )
    {
        iViewUtility->ActivateViewL( TUid::Uid( KUidMyVideosViewTypeId ) );
    }
    else if ( msgHandler.iMsgType == TVideoPlayerActivationMessage::ELaunchVideoToPlayer )
    {
        //
        //  Launch video to player, it can be either local video or stream.
        //  If there's path and mpx id, clip can be played directly.
        //
        if ( msgHandler.iServiceId && msgHandler.iFullPath.Length() > 0 )
        {
            if ( iRecognizer->IdentifyMediaTypeL( msgHandler.iFullPath )
                  ==  CMediaRecognizer::ELocalVideoFile )
            {
                TMPXItemId id;
                id.iId1 = msgHandler.iServiceId;
                CMPXMedia* media = CMPXMedia::NewL();
                CleanupStack::PushL( media );
                media->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, id );
                OpenMediaL( *media );
                CleanupStack::PopAndDestroy( media );
            }
            else
            {
                // Most likely the clip has been deleted
                User::Leave( KErrNotFound );
            }
        }
        else
        {
            TBool vodUiRunning = ( iViewUtility->ActiveViewType() ==
                                   TUid::Uid( KMpxVideoPlayerVodViewPluginTypeId ) );

            if ( vodUiRunning )
            {
                // VOD is active, must use the utility to pass the message.
                SendCustomCommandToActiveViewL( aMsg );
            }
            else
            {
                // VOD is not running, activate with custom message.
                HBufC* customMsg = HBufC::NewLC( paramsPckg.Length() );
                customMsg->Des().Copy( paramsPckg );
                iViewUtility->ActivateViewL( TUid::Uid( KMpxVideoPlayerVodViewPluginTypeId ), customMsg );
                CleanupStack::PopAndDestroy( customMsg );

                // Clear the view history, so playback returns where it was started
                iViewUtility->PushDefaultHistoryL();
            }
        }
    }
    else if ( msgHandler.iMsgType == TVideoPlayerActivationMessage::ELaunchServiceById )
    {
        if ( iViewUtility->ActiveViewType() == TUid::Uid( KMpxVideoPlayerVodViewPluginTypeId ) )
        {
            // VOD is active, must use the utility to pass the message.
            SendCustomCommandToActiveViewL( aMsg );
        }
        else
        {
            // VOD is not running, activate with custom message.
            HBufC* customMsg = HBufC::NewLC( paramsPckg.Length() );
            customMsg->Des().Copy( paramsPckg );
            iViewUtility->ActivateViewL( TUid::Uid( KMpxVideoPlayerVodViewPluginTypeId ), customMsg );
            CleanupStack::PopAndDestroy( customMsg );
        }
    }
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleMessageL()
// -----------------------------------------------------------------------------
//
TBool CMpxVideoPlayerAppUiEngine::HandleMessageL( TUid aMessageUid,
                                                  const TDesC8& aMessageParameters )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleMessageL()"));

    TBool msgHandled = EFalse;

    switch ( aMessageUid.iUid )
    {
        // TVideoPlayerActivationMessage received.
        case KVideoPlayerVodStartPlayer:
        {
            //
            //  Do nothing if we are loaded in a playback view
            //
            if ( iViewUtility->ActiveViewType() != TUid::Uid( KMpxPlaybackPluginTypeUid ) )
            {
                ProcessActivationMessageL( aMessageParameters );
            }

            msgHandled = ETrue;

            break;
        }
        case KVideoPlayerStartPDlPlayer:
        {
            // Start the PDl
            if ( iPdlHandler )
            {
                CBufFlat* paramBuf = CBufFlat::NewL( 1 );
                CleanupStack::PushL( paramBuf );
                paramBuf->InsertL( 0, aMessageParameters );
                RBufReadStream readStream( *paramBuf );

                CAiwGenericParamList* genParamList = CAiwGenericParamList::NewLC( readStream );

                HandleAiwGenericParamListL( genParamList );

                CleanupStack::PopAndDestroy( genParamList );
                CleanupStack::PopAndDestroy( paramBuf );

                msgHandled = ETrue;
            }

            break;
        }
    }

    return msgHandled;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleCollectionMessage()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleCollectionMessage()"));

    if ( aError == KErrNone && aMessage )
    {
        TRAP_IGNORE( DoHandleCollectionMessageL( aMessage ) )
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleCollectionMediaL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleCollectionMediaL( const CMPXMedia& aMedia, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleCollectionMediaL()"));

    if ( aError == KErrNone )
    {
        DoHandleCollectionMediaL( aMedia );
    }
}

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver.
// Handles the collection entries being opened.
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleOpenL( const CMPXMedia& /* aEntries */,
                                              TInt /* aIndex */,
                                              TBool /* aComplete */,
                                              TInt aError )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleOpenL(CMPXMedia)"));

    if ( aError != KErrNone )
    {
        //
        //  Clip the collection path to root level and reopen
        //  This could happen after a USB refresh event where the
        //  item no longer exists
        //
        CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();

        CleanupStack::PushL( path );

        while( path->Levels() > 1 )
        {
            path->Back();
        }

        iCollectionUtility->Collection().OpenL( *path );
        CleanupStack::PopAndDestroy( path );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::HandleOpenL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, TInt aError )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleOpenL(CMPXCollectionPlaylist)"));

    if ( aError == KErrNone )
    {
        InitializePlaylistL( aPlaylist, EFalse );
    }
}


// ---------------------------------------------------------------------------
// From MMPXCHelperEmbeddedOpenObserver
// Handles OpenL from that occured in embedded mode
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::HandleEmbeddedOpenL( TInt aErr, TMPXGeneralCategory /*aCategory*/ )
{
    MPX_DEBUG(_L("CMPXVideoAppUi::HandleEmbeddedOpenL()"));

    if ( aErr == KErrNone )
    {
        iPlaybackUtility->CommandL( EPbCmdDisableEffect );
    }
}


void CMpxVideoPlayerAppUiEngine::HandleSoftKeyBackL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::HandleSoftKeyBackL()"));

    //
    //  if we're in top level of our local view stack,
    //  back command exits the app.  otherwise activate
    //  the previous view in our local view stack
    //
    if ( iViewUtility->ViewHistoryDepth() <= 1 )
    {
        iExitToMatrixMenu = EFalse;
        ActivateExitActiveObject();
    }
    else
    {
        StepBackCollectionPathL();
    }
}

TBool CMpxVideoPlayerAppUiEngine::ProcessCommandParametersL( TApaCommand aCommand,
                                                             TFileName& aDocumentName,
                                                             const TDesC8& aTail )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ProcessCommandParametersL()"));

    TBool retVal = EFalse;

    //
    //  If command line has tail, this is probably a message from Matrix, Live TV
    //  reminder, Video Center soft notification, or other VC component.
    //
    if ( aTail.Length() > 0 )
    {
        ProcessActivationMessageL( aTail );
    }
    else
    {
        //
        //  If we are embedded or the command is to open a document
        //
        if ( iAppUi->IsEmbedded() || aCommand != EApaCommandRun )
        {
            if ( iRecognizer->IsValidStreamingPrefix( aDocumentName ) )
            {
                retVal = ETrue;
            }
            else
            {
                retVal = ConeUtils::FileExists( aDocumentName );
            }
        }
        else
        {
            StartStandAloneL();
        }
    }

    return retVal;
}

void CMpxVideoPlayerAppUiEngine::ActivateExitActiveObject()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ActivateExitActiveObject()"));

    if ( ! iExitAo->IsActive() )
    {
        iExitAo->Start( TCallBack( CMpxVideoPlayerAppUiEngine::ExitApplicationL, this ) );
    }
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::ExitApplicationL
// -----------------------------------------------------------------------------
//
TInt CMpxVideoPlayerAppUiEngine::ExitApplicationL( TAny* aPtr )
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::ExitApplicationL()"));

    static_cast<CMpxVideoPlayerAppUiEngine*>(aPtr)->DoExitApplicationL();
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::DoExitApplicationL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::DoExitApplicationL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::DoExitApplicationL()"));

    iAppUi->HandleCommandL( EEikCmdExit );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::UpdatePbPluginMedia()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::UpdatePbPluginMediaL()
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::UpdatePbPluginMediaL()iSeekable %d"), iSeekable);

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL<TBool>( KMPXCommandGeneralDoSync, ETrue );

    cmd->SetTObjectValueL<TInt>( KMPXCommandGeneralId, KMPXMediaIdVideoPlayback );

    cmd->SetTObjectValueL<TMPXVideoPlaybackCommand>( KMPXMediaVideoPlaybackCommand,
                                                     EPbCmdUpdateSeekable );

    cmd->SetTObjectValueL<TBool>( KMPXMediaGeneralExtVideoSeekable, iSeekable );

    iPlaybackUtility->CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoBasePlaybackView::ViewHistoryDepth()
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayerAppUiEngine::ViewHistoryDepth()
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::ViewHistoryDepth()"));

    return ( iViewUtility->ViewHistoryDepth() );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::SendCustomCommandToActiveViewL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::SendCustomCommandToActiveViewL( const TDesC8& aMsg )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::SendCustomCommandToActiveViewL()"));

    User::LeaveIfNull( iAppUi->View() );

    HBufC8* msgBuf = aMsg.AllocLC();

    TAny* ptr = (TAny*) msgBuf;

    UserSvr::DllSetTls( KVcxCustomViewMessagePtr, ptr );

    iAppUi->View()->HandleCommandL( KVcxCustomViewMessageCmd );

    UserSvr::DllFreeTls( KVcxCustomViewMessagePtr );

    CleanupStack::PopAndDestroy( msgBuf );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::ClearPdlInformation()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::ClearPdlInformation()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ClearPdlInformation()"));

    if ( iPdlHandler )
    {
        iPdlHandler->ClearPdlInformation();
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::StartedStandAlone()
// -------------------------------------------------------------------------------------------------
//
TBool CMpxVideoPlayerAppUiEngine::ExitToMatrixMenu()
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUiEngine::ExitToMatrixMenu(%d)"), iExitToMatrixMenu);
    return iExitToMatrixMenu;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::InitializeStreamingLinkL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::InitializeStreamingLinkL( const TDesC& aUri )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::InitializeStreamingLinkL()"),
                   _L("aUri = %S"), &aUri );

    SetAccessPointL();

    iPlaybackUtility->InitStreamingL( aUri,
                                      (TDesC8*)(&KDATATYPEVIDEOHELIX),
                                      iAccessPointId );

    ActivatePlaybackViewL();
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::InitializeFileL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::InitializeFileL( const TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::InitializeFileL()"),
                   _L("aFileName = %S"), &aFileName );

    iPlaybackUtility->InitL( aFileName );

    ActivatePlaybackViewL();
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::InitializePlaylistL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::InitializePlaylistL( const CMPXCollectionPlaylist& aPlaylist,
                                                      TBool aPlay )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::InitializePlaylistL()"));

    iPlaybackUtility->InitL( aPlaylist, aPlay );

    ActivatePlaybackViewL();
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::ClosePlaybackPluginL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::ClosePlaybackPluginL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ClosePlaybackPluginL"));

    if ( iViewUtility->ActiveViewType() == TUid::Uid( KMpxPlaybackPluginTypeUid ) )
    {
        //
        //  The display window must be removed before closing the playback plugin
        //
        iAppUi->View()->HandleCommandL( EAknSoftkeyClose );
    }

    iPlaybackUtility->CommandL( EPbCmdClose );
}


// EOF
