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
* Description:  mpxvideoplayerappuiengine test class for STIF testing
*
*/

// Version : %version: ou1cpsw#e92_23 %


#include "mpxvideoplayer_appuienginetester.h"
#include "mpxviewutilityimp_stub.h"
#include "mpxplaybackutilityimp_stub.h"
#include "mpxvideoplayerappui_stub.h"
#include "mpxvideoplayerappuiengine.h"
#include "mpxvideoplayerconstants.h"
#include "mpxcollectionutilityimp_stub.h"

#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionmessage.h>
#include <f32file.h>
#include <videoplayeractivationmessage.h>
#include <bautils.h>
#include <vcxmyvideosdefs.h>

#include "mpxvideo_debug.h"


#define KMPXPLAYBACKPLUGINTYPEUID 0x101FFCA0
#define EMatrix 4

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::NewL
// -------------------------------------------------------------------------------------------------
//
CMpxVideoPlayer_AppUiEngineTester*
CMpxVideoPlayer_AppUiEngineTester::NewL( CScriptBase* aScriptBase )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::NewL()"));

    CMpxVideoPlayer_AppUiEngineTester* self =
        new (ELeave) CMpxVideoPlayer_AppUiEngineTester( aScriptBase );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayer_AppUiEngineTester::~CMpxVideoPlayer_AppUiEngineTester
//
// -----------------------------------------------------------------------------
//
CMpxVideoPlayer_AppUiEngineTester::~CMpxVideoPlayer_AppUiEngineTester()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::~CMpxVideoPlayer_AppUiEngineTester()"));

    delete iAppUiEngine;
    iAppUiEngine = NULL;

    delete iAppUi;
    iAppUi = NULL;

    if ( iViewUtility )
    {
        static_cast<CMPXViewUtility*>(iViewUtility)->RemoveStifObserver();
        iViewUtility->Close();
    }

    if ( iPlaybackUtility )
    {
        static_cast<CMPXPlaybackUtility*>(iPlaybackUtility)->RemoveStifObserverL();
        iPlaybackUtility->Close();
    }

    if ( iCollectionUtility )
    {
        iCollectionUtility->RemoveStifObserverL();
        iCollectionUtility->Close();
    }

    if ( iGenericParamList )
    {
        iGenericParamList->Reset();
        delete iGenericParamList;
    }

    iExpectedEventArray->ResetAndDestroy();

    delete iTimeoutController;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::ConstructL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayer_AppUiEngineTester::ConstructL()
{
    iExpectedEventArray = new (ELeave) CArrayPtrFlat<TCallbackEvent>( 1 );

    iViewUtility = MMPXViewUtility::UtilityL();
    static_cast<CMPXViewUtility*>(iViewUtility)->AddStifObserverL( this );

    iPlaybackUtility = MMPXPlaybackUtility::UtilityL( EMPXCategoryVideo, KPbModeNewPlayer );
    static_cast<CMPXPlaybackUtility*>(iPlaybackUtility)->AddStifObserverL( this );
    iPlaybackUtility->CommandL( EPbCmdSetAutoResume, EFalse );

    iCollectionUtility = CMPXCollectionUtility::NewL( KNullUid, NULL );
    iCollectionUtility->AddStifObserverL( this );
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayer_AppUiEngineTester::CMpxVideoPlayer_AppUiEngineTester
//
// -----------------------------------------------------------------------------
//
CMpxVideoPlayer_AppUiEngineTester::CMpxVideoPlayer_AppUiEngineTester( CScriptBase* aScriptBase )
    : iScriptBase( aScriptBase )
{
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::CreateEngineL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::CreateEngineL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::CreateEngineL()"));

    TBool embedded;

    User::LeaveIfError( aItem.GetNextInt( embedded ) );

    iAppUi = new (ELeave) CMpxVideoPlayerAppUi( embedded, this );

    iAppUiEngine = CMpxVideoPlayerAppUiEngine::NewL( iAppUi );

    return KErrNone;
}

TInt CMpxVideoPlayer_AppUiEngineTester::CreateGenericParametersTerminateL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::CreateGenericParametersTerminateL()"));

    if ( iGenericParamList )
    {
        iGenericParamList->Reset();
        delete iGenericParamList;
    }

    iGenericParamList = CAiwGenericParamList::NewL();

    TAiwVariant paramTerminate( ETrue );
    TAiwGenericParam genericParam( EGenericParamTerminate, paramTerminate );
    iGenericParamList->AppendL( genericParam );

    TCallbackEvent* event = new (ELeave) TCallbackEvent;
    event->iEvent = EAppUiCmdExit;
    AddExpectedEvent( event );

    return KErrNone;
}

