/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include "CIptvServiceManagementSync.h"
#include "CIptvServiceManagementClient.h"
#include "CIptvTestUtilities.h"
#include "CIptvService.h"
#include "VCXTestLog.h"
#include "CIptvTestTimer.h"
#include "CIptvTestActiveWait.h"

#include <f32file.h>
#include <S32FILE.H>
#include <BAUTILS.H>
//#include <CIptvSmEvent.h>

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
CIptvServiceManagementSync* CIptvServiceManagementSync::NewL()
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::NewL");
    CIptvServiceManagementSync* self = new (ELeave) CIptvServiceManagementSync;

    CleanupStack::PushL( self );
    self->ConstructL();

    CleanupStack::Pop();
    VCXLOGLO1("<<<CIptvServiceManagementSync::NewL");
    return( self ) ;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::ConstructL()
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::ConstructL");

    iActiveWait = CIptvTestActiveWait::NewL();

    iIptvTestTimer = CIptvTestTimer::NewL( *this, 0 );

    iIptvServiceManagementClient = CIptvServiceManagementClient::NewL(*this);

    TRAPD(err, iIptvTestUtilities = CIptvTestUtilities::NewL() );
    if(err != KErrNone)
        {
        VCXLOGLO1("CIptvServiceManagementSync:: ** FAIL ** Creating CIptvTestUtilities failed! Leaving.");
        User::Leave(KErrGeneral);
        }


    VCXLOGLO1("<<<CIptvServiceManagementSync::ConstructL");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CIptvServiceManagementSync()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CIptvServiceManagementSync::CIptvServiceManagementSync()
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CIptvServiceManagementSync");

    VCXLOGLO1("<<<CIptvServiceManagementSync::CIptvServiceManagementSync");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::~CIptvServiceManagementSync()
// Destructor.
// -----------------------------------------------------------------------------
//
CIptvServiceManagementSync::~CIptvServiceManagementSync()
    {
    VCXLOGLO1(">>>~CIptvServiceManagementSync()");

    if(iServicesArray)
        {
        iServicesArray->Reset();
        delete iServicesArray;
        iServicesArray = NULL;
        }

    delete iIptvServiceManagementClient;
    iIptvServiceManagementClient = NULL;

    delete iIptvTestTimer;
    iIptvTestTimer = NULL;

    delete iIptvTestUtilities;
    iIptvTestUtilities = NULL;

    delete iActiveWait;
    iActiveWait = NULL;

    VCXLOGLO1("<<<~CIptvServiceManagementSync()");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CallAddService(TIptvSmTestConnection aConn, CIptvService& aService);
// Adds a service to Service data base.
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallAddService(TIptvSmTestConnection aConn, CIptvService& aService)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CallAddService()");

    TInt err = KErrNone, err2 = KErrNone;

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallAddService.");
        return EGeneralError;
        }

    if(aConn == EIptvSmTestAsync)
        {
        VCXLOGLO1("Async service request");

        while(iRequestRepeatCount-- > 0)
            {
            SetExpectedSmEvent(CIptvSmEvent::EServiceAdded, 1);

            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err2, err = iIptvServiceManagementClient->AddServiceReqL(aService) );

                if(err2 == KErrNone && err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err2 != KErrNone)
                {
                VCXLOGLO2("AddServiceReqL caused a leave: %d", err2);
                VCXLOGLO1("<<<CIptvServiceManagementSync::CallAddService");
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // don't just leave, wait possible request from server to end
                return EGeneralError;
                }
            if(err != KErrNone)
                {
                VCXLOGLO2("AddServiceReqL returned error: %d", err);
                VCXLOGLO1("<<<CIptvServiceManagementSync::CallAddService");
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // don't just leave, wait possible request from server to end
                return EGeneralError;
                }
          }

        if(iSpecialAction == EIptvSmCancelNextAsyncRequest)
            {
            iIptvServiceManagementClient->CancelRequest();
            VCXLOGLO1("Canceling request to server.");
            }
        else
            {
            iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
            if(WaitForSmEvent() != KErrNone)
                {
                iSyncReturnValue = MIptvServiceManagementClientObserver::EGeneralError;
                }
            }
        }
    else
        {
        VCXLOGLO1("Sync service request");
        TRespStatus rv = ESucceeded;

        while(iRequestRepeatCount-- > 0)
            {
            SetExpectedSmEvent(CIptvSmEvent::EServiceAdded, 1);

            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err2, err = iIptvServiceManagementClient->AddServiceL(aService, rv) );

                if(err2 == KErrNone && err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err2 != KErrNone)
                {
                VCXLOGLO2("AddServiceL caused a leave: %d", err2);
                rv = EGeneralError;
                }
            else
            if(err != KErrNone)
                {
                VCXLOGLO2("AddServiceL returned error: %d", err);
                rv = EGeneralError;
                }
            else
            if(MIptvServiceManagementClientObserver::ESucceeded != rv)
               {
               VCXLOGLO2("AddServiceL response error: %d", rv);
               }
            else
            if(WaitForSmEvent() != KErrNone)
                {
                iSyncReturnValue = MIptvServiceManagementClientObserver::EGeneralError;
                }
            }

        iSyncReturnValue = rv;
        }

    FinishRequest();

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallAddService");
    return iSyncReturnValue;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::UpdateServiceReq()
