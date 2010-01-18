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
* Description:   Test module for IptvVodContentClient*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "VCXVodContentApiTest.h"
#include "VCXTestLog.h"

#include "CIptvVodContentClient.h"

#include "CIptvVodContentCategoryBriefDetails.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentContentFullDetails.h"

#include "CIptvUtil.h"
#include "CIptvTestTimer.h"
#include "CIptvTestUtilities.h"
#include "CIptvTestEcgUpdateInfo.h"
#include "TIptvRssSearchQuery.h"
#include <CentralRepository.h>

#include <f32file.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::Delete()
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::Delete");
    DeAllocateResources();
    VCXLOGLO1("<<<CIptvVodContentApiTest::Delete");
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "EnableVerifying", CIptvVodContentApiTest::EnableVerifying ),
        ENTRY( "Verify", CIptvVodContentApiTest::Verify ),
        ENTRY( "DeleteEpgDb", CIptvVodContentApiTest::DeleteEpgDb ),
        ENTRY( "Create", CIptvVodContentApiTest::CreateL ),
        ENTRY( "GetEcgList", CIptvVodContentApiTest::GetEcgList ),
        ENTRY( "EcgCategoryList", CIptvVodContentApiTest::EcgCategoryList ),
        ENTRY( "EcgEcgAllList", CIptvVodContentApiTest::EcgEcgAllList ),
        ENTRY( "CategoryDetails", CIptvVodContentApiTest::CategoryDetails ),
        ENTRY( "ParentCategory", CIptvVodContentApiTest::ParentCategory ),
        ENTRY( "ContentDetailsList", CIptvVodContentApiTest::ContentDetailsList ),
        ENTRY( "VerifyAllCategoryContent", CIptvVodContentApiTest::VerifyAllCategoryContentL ),
        ENTRY( "UpdateEcg", CIptvVodContentApiTest::UpdateEcg ),
        ENTRY( "SetTestCaseTimeOut", CIptvVodContentApiTest::SetTestCaseTimeOut ),
        ENTRY( "SelectIap", CIptvVodContentApiTest::SelectIapL ),
        ENTRY( "GetUpdateTime", CIptvVodContentApiTest::GetUpdateTimeL ),
        ENTRY( "SetSpecialAction", CIptvVodContentApiTest::SetSpecialAction ),
        ENTRY( "DisableThumbnails", CIptvVodContentApiTest::DisableThumbnails ),
        ENTRY( "EnableThumbnails", CIptvVodContentApiTest::EnableThumbnails ),
        ENTRY( "SetParentalControlAge", CIptvVodContentApiTest::SetParentalControlAge ),
        ENTRY( "Search", CIptvVodContentApiTest::Search ),
        ENTRY( "CancelSearch", CIptvVodContentApiTest::CancelSearch ),
        ENTRY( "CancelUpdate", CIptvVodContentApiTest::CancelUpdate ),
        ENTRY( "SetLastPosition", CIptvVodContentApiTest::SetLastPosition ),
        ENTRY( "UpdateAvailable", CIptvVodContentApiTest::UpdateAvailableL ),
        ENTRY( "CategoryCount", CIptvVodContentApiTest::CategoryCountL ),
        ENTRY( "ContentCount", CIptvVodContentApiTest::ContentCountL ),
        ENTRY( "SetMsgWhichToSignal", CIptvVodContentApiTest::SetMsgWhichToSignal ),
        ENTRY( "IgnoreMsg", CIptvVodContentApiTest::IgnoreMsg ),
        ENTRY( "Destroy", CIptvVodContentApiTest::Destroy ),
        };


    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::CreateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::CreateL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::CreateL");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KCreateL, "In CreateL" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KCreateL );
    // Print to log file
    iLog->Log( KCreateL );

    //Script parameters:
    // service id, must be >0
    TPtrC serviceName;

    iMsgWhichToSignal = -1;
    iIgnoreMsg = KIptvTestNoSignal;

    TInt testCaseStatus = KErrNone;

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    if(aItem.GetNextString(serviceName) != KErrNone)
        {
        VCXLOGLO1("Can't read testcase parameter, serviceName/number !!!");
        VCXLOGLO1("<<<CIptvVodContentApiTest::CreateL");
        iLog->Log( _L("**** FAIL ***** Can't read testcase parameter!!!") );
        return KErrGeneral;
        }

    VCXLOGLO2("serviceName/orderNumber: %S", &serviceName);

    TInt timeoutValue;
    if(aItem.GetNextInt(timeoutValue) == KErrNone)
        {
        iTimeoutMinutes = timeoutValue;
        }
    else
        {
        iTimeoutMinutes = 4;
        }

    testCaseStatus = CreateInstance(serviceName);

    iRetryCount = 0;

    VCXLOGLO1("<<<CIptvVodContentApiTest::CreateL");
    return testCaseStatus;
    }


// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::DeleteEpgDb();
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::DeleteEpgDb( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::DeleteEpgDb");

    RFs fs;
    fs.Connect();

    CFileMan* fileMan = CFileMan::NewL(fs);

    TBuf<512> path;

//  CIptvUtil::GetPathL(fs, EIptvPathMyVideos, path, EDriveC);
//  fileMan->RmDir(path);

    CIptvUtil::GetPathL(fs, EIptvPathEcg, path, EDriveC);
    fileMan->RmDir(path);

//  CIptvUtil::GetPathL(fs, EIptvPathMyVideos, path, EDriveE);
//  fileMan->RmDir(path);

    CIptvUtil::GetPathL(fs, EIptvPathEcg, path, EDriveE);
    fileMan->RmDir(path);

    delete fileMan;

    fs.Close();

    VCXLOGLO1("<<<CIptvVodContentApiTest::DeleteEpgDb");

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::GetEcgList();
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::GetEcgList( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::GetEcgList");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KGetEcgList, "In GetEcgList" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KGetEcgList );
    // Print to log file
    iLog->Log( KGetEcgList );

    //Script parameters

    TPtrC  searchStringPtr;

    TInt categoryId = 0;
    TInt from =0;
    TInt amount = 0;
    TInt testCaseStatus = KErrNone;
    TBool isParametersValid = TRUE;

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

   //Script parameters
    if(aItem.GetNextInt(categoryId) != KErrNone)
        {
        isParametersValid = FALSE;
        }

    if(aItem.GetNextString(searchStringPtr) != KErrNone)
        {
        isParametersValid = FALSE;
        }

    if(aItem.GetNextInt(from) == KErrNone)
        {
        //check validity
        if(from<0)
            {
            isParametersValid = FALSE;
            }
        }
    else
        {
        isParametersValid = FALSE;
        }

    if(aItem.GetNextInt(amount) == KErrNone)
        {
        //check validity
        if(amount<0)
            {
            isParametersValid = FALSE;
            }
        }
    else
        {
        isParametersValid = FALSE;
        }

    TBuf<256> searchString(searchStringPtr);
    if(!searchString.Compare(_L("\"\"")) )
        {
        searchString = _L("");
        }

    TUint32 categoryId32 = categoryId;
    if(categoryId == KIptvTestSearchCategoryId)
        {
        categoryId32 = KIptvRssSearchCategoryId;
        }

    VCXLOGLO2("categoryId: %d", categoryId32);
    VCXLOGLO2("searchString: %S", &searchString);
    VCXLOGLO2("from: %d", from);
    VCXLOGLO2("amount: %d", amount);

    if(!isParametersValid)
        {
        VCXLOGLO1("Error in testparameters");
        iLog->Log(_L("Error in testparameters"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::GetEcgList");
        return KErrGeneral;
        }

    //TestGetEcgList(TIptvCategoryId aCategoryId, TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount)
    //testCaseStatus = TestGetEcgList(categoryId, searchString, from,amount);
    testCaseStatus = TestGetEcgList(categoryId32, searchString, from,amount);

    VCXLOGLO1("<<<CIptvVodContentApiTest::GetEcgList");
    return testCaseStatus;
    }



// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::EnableVerifying();
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::EnableVerifying( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::EnableVerifying");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KEnableVerifying , "In EnableVerifying" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KEnableVerifying );
    // Print to log file
    iLog->Log( KEnableVerifying );

    //Script parameters

    TInt result = KErrNone;

    TPtrC caseId;
    TInt countNumber = -1;

    TBool isParametersValid = ETrue;

   //Script parameters
    if(aItem.GetNextString(caseId) != KErrNone)
        {
        isParametersValid = EFalse;
        }

    TInt appendInt = 0;
    TBool append = EFalse;
    if(aItem.GetNextInt(appendInt) == KErrNone)
        {
        append = static_cast<TBool>(appendInt);
        }
   aItem.GetNextInt(countNumber);

   if(!isParametersValid)
        {
        VCXLOGLO1("Error in testparameters");
        iLog->Log(_L("Error in testparameters"));
        result = KErrGeneral;
        }
    else
        {
        iCaseId = caseId;
        if(countNumber >= 0)
            {
            iCaseId.AppendNum(countNumber);
            }
        iIptvTestUtilities->CreateVerifyData(iCaseId, append);
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::EnableVerifying");
    return result;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::Verify();
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::Verify( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::Verify");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KVerify , "In Verify" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KVerify  );
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


    VCXLOGLO1("<<<CIptvVodContentApiTest::Verify");
    return error;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::EcgCategoryList
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::EcgCategoryList( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::EcgCategoryList");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KEcgCategoryList, "In EcgCategoryList" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KEcgCategoryList );
    // Print to log file
    iLog->Log( KEcgCategoryList );

    //Script parameters
    //use KIptvVodContentCategoryRootId

    TInt parentCategoryId = 0;
    TInt testCaseStatus = KErrNone;

    TBool isParametersValid = TRUE;

    if(aItem.GetNextInt(parentCategoryId) != KErrNone)
        {
        isParametersValid = FALSE;
        }

    VCXLOGLO2("parentCategoryId: %d", parentCategoryId);

    if(!isParametersValid)
        {
        VCXLOGLO1("Error in testparameters");
        iLog->Log(_L("Error in testparameters"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::EcgCategoryList");
        return KErrGeneral;
        }

    TUint32 parentCategoryId32 = parentCategoryId;
    if(parentCategoryId == KIptvTestSearchCategoryId)
        {
        parentCategoryId32 = KIptvRssSearchCategoryId;
        }

    //rv = TestGetEcgCategoryList(TIptvCategoryId aParentCategoryId);
    testCaseStatus = TestGetEcgCategoryList( parentCategoryId32 );

    VCXLOGLO1("<<<CIptvVodContentApiTest::EcgCategoryList");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::EcgEcgAllList
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::EcgEcgAllList( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::EcgEcgAllList");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KEcgEcgAllList, "In EcgEcgAllList" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KEcgEcgAllList );
    // Print to log file
    iLog->Log( KEcgEcgAllList );

    TInt testCaseStatus = KErrNone;
    TPtrC searchStringPtr;
    TInt from = 0;
    TInt amount = 0;

    TBool isParametersValid = TRUE;

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    //Script parameters
    if(aItem.GetNextString(searchStringPtr)==KErrNone)
        {
        //check string content
        }
    else
        {
        //isParametersValid = FALSE;
        }

    if(aItem.GetNextInt(from) == KErrNone)
        {
        //check validity
        if(from<0)
            {
            isParametersValid = FALSE;
            }
        }
    else
        {
        isParametersValid = FALSE;
        }

    if(aItem.GetNextInt(amount) == KErrNone)
        {
        //check validity
        if(amount<0)
            {
            isParametersValid = FALSE;
            }
        }
    else
        {
        isParametersValid = FALSE;
        }

    TBuf<2560> searchString(searchStringPtr);
    if(!searchString.Compare(_L("\"\"")) )
        {
        searchString = _L("");
        }

    VCXLOGLO2("searchString: %S", &searchString);
    VCXLOGLO2("from: %d", from);
    VCXLOGLO2("amount: %d", amount);

    if(!isParametersValid)
        {
        VCXLOGLO1("Error in testparameters");
        iLog->Log(_L("Error in testparameters"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::EcgCategoryList");
        return KErrGeneral;
        }

    //rv = TestGetEcgAllList(TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount);
    testCaseStatus = TestGetEcgAllList( searchString, from, amount);

    VCXLOGLO1("<<<CIptvVodContentApiTest::EcgEcgAllList");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::CategoryDetails
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::CategoryDetails( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::CategoryDetails");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KCategoryDetails, "In CategoryDetails" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KCategoryDetails );
    // Print to log file
    iLog->Log( KCategoryDetails );
    TInt testCaseStatus = KErrNone;
    TBool isParametersValid = TRUE;

    //Script parameters
    TInt categoryId = 0;

    if(aItem.GetNextInt(categoryId) != KErrNone)
        {
        isParametersValid = FALSE;
        }

    VCXLOGLO2("categoryId: %d", categoryId);

    if(!isParametersValid)
        {
        VCXLOGLO1("Error in testparameters");
        iLog->Log(_L("Error in testparameters"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::CategoryDetails");
        return KErrGeneral;
        }

    TUint32 categoryId32 = categoryId;
    if(categoryId == KIptvTestSearchCategoryId)
        {
        categoryId32 = KIptvRssSearchCategoryId;
        }

    //testCaseStatus = TestGetEcgCategoryDetails(TIptvCategoryId aCategoryId);
    testCaseStatus = TestGetEcgCategoryDetails(categoryId32);

    VCXLOGLO1("<<<CIptvVodContentApiTest::CategoryDetails");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::ParentCategory
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::ParentCategory( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::ParentCategory");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KParentCategory, "In ParentCategory" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KParentCategory );
    // Print to log file
    iLog->Log( KParentCategory );

    TInt testCaseStatus = KErrNone;
    TBool isParametersValid = TRUE;

    //Script parameters
    TInt categoryId = 0;

    if(aItem.GetNextInt(categoryId) != KErrNone)
        {
        isParametersValid = FALSE;
        }

    if(!isParametersValid)
        {
        VCXLOGLO1("Error in testparameters");
        iLog->Log(_L("Error in testparameters"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::ParentCategory");
        return KErrGeneral;
        }

    TUint32 categoryId32 = categoryId;
    if(categoryId == KIptvTestSearchCategoryId)
        {
        categoryId32 = KIptvRssSearchCategoryId;
        }

    //rv = TestGetParentCategory(TIptvCategoryId aCategoryId);
    testCaseStatus = TestGetParentCategory(categoryId32);

    VCXLOGLO1("<<<CIptvVodContentApiTest::ParentCategory");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::ContentDetailsList
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::ContentDetailsList( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::ContentDetailsList");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KContentDetailsList, "In ContentDetailsList" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KContentDetailsList );
    // Print to log file
    iLog->Log( KContentDetailsList );

    TInt testCaseStatus = KErrNone;
    TBool isParametersValid = TRUE;

    //Script parameters
    TInt contentId = 0;

    if(aItem.GetNextInt(contentId) == KErrNone)
        {
        if(contentId < 0)
            {
            isParametersValid = FALSE;
            }
        }
    else
        {
        isParametersValid = FALSE;
        }

    VCXLOGLO2("contentId: %d", contentId);

    if(!isParametersValid)
        {
        VCXLOGLO1("Error in testparameters");
        iLog->Log(_L("Error in testparameters"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::ContentDetailsList");
        return KErrGeneral;
        }


    testCaseStatus = TestGetContentDetailsList( contentId );

    VCXLOGLO1("<<<CIptvVodContentApiTest::ContentDetailsList");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::VerifyAllCategoryContentL
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::VerifyAllCategoryContentL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::VerifyAllCategoryContentL");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KAllCategoryContentL, "In VerifyAllCategoryContentL" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KAllCategoryContentL );
    // Print to log file
    iLog->Log( KAllCategoryContentL );

    TInt testCaseStatus = KErrNone;
    TBool isParametersValid = ETrue;

    //Script parameters
    TInt parentCategoryId = 0;

    if(aItem.GetNextInt(parentCategoryId) != KErrNone)
        {
        isParametersValid = EFalse;
        }

    if(!isParametersValid)
        {
        VCXLOGLO1("Error in testparameters");
        iLog->Log(_L("Error in testparameters"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::VerifyAllCategoryContentL");
        return KErrGeneral;
        }

    TUint32 parentCategoryId32 = parentCategoryId;
    if(parentCategoryId == KIptvTestSearchCategoryId)
        {
        parentCategoryId32 = KIptvRssSearchCategoryId;
        }

    testCaseStatus = TestVerifyAllCategoryContentL( parentCategoryId32 );

    VCXLOGLO1("<<<CIptvVodContentApiTest::VerifyAllCategoryContentL");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::UpdateEcg
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::UpdateEcg( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::UpdateEcg");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KUpdateEcg, "In UpdateEcg" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KUpdateEcg );
    // Print to log file
    iLog->Log( KUpdateEcg );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TInt testCaseStatus = KErrNone;
    if( aItem.GetNextInt( iRetryCount ) != KErrNone)
        {
        iRetryCount = 0;
        }

    iExpectedThumbnailCount = -1;
    iThumbnailCount = -1;

    VCXLOGLO1("CIptvVodContentApiTest::UpdateEcg -- starting update");
    testCaseStatus = TestUpdateEcg();

    VCXLOGLO1("<<<CIptvVodContentApiTest::UpdateEcg");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::SetTestCaseTimeOut
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::SetTestCaseTimeOut( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::SetTestCaseTimeOut");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KSetTestCaseTimeOut, "In SetTestCaseTimeOut" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KSetTestCaseTimeOut );
    // Print to log file
    iLog->Log( KSetTestCaseTimeOut );

    TBool isParametersValid = TRUE;

    //Script parameters:
     TInt timeOutSeconds = 0;

    if(aItem.GetNextInt(timeOutSeconds) == KErrNone)
        {
        if(timeOutSeconds < 0)
            {
            isParametersValid = FALSE;
            }
        }
    else
        {
        isParametersValid = FALSE;
        }

    if(!isParametersValid)
        {
        VCXLOGLO1("Error in testparameters");
        iLog->Log(_L("Error in testparameters"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::SetTestCaseTimeOut");
        return KErrGeneral;
        }
    VCXLOGLO2("Timer %d seconds", timeOutSeconds);

    TTimeIntervalMicroSeconds32 interval(timeOutSeconds * 1000 * 1000);

    iIptvTestTimer->After( interval );

    VCXLOGLO1("<<<CIptvVodContentApiTest::SetTestCaseTimeOut");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::DisableThumbnails
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::DisableThumbnails( CStifItemParser& /* aItem */ )
    {

    VCXLOGLO1(">>>CIptvVodContentApiTest::DisableThumbnails");
    CRepository* cenRep = CRepository::NewL(KIptvCenRepUid);
    if (cenRep)
        {
        CleanupStack::PushL(cenRep);
        TInt error = cenRep->Set(KIptvCenRepVoDThumbnailsKey, EFalse);
        if (error != KErrNone)
            {
            VCXLOGLO2("Setting cenrep value returned error: %d", error);
            }
        CleanupStack::PopAndDestroy(cenRep);
        }
    VCXLOGLO1("<<<CIptvVodContentApiTest::DisableThumbnails");
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::EnableThumbnails
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::EnableThumbnails( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::EnableThumbnails");
    CRepository* cenRep = CRepository::NewL(KIptvCenRepUid);
    if (cenRep)
        {
        CleanupStack::PushL(cenRep);
        TInt error = cenRep->Set(KIptvCenRepVoDThumbnailsKey, ETrue);
        if (error != KErrNone)
            {
            VCXLOGLO2("Setting cenrep value returned error: %d", error);
            }
        CleanupStack::PopAndDestroy(cenRep);
        }
    VCXLOGLO1("<<<CIptvVodContentApiTest::EnableThumbnails");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::Search
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::Search( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::Search");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KWhere, "In Search" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TPtrC searchStr;
    TInt opt;

    // Get the first string. This can be search string or an option
    if(KErrNone != aItem.GetNextString(searchStr))
        {
        VCXLOGLO1("Parameter search string/option missing!");
        VCXLOGLO1("<<<CIptvVodContentApiTest::Search");
        return KErrArgument;
        }

    //
    // It's an option if it's a number
    //
    TLex lex(searchStr);
    if(lex.Val(opt) == KErrNone)
        {
        VCXLOGLO2("Checking if %d is a search test option.", opt);

        if(opt == KBadCharacters)
            {
            VCXLOGLO1("YES. Searching with bad characters");
            TBuf<50> tempStr;
            tempStr.Append(KBadCharacters);
            iSearchQuery.SetSearchString(tempStr);
            }

        VCXLOGLO1("NO, continuing with normal search.");
        }
    //
    // It's a search string
    //
    VCXLOGLO2("Search: %S", &searchStr);
    iSearchQuery.SetSearchString(searchStr);

    TInt err = iIptvVodContenClient->Search( iSearchQuery );
    if(err != KErrNone)
        {
        VCXLOGLO2("* FAIL * Search returned an error: %d.", err);
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::Search");
    return err;
}

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::CancelSearch
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::CancelSearch( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::CancelSearch");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KWhere, "In CancelSearch" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt err = KErrNone;
    err = iIptvVodContenClient->CancelSearch();
    if(err != KErrNone)
        {
        VCXLOGLO2("* FAIL * CancelSearch returned an error: %d.", err);
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::CancelSearch");
    return err;
}

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::CancelUpdate
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::CancelUpdate( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::CancelUpdate");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KWhere, "In CancelUpdate" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    iIptvVodContenClient->CancelUpdate();

    VCXLOGLO1("<<<CIptvVodContentApiTest::CancelUpdate");
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::SetLastPosition
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::SetLastPosition( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::SetLastPosition");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KWhere, "In SetLastPosition" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TInt contentId;
    TUint32 index;
    TUint32 position;

    TInt temp;

    if(KErrNone != aItem.GetNextInt(contentId))
        {
        VCXLOGLO1("Parameter contentId missing!");
        VCXLOGLO1("<<<CIptvVodContentApiTest::SetLastPosition");
        return KErrArgument;
        }

    if(KErrNone != aItem.GetNextInt(temp))
        {
        VCXLOGLO1("Parameter index missing!");
        VCXLOGLO1("<<<CIptvVodContentApiTest::SetLastPosition");
        return KErrArgument;
        }
    index = temp;

    if(KErrNone != aItem.GetNextInt(temp))
        {
        VCXLOGLO1("Parameter position missing!");
        VCXLOGLO1("<<<CIptvVodContentApiTest::SetLastPosition");
        return KErrArgument;
        }
    position = temp;

    TInt err = KErrNone;
    TRAPD( err2, err = iIptvVodContenClient->SetLastPositionL( contentId, index, position ) );
    if(err2 != KErrNone)
        {
        VCXLOGLO2("* FAIL * SetLastPosition left with %d.", err);
        err = err2;
        }
    else
    if(err != KErrNone)
        {
        VCXLOGLO2("* FAIL * SetLastPosition returned an error: %d.", err);
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::SetLastPosition");
    return err;
}

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::SetParentalControlAge
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::SetParentalControlAge( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::SetParentalControlAge");

    TInt age;
    if(KErrNone != aItem.GetNextInt(age))
    {
        VCXLOGLO1("Parameter age missing!");
        VCXLOGLO1("<<<CIptvVodContentApiTest::SetParentalControlAge");
        return KErrArgument;
    }

    VCXLOGLO2("Set parental control age to: %d", age);
    CRepository* cenRep = CRepository::NewL(KIptvCenRepUid);

    if (cenRep)
        {
        CleanupStack::PushL(cenRep);
        TInt error = cenRep->Set(KIptvCenRepParentControlKey, age);
        if (error != KErrNone)
            {
            VCXLOGLO2("Setting cenrep value returned error: %d", error);
            }
        CleanupStack::PopAndDestroy(cenRep);
        }
    VCXLOGLO1("<<<CIptvVodContentApiTest::SetParentalControlAge");
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::SelectIapL
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::SelectIapL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::SelectIapL");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KSetIap, "In SelectIapL" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KSetIap );
    // Print to log file
    iLog->Log( KSetIap );

    TPtrC iAPName;
    TUint32 iapId = 0;
    TInt rv = KErrNone;

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    if(aItem.GetNextString(iAPName) != KErrNone)
        {
        VCXLOGLO1("Can't read iap name from cfg file");
        VCXLOGLO1("<<<CIptvVodContentApiTest::SelectIapL");
        return KErrArgument;
        }

    if(iIptvTestUtilities->GetIapIdL(iAPName, iapId) == FALSE)
        {
        VCXLOGLO2("Can't find iap, name: %S. Using invalid IAP id: 1000", &iAPName);
        iapId = 1000;
        }
    else
        {
        VCXLOGLO3("Found iap, name: %S, id = %d", &iAPName,iapId);
        }

    rv = SetIap(iapId);

    VCXLOGLO1("<<<CIptvVodContentApiTest::SelectIapL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::GetUpdateTimeL
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::GetUpdateTimeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::GetUpdateTimeL");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KGetUpdateTime, "In GetUpdateTimeL" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KGetUpdateTime );
    // Print to log file
    iLog->Log( KGetUpdateTime );

    TInt rv = KErrNone;

    TInt maxMinutes = -1;
    aItem.GetNextInt(maxMinutes);

    TTime updateTime;

    rv = TestGetUpdateTimeL(updateTime);

    _LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3");
    _LIT(KTimeString,"%-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
    TBuf<30> dateString;
    TBuf<30> timeString;

    updateTime.FormatL(dateString, KDateString);
    updateTime.FormatL(timeString, KTimeString);    ;

    if(rv == KErrNone)
        {
        VCXLOGLO2("UpdateTime from server - date: %S", &dateString);
        VCXLOGLO2("UpdateTime from server - time: %S", &timeString);

       if(maxMinutes >= 0)
            {
            TTime currentTime;
            TTimeIntervalMinutes minutes;

            currentTime.UniversalTime();
            currentTime.MinutesFrom(updateTime, minutes);

            if(minutes.Int() > maxMinutes)
                {
                VCXLOGLO3("Time to last ECG update is more than specified maximum: %d. Minutes to last update: %d", maxMinutes, minutes.Int() );
                currentTime.FormatL(dateString, KDateString);
                currentTime.FormatL(timeString, KTimeString);
                VCXLOGLO2("Current time - date: %S", &dateString);
                VCXLOGLO2("Current time - time: %S", &timeString);
                rv = KErrGeneral;
                }
            }
        }
    else
        {
        VCXLOGLO2("GetUpdateTimeL returned an error: %d", rv);
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::SelectIapL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::UpdateAvailableL
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::UpdateAvailableL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::UpdateAvailableL");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KWhere, "In UpdateAvailableL" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt intArg;
    User::LeaveIfError( aItem.GetNextInt( intArg ) );

    TBool expectedAvailability = static_cast<TBool>( intArg );

    TBool availability( EFalse );

    TInt ret( KErrNone );
    TRAPD(error, ret = iIptvVodContenClient->UpdateAvailableL( availability ));

    if( ret != KErrNone || error != KErrNone )
        {
        VCXLOGLO3("CIptvVodContentApiTest:: UpdateAvailableL left or returned error: returned: %d, leave: %d", ret, error);
        if( error == KErrNone )
            {
            error = ret;
            }
        }
    else if( expectedAvailability != availability )
        {
        VCXLOGLO2("CIptvVodContentApiTest:: UpdateAvailableL availability: %d, it's not what was expected!", availability);
        error = KErrCorrupt;
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::UpdateAvailableL");
    return error;
    }


// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::CategoryCountL
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::CategoryCountL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::CategoryCountL");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KWhere, "In CategoryCountL" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt expectedCount;
    User::LeaveIfError( aItem.GetNextInt( expectedCount ) );

    TInt error = TestCategoryCountL( expectedCount );
    
    VCXLOGLO1("<<<CIptvVodContentApiTest::CategoryCountL");
    return error;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::ContentCountL
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::ContentCountL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::ContentCountL");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KWhere, "In CategoryCountL" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt expectedCount;
    User::LeaveIfError( aItem.GetNextInt( expectedCount ) );

    TInt error = TestContentCountL( expectedCount );
    
    VCXLOGLO1("<<<CIptvVodContentApiTest::ContentCountL");
    return error;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::SetSpecialAction
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::SetSpecialAction( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::SetSpecialAction");

    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KSetSpecialAction, "In SetSpecialAction" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KSetSpecialAction );
    // Print to log file
    iLog->Log( KSetSpecialAction );

    TInt action(-1);

    aItem.GetNextInt(action);

    if(action < 0)
        {
        VCXLOGLO1("Incorrect parameter.");
        VCXLOGLO1("<<<CIptvVodContentApiTest::SetSpecialAction");
        return KErrNone;
        }

    iSpecialAction = static_cast<TIptvVodContentApiTestSpecialAction>(action);

    VCXLOGLO2("Action: %d", action);

    VCXLOGLO1("<<<CIptvVodContentApiTest::SetSpecialAction");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::SetMsgWhichToSignal
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::SetMsgWhichToSignal( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::SetMsgWhichToSignal");

    if(aItem.GetNextInt(iMsgWhichToSignal) != KErrNone)
        {
        VCXLOGLO1("* ERROR * Argument is wrong.");
        VCXLOGLO1("<<<CIptvVodContentApiTest::SetMsgWhichToSignal");
        return KErrArgument;
        }

    VCXLOGLO2("Will signal for %d msg from VOD.", iMsgWhichToSignal);

    VCXLOGLO1("<<<CIptvVodContentApiTest::SetMsgWhichToSignal");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::IgnoreMsg
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::IgnoreMsg( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::IgnoreMsg");

    if(aItem.GetNextInt(iIgnoreMsg) != KErrNone)
        {
        VCXLOGLO1("* ERROR * Argument is wrong.");
        VCXLOGLO1("<<<CIptvVodContentApiTest::IgnoreMsg");
        return KErrArgument;
        }

    VCXLOGLO2("Will ignore next %d msg from VOD.", iIgnoreMsg);

    VCXLOGLO1("<<<CIptvVodContentApiTest::IgnoreMsg");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::Destroy
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::Destroy( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::Destroy");
    // Print to UI
    _LIT( KIptvVodContentApiTest, "IptvVodContentApiTest" );
    _LIT( KDestroy, "In Destroy" );
    TestModuleIf().Printf( 0, KIptvVodContentApiTest, KDestroy );
    // Print to log file
    iLog->Log( KDestroy );

    //Script parameters:
    //None

    TInt testCaseStatus = KErrNone;

    DeAllocateResources();

    VCXLOGLO1("<<<CIptvVodContentApiTest::Destroy");
    return testCaseStatus;
    }

// ============================= CALLBACK FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::EcgCategoryListUpdated
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::HandleEpgManagerMsgL(TInt aMsg, TInt aInfo, TIptvServiceId aServiceId)
{
    VCXLOGLO1(">>>CIptvVodContentApiTest::HandleEpgManagerMsgL");

    VCXLOGLO2("aMsg = %d", aMsg);
    VCXLOGLO2("aInfo = %d", aInfo);
    VCXLOGLO2("aServiceId = %d", aServiceId);

    TInt signalValue = KIptvTestNoSignal;

    switch(aMsg)
        {

        // 137, aInfo is 0
        case KIptvVodUpdateNotStarted:
            {
            VCXLOGLO1("Message: KIptvVodUpdateNotStarted");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvVodUpdateNotStarted"));

            if(iSpecialAction == EIgnoreUpdateNotStarted)
                {
                VCXLOGLO1("Ignoring, special action set.");
                iSpecialAction = ENone;
                break;
                }

            iIptvTestEcgUpdateInfo->EcgUpdateStop(aServiceId, aMsg, aInfo);

            signalValue = KIptvVodUpdateNotStarted;
            }
            break;

        // 139, aInfo is 0
        case KIptvErrorVodNoIap:
            {
            VCXLOGLO1("Message: KIptvErrorVodNoIap");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvErrorVodNoIap"));
            signalValue = aMsg;
            }
            break;

        //// 136,  aInfo is 0
        case KIptvVodUpdateStarted:
            {
            VCXLOGLO1("Message: KIptvVodUpdateStarted");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvVodUpdateStarted"));
            //start 10 min timer to wait download
            iIptvTestTimer->After(iTimeoutMinutes*60*1000000);

            iExpectedThumbnailCount = -1;
            iThumbnailCount = -1;

            CIptvService* service = iIptvTestUtilities->GetServiceFromDb(aServiceId);
            if(service)
                {
                CleanupStack::PushL(service);
                iLastUpdateTimeFromService = service->GetEpgUpdateTimeL();
                CleanupStack::PopAndDestroy(service);
                service = NULL;
                }
            else
                {
                iIptvTestEcgUpdateInfo->WriteLogL(_L("Error, could not get service."));
                }
            }

            break;

        // 146, aInfo is 0
        case KIptvContentUpdateCompleted:
            {
            VCXLOGLO1("Message: KIptvContentUpdateCompleted");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvContentUpdateCompleted"));
            }
            break;

        // 143, parameter is service id
        case KIptvServiceThumbnailDownloaded:
            {
            VCXLOGLO1("Message: KIptvServiceThumbnailDownloaded");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvServiceThumbnailDownloaded"));
            iThumbnailCount++;
            }
            break;

        // 144, aInfo parameter is content id
        case KIptvContentThumbnailDownloaded:
            {
            VCXLOGLO1("Message: KIptvContentThumbnailDownloaded");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvContentThumbnailDownloaded"));
            iThumbnailCount++;
            }
            break;

        // 133, aInfo: KIptvRssParserError (145) or TIptvDlError (defined in this file)
        case KIptvErrorEpgUpdateFailed:
            {
            VCXLOGLO1("Message: KIptvErrorEpgUpdateFailed");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvErrorEpgUpdateFailed"));
            iIptvTestEcgUpdateInfo->EcgUpdateStop(aServiceId, aMsg, aInfo);

            if(iRetryCount > 0)
                {
                iRetryCount--;
                VCXLOGLO2("CIptvVodContentApiTest: Retrying update! Retries left: %d", iRetryCount );
                iIptvTestEcgUpdateInfo->WriteLogL(_L("Retrying update!"));
                User::LeaveIfError( TestUpdateEcg() );
                break;
                }

            iEcgUpdateSuccesfull = EFalse;

            if(EIptvDlNoError != aInfo)
                {
                signalValue = aInfo;
                }
            else
                {
                // Be sure that error is handled if aInfo == 0
                signalValue = aMsg;
                }
            }
            break;

        // 134, aInfo is 0
        case KIptvErrorEpgUpdateSuccessed:
            {
            VCXLOGLO1("Message: KIptvErrorEpgUpdateSuccessed");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Thubmnails downloaded: %d"), iThumbnailCount);
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvErrorEpgUpdateSuccessed"));

            CIptvService* service = iIptvTestUtilities->GetServiceFromDb(aServiceId);
            if(service)
                {
                CleanupStack::PushL(service);

                TTime time = service->GetEpgUpdateTimeL();

                _LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3");
                _LIT(KTimeString,"%-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
                TBuf<30> timeString;

                time.FormatL(timeString, KDateString);
                time.FormatL(timeString, KTimeString);
                VCXLOGLO2("Update time: %S", &timeString);

                time.UniversalTime();
                time.FormatL(timeString, KDateString);
                time.FormatL(timeString, KTimeString);
                VCXLOGLO2("Time now: %S", &timeString);

                if(iLastUpdateTimeFromService == time)
                    {
                    // last update time was not updated, return error
                    signalValue = KErrGeneral;
                    }
                else
                    {
                    signalValue = KErrNone;
                    }
                CleanupStack::PopAndDestroy(service);
                service = NULL;
                }
            else
                {
                iIptvTestEcgUpdateInfo->WriteLogL(_L("Error, could not get service."));
                }

            iEcgUpdateSuccesfull = ETrue;
            iIptvTestEcgUpdateInfo->EcgUpdateStop(aServiceId, aMsg, aInfo);
            }
            break;

        // 135, EPG manager messages sent to client (LIVE TV ONLY)
        case KIptvErrorFailedToLoadPlugin:
            {
            VCXLOGLO1("Message: KIptvErrorFailedToLoadPlugin");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvErrorFailedToLoadPlugin"));
            }
            break;

        // 141, // aInfo is thumbnail count
        case KIptvThumbnailCount:
            {
            VCXLOGLO2("Message: KIptvThumbnailCount: %d", aInfo);
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvThumbnailCount: %d"), aInfo);

            iExpectedThumbnailCount = aInfo;
            iThumbnailCount = 0;
            }
            break;

        case KIptvErrorRssSearchStarted: // 170
            {
            VCXLOGLO1("Message: KIptvErrorRssSearchStarted");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvErrorRssSearchStarted"));
            //start 10 min timer to wait download
            iIptvTestTimer->After(iTimeoutMinutes*60*1000000);
            }
            break;

        case KIptvErrorRssSearchFailed: // 171
            {
            VCXLOGLO1("Message: KIptvErrorRssSearchFailed");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvErrorRssSearchFailed"));

            if(iRetryCount > 0)
                {
                iRetryCount--;
                VCXLOGLO2("CIptvVodContentApiTest: Retrying search! Retries left: %d", iRetryCount );
                iIptvTestEcgUpdateInfo->WriteLogL(_L("Retrying search!"));

                TInt err = iIptvVodContenClient->Search( iSearchQuery );
                if(err != KErrNone)
                    {
                    VCXLOGLO2("* FAIL * Search returned an error: %d.", err);
                    User::Leave(err);
                    }

                break;
                }

            iIptvTestEcgUpdateInfo->EcgUpdateStop(aServiceId, aMsg, aInfo);

            iEcgUpdateSuccesfull = EFalse;

            if(EIptvDlNoError != aInfo)
                {
                signalValue = aInfo;
                }
            else
                {
                //be sure that error is handled if aInfo == 0
                signalValue = aMsg;
                }
            }
            break;

        case KIptvErrorRssSearchSucceed: // 172
            {
            VCXLOGLO1("Message: KIptvErrorRssSearchSucceed");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvErrorRssSearchSucceed"));

            iEcgUpdateSuccesfull = ETrue;
            iIptvTestEcgUpdateInfo->EcgUpdateStop(aServiceId, aMsg, aInfo);
            signalValue = KErrNone;
            }
            break;

        case KIptvErrorRssSearchNoIap: // 173
            {
            VCXLOGLO1("Message: KIptvErrorRssSearchNoIap");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("KIptvErrorRssSearchNoIap"));
            signalValue = aMsg;
            }
            break;

        case KIptvRssSearchContentUpdateCompleted:
            {
            VCXLOGLO1("Message: KIptvRssSearchContentUpdateCompleted");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvRssSearchContentUpdateCompleted"));
            }
            break;
            
        case KIptvRssSearchThumbnailCount:
            {
            VCXLOGLO2("Message: KIptvThumbnailCount: %d", aInfo);
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvThumbnailCount: %d"), aInfo);

            iExpectedThumbnailCount = aInfo;
            iThumbnailCount = 0;
            }
            break;

        case KIptvRssSearchContentThumbnailDownloaded:
            {
            VCXLOGLO2("Message: KIptvRssSearchContentThumbnailDownloaded, id %d", aInfo);
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: KIptvRssSearchContentThumbnailDownloaded, id %d"), aInfo);

            ++iThumbnailCount;
            }
            break;
            
#ifdef __S60_50__
        case KIptvErrorEpgUpdateStopped:
            {
            // Do nothing right now, test script is signaled in other cases.
            VCXLOGLO1("Message: KIptvErrorEpgUpdateStopped");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("KIptvErrorEpgUpdateStopped"));
            }
            break;
#endif

        default:
            {
            VCXLOGLO1("Unknown message, panicing");
            iIptvTestEcgUpdateInfo->WriteLogL(_L("Message: UNKNOWN: aMsg: %d, aInfo: %d, aServiceId: %d"), aMsg, aInfo, aServiceId);
            User::Panic(_L("CIptvVodContentApiTest: Unknown msg"), KErrGeneral);
            }
            break;

        } // Switch

    // Is there message which will be signaled to the script?
    if(signalValue != KIptvTestNoSignal)
        {
        // Is the message to be ignored
        if(aMsg != iIgnoreMsg)
            {
            AsyncCompleted(signalValue);
            }
        iIgnoreMsg = KIptvTestNoSignal;
        }
    // Has script set msg which must be signaled?
    else if(iMsgWhichToSignal != -1 && iMsgWhichToSignal == aMsg)
        {
        AsyncCompleted(aMsg);
        iMsgWhichToSignal = -1;
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::HandleEpgManagerMsgL");
}

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TimerComplete
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::TimerComplete(TInt aTimerId, TInt aError)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::TimerComplete");
    VCXLOGLO2("aTimerId = %d",aTimerId);
    VCXLOGLO2("aError = %d",aError);

    if(aError == KErrNone)
        {
        iLog->Log(_L("*** Fail *** TimeOut!!!"));
        aError = KErrTimedOut;
        iIptvTestEcgUpdateInfo->EcgUpdateStop(iServiceId, aError, 0);
        }
    VCXLOGLO2("aError = %d",aError);
    AsyncCompleted(aError);
    VCXLOGLO1("<<<CIptvVodContentApiTest::TimerComplete");
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::AsyncCompleted
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::AsyncCompleted(TInt aError)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::AsyncCompleted");
    VCXLOGLO2("aError = %d", aError);

    iIptvTestTimer->Cancel();

    if(iSpecialAction == EUpdateSucceedsAlways)
        {
        VCXLOGLO1("Signaling KErrNone, special action EUpdateSucceedsAlways set.");
        aError = KErrNone;
        }

    Signal(aError);
    VCXLOGLO1("<<<CIptvVodContentApiTest::AsyncCompleted");
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
