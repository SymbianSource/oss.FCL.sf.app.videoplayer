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
#include "CIptvIapList.h"
#include "TIptvIap.h"
#include "VCXIapListTest.h"
#include "VCXTestLog.h"
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
// CIptvIapListTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CIptvIapListTest::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CIptvIapListTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Create", CIptvIapListTest::Create ),
        ENTRY( "AddIap", CIptvIapListTest::AddIap ),
        ENTRY( "DeleteIap", CIptvIapListTest::DeleteIap ),
        ENTRY( "DeleteIapByReference", CIptvIapListTest::DeleteIapByReference ),
        ENTRY( "ModifyIap", CIptvIapListTest::ModifyIap ),
        ENTRY( "GetIap", CIptvIapListTest::GetIap ),
        ENTRY( "CountIaps", CIptvIapListTest::CountIaps ),
        ENTRY( "SetIapList", CIptvIapListTest::SetIapList ),
        ENTRY( "SortListByPriority", CIptvIapListTest::SortListByPriority ),
        ENTRY( "FindIap", CIptvIapListTest::FindIap ),
        ENTRY( "CreateTestIapList", CIptvIapListTest::CreateTestIapList ),
        ENTRY( "PrintIapList", CIptvIapListTest::PrintIapList ),
		ENTRY( "DoExternalizeAndInternalize", CIptvIapListTest::DoExternalizeAndInternalize ),
        ENTRY( "Destroy", CIptvIapListTest::Destroy ),
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CIptvIapListTest::Create
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::Create( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvIapListTest::Create");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KCreate, "In Create" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KCreate );
    // Print to log file
    iLog->Log( KCreate );

    TInt rv = KErrNone;

    TRAP(rv, iIptvIapList = CIptvIapList::NewL());

    if(KErrNone != rv)
        {
        VCXLOGLO1("<<<CIptvIapListTest::Create: creating instance failed!!");
        }

    VCXLOGLO1("<<<CIptvIapListTest::Create");
    return rv;

    }

// -----------------------------------------------------------------------------
// CIptvIapListTest::AddIap
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::AddIap( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvIapListTest::AddIap");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KAddIap, "In AddIap" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KAddIap );
    // Print to log file
    iLog->Log( KAddIap );

    TUint id(0);
    TUint priority(0);
    TInt rv = KErrNone;
    TInt ignoreMax = 0;

    if(aItem.GetNextInt(id) == KErrNone)
        {
        VCXLOGLO2("CIptvIapListTest::AddIap: id = %d",id);
        }
    else
        {
        VCXLOGLO1("CIptvIapListTest::AddIap: reading id parameter failed");
        iLog->Log(_L("Reading id parameter failed"));
        VCXLOGLO1("<<<CIptvIapListTest::AddIap");
        return KErrArgument;
        }

    if(aItem.GetNextInt(priority) == KErrNone)
        {
        VCXLOGLO2("CIptvIapListTest::AddIap: priority = %d", priority);
        }
    else
        {
        VCXLOGLO1("CIptvIapListTest::AddIap: reading priority parameter failed");
        iLog->Log(_L("Reading priority parameter failed"));
        VCXLOGLO1("<<<CIptvIapListTest::AddIap");
        return KErrArgument;
        }

	if(aItem.GetNextInt(ignoreMax) != KErrNone)
        {
        ignoreMax = 0;
        }

    VCXLOGLO2("CIptvIapListTest::AddIap: ignoreMax = %d", ignoreMax);

    TIptvIap iap;
    iap.iId = id;
    iap.iPriority = priority;

    //rv = AddIap(iap);

    /**
         * AddIap.
         * Adds Internet access point.\n
         * Maximun of KIptvServicesDbMaxIaps access points are allowed,\n
// real: KIptvSmServicesDbMaxIaps
         * function call fails if tried to add more.\n
         * @param aIap
         * @return System wide error code.
         */
        //IMPORT_C TInt AddIap(TIptvIap& aIap);

    TInt iapCount = iIptvIapList->Count();

    rv = iIptvIapList->AddIap(iap, ignoreMax);

    // verify that iap is added

    if(rv != KErrNone)
    	{
        if(rv == KErrOverflow && !ignoreMax)
        	{
	        VCXLOGLO1("CIptvIapListTest::AddIap: KIptvSmServicesDbMaxIaps reached");
	        iLog->Log(_L("KIptvSmServicesDbMaxIaps reached"));
	        VCXLOGLO1("<<<CIptvIapListTest::AddIap");
	        return rv;
        	}
        else
        	{
	        VCXLOGLO1("CIptvIapListTest::AddIap: CIptvIapList::AddIap returned error");
	        iLog->Log(_L("AddIap returned error"));
	        VCXLOGLO1("<<<CIptvIapListTest::AddIap");
	        return rv;
        	}
    	}
    else
    if(iIptvIapList->Count() <= iapCount)
    	{
        VCXLOGLO1("CIptvIapListTest::AddIap: Iap count of list did not increment");
        iLog->Log(_L("Iap count of list did not increment"));
        VCXLOGLO1("<<<CIptvIapListTest::AddIap");
        return KErrGeneral;
    	}

  	// verify values
  	TIptvIap iap2;
  	TInt err = iIptvIapList->GetIap( iIptvIapList->Count()-1, iap2 );
    VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", err);

  	if(iap.iId != iap2.iId)
  		{
   		VCXLOGLO1("CIptvIapListTest::AddIap: Id of iap is wrong");
        iLog->Log(_L("Id of iap is wrong"));
        VCXLOGLO1("<<<CIptvIapListTest::AddIap");
        return KErrCorrupt;
  		}

  	if(iap.iPriority != iap2.iPriority)
  		{
   		VCXLOGLO1("CIptvIapListTest::AddIap: Priority of iap is wrong");
        iLog->Log(_L("Priority of iap is wrong"));
        VCXLOGLO1("<<<CIptvIapListTest::AddIap");
        return KErrCorrupt;
  		}

	if(iIptvIapList->Count() > KIptvSmServicesDbMaxIaps && !ignoreMax)
		{
        VCXLOGLO1("CIptvIapListTest::AddIap: Iap list has more than KIptvSmServicesDbMaxIaps iaps");
        iLog->Log(_L("Iap list has more than KIptvSmServicesDbMaxIaps iaps"));
        VCXLOGLO1("<<<CIptvIapListTest::AddIap");
        return KErrOverflow;
		}

    VCXLOGLO1("<<<CIptvIapListTest::AddIap");
    return rv;

    }



