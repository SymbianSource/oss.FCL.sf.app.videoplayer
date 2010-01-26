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
* Description:  AppUI implementation
*
*/

// Version : %version: 34 %


#include <hlplch.h>                     // Help launcher
#include <sysutil.h>                    // disk space check
#include <f32file.h>                    // TDriveNumber::EDriveC
#include <s32mem.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknview.h>
#include <aknconsts.h>
#include <AknDef.h>
#include <AiwGenericParam.h>
#include <coeutils.h>

#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxviewutility.h>
#include <vcxmyvideosdefs.h>
#include <mpxvideoplayer.rsg>

#include "mpxvideoplayerappui.h"
#include "mpxvideoplayerconstants.h"
#include "mpxvideoplayerlogger.h"
#include "mpxvideoplayerappuiengine.h"
#include "mpxvideoplayeraiwparamextractor.h"
#include "mpxvideo_debug.h"
#include "videoplayerpskeys.h"

// Matrix uid, needed for activating the suite view.
const TInt KVcxMatrixUid = 0x101F4CD2;

// The action=exit flag tells menu that the suite app view is exiting via options->exit.
_LIT8( KVcxVideoSuiteExitMessage, "mm://tvvideosuite?action=exit" );

// ======== MEMBER FUNCTIONS ========
// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::CMpxVideoPlayerAppUi
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerAppUi::CMpxVideoPlayerAppUi()
    : iEndKeyExit( EFalse )
{
    MPX_DEBUG(_L("CMpxVideoPlayerAppUi::CMpxVideoPlayerAppUi()"));
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::ConstructL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::ConstructL()"));

    BaseConstructL( EAknEnableSkin | EAknSingleClickCompatible );

    DoWgIdUpkeep();

    if ( IsEmbedded() )
    {
        //
        //  Set the orientation to landscape for Embedded instances
        //
        SetOrientationL( CAknAppUiBase::EAppUiOrientationLandscape );
    }
    else
    {
        //
        //  if we are stand alone mode store window group id to RProperty
        //  so we can identify it in ActionHandlerPlugin.
        //
        RProperty::Define( KVideoPlayerRPropertyCategory,
                           KVideoPlayerRPropertyWGIdKey,
                           RProperty::EInt );

        RProperty::Set( KVideoPlayerRPropertyCategory,
                        KVideoPlayerRPropertyWGIdKey,
                        iCoeEnv->RootWin().Identifier() );
    }

    iAppUiEngine = CMpxVideoPlayerAppUiEngine::NewL( this );
}


// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::DoWgIdUpkeer
// Checks if stored wgid is run by other application than us
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::DoWgIdUpkeep()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::DoWgIdUpkeep()"));

    // Do the upkeep for stand alone wg identifier
    // If we find that the WgId of this application is same
    // as the one stored in RProperty we know that the stand
    // alone has crashed and we have to clear it.
    TInt wgId( 0 );
    TInt err( RProperty::Get( KVideoPlayerRPropertyCategory, KVideoPlayerRPropertyWGIdKey, wgId ) );

    if ( wgId && !err )
    {
        if ( IsEmbedded() && wgId == iCoeEnv->RootWin().Identifier() )
        {
            // Stored wgId is ours and we are in embedded mode. Delete!
            RProperty::Delete( KVideoPlayerRPropertyCategory,
                               KVideoPlayerRPropertyWGIdKey );
        }
    }
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::~CMpxVideoPlayerAppUi
// Destructor. Frees reserved resources
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerAppUi::~CMpxVideoPlayerAppUi()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::~CMpxVideoPlayerAppUi()"));

    // If we are running in embedded we keep the RProperty alive when exiting.
    if ( ! IsEmbedded() )
    {
        RProperty::Delete( KVideoPlayerRPropertyCategory,
                           KVideoPlayerRPropertyWGIdKey );
    }

    delete iAppUiEngine;
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::HandleExit
// Handles exit command
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::HandleExit()
{
    Exit();
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::ProcessCommandParametersL
// From CEikAppUi, called from plugin when remind occurs and MpxVideoPlayer is not
// exist
// -----------------------------------------------------------------------------
//
TBool CMpxVideoPlayerAppUi::ProcessCommandParametersL( TApaCommand aCommand,
                                                       TFileName& aDocumentName,
                                                       const TDesC8& aTail )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::ProcessCommandParametersL()"),
                   _L("aCommand = %d"), aCommand );


    return iAppUiEngine->ProcessCommandParametersL( aCommand, aDocumentName, aTail );
}