// Updates a service to Service data base.
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallUpdateService(TIptvSmTestConnection aConn, CIptvService& aService, CIptvSmEvent::TEvent aExpectedEvent)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CallUpdateService()");

    TInt err = KErrNone, err2 = KErrNone;

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallUpdateService.");
        return EGeneralError;
        }

    if(iRequestRepeatCount <= 0) iRequestRepeatCount = 1;

    if(aConn == EIptvSmTestAsync)
        {
        VCXLOGLO1("Async service request");

        while(iRequestRepeatCount-- > 0)
            {
            SetExpectedSmEvent(aExpectedEvent, 1);

            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err2, err = iIptvServiceManagementClient->UpdateServiceReqL(aService)  );

                if(err2 == KErrNone && err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err2 != KErrNone)
                {
                VCXLOGLO2("UpdateServiceReqL caused a leave: %d", err2);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // don't just leave, wait possible request from server to end
                VCXLOGLO1("<<<CIptvServiceManagementSync::CallUpdateService");
                return EGeneralError;
                }
            if(err != KErrNone)
                {
                VCXLOGLO2("UpdateServiceReqL returned error: %d", err);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // don't just leave, wait possible request from server to end
                VCXLOGLO1("<<<CIptvServiceManagementSync::CallUpdateService");
                return EGeneralError;
                }
            }

        if(iSpecialAction == EIptvSmCancelNextAsyncRequest)
            {
            iIptvServiceManagementClient->CancelRequest();
            VCXLOGLO1("Canceling request to server.");
            }
        else
            {
            iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // wait for the response from server
            if(WaitForSmEvent() != KErrNone)
                {
                iSyncReturnValue = MIptvServiceManagementClientObserver::EGeneralError;
                }
            }

        }
    else
        {
        VCXLOGLO1("Sync service request");
        TRespStatus rv = ESucceeded;

        while(iRequestRepeatCount-- > 0)
            {
            SetExpectedSmEvent(aExpectedEvent, 1);

            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err2, err = iIptvServiceManagementClient->UpdateServiceL(aService, rv)  );

                if(err2 == KErrNone && err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err2 != KErrNone)
                {
                VCXLOGLO2("UpdateServiceL caused a leave: %d", err2);
                rv = EGeneralError;
                }
            else
            if(err != KErrNone)
                {
                VCXLOGLO2("UpdateServiceL returned error: %d", err);
                rv = EGeneralError;
                }
            else
            if(MIptvServiceManagementClientObserver::ESucceeded != rv)
               {
               VCXLOGLO2("UpdateServiceL response error: %d", rv);
               }
            else
            if(WaitForSmEvent() != KErrNone)
                {
                iSyncReturnValue = MIptvServiceManagementClientObserver::EGeneralError;
                }

            }

        iSyncReturnValue = rv;
        }

    FinishRequest();

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallUpdateService() ");
    return iSyncReturnValue;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CallGetServices()
// Gets service entries from Services database.
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallGetServices(
                                    TIptvSmTestConnection aConn,
                                    TUint32 aStartId,
                                    TUint32 aEndId,
                                    CDesC8ArraySeg** aServicesArray
                                    )
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::GetServicesReqSync()");

    TInt err = KErrNone, err2 = KErrNone;

    // this should be always NULL, otherwise there probably has been an error and we still own this
    if(iServicesArray)
        {
        VCXLOGLO1("* ERROR * iServicesArray not NULL, error in previous GetServices request?");
        iServicesArray->Reset();
        delete iServicesArray;
        iServicesArray = NULL;
        }

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::GetServicesReqSync");
        return EGeneralError;
        }

    if(aConn == EIptvSmTestAsync)
        {
        VCXLOGLO1("Async service request");

        while(iRequestRepeatCount-- > 0)
            {

            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err2, err = iIptvServiceManagementClient->GetServicesReqL(aStartId, aEndId)  );

                if(err2 == KErrNone && err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err2 != KErrNone)
                {
                VCXLOGLO2("GetServicesReqL caused a leave: %d", err2);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // don't just leave, wait possible request from server to end
                VCXLOGLO1("<<<CIptvServiceManagementSync::GetServicesReqSync");
                return EGeneralError;
                }
            if(err != KErrNone)
                {
                VCXLOGLO2("GetServicesReqL returned error: %d", err);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // don't just leave, wait possible request from server to end
                VCXLOGLO1("<<<CIptvServiceManagementSync::GetServicesReqSync");
                return EGeneralError;
                }

            }
        if(iSpecialAction == EIptvSmCancelNextAsyncRequest)
            {
            iIptvServiceManagementClient->CancelRequest();
            VCXLOGLO1("Canceling request to server.");
            }
        else
            {
            iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
            }
        }
    else
        {
        VCXLOGLO1("Sync service request");
        TRespStatus rv = ESucceeded;

        while(iRequestRepeatCount-- > 0)
            {
            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err2, err = iIptvServiceManagementClient->GetServicesL(aStartId, aEndId, iServicesArray, rv) );

                if(err2 == KErrNone && err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err2 != KErrNone)
                {
                VCXLOGLO2("GetServicesL caused a leave: %d", err2);
                rv = EGeneralError;
                }
            else
            if(err != KErrNone)
                {
                VCXLOGLO2("GetServicesL returned error: %d", err);
                rv = EGeneralError;
                }
            else
            if(MIptvServiceManagementClientObserver::ESucceeded != rv)
               {
               VCXLOGLO2("GetServicesL response error: %d", rv);
               }
            }

        iSyncReturnValue = rv;
        }

    if(iSpecialAction == EIptvSmCancelNextAsyncRequest)
        {
        if(iServicesArray == NULL)
            {
            iServicesArray  = new (ELeave) CDesC8ArraySeg(10);
            }
        }

    FinishRequest();

    *aServicesArray = iServicesArray;
    iServicesArray = NULL; // we don't need this anymore, ownership moves

    VCXLOGLO1("<<<CIptvServiceManagementSync::GetServicesReqSync()");
    return iSyncReturnValue;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::GetAllSelectedServicesReq()