// -----------------------------------------------------------------------------
// CIptvIapListTest::DeleteIap
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::DeleteIap( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvIapListTest::DeleteIap");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KDeleteIap, "In DeleteIap" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KDeleteIap );
    // Print to log file
    iLog->Log( KDeleteIap );

    TInt rv = KErrNone;
    TInt index = 0;

    if(aItem.GetNextInt(index) == KErrNone)
        {
         VCXLOGLO2("CIptvIapListTest::DeleteIap: index = %d",index);
        }
    else
        {
        VCXLOGLO1("CIptvIapListTest::DeleteIap: Reading index parameter failed");
        iLog->Log(_L("Reading index parameter failed"));
        VCXLOGLO1("<<<CIptvIapListTest::DeleteIap");
        return KErrArgument;
        }

     /**
     * DeleteAp.
     * Deletes Internet access point.\n
     * @param aIndex Index of the IAP to delete.
     * @return System wide error code.
     */
    //IMPORT_C TInt DeleteIap(TUint8 aIndex);

	// make a copy of list for verifying that iap has been really deleted
	CIptvIapList* listCopy;
    TRAP( rv, listCopy = CIptvIapList::NewL() );

    if(KErrNone != rv)
        {
        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference: creating instance of listCopy (CIptvIapList) failed!");
        iLog->Log(_L("Creating new instance of listCopy (CIptvIapList) failed"));
    	VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
    	return KErrNoMemory;
        }

    CleanupStack::PushL(listCopy);

    listCopy->SetL(*iIptvIapList);

    // delete the iap

    TInt iapCount = iIptvIapList->Count();

    rv = iIptvIapList->DeleteIap(index);

    // verify that iap was deleted

    if(rv != KErrNone)
    	{
        VCXLOGLO1("CIptvIapListTest::DeleteIap: DeleteIap returned error");
        iLog->Log(_L("DeleteIap returned error"));
        CleanupStack::PopAndDestroy(listCopy);
        VCXLOGLO1("<<<CIptvIapListTest::DeleteIap");
        return rv;
    	}
    else
    if(iIptvIapList->Count() >= iapCount)
    	{
        VCXLOGLO1("CIptvIapListTest::DeleteIap: Iap count of iaps is not smaller than before deletion");
        iLog->Log(_L("Iap count of iaps is not smaller than before deletion"));
        CleanupStack::PopAndDestroy(listCopy);
        VCXLOGLO1("<<<CIptvIapListTest::DeleteIap");
        return KErrGeneral;
    	}
    else
    	if(iIptvIapList->Count() > 0)
    	{
			// compare the list to the old list

			TInt i;
			for(i=0; i<index; i++)
				{

				TIptvIap iap1;
				TInt ret = iIptvIapList->GetIap(i, iap1);
				VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

				TIptvIap iap2;
				ret = listCopy->GetIap(i, iap2);
				VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

				if(iap1.iId != iap2.iId)
			    	{
			        VCXLOGLO3("CIptvIapListTest::DeleteIapByReference: Iap at index: %d has different id after deleting iap at index: %d", i, index);
			        iLog->Log(_L("Wrong id after deleting"));
			        CleanupStack::PopAndDestroy(listCopy);
			        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
			        return KErrCorrupt;
			    	}

			    if(iap1.iPriority != iap2.iPriority)
			    	{
			        VCXLOGLO3("CIptvIapListTest::DeleteIapByReference: Iap at index: %d has different priority after deleting iap at index: %d", i, index);
			        iLog->Log(_L("Wrong priority after deleting"));
			        CleanupStack::PopAndDestroy(listCopy);
			        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
			        return KErrCorrupt;
			    	}

				}

			if(iIptvIapList->Count() > index)
				{
				for(i=index; i<iIptvIapList->Count(); i++)
					{
					TIptvIap iap1;
					TInt ret = iIptvIapList->GetIap(i, iap1);
					VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

					TIptvIap iap2;
					ret = listCopy->GetIap(i+1, iap2);
					VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

					if(iap1.iId != iap2.iId)
				    	{
				        VCXLOGLO3("CIptvIapListTest::DeleteIapByReference: Iap at index: %d has different id after deleting iap at index: %d", i, index);
				        iLog->Log(_L("Wrong id after deleting"));
				        CleanupStack::PopAndDestroy(listCopy);
				        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
				        return KErrCorrupt;
				    	}

				    if(iap1.iPriority != iap2.iPriority)
				    	{
				        VCXLOGLO3("CIptvIapListTest::DeleteIapByReference: Iap at index: %d has different priority after deleting iap at index: %d", i, index);
				        iLog->Log(_L("Wrong priority after deleting"));
				        CleanupStack::PopAndDestroy(listCopy);
				        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
				        return KErrCorrupt;
				    	}

					}
				}
    	}

    CleanupStack::PopAndDestroy(listCopy);
    VCXLOGLO1("<<<CIptvIapListTest::DeleteIap");
    return rv;

    }