// ---------------------------------------------------------------------------
// From CEikAppUi.
// Handle window server events.
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::HandleWsEventL( const TWsEvent& aEvent, CCoeControl* aDestination )
{
    TInt eventType = aEvent.Type();
    if ( eventType == KAknUidValueEndKeyCloseEvent // End Call key
            || eventType == KAknShutOrHideApp ) // Exit cmd from Task Switcher
    {
        MPX_DEBUG(_L("CMpxVideoPlayerAppUi::HandleWsEventL() Closed by framework"));

        iEndKeyExit = ETrue;
        iAppUiEngine->PrepareCloseMpxL();
    }

    CAknAppUi::HandleWsEventL( aEvent, aDestination );
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::HandleCommandL
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::HandleCommandL( TInt aCommand )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::HandleCommandL()"),
                   _L("aCommand = %d"), aCommand );

    switch ( aCommand )
    {
        case EEikCmdExit: // fall through
        case EAknCmdExit:
        {
            MPX_DEBUG(_L("CMpxVideoPlayerAppUi::HandleCommandL() - Exit"));

            iAppUiEngine->PrepareCloseMpxL();

            //
            //  Send message to matrix menu on a stand alone instance if:
            //     1)  End Key is pressed
            //     2)  Exit command has been issued
            //
            if ( ! IsEmbedded() && ( ! iEndKeyExit && iAppUiEngine->ExitToMatrixMenu() ) )
            {
                LaunchMmViewL( KVcxVideoSuiteExitMessage );
            }

            HandleExit();
            break;
        }
        case EAknSoftkeyBack:
        {
            iAppUiEngine->HandleSoftKeyBackL();
            break;
        }
        case EAknCmdHelp:
        {
            MPX_DEBUG(_L("CMpxVideoPlayerAppUi::HandleCommandL() - Help"));
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), AppHelpContextL() );
            break;
        }
        case EAknCmdHideInBackground:
        {
            iAppUiEngine->ClearPdlInformation();
            break;
        }
        default:
        {
            // do nothing if the command is not reconized
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Querying whether the application is launched in embedded mode or not.
// ---------------------------------------------------------------------------
//
TBool CMpxVideoPlayerAppUi::IsEmbedded() const
{
    TBool embedded = iEikonEnv->StartedAsServerApp();

    MPX_DEBUG(_L("CMpxVideoPlayerAppUi::IsEmbedded(%d)"), embedded );

    return embedded;
}

// ---------------------------------------------------------------------------
// Opens the specified file in response to a corresponding message.
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::OpenFileL( RFile& aFile, const CAiwGenericParamList* aParams )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::OpenFileL(RFile)"));

    MPX_TRAPD( err, iAppUiEngine->OpenFileL( aFile, aParams ) );

    //
    //  Cancel comes from a Cancel on the Select AP dialog
    //  Perform a softkey back
    //
    if ( err )
    {
        if ( err == KErrCancel )
        {
            // if the app is launched in embedded mode (view history depth would be zero)
            // we close the app using the softkey back, if it was launched by a active view
            // we stay in that view.
            if ( iAppUiEngine->ViewHistoryDepth() <= 0 )
            {
                MPX_TRAPD( error, HandleCommandL( EAknSoftkeyBack ) );
            }
        }
        else
        {
            User::Leave( err );
        }
    }

    aFile.Close();
}

// ---------------------------------------------------------------------------
// From CEikAppUi.
// Opens the specified file
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::OpenFileL( const TDesC& aFileName )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::OpenFileL()"),
                   _L("aFileName = %S"), &aFileName );

    MPX_TRAPD( err, iAppUiEngine->OpenFileL( aFileName ) );

    //
    //  Cancel comes from a Cancel on the Select AP dialog
    //  Perform a softkey back
    //
    if ( err )
    {
        if ( err == KErrCancel )
        {
            // if the app is launched in embedded mode (view history depth would be zero)
            // we close the app using the softkey back, if it was launched by a active view
            // we stay in that view.
            if ( iAppUiEngine->ViewHistoryDepth() <= 0 )
            {
                MPX_TRAPD( error, HandleCommandL( EAknSoftkeyBack ) );
            }
        }
        else
        {
            User::Leave( err );
        }
    }
}

// -----------------------------------------------------------------------------
// From CEikAppUi
// -----------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse
CMpxVideoPlayerAppUi::HandleMessageL( TUint32 aClientHandleOfTargetWindowGroup,
                                      TUid aMessageUid,
                                      const TDesC8& aMessageParameters )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::HandleMessageL()"));

    MCoeMessageObserver::TMessageResponse ret( EMessageNotHandled );

    if ( iAppUiEngine->HandleMessageL( aMessageUid, aMessageParameters ) )
    {
        ret = EMessageHandled;
    }
    else
    {
        ret = CAknViewAppUi::HandleMessageL( aClientHandleOfTargetWindowGroup,
                                             aMessageUid,
                                             aMessageParameters );
    }

    return  ret;
}

// ---------------------------------------------------------------------------
// From CCoeAppUi.
// Handle foreground event.
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::HandleForegroundEventL( TBool aForeground )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::HandleForegroundEventL()"),
                   _L("aForeground = %d"), aForeground );

    CAknViewAppUi::HandleForegroundEventL( aForeground );
}

// ---------------------------------------------------------------------------
// Return active view.
// ---------------------------------------------------------------------------
CAknView* CMpxVideoPlayerAppUi::View()
{
    return iView;
}

// ---------------------------------------------------------------------------
// Sends a message to Matrix Menu
// ---------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::LaunchMmViewL( const TDesC8& aMessage )
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::LaunchMmViewL()"));

    TApaTaskList taskList( iCoeEnv->WsSession() );
    TApaTask task = taskList.FindApp( TUid::Uid( KVcxMatrixUid ) );

    if ( task.Exists() )
    {
        //
        //  Matrix is already running in background - send APA Message
        //
        task.SendMessage( TUid::Uid( KUidApaMessageSwitchOpenFileValue ), aMessage );
    }
}

