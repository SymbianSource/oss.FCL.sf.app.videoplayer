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
#include "CIptvTestMobilecrashWatcher.h"
#include "VCXTestLog.h"
#include <f32file.h>
#include <S32FILE.H>
#include "IptvTestUtilALR.h"

#include <mmf/common/mmfcontrollerpluginresolver.h>
#include "../../../server/IptvScheduledDownload/inc/iptvvodscheduleddownloaddefinitions.h"

#include "CIptvTestServiceStore.h"

//#include <commdb.h>

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
// CIptvServiceManagementApiTest::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementApiTest::Delete()
    {
    DeAllocateResources();
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Create", CIptvServiceManagementApiTest::CreateL ),
        ENTRY( "AddHardCodedService", CIptvServiceManagementApiTest::AddHardCodedServiceL ),
        ENTRY( "UpdateService", CIptvServiceManagementApiTest::UpdateServiceL ),
        ENTRY( "UpdateServiceField", CIptvServiceManagementApiTest::UpdateServiceFieldL ),
        ENTRY( "DeleteService", CIptvServiceManagementApiTest::DeleteServiceL ),
        ENTRY( "DeleteAllServices", CIptvServiceManagementApiTest::DeleteAllServicesL ),
        ENTRY( "GetServices", CIptvServiceManagementApiTest::GetServicesL ),
        ENTRY( "GetServicesByProviderId", CIptvServiceManagementApiTest::GetServicesByProviderIdL ),
        ENTRY( "GetAllSelectedServices", CIptvServiceManagementApiTest::GetAllSelectedServicesL ), // GetFilteredServices should be used
        ENTRY( "GetServicesFiltered", CIptvServiceManagementApiTest::GetServicesFilteredL ),
        ENTRY( "GetServicesByType", CIptvServiceManagementApiTest::GetServicesByTypeL ),
        ENTRY( "ShutdownServer", CIptvServiceManagementApiTest::ShutdownServerL ),
        ENTRY( "SetUsedIap", CIptvServiceManagementApiTest::SetUsedIap ),
        ENTRY( "GetUsedIap", CIptvServiceManagementApiTest::GetUsedIap ),
        ENTRY( "ClearUsedIap", CIptvServiceManagementApiTest::ClearUsedIapL ),
        ENTRY( "SetAllIaps", CIptvServiceManagementApiTest::SetAllIapsL ),
        ENTRY( "SetConnectionAllowed", CIptvServiceManagementApiTest::SetConnectionAllowedL ),
        ENTRY( "IsConnectionAllowed", CIptvServiceManagementApiTest::IsConnectionAllowedL ),
        ENTRY( "VerifyServerIsShutdown", CIptvServiceManagementApiTest::VerifyServerIsShutdownL ),
        ENTRY( "SetSpecialAction", CIptvServiceManagementApiTest::SetSpecialAction),
        ENTRY( "RepeatNextRequest", CIptvServiceManagementApiTest::RepeatNextRequest),

        ENTRY( "DeleteServicesDb", CIptvServiceManagementApiTest::Dummy),
        ENTRY( "Cleanup", CIptvServiceManagementApiTest::Dummy),
        ENTRY( "Initialize", CIptvServiceManagementApiTest::Dummy),

        ENTRY( "Destroy", CIptvServiceManagementApiTest::DestroyL ),
        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }
