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
#include "MPSettingsROPSettings.hrh"


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
// CMPXMediaSettingsEngineTest::LogMethod
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
// CMPXMediaSettingsEngineTest::PrintError
// Prints the errorcode to UI
// -----------------------------------------------------------------------------
//
void CMPXMediaSettingsEngineTest::PrintError( TInt aError )
    {
    // Print to UI
    TBuf< 24 > buffer;
    buffer.Format( KLogErrorCode, aError );
    TestModuleIf().Printf( 0, KLogError, buffer );
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::CreateL
// Generate log from current method
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::CreateL( CStifItemParser& aItem )
    {
    LogMethod(_L("CreateL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::CreateL");
    TInt error( 0 );
    
    const TUid KSettingsModelForROPUid = {KMPSettEngImplUidROP};
    
    iModel = CMPSettingsModel::NewL(KSettingsModelForROPUid);
    
    iSettingsModel = static_cast<CMPSettingsModelForROP*>(iModel);
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::CreateL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::ListImplementationsL
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

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::ListImplementationsL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::LoadSettingsLL
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
        PrintError( error );
        return error;
        }

    PrintError( error );
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::LoadSettingsLL");
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::LogMethod
// Generate log from current method
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::StoreSettingsLL( CStifItemParser& /*aItem*/ )
    {
    LogMethod(_L("StoreSettingsLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::StoreSettingsLL");
    TRAPD( error, iSettingsModel->StoreSettingsL() );
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::StoreSettingsLL");
    PrintError( error );
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::LogMethod
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

    PrintError( error );
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::SetVideoContrastL");
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::LogMethod
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
    
    PrintError( error );
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::GetVideoContrastL");
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetProxyModeL
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

    PrintError( error );
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::SetProxyModeL");
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetProxyModeL
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
    
    PrintError( error );
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::GetProxyModeL");
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetProxyHostNameLL
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

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetProxyHostNameLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetProxyHostNameL
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
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetProxyHostNameL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetProxyPortL
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

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetProxyPortL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetProxyPortL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetProxyPortL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::GetProxyPortL");    
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetDefaultApL
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
    TBool iapFound = GetIapIdL( iapName, iapId );
    
    if( !iapFound )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::SetDefaultApL -- no IAP id found, using %d", KDefaultIapId );
        iapId = KDefaultIapId;
        }
    
    error = iSettingsModel->SetDefaultAp( iapId );
    VCXLOGLO2("CMPXMediaSettingsEngineTest::SetDefaultApL -- error after call: %d", error );

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetDefaultApL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetDefaultApL
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
    TBool iapFound = GetIapIdL( expectedIap, iapId );
    
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
        PrintError( error );
        return error;
        }

    if( value != iapId )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetDefaultApL -- value %d, expected value %d", value, iapId );
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO1("<-- CMPXMediaSettingsEngineTest::GetDefaultApL");    
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetBandwidthControlModeL
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

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetBandwidthControlModeL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetBandwidthControlModeL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetBandwidthControlModeL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetMaxBandwidthL
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

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetMaxBandwidthL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetMaxBandwidthL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetMaxBandwidthL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetMaxBandwidthL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetConnectionTimeoutL
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
        PrintError( error );
        return error;
        }

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetConnectionTimeoutL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetConnectionTimeoutL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetConnectionTimeoutL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetConnectionTimeoutL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetServerTimeoutL
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
        PrintError( error );
        return error;
        }

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetServerTimeoutL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetServerTimeoutL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetServerTimeoutL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetServerTimeoutL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetMinUDPPortL
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

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetMinUDPPortL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetMinUDPPortL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetMinUDPPortL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetMinUDPPortL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetMaxUDPPortL
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

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetMaxUDPPort return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetMaxUDPPortL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetMaxUDPPortL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetMaxUDPPortL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetControllerVersionInfoL
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
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetControllerVersionInfoL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetControllerBuildDateL
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
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetControllerBuildDateL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetControllerAdditionalInfoL
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
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetControllerAdditionalInfoL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetDemandBwFactorL
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

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetDemandBwFactorL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetDemandBwFactorL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetDemandBwFactorL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetDemandBwFactorL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetSustainBandwidthL
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

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetSustainBandwidthL return %d", error);       
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetSustainBandwidthL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::GetSustainBandwidthL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetSustainBandwidthL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetSustainBwPresetsLL
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
        PrintError( error );
        return error;
        }

    VCXLOGLO1("CMPXMediaSettingsEngineTest::SetSustainBandwidthL returned values:");
    TInt i( 0 );
    for( i = 0; i < values.Count(); i++ )
        {
        VCXLOGLO3("[%d] = %d", i, values[i]);
        }

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetSustainBandwidthL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetMaxBwPresetsLL
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
        PrintError( error );
        return error;
        }

    VCXLOGLO1("CMPXMediaSettingsEngineTest::GetMaxBwPresetsL returned values:");
    TInt i( 0 );
    for( i = 0; i < values.Count(); i++ )
        {
        VCXLOGLO3("[%d] = %d", i, values[i]);
        }

    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::GetMaxBwPresetsLL return %d", error);
    
    PrintError( error );
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetAutoDisconTimeLL
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
        PrintError( error );
        return error;
        }

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetAutoDisconTimeLL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::AutoDisconTimeLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::AutoDisconTimeLL( CStifItemParser& aItem )
    {
    LogMethod(_L("AutoDisconTimeLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::AutoDisconTimeLL");
    TInt error( 0 );
    
    TInt expectedValue( 0 );
    aItem.GetNextInt( expectedValue );
    
    TInt value( 0 );
    value = iSettingsModel->AutoDisconTimeL();
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::AutoDisconTimeLL -- error after call: %d", error );
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::AutoDisconTimeLL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::AutoDisconTimeLL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetVideoRepeatLL
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

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetVideoRepeatLL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::IsVideoRepeatOnLL
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
        PrintError( error );
        return error;
        }
   
    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::IsVideoRepeatOnL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::IsVideoRepeatOnLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetDefaultViewLL
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
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetDefaultViewLL return %d", error);    
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::IsDefaultViewOnLL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::IsDefaultViewOnLL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::IsDefaultViewOnLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::SetRockerKeysLL
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
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetRockerKeysLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::ShowRockerKeysLL
// -----------------------------------------------------------------------------
//
TInt CMPXMediaSettingsEngineTest::ShowRockerKeysLL( CStifItemParser& aItem )
    {
    LogMethod(_L("ShowRockerKeysLL"));
    VCXLOGLO1("--> CMPXMediaSettingsEngineTest::SetRockerKeysLL");

    TInt temp( 0 );
    User::LeaveIfError( aItem.GetNextInt( temp ) );
    TBool expectedValue( temp );
    
    // Set expected value to false if rocker keys are not supported
    if( !iSettingsModel->IsRockerKeysSupportedL() )
        {
        expectedValue = EFalse;
        }
    
    TBool value( 0 );
    TRAPD( error, value = iSettingsModel->ShowRockerKeysL() );
    
    if( error != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::ShowRockerKeysLL -- error after call: %d", error );
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::ShowRockerKeysLL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }
    
    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::SetRockerKeysLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::IsRockerKeysSupportedLL
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
        PrintError( error );
        return error;
        }

    if( value != expectedValue )
        {
        VCXLOGLO3("CMPXMediaSettingsEngineTest::IsRockerKeysSupportedLL -- value %d, expected value %d", value, expectedValue);
        error = KErrCorrupt;
        }

    PrintError( error );
    VCXLOGLO2("<-- CMPXMediaSettingsEngineTest::IsRockerKeysSupportedLL return %d", error);
    return error;
    }