// -----------------------------------------------------------------------------
// CIptvIapListTest::DeleteIapByReference
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::DeleteIapByReference( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvIapListTest::DeleteIapByReference");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KDeleteIap, "In DeleteIapByReference" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KDeleteIap );
    // Print to log file
    iLog->Log( KDeleteIap );

    TInt rv = KErrNone;
    TInt index = 0;

    if(aItem.GetNextInt(index) == KErrNone)
        {
         VCXLOGLO2("CIptvIapListTest::DeleteIapByReference: index = %d",index);
        }
    else
        {
        VCXLOGLO1("CIptvIapListTest::DeleteIapByReference: Reading index parameter failed");
        iLog->Log(_L("Reading index parameter failed"));
        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
        return KErrArgument;
        }

     /**
     * DeleteAp.
     * Deletes Internet access point.\n
     * @param aIndex Index of the IAP to delete.
     * @return System wide error code.
     */
    //IMPORT_C TInt DeleteIap(TUint8 aIndex);


	// make a copy of list for verifying that iap has been really deleted
	CIptvIapList* listCopy;
    TRAP( rv, listCopy = CIptvIapList::NewL() );

    if(KErrNone != rv)
        {
        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference: creating instance of listCopy (CIptvIapList) failed!");
        iLog->Log(_L("Creating new instance of listCopy (CIptvIapList) failed"));
    	VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
    	return KErrNoMemory;
        }

    CleanupStack::PushL(listCopy);

    listCopy->SetL(*iIptvIapList);

    // delete the iap

    TInt iapCount = iIptvIapList->Count();

	TIptvIap iap;
	TInt ret = iIptvIapList->GetIap(index, iap);
	VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);
    rv = iIptvIapList->DeleteIap(iap);

    // verify that iap was deleted

    if(rv != KErrNone)
    	{
        VCXLOGLO1("CIptvIapListTest::DeleteIapByReference: DeleteIap returned error");
        iLog->Log(_L("DeleteIap returned error"));
        CleanupStack::PopAndDestroy(listCopy);
        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
        return rv;
    	}
    else
    if(iIptvIapList->Count() >= iapCount)
    	{
        VCXLOGLO1("CIptvIapListTest::DeleteIapByReference: Iap count of iaps is not smaller than before deletion");
        iLog->Log(_L("Iap count of iaps is not smaller than before deletion"));
        CleanupStack::PopAndDestroy(listCopy);
        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
        return KErrGeneral;
    	}
    else
    	if(iIptvIapList->Count() > 0)
    	{
			// compare the list to the old list

			TInt i;
			for(i=0; i<index; i++)
				{

				TIptvIap iap1;
				TInt ret = iIptvIapList->GetIap(i, iap1);
				VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

				TIptvIap iap2;
				ret  = listCopy->GetIap(i, iap2);
				VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

				if(iap1.iId != iap2.iId)
			    	{
			        VCXLOGLO3("CIptvIapListTest::DeleteIapByReference: Iap at index: %d has different id after deleting iap at index: %d", i, index);
			        iLog->Log(_L("Wrong id after deleting"));
			        CleanupStack::PopAndDestroy(listCopy);
			        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
			        return KErrCorrupt;
			    	}

			    if(iap1.iPriority != iap2.iPriority)
			    	{
			        VCXLOGLO3("CIptvIapListTest::DeleteIapByReference: Iap at index: %d has different priority after deleting iap at index: %d", i, index);
			        iLog->Log(_L("Wrong priority after deleting"));
			        CleanupStack::PopAndDestroy(listCopy);
			        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
			        return KErrCorrupt;
			    	}

				}

			if(iIptvIapList->Count() > index)
				{
				for(i=index; i<iIptvIapList->Count(); i++)
					{
					TIptvIap iap1;
					TInt ret = iIptvIapList->GetIap(i, iap1);
					VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

					TIptvIap iap2;
					ret = listCopy->GetIap(i+1,  iap2);
					VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

					if(iap1.iId != iap2.iId)
				    	{
				        VCXLOGLO3("CIptvIapListTest::DeleteIapByReference: Iap at index: %d has different id after deleting iap at index: %d", i, index);
				        iLog->Log(_L("Wrong id after deleting"));
				        CleanupStack::PopAndDestroy(listCopy);
				        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
				        return KErrCorrupt;
				    	}

				    if(iap1.iPriority != iap2.iPriority)
				    	{
				        VCXLOGLO3("CIptvIapListTest::DeleteIapByReference: Iap at index: %d has different priority after deleting iap at index: %d", i, index);
				        iLog->Log(_L("Wrong priority after deleting"));
				        CleanupStack::PopAndDestroy(listCopy);
				        VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
				        return KErrCorrupt;
				    	}

					}
				}
    	}

    CleanupStack::PopAndDestroy(listCopy);
    VCXLOGLO1("<<<CIptvIapListTest::DeleteIapByReference");
    return rv;

    }