void CMpxVideoPlayer_AppUiEngineTester::AddLocalFileCallbacksL( TDesC& aFileName,
                                                                TBool aUseFileHandle )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::AddLocalFileCallbacksL()"));

    TCallbackEvent* event = new (ELeave) TCallbackEvent;
    event->iEvent = EViewUtilityPreLoadView;
    event->iUid   = KVideoPlaybackViewUid;
    AddExpectedEvent( event );

    event = new (ELeave) TCallbackEvent;
    event->iFileName = aFileName;

    if ( aUseFileHandle )
    {
        event->iEvent = EPlaybackUtilityInitFileHandle;
    }
    else
    {
        event->iEvent = EPlaybackUtilityInitFileName;
        event->iFileName = aFileName;
    }

    AddExpectedEvent( event );
}

void CMpxVideoPlayer_AppUiEngineTester::AddCollectionMediaCallbacksL( TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::AddCollectionMediaCallbacksL()"));

    TCallbackEvent* event = new (ELeave) TCallbackEvent;
    event->iEvent = EViewUtilityPreLoadView;
    event->iUid   = KVideoPlaybackViewUid;
    AddExpectedEvent( event );

    event = new (ELeave) TCallbackEvent;
    event->iEvent = EPlaybackUtilityInitFileName;
    event->iFileName = aFileName;

    AddExpectedEvent( event );
}

void CMpxVideoPlayer_AppUiEngineTester::AddSdpFileCallbacksL( TDesC& aFileName,
                                                              TBool aUseFileHandle,
                                                              CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::AddSdpFileCallbacksL()"));

    TInt expectedApId;

    User::LeaveIfError( aItem.GetNextInt( expectedApId ) );

    //
    //  We should get 1 callback
    //  iPlaybackUtility->InitStreamingL( iFile / aUri )
    //
    TCallbackEvent* event = new (ELeave) TCallbackEvent;

        event->iFileName = aFileName;
        event->iApId     = expectedApId;

    if ( aUseFileHandle )
    {
        event->iEvent = EPlaybackUtilityInitStreamingFileHandle;
    }
    else
    {
        event->iEvent = EPlaybackUtilityInitStreamingUrl;
    }

    AddExpectedEvent( event );
}

void CMpxVideoPlayer_AppUiEngineTester::AddRamFileCallbacksL( TDesC& /*aFileName*/,
                                                              TBool /*aUseFileHandle*/,
                                                              CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::AddRamFileCallbacksL()"));

    _LIT( KRamStreamingLink, "rtsp://stream.3gp" );

#ifdef __WINSCW__
    _LIT( KRamLocalLink, "c:\\data\\videos\\local.3gp" );
#else
    _LIT( KRamLocalLink, "f:\\testing\\data\\local.3gp" );
#endif

    TInt  expectedApId;
    TBool streamLinksExist;

    User::LeaveIfError( aItem.GetNextInt( expectedApId ) );
    User::LeaveIfError( aItem.GetNextInt( streamLinksExist ) );

    //
    //  We should get 1 callback
    //  iPlaybackUtility->InitStreamingL( iFile );
    //
    TCallbackEvent* event = new (ELeave) TCallbackEvent;

    if ( streamLinksExist )
    {
        event->iEvent    = EPlaybackUtilityInitStreamingUrl;
        event->iFileName = KRamStreamingLink;
        event->iApId     = expectedApId;
    }
    else
    {
        event->iEvent    = EPlaybackUtilityInitFileName;
        event->iFileName = KRamLocalLink;
    }

    event->iApId     = expectedApId;

    AddExpectedEvent( event );
}

