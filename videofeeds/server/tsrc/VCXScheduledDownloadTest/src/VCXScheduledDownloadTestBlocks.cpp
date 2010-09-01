/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains testclass implementation.*
*/


// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <centralrepository.h>

#include "VCXTestLog.h"
#include "VCXScheduledDownloadTest.h"
#include "VCXScheduledDownloadTester.h"
#include "VCXTestCommon.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CVCXScheduledDownloadTest::Delete() 
    {
    delete iTester;
    iTester = NULL;
    
    delete iTestCommon;
    iTestCommon = NULL;

    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Create", CVCXScheduledDownloadTest::CreateL ),
        ENTRY( "RemoveAllMedias", CVCXScheduledDownloadTest::RemoveAllMediasL ),
        ENTRY( "RemoveAllSchedules", CVCXScheduledDownloadTest::RemoveAllSchedulesL ),
        ENTRY( "RemoveVideo", CVCXScheduledDownloadTest::RemoveVideoL ),
        ENTRY( "Download", CVCXScheduledDownloadTest::DownloadL ),
        ENTRY( "CancelDownload", CVCXScheduledDownloadTest::CancelDownloadL ),
        ENTRY( "WaitForMessages", CVCXScheduledDownloadTest::WaitForMessagesL ),
        ENTRY( "CoolDown", CVCXScheduledDownloadTest::CoolDown ),
        ENTRY( "CheckVideoCount", CVCXScheduledDownloadTest::CheckVideoCount ),
        ENTRY( "CheckMinimumVideoCount", CVCXScheduledDownloadTest::CheckMinimumVideoCount ),
        ENTRY( "CheckMaximumVideoCount", CVCXScheduledDownloadTest::CheckMaximumVideoCount ),
        ENTRY( "CheckError", CVCXScheduledDownloadTest::CheckError ),
        ENTRY( "SetPreferredMemory", CVCXScheduledDownloadTest::SetPreferredMemoryL ),
        
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::CreateL
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::CreateL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::CreateL");
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In CreateL" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    User::LeaveIfError( iFs.Connect() );
    
    TRAPD( err, iTester = CVCXScheduledDownloadTester::NewL( this ) );
    
    if( err != KErrNone )
        {
        VCXLOGLO2("CVCXScheduledDownloadTest:: error: %d", err);
        }
    
    iTestCommon = CVCXTestCommon::NewL();
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::CreateL");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::RemoveAllMediasL
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::RemoveAllMediasL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::RemoveAllMediasL");
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In RemoveAllMediasL" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
    
    TRAPD( err, iTester->RemoveAllMediasL() );
    
    if( err != KErrNone )
        {
        VCXLOGLO2("CVCXScheduledDownloadTest:: error: %d", err);
        }
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::RemoveAllMediasL");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::RemoveAllSchedulesL
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::RemoveAllSchedulesL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::RemoveAllSchedulesL");
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In RemoveAllSchedulesL" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
    
    TRAPD( err, iTester->RemoveAllSchedulesL() );
    
    if( err != KErrNone )
        {
        VCXLOGLO2("CVCXScheduledDownloadTest:: error: %d", err);
        }
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::RemoveAllSchedulesL");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::RemoveVideoL
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::RemoveVideoL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::RemoveAllMediasL");
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In RemoveVideoL" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TInt index;
    User::LeaveIfError( aItem.GetNextInt( index ) );
    
    TRAPD( err, iTester->RemoveVideoL( index ) );
    
    if( err != KErrNone )
        {
        VCXLOGLO2("CVCXScheduledDownloadTest:: error: %d", err);
        }
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::RemoveAllMediasL");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::DownloadL
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::DownloadL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::DownloadL");
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In DownloadL" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
    
    TUint32 iapId(0);
    TPtrC iapName;
    User::LeaveIfError( aItem.GetNextString( iapName ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: iapName: %S", &iapName);
    User::LeaveIfError( iTestCommon->GetIapIdL( iapName, iapId ) );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: iapId: %d", iapId);

    TPtrC url;
    User::LeaveIfError( aItem.GetNextString( url ) );
    
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
    
    TRAPD( err, iTester->DownloadL( url, iapId, userName, password ) );
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::DownloadL");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::CancelDownloadL
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::CancelDownloadL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::CancelDownloadL");
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In CancelDownloadL" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TInt index;
    User::LeaveIfError( aItem.GetNextInt( index ) );
    
    TRAPD( err, iTester->CancelDownloadL( index ) );
    
    if( err != KErrNone )
        {
        VCXLOGLO2("CVCXScheduledDownloadTest:: error: %d", err);
        }
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::CancelDownloadL");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::WaitForMessagesL
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::WaitForMessagesL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::WaitForMessagesL");
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In WaitForMessagesL" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    RArray<TInt32> messages;
    
    TInt timeoutSeconds(0);
    User::LeaveIfError( aItem.GetNextInt( timeoutSeconds ) );
    
    TInt msg;
    while( aItem.GetNextInt( msg ) == KErrNone )
        {
        messages.Append( msg );
        }
    
    TInt err( KErrArgument );
    
    if( messages.Count() > 0 )
        {
        TRAP( err, iTester->WaitForVideoEventsL( timeoutSeconds, messages ) );
        }
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::WaitForMessagesL");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::CoolDown
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::CoolDown( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::CoolDown");
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In CoolDown" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    iTester->CoolDown();
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::CoolDown");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::CheckVideoCount
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::CheckVideoCount( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::CheckVideoCount");    
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In CheckVideoCount" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
    
    TInt expectedCount;
    
    User::LeaveIfError( aItem.GetNextInt( expectedCount ) );
    
    TInt err( KErrNone );
    
    TRAP( err, iTester->RefreshL() );
    
    if( err == KErrNone && iTester->VideoCount() != expectedCount )
        {
        err = KErrCorrupt;
        }
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::CheckVideoCount");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::CheckMinimumVideoCount
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::CheckMinimumVideoCount( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::CheckMinimumVideoCount");    
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In CheckMinimumVideoCount" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TInt expectedCount;
    
    User::LeaveIfError( aItem.GetNextInt( expectedCount ) );
    
    TInt err( KErrNone );
    
    TRAP( err, iTester->RefreshL() );
    
    if( err == KErrNone && iTester->VideoCount() < expectedCount )
        {
        err = KErrCorrupt;
        }
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::CheckMinimumVideoCount");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::CheckMaximumVideoCount
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::CheckMaximumVideoCount( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::CheckMaximumVideoCount");    
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In CheckMaximumVideoCount" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TInt expectedCount;
    
    User::LeaveIfError( aItem.GetNextInt( expectedCount ) );
    
    TInt err( KErrNone );
    
    TRAP( err, iTester->RefreshL() );
    
    if( err == KErrNone && iTester->VideoCount() > expectedCount )
        {
        err = KErrCorrupt;
        }
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::CheckMaximumVideoCount");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::CheckError
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::CheckError( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::CheckError");    
    // Print to UI
    _LIT( KVCXScheduledDownloadTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In CheckError" );
    TestModuleIf().Printf( 0, KVCXScheduledDownloadTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TInt err = iTester->GetLastError();
    
    VCXLOGLO1("<<<CVCXScheduledDownloadTest::CheckError");
    return err;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::SetPreferredMemoryL
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::SetPreferredMemoryL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledDownloadTest::SetPreferredMemoryL ---------->");
    // Print to UI
    _LIT( KVCXMyVideosCollectionPluginTest, "VCXScheduledDownloadTest" );
    _LIT( KWhere, "In SetPreferredMemoryL" );
    TestModuleIf().Printf( 0, KVCXMyVideosCollectionPluginTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

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

    VCXLOGLO1("<<<CVCXScheduledDownloadTest::SetPreferredMemoryL <----------");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledDownloadTest::GetDriveParam
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledDownloadTest::GetDriveParam( CStifItemParser& aItem, TInt& aDrive )
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
            VCXLOGLO3("CVCXScheduledDownloadTest:: drive: %S = %d", &driveLetter, aDrive );
            }
        }

    return err;
    }

//  [End of File] - Do not remove
