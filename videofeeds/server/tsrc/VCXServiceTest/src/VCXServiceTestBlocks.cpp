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
#include "VCXServiceTest.h"

#include "VCXTestLog.h"
#include "CIptvUtil.h"

#include "CIptvService.h"
#include "CIptvServices.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

_LIT( KSpace, " " );
const TInt KIptvTestMaxTextLength	= 2048;


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
// CIptvServiceTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CIptvServiceTest::Delete()
    {
	if(iService)
		{
		delete iService;
		iService = NULL;
		}

	if(iBuff)
		{
		delete iBuff;
		iBuff = NULL;
		}

	if(iServices)
		{
		delete iServices;
		iServices = NULL;
		}

	if(iServicesBuff)
		{
		delete iServicesBuff;
		iServicesBuff = NULL;
		}
	
    }

// -----------------------------------------------------------------------------
// CIptvServiceTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "CreateService", CIptvServiceTest::CreateServiceL ),
        ENTRY( "DestroyService", CIptvServiceTest::DestroyServiceL ),
        ENTRY( "SetId", CIptvServiceTest::SetIdL ),
        ENTRY( "SetName", CIptvServiceTest::SetNameL ),
        ENTRY( "SetAddress", CIptvServiceTest::SetAddressL ),
        ENTRY( "SetIconPath", CIptvServiceTest::SetIconPathL ),
        ENTRY( "SetEpgPluginUid", CIptvServiceTest::SetEpgPluginUidL ),
        ENTRY( "SetVodPluginUid", CIptvServiceTest::SetVodPluginUidL ),
        ENTRY( "SetIptvPluginUid", CIptvServiceTest::SetIptvPluginUidL ),
        ENTRY( "SetType", CIptvServiceTest::SetTypeL ),
		ENTRY( "CreateIapList", CIptvServiceTest::CreateIapListL ),
		ENTRY( "CheckIapList", CIptvServiceTest::CheckIapListL ),
        ENTRY( "SetUserName", CIptvServiceTest::SetUserNameL ),
        ENTRY( "SetPassword", CIptvServiceTest::SetPasswordL ),
        ENTRY( "SetDesc", CIptvServiceTest::SetDescL ),
        ENTRY( "SetFlags", CIptvServiceTest::SetFlagsL ),
        ENTRY( "SetApplicationUid", CIptvServiceTest::SetApplicationUidL ),
        ENTRY( "SetDisplayOrder", CIptvServiceTest::SetDisplayOrderL ),
        ENTRY( "SetProviderId", CIptvServiceTest::SetProviderIdL ),
        ENTRY( "SetAccountManagementUrl", CIptvServiceTest::SetAccountManagementUrlL ),
        ENTRY( "DoExternalize", CIptvServiceTest::DoExternalizeL),
        ENTRY( "DoInternalize", CIptvServiceTest::DoInternalizeL ),
        ENTRY( "SetL", CIptvServiceTest::SetL ),

        ENTRY( "CreateServices", CIptvServiceTest::CreateServicesL ),
        ENTRY( "DestroyServices", CIptvServiceTest::DestroyServicesL ),
        ENTRY( "ServicesAdd", CIptvServiceTest::ServicesAddL ),
        ENTRY( "ServicesCount", CIptvServiceTest::ServicesCountL ),
        ENTRY( "ServicesInternalize", CIptvServiceTest::ServicesInternalizeL ),
        ENTRY( "ServicesExternalize", CIptvServiceTest::ServicesExternalizeL ),
        ENTRY( "ServicesGetService", CIptvServiceTest::ServicesGetServiceL ),
        ENTRY( "ServicesGetServiceByRef", CIptvServiceTest::ServicesGetServiceByRefL ),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CIptvServiceTest::CreateServiceL
// CreateServiceL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::CreateServiceL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvServiceTest::CreateServiceL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In CreateServiceL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

	TInt err = KErrNone;

    TRAP( err, iService = CIptvService::NewL() );

    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::CreateServiceL: creating instance failed!");
		VCXLOGLO1("<<<CIptvServiceTest::CreateServiceL");
        }

	VCXLOGLO1("<<<CIptvServiceTest::CreateServiceL");
    return err;
    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::DestroyServiceL
// DestroyServiceL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::DestroyServiceL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvServiceTest::DestroyServiceL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In DestroyServiceL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt err = KErrNone;

	if(iService)
		{
		delete iService;
		iService = NULL;
		}

	VCXLOGLO1("<<<CIptvServiceTest::DestroyServiceL");
    return err;
    }



// -----------------------------------------------------------------------------
// CIptvServiceTest::SetIdL
// SetIdL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetIdL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetIdL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetIdL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

	TInt err = KErrNone;

// get params
	TInt id;

    if(aItem.GetNextInt(id) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::SetIdL: param id = %d", id);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetIdL: reading id parameter failed");
        iLog->Log(_L("Reading id parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetIdL");
        return KErrArgument;
        }

// do stuff
        /**
         * Sets service unique ID.
         * @since
         * @param aId unique id.
         */
        //IMPORT_C void SetId(TUint32 aId);

	iService->SetId(id);