void CMpxVideoPlayer_AppUiEngineTester::AddAsxFileCallbacksL( TDesC& /*aFileName*/,
                                                              TBool /*aUseFileHandle*/,
                                                              CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::AddRamFileCallbacksL()"));

    _LIT( KAsxStreamingLink, "http://10.48.2.56:443/stream.wmv" );

#ifdef __WINSCW__
    _LIT( KAsxLocalLink, "c:\\data\\videos\\local.3gp" );
#else
    _LIT( KAsxLocalLink, "f:\\testing\\data\\local.3gp" );
#endif

    TInt  expectedApId;
    TBool streamLinksExist;

    User::LeaveIfError( aItem.GetNextInt( expectedApId ) );
    User::LeaveIfError( aItem.GetNextInt( streamLinksExist ) );

    //
    //  We should get 1 callback
    //  iPlaybackUtility->InitStreamingL( iFile );
    //
    TCallbackEvent* event = new (ELeave) TCallbackEvent;

    if ( streamLinksExist )
    {
        event->iEvent    = EPlaybackUtilityInitStreamingUrl;
        event->iFileName = KAsxStreamingLink;
        event->iApId     = expectedApId;
    }
    else
    {
        event->iEvent    = EPlaybackUtilityInitFileName;
        event->iFileName = KAsxLocalLink;
    }

    event->iApId     = expectedApId;

    AddExpectedEvent( event );
}

void CMpxVideoPlayer_AppUiEngineTester::AddUrlCallbacksL( TDesC& aFileName,
                                                          CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::AddUrlCallbacksL()"));


    TInt expectedApId;

    User::LeaveIfError( aItem.GetNextInt( expectedApId ) );

    //
    //  We should get 1 callback
    //  iPlaybackUtility->InitStreamingL( aUri )
    //
    TCallbackEvent* event = new (ELeave) TCallbackEvent;

    event->iEvent    = EPlaybackUtilityInitStreamingUrl;
    event->iFileName = aFileName;
    event->iApId     = expectedApId;

    AddExpectedEvent( event );
}