// -----------------------------------------------------------------------------
// CIptvIapListTest::ModifyIap
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::ModifyIap( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvIapListTest::ModifyIap");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KModifyIap, "In ModifyIap" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KModifyIap );
    // Print to log file
    iLog->Log( KModifyIap );

    TInt rv = KErrNone;
    TInt index = 0;
    TUint id(0);
    TUint priority(0);

    if(aItem.GetNextInt(index) == KErrNone)
        {
         VCXLOGLO2("CIptvIapListTest::ModifyIap: index = %d",index);
        }
    else
        {
        VCXLOGLO1("CIptvIapListTest::ModifyIap: reading index parameter failed");
        iLog->Log(_L("Reading index parameter failed"));
        VCXLOGLO1("<<<CIptvIapListTest::ModifyIap");
        return KErrArgument;
        }

    if(aItem.GetNextInt(id) == KErrNone)
        {
        VCXLOGLO2("CIptvIapListTest::ModifyIap: id = %d",id);
        }
    else
        {
        VCXLOGLO1("CIptvIapListTest::ModifyIap: reading id parameter failed");
        iLog->Log(_L("Reading id parameter failed"));
        VCXLOGLO1("<<<CIptvIapListTest::ModifyIap");
        return KErrArgument;
        }

    if(aItem.GetNextInt(priority) == KErrNone)
        {
        VCXLOGLO2("CIptvIapListTest::ModifyIap: priority = %d", priority);
        }
    else
        {
        VCXLOGLO1("CIptvIapListTest::ModifyIap: reading priority parameter failed");
        iLog->Log(_L("Reading priority parameter failed"));
        VCXLOGLO1("<<<CIptvIapListTest::ModifyIap");
        return KErrArgument;
        }

    TIptvIap updatedIap;
    updatedIap.iId = id;
    updatedIap.iPriority = priority;

            /**
         * ModifyIap.
         * Modifies Internet access point.
         * @param aIndex Index of the access point to modify.
         * @param aIap New access point.
         * @return System wide error code.
         */
        //IMPORT_C TInt ModifyIap(TUint8 aIndex, TIptvIap& aIap);
    rv = iIptvIapList->ModifyIap(index, updatedIap);


    if(rv != KErrNone)
    	{
        VCXLOGLO1("CIptvIapListTest::ModifyIap: ModifyIap returned error");
        iLog->Log(_L("ModifyIap returned error"));
        VCXLOGLO1("<<<CIptvIapListTest::ModifyIap");
        return rv;
    	}

    TIptvIap iap;
    TInt ret = iIptvIapList->GetIap(index, iap);
    VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

    VCXLOGLO4("CIptvIapListTest::ModifyIap: index = %d, id = %d, pri = %d", index, iap.iId, iap.iPriority);

    if(iap.iId != updatedIap.iId)
    	{
        VCXLOGLO1("CIptvIapListTest::ModifyIap: Wrong id after modification");
        iLog->Log(_L("Wrong id after modification"));
        VCXLOGLO1("<<<CIptvIapListTest::ModifyIap");
        return KErrCorrupt;
    	}

    if(iap.iPriority != updatedIap.iPriority)
    	{
        VCXLOGLO1("CIptvIapListTest::ModifyIap: Wrong priority after modification");
        iLog->Log(_L("Wrong priority after modification"));
        VCXLOGLO1("<<<CIptvIapListTest::ModifyIap");
        return KErrCorrupt;
    	}

    VCXLOGLO1("<<<CIptvIapListTest::ModifyIap");
    return rv;

    }
