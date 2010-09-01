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
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "VCXSettingsDatabaseTest.h"

#include "VCXTestLog.h"
#include "CIptvSettingsDatabase.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

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

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvSettingsDatabaseTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CIptvSettingsDatabaseTest::Delete()
    {
	if( iDatabase )
		{
		delete iDatabase;
		}
    }

// -----------------------------------------------------------------------------
// CIptvSettingsDatabaseTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CIptvSettingsDatabaseTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Create", CIptvSettingsDatabaseTest::CreateL ),
        ENTRY( "Destroy", CIptvSettingsDatabaseTest::DestroyL ),
        ENTRY( "TestSettings", CIptvSettingsDatabaseTest::TestSettingsL ),
        ENTRY( "TestLastUsage", CIptvSettingsDatabaseTest::TestLastUsageL )
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CIptvSettingsDatabaseTest::CreateL
//
// -----------------------------------------------------------------------------
//
TInt CIptvSettingsDatabaseTest::CreateL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KIptvSettingsDatabaseTest, "IptvSettingsDatabaseTest" );
    _LIT( KWhere, "In Create" );
    TestModuleIf().Printf( 0, KIptvSettingsDatabaseTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    VCXLOGLO1(">>>CIptvSettingsDatabaseTest::CreateL");

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

	if( aItem.GetNextString(iDriveLetter) != KErrNone )
	    {
	    VCXLOGLO1("<<<CIptvSettingsDatabaseTest::CreateL: invalid argument!");
	    return KErrArgument;
	    }
	VCXLOGLO2("CIptvSettingsDatabaseTest::CreateL iDriveLetter: %S", &iDriveLetter);

	if( aItem.GetNextString(iDatabaseFilename) != KErrNone )
	    {
	    VCXLOGLO1("<<<CIptvSettingsDatabaseTest::CreateL: invalid argument!");
	    return KErrArgument;
	    }
	VCXLOGLO2("CIptvSettingsDatabaseTest::CreateL iDatabaseFilename: %S", &iDatabaseFilename);

	TInt err( 0 );

	TBuf<256> tempPath;
	tempPath.Format( KSettingsDatabaseTestPath, &iDriveLetter );
	VCXLOGLO2("CIptvSettingsDatabaseTest::Checking path: %S", &tempPath);
	CheckPath( tempPath );

	if( iDatabaseFilename.Compare(_L("Badname") ) == 0)
		{
		VCXLOGLO1("CIptvSettingsDatabaseTest::CreateL: Using bad filename");
		TBuf<256> tempFileName;
		tempFileName.Append( iDriveLetter );
		tempFileName.Append( KSettingsDatabaseTestBadFilename );
		VCXLOGLO2("CIptvSettingsDatabaseTest::CreateL filename: %S", &tempFileName);
	    TRAP( err, iDatabase = CIptvSettingsDatabase::NewL( tempFileName ));
		}
	else
		{
		TBuf<256> tempFileName;
		tempFileName.Format( KSettingsDatabaseTestFileName, &iDriveLetter, &iDatabaseFilename );
		VCXLOGLO2("CIptvSettingsDatabaseTest::CreateL filename: %S", &tempFileName);
		PrepareFile( tempFileName );
	  TRAP( err, iDatabase = CIptvSettingsDatabase::NewL( tempFileName ) )
		}

	if( err != KErrNone )
		{
		VCXLOGLO2("CIptvSettingsDatabaseTest::CreateL: creating instance failed! error %d", err);
		}

    VCXLOGLO1("<<<CIptvSettingsDatabaseTest::CreateL");
    return err;

    }

// -----------------------------------------------------------------------------
// CIptvServiceTest::DestroyServiceL
// DestroyServiceL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvSettingsDatabaseTest::DestroyL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvSettingsDatabaseTest::DestroyL");
    // Print to UI
    _LIT( KIptvSettingsDatabaseTest, "IptvSettingsDatabaseTest" );
    _LIT( KWhere, "In DestroyL" );
    TestModuleIf().Printf( 0, KIptvSettingsDatabaseTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt err = KErrNone;

	if(iDatabase)
		{
		delete iDatabase;
		iDatabase = NULL;
		}

	VCXLOGLO1("<<<CIptvSettingsDatabaseTest::DestroyL");
    return err;
    }