TInt CMpxVideoPlayer_AppUiEngineTester::OpenFileL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::OpenFileL()"));

    iError = KErrNone;

    TBool useFileHandle;
    TInt fileType;

    User::LeaveIfError( aItem.GetNextInt( useFileHandle ) );

    TFileName filename = GetFileNameAndPathL( aItem );

    User::LeaveIfError( aItem.GetNextInt( fileType ) );

    //
    //  Add Callbacks
    //
    switch ( fileType )
    {
        case ELocalFile:
        {
            AddLocalFileCallbacksL( filename, useFileHandle );
            break;
        }
        case ESdpFile:
        {
            AddSdpFileCallbacksL( filename, useFileHandle, aItem );
            break;
        }
        case ERamFile:
        {
            AddRamFileCallbacksL( filename, useFileHandle, aItem );
            break;
        }
        case EUrl:
        {
            AddUrlCallbacksL( filename, aItem );
            break;
        }
        case EAsxFile:
        {
            AddAsxFileCallbacksL( filename, useFileHandle, aItem );
            break;
        }
        case EMultiLinkFile:
        default:
            break;
    }

    if ( useFileHandle )
    {
        RFs fs;
        RFile file;

        User::LeaveIfError( fs.Connect() );

        fs.ShareProtected();

        User::LeaveIfError( file.Open( fs, filename, EFileRead ) );

        MPX_TRAPD( trapErr, iAppUiEngine->OpenFileL( file, iGenericParamList ) );

        if ( trapErr != KErrNone )
        {
            TCallbackEvent* event = new (ELeave) TCallbackEvent;

            //
            //  When the apId is negative, an error code will be return with a Leave
            //
            event->iEvent = EFunctionLeave;
            event->iExtra = trapErr;

            HandleUtilityEvent( event );
        }

        file.Close();
        fs.Close();
    }
    else
    {
        iAppUiEngine->OpenFileL( filename );
    }

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::OpenMediaL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::OpenMediaL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::OpenMediaL()"));

    iError = KErrNone;

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    TFileName filename = GetFileNameAndPathL( aItem );

    media->SetTextValueL( KMPXMediaGeneralUri, filename );

    media->SetTObjectValueL( KMPXMediaGeneralId, TMPXItemId( 1, 2 ) );

    RFs fs;
    User::LeaveIfError( fs.Connect() );

    CleanupClosePushL( fs );

    if ( BaflUtils::FileExists( fs, filename ) )
        {
        TCallbackEvent* event1 = new (ELeave) TCallbackEvent;
        event1->iEvent = EViewUtilityPreLoadView;
        event1->iUid   = KVideoPlaybackViewUid;
        AddExpectedEvent( event1 );

        TCallbackEvent* event2 = new (ELeave) TCallbackEvent;
        event2->iEvent = EPlaybackUtilityInitPlaylist;
        AddExpectedEvent( event2 );
        }

    CleanupStack::PopAndDestroy( &fs );

    iAppUiEngine->OpenMediaL( *media );

    CleanupStack::PopAndDestroy( media );

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::SetGenericParamAccessPointL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::SetGenericParamAccessPointL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::SetGenericParamAccessPointL()"));

    TInt apId;

    //
    //  Read in the filename from the config file
    //
    User::LeaveIfError( aItem.GetNextInt( apId ) );

    if ( iGenericParamList )
    {
        iGenericParamList->Reset();
        delete iGenericParamList;
    }

    iGenericParamList = CAiwGenericParamList::NewL();

    TAiwVariant apVariant( apId );
    TAiwGenericParam genericParam( EGenericParamAccessPoint, apVariant );
    iGenericParamList->AppendL( genericParam );

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::OpenFileEmbeddedPdlL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::OpenFileEmbeddedPdlL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::OpenFileEmbeddedPdlL()"));

    iError = KErrNone;

    TInt downloadID;

    User::LeaveIfError( aItem.GetNextInt( downloadID ) );

    TFileName filename = GetFileNameAndPathL( aItem );

    CreateEmbeddedPdlGenericParametersL( downloadID, filename );

    if ( KErrNone == iError )
    {
        RFs fs;
        RFile file;

        User::LeaveIfError( fs.Connect() );

        fs.ShareProtected();

        User::LeaveIfError( file.Open( fs, filename, EFileRead ) );

        AddEmbeddedPdlCallbacksL( downloadID, filename );

        iAppUiEngine->OpenFileL( file, iGenericParamList );

        file.Close();
        fs.Close();
    }

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::CreateEmbeddedPdlGenericParametersL
// -------------------------------------------------------------------------------------------------
//
void
CMpxVideoPlayer_AppUiEngineTester::CreateEmbeddedPdlGenericParametersL( TInt aDownloadId,
                                                                        TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::CreateEmbeddedPdlGenericParametersL()"));

    if ( iGenericParamList )
    {
        iGenericParamList->Reset();
        delete iGenericParamList;
    }

    iGenericParamList = CAiwGenericParamList::NewL();

    TAiwVariant downloadIdVariant( aDownloadId );
    TAiwGenericParam genericParam( EGenericParamDownloadId, downloadIdVariant );
    iGenericParamList->AppendL( genericParam );


    TAiwVariant fileNameVariant( aFileName );
    TAiwGenericParam genericParam2( EGenericParamFile, fileNameVariant );
    iGenericParamList->AppendL( genericParam2 );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::AddEmbeddedPdlCallbacksL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayer_AppUiEngineTester::AddEmbeddedPdlCallbacksL( TInt aDownloadId,
                                                                  TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::AddEmbeddedPdlCallbacksL()"));

    //
    //  We should get 4 callbacks
    //  iViewUtility->PreLoadViewL( PDL View Uid )
    //  iPlaybackUtility->CommandL( EPbCmdStartPd );
    //  iPlaybackUtility->InitL( iFileName );
    //
    TCallbackEvent* event = new (ELeave) TCallbackEvent;
    event->iEvent = EViewUtilityPreLoadView;
    event->iUid   = KVideoPdlPlaybackViewUid;
    AddExpectedEvent( event );

    event = new (ELeave) TCallbackEvent;
    event->iEvent      = EPlaybackUtilityStartPdl;
    event->iDownloadId = aDownloadId;
    event->iFileName   = aFileName;
    event->iExtra      = ETrue;
    AddExpectedEvent( event );

    event = new (ELeave) TCallbackEvent;
    event->iEvent    = EPlaybackUtilityInitFileName;
    event->iFileName = aFileName;
    AddExpectedEvent( event );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::AddMessagePdlCallbacksL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayer_AppUiEngineTester::AddMessagePdlCallbacksL( TInt aNewFile,
                                                                 TInt aDownloadId,
                                                                 TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::AddMessagePdlCallbacksL()"));

    //
    //  We should the following callbacks
    //  iPlaybackUtility->CommandL( EPdCmdClose )
    //  iViewUtility->PreLoadViewL( PDL View Uid )
    //  iPlaybackUtility->CommandL( EPbCmdStartPd );
    //  iPlaybackUtility->InitL( iFileName );
    //

    TCallbackEvent* event;

    if ( aNewFile == 1 )
    {
        event = new (ELeave) TCallbackEvent;
        event->iEvent = EPlaybackUtilityClose;
        AddExpectedEvent( event );
    }

    event = new (ELeave) TCallbackEvent;
    event->iEvent = EViewUtilityPreLoadView;
    event->iUid   = KVideoPdlPlaybackViewUid;
    AddExpectedEvent( event );

    event = new (ELeave) TCallbackEvent;
    event->iEvent      = EPlaybackUtilityStartPdl;
    event->iDownloadId = aDownloadId;
    event->iFileName   = aFileName;
    event->iExtra      = ETrue;
    AddExpectedEvent( event );

    event = new (ELeave) TCallbackEvent;
    event->iEvent    = EPlaybackUtilityInitFileName;
    event->iFileName = aFileName;
    AddExpectedEvent( event );
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::HandleMessageEmbeddedPdlL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::HandleMessageEmbeddedPdlL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::HandleMessageEmbeddedPdlL()"));

    TInt newFile;
    User::LeaveIfError( aItem.GetNextInt( newFile ) );

    if ( newFile )
    {
        TInt downloadID;

        User::LeaveIfError( aItem.GetNextInt( downloadID ) );
        TFileName filename = GetFileNameAndPathL( aItem );

        CreateEmbeddedPdlGenericParametersL( downloadID, filename );

        AddMessagePdlCallbacksL( newFile, downloadID, filename );
    }
    else
    {
        TCallbackEvent* event = new (ELeave) TCallbackEvent;
        event->iEvent      = EPlaybackUtilityPlayCmd;
        AddExpectedEvent( event );
    }

    HBufC8* param8 = HBufC8::NewLC( 1024 );
    TPtr8 paramPtr8 = param8->Des();
    RDesWriteStream writeStream( paramPtr8 );

    iGenericParamList->ExternalizeL( writeStream );

    writeStream.CommitL();
    writeStream.Close();

    iAppUiEngine->HandleMessageL( TUid::Uid(KVideoPlayerStartPDlPlayer), paramPtr8 );

    CleanupStack::PopAndDestroy();

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::HandleMessageActivationMessageL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::HandleMessageActivationMessageL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::HandleMessageActivationMessageL()"));

    TInt msgSender;
    TInt msgType;
    TBool playbackView;

    User::LeaveIfError( aItem.GetNextInt( msgSender ) );
    User::LeaveIfError( aItem.GetNextInt( msgType ) );
    User::LeaveIfError( aItem.GetNextInt( playbackView ) );

    TVideoPlayerActivationMessage params;
    params.iMsgType   = (TVideoPlayerActivationMessage::TMessageType)msgType;
    params.iMsgSender = (TVideoPlayerActivationMessage::TMessageSender)msgSender;

    TPckg<TVideoPlayerActivationMessage> paramsPckg( params );

    if ( msgSender == EMatrix )
    {
        if ( playbackView )
  	  	{
  	  	    static_cast<CMPXViewUtility*>
  	  	  	    (iViewUtility)->PreLoadViewL( TUid::Uid(KMPXPLAYBACKPLUGINTYPEUID) );
        }
  	  	else
  	  	{
            TCallbackEvent* event = new TCallbackEvent;
            event->iEvent = EViewUtilityActivateViewUid;
            event->iUid   = TUid::Uid( KUidMyVideosViewTypeId );

            AddExpectedEvent( event );
        }

        if ( ! iAppUiEngine->HandleMessageL( TUid::Uid( KVideoPlayerVodStartPlayer ), paramsPckg ) )
        {
            iError = -1;
        }
    }
    else
    {
        if ( iAppUiEngine->HandleMessageL( TUid::Uid( 0xBADCAFE ), paramsPckg ) )
        {
            iError = -1;
        }
    }

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::ClearPdlInformation
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayer_AppUiEngineTester::ClearPdlInformation()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::ClearPdlInformation()"));

    iAppUiEngine->ClearPdlInformation();
    EndTest();
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::CreateMpxMessageLC
// -------------------------------------------------------------------------------------------------
//
CMPXMessage*
CMpxVideoPlayer_AppUiEngineTester::CreateMpxMessageLC( TInt aEvent, TInt aType, TInt aData )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::CreateMpxMessageLC()"),
                   _L("aEvent = %d, aType = %d, aData = %d"), aEvent, aType, aData );

    CMPXMessage* pbMessage = CMPXMessage::NewL();
    CleanupStack::PushL( pbMessage );

    pbMessage->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
    pbMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralEvent, aEvent );
    pbMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralType, aType );
    pbMessage->SetTObjectValueL<TInt>( KMPXMessageGeneralData, aData );

    return pbMessage;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::StartStandAloneL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::StartStandAloneL( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::StartStandAloneL()"));

    TCallbackEvent* event = new (ELeave) TCallbackEvent;
    event->iEvent = EViewUtilitySetAsDefaultView;
    event->iUid   = KUidMyVideosViewType;
    AddExpectedEvent( event );

    iAppUiEngine->StartStandAloneL();

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::HandleCollectionMessageL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::HandleCollectionMessageL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::HandleCollectionMessageL()"));

    TInt event;
    TInt type;
    TInt data;

    User::LeaveIfError( aItem.GetNextInt( event ) );
    User::LeaveIfError( aItem.GetNextInt( type ) );
    User::LeaveIfError( aItem.GetNextInt( data ) );

    //
    //  Add callback event
    //
    TCallbackEvent* callback = new (ELeave) TCallbackEvent;

    if (data)
    {
        callback->iEvent = EPlaybackUtilityInitPlaylist;
    }
    else
    {
        callback->iEvent = ECollectionUtilityMedia;
    }

    AddExpectedEvent( callback );

    CMPXMessage* collectionMessage = CreateMpxMessageLC( event, type, data );

    iAppUiEngine->HandleCollectionMessage( collectionMessage, KErrNone );

    CleanupStack::PopAndDestroy( collectionMessage );

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::AddCollectionMediaPdlCallbacksL
// -------------------------------------------------------------------------------------------------
//
void CMpxVideoPlayer_AppUiEngineTester::AddCollectionMediaPdlCallbacksL( TInt aDownloadId,
                                                                         TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::AddCollectionMediaPdlCallbacksL()"));

    //
    //  We should the following callbacks
    //  iViewUtility->PreLoadViewL( PDL View Uid )
    //  iPlaybackUtility->CommandL( EPbCmdStartPd );
    //  iPlaybackUtility->InitL( iFileName );
    //

    TCallbackEvent* event;

    event = new (ELeave) TCallbackEvent;
    event->iEvent = EViewUtilityPreLoadView;
    event->iUid   = KVideoPdlPlaybackViewUid;
    AddExpectedEvent( event );

    event = new (ELeave) TCallbackEvent;
    event->iEvent      = EPlaybackUtilityStartPdl;
    event->iDownloadId = aDownloadId;
    event->iFileName   = aFileName;
    event->iExtra      = EFalse;
    AddExpectedEvent( event );

    event = new (ELeave) TCallbackEvent;
    event->iEvent    = EPlaybackUtilityInitFileName;
    event->iFileName = aFileName;
    AddExpectedEvent( event );
}


// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::HandleCollectionMediaL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::HandleCollectionMediaL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::HandleCollectionMediaL()"));

    TFileName filename = GetFileNameAndPathL( aItem );

    TInt downloadId;

    TInt pdlTestCase = aItem.GetNextInt( downloadId );

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );

    media->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, KMPXMessageGeneral );
    media->SetTextValueL( KMPXMediaGeneralUri, filename );

    if ( pdlTestCase == KErrNone )
    {
        AddCollectionMediaPdlCallbacksL( downloadId, filename );

        media->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, downloadId );
    }
    else
    {
        //
        //  Add callback event
        //
        AddCollectionMediaCallbacksL( filename );
    }


    iAppUiEngine->HandleCollectionMediaL( *media, KErrNone );

    CleanupStack::PopAndDestroy( media );

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::HandleSoftkeyBackL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::HandleSoftkeyBackL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::HandleSoftkeyBackL()"));

    TInt viewDepth;

    User::LeaveIfError( aItem.GetNextInt( viewDepth ) );

    static_cast<CMPXViewUtility*>(iViewUtility)->SetViewHistoryDepth( viewDepth );

    if ( viewDepth > 1 )
    {
        TCallbackEvent* event = new (ELeave) TCallbackEvent;
        event->iEvent = ECollectionUtilityOpen;
        AddExpectedEvent( event );

        event = new TCallbackEvent;
        event->iEvent = EViewUtilityActivatePreviousView;
        AddExpectedEvent( event );
    }
    else
    {
        TCallbackEvent* event = new (ELeave) TCallbackEvent;
        event->iEvent = EAppUiCmdExit;
        AddExpectedEvent( event );

        iTimeoutController = CSimpleTimeout::NewL( this );

        iTimeoutController->Start( TTimeIntervalMicroSeconds(1000000) );
    }

    iAppUiEngine->HandleSoftKeyBackL();

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::HandleOpenPlaylistL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::HandleOpenPlaylistL( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::HandleOpenPlaylistL()"));

    TCallbackEvent* event = new TCallbackEvent;
    event->iEvent = EPlaybackUtilityInitPlaylist;
    AddExpectedEvent( event );

    CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
    CMPXCollectionPlaylist* playlist = CMPXCollectionPlaylist::NewL( *path );
    CleanupStack::PushL( playlist );

    iAppUiEngine->HandleOpenL( *playlist, KErrNone );

    CleanupStack::PopAndDestroy( playlist );

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::HandleOpenMediaL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::HandleOpenMediaL( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::HandleOpenPlaylistL()"));

    TCallbackEvent* event = new TCallbackEvent;
    event->iEvent = ECollectionUtilityOpen;
    AddExpectedEvent( event );

    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );

    iAppUiEngine->HandleOpenL( *media, 0, ETrue, KErrGeneral );

    CleanupStack::PopAndDestroy( media );

    EndTest();

    return iError;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::HandleViewActivation
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::HandleViewActivation( CStifItemParser& /*aItem*/ )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::HandleViewActivation()"));

    iAppUiEngine->HandleViewActivation( KNullUid, KNullUid );

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::GetViewDepthL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::GetViewDepthL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::GetViewDepthL()"));

    TInt viewDepth;
    TInt error = KErrNone;

    User::LeaveIfError( aItem.GetNextInt( viewDepth ) );

    static_cast<CMPXViewUtility*>(iViewUtility)->SetViewHistoryDepth( viewDepth );

    if ( viewDepth != iAppUiEngine->ViewHistoryDepth() )
    {
        error = KErrGeneral;
    }

    return error;
}

