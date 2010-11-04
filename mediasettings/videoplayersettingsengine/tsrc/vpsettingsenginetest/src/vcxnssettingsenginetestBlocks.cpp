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




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include "vcxnssettingsenginetest.h"
#include "vcxtestlog.h"
#include "videosettingsdrivemonitor.h"
#include "IptvTestUtilALR.h"

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
// CVcxNsSettingsEngineTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsEngineTest::Delete() 
    {
    if( iSettingsEngine )
           {
           delete iSettingsEngine;
           iSettingsEngine = NULL;
           }
    
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Create", CVcxNsSettingsEngineTest::CreateL ),
        ENTRY( "GetUsedMemory", CVcxNsSettingsEngineTest::GetUsedMemoryL ),
        ENTRY( "SetUsedMemory", CVcxNsSettingsEngineTest::SetUsedMemoryL ),
        ENTRY( "GetParentControl", CVcxNsSettingsEngineTest::GetParentControl ),
        ENTRY( "SetParentControl", CVcxNsSettingsEngineTest::SetParentControl ),
        ENTRY( "GetVideoOnDemandThumbnails", CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails ),
        ENTRY( "SetVideoOnDemandThumbnails", CVcxNsSettingsEngineTest::SetVideoOnDemandThumbnails ),
        ENTRY( "GetUsedSnapId", CVcxNsSettingsEngineTest::GetUsedSnapId ),
        ENTRY( "SetUsedSnapId", CVcxNsSettingsEngineTest::SetUsedSnapId ),
        ENTRY( "GetAllowUseGprs", CVcxNsSettingsEngineTest::GetAllowUseGprs ),
        ENTRY( "SetAllowUseGprs", CVcxNsSettingsEngineTest::SetAllowUseGprs ),
        ENTRY( "GetAllowRoaming", CVcxNsSettingsEngineTest::GetAllowRoaming ),
        ENTRY( "SetAllowRoaming", CVcxNsSettingsEngineTest::SetAllowRoaming ),
        ENTRY( "CheckLockCodeL", CVcxNsSettingsEngineTest::CheckLockCodeL ),
        ENTRY( "GetPlayerVolume", CVcxNsSettingsEngineTest::GetPlayerVolume ),  
        ENTRY( "SetPlayerVolume", CVcxNsSettingsEngineTest::SetPlayerVolume )  
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::GetUsedMemoryL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::CreateL(
   CStifItemParser& /*aItem*/ )
   {
   // Print to UI
   _LIT( KVcxVideoSettingsEngineTest, "VcxVideoSettingsEngineTest" );
   _LIT( KCurrentFunction, "In CreateL" );
   TestModuleIf().Printf( 0, KVcxVideoSettingsEngineTest, KCurrentFunction );
   // Print to log file
   iLog->Log( KCurrentFunction );
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::CreateL");

   if( !iSettingsEngine )
       {
       iSettingsEngine = CVcxNsSettingsEngine::NewL();
       }
   
   iIadUpdateTime = 0;
   
   User::LeaveIfError( iFs.Connect() );
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::CreateL");
   return KErrNone;
   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::LogMethod
// Generate log from current method
// -----------------------------------------------------------------------------
//
void CVcxNsSettingsEngineTest::LogMethod( TPtrC aMethod )
    {
    // Print to UI
    TBuf< 64 > buffer;
    buffer.Format( KLogLocation, &aMethod );
    TestModuleIf().Printf( 0, KTestModuleName, buffer );
    // Print to log file
    iLog->Log( buffer );
    }

// -----------------------------------------------------------------------------
// CVCXPlayerCoreApiTest::LogMethod
// Generate log from current method
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::FindDrive( TInt aType )
    {
    VCXLOGLO1(">>> CVcxNsSettingsEngineTest::FindDrive");
    
    CVideoSettingsDriveMonitor* driveMonitor = CVideoSettingsDriveMonitor::NewL( iFs );
    TInt driveNumber( -1 );
    
    switch( aType )
        {
        case EFixedDrive:
            driveNumber = driveMonitor->FixedDrive(EFalse);
            break;
        case EMassStorageDrive:
            driveNumber = driveMonitor->MassStorageDrive();
            break;
        case EPhoneMemoryDrive:
            driveNumber = driveMonitor->PhoneMemoryDrive();
            break;
        case EMemoryCardDrive:
            driveNumber = driveMonitor->MemoryCardDrive();
            break;
        default:
            //Set given int to drivenumber
            driveNumber = aType;
        }
    
    VCXLOGLO2("<<< CVcxNsSettingsEngineTest::FindDrive (%d)", driveNumber);
    return driveNumber;
    }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::GetUsedMemoryL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::GetUsedMemoryL(
   CStifItemParser& aItem )
   {
   LogMethod(_L("GetUsedMemoryL"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::GetUsedMemoryL");
   
   TInt expectedValue( -1 );
   if( aItem.GetNextInt( expectedValue ) != KErrNone )
       {
       expectedValue = iUsedMemory;
       }

   TInt usedMemory( 0 );
   TRAPD( error, iSettingsEngine->GetUsedMemoryL( usedMemory ) );

   if( error != KErrNone )
       {
       VCXLOGLO2("CVcxNsSettingsEngineTest::GetUsedMemoryL -- error after call: %d", error);
	   VCXLOGLO2("<<< CVcxNsSettingsEngineTest::GetUsedMemoryL (%d)", error);
       return error;
       }

   if( expectedValue != usedMemory )
       {
       VCXLOGLO3("CVcxNsSettingsEngineTest::GetUsedMemoryL -- Expected memory: %d Used memory: %d", expectedValue, usedMemory);
       error = KErrCorrupt;
       }
      
   VCXLOGLO2("<<< CVcxNsSettingsEngineTest::GetUsedMemoryL (%d)", error);
   return error;
   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::SetUsedMemory
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::SetUsedMemoryL(
   CStifItemParser& aItem )
   {
   LogMethod(_L("SetUsedMemoryL"));
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::SetUsedMemoryL");

   TInt error( 0 );

   TInt driveType( -1 );
   User::LeaveIfError( aItem.GetNextInt( driveType ) );

   TInt driveNumber( 0 );
   driveNumber = FindDrive( driveType );
  
   // Attempt to set the used memory
   TBool ret;
   ret = iSettingsEngine->SetUsedMemory( driveNumber );

   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::SetUsedMemory -- Returned false!");
       error = KErrGeneral;
       }
   else
       {
       iUsedMemory = driveNumber;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetUsedMemory");
   return error;
   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::GetParentControl
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::GetParentControl(
   CStifItemParser& aItem )
   {
   LogMethod(_L("GetParentControl"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::GetParentControl");

   TInt expectedParentControl( -1 );
   aItem.GetNextInt( expectedParentControl );
   
   TInt parentControl( 0 );
   TBool ret( EFalse );
   ret = iSettingsEngine->GetParentControl( parentControl );

   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::GetParentControl -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetParentControl");
       return KErrGeneral;
       }
   
   if( expectedParentControl != -1 && expectedParentControl != parentControl )
       {
       VCXLOGLO3("CVcxNsSettingsEngineTest::GetParentControl -- Expected value: %d Returned value: %d", expectedParentControl, parentControl);
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetParentControl");
       return KErrCorrupt;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetParentControl");
   return KErrNone;
   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::SetParentControl
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::SetParentControl(
   CStifItemParser& aItem )
   {
   LogMethod(_L("SetParentControl"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::SetParentControl");

   TInt parentControl( 0 );
   aItem.GetNextInt( parentControl );

   TBool ret;
   ret = iSettingsEngine->SetParentControl( parentControl );
   
   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::SetParentControl -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetParentControl");
       return KErrGeneral;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetParentControl");
   return KErrNone;
   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails(
   CStifItemParser& aItem )
   {
   LogMethod(_L("GetVideoOnDemandThumbnails"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails");
   
   TInt expectedValue( -1 );
   aItem.GetNextInt( expectedValue );
   
   TInt value( 0 );
   TBool ret;
   TRAPD( error, ret = iSettingsEngine->GetVideoOnDemandThumbnails( value ) );
   
   if( error )
       {
       VCXLOGLO2("CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails -- Returned %d!", error);
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails");
       return error;
       }
   
   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails");
       return KErrGeneral;
       }
   
   if( expectedValue != -1 && expectedValue != value )
       {
       VCXLOGLO3("CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails -- Expected value: %d Returned value: %d", expectedValue, value);
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails");
       return KErrCorrupt;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetVideoOnDemandThumbnails");
   return KErrNone;
   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::SetVideoOnDemandThumbnails
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::SetVideoOnDemandThumbnails(
   CStifItemParser& aItem )
   {
   LogMethod(_L("SetVideoOnDemandThumbnails"));

   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::SetVideoOnDemandThumbnails");

   TInt value( 0 );
   aItem.GetNextInt( value );

   TBool ret;
   TRAPD( error, ret = iSettingsEngine->SetVideoOnDemandThumbnails( value ) );
   
   if( error )
       {
       VCXLOGLO2("CVcxNsSettingsEngineTest::SetVideoOnDemandThumbnails -- Returned %d!", error);
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetVideoOnDemandThumbnails");
       return error;
       }
   
   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::SetVideoOnDemandThumbnails -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetVideoOnDemandThumbnails");
       return KErrGeneral;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetVideoOnDemandThumbnails");
   return KErrNone;
   }


// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::GetUsedSnapId
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::GetUsedSnapId(
   CStifItemParser& aItem )
   {
   LogMethod(_L("GetUsedSnapId"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::GetUsedSnapId");
   
   aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
   
   TPtrC snapName;
   aItem.GetNextString( snapName );
   
   TInt expectedValue( -1 );

   if( snapName.Length() > 0 )
       {
       CIptvTestUtilALR* alrUtil = CIptvTestUtilALR::NewLC();
       expectedValue = alrUtil->GetDestinationIdL( snapName );
       CleanupStack::PopAndDestroy( alrUtil );
       }
   
   TInt value( 0 );
   TBool ret;
   ret = iSettingsEngine->GetUsedSnapId( value );
   
   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::GetUsedSnapId -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetUsedSnapId");
       return KErrGeneral;
       }
   
   VCXLOGLO2("<<< CVcxNsSettingsEngineTest::GetUsedSnapId -- value %d", value);
   
   if( expectedValue != -1 && expectedValue != value )
       {
       VCXLOGLO3("CVcxNsSettingsEngineTest::GetUsedSnapId -- Expected value: %d Returned value: %d", expectedValue, value);
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetUsedSnapId");
       return KErrCorrupt;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetUsedSnapId");
   return KErrNone;

   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::SetUsedSnapId
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::SetUsedSnapId(
   CStifItemParser& aItem )
   {
   LogMethod(_L("SetUsedSnapId"));   

   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::SetUsedSnapId");

   aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
   
   TPtrC snapName;
   aItem.GetNextString( snapName );
   
   TInt usedSnap( -1 );

   if( snapName.Compare(_L("INVALID")) != 0 )
       {
       CIptvTestUtilALR* alrUtil = CIptvTestUtilALR::NewLC();
       usedSnap = alrUtil->GetDestinationIdL( snapName );
       CleanupStack::PopAndDestroy( alrUtil );
       }

   TBool ret;
   ret = iSettingsEngine->SetUsedSnapId( usedSnap );
   
   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::SetUsedSnapId -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetUsedSnapId");
       return KErrGeneral;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetUsedSnapId");
   return KErrNone;

   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::GetAllowUseGprs
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::GetAllowUseGprs(
   CStifItemParser& aItem )
   {
   LogMethod(_L("GetAllowUseGprs"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::GetAllowUseGprs");

   TInt expectedValue( -1 );
   aItem.GetNextInt( expectedValue );
   
   TInt value( 0 );
   TBool ret;
   ret = iSettingsEngine->GetAllowUseGprs( value );
   
   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::GetAllowUseGprs -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetAllowUseGprs");
       return KErrGeneral;
       }
   
   if( expectedValue != -1 && expectedValue != value )
       {
       VCXLOGLO3("CVcxNsSettingsEngineTest::GetAllowUseGprs -- Expected value: %d Returned value: %d", expectedValue, value);
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetAllowUseGprs");
       return KErrCorrupt;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetAllowUseGprs");

   return KErrNone;

   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::SetAllowUseGprs
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::SetAllowUseGprs(
   CStifItemParser& aItem )
   {
   LogMethod(_L("SetAllowUseGprs"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::SetAllowUseGprs");

   TInt value( 0 );
   aItem.GetNextInt( value );

   TBool ret;
   ret = iSettingsEngine->SetAllowUseGprs( value );
   
   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::SetAllowUseGprs -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetAllowUseGprs");
       return KErrGeneral;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetAllowUseGprs");

   return KErrNone;

   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::GetAllowRoaming
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::GetAllowRoaming(
   CStifItemParser& aItem )
   {
   LogMethod(_L("GetAllowRoaming"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::GetAllowRoaming");

   TInt expectedValue( -1 );
   aItem.GetNextInt( expectedValue );
   
   TInt value( 0 );
   TBool ret;
   ret = iSettingsEngine->GetAllowRoaming( value );
   
   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::GetAllowRoaming -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetAllowRoaming");
       return KErrGeneral;
       }
   
   if( expectedValue != -1 && expectedValue != value )
       {
       VCXLOGLO3("CVcxNsSettingsEngineTest::GetAllowRoaming -- Expected value: %d Returned value: %d", expectedValue, value);
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetAllowRoaming");
       return KErrCorrupt;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetAllowRoaming");

   return KErrNone;

   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::SetAllowRoaming
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::SetAllowRoaming(
   CStifItemParser& aItem )
   {
   LogMethod(_L("SetAllowRoaming"));
  
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::SetAllowRoaming");

   TInt value( 0 );
   aItem.GetNextInt( value );

   TBool ret;
   ret = iSettingsEngine->SetAllowRoaming( value );
   
   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::SetAllowRoaming -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetAllowRoaming");
       return KErrGeneral;
       }
  
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetAllowRoaming");

   return KErrNone;

   }

// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::CheckLockCodeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::CheckLockCodeL(
   CStifItemParser& aItem )
   {
   LogMethod(_L("CheckLockCodeL"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::CheckLockCodeL");

   TInt expectedValue( -1 );
   aItem.GetNextInt( expectedValue );
   
   TInt value( 0 );
   TBool ret;
   TRAPD(error, ret = iSettingsEngine->CheckLockCodeL());

   if( error != KErrNone )
       {
       VCXLOGLO2("CVcxNsSettingsEngineTest::CheckLockCodeL -- Error %d", error);
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::CheckLockCodeL");
       return KErrGeneral;
       }

   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::CheckLockCodeL -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::CheckLockCodeL");
       return KErrGeneral;
       }
   
   if( expectedValue != -1 && expectedValue != value )
       {
       VCXLOGLO3("CVcxNsSettingsEngineTest::CheckLockCodeL -- Expected value: %d Returned value: %d", expectedValue, value);
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::CheckLockCodeL");
       return KErrCorrupt;
       }
   
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::CheckLockCodeL");

   return KErrNone;
   }


// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::GetPlayerVolume
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::GetPlayerVolume(
   CStifItemParser& aItem )
   {
   LogMethod(_L("GetPlayerVolume"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::GetPlayerVolume");

    TInt expectedValue( -1 );
    aItem.GetNextInt( expectedValue );
    
    TInt value( 0 );
    TBool ret;
    ret = iSettingsEngine->GetPlayerVolume( value );
    
    if( !ret )
        {
        VCXLOGLO1("CVcxNsSettingsEngineTest::GetPlayerVolume -- Returned false!");
        VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetPlayerVolume");
        return KErrGeneral;
        }
    
    if( expectedValue != -1 && expectedValue != value )
        {
        VCXLOGLO3("CVcxNsSettingsEngineTest::GetPlayerVolume -- Expected value: %d Returned value: %d", expectedValue, value);
        VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetPlayerVolume");
        return KErrCorrupt;
        }

	VCXLOGLO1("<<< CVcxNsSettingsEngineTest::GetPlayerVolume");

   return KErrNone;
   }



// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::SetPlayerVolume
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVcxNsSettingsEngineTest::SetPlayerVolume(
   CStifItemParser& aItem )
   {
   LogMethod(_L("SetPlayerVolume"));
   
   VCXLOGLO1(">>> CVcxNsSettingsEngineTest::SetPlayerVolume");

   TInt value( 0 );
   aItem.GetNextInt( value );

   TBool ret;
   ret = iSettingsEngine->SetPlayerVolume( value );
   
   if( !ret )
       {
       VCXLOGLO1("CVcxNsSettingsEngineTest::SetPlayerVolume -- Returned false!");
       VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetPlayerVolume");
       return KErrGeneral;
       }
  
   VCXLOGLO1("<<< CVcxNsSettingsEngineTest::SetPlayerVolume");

   return KErrNone;

   }


// -----------------------------------------------------------------------------
// CVcxNsSettingsEngineTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CVcxNsSettingsEngineTest::?member_function(
   CStifItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
