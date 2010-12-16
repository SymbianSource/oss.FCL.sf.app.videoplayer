/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description*
*/


// INCLUDE FILES
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroyPushL
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <centralrepository.h>
#include <collate.h>

#include "VCXTestLog.h"
#include "VCXTestCommon.h"
#include "CIptvTestActiveWait.h"
#include "CIptvTestTimer.h"
#include "VCXTestStatsKeeper.h"

#include "VCXMyVideosCollectionPluginTest.h"
#include "VCXMyVideosCollectionPluginTester.h"
#include "VCXMyVideosTestCommon.h"
#include "VCXMyVideosTestDlWatcher.h"
#include "VCXMyVideosTestServiceEmu.h"
#include "VCXTestMdsDbModifier.h"

#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>

#include "vcxmyvideosuids.h"

// CONSTANTS

const TInt KVcxTestFileSize3GB = -999;

// MACROS
#define VCXMVTEST_ERR(err) ( iStatsEnabled ) ? KErrNone : err 

// MODULE DATA STRUCTURES

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTest::Delete()
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::Delete ---------->");
    
    delete iTester;
    iTester = NULL;

    if( iDeleteSideloadedFilesAndFolders )
        {
        iDeleteSideloadedFilesAndFolders = EFalse;
        CStifItemParser* parser = NULL;
        TRAP_IGNORE( EmptySideloadFoldersL( *parser ) ); // oeh noes
        }

    iSideloadedFiles.ResetAndDestroy();

    // Remove folders used in sideloading, if they'r empty
    for( TInt i=0; i < iSideloadFolders.Count(); i++ )
        {
        HBufC* path = iSideloadFolders[i];
        TInt pos = 0;
        TInt firstPos = 0;
        while( ETrue )
            {
            pos = path->Des().LocateReverse('\\');
            firstPos = path->Des().Locate('\\');
            // Check that it's not the root folder.
            if( pos != KErrNotFound && pos != firstPos )
                {
                // Remove last folder from the path string, or the filename.
                path->Des().SetLength( pos+1 );
                if( DeleteDirectoryIfEmpty( iFs, *iFileMan, *path ) != KErrNone)
                    {
                    break;
                    }
                // Remove trailing backslash.
                path->Des().SetLength( pos );
                }
            else
                {
                break;
                }
            }
        }

    iSideloadFolders.ResetAndDestroy();

    delete iTestCommon;
    iTestCommon = NULL;

#ifdef __WINSCW__
    iClient.Close();
#endif // __WINSCW__

    if( iActiveWait )
        {
        iActiveWait->Stop();
        delete iActiveWait;
        iActiveWait = NULL;
        }

    if( iActiveWaitBlocking )
        {
        iActiveWaitBlocking->Stop();
        delete iActiveWaitBlocking;
        iActiveWaitBlocking = NULL;
        }
    
    if( iTimeoutTimer )
        {
        iTimeoutTimer->CancelTimer();
        delete iTimeoutTimer;
        iTimeoutTimer = NULL;
        }
    
    if( iCoolDownTimer )
        {
        iCoolDownTimer->CancelTimer();
        delete iCoolDownTimer;
        iCoolDownTimer = NULL;
        }

    delete iMpxMedia;
    iMpxMedia = NULL;
    
    delete iServiceEmu;
    iServiceEmu = NULL;
    
    delete iMdsDbModifier;
    iMdsDbModifier = NULL;

    delete iFileMan;
    iFileMan = NULL;

    iFs.Close();
    
    delete iStats;
    iStats = NULL;    

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::Delete <----------");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Create", CVCXMyVideosCollectionPluginTest::CreateL ),
        
        ENTRY( "EnableStats", CVCXMyVideosCollectionPluginTest::EnableStatsL ),
        ENTRY( "SetPreferredMemory", CVCXMyVideosCollectionPluginTest::SetPreferredMemoryL ),
        ENTRY( "SetSortingOrder", CVCXMyVideosCollectionPluginTest::SetSortingOrderL ),
        ENTRY( "SetAutomaticRefresh", CVCXMyVideosCollectionPluginTest::SetAutomaticRefresh ),
        ENTRY( "SetQuietMode", CVCXMyVideosCollectionPluginTest::SetQuietMode ),
        ENTRY( "CoolDown", CVCXMyVideosCollectionPluginTest::CoolDownL ),
        ENTRY( "DisableDownloadCheck", CVCXMyVideosCollectionPluginTest::DisableDownloadCheckL ),
        ENTRY( "SetFileCheck", CVCXMyVideosCollectionPluginTest::SetFileCheckL ),
        ENTRY( "SetAutoResume", CVCXMyVideosCollectionPluginTest::SetAutoResumeL ),

        ENTRY( "OpenCollection", CVCXMyVideosCollectionPluginTest::OpenCollectionL ),
        ENTRY( "OpenLevel", CVCXMyVideosCollectionPluginTest::OpenLevelL ),
        ENTRY( "RefreshContents", CVCXMyVideosCollectionPluginTest::RefreshContentsL ),
        ENTRY( "GetMediasByMpxId", CVCXMyVideosCollectionPluginTest::GetMediasByMpxIdL ),
        ENTRY( "CloseLevel", CVCXMyVideosCollectionPluginTest::CloseLevelL ),
        
        ENTRY( "GetMediaFullDetails", CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsL ),
        ENTRY( "GetMediaFullDetailsByMpxId", CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsByMpxIdL ),
        ENTRY( "GetAllMediaFullDetails", CVCXMyVideosCollectionPluginTest::GetAllMediaFullDetailsL ),
        ENTRY( "SetMediaDetail", CVCXMyVideosCollectionPluginTest::SetMediaDetailL ),
        ENTRY( "CheckMediaDetail", CVCXMyVideosCollectionPluginTest::CheckMediaDetailL ),
        ENTRY( "CheckMediaCount", CVCXMyVideosCollectionPluginTest::CheckMediaCountL ),
        ENTRY( "CheckMinimumMediaCount", CVCXMyVideosCollectionPluginTest::CheckMinimumMediaCountL ),
        ENTRY( "CheckMaximumMediaCount", CVCXMyVideosCollectionPluginTest::CheckMaximumMediaCountL ),
        ENTRY( "CheckDownloadCount", CVCXMyVideosCollectionPluginTest::CheckDownloadCountL ),
        
        ENTRY( "CreateCopyOfMedia", CVCXMyVideosCollectionPluginTest::CreateCopyOfMediaL ),
        ENTRY( "CreateEmptyMedia", CVCXMyVideosCollectionPluginTest::CreateEmptyMediaL ),
        ENTRY( "AddMedia", CVCXMyVideosCollectionPluginTest::AddMediaL ),
        ENTRY( "SetMedia", CVCXMyVideosCollectionPluginTest::SetMediaL ),
        ENTRY( "OutsideMediaUpdate", CVCXMyVideosCollectionPluginTest::OutsideMediaUpdateL ),
        ENTRY( "CancelNextOperation", CVCXMyVideosCollectionPluginTest::CancelNextOperationL ),
        
        ENTRY( "DeleteFileOfMedia", CVCXMyVideosCollectionPluginTest::DeleteFileOfMediaL ),
        ENTRY( "DeleteFilesOfAllMedias", CVCXMyVideosCollectionPluginTest::DeleteFilesOfAllMediasL ),
        ENTRY( "RemoveMedia", CVCXMyVideosCollectionPluginTest::RemoveMediaL ),
        ENTRY( "RemoveAllMedia", CVCXMyVideosCollectionPluginTest::RemoveAllMediaL ),
        ENTRY( "MoveMedia", CVCXMyVideosCollectionPluginTest::MoveMediaL ),
        ENTRY( "MoveMedias", CVCXMyVideosCollectionPluginTest::MoveMediasL ),
        ENTRY( "CancelMove", CVCXMyVideosCollectionPluginTest::CancelMoveL ),
        ENTRY( "CopyMedia", CVCXMyVideosCollectionPluginTest::CopyMediaL ),
        ENTRY( "CopyMedias", CVCXMyVideosCollectionPluginTest::CopyMediasL ),
        ENTRY( "CancelCopy", CVCXMyVideosCollectionPluginTest::CancelCopyL ),
        ENTRY( "DeleteMedia", CVCXMyVideosCollectionPluginTest::DeleteMediaL ),
        ENTRY( "DeleteMedias", CVCXMyVideosCollectionPluginTest::DeleteMediasL ),
        ENTRY( "CancelDelete", CVCXMyVideosCollectionPluginTest::CancelDeleteL ),
        
        ENTRY( "CreateVideoFile", CVCXMyVideosCollectionPluginTest::CreateVideoFileL ),
        ENTRY( "CreateVideoFileNoWait", CVCXMyVideosCollectionPluginTest::CreateVideoFileNoWaitL ),
        ENTRY( "EnsureDriveForVideos", CVCXMyVideosCollectionPluginTest::EnsureDriveForVideosL ),
        ENTRY( "EmptySideloadFolders", CVCXMyVideosCollectionPluginTest::EmptySideloadFoldersL ),
        ENTRY( "SetDeleteSideloadedVideos", CVCXMyVideosCollectionPluginTest::SetDeleteSideloadedVideos ),
        
        ENTRY( "Download", CVCXMyVideosCollectionPluginTest::DownloadL ),
        ENTRY( "ResumeDownload", CVCXMyVideosCollectionPluginTest::ResumeDownloadL ),
        ENTRY( "ResumeAllDownloads", CVCXMyVideosCollectionPluginTest::ResumeAllDownloadsL ),
        ENTRY( "DownloadUsingMedia", CVCXMyVideosCollectionPluginTest::DownloadUsingMediaL ),
        ENTRY( "ResumeDownloadUsingMedia", CVCXMyVideosCollectionPluginTest::ResumeDownloadUsingMediaL ),
        ENTRY( "DownloadUrl", CVCXMyVideosCollectionPluginTest::DownloadUrlL ),
        ENTRY( "CancelDownload", CVCXMyVideosCollectionPluginTest::CancelDownloadL ),
        ENTRY( "CancelDownloadById", CVCXMyVideosCollectionPluginTest::CancelDownloadByIdL ),
        ENTRY( "PauseDownload", CVCXMyVideosCollectionPluginTest::PauseDownloadL ),
        ENTRY( "PauseDownloadByUrl", CVCXMyVideosCollectionPluginTest::PauseDownloadByUrlL ),
        ENTRY( "CheckDownloadProgress", CVCXMyVideosCollectionPluginTest::CheckDownloadProgressL ),
        
        ENTRY( "WaitAllDownloads", CVCXMyVideosCollectionPluginTest::WaitAllDownloadsL ),
        ENTRY( "WaitForMessage", CVCXMyVideosCollectionPluginTest::WaitForMessageL ),
        ENTRY( "WaitForAnyMessage", CVCXMyVideosCollectionPluginTest::WaitForAnyMessageL ),
        ENTRY( "WaitForMessages", CVCXMyVideosCollectionPluginTest::WaitForMessagesL ),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::EnableStatsL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::EnableStatsL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::EnableStatsL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In EnableStatsL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TPtrC statsName;
    TPtrC fileName;
    User::LeaveIfError( aItem.GetNextString( statsName) );
    User::LeaveIfError( aItem.GetNextString( fileName) );

    TRAP_IGNORE( iStats->StartStatsKeepingL( statsName, fileName ) );
    iStatsEnabled = ETrue;
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::EnableStatsL <----------");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::CreateL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CreateL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CreateL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CreateL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    User::LeaveIfError( iFs.Connect() );
    iFileMan = CFileMan::NewL( iFs );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    iTestCommon = CVCXTestCommon::NewL();
    iStats = CVCXTestStatsKeeper::NewL();
    iTester = CVCXMyVideosCollectionPluginTester::NewL( this, iTestCommon, iStats );
    iActiveWait = CIptvTestActiveWait::NewL();
    iActiveWaitBlocking = CIptvTestActiveWait::NewL();
    iTimeoutTimer = CIptvTestTimer::NewL(*this, 0);
    iCoolDownTimer = CIptvTestTimer::NewL(*this, 1);   
    
#ifdef __WINSCW__
    TInt err = iClient.Connect();
    if( KErrNone != err )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Could not start MDS server session! Err: %d", err);
        }
