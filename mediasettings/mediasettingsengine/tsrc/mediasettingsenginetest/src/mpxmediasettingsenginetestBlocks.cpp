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
* Description:   This file contains testclass implementation.*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <commdb.h>
#include "mpxmediasettingsenginetest.h"
#include "MPSettEngPluginImplementationUIDs.hrh"
#include "MPSettingsModelForROP.h"
#include "vcxtestlog.h"
#include "IptvTestUtilALR.h"
#include "MPSettingsROPSettings.hrh"
#include "VCXTestCommon.h"

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
// CMPXMediaSettingsEngineTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMPXMediaSettingsEngineTest::Delete() 
    {

    if( iModel )
        {
        delete iModel;
        iModel = NULL;
        }

    REComSession::FinalClose();

    if (iCommDb) 
        {
        delete iCommDb;
        iCommDb = NULL;
        }
    
    if( iVCXTestCommon )
        {
        delete iVCXTestCommon;
        iVCXTestCommon = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Create", CMPXMediaSettingsEngineTest::CreateL ),
        ENTRY( "ListImplementations", CMPXMediaSettingsEngineTest::ListImplementationsL ),
        ENTRY( "LoadSettings", CMPXMediaSettingsEngineTest::LoadSettingsLL ),
        ENTRY( "StoreSettings", CMPXMediaSettingsEngineTest::StoreSettingsLL ),
        ENTRY( "SetVideoContrast", CMPXMediaSettingsEngineTest::SetVideoContrastL ),
        ENTRY( "GetVideoContrast", CMPXMediaSettingsEngineTest::GetVideoContrastL ),
        ENTRY( "SetProxyMode", CMPXMediaSettingsEngineTest::SetProxyModeL ),
        ENTRY( "GetProxyMode", CMPXMediaSettingsEngineTest::GetProxyModeL ),
        ENTRY( "SetProxyHostName", CMPXMediaSettingsEngineTest::SetProxyHostNameLL ),
        ENTRY( "GetProxyHostName", CMPXMediaSettingsEngineTest::GetProxyHostNameL ),
        ENTRY( "SetProxyPort", CMPXMediaSettingsEngineTest::SetProxyPortL ),
        ENTRY( "GetProxyPort", CMPXMediaSettingsEngineTest::GetProxyPortL ),
        ENTRY( "SetDefaultAp", CMPXMediaSettingsEngineTest::SetDefaultApL ),
        ENTRY( "GetDefaultAp", CMPXMediaSettingsEngineTest::GetDefaultApL ),
        ENTRY( "SetBandwidthControlMode", CMPXMediaSettingsEngineTest::SetBandwidthControlModeL ),
        ENTRY( "GetBandwidthControlMode", CMPXMediaSettingsEngineTest::GetBandwidthControlModeL ),
        ENTRY( "SetMaxBandwidth", CMPXMediaSettingsEngineTest::SetMaxBandwidthL ),
        ENTRY( "GetMaxBandwidth", CMPXMediaSettingsEngineTest::GetMaxBandwidthL ),
        ENTRY( "SetConnectionTimeout", CMPXMediaSettingsEngineTest::SetConnectionTimeoutL ),
        ENTRY( "GetConnectionTimeout", CMPXMediaSettingsEngineTest::GetConnectionTimeoutL ),
        ENTRY( "SetServerTimeout", CMPXMediaSettingsEngineTest::SetServerTimeoutL ),
        ENTRY( "GetServerTimeout", CMPXMediaSettingsEngineTest::GetServerTimeoutL ),
        ENTRY( "SetMinUDPPort", CMPXMediaSettingsEngineTest::SetMinUDPPortL ),
        ENTRY( "GetMinUDPPort", CMPXMediaSettingsEngineTest::GetMinUDPPortL ),
        ENTRY( "SetMaxUDPPort", CMPXMediaSettingsEngineTest::SetMaxUDPPortL ),
        ENTRY( "GetMaxUDPPort", CMPXMediaSettingsEngineTest::GetMaxUDPPortL ),
        ENTRY( "GetControllerVersionInfo", CMPXMediaSettingsEngineTest::GetControllerVersionInfoL ),
        ENTRY( "GetControllerBuildDate", CMPXMediaSettingsEngineTest::GetControllerBuildDateL ),
        ENTRY( "GetControllerAdditionalInfo", CMPXMediaSettingsEngineTest::GetControllerAdditionalInfoL ),
        ENTRY( "SetDemandBwFactor", CMPXMediaSettingsEngineTest::SetDemandBwFactorL ),
        ENTRY( "GetDemandBwFactor", CMPXMediaSettingsEngineTest::GetDemandBwFactorL ),
        ENTRY( "SetSustainBandwidth", CMPXMediaSettingsEngineTest::SetSustainBandwidthL ),
        ENTRY( "GetSustainBandwidth", CMPXMediaSettingsEngineTest::GetSustainBandwidthL ),
        ENTRY( "GetSustainBwPresets", CMPXMediaSettingsEngineTest::GetSustainBwPresetsLL ),
        ENTRY( "GetMaxBwPresets", CMPXMediaSettingsEngineTest::GetMaxBwPresetsLL ),
        ENTRY( "SetAutoDisconTime", CMPXMediaSettingsEngineTest::SetAutoDisconTimeLL ),
        ENTRY( "AutoDisconTime", CMPXMediaSettingsEngineTest::AutoDisconTimeLL ),
        ENTRY( "SetVideoRepeat", CMPXMediaSettingsEngineTest::SetVideoRepeatLL ),
        ENTRY( "IsVideoRepeatOn", CMPXMediaSettingsEngineTest::IsVideoRepeatOnLL ),
        ENTRY( "SetDefaultView", CMPXMediaSettingsEngineTest::SetDefaultViewLL ),
        ENTRY( "IsDefaultViewOn", CMPXMediaSettingsEngineTest::IsDefaultViewOnLL ),
        ENTRY( "SetRockerKeys", CMPXMediaSettingsEngineTest::SetRockerKeysLL ),
        ENTRY( "ShowRockerKeys", CMPXMediaSettingsEngineTest::ShowRockerKeysLL ),
        ENTRY( "IsRockerKeysSupported", CMPXMediaSettingsEngineTest::IsRockerKeysSupportedLL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::LogMethod
// Generate log from current method
// -----------------------------------------------------------------------------
//
void CMPXMediaSettingsEngineTest::LogMethod( TPtrC aMethod )
    {
    // Print to UI
    TBuf< 64 > buffer;
    buffer.Format( KLogLocation, &aMethod );
    TestModuleIf().Printf( 0, KTestModuleName, buffer );
    // Print to log file
    iLog->Log( buffer );
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::LogMethod
// Generate log from current method
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::CreateL( CStifItemParser& aItem )
    {
    LogMethod(_L("CreateL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::CreateL");
    TInt error( 0 );
    
    iVCXTestCommon = CVCXTestCommon::NewL();
    
    const TUid KSettingsModelForROPUid = {KMPSettEngImplUidROP};
    
    iModel = CMPSettingsModel::NewL(KSettingsModelForROPUid);
    
    iSettingsModel = static_cast<CMPSettingsModelForROP*>(iModel);
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::CreateL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::ListImplementationsL
// List implementations of mpsettingsmodel
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::ListImplementationsL( CStifItemParser& aItem )
    {
    LogMethod(_L("ListImplementationsL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::ListImplementationsL");
    TInt error( 0 );
    
    RImplInfoPtrArray array;
    iSettingsModel->ListImplementationsL( array );
    
    TInt i( 0 );
    for( i = 0; i < array.Count(); i++ )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::ListImplementationsL, [%d] uid = %d", i, array[i]->ImplementationUid() );
        }

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::ListImplementationsL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::LogMethod
// Generate log from current method
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::LoadSettingsLL( CStifItemParser& aItem )
    {
    LogMethod(_L("LoadSettingsLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::LoadSettingsLL");
    
    TInt configVersion(0);
    User::LeaveIfError( aItem.GetNextInt( configVersion ) );
    
    TRAPD( error, iSettingsModel->LoadSettingsL( configVersion ) );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::LoadSettingsLL -- error after call: %d", error );
        return error;
        }

    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::LoadSettingsLL");
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::LogMethod
// Generate log from current method
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::StoreSettingsLL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("StoreSettingsLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::StoreSettingsLL");
    TRAPD( error, iSettingsModel->StoreSettingsL() );
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::StoreSettingsLL");
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::LogMethod
// Generate log from current method
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetVideoContrastL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetVideoContrastL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetVideoContrastL");
    TInt error( 0 );
    
    TInt value(0);
    User::LeaveIfError( aItem.GetNextInt( value ) );
    
    error = iSettingsModel->SetVideoContrast( value );
    VCXLOGLO2("CMPXMediaSettingsEngineTest::SetVideoContrastL -- error after call: %d", error );

    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::SetVideoContrastL");
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::LogMethod
// Generate log from current method
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetVideoContrastL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetVideoContrastL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetVideoContrastL");
    TInt error( 0 );
    
    TInt expectedValue(0);
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetVideoContrast( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetVideoContrastL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetVideoContrastL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::GetVideoContrastL");
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetProxyModeL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetProxyModeL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetProxyModeL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetProxyModeL");
    TInt error( 0 );

    TInt value(0);
    User::LeaveIfError( aItem.GetNextInt( value ) );
    
    error = iSettingsModel->SetProxyMode( value );
    VCXLOGLO2("CMPXMediaSettingsEngineTest::SetProxyModeL -- error after call: %d", error );

    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::SetProxyModeL");
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetProxyModeL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetProxyModeL ( CStifItemParser& aItem )
    {
    LogMethod(_L("GetProxyModeL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetProxyModeL");
    TInt error( 0 );
    
    TInt expectedValue(0);
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetProxyMode( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetProxyModeL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetProxyModeL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::GetProxyModeL");
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetProxyHostNameLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetProxyHostNameLL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetProxyHostNameLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetProxyHostNameLL");

    TPtrC value;
    User::LeaveIfError( aItem.GetNextString( value ) );
    
    TRAPD( error, iSettingsModel->SetProxyHostNameL( value ) );
    VCXLOGLO2("CMPXMediaSettingsEngineTest::SetProxyHostNameLL -- error after call: %d", error );

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetProxyHostNameLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetProxyHostNameL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetProxyHostNameL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetProxyHostNameL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetProxyHostNameL");
    
    TPtrC expectedValue;
    User::LeaveIfError( aItem.GetNextString( expectedValue ) );

    TBuf<256> value;
    TRAPD( error, iSettingsModel->GetProxyHostName( value ) );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetProxyHostNameL -- error after call: %d", error );
        return error;
        }

    if( value.CompareC(expectedValue) )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetProxyHostNameL -- value %S, expected value %S", &value, &expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetProxyHostNameL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetProxyPortL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetProxyPortL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetProxyPortL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetProxyPortL");
    TInt error( 0 );

    TInt value(0);
    User::LeaveIfError( aItem.GetNextInt( value ) );
    
    error = iSettingsModel->SetProxyPort( value );
    VCXLOGLO2("CMPXMediaSettingsEngineTest::SetProxyPortL -- error after call: %d", error );

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetProxyPortL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetProxyPortL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetProxyPortL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetProxyPortL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetProxyPortL");
    TInt error( 0 );
    
    TInt expectedValue(0);
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetProxyPort( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetProxyPortL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetProxyPortL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::GetProxyPortL");    
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetDefaultApL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetDefaultApL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetDefaultApL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetDefaultApL");
    TInt error( 0 );

    TPtrC iapName;
    User::LeaveIfError( aItem.GetNextString( iapName ) );

    TUint32 iapId( 0 );
    TBool iapFound = iVCXTestCommon->GetIapIdL( iapName, iapId );
    
    if( !iapFound )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::SetDefaultApL -- no IAP id found, using %d", KDefaultIapId );
        iapId = KDefaultIapId;
        }
    
    error = iSettingsModel->SetDefaultAp( iapId );
    VCXLOGLO2("CMPXMediaSettingsEngineTest::SetDefaultApL -- error after call: %d", error );

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetDefaultApL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetDefaultApL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetDefaultApL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetDefaultApL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetDefaultApL");
    TInt error( 0 );
    
    TPtrC expectedIap;
    User::LeaveIfError( aItem.GetNextString( expectedIap ) );
    
    TUint32 iapId( 0 );
    TBool iapFound = iVCXTestCommon->GetIapIdL( expectedIap, iapId );
    
    if( !iapFound )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetDefaultApL -- no IAP id found, using %d", KDefaultIapId );
        iapId = KDefaultIapId;
        }
    
    TUint32 value( 0 );
    error = iSettingsModel->GetDefaultAp( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetDefaultApL -- error after call: %d", error );
        return error;
        }

    if( value != iapId )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetDefaultApL -- value %d, expected value %d", value, iapId );
        error = KErrCorrupt;
        }
    
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::GetDefaultApL");    
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetBandwidthControlModeL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetBandwidthControlModeL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetBandwidthControlModeL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetBandwidthControlModeL");
    TInt error( 0 );

    TInt value(0);
    User::LeaveIfError( aItem.GetNextInt( value ) );
    
    error = iSettingsModel->SetBandwidthControlMode( value );
    VCXLOGLO2("CMPXMediaSettingsEngineTest::SetBandwidthControlModeL -- error after call: %d", error );

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetBandwidthControlModeL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetBandwidthControlModeL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetBandwidthControlModeL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetBandwidthControlModeL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetBandwidthControlModeL");
    TInt error( 0 );
    
    TInt expectedValue(0);
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetBandwidthControlMode( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetBandwidthControlModeL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetBandwidthControlModeL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetMaxBandwidthL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetMaxBandwidthL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetMaxBandwidthL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetMaxBandwidthL");
    TInt error( 0 );

    // value for databearer
    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    TDataBearer dataBearer = static_cast<TDataBearer>(value);
    
    // value for maxbandwidth
    User::LeaveIfError( aItem.GetNextInt( value ) );
   
    error = iSettingsModel->SetMaxBandwidth( value, dataBearer );
    
    VCXLOGLO2("CMPXMediaSettingsEngineTest::SetMaxBandwidthL -- error after call: %d", error );

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetMaxBandwidthL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetMaxBandwidthL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetMaxBandwidthL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetMaxBandwidthL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetMaxBandwidthL");
    TInt error( 0 );

    // value for databearer
    TInt bearerType( 0 );
    User::LeaveIfError( aItem.GetNextInt( bearerType ) );

    TDataBearer dataBearer = static_cast<TDataBearer>(bearerType);
    
    // value for expected value
    TInt expectedValue( 0 );
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetMaxBandwidth( value, dataBearer );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetMaxBandwidthL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetMaxBandwidthL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetMaxBandwidthL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetConnectionTimeoutL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetConnectionTimeoutL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetConnectionTimeoutL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetConnectionTimeoutL");
    TInt error( 0 );
    
    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    error = iSettingsModel->SetConnectionTimeout( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::SetConnectionTimeoutL -- error after call: %d", error );
        return error;
        }

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetConnectionTimeoutL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetConnectionTimeoutL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetConnectionTimeoutL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetConnectionTimeoutL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetConnectionTimeoutL");
    
    TInt error( 0 );

    TInt expectedValue( 0 );
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetConnectionTimeout( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetConnectionTimeoutL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetConnectionTimeoutL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetConnectionTimeoutL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetServerTimeoutL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetServerTimeoutL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetServerTimeoutL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetServerTimeoutL");
    TInt error( 0 );
    
    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    error = iSettingsModel->SetServerTimeout( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::SetServerTimeoutL -- error after call: %d", error );
        return error;
        }

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetServerTimeoutL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetServerTimeoutL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetServerTimeoutL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetServerTimeoutL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetServerTimeoutL");
    
    TInt error( 0 );

    TInt expectedValue( 0 );
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetServerTimeout( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetServerTimeoutL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetServerTimeoutL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetServerTimeoutL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetMinUDPPortL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetMinUDPPortL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetMinUDPPortL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetMinUDPPortL");
    TInt error( 0 );
    
    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    error = iSettingsModel->SetMinUDPPort( value );

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetMinUDPPortL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetMinUDPPortL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetMinUDPPortL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetMinUDPPortL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetMinUDPPortL");
    
    TInt error( 0 );

    TInt expectedValue( 0 );
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetMinUDPPort( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetMinUDPPortL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetMinUDPPortL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetMinUDPPortL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetMaxUDPPortL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetMaxUDPPortL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetMaxUDPPortL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetMaxUDPPortL");
    TInt error( 0 );
    
    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    error = iSettingsModel->SetMaxUDPPort( value );

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetMaxUDPPort return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetMaxUDPPortL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetMaxUDPPortL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetMaxUDPPortL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetMaxUDPPortL");
    
    TInt error( 0 );

    TInt expectedValue( 0 );
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetMaxUDPPort( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetMaxUDPPortL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetMaxUDPPortL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetMaxUDPPortL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetControllerVersionInfoL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetControllerVersionInfoL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetControllerVersionInfoL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetControllerVersionInfoL");
    TInt error( 0 );
    
    TBuf<256> verInfo;
    error = iSettingsModel->GetControllerVersionInfo( verInfo );

    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetControllerVersionInfoL -- error after call: %d", error );
        }
    
    VCXLOGLO2("CMPXMediaSettingsEngineTest::GetControllerVersionInfoL : %S", &verInfo );
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetControllerVersionInfoL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetControllerBuildDateL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetControllerBuildDateL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetControllerBuildDateL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetControllerBuildDateL");
    TInt error( 0 );
    
    TBuf<256> bldDate;
    error = iSettingsModel->GetControllerBuildDate( bldDate );

    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetControllerBuildDateL -- error after call: %d", error );
        }
    
    VCXLOGLO2("CMPXMediaSettingsEngineTest::GetControllerBuildDateL : %S", &bldDate );
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetControllerBuildDateL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetControllerAdditionalInfoL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetControllerAdditionalInfoL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetControllerAdditionalInfoL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetControllerAdditionalInfoL");
    TInt error( 0 );
    
    TBuf<256> addInfo;
    error = iSettingsModel->GetControllerAdditionalInfo( addInfo );

    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetControllerAdditionalInfoL -- error after call: %d", error );
        }
    
    VCXLOGLO2("CMPXMediaSettingsEngineTest::GetControllerAdditionalInfoL : %S", &addInfo );
     
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetControllerAdditionalInfoL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetDemandBwFactorL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetDemandBwFactorL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetDemandBwFactorL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetDemandBwFactorL");
    TInt error( 0 );
    
    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    error = iSettingsModel->SetDemandBwFactor( value );

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetDemandBwFactorL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetDemandBwFactorL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetDemandBwFactorL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetDemandBwFactorL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetDemandBwFactorL");
    
    TInt error( 0 );

    TInt expectedValue( 0 );
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetDemandBwFactor( value );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetDemandBwFactorL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetDemandBwFactorL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetDemandBwFactorL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetSustainBandwidthL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetSustainBandwidthL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetSustainBandwidthL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetSustainBandwidthL");
    TInt error( 0 );

    // value for databearer
    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    TDataBearer dataBearer = static_cast<TDataBearer>(value);
    
    // value for maxbandwidth
    User::LeaveIfError( aItem.GetNextInt( value ) );
   
    error = iSettingsModel->SetSustainBandwidth( value, dataBearer );

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetSustainBandwidthL return %d", error);       
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetSustainBandwidthL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetSustainBandwidthL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetSustainBandwidthL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetSustainBandwidthL");
    TInt error( 0 );

    // value for databearer
    TInt bearerType( 0 );
    User::LeaveIfError( aItem.GetNextInt( bearerType ) );

    TDataBearer dataBearer = static_cast<TDataBearer>(bearerType);
    
    // value for expected value
    TInt expectedValue( 0 );
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    error = iSettingsModel->GetSustainBandwidth( value, dataBearer );

    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetSustainBandwidthL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetSustainBandwidthL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetSustainBandwidthL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetSustainBwPresetsLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetSustainBwPresetsLL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetSustainBwPresetsLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetSustainBandwidthL");
    TInt error( 0 );

    // value for databearer
    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    TDataBearer dataBearer = static_cast<TDataBearer>(value);
    
    RArray<TInt> values;
    error = iSettingsModel->GetSustainBwPresetsL( values, dataBearer );

    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetSustainBwPresetsLL -- error after call: %d", error );
        return error;
        }

    VCXLOGLO1("CMPXMediaSettingsEngineTest::SetSustainBandwidthL returned values:");
    TInt i( 0 );
    for( i = 0; i < values.Count(); i++ )
        {
        VCXLOGLO3("[%d] = %d", i, values[i]);
        }

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetSustainBandwidthL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::GetMaxBwPresetsLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::GetMaxBwPresetsLL( CStifItemParser& aItem )
    {
    LogMethod(_L("GetMaxBwPresetsLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::GetMaxBwPresetsLL");
    TInt error( 0 );

    // value for databearer
    TInt bearerType( 0 );
    User::LeaveIfError( aItem.GetNextInt( bearerType ) );

    TDataBearer dataBearer = static_cast<TDataBearer>(bearerType);
    
    RArray<TInt> values;
    error = iSettingsModel->GetMaxBwPresetsL( values, dataBearer );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetMaxBwPresetsLL -- error after call: %d", error );
        return error;
        }

    VCXLOGLO1("CMPXMediaSettingsEngineTest::GetMaxBwPresetsL returned values:");
    TInt i( 0 );
    for( i = 0; i < values.Count(); i++ )
        {
        VCXLOGLO3("[%d] = %d", i, values[i]);
        }

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetMaxBwPresetsLL return %d", error);
    
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetAutoDisconTimeLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetAutoDisconTimeLL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetAutoDisconTimeLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetAutoDisconTimeLL");
    
    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    TRAPD( error, iSettingsModel->SetAutoDisconTimeL( value ) );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::SetAutoDisconTimeLL -- error after call: %d", error );
        return error;
        }

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetAutoDisconTimeLL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::AutoDisconTimeLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::AutoDisconTimeLL( CStifItemParser& aItem )
    {
    LogMethod(_L("AutoDisconTimeLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::AutoDisconTimeLL");
    TInt error( 0 );
    
    TInt expectedValue( 0 );
    User::LeaveIfError( aItem.GetNextInt( expectedValue ) );
    
    TInt value( 0 );
    value = iSettingsModel->AutoDisconTimeL();
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::AutoDisconTimeLL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::AutoDisconTimeLL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::AutoDisconTimeLL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetVideoRepeatLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetVideoRepeatLL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetVideoRepeatLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetVideoRepeatLL");

    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    TBool repeat( value );
    TRAPD( error, iSettingsModel->SetVideoRepeatL( repeat ) );

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetVideoRepeatLL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::IsVideoRepeatOnLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::IsVideoRepeatOnLL( CStifItemParser& aItem )
    {
    LogMethod(_L("IsVideoRepeatOnLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::IsVideoRepeatOnLL");

    TInt temp( 0 );
    User::LeaveIfError( aItem.GetNextInt( temp ) );
    
    TBool expectedValue( temp );
    
    TBool value( 0 );
    TRAPD( error, value = iSettingsModel->IsVideoRepeatOnL() );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::IsVideoRepeatOnL -- error after call: %d", error );
        return error;
        }
   
    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::IsVideoRepeatOnL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::IsVideoRepeatOnLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetDefaultViewLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetDefaultViewLL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetDefaultViewLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetDefaultViewLL");

    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    TBool view( value );
    TRAPD( error, iSettingsModel->SetDefaultViewL( view ) );
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetDefaultViewLL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::IsDefaultViewOnLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::IsDefaultViewOnLL( CStifItemParser& aItem )
    {
    LogMethod(_L("IsDefaultViewOnLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::IsDefaultViewOnLL");
    
    TInt temp( 0 );
    User::LeaveIfError( aItem.GetNextInt( temp ) );
    
    TBool expectedValue( temp );
    
    TBool value( 0 );
    TRAPD( error, value = iSettingsModel->IsDefaultViewOnL() );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::IsDefaultViewOnLL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::IsDefaultViewOnLL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::IsDefaultViewOnLL return %d", error);

    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::SetRockerKeysLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::SetRockerKeysLL( CStifItemParser& aItem )
    {
    LogMethod(_L("SetRockerKeysLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetRockerKeysLL");
    
    TInt value( 0 );
    User::LeaveIfError( aItem.GetNextInt( value ) );

    TBool keys( value );
    TRAPD( error, iSettingsModel->SetRockerKeysL( keys ) );
    
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetRockerKeysLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::ShowRockerKeysLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::ShowRockerKeysLL( CStifItemParser& aItem )
    {
    LogMethod(_L("ShowRockerKeysLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetRockerKeysLL");

    TInt temp( 0 );
    User::LeaveIfError( aItem.GetNextInt( temp ) );
    
    TBool expectedValue( temp );
    
    TBool value( 0 );
    TRAPD( error, value = iSettingsModel->ShowRockerKeysL() );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::ShowRockerKeysLL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::ShowRockerKeysLL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetRockerKeysLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CVideoScheduler_ApiTest::IsRockerKeysSupportedLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::IsRockerKeysSupportedLL( CStifItemParser& aItem )
    {
    LogMethod(_L("IsRockerKeysSupportedLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::IsRockerKeysSupportedLL");
    
    TInt temp( 0 );
    User::LeaveIfError( aItem.GetNextInt( temp ) );
    
    TBool expectedValue( temp );
    
    TBool value( 0 );
    TRAPD( error, value = iSettingsModel->IsRockerKeysSupportedL() );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::IsRockerKeysSupportedLL -- error after call: %d", error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::IsRockerKeysSupportedLL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::IsRockerKeysSupportedLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CMPXMediaSettingsEngineTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