// Requests all filtered services from the database.
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallGetServicesFiltered(
                                            TUint32 aSearchLimitFlags,
                                            CIptvServiceManagementClient::TOrder aOrder,
                                            TIptvSmTestConnection aConn,
                                            CDesC8ArraySeg** aServicesArray
                                            )
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CallGetServicesFiltered()");

    TInt err = KErrNone, err2 = KErrNone;

    // this should be always NULL, otherwise there probably has been an error and we still own this
    if(iServicesArray)
        {
        VCXLOGLO1("* ERROR * iServicesArray not NULL, error in previous GetServices request?");
        iServicesArray->Reset();
        delete iServicesArray;
        iServicesArray = NULL;
        }

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallGetServicesFiltered");
        return EGeneralError;
        }

    if(aConn == EIptvSmTestAsync)
        {
        VCXLOGLO1("Async service request");

        while(iRequestRepeatCount-- > 0)
            {
            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err2, err = iIptvServiceManagementClient->GetServicesReqL(aSearchLimitFlags, aOrder)  );

                if(err2 == KErrNone && err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err2 != KErrNone)
                {
                VCXLOGLO2("GetServicesReqL caused a leave: %d", err2);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // don't just leave, wait possible request from server to end
                VCXLOGLO1("<<<CIptvServiceManagementSync::CallGetServicesFiltered");
                return EGeneralError;
                }
            if(err != KErrNone)
                {
                VCXLOGLO2("GetServicesReqL returned error: %d", err);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // don't just leave, wait possible request from server to end
                VCXLOGLO1("<<<CIptvServiceManagementSync::CallGetServicesFiltered");
                return EGeneralError;
                }
            }

        if(iSpecialAction == EIptvSmCancelNextAsyncRequest)
            {
            //removed: iIptvTestTimer->After(5 * 1000000);

            iIptvServiceManagementClient->CancelRequest();
            VCXLOGLO1("Canceling request to server.");
            }
        else
            {
            iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
            }

        }
    else
        {
        VCXLOGLO1("Sync service request");

        TRespStatus rv = ESucceeded;

        while(iRequestRepeatCount-- > 0)
            {
            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err2, err = iIptvServiceManagementClient->GetServicesL(aSearchLimitFlags, aOrder, iServicesArray, rv)  );

                if(err2 == KErrNone && err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err2 != KErrNone)
                {
                VCXLOGLO2("GetServicesL caused a leave: %d", err2);
                rv = EGeneralError;
                }
            if(err != KErrNone)
                {
                VCXLOGLO2("GetServicesL returned error: %d", err);
                rv = EGeneralError;
                }
            }

            iSyncReturnValue = rv;
        }


    if(iSpecialAction == EIptvSmCancelNextAsyncRequest)
        {
        if(iServicesArray == NULL)
            {
            iServicesArray  = new (ELeave) CDesC8ArraySeg(10);
            }
        }

    *aServicesArray = iServicesArray;
    iServicesArray = NULL; // ownership moves

    FinishRequest();

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallGetServicesFiltered()");
    return iSyncReturnValue;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::GetGetServicesByProviderId()
// Requests all filtered services from the database.
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallGetServicesByProviderId(
                                            TDesC& aProviderId,
                                            CIptvServices& aServices
                                            )
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CallGetServicesByProviderId()");

    TInt err = KErrNone, err2 = KErrNone;

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallGetServicesByProviderId");
        return EGeneralError;
        }

    VCXLOGLO1("Sync service request");

    TInt retryCount = 5;

    while(retryCount-- > 0)
        {
        TRAP(err2, err = iIptvServiceManagementClient->GetServicesL(aProviderId, aServices)  );

        if(err2 == KErrNone && err == KErrNotReady)
            {
            VCXLOGLO1("Server not ready, waiting a second before retry.");
            iIptvTestTimer->After(1 * 1000000);
            iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
            }
        else
            {
            break; // request succeeded or there's an error
            }
        }

    if(err2 != KErrNone)
        {
        VCXLOGLO2("CallGetServicesByProviderId caused a leave: %d", err2);
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallGetServicesByProviderId");
        return EGeneralError;
        }
    if(err != KErrNone)
        {
        VCXLOGLO2("CallGetServicesByProviderId returned error: %d", err);
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallGetServicesByProviderId");
        return EGeneralError;
        }

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallGetServicesByProviderId()");

    FinishRequest();

    return iSyncReturnValue;
    }


// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CallDeleteService()
// Deletes a service from the database.
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallDeleteService(TIptvSmTestConnection aConn, TUint32 aId)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CallDeleteService()");

    TInt err = KErrNone, err2 = KErrNone;

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallDeleteService");
        return EGeneralError;
        }

    if(aConn == EIptvSmTestAsync)
        {
        VCXLOGLO1("Async service request");

        while(iRequestRepeatCount-- > 0)
            {
            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                SetExpectedSmEvent(CIptvSmEvent::EServiceDeleted, 1);
                TRAP(err2, err = iIptvServiceManagementClient->DeleteServiceReqL(aId) );
                iIptvTestTimer->After(2 * 1000000);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);

                if(err2 == KErrNone && err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err2 != KErrNone)
                {
                VCXLOGLO2("DeleteServiceReqL caused a leave: %d", err2);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // don't just leave, wait possible request from server to end
                VCXLOGLO1("<<<CIptvServiceManagementSync::CallDeleteService");
                return EGeneralError;
                }
            if(err != KErrNone)
                {
                VCXLOGLO2("DeleteServiceReqL returned error: %d", err);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // don't just leave, wait possible request from server to end
                VCXLOGLO1("<<<CIptvServiceManagementSync::CallDeleteService");
                return EGeneralError;
                }
            }

        if(iSpecialAction == EIptvSmCancelNextAsyncRequest)
            {
            iIptvServiceManagementClient->CancelRequest();
            VCXLOGLO1("Canceling request to server.");
            }
        else
            {
            iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart); // wait for the response from server
            if(WaitForSmEvent() != KErrNone)
                {
                iSyncReturnValue = MIptvServiceManagementClientObserver::EGeneralError;
                }
            }
        }
    else
        {
        VCXLOGLO1("Sync service request");
        TRespStatus rv = ESucceeded;

        while(iRequestRepeatCount-- > 0)
            {
            SetExpectedSmEvent(CIptvSmEvent::EServiceDeleted, 1);

            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err2, err = iIptvServiceManagementClient->DeleteServiceL(aId + iRequestRepeatCount, rv) );

                VCXLOGLO1("Request done.");

                if(err2 == KErrNone && err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            VCXLOGLO1("Checking errors.");

            if(err2 != KErrNone)
                {
                VCXLOGLO2("DeleteServiceL caused a leave: %d", err2);
                rv = EGeneralError;
                }
            else
            if(err != KErrNone)
                {
                VCXLOGLO2("DeleteServiceL returned error: %d", err);
                rv = EGeneralError;
                }
            else
            if(MIptvServiceManagementClientObserver::ESucceeded != rv)
               {
               VCXLOGLO2("DeleteServiceL response error: %d", rv);
               }
            else
            if(WaitForSmEvent() != KErrNone)
                {
                iSyncReturnValue = MIptvServiceManagementClientObserver::EGeneralError;
                }
            }
        iSyncReturnValue = rv;
        }

    FinishRequest();

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallDeleteService()");
    return iSyncReturnValue;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CallShutdownServer()
// Requests server to shutdown itself.
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallShutdownServer(TIptvSmTestConnection aConn )
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CallShutdownServer()");

    TInt err = KErrNone;

    if(aConn == EIptvSmTestAsync)
        {
        VCXLOGLO1("Async service request");
        err = iIptvServiceManagementClient->ServerShutdownReq();
        if(err != KErrNone)
            {
            VCXLOGLO2("ServerShutdownReq returned error: %d", err);
            VCXLOGLO1("<<<CIptvServiceManagementSync::CallShutdownServer");
            return EGeneralError;
            }

        if(iSpecialAction == EIptvSmCancelNextAsyncRequest)
            {
            iIptvServiceManagementClient->CancelRequest();
            VCXLOGLO1("Canceling request to server.");
            }
        else
            {
            iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
            }

        }
    else
        {
        VCXLOGLO1("Sync service request");
        TRespStatus rv = ESucceeded;
        err = iIptvServiceManagementClient->ServerShutdown(rv);
        if(err != KErrNone)
            {
            VCXLOGLO2("ServerShutdown returned error: %d", err);
            rv = EGeneralError;
            }
        else
        if(MIptvServiceManagementClientObserver::ESucceeded != rv)
           {
           VCXLOGLO2("ServerShutdown response error: %d", rv);
           }

        iSyncReturnValue = rv;
        }
    VCXLOGLO1("<<<CIptvServiceManagementSync::CallShutdownServer()");

    return iSyncReturnValue;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CallSetUsedIap()