#endif // __WINSCW__

    iAutoResume = ETrue;

    iTester->SetAutoResume( iAutoResume );

    iServiceEmu = CVCXMyVideosTestServiceEmu::NewL();

    iMdsDbModifier = CVCXTestMdsDbModifier::NewL();

    iCurrentSortOrder = EVcxMyVideosSortingNone;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CreateL <----------");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::SetPreferredMemoryL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::SetPreferredMemoryL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::SetPreferredMemoryL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In SetPreferredMemoryL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt drive(0);
    User::LeaveIfError( GetDriveParam( aItem, drive ) );

    // These were defined in CVcxMyVideosDownloadUtil::PreferredMemoryDesL()
    const TInt KVcxCenRepUid = 0x102750E2; // same as KIptvCenRepUid in CIptvUtil.h
    const TInt KVcxCenRepPreferredMemoryKey = 0x01; // same as KIptvCenRepPreferredMemoryKey in CIptvUtil.h

    TUid uid;
    uid.iUid = KVcxCenRepUid;

    CRepository* cenRep = CRepository::NewLC( uid );
    User::LeaveIfError( cenRep->Set( KVcxCenRepPreferredMemoryKey, drive ) );
    CleanupStack::PopAndDestroy( cenRep );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::SetPreferredMemoryL <----------");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::SetSortingOrderL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::SetSortingOrderL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::SetSortingOrderL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In SetSortingOrderL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt value(0);
    TInt err = aItem.GetNextInt( value );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: New sorting order: %d", value);

    TUid uid;
    uid.iUid = KVcxMyVideosCollectionCenrepUid;

    CRepository* cenRep = CRepository::NewLC( uid );
    User::LeaveIfError( cenRep->Set( KVcxMyVideosCollectionCenrepKeySortingOrder, value ) );
    CleanupStack::PopAndDestroy( cenRep );

    TRAP( err, DoTotalRefreshL() );

    iCurrentSortOrder = value;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::SetSortingOrderL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::SetAutomaticRefresh
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::SetAutomaticRefresh( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::SetAutomaticRefresh ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In SetAutomaticRefresh" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt value(0);
    TInt err = aItem.GetNextInt( value );

    if( iTester )
        {
        iTester->SetAutomaticRefresh( static_cast<TBool>(value) );
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::SetAutomaticRefresh <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::SetQuietMode
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::SetQuietMode( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::SetQuietMode ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In SetQuietMode" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt value(0);
    TInt err = aItem.GetNextInt( value );

    if( iTester )
        {
        iTester->SetQuietMode( static_cast<TBool>(value) );
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::SetQuietMode <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CoolDownL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CoolDownL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CoolDownL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CoolDownL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iCoolDownWait = ETrue;
    iCoolDownTimer->After( KTimeoutSecond * 5 );
    iActiveWait->Start();
    
    if( iEventError == KErrAbort )
        {
        iEventError = KErrNone;
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Error, ABORTING!");
        User::Leave( KErrAbort );
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CoolDownL <----------");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::DisableDownloadCheckL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::DisableDownloadCheckL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::DisableDownloadCheckL");
    if( iTester )
        {
        iTester->GetDownloadWatcher()->DisableDownloadCheck();
        }
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::DisableDownloadCheckL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::SetFileCheckL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::SetFileCheckL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::SetFileCheckL");

    TInt value(0);
    User::LeaveIfError( aItem.GetNextInt( value ) );
    iFileCheckDisabled = !(static_cast<TBool>(value));

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::SetFileCheckL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::SetAutoResumeL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::SetAutoResumeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::SetAutoResumeL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In SetAutoResumeL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt autoResume(0);
    User::LeaveIfError( aItem.GetNextInt( autoResume ) );
    iAutoResume = autoResume;
    if( iTester )
        {
        iTester->SetAutoResume( iAutoResume );
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::SetAutoResumeL <----------");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::OpenCollectionL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::OpenCollectionL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::OpenCollectionL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In OpenCollectionL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    TUint32 collectionId( 0 );
    TPtrC collectionIdStr;
    if( aItem.GetNextString( collectionIdStr ) == KErrNone )
        {
        TLex lex( collectionIdStr );
        User::LeaveIfError( lex.Val( collectionId, EHex ) );
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: collectionId: 0x%x", collectionId);
        }

    if( collectionId == 0 )
        {
        collectionId = KVcxUidMyVideosMpxCollection;
        }

    iLastReceivedMessage = -1;

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->OpenCollectionL( collectionId ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::OpenCollectionL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::OpenLevelL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::OpenLevelL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::OpenLevelL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In OpenLevelL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    TInt index;
    User::LeaveIfError( aItem.GetNextInt( index ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: index: %d", index);

    iLastReceivedMessage = -1;

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->OpenLevelL( index ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::OpenLevelL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::CloseLevelL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CloseLevelL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CloseLevelL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CloseLevelL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->CloseLevelL() );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CloseLevelL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::RefreshContentsL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::RefreshContentsL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::RefreshContentsL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In RefreshContentsL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->RefreshContentsL() );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::RefreshContentsL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::RefreshContentsL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::GetMediasByMpxIdL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::GetMediasByMpxIdL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In GetMediasByMpxIdL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt startIndex;
    User::LeaveIfError( aItem.GetNextInt( startIndex ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: startIndex: %d", startIndex);

    TInt endIndex;
    User::LeaveIfError( aItem.GetNextInt( endIndex ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: endIndex: %d", endIndex);
    
    TBool syncCall( EFalse );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->GetMediasByMpxIdL( startIndex, endIndex, syncCall ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::GetMediasByMpxIdL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In GetMediaFullDetailsL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt drive(0);
    User::LeaveIfError( GetDriveParam( aItem, drive ) );

    TInt index;
    User::LeaveIfError( aItem.GetNextInt( index ) );

    RArray<TInt> messages;
    messages.Append( KVCXMYVideosTestMessageCollectionMedia );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->GetMediaFullDetailsL( drive, index ) );
        }

    if( err == KErrNone )
        {
        TRAPD( err, WaitForMessagesL( ETrue, messages, 1, ETrue ) );
        messages.Reset();
        
        if( err == KErrNone )
            {
            CMPXMedia* media = iTester->GetLastFullMedia();
            TRAP_IGNORE( iTester->PrintMPXMediaL( *media, ETrue ) );
            }
        }

    if( err != KErrNone )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsL err: %d", err);            
        }
    
    messages.Reset();

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsByMpxIdL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsByMpxIdL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsByMpxIdL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In GetMediaFullDetailsByMpxIdL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt drive(0);
    User::LeaveIfError( GetDriveParam( aItem, drive ) );

    TInt index;
    User::LeaveIfError( aItem.GetNextInt( index ) );

    TBool sync;
    TInt syncInt;
    if( aItem.GetNextInt( syncInt ) != KErrNone )
        {
        sync = EFalse;
        }
    else
        {
        sync = static_cast<TBool>( syncInt );
        }
    
    RArray<TInt> messages;
    messages.Append( KVCXMYVideosTestMessageCollectionMedia );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->GetMediaFullDetailsByMpxIdL( drive, index, sync ) );
        }

    if( err == KErrNone )
        {
        TRAPD( err, WaitForMessagesL( ETrue, messages, 1, ETrue ) );
        messages.Reset();
        
        if( err == KErrNone )
            {
            CMPXMedia* media = iTester->GetLastFullMedia();
            TRAP_IGNORE( iTester->PrintMPXMediaL( *media, ETrue ) );
            }
        }

    if( err != KErrNone )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsByMpxIdL err: %d", err);            
        }
    
    messages.Reset();

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::GetMediaFullDetailsByMpxIdL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::GetAllMediaFullDetailsL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::GetAllMediaFullDetailsL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::GetAllMediaFullDetailsL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In GetAllMediaFullDetailsL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->GetAllMediaFullDetailsL( ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    iLastReceivedMessage = -1;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::GetAllMediaFullDetailsL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::CheckMediaDetailL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CheckMediaDetailL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CheckMediaDetailL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CheckMediaDetailL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt err( KErrNone );

    TRAP_IGNORE( WaitForRefreshL( EFalse ) ); // Wait for possible refresh to finish.
 
    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt drive(0);
    User::LeaveIfError( GetDriveParam( aItem, drive ) );

    TInt itemIndex(0);
    User::LeaveIfError( aItem.GetNextInt( itemIndex ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: item index: %d", itemIndex);

    if( itemIndex < 0 || itemIndex >= iTester->GetMediaCount() )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Error. Index (%d) out of bounds.", itemIndex);
        User::Leave( KErrArgument );
        }

    TInt mpxAttributeMappingId(0);
    User::LeaveIfError( aItem.GetNextInt( mpxAttributeMappingId ) );

    TVcxTestMediaAttributeType type;
    TMPXAttributeData mpxMediaAttribute;
    User::LeaveIfError( MapMpxMediaAttribute(
            static_cast<TVcxTestMediaAttributeIdMapping>(mpxAttributeMappingId),
            mpxMediaAttribute, type ) );
    
    
    CMPXMedia* media( NULL );        
    TRAP( err, media = iTester->GetMediaL( drive, itemIndex ) );    
    
    // If attribute is not supported, fetch full media details.
    if( media && !media->IsSupported( mpxMediaAttribute ) )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Requesting full media details.");
        RArray<TInt> messages;
        messages.Append( KVCXMYVideosTestMessageCollectionMedia );
        TRAP( err, iTester->GetMediaFullDetailsL( drive, itemIndex ) );
        if( err == KErrNone )
        	{
        	TRAP( err, WaitForMessagesL( ETrue, messages, 5, ETrue ) );
            media = iTester->GetLastFullMedia();
        	}
        messages.Reset();
        }

    if( err != KErrNone )
        {
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CheckMediaDetailL <----------");
        return VCXMVTEST_ERR( err );
        }

    if( !media->IsSupported( mpxMediaAttribute ) )
        {
        VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error: iAttributeId: %d, iContentId: %d is not supported.",
                mpxMediaAttribute.iAttributeId == 0, mpxMediaAttribute.iContentId);

        iTester->PrintMPXMediaL( *media, ETrue );
        
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CheckMediaDetailL <---------");
        return VCXMVTEST_ERR( KErrNotSupported );
        }

    if( type == EVcxTestMediaAttributeTypeString )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type text");

        TPtrC expectedValue;
        User::LeaveIfError( aItem.GetNextString( expectedValue ) );

        const TDesC& value = media->ValueText( mpxMediaAttribute );

        if( expectedValue != value )
            {
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, values do not match: %S and expected: %S", &value, &expectedValue);
            err = KErrCorrupt;
            }
        }

    if( type == EVcxTestMediaAttributeTypeTTime )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TInt64(date)");

        User::Leave( KErrNotSupported );
#if 0
        //TPtrC expectedValue;
        //User::LeaveIfError( aItem.GetNextString( expectedValue ) );

        TInt64 value = *(*media).Value<TInt64>( mpxMediaAttribute );

        TTime time ( value );
        TBuf<60> timeStr;
        _LIT(KDateTimeString, "%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
        time.FormatL(timeStr, KDateTimeString);
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: date value: %S", &timeStr);
#endif
        }

    if( type == EVcxTestMediaAttributeTypeTDateTime )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TDateTime");

        User::Leave( KErrNotSupported );
#if 0
        TInt expectedValue;
        User::LeaveIfError( aItem.GetNextInt( expectedValue ) );

        TTime time ( *(*media).Value<TDateTime>( mpxMediaAttribute ) );

        TBuf<60> timeStr;
        _LIT(KDateTimeString, "%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
        time.FormatL(timeStr, KDateTimeString);
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: date value: %S", &timeStr);
#endif
        }

    if( type == EVcxTestMediaAttributeTypeTInt )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TInt");
        TInt expectedValue;
        User::LeaveIfError( aItem.GetNextInt( expectedValue ) );

        TInt value = *(*media).Value<TInt>( mpxMediaAttribute );

        if( expectedValue != value )
            {
            err = KErrCorrupt;
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, values do not match: %d and expected: %d", value, expectedValue);
            }
        }

    if( type == EVcxTestMediaAttributeTypeTUint )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TUint");

        TInt expectedValue;
        User::LeaveIfError( aItem.GetNextInt( expectedValue ) );

        TUint value = *(*media).Value<TUint>( mpxMediaAttribute );

        if( expectedValue != value )
            {
            err = KErrCorrupt;
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, values do not match: %d and expected: %d", value, expectedValue);
            }
        }

    if( type == EVcxTestMediaAttributeTypeTUint8 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TUint8");

        TInt expectedValue;
        User::LeaveIfError( aItem.GetNextInt( expectedValue ) );

        TUint8 value = *(*media).Value<TUint8>( mpxMediaAttribute );

        if( expectedValue != value )
            {
            err = KErrCorrupt;
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, values do not match: %d and expected: %d", value, expectedValue);
            }
        }
    
    TBool isCategoryNewItemCount = 
            mpxMediaAttribute.iAttributeId == KVcxMediaMyVideosCategoryNewItemCount.iAttributeId &&
            mpxMediaAttribute.iContentId == KVcxMediaMyVideosCategoryNewItemCount.iContentId;
    TInt isCategoryItemCount = 
            mpxMediaAttribute.iAttributeId == KVcxMediaMyVideosCategoryItemCount.iAttributeId &&
            mpxMediaAttribute.iContentId == KVcxMediaMyVideosCategoryItemCount.iContentId;
    
    // Category item counts.
    if( isCategoryNewItemCount || isCategoryItemCount )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting item counts for category.");
        
        TInt expectedValue;
        User::LeaveIfError( aItem.GetNextInt( expectedValue ) );

        // Get actual count from item.
        TUint32 count = *(*media).Value<TUint32>( mpxMediaAttribute );
        
        // Get count of ROM medias for this category.
        
        TUint flags = ( isCategoryNewItemCount ? EVcxMyVideosVideoNew : 0 );

        CMPXMediaArray* medias = GetMediasForLevelL( itemIndex, flags );
                
        TInt romCount = 0;
        for( TInt i = 0; i < medias->Count(); i++ )
            {
            CMPXMedia* collectionMedia = (*medias)[i];
            if( collectionMedia->IsSupported( KMPXMediaGeneralUri ) )
                {
                const TDesC& url = collectionMedia->ValueText( KMPXMediaGeneralUri );
                if( url.FindC( _L("z:" ) ) != KErrNotFound )
                    {
                    romCount++;
                    }
                }
            }

        delete medias;
        
        if( expectedValue != count )
            {
            // Check ROM videos are not interfering. 
            if( romCount != count - expectedValue )
                {
                err = KErrCorrupt;
                VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, values do not match: %d and expected: %d", count, expectedValue);
                }
            }
        }
    else
    // Other TUInt32 values.
    if( type == EVcxTestMediaAttributeTypeTUint32 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TUint32");

        TInt expectedValue;
        User::LeaveIfError( aItem.GetNextInt( expectedValue ) );

        TUint32 value = *(*media).Value<TUint32>( mpxMediaAttribute );

        if( expectedValue != value )
            {
            err = KErrCorrupt;
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, values do not match: %d and expected: %d", value, expectedValue);
            }
        }

    if( type == EVcxTestMediaAttributeTypeTReal32 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TReal32");

        TInt num;
        User::LeaveIfError( aItem.GetNextInt( num ) );
        TReal32 expectedValue( num );

        TInt digits;
        if( aItem.GetNextInt( digits ) == KErrNone )
            {
            expectedValue += static_cast<TReal32>(digits)/10000;
            }

        TReal32 value = *(*media).Value<TReal32>( mpxMediaAttribute );

        if( expectedValue != value )
            {
            err = KErrCorrupt;
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, values do not match: %f and expected: %f", value, expectedValue);
            }
        }

    if( type == EVcxTestMediaAttributeTypeTInt32 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TInt32");

        TInt expectedValue;
        User::LeaveIfError( aItem.GetNextInt( expectedValue ) );

        TInt32 value = *(*media).Value<TUint32>( mpxMediaAttribute );

        if( expectedValue != value )
            {
            err = KErrCorrupt;
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, values do not match: %d and expected: %d", value, expectedValue);
            }
        }

    if( type == EVcxTestMediaAttributeTypeTInt8 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TInt8");

        TInt expectedValue;
        User::LeaveIfError( aItem.GetNextInt( expectedValue ) );

        TInt8 value = *(*media).Value<TInt8>( mpxMediaAttribute );

        if( expectedValue != value )
            {
            err = KErrCorrupt;
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, values do not match: %d and expected: %d", value, expectedValue);
            }
        }

    if( type == EVcxTestMediaAttributeTypeTMPXItemId )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TMPXItemId");

        TInt expectedValue;
        User::LeaveIfError( aItem.GetNextInt( expectedValue ) );

        TInt expectedValue2;
        User::LeaveIfError( aItem.GetNextInt( expectedValue2 ) );

        TMPXItemId itemId;
        itemId.iId1 = expectedValue;
        itemId.iId2 = expectedValue2;

        TMPXItemId value = *(*media).Value<TMPXItemId>( mpxMediaAttribute );

        if( itemId != value )
            {
            err = KErrCorrupt;
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: expected Id1: %d, Id2: %d", itemId.iId1, itemId.iId2);
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: actual Id1: %d, Id2: %d", value.iId1, value.iId2);
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: ERROR no match!");
            }
        }

    if( type == EVcxTestMediaAttributeTypeTInt64 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type TInt64");

        TInt expectedValue;
        User::LeaveIfError( aItem.GetNextInt( expectedValue ) );

        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute from media");
        TInt64 value = *(*media).Value<TInt64>( mpxMediaAttribute );

        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: value %d", value);
        if( expectedValue != value )
            {
            err = KErrCorrupt;
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, values do not match: %d and expected: %d", value, expectedValue);
            }
        }

    if ( type == EVcxTestMediaAttributeTypeMvFlags )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Getting attribute of type MV flags ( TUint32 )");

        TUint32 value = *(*media).Value<TUint32>( mpxMediaAttribute );

        TInt expectedValue(0);
        while( aItem.GetNextInt( expectedValue ) == KErrNone )
            {
            if( ! (value & expectedValue) )
                {
                err = KErrCorrupt;
                VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: error, flag is: %d, flag %d is not set!", value, expectedValue);
                }
            }
        }
    
    if( err != KErrNone )
        {
        TRAP_IGNORE( iTester->PrintMediasL( NULL, EFalse, _L("Videos") ) );
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CheckMediaDetailL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::SetItemDetailsL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::SetMediaDetailL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::SetMediaDetailL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In SetMediaDetailL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    if( !iMpxMedia )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Error. iMpxMedia not set!");
        User::Leave( KErrArgument );
        }

    TInt mpxAttributeMappingId(0);
    User::LeaveIfError( aItem.GetNextInt( mpxAttributeMappingId ) );

    TVcxTestMediaAttributeType type;
    TMPXAttributeData mpxMediaAttribute;
    User::LeaveIfError( MapMpxMediaAttribute(
            static_cast<TVcxTestMediaAttributeIdMapping>(mpxAttributeMappingId),
            mpxMediaAttribute, type ) );

    TInt err( KErrNone );

    if( type == EVcxTestMediaAttributeTypeString )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type text");

        TPtrC value;
        User::LeaveIfError( aItem.GetNextString( value ) );

        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %S", &value);
        iMpxMedia->SetTextValueL( mpxMediaAttribute, value );
        }

    if( type == EVcxTestMediaAttributeTypeTTime )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type TInt64(date)");

        TTime time;
        time.UniversalTime();
        TTimeIntervalSeconds timeOffset = User::UTCOffset();
        TTime localTime = time + timeOffset;

        TInt64 value = localTime.Int64();
        iMpxMedia->SetTObjectValueL<TInt64>( mpxMediaAttribute, value );
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Set to current time.");
        }

    if( type == EVcxTestMediaAttributeTypeTDateTime )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type TDateTime");

        User::Leave( KErrNotSupported );
