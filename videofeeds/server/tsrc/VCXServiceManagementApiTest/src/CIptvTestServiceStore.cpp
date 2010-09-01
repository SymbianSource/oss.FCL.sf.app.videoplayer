/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/



//  INCLUDES
#include "CIptvTestServiceStore.h"
#include "VCXTestLog.h"
#include "CIptvTestUtilities.h"

#include "CIptvService.h"
#include "CIptvServices.h"

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::NewL
// -----------------------------------------------------------------------------
//
CIptvTestServiceStore* CIptvTestServiceStore::NewL( CIptvTestServiceStore* aStore, CIptvTestUtilities* aTestUtil )
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::NewL");
    CIptvTestServiceStore* self = new (ELeave) CIptvTestServiceStore();
    CleanupStack::PushL(self);
    self->ConstructL( aStore, aTestUtil, NULL );
    CleanupStack::Pop(self);
    VCXLOGLO1("<<<CIptvTestServiceStore::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::NewL
// -----------------------------------------------------------------------------
//
CIptvTestServiceStore* CIptvTestServiceStore::NewL( CIptvServices* aServices )
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::NewL");
    CIptvTestServiceStore* self = new (ELeave) CIptvTestServiceStore();
    CleanupStack::PushL(self);
    self->ConstructL( NULL, NULL, aServices );
    CleanupStack::Pop(self);
    VCXLOGLO1("<<<CIptvTestServiceStore::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::~CIptvTestServiceStore
// -----------------------------------------------------------------------------
//
CIptvTestServiceStore::~CIptvTestServiceStore()
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::~CIptvTestServiceStore");

    iServices.ResetAndDestroy();
    iServices.Close();

    VCXLOGLO1("<<<CIptvTestServiceStore::~CIptvTestServiceStore");
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::CIptvTestServiceStore
// -----------------------------------------------------------------------------
//
CIptvTestServiceStore::CIptvTestServiceStore()
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::CIptvTestServiceStore");

    VCXLOGLO1("<<<CIptvTestServiceStore::CIptvTestServiceStore");
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::ConstructL
// -----------------------------------------------------------------------------
//
void CIptvTestServiceStore::ConstructL( CIptvTestServiceStore* aStore, CIptvTestUtilities* aTestUtil, CIptvServices* aServices )
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::ConstructL");

    iIptvTestUtilities = aTestUtil;

    if(aStore != NULL)
        {
        TInt i;
        for(i=0; i<aStore->ServiceCount(); i++)
            {
            CIptvService* service;
            service = aStore->GetService(i);
            if(service != NULL)
                {
                AddServiceL( *service );
                }
            }
        }

    if(aServices != NULL)
        {
        TInt i;
        for(i=0; i<aServices->Count(); i++)
            {
            CIptvService* service;
            service = aServices->GetServiceL(i);
            CleanupStack::PushL( service );
            AddServiceL( *service );
            CleanupStack::PopAndDestroy( service );
            }
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::ConstructL");
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::RefreshServicesFromDbL
// -----------------------------------------------------------------------------
//
void CIptvTestServiceStore::RefreshServicesFromDbL(TUint32 aFlagsFilter, CIptvServiceManagementClient::TOrder aOrder)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::RefreshServicesFromDbL");

    if(!iIptvTestUtilities)
        {
        VCXLOGLO1("No instance of CIptvTestUtilities!");
        VCXLOGLO1("<<<CIptvTestServiceStore::RefreshServicesFromDbL");
        User::Leave(KErrNotReady);
        }

    DeleteAllServices();

    TBool ok = EFalse;
    TRAPD(error, ok = iIptvTestUtilities->GetServicesFromDbL(iServices, aFlagsFilter, aOrder) );

    if(!ok || error)
        {
        iServices.ResetAndDestroy();

        VCXLOGLO3("** FAIL ** Getting services failed. ok: %d, leave: %d", ok, error);
        VCXLOGLO1("<<<CIptvTestServiceStore::RefreshServicesFromDbL");
        }

    error = (error == KErrNone && ok == EFalse) ? KErrGeneral : error;
    if(error != KErrNone)
        {
        VCXLOGLO2("CIptvTestServiceStore::RefreshServicesFromDbL ERROR %d", error);
        VCXLOGLO1("<<<CIptvTestServiceStore::RefreshServicesFromDbL");
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::RefreshServicesFromDbL");
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::AddServiceL
// -----------------------------------------------------------------------------
//
void CIptvTestServiceStore::AddServiceL(CIptvService& aService)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::AddServiceL");

    CIptvService* service = CloneServiceL(aService);
    iServices.Append( service );

    VCXLOGLO1("<<<CIptvTestServiceStore::AddServiceL");
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::AddServicesL
// -----------------------------------------------------------------------------
//
void CIptvTestServiceStore::AddServicesL(RPointerArray<CIptvService>& aServiceArray)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::AddServicesL");

    TInt i;
    for(i=0; i<aServiceArray.Count(); i++)
        {
        AddServiceL(*aServiceArray[i]);
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::AddServicesL");
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::ReplaceServiceL
// -----------------------------------------------------------------------------
//
void CIptvTestServiceStore::ReplaceServiceL(TInt aId, CIptvService& aService)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::ReplaceServiceL");

    TInt i = FindService(iServices, aId);
    if(i < KErrNone)
        {
        VCXLOGLO1("<<<CIptvTestServiceStore::ReplaceServiceL");
        User::Leave(i);
        }

    CIptvService* service = CloneServiceL(aService);

    VCXLOGLO1("Services before:");
    PrintServices();

    // Get the old service
    CIptvService* oldService = iServices[i];
    service->SetId( oldService->GetId() );
    delete oldService;
    oldService = NULL;
    
    iServices.Insert(service, i);
    iServices.Remove(i+1);

    VCXLOGLO1("Services now:");
    PrintServices();

    VCXLOGLO1("<<<CIptvTestServiceStore::ReplaceServiceL");
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::DeleteService
// -----------------------------------------------------------------------------
//
TInt CIptvTestServiceStore::DeleteService(TInt aId)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::DeleteService");

    TInt i = FindService(iServices, aId);
    if(i < KErrNone)
        {
        VCXLOGLO1("<<<CIptvTestServiceStore::DeleteService");
        return i;
        }

    CIptvService* service = iServices[i];
    if( service->GetType() == CIptvService::EServiceGroup )
        {
        /* Get groupId and delete the group */
        TInt groupID( service->GetGroupId() );
        delete service;
        service = NULL;
        iServices.Remove(i);
        
        /* delete services with groupID */
        TInt groupedIndex( 0 );
        groupedIndex = FindServiceByGroupId( iServices, groupID );
        while( groupedIndex != KErrNotFound )
            {
            iServices.Remove( groupedIndex );
            groupedIndex = FindServiceByGroupId( iServices, groupID );
            }
        }
    else
        {
        delete service;
        service = NULL;
        iServices.Remove(i);        
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::DeleteService");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::Compare
// -----------------------------------------------------------------------------
//
TInt CIptvTestServiceStore::Compare(CIptvTestServiceStore& aStore)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::Compare");

    if( aStore.ServiceCount() != iServices.Count() )
        {
        VCXLOGLO3("Counts of services are different, %d vs. %d.", aStore.ServiceCount(), iServices.Count() );
        VCXLOGLO1("<<<CIptvTestServiceStore::Compare");
        return KErrCorrupt;
        }

    TInt i;
    for(i=0; i< aStore.ServiceCount(); i++)
        {
        CIptvService* service = aStore.GetService(i);
        if(!CompareServices(*(iServices[i]), *service ))
            {
            VCXLOGLO2("Services at index %d are different!", i);
            VCXLOGLO1("<<<CIptvTestServiceStore::Compare");
            return KErrCorrupt;
            }
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::Compare");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::GetService
// -----------------------------------------------------------------------------
//
CIptvService* CIptvTestServiceStore::GetService(TInt aIndex)
    {
    CIptvService* service = NULL;
    ASSERT(aIndex >= 0 && aIndex < iServices.Count());
    service = iServices[aIndex];
    return service;
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::GetServiceById
// -----------------------------------------------------------------------------
//
CIptvService* CIptvTestServiceStore::GetServiceById(TInt aId)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::GetServiceById");
    CIptvService* service = NULL;

    TInt i;
    for(i=0; i< iServices.Count(); i++)
        {
        if(iServices[i]->GetId() == aId)
            {
            service = iServices[i];
            break;
            }
        }

    if(service == NULL)
        {
        VCXLOGLO2("Service not found with id: %d", aId);
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::GetServiceById");
    return service;
    }


// -----------------------------------------------------------------------------
// CIptvTestServiceStore::GetGroupedServicesById
// -----------------------------------------------------------------------------
//
TInt CIptvTestServiceStore::GetGroupedServicesByGroupId(RPointerArray<CIptvService>& aServicesArray, TInt aId)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::GetGroupedServicesById");

    TInt i;
    for( i=0; i< iServices.Count(); i++ )
        {
        if( iServices[i]->GetGroupId() == aId && iServices[i]->GetType() != CIptvService::EServiceGroup )
            {
            aServicesArray.Append( iServices[i] );
            }
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::GetGroupedServicesById");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::GetServices
// -----------------------------------------------------------------------------
//
TInt CIptvTestServiceStore::GetServices(RPointerArray<CIptvService>& aServicesArray)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::GetServices");

    TInt i;
    for(i=0; i< iServices.Count(); i++)
        {
        aServicesArray.Append( iServices[i] );
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::GetServices");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIptvTestServiceStore::GetServicesByProviderId
// -----------------------------------------------------------------------------
//
TInt CIptvTestServiceStore::GetServicesByProviderId(RPointerArray<CIptvService>& aServicesArray, TDes& aProviderId)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::GetServicesByProviderId");

    TInt i;
    for(i=0; i< iServices.Count(); i++)
        {
        if(iServices[i]->GetProviderId() == aProviderId)
            {
            aServicesArray.Append( iServices[i] );
            }
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::GetServicesByProviderId");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::ServiceCount
// -----------------------------------------------------------------------------
//
TInt CIptvTestServiceStore::ServiceCount()
    {
    return iServices.Count();
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::PrintServices
// -----------------------------------------------------------------------------
//
TInt CIptvTestServiceStore::PrintServices()
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::PrintServices");

    TInt i;
    for(i=0; i< iServices.Count(); i++)
        {
        TBuf<256> name;
        TBuf<256> providerId;
        name = iServices[i]->GetName();
        providerId = iServices[i]->GetProviderId();
        VCXLOGLO4("%d, Name: %S, Provider: %S", iServices[i]->GetId(), &name, &providerId );
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::PrintServices");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIptvTestServiceStore::DeleteAllServices
// -----------------------------------------------------------------------------
//
void CIptvTestServiceStore::DeleteAllServices()
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::DeleteAllServices");

    iServices.ResetAndDestroy();

    VCXLOGLO1("<<<CIptvTestServiceStore::DeleteAllServices");
    }


// -----------------------------------------------------------------------------
// CIptvTestServiceStore::CompareServices
// -----------------------------------------------------------------------------
//
TBool CIptvTestServiceStore::CompareServices(CIptvService& aS1, CIptvService& aS2)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::CompareServices");

    TBool rv(TRUE);
    TUint8 i(0);

    VCXLOGLO3("Comparing services: ID: %d - %d", aS1.GetId(), aS2.GetId());
    VCXLOGLO3(" - Name: %S - %S", &aS1.GetName(), &aS2.GetName() );

    if(aS1.GetName() != aS2.GetName())
        {
        rv = EFalse;
        VCXLOGLO3("Name doesn't match: name1 = %S, name2 = %S",&aS1.GetName(),&aS2.GetName());
        }

    if(aS1.GetAddress() != aS2.GetAddress())
        {
         rv = EFalse;
        VCXLOGLO3("Address doesn't match: address1 = %S, address2 = %S",&aS1.GetAddress(),&aS2.GetAddress());
        }

    if(aS1.GetIconPath() != aS2.GetIconPath())
        {
         rv = EFalse;
        VCXLOGLO3("IconPath doesn't match: IconPath1 = %S, IconPath2 = %S",&aS1.GetIconPath(),&aS2.GetIconPath());
        }

    if(aS1.GetEpgPluginUid() != aS2.GetEpgPluginUid())
        {
         rv = EFalse;
        VCXLOGLO3("EpgPluginUid doesn't match: EpgPluginUid1 = %d, EpgPluginUid2 = %d",aS1.GetEpgPluginUid(),aS2.GetEpgPluginUid());
        }

    if(aS1.GetVodPluginUid() != aS2.GetVodPluginUid())
        {
         rv = EFalse;
        VCXLOGLO3("VodPluginUid doesn't match: VodPluginUid1 = %d, VodPluginUid2 = %d",aS1.GetVodPluginUid(),aS2.GetVodPluginUid());
        }

    if(aS1.GetIptvPluginUid() != aS2.GetIptvPluginUid())
        {
         rv = EFalse;
        VCXLOGLO3("IptvPluginUid doesn't match: IptvPluginUid1 = %d, IptvPluginUid2 = %d",aS1.GetIptvPluginUid(),aS2.GetIptvPluginUid());
        }

    if(aS1.GetType() != aS2.GetType())
        {
        rv = EFalse;
        VCXLOGLO3("Type doesn't match: Type1 = %d Type2 = %d", aS1.GetType(), aS2.GetType());
        }

    CIptvIapList& iapListS1 = aS1.GetIapList();
    CIptvIapList& iapListS2 = aS2.GetIapList();
    TIptvIap iapS1;
    TIptvIap iapS2;

    if(iapListS1.Count() != iapListS2.Count())
        {
        rv = EFalse;
        VCXLOGLO3("Cnt of iaps doesn't match: Service1 = %d Service2 = %d", iapListS1.Count(), iapListS2.Count());
        }

    if(rv) // Same count of iaps
        {
        for(i = 0; i < iapListS1.Count(); i++)
            {
            iapListS1.GetIap(i, iapS1);
            iapListS2.GetIap(i, iapS2);

            if(iapS1.iId != iapS2.iId || iapS1.iPriority != iapS2.iPriority)
                {
                rv = EFalse;
                VCXLOGLO2("IAP %d doesn't match", i);
                }
            }
        }

    if(aS1.GetUserName() != aS2.GetUserName())
        {
        rv = EFalse;
        VCXLOGLO3("UserName doesn't match: UserName1 = %S, UserName2 = %S",&aS1.GetUserName() ,&aS2.GetUserName());
        }

    if(aS1.GetPassword() != aS2.GetPassword())
        {
        rv = EFalse;
        VCXLOGLO3("Password doesn't match:Password1 = %S, Password2 = %S",&aS1.GetPassword() ,&aS2.GetPassword());
        }

    if(aS1.GetDesc() != aS2.GetDesc())
        {
        rv = EFalse;
        VCXLOGLO3("Desription doesn't match: Desription1 = %S, Desription2 = %S",&aS1.GetDesc() ,&aS2.GetDesc());
        }

    if(aS1.GetFlags() != aS2.GetFlags())
        {
        rv = EFalse;
        VCXLOGLO3("Flags doesn't match: 0x%x & 0x%x", aS1.GetFlags(), aS2.GetFlags() );

        VCXLOGLO1("Service1 flags: ");
        if(aS1.GetFlags() & CIptvService::ESelected)
            {
            VCXLOGLO1("ESelected");
            }
        if(aS1.GetFlags() & CIptvService::EReadOnly)
            {
            VCXLOGLO1("EReadOnly");
            }
        if(aS1.GetFlags() & CIptvService::EMainService)
            {
            VCXLOGLO1("EMainService");
            }
        if(aS1.GetFlags() & CIptvService::EConnectionApproved)
            {
            VCXLOGLO1("EConnectionApproved");
            }
        if(aS1.GetFlags() & CIptvService::EReadOnlyIaps)
            {
            VCXLOGLO1("EReadOnlyIaps");
            }

        VCXLOGLO1("Service2 flags: ");
        if(aS2.GetFlags() & CIptvService::ESelected)
            {
            VCXLOGLO1("ESelected");
            }
        if(aS2.GetFlags() & CIptvService::EReadOnly)
            {
            VCXLOGLO1("EReadOnly");
            }
        if(aS2.GetFlags() & CIptvService::EMainService)
            {
            VCXLOGLO1("EMainService");
            }
        if(aS2.GetFlags() & CIptvService::EConnectionApproved)
            {
            VCXLOGLO1("EConnectionApproved");
            }
        if(aS2.GetFlags() & CIptvService::EReadOnlyIaps)
            {
            VCXLOGLO1("EReadOnlyIaps");
            }

        }

    if(aS1.GetApplicationUid() != aS2.GetApplicationUid())
        {
        rv = EFalse;
        VCXLOGLO1("Application uid doesn't match");
        }

    /*if(aS1.GetDisplayOrder() != aS2.GetDisplayOrder())
        {
        rv = EFalse;
        VCXLOGLO1("Display order doesn't match");
        }
    */
        if(aS1.GetProviderId() != aS2.GetProviderId())
        {
        rv = EFalse;
        VCXLOGLO3("Provider id doesn't match: id1 = %S, id2 = %S", &aS1.GetProviderId(), &aS2.GetProviderId());
        }

            if(aS1.GetAccountManagementUrl() != aS2.GetAccountManagementUrl())
        {
        rv = EFalse;
        VCXLOGLO3("AccountManagementUrl doesn't match: url1 = %S, url2 = %S", &aS1.GetAccountManagementUrl(), &aS2.GetAccountManagementUrl());
        }

    VCXLOGLO1("<<<CIptvTestServiceStore::CompareServices");
    return rv;
    }


// -----------------------------------------------------------------------------
// CIptvTestServiceStore::FindService
// -----------------------------------------------------------------------------
//
TInt CIptvTestServiceStore::FindService(RPointerArray<CIptvService>& aServices, TInt aId)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::FindService");
    TInt i;

    if( aServices.Count() > 0 )
        {
        for( i=aServices.Count()-1; i>=0; i-- )
        //for(i=0; i<aServices.Count(); i++)
            {
            if( aServices[i]->GetId() == aId )
                {
                VCXLOGLO2("Found ID: %d",  aServices[i]->GetId());
                return i;
                }
            }
        }

    VCXLOGLO2("Service not found: %d", aId);
    VCXLOGLO1("<<<CIptvTestServiceStore::FindService");
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::FindServiceByGroupId
// -----------------------------------------------------------------------------
//
TInt CIptvTestServiceStore::FindServiceByGroupId(RPointerArray<CIptvService>& aServices, TInt aGroupId)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::FindServiceByGroupId");
    TInt i;

    VCXLOGLO2("Searching for service with group ID %d.",  aGroupId);
    
    if( aServices.Count() > 0 )
        {
        for( i=aServices.Count()-1; i>=0; i-- )
            {
            if( aServices[i]->GetGroupId() == aGroupId )
                {
                VCXLOGLO2("Found service: %d",  aServices[i]->GetId());
                return i;
                }
            }
        }

    VCXLOGLO2("Service with group ID %d not found!", aGroupId);
    VCXLOGLO1("<<<CIptvTestServiceStore::FindServiceByGroupId");
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CIptvTestServiceStore::CloneServiceL
// -----------------------------------------------------------------------------
//
CIptvService* CIptvTestServiceStore::CloneServiceL(CIptvService& aService)
    {
    VCXLOGLO1(">>>CIptvTestServiceStore::CloneServiceL");

    CIptvService* iptvService = CIptvService::NewL();
    CleanupStack::PushL(iptvService);
    HBufC8* buff = aService.GetL();
    CleanupStack::PushL( buff );
    iptvService->SetL( buff->Des() );
    CleanupStack::Pop( buff );
    delete buff;
    CleanupStack::Pop(iptvService);

    VCXLOGLO1("<<<CIptvTestServiceStore::CloneServiceL");

    return iptvService;
    }

// End of File
