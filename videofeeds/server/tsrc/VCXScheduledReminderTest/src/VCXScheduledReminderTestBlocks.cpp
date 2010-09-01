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

#include "VCXScheduledReminderTest.h"

#include <f32file.h>
#include "VCXTestLog.h"
#include "CIptvUtil.h"
#include <s32mem.h>
#include "CIptvTestUtilities.h"

#include <ipvideo/CCseSchedulerAPI.h>
#include <ipvideo/CCseScheduledProgram.h>
//#include <CseSchedulerPluginIF.hrh>
#include "iptvvodscheduleddownloaddefinitions.h"
#include "IptvEngineUids.h"

#include "CIptvTestTimer.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
const TInt KTestPanic = -69;
const TInt KTmpBufLen = 256;
const TInt32 KInvalidPluginUid = -1;

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
void PrintTime(TTime time)
    {
    TDateTime date(time.DateTime());
    _LIT( KTimeFormat, "Time = %d.%d.%d %d:%d" );
    TBuf<80>    buffer;
    buffer.Format( KTimeFormat, date.Day() + 1, date.Month() + 1, date.Year(), date.Hour(), date.Minute() );
	VCXLOGLO2( "%S", &buffer );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CVCXScheduledReminderTest::Delete()
    {
    VCXLOGLO1(">>>CVCXScheduledReminderTest::Delete");
    if(iSchedulerClient)
        {
        delete iSchedulerClient;
        iSchedulerClient = NULL;
        }

	if(iTestUtil!=NULL)
		{
		delete iTestUtil;
		iTestUtil=NULL;
		}

	if(iShutdownTimer!=NULL)
		{
		iShutdownTimer->CancelTimer();
		delete iShutdownTimer;
		iShutdownTimer = NULL;
		}

	iScheduleArray.ResetAndDestroy();
	iScheduleArray.Close();
	VCXLOGLO1("<<<CVCXScheduledReminderTest::Delete");
    }

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Create", CVCXScheduledReminderTest::CreateL ),
        ENTRY( "RefreshService", CVCXScheduledReminderTest::RefreshServiceL ),
        ENTRY( "SetIap", CVCXScheduledReminderTest::SetIapL ),
        ENTRY( "ScheduleDownload", CVCXScheduledReminderTest::ScheduleDownloadL ),
        ENTRY( "AddSchedule", CVCXScheduledReminderTest::AddScheduleL ),
        ENTRY( "RemoveSchedule", CVCXScheduledReminderTest::RemoveScheduleL ),
        ENTRY( "RemoveAllSchedules", CVCXScheduledReminderTest::RemoveAllSchedulesL ),
        ENTRY( "RemoveAllFromArray", CVCXScheduledReminderTest::RemoveAllFromArrayL ),
        ENTRY( "GetSchedulesByAppUid", CVCXScheduledReminderTest::GetSchedulesByAppUidL ),
        ENTRY( "GetOverlappingSchedules", CVCXScheduledReminderTest::GetOverlappingSchedulesL ),
        ENTRY( "GetSchedulesByPluginUid", CVCXScheduledReminderTest::GetSchedulesByPluginUidL ),
        ENTRY( "GetSchedulesByType", CVCXScheduledReminderTest::GetSchedulesByTypeL ),
        ENTRY( "GetSchedulesByTime", CVCXScheduledReminderTest::GetSchedulesByTimeL ),
        ENTRY( "CheckCount", CVCXScheduledReminderTest::CheckCountL ),
        ENTRY( "EmptyArray", CVCXScheduledReminderTest::EmptyArray ),
        ENTRY( "AdjustClock", CVCXScheduledReminderTest::AdjustClockL ),
        ENTRY( "SetTimeout", CVCXScheduledReminderTest::SetTimeoutL ),
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CIptvVodDlApiTest::SetTimeoutL
// Set the timeout timer for the testcase. Timeout is given as 1/1000 seconds.
// Timeout's max value is 2147482 ms = 37 minutes.
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::SetTimeoutL( CStifItemParser& aItem )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::SetTimeoutL");

	iShutdownTimer = CIptvTestTimer::NewL(*this, 0);

	TInt timeout(0);

	if(iShutdownTimer != NULL)
		{
		if(KErrNone!=aItem.GetNextInt(timeout))
			{
			VCXLOGLO1("Timeout value  missing");
			VCXLOGLO1("<<<CVCXScheduledReminderTest::SetTimeoutL");

			delete iShutdownTimer;
			iShutdownTimer = NULL;
			return KErrArgument;
			}
		}
	else
		{
		VCXLOGLO1("Couldn't create the timer");
		VCXLOGLO1("<<<CVCXScheduledReminderTest::SetTimeoutL");

		return KErrGeneral;
		}

	TTimeIntervalMicroSeconds32 interval((timeout * 1000));

	iShutdownTimer->After(interval);

	VCXLOGLO1("<<<CVCXScheduledReminderTest::SetTimeoutL");

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::TimerComplete
//
// -----------------------------------------------------------------------------
//
void CVCXScheduledReminderTest::TimerComplete( TInt /* aTimerId */, TInt /* aError */ )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::TimerComplete");

	VCXLOGLO1("XXXXXXXX TESTCASE TIMEOUT XXXXXXXXX");

	Signal( KErrTimedOut );
	//User::Leave( KErrTimedOut );

	VCXLOGLO1("<<<CVCXScheduledReminderTest::TimerComplete");
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::CreateL
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::CreateL( CStifItemParser& /*aItem*/ )
    {
    VCXLOGLO1(">>>CVCXScheduledReminderTest::CreateL");
    // Print to UI
    _LIT( KVCXScheduledReminderTest, "VCXScheduledReminderTest" );
    _LIT( KCreate, "In Create" );
    TestModuleIf().Printf( 0, KVCXScheduledReminderTest, KCreate );
    // Print to log file
    iLog->Log( KCreate );

    iSchedulerClient = CCseSchedulerApi::NewL();

	iTestUtil = CIptvTestUtilities::NewL();

    VCXLOGLO1("<<<CVCXScheduledReminderTest::CreateL");

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::RefreshServiceL
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::RefreshServiceL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledReminderTest::RefreshServiceL");
    // Print to UI
    _LIT( KVCXScheduledReminderTest, "VCXScheduledReminderTest" );
    _LIT( KRefreshService, "In RefreshService" );
    TestModuleIf().Printf( 0, KVCXScheduledReminderTest, KRefreshService );
    // Print to log file
    iLog->Log( KRefreshService );

    TUint serviceId = GetServiceL(aItem);

	aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

	TTime refreshDate(0);

	TPtrC dateString;
	if(aItem.GetNextString(dateString) != KErrNone) {
		VCXLOGLO1("CVCXScheduledReminderTest::RefreshServiceL -- No date given as parameter, using Universal Time.");
		refreshDate.UniversalTime();
	}
	else {
		VCXLOGLO2("CVCXScheduledReminderTest::RefreshServiceL -- Using date string %S", &dateString);
		TInt err;
		err = refreshDate.Set( dateString );
		if( err != KErrNone ) {
			VCXLOGLO2("CVCXScheduledReminderTest::RefreshServiceL -- refreshDate.Set() returned error code %d", err);
			return KErrArgument;
		}
	}

    VCXLOGLO2("CSchedulingTest::RefreshService: sid = %d", serviceId);

	TInt ret = iTestUtil->RefreshService(serviceId, refreshDate);

    VCXLOGLO2("CSchedulingTest::RefreshService returned %d", ret);

    VCXLOGLO1("<<<CVCXScheduledReminderTest::RefreshServiceL");

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::SetIapL ( service (name or id), iap (name or id) )
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::SetIapL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledReminderTest::SetIapL");
    // Print to UI
    _LIT( KVCXScheduledReminderTest, "VCXScheduledReminderTest" );
    _LIT( KSetIap, "In SetIap" );
    TestModuleIf().Printf( 0, KVCXScheduledReminderTest, KSetIap );
    // Print to log file
    iLog->Log( KSetIap );

    TUint sid = GetServiceL(aItem);
    TUint iap = GetIapL(aItem);

    VCXLOGLO3("CSchedulingTest::SetIap: sid = %d, iap = %d",sid, iap);

    TInt  ret = iTestUtil->SetUsedIap(sid, iap);

    VCXLOGLO2("CSchedulingTest:  SetUsedIap returned %d", ret);

    VCXLOGLO1("<<<CVCXScheduledReminderTest::SetIapL");

    return ret;
    }


// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::ScheduleDownloadL(start, end, service )
//
// start and end times are given as seconds relative to this moment.
// If values are negative, times are in the past.
// Service can be name or id
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::ScheduleDownloadL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledReminderTest::ScheduleDownloadL");
    // Print to UI
    _LIT( KVCXScheduledReminderTest, "VCXScheduledReminderTest" );
    _LIT( KScheduleDownload, "In ScheduleDownload" );
    TestModuleIf().Printf( 0, KVCXScheduledReminderTest, KScheduleDownload );
    // Print to log file
    iLog->Log( KScheduleDownload );

    CCseScheduledProgram* program = CCseScheduledProgram::NewL();


    TInt start(0);
    TInt end(0);
    aItem.GetNextInt(start);
    aItem.GetNextInt(end);
    TTime now;
    now.UniversalTime();
    TTime startTime = now + TTimeIntervalSeconds(start);
    TTime endTime   = now + TTimeIntervalSeconds(end);
    program->SetStartTime(TTime(startTime));
    program->SetEndTime(TTime(endTime));


    VCXLOGLO1("Now / Universal time:");
    PrintTime(now);
    TTime local;
    local.HomeTime();
    VCXLOGLO1("Now / Local time:");
    PrintTime(local);
    VCXLOGLO1("Scheduled starttime:");
    PrintTime(startTime);
    VCXLOGLO1("Scheduled endtime:");
    PrintTime(endTime);


    TUint id = GetServiceL(aItem);
    HBufC8* dataBuf = HBufC8::NewLC(sizeof(TIptvServiceId));
    TPtr8 dataPtr(dataBuf->Des());
    RDesWriteStream writeStream;
    CleanupClosePushL(writeStream);
    writeStream.Open(dataPtr);
    writeStream.WriteUint32L(id);
    CleanupStack::PopAndDestroy(&writeStream);
    program->SetApplicationDataL(dataPtr);
    CleanupStack::PopAndDestroy(dataBuf);


    TPtrC name(_L("<empty>"));
    //aItem.GetNextString(name);
    TBuf8<KTmpBufLen> nameBuf;
    nameBuf.Copy(name);
    program->SetName(nameBuf);


    TInt dbId(id);
    //aItem.GetNextInt(dbId);
    program->SetDbIdentifier(dbId);


    TInt appUid( IPTV_SERVER_UID );
    //aItem.GetNextInt(appUid);
    program->SetAppUid(appUid);


    TInt pluginUid( KIptvScheduledDownloadPluginImplementationUid );
    //aItem.GetNextInt(pluginUid);
    program->SetPluginUid(pluginUid);


    TInt type( CCseScheduledProgram::ECseScheduleDownload );
    //aItem.GetNextInt(type);
    program->SetScheduleType(type);

    //TUint32 pluginType = CCseScheduledProgram::ECseUniPlugin;
    //program->SetPluginType(pluginType);

    iSchedulerClient->AddSchedule(*program);


    VCXLOGLO1("<<<CVCXScheduledReminderTest::ScheduleDownloadL");

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::AddScheduleL( CStifItemParser& aItem )
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::AddScheduleL( CStifItemParser& aItem )
	{
    VCXLOGLO1(">>>VCXScheduledReminderTest::AddScheduleL");

    // Print to UI
    _LIT( KModule, "VCXScheduledReminderTest" );
    _LIT( KWhere, "In ScheduleDownload" );
    TestModuleIf().Printf( 0, KModule, KWhere );

	// Construct the program
    CCseScheduledProgram* program = CCseScheduledProgram::NewL();
    CleanupStack::PushL( program );

    TPtrC name(_L("<empty>"));
    TBuf8<KTmpBufLen> nameBuf;
    nameBuf.Copy(name);
    program->SetName(nameBuf);
    VCXLOGLO2("VCXScheduledReminderTest: ServiceName: %S", &nameBuf);

    // Set type
    TInt schedType( 0 );
    if( aItem.GetNextInt( schedType ) != KErrNone )
    	{
    	VCXLOGLO1("Unable to read NextInt");
    	VCXLOGLO1("<<<VCXScheduledReminderTest::AddScheduleL");
    	CleanupStack::PopAndDestroy();	//CCseScheduledProgram
    	return KErrArgument;
    	}
    program->SetScheduleType( schedType );
	VCXLOGLO2("VCXScheduledReminderTest: ScheduleType: %d", schedType);

   	// Set start & end
    TInt start(0);
    TInt end(0);
    if( aItem.GetNextInt(start) != KErrNone )
    	{
    	VCXLOGLO1("Unable to read NextInt to start");
    	VCXLOGLO1("<<<VCXScheduledReminderTest::AddScheduleL");
    	CleanupStack::PopAndDestroy();	//CCseScheduledProgram
    	return KErrArgument;
    	}
	if( aItem.GetNextInt(end) != KErrNone )
		{
    	VCXLOGLO1("Unable to read NextInt to end");
    	VCXLOGLO1("<<<VCXScheduledReminderTest::AddScheduleL");
    	CleanupStack::PopAndDestroy();	//CCseScheduledProgram
    	return KErrArgument;
		}
    TTime now;
    now.UniversalTime();
    TTime startTime = now + TTimeIntervalSeconds(start);
    TTime endTime   = now + TTimeIntervalSeconds(end);
    VCXLOGLO2("VCXScheduledReminderTest: Start: %d", start);
    VCXLOGLO2("VCXScheduledReminderTest: End: %d", end);
    program->SetStartTime(TTime(startTime));
    program->SetEndTime(TTime(endTime));

	// Print times
    VCXLOGLO1("Now / Universal time:");
    PrintTime(now);
    TTime local;
    local.HomeTime();
    VCXLOGLO1("Now / Local time:");
    PrintTime(local);
    VCXLOGLO1("Scheduled starttime:");
    PrintTime(TTime(startTime));
    VCXLOGLO1("Scheduled endtime:");
    PrintTime(TTime(endTime));

    // Set plugin UID to use
    TInt pluginId( 0 );
    if ( aItem.GetNextInt( pluginId ) != KErrNone )
    	{
    	VCXLOGLO1("Unable to read NextInt");
    	VCXLOGLO1("<<<VCXScheduledReminderTest::AddScheduleL");
    	CleanupStack::PopAndDestroy();	//CCseScheduledProgram
    	return KErrArgument;
    	}

    program->SetPluginUid( GetPluginUidByNumber(pluginId) );

	// Set plugin type
    TInt pluginType( 0 );
    if ( aItem.GetNextInt( pluginType ) != KErrNone )
    	{
    	VCXLOGLO1("Unable to read NextInt");
    	VCXLOGLO1("<<<VCXScheduledReminderTest::AddScheduleL");
    	CleanupStack::PopAndDestroy();	//CCseScheduledProgram
    	return KErrArgument;
    	}
    if( pluginType == 0 )
    	{
   	    program->SetPluginType( CCseScheduledProgram::ECseUniPlugin );
	   	}
	else
		{
		program->SetPluginType( CCseScheduledProgram::ECseMultiPlugin );
		}

    // Set app uid
    // This is used as a default, does it matter?
    program->SetAppUid( IPTV_SERVER_UID );

    // Set application data
    TUint id = GetServiceL(aItem);
    VCXLOGLO2("VCXScheduledReminderTest: Using Service id: %d", id);
    HBufC8* dataBuf = HBufC8::NewLC(sizeof(TIptvServiceId));
    TPtr8 dataPtr(dataBuf->Des());
    RDesWriteStream writeStream;
    CleanupClosePushL(writeStream);
    writeStream.Open(dataPtr);
    writeStream.WriteUint32L(id);
    CleanupStack::PopAndDestroy(&writeStream);
    program->SetApplicationDataL(dataPtr);
    CleanupStack::PopAndDestroy(dataBuf);

	VCXLOGLO1("VCXScheduledReminderTest::AddScheduleL: Adding following item:");
	program->DebugDump();

    // Add schedule to engine
    TInt error( 0 );
	error = iSchedulerClient->AddSchedule(*program);
	VCXLOGLO2("VCXScheduledReminderTest: AddSchedule returned %d", error);

    CleanupStack::PopAndDestroy( program );

   	VCXLOGLO1("<<<VCXScheduledReminderTest::AddScheduleL");

    return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::RemoveScheduleL( CStifItemParser& aItem )
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::RemoveScheduleL( CStifItemParser& aItem )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::RemoveScheduleL");

    // Print to UI
    _LIT( KModule, "VCXScheduledReminderTest" );
    _LIT( KWhere, "In RemoveSchedule" );
    TestModuleIf().Printf( 0, KModule, KWhere );

	TInt index( 0 );

	// Attempt to read from aItem
	if( aItem.GetNextInt( index ) != KErrNone )
		{
		VCXLOGLO1("CVCXScheduledReminderTest::RemoveScheduleL: Unable to read parameter");
    	VCXLOGLO1("<<<CVCXScheduledReminderTest::RemoveScheduleL");
    	return KErrArgument;
		}

	// Attempt to remove schedule by given index
	TInt error( KErrNone );
	error = iSchedulerClient->RemoveSchedule( index );
	VCXLOGLO2("VCXScheduledReminderTest: RemoveSchedule returned %d", error);

	VCXLOGLO1("<<<CVCXScheduledReminderTest::RemoveScheduleL");
	return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::RemoveAllSchedulesL( CStifItemParser& aItem )
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::RemoveAllSchedulesL( CStifItemParser& /* aItem */ )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::RemoveAllSchedulesL");

    // Print to UI
    _LIT( KModule, "VCXScheduledReminderTest" );
    _LIT( KWhere, "In RemoveAllSchedules" );
    TestModuleIf().Printf( 0, KModule, KWhere );

	RPointerArray<CCseScheduledProgram> tempArray;
	iSchedulerClient->GetSchedulesByAppUid( IPTV_SERVER_UID, tempArray );

	TInt count( 0 );
	TInt error( 0 );
	for( count = 0; count < tempArray.Count(); count++ )
		{
		error = iSchedulerClient->RemoveSchedule( tempArray[count]->DbIdentifier() );
		VCXLOGLO2("CVCXScheduledReminderTest: RemoveSchedule returned %d", error);
		}

	tempArray.ResetAndDestroy();
	tempArray.Close();

	VCXLOGLO1("<<<CVCXScheduledReminderTest::RemoveAllSchedulesL");
	return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::RemoveAllFromArrayL( CStifItemParser& aItem )
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::RemoveAllFromArrayL( CStifItemParser& aItem )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::RemoveAllFromArrayL");

    // Print to UI
    _LIT( KModule, "VCXScheduledReminderTest" );
    _LIT( KWhere, "In RemoveAllFromArrayL" );
    TestModuleIf().Printf( 0, KModule, KWhere );

	TInt count( 0 );
	TInt error( 0 );
	for( count = 0; count < iScheduleArray.Count(); count++ )
		{
		error = iSchedulerClient->RemoveSchedule( iScheduleArray[count]->DbIdentifier() );
		VCXLOGLO2("CVCXScheduledReminderTest: RemoveSchedule returned %d", error);
		}

	// Clean up the iScheduleArray
	EmptyArray( aItem );

	VCXLOGLO1("<<<CVCXScheduledReminderTest::RemoveAllFromArrayL");
	return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::GetScheduleL()
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::GetScheduleL( CStifItemParser& /* aItem */ )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::GetScheduleL");

    // Print to UI
    _LIT( KModule, "VCXScheduledReminderTest" );
    _LIT( KWhere, "In GetScheduleL" );
    TestModuleIf().Printf( 0, KModule, KWhere );

    CCseScheduledProgram* iProgram = CCseScheduledProgram::NewL();

    TInt error( 0 );
    error = iSchedulerClient->GetSchedule( NULL, iProgram );
    VCXLOGLO2("CVCXScheduledReminderTest::GetScheduleL: error: %d", error);

	// Print
	iProgram->DebugDump();

    VCXLOGLO1("<<<CVCXScheduledReminderTest::GetScheduleL");
    return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::GetSchedulesByAppUid()
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::GetSchedulesByAppUidL( CStifItemParser& /* aItem */ )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::GetSchedulesByAppUidL");

    // Print to UI
    _LIT( KModule, "VCXScheduledReminderTest" );
    _LIT( KWhere, "In GetSchedulesByAppUIDL" );
    TestModuleIf().Printf( 0, KModule, KWhere );

	// Fetch schedules from client
	TInt error( 0 );
	error = iSchedulerClient->GetSchedulesByAppUid( IPTV_SERVER_UID, iScheduleArray );
	VCXLOGLO2("CVCXScheduledReminderTest: GetSchedulesByAppUid returned errorcode %d", error);

	// Print schedules
	PrintArray( iScheduleArray );

	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetSchedulesByAppUidL");
	return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::GetOverlappingSchedulesL()
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::GetOverlappingSchedulesL( CStifItemParser& aItem )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::GetOverlappingSchedulesL");

    // Print to UI
    _LIT( KModule, "VCXScheduledReminderTest" );
    _LIT( KWhere, "In GetOverlappingSchedulesL" );
    TestModuleIf().Printf( 0, KModule, KWhere );

	CCseScheduledProgram* program = CCseScheduledProgram::NewL();
	CleanupStack::PushL( program );

	//Program object, add type and starttime/endtime, etc etc
	TInt type( 0 );
	if( aItem.GetNextInt( type ) != KErrNone )
		{
		VCXLOGLO1("Unable to read NextInt");
		VCXLOGLO1("<<<CVCXScheduledReminderTest::GetSchedulesL");
		CleanupStack::PopAndDestroy();	//CCseScheduledProgram
		return KErrArgument;
		}

   	// Start & end times
    TInt start(0);
    TInt end(0);
    if( aItem.GetNextInt( start ) != KErrNone )
    	{
    	VCXLOGLO1("Unable to read NextInt");
    	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetSchedulesL");
		CleanupStack::PopAndDestroy();	//CCseScheduledProgram
    	return KErrArgument;
    	}

	if( aItem.GetNextInt( end ) != KErrNone )
		{
    	VCXLOGLO1("Unable to read NextInt");
    	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetSchedulesL");
		CleanupStack::PopAndDestroy();	//CCseScheduledProgram
    	return KErrArgument;
		}

    TTime now;
    now.UniversalTime();
    TTime startTime = now - TTimeIntervalDays( start );
    TTime endTime   = now + TTimeIntervalDays( end );

    // Set the needed values
    program->SetScheduleType( type );
    program->SetStartTime( startTime );
    program->SetEndTime( endTime );

	// Fetch schedules from client
	TInt error( 0 );
	error = iSchedulerClient->GetOverlappingSchedules( *program, iScheduleArray );
	VCXLOGLO2("CVCXScheduledReminderTest: GetOverlappingSchedules returned errorcode %d", error);

	CleanupStack::PopAndDestroy();	//CCseScheduledProgram

	// Print schedules
	PrintArray( iScheduleArray );

	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetOverlappingSchedulesL");
	return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::GetSchedulesByPluginUIDL()
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::GetSchedulesByPluginUidL( CStifItemParser& aItem )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::GetSchedulesByPluginUidL");

    // Print to UI
    _LIT( KModule, "VCXScheduledReminderTest" );
    _LIT( KWhere, "In GetSchedulesByPluginUidL" );
    TestModuleIf().Printf( 0, KModule, KWhere );

	// Set plugin UID to use
	TInt pluginId( 0 );
	if ( aItem.GetNextInt( pluginId ) != KErrNone )
		{
		VCXLOGLO1("Unable to read NextInt");
		VCXLOGLO1("<<<VCXScheduledReminderTest::GetSchedulesByPluginUidL");
		return KErrArgument;
		}

	TInt32 pluginUid( 0 );
	pluginUid = GetPluginUidByNumber(pluginId);

	// Fetch schedules from client
	TInt error( 0 );
	error = iSchedulerClient->GetSchedulesByPluginUid( pluginUid, iScheduleArray );
	VCXLOGLO2("CVCXScheduledReminderTest: GetSchedulesByPluginUidL returned errorcode %d", error);

	// Print schedules
	PrintArray( iScheduleArray );

	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetSchedulesByPluginUidL");
	return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::GetSchedulesByTypeL()
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::GetSchedulesByTypeL( CStifItemParser& aItem )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::GetSchedulesByTypeL");

    // Print to UI
    _LIT( KModule, "VCXScheduledReminderTest" );
    _LIT( KWhere, "In GetSchedulesByTypeL" );
    TestModuleIf().Printf( 0, KModule, KWhere );

	TInt type( 0 );
    if( aItem.GetNextInt( type ) != KErrNone )
    	{
    	VCXLOGLO1("Unable to read NextInt");
    	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetSchedulesL");
    	return KErrArgument;
    	}

	// Fetch schedules from client
	TInt error( 0 );
	error = iSchedulerClient->GetSchedulesByType( type, iScheduleArray );
	VCXLOGLO2("<<<CVCXScheduledReminderTest: GetSchedulesByType returned errorcode %d", error);

	// Print schedules
	PrintArray( iScheduleArray );

	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetSchedulesByTypeL");
	return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::GetSchedulesByTime()
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::GetSchedulesByTimeL( CStifItemParser& aItem )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::GetSchedulesByTimeL");

    // Print to UI
    _LIT( KModule, "VCXScheduledReminderTest" );
    _LIT( KWhere, "In GetSchedulesByTimeL" );
    TestModuleIf().Printf( 0, KModule, KWhere );

   	// Start & end times
    TInt start( 0 );
    TInt end( 0 );
    if( aItem.GetNextInt( start ) != KErrNone )
    	{
    	VCXLOGLO1("Unable to read NextInt");
    	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetSchedulesL");
    	return KErrArgument;
    	}
	if( aItem.GetNextInt( end ) != KErrNone )
		{
    	VCXLOGLO1("Unable to read NextInt");
    	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetSchedulesL");
    	return KErrArgument;
		}
    TTime now;
    now.UniversalTime();
    TTime startTime = now - TTimeIntervalDays( start );
    TTime endTime   = now + TTimeIntervalDays( end );

	// Fetch schedules from client
	TInt error( 0 );
	error = iSchedulerClient->GetSchedulesByTime( startTime, endTime, iScheduleArray );
	VCXLOGLO2("VCXScheduledReminderTest: GetSchedulesByTime returned errorcode %d", error);

	// Print schedules
	PrintArray( iScheduleArray );

	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetSchedulesByTimeL");
	return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::CheckCountL()
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::CheckCountL( CStifItemParser& aItem )
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::CheckCountL");
	TInt expectedCount( 0 );
	TInt error( KErrNone );

	if( aItem.GetNextInt( expectedCount ) != KErrNone )
		{
		VCXLOGLO1("Unable to read NextInt");
		VCXLOGLO1("<<<CVCXScheduledReminderTest::CheckCountL");
		return KErrArgument;
		}

	VCXLOGLO2("CVCXScheduledReminderTest::CheckCountL: Expecting %d items", expectedCount);
	VCXLOGLO2("CVCXScheduledReminderTest::CheckCountL: iScheduleArray.Count() = %d", iScheduleArray.Count());

	if( iScheduleArray.Count() != expectedCount )
		{
		VCXLOGLO1("CVCXScheduledReminderTest::CheckCountL: The amount of scheduled items doesn't match the expected value!");
		error = KErrGeneral;
		}
	VCXLOGLO1("<<<CVCXScheduledReminderTest::CheckCountL");
	return error;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::CheckCountL()
//
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::EmptyArray( CStifItemParser& /* aItem */ )
	{
	iScheduleArray.Reset();
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::CompareScheduleLists()
// Compare schedule array to the class member array
// -----------------------------------------------------------------------------
//
TBool CVCXScheduledReminderTest::CompareScheduleLists( RPointerArray<CCseScheduledProgram> aList )
	{
	// if the amount of items doesn't match, then lists obviously don't match
	if( aList.Count() != iScheduleArray.Count() )
		{
		return EFalse;
		}

	// compare every item
	TInt count(0);
	for( count = 0; count < iScheduleArray.Count(); count++ )
		{
		if( !CompareScheduleItems( aList[count], iScheduleArray[count] ) )
			{
			return EFalse;
			}
		}

	// finally, if previous tests didn't return false, the lists obviously match!
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::CompareScheduleItems()
// Compares given programs
// -----------------------------------------------------------------------------
//
TBool CVCXScheduledReminderTest::CompareScheduleItems( CCseScheduledProgram* aFirst, CCseScheduledProgram* aSecond )
	{
	// Now this is easy and stupid. Is there any convenient way?

	if( aFirst->Name() != aSecond->Name() ) return EFalse;
	if( aFirst->StartTime() != aSecond->StartTime() ) return EFalse;
	if( aFirst->EndTime() != aSecond->EndTime() ) return EFalse;
	if( aFirst->AppUid() != aSecond->AppUid() ) return EFalse;
	if( aFirst->PluginUid() != aSecond->PluginUid() ) return EFalse;
	if( aFirst->ApplicationData() != aSecond->ApplicationData() ) return EFalse;
	if( aFirst->DbIdentifier() != aSecond->DbIdentifier() ) return EFalse;
	if( aFirst->ScheduleType() != aSecond->ScheduleType() ) return EFalse;

	// If all else "fails"
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::PrintArray()
//
// -----------------------------------------------------------------------------
//
void CVCXScheduledReminderTest::PrintArray( RPointerArray<CCseScheduledProgram> aList )
	{
	TInt count( 0 );
	for( count = 0; count < aList.Count(); count++ )
		{
		aList[count]->DebugDump();
		}
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::GetPluginUidByNumber( TInt aValue )
// Returns plugin uid based on number
// -----------------------------------------------------------------------------
//
TInt32 CVCXScheduledReminderTest::GetPluginUidByNumber( TInt aValue )
	{
	TInt32 pluginUID( 0 );

    switch( aValue )
    	{
		case 0:
			{
			VCXLOGLO1("VCXScheduledReminderTest: Plugin id: KSceSchedulerPluginEcomIfUid");
			//pluginUID = KSceSchedulerPluginEcomIfUid;
			break;
			}
		case 1:
			{
			VCXLOGLO1("VCXScheduledReminderTest: Plugin id: KIptvSchedulerPluginIpmlementationUid");
			//pluginUID = KIptvSchedulerPluginIpmlementationUid;
			break;
			}
		case 2:
			{
			VCXLOGLO1("VCXScheduledReminderTest: Plugin id: KIptvSchedulerPluginUid");
			//pluginUID = KIptvSchedulerPluginUid;
			break;
			}
		case 3:
			{
			VCXLOGLO1("VCXScheduledReminderTest: Plugin id: INVALID PLUGIN UID");
			pluginUID = KInvalidPluginUid;
			break;
			}
    	default:
    		{
    		VCXLOGLO1("VCXScheduledReminderTest: Plugin id: KIptvScheduledDownloadPluginImplementationUid");
			pluginUID = KIptvScheduledDownloadPluginImplementationUid;
			break;
    		}
    	}
	return pluginUID;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::AdjustClockL ( timeintervalinseconds )
// adds to given interval to current system time. interval may also be negative.
// -----------------------------------------------------------------------------
//
TInt CVCXScheduledReminderTest::AdjustClockL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CVCXScheduledReminderTest::AdjustClockL");
    // Print to UI
    _LIT( KVCXScheduledReminderTest, "VCXScheduledReminderTest" );
    _LIT( KAdjustClock, "In AdjustClock" );
    TestModuleIf().Printf( 0, KVCXScheduledReminderTest, KAdjustClock );
    // Print to log file
    iLog->Log( KAdjustClock );

    TInt tmp;
    T1L( aItem.GetNextInt(tmp), KErrNone );
    TTimeIntervalSeconds ts(tmp);

    TTime oldTime;
    oldTime.UniversalTime();

    User::SetUTCTime(TTime(oldTime + ts));

    VCXLOGLO2("CSchedulingTest:  AdjustClock ts: %d", ts.Int());

    VCXLOGLO1("<<<CVCXScheduledReminderTest::AdjustClockL");

    return KErrNone;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::GetServiceL
// If aItem = string, returns given service's id, otherwise returns given number.
// -----------------------------------------------------------------------------
//
TUint32 CVCXScheduledReminderTest::GetServiceL(CStifItemParser& aItem)
	{
	VCXLOGLO1(">>>CVCXScheduledReminderTest::GetServiceL");

    TPtrC   serviceName;
    TUint   serviceId(0);

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    if( KErrNone == aItem.GetNextString(serviceName) && iTestUtil!=NULL)
    	{
    	VCXLOGLO2("CVCXScheduledReminderTest::  Service name = %S",&serviceName);
    	TInt sidTemp(0);

    	if( iTestUtil->GetServiceIdByNameL(serviceName, sidTemp) == KErrNone )
    		{
    		serviceId = sidTemp;
    		}
    	else
    		{
			TLex lex(serviceName);
			TInt64 tmp2;
			if(lex.Val(tmp2)==KErrNone)
				{
				serviceId = tmp2;
	    		VCXLOGLO2("Service name not found, using parsed int %d.",serviceId);
				}
		   	else
		    	{
		    	VCXLOGLO1("Service not found");
		   		_LIT(KTestErr, "Invalid service");
		   		User::Panic(KTestErr, KTestPanic);
		    	}
    		}
		}
   	else
    	{
    	VCXLOGLO1("Missing parameter: service");
		_LIT(KTestErr, "Service missing");
		User::Panic(KTestErr,KTestPanic);
		}

	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetServiceL");
	return serviceId;
	}

// -----------------------------------------------------------------------------
// CVCXScheduledReminderTest::GetIapL
// If aItem = string, returns given iap's id, otherwise returns given number.
// -----------------------------------------------------------------------------
//
TUint32 CVCXScheduledReminderTest::GetIapL( CStifItemParser& aItem ) {
	VCXLOGLO1(">>>CVCXScheduledReminderTest::GetIapL");

	TPtrC   iapName;
	TUint32 iapId(0);

	aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

	/*
	TInt temp;
	if( aItem.GetNextInt( temp ) == KErrNone ) {
		iapId = (TUint32)temp;
		VCXLOGLO2("Using int %d as Iap ID.",iapId);
	}
	else {
		aItem.GetNextString(iapName);
		if( !iTestUtil->GetIapIdL(iapName, iapId) ) {
			VCXLOGLO1("Iap not found");
			_LIT(KTestErr, "Invalid iap");
			User::Panic(KTestErr,KTestPanic);
		}
	}*/


	if( KErrNone == aItem.GetNextString(iapName) && iTestUtil!=NULL) {
		VCXLOGLO2("CVCXScheduledReminderTest::  Iap name     = %S",&iapName);
		if( !iTestUtil->GetIapIdL(iapName, iapId) ) {
			TLex lex(iapName);
			TInt64 tmp1;
			if(lex.Val(tmp1)==KErrNone) {
				iapId = tmp1;
				VCXLOGLO2("Iap name not found, using parsed int %d.",iapId);
			}
			else {
				VCXLOGLO1("Iap not found");
				_LIT(KTestErr, "Invalid iap");
				User::Panic(KTestErr,KTestPanic);
			}
		}
	}
	else {
		VCXLOGLO1("Error finding Iap !");
		_LIT(KTestErr, "iap missing");
		User::Panic(KTestErr,KTestPanic);
	}

	VCXLOGLO1("<<<CVCXScheduledReminderTest::GetIapL");
	return iapId;
}

//  End of File