// -------------------------------------------------------------------------------------------------
//   CMpxVideoPlayer_AppUiEngineTester::ProcessCommandParametersL
// -------------------------------------------------------------------------------------------------
//
TInt CMpxVideoPlayer_AppUiEngineTester::ProcessCommandParametersL( CStifItemParser& aItem )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayer_AppUiEngineTester::ProcessCommandParametersL()"));

    TInt fileExists = EFalse;

    TInt cmdId;
    TFileName filename;
    TPtrC8 tail;

    User::LeaveIfError( aItem.GetNextInt( cmdId ) );

    switch ( cmdId )
    {
        case EProcessStandAlone:
        {
            TCallbackEvent* event = new (ELeave) TCallbackEvent;
            event->iEvent = EViewUtilitySetAsDefaultView;
            event->iUid   = KUidMyVideosViewType;
            AddExpectedEvent( event );

            break;
        }
        case EProcessFile:
        {
            TFileName file = GetFileNameAndPathL( aItem );
            User::LeaveIfError( aItem.GetNextInt( fileExists ) );

            filename = file;
            break;
        }
    }

    TBool retVal = iAppUiEngine->ProcessCommandParametersL( (TApaCommand)cmdId, filename, tail );

    if ( iError == KErrNone && retVal != fileExists )
    {
        iError = KErrArgument;
    }

    EndTest();

    return iError;
}