#if 0
        TDateTime time;
        iMpxMedia->SetTObjectValueL<TDateTime>( mpxMediaAttribute, time );
        //VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %d", value);
#endif
        }

    if( type == EVcxTestMediaAttributeTypeTInt )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type TInt");
        TInt value;
        User::LeaveIfError( aItem.GetNextInt( value ) );
        iMpxMedia->SetTObjectValueL<TInt>( mpxMediaAttribute, value );
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %d", value);
        }

    if( type == EVcxTestMediaAttributeTypeTUint )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type TUint");

        TInt value;
        User::LeaveIfError( aItem.GetNextInt( value ) );
        iMpxMedia->SetTObjectValueL<TUint>( mpxMediaAttribute, value );
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %d", value);
        }

    if( type == EVcxTestMediaAttributeTypeTUint8 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type TUint8");

        TInt value;
        User::LeaveIfError( aItem.GetNextInt( value ) );
        iMpxMedia->SetTObjectValueL<TUint8>( mpxMediaAttribute, value );
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %d", value);
        }

    if( type == EVcxTestMediaAttributeTypeTUint32 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type TUint32");

        TInt value;
        User::LeaveIfError( aItem.GetNextInt( value ) );
        iMpxMedia->SetTObjectValueL<TUint32>( mpxMediaAttribute, value );
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %d", value);
        }

    if( type == EVcxTestMediaAttributeTypeTReal32 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type TReal32");

        TInt num;
        User::LeaveIfError( aItem.GetNextInt( num ) );
        TReal32 value( num );

        TInt digits;
        if( aItem.GetNextInt( digits ) == KErrNone )
            {
            value += static_cast<TReal32>(digits)/10000;
            }

        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %f", value);

        iMpxMedia->SetTObjectValueL<TReal32>( mpxMediaAttribute, value );
        }

    if( type == EVcxTestMediaAttributeTypeTInt32 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type TInt32");

        TInt value;
        User::LeaveIfError( aItem.GetNextInt( value ) );
        iMpxMedia->SetTObjectValueL<TInt32>( mpxMediaAttribute, value );
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %d", value);
        }

    if( type == EVcxTestMediaAttributeTypeTMPXItemId )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type EVcxTestMediaAttributeTypeTMPXItemId");

        TInt value;
        User::LeaveIfError( aItem.GetNextInt( value ) );

        TInt value2;
        User::LeaveIfError( aItem.GetNextInt( value2 ) );

        TMPXItemId itemId;
        itemId.iId1 = value;
        itemId.iId2 = value2;

        iMpxMedia->SetTObjectValueL<TMPXItemId>( mpxMediaAttribute, itemId );
        VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: Set to Id1: %d, Id2: %d", value, value2);
        }

    if( type == EVcxTestMediaAttributeTypeTInt8 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type TInt8");

        TInt value;
        User::LeaveIfError( aItem.GetNextInt( value ) );
        iMpxMedia->SetTObjectValueL<TInt8>( mpxMediaAttribute, value );
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %d", value);
        }

    if( type == EVcxTestMediaAttributeTypeTInt64 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type TInt64");

        TInt value;
        User::LeaveIfError( aItem.GetNextInt( value ) );
        if( value == KVcxTestFileSize3GB )
            {
            iMpxMedia->SetTObjectValueL<TInt64>( mpxMediaAttribute, 3000000000 );
            }
        else
            {
            iMpxMedia->SetTObjectValueL<TInt64>( mpxMediaAttribute, value );
            }
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %d", value);
        }

    if ( type == EVcxTestMediaAttributeTypeMvFlags )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Setting attribute of type MV flags ( TUint32 )");

        TInt value(0);
        while( aItem.GetNextInt( value ) == KErrNone )
            {
            value |= value;
            }
        iMpxMedia->SetTObjectValueL<TUint32>( mpxMediaAttribute, value );
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Set to %d", value);
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::SetMediaDetailL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CheckMediaCountL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CheckMediaCountL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CheckMediaCountL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CheckMediaCountL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    WaitForRefreshL( EFalse ); // Wait for possible refresh to finish.
    
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt drive(0);
    User::LeaveIfError( GetDriveParam( aItem, drive ) );

    TInt expectedCount(0);
    User::LeaveIfError(aItem.GetNextInt(expectedCount));

    TRAPD( err, CheckMediasL( expectedCount, drive, 0 ) );
    
    if( err != KErrNone ) 
        {
        iTester->PrintMediasL( NULL, EFalse, _L("Videos") );
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CheckMediaCountL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CheckMinimumMediaCountL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CheckMinimumMediaCountL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CheckMinimumMediaCountL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CheckMinimumMediaCountL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    WaitForRefreshL( EFalse ); // Wait for possible refresh to finish.

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt drive(0);
    User::LeaveIfError( GetDriveParam( aItem, drive ) );

    TInt expectedCount(0);
    User::LeaveIfError(aItem.GetNextInt(expectedCount));

    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: drive: %d", drive);

    TRAPD( err, CheckMediasL( expectedCount, drive, -1 ) );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CheckMinimumMediaCountL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CheckMaximumMediaCountL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CheckMaximumMediaCountL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CheckMaximumMediaCountL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CheckMaximumMediaCountL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    WaitForRefreshL( EFalse ); // Wait for possible refresh to finish.

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt drive(0);
    User::LeaveIfError( GetDriveParam( aItem, drive ) );

    TInt expectedCount(0);
    User::LeaveIfError( aItem.GetNextInt(expectedCount) );

    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: drive: %d", drive);

    TRAPD( err, CheckMediasL( expectedCount, drive, 1 ) );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CheckMaximumMediaCountL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CheckDownloadCountL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CheckDownloadCountL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CheckDownloadCountL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CheckMediaCountL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    
    WaitForRefreshL( EFalse ); // Wait for possible refresh to finish.

    TInt expectedCount(0);
    User::LeaveIfError(aItem.GetNextInt(expectedCount));

    TInt err( KErrNotReady );
    if( iTester )
        {
        err = KErrNone;
        if( iTester->GetDownloadWatcher()->GetDownloadCount() != expectedCount )
            {
            err = KErrCorrupt;
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: ERROR: Not expected count of downloads!");
            }
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CheckDownloadCountL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::CreateCopyOfMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CreateCopyOfMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CreateCopyOfMediaL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CreateCopyOfMediaL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt drive(0);
    User::LeaveIfError( GetDriveParam( aItem, drive ) );

    TInt index(0);
    User::LeaveIfError( aItem.GetNextInt( index ) );

    if( index < 0 || index >= iTester->GetMediaCount() )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Error. Index (%d) out of bounds.", index);
        User::Leave( KErrArgument );
        }

    TInt fullCopy(1);
    if( aItem.GetNextInt( fullCopy ) != KErrNone )
        {
        fullCopy = 1;
        }

    if( iTester->GetMediaCount() <= 0 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: ERROR: There's zero medias!");
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CreateCopyOfMediaL <----------");
        return VCXMVTEST_ERR( KErrArgument );
        }

    if( iMpxMedia )
        {
        delete iMpxMedia;
        iMpxMedia = NULL;
        }

    TInt err( KErrNone );
    
    if( fullCopy )
        {
        iLastReceivedMessage = -1;
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Requesting full media details.");
        RArray<TInt> messages;
        messages.Append( KVCXMYVideosTestMessageCollectionMedia );
        TRAP( err, iTester->GetMediaFullDetailsL( drive, index ) );
        if( err == KErrNone )
            {
            TRAP( err, WaitForMessagesL( ETrue, messages, 5, ETrue ) );

            if( err == KErrNone )
                {
                CMPXMedia* copiedMedia = iTester->GetLastFullMedia();
    
                const TDesC& title = copiedMedia->ValueText( KMPXMediaGeneralTitle );
                TMPXItemId itemId = *(copiedMedia->Value<TMPXItemId>( KMPXMediaGeneralId ));
                
                VCXLOGLO4("CVCXMyVideosCollectionPluginTest:: Creating copy of MPX media: %S, id1: %d, id2: %d.", &title, itemId.iId1, itemId.iId2);
                TRAP( err, iMpxMedia = copiedMedia->CopyL( *copiedMedia ) );            
                }
            }
        messages.Reset();
        }
    else
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Creating empty MPX media.");
        iMpxMedia = CMPXMedia::NewL();

        CMPXMedia* copiedMedia( NULL );
        
        TRAP( err, copiedMedia = iTester->GetMediaL( drive, index ) );

        if( copiedMedia )
            {
            TMPXItemId mpxId = *(copiedMedia->Value<TMPXItemId>( KMPXMediaGeneralId ));
            iMpxMedia->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxId );
            }
        }
    
    if( err != KErrNone )
        {
        VCXLOGLO2("<<<CVCXMyVideosCollectionPluginTest:: err: %d", err);
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CreateCopyOfMediaL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::CreateEmptyMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CreateEmptyMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CreateEmptyMediaL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CreateEmptyMediaL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    if( iMpxMedia )
        {
        delete iMpxMedia;
        iMpxMedia = NULL;
        }

    iMpxMedia = CMPXMedia::NewL();

    TMPXItemId mpxId;
    mpxId.iId1 = 0;
    mpxId.iId2 = 0;
    iMpxMedia->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxId );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CreateEmptyMediaL <----------");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::AddMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::AddMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::AddMediaL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In AddMediaL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt syncCallInt(0);
    User::LeaveIfError( aItem.GetNextInt( syncCallInt ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCallInt);
    TBool syncCall = static_cast<TBool>(syncCallInt);

    TRAPD( err, iTester->AddMediaL( iMpxMedia, syncCall ) );

#ifndef  __WINSCW__
    // In urel builds add media is not supported, check mpx plugin file size if this is urel or udeb build.
    if( err == KErrNotSupported )
        {
        RFile rf;
        _LIT( KVCXTestSystemDriveMpxPluginPath, "z:\\sys\\bin\\vcxmyvideoscollectionplugin.dll" );
        _LIT( KVCXTestUserDriveMpxPluginPath, "c:\\sys\\bin\\vcxmyvideoscollectionplugin.dll" );

        TInt err2( KErrNone );
        if( BaflUtils::FileExists( iFs, KVCXTestUserDriveMpxPluginPath ) )
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Open C drive bin.");
            err2 = rf.Open( iFs, KVCXTestUserDriveMpxPluginPath, EFileRead|EFileShareAny );
            }
        else
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Open Z drive bin.");
            err2 = rf.Open( iFs, KVCXTestSystemDriveMpxPluginPath, EFileRead|EFileShareAny );
            }

        if( err2 == KErrNone )
            {
            TInt size(0);
            if( rf.Size( size ) == KErrNone )
                {
                if( size < 1024*40 )
                    {
                    err = KErrNone;
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: !OBS! Looks like UREL build, Add media failed as expected.");
                    iAddMediaCmdCount++; // Increase 'fake' video count for media count check.
                    }
                else
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: !OBS! ERROR: UDEB build I guess, add media failed, check this.");
                    }
                }
            rf.Close();
            }
        else
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Open plugin binary failed: %d", err2);
            }
        }