// -----------------------------------------------------------------------------
// CIptvSettingsDatabaseTest::SetSettingsL
//
// -----------------------------------------------------------------------------
//
TInt CIptvSettingsDatabaseTest::TestSettingsL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KIptvSettingsDatabaseTest, "IptvSettingsDatabaseTest" );
    _LIT( KWhere, "In TestSettings" );
    TestModuleIf().Printf( 0, KIptvSettingsDatabaseTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    VCXLOGLO1(">>>CIptvSettingsDatabaseTest::TestSettings");

    TInt iParentalControl;
    if ( aItem.GetNextInt( iParentalControl ) != KErrNone )
    	{
    	iParentalControl = KMaxTInt;
    	}

    TInt iRecordingTimeReserve;
    if ( !aItem.GetNextInt( iRecordingTimeReserve ) != KErrNone )
    	{
    	iRecordingTimeReserve = KMaxTInt;
    	}

    TInt iReminderTime;
    if( !aItem.GetNextInt( iReminderTime ) != KErrNone )
    	{
    	iReminderTime = KMaxTInt;
    	}

    iSettings.iParentalControl = iParentalControl;
    iSettings.iRecordingTimeReserve = iRecordingTimeReserve;
    iSettings.iReminderTime = iReminderTime;
    iSettings.iLastUsedServiceId = 0;
    iSettings.iLastUsedChannelId = 0;

    iDatabase->SaveSettingsL( iSettings );

    TIptvLiveUISettings iReceivedSettings;
    iDatabase->ReadSettingsL( iReceivedSettings );

    if( (iReceivedSettings.iParentalControl != iSettings.iParentalControl) &&
    	(iReceivedSettings.iRecordingTimeReserve != iSettings.iRecordingTimeReserve) &&
    	(iReceivedSettings.iReminderTime != iSettings.iReminderTime) &&
    	(iReceivedSettings.iLastUsedServiceId != iSettings.iLastUsedServiceId) &&
    	(iReceivedSettings.iLastUsedChannelId != iSettings.iLastUsedChannelId)
    	)
    {
		VCXLOGLO1("CIptvSettingsDatabaseTest::TestSettingsL: settings do not match!");
		VCXLOGLO1("<<<CIptvSettingsDatabaseTest::TestSettingsL");
		return KErrArgument;
    }

    VCXLOGLO1("<<<CIptvSettingsDatabaseTest::TestSettingsL");
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIptvSettingsDatabaseTest::TestLastUsageL
//
// -----------------------------------------------------------------------------
//
TInt CIptvSettingsDatabaseTest::TestLastUsageL( CStifItemParser& /* aItem */ )
    {
    // Print to UI
    _LIT( KIptvSettingsDatabaseTest, "IptvSettingsDatabaseTest" );
    _LIT( KWhere, "In SetSettings" );
    TestModuleIf().Printf( 0, KIptvSettingsDatabaseTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    VCXLOGLO1(">>>CIptvSettingsDatabaseTest::TestLastUsageL");

    iDatabase->SaveLastUsageL( iSettings );

    TIptvLiveUISettings iReceivedSettings;
    iDatabase->ReadLastUsageL( iReceivedSettings );

    if( (iReceivedSettings.iParentalControl != iSettings.iParentalControl) &&
    	(iReceivedSettings.iRecordingTimeReserve != iSettings.iRecordingTimeReserve) &&
    	(iReceivedSettings.iReminderTime != iSettings.iReminderTime) &&
    	(iReceivedSettings.iLastUsedServiceId != iSettings.iLastUsedServiceId) &&
    	(iReceivedSettings.iLastUsedChannelId != iSettings.iLastUsedChannelId)
    	)
    {
		VCXLOGLO1("CIptvSettingsDatabaseTest::TestLastUsageL: settings do not match!");
		VCXLOGLO1("<<<CIptvSettingsDatabaseTest::TestLastUsageL");
		return KErrArgument;
    }

    VCXLOGLO1("<<<CIptvSettingsDatabaseTest::TestLastUsageL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvSettingsDatabaseTest::PrepareFile
//
// -----------------------------------------------------------------------------
//
TInt CIptvSettingsDatabaseTest::PrepareFile( const TDesC &aFileName )
	{
	RFs iFs;
	iFs.Connect();

	VCXLOGLO2("CIptvSettingsDatabaseTest::PrepareFile: File exists: %S", &aFileName);
	TRAPD( error, iFs.Delete( aFileName ) );

	iFs.Close();

	return error;
	}

// -----------------------------------------------------------------------------
// CIptvSettingsDatabaseTest::CheckPath
//
// -----------------------------------------------------------------------------
//
TInt CIptvSettingsDatabaseTest::CheckPath( const TDesC &aPath )
	{
	RFs iFs;
	iFs.Connect();

	TRAPD( error, BaflUtils::EnsurePathExistsL( iFs, aPath ) );

	iFs.Close();

	return error;
	}


// -----------------------------------------------------------------------------
// CIptvSettingsDatabaseTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CIptvSettingsDatabaseTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
