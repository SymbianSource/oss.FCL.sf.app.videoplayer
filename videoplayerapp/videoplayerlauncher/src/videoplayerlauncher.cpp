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
* Description:   Main application class
*
*/

// Version : %version: da1mmcf#3 %

// Command Line and APA
#include <apgtask.h>
#include <apgcli.h>
#include <apaid.h>
#include <apacmdln.h>
#include <centralrepository.h>

#include "mpxvideo_debug.h"
#include "videoplayerpskeys.h"

const TInt KVcxMatrixUid = 0x101F4CD2;
const TUint KVideoPlayerWgNameSize = 50;

_LIT8( KVcxVideoSuiteActivationMessage, "mm://tvvideosuite?exit=hide" );
_LIT( KVideoPlayerWgName, "*200159b2*");

// -----------------------------------------------------------------------------
// LaunchMenuViewL
// -----------------------------------------------------------------------------
//
void LaunchMenuViewL( const TDesC8& aMessage )
    {
    MPX_ENTER_EXIT(_L("VideoPlayerLauncher LaunchMenuViewL()"));
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL( wsSession );
    
    TApaTaskList taskList( wsSession );
    TApaTask task = taskList.FindApp( TUid::Uid( KVcxMatrixUid ) );

    if ( task.Exists() )
        {
        // Matrix is already running in background - send APA Message
        task.SendMessage( TUid::Uid( KUidApaMessageSwitchOpenFileValue ), aMessage );
        }
    else
        {
        // Matrix not running yet - use Command Line Tail
        RApaLsSession appArcSession;
        CleanupClosePushL( appArcSession );
        User::LeaveIfError( appArcSession.Connect() );      
        TApaAppInfo appInfo;
        TInt err = appArcSession.GetAppInfo( appInfo, TUid::Uid( KVcxMatrixUid ) );
        if ( err == KErrNone )
            {
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( appInfo.iFullName );
            cmdLine->SetCommandL( EApaCommandRun );
            cmdLine->SetTailEndL( aMessage );

            appArcSession.StartApp( *cmdLine );
            CleanupStack::PopAndDestroy( cmdLine );
            }
        CleanupStack::PopAndDestroy( &appArcSession ); 
        }

    CleanupStack::PopAndDestroy( &wsSession ); 
    }

// ---------------------------------------------------------------------------
// CVideoActionHandler::TryBringToForegroundL
// ---------------------------------------------------------------------------
//
TInt TryBringToForegroundL()
    {
    TInt ret ( KErrNotFound );
    RWsSession session;
    User::LeaveIfError( session.Connect() );
	CleanupClosePushL( session );

    TInt ourWgId( 0 );
    TInt error = RProperty::Get( KVideoPlayerRPropertyCategory, KVideoPlayerRPropertyWGIdKey, ourWgId );
        
    if( error == KErrNone && ourWgId != 0 )
        {
        TBuf<KVideoPlayerWgNameSize> wgName;
        session.GetWindowGroupNameFromIdentifier(ourWgId, wgName );
        
        if( wgName.MatchF( KVideoPlayerWgName ) != KErrNotFound )
            {
            ret = session.SetWindowGroupOrdinalPosition( ourWgId, 0 );
            }
        }

	CleanupStack::PopAndDestroy( &session );
    return ret;
    }

// -----------------------------------------------------------------------------
// E32Main
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    // create cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();

    TInt broughtToForegrondResp( KErrGeneral );
	
    TRAP_IGNORE( broughtToForegrondResp = TryBringToForegroundL() );
	
    if ( broughtToForegrondResp != KErrNone )
        {
        TRAP_IGNORE( LaunchMenuViewL( KVcxVideoSuiteActivationMessage ) );
        }

    // destroy cleanup stack
    delete cleanup;
    __UHEAP_MARKEND;
    return 0;
    }