void
CMpxVideoPlayer_AppUiEngineTester::AddExpectedEvent( TCallbackEvent* aEvent )
{
    MPX_DEBUG(_L("CMpxVideoPlayer_AppUiEngineTester::AddExpectedEvent()"));

    iExpectedEventArray->AppendL( aEvent );
}

void
CMpxVideoPlayer_AppUiEngineTester::HandleUtilityEvent( TCallbackEvent* aEvent )
{
    MPX_DEBUG(_L("CMpxVideoPlayer_AppUiEngineTester::HandleUtilityEvent()"));

    if ( iExpectedEventArray->Count() > 0 )
    {
        TCallbackEvent* expectedCallback = (*iExpectedEventArray)[0];

        MPX_DEBUG(_L("CMpxVideoPlayer_AppUiEngineTester::ProcessEvent(%d) Expected(%d)"),
            aEvent->iEvent, expectedCallback->iEvent);

        if ( expectedCallback->iEvent == aEvent->iEvent )
        {
            //
            //  Pop the event from the array
            //
            iExpectedEventArray->Delete( 0 );

            switch ( aEvent->iEvent )
            {
                case EViewUtilityPreLoadView:
                case EViewUtilityActivateViewUid:
                {
                    if ( expectedCallback->iUid == aEvent->iUid )
                    {
                        MPX_DEBUG(_L("Proper callback received"));
                    }
                    else
                    {
                        MPX_DEBUG(_L("Proper callback not received"));
                        iError = KErrArgument;
                    }

                    break;
                }
                case EPlaybackUtilityInitFileHandle:
                case EPlaybackUtilityInitFileName:
                {
                    if ( ! expectedCallback->iFileName.CompareF( aEvent->iFileName ) )
                    {
                        MPX_DEBUG(_L("Proper callback received"));
                    }
                    else
                    {
                        MPX_DEBUG(_L("Proper callback not received"));
                        iError = KErrArgument;
                    }

                    break;
                }
                case EPlaybackUtilityInitStreamingFileHandle:
                case EPlaybackUtilityInitStreamingUrl:
                {
                    if ( ! expectedCallback->iFileName.CompareF( aEvent->iFileName ) &&
                         ( expectedCallback->iApId == aEvent->iApId ) )
                    {
                        MPX_DEBUG(_L("Proper callback received"));
                    }
                    else
                    {
                        MPX_DEBUG(_L("Proper callback not received"));
                        iError = KErrArgument;
                    }

                    break;
                }
                case EAppUiCmdExit:
                {
                    MPX_DEBUG(_L("Proper callback received"));

                    //
                    //  Clear other events since terminate message was received
                    //
                    iExpectedEventArray->ResetAndDestroy();

                    break;
                }
                case EPlaybackUtilityStartPdl:
                {
                    if ( ( expectedCallback->iDownloadId == aEvent->iDownloadId ) &&
                         ( ! expectedCallback->iFileName.CompareF( aEvent->iFileName ) ) &&
                         ( expectedCallback->iExtra == aEvent->iExtra ) )
                    {
                        MPX_DEBUG(_L("Proper callback received"));
                    }
                    else
                    {
                        MPX_DEBUG(_L("Proper callback not received"));
                        iError = KErrArgument;
                    }

                    break;
                }
                case EAsxAttributeSeekUpdate:
                {
                    if ( expectedCallback->iExtra == aEvent->iExtra )
                    {
                        MPX_DEBUG(_L("Proper callback received"));
                    }
                    else
                    {
                        MPX_DEBUG(_L("Proper callback not received"));
                        iError = KErrArgument;
                    }

                    break;
                }
                case EFunctionLeave:
                {
                    MPX_DEBUG(_L("expected->iExtra %d, aEvent->iExtra %d"),
                        expectedCallback->iExtra, aEvent->iExtra );

                    if ( expectedCallback->iExtra == aEvent->iExtra )
                    {
                        MPX_DEBUG(_L("Proper callback received"));
                    }
                    else
                    {
                        MPX_DEBUG(_L("Proper callback not received"));
                        iError = KErrArgument;
                    }
                    break;
                }
            }

            if ( iTimeoutController && iExpectedEventArray->Count() == 0 )
            {
                iTimeoutController->Cancel();
                iScriptBase->Signal();
            }
        }
    }

    delete aEvent;
}

