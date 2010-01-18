/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "FusionMpxPlayer_ApiTest.h"
#include "FusionMpxPlayerTestCommon.h"

#include "VCXTestLog.h"
#include "VCXTestCommon.h"
#include "VCXTestPSSubscriber.h"
#include "CIptvTestTimer.h"
#include "CIptvTestActiveWait.h"
#include "VCXTestTimerWait.h"
#include <mpxvideoplaybackdefs.h>

#include <e32math.h>
#include <mpxplaybackutility.h>
#include <mpxplaybackmessage.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxviewutility.h>
#include <mpxcollectionplaylist.h>


// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::Delete()
    {
    VCXLOGLO1("CFusionMpxPlayer_ApiTest::Delete");

    if( !iShutdownCalled )
        {
        CStifItemParser* fakeParser = NULL;
        TRAP_IGNORE( ShutdownTesterL( *fakeParser ) );
        }

    delete iTestCommon;
    iTestCommon = NULL;

    if( iTimeoutTimer )
        {
        iTimeoutTimer->CancelTimer();
        delete iTimeoutTimer;
        iTimeoutTimer = NULL;
        }

    delete iWait;
    iWait = NULL;    
    
    delete iTesterGetAckResponseSubscriber;
    iTesterGetAckResponseSubscriber = NULL;
    
    delete iTesterGetMessageListResponseSubscriber;
    iTesterGetMessageListResponseSubscriber = NULL;
    
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

        ENTRY( "SetCommandCompletionWait", CFusionMpxPlayer_ApiTest::SetCommandCompletionWaitL ),
        ENTRY( "PrepareCase", CFusionMpxPlayer_ApiTest::PrepareCaseL ),
        ENTRY( "ShutdownTester", CFusionMpxPlayer_ApiTest::ShutdownTesterL ),

        ENTRY( "Create", CFusionMpxPlayer_ApiTest::CreateL ),

        ENTRY( "OpenRandomFile", CFusionMpxPlayer_ApiTest::OpenRandomFileL ),
        ENTRY( "OpenLocalFile", CFusionMpxPlayer_ApiTest::OpenLocalFileL ),
        ENTRY( "OpenUrl", CFusionMpxPlayer_ApiTest::OpenUrlL ),

        ENTRY( "Play", CFusionMpxPlayer_ApiTest::PlayL ),
        ENTRY( "PlayWithFadeIn", CFusionMpxPlayer_ApiTest::PlayWithFadeInL ),
        ENTRY( "Pause", CFusionMpxPlayer_ApiTest::PauseL ),
        ENTRY( "Stop", CFusionMpxPlayer_ApiTest::StopL ),
        ENTRY( "PlayPause", CFusionMpxPlayer_ApiTest::PlayPauseL ),
        ENTRY( "Next", CFusionMpxPlayer_ApiTest::NextL ),
        ENTRY( "Previous", CFusionMpxPlayer_ApiTest::PreviousL ),
        ENTRY( "Replay", CFusionMpxPlayer_ApiTest::ReplayL ),
        ENTRY( "StartSeekForward", CFusionMpxPlayer_ApiTest::StartSeekForwardL ),
        ENTRY( "StartSeekBackward", CFusionMpxPlayer_ApiTest::StartSeekBackwardL ),
        ENTRY( "StopSeeking", CFusionMpxPlayer_ApiTest::StopSeekingL ),
        ENTRY( "DecreaseVolume", CFusionMpxPlayer_ApiTest::DecreaseVolumeL ),
        ENTRY( "IncreaseVolume", CFusionMpxPlayer_ApiTest::IncreaseVolumeL ),
        ENTRY( "MuteVolume", CFusionMpxPlayer_ApiTest::MuteVolumeL ),
        ENTRY( "UnMuteVolume", CFusionMpxPlayer_ApiTest::UnMuteVolumeL ),
        ENTRY( "SetVolume", CFusionMpxPlayer_ApiTest::SetVolumeL ),
        ENTRY( "Close", CFusionMpxPlayer_ApiTest::CloseL ),
        ENTRY( "ApplyEffect", CFusionMpxPlayer_ApiTest::ApplyEffectL ),
        ENTRY( "DisableEffect", CFusionMpxPlayer_ApiTest::DisableEffectL ),
        ENTRY( "PreservePosition", CFusionMpxPlayer_ApiTest::PreservePositionL ),
        ENTRY( "PreserveState", CFusionMpxPlayer_ApiTest::PreserveStateL ),
        ENTRY( "CloseItem", CFusionMpxPlayer_ApiTest::CloseItemL ),
        ENTRY( "CancelInit", CFusionMpxPlayer_ApiTest::CancelInitL ),
        ENTRY( "ResetPreserveState", CFusionMpxPlayer_ApiTest::ResetPreserveStateL ),
        ENTRY( "UnloadNonActivePlugin", CFusionMpxPlayer_ApiTest::UnloadNonActivePluginL ),
        ENTRY( "ClearKeyBuffer", CFusionMpxPlayer_ApiTest::ClearKeyBufferL ),

        ENTRY( "VideoSeekForward", CFusionMpxPlayer_ApiTest::VideoSeekForwardL ),
        ENTRY( "VideoSeekBackward", CFusionMpxPlayer_ApiTest::VideoSeekBackwardL ),
        ENTRY( "VideoEndSeek", CFusionMpxPlayer_ApiTest::VideoEndSeekL ),
        ENTRY( "VideoNaturalAspectRatio", CFusionMpxPlayer_ApiTest::VideoNaturalAspectRatioL ),
        ENTRY( "VideoZoomAspectRatio", CFusionMpxPlayer_ApiTest::VideoZoomAspectRatioL ),
        ENTRY( "VideoStretchAspectRatio", CFusionMpxPlayer_ApiTest::VideoStretchAspectRatioL ),
        ENTRY( "VideoDefaultAspectRatio", CFusionMpxPlayer_ApiTest::VideoDefaultAspectRatioL ),
        ENTRY( "VideoHandleForeground", CFusionMpxPlayer_ApiTest::VideoHandleForegroundL ),
        ENTRY( "VideoHandleBackground", CFusionMpxPlayer_ApiTest::VideoHandleBackgroundL ),
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::PrepareCaseL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::PrepareCaseL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::PrepareCaseL ------>");
    // Print to UI
    _LIT( KTestModule, "CFusionMpxPlayer_ApiTest" );
    _LIT( KWhere, "In PrepareCaseL" );
    TestModuleIf().Printf( 0, KTestModule, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt err = CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExeGlobalShutdownKey, 1 );
    if( err != KErrNone )
        {
        VCXLOGLO2("CFusionMpxPlayer_ApiTest:: Could not read KFusionMpxPlayerTestExeGlobalShutdownKey from PS. err: %d", err);
        }

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::PrepareCaseL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::SetCommandCompletionWaitL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::SetCommandCompletionWaitL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::SetCommandCompletionWaitL ------>");
    // Print to UI
    _LIT( KTestModule, "CFusionMpxPlayer_ApiTest" );
    _LIT( KWhere, "In SetCommandCompletionWaitL" );
    TestModuleIf().Printf( 0, KTestModule, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    TInt ack;
    User::LeaveIfError( aItem.GetNextInt( ack ) );
    
    iCommandWait = static_cast<TBool>( ack );

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::SetCommandCompletionWaitL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::ShutdownTesterL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::ShutdownTesterL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::ShutdownTesterL ------>");
    // Print to UI
    _LIT( KTestModule, "CFusionMpxPlayer_ApiTest" );
    _LIT( KWhere, "In ShutdownTesterL" );
    TestModuleIf().Printf( 0, KTestModule, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iShutdownCalled = ETrue;

    // Tell tester exe to shutdown.
    if( iProcess.ExitReason() == KErrNone && iProcess.ExitType() == EExitPending )
        {
        VCXLOGLO1("CFusionMpxPlayer_ApiTest:: Setting cmd EVcxFusionMpxPlayerCommandShutdown and waiting response ----->");
        TRAP_IGNORE( SendExeCmdL( EVcxFusionMpxPlayerCommandShutdown ) );
        User::After( 3000 );
        }
    
    // Check the shutdown.
    VCXLOGLO2("CFusionMpxPlayer_ApiTest:: Exit type: %d", iProcess.ExitType());
    VCXLOGLO2("CFusionMpxPlayer_ApiTest:: Exit reason: %d", iProcess.ExitReason());
    TExitCategoryName exitCatName = iProcess.ExitCategory();
    VCXLOGLO2("CFusionMpxPlayer_ApiTest:: Exit category: %S", &exitCatName);

    TInt err = iProcess.ExitReason();

    if( err == KErrNone && iProcess.ExitType() != EExitKill && exitCatName.Length() == 0 )
        {
        VCXLOGLO1("CFusionMpxPlayer_ApiTest:: No exit error, no panic but exit type is unusual or exe is stil running.");
        //err = KErrGeneral; // We don't care, test classes have been destroyed by now.
        }        

    // Stop listening P&S changes.
    delete iTesterGetAckResponseSubscriber;
    iTesterGetAckResponseSubscriber = NULL;
    
    delete iTesterGetMessageListResponseSubscriber;
    iTesterGetMessageListResponseSubscriber = NULL;
    
    // Remove the P&S keys for this process.
    RProperty::Delete( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExePsKeyCmd + iPSKeyBase);
    RProperty::Delete( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExePsKeyCmdIntParam1 + iPSKeyBase);
    RProperty::Delete( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExePsKeyCmdIntParam2 + iPSKeyBase);
    RProperty::Delete( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExePsKeyCmdDescParam1 + iPSKeyBase);
    RProperty::Delete( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExePsKeyResponse + iPSKeyBase);
    RProperty::Delete( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExePsKeyResponseError + iPSKeyBase);
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::ShutdownTesterL");
    return err;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::CreateL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::CreateL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::CreateL ------>");
    
    // Print to UI
    _LIT( KFusionMpxPlayer_ApiTest, "FusionMpxPlayer_ApiTest" );
    _LIT( KWhere, "In Create" );
    TestModuleIf().Printf( 0, KFusionMpxPlayer_ApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iCommandWait = ETrue;
    
    User::LeaveIfError( iFs.Connect() );
    
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iTestCommon = CVCXTestCommon::NewL();
    iWait = CIptvTestActiveWait::NewL();
    iTimeoutTimer = CIptvTestTimer::NewL( *this, KTimeoutTimerId );

    // Start tester exe process
    User::LeaveIfError( iProcess.Create( _L("FusionMpxPlayerTestExeUi.exe"), _L("") ) );
    User::After( 2000000 );
    iProcess.Resume();
    User::After( 2000000 );

    TesterExeAliveL();

    VCXLOGLO1("CFusionMpxPlayer_ApiTest:: tester exe started ok ------>");

    User::LeaveIfError( CVCXTestCommon::GetPSProperty( KFusionMpxPlayerTestPScategory, 
            KFusionMpxPlayerTestExeGlobalTesterCount, iPSKeyBase ) );
    iPSKeyBase = iPSKeyBase * 1000 + iProcess.Id();

    // Start listening P&S for the tester responses.
    iTesterGetAckResponseSubscriber = CVCXTestPSSubscriber::NewL( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyResponse + iPSKeyBase, RProperty::EInt, this );
    iTesterGetAckResponseSubscriber->Start();
    VCXLOGLO2("CFusionMpxPlayer_ApiTest:: Listening P&S key KFusionMpxPlayerTestExePsKeyResponse: %d", KFusionMpxPlayerTestExePsKeyResponse + iPSKeyBase);
    
    TTime time;
    time.HomeTime();
    iRandomSeed = time.Int64();

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::CreateL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::OpenRandomFileL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::OpenRandomFileL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::OpenRandomFileL ------>");
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    
    TInt fileNum = ( Math::Rand( iRandomSeed ) % 5 );
    TBuf<256> file( _L(":\\testing\\data\\") );

    switch( fileNum )
        {
        case 0:
            {
            file.Append( _L("sotajumala1.3gp") );
            }
            break;
        case 1:
            {
            file.Append( _L("happytreefriends_mpeg4.mp4") );
            }
            break;
        case 2:
            {
            file.Append( _L("prettyfu.3gp") );
            }
            break;
        case 3:
            {
            file.Append( _L("SufferWell.3gp") );
            }
            break;
        case 4:
            {
            file.Append( _L("5thelement_mpeg4.mp4") );
            }
            break;
        default:
            {
            file.Append( _L("5thelement_mpeg4.mp4") );
            }
            break;
        }

    // Find which drive the file is on.
    TBuf<256> path;
    path.Copy( _L("c") );
    path.Append( file );
    if( !BaflUtils::FileExists( iFs, path ) )
        {
        path.Copy( _L("e") );
        path.Append( file );
        if( !BaflUtils::FileExists( iFs, path ) )
            {
            path.Copy( _L("f") );
            path.Append( file );
            if( !BaflUtils::FileExists( iFs, path ) )
                {
                VCXLOGLO1("CFusionMpxPlayer_ApiTest:: Error! Test video not found!");
                User::Leave( KErrNotFound );
                }
            }
        }

    SendExeCmdL( EVcxFusionMpxPlayerCommandOpenLocalFile, path );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::OpenRandomFileL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::OpenLocalFileL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::OpenLocalFileL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::OpenLocalFileL ------>");
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TPtrC file;
    User::LeaveIfError( aItem.GetNextString( file ) );
    
    SendExeCmdL( EVcxFusionMpxPlayerCommandOpenLocalFile, file );

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::OpenLocalFileL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::OpenUrlL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::OpenUrlL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::OpenUrlL ------>");
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TPtrC url;
    User::LeaveIfError( aItem.GetNextString( url ) );

    TPtrC iapName;
    User::LeaveIfError( aItem.GetNextString( iapName ) );

    TUint32 iapId;
    User::LeaveIfError( iTestCommon->GetIapIdL( iapName, iapId ) );

    SendExeCmdL( EVcxFusionMpxPlayerCommandOpenUrl, url, iapId );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::OpenUrlL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::PlayL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::PlayL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::PlayL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdPlay );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::PlayL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::PlayWithFadeInL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::PlayWithFadeInL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::PlayWithFadeInL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdPlayWithFadeIn );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::PlayWithFadeInL");
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::PauseL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::PauseL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::PauseL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    
    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdPause );

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::PauseL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::StopL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::StopL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::StopL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdStop );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::StopL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::PlayPauseL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::PlayPauseL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::PlayPauseL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdPlayPause );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::PlayPauseL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::NextL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::NextL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::NextL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdNext );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::NextL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::PreviousL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::PreviousL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::PreviousL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdPrevious );

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::PreviousL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::ReplayL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::ReplayL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::ReplayL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdReplay );

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::ReplayL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::StartSeekForwardL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::StartSeekForwardL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::StartSeekForwardL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdStartSeekForward );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::StartSeekForwardL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::StartSeekBackwardL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::StartSeekBackwardL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::StartSeekBackwardL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdStartSeekBackward );

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::StartSeekBackwardL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::StopSeekingL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::StopSeekingL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::StopSeekingL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdStopSeeking );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::StopSeekingL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::DecreaseVolumeL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::DecreaseVolumeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::DecreaseVolumeL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdDecreaseVolume );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::DecreaseVolumeL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::IncreaseVolumeL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::IncreaseVolumeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::IncreaseVolumeL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdIncreaseVolume );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::IncreaseVolumeL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::MuteVolumeL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::MuteVolumeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::MuteVolumeL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdMuteVolume );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::MuteVolumeL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::UnMuteVolumeL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::UnMuteVolumeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::UnMuteVolumeL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdUnMuteVolume );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::UnMuteVolumeL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::SetVolumeL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::SetVolumeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::SetVolumeL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt volume;
    User::LeaveIfError( aItem.GetNextInt( volume ) );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdSetVolume, volume );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::SetVolumeL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::CloseL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::CloseL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::CloseL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdClose );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::CloseL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::ApplyEffectL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::ApplyEffectL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::ApplyEffectL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt effect;
    User::LeaveIfError( aItem.GetNextInt( effect ) );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbApplyEffect, effect );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::ApplyEffectL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::DisableEffectL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::DisableEffectL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::DisableEffectL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdDisableEffect );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::DisableEffectL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::PreservePositionL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::PreservePositionL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::PreservePositionL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdPreservePosition );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::PreservePositionL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::PreserveStateL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::PreserveStateL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::PreserveStateL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdPreserveState );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::PreserveStateL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::CloseItemL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::CloseItemL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::CloseItemL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdCloseItem );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::CloseItemL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::CancelInitL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::CancelInitL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::CancelInitL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdCancelInit );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::CancelInitL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::ResetPreserveStateL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::ResetPreserveStateL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::ResetPreserveStateL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdResetPreserveState );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::ResetPreserveStateL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::UnloadNonActivePluginL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::UnloadNonActivePluginL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::UnloadNonActivePluginL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdUnloadNonActivePlugin );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::UnloadNonActivePluginL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::ClearKeyBufferL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::ClearKeyBufferL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::ClearKeyBufferL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    
    SendExeCmdL( EVcxFusionMpxPlayerCommandGeneralPlaybackCommand, EPbCmdClearKeyBuffer );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::ClearKeyBufferL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::VideoSeekForwardL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::VideoSeekForwardL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::VideoSeekForwardL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandVideoPlaybackCommand, EPbCmdStartVideoSeekingForward );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::VideoSeekForwardL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::VideoSeekBackwardL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::VideoSeekBackwardL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::VideoSeekBackwardL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandVideoPlaybackCommand, EPbCmdStartVideoSeekingBackward );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::VideoSeekBackwardL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::VideoEndSeekL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::VideoEndSeekL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::VideoEndSeekL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandVideoPlaybackCommand, EPbCmdStopVideoSeeking );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::VideoEndSeekL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::VideoNaturalAspectRatioL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::VideoNaturalAspectRatioL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::VideoNaturalAspectRatioL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    
    SendExeCmdL( EVcxFusionMpxPlayerCommandVideoPlaybackCommand, EPbCmdNaturalAspectRatio );

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::VideoNaturalAspectRatioL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::VideoZoomAspectRatioL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::VideoZoomAspectRatioL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::VideoZoomAspectRatioL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandVideoPlaybackCommand, EPbCmdZoomAspectRatio );

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::VideoZoomAspectRatioL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::VideoStretchAspectRatioL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::VideoStretchAspectRatioL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::VideoStretchAspectRatioL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    
    SendExeCmdL( EVcxFusionMpxPlayerCommandVideoPlaybackCommand, EPbCmdStretchAspectRatio );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::VideoStretchAspectRatioL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::VideoDefaultAspectRatioL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::VideoDefaultAspectRatioL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::VideoDefaultAspectRatioL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandVideoPlaybackCommand, EPbCmdSetDefaultAspectRatio );
    
    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::VideoDefaultAspectRatioL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::VideoHandleForegroundL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::VideoHandleForegroundL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::VideoHandleForegroundL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandVideoPlaybackCommand, EPbCmdHandleForeground );

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::VideoHandleForegroundL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::VideoHandleBackgroundL
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayer_ApiTest::VideoHandleBackgroundL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::VideoHandleBackgroundL ------>");

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    SendExeCmdL( EVcxFusionMpxPlayerCommandVideoPlaybackCommand, EPbCmdHandleBackground);

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::VideoHandleBackgroundL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::ValueChangedL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::ValueChangedL( const TUid& aUid, const TUint32& aKey, const TInt&  aValue )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::ValueChangedL ------>");

    if( aUid == KFusionMpxPlayerTestPScategory )
        {
        if( aKey == KFusionMpxPlayerTestExePsKeyResponse + iPSKeyBase )
            {
            switch( aValue )
                {
                case EVcxFusionMpxPlayerResponseGeneralError:
                    VCXLOGLO1("CFusionMpxPlayer_ApiTest:: received response EVcxFusionMpxPlayerResponseGeneralError ------>");
                    iTimeoutTimer->Cancel();
                    iWait->Stop();
                    break;
                    
                case EVcxFusionMpxPlayerResponseCommandComplete:
                    VCXLOGLO1("CFusionMpxPlayer_ApiTest:: received response EVcxFusionMpxPlayerResponseCommandComplete ------>");
                    iTimeoutTimer->Cancel();
                    iWait->Stop();
                    iCommandComplete = ETrue;
                    break;
                
                case EVcxFusionMpxPlayerResponseShutdown:
                    VCXLOGLO1("CFusionMpxPlayer_ApiTest:: received response EVcxFusionMpxPlayerResponseShutdown ------>");
                    iTimeoutTimer->Cancel();
                    iWait->Stop();
                    iCommandComplete = ETrue;
                    break;

                default:
                    break;
                }
            
            CVCXTestCommon::GetPSProperty( KFusionMpxPlayerTestPScategory, 
                    KFusionMpxPlayerTestExePsKeyResponseError + iPSKeyBase, iError );
            
            if( iError != KErrNone )
                {
                VCXLOGLO2("CFusionMpxPlayer_ApiTest:: response error: %d ----->", iError);
                User::Leave( iError );
                }
            }
        }

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::ValueChangedL");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::ValueChangedL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::ValueChangedL( const TUid& aUid, const TUint32& /* aKey */, const TDesC& /* aValue */ )
    {
    VCXLOGLO1(">>>CFusionMpxPlayer_ApiTest::ValueChangedL ------>");

    if( aUid == KFusionMpxPlayerTestPScategory )
        {

        }

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::ValueChangedL");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::TimerComplete
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::TimerComplete( TInt aTimerId, TInt aError )
    {
    VCXLOGLO2(">>>CFusionMpxPlayer_ApiTest::TimerComplete, err: %d ------>", aError);

    if( aError == KErrNone && aTimerId == KTimeoutTimerId )
        {
        iError = KErrTimedOut;
        iWait->Stop();
        VCXLOGLO1("CFusionMpxPlayer_ApiTest:: TIMEOUT!");
        }

    VCXLOGLO1("<<<CFusionMpxPlayer_ApiTest::TimerComplete");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::TesterExeAliveL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::TesterExeAliveL()
    {
    if( iProcess.ExitType() != EExitPending || iProcess.ExitReason() != KErrNone )
        {
        VCXLOGLO1("CFusionMpxPlayer_ApiTest:: ERROR: Tester process has terminated!");
        VCXLOGLO2("CFusionMpxPlayer_ApiTest:: Exit type: %d", iProcess.ExitType());
        VCXLOGLO2("CFusionMpxPlayer_ApiTest:: Exit reason: %d", iProcess.ExitReason());
        TExitCategoryName exitCatName = iProcess.ExitCategory();
        VCXLOGLO2("CFusionMpxPlayer_ApiTest:: Exit category: %S", &exitCatName);
        User::Leave( KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::SendExeCmdL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::SendExeCmdL( TInt aCmd )
    {
    iCommandComplete = EFalse;
    
    VCXLOGLO2("CFusionMpxPlayer_ApiTest::SendExeCmdL aCmd: %d", aCmd);
    
    // Set the cmd.
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmd + iPSKeyBase, aCmd ) );
    
    WaitForCommandL();
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::SendExeCmdL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::SendExeCmdL( TInt aCmd, TInt aParam1 )
    {
    iCommandComplete = EFalse;

    VCXLOGLO3("CFusionMpxPlayer_ApiTest::SendExeCmdL aCmd: %d, aParam1: %d", aCmd, aParam1);
    
    // Set parameter for the command
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmdIntParam1 + iPSKeyBase, aParam1 ) );
    
    // Set the cmd.
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmd + iPSKeyBase, aCmd ) );
    
    WaitForCommandL();
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::SendExeCmdL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::SendExeCmdL( TInt aCmd, TInt aParam1, TInt aParam2 )
    {
    iCommandComplete = EFalse;
   
    VCXLOGLO4("CFusionMpxPlayer_ApiTest::SendExeCmdL aCmd: %d, aParam1: %d, aParam2: %d", aCmd, aParam1, aParam2);
    
    // Set parameter for the command
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmdIntParam1 + iPSKeyBase, aParam1 ) );
    
    // Set parameter for the command
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmdIntParam2 + iPSKeyBase, aParam2 ) );
    
    // Set the cmd.
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmd + iPSKeyBase, aCmd ) );

    WaitForCommandL();
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::SendExeCmdL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::SendExeCmdL( TInt aCmd, const TDesC& aParam1 )
    {
    iCommandComplete = EFalse;

    VCXLOGLO3("CFusionMpxPlayer_ApiTest::SendExeCmdL aCmd: %d, aParam1: '%S'", aCmd, &aParam1);
    
    // Set parameter for the command
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmdDescParam1 + iPSKeyBase, aParam1 ) );
    
    // Set the cmd.
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmd + iPSKeyBase, aCmd ) );

    WaitForCommandL();
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::SendExeCmdL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::SendExeCmdL( TInt aCmd, const TDesC& aParam1, TInt aParam2 )
    {
    iCommandComplete = EFalse;
    
    VCXLOGLO4("CFusionMpxPlayer_ApiTest::SendExeCmdL aCmd: %d, aParam1: '%S', aParam2: %d", aCmd, &aParam1, aParam2);
    
    // Set parameter for the command
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmdDescParam1 + iPSKeyBase, aParam1 ) );
    
    // Set parameter for the command
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmdIntParam1 + iPSKeyBase, aParam2 ) );
    
    // Set the cmd.
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmd + iPSKeyBase, aCmd ) );

    WaitForCommandL();
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayer_ApiTest::WaitForCommandL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayer_ApiTest::WaitForCommandL()
    {
    if( !iCommandWait ) return;
    
    if( !iCommandComplete )
        {
        VCXLOGLO1("CFusionMpxPlayer_ApiTest:: Waiting for command ------>");
        iTimeoutTimer->After( KFusionTestCommandTimeout );        
        iWait->Start();
        VCXLOGLO1("CFusionMpxPlayer_ApiTest:: Wait done");
        }
    
    User::LeaveIfError( iError );        
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
