/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Object to offer some common utility functions to Iptv engine testing*
*/




// INCLUDE FILES
#include <e32Base.h>
#include "CIptvTestSMApiUtil.h"
#include "CIptvService.h"
#include "VCXTestLog.h"
#include <BAUTILS.H>
//#include <CIptvServiceManager.h>
#include "CIptvTestActiveWait.h"

#include "CIptvServiceManagementClient.h"

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
// CIptvTestSMApiUtil::CIptvTestSMApiUtil
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvTestSMApiUtil::CIptvTestSMApiUtil()
    {
    }

// -----------------------------------------------------------------------------
// CIptvTestSMApiUtil::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvTestSMApiUtil::ConstructL()
    {
    iIptvServiceManagementClient = CIptvServiceManagementClient::NewL(*this);
    iActiveWait = CIptvTestActiveWait::NewL();

    iRefreshNeeded = ETrue;
    }

// -----------------------------------------------------------------------------
// CIptvServicesFromFile::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvTestSMApiUtil* CIptvTestSMApiUtil::NewL()
    {
    VCXLOGLO1(">>>CIptvTestSMApiUtil::NewL");
    CIptvTestSMApiUtil* self = new( ELeave ) CIptvTestSMApiUtil;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
	VCXLOGLO1("<<<CIptvTestSMApiUtil::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvServicesFromFile::NewL
// Destructor
// -----------------------------------------------------------------------------
//
CIptvTestSMApiUtil::~CIptvTestSMApiUtil()
    {
    VCXLOGLO1(">>>CIptvTestSMApiUtil::~CIptvTestSMApiUtil");

  	iServicesArray.ResetAndDestroy();

	delete iActiveWait;

    if(iIptvServiceManagementClient)
        {
        delete iIptvServiceManagementClient;
        iIptvServiceManagementClient=NULL;
        }

 	iFsSession.Close();

    VCXLOGLO1("<<<CIptvTestSMApiUtil::~CIptvTestSMApiUtil");
    }

// -----------------------------------------------------------------------------
// CIptvTestSMApiUtil::GetServiceFromDb
// Creates CIptvServiceInstance from parameters
// returns pointer to service if succeeded, otherwise NULL
// -----------------------------------------------------------------------------
//
CIptvService* CIptvTestSMApiUtil::GetServiceFromDb(TInt aId)
    {
    if( iRefreshNeeded )
        {
        RefreshServicesL();
        }

    for(TInt i=iServicesArray.Count()-1; i>=0; i--)
        {
        if( iServicesArray[i]->GetId() == aId )
            {
            return CopyServiceL( *iServicesArray[i] );
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CIptvTestSMApiUtil::GetUsedIap
// -----------------------------------------------------------------------------
//
TBool CIptvTestSMApiUtil::GetUsedIap(TUint32 aServiceId, TUint32& aIAPId, TDes& aIapName)
    {
    VCXLOGLO1(">>>CIptvTestSMApiUtil::GetUsedIap");
    TBool rv = ETrue;

    CIptvNetworkSelection::TConnectionPermission connectionPermission;
    CIptvNetworkSelection::TRespStatus respStatus;

    TRAPD(error, iIptvServiceManagementClient->GetUsedIapL(aServiceId,
                                                                aIAPId,
                                                                aIapName,
                                                                connectionPermission,
                                                                respStatus));
    if(error != KErrNone ||
        respStatus != CIptvNetworkSelection::ESucceeded)
        {
        VCXLOGLO1("<<<CIptvTestSMApiUtil::GetUsedIap failed!!!");
        rv = EFalse;
        }

    VCXLOGLO1("<<<CIptvTestSMApiUtil::GetUsedIap");
    return rv;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::GetServicesFromDbL()
// Get services from databases.
// ---------------------------------------------------------------------------
//
TBool CIptvTestSMApiUtil::GetServicesFromDbL(RPointerArray<CIptvService>& aServicesArray, TUint32 aFlagsFilter,
        CIptvServiceManagementClient::TOrder aOrder)
    {
    VCXLOGLO1(">>>CIptvTestSMApiUtil::GetServicesFromDbL");

	aServicesArray.ResetAndDestroy();

	TInt err = KErrNone, err2 = KErrNone;

    TRAP(err2, err = iIptvServiceManagementClient->GetServicesReqL( aFlagsFilter, aOrder ) );

	if(err2 != KErrNone)
    	{
        VCXLOGLO2("GetServicesReqL caused a leave: %d", err2);
        VCXLOGLO1("<<<CIptvTestSMApiUtil::GetServicesFromDbL");
    	return EFalse;
    	}
    else
    if(err != KErrNone)
    	{
        VCXLOGLO2("GetServicesReqL returned error: %d", err);
        VCXLOGLO1("<<<CIptvTestSMApiUtil::GetServicesFromDbL");
        return EFalse;
    	}

	iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);

    TBool ret = ETrue;
    if( iSMRespStatus != MIptvServiceManagementClientObserver::ESucceeded
            && iSMRespStatus != MIptvServiceManagementClientObserver::EEmpty
            )
        {
        ret = EFalse;
        }

	for(TInt i = 0; i < iServicesArray.Count(); i++)
    	{
        aServicesArray.Append( CopyServiceL( *iServicesArray[i] ) );
    	}

    VCXLOGLO1("<<<CIptvTestSMApiUtil::GetServicesFromDbL");

    return ret;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::GetServicesFromDbL()
// Get services from databases.
// ---------------------------------------------------------------------------
//
TBool CIptvTestSMApiUtil::GetServicesFromDbL(RPointerArray<CIptvService>& aServicesArray)
    {
    VCXLOGLO1(">>>CIptvTestSMApiUtil::GetServicesFromDbL (2)");

    aServicesArray.ResetAndDestroy();

    if( iRefreshNeeded )
        {
        RefreshServicesL();
        }

    for(TInt i = 0; i < iServicesArray.Count(); i++)
        {
        aServicesArray.Append( CopyServiceL( *iServicesArray[i] ) );
        }

    VCXLOGLO1("<<<CIptvTestSMApiUtil::GetServicesFromDbL (2)");
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CIptvServiceManager::RefreshServicesL()
// ---------------------------------------------------------------------------
//
TBool CIptvTestSMApiUtil::RefreshServicesL()
    {
    VCXLOGLO1(">>>CIptvTestSMApiUtil::RefreshServicesL");
    iServicesArray.ResetAndDestroy();

    if( iRefreshingServices )
        {
        VCXLOGLO1("<<<CIptvTestSMApiUtil:: Already refreshing.");
        VCXLOGLO1("<<<CIptvTestSMApiUtil::RefreshServicesL");
        return ETrue;
        }

    TInt err = KErrNone, err2 = KErrNone;
    TRAP(err2, err = iIptvServiceManagementClient->GetServicesReqL(0, CIptvServiceManagementClient::EDisplayOrderAscending ));

    if(err2 != KErrNone)
        {
        VCXLOGLO2("GetServicesReqL caused a leave: %d", err2);
        VCXLOGLO1("<<<CIptvTestSMApiUtil::RefreshServicesL");
        return EFalse;
        }
    else
    if(err != KErrNone)
        {
        VCXLOGLO2("GetServicesReqL returned error: %d", err);
        VCXLOGLO1("<<<CIptvTestSMApiUtil::RefreshServicesL");
        return EFalse;
        }

    iRefreshingServices = ETrue;

    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);

    TBool ret = ETrue;
    if( iSMRespStatus != MIptvServiceManagementClientObserver::ESucceeded
            && iSMRespStatus != MIptvServiceManagementClientObserver::EEmpty
            )
        {
        ret = EFalse;
        }

    VCXLOGLO1("<<<CIptvTestSMApiUtil::RefreshServicesL");
    return ret;
    }

// ---------------------------------------------------------------------------
// CIptvTestSMApiUtil::IsServiceIncludedInSearch()
//
// Decides if the service is included in the search.
// ---------------------------------------------------------------------------
//
TBool CIptvTestSMApiUtil::IsServiceIncludedInSearch(TUint32 aSearchLimitFlags, CIptvService& aIptvService)
    {
    if(aSearchLimitFlags == 0)
        {
        return ETrue;
        }

         /*   ESelectedServices = (1 << 0),
            EMainServices     = (1 << 1),
            ESubServices      = (1 << 2),
            EVod              = (1 << 3),
            ELiveTv           = (1 << 4),
            EVodCast          = (1 << 5),
            EOther            = (1 << 6)        */


    if((aSearchLimitFlags & CIptvServiceManagementClient::ESelectedServices)
       && aIptvService.GetFlags() & CIptvService::ESelected)
        {
        return ETrue;
        }

    if((aSearchLimitFlags & CIptvServiceManagementClient::EMainServices)
       && (aIptvService.GetFlags() & CIptvService::EMainService))
        {
        return ETrue;
        }

    if((aSearchLimitFlags & CIptvServiceManagementClient::ESubServices)
        && !(aIptvService.GetFlags() & CIptvService::EMainService))
        {
        return ETrue;
        }

    if((aSearchLimitFlags & CIptvServiceManagementClient::EVod)
       && (aIptvService.GetType() == CIptvService::EVod))
        {
        return ETrue;
        }

    if((aSearchLimitFlags & CIptvServiceManagementClient::EVodCast)
       && (aIptvService.GetType() == CIptvService::EVodCast))
        {
        return ETrue;
        }

    if((aSearchLimitFlags & CIptvServiceManagementClient::EBrowser)
       && (aIptvService.GetType() == CIptvService::EBrowser))
        {
        return ETrue;
        }

    if((aSearchLimitFlags & CIptvServiceManagementClient::EVideoRemote)
       && (aIptvService.GetType() == CIptvService::EVideoRemote))
        {
        return ETrue;
        }


    if((aSearchLimitFlags & CIptvServiceManagementClient::EApplication)
       && (aIptvService.GetType() == CIptvService::EApplication))
        {
        return ETrue;
        }

    if(aSearchLimitFlags & CIptvServiceManagementClient::ELiveTv
       && (aIptvService.GetType() == CIptvService::ELiveTv))
        {
        return ETrue;
        }

    if(aSearchLimitFlags & CIptvServiceManagementClient::EMobileTv
       && (aIptvService.GetType() == CIptvService::EMobileTv))
        {
        return ETrue;
        }

    if(aSearchLimitFlags & CIptvServiceManagementClient::EOther
       && (aIptvService.GetType() == CIptvService::EOther))
        {
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CIptvTestSMApiUtil::CopyService
// -----------------------------------------------------------------------------
//
CIptvService* CIptvTestSMApiUtil::CopyServiceL( CIptvService& aService )
    {
    CIptvService* iptvService = NULL;

    iptvService = CIptvService::NewL();
    CleanupStack::PushL(iptvService);
    HBufC8* buff = aService.GetL();
    CleanupStack::PushL(buff);
    iptvService->SetL( buff->Des() );
    CleanupStack::PopAndDestroy(buff);
    CleanupStack::Pop(iptvService);

    return iptvService;
    }

// -----------------------------------------------------------------------------
// CIptvTestSMApiUtil::SetUsedIap(TUint32 aServiceId, TUint32 aIapId)
//
// Sets default iap for a service, and set connecting allowed for the iap.
// -----------------------------------------------------------------------------
//
TInt CIptvTestSMApiUtil::SetUsedIap(TUint32 aServiceId, TUint32 aIapId)
	{
	TInt err(KErrNone);
	CIptvNetworkSelection::TRespStatus respStatus;

    TRAP(err, iIptvServiceManagementClient->SetUsedIapL(aIapId, aServiceId, ETrue, respStatus));

	if(KErrNone != err)
	    {
	    VCXLOGLO1("CIptvServiceManagementClient::SetUsedIapL leaved");
	    return err;
	    }
	else if( CIptvNetworkSelection::ESucceeded != respStatus )
	    {
	    VCXLOGLO1("CIptvServiceManagementClient::SetUsedIapL returned error");
	    return respStatus;
	    }

    TRAP(err, iIptvServiceManagementClient->SetConnectionAllowedL(ETrue, aIapId, respStatus));

	if(KErrNone != err)
	    {
	    VCXLOGLO1("CIptvServiceManagementClient::SetConnectionAllowedL leaved");
	    return err;
	    }

	else if( CIptvNetworkSelection::ESucceeded != respStatus )
	    {
	    VCXLOGLO1("CIptvServiceManagementClient::SetConnectionAllowedL returned error");
	    return respStatus;
	    }

	return err;
	}

// -----------------------------------------------------------------------------
// CIptvTestSMApiUtil::RefreshService(TUint32 aServiceId)
//
// Loads old service, creates a new duplicate of it and delete the old service.
// Useful for testing scheduled download, because all the contents seems to be
// new, not previously downloaded after calling this for a service.
// Old service id is not valid after calling this
// -----------------------------------------------------------------------------
//
TInt CIptvTestSMApiUtil::RefreshService(TUint32 aServiceId, TTime aLastDownloadTime)
	{
	VCXLOGLO1(">>>CIptvTestSMApiUtil::RefreshService");
    MIptvServiceManagementClientObserver::TRespStatus status;
    TInt leave(KErrNone);
    TInt err(KErrNone);
    CIptvService* service(NULL);

    TRAP(leave, service = GetServiceFromDb(aServiceId));

    if( leave!=KErrNone || service==NULL )
        {
        VCXLOGLO1("<<<CIptvTestSMApiUtil::RefreshService: GetServiceFromDb");
        return KErrNotFound;
        }

    TRAP(leave, err = iIptvServiceManagementClient->DeleteServiceL(aServiceId, status));

    if( MIptvServiceManagementClientObserver::ESucceeded != status ||
        KErrNone != leave || KErrNone != err)
        {
        VCXLOGLO1("<<<CIptvTestSMApiUtil::RefreshService: DeleteServiceL");
        return KErrGeneral;
        }

    // Create new unique provider id.
    _LIT(Kd,"%d");
    TBuf<24> buf;
    TTime now;
    now.HomeTime();
    buf.Format(Kd, now.Int64());
    service->SetProviderId(buf);

    //  After this it seems that downloas haven't ever been scheduled for this service.
    service->SetScheduledLastDownloadTime(aLastDownloadTime);

    TRAP(leave, err = iIptvServiceManagementClient->AddServiceL(*service, status));

    if( MIptvServiceManagementClientObserver::ESucceeded != status||
        KErrNone != leave|| KErrNone != err )
        {
        VCXLOGLO1("<<<CIptvTestSMApiUtil::RefreshService: AddServiceL");
        return KErrGeneral;
        }

	VCXLOGLO1("<<<CIptvTestSMApiUtil::RefreshService");
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CIptvTestSMApiUtil::GetServicesResp()
//
// -----------------------------------------------------------------------------
//
void CIptvTestSMApiUtil::GetServicesResp(TRespStatus aRespStatus, CDesC8ArraySeg* aServicesArray)
	{
	VCXLOGLO1(">>>CIptvTestSMApiUtil::GetServicesResp");

	iRefreshNeeded = EFalse;
	switch(iSMRespStatus)
        {
        case MIptvServiceManagementClientObserver::ESucceeded:
            {
            VCXLOGLO1("GetServicesReqL response: ESucceeded");
            break;
            }

        case MIptvServiceManagementClientObserver::EEmpty:
            {
            VCXLOGLO1("GetServicesReqL response: EEmpty");
            break;
            }

        case MIptvServiceManagementClientObserver::EServiceNotFound:
            {
            VCXLOGLO1("GetServicesReqL response: EServiceNotFound");
            iRefreshNeeded = ETrue;
            break;
            }

        case MIptvServiceManagementClientObserver::EGeneralError:
            {
            VCXLOGLO1("GetServicesReqL response: EGeneralError");
            iRefreshNeeded = ETrue;
            break;
            }

        case MIptvServiceManagementClientObserver::ECancelled:
            {
            VCXLOGLO1("GetServicesReqL response: ECancelled");
            iRefreshNeeded = ETrue;
            break;
            }

        default:
            {
            VCXLOGLO2("GetServicesReqL response: unknown: %d", aRespStatus);
            iRefreshNeeded = ETrue;
            break;
            }
        }

	iSMRespStatus = aRespStatus;
    iRefreshingServices = EFalse;

    if( aServicesArray )
        {
        iServicesArray.ResetAndDestroy();

        CIptvService* iptvService = NULL;

        for(TInt i = 0; i < aServicesArray->MdcaCount(); i++)
            {
            iptvService = CIptvService::NewL();
            CleanupStack::PushL(iptvService);
            iptvService->SetL(aServicesArray->MdcaPoint(i));
            iServicesArray.Append( iptvService );
            VCXLOGLO3("CIptvTestSMApiUtil:: service id: %d, name: %S", iptvService->GetId(), &(iptvService->GetName()) );
            CleanupStack::Pop(iptvService);
            iptvService = NULL;
            }

        aServicesArray->Reset();
        delete aServicesArray;
        aServicesArray = NULL;
        }
    else
        {
        VCXLOGLO1("CIptvTestSMApiUtil:: aServicesArray = NULL");
        iRefreshNeeded = ETrue;
        }

    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStop);

    VCXLOGLO1("<<<CIptvTestSMApiUtil::GetServicesResp");
	}

// -----------------------------------------------------------------------------
// CIptvTestSMApiUtil::HandleSmEvent()
//
// -----------------------------------------------------------------------------
//
void CIptvTestSMApiUtil::HandleSmEvent(CIptvSmEvent& aEvent )
    {
    VCXLOGLO1(">>>CIptvTestSMApiUtil::HandleSmEvent");
    VCXLOGLO2(">>>CIptvTestSMApiUtil:: Event: %d", aEvent.iEvent );
    switch( aEvent.iEvent )
        {
        case CIptvSmEvent::EServiceAdded:
        case CIptvSmEvent::EServiceModified:
        case CIptvSmEvent::EServiceScheduleModified:
        case CIptvSmEvent::EServiceDeleted:
            {
            iRefreshNeeded = ETrue;
            }
            break;

        case CIptvSmEvent::EServiceSelectionModified:
            {

            }
            break;

        default:
            {
            }
            break;
        }
    VCXLOGLO1("<<<CIptvTestSMApiUtil::HandleSmEvent");
    }

//  End of File