#endif // __WINSCW__


    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::AddMediaL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::SetMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::SetMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::SetMediaL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In SetMediaL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    if( !iMpxMedia )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Error. iMpxMedia not set!");
        User::Leave( KErrArgument );
        }

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt syncCallInt(0);
    User::LeaveIfError( aItem.GetNextInt( syncCallInt ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCallInt);
    TBool syncCall = static_cast<TBool>(syncCallInt);

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->SetMediaL( iMpxMedia, syncCall ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::SetMediaL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::OutsideMediaUpdateL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::OutsideMediaUpdateL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::OutsideMediaUpdateL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In OutsideMediaUpdateL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    if( !iMpxMedia )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Error. iMpxMedia not set!");
        User::Leave( KErrArgument );
        }

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TRAPD( err, iMdsDbModifier->UpdateVideoL( *iMpxMedia ) );

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::OutsideMediaUpdateL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::RemoveMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::RemoveMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::RemoveMediaL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In RemoveMediaL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt drive(0);
    User::LeaveIfError( GetDriveParam( aItem, drive ) );

    TInt index(0);
    User::LeaveIfError( aItem.GetNextInt( index ) );

    TBool syncCall( EFalse );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    iLastReceivedMessage = -1;

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->RemoveMediaL( drive, index, syncCall ) );
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::RemoveMediaL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::RemoveAllMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::RemoveAllMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::RemoveAllMediaL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In RemoveAllMediaL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TRAP_IGNORE( iTester->EnsureMediaFilesAreNotInUseL() );
    
    TInt error( KErrNone );
    
    TInt mediaCount = iTester->GetMediaCount();
    
    // Store info about the mpx items. 
    RArray<TUint> mpxIds;
    CleanupClosePushL( mpxIds ); 
            
    RArray<TUint> downloadIds;
    CleanupClosePushL( downloadIds );
    
    RArray<TInt> downloadStates;
    CleanupClosePushL( downloadStates );

    RPointerArray<HBufC> filePaths;
    CleanupResetAndDestroyPushL( filePaths );
    
    // Get ids and other needed details. 
    
    for( TInt i=0; i<mediaCount; i++ )
        {
        CMPXMedia* media( NULL );
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest::RemoveAllMediaL: GetMedia %d", i);
        TRAP( error, media = iTester->GetMediaL( -1, i ) );

        if( error != KErrNone )
            {
            CleanupStack::PopAndDestroy( &filePaths );
            CleanupStack::PopAndDestroy( &downloadStates );
            CleanupStack::PopAndDestroy( &downloadIds );
            CleanupStack::PopAndDestroy( &mpxIds );
            
            VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::RemoveAllMediaL <----------");
            return VCXMVTEST_ERR( error );
            }
        
        if( media && media->IsSupported( KMPXMediaGeneralUri ) )
            {
            TUint32 downloadId = 0;
            if( media->IsSupported( KVcxMediaMyVideosDownloadId ) )
                {
                downloadId = media->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );
                }

            TInt state = -1;
            if( media->IsSupported( KVcxMediaMyVideosDownloadState ) )
                {
                state = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );
                }                
                
            HBufC* path( NULL );
            if( media->IsSupported( KMPXMediaGeneralUri ) )
                {
                path = media->ValueText( KMPXMediaGeneralUri ).AllocL();
                }
            else
                {
                path = HBufC::NewL( 32 );
                }
            
            TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
            
            filePaths.Append( path );
            downloadIds.Append( downloadId );
            downloadStates.Append( state );
            mpxIds.Append( itemId.iId1 );
            }
        }
    
    // Cancel downloads.
    
    for( TInt i = mpxIds.Count()-1; i >= 0; i-- )
        {
        if( downloadIds[i] != 0 || downloadStates[i] != -1 )
            {
            // Cancel the download.
            RArray<TInt> messages;
            if( BaflUtils::FileExists( iFs, filePaths[i]->Des() ) )
                {
                messages.Append( KVCXMYVideosTestMessageMpxItemDeleted );
                }
            messages.Append( KVCXMYVideosTestMessageCommandComplete );
            messages.Append( KVCXMYVideosTestMessageCollectionOpened );
            TRAP( error, iTester->CancelDownloadL( mpxIds[i], downloadIds[i], filePaths[i]->Des(), EFalse ) );
            if( error == KErrNone )
                {
                TRAP( error, WaitForMessagesL( ETrue, messages, 30, ETrue ) );
                }
            messages.Reset();
            // Error occured, lets hope deleting works.
            if( error != KErrNone )
                {
                downloadIds[i] = 0;
                downloadStates[i] = -1;
                }
            }    
        }
    
    // Get current list of medias.
    
    CMPXMediaArray* medias( NULL );
    medias = iTester->SelectMediasL( -1, 0, iTester->GetMediaCount() );
    CleanupStack::PushL( medias );

    TInt retries = 3;
    
    // Delete the videos, and retry few times if needed.
    while( iTester->GetMediaCount() > 0 && retries-- > 0 )
        {
        // Get medias to delete.
        TRAP( error, iTester->DeleteMediasByMpxIdsL( medias, EFalse ) );
    
        if( error == KErrNone )
            {
            // Wait for deletion to complete.
            RArray<TInt> messages;
            CleanupClosePushL( messages );
            messages.Append( KVCXMYVideosTestMessageDeleteResp );
            messages.Append( KVCXMYVideosTestMessageCommandComplete );
            messages.Append( KVCXMYVideosTestMessageDeleteStarted );
    
            TInt timeout = medias->Count()*5 + 60;
    
            TRAP( error, WaitForMessagesL( ETrue, messages, timeout, ETrue ) );
            CleanupStack::PopAndDestroy( &messages );
            }
        
        CleanupStack::PopAndDestroy( medias );
        
        // Refresh the medias.
        RArray<TInt> messages;
        messages.Append( KVCXMYVideosTestMessageCollectionOpened );
        TRAP( error, iTester->RefreshContentsL() );
        if( error == KErrNone )
            {
            TRAP( error, WaitForMessagesL( ETrue, messages, 120, ETrue ) );
            }
        messages.Reset();
        
        medias = iTester->SelectMediasL( -1, 0, iTester->GetMediaCount() );
        CleanupStack::PushL( medias );        
        
        if( iTester->GetMediaCount() > 0 )
            {
            User::After( 60 * 1000000 ); // Wait a minute.
            }
        }

    CleanupStack::PopAndDestroy( medias );

    iLastReceivedMessage = -1;

    CleanupStack::PopAndDestroy( &filePaths );
    CleanupStack::PopAndDestroy( &downloadStates );
    CleanupStack::PopAndDestroy( &downloadIds );
    CleanupStack::PopAndDestroy( &mpxIds );

    if( iTester && iTester->GetDownloadWatcher() )
        {
        iTester->GetDownloadWatcher()->Reset();
        }

    if( iTester->GetMediaCount() > 0 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: All medias could not be removed."); 
        error = KErrCorrupt;
        }
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::RemoveAllMediaL <----------");
    return VCXMVTEST_ERR( error ); 
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::MoveMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::MoveMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::MoveMediaL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In MoveMediaL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TInt sourceDrive(0);
    User::LeaveIfError( GetDriveParam( aItem, sourceDrive ) );

    TInt index(0);
    User::LeaveIfError( aItem.GetNextInt( index ) );

    TInt destDrive(0);
    User::LeaveIfError( GetDriveParam( aItem, destDrive ) );

    TBool syncCall( EFalse );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->EnsureMediaFilesAreNotInUseL() );
        err = VCXMVTEST_ERR( err );
        if( err == KErrNone )
            {
            TRAP( err, iTester->MoveMediasL( sourceDrive, index, index+1, destDrive, syncCall ) );
            }
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::MoveMediaL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::MoveMediasL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::MoveMediasL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::MoveMediasL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In MoveMediasL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TInt sourceDrive(0);
    User::LeaveIfError( GetDriveParam( aItem, sourceDrive ) );

    TInt index(0);
    User::LeaveIfError( aItem.GetNextInt( index ) );

    TInt endIndex(0);
    User::LeaveIfError( aItem.GetNextInt( endIndex ) );

    TInt destDrive(0);
    User::LeaveIfError( GetDriveParam( aItem, destDrive ) );

    TBool syncCall( EFalse );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->EnsureMediaFilesAreNotInUseL() );
        err = VCXMVTEST_ERR( err );
        if( err == KErrNone )
            {
            TRAP( err, iTester->MoveMediasL( sourceDrive, index, endIndex, destDrive, syncCall ) );
            }
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::MoveMediasL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CancelMoveL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CancelMoveL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CancelMoveL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CancelMoveL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TBool syncCall( ETrue );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->CancelMoveOrCopyL( syncCall ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CancelMoveL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CopyMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CopyMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CopyMediaL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CopyMediaL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TInt sourceDrive(0);
    User::LeaveIfError( GetDriveParam( aItem, sourceDrive ) );

    TInt index(0);
    User::LeaveIfError( aItem.GetNextInt( index ) );

    TInt destDrive(0);
    User::LeaveIfError( GetDriveParam( aItem, destDrive ) );

    TBool syncCall( EFalse );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->CopyMediasL( sourceDrive, index, index+1, destDrive, syncCall ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CopyMediaL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CopyMediasL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CopyMediasL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CopyMediasL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CopyMediasL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TInt sourceDrive(0);
    User::LeaveIfError( GetDriveParam( aItem, sourceDrive ) );

    TInt index(0);
    User::LeaveIfError( aItem.GetNextInt( index ) );

    TInt endIndex(0);
    User::LeaveIfError( aItem.GetNextInt( endIndex ) );

    TInt destDrive(0);
    User::LeaveIfError( GetDriveParam( aItem, destDrive ) );

    TBool syncCall( EFalse );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->CopyMediasL( sourceDrive, index, endIndex, destDrive, syncCall ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CopyMediasL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CancelCopyL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CancelCopyL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CancelCopyL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CancelCopyL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TBool syncCall( EFalse );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->CancelMoveOrCopyL( syncCall ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CancelCopyL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::DeleteMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::DeleteMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::DeleteMediaL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In DeleteMediaL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TInt sourceDrive(0);
    User::LeaveIfError( GetDriveParam( aItem, sourceDrive ) );

    TInt index(0);
    User::LeaveIfError( aItem.GetNextInt( index ) );

    TBool syncCall( ETrue );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->EnsureMediaFilesAreNotInUseL() );
        err = VCXMVTEST_ERR( err );
        if( err == KErrNone )
            {
            TRAP( err, iTester->DeleteMediasL( sourceDrive, index, index+1, syncCall ) );
            }
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::DeleteMediaL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::DeleteMediasL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::DeleteMediasL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::DeleteMediasL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In DeleteMediasL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TInt sourceDrive(0);
    User::LeaveIfError( GetDriveParam( aItem, sourceDrive ) );

    TInt index(0);
    User::LeaveIfError( aItem.GetNextInt( index ) );

    TInt endIndex(0);
    User::LeaveIfError( aItem.GetNextInt( endIndex ) );

    TBool syncCall( EFalse );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->EnsureMediaFilesAreNotInUseL() );
        err = VCXMVTEST_ERR( err );
        if( err == KErrNone )
            {
            TRAP( err, iTester->DeleteMediasL( sourceDrive, index, endIndex, syncCall ) );
            }
        }

    if( err != KErrNone )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::DeleteMediasL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CancelDeleteL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CancelDeleteL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CancelDeleteL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CancelDeleteL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TBool syncCall( ETrue );
    TInt syncCallInt(0);
    if( aItem.GetNextInt( syncCallInt ) == KErrNone )
        {
        syncCall = static_cast<TBool>(syncCallInt);
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCall );

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->CancelDeleteL( syncCall ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CancelDeleteL <----------");
    return VCXMVTEST_ERR( err );
    }

//----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CancelNextOperationL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CancelNextOperationL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CancelNextOperationL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CancelNextOperationL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iCancelNextOperation = ETrue;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CancelNextOperationL <----------");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::DeleteFileOfMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::DeleteFileOfMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::DeleteFileOfMediaL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In DeleteFileOfMediaL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt drive(0);
    User::LeaveIfError( GetDriveParam( aItem, drive ) );

    TInt index(0);
    User::LeaveIfError( aItem.GetNextInt( index ) );

    if( index < 0 || index >= iTester->GetMediaCount() )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Error. Index (%d) out of bounds.", index);
        User::Leave( KErrArgument );
        }

    iLastReceivedMessage = -1;

    TRAPD( err, iTester->DeleteFileOfMediaL( drive, index ) );

    if( err != KErrNone )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::DeleteFileOfMediaL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::DeleteFilesOfAllMediasL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::DeleteFilesOfAllMediasL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::DeleteFilesOfAllMediasL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In DeleteFilesOfAllMediasL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->EnsureMediaFilesAreNotInUseL() );
        err = VCXMVTEST_ERR( err );
        if( err == KErrNone )
            {        
            TRAP( err, iTester->DeleteAllMediaFilesL() );
            }
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::DeleteFilesOfAllMediasL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CreateVideoFileL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CreateVideoFileL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CreateVideoFileL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CreateVideoFileL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt temp(0);

    // Parameter video type
    CIptvTestVideoCreator::TIptvTestVideoType videoType;
    User::LeaveIfError( aItem.GetNextInt(temp) );
    videoType = static_cast<CIptvTestVideoCreator::TIptvTestVideoType>(temp);
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: videoType: %d", videoType);

    // Parameter drive letter
    TPtrC driveLetter;
    User::LeaveIfError(aItem.GetNextString(driveLetter));