// Sets used Iap
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallSetUsedIap(TUint32 aIapId,
                                  TUint32 aServiceId,
                                  TBool aSetToDefaultForService
                                  )
    {
    VCXLOGLO1("<<<CIptvServiceManagementSync::CallSetUsedIap()");
    TRespStatus rv = ESucceeded;
    CIptvNetworkSelection::TRespStatus status = CIptvNetworkSelection::ESucceeded;

    TInt err = KErrNone;

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallSetUsedIap()");
        return EGeneralError;
        }

    VCXLOGLO1("Sync service request");

    TInt timeOut = 30;

    if(iRequestRepeatCount <= 0)
        iRequestRepeatCount = 1; // do once if repeat not set
    else
        timeOut = 0; // don't wait server to get ready if repeat is set

    while(iRequestRepeatCount-- > 0)
        {

        do // try again if server busy
            {

            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err, iIptvServiceManagementClient->SetUsedIapL(aIapId, aServiceId, aSetToDefaultForService, status) );

                if(err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err == KErrNone || err != KErrNotReady) break; // request ok

            timeOut -= 2;
            }
        while (timeOut > 0);

        if(err != KErrNone)
            {
            VCXLOGLO2("SetUsedIapL caused a leave: %d", err);
            rv = EGeneralError;
            if(err == KErrNotReady)
                {
                VCXLOGLO1(" - previous request still on server. Not ready");
                }
            }
        else
        if(CIptvNetworkSelection::ESucceeded != status)
           {
           VCXLOGLO2("SetUsedIapL response error: %d", status);
           rv = EGeneralError;
           }
        }

    // network selection api specific return values!!

    FinishRequest();

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallSetUsedIap()");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CallClearUsedIap()
// Sets used Iap
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallClearUsedIap()
    {
    VCXLOGLO1("<<<CIptvServiceManagementSync::CallClearUsedIap()");
    TRespStatus rv = ESucceeded;

    TInt err = KErrNone;

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallClearUsedIap()");
        return EGeneralError;
        }

    VCXLOGLO1("Sync service request");

    if(iRequestRepeatCount <= 0)
        iRequestRepeatCount = 1; // do once if repeat not set

    while(iRequestRepeatCount-- > 0)
        {

        TInt retryCount = 5;

        while(retryCount-- > 0)
            {
            err = iIptvServiceManagementClient->ClearUsedIap();

            if(err == KErrNotReady)
                {
                VCXLOGLO1("Server not ready, waiting a second before retry.");
                iIptvTestTimer->After(1 * 1000000);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                }
            else
                {
                break; // request succeeded or there's an error
                }
            }

        if(err != KErrNone)
            {
            VCXLOGLO2("ClearUsedIap returned error: %d", err);
            rv = EGeneralError;
            }
        }

    FinishRequest();

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallClearUsedIap()");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CallGetUsedIap()
// Gets used iap
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallGetUsedIap(
                                  TIptvSmTestConnection aConn,
                                  TUint32 aServiceId,
                                  TUint32& aIapId
                                  )
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CallGetUsedIap()");

    TInt err = KErrNone;

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallGetUsedIap()");
        return EGeneralError;
        }


    TRespStatus rv = ESucceeded;
    TBuf<256> iapName;
    CIptvNetworkSelection::TRespStatus status = CIptvNetworkSelection::ESucceeded;
    CIptvNetworkSelection::TConnectionPermission connectionPermission;

    if(iRequestRepeatCount <= 0) iRequestRepeatCount = 1;

    // network selection api specific return values!!
    if(aConn == EIptvSmTestAsync)
        {

        VCXLOGLO1("Async service request");

        while(iRequestRepeatCount-- > 0)
            {

            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err, iIptvServiceManagementClient->GetUsedIapReqL(aServiceId) );

                if(err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err != KErrNone)
                {
                VCXLOGLO2("GetUsedIapReqL caused a leave: %d", err);
                rv = EGeneralError;
                iSyncReturnValue = rv;
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                return rv;
                }
            }

        if(iSpecialAction == EIptvSmCancelNextAsyncRequest)
            {
            iIptvServiceManagementClient->CancelRequest();
            VCXLOGLO1("Canceling request to server.");
            }
        else
            {
            iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
            }

        aIapId = iIapId;
        }
    else
        {
        VCXLOGLO1("Sync service request");
        TRespStatus rv = ESucceeded;

        while(iRequestRepeatCount-- > 0)
            {
            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err, iIptvServiceManagementClient->GetUsedIapL(aServiceId,
                                                    aIapId,
                                                    iapName ,
                                                    connectionPermission,
                                                    status) );


                if(err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err != KErrNone)
                {
                VCXLOGLO2("GetUsedIapL caused a leave: %d", err);
                rv = EGeneralError;
                }
            else
            if(CIptvNetworkSelection::ESucceeded != status)
                {
                VCXLOGLO2("GetUsedIapL response error: %d", status);
                rv = EGeneralError;
                }
            }

        iSyncReturnValue = rv;
        }

     if(iSyncReturnValue == ESucceeded)
        {
        VCXLOGLO2("aIapId = %d",aIapId);
        VCXLOGLO2("iapName = %S",&iapName);
        VCXLOGLO2("connectionPermission = %d",connectionPermission);
        VCXLOGLO2("status = %d",status);
        }

    FinishRequest();

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallGetUsedIap()");
    return iSyncReturnValue;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CallSetAllIaps()
// Sets iaplist to all services
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallSetAllIaps(
                                    TIptvSmTestConnection aConn,
                                    CIptvIapList& aIapList,
                                    TBool aIgnoreReadOnlyFlag
                                    )
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CallSetAllIaps()");

    TInt err = KErrNone;

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallSetAllIaps");
        return EGeneralError;
        }


    TRespStatus rv = ESucceeded;
    TBuf<255> iapName;

    if(iRequestRepeatCount <= 0) iRequestRepeatCount = 1;

    // network selection api specific return values!!
    if(aConn == EIptvSmTestAsync)
        {
        VCXLOGLO1("Async service request");

        while(iRequestRepeatCount-- > 0)
            {
            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err, iIptvServiceManagementClient->SetAllIapsReqL(aIapList, aIgnoreReadOnlyFlag) );

                if(err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err != KErrNone)
                {
                VCXLOGLO2("SetAllIapsReqL caused a leave: %d", err);
                VCXLOGLO1("<<<CIptvServiceManagementSync::CallSetAllIaps");
                rv = EGeneralError;
                iSyncReturnValue = rv;
                return EGeneralError;
                }

            if(iSpecialAction == EIptvSmCancelNextAsyncRequest)
                {
                iIptvServiceManagementClient->CancelRequest();
                VCXLOGLO1("Canceling request to server.");
                }
            else
                {
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                }
            }

        }
    else
        {
        VCXLOGLO1("Sync service request");
        TRespStatus rv = ESucceeded;

        while(iRequestRepeatCount-- > 0)
            {
            TInt retryCount = 5;

            while(retryCount-- > 0)
                {
                TRAP(err, iIptvServiceManagementClient->SetAllIapsL(aIapList,
                                                                aIgnoreReadOnlyFlag,
                                                                rv) );

                if(err == KErrNotReady)
                    {
                    VCXLOGLO1("Server not ready, waiting a second before retry.");
                    iIptvTestTimer->After(1 * 1000000);
                    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                    }
                else
                    {
                    break; // request succeeded or there's an error
                    }
                }

            if(err != KErrNone)
                {
                VCXLOGLO2("SetAllIapsL caused a leave: %d", err);
                rv = EGeneralError;
                }
            else
            if(MIptvServiceManagementClientObserver::ESucceeded != rv)
                {
                VCXLOGLO2("SetAllIapsL response error: %d", rv);
                rv = EGeneralError;
                }
            }

        iSyncReturnValue = rv;
        }

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallSetAllIaps()");

    FinishRequest();

    return iSyncReturnValue;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CallSetConnectionAllowed()
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallSetConnectionAllowed(TBool aConnectionAllowed,
                                    TUint32 aIapId)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CallSetConnectionAllowed");

    TInt err = KErrNone;

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallSetConnectionAllowed.");
        return EGeneralError;
        }

    TRespStatus rv = ESucceeded;
    TBuf<255> iapName;
    CIptvNetworkSelection::TRespStatus status = CIptvNetworkSelection::ESucceeded;


    VCXLOGLO1("Sync service request");

    if(iRequestRepeatCount <= 0) iRequestRepeatCount = 1;

    while(iRequestRepeatCount-- > 0)
        {
        TInt retryCount = 5;

        while(retryCount-- > 0)
            {
            TRAP(err, iIptvServiceManagementClient->SetConnectionAllowedL(aConnectionAllowed, aIapId, status) );

            if(err == KErrNotReady)
                {
                VCXLOGLO1("Server not ready, waiting a second before retry.");
                iIptvTestTimer->After(1 * 1000000);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                }
            else
                {
                break; // request succeeded or there's an error
                }
            }

        if(err != KErrNone)
            {
            VCXLOGLO2("SetConnectionAllowedL caused a leave: %d", err);
            rv = EGeneralError;
            }
        else
        if(CIptvNetworkSelection::ESucceeded != status)
            {
            VCXLOGLO2("SetConnectionAllowedL response error: %d", status);
            rv = EGeneralError;
            }
        }

    iSyncReturnValue = rv;

    FinishRequest();

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallSetConnectionAllowed()");
    return iSyncReturnValue;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CallIsConnectionAllowed()
