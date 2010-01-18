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
#include "VCXProvisioningTest.h"
#include "VCXTestLog.h"
#include "CIptvTestUtilities.h"
#include <f32file.h>

#include "CIptvServices.h"
#include "CIptvProvisioningProcessor.h"
#include "CIptvServiceManagementClient.h"

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
// CIptvProvisioningTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CIptvProvisioningTest::Delete()
    {
    DeAllocateResources();
    }

// -----------------------------------------------------------------------------
// CIptvProvisioningTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CIptvProvisioningTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Create", CIptvProvisioningTest::Create ),
        ENTRY( "Destroy", CIptvProvisioningTest::Destroy ),
        ENTRY( "EnableVerifying", CIptvProvisioningTest::EnableVerifying ),
        ENTRY( "Verify", CIptvProvisioningTest::Verify ),
        ENTRY( "ExternalProvision", CIptvProvisioningTest::ExternalProvision ),
        ENTRY( "LocalProvision", CIptvProvisioningTest::LocalProvision ),

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CIptvProvisioningTest::Create
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvProvisioningTest::Create( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvProvisioningTest::DestroyL");
    // Print to UI
    _LIT( KIptvProvisioningTest, "IptvProvisioningTest" );
    _LIT( KCreate, "In Create" );
    TestModuleIf().Printf( 0, KIptvProvisioningTest, KCreate );
    // Print to log file
    iLog->Log( KCreate);

    // TRAP
    iIptvProvisioningProcessor = CIptvProvisioningProcessor::NewL();
    iIptvTestUtilities = CIptvTestUtilities::NewL();
    iIptvServiceManagementClient = CIptvServiceManagementClient::NewL(*this);

    VCXLOGLO1("<<<CIptvProvisioningTest::DestroyL");
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIptvProvisioningTest::Destroy
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvProvisioningTest::Destroy( CStifItemParser& /* aItem */ )
    {

    // Print to UI
    _LIT( KIptvProvisioningTest, "IptvProvisioningTest" );
    _LIT( KDestroy, "In Destroy" );
    TestModuleIf().Printf( 0, KIptvProvisioningTest, KDestroy );
    // Print to log file
    iLog->Log( KDestroy);

    DeAllocateResources();
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIptvProvisioningTest::EnableVerifying();
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvProvisioningTest::EnableVerifying( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvProvisioningTest::EnableVerifying");
    // Print to UI
    _LIT( KIptvProvisioningTest, "CIptvProvisioningTest" );
    _LIT( KEnableVerifying , "In EnableVerifying" );
    TestModuleIf().Printf( 0, KIptvProvisioningTest, KEnableVerifying );
    // Print to log file
    iLog->Log( KEnableVerifying );

    //Script parameters

    TInt result = KErrNone;

    TPtrC caseId;

    TBool IsParametersValid = ETrue;

   //Script parameters
    if(aItem.GetNextString(caseId) != KErrNone)
        {
        IsParametersValid = EFalse;
        }

    TInt appendInt = 0;
    TBool append = EFalse;
    if(aItem.GetNextInt(appendInt) == KErrNone)
        {
        append = static_cast<TBool>(appendInt);
        }

   if(!IsParametersValid)
        {
        VCXLOGLO1("Error in testparameters");
        iLog->Log(_L("Error in testparameters"));
        result = KErrGeneral;
        }
    else
        {
        iCaseId = caseId;
        iIptvTestUtilities->CreateVerifyData(iCaseId, append);
        }

    VCXLOGLO1("<<<CIptvProvisioningTest::EnableVerifying");
    return result;
    }


// -----------------------------------------------------------------------------
// CIptvProvisioningTest::Verify
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvProvisioningTest::Verify( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvProvisioningTest::Verify");
    // Print to UI
    _LIT( KIptvProvisioningTest, "CIptvProvisioningTest" );
    _LIT( KVerify , "In Verify" );
    TestModuleIf().Printf( 0, KIptvProvisioningTest, KVerify  );
    // Print to log file
    iLog->Log( KVerify  );

    TInt paramCount = 0;
    TPtrC caseId;
    TInt error = KErrNone;
    while(aItem.GetNextString(caseId) == KErrNone)
        {
        paramCount++;
        VCXLOGLO2("Verifying: %S.", &caseId);
        CIptvTestVerifyData::TVerifyResult verifyResult;
        error = iIptvTestUtilities->VerifyData(caseId, verifyResult);
        if(verifyResult != CIptvTestVerifyData::EVerifyOk)
	    	{
	    	VCXLOGLO3("Verify failed, VerifyResult: %d, err: %d", verifyResult, error);
	    	TestModuleIf().Printf( 0, _L("Verify failed"), _L("") );
	    	error = verifyResult;
	    	}
	    else
	    	{
			VCXLOGLO1("Data verified succesfully.");
	    	}
        }

    if(paramCount == 0)
        {
        error = KErrGeneral;
        VCXLOGLO1("Test case logic error, no CaseId(s) defined for verification..");
        }


    VCXLOGLO1("<<<CIptvProvisioningTest::Verify");
    return error;
    }


 // -----------------------------------------------------------------------------
// CIptvProvisioningTest::ExternalProvision
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvProvisioningTest::ExternalProvision( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvProvisioningTest::ExternalProvision");
    // Print to UI
    _LIT( KIptvProvisioningTest, "IptvProvisioningTest" );
    _LIT( KExternalProvision, "In ExternalProvision" );
    TestModuleIf().Printf( 0, KIptvProvisioningTest, KExternalProvision );
    // Print to log file
    iLog->Log( KExternalProvision);


    TInt i=0;
    TInt error = KErrNone;
    TPtrC fileName;

    //services from provisioning xml
    CIptvServices* pIptvServices = NULL;
    CIptvService* pIptvService = NULL;

    //Allow quotes in cfg file
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    //Get xml -filename of the xml file
    aItem.GetNextString(fileName);

    TInt checkSchemaValidity = 0;
    if(KErrNone != aItem.GetNextInt(checkSchemaValidity))
        {
        checkSchemaValidity = 0;
        }

    RFs session;
    RFile fileHandle;

    error = session.Connect();

    if (error != KErrNone)
        {
        VCXLOGLO2("CIptvProvisioningTest::ExternalProvision, can't create RFs session, error %d", error);
        VCXLOGLO1("<<<CIptvProvisioningTest::ExternalProvision");
        return error;
        }

    TInt err = KErrNone;

    //invalid handle case
    if(fileName.Compare(_L("Invalid handle"))==0)
        {
        TRAP(err, pIptvServices = iIptvProvisioningProcessor->ExternalProvisionL(fileHandle, error));
        }
    else //open the file to get valid handle
        {

        // check the xml schema if specified
        /*error = CheckIptvXmlSchema(fileName);
        if(checkSchemaValidity && KErrNone != error)
        {
            VCXLOGLO1("<<<CIptvProvisioningTest::ExternalProvision");
            session.Close();
            return error;
        }*/

        error = fileHandle.Open( session, fileName, EFileRead );
        }

    if (error != KErrNone)
        {
        VCXLOGLO2("CIptvProvisioningTest::ExternalProvision, file can't be opened, response error %d", error);
        session.Close();
        VCXLOGLO1("<<<CIptvProvisioningTest::ExternalProvision");
        return error;
        }

    if (err != KErrNone)
        {
        VCXLOGLO2("CIptvProvisioningTest::ExternalProvision, file can't be opened, ExternalProvisionL left: %d", err);
        session.Close();
        VCXLOGLO1("<<<CIptvProvisioningTest::ExternalProvision");
        return err;
        }


    //Read services from xml file to CIptvServices -classes
    TRAP(err, pIptvServices = iIptvProvisioningProcessor->ExternalProvisionL(fileHandle,error));
    if(err != KErrNone)
        {
        VCXLOGLO2("ExternalProvisionL left, err = %d",err);
        }
    else if(error != KErrNone)
        {
        VCXLOGLO2("ExternalProvisionL failed, error = %d",error);
        }
    else //ExternalProvisionL succeeded
        {
        CleanupStack::PushL(pIptvServices);

        VCXLOGLO2("Count = %d",pIptvServices->Count());
        for(i = 0; i < pIptvServices->Count();i++)
            {
            pIptvService =  pIptvServices->GetServiceL(i);
            CleanupStack::PushL(pIptvService);
            iIptvTestUtilities->PrintIptvService(pIptvService);
            iIptvTestUtilities->WriteVerifyIptvService(iCaseId, pIptvService);
            //Add service to db
            error = AddService(*pIptvService);
            CleanupStack::PopAndDestroy(pIptvService);
            }
        CleanupStack::PopAndDestroy(pIptvServices);
        }

    fileHandle.Close();
    session.Close();

    VCXLOGLO1("<<<CIptvProvisioningTest::ExternalProvision");
    return error;

    }



// -----------------------------------------------------------------------------
// CIptvProvisioningTest::LocalProvision
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvProvisioningTest::LocalProvision( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvProvisioningTest::LocalProvision");
    // Print to UI
    _LIT( KIptvProvisioningTest, "IptvProvisioningTest" );
    _LIT( KLocalProvision, "In DestrLocalProvisionoy" );
    TestModuleIf().Printf( 0, KIptvProvisioningTest, KLocalProvision );
    // Print to log file
    iLog->Log( KLocalProvision);

    TInt i=0;
    TPtrC fileName;
    TInt rv(KErrNone);
    //services from provisioning xml
    CIptvServices* pIptvServices = NULL;
    CIptvService* pIptvService = NULL;

    TInt error(KErrNone);

    //Allow quotes in cfg file
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    //Get xml -filename of the xml file
    aItem.GetNextString(fileName);

    TInt checkSchemaValidity = 0;
    if(KErrNone != aItem.GetNextInt(checkSchemaValidity))
        {
        checkSchemaValidity = 0;
        }

    // check the xml schema if specified
    error = CheckIptvXmlSchema(fileName);
    if(checkSchemaValidity && KErrNone != error)
    {
        VCXLOGLO1("<<<CIptvProvisioningTest::ExternalProvision");
        return error;
    }

    TRAPD(err,pIptvServices = iIptvProvisioningProcessor->LocalProvisionL(fileName,error));

     if(err != KErrNone)
        {
        VCXLOGLO2("LocalProvisionL left, err = %d",err);
        rv = err;
        }
    else if(error != KErrNone)
        {
        VCXLOGLO2("LocalProvisionL failed, error = %d",error);
        rv = error;
        }
    else //LocalProvisionL succeeded
        {
        CleanupStack::PushL(pIptvServices);

        VCXLOGLO2("services in xml = %d",pIptvServices->Count());
        for(i = 0; i < pIptvServices->Count();i++)
            {
            pIptvService =  pIptvServices->GetServiceL(i);
            CleanupStack::PushL(pIptvService);

            iIptvTestUtilities->PrintIptvService(pIptvService);
            iIptvTestUtilities->WriteVerifyIptvService(iCaseId, pIptvService);

            //Add service to db
            rv = AddService(*pIptvService);

            CleanupStack::PopAndDestroy(pIptvService);
            }
        CleanupStack::PopAndDestroy(pIptvServices);
        }

    VCXLOGLO1("<<<CIptvProvisioningTest::LocalProvision");
    return rv;

    }
 // -----------------------------------------------------------------------------
// CIptvProvisioningTest::AddService
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CIptvProvisioningTest::AddService(CIptvService& aService)
    {
    VCXLOGLO1(">>>CIptvProvisioningTest::AddService");
    TInt rv = 0;
    MIptvServiceManagementClientObserver::TRespStatus respStatus = ESucceeded;

    TRAPD(error, rv = iIptvServiceManagementClient->AddServiceL(aService,respStatus));

    if(error != KErrNone)
        {
        VCXLOGLO2("iIptvServiceManagementClient->AddServiceL() failed, error = %d", error);
        rv = error;
        }
    else if(respStatus != ESucceeded)
        {
        VCXLOGLO2("iIptvServiceManagementClient->AddServiceL() failed, error = %d", respStatus);
        rv = KErrGeneral;
        }

    VCXLOGLO1(">>>CIptvProvisioningTest::AddService");
    return rv;
}

TInt CIptvProvisioningTest::CheckIptvXmlSchema( TDesC& aFileName )
    {
    // No need to check by tests.
    return KErrNone;
    }

 // -----------------------------------------------------------------------------
// CIptvProvisioningTest::DeAllocateResources
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CIptvProvisioningTest::DeAllocateResources()
    {
    VCXLOGLO1(">>>CIptvProvisioningTest::DeAllocateResources");
    if(iIptvProvisioningProcessor)
        {
         delete iIptvProvisioningProcessor;
         iIptvProvisioningProcessor = NULL;
        }

    if(iIptvTestUtilities)
        {
         delete iIptvTestUtilities;
         iIptvTestUtilities = NULL;
        }

    if(iIptvServiceManagementClient)
        {
         delete iIptvServiceManagementClient;
         iIptvServiceManagementClient = NULL;
        }

    VCXLOGLO1(">>>CIptvProvisioningTest::DeAllocateResources");
}

/***** Callbakcs *********/
/*void CIptvProvisioningTest::Notify(TInt aNotification)
    {

    }*/
// -----------------------------------------------------------------------------
// CIptvProvisioningTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CIptvProvisioningTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
