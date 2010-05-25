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

// Version : %version: e92_71 %


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
#include <coeutils.h>
#include <videoplaylistutility.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>

#include "mpxvideoplayerappuiengine.h"
#include "mpxvideoplayerlogger.h"
#include "mpxvideoplayerconstants.h"
#include "mpxvideoembeddedpdlhandler.h"
#include <mpxvideoplaybackdefs.h>
#include "mpxvideo_debug.h"
#include "mpxvideoplayercustomviewmsgconsts.h"

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
      iAccessPointId( KUseDefaultIap ),
      iMultilinkPlaylist(EFalse),
      iSeekable(ETrue),
      iUpdateSeekInfo(EFalse)
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
    //  Create the playback utility to reduce startup time for embedded cases
    //
    else
    {
        PlaybackUtilityL();
    }

    //
    //  Create Active Object for exiting the application
    //
    iExitAo = CIdle::NewL( CActive::EPriorityStandard );
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::PlaybackUtilityL
// -----------------------------------------------------------------------------
//
MMPXPlaybackUtility& CMpxVideoPlayerAppUiEngine::PlaybackUtilityL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::PlaybackUtilityL()"));

    if ( ! iPlaybackUtility )
    {
        //
        //  Create VideoHelix playback plugin
        //
        MMPXPlaybackUtility* playbackUtility =
            MMPXPlaybackUtility::UtilityL( EMPXCategoryVideo, KPbModeNewPlayer );
        MMPXPlayerManager& manager = playbackUtility->PlayerManager();
        TRAPD( err,
        {
            manager.SelectPlayerL( KVideoHelixPlaybackPluginUid );
            playbackUtility->CommandL( EPbCmdSetAutoResume, EFalse );
        } );
        if ( err == KErrNone )
        {
            iPlaybackUtility = playbackUtility;
        }
        else
        {
            TRAP_IGNORE( manager.ClearSelectPlayersL() );
            playbackUtility->Close();
            User::Leave( err );
        }
    }

    return *iPlaybackUtility;
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
//   CMpxVideoPlayerAppUiEngine::PreLoadPdlPlaybackViewL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::PreLoadPdlPlaybackViewL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::PreLoadPdlPlaybackViewL()"));

    iViewUtility->PreLoadViewL( KVideoPdlPlaybackViewUid );
}

// -----------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::~CMpxVideoPlayerAppUiEngine
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerAppUiEngine::~CMpxVideoPlayerAppUiEngine()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::~CMpxVideoPlayerAppUiEngine()"));

    if ( iConstructTimer )
    {
        delete iConstructTimer;
        iConstructTimer = NULL;
    }

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
        MMPXPlayerManager& manager = iPlaybackUtility->PlayerManager();
        TRAP_IGNORE( manager.ClearSelectPlayersL() );
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
                iAccessPointId = apId;
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
            PlaybackUtilityL().InitStreamingL( aFile, iAccessPointId );
            ActivatePlaybackViewL();
        }
        else
        {
            iViewUtility->PreLoadViewL( KVideoPlaybackViewUid );
            PlaybackUtilityL().InitL( aFile );
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
    //  pre load the view and initialize the playback framework
    //
    iViewUtility->PreLoadViewL( KVideoPlaybackViewUid );

    CMediaRecognizer::TMediaType mediaType = iRecognizer->IdentifyMediaTypeL(aFileName);

    if ( mediaType == CMediaRecognizer::ELocalVideoFile )
    {
        InitializeFileL( aFileName );
    }
    //  check if aFileName is a path to a ram or asx file
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

    MMPXPlayer* player = PlaybackUtilityL().PlayerManager().CurrentPlayer();

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
// Sets AppUiEngine in stand alone "mode"
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::StartStandAloneL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::StartStandAloneL()"));

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
        // We use file name to initialize playback instead of mpxmedia object.
        iViewUtility->PreLoadViewL( KVideoPlaybackViewUid );
        InitializeFileL( fileUri );
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

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::DoHandleMultiLinksFileL()
// -------------------------------------------------------------------------------------------------
//
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
            //  Do nothing
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
        PlaybackUtilityL().CommandL( EPbCmdDisableEffect );
    }
}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
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
        ActivateExitActiveObject();
    }
    else
    {
        StepBackCollectionPathL();
    }
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool CMpxVideoPlayerAppUiEngine::ProcessCommandParametersL( TApaCommand aCommand,
                                                             TFileName& aDocumentName,
                                                             const TDesC8& /*aTail*/ )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ProcessCommandParametersL()"));

    TBool retVal = EFalse;

    //
    //  If we are embedded or the command is to open a document
    //
    if ( iAppUi->IsEmbedded() || ( aCommand != EApaCommandRun && aDocumentName.Length() > 0 ) )
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

    return retVal;
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::ActivateExitActiveObject()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ActivateExitActiveObject()"));

    if ( ! iExitAo->IsActive() )
    {
        iExitAo->Start( TCallBack( CMpxVideoPlayerAppUiEngine::ExitApplicationL, this ) );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::ExitApplicationL
// -------------------------------------------------------------------------------------------------
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

    PlaybackUtilityL().CommandL( *cmd );

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
//   CMpxVideoPlayerAppUiEngine::InitializeStreamingLinkL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::InitializeStreamingLinkL( const TDesC& aUri )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::InitializeStreamingLinkL()"),
                   _L("aUri = %S"), &aUri );

    PlaybackUtilityL().InitStreamingL( aUri,
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

    PlaybackUtilityL().InitL( aFileName );

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

    PlaybackUtilityL().InitL( aPlaylist, aPlay );

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
    else
    {
        if ( iPlaybackUtility )
        {
            iPlaybackUtility->CommandL( EPbCmdClose );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::SignalViewPdlReloadingL()
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::SignalViewPdlReloadingL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::SignalViewPdlReloadingL"));

    if ( iViewUtility->ActiveViewType() == TUid::Uid( KMpxPlaybackPluginTypeUid ) )
    {
        //
        //  The display window must be removed before closing the playback plugin
        //
        iAppUi->View()->HandleCommandL( KMpxVideoPlaybackPdlReloading );
    }
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUiEngine::ActivateLateConstructTimerL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::ActivateLateConstructTimerL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::ActivateLateConstructTimerL()"));
	
    if ( !iConstructTimer )
    {
        const TTimeIntervalMicroSeconds32 timeout  = 250000; // 250 ms
    
        TCallBack callback ( CMpxVideoPlayerAppUiEngine::LateConstructCallback, this );
    
        iConstructTimer = CPeriodic::NewL( CActive::EPriorityIdle );

        iConstructTimer->Start( timeout, 0, callback );
    }
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::LateConstructCallback
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayerAppUiEngine::LateConstructCallback( TAny* aPtr )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::LateConstructCallback()"));
    
    TRAP_IGNORE( static_cast<CMpxVideoPlayerAppUiEngine*>(aPtr)->DoLateConstructL() );
    
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayerAppUiEngine::DoLateConstructL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUiEngine::DoLateConstructL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUiEngine::DoLateConstructL()"));

    iConstructTimer->Cancel(); 
        
    PlaybackUtilityL();
}

// EOF
