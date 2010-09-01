/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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





#ifndef CIPTVSERVICEMSGHANDLER_H
#define CIPTVSERVICEMSGHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "CIptvServiceManager.h"
#include "MIptvServiceManagerObserver.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvServer;
class CIptvEventGenerator;
class CIptvSmEvent;

// CLASS DECLARATION

/**
* Handles IPC messages from the client process. Uses CIptvServiceManager.
* This class is instantiated when client uses service management methods from
* CIptvServiceManagementClient class. Owned by CIptvServerSession.
*/
class CIptvServiceMsgHandler : public CBase, public MIptvServiceManagerObserver
    {
                
    public:

        /**
        * Two-phased constructor.
        *
        * @param aServer IPTV Server, needed for accessing EPG and Service Manager.
        */
        static CIptvServiceMsgHandler* NewL(CIptvServer& aServer);
        
        /**
        * Destructor.
        */
        virtual ~CIptvServiceMsgHandler();
 
        /**
        * Handles messages from the client.
        *
        * @param aMessage A message to handle.
        */
        void HandleServiceManagerMsgL(const RMessage2& aMessage);

    private:
        
        /**
         * Handles EIptvEngineServerGetServicesSizeReq message.
         *
         * @param aMessage IPC message where the response is written.
         */
        void HandleGetServicesSizeReqL(const RMessage2& aMessage);

        /**
         * Called by HandleGetServicesSizeReqL().
         *
         * @param aMessage  IPC message where the response is written.
         */
        void GetServicesSizeReqL(const RMessage2& aMessage);

        /**
         * Handles EIptvEngineServerGetServicesDataReq message.
         *
         * @param aMessage IPC message where the response is written.
         */
        void HandleGetServicesDataReqL(const RMessage2& aMessage);

        /**
         * Called by HandleGetServicesDataReqL().
         *
         * @param aMessage IPC message where the response is written.
         */
        void GetServicesDataReqL(const RMessage2& aMessage);

        /**
         * Constructs and sends size response to GetServicesUsingIdSizeReq message.
         *
         * @param aMessage IPC message where the response is written.
         */
        void HandleGetServicesUsingIdSizeReqL(const RMessage2& aMessage);

        /**
         * Constructs and sends size response to GetServicesUsingIdDataReq message.
         *
         * @param aMessage IPC message where the response is written.
         */
        void HandleGetServicesUsingIdDataReqL(const RMessage2& aMessage);

        /**
         * Handles EIptvEngineSmAddServiceReq message.
         *
         * @param aMessage Message from client-server framework
         */         
        void HandleAddServiceReqL(const RMessage2& aMessage);

        /**
         * Handles EIptvEngineSmDeleteServiceReq message.
         *
         * @param aMessage Message from client-server framework
         */         
        void HandleDeleteServiceReqL(const RMessage2& aMessage);

        /**
         * Handles EIptvEngineSmUpdateServiceReq message.
         *
         * @param aMessage Message from client-server framework
         */         
        void HandleUpdateServiceReqL(const RMessage2& aMessage);

        /**
         * Handles EIptvEngineSmGetServiceSizeUsingProviderId message.
         *
         * @param aMessage Message from client-server framework
         */         
        void HandleGetServiceSizeUsingProviderIdL(const RMessage2& aMessage);

        /**
         * Handles EIptvEngineSmGetServiceDataUsingProviderId message.
         *
         * @param aMessage Message from client-server framework
         */         
        void HandleGetServiceDataUsingProviderIdL(const RMessage2& aMessage);

        /**
         * Creates HBufC* and copies data from client address space
         * into it.
         *
         * @param aMessage Message from client-server framework
         * @return         Pointer to data.
         */
        HBufC8* GetDataFromClientAddrSpaceLC(const RMessage2& aMessage);

        /**
         * Used for EIptvEngineSmGetServicesReq and EIptvEngineSmGetServicesUsingIdReq.
         * Builds size (= iServicesSizeRespIpcMsg) and data (= iServicesDataRespIpcMsg)
         * messages from iServicesArray.
         * iServicesSizeRespIpcMsg message contains iServicesDataRespIpcMsg length. 
         */
        void BuildServiceIpcMsgRespsL();

        /**
         * Updates last update time to Service db LastUpdate column.
         * @param aServiceId The service id.
         * @param aLastUpdate The last update time.
         */
        void SetEpgLastUpdateTimeL( TUint32 aServiceId, TTime& aLastUpdate );

        /**
         * Completes the request with status information.
         *
         * @param aMessage
         * @param aError       Error code from Service Manager
         * @param aGlobalError If operation leaved this contains the reason code.
         */
        void SendStatusRespL( const RMessage2& aMessage,
                              MIptvServiceManagementClientObserver::TRespStatus aError,
                              TInt aGlobalError );

        /**
         * Deletes service group with all belog feeeds.
         *
         * @param aGroupId a group id of service(s) to be deleted
         * @return         error code.
         */         
        TInt DeleteGroupL( const TUint32 aGroupId );

        /**
         * Handles EIptvEngineSmDeleteServiceReq message.
         *
         * @param aServiceId a service id of service to be deleted
         * @param aServiceType a type of service to be deleted
         * @return         error code.
         */         
        TInt DeleteServiceL( const TUint32 aServiceId,
                             const CIptvService::TServiceType aServiceType );
        
    public:
                   
        /**
        * From MIptvServiceManagerObserver
        */
        void HandleSmEvent(CIptvSmEvent& aEvent);

        /**
        * Actual implementation for HandleSmEvent.
        */
        void HandleSmEventL(CIptvSmEvent& aEvent);
        
    private:

        /**
         * C++ default constructor.
         */
        CIptvServiceMsgHandler(CIptvServer& aServer);

        /**
         * Symbian 2nd phase constructor.
         */
        void ConstructL();

    private:    // Data
		
        /**
        * Owner of this object
        */
        CIptvServer&       iServer;	

        /**
        * Used to store services data resp IPC msg
        */
        HBufC8*            iServicesDataRespIpcMsg;
        
        /**
        * Used to store services size resp IPC msg
        */
        HBufC8*            iServicesSizeRespIpcMsg;
                
        /**
        * Stores CIptvServices object
        */
        CIptvServices*     iServices;

        /**
        * Generates events, CIptvEventListener catches them in client process.
        */
    	  CIptvEventGenerator* iEventGenerator;

    };

#endif      // CIPTVSERVICEMSGHANDLER_H   
            
// End of File