// -----------------------------------------------------------------------------
// CIptvIapListTest::GetIap
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::GetIap( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvIapListTest::GetIap");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KGetIap, "In GetIap" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KGetIap );
    // Print to log file
    iLog->Log( KGetIap );

    TInt index = 0;
    TInt rv = KErrNone;

    if(aItem.GetNextInt(index) == KErrNone)
        {
         VCXLOGLO2("CIptvIapListTest::GetIap: index = %d",index);
        }
    else
        {
        VCXLOGLO1("CIptvIapListTest::GetIap: reading index parameter failed");
        iLog->Log(_L("Reading index parameter failed"));
        VCXLOGLO1("<<<CIptvIapListTest::GetIap");
        return KErrArgument;
        }

           /**
         * GetIap.
         * Gets Internet access point.
         * @param aIndex index of the IAP on the list. Index starts from 0.
         *        It must not be greater than the number of objects currently
         *        in the list, otherwise the method raises a USER-130 panic.
         * @return Reference to TIptvIap.
         */
        //IMPORT_C TIptvIap& GetIap(TUint8 aIndex);
    TIptvIap iap;
    rv = iIptvIapList->GetIap(index, iap);
    VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", rv);

    //TRACE values
    //VCXLOGLO2("CIptvIapListTest::GetIap: iap.iValueExists = %d", iap.iValueExists);
    VCXLOGLO4("CIptvIapListTest::GetIap: index = %d, id = %d, pri = %d", index, iap.iId, iap.iPriority);
    // verify results

    VCXLOGLO1("<<<CIptvIapListTest::GetIap");
    return rv;

    }