// -----------------------------------------------------------------------------
//
TRespStatus CIptvServiceManagementSync::CallIsConnectionAllowed(CIptvNetworkSelection::TConnectionPermission &aPermission,
                                    TUint32 aIapId)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::CallIsConnectionAllowed");

    TInt err = KErrNone;

    err = PrepareForRequest();
    if(err != KErrNone)
        {
        VCXLOGLO1("<<<CIptvServiceManagementSync::CallIsConnectionAllowed()");
        return EGeneralError;
        }

    TRespStatus rv = ESucceeded;
    TBuf<255> iapName;
    CIptvNetworkSelection::TRespStatus status = CIptvNetworkSelection::ESucceeded;

    VCXLOGLO1("Sync service request");

    if(iRequestRepeatCount <= 0) iRequestRepeatCount = 1;

    while(iRequestRepeatCount-- > 0)
        {
        TInt retryCount = 5;

        while(retryCount-- > 0)
            {
            TRAP(err, iIptvServiceManagementClient->IsConnectionAllowedL(aIapId, aPermission, status) );

            if(err == KErrNotReady)
                {
                VCXLOGLO1("Server not ready, waiting a second before retry.");
                iIptvTestTimer->After(1 * 1000000);
                iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
                }
            else
                {
                break; // request succeeded or there's an error
                }
            }

        if(err != KErrNone)
            {
            VCXLOGLO2("IsConnectionAllowedL caused a leave: %d", err);
            rv = EGeneralError;
            }
        else
        if(CIptvNetworkSelection::ESucceeded != status)
            {
            VCXLOGLO2("IsConnectionAllowedL response error: %d", status);
            rv = EGeneralError;
            }
        }
    iSyncReturnValue = rv;

    FinishRequest();

    VCXLOGLO1("<<<CIptvServiceManagementSync::CallIsConnectionAllowed()");
    return iSyncReturnValue;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::CancelRequest()
// Cancels an outstanding request.
// -----------------------------------------------------------------------------
//
/*void CIptvServiceManagementSync::CancelRequest()
    {
    Cancel() ; // Causes call to DoCancel()
    }
*/

/*****************************************************/
/***************        Utilities   ******************/
/*****************************************************/


// -----------------------------------------------------------------------------
// CIptvServiceManagementApiTest::ResponseStatus()
//
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementSync::ResponseStatus(TUint8 aStatus)
    {
    VCXLOGLO1(">>>CIptvServiceManagementApiTest::ResponseStatus");
    VCXLOGLO2("aStatus = %d", aStatus);
    iSyncReturnValue = static_cast<TRespStatus>(aStatus);
    VCXLOGLO1("<<<CIptvServiceManagementApiTest::ResponseStatus");
    return KErrNone;
    }

/*****************************************************/
/***************        Callbacks   ******************/
/*****************************************************/


// -----------------------------------------------------------------------------
// CIptvVodContentApiTest::TimerComplete
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::TimerComplete(TInt aTimerId, TInt aError)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::TimerComplete");
    VCXLOGLO2("aTimerId = %d", aTimerId);
    VCXLOGLO2("aError = %d", aError);

    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStop);
    VCXLOGLO1("<<<CIptvServiceManagementSync::TimerComplete");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::AddServiceResp()
// iptv_client_api.dll calls this as a response to AddServiceReq()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::AddServiceResp(TRespStatus aRespStatus)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::AddServiceResp");
    //Callback ready

    ResponseStatus(aRespStatus);
    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStop);
    VCXLOGLO1("<<<CIptvServiceManagementSync::AddServiceResp");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::UpdateServiceResp()
// iptv_client_api.dll calls this as a response to UpdateServiceReq()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::UpdateServiceResp(TRespStatus aRespStatus)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::UpdateServiceResp");
   //Callback ready

    ResponseStatus(aRespStatus);
    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStop);
    VCXLOGLO1("<<<CIptvServiceManagementSync::UpdateServiceResp");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::DeleteServiceResp()
// iptv_client_api.dll calls this as a response to DeleteServiceReq()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::DeleteServiceResp(TRespStatus aRespStatus)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::DeleteServiceResp");
    //Callback ready

    ResponseStatus(aRespStatus);
    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStop);
    VCXLOGLO1("<<<CIptvServiceManagementSync::DeleteServiceResp");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::GetServicesResp()
