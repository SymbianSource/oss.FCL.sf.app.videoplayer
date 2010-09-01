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
#include "IptvServiceManagementApiTest.h"
#include "CIptvServiceManagementSync.h"
#include "CIptvTestUtilities.h"
#include "VCXTestLog.h"
#include "CIptvIapList.h"
#include "TIptvIap.h"
#include <f32file.h>
#include "IptvEngineUids.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT(KUninitializedService, "UNINITIALIZED");

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::UpdateService
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CIptvServiceManagementApiTest::TestUpdateService(CIptvServiceManagementSync::TIptvSmTestConnection aConn, TInt aIdDestination, TInt aIdSource)
    {
    TRespStatus expectedResp = ESucceeded;
    TRespStatus respStatus = ESucceeded;
    TInt testCaseStatus = KErrNone;

    CIptvService* pServiceSource = NULL;
    CIptvService* pServiceDestination = NULL;

    pServiceSource = iIptvTestUtilities->GetServiceFromDb(aIdSource);

    CIptvService* pServiceDest = NULL;
    pServiceDest = iIptvTestUtilities->GetServiceFromDb(aIdDestination);

    // if addresses are same then there won't be an event
    if(pServiceDest && pServiceSource)
    if(pServiceDest->GetAddress() == pServiceSource->GetAddress())
        {
        iIptvServiceManagementSync->SetSpecialAction(CIptvServiceManagementSync::EIptvSmWaitNoEvents);
        }

    delete pServiceDest;
    pServiceDest = NULL;

    // update with uninitialized data
    if(!pServiceSource)
        {
        TBuf<256> uninitializedService(KUninitializedService);
        pServiceSource = GetHardcodedService(uninitializedService, 0);
        }

    if(pServiceSource)
        {
        pServiceSource->SetId(aIdDestination);//set destination id
        if(IsServiceInDb(aIdDestination))
            {
            //lets update

                respStatus = iIptvServiceManagementSync->CallUpdateService(aConn, *pServiceSource, CIptvSmEvent::EServiceModified);
                if(respStatus != ESucceeded)
                    {
                    testCaseStatus = KErrGeneral;
                    }


            }
        else
            {
            //try to update non existing service
            expectedResp = EServiceNotFound;

            respStatus = iIptvServiceManagementSync->CallUpdateService(aConn, *pServiceSource, CIptvSmEvent::EServiceModified);
            if(respStatus != expectedResp)
                {
                VCXLOGLO2("** FAIL ** Response to service request is not correct: %d", respStatus);
                iLog->Log(_L("**** Fail *** Response to service request is not correct"));
                testCaseStatus = KErrGeneral;
                }

            }
        }
    else
        {
        VCXLOGLO1("** FAIL ** Error in testcase, source service not found");
        iLog->Log(_L("*** FAIL *** Error in testcase, source service not found"));
        testCaseStatus = KErrGeneral;
        }

    if(pServiceSource)
        {
        delete pServiceSource;
        pServiceSource = NULL;
        }

    if(pServiceDestination)
        {
        delete pServiceDestination;
        pServiceDestination = NULL;
        }

    return testCaseStatus;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::IsServiceInDb
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TBool CIptvServiceManagementApiTest::IsServiceInDb(TInt aId)
    {
    TBool rv = FALSE;
    CIptvService* p = NULL;
    p = iIptvTestUtilities->GetServiceFromDb(aId);

    if(p)
        {
        rv = TRUE;
        delete p;
        p = NULL;
        }

    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::InitHardcodedServicesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::InitHardcodedServicesL()
    {
    CIptvServiceInfo* serviceInfo;

    //
    // Yle media sali
    //

    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 3;
    serviceInfo->iName.Append(_L("YLE Mediasali"));
    serviceInfo->iAddress.Append(_L("http://193.65.182.78/yle.xml"));
    serviceInfo->iIconPath.Append(_L("c:\\resource\\apps\\logo_yle_24.mif"));
    serviceInfo->iEpgPluginUid.iUid = 0x102750E1;
    serviceInfo->iVodPluginUid.iUid = 0x102750D9;
    serviceInfo->iIptvPluginUid.iUid = 0x00000000;
    serviceInfo->iServiceType = CIptvService::EVod;

    serviceInfo->iUserName.Append(_L("mediacharger"));
    serviceInfo->iPassWord.Append(_L("Buran_9o"));
    serviceInfo->iDesc.Append(_L("YLE Mediasali"));

    serviceInfo->iSelectedFlag = FALSE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = FALSE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x0;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_ylemediasali"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);

    //
    // OOPS
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 0;
    serviceInfo->iName.Append(_L("OOPS"));
    serviceInfo->iAddress.Append(_L("http://193.65.182.78/oops.xml"));
    serviceInfo->iIconPath.Append(_L("c:\\"));
    serviceInfo->iEpgPluginUid.iUid = 0x102750E1;
    serviceInfo->iVodPluginUid.iUid = 0x102750D9;
    serviceInfo->iIptvPluginUid.iUid = 0x00000000;
    serviceInfo->iServiceType = CIptvService::EVod;

    serviceInfo->iUserName.Append(_L("mediacharger"));
    serviceInfo->iPassWord.Append(_L("Buran_9o"));
    serviceInfo->iDesc.Append(_L("OOPS"));

    serviceInfo->iSelectedFlag = TRUE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = TRUE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x1;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_oops"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);


    //
    // OOPS6
    //
    serviceInfo = CIptvServiceInfo::NewL(*serviceInfo);
    serviceInfo->iId = 6;
    serviceInfo->iName.Zero();
    serviceInfo->iName.Append(_L("OOPS6"));
    iHardcodedServices.Append(serviceInfo);
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_oops6"));;

    //
    // OOPS9
    //
    serviceInfo = CIptvServiceInfo::NewL(*serviceInfo);
    serviceInfo->iId = 9;
    serviceInfo->iName.Zero();
    serviceInfo->iName.Append(_L("OOPS9"));
    iHardcodedServices.Append(serviceInfo);
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_oops9"));;


    //
    // Service15Iaps
    //


    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 50;
    serviceInfo->iName.Append(_L("Service15Iaps"));
    serviceInfo->iAddress.Append(_L("http://193.65.182.78/oops.xml"));
    serviceInfo->iIconPath.Append(_L("c:\\"));
    serviceInfo->iEpgPluginUid.iUid = 0x102750E1;
    serviceInfo->iVodPluginUid.iUid = 0x102750D9;
    serviceInfo->iIptvPluginUid.iUid = 0x00000000;
    serviceInfo->iServiceType = CIptvService::EVod;

    serviceInfo->iUserName.Append(_L("mediacharger"));
    serviceInfo->iPassWord.Append(_L("Buran_9o"));
    serviceInfo->iDesc.Append(_L("OOPS"));

    serviceInfo->iSelectedFlag = TRUE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = TRUE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x1;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_15iaps"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    TInt aIapsToAdd = 15;

    serviceInfo->iIapList = CIptvIapList::NewL();

    TInt index = 0;
    TIptvIap iap;
    while(aIapsToAdd--)
        {

        if(index > iIapNumList->Count())
            {
            index = 0;
            }

        if(KErrNone == GetCommDbIapIdByOrder(index, iap.iId) )
            {
            iap.iPriority = index;
            serviceInfo->iIapList->AddIap(iap, ETrue);
            }

        index++;
        }

    iHardcodedServices.Append(serviceInfo);

    //
    // NokiaVideos
    //

    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 1;
    serviceInfo->iName.Append(_L("NokiaVideos"));
    serviceInfo->iAddress.Append(_L("http://193.65.182.78/nokia_vod.xml"));
    serviceInfo->iIconPath.Append(_L("C:\\"));
    serviceInfo->iEpgPluginUid.iUid = 0x102750E1;
    serviceInfo->iVodPluginUid.iUid = 0x102750D9;
    serviceInfo->iIptvPluginUid.iUid = 0x00000000;
    serviceInfo->iServiceType = CIptvService::EVod;

    serviceInfo->iUserName.Append(_L("mediacharger"));
    serviceInfo->iPassWord.Append(_L("Buran_9o"));
    serviceInfo->iDesc.Append(_L("Own TestService"));

    serviceInfo->iSelectedFlag = FALSE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = FALSE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x0;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_nokiavideos"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);

    //
    // NokiaVideos7
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo = CIptvServiceInfo::NewL(*serviceInfo);
    serviceInfo->iId = 7;
    serviceInfo->iName.Zero();
    serviceInfo->iProviderId.Zero();
    serviceInfo->iName.Append(_L("NokiaVideos7"));
    iHardcodedServices.Append(serviceInfo);

    //
    // Kärppäkanava
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 4;
    serviceInfo->iName.Append(_L("Kärppäkanava"));
    serviceInfo->iAddress.Append(_L("http://ws27.oyy.fi/rss/kanava.xml"));
    serviceInfo->iIconPath.Append(_L("c:\\logo_karpat.mif"));
    serviceInfo->iEpgPluginUid.iUid = 0x102750E1;
    serviceInfo->iVodPluginUid.iUid = 0x102750D9;
    serviceInfo->iIptvPluginUid.iUid = 0x00000000;
    serviceInfo->iServiceType = CIptvService::EVod;

    serviceInfo->iUserName.Append(_L(""));
    serviceInfo->iPassWord.Append(_L(""));
    serviceInfo->iDesc.Append(_L("Karppakanava video in hw"));

    serviceInfo->iSelectedFlag = TRUE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = TRUE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x1;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_karppakanava"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);

    //
    // Rocketboom
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 2;
    serviceInfo->iName.Append(_L("Rocketboom"));
    serviceInfo->iAddress.Append(_L("http://193.65.182.78/testingfeed_iptvengine.xml"));
    serviceInfo->iIconPath.Append(_L("C:\\"));
    serviceInfo->iEpgPluginUid.iUid = 0x102750E1;
    serviceInfo->iVodPluginUid.iUid = 0x102750D9;
    serviceInfo->iIptvPluginUid.iUid = 0x00000000;
    serviceInfo->iServiceType = CIptvService::EVod;

    serviceInfo->iUserName.Append(_L("mediacharger"));
    serviceInfo->iPassWord.Append(_L("Buran_9o"));
    serviceInfo->iDesc.Append(_L("Rocketboom"));

    serviceInfo->iSelectedFlag = TRUE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = TRUE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x1;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_rocketboom"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);

    //
    // Rocketboom5
    //

    serviceInfo = CIptvServiceInfo::NewL(*serviceInfo);
    serviceInfo->iId = 5;
    serviceInfo->iName.Zero();
    serviceInfo->iName.Append(_L("Rocketboom5"));
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_rocketboom5"));;
    iHardcodedServices.Append(serviceInfo);

    //
    // Rocketboom8
    //
    serviceInfo = CIptvServiceInfo::NewL(*serviceInfo);
    serviceInfo->iId = 8;
    serviceInfo->iName.Zero();
    serviceInfo->iName.Append(_L("Rocketboom8"));
    iHardcodedServices.Append(serviceInfo);
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_rocketboom8"));;

    //
    // NSeries
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 10;
    serviceInfo->iName.Append(_L("NSeries"));
    serviceInfo->iAddress.Append(_L("http://193.65.182.78/nseries.xml"));
    serviceInfo->iIconPath.Append(_L("C:\\"));
    serviceInfo->iEpgPluginUid.iUid = 0x102750E1;
    serviceInfo->iVodPluginUid.iUid = 0x102750D9;
    serviceInfo->iIptvPluginUid.iUid = 0x00000000;
    serviceInfo->iServiceType = CIptvService::EVod;

    serviceInfo->iUserName.Append(_L("mediacharger"));
    serviceInfo->iPassWord.Append(_L("Buran_9o"));
    serviceInfo->iDesc.Append(_L("NSeries"));

    serviceInfo->iSelectedFlag = TRUE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = TRUE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x1;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_nseries"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);

    //
    // InvalidService3
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 15;
    serviceInfo->iName.Append(_L("InvalidService3"));
    serviceInfo->iAddress.Append(_L("http://"));
    serviceInfo->iIconPath.Append(_L("C:\\"));
    serviceInfo->iEpgPluginUid.iUid = 0x500;
    serviceInfo->iVodPluginUid.iUid = 0x700;
    serviceInfo->iIptvPluginUid.iUid = 0x00000000;
    serviceInfo->iServiceType = CIptvService::EVod;

    serviceInfo->iUserName.Append(_L("username"));
    serviceInfo->iPassWord.Append(_L("password"));
    serviceInfo->iDesc.Append(_L("Invalid testservice2"));

    serviceInfo->iSelectedFlag = TRUE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = TRUE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x1;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_invalidserv3"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);

    //
    // InvalidService4
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 16;
    serviceInfo->iName.Append(_L("InvalidService4"));
    serviceInfo->iAddress.Append(_L("http://www.nokia.com"));
    serviceInfo->iIconPath.Append(_L("C:\\"));
    serviceInfo->iEpgPluginUid.iUid = 0x102750DF;
    serviceInfo->iVodPluginUid.iUid = 0x102750D9;
    serviceInfo->iIptvPluginUid.iUid = 0x00000000;
    serviceInfo->iServiceType = CIptvService::EVod;

    serviceInfo->iUserName.Append(_L("username"));
    serviceInfo->iPassWord.Append(_L("password"));
    serviceInfo->iDesc.Append(_L("Invalid testservice2"));

    serviceInfo->iSelectedFlag = TRUE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = FALSE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x1;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_invalidserv4"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);

    //
    // ServiceNoIaps
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 20;
    serviceInfo->iName.Append(_L("ServiceNoIaps"));
    serviceInfo->iAddress.Append(_L("http://www.nokia.com"));
    serviceInfo->iIconPath.Append(_L("C:\\"));
    serviceInfo->iEpgPluginUid.iUid = 0x102750DF;
    serviceInfo->iVodPluginUid.iUid = 0x102750D9;
    serviceInfo->iIptvPluginUid.iUid = 0x00000000;
    serviceInfo->iServiceType = CIptvService::EVod;

    serviceInfo->iUserName.Append(_L("username"));
    serviceInfo->iPassWord.Append(_L("password"));
    serviceInfo->iDesc.Append(_L("Invalid testservice2"));

    serviceInfo->iSelectedFlag = TRUE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = FALSE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x1;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_servicenoiaps"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);


    //
    // UNINITIALIZED
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 30;

    iHardcodedServices.Append(serviceInfo);

    //
    // Live TV XMLTV
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 51;
    serviceInfo->iName.Append(_L("XmlTvTestService"));
    serviceInfo->iAddress.Append(_L("http://193.65.182.78/dvbtest/xmltv/xmltv_1day.xml"));
    serviceInfo->iIconPath.Append(_L("C:\\"));

    serviceInfo->iEpgPluginUid.iUid = KIptvLiveTvXmlTvPluginUid;
    serviceInfo->iIptvPluginUid.iUid = KIptvLiveTvXmlTvPluginUid;
    serviceInfo->iVodPluginUid.iUid = IPTV_HTTP_VOD_DOWNLOAD_PLUGIN_IMPLEMENTATION_UID;

    serviceInfo->iServiceType = CIptvService::ELiveTv;

    serviceInfo->iUserName.Append(_L("mediacharger"));
    serviceInfo->iPassWord.Append(_L("Buran_9o"));
    serviceInfo->iDesc.Append(_L("XmlTvTestService"));

    serviceInfo->iSelectedFlag = TRUE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = FALSE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x1;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_enginetesting_livetv_xmltv"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);


    //
    // Live TV Thomson
    //
    serviceInfo = CIptvServiceInfo::NewL();

    serviceInfo->iId = 50;
    serviceInfo->iName.Append(_L("ThomsonTestService"));
    serviceInfo->iAddress.Append(_L("http://193.65.64.241/dvb/sdns/service_discovery"));
    serviceInfo->iIconPath.Append(_L("C:\\"));

    serviceInfo->iEpgPluginUid.iUid = KIptvSmartvisionPluginUid;
    serviceInfo->iIptvPluginUid.iUid = KIptvSmartvisionPluginUid;
    serviceInfo->iVodPluginUid.iUid = IPTV_HTTP_VOD_DOWNLOAD_PLUGIN_IMPLEMENTATION_UID;

    serviceInfo->iServiceType = CIptvService::ELiveTv;

    serviceInfo->iUserName.Append(_L("aca"));
    serviceInfo->iPassWord.Append(_L("aca"));
    serviceInfo->iDesc.Append(_L("ThomsonTestService"));

    serviceInfo->iSelectedFlag = TRUE;
    serviceInfo->iReadOnlyFlag = FALSE;
    serviceInfo->iMainServiceFlag = FALSE;
    serviceInfo->iConnectionApprovedFlag = TRUE;
    serviceInfo->iApplicationUid = 0x0;
    serviceInfo->iDisplayOrder = 0x1;
    serviceInfo->iProviderId.Zero();
    serviceInfo->iProviderId.Append(_L("Provider_enginetesting_livetv_thomsonsmartvision"));;
    serviceInfo->iAccountManagementUrl.Append(_L("http://www.nokia.com"));

    iHardcodedServices.Append(serviceInfo);


    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::GetHardcodedService
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CIptvService* CIptvServiceManagementApiTest::GetHardcodedService(TDesC& aHardcodedServiceNameOrID, TDesC& aIapName)
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::GetHardCodedServiceL - iapname");

    CIptvService* service = GetHardcodedService(aHardcodedServiceNameOrID, 0);

    CleanupStack::PushL(service);

    TUint32 iapId;

    if(!iIptvTestUtilities->GetIapIdL(aIapName, iapId) )
        {
        VCXLOGLO2("** FAIL ** Did not find iap with name %S", &aIapName);
        CleanupStack::PopAndDestroy(service);
        return NULL;
        }

    CIptvIapList* iapList = CIptvIapList::NewL();
    CleanupStack::PushL(iapList);

    TIptvIap iap;
    iap.iPriority = 0;
    iap.iId = iapId;

    iapList->AddIap(iap);

    service->SetIapListL(*iapList);

    CleanupStack::PopAndDestroy(iapList);
    CleanupStack::Pop(service);

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetHardCodedServiceL - iapname");

    return service;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::GetHardcodedService
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CIptvService* CIptvServiceManagementApiTest::GetHardcodedService(TDesC& aHardcodedServiceNameOrID, TInt aIapsToAdd)
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::GetHardCodedServiceL");

    if(iHardcodedServices.Count() <= 0)
        {
        InitHardcodedServicesL();
        }

    CIptvServiceInfo* serviceInfo = NULL;

    TBool hardcodedServiceId = -1;

    TInt serviceIdSpecified = CIptvTestUtilities::ParseIntFromString(hardcodedServiceId, aHardcodedServiceNameOrID);

    if(aHardcodedServiceNameOrID.Compare(_L("UNINITIALIZED")) == 0)
        {
        serviceIdSpecified = KErrNone;
        hardcodedServiceId = 30;
        }

    if(serviceIdSpecified == KErrNone)
        {

        //
        // Hardcoded service ID
        //

        if(hardcodedServiceId == 40)
            {
            if(++iHardCodedServiceCounter > 10) iHardCodedServiceCounter = 0;
            hardcodedServiceId = iHardCodedServiceCounter;
            }

        for(TInt i = 0; i < iHardcodedServices.Count(); i++)
            {
            if(hardcodedServiceId == iHardcodedServices[i]->iId)
                {
                serviceInfo = iHardcodedServices[i];
                break;
                }

            }

        }
        else
        {

        //
        // Hardcoded service name
        //

        for(TInt i = 0; i < iHardcodedServices.Count(); i++)
            {
            if(aHardcodedServiceNameOrID.Compare(iHardcodedServices[i]->iName) == 0)
                {

                serviceInfo = iHardcodedServices[i];
                break;
                }

            }
        }

    if(serviceInfo == NULL)
        {
        VCXLOGLO2("No hardcoded service found with: %S", &aHardcodedServiceNameOrID);
        return NULL;
        }


    CIptvService* iptvService = NULL;
    TRAPD( err, iptvService = iIptvTestUtilities->CreateIptvServiceInstanceL(*serviceInfo) );
    if(err != KErrNone)
        {
        VCXLOGLO1("CreateIptvServiceInstanceL caused a leave");
        iLog->Log(_L("CreateIptvServiceInstanceL caused a leave"));
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetHardCodedServiceL");
        return NULL;
        }

    if(serviceInfo->iId == 30) // uninitialized
        {
        return iptvService;
        }

    if(serviceInfo->iIapList == NULL)
        {
        TIptvIap iap;

        CIptvIapList* iapList = CIptvIapList::NewL();
        CleanupStack::PushL(iapList);

        if(aIapsToAdd > 0)
            {
            if(aIapsToAdd > iIapNumList->Count())
                {
                aIapsToAdd = iIapNumList->Count();
                VCXLOGLO2("Not enough iaps, adding only: %d.", aIapsToAdd);
                }

            for(TInt index = 0; index < aIapsToAdd; index++)
                {
                if(!KErrNone == GetCommDbIapIdByOrder(index, iap.iId) )
                    {
                    iap.iPriority = index;
                    iapList->AddIap(iap, ETrue);
                    }
                }
            }
        // add invalid iaps
        else
        if(aIapsToAdd < 0)
            {
            VCXLOGLO1("Adding invalid iaps.");
            aIapsToAdd *= -1;

            for(TInt index = 0; index < aIapsToAdd; index++)
                {
                iap.iId = 100 + index;
                iap.iPriority = index;
                iapList->AddIap(iap, ETrue);
                }
            }

        iptvService->SetIapListL(*iapList);

        CleanupStack::PopAndDestroy(iapList);
        }
    else
        {
        iptvService->SetIapListL(*serviceInfo->iIapList);
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetHardCodedServiceL");
    return iptvService;
    }

//EOF