// -----------------------------------------------------------------------------
// CIptvIapListTest::CountIaps
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::CountIaps( CStifItemParser& aItem )
    {
	VCXLOGLO1(">>>CIptvIapListTest::CountIaps");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KCountIaps, "In CountIaps" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KCountIaps );
    // Print to log file
    iLog->Log( KCountIaps );


    TInt expectedCount = -1;

    if(aItem.GetNextInt(expectedCount) == KErrNone)
        {
         VCXLOGLO2("CIptvIapListTest::CountIaps: expectedCount = %d", expectedCount);
        }
    else
        {
        VCXLOGLO1("CIptvIapListTest::CountIaps: no expectedCount defined, continuing");
        }


	if(expectedCount < -1)
		{
        VCXLOGLO1("CIptvIapListTest::CountIaps: Parameter excepted count is negative");
        iLog->Log(_L("Parameter excepted count is negative"));
        VCXLOGLO1("<<<CIptvIapListTest::CountIaps");
        return KErrArgument;
		}

    /**
         * Gets Internet access point count.
         * @return Internet access point count.
         */
        //IMPORT_C TUint8 Count();

    TInt count = iIptvIapList->Count();

    VCXLOGLO2("CIptvIapListTest::CountIaps: count = %d", count);

    if(expectedCount > -1)
    	{
		if(expectedCount != count)
			{
	        VCXLOGLO1("CIptvIapListTest::CountIaps: Count of iaps doesn't match to the expected count");
	        iLog->Log(_L("Count of iaps doesn't match to the expected count"));
	        VCXLOGLO1("<<<CIptvIapListTest::CountIaps");
	        return KErrGeneral;
			}
    	}

    if(count < 0)
    	{
        VCXLOGLO1("CIptvIapListTest::CountIaps: Negative iap count");
        iLog->Log(_L("Negative iap count"));
        VCXLOGLO1("<<<CIptvIapListTest::CountIaps");
        return KErrCorrupt;
    	}

	VCXLOGLO1("<<<CIptvIapListTest::CountIaps");
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CIptvIapListTest::SetIapList
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::SetIapList( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvIapListTest::SetIapList");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KSetIapList, "In SetIapList" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KSetIapList );
    // Print to log file
    iLog->Log( KSetIapList );

    TInt rv = KErrNone;
	CIptvIapList* newlist;
    TRAP( rv, newlist = CIptvIapList::NewL() );

    if(KErrNone != rv)
        {
        VCXLOGLO1("<<<CIptvIapListTest::SetIapList: creating instance of CIptvIapList failed!!");
        iLog->Log(_L("Creating new instance of CIptvIapList failed"));
    	VCXLOGLO1("<<<CIptvIapListTest::SetIapList");
    	return KErrNoMemory;
        }

    CleanupStack::PushL(newlist);

    newlist->SetL(*iIptvIapList);

    // verify results

    if( iIptvIapList->Count() != newlist->Count() )
    	{
        VCXLOGLO1("CIptvIapListTest::SetIapList: Iap count of new list is wrong");
        iLog->Log(_L("Iap count of new list is wrong"));
    	CleanupStack::PopAndDestroy(newlist);
    	VCXLOGLO1("<<<CIptvIapListTest::SetIapList");
    	return KErrCorrupt;
    	}

    TIptvIap iap, iap2;

    TInt i;
    for(i=0; i<iIptvIapList->Count(); i++)
    	{

   		TIptvIap iap;
   		TInt ret = iIptvIapList->GetIap(i, iap);
		VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

		TIptvIap iap2;
		ret = newlist->GetIap(i, iap2);
		VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

		if(iap.iId != iap2.iId)
			{
	    	VCXLOGLO2("CIptvIapListTest::SetIapList: iId of iap at index %d is wrong", i);
    	    iLog->Log(_L("iId of iap is wrong"));
    	    CleanupStack::PopAndDestroy(newlist);
    	    VCXLOGLO1("<<<CIptvIapListTest::SetIapList");
			return KErrCorrupt;
			}
		if(iap.iPriority != iap2.iPriority)
			{
	    	VCXLOGLO2("CIptvIapListTest::SetIapList: iPriority of iap at index %d is wrong", i);
    	    iLog->Log(_L("iPriority of iap is wrong"));
			CleanupStack::PopAndDestroy(newlist);
			VCXLOGLO1("<<<CIptvIapListTest::SetIapList");
			return KErrCorrupt;
			}
/*		if(iap.iValueExists != iap2.iValueExists)
			{
	    	VCXLOGLO2("CIptvIapListTest::SetIapList: iValueExists of iap at index %d is wrong", i);
    	    iLog->Log(_L("iValueExists of iap is wrong"));
    	    CleanupStack::PopAndDestroy(newlist);
    	    VCXLOGLO1("<<<CIptvIapListTest::SetIapList");
			return KErrCorrupt;
			}*/
    	}

    /**
         * Set this object from aIapList (Copies data).
         */
        //IMPORT_C void SetL(CIptvIapList& aIapList);

   	CleanupStack::PopAndDestroy(newlist);

   	VCXLOGLO1("<<<CIptvIapListTest::SetIapList");
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIptvIapListTest::SortListByPriority
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::SortListByPriority( CStifItemParser& /* aItem */ )
    {
	VCXLOGLO1(">>>CIptvIapListTest::SortListByPriority");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KSortListByPriority, "In SortListByPriority" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KSortListByPriority );
    // Print to log file
    iLog->Log( KSortListByPriority );

    /**
         * Sorts list, highest priority to index 0, 2nd highest to 1, etc
         */
        //IMPORT_C void SortByPriorityL();

	iIptvIapList->SortByPriorityL();

    if( iIptvIapList->Count() <= 1 )
    	{
        VCXLOGLO1("CIptvIapListTest::SortListByPriority: Less than 2 iaps in list, no results to verify");
        iLog->Log(_L("Less than 2 iaps in list, no results to verify"));
        VCXLOGLO1("<<<CIptvIapListTest::SortListByPriority");
    	return KErrNone;
    	}

	TInt i;
	TIptvIap iap;
	TInt ret = iIptvIapList->GetIap(0, iap);
	VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);
	TInt previousPriority = iap.iPriority;

	for(i=1; i<iIptvIapList->Count(); i++)
		{
		ret = iIptvIapList->GetIap(i, iap);
		VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);
		if(iap.iPriority < previousPriority)
			{
			VCXLOGLO2("CIptvIapListTest::SortListByPriority: List is not in priority order at index: %d", i);
    		iLog->Log(_L("List is not in priority order"));
    		VCXLOGLO1("<<<CIptvIapListTest::SortListByPriority");
    		return KErrGeneral;
			}

		ret = iIptvIapList->GetIap(i, iap);
		VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);
		previousPriority = iap.iPriority;
		}


    VCXLOGLO1("<<<CIptvIapListTest::SortListByPriority");
    return KErrNone;

    }