// verify results

	if(iService->GetId() != id)
		{
        VCXLOGLO1("CIptvServiceTest::SetIdL: GetId returned wrong value");
        iLog->Log(_L("GetId returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetIdL");
        return KErrCorrupt;
		}


	VCXLOGLO1("<<<CIptvServiceTest::SetIdL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetNameL
// SetNameL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetNameL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetNameL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetNameL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TPtrC string;

	TBuf<KIptvTestMaxTextLength> param;

	if(aItem.GetNextString(string) == KErrNone)
        {

        param.Format(string);
        while(aItem.GetNextString(string) == KErrNone)
        	{
			if(param.Length() + string.Length() + 1 >= KIptvTestMaxTextLength)
				{
				VCXLOGLO1("CIptvServiceTest::SetNameL: parameter name too long, rest of the text ignored");
				break;
				}
			param.Append(KSpace);
	        param.Append(string);
        	}

        VCXLOGLO1("CIptvServiceTest::SetNameL: parameter name read");
        //IPTV_LOG_STR_DESC_HIGH_LEVEL(param);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetNameL: reading name parameter failed");
        iLog->Log(_L("Reading name parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetNameL");
        return KErrArgument;
        }

// do stuff
/**
         * Sets the service name, KIptvSmServicesDbNameMaxLength is the max length.
         * @since
         * @param aName Name of the service
         * @return System wide error code.
         */
        //IMPORT_C TInt SetName(const TDesC16& aName);

	err = iService->SetName(param);

	if(err != KErrNone)
		{
        VCXLOGLO1("CIptvServiceTest::SetNameL: SetName returned error");
        iLog->Log(_L("SetName returned error"));
        VCXLOGLO1("<<<CIptvServiceTest::SetNameL");
        return err;
		}

// verify results

	if(param.Compare( iService->GetName() ) != 0)
		{
        VCXLOGLO1("CIptvServiceTest::SetNameL: GetName returned wrong value");
        iLog->Log(_L("GetName returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetNameL");
        return KErrCorrupt;
		}

	VCXLOGLO1("<<<CIptvServiceTest::SetNameL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetAddressL
// SetAddressL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetAddressL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetAddressL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetAddressL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);


	TInt err = KErrNone;

// get params
	TPtrC string;

	TBuf<KIptvTestMaxTextLength> param;

	if(aItem.GetNextString(string) == KErrNone)
        {
        param.Format(string);
        VCXLOGLO1("CIptvServiceTest::SetAddressL: parameter address read");
        //IPTV_LOG_STR_DESC_HIGH_LEVEL(param);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetAddressL: reading address parameter failed");
        iLog->Log(_L("Reading address parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetAddressL");
        return KErrArgument;
        }

// do stuff
        /**
        * Sets the service address, KIptvSmServicesDbAddressMaxLength
        * (defined in CIptvUtil.h) is the max length in bytes.
        * @since
        * @param aAddress The address of the service
        * @return System wide error code.
        */
        //IMPORT_C TInt SetAddress(const TDesC16& aAddress);

	err = iService->SetAddress(param);
	VCXLOGLO2("CIptvServiceTest::SetAddressL: param length: %d", param.Length());

	if(err != KErrNone)
		{
        VCXLOGLO1("CIptvServiceTest::SetAddressL: SetAddress returned error");
        iLog->Log(_L("SetAddress returned error"));
        VCXLOGLO1("<<<CIptvServiceTest::SetAddressL");
        return err;
		}

// verify results

	if(param.Compare( iService->GetAddress() ) != 0)
		{
        VCXLOGLO1("CIptvServiceTest::SetAddressL: GetAddress returned wrong value");
        iLog->Log(_L("GetAddress returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetAddressL");
        return KErrCorrupt;
		}

	VCXLOGLO1("<<<CIptvServiceTest::SetAddressL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetIconPathL
// SetIconPathL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetIconPathL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetIconPathL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetIconPathL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TPtrC string;

	TBuf<KIptvTestMaxTextLength> param;

	if(aItem.GetNextString(string) == KErrNone)
        {
        param.Format(string);
        VCXLOGLO1("CIptvServiceTest::SetIconPathL: parameter path read");
        //IPTV_LOG_STR_DESC_HIGH_LEVEL(param);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetIconPathL: reading path parameter failed");
        iLog->Log(_L("Reading path parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetIconPathL");
        return KErrArgument;
        }


// do stuff
        /**
        * Sets path to icon file. KIptvSmServicesDbIconPathMaxLength is the max
        * length (in 16-bit words).
        * @since
        * @param aIconPath Path to the icon file
        * @return System wide error code.
        */
        //IMPORT_C TInt SetIconPath(const TDesC16& aIconPath);

	err = iService->SetIconPath(param);

	if(err != KErrNone)
		{
        VCXLOGLO1("CIptvServiceTest::SetIconPathL: SetIconPath returned error");
        iLog->Log(_L("SetIconPath returned error"));
        VCXLOGLO1("<<<CIptvServiceTest::SetIconPathL");
        return err;
		}

// verify results

	if(param.Compare( iService->GetIconPath() ) != 0)
		{
        VCXLOGLO1("CIptvServiceTest::SetIconPathL: GetIconPath returned wrong value");
        iLog->Log(_L("GetIconPath returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetIconPathL");
        return KErrCorrupt;
		}

	VCXLOGLO1("<<<CIptvServiceTest::SetIconPathL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetEpgPluginUidL
// SetEpgPluginUidL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetEpgPluginUidL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetEpgPluginUidL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetEpgPluginUidL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TInt uid;

    if(aItem.GetNextInt(uid) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::SetEpgPluginUidL: param uid = %d", uid);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetEpgPluginUidL: reading uid parameter failed");
        iLog->Log(_L("Reading uid parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetEpgPluginUidL");
        return KErrArgument;
        }

// do stuff
        /**
        * Sets EPG plugin UID.
        * @since
        * @param aEpgPluginUid EPG plugin UID
        * @return none
        */
        //IMPORT_C void SetEpgPluginUid(TUid aEpgPluginUid);

	TUid uuid;
	uuid.iUid = uid;
	iService->SetEpgPluginUid(uuid);

// verify results
	if(iService->GetEpgPluginUid() != uuid)
		{
        VCXLOGLO1("CIptvServiceTest::SetEpgPluginUidL: SetEpgPluginUid returned wrong value");
        iLog->Log(_L("SetEpgPluginUid returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetEpgPluginUidL");
        return KErrCorrupt;
		}

	VCXLOGLO1("<<<CIptvServiceTest::SetEpgPluginUidL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetVodPluginUidL
// SetVodPluginUidL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetVodPluginUidL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetVodPluginUidL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetVodPluginUidL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TInt uid;

    if(aItem.GetNextInt(uid) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::SetVodPluginUidL: param uid = %d", uid);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetVodPluginUidL: reading uid parameter failed");
        iLog->Log(_L("Reading uid parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetVodPluginUidL");
        return KErrArgument;
        }


// do stuff
        /**
        * Sets VOD plugin UID.
        * @since
        * @param aVodPluginUid VOD plugin UID
        * @return none
        */
        //IMPORT_C void SetVodPluginUid(TUid aVodPluginUid);


	TUid uuid;
	uuid.iUid = uid;
	iService->SetVodPluginUid(uuid);

// verify results
	if(iService->GetVodPluginUid() != uuid)
		{
        VCXLOGLO1("CIptvServiceTest::SetVodPluginUidL: GetVodPluginUid returned wrong value");
        iLog->Log(_L("GetVodPluginUid returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetVodPluginUidL");
        return KErrCorrupt;
		}


	VCXLOGLO1("<<<CIptvServiceTest::SetVodPluginUidL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetIptvPluginUidL
// SetIptvPluginUidL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetIptvPluginUidL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetIptvPluginUidL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetIptvPluginUidL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TInt uid;

    if(aItem.GetNextInt(uid) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::SetIptvPluginUidL: param uid = %d", uid);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetIptvPluginUidL: reading uid parameter failed");
        iLog->Log(_L("Reading uid parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetIptvPluginUidL");
        return KErrArgument;
        }

// do stuff
        /**
        * Sets IPTV plugin UID.
        * @since
        * @param aIptvPluginUid IPTV plugin UID
        * @return none
        */
        //IMPORT_C void SetIptvPluginUid(TUid aIptvPluginUid);


	TUid uuid;
	uuid.iUid = uid;
	iService->SetIptvPluginUid(uuid);

// verify results
	if(iService->GetIptvPluginUid() != uuid)
		{
        VCXLOGLO1("CIptvServiceTest::SetIptvPluginUidL: GetIptvPluginUid returned wrong value");
        iLog->Log(_L("GetIptvPluginUid returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetIptvPluginUidL");
        return KErrCorrupt;
		}


	VCXLOGLO1("<<<CIptvServiceTest::SetIptvPluginUidL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetTypeL
// SetTypeL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetTypeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetTypeL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetTypeL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params

	TInt param;

    if(aItem.GetNextInt(param) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::SetTypeL: param service type = %d", param);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetTypeL: reading service type parameter failed");
        iLog->Log(_L("Reading service type parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetTypeL");
        return KErrArgument;
        }

	CIptvService::TServiceType serviceType = (CIptvService::TServiceType)param;

// do stuff
        /**
        * Sets service type.
        * @since
        * @param Service type.
        */
        //IMPORT_C void SetType(TServiceType aType);

	iService->SetType(serviceType);

// verify results

	if(iService->GetType() != serviceType)
		{
        VCXLOGLO1("CIptvServiceTest::SetTypeL: GetType returned wrong value");
        iLog->Log(_L("GetType returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetTypeL");
        return KErrCorrupt;
		}

	serviceType = iService->GetType();
	if(serviceType < CIptvService::EVod || serviceType > CIptvService::EOther)
		{
        VCXLOGLO1("CIptvServiceTest::SetTypeL: GetType returned invalid type");
        iLog->Log(_L("GetType returned valid type"));
        VCXLOGLO1("<<<CIptvServiceTest::SetTypeL");
		return KErrTooBig;
		}

	VCXLOGLO1("<<<CIptvServiceTest::SetTypeL");
    return err;

    }

// -----------------------------------------------------------------------------
// CIptvServiceTest::CreateIapListL
// CreateIapListL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::CreateIapListL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::CreateIapListL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In CreateIapListL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone, err2 = KErrNone;

// get params
	TInt iapCount;

    if(aItem.GetNextInt(iapCount) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::CreateIapListL: parameter iap count = %d", iapCount);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::CreateIapListL: Reading iap count parameter failed");
        iLog->Log(_L("Reading iap count parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::CreateIapListL");
        return KErrArgument;
        }

// do stuff

	CIptvIapList* iapList;

    TRAP(err, iapList = CIptvIapList::NewL());
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::CreateIapListL: Creating instance of CIptvIapList failed!");
        iLog->Log(_L("Creating instance of CIptvIapList failed!"));
        VCXLOGLO1("<<<CIptvServiceTest::CreateIapListL");
		return KErrNoMemory;
        }

	CleanupStack::PushL(iapList);

    TUint i;
    TIptvIap iap;

    for(i = 0; i < iapCount; i++)
        {
        iap.iId = i + 1;
        iap.iPriority = i + 2;
        iap.iValueExists = EFalse;

        err = iapList->AddIap(iap, ETrue); // ignore max iap
        if(err != KErrNone)
            {
			VCXLOGLO1("CIptvServiceTest::CreateIapListL: Adding iap into list failed!");
        	iLog->Log(_L("Adding iap into list failed!"));
        	VCXLOGLO1("<<<CIptvServiceTest::CreateIapListL");
			return err;
            }
        }

	TRAP( err, err2 = iService->SetIapListL(*iapList) );
    if(err != KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::CreateIapListL: SetIapListL caused a leave (%d)!", err);
        iLog->Log(_L("SetIapListL caused a leave"));
        CleanupStack::PopAndDestroy(iapList);
        VCXLOGLO1("<<<CIptvServiceTest::CreateIapListL");
		return KErrGeneral;
        }

    if(err2 != KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::CreateIapListL: SetIapListL failed (%d)", err2);
        iLog->Log(_L("SetIapListL failed"));
        CleanupStack::PopAndDestroy(iapList);
        VCXLOGLO1("<<<CIptvServiceTest::CreateIapListL");
		return KErrGeneral;
        }

// verify results

	if( iapList->Count() != iService->GetIapList().Count() )
    	{
        VCXLOGLO1("CIptvServiceTest::CreateIapListL: Iap count of IptvService's iap list is wrong");
        iLog->Log(_L("Iap count of IptvService's iap list is wrong"));
    	CleanupStack::PopAndDestroy(iapList);
    	VCXLOGLO1("<<<CIptvServiceTest::CreateIapListL");
    	return KErrCorrupt;
    	}

	TIptvIap iap1, iap2;

	for(i=0; i<iapList->Count(); i++)
		{
        TInt err = iapList->GetIap(i, iap1);
        VCXLOGLO2("CIptvServiceTest:: GetIap: ret: %d", err);
		User::LeaveIfError(err);
		err = iService->GetIapList().GetIap(i, iap2);
		VCXLOGLO2("CIptvServiceTest:: GetIap: ret: %d", err);
		User::LeaveIfError(err);

		if(iap1.iId != iap2.iId)
			{
	    	VCXLOGLO2("CIptvServiceTest::CreateIapListL: iId of iap at index %d is wrong", i);
    	    iLog->Log(_L("iId of iap is wrong"));
    	    CleanupStack::PopAndDestroy(iapList);
    	    VCXLOGLO1("<<<CIptvServiceTest::CreateIapListL");
			return KErrCorrupt;
			}
		if(iap1.iPriority != iap2.iPriority)
			{
	    	VCXLOGLO2("CIptvServiceTest::CreateIapListL: iPriority of iap at index %d is wrong", i);
    	    iLog->Log(_L("iPriority of iap is wrong"));
			CleanupStack::PopAndDestroy(iapList);
			VCXLOGLO1("<<<CIptvServiceTest::CreateIapListL");
			return KErrCorrupt;
			}
/*
		if(iap1.iValueExists != iap2.iValueExists)
			{
	    	VCXLOGLO2("CIptvServiceTest::CreateIapListL: iValueExists of iap at index %d is wrong", i);
    	    iLog->Log(_L("iValueExists of iap is wrong"));
    	    CleanupStack::PopAndDestroy(iapList);
    	    VCXLOGLO1("<<<CIptvServiceTest::CreateIapListL");
			return KErrCorrupt;
			}			*/
		}

	CleanupStack::PopAndDestroy(iapList);


	VCXLOGLO1("<<<CIptvServiceTest::CreateIapListL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::CheckIapListL
// CheckIapListL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::CheckIapListL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::CheckIapListL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In CheckIapListL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

	TInt err = KErrNone;

// get params
	TInt exceptedIapCount;

    if(aItem.GetNextInt(exceptedIapCount) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::CheckIapListL: parameter expected iap count = %d", exceptedIapCount);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::CheckIapListL: Reading expected iap count parameter failed");
        iLog->Log(_L("Reading expected iap count parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::CheckIapListL");
        return KErrArgument;
        }

// verify results

	if( exceptedIapCount != iService->GetIapList().Count() )
    	{
        VCXLOGLO1("CIptvServiceTest::CheckIapListL: Iap count of IptvService's iap list is wrong");
        iLog->Log(_L("Iap count of IptvService's iap list is wrong"));
    	VCXLOGLO1("<<<CIptvServiceTest::CheckIapListL");
    	return KErrCorrupt;
    	}

	TIptvIap iap1, iap2;

	TInt i;

	for(i=0; i<iService->GetIapList().Count(); i++)
		{

		TInt err = iService->GetIapList().GetIap(i, iap2);
		VCXLOGLO2("CIptvServiceTest:: GetIap ret: %d", err);
		User::LeaveIfError(err);

  		iap1.iId = i + 1;
        iap1.iPriority = i + 2;
        iap1.iValueExists = ETrue;

		if(iap1.iId != iap2.iId)
			{
	    	VCXLOGLO2("CIptvServiceTest::CheckIapListL: iId of iap at index %d is wrong", i);
    	    iLog->Log(_L("iId of iap is wrong"));
    	    VCXLOGLO1("<<<CIptvServiceTest::CheckIapListL");
			return KErrCorrupt;
			}
		if(iap1.iPriority != iap2.iPriority)
			{
	    	VCXLOGLO2("CIptvServiceTest::CheckIapListL: iPriority of iap at index %d is wrong", i);
    	    iLog->Log(_L("iPriority of iap is wrong"));
			VCXLOGLO1("<<<CIptvServiceTest::CheckIapListL");
			return KErrCorrupt;
			}
/*
		if(iap1.iValueExists != iap2.iValueExists)
			{
	    	VCXLOGLO2("CIptvServiceTest::CheckIapListL: iValueExists of iap at index %d is wrong", i);
    	    iLog->Log(_L("iValueExists of iap is wrong"));
    	    VCXLOGLO1("<<<CIptvServiceTest::CheckIapListL");
			return KErrCorrupt;
			}			*/
		}


	VCXLOGLO1("<<<CIptvServiceTest::CheckIapListL");
    return err;

    }



// -----------------------------------------------------------------------------
// CIptvServiceTest::SetUserNameL
// SetUserNameL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetUserNameL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetUserNameL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetUserNameL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TPtrC string;

	TBuf<KIptvTestMaxTextLength> param;

	if(aItem.GetNextString(string) == KErrNone)
        {

        param.Format(string);
        while(aItem.GetNextString(string) == KErrNone)
        	{
			if(param.Length() + string.Length() + 1 >= KIptvTestMaxTextLength)
				{
				VCXLOGLO1("CIptvServiceTest::SetUserNameL: parameter user name too long, rest of the text ignored");
				break;
				}
			param.Append(KSpace);
	        param.Append(string);
        	}

        VCXLOGLO1("CIptvServiceTest::SetUserNameL: parameter user name read");
        //IPTV_LOG_STR_DESC_HIGH_LEVEL(param);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetUserNameL: reading user name parameter failed");
        iLog->Log(_L("Reading user name parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetUserNameL");
        return KErrArgument;
        }

// do stuff
        /**
         * Gets user name.
         * @since
         * @return TDes16&, user name
         */
        //IMPORT_C TDes16& GetUserName();

	err = iService->SetUserName(param);

	if(err != KErrNone)
		{
        VCXLOGLO1("CIptvServiceTest::SetUserNameL: SetUserName returned error");
        iLog->Log(_L("SetUserName returned error"));
        VCXLOGLO1("<<<CIptvServiceTest::SetUserNameL");
        return err;
		}

// verify results

	if(param.Compare( iService->GetUserName() ) != 0)
		{
        VCXLOGLO1("CIptvServiceTest::SetUserNameL: GetUserName returned wrong value");
        iLog->Log(_L("GetUserName returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetUserNameL");
        return KErrCorrupt;
		}


	VCXLOGLO1("<<<CIptvServiceTest::SetUserNameL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetPasswordL
// SetPasswordL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetPasswordL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetPasswordL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetPasswordL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TPtrC string;

	TBuf<KIptvTestMaxTextLength> param;

	if(aItem.GetNextString(string) == KErrNone)
        {
        param.Format(string);

        VCXLOGLO1("CIptvServiceTest::SetPasswordL: parameter password read");
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetPasswordL: reading password parameter failed");
        iLog->Log(_L("Reading password parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetPasswordL");
        return KErrArgument;
        }


// do stuff
        /**
         * Sets password, KIptvSmPasswordsDbPasswordMaxLength is the max length (in 16-bit words,
         * defined in CIptvUtil.h).
         * @since
         * @param aPassword Password, used for authentication.
         * @return System wide error code.
         */
        //IMPORT_C TInt SetPassword(const TDesC16& aPassword);

	err = iService->SetPassword(param);

	if(err != KErrNone)
		{
        VCXLOGLO1("CIptvServiceTest::SetPasswordL: SetPassword returned error");
        iLog->Log(_L("SetPassword returned error"));
        VCXLOGLO1("<<<CIptvServiceTest::SetPasswordL");
        return err;
		}

// verify results

	if(param.Compare( iService->GetPassword() ) != 0)
		{
        VCXLOGLO1("CIptvServiceTest::SetPasswordL: GetPassword returned wrong value");
        iLog->Log(_L("GetPassword returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetPasswordL");
        return KErrCorrupt;
		}

	VCXLOGLO1("<<<CIptvServiceTest::SetPasswordL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetDescL
// SetDescL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetDescL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetDescL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetDescL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TPtrC string;

	TBuf<KIptvTestMaxTextLength> param;

	if(aItem.GetNextString(string) == KErrNone)
        {

        param.Format(string);
        while(aItem.GetNextString(string) == KErrNone)
        	{
			if(param.Length() + string.Length() + 1 >= KIptvTestMaxTextLength)
				{
				VCXLOGLO1("CIptvServiceTest::SetDescL: parameter desc too long, rest of the text ignored");
				break;
				}
			param.Append(KSpace);
	        param.Append(string);
        	}

        VCXLOGLO1("CIptvServiceTest::SetDescL: parameter desc read");
        //IPTV_LOG_STR_DESC_HIGH_LEVEL(param);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetDescL: reading desc parameter failed");
        iLog->Log(_L("Reading desc parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetDescL");
        return KErrArgument;
        }


// do stuff
        /**
         * Sets description of the service.
         * KIptvSmServicesDbDescMaxLength is the max length (in 16-bit words).
         * @since
         * @param aDesc Description.
         * @return System wide error code.
         */
        //IMPORT_C TInt SetDesc(const TDesC16& aDesc);
	err = iService->SetDesc(param);

	if(err != KErrNone)
		{
        VCXLOGLO1("CIptvServiceTest::SetDescL: SetDesc returned error");
        iLog->Log(_L("SetDesc returned error"));
        VCXLOGLO1("<<<CIptvServiceTest::SetDescL");
        return err;
		}

// verify results

	if(param.Compare( iService->GetDesc() ) != 0)
		{
        VCXLOGLO1("CIptvServiceTest::SetDescL: GetDesc returned wrong value");
        iLog->Log(_L("GetDesc returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetDescL");
        return KErrCorrupt;
		}

	VCXLOGLO1("<<<CIptvServiceTest::SetDescL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetFlagsL
// SetFlagsL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetFlagsL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetFlagsL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetFlagsL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TUint32 flags = 0;

	TInt mask;

    while(aItem.GetNextInt(mask) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::SetIdL: param flag mask = %d", mask);

    	flags |= mask;
        }

// do stuff
        /**
         * Sets service flags, see TFlags for bit values.
         * @param aFlags 32 bits of flags
         * @return None.
         */
        //IMPORT_C void SetFlags(TUint32 aFlags);

	iService->SetFlags(flags);

// verify results

	if( flags != iService->GetFlags() )
		{
        VCXLOGLO1("CIptvServiceTest::SetFlagsL: GetFlags returned wrong value");
        iLog->Log(_L("GetFlags returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetFlagsL");
        return KErrArgument;
		}

	VCXLOGLO1("<<<CIptvServiceTest::SetFlagsL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetApplicationUidL
// SetApplicationUidL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetApplicationUidL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetApplicationUidL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetApplicationUidL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TInt uid;

    if(aItem.GetNextInt(uid) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::SetApplicationUidL: param uid = %d", uid);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetApplicationUidL: reading uid parameter failed");
        iLog->Log(_L("Reading uid parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetApplicationUidL");
        return KErrArgument;
        }

// do stuff
        /**
         * Sets application UID value.
         * @param aUid
         * @return none.
         */
        //IMPORT_C void SetApplicationUid(TInt32 aUid);


	iService->SetApplicationUid(uid);

// verify results
	if(iService->GetApplicationUid() != uid)
		{
        VCXLOGLO1("CIptvServiceTest::SetApplicationUidL: GetApplicationUid returned wrong value");
        iLog->Log(_L("GetApplicationUid returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetApplicationUidL");
        return KErrCorrupt;
		}


	VCXLOGLO1("<<<CIptvServiceTest::SetApplicationUidL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetDisplayOrderL
// SetDisplayOrderL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetDisplayOrderL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetDisplayOrderL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetDisplayOrderL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params

	TInt displayOrder;

    if(aItem.GetNextInt(displayOrder) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::SetDisplayOrderL: param displayOrder = %d", displayOrder);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetDisplayOrderL: reading displayOrder parameter failed");
        iLog->Log(_L("Reading displayOrder parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetDisplayOrderL");
        return KErrArgument;
        }

// do stuff
        /**
         * Sets display order.
         * @return none.
         */
        //IMPORT_C void SetDisplayOrder(TUint32 aOrder);


	iService->SetDisplayOrder(displayOrder);

// verify results
	if(iService->GetDisplayOrder() != displayOrder)
		{
        VCXLOGLO1("CIptvServiceTest::SetDisplayOrderL: GetDisplayOrder returned wrong value");
        iLog->Log(_L("GetDisplayOrder returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetDisplayOrderL");
        return KErrCorrupt;
		}


	VCXLOGLO1("<<<CIptvServiceTest::SetDisplayOrderL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetProviderIdL
// SetProviderIdL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetProviderIdL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetProviderIdL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetProviderIdL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TPtrC string;

	TBuf<KIptvTestMaxTextLength> param;

	if(aItem.GetNextString(string) == KErrNone)
        {
        param.Format(string);
        VCXLOGLO1("CIptvServiceTest::SetProviderIdL: parameter provider id read");
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetProviderIdL: reading provider id parameter failed");
        iLog->Log(_L("Reading provider id parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetProviderIdL");
        return KErrArgument;
        }

// do stuff
        /**
         * Sets provider ID.
         * KIptvSmServicesDbProviderIdMaxLength is the maximum length.
         * @param aProviderId
         * @return System wide error code.
         */
        //IMPORT_C TInt SetProviderId(const TDesC& aProviderId);

	err = iService->SetProviderId(param);

	if(err != KErrNone)
		{
        VCXLOGLO1("CIptvServiceTest::SetProviderIdL: SetProviderId returned error");
        iLog->Log(_L("SetProviderId returned error"));
        VCXLOGLO1("<<<CIptvServiceTest::SetProviderIdL");
        return err;
		}

// verify results

	if(param.Compare( iService->GetProviderId() ) != 0)
		{
        VCXLOGLO1("CIptvServiceTest::SetProviderIdL: GetProviderId returned wrong value");
        iLog->Log(_L("GetProviderId returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetProviderIdL");
        return KErrCorrupt;
		}

	VCXLOGLO1("<<<CIptvServiceTest::SetProviderIdL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::SetAccountManagementUrlL
// SetAccountManagementUrlL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetAccountManagementUrlL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetAccountManagementUrlL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetAccountManagementUrlL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TPtrC string;

	TBuf<KIptvTestMaxTextLength> param;

	if(aItem.GetNextString(string) == KErrNone)
        {
        param.Format(string);
        VCXLOGLO1("CIptvServiceTest::SetAccountManagementUrlL: parameter url read");
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::SetAccountManagementUrlL: reading url parameter failed");
        iLog->Log(_L("Reading url parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::SetAccountManagementUrlL");
        return KErrArgument;
        }

// do stuff
        /**
         * Sets account management url.
         * KIptvSmServicesDbAccountManagementUrlMaxLength is the maximum length.
         * @param aAccountManagementUrl
         * @return System wide error code.
         */
        //IMPORT_C TInt SetAccountManagementUrl(const TDesC& aAccountManagementUrl);

	err = iService->SetAccountManagementUrl(param);

	if(err != KErrNone)
		{
        VCXLOGLO1("CIptvServiceTest::SetAccountManagementUrlL: SetAccountManagementUrl returned error");
        iLog->Log(_L("SetAccountManagementUrl returned error"));
        VCXLOGLO1("<<<CIptvServiceTest::SetAccountManagementUrlL");
        return err;
		}

// verify results

	if(param.Compare( iService->GetAccountManagementUrl() ) != 0)
		{
        VCXLOGLO1("CIptvServiceTest::SetAccountManagementUrlL: GetAccountManagementUrl returned wrong value");
        iLog->Log(_L("GetAccountManagementUrl returned wrong value"));
        VCXLOGLO1("<<<CIptvServiceTest::SetAccountManagementUrlL");
        return KErrCorrupt;
		}

	VCXLOGLO1("<<<CIptvServiceTest::SetAccountManagementUrlL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::DoExternalizeL
// DoExternalizeL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::DoExternalizeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::DoExternalizeL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In DoExternalizeL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params

	TInt useTooSmallBuffer = 0;

    aItem.GetNextInt(useTooSmallBuffer);

// do stuff

        /**
         * Write internal data to binary format which can be sent over IPC.
         * @param aStream The Stream where data is written.
         */
        //IMPORT_C void ExternalizeL(RDesWriteStream& aStream) const;

	// setup buffer and externalize
	TUint32 dataSize = iService->CountExternalizeSize();

    if(useTooSmallBuffer)
    	{
    		dataSize = 1;
    	}

    if(iBuff != NULL)
    	{
    	delete iBuff;
    	iBuff = NULL;
    	}

	TRAP( err, iBuff = HBufC8::NewL(dataSize) );
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::DoExternalizeL: Creating instance of HBufC8 failed!");
        iLog->Log(_L("Creating instance of HBufC8 failed!"));
        VCXLOGLO1("<<<CIptvServiceTest::DoExternalizeL");
		return KErrNoMemory;
        }

	TPtr8 buffPtr(iBuff->Des());

	RDesWriteStream writeStream;
	writeStream.Open( buffPtr );
	TRAP( err, iService->ExternalizeL(writeStream) );
	if(err == KErrNone)
		{
		TRAP( err, writeStream.CommitL() );
		}
	writeStream.Close();
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::DoExternalizeL: ExternalizeL failed!");
        iLog->Log(_L("ExternalizeL failed!"));
        delete iBuff;
        iBuff = NULL;
        VCXLOGLO1("<<<CIptvServiceTest::DoExternalizeL");
		return KErrGeneral;
        }

	VCXLOGLO1("<<<CIptvServiceTest::DoExternalizeL");
    return err;

    }



// -----------------------------------------------------------------------------
// CIptvServiceTest::DoInternalizeL
// DoInternalizeL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::DoInternalizeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::DoInternalizeL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In DoInternalizeL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params

	TInt useTooSmallBuffer = 0;

    aItem.GetNextInt(useTooSmallBuffer);

// do stuff

        /**
         * Write internal data to binary format which can be sent over IPC.
         * @param aStream The Stream where data is written.
         */
        //IMPORT_C void ExternalizeL(RDesWriteStream& aStream) const;


	// if we have no buffer then create minimum size buffer to test error handling
	if(useTooSmallBuffer == 1 || iBuff == NULL)
		{
		if(iBuff)
			{
			delete iBuff;
			iBuff = NULL;
			}

		VCXLOGLO1("CIptvServiceTest::DoInternalizeL: Creating too small buffer to test InternalizeL error handling");
		TRAP( err, iBuff = HBufC8::NewL(1) );
	    if(err != KErrNone)
	        {
	        VCXLOGLO1("CIptvServiceTest::DoInternalizeL: Creating instance of HBufC8 failed!");
	        iLog->Log(_L("Creating instance of HBufC8 failed!"));
	        VCXLOGLO1("<<<CIptvServiceTest::DoInternalizeL");
			return KErrNoMemory;
	        }
		}

	TPtr8 buffPtr(iBuff->Des());

//  setup new CIptvService
	CIptvService* service;

    TRAP(err, service = CIptvService::NewL());
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::DoInternalizeL: Creating instance of CIptvService failed!");
        iLog->Log(_L("Creating instance of CIptvService failed!"));
        VCXLOGLO1("<<<CIptvServiceTest::DoInternalizeL");
		return KErrNoMemory;
        }

  	CleanupStack::PushL(service);

	// internalize the data

	RDesReadStream readStream;
	readStream.Open( buffPtr );
	TRAP( err, service->InternalizeL(readStream) );
    readStream.Close();
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::DoInternalizeL: InternalizeL failed!");
        iLog->Log(_L("InternalizeL failed!"));
        CleanupStack::PopAndDestroy(service);
        VCXLOGLO1("<<<CIptvServiceTest::DoInternalizeL");
		return KErrGeneral;
        }

       /**
         * Read data stream and construct this class from it.
         * @param aStream Stream to read data from.
         */
        //IMPORT_C void InternalizeL(RDesReadStream& aStream);

// verify results

	TBufC<512> errorMsg (_L("CIptvServiceTest::DoInternalizeL: New externalized service ") );

	err = CompareService(errorMsg.Des(), *iService, *service);

	if(err != KErrNone)
		{
			IPTV_LOGTEXT(errorMsg.Des());
			iLog->Log(errorMsg.Des());
			CleanupStack::PopAndDestroy(service);
	    	VCXLOGLO1("<<<CIptvServiceTest::DoInternalizeL");
			return KErrCorrupt;
		}

	CleanupStack::PopAndDestroy(service);

	delete iBuff;
	iBuff = NULL;

	VCXLOGLO1("<<<CIptvServiceTest::DoInternalizeL");
    return err;

    }




// -----------------------------------------------------------------------------
// CIptvServiceTest::SetL
// SetL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::SetL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::SetL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In SetL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

	TInt err = KErrNone;

// get params

	TInt params = 0;

    aItem.GetNextInt(params);

// do stuff

        /**
         * Write internal data to binary format which can be sent over IPC.
         * @param aStream The Stream where data is written.
         */
        //IMPORT_C void ExternalizeL(RDesWriteStream& aStream) const;


	CIptvService* service;

	//  setup new CIptvService

    TRAP(err, service = CIptvService::NewL());
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::SetL: Creating instance of CIptvService failed!");
        iLog->Log(_L("Creating instance of CIptvService failed!"));
        VCXLOGLO1("<<<CIptvServiceTest::SetL");
		return KErrNoMemory;
        }

	CleanupStack::PushL(service);

	switch(params)
		{
		default:
			{
			// setup buffer and externalize
			TUint32 dataSize = iService->CountExternalizeSize();

		    HBufC8* buff;

			TRAP( err, buff = HBufC8::NewL(dataSize) );
		    if(err != KErrNone)
		        {
		        VCXLOGLO1("CIptvServiceTest::SetL: Creating instance of HBufC8 failed!");
		        iLog->Log(_L("Creating instance of HBufC8 failed!"));
		        CleanupStack::PopAndDestroy(service);
		        VCXLOGLO1("<<<CIptvServiceTest::SetL");
				return KErrNoMemory;
		        }

			CleanupStack::PushL(buff);

			TPtr8 buffPtr(buff->Des());

			RDesWriteStream writeStream;
			writeStream.Open( buffPtr );
			TRAP( err, iService->ExternalizeL(writeStream) );
			if(err != KErrNone)
				{
				TRAP( err, writeStream.CommitL() );
				}
			writeStream.Close();
		    if(err != KErrNone)
		        {
		        VCXLOGLO1("CIptvServiceTest::SetL: ExternalizeL failed!");
		        iLog->Log(_L("ExternalizeL failed!"));
		        CleanupStack::PopAndDestroy(buff);
		        CleanupStack::PopAndDestroy(service);
		        VCXLOGLO1("<<<CIptvServiceTest::SetL");
				return KErrGeneral;
		        }

			TInt err2;
			TRAP(err2, err = service->SetL( buffPtr ) );

			if(err2 != KErrNone || err != KErrNone)
		        {
		        VCXLOGLO1("CIptvServiceTest::SetL: SetL failed");
		        iLog->Log(_L("SetL failed"));
		        CleanupStack::PopAndDestroy(buff);
		        CleanupStack::PopAndDestroy(service);
		        VCXLOGLO1("<<<CIptvServiceTest::SetL");
				return KErrGeneral;
		        }

			CleanupStack::PopAndDestroy(buff);
			}
			break;

		case 1: // setl with invalid buffer
			{
			TUint32 dataSize = iService->CountExternalizeSize();

			HBufC8* buff;

			TRAP( err, buff = HBufC8::NewL(dataSize) );
		    if(err != KErrNone)
		        {
		        VCXLOGLO1("CIptvServiceTest::SetL: Creating instance of HBufC8 failed!");
		        iLog->Log(_L("Creating instance of HBufC8 failed!"));
		        CleanupStack::PopAndDestroy(service);
		        VCXLOGLO1("<<<CIptvServiceTest::SetL");
				return KErrNoMemory;
		        }

			CleanupStack::PushL(buff);

			TPtr8 buffPtr(buff->Des());

			TInt err2;
			TRAP(err2, err = service->SetL( buffPtr ) );

			if(err2 != KErrNone || err != KErrNone)
		        {
		        VCXLOGLO1("CIptvServiceTest::SetL: SetL failed!");
		        iLog->Log(_L("SetL failed!"));
		        CleanupStack::PopAndDestroy(buff);
		        CleanupStack::PopAndDestroy(service);
		        VCXLOGLO1("<<<CIptvServiceTest::SetL");
				return KErrGeneral;
		        }

			CleanupStack::PopAndDestroy(buff);

			}
			break;
		}


    if(service == NULL)
        {
        VCXLOGLO1("CIptvServiceTest::SetL: Service is NULL");
        iLog->Log(_L("Service is NULL"));
        VCXLOGLO1("<<<CIptvServiceTest::SetL");
		return KErrUnknown;
        }


	TBufC<512> errorMsg (_L("CIptvServiceTest::SetL: New service ") );

	err = CompareService(errorMsg.Des(), *iService, *service);

	if(err != KErrNone)
		{
			IPTV_LOGTEXT(errorMsg.Des());
			iLog->Log(errorMsg.Des());
			CleanupStack::PopAndDestroy(service);
	    	VCXLOGLO1("<<<CIptvServiceTest::SetL");
			return KErrCorrupt;
		}


	CleanupStack::PopAndDestroy(service);

	VCXLOGLO1("<<<CIptvServiceTest::SetL");
    return err;

    }



// -----------------------------------------------------------------------------
// CompareService::CompareService
// CompareService helper method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::CompareService( TPtr aErrPtr, CIptvService& aOriginalService, CIptvService& aNewService )
	{

	 VCXLOGLO1(">>>CIptvServiceTest::CompareServiceL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In CompareServiceL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

	// verify member data

	if(aOriginalService.GetId() != aNewService.GetId() )
		{
	  	aErrPtr.Append( _L("service has wrong id") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}

	if(aOriginalService.GetEpgPluginUid() != aNewService.GetEpgPluginUid() )
		{
	  	aErrPtr.Append( _L("service has wrong epg plugin uid") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}


	if(aOriginalService.GetVodPluginUid() != aNewService.GetVodPluginUid() )
		{
	  	aErrPtr.Append( _L("service has wrong vod plugin uid") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}


	if(aOriginalService.GetIptvPluginUid() != aNewService.GetIptvPluginUid() )
		{
	  	aErrPtr.Append( _L("service has wrong iptv plugin uid") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}


	if(aOriginalService.GetType() != aNewService.GetType() )
		{
	  	aErrPtr.Append( _L("service has wrong type") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}


	if(aOriginalService.GetFlags() != aNewService.GetFlags() )
		{
	  	aErrPtr.Append( _L("service has wrong flags") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}


	if(aOriginalService.GetApplicationUid() != aNewService.GetApplicationUid() )
		{
	  	aErrPtr.Append( _L("service has wrong application uid") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}


	if(aOriginalService.GetDisplayOrder() != aNewService.GetDisplayOrder() )
		{
	  	aErrPtr.Append( _L("service has wrong display order") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}


	if(aOriginalService.GetProviderId() != aNewService.GetProviderId() )
		{
	  	aErrPtr.Append( _L("service has wrong provider id") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}

	if(aOriginalService.GetName().Compare( aNewService.GetName() ) != 0)
		{
		aErrPtr.Append( _L("service has wrong name") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}

	if(aOriginalService.GetAddress().Compare( aNewService.GetAddress() ) != 0)
		{
		aErrPtr.Append( _L("service has wrong address") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
   		return KErrCorrupt;
		}

	if(aOriginalService.GetIconPath().Compare( aNewService.GetIconPath() ) != 0)
		{
		aErrPtr.Append( _L("service has wrong icon path") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
		return KErrCorrupt;
		}

	if(aOriginalService.GetUserName().Compare( aNewService.GetUserName() ) != 0)
		{
		aErrPtr.Append( _L("service has wrong user name") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
   		return KErrCorrupt;
		}

	if(aOriginalService.GetPassword().Compare( aNewService.GetPassword() ) != 0)
		{
		aErrPtr.Append( _L("service has wrong password") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
   		return KErrCorrupt;
		}

	if(aOriginalService.GetDesc().Compare( aNewService.GetDesc() ) != 0)
		{
		aErrPtr.Append( _L("service has wrong desc") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
   		return KErrCorrupt;
		}

	if(aOriginalService.GetProviderId().Compare( aNewService.GetProviderId() ) != 0)
		{
		aErrPtr.Append( _L("service has wrong provider id") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
   		return KErrCorrupt;
		}

	if(aOriginalService.GetAccountManagementUrl().Compare( aNewService.GetAccountManagementUrl() ) != 0)
		{
		aErrPtr.Append( _L("service has wrong account managment url") );
	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
   		return KErrCorrupt;
		}


	// verify iaplist

	TIptvIap iap1, iap2;

	TInt i;

	for(i=0; i<aOriginalService.GetIapList().Count(); i++)
		{

		TInt err  = aOriginalService.GetIapList().GetIap(i, iap1);
		VCXLOGLO2("CIptvServiceTest:: GetIap ret: %d", err);
		User::LeaveIfError(err);
		err = aNewService.GetIapList().GetIap(i, iap2);
		VCXLOGLO2("CIptvServiceTest:: GetIap ret: %d", err);
		User::LeaveIfError(err);

		if(iap1.iId != iap2.iId)
			{
	    	aErrPtr.Append( _L("Service's iap list has wrong iap.iId at index: ") );
	    	aErrPtr.AppendNum(i);
    	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
			return KErrCorrupt;
			}
		if(iap1.iPriority != iap2.iPriority)
			{
	    	aErrPtr.Append( _L("Service's iap list has wrong iap.iPriority at index: ") );
	    	aErrPtr.AppendNum(i);
			VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
			return KErrCorrupt;
			}
/*
		if(iap1.iValueExists != iap2.iValueExists)
			{
	    	aErrPtr.Append( _L("service's iap list has wrong iap.iValueExists at index %d", i);
    	    iLog->Log(_L("New externalized service's iap list has wrong iap.iValueExists"));
			CleanupStack::PopAndDestroy(service);
			CleanupStack::PopAndDestroy(buff);
    	    VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");
			return KErrCorrupt;
			}			*/
		}

	VCXLOGLO1("<<<CIptvServiceTest::CompareServiceL");

	return KErrNone;
	}



// -----------------------------------------------------------------------------
// CIptvServiceTest::CreateServicesL
// CreateServicesL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::CreateServicesL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvServiceTest::CreateServicesL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In CreateServicesL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

	TInt err = KErrNone;

    TRAP( err, iServices = CIptvServices::NewL() );

    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::CreateServicesL: creating instance failed!");
        }

	VCXLOGLO1("<<<CIptvServiceTest::CreateServicesL");
    return err;
    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::DestroyServicesL
// DestroyServicesL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::DestroyServicesL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvServiceTest::DestroyServicesL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In DestroyServicesL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt err = KErrNone;

	if(iServices)
		{
		delete iServices;
		iServices = NULL;
		}

	iServicesArray.ResetAndDestroy();

	VCXLOGLO1("<<<CIptvServiceTest::DestroyServicesL");
    return err;
    }



// -----------------------------------------------------------------------------
// CIptvServiceTest::ServicesAddL
// ServicesAddL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::ServicesAddL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvServiceTest::ServicesAddL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In ServicesAddL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

	TInt err = KErrNone;

	CIptvService* copyService;
	TRAP( err, copyService = CIptvService::NewL() );
	if(err != KErrNone)
		{
		VCXLOGLO1("CIptvServiceTest::ServicesAddL: Creating instance of CIptvService failed.");
	    iLog->Log(_L("Creating instance of CIptvService failed."));
	    VCXLOGLO1("<<<CIptvServiceTest::ServicesAddL");
		return KErrNoMemory;
		}

	CleanupStack::PushL(copyService);
	HBufC8* buff;
	TRAP( err, buff = iService->GetL() );
	if(err != KErrNone)
		{
		VCXLOGLO1("CIptvServiceTest::ServicesAddL: GetL failed.");
	    iLog->Log(_L("GetL failed."));
	    CleanupStack::PopAndDestroy(copyService);
	    VCXLOGLO1("<<<CIptvServiceTest::ServicesAddL");
		return KErrGeneral;
		}
	CleanupStack::PushL(buff);
	TRAP( err, copyService->SetL( buff->Des() ) );
	if(err != KErrNone)
		{
		VCXLOGLO1("CIptvServiceTest::ServicesAddL: SetL failed.");
	    iLog->Log(_L("SetL failed."));
	    CleanupStack::PopAndDestroy(buff);
	    CleanupStack::PopAndDestroy(copyService);
	    VCXLOGLO1("<<<CIptvServiceTest::ServicesAddL");
		return KErrGeneral;
		}

	err = iServicesArray.Append(copyService);

	CleanupStack::PopAndDestroy(buff);
	CleanupStack::Pop(copyService);
	copyService = NULL;

	if(err != KErrNone)
		{
		VCXLOGLO1("CIptvServiceTest::ServicesAddL: Adding service to array failed.");
	    iLog->Log(_L("Adding service to array failed."));
	    VCXLOGLO1("<<<CIptvServiceTest::ServicesAddL");
		return err;
		}

	// do stuff
	TInt serviceCount = iServices->Count();

	TInt err2 = KErrNone;
	TRAP(err2, err = iServices->AddL(*iService));


	if(err != KErrNone)
		{
		VCXLOGLO1("CIptvServiceTest::ServicesAddL: CIptvServices::AddL returned error.");
	    iLog->Log(_L("CIptvServices returned error."));
	    VCXLOGLO1("<<<CIptvServiceTest::ServicesAddL");
		return err;
		}

	if(err2 != KErrNone)
		{
		VCXLOGLO1("CIptvServiceTest::ServicesAddL: CIptvServices::AddL caused a leave.");
	    iLog->Log(_L("CIptvServices caused a leave."));
	    VCXLOGLO1("<<<CIptvServiceTest::ServicesAddL");
		return err;
		}

// verify results

	if( serviceCount >= iServices->Count() )
		{
		VCXLOGLO1("CIptvServiceTest::ServicesAddL: CIptvServices::Count of services did not incerement.");
	    iLog->Log(_L("CIptvServices::Count of services did not incerement."));
	    VCXLOGLO1("<<<CIptvServiceTest::ServicesAddL");
		return KErrGeneral;
		}

	CIptvService* service;
	TRAP( err, service = iServices->GetServiceL(serviceCount) );
	if(err != KErrNone)
		{
		VCXLOGLO1("CIptvServiceTest::ServicesAddL: CIptvServices::GetServiceL caused a leave.");
	    iLog->Log(_L("CIptvServices::GetServiceL caused a leave."));
	    VCXLOGLO1("<<<CIptvServiceTest::ServicesAddL");
		return err;
		}

	CleanupStack::PushL(service);

	TBufC<512> errorMsg(_L("CIptvServiceTest::ServicesAddL: Service was added in the list and now "));

	err = CompareService(errorMsg.Des(), *iService, *service);

	if(err != KErrNone)
		{
		IPTV_LOGTEXT(errorMsg.Des());
		iLog->Log(errorMsg.Des());
		CleanupStack::PopAndDestroy(service);
    	VCXLOGLO1("<<<CIptvServiceTest::DoInternalizeL");
		return err;
    	}

	CleanupStack::PopAndDestroy(service);


	VCXLOGLO1("<<<CIptvServiceTest::ServicesAddL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::ServicesCountL
// ServicesCountL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::ServicesCountL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::ServicesCountL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In ServicesCountL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TInt expectedCount = 0;

    if(aItem.GetNextInt(expectedCount) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::ServicesCountL: param expected count = %d", expectedCount);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::ServicesCountL: reading expected count parameter failed");
        iLog->Log(_L("Reading expected count parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::ServicesCountL");
        return KErrArgument;
        }

// do stuff
	if(iServices->Count() != expectedCount)
		{
		VCXLOGLO1("CIptvServiceTest::ServicesCountL: Instance of CIptvServices::Count didn't return the expected value.");
	    iLog->Log(_L(" Instance of CIptvServices::Count didn't return the expected value."));
	    VCXLOGLO1("<<<CIptvServiceTest::ServicesCountL");
		return err;
		}

// verify results


	VCXLOGLO1("<<<CIptvServiceTest::ServicesCountL");
    return err;

    }



// -----------------------------------------------------------------------------
// CIptvServiceTest::ServicesInternalizeL
// ServicesInternalizeL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::ServicesInternalizeL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvServiceTest::ServicesInternalizeL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In ServicesInternalizeL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

	if(iServicesBuff == NULL)
	{
        VCXLOGLO1("CIptvServiceTest::ServicesInternalizeL: ServicesExternalizeL must be called first");
        iLog->Log(_L("ServicesExternalizeL must be called first"));
        VCXLOGLO1("<<<CIptvServiceTest::ServicesInternalizeL");
        return KErrArgument;
	}

	TPtr8 buffPtr(iServicesBuff->Des());

//  setup new CIptvService
	CIptvServices* services;

    TRAP(err, services = CIptvServices::NewL());
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::ServicesInternalizeL: Creating instance of CIptvServices failed!");
        iLog->Log(_L("Creating instance of CIptvServices failed!"));
        VCXLOGLO1("<<<CIptvServiceTest::ServicesInternalizeL");
		return KErrNoMemory;
        }

  	CleanupStack::PushL(services);

	// internalize the data

	RDesReadStream readStream;
	readStream.Open( buffPtr );
	TRAP( err, services->InternalizeL(readStream) );
    readStream.Close();
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::ServicesInternalizeL: InternalizeL failed!");
        iLog->Log(_L("InternalizeL failed!"));
        CleanupStack::PopAndDestroy(services);
        VCXLOGLO1("<<<CIptvServiceTest::ServicesInternalizeL");
		return KErrGeneral;
        }

// verify results

	TBufC<512> errorMsg(_L("CIptvServiceTest::ServicesInternalizeL: Service was internalized and now "));


	//err = CompareService(errorMsg.Des(), *iService, *services);

	if(err != KErrNone)
		{
		IPTV_LOGTEXT(errorMsg.Des());
		iLog->Log(errorMsg.Des());
		CleanupStack::PopAndDestroy(services);
    	VCXLOGLO1("<<<CIptvServiceTest::ServicesInternalizeL");
		return err;
    	}

	delete iServicesBuff;
	iServicesBuff = NULL;

	CleanupStack::PopAndDestroy(services);

	VCXLOGLO1("<<<CIptvServiceTest::ServicesInternalizeL");
    return err;

    }



// -----------------------------------------------------------------------------
// CIptvServiceTest::ServicesExternalizeL
// ServicesExternalizeL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::ServicesExternalizeL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvServiceTest::ServicesExternalizeL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In ServicesExternalizeL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

	TInt err = KErrNone;

	TUint32 dataSize = iServices->CountExternalizeSize();

    if(iServicesBuff != NULL)
    	{
    	delete iServicesBuff;
    	iServicesBuff = NULL;
    	}

	TRAP( err, iServicesBuff = HBufC8::NewL(dataSize) );
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::ServicesExternalizeL: Creating instance of HBufC8 failed!");
        iLog->Log(_L("Creating instance of HBufC8 failed!"));
        VCXLOGLO1("<<<CIptvServiceTest::ServicesExternalizeL");
		return KErrNoMemory;
        }

	TPtr8 buffPtr(iServicesBuff->Des());

	RDesWriteStream writeStream;
	writeStream.Open( buffPtr );
	TRAP( err, iServices->ExternalizeL(writeStream) );
	if(err != KErrNone)
		{
		TRAP( err, writeStream.CommitL() );
		}
	writeStream.Close();
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::ServicesExternalizeL: ExternalizeL failed!");
        iLog->Log(_L("ExternalizeL failed!"));
        delete iServicesBuff;
        iServicesBuff = NULL;
        VCXLOGLO1("<<<CIptvServiceTest::ServicesExternalizeL");
		return KErrGeneral;
        }

	VCXLOGLO1("<<<CIptvServiceTest::ServicesExternalizeL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::ServicesGetServiceL
// ServicesGetServiceL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::ServicesGetServiceL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::ServicesGetServiceL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In ServicesGetServiceL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params
	TInt index;

 	if(aItem.GetNextInt(index) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::ServicesGetServiceL: param index = %d", index);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::ServicesGetServiceL: reading index parameter failed");
        iLog->Log(_L("Reading index parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::ServicesGetServiceL");
        return KErrArgument;
        }

// do stuff
	CIptvService* service = NULL;

	TRAP(err, service = iServices->GetServiceL(index) );
  	if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceTest::ServicesGetServiceL: CIptvServices::GetServiceL failed");
        iLog->Log(_L("CIptvServices::GetServiceL failed"));
        VCXLOGLO1("<<<CIptvServiceTest::ServicesGetServiceL");
		return KErrGeneral;
        }

	if(service == NULL)
		{
 		VCXLOGLO1("CIptvServiceTest::ServicesGetServiceL: CIptvServices::GetServiceL returned NULL");
        iLog->Log(_L("CIptvServices::GetServiceL returned NULL"));
        VCXLOGLO1("<<<CIptvServiceTest::ServicesGetServiceL");
		return KErrBadHandle;
		}

   	CleanupStack::PushL(service);

// verify results

	CIptvService* service2 = NULL;

	if( index >= iServicesArray.Count() )
		{
		VCXLOGLO1("CIptvServiceTest::ServicesGetServiceL: Index larget than iServicesArray.Count, cannot verify result");
        iLog->Log(_L("Index larget than iServicesArray.Count, cannot verify result"));
        VCXLOGLO1("<<<CIptvServiceTest::ServicesGetServiceL");
		return KErrGeneral;
		}

	service2 = iServicesArray[index];

	TBufC<512> errorMsg (_L("CIptvServiceTest::ServicesGetServiceL: CIptvServices::GetServiceL returned ") );

	err = CompareService(errorMsg.Des(), *service, *service2);

	if(err != KErrNone)
		{
		IPTV_LOGTEXT(errorMsg.Des());
		iLog->Log(errorMsg.Des());
		CleanupStack::PopAndDestroy(service);
    	VCXLOGLO1("<<<CIptvServiceTest::ServicesGetServiceL");
		return KErrCorrupt;
		}

	CleanupStack::PopAndDestroy(service);

	VCXLOGLO1("<<<CIptvServiceTest::ServicesGetServiceL");
    return err;

    }


// -----------------------------------------------------------------------------
// CIptvServiceTest::ServicesGetServiceByRefL
// ServicesGetByRefL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::ServicesGetServiceByRefL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::ServicesGetServiceByRefL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In ServicesGetServiceByRefL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params

	TInt index;

 	if(aItem.GetNextInt(index) == KErrNone)
        {
        VCXLOGLO2("CIptvServiceTest::ServicesGetServiceByRefL: param index = %d", index);
        }
    else
        {
        VCXLOGLO1("CIptvServiceTest::ServicesGetServiceByRefL: reading index parameter failed");
        iLog->Log(_L("Reading index parameter failed"));
        VCXLOGLO1("<<<CIptvServiceTest::ServicesGetServiceByRefL");
        return KErrArgument;
        }

// do stuff


// verify results
	CIptvService* service2 = NULL;

	if( index >= iServicesArray.Count() )
		{
		VCXLOGLO1("CIptvServiceTest::ServicesGetServiceByRefL: Index larget than iServicesArray.Count, cannot verify result");
        iLog->Log(_L("Index larget than iServicesArray.Count, cannot verify result"));
        VCXLOGLO1("<<<CIptvServiceTest::ServicesGetServiceByRefL");
		return KErrGeneral;
		}

	service2 = iServicesArray[index];

	TBufC<512> errorMsg (_L("CIptvServiceTest::ServicesGetServiceByRefL: CIptvServices::GetServiceL returned  ") );

	err = CompareService(errorMsg.Des(), iServices->GetServiceRefL(index), *service2);

	if( err == KErrNone )
	    {
	    CIptvService& service3 = iServices->Service(index, err);
	    if( err == KErrNone )
	        {
	        err = CompareService(errorMsg.Des(), service3, *service2);
	        }
	    if( err != KErrNone )
    	    {
    	    VCXLOGLO2("CIptvServiceTest:: getservice() compare error: %d", err);
    	    }
	    }
	else
	    {
		IPTV_LOGTEXT(errorMsg.Des());
		iLog->Log(errorMsg.Des());
    	VCXLOGLO1("<<<CIptvServiceTest::ServicesGetServiceByRefL");
		return KErrCorrupt;
		}

	VCXLOGLO1("<<<CIptvServiceTest::ServicesGetServiceByRefL");
    return err;

    }


    /*
// -----------------------------------------------------------------------------
// CIptvServiceTest::XXXXXL
// XXXXXL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceTest::XXXXXL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceTest::XXXXXL");
    // Print to UI
    _LIT( KIptvServiceTest, "IptvServiceTest" );
    _LIT( KWhere, "In XXXXXL" );
    TestModuleIf().Printf( 0, KIptvServiceTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );


	TInt err = KErrNone;

// get params


// do stuff


// verify results


	VCXLOGLO1("<<<CIptvServiceTest::XXXXXL");
    return err;

    }
    */




// -----------------------------------------------------------------------------
// CIptvServiceTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CIptvServiceTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
