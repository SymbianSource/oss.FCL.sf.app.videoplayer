/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   header file for CIptvServiceManagementSync class*
*/




#ifndef __CIPTVSERVICEMANAGEMENTSYNC_H__
#define __CIPTVSERVICEMANAGEMENTSYNC_H__

// INCLUDE FILES
#include <e32base.h>
#include "MIptvServiceManagementClientObserver.h"
#include "CIptvServiceManagementClient.h"
#include "CIptvService.h"
#include "MIptvTestTimerObserver.h"
#include <badesca.h>
#include <f32file.h>
#include <S32FILE.H>

// DATA TYPES
typedef  MIptvServiceManagementClientObserver::TRespStatus TRespStatus;

// FORWARD DECLARATIONS
class CIptvServiceManagementClient;
class CIptvTestTimer;
class CIptvTestUtilities;
class CIptvTestActiveWait;
class CIptvSmEvent;

// CLASS DECLARATION
class CIptvServiceManagementSync : public CBase, public MIptvServiceManagementClientObserver, public MIptvTestTimerObserver
    {
    public: //enums

        /**
        * TIptvSmTestConnection
        */
        enum TIptvSmTestConnection
            {
            EIptvSmTestSync,
            EIptvSmTestAsync
            } ;
        /**
        * TIptvSmSpecialAction
        */
        enum TIptvSmSpecialAction
            {
            EIptvSmNoSpecialAction,
            EIptvSmCancelNextAsyncRequest,
            EIptvSmWaitNoEvents
            };

    public: // Constructors and destructors
        /**
        * NewL.
        * Two-phased constructor.
        * Creates a CIptvServiceManagementSync object using two phase construction,
        * and return a pointer to the created object.
        * @param aClient pointer to client, no ownership is transferred.
        * @return A pointer to the created instance of CIptvServiceManagementSync.
        */
        static CIptvServiceManagementSync* NewL();

        /**
        * ~CIptvServiceManagementSync.
        * Destructor.
        * Destroys the object and releases all memory objects.
        */
        virtual ~CIptvServiceManagementSync();

    public: // New functions

        /**
        * Adds service to service database using CIptvServiceManagementClient method AddServiceReqL or AddServiceL.
        * @param aConn                  Async/sync call.
        * @param aService               Service data which is added to database.
        * @return Response status.
        */
        TRespStatus CallAddService(TIptvSmTestConnection aConn, CIptvService& aService);

        /**
        * Updates existing service in service database using CIptvServiceManagementClient method UpdateServiceReqL or UpdateServiceL.
        * @param aConn                  Async/sync call.
        * @param aService               Updated service data.
        * @param aExpectedEvent         Expected event, this usually is CIptvSmEvent::EServiceModified.
        * @return Response status.
        */
        TRespStatus CallUpdateService(TIptvSmTestConnection aConn, CIptvService& aService, CIptvSmEvent::TEvent aExpectedEvent);

        /**
        * Gets services by id from service database using CIptvServiceManagementClient method
        * GetServicesReqL or GetServicesL.
        * @param aConn                  Async/sync call.
        * @param aStartId
        * @param aEndId
        * @param aServicesArray         Returned serviecs are stored here
        * @return Response status.
        */
        TRespStatus CallGetServices(
                                    TIptvSmTestConnection aConn,
                                    TUint32 aStartId,
                                    TUint32 aEndId,
                                    CDesC8ArraySeg** aServicesArray
                                    );

        /**
        * Gets services by provider id from service database by calling method CIptvServiceManagementClient::GetServicesL.
        * @param aProviderId
        * @param aServices              Returned serviecs are stored here
        * @return Response status.
        */
        TRespStatus CallGetServicesByProviderId(
                                    TDesC& aProviderId,
                                    CIptvServices& aServices
                                    );

        /**
        * Gets filtered services from service database using CIptvServiceManagementClient method GetServicesReqL or
        * GetServicesL.
        * @param aSearchLimitFlags
        * @param aOrder
        * @param aConn                  Async/sync call.
        * @param aServicesArray         Returned serviecs are stored here.
        * @return Response status.
        */
        TRespStatus CallGetServicesFiltered(
                                            TUint32 aSearchLimitFlags,
                                            CIptvServiceManagementClient::TOrder aOrder,
                                            TIptvSmTestConnection aConn,
                                            CDesC8ArraySeg** aServicesArray
                                            );

        /**
        * Deletes a service from service database using CIptvServiceManagementClient method DeleteServiceReqL or
        * DeleteServiceL.
        * @param aConn                  Async/sync call.
        * @param aId                    Id of service to be deleted.
        * @return Response status.
        */
        TRespStatus CallDeleteService(TIptvSmTestConnection aConn, TUint32 aId);

        /**
        * Functionality disabled in CIptvServiceManagementClient.
        * Requests IPTV server to shutdown itself. Request is asynchronous.
        * @param aConn                  Async/sync call.
        * @return Response status.
        */
        TRespStatus CallShutdownServer(TIptvSmTestConnection aConn );

        /**
        * Sets the used IAP for a service using CIptvServiceManagementClient method SetUsedIapL.
        * @param aIapId
        * @param aServiceId
        * @param aSetToDefaultForService
        * @return Response status.
        */
        TRespStatus CallSetUsedIap(TUint32 aIapId,
                                  TUint32 aServiceId,
                                  TBool aSetToDefaultForService
                                  );
        /**
        * Gets the used IAP for a service using CIptvServiceManagementClient method GetUsedIapL or GetUsedIapReqL.
        * @param aConn                  Async/sync call.
        * @param aServiceId
        * @param aIapId
        * @return Response status.
        */
        TRespStatus CallGetUsedIap(
                                  TIptvSmTestConnection aConn,
                                  TUint32 aServiceId,
                                  TUint32& aIapId
                                  );

        /**
        * Clears the used IAP for a service using CIptvServiceManagementClient method ClearUsedIap.
        * @return Response status.
        */
        TRespStatus CallClearUsedIap();

        /**
        * Sets IAP list to all services using CIptvServiceManagementClient method SetAllIapsReqL or SetAllIapsL.
        * @param aConn                  Async/sync call.
        * @param aIapList
        * @param aIgnoreReadOnlyFlag
        * @return Response status.
        */
        TRespStatus CIptvServiceManagementSync::CallSetAllIaps(
                                    TIptvSmTestConnection aConn,
                                    CIptvIapList& aIapList,
                                    TBool aIgnoreReadOnlyFlag
                                    );

        /**
        * Sets connection allowed flag for service using CIptvServiceManagementClient method SetConnectionAllowedL.
        * @param aConnectionAllowed
        * @param aIapId
        * @return Response status.
        */
        TRespStatus CallSetConnectionAllowed(TBool aConnectionAllowed,
                                    TUint32 aIapId);

        /**
        * Gets the connection allowed flag for service using CIptvServiceManagementClient method IsConnectionAllowedL.
        * @param aPermission
        * @param aIapId
        * @return Response status.
        */
        TRespStatus CallIsConnectionAllowed(CIptvNetworkSelection::TConnectionPermission &aPermission,
                                    TUint32 aIapId);



        /**
        * Cancels the outstanding request.
        */
        void CancelRequest() ;

        /**
        * Sets the repeat count for next request. Async requests will be sent w/o waiting the request to complete
        * @param aCount
        */
        void RepeatNextRequest(TInt aRequestRepeatCount);

        /**
        * Gets the count of repeats for next request.
        * @return Count of repeats.
        */
        TUint GetRepeatNextRequestCount();

        /**
        * Sets the member variables which point to the database locations.
        * @param aServicesDbFile
        * @param aPasswordsDbFile
        */
        void SetDbFileLocations(TDesC& aServicesDbFile, TDesC& aPasswordsDbFile);

        /**
        * Sets special action.
        * @param aSpecialAction
        */
        void SetSpecialAction(TInt aSpecialAction);

        /**
        * Waits for a second for SM events and if event is received starts the wait again.
        */
        void WaitForProvisioningEvents();

        /**
        * Gets the instance of CIptvServiceManagementClient. Ownership doesn't transfer.
        * @return Pointer to instance of iIptvServiceManagementClient.
        */
        CIptvServiceManagementClient* GetSmClient() { return iIptvServiceManagementClient; }

    protected: // Functions from base classes


    public: // Functions from base classes

        /**
        * From MIptvTestTimerObserver. Asynchronous callback observer function signaling application that timer has elapsed.
        * @since
        * @param error Error code.
        */
        virtual void TimerComplete(TInt aTimerId, TInt aError);

        /*
        * From MIptvServiceManagementClientObserver. Callback function for AddServiceReq operation.
        * @param aRespStatus
        */
        void AddServiceResp(TRespStatus aRespStatus);

        /**
        * From MIptvServiceManagementClientObserver. Callback function for UpdateServiceReq.
        * @param aRespStatus
        */
        void UpdateServiceResp(TRespStatus aRespStatus);

        /**
        * From MIptvServiceManagementClientObserver. Callback function for DeleteServiceReq operation.
        * @param aRespStatus
        */
        void DeleteServiceResp(TRespStatus aRespStatus);

        /**
        * From MIptvServiceManagementClientObserver. Callback function for GetServiceReq operation.
        * aServicesArray ownership moves from dll to application, ie application
        * is responsible for freeing the array.
        * Array elements are binary descriptors built with CIptvService::GetL() method.
        * CIptvService::Set() method can be used to init CIptvService class with
        * array element data.
        * @param aRespStatus
        * @param aServicesArray contains array of pointers to descriptors.
        */
        void GetServicesResp(TRespStatus aRespStatus,
                                        CDesC8ArraySeg* aServicesArray);

        /**
        * From MIptvServiceManagementClientObserver. Callback function for ShutdownServerReq operation.
        * @param aRespStatus
        */
        void ServerShutdownResp(TRespStatus aRespStatus);

        /**
        * From MIptvServiceManagementClientObserver. Callback function for GetUsedIapReq() method.
        * @param aRespStatus
        * @param aIapId IAP ID which client should use.
        *        In case of error, aIapId is 0.
        */
        void GetUsedIapResp(TUint32 aIapId,
                            const TDesC& aIapName,
                            CIptvNetworkSelection::TConnectionPermission aConnectionPermission,
                           TBool   aWlanWhenGPRS,
                            CIptvNetworkSelection::TRespStatus aRespStatus);

        /**
        * From MIptvServiceManagementClientObserver. Callback for Service Manager generated events.
        * @param aEvent
        */
        void HandleSmEvent(CIptvSmEvent& aEvent);
        
        /**
        * Increases the expected event count.
        */
        void IncreaseExpectedEventCount( TInt aAmount );


    private:
        /**
        * CIptvServiceManagementSync.
        * Default C++ constructor.
        * @param
        */
        CIptvServiceManagementSync();

        /**
        * ConstructL.
        * Performs the second phase construction of a
        * CIptvRequestHandler object.
        */
        void ConstructL();

        /**
        * Sets iSyncReturnValue member variable.
        */
        TInt ResponseStatus(TUint8 aStatus);

        /**
        * Prepares for a request. Special actions set etc.
        */
        TInt PrepareForRequest();

        /**
        * Finishes request.
        */
        TInt FinishRequest();

        /**
        * Sets the expected event from CIptvServiceManagementClient.
        */
        void SetExpectedSmEvent(CIptvSmEvent::TEvent aEvent, TInt aEventCount);
        
        /**
        * Starts waiting asynchronously for next event from CIptvServiceManagementClient.
        */
        TInt WaitForSmEvent();

    private: // Data

        // Instance of CIptvTestActiveWait
        CIptvTestActiveWait* iActiveWait;

        // iSyncReturnValue Return value of synchronous request function.
        MIptvServiceManagementClientObserver::TRespStatus iSyncReturnValue;

        // Services from async GetService requests response are stored here.
        CDesC8ArraySeg* iServicesArray;

        // Instance of CIptvServiceManagementClient
        CIptvServiceManagementClient*   iIptvServiceManagementClient;

        // Iap ID from async GetUsedIap request response is stored here.
        TUint32 iIapId;

        // How many times next request is repeated.
        TInt iRequestRepeatCount;

        // The *special* action
        TIptvSmSpecialAction iSpecialAction;

        // Instance of CIptvTestTimer
        CIptvTestTimer*             iIptvTestTimer;

        // Instance of CIptvTestUtilities
        CIptvTestUtilities*         iIptvTestUtilities;

        /*
            ->PrepareForRequest sets IgnoreEvents OFF if WaitNoEvents is not set
            ->WaitingForEvent is SET
            ->Expected event is SET
            ->Request is made
            ->WaitForSmEvent is called, wait 6 seconds
            ->HandleSmEvent callback is called when events are received
                -> events are checked if ignoring is OFF
            ->FinishRequest sets IgnoreEvents ON
        */

        // Ignore is set always when there's no request made by this class
        TBool iIgnoreEvents;

        // Set when updating other service field than address because only that causes modify event
        TBool iWaitNoEvents;

        // Set when waiting for an event.
        TBool iWaitingForEvent;

        // Set when event is received.
        TBool iEventReceived;

        // The expected event.
        CIptvSmEvent::TEvent        iExpectedSmEvent;
        
        TInt iExpectedEventCount;

        // The expected events.
        RArray<CIptvSmEvent::TEvent> iExpectedSmEvents;

        // Are we currently waiting for provisioining to finish
        TBool iProvisioningWaitGoing;

        // Is provisioning event received.
        TBool iProvisioningEventReceived;
    };


#endif //__CIptvServiceManagementSync_H__

// End of File