// -----------------------------------------------------------------------------
// CMPXMediaSettingsEngineTest::GetIapIdL
// -----------------------------------------------------------------------------
//
TBool CMPXMediaSettingsEngineTest::GetIapIdL(const TDesC& aIapName, TUint32& aIapId)
    {
    VCXLOGLO1(">>>CMPXMediaSettingsEngineTest::GetIapIdL");

    TBool found( EFalse );

    VCXLOGLO2("CMPXMediaSettingsEngineTest:: Searching IAP: %S", &aIapName);

    found = GetIapIdByNameL(aIapName, aIapId);

    // Iap with exact name found
    if( found )
        {
        VCXLOGLO1("<<<CMPXMediaSettingsEngineTest::GetIapIdL");
        return found;
        }

    VCXLOGLO1("<<<CMPXMediaSettingsEngineTest:: Iap with exact name was not found.");

    TBuf<128> searchFor(aIapName);
    searchFor.LowerCase();

    if(searchFor == _L("invalidiap") )
        {
        aIapId = 6000;
        VCXLOGLO1("<<<CMPXMediaSettingsEngineTest::GetIapIdL");
        return ETrue;
        }

    TBool isWlanIapName( EFalse );

    if( aIapName == _L("wlan") )
        {
        found = GetIapIdByTypeL(EWlanIap, aIapId, 0);
        isWlanIapName = ETrue;
        }
    else
    if( aIapName == _L("wlan2") )
        {
        found = GetIapIdByTypeL(EWlanIap, aIapId, 1);
        isWlanIapName = ETrue;
        }
    else
    if( aIapName == _L("internet") )
        {
        found = GetIapIdByTypeL(EGprsIap, aIapId, 0);
        }
    else
    if( aIapName == _L("internet2") )
        {
        found = GetIapIdByTypeL(EGprsIap, aIapId, 1);
        }

    if( !found && !isWlanIapName )
        {
        VCXLOGLO1("<<<CMPXMediaSettingsEngineTest:: Trying to find alternative IAP.");

        // Try search other known GPRS IAPs
        _LIT(KIapElisaInternet, "elisa internet");
        _LIT(KIapElisaMMS, "elisa mms");
        _LIT(KIapInternet, "internet");
        _LIT(KIapProinternet, "prointernet");
        _LIT(KGprsInternet, "gprs internet");

        if( GetIapIdByNameL(KIapElisaInternet, aIapId) ||
            GetIapIdByNameL(KIapElisaMMS, aIapId) ||
            GetIapIdByNameL(KIapInternet, aIapId) ||
            GetIapIdByNameL(KIapProinternet, aIapId) ||
            GetIapIdByNameL(KGprsInternet, aIapId) )
            {
            found = ETrue;
            }

        if( !found )
            {
            VCXLOGLO1("<<<CMPXMediaSettingsEngineTest:: Any GPRS IAP is good...");
            found = GetIapIdByTypeL(EGprsIap, aIapId, 0);
            }
        }
    else
        {
        VCXLOGLO1("<<<CMPXMediaSettingsEngineTest:: WLAN IAP wanted. Not found.");
        }

    VCXLOGLO1("<<<CMPXMediaSettingsEngineTest::GetIapIdL");
    return found;
    }

