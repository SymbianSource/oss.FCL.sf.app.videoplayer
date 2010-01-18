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
* Description:  Implementation of CLWPLayerAppUi
 *
*/

// Version : %version: 7 %


#include <DocumentHandler.h>
#include <s32mem.h>
#include <AiwGenericParam.h>
#include <apgwgnam.h>
#include <lwplayer.rsg>
#include <aknappui.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <videoplayeractivationmessage.h>

#include "lwplayerappui.h"
#include "lwpuids.hrh"
#include "mpxvideo_debug.h"

const TInt KEVENTBLOCKTIMEOUT = 1000000;

// -----------------------------------------------------------------------------
//  CLWPlayerAppUi::ConstructL
//  Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLWPlayerAppUi::ConstructL()
{
    MPX_DEBUG(_L("CLWPlayerAppUi::ConstructL()"));

    BaseConstructL( EAknEnableSkin );

    iEmbeddedMPCreated = EFalse;

    iAllowEvents = ETrue;

    iCloseAO = CIdle::NewL( CActive::EPriorityStandard );

    iBlockEventsTimer = CPeriodic::NewL( CActive::EPriorityStandard );
}

// -----------------------------------------------------------------------------
//  CLWPlayerAppUi::~CLWPlayerAppUi()
//  Destructor
//  Frees reserved resources
// -----------------------------------------------------------------------------
//
CLWPlayerAppUi::~CLWPlayerAppUi()
{
    MPX_DEBUG(_L("CLWPlayerAppUi::~CLWPlayerAppUi()"));

    delete iDocHandler;

    if ( iCloseAO )
    {
        delete iCloseAO;
        iCloseAO = NULL;
    }

    if ( iBlockEventsTimer )
    {
        iBlockEventsTimer->Cancel();
        delete iBlockEventsTimer;
        iBlockEventsTimer = NULL;
    }
}