#ifdef __WINSCW__
    driveLetter.Set(_L("C"));
#endif

    // Parameter path and filename
    TPtrC filename;
    User::LeaveIfError(aItem.GetNextString(filename));
    if(filename.Length() < 64)
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: filename: %S", &filename);
        }

    // Parameter video size
    TInt size(-1);
    if( aItem.GetNextInt(size) != KErrNone )
        {
        size = -1;
        }
    
    // Parameter video count
    TInt count(1);
    if( aItem.GetNextInt( count ) != KErrNone || count <= 0 )
        {
        count = 1;
        }

    TRAPD( err, CreateVideoFileL( videoType, driveLetter, filename, size, count, ETrue ) );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CreateVideoFileL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CreateVideoFileNoWaitL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CreateVideoFileNoWaitL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CreateVideoFileNoWaitL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CreateVideoFileNoWaitL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt temp(0);

    // Parameter video type
    CIptvTestVideoCreator::TIptvTestVideoType videoType;
    User::LeaveIfError( aItem.GetNextInt(temp) );
    videoType = static_cast<CIptvTestVideoCreator::TIptvTestVideoType>(temp);
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: videoType: %d", videoType);

    // Parameter drive letter
    TPtrC driveLetter;
    User::LeaveIfError(aItem.GetNextString(driveLetter));
#ifdef __WINSCW__
    driveLetter.Set(_L("C"));