// -----------------------------------------------------------------------------
// CVCXTestCommon::GetIapIdByNameL
// -----------------------------------------------------------------------------
//
TBool CMPXMediaSettingsEngineTest::GetIapIdByNameL(const TDesC& aIapName, TUint32& aIapId)
    {
    VCXLOGLO1(">>>CMPXMediaSettingsEngineTest::GetIapIdByNameL");
    TBool found = EFalse;
    aIapId = 0;

    TBuf<128> searchFor(aIapName);
    searchFor.LowerCase();
    
    // Get IAP names and ids from the database
    CCommsDatabase* cdb( NULL );
    TRAPD( err, cdb = CCommsDatabase::NewL( EDatabaseTypeIAP ) );
    if( err != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetIapIdByNameL: CCommsDatabase::NewL left: %d, leaving", err);
        User::Leave( err );
        }
    CleanupStack::PushL( cdb );

    cdb->ShowHiddenRecords();

    CCommsDbTableView* view( NULL );
    TRAP( err, view = cdb->OpenTableLC( TPtrC(IAP) ); CleanupStack::Pop( view ); );
    CleanupStack::PushL( view );
    
    if( err != KErrNone )
        {
        VCXLOGLO2("CMPXMediaSettingsEngineTest::GetIapIdByNameL: CCommsDatabase::OpenTableLC left: %d, leaving", err);
        User::Leave( err );
        }
    
    TBuf<40> name;
    TUint32 id;
    TInt res = view->GotoFirstRecord();
    while( res == KErrNone )
        {
        TRAP( err, view->ReadTextL( TPtrC(COMMDB_NAME), name ) );
        if( err != KErrNone ) continue;
        TRAP( err, view->ReadUintL( TPtrC(COMMDB_ID), id ) );
        if( err != KErrNone ) continue;

        name.LowerCase();
        if(searchFor == name)
            {
            aIapId = id;
            VCXLOGLO3("CMPXMediaSettingsEngineTest:: Found IAP: %d, %S", aIapId, &aIapName);
            found = ETrue;
            break;
            }
        res = view->GotoNextRecord();
        }

    CleanupStack::PopAndDestroy( view );
    CleanupStack::PopAndDestroy( cdb );

    VCXLOGLO1("<<<CMPXMediaSettingsEngineTest::GetIapIdByNameL");
    return found;
    }