void
CMpxVideoPlayer_AppUiEngineTester::EndTest()
{
    if ( ! iTimeoutController )
    {
        if ( iExpectedEventArray->Count() > 0 )
        {
            iError = KErrTimedOut;
        }
    }
}

void
CMpxVideoPlayer_AppUiEngineTester::HandleTimeout( TInt aError )
{
    MPX_DEBUG(_L("CMpxVideoPlayer_AppUiEngineTester::HandleTimeout(%d)"), aError);

    if ( aError == KErrNone )
    {
        iScriptBase->Signal( KErrTimedOut );
    }
    else
    {
        iScriptBase->Signal( aError );
    }
}

TFileName
CMpxVideoPlayer_AppUiEngineTester::GetFileNameAndPathL( CStifItemParser& aItem )
{
    MPX_DEBUG(_L("CMpxVideoPlayer_AppUiEngineTester::GetFileNameAndPathL()"));

    TPtrC filename;
    TBool addPath;

#ifdef __WINSCW__
    _LIT( KVideoTestPath, "c:\\data\\Videos\\" );
#else
    _LIT( KVideoTestPath, "f:\\testing\\data\\" );
#endif

    User::LeaveIfError( aItem.GetNextString( filename ) );
    User::LeaveIfError( aItem.GetNextInt( addPath ) );

    TFileName fullPath;

    if ( addPath )
    {
        fullPath.Append( KVideoTestPath );
    }

    fullPath.Append( filename );

    return fullPath;
}

// EOF
