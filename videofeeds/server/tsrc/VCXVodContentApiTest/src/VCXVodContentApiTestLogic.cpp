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



#include "VCXVodContentApiTest.h"
#include "VCXTestLog.h"
#include "CIptvTestTimer.h"
#include "CIptvMyVideosGlobalFileId.h"
#include "CIptvTestUtilities.h"
#include "CIptvTestEcgUpdateInfo.h"
#include "CIptvMediaContent.h"

#include <mmf/common/mmfcontrollerpluginresolver.h>

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestGetEcgList
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestGetEcgList(TIptvCategoryId aCategoryId, TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::TestGetEcgList");
    TInt testCaseStatus(KErrNone);

    if(!iIptvVodContenClient)
        {
        VCXLOGLO1("No iIptvVodContenClient instance, error in testlogic");
        iLog->Log(_L("*** FAIL *** No iIptvVodContenClient instance, error in testlogic"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetEcgList");
        return KErrGeneral;
        }

    RPointerArray<CIptvVodContentContentBriefDetails> data;
    CleanupResetAndDestroyPushL(data);
    TUint32 totalAmount = 0;

    TRAPD(err, testCaseStatus = iIptvVodContenClient->GetEcgListL(aCategoryId, aSearchString, aFrom, aAmount, totalAmount, data));
    if(err != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetEcgListL caused a leave: %d.", err);
        testCaseStatus = err;
        }
    else
    if(testCaseStatus != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetEcgListL returned an error: %d.", testCaseStatus);
        }
    else
        {
        VCXLOGLO2("totalAmount = %d", totalAmount);

        for (TInt i = 0; i < data.Count(); i++)
            {
             PrintVodContentBriefDetails(*(data[i]));
            }
        }

    CleanupStack::PopAndDestroy(&data);

    VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetEcgList");
    return testCaseStatus;
    }


// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestGetEcgCategoryList
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestGetEcgCategoryList(TIptvCategoryId aParentCategoryId)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::TestGetEcgCategoryList");
    TInt testCaseStatus(KErrNone);

    if(!iIptvVodContenClient)
        {
        VCXLOGLO1("No iIptvVodContenClient instance, error in testlogic");
        iLog->Log(_L("*** FAIL *** No iIptvVodContenClient instance, error in testlogic"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetEcgList");
        return KErrGeneral;
        }

    RPointerArray<CIptvVodContentCategoryBriefDetails> data;
    CleanupResetAndDestroyPushL(data);

    TRAPD(err, testCaseStatus = iIptvVodContenClient->GetEcgCategoryListL(aParentCategoryId, data));
    if(err != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetEcgCategoryListL caused a leave: %d.", err);
        testCaseStatus = err;
        }
    else
    if(testCaseStatus != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetEcgCategoryListL returned an error: %d.", testCaseStatus);
        }
    else
        {
        TInt count = data.Count();

        for (TInt i = 0; i < count; i++)
            {
            PrintVodContentCategoryBriefDetails(*(data[i]));
            }
        }

    CleanupStack::PopAndDestroy(&data);

    VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetEcgCategoryList");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestGetEcgCategoryDetails
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestGetEcgCategoryDetails(TIptvCategoryId aCategoryId)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::TestGetCategoryDetails");
    TInt testCaseStatus(KErrNone);

    if(!iIptvVodContenClient)
        {
        VCXLOGLO1("No iIptvVodContenClient instance, error in testlogic");
        iLog->Log(_L("*** FAIL *** No iIptvVodContenClient instance, error in testlogic"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetEcgList");
        return KErrGeneral;
        }

    CIptvVodContentCategoryBriefDetails* data = CIptvVodContentCategoryBriefDetails::NewL();
    CleanupStack::PushL(data);

    // GetCategoryDetailsL is not supported for search category
    if( aCategoryId != KIptvRssSearchCategoryId )
        {
        TRAPD(err, testCaseStatus = iIptvVodContenClient->GetCategoryDetailsL(aCategoryId, *data));
        if(err != KErrNone)
            {
            VCXLOGLO2("* FAIL * GetCategoryDetailsL caused a leave: %d.", err);
            testCaseStatus = err;
            }
        else
        if(testCaseStatus != KErrNone)
            {
            VCXLOGLO2("* FAIL * GetCategoryDetailsL returned an error: %d.", testCaseStatus);
            }
        else
            {
            PrintVodContentCategoryBriefDetails(*data);
            }
        }
    CleanupStack::PopAndDestroy(data);

    VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetCategoryDetails");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestGetEcgAllList
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestGetEcgAllList(TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::TestGetEcgAllList");
    TInt testCaseStatus(KErrNone);

    if(!iIptvVodContenClient)
        {
        VCXLOGLO1("No iIptvVodContenClient instance, error in testlogic");
        iLog->Log(_L("*** FAIL *** No iIptvVodContenClient instance, error in testlogic"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetEcgList");
        return KErrGeneral;
        }

    RPointerArray<CIptvVodContentContentBriefDetails> data;
    CleanupResetAndDestroyPushL(data);

    TUint32 total=0;

    TRAPD(err, testCaseStatus = iIptvVodContenClient->GetEcgAllListL(aSearchString, aFrom, aAmount, total, data));
    if(err != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetEcgAllListL caused a leave: %d.", err);
        testCaseStatus = err;
        }
    else
    if(testCaseStatus != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetEcgAllListL returned an error: %d.", testCaseStatus);
        }
    else
        {
        VCXLOGLO2("totalAmount = %d", total);

        for (TInt i = 0; i < data.Count(); i++)
            {
            if(!(data[i]))
                {
                VCXLOGLO2("data at %d is NULL!", i);
                testCaseStatus = KErrCorrupt;
                }
            else
                {
                PrintVodContentBriefDetails(*(data[i]));
                }
            }
        }

    CleanupStack::PopAndDestroy(&data);

    VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetEcgAllList");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestGetParentCategory
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestGetParentCategory(TIptvCategoryId aParentCategoryId)
    {
    VCXLOGLO2(">>>CIptvVodContentApiTest::TestGetParentCategory: aParentCategoryId = %d",aParentCategoryId );
    TInt testCaseStatus(KErrNone);
    TInt rv = KErrNone;
    TIptvCategoryId parent = 0;

    if(!iIptvVodContenClient)
        {
        VCXLOGLO1("No iIptvVodContenClient instance, error in testlogic");
        iLog->Log(_L("*** FAIL *** No iIptvVodContenClient instance, error in testlogic"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetEcgList");
        return KErrGeneral;
        }

    RPointerArray<CIptvVodContentCategoryBriefDetails> data;
    CleanupResetAndDestroyPushL(data);

    VCXLOGLO2("Getting category list for: %d.", aParentCategoryId);
    TRAPD(err, testCaseStatus = iIptvVodContenClient->GetEcgCategoryListL(aParentCategoryId, data));
    if(err != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetEcgCategoryListL caused a leave: %d.", err);
        testCaseStatus = err;
        CleanupStack::PopAndDestroy(&data);
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetParentCategory");
        return err;
        }

    if(testCaseStatus != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetEcgCategoryListL returned an error: %d.", testCaseStatus);
        CleanupStack::PopAndDestroy(&data);
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetParentCategory");
        return err;
        }

    TInt count = data.Count();

    for (TInt i = 0; i < count; i++)
        {
        VCXLOGLO2("categoryid = %d", data[i]->iCategoryId);
        TRAP(err, rv = iIptvVodContenClient->GetParentCategoryL(data[i]->iCategoryId, parent));
        VCXLOGLO2("Parent = %d", parent);

        if(err != KErrNone)
            {
            VCXLOGLO2("* FAIL * GetParentCategoryL caused a leave: %d.", err);
            testCaseStatus = err;
            break;
            }
        if(rv != KErrNone)
            {
            VCXLOGLO2("CIptvVodContentApiTest::TestGetParentCategory: GetParentCategoryL returned an error: %d", rv);
            testCaseStatus = rv;
            break;
            }
        if(parent != aParentCategoryId)
            {
            VCXLOGLO1("CIptvVodContentApiTest::TestGetParentCategory: ParentCategory id doesn't match");
            testCaseStatus = KErrGeneral;
            break;
            }

        }


    CleanupStack::PopAndDestroy(&data);

    if(count == 0)
        {
        VCXLOGLO1("No Categories found!!!");
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetParentCategory");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestVerifyAllCategoryContentL
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestVerifyAllCategoryContentL(TIptvCategoryId aParentCategoryId)
    {
    // KIptvVodContentCategoryRootId
    VCXLOGLO2(">>>CIptvVodContentApiTest::TestVerifyAllCategoryContentL: aParentCategoryId = %d",aParentCategoryId );

    TInt testCaseStatus(KErrNone);

    if(!iIptvVodContenClient)
        {
        VCXLOGLO1("No iIptvVodContenClient instance, error in testlogic");
        iLog->Log(_L("*** FAIL *** No iIptvVodContenClient instance, error in testlogic"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestVerifyAllCategoryContentL");
        return KErrGeneral;
        }

    RPointerArray<CIptvVodContentCategoryBriefDetails> categoryList;
    CleanupResetAndDestroyPushL(categoryList);

    testCaseStatus = iIptvVodContenClient->GetEcgCategoryListL(aParentCategoryId, categoryList);
    if(testCaseStatus != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetEcgCategoryListL returned an error: %d.", testCaseStatus);
        CleanupStack::PopAndDestroy(&categoryList);
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestVerifyAllCategoryContentL");
        return testCaseStatus;
        }

    TInt count = categoryList.Count();

    for (TInt i = 0; i < count; i++)
        {
        VCXLOGLO2("categoryid = %d", categoryList[i]->iCategoryId);

        PrintVodContentCategoryBriefDetails( *categoryList[i] );

        testCaseStatus = TestGetEcgCategoryDetails(categoryList[i]->iCategoryId);
        if(testCaseStatus != KErrNone) break;

        RPointerArray<CIptvVodContentContentBriefDetails> ecgList;
        CleanupResetAndDestroyPushL(ecgList);
        TUint32 totalAmount = 0;

        TBuf<10> searchString(_L("") );
        testCaseStatus = iIptvVodContenClient->GetEcgListL(categoryList[i]->iCategoryId, searchString, 0, 0, totalAmount, ecgList);
        if(testCaseStatus != KErrNone)
            {
            VCXLOGLO2("* FAIL * GetEcgListL returned an error: %d.", testCaseStatus);
            CleanupStack::PopAndDestroy(&ecgList);
            break;
            }

        VCXLOGLO2("totalAmount = %d", totalAmount);

        for (TInt e = 0; e < ecgList.Count(); e++)
            {
            PrintVodContentBriefDetails(*(ecgList[e]));

            CIptvVodContentContentFullDetails* contentDetails = CIptvVodContentContentFullDetails::NewL();

            CleanupStack::PushL(contentDetails);

            testCaseStatus = iIptvVodContenClient->GetContentDetailsL(ecgList[e]->iContentId, *contentDetails);
            if(testCaseStatus != KErrNone)
                {
                VCXLOGLO2("* FAIL * GetContentDetailsL returned an error: %d.", testCaseStatus);
                CleanupStack::PopAndDestroy(contentDetails);
                CleanupStack::PopAndDestroy(&ecgList);
                i = 100000;
                break;
                }

            PrintVodContentFullDetails(ecgList[e]->iContentId, *contentDetails);

            CleanupStack::PopAndDestroy(contentDetails);

            }

        CleanupStack::PopAndDestroy(&ecgList);
        }

    CleanupStack::PopAndDestroy(&categoryList);

    if(count == 0)
        {
        VCXLOGLO1("No Categories found!!!");
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::TestVerifyAllCategoryContentL");
    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestGetContentDetailsList
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestGetContentDetailsList(TIptvContentId aContentId)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::TestGetContentDetailsList");
    TInt testCaseStatus(KErrNone);

    if(!iIptvVodContenClient)
        {
        VCXLOGLO1("No iIptvVodContenClient instance, error in testlogic");
        iLog->Log(_L("*** FAIL *** No iIptvVodContenClient instance, error in testlogic"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetEcgList");
        return KErrGeneral;
        }

    CIptvVodContentContentFullDetails* contentDetails = CIptvVodContentContentFullDetails::NewL();

    CleanupStack::PushL(contentDetails);

    TRAPD(err, testCaseStatus = iIptvVodContenClient->GetContentDetailsL(aContentId, *contentDetails));
    if(err != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetContentDetailsL caused a leave: %d.", err);
        testCaseStatus = err;
        }
    else
    if(testCaseStatus != KErrNone)
        {
        VCXLOGLO2("* FAIL * GetContentDetailsL returned an error: %d.", testCaseStatus);
        }
    else
        {
        PrintVodContentFullDetails(aContentId, *contentDetails);
        }

    CleanupStack::PopAndDestroy(contentDetails);

    VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetContentDetailsList");
    return testCaseStatus;
    }


// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestUpdateEcg
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestUpdateEcg( )
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::TestUpdateEcg");
     TInt testCaseStatus(KErrNone);

    if(!iIptvVodContenClient)
        {
        VCXLOGLO1("No iIptvVodContenClient instance, error in testlogic");
        iLog->Log(_L("*** FAIL *** No iIptvVodContenClient instance, error in testlogic"));
        VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetEcgList");
        return KErrGeneral;
        }
     TRAPD(err, testCaseStatus = iIptvVodContenClient->UpdateEcgL( ));

    if(err != KErrNone)
        {
        VCXLOGLO2("* FAIL * UpdateEcgL caused a leave: %d.", err);
        testCaseStatus = err;
        }
    else
    if(testCaseStatus != KErrNone)
        {
        VCXLOGLO2("* FAIL * UpdateEcgL returned an error: %d.", testCaseStatus);
        }
    else
        {
        iIptvTestEcgUpdateInfo->EcgUpdateStart(iServiceId, iIapId);
        }

     VCXLOGLO1("<<<CIptvVodContentApiTest::TestUpdateEcg");
     return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestCategoryCount
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestCategoryCountL( TInt aExpectedCount )
    {
    // KIptvVodContentCategoryRootId
    VCXLOGLO1(">>>CIptvVodContentApiTest::TestCategoryCountL");

    TInt err( KErrNone );

    if( !iIptvVodContenClient )
        {
        err = KErrNotReady;
        VCXLOGLO1("No iIptvVodContenClient instance, error in testlogic");
        iLog->Log(_L("*** FAIL *** No iIptvVodContenClient instance, error in testlogic"));
        }

    RPointerArray<CIptvVodContentCategoryBriefDetails> categoryList;
    CleanupResetAndDestroyPushL( categoryList );

    err = iIptvVodContenClient->GetEcgCategoryListL( 0 /* all categories */, categoryList );
    if( err!= KErrNone )
        {
        VCXLOGLO2("* FAIL * GetEcgCategoryListL returned an error: %d.", err);
        }
    else
        {
        if( categoryList.Count() != aExpectedCount )
            {
            VCXLOGLO3("* FAIL * Count of categories (%d) is not expected (%d).", categoryList.Count(), aExpectedCount );
            }
        }
    CleanupStack::PopAndDestroy( &categoryList );

    VCXLOGLO1("<<<CIptvVodContentApiTest::TestCategoryCountL");
    return err;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestContentCount
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestContentCountL( TInt aExpectedCount )
    {
    // KIptvVodContentCategoryRootId
    VCXLOGLO1(">>>CIptvVodContentApiTest::TestContentCountL");

    TInt err( KErrNone );

    if( !iIptvVodContenClient )
        {
        VCXLOGLO1("No iIptvVodContenClient instance, error in testlogic");
        iLog->Log(_L("*** FAIL *** No iIptvVodContenClient instance, error in testlogic"));
        User::Leave( KErrNotReady );
        }

    RPointerArray<CIptvVodContentCategoryBriefDetails> categoryList;
    CleanupResetAndDestroyPushL( categoryList );

    err = iIptvVodContenClient->GetEcgCategoryListL( 0 /* all categories */, categoryList );    
    
    TInt contentCount( 0 );
    
    TInt categoryCount = categoryList.Count();
    
    if( err!= KErrNone )
        {
        VCXLOGLO2("* FAIL * GetEcgCategoryListL returned an error: %d.", err);
        }
    else
    for (TInt i = 0; i < categoryCount; i++)
        {
        RPointerArray<CIptvVodContentContentBriefDetails> ecgList;
        CleanupResetAndDestroyPushL(ecgList);

        TBuf<10> searchString(_L("") );
        TUint32 totalAmount;
        err = iIptvVodContenClient->GetEcgListL( categoryList[i]->iCategoryId, searchString, 0, 0, totalAmount, ecgList );
        if(err != KErrNone)
            {
            VCXLOGLO2("* FAIL * GetEcgListL returned an error: %d.", err);
            CleanupStack::PopAndDestroy( &ecgList );
            break;
            }

        VCXLOGLO4("Category: %d content count: %d (ta: %d).", i, ecgList.Count(), totalAmount);
        
        contentCount += totalAmount;
        CleanupStack::PopAndDestroy( &ecgList );
        }
    
    if( err == KErrNone )
        {
        if( contentCount != aExpectedCount )
            {
            VCXLOGLO3("* FAIL * Count of content (%d) is not expected (%d).", contentCount, aExpectedCount );
            err = KErrCorrupt;
            }
        }
    CleanupStack::PopAndDestroy( &categoryList );

    VCXLOGLO1("<<<CIptvVodContentApiTest::TestContentCountL");
    return err;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::CreateInstanceL
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::CreateInstance(TDesC& serviceName)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::CreateInstanceL");
    TInt rv(KErrNone);
    TInt error(KErrNone);

    TRAP(error, iIptvTestUtilities = CIptvTestUtilities::NewL( ));
    if(KErrNone != error)//Leave
        {
        if(iIptvTestUtilities)
            {
            delete iIptvTestUtilities;
            iIptvTestUtilities = NULL;
            }
        VCXLOGLO1("Leave in creating CIptvTestUtilities instance");
        iLog->Log(_L("*** Fail *** Leave in creating CIptvTestUtilities instance"));
        return error;
        }

    TUint32 serviceId = 0;
    TInt err = KErrNone;

    TBuf<256> sname(serviceName);
    TInt sid;
    TRAP(err, rv = iIptvTestUtilities->GetServiceIdByNameL(sname, sid));
    serviceId = sid;
    if(KErrNone != err)
        {
        VCXLOGLO2("GetServiceIdByNameL caused a leave: %d.", err);
        VCXLOGLO1("<<<CIptvVodContentApiTest::CreateL");
        return err;
        }
    if(KErrNone != rv)
        {
        VCXLOGLO2("GetServiceIdByNameL returned error: %d.", rv);
        VCXLOGLO1("<<<CIptvVodContentApiTest::CreateL");
        return rv;
        }

    TRAP(error, iIptvVodContenClient = CIptvVodContentClient::NewL(serviceId, *this));

    if(KErrNone != error)//Leave
        {
        if(iIptvVodContenClient)
            {
            delete iIptvVodContenClient;
            iIptvVodContenClient = NULL;
            }
        VCXLOGLO2("Leave in creating CIptvVodContentClient instance: %d", error);
        iLog->Log(_L("*** Fail *** Leave in creating CIptvVodContentClient instance"));
        return error;
        }

    iServiceId = serviceId;
    TRAP(error, iIptvTestTimer = CIptvTestTimer::NewL( *this, 0 ));

    if(KErrNone != error)//Leave
        {
        if(iIptvTestTimer)
            {
            delete iIptvTestTimer;
            iIptvTestTimer = NULL;
            }
        VCXLOGLO1("Leave in creating CIptvTestTimer instance");
        iLog->Log(_L("*** Fail *** Leave in creating CIptvTestTimer instance"));
        return error;
        }

    iIptvTestEcgUpdateInfo = CIptvTestEcgUpdateInfo::NewL();

    VCXLOGLO1("<<<CIptvVodContentApiTest::CreateInstanceL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::DeAllocateResources
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::DeAllocateResources()
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::DeAllocateResources");
    if(iIptvVodContenClient)
        {
        delete iIptvVodContenClient;
        iIptvVodContenClient = NULL;
        }

    if(iIptvTestTimer)
        {
        delete iIptvTestTimer;
        iIptvTestTimer = NULL;
        }

    if(iIptvTestUtilities)
        {
        delete iIptvTestUtilities;
        iIptvTestUtilities = NULL;
        }

    if(iIptvTestEcgUpdateInfo)
        {
        delete iIptvTestEcgUpdateInfo;
        iIptvTestEcgUpdateInfo = NULL;
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::DeAllocateResources");
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::PrintVodContentBriefDetails
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::PrintVodContentBriefDetails(CIptvVodContentContentBriefDetails& aDetails)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::PrintVodContentBriefDetails");

    TInt year, month, day, hour, minute, seconds;
    GetDateInts(aDetails.iPubDate.DateTime(), year, month, day, hour, minute, seconds);

    VCXLOGLO3("Id: %d Name = %S", aDetails.iContentId, &aDetails.iName);
    VCXLOGLO2("iThumbnailPath: %S", &aDetails.iThumbnailPath);

    VCXLOGLO2("iPlaytime: %d", aDetails.iPlaytime);
    VCXLOGLO2("iSize: %d", aDetails.iSize);

    if(aDetails.iFileId)
        {
        VCXLOGLO3("iFileId.iDrive = %d, iFileId: %d", aDetails.iFileId->iDrive, aDetails.iFileId->iFileId);
        }

    VCXLOGLO4("Pub date = %d.%d.%d", year, month, day);


    TPtrC browserUrl = aDetails.GetBrowserUrlL();

    VCXLOGLO2("iBrowserUrl: %S",&browserUrl);


    WriteVerifyVodContentBriefDetails(aDetails);

    VCXLOGLO1("<<<CIptvVodContentApiTest::PrintVodContentBriefDetails");
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::PrintVodContentFullDetails
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::PrintVodContentFullDetails(TUint32  aContentId, CIptvVodContentContentFullDetails& aDetails)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::PrintVodContentFullDetails");

    //
    // REL 1.1 MEDIACONTENT
    //

    VCXLOGLO2("Description = %S", &aDetails.iDescription);
    VCXLOGLO2("iContentProtection = %d", aDetails.iContentProtection);
    VCXLOGLO2("iLangugage = %S", &aDetails.iLanguage);
    VCXLOGLO2("iAuthor = %S", &aDetails.iAuthor);
    VCXLOGLO2("iCopyright = %S", &aDetails.iCopyright);

    //
    // copied from PrintVodContentBriefDetails
    //

    TInt year, month, day, hour, minute, seconds;
    GetDateInts(aDetails.iPubDate.DateTime(), year, month, day, hour, minute, seconds);

    VCXLOGLO3("Id: %d Name = %S", aDetails.iContentId, &aDetails.iName);
    VCXLOGLO2("iThumbnailPath: %S", &aDetails.iThumbnailPath);
    VCXLOGLO2("iPlaytime: %d", aDetails.iPlaytime);
    VCXLOGLO2("iSize: %d", aDetails.iSize);

    if(aDetails.iFileId)
        {
        VCXLOGLO3("iFileId.iDrive = %d, iFileId: %d", aDetails.iFileId->iDrive, aDetails.iFileId->iFileId);
        }

    VCXLOGLO4("Pub date = %d.%d.%d", year, month, day);

    TPtrC browserUrl = aDetails.GetBrowserUrlL();
    VCXLOGLO2("iBrowserUrl: %S",&browserUrl);

    //
    // Write to the verify file
    //
    WriteVerifyVodContentFullDetails(aContentId, aDetails);

    VCXLOGLO1("<<<CIptvVodContentApiTest::PrintVodContentFullDetails");
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::PrintVodContentCategoryBriefDetails
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::PrintVodContentCategoryBriefDetails(CIptvVodContentCategoryBriefDetails& aDetails)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::PrintVodContentCategoryBriefDetails");

    TInt year, month, day, hour, minute, seconds;
    GetDateInts(aDetails.iPubDate.DateTime(), year, month, day, hour, minute, seconds);

    VCXLOGLO3("Id: %d Name = %S",aDetails.iCategoryId, &aDetails.iName);
    VCXLOGLO4("Pub date = %d.%d.%d", year, month, day);
    VCXLOGLO2("iThumbnailPath: %S", &aDetails.iThumbnailPath);
    VCXLOGLO2("iContentCount: %d", aDetails.iContentCount);
    VCXLOGLO2("iIsSearchCategory: %d", aDetails.iIsSearchCategory);

    WriteVerifyVodContentCategoryBriefDetails(aDetails);

    VCXLOGLO1("<<<CIptvVodContentApiTest::PrintVodContentCategoryBriefDetails");
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::WriteVerifyVodContentBriefDetails
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::WriteVerifyVodContentBriefDetails(CIptvVodContentContentBriefDetails& aDetails)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::WriteVerifyVodContentBriefDetails");

    TInt year, month, day, hour, minute, seconds;
    GetDateInts(aDetails.iPubDate.DateTime(), year, month, day, hour, minute, seconds);

    if(iSpecialAction == EBriefContentDetails)
        {
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("VodContentBriefDetails: Id: %d Name = %S"), aDetails.iContentId, &aDetails.iName);
        return;
        }

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("-----------------------------------------------------------"));
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("VodContentBriefDetails"));
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("-----------------------------------------------------------"));

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("Id: %d Name = %S"), aDetails.iContentId, &aDetails.iName);

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iRatingAge: %d"), aDetails.iRatingAge);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iLanguage: %S"), &aDetails.iLanguage);

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iThumbnailPath: %S"), &aDetails.iThumbnailPath);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iPlaytime: %d"), aDetails.iPlaytime);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iSize: %d"), aDetails.iSize);

    if(aDetails.iFileId)
        {
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iFileId.iDrive: %d, iFileId.iFileId: %d"), aDetails.iFileId->iDrive, aDetails.iFileId->iFileId);
        }
    else
        {
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("No iFileId ID!") );
        }

    if(aDetails.iPreviewGlobalFileId)
        {
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iPreviewGlobalFileId.iDrive: %d, iPreviewGlobalFileId.iFileId: %d"), aDetails.iPreviewGlobalFileId->iDrive, aDetails.iPreviewGlobalFileId->iFileId);
        }
    else
        {
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("No iPreviewGlobalFileId ID!") );
        }

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("Pub date = %d.%d.%d"), year, month, day);
    TPtrC browserUrl = aDetails.GetBrowserUrlL();
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iBrowserUrl: %S"), &browserUrl);

    VCXLOGLO1("<<<CIptvVodContentApiTest::WriteVerifyVodContentBriefDetails");
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::WriteVerifyVodContentFullDetails
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::WriteVerifyVodContentFullDetails(TUint32 aContentId, CIptvVodContentContentFullDetails& aDetails)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::WriteVerifyVodContentFullDetails");

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("-----------------------------------------------------------"));
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("VodContentFullDetails"));
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("-----------------------------------------------------------"));

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("aContentId = %d"), aContentId);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("Description = %S"),&aDetails.iDescription);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iContentProtection = %d"),aDetails.iContentProtection);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iLanguage = %S"),&aDetails.iLanguage);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iAuthor = %S"),&aDetails.iAuthor);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iCopyright = %S"),&aDetails.iCopyright);

    //
    // Print content access list
    //
    //IMPORT_C TInt GetContentAccessListL(TUint32 aContentId, RPointerArray<CIptvMediaContent>& aCAList);

    RPointerArray<CIptvMediaContent> CAList;

    TInt err = KErrNone, ret = KErrNone;
    TRAP(err, ret = iIptvVodContenClient->GetContentAccessListL(aContentId, CAList));

    CleanupResetAndDestroyPushL(CAList);

    if(err != KErrNone)
        {
        VCXLOGLO2("* ERROR * GetContentAccessListLApiTest caused a leave!: %d", err);
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("GetContentAccessListLApiTest caused a leave."));
        }
    else if(ret != KErrNone)
        {
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("GetContentAccessListLApiTest returned error"));
        VCXLOGLO2("* ERROR * GetContentAccessListLApiTest returned error: %d", ret);
        }
    else
        {
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" "));
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("Content access list:"));
        WriteVerifyCAList(CAList);
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" "));
        }

    CleanupStack::PopAndDestroy(&CAList);

    // copied from WriteVerifyVodContentBriefDetails

    TInt year, month, day, hour, minute, seconds;
    GetDateInts(aDetails.iPubDate.DateTime(), year, month, day, hour, minute, seconds);

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("Id: %d Name = %S"), aDetails.iContentId, &aDetails.iName);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iThumbnailPath: %S"), &aDetails.iThumbnailPath);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iPlaytime: %d"), aDetails.iPlaytime);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iSize: %d"), aDetails.iSize);

    if(aDetails.iFileId)
        {
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iFileId.iDrive: %d, iFileId.iFileId: %d"), aDetails.iFileId->iDrive, aDetails.iFileId->iFileId);
        }

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("Pub date = %d.%d.%d"), year, month, day);
    TPtrC browserUrl = aDetails.GetBrowserUrlL();
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iBrowserUrl: %S"),&browserUrl);


    VCXLOGLO1("<<<CIptvVodContentApiTest::WriteVerifyVodContentFullDetails");
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::WriteVerifyVodContentCategoryBriefDetails
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::WriteVerifyVodContentCategoryBriefDetails(CIptvVodContentCategoryBriefDetails& aDetails)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::WriteVerifyVodContentCategoryBriefDetails");

    TInt year, month, day, hour, minute, seconds;
    GetDateInts(aDetails.iPubDate.DateTime(), year, month, day, hour, minute, seconds);

    if(iSpecialAction == EBriefContentDetails)
        {
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("VodContentCategoryBriefDetails: Id: %d Name = %S"),aDetails.iCategoryId, &aDetails.iName);
        return;
        }

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("-----------------------------------------------------------"));
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("VodContentCategoryBriefDetails"));
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("-----------------------------------------------------------"));

    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("Id: %d Name = %S"),aDetails.iCategoryId, &aDetails.iName);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("Pub date = %d.%d.%d (OBSOLOTE)"), year, month, day);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iThumbnailPath: %S (OBSOLOTE)"), &aDetails.iThumbnailPath);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iContentCount: %d"), aDetails.iContentCount);
    iIptvTestUtilities->WriteVerifyData(iCaseId, _L("iIsSearchCategory: %d"), aDetails.iIsSearchCategory);

    VCXLOGLO1("<<<CIptvVodContentApiTest::WriteVerifyVodContentCategoryBriefDetails");
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::WriteVerifyCAList
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::WriteVerifyCAList(RPointerArray<CIptvMediaContent>& aCAList)
    {

    for(TInt i = 0; i < aCAList.Count(); i++)
        {

        iIptvTestUtilities->WriteVerifyData(iCaseId, _L("%d."), i);

        CIptvMediaContent *mediaContent = aCAList[i];

        TBuf<256> str;

    /**
        * Content key
        */
        //TUint32 iContentKey;

        /**
        * Index (This index identifies content access objects which belong to some content defined by iContentKey)
        */
        //TUint32 iIndex;

        /**
        * Download type.
        */
        //TUint32 iDownloadType

        switch(mediaContent->iDownloadType)
            {

            case EIptvDownloadTypeStream:
                str = _L("Stream");
                break;

            case EIptvDownloadTypeImmediate:
                str = _L("Immediate");
                break;

            case EIptvDownloadTypePostponed:
                str = _L("Postponed");
                break;

            case EIptvDownloadTypeScheduled:
                str = _L("Scheduled");
                break;

            default:
                str = _L("Unknown");
                break;
        }

        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iDownloadType: %S (%d)"), &str, mediaContent->iDownloadType);

        /**
        * File id (Part of global file id (1/2)
        */
        //TUint32 iFileId;

        /**
        * Drive id (Part of global file id (2/2)
        */
        //TUint32 iDriveId;

        /**
        * Content URL (MRSS)
        */
        //TBuf<KIptvCAContentUrlLength> iUrl;

        const TPtrC url = mediaContent->GetMediaContentUrl();

        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iUrl: %S"), &url);

        /**
        * File size (MRSS)
        */
        //TUint32 iFileSize;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iFileSize: %d"), mediaContent->iFileSize);

        /**
        * Mime-type (MRSS)
        */
        //TBuf<KIptvCAMimeTypeLength> iMimeType;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iMimeType: %S"), &mediaContent->iMimeType);

        /**
        * Medium (MRSS) (Not used in client side)
        * Medium is the type of object (image | audio | video | document | executable). While this
        * attribute can at times seem redundant if type is supplied, it is included because it simplifies
        * decision making on the reader side, as well as flushes out any ambiguities between MIME type and
        * object type. It is an optional attribute.
        */
        //TBuf<KIptvMediumMaxLength> iMedium;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iMedium: %S"), &mediaContent->iMedium);

        /**
        * Is default. (MRSS) (Not used in client side)
        * isDefault determines if this is the default object that should be used for the <media:group>.
        * There should only be one default object per <media:group>. It is an optional attribute.
        */
        //TBool iIsDefault;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iIsDefault: %d"), mediaContent->iIsDefault);

        /**
        * Expression. (MRSS)
        * Expression determines if the object is a sample or the full version of the object, or even if it is a
        * continuous stream (sample | full | nonstop). Default value is 'full'. It is an optional attribute
        */
        //TUint32 iExpression;
        if(mediaContent->iExpression == CIptvMediaContent::EFull) str = _L("FULL");
        if(mediaContent->iExpression == CIptvMediaContent::ESample) str = _L("Sample");
        if(mediaContent->iExpression == CIptvMediaContent::ENonStop) str = _L("NonStop");

        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iExpression: %S (%d)"), &str, mediaContent->iExpression);

        /**
        * Bitrate. (MRSS) (Not used in client side)
        * Bitrate is the kilobits per second rate of media. It is an optional attribute
        */
        //TUint32 iBitrate;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iBitrate: %d"), mediaContent->iBitrate);

        /**
        * Framerate. (MRSS) (Not used in client side)
        * Framerate is the number of frames per second for the media object. It is an optional attribute
        */
        //TUint32 iFramerate;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iFramerate: %d"), mediaContent->iFramerate);

        /**
        * Samplingrate (MRSS) (Not used in client side)
        * Samplingrate is the number of samples per second taken to create the media object. It is expressed in
        * thousands of samples per second (kHz). It is an optional attribute.
        */
        //TUint32 iSamplingrate;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iSamplingrate: %d"), mediaContent->iSamplingrate);

        /**
        * Channels. (MRSS) (Not used in client side)
        * Not supported.
        */

        /**
        * Duration. (MRSS)
        * Duration is the number of seconds the media object plays. It is an optional attribute.
        */
        //TUint32 iDuration;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iDuration: %d"), mediaContent->iDuration);

        /**
        * Height. (MRSS) (Not used in client side)
        * Height is the height of the media object. It is an optional attribute.
        */
        //TUint32 iHeight;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iHeight: %d"), mediaContent->iHeight);

        /**
        * Width. (MRSS) (Not used in client side)
        * Width is the width of the media object. It is an optional attribute.
        */
        //TUint32 iWidth;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iWidth: %d"), mediaContent->iWidth);

        /**
        * Language. (MRSS)
        * Lang is the primary language encapsulated in the media object. Language codes possible are detailed in
        * RFC 3066. This attribute is used similar to the xml:lang attribute detailed in the XML 1.0 Specification
        * (Third Edition). It is an optional attribute.
        */
        //TBuf<KIptvCALanguageLength> iLanguage;
        iIptvTestUtilities->WriteVerifyData(iCaseId, _L(" iLanguage: %S"), &mediaContent->iLanguage);
        }
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::SetIap
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::SetIap(const TUint32 aIap)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::SetIap");
    TInt rv = KErrNone;
    VCXLOGLO2("Selected Iap = %d",aIap);

    TRAPD(error,rv = iIptvVodContenClient->SetIapL(aIap));
    if(KErrNone != error)
        {
        rv =error;
        }
    else
        {
        iIapId = aIap;
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::SetIap");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TestGetUpdateTimeL
// -----------------------------------------------------------------------------
//
TInt CIptvVodContentApiTest::TestGetUpdateTimeL(TTime& aTime)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::TestGetUpdateTimeL");
    TInt rv = KErrNone;

    TRAPD(error, rv = iIptvVodContenClient->GetUpdateTimeL(aTime));

    if(KErrNone != error)
        {
        rv = error;
        }

    VCXLOGLO1("<<<CIptvVodContentApiTest::TestGetUpdateTimeL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::GetDateInts
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::GetDateInts(TDateTime aDateTime, TInt& year, TInt& month, TInt& day, TInt& hour, TInt& minute, TInt& second )
    {
    year = aDateTime.Year();
    month = aDateTime.Month() + 1;
    day = aDateTime.Day() + 1;
    hour = aDateTime.Hour();
    minute = aDateTime.Minute();
    second = aDateTime.Second();
    }


// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::RemoveServiceNumberFromThumbnailPathL
// -----------------------------------------------------------------------------
//
void CIptvVodContentApiTest::RemoveServiceNumberFromThumbnailPathL(TDes& aThumbnailPath)
    {
    VCXLOGLO1(">>>CIptvVodContentApiTest::RemoveServiceNumberFromThumbnailPath");

    HBufC* thumbPath;
    thumbPath = HBufC::NewL(2048);

    CleanupStack::PushL(thumbPath);

    TInt pos;

    TBool success = EFalse;

    pos = aThumbnailPath.Find( _L("ecg\\") );
    if( pos != KErrNotFound )
        {
        thumbPath->Des().Append( aThumbnailPath.Left(pos+4) );

        pos = aThumbnailPath.Find( _L("tbn\\") );
        if( pos != KErrNotFound )
            {
            thumbPath->Des().Append( _L("X") );
            thumbPath->Des().Append( aThumbnailPath.Right( aThumbnailPath.Length() - (pos-1) ) );

            success = ETrue;
            }
        }

    if(success)
        {
        aThumbnailPath = thumbPath->Des();
        }

    CleanupStack::PopAndDestroy(thumbPath);

    VCXLOGLO1("<<<CIptvVodContentApiTest::RemoveServiceNumberFromThumbnailPath");
    }

//  End of File
