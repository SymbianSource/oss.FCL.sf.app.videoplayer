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

// Version : %version: e92_40 %


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

// ======== MEMBER FUNCTIONS ========
// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::CMpxVideoPlayerAppUi
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerAppUi::CMpxVideoPlayerAppUi()
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

    if ( IsEmbedded() )
    {
        //
        //  Set the orientation to landscape for Embedded instances
        //
        SetOrientationL( CAknAppUiBase::EAppUiOrientationLandscape );

        //
        //  Hide status pane for embedded use cases
        //
        MPX_DEBUG(_L("CMpxVideoPlayerAppUi::ConstructL() Hide Status Pane"));
        StatusPane()->MakeVisible( EFalse );
    }

    iAppUiEngine = CMpxVideoPlayerAppUiEngine::NewL( this );
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::~CMpxVideoPlayerAppUi
// Destructor. Frees reserved resources
// -----------------------------------------------------------------------------
//
CMpxVideoPlayerAppUi::~CMpxVideoPlayerAppUi()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::~CMpxVideoPlayerAppUi()"));

    delete iAppUiEngine;
}

// -----------------------------------------------------------------------------
// CMpxVideoPlayerAppUi::HandleExit
// Handles exit command
// -----------------------------------------------------------------------------
//
void CMpxVideoPlayerAppUi::HandleExit()
{
    MPX_ENTER_EXIT(_L("CMpxVideoPlayerAppUi::HandleExit()"));
    
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

    //
    //  Close from the End Call Key or the Exit Cmd from the Task Switcher
    //
    if ( eventType == KAknUidValueEndKeyCloseEvent || eventType == KAknShutOrHideApp )
    {
        MPX_DEBUG(_L("CMpxVideoPlayerAppUi::HandleWsEventL() Closed by framework"));

        iAppUiEngine->ClosePlaybackPluginL();
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

            // Ignore leave, we are about close anyways
            TRAP_IGNORE( iAppUiEngine->ClosePlaybackPluginL() );            
            
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
        case KVcxMyVideosCmdDoLateConstruct:
        {
             MPX_DEBUG(_L("CMpxVideoPlayerAppUi::HandleCommandL( DoLateConstruct )"));
             iAppUiEngine->ActivateLateConstructTimerL();
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