// -----------------------------------------------------------------------------
// CIptvIapListTest::FindIap
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::FindIap( CStifItemParser& aItem )
    {
	VCXLOGLO1(">>>CIptvIapListTest::FindIap");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KFindIap, "In FindIap" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KFindIap );
    // Print to log file
    iLog->Log( KFindIap );

    /**
         * Finds the first occurance of IAP which has aIapId as ID.
         * @param aIapId
         * @param aIndex index of the found IAP
         * @return System-wide error code.
         */
        //IMPORT_C TInt FindIap(TUint32 aIapId, TUint8& aIndex);

	TInt rv = KErrNone;
    TInt id = 0;

    if(aItem.GetNextInt(id) == KErrNone)
        {
         VCXLOGLO2("CIptvIapListTest::FindIap: id = %d", id);
        }
    else
        {
        VCXLOGLO1("CIptvIapListTest::FindIap: reading id parameter failed");
        iLog->Log(_L("Reading id parameter failed"));
        VCXLOGLO1("<<<CIptvIapListTest::FindIap");
        return KErrArgument;
        }

    TUint8 index;

    rv = iIptvIapList->FindIap(id, index);

    TBool iapInList = EFalse;

	for(TInt i=0; i<iIptvIapList->Count(); i++)
		{
		TIptvIap iap;
		TInt ret = iIptvIapList->GetIap(i, iap);
		VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);
		if(iap.iId == id)
			{
			iapInList = ETrue;
			}
		}

	// not found but in the list
	if(rv != KErrNone && iapInList != EFalse)
		{
        VCXLOGLO2("CIptvIapListTest::FindIap: Iap not found but iap with id: %d is in the list", id);
        iLog->Log(_L("Iap not found but iap with the id is in the list"));
        VCXLOGLO1("<<<CIptvIapListTest::FindIap");
		return KErrGeneral;
		}

	if(rv != KErrNone)
		{
        VCXLOGLO2("CIptvIapListTest::FindIap: Iap not found with id: %d", id);
        iLog->Log(_L("Iap not found"));
        VCXLOGLO1("<<<CIptvIapListTest::FindIap");
		return KErrNotFound;
		}

	TIptvIap iap;
	TInt ret = iIptvIapList->GetIap(index, iap);
	VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

	if(rv == KErrNone && iap.iId != id)
		{
        VCXLOGLO2("CIptvIapListTest::FindIap: Iap found but it has wrong id, at index: %d", index);
        iLog->Log(_L("Iap found but it has wrong id"));
        VCXLOGLO1("<<<CIptvIapListTest::FindIap");
		return KErrBadHandle;
		}

	VCXLOGLO4("CIptvIapListTest::FindIap: index = %d, id = %d, pri = %d", index, iap.iId, iap.iPriority);

    VCXLOGLO1("<<<CIptvIapListTest::FindIap");
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CIptvIapListTest::PrintIapList
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::PrintIapList( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvIapListTest::PrintIapList");
      // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KPrintIapList, "In PrintIapList" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KPrintIapList );
    // Print to log file
    iLog->Log( KPrintIapList );

    TUint i=0;
    TIptvIap iap;
    for(i = 0; i < iIptvIapList->Count(); i++)
        {
        TInt ret = iIptvIapList->GetIap(i, iap);
        VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);
        VCXLOGLO4("CIptvIapListTest::PrintIapList: index = %d, id = %d, pri = %d", i, iap.iId, iap.iPriority);
        }

    VCXLOGLO1("<<<CIptvIapListTest::PrintIapList");
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CIptvIapListTest::CreateTestIapList
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CIptvIapListTest::CreateTestIapList( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvIapListTest::CreateTestIapList");
      // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KCreateTestIapList, "In CreateTestIapList" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KCreateTestIapList );
    // Print to log file
    iLog->Log( KCreateTestIapList );

    TUint i=0;
    TInt rv = KErrNone;
    TIptvIap iap;

    for(i = 0; i < KIptvSmServicesDbMaxIaps; i++)
        {
        iap.iId = i + 1;
        iap.iPriority = i + 2;
        rv = iIptvIapList->AddIap(iap);
        if(rv != KErrNone)
            {
            VCXLOGLO2("CIptvIapListTest::CreateTestIapList: AddIap failed, rv = %d", rv);
            break;
            }
        }

    VCXLOGLO1("<<<CIptvIapListTest::CreateTestIapList");
    return rv;
    }



