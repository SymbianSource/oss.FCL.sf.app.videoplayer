/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header file for CIptvServiceManagementClient class*
*/





#ifndef __CIPTVSERVICEMANAGEMENTCLIENT_H__
#define __CIPTVSERVICEMANAGEMENTCLIENT_H__

// INCLUDE FILES
#include <e32base.h>
#include "CIptvIapList.h"
#include "CIptvTimer.h"
#include "MIptvTimerObserver.h"

#include "CIptvClientBase.h"
#include "RIptvClientSession.h"
#include "IptvClientServerCommon.h"
#include "CIptvNetworkSelection.h"
#include "CIptvSmEvent.h"
#include "CIptvServices.h"
#include "MIptvServiceManagementClientObserver.h"

// FORWARD DECLARATIONS
class CIptvSmClientSynchronizer;
class CIptvService;
class CIptvServices;
class MIptvServiceManagementClient;
class MIptvEvent;
class CIptvEventListener;

// CLASS DECLARATION
/**
*  CIptvServiceManagementClient is the API object for IPTV Service database and Network Selection.\n
*  Application must implement virtual callback functions from MIptvServiceManagementClientObserver class.\n
*
*  Instantiating of CIptvServiceManagementClient will start IPTV engine server if it is not running\n
*  already.
*
*  Only one request pending per CIptvServiceManagementClient object is allowed simultaneously.\n
*  There is no protocol involved in calling functions. Any request may be called anytime assuming\n
*  that there is no other request pending already.\n
*  After service shutdown, client may not call any API functions anymore for that\n
*  CIptvServiceManagementClient object.\n
*
*  Example:\n
*
*   void CIptvClient::SomeFunc()\n
*       {\n
*       iIptvServiceManagementClient = CIptvServiceManagementClient::NewL(*this);\n							
*       iIptvServiceManagementClient->GetAllServicesReq();\n
*       }\n
*                  
*   void CIptvClient::GetAllServicesResp(MIptvServiceManagementClientObserver::TRespStatus aRespStatus,\n
*                                                  CDesC16ArraySeg* aServicesArray)\n
*   {\n
*    
*   TInt i;\n
*
*   CIptvService* iptvService;\n
*   iptvService = CIptvService::NewL();\n				
*   CleanupStack::PushL(iptvService);\n
*			
*   for(i = 0; i < aServicesArray->MdcaCount(); i++)\n
*      {\n
*      iptvService->Set(aServicesArray->MdcaPoint(i));\n
*      RDebug::Print(_L("Service: %S"), &(iptvService->GetName()));\n
*      }\n
*
*   CleanupStack::PopAndDestroy(iptvService);\n
*   delete aServicesArray;\n
*
*   delete iIptvServiceManagementClient;\n
*   iIptvServiceManagementClient = NULL;\n
*   }\n
*/
class CIptvServiceManagementClient : public CIptvClientBase
    {
    public:
    
        /**
        * Flags used for limiting search from the database.
        */
        enum TSearchLimitFlag
            {
            ESelectedServices = ( 1 << 0 ),  // 1
            EMainServices     = ( 1 << 1 ),  // 2
            ESubServices      = ( 1 << 2 ),  // 4
            EVod              = ( 1 << 3 ),  // 8
            ELiveTv           = ( 1 << 4 ),  // 16
            EVodCast          = ( 1 << 5 ),  // 32
            EBrowser          = ( 1 << 6 ),  // 64
            EVideoRemote      = ( 1 << 7 ),  // 128
            EApplication      = ( 1 << 8 ),  // 256
            EMobileTv         = ( 1 << 9 ),  // 512
            EVodServices      = ( 1 << 10 ), // 1024
            EServiceGroup     = ( 1 << 11 ), // 2048
            EGroupedServices  = ( 1 << 12 ), // 4096
            EOther            = ( 1 << 13 )  // 8192
            };
        
        /**
        * Defines the sorting order of the search.
        */    
        enum TOrder
            {
            /**
            * Sort by creation date, ascending
            */
            EDateAscending = 0,
            
            /**
            * Sort by creation date, descending
            */
            EDateDescending,
            
            /**
            * Sort by display order, ascending
            */
            EDisplayOrderAscending,

            /**
            * Sort by display order, descending
            */
            EDisplayOrderDescending
            };

    private:
    
        /**
        * We have to know which service entity has the message pending
        * in order to send correct cancel message.
        */
        enum TRequestPendingFor
            {
            ENoRequestPending = 0,
            EServerRequestPending,
            EServiceManagerRequestPending,
            ENetworkSelectionRequestPending
            };
                                 		    		       
    public: // Constructors and destructors
        /**
        * Two-phased constructor.
        * Creates a CIptvServiceManagementClient object using two phase construction,
        * and return a pointer to the created object.
        * @param aClient A reference to client.
        * @return        A pointer to the created instance of CIptvServiceManagementClient.
        */
        IMPORT_C static CIptvServiceManagementClient* NewL(MIptvServiceManagementClientObserver& aClient);
		
        /**
        * Destructor.
        * Destroys the object and releases all memory objects.
        */
        IMPORT_C virtual ~CIptvServiceManagementClient();

    public: // New functions

        /**
         * Adds a new entry to Services database asynchronously.
         * AddServiceResp is the callback function.
         *
         * @param aService Service data which is added to database.
         * @return         KErrNone if request was sent to server successfully,
         *                 system-wide error code otherwise.
         */
        IMPORT_C TInt AddServiceReqL(CIptvService& aService);

        /**
         * Adds a new entry to Services database synchronously.
         *
         * @param aService    Service data which is added to database.
         * @param aRespStatus Status of the operation.
         * @return            Returns KErrNone if request was sent to server successfully,
         *                    system-wide error code otherwise. If != KErrNone, aRespStatus
         *                    doesn't contain valid data.
         */
        IMPORT_C TInt AddServiceL(CIptvService& aService,
		                              MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);

        /**
         * Updates existing entry in Services database asynchronously.
         * Every item in CIptvService class is compared with database values,
         * if they differ the database value is changed.
         * If entry does not exist, EServiceNotFound error is given in UpdateServiceResp
         * callback function argument.
         * @param aService New service data
         * @return         Returns KErrNone if request was sent to server successfully,
         *                 system-wide error code otherwise.
         */
        IMPORT_C TInt UpdateServiceReqL(CIptvService& aService);

        /**
         * Updates existing entry in Services database synchronously.
         * If entry does not exist, EServiceNotFound error is given in aRespStatus.
         * @param aService    New service data
         * @param aRespStatus Status of the operation.
         * @return            Returns KErrNone if request was sent to server successfully,
         *                    System-wide error code otherwise. If != KErrNone, aRespStatus
         *                    doesn't contain valid data.
         */
        IMPORT_C TInt UpdateServiceL(CIptvService& aService,
                                     MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);

        /**
         * Gets services from the database asynchronously.
         * GetServicesResp is the callback function.
         * @param aSearchLimitFlags Indicates how the search is limited. Flags can be
         *                          combined with "or" -operation. 0 returns all services.
         * @param aOrder            Order in which the services are returned.
         * @return                  Returns KErrNone if request was sent to server successfully,
         *                          system-wide error code otherwise.
         */
        IMPORT_C TInt GetServicesReqL(TUint32 aSearchLimitFlags, TOrder aOrder);

        /**
         * Gets services from database synchronously.
         * @param aSearchLimitFlags Indicates how the search is limited, ie only services which have the feature
         *                          flag set are included in the search. See TSearchLimitFlag
         *                          for bit values, flags can be combined with "or" -operation.
         *                          0 returns all services.
         * @param aOrder            Order in which the services are returned.
         * @param aServicesArray    Contains array of pointers to descriptors.
         *                          aServicesArray ownership is transferred to caller, ie client
         *                          is responsible for freeing the array.
         *                          Array elements are binary descriptors built with CIptvService::GetL() method.
         *                          CIptvService::SetL() method can be used to init CIptvService class with
         *                          array element data. Client should ensure that aServicesArray pointer
         *                          does not point to reserved memory when calling this method, since the pointer
         *                          will be overwritten.
         *                          In case of error, aServicesArray is set to NULL.
         * @param aRespStatus       ESucceeded is returned even if no matching
         *                          services were found. In that case the aServicesArray contains
         *                          empty array. If aRespStatus != ESucceeded, aServicesArray
         *                          pointer is NULL.
         * @return                  Returns KErrNone if request was sent to server successfully,
         *                          system-wide error code otherwise. If != KErrNone, aServicesArray
         *                          and aRespStatus do not contain valid data.
         */
        IMPORT_C TInt GetServicesL(TUint32 aSearchLimitFlags,
                                   TOrder aOrder,
                                   CDesC8ArraySeg*& aServicesArray,
                                   MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);

        /**
         * Gets service entries from Services database asynchronously.
         * Services whos ID are between aStartId and aEndId are returned.
         * This method is not meant for searching the database, one should know
         * service ids already when issuing the call.
         * GetServicesResp is the callback function.
		     * @param aStartId Unique ID of the first service to be fetched.
		     * @param aEndId   Unique ID of the last service to be fetched.
         * @return         Returns KErrNone if request was sent to server successfully,
         *                 system-wide error code otherwise.
         */
        IMPORT_C TInt GetServicesReqL(TUint32 aStartId, TUint32 aEndId);

        /**
         * Gets service entries from Services database synchronously.
         * Services whos ID are between aStartId and aEndId are returned.
         * This method is not meant for searching the database, one should know
         * service ids already when issuing the call.
         * @param aStartId       Unique ID of the first service to be fetched.
         * @param aEndId         Unique ID of the last service to be fetched.
         * @param aServicesArray Contains array of pointers to descriptors.
         *                       aServicesArray ownership is transferred to caller, ie application
         *                       is responsible for freeing the array.
         *                       Array elements are binary descriptors built with CIptvService::GetL() method.
         *                       CIptvService::SetL() method can be used to init CIptvService class with
         *                       array element data. Client should ensure that aServicesArray pointer
         *                       does not point to reserved memory when calling this method, since the pointer
         *                       will be overwritten.
         *                       In case of error, aServicesArray is set to NULL.
         * @param aRespStatus    ESucceeded is returned even if no matching
         *                       services were found. In that case the aServicesArray contains
         *                       empty array. If aRespStatus != ESucceeded, the aServicesArray
         *                       pointer is NULL.
         * @return Returns       KErrNone if request was sent to server successfully,
         *                       system-wide error code otherwise. If != KErrNone, aServicesArray and
         *                       aRespStatus do not contain valid data.
         */
        IMPORT_C TInt GetServicesL(TUint32 aStartId,
                                   TUint32 aEndId,
                                   CDesC8ArraySeg*& aServicesArray,
                                   MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);

        
        /**
         * Deletes entry from Service database asynchronously.
         * If entry is not found, ESucceeded is still returned
         * in aRespStatus.
         * DeleteServiceResp is the callback function.
         *
         * @param aId Unique ID of the service to delete.
         * @return    Returns KErrNone if request was sent to server successfully,
         *            System-wide error code otherwise.
         */
        IMPORT_C TInt DeleteServiceReqL(TUint32 aId);

        /**
         * Deletes entry from Service database synchronously.
         * If entry is not found, ESucceeded is still returned
         * in aRespStatus.
         *
         * @param aId         Unique ID of the service to delete.
         * @param aRespStatus Status of the operation.
         * @return            Returns KErrNone if request was sent to server successfully,
         *                    System-wide error code otherwise. If != KErrNone, aRespStatus
         *                    doesn't contain valid data.
         */
        IMPORT_C TInt DeleteServiceL(TUint32 aId,
                                     MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);

        /**
         * Gets services by provider ID synchronously.
         *
         * @param aProviderId Provider ID in Services database.  
         * @param aServices   All services which provider ID match, will be written here.
         * @return            Returns KErrNone if operation was successful,
         *                    system-wide error code otherwise. If != KErrNone, aServices
         *                    does not contain valid data.
         */
        IMPORT_C TInt GetServicesL(const TDesC& aProviderId,
                                   CIptvServices& aServices);

        /**
         * Sets IAP list to every service asynchronously.
         * SetAllIapsResp is the callback function.
         * Not supported currently.
         *
         * @param aIapList            IAP list to be written to all service records.
         * @param aIgnoreReadOnlyFlag If ETrue the read only flag has no effect on Service DB,
         *                            ie. all services are changed.
         */
        IMPORT_C void SetAllIapsReqL(CIptvIapList& aIapList, TBool aIgnoreReadOnlyFlag);

        /**
         * Sets IAP list to every service synchronously.
         * Not supported.
         *
         * @param aIapList            IAP list to be written to all service records.
         * @param aIgnoreReadOnlyFlag If ETrue the read only flag has no effect on Service DB,
         *                            ie. all services are changed.
         */
        IMPORT_C void SetAllIapsL(CIptvIapList& aIapList,
                                  TBool aIgnoreReadOnlyFlag,
                                  MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);
        
        /* Network selection API */

        /**
         * Sets IAP aIapId to be used for all services.
         * This setting is valid for the server lifetime.
         * This function is synchronous.
         *
         * @param aIapId                  IAP ID to be used.
         * @param aServiceId.             Service ID in Services database.
         * @param aSetToDefaultForService If ETrue, then the IAP is written to Service database
         *                                (only to aServiceId) and it lasts over reboots.
         * @param aRespStatus             Status of the operation.
         */
        IMPORT_C void SetUsedIapL(TUint32 aIapId,
                                  TUint32 aServiceId,
                                  TBool aSetToDefaultForService,
                                  CIptvNetworkSelection::TRespStatus& aRespStatus);

        /**
         * Clears used IAP from Network Selection, set previosly with SetUsedIapL
         * method.
         * Function is synchronous.
         *
         * @return System-wide error code.
         */        
        IMPORT_C TInt ClearUsedIap();

        /**
         * Gets IAP ID which is used when connecting to aServiceId.
         * Function is asynchronous, GetUsedIapResp is the callback function.
         *
         * @param aServiceId
         */        
        IMPORT_C void GetUsedIapReqL(TUint32 aServiceId);

        /**
         * Gets IAP ID which is used when connecting to aServiceId.
         * Warning, this might block a few seconds since it involves WLAN scan,
         * use GetUsedIapReqL if this is a problem.
         * Function is synchronous.
         *
         * @param aServiceId            Service ID in Services database.
         * @param aIapId                IAP ID in CommsDb.
         * @param aIapName              IAP name is written here, KIptvNsIapNameMaxLength is the max length.
         * @param aConnectionPermission This is valid only if aIapId is valid.
         * @param aRespStatus           If != ESucceeded, aIapId, aIapName and aConnectionPermission
         *                              do not contain valid data.
         */        
        IMPORT_C void GetUsedIapL( TUint32 aServiceId,
                                   TUint32& aIapId,
                                   TDes& aIapName,
                                   CIptvNetworkSelection::TConnectionPermission& aConnectionPermission,
                                   CIptvNetworkSelection::TRespStatus& aRespStatus );

        /**
         * Gets connection permission status for aIapId.
         * Function is synchronous.
         *
         * @param aIapId                IAP ID in CommsDb.
         * @param aConnectionPermission Connection permission status is written here.
         * @param aRespStatus           Status of the operation.
         */
        IMPORT_C void IsConnectionAllowedL(TUint32 aIapId,
                                           CIptvNetworkSelection::TConnectionPermission& aConnectionPermission,
                                           CIptvNetworkSelection::TRespStatus& aRespStatus);
        /**
         * Sets connection permission status for the connection type used
         * by aIapId.
         * Function is synchronous.
         *
         * @param aConnectionAllowed If ETrue, then connection will be allowed, not allowed otherwise.
         * @param aIapId             IAP ID in CommsDb, used for getting the connection type: WLAN, GPRS/CSD.
         * @param aRespStatus        Status of the operation.
         */
        IMPORT_C void SetConnectionAllowedL(TBool aConnectionAllowed,
                                            TUint32 aIapId,
                                            CIptvNetworkSelection::TRespStatus& aRespStatus);
      
       /* Server Control API */
       
        /**
         * This function is not supported. It does not close the server.
         * ServerShutdownResp is the callback function.
         *
         * @return System-wide error code.
         */
        IMPORT_C TInt ServerShutdownReq();

        /**
         * This function is not supported. It does not close the server.
         *
         * @param aRespStatus Status of the operation.
         * @return            System-wide error code. If != KErrNone, aRespStatus
         *                    doesn't contain valid data.
         */
        IMPORT_C TInt ServerShutdown(MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);
		
        /**
         * Cancels the pending asynchronous request synchronously.
         * This method always succees. If there was asynchronous request pending, after
         * this call it is gone and a new asynchronous
         * request may be sent immedetially.
         */
        IMPORT_C void CancelRequest() ;
        
        /**
        * From MIptvEventListenerObserver. MIptvEvent is converted to CIptvSmEvent
        * and sent to iClient.HandleSmEvent(). 
        * @param aEvent Event.
        */        
        void HandleEvent(MIptvEvent& aEvent);

    protected: // Functions from base classes

        /**
         * From CActive, RunL.
         * Callback function.
         * Invoked to handle responses from the server.
         */
        void RunL();

        /**
         * From CActive, DoCancel.
         * Cancels any outstanding operation.
         */
        void DoCancel();
		 
    private:

        /**
        * Default C++ constructor.
        * @param aClient pointer to client, no ownership is transferred.
        */
        IMPORT_C CIptvServiceManagementClient(MIptvServiceManagementClientObserver& aClient);

        /**
        * Performs the second phase construction of a
        * CIptvRequestHandler object.
        */
        void ConstructL();

        /**
        * Handles EIptvEngineSmGetServicesSizeResp,\n
        * and EIptvEngineSmGetServicesUsingIdSizeResp messages.\n
        * @param aMsgId identifies which message completed.
        */
        void HandleGetServicesSizeRespL(TUint8 aMsgId);

        /**
        * Handles EIptvEngineSmGetServicesDataResp,\n
        * and EIptvEngineSmGetServicesUsingIdDataResp messages.\n
        * @param aMsgId identifies which of the three message completed.
        */
        void HandleGetServicesDataRespL(TUint8 aMsgId);
                
        /**
        * Gets resp status from ipc message buffer.
        */
        MIptvServiceManagementClientObserver::TRespStatus GetRespStatusL();

        /**
        * Builds IPC msg which is sent to server in service adding.
        */
        void BuildAddServiceReqL(CIptvService& aService);

        /**
        * Builds IPC msg which is sent to server in service updating.
        */
        void BuildUpdateServiceReqL(CIptvService& aService);

        /**
        * Builds GetServicesSizeReq IPC msg.
        */
        void BuildGetServicesSizeReqL(TUint32 aSearchLimitFlags,
                                      TOrder aOrder);
        
        /**
         * Internalizes GetServicesDataResp IPC message and builds CDesC8ArraySeg* of it.
         */
        CDesC8ArraySeg* InternalizeGetServicesDataRespL();

        /**
        * Builds DeleteServiceReq IPC msg.
        */
        void BuildDeleteServiceReqL(TUint32 aId);

        /**
        * Builds GetServicesUsingIndexSizeReq IPC msg.
        */
        void BuildGetServicesUsingIndexSizeReqL(TUint32 aStartId, TUint32 aEndId);

        /**
        * Builds GetUsedIapReq IPC msg.
        *
        * @param aServiceId Service id which is wished to connect.
        */        
        void BuildGetUsedIapReqL( TUint32 aServiceId );

        /**
        * Handles GetUsedIapResp from the server, fills all parameters.
        */
        void HandleGetUsedIapRespL( TUint32& aIapId,
                                    TDes& aIapName,
                                    CIptvNetworkSelection::TConnectionPermission& aConnectionPermission,
                                    TUint8& aWlanGprs,
                                    CIptvNetworkSelection::TRespStatus& aRespStatus );
        
    private: // Data

        /**
        * A client which is making the requests.
        */        
        MIptvServiceManagementClientObserver& iClient;

        /**
        * iMsg Heap object to restore memory for IPC messaging.
        */
        HBufC8* iMsg;
        
        /**
        * Passed to RIptvClientSession object to store the sent and received data.
        * Points to iMsg heap object.
        */
        TPtr8                      iMsgPtr;
                
        /**
        * Used to store information or which entity has the message pending.
        * This information is needed when doing cancel.
        */
        TRequestPendingFor         iRequestPendingFor;

        /**
        * Used to store the resp op-code, so we know in RunL which request
        * completed.
        */
        TInt                       iResponseMsgId;
        
        /**
        * Listens events from the server side (CIptvEventGenerator sends them).
        */
        CIptvEventListener*        iEventListener;
				        
    };


#endif //__CIPTVSERVICEMANAGEMENTCLIENT_H__

// End of File