/*****************************************************/
/***************        Test methods *****************/
/*****************************************************/

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::CreateL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::CreateL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::CreateL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KCreate, "In Create" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KCreate );
    // Print to log file
    iLog->Log( KCreate );

    TInt err = KErrNone;

    TRAP(err, iIptvTestMobilecrashWatcher = CIptvTestMobilecrashWatcher::NewL() );
    if(KErrNone != err)//Leave
        {
        if(iIptvTestMobilecrashWatcher)
            {
            delete iIptvTestMobilecrashWatcher;
            iIptvTestMobilecrashWatcher = NULL;
            }
        VCXLOGLO1("Leave in creating iIptvTestMobilecrashWatcher instance");
        return err;
        }

    TRAP(err, iIptvServiceManagementSync = CIptvServiceManagementSync::NewL() );
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceManagementApiTest:: ** FAIL ** Creating CIptvServiceManagementSync failed!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::CreateL");
        return err;
        }

    CDesC8ArraySeg* servicesArrayDesc = NULL;

    // This will execute the possible provisioning of services.xml
    TRespStatus respStatus = iIptvServiceManagementSync->CallGetServices(CIptvServiceManagementSync::EIptvSmTestSync, 0, 10, &servicesArrayDesc);
    if(respStatus == ESucceeded)
        {
        servicesArrayDesc->Reset();
        delete servicesArrayDesc;
        servicesArrayDesc = NULL;
        }

    // Wait for the provisioning events
    iIptvServiceManagementSync->WaitForProvisioningEvents();

    TRAP(err, iIptvTestUtilities = CIptvTestUtilities::NewL() );
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceManagementApiTest:: ** FAIL ** Creating CIptvTestUtilities failed!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::CreateL");
        return err;
        }

    iServiceStore = CIptvTestServiceStore::NewL(NULL, iIptvTestUtilities);

    // Construct list of iaps

    iIapNumList = new (ELeave) CArrayFixFlat<TUint32>(30);

    TBuf<1024> iapName;

    for(TInt id=0;id<20;id++)
        {
        // If there's iap with the id then add it
        iapName.Zero();
        if(iIptvTestUtilities->GetIapNameById(iapName, id) )
            {
            iIapNumList->AppendL(id);
            }
        }

    iAlrUtil = CIptvTestUtilALR::NewL();

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::CreateL");
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::DestroyL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::DestroyL( CStifItemParser& /*aItem*/ )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::DestroyL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KDestroy, "In Destroy" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KDestroy );
    // Print to log file
    iLog->Log( KDestroy );

    DeAllocateResources();

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::DestroyL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::Dummy
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::Dummy( CStifItemParser& /*aItem*/ )
    {
    return KErrNone;
    }

 // -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::AddHardCodedServiceL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::AddHardCodedServiceL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::AddHardCodedServiceL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KAddHardCodedService, "In AddHardCodedService" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KAddHardCodedService );
    // Print to log file
    iLog->Log( KAddHardCodedService );

    TInt msgTypeInt = 0;
    TPtrC serviceToAdd;
    TInt loopCnt = 1; // Default loop once
    TInt iapsToAdd = 0;
    TInt expectedServiceId = -1;
    TBool iapNameSpecified = EFalse;
    TPtrC iapName;
    TRespStatus respStatus = ESucceeded;
    TRespStatus expectedRespStatus = ESucceeded;
    TInt rv = KErrNone;

    CIptvService* iptvService = NULL;

    // Read parameters

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    aItem.GetNextInt(msgTypeInt);
    aItem.GetNextString(serviceToAdd);
    if(KErrNone != aItem.GetNextString(iapName) )
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter! Iap name or count was not specified.");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::AddHardCodedServiceL");
        return KErrArgument;
        }

    TLex lex(iapName);
    if( KErrNone != lex.Val(iapsToAdd) )
        {
        iapNameSpecified = ETrue;
        }

    aItem.GetNextInt(loopCnt);

    VCXLOGLO2("MsgType: %d", msgTypeInt);
    VCXLOGLO2("ServiceNumberToAdd: %S", &serviceToAdd);
    VCXLOGLO2("IapsToAdd: %d", iapsToAdd);
    VCXLOGLO2("IapName: %d", &iapName);
    VCXLOGLO2("LoopCount: %d", loopCnt);
    VCXLOGLO2("ExpectedServiceId: %d", expectedServiceId);

    if(loopCnt < 0 || msgTypeInt < 0 || msgTypeInt > 1)
        {
        VCXLOGLO1("** FAIL **Invalid testcase parameter!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::AddHardCodedServiceL");
        return KErrArgument;
        }

    CIptvServiceManagementSync::TIptvSmTestConnection msgType = static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    // No full verifying when request is repeated

    TUint repeatRequest = iIptvServiceManagementSync->GetRepeatNextRequestCount();

    for(TInt i = 0; i < loopCnt; i++)
        {
        VCXLOGLO2("loop %d",i);

        // Create service

        if(iapNameSpecified)
            iptvService = GetHardcodedService(serviceToAdd, iapName);
        else
            iptvService = GetHardcodedService(serviceToAdd, iapsToAdd);

        if(iptvService == NULL)
            {
            VCXLOGLO1("** FAIL ** Invalid params? GetHardcodedService failed");
            rv = KErrArgument;
            break;
            }

        CleanupStack::PushL(iptvService);

        // Check provider ID

        TBuf<512> newProviderId(iptvService->GetProviderId());

        iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

        RPointerArray<CIptvService> services;
        iServiceStore->GetServicesByProviderId(services, newProviderId);

        // If there's already service(s) with the provider ID, create an unique ID
        if(services.Count() > 0)
            {
            TUint32 lastServiceId (0);

            if(iServiceStore->ServiceCount() >= 1)
                {
                lastServiceId = iServiceStore->GetService( iServiceStore->ServiceCount()-1 )->GetId();
                newProviderId.AppendNum(lastServiceId);
                }

            iptvService->SetProviderId(newProviderId);
            }

        services.Reset();

        // Do the request

        respStatus = iIptvServiceManagementSync->CallAddService(msgType, *iptvService);

        if(respStatus != expectedRespStatus)
            {
            rv = respStatus;
            break;
            }

        // Verify
        CIptvTestServiceStore* oldServices = CIptvTestServiceStore::NewL( iServiceStore );
        CleanupStack::PushL(oldServices);

        oldServices->AddServiceL( *iptvService );

        iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);
        iServiceStore->PrintServices();

        rv = iServiceStore->Compare( *oldServices );
        if(rv != KErrNone)
            {
            break;
            }

        CleanupStack::PopAndDestroy(oldServices);
        CleanupStack::PopAndDestroy(iptvService);
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::AddHardCodedServiceL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::UpdateServiceL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::UpdateServiceL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::UpdateServiceL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KUpdateService, "In UpdateService" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KUpdateService );
    // Print to log file
    iLog->Log( KUpdateService );

    TInt msgTypeInt(0);
    TInt destinationId(0);
    TInt sourceId(0);
    TBuf<256> destinationName;
    TBuf<256> sourceName;
    TInt rv = KErrNone;
    TPtrC service1, service2;

    // Read parameters

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    aItem.GetNextInt(msgTypeInt);

    rv = GetServiceIdFromParam(aItem, destinationId, destinationName);
    if(rv != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceL");
        return rv;
        }
    rv = GetServiceIdFromParam(aItem, sourceId, sourceName);
    if(rv != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceL");
        return rv;
        }

    iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

    if(sourceId == KLastServiceInDb)
        {
        sourceId = iServiceStore->GetService( iServiceStore->ServiceCount()-1 )->GetId();
        sourceName = iServiceStore->GetService( iServiceStore->ServiceCount()-1 )->GetName();
        }

    VCXLOGLO2("destinationId = %d", destinationId );
    VCXLOGLO2("destinationName = %S", &destinationName );
    VCXLOGLO2("sourceId = %d", sourceId );
    VCXLOGLO2("sourceName = %S", &sourceName );

    if(destinationId < 0 || sourceId < 0 || msgTypeInt < 0 || msgTypeInt > 1)
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceL");
        return KErrArgument;
        }

    CIptvServiceManagementSync::TIptvSmTestConnection msgType = static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    // Do the request

    rv = TestUpdateService(msgType, destinationId, sourceId);
    if(rv == KErrNone)
        {

        // Verify

        CIptvTestServiceStore* oldServices = CIptvTestServiceStore::NewL( iServiceStore );
        CleanupStack::PushL(oldServices);

        iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

        VCXLOGLO1("Services before: ");
        oldServices->PrintServices();
        VCXLOGLO1("Services after: ");
        iServiceStore->PrintServices();

        CIptvService* sourceService;
        CIptvService* destService;

        sourceService = oldServices->GetServiceById( sourceId );
        destService = oldServices->GetServiceById( destinationId );

        if( sourceService && destService )
            {
            oldServices->ReplaceServiceL( destService->GetId(), *sourceService );

            rv = iServiceStore->Compare( *oldServices );
            }

        CleanupStack::PopAndDestroy(oldServices);

        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::UpdateServiceFieldL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::UpdateServiceFieldL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::UpdateServiceFieldL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KUpdateServiceField, "In UpdateServiceFieldL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KUpdateServiceField );
    // Print to log file
    iLog->Log( KUpdateServiceField );

    TInt msgTypeInt(0);
    TInt fieldIdInt(0);
    TInt serviceId(-1);
    TBuf<256> serviceName;
    TInt rv = KErrNone;

    // Read parameters

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    aItem.GetNextInt(msgTypeInt);

    rv = GetServiceIdFromParam(aItem, serviceId, serviceName);
    if(rv != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
        return rv;
        }

    aItem.GetNextInt(fieldIdInt);

    VCXLOGLO3("serviceId, Name = %d, %S", serviceId, &serviceName );
    VCXLOGLO2("fieldId = %d", fieldIdInt );

    iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

    if(iServiceStore->ServiceCount() <= 0)
        {
        VCXLOGLO1("** FAIL ** No services in db, Cannot update!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
        return KErrArgument;
        }

    CIptvServiceManagementSync::TIptvSmTestConnection msgType = static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    TServiceFieldId fieldId = static_cast<TServiceFieldId>(fieldIdInt);

    TPtrC stringValue;
    TInt intValue(0);

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TInt err = KErrNone;

    // disables event verifying. run cases without this line
    iIptvServiceManagementSync->SetSpecialAction(CIptvServiceManagementSync::EIptvSmWaitNoEvents);

/*    // Only updating these fields causes an event
    if(!(fieldId == EAddress || fieldId == EScheduleDlTime || fieldId == EScheduleDlNetwork ||
         fieldId ==  EScheduleDlType || fieldId == EScheduleLastDlTime) )
        {
//        iIptvServiceManagementSync->SetSpecialAction(CIptvServiceManagementSync::EIptvSmWaitNoEvents);
        }*/

    if( (serviceId < 0 || msgTypeInt < 0 || msgTypeInt > 1)
        || (intValue < 0 ) )
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter!");

        VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
        return KErrArgument;
        }

    // Find the service

    CIptvService *service = NULL;
    service = iServiceStore->GetServiceById( serviceId );

    if(service == NULL)
        {
        VCXLOGLO1("** FAIL ** Could not find service.");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
        return KErrArgument;
        }

    // Read the new value and update the service field

    VCXLOGLO1("Updating the field.");

    CIptvSmEvent::TEvent expectedEvent;
    expectedEvent = CIptvSmEvent::EServiceModified;

    switch(fieldId)
        {
        case EName:
            {
            VCXLOGLO1("service->SetName.");
            aItem.GetNextString(stringValue);
            err = service->SetName(stringValue);
            }
            break;

        case EAddress:
            {
            VCXLOGLO1("service->SetAddress.");
            aItem.GetNextString(stringValue);

            HBufC* address = HBufC::NewL(2048 + 128); // 2048 is the max, reserve some extra for too long addresses

            CleanupStack::PushL(address);

            *address = stringValue;

            if(stringValue == _L("Address_2kLongUrl"))
                {
                *address = K2kLongUrl;
                }
            else
            if(stringValue == _L("Address_256CharsLongFileName"))
                {
                *address = K256LongFile;
                }

            // if addresses are same there will not be ServiceModified event
            if(service->GetAddress() == stringValue)
                {
                iIptvServiceManagementSync->SetSpecialAction(CIptvServiceManagementSync::EIptvSmWaitNoEvents);
                }

            TInt len = address->Length();
            VCXLOGLO2("Address length: %d", len);
            err = service->SetAddress(*address);

            CleanupStack::PopAndDestroy(address);
            }
            break;

        case EIconPath:
            {
            VCXLOGLO1("service->SetIconPath.");
            aItem.GetNextString(stringValue);
            err = service->SetIconPath(stringValue);
            }
            break;

        case EEpgPluginUid: // TUid
            {
            VCXLOGLO1("service->SetEpgPluginUid.");
            aItem.GetNextInt(intValue);
            TUid uid;
            uid.iUid = intValue;
            service->SetEpgPluginUid(uid);
            }
            break;

        case EVodPluginUid: // TUid
            {
            VCXLOGLO1("service->SetVodPluginUid.");
            aItem.GetNextInt(intValue);
            TUid uid;
            uid.iUid = intValue;
            service->SetVodPluginUid(uid);
            }
            break;

        case EIptvPluginUid: // TUid
            {
            VCXLOGLO1("service->SetIptvPluginUid.");
            aItem.GetNextInt(intValue);
            TUid uid;
            uid.iUid = intValue;
            service->SetIptvPluginUid(uid);
            }
            break;

        case EServiceType: // TServiceType
            {
            aItem.GetNextInt(intValue);
            VCXLOGLO2("service->SetType (%d).", intValue);
            service->SetType( static_cast<CIptvService::TServiceType>(intValue) );
            }
            break;

        case ESetIapList: // TInt count of iaps
            {
            VCXLOGLO1("service->SetIapListL.");
            TInt index;
            TIptvIap iap;

            TPtrC iapName;

            intValue = 1; // otherwise parameter check later will fail

            CIptvIapList *iapList;
            iapList = CIptvIapList::NewL();
            CleanupStack::PushL(iapList);

            if(KErrNone != aItem.GetNextString(iapName) )
                {
                // empty iap list

                TRAPD(err2, err = service->SetIapListL(*iapList) );

                CleanupStack::PopAndDestroy(iapList);

                if(err2 != KErrNone)
                    {
                    VCXLOGLO1("** FAIL ** SetIapListL (empty) caused a leave.");
                    VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
                    return err2;
                    }
                break;
                }

            // parameter can be iap count or list of iap names and priorities

            TBool iapNameSpecified = EFalse;

            TLex lex(iapName);
            if( KErrNone !=  lex.Val(intValue) )
                {
                iapNameSpecified = ETrue;
                }

            // Iap count was specified
            if(!iapNameSpecified)
                {

                if(intValue >= 0)
                    {
                    if(intValue > iIapNumList->Count())
                        {
                        intValue = iIapNumList->Count();
                        VCXLOGLO2("Not enough iaps, adding only: %d.", intValue);

                        }

                    for(index = 0; index < intValue; index++)
                        {
                        GetCommDbIapIdByOrder(index, iap.iId);
                        iap.iPriority = index;
                        iapList->AddIap(iap, ETrue);
                        }
                    }
                // add invalid iaps
                else
                    {
                    VCXLOGLO1("Adding invalid iaps.");

                    intValue *= -1;

                    if(intValue > iIapNumList->Count())
                        {
                        intValue = iIapNumList->Count();
                        }

                    for(index = 0; index < intValue; index++)
                        {
                        iap.iId = 100 + index;
                        iap.iPriority = index;
                        iapList->AddIap(iap, ETrue);
                        }
                    }
                }

            // Get the iap names and priorities
            else
                {
                while(1)
                    {
                    TInt prio;
                    if(KErrNone == aItem.GetNextInt(prio) )
                        {
                        TUint32 iapId;
                        TBufC<256> name(iapName);

                        if(iIptvTestUtilities->GetIapIdL(name, iapId))
                            {
                            iap.iId = iapId;
                            iap.iPriority = prio;
                            iapList->AddIap(iap, ETrue);
                            }
                        else
                            {
                            VCXLOGLO2("** FAIL ** Test case error. No iap with name: %S.", &iapName);
                            CleanupStack::PopAndDestroy(iapList);

                            VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
                            return KErrArgument;
                            }

                        }
                    else
                        {
                        VCXLOGLO2("** FAIL ** Test case error. No priority specified for iap: %S", &iapName);
                        CleanupStack::PopAndDestroy(iapList);

                        VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
                        return KErrArgument;
                        }

                    if(KErrNone != aItem.GetNextString(iapName) )
                        {
                        VCXLOGLO1("All iaps added.");
                        break;
                        }
                    }
                }

            // set service iaplist
            TRAPD(err2, err = service->SetIapListL(*iapList) );

            CleanupStack::PopAndDestroy(iapList);

            if(err2 != KErrNone)
                {
                VCXLOGLO1("** FAIL ** SetIapListL caused a leave.");
                VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
                return err2;
                }

            }
            break;

        case EUserName:
            {
            VCXLOGLO1("service->SetUserName.");
            aItem.GetNextString(stringValue);
            err = service->SetUserName(stringValue);
            }
            break;

        case EPassword:
            {
            VCXLOGLO1("service->SetPassword.");
            aItem.GetNextString(stringValue);
            err = service->SetPassword(stringValue);
            }
            break;

        case EDesc:
            {
            VCXLOGLO1("service->SetDesc.");
            aItem.GetNextString(stringValue);
            err = service->SetDesc(stringValue);
            }
            break;

        case EFlags:
            {
            VCXLOGLO1("service->SetFlags with next flags.");
            TUint flags = 0;

            while( KErrNone == aItem.GetNextInt(intValue) )
                {
                flags |= intValue;

                if(intValue & CIptvService::ESelected)
                    {
                    VCXLOGLO1("ESelected");
                    }
                if(intValue & CIptvService::EReadOnly)
                    {
                    VCXLOGLO1("EReadOnly");
                    }
                if(intValue & CIptvService::EMainService)
                    {
                    VCXLOGLO1("EMainService");
                    }
                if(intValue & CIptvService::EConnectionApproved)
                    {
                    VCXLOGLO1("EConnectionApproved");
                    }
                if(intValue & CIptvService::EReadOnlyIaps)
                    {
                    VCXLOGLO1("EReadOnlyIaps");
                    }
                if(intValue & CIptvService::EGroupedService)
                    {
                    VCXLOGLO1("EGroupedService");
                    }
                if(intValue & CIptvService::EGroupReceived)
                    {
                    VCXLOGLO1("EGroupReceived");
                    }
                }

			VCXLOGLO2("old selection: %d", service->GetFlags() & CIptvSmEvent::EServiceSelectionModified);
			VCXLOGLO2("new selection: %d", flags & CIptvSmEvent::EServiceSelectionModified);


			if( (service->GetFlags() & CIptvSmEvent::EServiceSelectionModified)
			    != (flags & CIptvSmEvent::EServiceSelectionModified) )
				{
				// Service selection was modified. Expect event.
				expectedEvent = CIptvSmEvent::EServiceSelectionModified;
				}
			else
				{
				// Service selection was not modified
				iIptvServiceManagementSync->SetSpecialAction(CIptvServiceManagementSync::EIptvSmWaitNoEvents);
				}

            service->SetFlags(flags);
            if(service->GetFlags() != flags)
                {
                VCXLOGLO1("CIptvService::GetFlags returned wrong flags right after SetFlags.");
                }

            VCXLOGLO4("Service ID: %d, Name: %S, Flags: 0x%x", service->GetId(), &service->GetName(), flags);

            }
            break;

        case EApplicationUid: // TInt32
            {
            VCXLOGLO1("service->SetApplicationUid.");
            aItem.GetNextInt(intValue);
            service->SetApplicationUid(intValue);
            }
            break;

        case EDisplayOrder: // TUint32
            {
            VCXLOGLO1("service->SetDisplayOrder.");
            aItem.GetNextInt(intValue);
            service->SetDisplayOrder(intValue);
            }
            break;

        case EProviderId: // TDesC
            {
            VCXLOGLO1("service->SetProviderId.");
            aItem.GetNextString(stringValue);
            err = service->SetProviderId(stringValue);
            }
            break;

        case EAccountManagementUrl:
            {
            VCXLOGLO1("service->SetAccountManagementUrl.");
            aItem.GetNextString(stringValue);
            err = service->SetAccountManagementUrl(stringValue);
            }
            break;

        case EScheduleDlTime:
            {
            expectedEvent = CIptvSmEvent::EServiceScheduleModified;

            TUint flags = 0;

            while( KErrNone == aItem.GetNextInt(intValue) )
                {
                flags |= intValue;

                if(intValue == 0 )
                    {
                    VCXLOGLO1("ENoSchedule");
                    flags = 0;
                    break;
                    }
                if(intValue == 1 )
                    {
                    VCXLOGLO1("ENight");
                    }
                if(intValue == 2 )
                    {
                    VCXLOGLO1("EMorning");
                    }
                if(intValue == 4 )
                    {
                    VCXLOGLO1("ENoon");
                    }
                if(intValue == 8 )
                    {
                    VCXLOGLO1("EAfternoon");
                    }
                if(intValue == 16 )
                    {
                    VCXLOGLO1("EEvening");
                    }
                }

            VCXLOGLO1("service->SetScheduleDlTime.");
            service->SetScheduleDlTime(flags);
            }
            break;

        case EScheduleDlNetwork:
            {
            expectedEvent = CIptvSmEvent::EServiceScheduleModified;

            TInt network;
            if(KErrNone != aItem.GetNextInt(network) )
                {
                VCXLOGLO1("** FAIL ** Specify parameter: network type be used for scheduled download");
                VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
                return KErrArgument;
                }

            VCXLOGLO1("service->SetScheduleDlNetwork.");
            service->SetScheduleDlNetwork(network);

            }
            break;

        case EScheduleLastDlTime:
            {
            expectedEvent = CIptvSmEvent::EServiceScheduleModified;

            VCXLOGLO1("service->SetScheduledLastDownloadTime.");
            TTime time;
            time.UniversalTime();
            service->SetScheduledLastDownloadTime(time);

            TTime gotTime;
            gotTime = service->GetScheduledLastDownloadTime();

            _LIT(KDateTimeString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
            TBuf<256> str;

            if(gotTime != time)
                {
                VCXLOGLO1("GetScheduledLastDownloadTime does not match to what was set!");

                TRAPD(err, gotTime.FormatL(str, KDateTimeString));
                if(err != KErrNone)
                    {
                    VCXLOGLO2("TTime::FormatL failed for gotTime: %d", err);
                    }
                VCXLOGLO2("gotTime: %S", &str);

                TRAP(err, time.FormatL(str, KDateTimeString));
                if(err != KErrNone)
                    {
                    VCXLOGLO2("TTime::FormatL failed for time: %d", err);
                    }
                VCXLOGLO2("time: %S", &str);

                VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
                return KErrCorrupt;
                }

            }
            break;

        case EScheduleDlType:
            {
            expectedEvent = CIptvSmEvent::EServiceScheduleModified;

            TInt dlType;
            TInt dlCount = 0;
            if(KErrNone != aItem.GetNextInt(dlType))
                {
                VCXLOGLO1("** FAIL ** Specify parameter: dl type to be used for scheduled download");
                VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
                return KErrArgument;
                }

            TInt type = 0;

            if(dlType != EIptvTestNoDownload)
                {
                if(KErrNone != aItem.GetNextInt(dlCount))
                    {
                    VCXLOGLO1("** FAIL ** Specify parameter: dl count to be used for scheduled download");
                    VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
                    return KErrArgument;
                    }
                }

            type |= dlCount;

            switch(dlType)
                {
                case EIptvTestNoDownload:
                    {
                    type = 0;
                    }
                    break;

                case EIptvTestDownloadLatest:
                    {
                    type |= EDownloadLatest;
                    }
                    break;

                case EIptvTestDownloadOldest:
                    {
                    type |= EDownloadOldest;
                    }
                    break;

                case EIptvTestDownloadAddedAfterLastScheduledDownload:
                    {
                    type |= EDownloadAddedAfterLastScheduledDownload;
                    }
                    break;

                case EIptvTestDownloadAll:
                    {
                    type |= EDownloadAll;
                    }
                    break;

                default:
                    {
                    VCXLOGLO1("** FAIL ** Uknown dl type for scheduled download");
                    VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
                    return KErrArgument;
                    }
                }

            VCXLOGLO1("service->SetScheduleDlType.");
            service->SetScheduleDlType(type);

            }
            break;

        case ESearchUrl:
            {
            VCXLOGLO1("service->SetSearchUrl.");
            aItem.GetNextString(stringValue);

            HBufC* address = HBufC::NewL(2048 + 128); // 2048 is the max, reserve some extra for too long addresses

            CleanupStack::PushL(address);

            *address = stringValue;

            if(stringValue == _L("Address_256charsLongSearchUrl"))
                {
                *address = K256charsLongSearchUrl;
                }

            service->SetSearchUrlL( address->Des() );

            CleanupStack::PopAndDestroy(address);
            }
            break;

        default:
            {
            VCXLOGLO2("** FAIL ** Invalid field id parameter! %d", fieldId);
            VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
            return KErrArgument;
            }
        }

    VCXLOGLO2("intValue = %d", intValue);
    VCXLOGLO2("stringValue = %S", &stringValue);

    if(err != KErrNone)
        {
        VCXLOGLO1("** FAIL ** Service set method returned error.");

        VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
        return err;
        }

    // Do the request
    TRespStatus respStatus = iIptvServiceManagementSync->CallUpdateService(msgType, *service, expectedEvent);

    if(respStatus != ESucceeded)
        {
        VCXLOGLO2("** FAIL ** Updating service failed: status = %d", respStatus);
        rv = respStatus;
        }
    else
        {
        // Verify
        CIptvTestServiceStore* oldServices = CIptvTestServiceStore::NewL( iServiceStore );
        CleanupStack::PushL(oldServices);

        oldServices->ReplaceServiceL( service->GetId(), *service );

        iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);
        rv = iServiceStore->Compare( *oldServices );

        CleanupStack::PopAndDestroy(oldServices);
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::UpdateServiceFieldL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::DeleteServiceL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::DeleteServiceL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::DeleteServiceL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KDeleteService, "In DeleteService" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KDeleteService );
    // Print to log file
    iLog->Log( KDeleteService );

    TRespStatus expectedRespStatus = ESucceeded;
    TRespStatus respStatus = ESucceeded;
    TInt msgTypeInt(0);
    TInt serviceId(-1);
    TBuf<256> serviceName;
    TInt rv = KErrNone;

    // Read parameters

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    aItem.GetNextInt(msgTypeInt);

    rv = GetServiceIdFromParam(aItem, serviceId, serviceName);
    if(rv != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::DeleteServiceL");
        return rv;
        }

    VCXLOGLO3("ServiceId = %d, name: %S",serviceId, &serviceName );

    if(serviceId < 0 || msgTypeInt < 0 || msgTypeInt > 1)
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::DeleteServiceL");
        return KErrArgument;
        }

    CIptvServiceManagementSync::TIptvSmTestConnection msgType = static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    // Do the request

    TUint repeatRequest = iIptvServiceManagementSync->GetRepeatNextRequestCount();
    if(repeatRequest == 0) repeatRequest = 1; // Still need to do this once

    iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

        CIptvTestServiceStore* oldServices = CIptvTestServiceStore::NewL( iServiceStore );
        CleanupStack::PushL(oldServices);

    respStatus = iIptvServiceManagementSync->CallDeleteService(msgType, serviceId);

    if(respStatus != expectedRespStatus)
        {
        VCXLOGLO3("** FAIL ** Service return value was not expected: expected %d, returned %d", expectedRespStatus, respStatus);
        rv = respStatus;
        }
    else
        {
        // Verify
        if( repeatRequest <= 1 )
            {
            iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

            CIptvService* service(NULL);
            service = oldServices->GetServiceById( serviceId );
            if(service)
                {
                oldServices->DeleteService( service->GetId() );

                rv = iServiceStore->Compare( *oldServices );
                }
            }
        }

    CleanupStack::PopAndDestroy(oldServices);

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::DeleteServiceL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::DeleteAllServicesL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::DeleteAllServicesL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::DeleteAllServicesL");

    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KWhere, "In DeleteAllServicesL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt rv = KErrNone;
    TRespStatus respStatus = ESucceeded;
    TInt msgTypeInt(0);

    // Read parameters

    aItem.GetNextInt(msgTypeInt);

    if(msgTypeInt < 0 || msgTypeInt > 1)
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::DeleteServiceL");
        return KErrArgument;
        }

    CIptvServiceManagementSync::TIptvSmTestConnection msgType = static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

    VCXLOGLO2("Services found: %d", iServiceStore->ServiceCount() );

    /* If there is a deletion of group going on .. */
    RPointerArray<CIptvService> services;

    /*for(i = 0; i < iServiceStore->ServiceCount(); i++)*/
    while( iServiceStore->ServiceCount() > 0)
        {
        iServiceStore->GetServices( services );

        CIptvService* iptvService = NULL;
        iptvService = services[0];

        VCXLOGLO2("Deleting service: %d", iptvService->GetId());

        if( iptvService->GetType() == CIptvService::EServiceGroup )
            {
            respStatus = iIptvServiceManagementSync->CallDeleteService( msgType, iptvService->GetId() );

            /* Grouped services are also deleted, first get the grouped because they still exist */
            iServiceStore->GetGroupedServicesByGroupId( services, iptvService->GetGroupId() );
            iServiceStore->DeleteService( iptvService->GetId() );

            /* Increase expected event count by the count of grouped services*/
            if( services.Count() > 0 )
                {
                iIptvServiceManagementSync->IncreaseExpectedEventCount( services.Count() );
                services.Reset();
                }
            }
        else
            {
            /* Delete a single service, also from servicestore */
            respStatus = iIptvServiceManagementSync->CallDeleteService(msgType, iptvService->GetId());
            iServiceStore->DeleteService( iptvService->GetId() );
            }

        if(respStatus != ESucceeded)
            {
            VCXLOGLO2("** FAIL **  DeleteServiceRequest failed, response = %d",respStatus);
            rv = respStatus;
            break;
            }

        services.Reset();
        }

    iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

    if(iServiceStore->ServiceCount() != 0)
        {
        VCXLOGLO1("* FAIL * Service DB has services.");
        rv = KErrCorrupt;
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::DeleteAllServicesL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::GetServicesByProviderIdL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::GetServicesByProviderIdL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::GetServicesByProviderIdL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KGetServicesByProviderId, "In GetServicesByProviderIdL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KGetServicesByProviderId );
    // Print to log file
    iLog->Log( KGetServicesByProviderId );

    TRespStatus respStatus = ESucceeded;
    TInt rv = KErrNone;
    CIptvServices* services = NULL;
    TPtrC providerId;

    // Read parameters

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    if(aItem.GetNextString(providerId) == KErrNone)
        {
        VCXLOGLO2("Parameter provider id: %S", &providerId);
        }
    else
        {
        VCXLOGLO1("** FAIL ** Reading parameter provider id failed");
        VCXLOGLO1("<<<IptvServiceManagementApiTest::GetServicesByProviderIdL");
        return KErrArgument;
        }

    services = CIptvServices::NewL();
    CleanupStack::PushL(services);

    TRAP(rv, respStatus = iIptvServiceManagementSync->CallGetServicesByProviderId(providerId, *services));

    if(rv != KErrNone)
        {
        VCXLOGLO2("** FAIL ** CallGetServicesByProviderId left: %d.", rv);
        }
    else
    if(respStatus != ESucceeded)
        {
        VCXLOGLO2("** FAIL **  GetServicesByProviderId failed, response = %d",respStatus);
        rv = respStatus;
        }
    else
        {
        // Verify
        CIptvTestServiceStore* gotServices = CIptvTestServiceStore::NewL( services );
        CleanupStack::PushL( gotServices );

        CIptvTestServiceStore* verifyServices = CIptvTestServiceStore::NewL( );
        CleanupStack::PushL( verifyServices );

        iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);
        iServiceStore->PrintServices();

        TInt i;
        for(i=0; i<iServiceStore->ServiceCount(); i++)
            {
            CIptvService *service;
            service = iServiceStore->GetService(i);
            if(service->GetProviderId() == providerId)
                {
                verifyServices->AddServiceL( *service );
                }
            }

        rv = verifyServices->Compare( *gotServices );

        CleanupStack::PopAndDestroy(verifyServices);
        CleanupStack::PopAndDestroy(gotServices);
        }

    CleanupStack::PopAndDestroy(services);

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetServicesByProviderIdL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::SetUsedIap
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::SetUsedIap( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::SetUsedIap");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KSetUsedIap, "In SetUsedIap" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KSetUsedIap );
    // Print to log file
    iLog->Log( KSetUsedIap );

    //TPtrC iapName;
    TUint32 iapId(0);
    TInt usedIapNum(0);
    TInt runOnlyIfIapExists(0);
    TInt serviceId(-1);
    TBuf<256> serviceName;
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    TInt rv = KErrNone;

    // Read parameters

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    rv = GetServiceIdFromParam(aItem, serviceId, serviceName);
    if(rv != KErrNone)
    {
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetUsedIap");
        return rv;
    }

    TPtrC iapName;
    if(KErrNone != aItem.GetNextString(iapName) )
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter! Iap name or count was not specified.");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetUsedIap");
        return KErrArgument;
        }

    TBool iapNameSpecified = EFalse;
    TLex lex(iapName);
    if( KErrNone !=  lex.Val(usedIapNum) )
        {
        iapNameSpecified = ETrue;
        }

    aItem.GetNextInt(runOnlyIfIapExists);

    VCXLOGLO2("iapName = %S", &iapName);
    VCXLOGLO2("iapNum = %d", usedIapNum);
    VCXLOGLO3("ServiceId = %d, name: %S",serviceId, &serviceName );

    if(iapNameSpecified)
        {
        TBufC<256> name(iapName);
        if(!iIptvTestUtilities->GetIapIdL(name, iapId))
            {
            VCXLOGLO2("** FAIL ** Test case error. Iap with name %S does not exist.", &name);
            VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetUsedIap");
            return KErrArgument;
            }
        }
    else
        {
        if(usedIapNum >= 0)
            {
            // Get iap id and skip the test if needed
            if(KErrNone != GetCommDbIapIdByOrder(usedIapNum , iapId) && runOnlyIfIapExists )
                {
                VCXLOGLO1("Iap does not exist and flag set, skipping.");
                VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetUsedIap");
                return KErrNone;
                }
            }
        // Invalid iap id
        else
            {
            iapId = 1000;
            }
        }

    if(serviceId < 0)
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetUsedIap");
        return KErrArgument;
        }

    VCXLOGLO2("iapId = %d", iapId);

    // Do the request

    respStatus = iIptvServiceManagementSync->CallSetUsedIap(iapId, serviceId, TRUE );

    // Set also used destination and set the IAP to highest priority.
    // We presume the IAPs are in Internet destination

    _LIT(KDestinationName, "Internet");
    TInt err (KErrNone);
    TBuf<64> destName( KDestinationName );
    TBuf<64> iapNameTemp( iapName );
    TRAP(err, iAlrUtil->SetUsedDestinationL( destName ) );
    if(err != KErrNone)
    	{
    	VCXLOGLO2("** FAIL ** SetUsedDestinationL failed. Error: %d", err );
    	}
    TRAP(err, iAlrUtil->SetMethodPriorityL( destName, iapNameTemp, 0 ) );
    if(err != KErrNone)
    	{
    	VCXLOGLO2("** FAIL ** SetMethodPriorityL failed. Error: %d", err );
    	}

    if(respStatus != MIptvServiceManagementClientObserver::ESucceeded)
        {
        VCXLOGLO2("** FAIL ** CallSetUsedIap returned error. error: %d", respStatus );
        rv = respStatus;
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetUsedIap");

    return rv;
    }
// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::GetUsedIap
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::GetUsedIap( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::GetUsedIap");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KGetUsedIap, "In GetUsedIap" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KGetUsedIap );
    // Print to log file
    iLog->Log( KGetUsedIap );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TUint32 iapId(10000);
    TUint32 expectedIapId(20000);
    TInt msgTypeInt(0);
    TInt expectedIapNumber(0);
    TInt runOnlyIfIapExists(0);
    TInt serviceId(-1);
    TBuf<256> serviceName;
    TInt rv = KErrNone;

    // Read parameters

    aItem.GetNextInt(msgTypeInt);

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    rv = GetServiceIdFromParam(aItem, serviceId, serviceName);
    if(rv != KErrNone)
    {
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetUsedIap");
        return rv;
    }

    TPtrC expectedIapName;
    if(KErrNone != aItem.GetNextString(expectedIapName) )
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter! Iap name or count was not specified.");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetUsedIap");
        return KErrArgument;
        }

    if(!iIptvTestUtilities->GetIapIdByNameL(expectedIapName, expectedIapId))
        {
        VCXLOGLO2("** FAIL ** Test case error. Iap with name %S does not exist.", &expectedIapName);
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetUsedIap");
        return KErrArgument;
        }

	aItem.GetNextInt(runOnlyIfIapExists);

    VCXLOGLO3("ServiceId = %d, name: %S",serviceId, &serviceName );
    VCXLOGLO2("runOnlyIfIapExists: %d", runOnlyIfIapExists);
    VCXLOGLO2("expectedIapName: %S", &expectedIapName);
    VCXLOGLO2("expectedIapNumber: %d", expectedIapNumber);
    VCXLOGLO2("expectedIapId: %d", expectedIapId);

    if(serviceId < 0 || msgTypeInt < 0)
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetUsedIap");
        return KErrArgument;
        }

    CIptvServiceManagementSync::TIptvSmTestConnection msgType = static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    MIptvServiceManagementClientObserver::TRespStatus respStatus;

    CIptvService *service = iIptvTestUtilities->GetServiceFromDb(serviceId);
    if(!service)
        {
        VCXLOGLO1("** FAIL ** No such service in DB. Continuing test run.");
        }
    else
        {
        iIptvTestUtilities->PrintIptvService(service);
        delete service;
        service = NULL;
        }

    // Do the request

    respStatus = iIptvServiceManagementSync->CallGetUsedIap(msgType, serviceId, iapId );

    if(respStatus != MIptvServiceManagementClientObserver::ESucceeded)
        {
        VCXLOGLO2("** FAIL ** CallGetUsedIap returned error. error: %d", respStatus );
        rv = respStatus;
        }
    else
	if( iapId != expectedIapId )
		{
        VCXLOGLO2("** FAIL ** CallGetUsedIap returned iapId %d", iapId);
        VCXLOGLO2("Expected iap id: %d", expectedIapId );
        rv = KErrCorrupt;
		}

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetUsedIap");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::ClearUsedIapL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::ClearUsedIapL( CStifItemParser& /*aItem*/ )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::ClearUsedIapL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KClearUsedIap, "In ClearUsedIapL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KClearUsedIap );
    // Print to log file
    iLog->Log( KClearUsedIap );

    TInt rv = KErrNone;

    MIptvServiceManagementClientObserver::TRespStatus respStatus;

    respStatus = iIptvServiceManagementSync->CallClearUsedIap();

    if(respStatus == MIptvServiceManagementClientObserver::ESucceeded)
        {

        // verify somehow
        }
    else
        {
        VCXLOGLO2("** FAIL ** CallClearUsedIap returned error. error: %d", rv );
        rv = respStatus;
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::ClearUsedIapL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::SetAllIapsL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::SetAllIapsL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::SetAllIapsL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KSetAllIapsL, "In SetAllIapsL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KSetAllIapsL );
    // Print to log file
    iLog->Log( KSetAllIapsL );

    TRespStatus respStatus = ESucceeded;
    TInt rv = KErrNone;
    TInt i = 0;
    TInt iapCount = 0;
    TInt ignoreReadOnlyInt = 0;
    TInt msgTypeInt(0);

    // Read parameters

    aItem.GetNextInt(msgTypeInt);
    aItem.GetNextInt(iapCount);
    aItem.GetNextInt(ignoreReadOnlyInt);

    VCXLOGLO2("iapCount: %d", iapCount);
    VCXLOGLO2("ignoreReadOnlyInt: %d", ignoreReadOnlyInt);

    if(iapCount < 0 || ignoreReadOnlyInt < 0 || ignoreReadOnlyInt > 1 || msgTypeInt < 0 || msgTypeInt > 1)
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetAllIapsL");
        return KErrArgument;
        }

    CIptvServiceManagementSync::TIptvSmTestConnection msgType = static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    TBool ignoreReadOnly;
    ignoreReadOnlyInt == 1 ? ignoreReadOnly = ETrue : ignoreReadOnly = EFalse;

    CIptvIapList* iapList = CIptvIapList::NewL();
    CleanupStack::PushL(iapList);

    TIptvIap iap;

    for(i=0;i<iapCount;i++)
        {
        iap.iId = i+1;
        iap.iPriority = i+1;
        iapList->AddIap(iap);
        }

    respStatus = iIptvServiceManagementSync->CallSetAllIaps(msgType, *iapList, ignoreReadOnly);

    CleanupStack::PopAndDestroy(iapList);

    if(respStatus != ESucceeded)
        {
        VCXLOGLO2("** FAIL ** CallSetAllIapsL returned error. error: %d", respStatus );
        rv = respStatus;
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetAllIapsL");
    return rv;
    }


// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::SetConnectionAllowedL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::SetConnectionAllowedL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::SetConnectionAllowedL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KSetConnectionAllowed, "In SetConnectionAllowedL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KSetConnectionAllowed );
    // Print to log file
    iLog->Log( KSetConnectionAllowed );

    TUint32 iapId(0);
    TPtrC iapName;
    TInt connectionAllowedInt(0);

    // Read parameters

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    User::LeaveIfError( aItem.GetNextString(iapName) );

    if( iapName == _L("INVALIDIAPID") )
        {
        iapId = 1000;
        }
    else
    if( !iIptvTestUtilities->GetIapIdL(iapName, iapId) )
        {
        User::Leave( KErrArgument );
        }

    aItem.GetNextInt( connectionAllowedInt );

    TBool connectionAllowed = static_cast<TBool>(connectionAllowedInt);

    MIptvServiceManagementClientObserver::TRespStatus respStatus;

    VCXLOGLO2("iapId = %d", iapId);
    VCXLOGLO2("connectionAllowedInt = %d", connectionAllowedInt);

    TInt rv = KErrNone;

    respStatus = iIptvServiceManagementSync->CallSetConnectionAllowed(connectionAllowed, iapId);

    if(respStatus != MIptvServiceManagementClientObserver::ESucceeded)
        {
        VCXLOGLO2("** FAIL ** CallSetConnectionAllowedL returned error. error: %d", respStatus );
        rv = respStatus;
        }
    else
        {
        // Verify the result
        CIptvNetworkSelection::TConnectionPermission currentPermission = CIptvNetworkSelection::EMustAskConfirmation;

        CIptvNetworkSelection::TConnectionPermission expectedPermission = CIptvNetworkSelection::EMustAskConfirmation;
        if( connectionAllowed )
            {
            expectedPermission = CIptvNetworkSelection::EAllowed;
            }

        respStatus = iIptvServiceManagementSync->CallIsConnectionAllowed( currentPermission, iapId );

        if(respStatus != MIptvServiceManagementClientObserver::ESucceeded)
            {
            VCXLOGLO2("** FAIL ** CallIsConnectionAllowedL failed, response = %d", respStatus);
            rv = respStatus;
            }
        else
        if( expectedPermission != currentPermission )
            {
            VCXLOGLO1("** FAIL ** CallIsConnectionAllowedL returned wrong permission");
            rv = KErrCorrupt;
            }
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetConnectionAllowedL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::IsConnectionAllowedL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::IsConnectionAllowedL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::IsConnectionAllowedL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KIsConnectionAllowed, "In IsConnectionAllowedL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KIsConnectionAllowed );
    // Print to log file
    iLog->Log( KIsConnectionAllowed );

    TPtrC iapName;
    TUint32 iapId(0);
    TInt expectedPermissionInt(0);

    // Read parameters

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    User::LeaveIfError( aItem.GetNextString(iapName) );

    if( iapName == _L("INVALIDIAPID") )
        {
        iapId = 1000;
        }
    else
    if( !iIptvTestUtilities->GetIapIdL(iapName, iapId) )
        {
        User::Leave( KErrArgument );
        }

    aItem.GetNextInt(expectedPermissionInt);

    if(expectedPermissionInt < 0)
        {
        VCXLOGLO1("** FAIL ** Invalid testcase parameter!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::IsConnectionAllowedL");
        return KErrArgument;
        }

    CIptvNetworkSelection::TConnectionPermission expectedPermission = static_cast<CIptvNetworkSelection::TConnectionPermission>(expectedPermissionInt);

    if(expectedPermission == CIptvNetworkSelection::ENotAllowed)
        {
        expectedPermission = CIptvNetworkSelection::EMustAskConfirmation;
        }

    MIptvServiceManagementClientObserver::TRespStatus respStatus;

    VCXLOGLO2("iapId = %d", iapId);
    VCXLOGLO2("expectedPermissionInt = %d", expectedPermissionInt);

    TInt rv = KErrNone;

    CIptvNetworkSelection::TConnectionPermission permission = CIptvNetworkSelection::ENotAllowed;

    respStatus = iIptvServiceManagementSync->CallIsConnectionAllowed(permission, iapId);

    if(respStatus != MIptvServiceManagementClientObserver::ESucceeded)
    {
        VCXLOGLO2("** FAIL ** CallIsConnectionAllowedL failed, response = %d", respStatus);
        rv = respStatus;
    }
    else
    if(permission != expectedPermission)
        {
        VCXLOGLO1("** FAIL ** CallIsConnectionAllowedL returned wrong permission");
        rv = KErrCorrupt;
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::IsConnectionAllowedL");
    return rv;

    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::GetServicesL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::GetServicesL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::GetServicesL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KGetServices, "In GetServicesL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KGetServices );
    // Print to log file
    iLog->Log( KGetServices );

    TRespStatus respStatus = ESucceeded;
    TInt rv = KErrNone;
    TInt i = 0;
    CDesC8ArraySeg* servicesArrayDesc = NULL;
    TInt msgTypeInt(0);
    TUint startId(0);
    TUint endId(0);

    aItem.GetNextInt(msgTypeInt);

    CIptvServiceManagementSync::TIptvSmTestConnection msgType = static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    aItem.GetNextInt(startId);
    aItem.GetNextInt(endId);

    VCXLOGLO2("startId = %d", startId);
    VCXLOGLO2("endId = %d", endId);

    iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

    respStatus = iIptvServiceManagementSync->CallGetServices(msgType,
                                                            startId,
                                                            endId,
                                                            &servicesArrayDesc);
    if(servicesArrayDesc != NULL)
        {

        VCXLOGLO1("Services:");
        for(i = 0; i < servicesArrayDesc->MdcaCount(); i++)
            {
            CIptvService* iptvService = CIptvService::NewL();

            iptvService->SetL(servicesArrayDesc->MdcaPoint(i));
            iIptvTestUtilities->PrintIptvService(iptvService);

            delete iptvService;
            }
        }

    if(respStatus != ESucceeded && servicesArrayDesc != NULL)
        {
        VCXLOGLO2("** FAIL ** CallGetServices returned error. error: %d", respStatus );
        if(servicesArrayDesc != NULL)
            {
            servicesArrayDesc->Reset();
            delete servicesArrayDesc;
            servicesArrayDesc = NULL;
            }
        else
            {
            VCXLOGLO1("** FAIL ** servicesArrayDesc is NULL");
            }

        rv = respStatus;
        }
    else
        {
        // verify that right services were returned.
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetServicesL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::GetAllSelectedServicesL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::GetAllSelectedServicesL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::GetAllSelectedServicesL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KGetAllSelectedServices, "In GetAllSelectedServicesL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KGetAllSelectedServices );
    // Print to log file
    iLog->Log( KGetAllSelectedServices );
    TRespStatus respStatus = ESucceeded;
    TInt rv = KErrNone;
    CDesC8ArraySeg* selectedServicesArray = NULL;

    TInt msgTypeInt;
    aItem.GetNextInt(msgTypeInt);

    CIptvServiceManagementSync::TIptvSmTestConnection msgType =
                    static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    // Get selected services
    respStatus = iIptvServiceManagementSync->CallGetServicesFiltered(
                CIptvServiceManagementClient::ESelectedServices,
                CIptvServiceManagementClient::EDisplayOrderAscending,
                msgType,
                &selectedServicesArray
                );

    if(respStatus != MIptvServiceManagementClientObserver::ESucceeded)
        {
        if(selectedServicesArray)
            {
            selectedServicesArray->Reset();
            delete selectedServicesArray;
            selectedServicesArray = NULL;
            }

        VCXLOGLO2("** FAIL ** CallGetServices returned error. error: %d", respStatus );
        rv = respStatus;
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetAllSelectedServicesL");
    return rv;

    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::GetServicesFilteredL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::GetServicesFilteredL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::GetServicesFilteredL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KGetServicesFilteredL, "In GetServicesFilteredL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KGetServicesFilteredL );
    // Print to log file
    iLog->Log( KGetServicesFilteredL );
    TRespStatus respStatus = ESucceeded;
    TInt rv = KErrNone;
    CDesC8ArraySeg* selectedServicesArray = NULL;

    TInt msgTypeInt;
    TInt expectedServiceCount = 0;
    TUint32 flags = 0;

    aItem.GetNextInt(msgTypeInt);
    CIptvServiceManagementSync::TIptvSmTestConnection msgType = static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    aItem.GetNextInt(expectedServiceCount);

    TInt orderInt = 2;
    aItem.GetNextInt(orderInt);
    CIptvServiceManagementClient::TOrder order = static_cast<CIptvServiceManagementClient::TOrder>(orderInt);
    VCXLOGLO2("Order: %d", order);

    TBuf<256> searchLimitFlags;

    TInt flagInt = 0;
    while(KErrNone == aItem.GetNextInt(flagInt))
        {
        flags |= flagInt;

        CIptvServiceManagementClient::TSearchLimitFlag flag = static_cast<CIptvServiceManagementClient::TSearchLimitFlag>(flagInt);

        switch(flag)
            {
            case CIptvServiceManagementClient::ESelectedServices:
                {
                searchLimitFlags.Append(_L("ESelectedServices "));
                VCXLOGLO1("Appending ESelectedServices to search flags");
                }
                break;
            case CIptvServiceManagementClient::EMainServices:
                {
                searchLimitFlags.Append(_L("EMainServices "));
                VCXLOGLO1("Appending EMainServices to search flags");
                }
                break;
            case CIptvServiceManagementClient::ESubServices:
                {
                searchLimitFlags.Append(_L("ESubServices "));
                VCXLOGLO1("Appending ESubServices to search flags");
                }
                break;
            case CIptvServiceManagementClient::EVod:
                {
                searchLimitFlags.Append(_L("EVod "));
                VCXLOGLO1("Appending EVod to search flags");
                }
                break;
            case CIptvServiceManagementClient::ELiveTv:
                {
                searchLimitFlags.Append(_L("ELiveTv "));
                VCXLOGLO1("Appending ELiveTv to search flags");
                }
                break;
            case CIptvServiceManagementClient::EVodCast:
                {
                searchLimitFlags.Append(_L("EVodCast "));
                VCXLOGLO1("Appending EVodCast to search flags");
                }
                break;
            case CIptvServiceManagementClient::EBrowser:
                {
                searchLimitFlags.Append(_L("EBrowser "));
                VCXLOGLO1("Appending EBrowser to search flags");
                }
                break;
            case CIptvServiceManagementClient::EApplication:
                {
                searchLimitFlags.Append(_L("EApplication "));
                VCXLOGLO1("Appending EApplication to search flags");
                }
                break;
            case CIptvServiceManagementClient::EVideoRemote:
                {
                searchLimitFlags.Append(_L("EVideoRemote "));
                VCXLOGLO1("Appending EVideoRemote to search flags");
                }
                break;
            case CIptvServiceManagementClient::EMobileTv:
                {
                searchLimitFlags.Append(_L("EMobileTv "));
                VCXLOGLO1("Appending EMobileTv to search flags");
                }
                break;
            case CIptvServiceManagementClient::EServiceGroup:
                {
                searchLimitFlags.Append(_L("EServiceGroup "));
                VCXLOGLO1("Appending EServiceGroup to search flags");
                }
                break;
            case CIptvServiceManagementClient::EGroupedServices:
                {
                searchLimitFlags.Append(_L("EGroupedServices "));
                VCXLOGLO1("Appending EGroupedServices to search flags");
                }
                break;
            case CIptvServiceManagementClient::EOther:
                {
                searchLimitFlags.Append(_L("EOther "));
                VCXLOGLO1("Appending EOther to search flags");
                }
                break;
            case 0:
                break;
            default:
                VCXLOGLO2("Invalid search limit flag in test case! %d", flagInt);
                VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetServicesFilteredL");
                return KErrArgument;
            }
        }

    VCXLOGLO2("Search limit flags: ", &searchLimitFlags);

    iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

    respStatus = iIptvServiceManagementSync->CallGetServicesFiltered(flags,
                                                                order,
                                                                msgType,
                                                                &selectedServicesArray
                                                                );

    if(respStatus != MIptvServiceManagementClientObserver::ESucceeded)
        {
        if(selectedServicesArray)
            {
            selectedServicesArray->Reset();
            delete selectedServicesArray;
            selectedServicesArray = NULL;
            }
        VCXLOGLO2("** FAIL ** CallGetServicesFiltered returned error. error: %d", respStatus );
        rv = respStatus;
        }
    else
        {
        if(selectedServicesArray != NULL)
            {
            /* Print all received services */
            VCXLOGLO1("Filtered services:");
            TInt i( 0 );
            for(i = 0; i < selectedServicesArray->MdcaCount(); i++)
                {
                CIptvService* iptvService = CIptvService::NewL();
                iptvService->SetL(selectedServicesArray->MdcaPoint(i));
                iIptvTestUtilities->PrintIptvService(iptvService);
                delete iptvService;
                }

            /* Verifying the expected amount of services */
            VCXLOGLO3("CIptvServiceManagementApiTest::GetServicesFilteredL -- Expected %d, Received %d", expectedServiceCount, selectedServicesArray->MdcaCount());

            if( expectedServiceCount != -1 )
                {
                /* We were expecting some count */
                if( expectedServiceCount != selectedServicesArray->MdcaCount() )
                    {
                    VCXLOGLO1("Wrong service count!");
                    VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetServicesFilteredL");
                    return KErrCorrupt;
                    }
                }
            }
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetServicesFilteredL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::GetServicesByTypeL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::GetServicesByTypeL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::GetServicesByTypeL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KWhere, "In GetServicesByTypeL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TInt rv = KErrNone;

    TInt msgTypeInt;
    TInt expectedServiceCount = 0;

    User::LeaveIfError( aItem.GetNextInt(msgTypeInt) );

    User::LeaveIfError( aItem.GetNextInt(expectedServiceCount) );

    TInt type;
    User::LeaveIfError( aItem.GetNextInt( type ) );

    VCXLOGLO2("Services with type %d requested.", type);

    iServiceStore->RefreshServicesFromDbL(KNoFilter, KNoSort);

    TInt typeCount(0);
    for( TInt i=0; i<iServiceStore->ServiceCount(); i++ )
        {
        if( iServiceStore->GetService(i)->GetType() == type )
            {
            typeCount++;
            }
        }

    // Verifying the expected amount of services
    VCXLOGLO3("CIptvServiceManagementApiTest::GetServicesFilteredL -- Expected %d, Received %d", expectedServiceCount, typeCount);

    if( expectedServiceCount != typeCount )
        {
        VCXLOGLO2("Wrong service count: %d!", typeCount);
        rv = KErrCorrupt;
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetServicesFilteredL");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::ShutdownServerL
// -----------------------------------------------------------------------------
//
// Not supported
//
TInt CIptvServiceManagementApiTest::ShutdownServerL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::ShutdownServerL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KWhere, "In ShutdownServerL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KWhere );

    // Print to log file
    iLog->Log( KWhere );

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TInt msgTypeInt;
    if( KErrNone != aItem.GetNextInt(msgTypeInt) )
        {
        VCXLOGLO1("* ERROR * Specify parameter async / sync!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::ShutdownServerL");
        return KErrArgument;
        }

    VCXLOGLO2("MsgType: %d", msgTypeInt);

    CIptvServiceManagementSync::TIptvSmTestConnection msgType = static_cast<CIptvServiceManagementSync::TIptvSmTestConnection>(msgTypeInt);

    TRespStatus respStatus = ESucceeded;

    respStatus = iIptvServiceManagementSync->CallShutdownServer(msgType);

    if(respStatus == ESucceeded)
        {
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::ShutdownServerL");
        return KErrNone;
        }

    VCXLOGLO2("Shutdown failed with error code: %d", respStatus);
    VCXLOGLO1("<<<CIptvServiceManagementApiTest::ShutdownServerL");
    return respStatus;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::SetSpecialAction()
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::SetSpecialAction( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::SetSpecialAction");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KSetSpecialAction, "In SetSpecialAction" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KSetSpecialAction );
    // Print to log file
    iLog->Log( KSetSpecialAction );

    TInt action(-1);

    aItem.GetNextInt(action);

    if(action < 0)
        {
        VCXLOGLO1("Incorrect parameter.");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetSpecialAction");
        return KErrArgument;
        }

    iIptvServiceManagementSync->SetSpecialAction(action);
    VCXLOGLO2("SetSpecialAction: %d", action);

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::SetSpecialAction");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::RepeatNextRequest
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::RepeatNextRequest( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::RepeatNextRequest");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KRepeatNextRequest, "In RepeatNextRequest" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KRepeatNextRequest );
    // Print to log file
    iLog->Log( KRepeatNextRequest );

    TInt repeatRequest(0);

    aItem.GetNextInt(repeatRequest);

    iIptvServiceManagementSync->RepeatNextRequest(repeatRequest);

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::RepeatNextRequest");

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::VerifyServerIsShutdownL
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::VerifyServerIsShutdownL( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::VerifyServerIsShutdownL");
    // Print to UI
    _LIT( KIptvServiceManagementApiTest, "IptvServiceManagementApiTest" );
    _LIT( KShutDownServer, "In VerifyServerIsShutdownL" );
    TestModuleIf().Printf( 0, KIptvServiceManagementApiTest, KShutDownServer );
    // Print to log file
    iLog->Log( KShutDownServer );

    TInt rv = KErrNone;

    TFindServer findIptvServer( KIptvServerName );
    TFullName name;

    if ( findIptvServer.Next( name ) == KErrNone )
        {
        // Server running
        VCXLOGLO1("** FAIL ** IptvServer is not closed!");
        VCXLOGLO2("Name: %S", &name);
        rv = KErrCorrupt;
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::VerifyServerIsShutdownL");

    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::DeAllocateResources
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementApiTest::DeAllocateResources()
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::DeAllocateResources");

    iHardcodedServices.ResetAndDestroy();
    iHardcodedServices.Close();

    delete iIptvServiceManagementSync;
    iIptvServiceManagementSync = NULL;

    delete iIptvTestUtilities;
    iIptvTestUtilities = NULL;

    if(iIapNumList)
        {
        iIapNumList->Reset();
        delete iIapNumList;
        iIapNumList = NULL;
        }

    TInt err = KErrNone;
    if(iIptvTestMobilecrashWatcher)
        {
        TRAP(err, iIptvTestMobilecrashWatcher->CheckNewCrashesL());
        delete iIptvTestMobilecrashWatcher;
        iIptvTestMobilecrashWatcher = NULL;
        }

    if(err != KErrNone)
        {
        User::Leave(err);
        }

    if(iServiceStore)
        {
        delete iServiceStore;
        iServiceStore = NULL;
        }

    delete iAlrUtil;
    iAlrUtil = NULL;

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::DeAllocateResources");
}

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::GetCommDbIapIdByOrder
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::GetCommDbIapIdByOrder(TInt aIndex, TUint32& aIapId)
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::GetCommDbIapIdByOrder");
    aIapId = 0;
    if(aIndex >= iIapNumList->Count())
        {
        VCXLOGLO1("Index out of bounds.");
        return KErrOverflow;
        }
    VCXLOGLO1("<<<CIptvServiceManagementApiTest::GetCommDbIapIdByOrder");
    aIapId = iIapNumList->At(aIndex);

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::GetServiceIdFromParam
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementApiTest::GetServiceIdFromParam(CStifItemParser& aItem, TInt& aServiceId, TDes& aServiceName)
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::ReadServiceNameOrIdParam");

    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);

    TPtrC serviceString;

    if(aItem.GetNextString(serviceString) != KErrNone)
        {
        VCXLOGLO1("Can't read testcase parameter, serviceName/number !!!");
        VCXLOGLO1("<<<CIptvServiceManagementApiTest::ReadServiceNameOrIdParam");
        return KErrArgument;
        }

    TInt err = KErrNone;

    if(KErrNone != CIptvTestUtilities::ParseIntFromString(aServiceId, serviceString))
        {
        // service name specified, find the service ID

        aServiceName = serviceString;
        VCXLOGLO2("ServiceName specified: %S", &aServiceName);

        TInt rv = KErrNone;

        TRAPD(err, rv = iIptvTestUtilities->GetServiceIdByNameL(aServiceName, aServiceId));
        if(KErrNone != err)
            {
            VCXLOGLO2("GetServiceIdByNameL caused a leave: %d.", err);
            VCXLOGLO1("<<<CIptvServiceManagementApiTest::ReadServiceNameOrIdParam");
            return err;
            }
        if(KErrNone != rv)
            {
            VCXLOGLO2("GetServiceIdByNameL returned error: %d.", rv);
            VCXLOGLO1("<<<CIptvServiceManagementApiTest::ReadServiceNameOrIdParam");
            return rv;
            }
        }
    else
        {
        RPointerArray<CIptvService> services;

        VCXLOGLO2("ServiceID specified: %d", aServiceId);

        CleanupResetAndDestroyPushL(services);

        if(!iIptvTestUtilities->GetServicesFromDbL(services))
            {
            VCXLOGLO1("GetServicesFromDbL returned failed.");
            VCXLOGLO1("<<<CIptvServiceManagementApiTest::ReadServiceNameOrIdParam");

            CleanupStack::PopAndDestroy(&services);
            return err;
            }

        TInt i;
        for(i=0; i<services.Count(); i++)
            {
            VCXLOGLO2("ServiceName id: %d", services[i]->GetId() );
            if(services[i]->GetId() == aServiceId)
                {
                aServiceName = services[i]->GetName();
                break;
                }
            }
        CleanupStack::PopAndDestroy(&services);
        }

    VCXLOGLO1("<<<CIptvServiceManagementApiTest::ReadServiceNameOrIdParam");

    return err;
    }

//  End of File

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None


//  End of File