// iptv_client_api.dll calls this as a response to GetServicesReq()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::GetServicesResp(TRespStatus aRespStatus, CDesC8ArraySeg* aServicesArray)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::GetServicesResp");

// check this
    ResponseStatus(aRespStatus);
    iServicesArray = aServicesArray;
    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStop);
    VCXLOGLO1("<<<CIptvServiceManagementSync::GetServicesResp");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::ServerShutdownResp()
// iptv_client_api.dll calls this as a response to ServerShutdownResp()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::ServerShutdownResp(TRespStatus aRespStatus)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::ServerShutdownResp");
    //Callback ready
    ResponseStatus(aRespStatus);
    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStop);
    VCXLOGLO1("<<<CIptvServiceManagementSync::ServerShutdownResp");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::GetUsedIapResp()
// iptv_client_api.dll calls this as a response to GetUsedIapResp()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::GetUsedIapResp(TUint32 aIapId,
                                                const TDesC& aIapName,
                                                CIptvNetworkSelection::TConnectionPermission aConnectionPermission,
                                                TBool   aWlanWhenGPRS,
                                                CIptvNetworkSelection::TRespStatus aRespStatus)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::GetUsedIapResp");
    //Callback ready

    VCXLOGLO2("aRespStatus = %d", aRespStatus);
    VCXLOGLO2("aWlanWhenGPRS = %d", aWlanWhenGPRS);
    TRespStatus respStatus = ESucceeded;

    if(CIptvNetworkSelection::ESucceeded != aRespStatus)
        {
        respStatus = EGeneralError;
        iIapId = 0;
        }
    else
        {
        respStatus = ESucceeded;
        iIapId = aIapId;
        VCXLOGLO2("aIapId = %d",aIapId);
        VCXLOGLO2("iapName = %S",&aIapName);
        VCXLOGLO2("connectionPermission = %d",aConnectionPermission);
        }

    ResponseStatus(respStatus);
    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStop);

    VCXLOGLO1("<<<CIptvServiceManagementSync::GetUsedIapResp");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::SetSpecialAction()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::SetSpecialAction(TInt aSpecialAction)
    {
    switch(aSpecialAction)
        {
        case EIptvSmNoSpecialAction:
        default:
            break;

        case EIptvSmWaitNoEvents:
            iWaitNoEvents = ETrue;
            break;

        case EIptvSmCancelNextAsyncRequest:
            iSpecialAction = static_cast<TIptvSmSpecialAction>(aSpecialAction);
            break;
        }
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::WaitForProvisioningEvents()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::WaitForProvisioningEvents()
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::WaitForProvisioningEvents");

    iProvisioningWaitGoing = ETrue;
    do
        {
        iProvisioningEventReceived = EFalse;

        iIptvTestTimer->CancelTimer();
        iIptvTestTimer->After(1000000);
        iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);
        }
        while (iProvisioningEventReceived);
    iProvisioningWaitGoing = EFalse;

    VCXLOGLO1("<<<CIptvServiceManagementSync::WaitForProvisioningEvents");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::RepeatNextRequest()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::RepeatNextRequest(TBool aRequestRepeatCount)
    {
    iRequestRepeatCount = aRequestRepeatCount;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::GetRepeatNextRequestCount()
// -----------------------------------------------------------------------------
//
TUint CIptvServiceManagementSync::GetRepeatNextRequestCount()
    {
    if(iRequestRepeatCount < 0) iRequestRepeatCount = 1;
    return iRequestRepeatCount;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::SetExpectedSmEvent()
// -----------------------------------------------------------------------------//
void CIptvServiceManagementSync::SetExpectedSmEvent(CIptvSmEvent::TEvent aEvent, TInt aEventCount)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::SetExpectedSmEvent");
    iEventReceived = EFalse;

    if(iIgnoreEvents)
        {
        iWaitingForEvent = EFalse;
        VCXLOGLO1("Not waiting event flag is set for next request.");
        }
    else
        {
        VCXLOGLO1("******************************************************************");
        switch(aEvent)
            {
            case CIptvSmEvent::EServiceAdded:
                {
                VCXLOGLO1("Event: EServiceAdded");
                break;
                }

            case CIptvSmEvent::EServiceModified:
                {
                VCXLOGLO1("Event: EServiceModified");
                break;
                }

            case CIptvSmEvent::EServiceDeleted:
                {
                VCXLOGLO1("Event: EServiceDeleted");
                break;
                }

            case CIptvSmEvent::EServiceScheduleModified:
                {
                VCXLOGLO1("Event: EServiceScheduleModified");
                break;
                }

            default:
                VCXLOGLO2("Unexpected event: %d", aEvent);
                break;
            }
        
        if( iExpectedEventCount > 0 )
            {
            VCXLOGLO3("Already expecting %d events, adding %d to count.", iExpectedEventCount, aEventCount);
            iExpectedEventCount += aEventCount;
            }
        else
            {
            iExpectedEventCount = aEventCount;
            }
        VCXLOGLO2("Expected count: %d", iExpectedEventCount);
        
        iWaitingForEvent = ETrue;
        iExpectedSmEvent = aEvent;
        }
    VCXLOGLO1("<<<CIptvServiceManagementSync::SetExpectedSmEvent");
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::HandleSmEvent()
// -----------------------------------------------------------------------------
//
void CIptvServiceManagementSync::HandleSmEvent(CIptvSmEvent& aEvent)
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::HandleSmEvent");
    VCXLOGLO1("**********************************************************");
	VCXLOGLO2("CIptvServiceManagementSync:: iEvent: %d", aEvent.iEvent);
	VCXLOGLO2("CIptvServiceManagementSync:: iServiceId: %d", aEvent.iServiceId);

    switch(aEvent.iEvent)
        {
        case CIptvSmEvent::EServiceAdded:
            {
            VCXLOGLO1("Event: EServiceAdded");
            }
            break;

        case CIptvSmEvent::EServiceModified:
            {
            VCXLOGLO1("Event: EServiceModified");
            }
            break;

        case CIptvSmEvent::EServiceDeleted:
            {
            VCXLOGLO1("Event: EServiceDeleted");
            }
            break;

        case CIptvSmEvent::EServiceScheduleModified:
            {
            VCXLOGLO1("Event: EServiceScheduleModified");
            }
            break;

		case CIptvSmEvent::EServiceSelectionModified:
			{
			VCXLOGLO1("Event: EServiceSelectionModified");
			}
			break;

        default:
            VCXLOGLO1("CIptvServiceManagementSync:: Unexpected event!");
            break;
        }

    if(iProvisioningWaitGoing)
        {
        VCXLOGLO1("Wait for provisioning events going.");
        iProvisioningEventReceived = ETrue;
        VCXLOGLO1("<<<CIptvServiceManagementSync::HandleSmEvent");
        return;
        }

    if(iIgnoreEvents)
        {
        VCXLOGLO1("Ignoring the event.");
        VCXLOGLO1("<<<CIptvServiceManagementSync::HandleSmEvent");
        return;
        }

    if(iWaitingForEvent)
        {
        iEventReceived = ETrue;

        iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStop);
        if(iIptvTestTimer)
            {
            iIptvTestTimer->CancelTimer();
            }

        if(aEvent.iEvent != iExpectedSmEvent)
            {
            // We let modified event go for added event
            if( !(iExpectedSmEvent == CIptvSmEvent::EServiceAdded && (aEvent.iEvent == CIptvSmEvent::EServiceModified
                || aEvent.iEvent == CIptvSmEvent::EServiceSelectionModified) ) )
                {
                VCXLOGLO2("* ERROR * Received an unexpected event, was waiting for %d. Leaving!", iExpectedSmEvent);
                User::Leave(KErrUnknown);
                }
            else
                {
                VCXLOGLO1("Correct event received.");
                }
            }
        
        --iExpectedEventCount;
        
        if( iExpectedEventCount <= 0)
            {
            // stop the possible wait for event
            iWaitingForEvent = EFalse;
            }
        }
    else
        {
        VCXLOGLO1("* ERROR * Received an event when not waiting for one. Leaving!");
        User::Leave(KErrUnknown);
        }

    VCXLOGLO1("<<<CIptvServiceManagementSync::HandleSmEvent");
    }