TInt CIptvIapListTest::DoExternalizeAndInternalize(CStifItemParser& /* aItem */ )
	{
	VCXLOGLO1(">>>CIptvIapListTest::DoExternalizeAndInternalize");
      // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KDoExternalizeAndInternalize, "In DoExternalizeAndInternalize" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KDoExternalizeAndInternalize );
    // Print to log file
    iLog->Log( KDoExternalizeAndInternalize );

	// setup buffer and externalize
	TUint32 dataSize = iIptvIapList->CountExternalizeSize();

    HBufC8* buff;
	buff = HBufC8::NewL(dataSize);
	TPtr8 buffPtr(buff->Des());

	RDesWriteStream writeStream;
	writeStream.Open( buffPtr );
	iIptvIapList->ExternalizeL(writeStream);
	writeStream.CommitL();
	writeStream.Close();

    // setup new CIptvIapList

    TInt rv = KErrNone;
	CIptvIapList* newlist;

    TRAP( rv, newlist = CIptvIapList::NewL() );

    if(KErrNone != rv)
        {
        VCXLOGLO1("<<<CIptvIapListTest::DoExternalizeAndInternalize: Creating instance of CIptvIapList failed!!");
        iLog->Log(_L("Creating instance of CIptvIapList failed"));
    	VCXLOGLO1("<<<CIptvIapListTest::DoExternalizeAndInternalize");
    	return KErrNoMemory;
        }

    CleanupStack::PushL(newlist);

	// internalize the data

	RDesReadStream readStream;
	readStream.Open( buffPtr );
	newlist->InternalizeL(readStream);
	readStream.Close();

	// compare here
	TBool errors = EFalse;

	if( newlist->Count() != iIptvIapList->Count() )
		{
		VCXLOGLO1("<<<CIptvIapListTest::DoExternalizeAndInternalize: New internalized list has different count of iaps");
		iLog->Log(_L("New internalized list has different count of iaps"));
		errors = ETrue;
		}
	else
		{
		for(TInt i=0; i<newlist->Count(); i++)
			{
			TIptvIap iap1, iap2;
			TInt ret = newlist->GetIap(i, iap1);
			VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

			ret = iIptvIapList->GetIap(i, iap2);
			VCXLOGLO2("CIptvIapListTest:: GetIap returned: %d", ret);

			if(iap1.iId != iap2.iId)
				{
				VCXLOGLO2("<<<CIptvIapListTest::DoExternalizeAndInternalize: Iap on new list (index %d) has different id", i);
				iLog->Log(_L("Iap on new list has different id"));
				errors = ETrue;
				break;
				}
			if(iap1.iPriority != iap2.iPriority)
				{
				VCXLOGLO2("<<<CIptvIapListTest::DoExternalizeAndInternalize: Iap on new list (index %d) has different priority", i);
				iLog->Log(_L("Iap on new list has different priority"));
				errors = ETrue;
				break;
				}

/*			if(iap1.iValueExists != iap2.iValueExists)
				{
				VCXLOGLO2("<<<CIptvIapListTest::DoExternalizeAndInternalize: Iap on new list (index %d) has different iValueExists", i);
				iLog->Log(_L("Iap on new list has different iValueExists"));
				errors = ETrue;
				break;
				}
*/
			}
		}

	if(errors != EFalse)
		{
		CleanupStack::PopAndDestroy(newlist);
		VCXLOGLO1("<<<CIptvIapListTest::DoExternalizeAndInternalize");
		return KErrCorrupt;
		}

  	if(iIptvIapList)
        {
         delete iIptvIapList;
         iIptvIapList = NULL;
        }

    CleanupStack::Pop(newlist);

    iIptvIapList = newlist;

	VCXLOGLO1("<<<CIptvIapListTest::DoExternalizeAndInternalize");
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CIptvIapListTest::Destroy
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvIapListTest::Destroy( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvIapListTest::Destroy");
    // Print to UI
    _LIT( KIptvIapListTest, "IptvIapListTest" );
    _LIT( KDestroy, "In Destroy" );
    TestModuleIf().Printf( 0, KIptvIapListTest, KDestroy );

    // Print to log file
    iLog->Log( KDestroy );

    if(iIptvIapList)
        {
         delete iIptvIapList;
         iIptvIapList = NULL;
        }
    VCXLOGLO1("<<<CIptvIapListTest::Destroy");
    return KErrNone;

    }

/*
 IMPORT_C TInt AddIap(TIptvIap& aIap, TBool aIgnoreMax);
 IMPORT_C TInt DeleteIap(TIptvIap& aIap);
 IMPORT_C void ExternalizeL(RDesWriteStream& aStream) const;
 IMPORT_C void InternalizeL(RDesReadStream& aStream);
*/


// -----------------------------------------------------------------------------
// CIptvIapListTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CIptvIapListTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