#endif
    
    // Parameter path and filename
    TPtrC filename;
    User::LeaveIfError(aItem.GetNextString(filename));
    if(filename.Length() < 64)
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: filename: %S", &filename);
        }

    // Parameter video size
    TInt size(-1);
    if( aItem.GetNextInt( size ) != KErrNone )
        {
        size = -1;
        }
    
    size = size == 0 ? size -1 : size;

    // Parameter video count
    TInt count(1);
    if( aItem.GetNextInt( count ) != KErrNone || count <= 0 )
        {
        count = 1;
        }

    TRAPD( err, CreateVideoFileL( videoType, driveLetter, filename, size, count, EFalse ) );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CreateVideoFileNoWaitL ---------->");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CreateVideoFileL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTest::CreateVideoFileL( CIptvTestVideoCreator::TIptvTestVideoType aType, const TDesC& aDriveLetter, const TDesC& aFileName, TInt aSize, TInt aVideoCount, TBool aDoSync )
    {
    HBufC* path = HBufC::NewL( 2048 );
    CleanupStack::PushL(path);

    // Make sure path exists and resolve path + filename
    TInt pos = aFileName.LocateReverse('\\');
    // Folder is specified in the test case
    if(pos != KErrNotFound)
        {
        path->Des().Copy( aDriveLetter );
        path->Des().Append( aFileName.Left( pos+1 ) );
        if( path->Des().Length() < 64 )
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: path: %S", path);
            }
        BaflUtils::EnsurePathExistsL(iFs, path->Des());

        path->Des().Copy( aDriveLetter );
        path->Des().Append( aFileName );
        }
    // No folder specified. If drive is C use :\data\my videos, else :\my videos
    else
        {
        path->Des().Copy( aDriveLetter );
        TInt driveNum(0);
        RFs::CharToDrive( aDriveLetter[0], driveNum );
        if( driveNum == EDriveC )
            {
            path->Des().Append( _L(":\\data\\my videos\\") );
            }
        else
            {
            path->Des().Append( _L(":\\my videos\\") );
            }

        if( path->Des().Length() < 64 )
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: path: %S", path);
            }

        BaflUtils::EnsurePathExistsL( iFs, path->Des() );
        path->Des().Append( aFileName );
        }

    path->ReAllocL( path->Des().Length()+1 );

    // Save path for later cleanup
    HBufC* pathOnly = path->Des().Left( path->Des().LocateReverse('\\') + 1 ).AllocL();
    CleanupStack::PushL( pathOnly );
    TBool pathStoredAlready = EFalse;
    for( TInt i = 0; i < iSideloadFolders.Count(); i++ )
        {
        if( iSideloadFolders[i]->Des().Find( *pathOnly ) != KErrNotFound )
            {
            pathStoredAlready = ETrue;
            break;
            }
        }
    if( !pathStoredAlready )
        {
        CleanupStack::Pop( pathOnly );
        iSideloadFolders.Append( pathOnly );
        }
    else
        {
        CleanupStack::PopAndDestroy( pathOnly );
        }

    // Create file(s)
    TInt err( KErrNone );

    iLastReceivedMessage = -1;

    if( aVideoCount == 1 )
        {
        // Save the path for later use.
        CleanupStack::Pop( path );
        iSideloadedFiles.Append( path );

        // Create the video.
        TRAP( err, iTestCommon->CreateVideoFileL( aType, *path, aSize ) );
        if(err != KErrNone)
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: CreateVideoFileL returned error: %d:", err);
            }
        TRAP_IGNORE( iStats->ActionStartL( KSideloadVideoActionId, _L("Sideload video") ) );
        }
    else
        {
        TRAP( err, iTestCommon->CreateVideoFilesL( aType, *path, aVideoCount, iSideloadedFiles ) );
        if(err != KErrNone)
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: CreateVideoFilesL returned error: %d:", err);
            }
        for( TInt i = 0; i < aVideoCount; i++ )
            {
            TRAP_IGNORE( iStats->ActionStartL( KSideloadVideoActionId, _L("Sideload video") ) );
            }
        CleanupStack::PopAndDestroy( path );
        }

    if( iTester )
        {
        // Wait until videos have been created and collection refreshed.
        // Only if category is open.
        if( aDoSync && iTester->GetCurrentLevel() == 3 )
            {
            // Disable automatic refresh if adding multiple videos.
            if( aVideoCount > 1 )
                {
                iTester->SetAutomaticRefresh( EFalse );
                }

            // Wait for insertion events.
            RArray<TInt> messages;
            for( TInt i=0; i<aVideoCount; i++ )
                {
                messages.Append( KVCXMYVideosTestMessageMpxItemInserted );
                }
            // Only one video added, refresh is automatic.
            if( aVideoCount <= 1 )
                {
                messages.Append( KVCXMYVideosTestMessageCollectionOpened );
                }
            
            TInt timeout = aVideoCount*5 + 60;
            
            WaitForMessagesL( ETrue, messages, timeout, ETrue );
            messages.Reset();

            // Multiple videos added, refresh.
            if( aVideoCount > 1 )
                {
                iTester->RefreshContentsL();
                messages.Append( KVCXMYVideosTestMessageCollectionOpened );
                CleanupClosePushL( messages );
                WaitForMessagesL( ETrue, messages, 60, ETrue );
                CleanupStack::PopAndDestroy( &messages );
                iTester->SetAutomaticRefresh( ETrue );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::EnsureDriveForVideosL
// Delete everything from sideloading folders.
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::EnsureDriveForVideosL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::EnsureDriveForVideosL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In EnsureDriveForVideosL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    
    // Parameter drive letter
    TPtrC driveLetter;
    User::LeaveIfError(aItem.GetNextString(driveLetter));
    
    TInt drive(0);
    if( driveLetter == _L("anydrive") )
        {
        drive = -1;
        }
    else
        {
        User::LeaveIfError( iFs.CharToDrive( driveLetter[0], drive ) );
        VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: drive: %S = %d", &driveLetter, drive );
        }    
    
    TInt amount(0);
    User::LeaveIfError( aItem.GetNextInt( amount ) );
    
    TInt videoCount = iTester->GetVideoCountForDrive( drive );
    
    TInt err( KErrNone );
    
    if( videoCount < amount )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest::EnsureDriveForVideosL: drive has less than %d videos.", amount);
        
        // Parameter path and filename
        TPtrC filename;
        User::LeaveIfError(aItem.GetNextString(filename));
        if(filename.Length() < 64)
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: filename: %S", &filename);
            }
        
        TInt createCount = amount - videoCount;
        
        TRAP( err, CreateVideoFileL( 
                CIptvTestVideoCreator::IptvTestVideoMpeg4, driveLetter, filename, -1, createCount, ETrue ) );    
        }
    else
    if( videoCount > amount )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest::EnsureDriveForVideosL: drive has more than %d videos.", amount);

        TInt deleteCount = videoCount - amount;

        TRAP_IGNORE( iTester->EnsureMediaFilesAreNotInUseL() );
        TRAP( err, iTester->DeleteMediasL( drive, 0, deleteCount, EFalse ) );

        if( err == KErrNone )
            {
            // Wait for deletion to complete.
            RArray<TInt> messages;
            CleanupClosePushL( messages );
            messages.Append( KVCXMYVideosTestMessageDeleteResp );
            messages.Append( KVCXMYVideosTestMessageCommandComplete );
            messages.Append( KVCXMYVideosTestMessageDeleteStarted );
    
            TInt timeout = deleteCount*5 + 60;
    
            TRAP_IGNORE( WaitForMessagesL( ETrue, messages, timeout, ETrue ) ); 
            CleanupStack::PopAndDestroy( &messages );
            }
        }
    else
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest::EnsureDriveForVideosL: drive already has %d videos.", amount);
        }
        
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::EnsureDriveForVideosL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::EmptySideloadFoldersL
// Delete everything from sideloading folders.
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::EmptySideloadFoldersL( CStifItemParser& /*aItem*/ )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::EmptySideloadFoldersL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In EmptySideloadFoldersL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Sideloaded file count: %d", iSideloadedFiles.Count());

    if( iSideloadedFiles.Count() <= 0 )
        {
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::EmptySideloadFoldersL <----------");
        return KErrNone;
        }

    RPointerArray<HBufC> filesInUse;
    CleanupResetAndDestroyPushL( filesInUse );

    iLastReceivedMessage = -1;

    TInt i;
    // Delete files
    for(i=iSideloadedFiles.Count()-1; i>=0; i--)
        {
        HBufC16* path = iSideloadedFiles[i];
        CleanupStack::PushL( path );
        iSideloadedFiles.Remove( i );

        TInt err = iFs.Delete( path->Des() );
        if( err == KErrInUse )
            {
            CleanupStack::Pop( path );
            filesInUse.Append( path );
            }
        else
            {
            CleanupStack::PopAndDestroy( path );
            if( err != KErrNone )
                {
                VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: delete error: %d", err);
                }
            }
        }

    // Try again deleting files which were in use
    for( i = filesInUse.Count()-1; i >= 0; i--)
        {
        HBufC16* path = filesInUse[i];
        CleanupStack::PushL( path );
        filesInUse.Remove( i );

        // Delete file
        iTestCommon->EnsureFileIsNotInUse( *path );
        TInt err = iFs.Delete( path->Des() );
        if( err != KErrNone )
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: delete 2nd error: %d", err);
            }

        CleanupStack::PopAndDestroy( path );
        }

    CleanupStack::PopAndDestroy( &filesInUse );
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::EmptySideloadFoldersL <----------");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::SetDeleteSideloadedVideos
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::SetDeleteSideloadedVideos( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::SetDeleteSideloadedVideos ---------->");
    iDeleteSideloadedFilesAndFolders = ETrue;
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::SetDeleteSideloadedVideos <----------");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::DownloadL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::DownloadL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::DownloadL ---------->");
    iLastReceivedMessage = -1;
    TRAPD( err, DownloadOrResumeL( aItem, EFalse, EFalse ) );
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::DownloadL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::ResumeDownloadL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::ResumeDownloadL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::ResumeDownloadL ---------->");
    iLastReceivedMessage = -1;
    TRAPD( err, DownloadOrResumeL( aItem, EFalse, ETrue ) );
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::ResumeDownloadL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::ResumeAllDownloadsL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::ResumeAllDownloadsL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::ResumeAllDownloadsL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In ResumeAllDownloadsL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TRAPD( err, iTester->ResumeAllDownloadsL() );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::ResumeAllDownloadsL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::DownloadUsingMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::DownloadUsingMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::DownloadUsingMediaL ---------->");

    if( !iMpxMedia )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Error. iMpxMedia not set!");
        User::Leave( KErrArgument );
        }

    iLastReceivedMessage = -1;

    TRAPD( err, DownloadOrResumeL( aItem, ETrue, EFalse ) );
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::DownloadUsingMediaL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::ResumeDownloadUsingMediaL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::ResumeDownloadUsingMediaL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::ResumeDownloadUsingMediaL ---------->");

    if( !iMpxMedia )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Error. iMpxMedia not set!");
        User::Leave( KErrArgument );
        }

    iLastReceivedMessage = -1;

    TRAPD( err, DownloadOrResumeL( aItem, ETrue, ETrue ) );
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::ResumeDownloadUsingMediaL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::DownloadOrResumeL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTest::DownloadOrResumeL( CStifItemParser& aItem, TBool aUseCurrentMedia, TBool aResume )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::DownloadOrResumeL");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In DownloadOrResumeL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    iLastReceivedMessage = -1;

    TUint32 iapId(0);
    TPtrC iapName;
    User::LeaveIfError( aItem.GetNextString( iapName ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: iapName: %S", &iapName);

    if( iapName == KVCXTestZeroIap )
        {
        iapId = 0;
        }
    else
    if( iapName == KVCXTestInvalidIap )
        {
        iapId = 10001;
        }
    else
        {
        User::LeaveIfError( iTestCommon->GetIapIdL(iapName, iapId) );
        }

    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: iapId: %d", iapId);

    TPtrC serviceName;
    User::LeaveIfError( aItem.GetNextString( serviceName ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: serviceName: %S", &serviceName);

    TInt serviceId(0);
    User::LeaveIfError( iServiceEmu->GetServiceIdL(serviceName, serviceId) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: serviceId: %d", serviceId);

    TInt contentId(0);
    User::LeaveIfError( aItem.GetNextInt( contentId ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: contentId: %d", contentId);

    TInt syncCallInt(0);
    User::LeaveIfError( aItem.GetNextInt( syncCallInt ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCallInt);
    TBool syncCall = static_cast<TBool>(syncCallInt);

    TPtrC userName;
    if( KErrNone == aItem.GetNextString( userName ) )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: userName: %S", &userName);
        }
    else
        {
        userName.Set( _L("") );
        }

    TPtrC password;
    if( KErrNone == aItem.GetNextString( password ) )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: password: %S", &password);
        }
    else
        {
        password.Set( _L("") );
        }

    const TPtrC url = iServiceEmu->GetContentUrlL( serviceId, contentId );
    const TPtrC title = iServiceEmu->GetContentTitleL( serviceId, contentId );

    TInt err(0);
    if( !aUseCurrentMedia )
        {
        if( !aResume )
            {
            TRAP( err, iTester->StartDownloadL( title, iapId, serviceId, contentId, url, syncCall, userName, password, NULL ) );
            }
        else
            {
            TRAP( err, iTester->ResumeDownloadL( title, iapId, serviceId, contentId, url, syncCall, userName, password, NULL ) );
            }
        }
    else
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: passing current media to MPX plugin.");
        if( !aResume )
            {
            TRAP( err, iTester->StartDownloadL( title, iapId, serviceId, contentId, url, syncCall, userName, password, iMpxMedia ) );
            }
        else
            {
            TRAP( err, iTester->ResumeDownloadL( title, iapId, serviceId, contentId, url, syncCall, userName, password, iMpxMedia ) );
            }
        }

    // Set autoresume on if there's only one paused download.
    CVCXMyVideosTestDlWatcher* dlWatcher = iTester->GetDownloadWatcher();
    if( aResume && dlWatcher && !iAutoResume )
        {
        TInt pausedDlCount = 0;
        for( TInt i=0; i<dlWatcher->GetDownloadCount(); i++ ) 
            {
            CVCXMyVideosTestDownload* dl = dlWatcher->GetDownloadByIndex( i );
            if( dl->iState == EVcxMyVideosDlStatePaused )
                {
                pausedDlCount++;
                }
            }
            if( pausedDlCount == 1 ) 
                {
                iAutoResume = ETrue;
                iTester->SetAutoResume( ETrue );
                }
        }
    
    if( err != KErrNone )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);
        User::Leave( err );
        }
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::DownloadOrResumeL");
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::DownloadUrlL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::DownloadUrlL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::DownloadUrlL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In DownloadUrlL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TUint32 iapId(0);
    TPtrC iapName;
    User::LeaveIfError( aItem.GetNextString( iapName ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: iapName: %S", &iapName);
    User::LeaveIfError( iTestCommon->GetIapIdL(iapName, iapId) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: iapId: %d", iapId);

    TPtrC url;
    User::LeaveIfError( aItem.GetNextString( url ) );
    
    TInt syncCallInt(0);
    User::LeaveIfError( aItem.GetNextInt( syncCallInt ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCallInt);
    TBool syncCall = static_cast<TBool>(syncCallInt);

    TPtrC userName;
    if( KErrNone == aItem.GetNextString( userName ) )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: userName: %S", &userName);
        }
    else
        {
        userName.Set( _L("") );
        }

    TPtrC password;
    if( KErrNone == aItem.GetNextString( password ) )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: password: %S", &password);
        }
    else
        {
        password.Set( _L("") );
        }

    if( url == KVCXTestZeroUrl)
        {
        url.Set( _L("" ) );
        }

    TInt err( KErrNone );
    
    if( iTester )
        {
        if( url == KVCXMYVideosTest2kURLTag )
            {
            TRAP( err, iTester->StartDownloadL( _L("2kcharsurl"), iapId, 0, 0, KVCXMYVideosTest2kURL, syncCall, userName, password, NULL ) );
            }
        else
        if( url == KVCXMYVideosTest1kURLTag )
            {
            TRAP( err, iTester->StartDownloadL( _L("1kcharsurl"), iapId, 0, 0, KVCXMYVideosTest1kURL, syncCall, userName, password, NULL ) );
            }
        else
            {
            TRAP( err, iTester->StartDownloadL( _L("urldl"), iapId, 0, 0, url, syncCall, userName, password, NULL ) );
            }
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::DownloadUrlL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CancelDownloadL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CancelDownloadL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CancelDownloadL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CancelDownloadL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TPtrC serviceName;
    User::LeaveIfError( aItem.GetNextString( serviceName ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: serviceName: %S", &serviceName);

    TInt serviceId(0);
    User::LeaveIfError( iServiceEmu->GetServiceIdL(serviceName, serviceId) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: serviceId: %d", serviceId);

    TInt contentId(0);
    User::LeaveIfError( aItem.GetNextInt( contentId ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: contentId: %d", contentId);

    TInt syncCallInt(0);
    User::LeaveIfError( aItem.GetNextInt( syncCallInt ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCallInt);
    TBool syncCall = static_cast<TBool>(syncCallInt);

    const TPtrC url = iServiceEmu->GetContentUrlL( serviceId, contentId );

    CVCXMyVideosTestDownload* dl = iTester->GetDownloadWatcher()->GetDownload( serviceId, contentId, url );

    TInt err( KErrNotReady );
    if( !dl )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Could not find download with specified params!");
        }
    else
    if( iTester )
        {
        TRAP( err, iTester->CancelDownloadL( dl, syncCall ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CancelDownloadL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CancelDownloadByIdL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CancelDownloadByIdL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CancelDownloadByIdL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CancelDownloadByIdL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TInt mpxId(0);
    User::LeaveIfError( aItem.GetNextInt( mpxId ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: mpxId: %d", mpxId);

    TInt downloadId(0);
    User::LeaveIfError( aItem.GetNextInt( downloadId ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: downloadId: %d", downloadId);

    TInt syncCallInt(0);
    User::LeaveIfError( aItem.GetNextInt( syncCallInt ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCallInt);
    TBool syncCall = static_cast<TBool>(syncCallInt);

    _LIT(KVCxTestEmptyString, "NULL");
    TPtrC downloadPath;
    if( aItem.GetNextString( downloadPath ) != KErrNone )
        {
        downloadPath.Set( KVCxTestEmptyString );
        }

    TInt err( KErrNotReady );
    if( iTester )
        {
        TRAP( err, iTester->CancelDownloadL( mpxId, downloadId, downloadPath, syncCall ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CancelDownloadByIdL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::PauseDownloadL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::PauseDownloadL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::PauseDownloadL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In PauseDownloadL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TPtrC serviceName;
    User::LeaveIfError( aItem.GetNextString( serviceName ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: serviceName: %S", &serviceName);

    TInt serviceId(0);
    User::LeaveIfError( iServiceEmu->GetServiceIdL(serviceName, serviceId) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: serviceId: %d", serviceId);

    TInt contentId(0);
    User::LeaveIfError( aItem.GetNextInt( contentId ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: contentId: %d", contentId);

    TInt syncCallInt(0);
    User::LeaveIfError( aItem.GetNextInt( syncCallInt ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: callTypeInt: %d", syncCallInt);
    TBool syncCall = static_cast<TBool>(syncCallInt);

    const TPtrC url = iServiceEmu->GetContentUrlL( serviceId, contentId );

    TInt err( KErrNotReady );
    if( iTester )
        {
        // Disable autoresume
        iAutoResume = EFalse;
        iTester->SetAutoResume( iAutoResume );

        TRAP( err, iTester->PauseDownloadL( serviceId, contentId, url, syncCall ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::PauseDownloadL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::PauseDownloadByUrlL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::PauseDownloadByUrlL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::PauseDownloadByUrlL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In PauseDownloadByUrlL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TPtrC url;
    User::LeaveIfError( aItem.GetNextString( url ) );

    TInt err( KErrNotReady );
    if( iTester )
        {
        // Disable autoresume
        iAutoResume = EFalse;
        iTester->SetAutoResume( iAutoResume );

        TRAP( err, iTester->PauseDownloadL( url, EFalse ) );
        }

    if( err != KErrNone )
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: err: %d", err);

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::PauseDownloadL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CheckDownloadProgressL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::CheckDownloadProgressL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CheckDownloadProgressL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In CheckDownloadProgressL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iLastReceivedMessage = -1;

    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TPtrC serviceName;
    User::LeaveIfError( aItem.GetNextString( serviceName ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: serviceName: %S", &serviceName);

    TInt serviceId(0);
    User::LeaveIfError( iServiceEmu->GetServiceIdL(serviceName, serviceId) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: serviceId: %d", serviceId);

    TInt contentId(0);
    User::LeaveIfError( aItem.GetNextInt( contentId ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: contentId: %d", contentId);

    TInt minimumProgress( 0 );
    User::LeaveIfError( aItem.GetNextInt( minimumProgress ) );

    const TPtrC url = iServiceEmu->GetContentUrlL( serviceId, contentId );

    CVCXMyVideosTestDlWatcher* dlWatcher = iTester->GetDownloadWatcher();

    TInt err( KErrNotReady );

    if( dlWatcher )
        {
        CVCXMyVideosTestDownload* dl = dlWatcher->GetDownload( serviceId, contentId, url );

        if( dl )
            {
            if( dl->iProgress < minimumProgress )
                {
                err = KErrCorrupt;
                }
            else
                {
                err = KErrNone;
                }
            }
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CheckDownloadProgressL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::WaitAllDownloadsL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::WaitAllDownloadsL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::WaitAllDownloadsL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXMyVideosCollectionPluginTest" );
    _LIT( KWhere, "In WaitAllDownloadsL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt err( KErrNone );

    TInt minutes(0);
    if( aItem.GetNextInt( minutes ) != KErrNone )
        {
        minutes = 10;
        }
    iTimeoutTimer->After( KTimeoutMinute * minutes );

    iWaitingForAllDownloads = ETrue;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::WaitAllDownloadsL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::WaitForMessageL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::WaitForMessageL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::WaitForMessageL ---------->");

    _LIT( KWhere, "In WaitForMessageL" );
    // Print to log file
    iLog->Log( KWhere );

    TInt message(0);
    User::LeaveIfError( aItem.GetNextInt( message ) );

    TInt timeout(0);
    User::LeaveIfError( aItem.GetNextInt( timeout ) );

    TInt count(0);
    if( aItem.GetNextInt( count ) != KErrNone )
        {
        count = 1;
        }
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: count: %d", count);

    _LIT(KWhat,"MsgWait");
    _LIT(KDesc, "All: %d");
    TestModuleIf().Printf( 0, KWhat, KDesc, count );
    
    RArray<TInt> messages;

    iWaitedTestMessages.Reset();
    for( TInt i=0; i<count; i++ )
        {
        messages.Append( message );
        }

    TRAPD( err, WaitForMessagesL( EFalse, messages, timeout, ETrue ) );
    
    messages.Reset();

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::WaitForMessageL <----------");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::WaitForAnyMessageL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::WaitForAnyMessageL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::WaitForAnyMessageL ---------->");
    // Print to UI
    _LIT( KWhere, "In WaitForAnyMessageL" );
    // Print to log file
    iLog->Log( KWhere );

    TInt timeout(0);
    User::LeaveIfError( aItem.GetNextInt( timeout ) );

    RArray<TInt> messages;

    TInt message(0);
    TInt error = KErrNone;
    while( error == KErrNone )
        {
        error = aItem.GetNextInt( message );
        if( error == KErrNone )
            {
            messages.Append( message );
            }
        }

    _LIT(KWhat,"MsgWait");
    _LIT(KDesc, "Any: %d");
    TestModuleIf().Printf( 0, KWhat, KDesc, messages.Count() );

    if( error != KErrNone && error != KErrNotFound )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Error reading parameters from script.");
        }
    else
        {
        TRAP( error, WaitForMessagesL( EFalse, messages, timeout, EFalse ) );
        }

    messages.Reset();

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::WaitForAnyMessageL <----------");
    return VCXMVTEST_ERR( error );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::WaitForMessagesL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::WaitForMessagesL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::WaitForMessagesL ---------->");
    // Print to UI
    _LIT( KWhere, "In WaitForMessagesL" );
    // Print to log file
    iLog->Log( KWhere );

    TInt timeout(0);
    User::LeaveIfError( aItem.GetNextInt( timeout ) );

    RArray<TInt> messages;

    TInt message(0);
    TInt error = KErrNone;
    while( error == KErrNone )
        {
        error = aItem.GetNextInt( message );
        if( error == KErrNone )
            {
            messages.Append( message );
            }
        }

    _LIT(KWhat,"MsgWait");
    _LIT(KDesc, "All: %d");
    TestModuleIf().Printf( 0, KWhat, KDesc, messages.Count() );
    
    if( error != KErrNone && error != KErrNotFound )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Error reading parameters from script.");
        }
    else
        {
        TRAP( error, WaitForMessagesL( EFalse, messages, timeout, ETrue ) );
        }

    messages.Reset();

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::WaitForMessagesL <----------");
    return VCXMVTEST_ERR( error );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::WaitForMessagesL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTest::WaitForMessagesL( TBool aBlock, RArray<TInt>& aMessages, TInt aTimeout,
        TBool aWaitAll )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::WaitForMessagesL 2");
    if( aMessages.Count() <= 0 )
        {
        User::Leave( KErrArgument );
        }

    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Timeout: %d seconds", aTimeout);

    iWaitingForAllMessages = aWaitAll;

    iMessageWaitTimeout = aTimeout;
    iTimeoutTimer->After( aTimeout * KTimeoutSecond );

    iWaitedTestMessages.Reset();

    TBool messageReceivedAlready( EFalse );

    for( TInt i=0; i<aMessages.Count(); i++ )
        {
        TBuf<256> msgStr;
        GetMessageDesc( aMessages[i], msgStr);

        VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: MsgQueue: wait: %S (%d)", &msgStr, aMessages[i]);

        iWaitedTestMessages.Append( aMessages[i] );

        if( aMessages[i] == iLastReceivedMessage )
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Received this message already.");
            iLastReceivedMessage = -1;

            HandleVcxMvTesterMessageL( aMessages[i], KErrNone );
            messageReceivedAlready = ETrue;
            }
        }

    if( aBlock && !messageReceivedAlready )
        {
        iMessageWaitIsBlocking = ETrue;
        iActiveWaitBlocking->Start();
        iMessageWaitIsBlocking = EFalse;
        
        // Only leave if it was blocking call.
        TInt error( iEventError );
        iEventError = KErrNone;
        User::LeaveIfError( error );
        }
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::WaitForMessagesL 2");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTest::DeleteDirectoryIfEmpty()
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::DeleteDirectoryIfEmpty( RFs& aFs, CFileMan& aFileMan, const TDesC& aDirectory )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::DeleteDirectoryIfEmpty");
    CDir* dirList = NULL;

    TInt result = aFs.GetDir( aDirectory, KEntryAttNormal | KEntryAttMatchMask, ESortByName, dirList );
    if( result != KErrNone ) VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: getdir result: %d", result );
    if( result == KErrNone && dirList->Count() == 0 )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: deleting folder: %S", &aDirectory );
        TInt err(KErrNone);
        err = aFileMan.RmDir( aDirectory );
        if( err != KErrNone )
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: delete error: %d", err );
        }

    delete dirList;
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::DeleteDirectoryIfEmpty");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::MapMpxMediaAttribute
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::MapMpxMediaAttribute(
        TVcxTestMediaAttributeIdMapping aMapping, TMPXAttributeData& aAttribute, TVcxTestMediaAttributeType& aType )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::MapMpxMediaAttribute");

    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: mapping id: %d", aMapping);

    aAttribute.iAttributeId = 0;
    aAttribute.iContentId = 0;

    switch( aMapping )
        {
        case EVcxTestMapping_KMPXMediaGeneralId:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralId");
            aAttribute = KMPXMediaGeneralId;
            aType = EVcxTestMediaAttributeTypeTMPXItemId;
            }
        break;

        case EVcxTestMapping_KMPXMediaGeneralTitle:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralTitle");
            aAttribute = KMPXMediaGeneralTitle;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KMPXMediaGeneralComment:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralComment");
            aAttribute = KMPXMediaGeneralComment;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KMPXMediaGeneralUri:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralUri");
            aAttribute = KMPXMediaGeneralUri;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KMPXMediaGeneralSize:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralSize");
            aAttribute = KMPXMediaGeneralSize;
            aType = EVcxTestMediaAttributeTypeTInt;
            }
            break;

        case EVcxTestMapping_KMPXMediaGeneralExtSizeInt64:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralExtSizeInt64");
            aAttribute = KMPXMediaGeneralExtSizeInt64;
            aType = EVcxTestMediaAttributeTypeTInt64;
            }
            break;            
            
        case EVcxTestMapping_KMPXMediaGeneralDate:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralDate");
            aAttribute = KMPXMediaGeneralDate;
            aType = EVcxTestMediaAttributeTypeTInt64;
            }
            break;

        case EVcxTestMapping_KMPXMediaGeneralFlags:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralFlags");
            aAttribute = KMPXMediaGeneralFlags;
            aType = EVcxTestMediaAttributeTypeMvFlags;
            }
            break;

        case EVcxTestMapping_KMPXMediaGeneralCopyright:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralCopyright");
            aAttribute = KMPXMediaGeneralCopyright;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KMPXMediaGeneralMimeType:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralMimeType");
            aAttribute = KMPXMediaGeneralMimeType;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosModifiedDate:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosModifiedDate");
            aAttribute = KVcxMediaMyVideosModifiedDate;
            aType = EVcxTestMediaAttributeTypeTInt64;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosModifiedDateCurrent:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosModifiedDate");
            aAttribute = KVcxMediaMyVideosModifiedDate;
            aType = EVcxTestMediaAttributeTypeTTime;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosAgeProfile:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosAgeProfile");
            aAttribute = KVcxMediaMyVideosAgeProfile;
            aType = EVcxTestMediaAttributeTypeTInt32;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosAudioLanguage:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosAudioLanguage");
            aAttribute = KVcxMediaMyVideosAudioLanguage;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosAuthor:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosAuthor");
            aAttribute = KVcxMediaMyVideosAuthor;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosOrigin:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosOrigin");
            aAttribute = KVcxMediaMyVideosOrigin;
            aType = EVcxTestMediaAttributeTypeTUint8;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosDuration:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosDuration");
            aAttribute = KVcxMediaMyVideosDuration;
            aType = EVcxTestMediaAttributeTypeTReal32;
            }
            break;

        case EVcxTestMapping_KMPXMediaGeneralLastPlaybackPosition:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KMPXMediaGeneralLastPlaybackPosition");
            aAttribute = KMPXMediaGeneralLastPlaybackPosition;
            aType = EVcxTestMediaAttributeTypeTInt;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosDownloadId:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosDownloadId");
            aAttribute = KVcxMediaMyVideosDownloadId;
            aType = EVcxTestMediaAttributeTypeTUint32;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosRemoteUrl:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosRemoteUrl");
            aAttribute = KVcxMediaMyVideosRemoteUrl;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosDownloadState:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosDownloadState");
            aAttribute = KVcxMediaMyVideosDownloadState;
            aType = EVcxTestMediaAttributeTypeTUint8;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosIapId:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosIapId");
            aAttribute = KVcxMediaMyVideosIapId;
            aType = EVcxTestMediaAttributeTypeTUint32;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosUsername:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosUsername");
            aAttribute = KVcxMediaMyVideosUsername;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosPassword:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosPassword");
            aAttribute = KVcxMediaMyVideosPassword;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosCommandId:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosCommandId");
            aAttribute = KVcxMediaMyVideosCommandId;
            aType = EVcxTestMediaAttributeTypeTUint;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosDownloadProgress:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosDownloadProgress");
            aAttribute = KVcxMediaMyVideosDownloadProgress;
            aType = EVcxTestMediaAttributeTypeTInt8;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosCategoryItemCount:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosCategoryItemCount");
            aAttribute = KVcxMediaMyVideosCategoryItemCount;
            aType = EVcxTestMediaAttributeTypeTUint32;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosCategoryNewItemCount:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosCategoryNewItemCount");
            aAttribute = KVcxMediaMyVideosCategoryNewItemCount;
            aType = EVcxTestMediaAttributeTypeTUint32;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosCategoryNewItemName:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosCategoryNewItemName");
            aAttribute = KVcxMediaMyVideosCategoryNewItemName;
            aType = EVcxTestMediaAttributeTypeString;
            }
            break;

        case EVcxTestMapping_KVcxMediaMyVideosAudioFourCc:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: KVcxMediaMyVideosAudioFourCc");
            aAttribute = KVcxMediaMyVideosAudioFourCc;
            aType = EVcxTestMediaAttributeTypeTUint32;
            }
            break;
        }

    VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: iAttributeId: %d, iContentId: %d", aAttribute.iAttributeId == 0, aAttribute.iContentId);

    TInt err( KErrNone );
    if( aAttribute.iAttributeId == 0 && aAttribute.iContentId == 0 )
        {
        err = KErrNotFound;
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::MapMpxMediaAttribute");
    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::GetMessageDesc
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTest::GetMessageDesc( TInt aMessage, TDes& aMsgBuff )
    {
    switch( aMessage )
        {
        case KVCXMYVideosTestMessageNotAccepted:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageNotAccepted") );
            }
            break;

        case KVCXMYVideosTestMessageDlSucceeded:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageDlSucceeded") );
            }
            break;
        case KVCXMYVideosTestMessageDlFailed:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageDlFailed") );
            }
            break;
        case KVCXMYVideosTestMessageDlPaused:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageDlPaused") );
            }
            break;
        case KVCXMYVideosTestMessageDlGeneral:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageDlGeneral") );
            }
            break;
        case KVCXMYVideosTestMessageDlRunning:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageDlRunning") );
            }
            break;
        case KVCXMYVideosTestMessageDlCanceled:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageDlCanceled") );
            }
            break;
        case KVCXMYVideosTestMessageDlProgressed:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageDlProgressed") );
            }
            break;
        case KVCXMYVideosTestMessageCollectionMessageError:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageCollectionMessageError") );
            }
            break;
        case KVCXMYVideosTestMessageCollectionOpened:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageCollectionOpened") );
            }
            break;
        case KVCXMYVideosTestMessagePlaylistOpened:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessagePlaylistOpened") );
            }
            break;
        case KVCXMYVideosTestMessageCollectionMedia:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageCollectionMedia") );
            }
            break;
        case KVCXMYVideosTestMessageCommandComplete:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageCommandComplete") );
            }
            break;
        case KVCXMYVideosTestMessageGotMediasByKMpxId:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageGotMediasByKMpxId") );
            }
            break;
        case KVCXMYVideosTestMessageMoveResp:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageMoveResp") );
            }
            break;
        case KVCXMYVideosTestMessageCopyResp:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageCopyResp") );
            }
            break;
        case KVCXMYVideosTestMessageCopyOrMoveStarted:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageCopyOrMoveStarted") );
            }
            break;
        case KVCXMYVideosTestMessageDeleteStarted:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageDeleteStarted") );
            }
            break;
        case KVCXMYVideosTestMessageDeleteResp:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageDeleteResp") );
            }
            break;
        case KVCXMYVideosTestMessageCommandCanceled:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageCommandCanceled") );
            }
            break;
        case KVCXMYVideosTestMessageCollectionGeneral:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageCollectionGeneral") );
            }
            break;
        case KVCXMYVideosTestMessageCollectionItemChanged:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageCollectionItemChanged") );
            }
            break;
        case KVCXMYVideosTestMessageCollectionCategoryChanged:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageCollectionCategoryChanged") );
            }
            break;
        case KVCXMYVideosTestMessageMpxItemInserted:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageMpxItemInserted") );
            }
            break;
        case KVCXMYVideosTestMessageMpxItemDeleted:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageMpxItemDeleted") );
            }
            break;
        case KVCXMYVideosTestMessageMpxItemModified:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestMessageMpxItemModified") );
            }
            break;
        case KVCXMyVideosTestMessageVideoListOrderChanged:
            {
            aMsgBuff.Copy( _L("KVCXMyVideosTestMessageVideoListOrderChanged") );
            }
            break;
        case KVCXMYVideosTestErrorDownloadNotFound:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestErrorDownloadNotFound") );
            }
            break;
        case KVCXMYVideosTestErrorFileNotFound:
            {
            aMsgBuff.Copy( _L("KVCXMYVideosTestErrorFileNotFound") );
            }
            break;

        default:
            {
            aMsgBuff.Copy( _L("") );
            }
            break;

        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::CheckMediasL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTest::CheckMediasL( TInt aExpectedVideoCount, TInt aDrive, TInt aCheckType )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::CheckMediasL");

    TInt actualVideoCount(0);

    // File check
    TInt videoCount = iTester->GetMediaCount();

    // attribute check that they are in allowed values

    for( TInt i = videoCount-1; i >= 0; i-- )
        {
        CMPXMedia* media = iTester->GetMediaL( -1, i );

        if( media->IsSupported( KMPXMediaGeneralUri ) )
            {
            TMPXItemId itemId  = *( media->Value<TMPXItemId>( KMPXMediaGeneralId ) );

            const TDesC& localFilePath = media->ValueText( KMPXMediaGeneralUri );

            // Drive specified
            if( aDrive != -1 )
                {
                TInt drive(0);
                User::LeaveIfError( iFs.CharToDrive( localFilePath[0], drive ) );
                // Don't count/check this file.
                if( drive != aDrive )
                    {
                    continue;
                    }
                }

            actualVideoCount++;

            TInt state = -1;
            if( media->IsSupported( KVcxMediaMyVideosDownloadState ) )
                {
                state = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );
                }

            TUint32 downloadId = 0;
            if( media->IsSupported( KVcxMediaMyVideosDownloadId ) )
                {
                downloadId = media->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );
                }

            // No file check for ongoing downloads
            if( !iFileCheckDisabled && ( downloadId == 0 || state == EVcxMyVideosDlStateDownloaded ) )
                {
                if( !iFs.IsValidName( localFilePath ) )
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTest: ERROR. File path is not valid.");
                    VCXLOGLO2("Local file path: %S", &localFilePath);
                    User::Leave( KErrCorrupt );
                    }

                if( !BaflUtils::FileExists( iFs, localFilePath ) )
                    {
                    VCXLOGLO2("CVCXMyVideosCollectionPluginTest: ERROR. File doesn't exist! Mpx id: %d", itemId.iId1);
                    VCXLOGLO2("Local file path: %S", &localFilePath);
                    User::Leave( KErrGeneral );
                    }

                for( TInt e = videoCount-1; e >= 0; e-- )
                    {
                    CMPXMedia* media2 = iTester->GetMediaL( -1, e );
                    TMPXItemId itemId2 = *( media2->Value<TMPXItemId>( KMPXMediaGeneralId ) );
                    if( media2->IsSupported( KMPXMediaGeneralUri ) )
                        {
                        const TDesC& localFilePath2 = media2->ValueText( KMPXMediaGeneralUri );

                        TCollationMethod cm = *Mem::CollationMethodByIndex( 0 );
                        cm.iFlags |= TCollationMethod::EFoldCase;

                        if( localFilePath.CompareC( localFilePath2, 0, &cm ) == KErrNone && itemId.iId1 != itemId2.iId1 )
                            {
                            VCXLOGLO3("CVCXMyVideosCollectionPluginTest: ERROR. 2 items with same path! Mpx id: %d & id: %d", itemId.iId1, itemId2.iId1);
                            User::Leave( KErrGeneral );
                            }
                        }
                    }
                }
            }
        }

    _LIT(KWhat,"CountCheck");
    _LIT(KDesc, "%d/%d");
    TestModuleIf().Printf( 0, KWhat, KDesc, actualVideoCount, aExpectedVideoCount);

    VCXLOGLO2("CVCXMyVideosCollectionPluginTest: Actual count: %d", actualVideoCount );

    if( aCheckType < 0 )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest: Minimum count: %d", aExpectedVideoCount );

        if ( actualVideoCount < aExpectedVideoCount )
            {
            User::Leave( KErrGeneral );
            }
        }
    else
    if( aCheckType > 0 )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest: Maximum count: %d", aExpectedVideoCount );

        if ( actualVideoCount > aExpectedVideoCount )
            {
            User::Leave( KErrGeneral );
            }
        }
    else
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest: Expected count: %d", aExpectedVideoCount );

        if ( aExpectedVideoCount + iAddMediaCmdCount != actualVideoCount )
            {
            User::Leave( KErrGeneral );
            }
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::CheckMediasL");
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::GetDriveParam
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTest::GetDriveParam( CStifItemParser& aItem, TInt& aDrive )
    {
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );

    TPtrC driveLetter;
    TInt err = aItem.GetNextString( driveLetter );
    if( err == KErrNone )
        {
        if( driveLetter == _L("anydrive") )
            {
            aDrive = -1;
            }
        else
            {
            err = iFs.CharToDrive( driveLetter[0], aDrive );
            VCXLOGLO3("CVCXMyVideosCollectionPluginTest:: drive: %S = %d", &driveLetter, aDrive );
            }
        }

    return VCXMVTEST_ERR( err );
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::DoTotalRefreshL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTest::DoTotalRefreshL()
    {
    if( iTester && iTester->GetCurrentLevel() == 3 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: Close & Open ------>");

        TInt currentOpenLevelIndex = iTester->GetCurrentOpenLevelIndex();

        // Close current level
        TRAPD( err, iTester->CloseLevelL() );
        if( err == KErrNone ) 
        	{
        	WaitForRefreshL( ETrue );
        	}

        // Open it again
        TRAP( err, iTester->OpenLevelL( currentOpenLevelIndex ) );
        if( err == KErrNone ) 
        	{
        	WaitForRefreshL( ETrue );
        	}

        
        // And refresh
        TRAP( err, iTester->RefreshContentsL() );
        if( err == KErrNone ) 
        	{
        	WaitForRefreshL( ETrue );
        	}
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::WaitForRefreshL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTest::WaitForRefreshL( TBool aForcedWait )
    {
    if( iTester && ( iTester->IsRefreshing() || aForcedWait ) )
        {
        RArray<TInt> messages;
        messages.Append( KVCXMYVideosTestMessageCollectionOpened );
        CleanupClosePushL( messages );
        WaitForMessagesL( ETrue, messages, 10, ETrue );
        CleanupStack::PopAndDestroy( &messages );
        }
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::GetMediasForLevelL
// -----------------------------------------------------------------------------
//
CMPXMediaArray* CVCXMyVideosCollectionPluginTest::GetMediasForLevelL( TInt aLevel, TUint aFlagsFilter )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::GetMediasForLevelL ---------->");

    iLastReceivedMessage = -1;

    TInt prevCurrentLevel = iTester->GetCurrentLevel();
    TInt prevOpenLevelIndex = iTester->GetCurrentOpenLevelIndex();
    
    TInt error( KErrNotReady );
    
    TBool wrongLevelOpen = ( prevCurrentLevel == 3 && prevOpenLevelIndex != aLevel );
    TBool levelNotOpen = ( prevCurrentLevel >= 2 );
    
    if( wrongLevelOpen || levelNotOpen )
        {
        RArray<TInt> messages;
        CleanupClosePushL( messages );
        if( !levelNotOpen )
            {
            // Close current level.
            messages.Append( KVCXMYVideosTestMessageCollectionOpened );
            TRAP( error, iTester->CloseLevelL() );
            if( error == KErrNone )
                {
                WaitForMessagesL( ETrue, messages, 10, ETrue );
                }
            messages.Reset();
            }
            
        // Open the wanted level.
        messages.Append( KVCXMYVideosTestMessageCollectionOpened );
        TRAP( error, iTester->OpenLevelL( aLevel ) );
        if( error == KErrNone )
            {
            WaitForMessagesL( ETrue, messages, 30, ETrue );
            }
        
        CleanupStack::PopAndDestroy( &messages );
        }

    // Get the medias.
    const CMPXMediaArray* collectionMediaArray = iTester->GetAllCollectionMedia();
    
    CMPXMediaArray* medias = CMPXMediaArray::NewL();
    
    for( TInt i = 0; i < collectionMediaArray->Count(); i++ )
        {
        CMPXMedia* media = (*collectionMediaArray)[i];
        // Default flags is none if item does not have the detail.
        TInt flags( 0 ); 
        if( media->IsSupported( KMPXMediaGeneralFlags ) )
            {
            flags = *media->Value<TUint>( KMPXMediaGeneralFlags );
            }
        // No flags specified or it matches with medias flags
        TBool fagsMatch = static_cast<TBool>( flags & aFlagsFilter );
        if( !aFlagsFilter || fagsMatch )
            {
            if( media->IsSupported( KMPXMediaGeneralTitle ) )
                {
                const TDesC& title = media->ValueText( KMPXMediaGeneralTitle );
                VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Adding media: %S", &title);
                }
            
            medias->AppendL( *media );
            }
        }
    
    if( levelNotOpen || wrongLevelOpen )
        {
        // Close current level.        
        TRAP( error, iTester->CloseLevelL() );
        if( error == KErrNone )
            {
            RArray<TInt> messages;
            CleanupClosePushL( messages );
            messages.Append( KVCXMYVideosTestMessageCollectionOpened );
            WaitForMessagesL( ETrue, messages, 10, ETrue );
            CleanupStack::PopAndDestroy( &messages );
            }

        // Open the previous level.
        if( wrongLevelOpen ) 
            {
            TRAP( error, iTester->OpenLevelL( prevOpenLevelIndex ) );
            if( error == KErrNone )
                {
                RArray<TInt> messages;
                CleanupClosePushL( messages );
                messages.Append( KVCXMYVideosTestMessageCollectionOpened );
                WaitForMessagesL( ETrue, messages, 30, ETrue );
                CleanupStack::PopAndDestroy( &messages );
                }
            }
        }
    
    User::LeaveIfError( error );
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::GetMediasForLevelL <----------");
    return medias;
    }

// -----------------------------------------------------------------------------
// CIptvMyVideosApiTest::HandleVcxMvTesterMessageL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTest::HandleVcxMvTesterMessageL( TInt aMessage, TInt aError )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTest::HandleVcxMvTesterMessageL");

    TBuf<256> msgStr;
    GetMessageDesc( aMessage, msgStr);

    iLastReceivedMessage = aMessage;
    
    VCXLOGLO4("CVCXMyVideosCollectionPluginTest:: MsgQueue: received: %S (%d), aError: %d", &msgStr, aMessage, aError);

    // Tester requires to abort test run.
    if( aMessage == KVCXMYVideosTestMessageCollectionGeneral && aError == KErrAbort ) 
        {
        iEventError = aError;
        Signal( aError );
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::HandleVcxMvTesterMessageL");
        return;
        }

    // Print info about video count to STIF UI.
    if( aMessage == KVCXMYVideosTestMessageCollectionOpened ||
        aMessage == KVCXMYVideosTestMessageListComplete ||
        aMessage == KVCXMYVideosTestMessageCollectionOpened ||
        aMessage == KVCXMYVideosTestMessageGotMediasByKMpxId )
        {
        _LIT(KWhat, "VidCount");
        _LIT(KDesc, "%d");
        TInt videoCount = iTester->GetMediaCount();
        TestModuleIf().Printf( 0, KWhat, KDesc, videoCount );
        }


    // Waiting for messages from mpx plugin to stop.
    if( iCoolDownWait )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: MsgQueue: Cooling down.");
        iCoolDownTimer->After( KTimeoutSecond * 5 );
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::HandleVcxMvTesterMessageL");
        return;
        }    
    
    if( aMessage == KVCXMYVideosTestMessageDlSucceeded ||
        aMessage == KVCXMYVideosTestMessageDlFailed ||
        aMessage == KVCXMYVideosTestMessageDlPaused ||
        aMessage == KVCXMYVideosTestMessageDlGeneral ||
        aMessage == KVCXMYVideosTestErrorDownloadNotFound )
        {
        TInt activeDownloads = iTester->GetActiveDownloadCountL();
        TInt downloads = iTester->GetDownloadWatcher()->GetDownloadCount();
        _LIT(KWhat, "Downloads");
        _LIT(KDesc, "%d/%d");
        TestModuleIf().Printf( 0, KWhat, KDesc, activeDownloads, downloads );

        if( aError != KErrNone )
            {
            iLastDownloadError = aError;
            }        
        }
    
    // Inform script of paused download when auto resume is off.
    if( !iWaitingForAllDownloads && aMessage == KVCXMYVideosTestMessageDlPaused && !iAutoResume )
        {
        // Add message to waited list and continue processing, check for signaling is done later.
        iWaitedTestMessages.Reset();
        iWaitedTestMessages.AppendL( aMessage );
        }
    
    // Waiting for all downloads to complete. Check downloads and signal if needed.
    if( iWaitingForAllDownloads )
        {
        TInt activeDownloads = iTester->GetActiveDownloadCountL();
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: Ongoing download count: %d", activeDownloads);
        
        if( activeDownloads <= 0 )
            {
            iTimeoutTimer->CancelTimer();
            iWaitingForAllDownloads = EFalse;

            TInt downloadCount = iTester->GetDownloadWatcher()->GetDownloadCount();
            TInt maxFailsToSucceed = downloadCount / 3;
            TInt fails = iTester->GetDownloadWatcher()->GetFailedDownloadCount();

            if( maxFailsToSucceed <= 0 )
                {
                maxFailsToSucceed = 0;
                }

            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: download count: %d", downloadCount);
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: failed download count: %d", fails);
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: max allowed fails: %d", maxFailsToSucceed);

            if( fails > maxFailsToSucceed )
                {
                if( iLastDownloadError != KErrNone )
                    {
                    iEventError = iLastDownloadError;
                    }
                else
                    {
                    iEventError = KVCXMYVideosTestMessageDlFailed;
                    }
                }
            else
                {
                iEventError = KErrNone;
                }

            // Add message to waited list and continue processing, check for signaling is done later.
            iWaitedTestMessages.Reset();
            iWaitedTestMessages.AppendL( aMessage );
            }
        }

    // See if there's cancel needed for move, copy or delete.
    if( aError == KErrNone && iCancelNextOperation )
        {
        if( aMessage == KVCXMYVideosTestMessageCopyOrMoveStarted )
            {
            TRAP( aError, iTester->CancelMoveOrCopyL( ETrue ) );
            }

        if( aMessage == KVCXMYVideosTestMessageDeleteStarted )
            {
            TRAP( aError, iTester->CancelDeleteL( ETrue ) );
            iCancelNextOperation = EFalse;
            }
        }

    // Check for errors. 
    if( aError != KErrNone && !iWaitingForAllDownloads )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: ----------> MsgQueue: Error: %d <----------", aError);
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

        iLastReceivedMessage = 0;
        if( !iMessageWaitIsBlocking )
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: ----------> MsgQueue: Signaling <----------");
            Signal( VCXMVTEST_ERR( aError ) );
            TestModuleIf().Printf( 0, _L("Signal"), _L("%d"), aError );
            iEventError = KErrNone;
            }
        else
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: ----------> MsgQueue: Stop wait <----------");
            iActiveWaitBlocking->Stop();
            iEventError = aError;
            }

        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::HandleVcxMvTesterMessageL");
        return;
        }

    // Signal script if message is waited for. 
    for( TInt i = 0; i < iWaitedTestMessages.Count(); i++ )
        {
        if( aMessage == iWaitedTestMessages[i] )
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: MsgQueue: found from wait queue.");
            iWaitedTestMessages.Remove(i);

            TBool doSignal = EFalse;

            // Signal for any matching message.
            if( !iWaitingForAllMessages )
                {
                VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: MsgQueue: Waiting any message, signal ok.");
                doSignal = ETrue;
                }
            // Signal if all messages have been received.
            else
                {
                if( iWaitedTestMessages.Count() >= 0 )
                    {
                    _LIT(KWhat,"MsgWait");
                    _LIT(KDesc, "Left: %d");
                    TestModuleIf().Printf( 0, KWhat, KDesc, iWaitedTestMessages.Count() );
                    }

                VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: MsgQueue: %d messages left in queue.", iWaitedTestMessages.Count() );
                if( iWaitedTestMessages.Count() == 0 )
                    {
                    doSignal = ETrue;
                    }
                else
                    {
                    // Restart the timeout for next message.
                    iTimeoutTimer->Cancel();
                    iTimeoutTimer->After( iMessageWaitTimeout * KTimeoutSecond );
                    break;
                    }
                }

            if( doSignal )
                {
                iWaitedTestMessages.Reset();

                if( iEventError != KErrNone )
                    {
                    VCXLOGLO1( "CVCXMyVideosCollectionPluginTest:: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: ----------> MsgQueue: Error: %d <---------", iEventError);
                    VCXLOGLO1( "CVCXMyVideosCollectionPluginTest:: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    }

                if( !iMessageWaitIsBlocking )
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: ----------> MsgQueue: Signaling <----------");
                    Signal( VCXMVTEST_ERR( iEventError ) );
                    TestModuleIf().Printf( 0, _L("Signal"), _L("%d"), iEventError );
                    iEventError = KErrNone;
                    }
                else
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTest:: ----------> MsgQueue: Stop wait <----------");
                    iActiveWaitBlocking->Stop();
                    }

                iTimeoutTimer->CancelTimer();
                iLastReceivedMessage = 0;
                break;
                }
            }
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTest::HandleVcxMvTesterMessageL");
    }