void CIptvServiceManagementSync::IncreaseExpectedEventCount( TInt aAmount )
    {
    iExpectedEventCount += aAmount;    
    VCXLOGLO3("Increasing expected event count by %d, now waiting for %d events.", aAmount, iExpectedEventCount);
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::WaitForSmEvent()
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementSync::WaitForSmEvent()
    {
    if(iIgnoreEvents) return KErrNone;

    VCXLOGLO1(">>>CIptvServiceManagementSync::WaitForSmEvent");
    VCXLOGLO1("--------------------------------------------------");

    if(!iWaitingForEvent)
        {
        if(!iEventReceived)
            {
            VCXLOGLO1("* ERROR * Not waiting for an event and no event received. Test code logic error.");
            VCXLOGLO1("<<<CIptvServiceManagementSync::WaitForSmEvent");
            return KErrUnknown;
            }
        else
            {
            // we got the event before call to this method so no need to wait
            VCXLOGLO1("Event received before this method call.");
            VCXLOGLO1("<<<CIptvServiceManagementSync::WaitForSmEvent");
            return KErrNone;
            }
        }

    iIptvTestTimer->After(4 * 1000000);
    iActiveWait->ActiveWait(CIptvTestActiveWait::EActiveWaitStart);

    if(!iEventReceived)
        {
        VCXLOGLO2("* ERROR * Did not receive expected event: %d", iExpectedSmEvent);
        VCXLOGLO1("<<<CIptvServiceManagementSync::WaitForSmEvent");
    //  return KErrUnknown;
        }

    VCXLOGLO2("Event received ok. %d", iExpectedSmEvent);
    VCXLOGLO1("<<<CIptvServiceManagementSync::WaitForSmEvent");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::PrepareForRequest()
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementSync::PrepareForRequest()
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::PrepareForRequest");

    if(iWaitNoEvents)
        {
        // special action set and we ignore all events for this request
        iIgnoreEvents = ETrue;
        iWaitNoEvents = EFalse;
        }
    else
        {
        // for default we check the events
        iIgnoreEvents = EFalse;
        }

    if(iRequestRepeatCount <= 0) iRequestRepeatCount = 1;

    TInt rv = KErrNone;

    VCXLOGLO1("<<<CIptvServiceManagementSync::PrepareForRequest");
    return rv;
    }

// -----------------------------------------------------------------------------
// CIptvServiceManagementSync::FinishRequest()
// -----------------------------------------------------------------------------
//
TInt CIptvServiceManagementSync::FinishRequest()
    {
    VCXLOGLO1(">>>CIptvServiceManagementSync::FinishRequest");

    iIgnoreEvents = ETrue;
    iWaitNoEvents = EFalse;

    iSpecialAction = EIptvSmNoSpecialAction;

    VCXLOGLO1("<<<CIptvServiceManagementSync::FinishRequest");
    return KErrNone;
    }


// End of File