// -----------------------------------------------------------------------------
// CLWPlayerAppUi::HandleCommandL
// -----------------------------------------------------------------------------
//
void CLWPlayerAppUi::HandleCommandL(TInt aCommand)
{
    MPX_DEBUG(_L("CLWPlayerAppUi::HandleCommandL(%d)"), aCommand);

    switch ( aCommand )
    {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            Exit();
            break;
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
//  New Methods added for Progressive Download
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//  CLWPlayerAppUi::ProcessCommandParametersL
//
//  When Prog Download is started, RApaLsSession creates
//  this Document and calls ProcessCommandParametersL.
// -----------------------------------------------------------------------------
//
TBool CLWPlayerAppUi::ProcessCommandParametersL( CApaCommandLine& aCommandLine )
{
    MPX_DEBUG(_L("CLWPlayerAppUi::ProcessCommandParametersL()"));

    //
    //  Since download manager can send multiple events with the same data
    //  Block events from Download Manager for a short time
    //
    BlockEvents();

    //
    //  Convert aFilename to 8-bit and convert to generic params
    //
    TPtrC param = aCommandLine.DocumentName();

    if ( param.Length() )
    {
        HBufC8* param8 = HBufC8::NewLC( param.Length() );
        param8->Des().Copy( param );
        ExtractGenericParamsL( *param8 );
        CleanupStack::PopAndDestroy( param8 );
    }

    return ETrue;
}

// -----------------------------------------------------------------------------
// CLWPlayerAppUi::HandleMessageL
// -----------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse
CLWPlayerAppUi::HandleMessageL( TUint32 aClientHandleOfTWG,
                                TUid aMessageUid,
                                const TDesC8& aMessageParameters )
{
    MPX_DEBUG(_L("CLWPlayerAppUi::HandleMessageL()"));

    if ( iAllowEvents && iEmbeddedMPCreated )
    {
        //
        //  pass the message to Media Player
        //
        SendMessageL( aMessageParameters );

        //
        //  Since download manager can send multiple events with the same data
        //  Block events from Download Manager for a short time
        //
        BlockEvents();
    }

    return CAknAppUi::HandleMessageL( aClientHandleOfTWG,
                                      aMessageUid,
                                      aMessageParameters );
}

// -----------------------------------------------------------------------------
// CLWPlayerAppUi::ExtractGenericParamsL
// -----------------------------------------------------------------------------
//
void CLWPlayerAppUi::ExtractGenericParamsL( const TDesC8& aParams )
{
    MPX_DEBUG(_L("CLWPlayerAppUi::ExtractGenericParamsL()"));

    CBufFlat* paramBuf = CBufFlat::NewL( /*anExpandSize*/1 );
    CleanupStack::PushL( paramBuf );
    paramBuf->InsertL( 0, aParams );
    RBufReadStream readStream( *paramBuf );

    CAiwGenericParamList* genParList = CAiwGenericParamList::NewLC(readStream);

    TInt index = 0;

    //
    //  Unpack fileName
    //
    const TAiwGenericParam* paramFileName =
        genParList->FindFirst( index, EGenericParamFile, EVariantTypeDesC );

    TPtrC fileName;

    //
    //  If the parameter was not found, KErrNotFound is returned
    //
    if ( index == KErrNotFound )
    {
        //
        //  Invalid parameter listing.  Set error so message will
        //  be displayed and LWP will exit
        //
        HandleErrorL( KErrArgument );
    }
    else
    {
        fileName.Set( paramFileName->Value().AsDes() );

        MPX_DEBUG(_L("    filename = %S"), &fileName);

        if ( !iEmbeddedMPCreated )
        {
            //
            //  Launch the Media Player for ProgDL playback
            //  Ignore the errors since the Media Player is
            //  responsible for all error messages once launched.
            //
            TRAPD( err, LaunchMediaPlayerL( fileName, genParList ) );

            if ( err != KErrNone )
            {
                ActivateClosePlayerActiveObject();
            }
        }
    }

    CleanupStack::PopAndDestroy( genParList );
    CleanupStack::PopAndDestroy( paramBuf );
}

TInt
CLWPlayerAppUi::LaunchMediaPlayerL( const TDesC& afilename,
                                    const CAiwGenericParamList* aparamList )
{
    MPX_DEBUG(_L("CLWPlayerAppUi::LaunchMediaPlayerL(%S)"), &afilename);


    //
    //  The download manager has already performed recognition on the file
    //  using the cenrep keys.  The LW Player should always launch the MP in
    //  embedded mode.  Using the MP UID to ensure that it is used.
    //
    TDataType datatype( KFUSIONPLAYERUID );

    CDocumentHandler& handler = DocumentHandlerL();

    RFile file;

    //
    //  Call the document handler to open the file
    //  It will leave if the filename is not present
    //
    TRAPD( err, handler.OpenTempFileL( afilename, file ));

    if ( err == KErrNotFound )
    {
        HandleErrorL( err );
    }
    else if ( err != KErrNone )
    {
        User::Leave( err );
    }
    else
    {
        CleanupClosePushL( file );

        TRAPD( err, handler.OpenFileEmbeddedL( file, datatype, *aparamList ) );

        //
        //  ignore some document handler errors
        //
        if ( err == KErrServerTerminated )
        {
            //
            //  Document handler leaves with -15 if end key is pressed when
            //  embedded application is not fully started or embedded
            //  application panics in start up.  So we can only close the
            //  App in this point and hope that user is press end key
            //
            ActivateClosePlayerActiveObject();
        }
        else if ( err != KErrNone )
        {
            //  Reset error since it was handled here and
            //  the program will exit
            //
            User::Leave( err );
        }

        iEmbeddedMPCreated = ETrue;

        CleanupStack::PopAndDestroy( &file ); // file.Close()
    }

    return KErrNone;
}

CDocumentHandler& CLWPlayerAppUi::DocumentHandlerL()
{
    MPX_DEBUG(_L("CLWPlayerAppUi::DocumentHandlerL()"));

    if ( !iDocHandler )
    {
        iDocHandler = CDocumentHandler::NewL();
    }

    iDocHandler->SetExitObserver( this );

    return *iDocHandler;
}

// -----------------------------------------------------------------------------
// CLWPlayerAppUi::NotifyExit
// Called when user exits
// -----------------------------------------------------------------------------
//
void CLWPlayerAppUi::HandleServerAppExit(TInt aReason)
{
    MPX_DEBUG(_L("CLWPlayerAppUi::HandleServerAppExit(%d)"), aReason);

    //
    //  should exit here - MP terminated, if MP BACK it will bring up Browser
    //
    ActivateClosePlayerActiveObject();
}

// -----------------------------------------------------------------------------
// CLWPlayerAppUi::SendMessageL
// -----------------------------------------------------------------------------
//
TInt CLWPlayerAppUi::SendMessageL(const TDesC8& aMessageParameters)
{
    MPX_DEBUG(_L("CLWPlayerAppUi::SendMessageL()"));

    TInt ret = KErrNone;

    //
    //  find the task number of the Media Player which is playing
    //  the ProgDL
    //
    RArray<RWsSession::TWindowGroupChainInfo> wgs;
    User::LeaveIfError(iCoeEnv->WsSession().WindowGroupList(0, &wgs));
    TBool progDLWindowGroup = EFalse;
    TInt count = wgs.Count();
    TInt LWPwgId=0;

    CApaWindowGroupName::FindByAppUid( KLWPLAYERUID ,
                                       iCoeEnv->WsSession(),
                                       LWPwgId );

    TApaTask task( iCoeEnv->WsSession() );

    //
    //  initialise task to non-existant task
    //
    task.SetWgId( 0 );

    //
    //  wgId = 0 tells FindAppByUid to start looking for apps
    //
    TInt wgId = 0;

    while ( ( wgId != KErrNotFound ) && ! progDLWindowGroup )
    {
        CApaWindowGroupName::FindByAppUid( KFUSIONPLAYERUID,
                                           iCoeEnv->WsSession(),
                                           wgId );

        for ( TInt i=0 ; i<count ; i++ )
        {
            const RWsSession::TWindowGroupChainInfo& info = wgs[i];

            // find the window group id and check that it's parent id is LWP
            if ( info.iId == wgId && info.iParentId == LWPwgId )
            {
                progDLWindowGroup = ETrue;
                break;
            }
        }
    };

    wgs.Close();

    if ( progDLWindowGroup )
    {
        task.SetWgId(wgId);
    }

    if ( task.Exists() )
    {
        task.BringToForeground();

        //
        //  8-bit buffer is required.
        //  Uid is not used
        //
        ret = task.SendMessage( TUid::Uid( KVideoPlayerStartPDlPlayer ), aMessageParameters );
    }

    return ret;
}

// -----------------------------------------------------------------------------
//  CLWPlayerAppUi::HandleErrorL
// -----------------------------------------------------------------------------
//
void CLWPlayerAppUi::HandleErrorL( TInt aError )
{
    MPX_DEBUG(_L("CLWPlayerAppUi::HandleErrorL(%d)"), aError);

    HBufC* text;

    switch ( aError )
    {
        case KErrNotFound:
        {
            text = StringLoader::LoadLC( R_LWP_CLIP_MISSING );
            break;
        }
        case KErrArgument:
        default:
        {
            text = StringLoader::LoadLC( R_LWP_INVALID_CLIP );
            break;
        }
    }

    CAknInformationNote* dlg = new (ELeave) CAknInformationNote(ETrue);
    dlg->ExecuteLD( text->Des() );
    CleanupStack::PopAndDestroy(text);

    //
    //  After the error message, Exit the Light Weight Player
    //
    ActivateClosePlayerActiveObject();
}

// -------------------------------------------------------------------------------------------------
//   CLWPlayerAppUi::ActivateClosePlayerActiveObject
// -------------------------------------------------------------------------------------------------
//
void CLWPlayerAppUi::ActivateClosePlayerActiveObject()
{
    MPX_ENTER_EXIT(_L("CLWPlayerAppUi::ActivateClosePlayerActiveObject()"));

    if ( ! iCloseAO->IsActive() )
    {
        iCloseAO->Start( TCallBack( CLWPlayerAppUi::CloseLWPlayerL, this ) );
    }
}

// -----------------------------------------------------------------------------
//   CLWPlayerAppUi::ClosePlayerL
// -----------------------------------------------------------------------------
//
TInt CLWPlayerAppUi::CloseLWPlayerL( TAny* aPtr )
{
    MPX_DEBUG(_L("CLWPlayerAppUi::ClosePlayerL()"));

    static_cast<CLWPlayerAppUi*>(aPtr)->DoCloseLWPlayerL();
    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CLWPlayerAppUi::DoClosePlayerL
// -------------------------------------------------------------------------------------------------
//
void CLWPlayerAppUi::DoCloseLWPlayerL()
{
    MPX_ENTER_EXIT(_L("CLWPlayerAppUi::DoClosePlayerL()"));

    HandleCommandL( EEikCmdExit );
}

// -------------------------------------------------------------------------------------------------
//   CLWPlayerAppUi::HandleBlockEventsTimeout
// -------------------------------------------------------------------------------------------------
//
TInt CLWPlayerAppUi::HandleBlockEventsTimeout( TAny* aPtr )
{
    MPX_DEBUG(_L("CLWPlayerAppUi::HandleEventBlockTimeout()"));

    static_cast<CLWPlayerAppUi*>(aPtr)->UnblockEvents();

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CLWPlayerAppUi::BlockEvents
// -------------------------------------------------------------------------------------------------
//
void CLWPlayerAppUi::BlockEvents()
{
    MPX_DEBUG(_L("CLWPlayerAppUi::BlockEvents()"));

    //
    //  Message received from Browser
    //  Block other messages for 1 second to prevent double click errors
    //
    iAllowEvents = EFalse;

    if ( iBlockEventsTimer->IsActive() )
    {
        iBlockEventsTimer->Cancel();
    }

    iBlockEventsTimer->Start( KEVENTBLOCKTIMEOUT,
                              0,
                              TCallBack( CLWPlayerAppUi::HandleBlockEventsTimeout, this ) );
}

// -------------------------------------------------------------------------------------------------
//   CLWPlayerAppUi::UnblockEvents
// -------------------------------------------------------------------------------------------------
//
void CLWPlayerAppUi::UnblockEvents()
{
    MPX_DEBUG(_L("CLWPlayerAppUi::UnblockEvents()"));

    iAllowEvents = ETrue;

    if ( iBlockEventsTimer->IsActive() )
    {
        iBlockEventsTimer->Cancel();
    }
}

// End of File