// -----------------------------------------------------------------------------
// CIptvEpgEngineTest::TimerComplete
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTest::TimerComplete(TInt aTimerId, TInt aError)
    {
    if( aTimerId == 0 )
        {
        if( iActiveWaitBlocking )
            iActiveWaitBlocking->Stop();
        
        if( aError == KErrNone )
            {
            VCXLOGLO2( "CVCXMyVideosCollectionPluginTest::TimerComplete: TESTCASE TIMEOUT -------> Last error from events: %d.", 
                    iEventError );

            iEventError = ( iEventError != KErrNone ? iEventError : KErrTimedOut );
            
            // Script is waiting for signal.
            if( !iMessageWaitIsBlocking )
                {
                VCXLOGLO2( "CVCXMyVideosCollectionPluginTest:: Signaling: %d", iEventError);
                Signal( VCXMVTEST_ERR( iEventError ) );
                }
            
            TRAP_IGNORE( iStats->ActionEndL( 0, iEventError ) );

            TestModuleIf().Printf( 0, _L("Timedout"), _L("%d"), iEventError );
            }
        }

    if( aTimerId == 1 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTest::TimerComplete: Cooldown done. ------->");
        iActiveWait->Stop();
        iCoolDownWait = EFalse;

        iWaitingForAllDownloads = EFalse;
        iWaitedTestMessages.Reset();
        iLastReceivedMessage = 0;
        }
    }

//  End of File