// -----------------------------------------------------------------------------
// CVCXTestCommon::GetIapIdByTypeL
// -----------------------------------------------------------------------------
//
TBool CMPXMediaSettingsEngineTest::GetIapIdByTypeL(TIapType aType, TUint32& aIapId, TInt aOrderNumber)
    {
    VCXLOGLO1(">>>CMPXMediaSettingsEngineTest::GetIapIdByTypeL");

    aIapId = 0;
    TBool found( EFalse );

    #ifdef __WINSCW__
        found = GetIapIdByNameL( _L("Ethernet with Daemon Dynamic IP"), aIapId );
        VCXLOGLO1("<<<CMPXMediaSettingsEngineTest::GetIapIdByTypeL");
        return found;
    #endif

    // Get AP names and ids from the database
    CCommsDatabase* TheDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( TheDb );

    TheDb->ShowHiddenRecords();

    CCommsDbTableView* view = TheDb->OpenTableLC( TPtrC(IAP) );

    TBuf<40> name;
    TUint32 id;
    TBuf<255> iapType;
    TInt matchCount(0);

    TInt res = view->GotoFirstRecord();

    while( res == KErrNone )
        {
        view->ReadTextL( TPtrC(COMMDB_NAME), name );
        view->ReadUintL( TPtrC(COMMDB_ID), id );
        view->ReadTextL( TPtrC(IAP_SERVICE_TYPE), iapType);

        VCXLOGLO3("CMPXMediaSettingsEngineTest:: name: %S, id: %d", &name, id);
        VCXLOGLO2("CMPXMediaSettingsEngineTest:: type: %S", &iapType);

        if( ( iapType == _L("LANService") && aType == EWlanIap && name != _L("Easy WLAN") ) ||
            ( iapType == _L("OutgoingGPRS") && aType == EGprsIap ) )
            {
            if( matchCount == aOrderNumber )
                {
                found = ETrue;
                aIapId = id;
                break;
                }
            matchCount++;
            }

        res = view->GotoNextRecord();
        }

    CleanupStack::PopAndDestroy( view ); // view
    CleanupStack::PopAndDestroy( TheDb ); // TheDb

    VCXLOGLO1("<<<CMPXMediaSettingsEngineTest::GetIapIdByTypeL");
    return found;
    }

//  [End of File] - Do not remove
