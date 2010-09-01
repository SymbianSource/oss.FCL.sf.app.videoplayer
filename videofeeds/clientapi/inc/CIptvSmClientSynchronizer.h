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
* Description:    Header file for CIptvSmClientSynchronizer class*
*/





#ifndef __CIPTVSMCLIENTSYNCHRONIZER_H__
#define __CIPTVSMCLIENTSYNCHRONIZER_H__

// INCLUDE FILES
#include "CIptvServiceManagementClient.h"

// FORWARD DECLARATIONS

class CIptvServiceManagementClient;
class CIptvSmEvent;

// CLASS DECLARATION
/**
 * This class implements synchronized versions of CIptvServiceManagementClient async
 * API functions. This class is not intended for client usage, use
 * CIptvServiceManagementClient instead.
 */
class CIptvSmClientSynchronizer : public CBase, public MIptvServiceManagementClientObserver
    {
    public:
                    		    		       
    public: // Constructors and destructors
       /**
        * NewL.
        * Two-phased constructor.
        * Creates a CIptvSmClientSynchronizer object using two phase construction,
        * and return a pointer to the created object.
        * @param aClient reference to client.
        * @return A pointer to the created instance of CIptvSMClientSynchronizer.
        */
        static CIptvSmClientSynchronizer* NewL(CIptvServiceManagementClient& aServiceManagementClient);
		
        /**
        * ~CIptvSmClientSynchronizer.
        * Destructor.
        * Destroys the object and releases all memory objects.
        */
        virtual ~CIptvSmClientSynchronizer();

    public: // New functions

        /**
         * See corresponding functions from CIptvServiceManagementClient.h.
         */
        TInt AddServiceL(CIptvService& aService,
		                     MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);

        TInt UpdateServiceL(CIptvService& aService,
                            MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);

        TInt GetServicesL(TUint32 aSearchLimitFlags,
                          CIptvServiceManagementClient::TOrder aOrder,
                          CDesC8ArraySeg*& aServicesArray,
                          MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);

        TInt GetServicesL(TUint32 aStartId,
                          TUint32 aEndId,
                          CDesC8ArraySeg*& aServicesArray,
                          MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);

        TInt DeleteServiceL(TUint32 aId,
                            MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);

        TInt SetAllIapsL(CIptvIapList& aIapList,
                         TBool aIgnoreReadOnlyFlag,
                         MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);
        
        /* Network selection API */
        /* See corresponding function headers from CIptvServiceManagementClient.h */

        void GetUsedIapL(TUint32 aServiceId,
                         TUint32& aIap,
                         TDes& aIapName,
                         CIptvNetworkSelection::TConnectionPermission& aConnectionPermission,
                         CIptvNetworkSelection::TRespStatus& aRespStatus);
             
        TInt ServerShutdown(MIptvServiceManagementClientObserver::TRespStatus& aRespStatus);
		    
		    /**
         * Callback functions, see MIptvServiceManagementClientObserver.h.
         */
        void AddServiceResp(MIptvServiceManagementClientObserver::TRespStatus aRespStatus);
        void UpdateServiceResp(MIptvServiceManagementClientObserver::TRespStatus aRespStatus);
        void DeleteServiceResp(MIptvServiceManagementClientObserver::TRespStatus aRespStatus);
        void GetServicesResp(MIptvServiceManagementClientObserver::TRespStatus aRespStatus,
                             CDesC8ArraySeg* aServicesArray);
        void GetUsedIapResp(TUint32 aIapId,
                            const TDesC& aIapName,
                            CIptvNetworkSelection::TConnectionPermission aConnPerm,
                            TBool   aWlanWhenGPRS,
                            CIptvNetworkSelection::TRespStatus aRespStatus
                            );
        void ServerShutdownResp(MIptvServiceManagementClientObserver::TRespStatus aRespStatus);        
        void HandleSmEvent(CIptvSmEvent& aEvent);
        
    protected: // Functions from base classes
		 
    private:

        /**
         * CIptvSmClientSynchronizer.
         * Default C++ constructor.
         */
        CIptvSmClientSynchronizer(CIptvServiceManagementClient& aServiceManagementClient);

        /**
         * ConstructL.
         * Performs the second phase construction of a
         * CIptvRequestHandler object.
         */
        void ConstructL();
        
    private: // Data
    
        CIptvServiceManagementClient&                     iServiceManagementClient;
        MIptvServiceManagementClientObserver::TRespStatus iRespStatus;
        CIptvNetworkSelection::TRespStatus                iNsRespStatus;
        CDesC8ArraySeg*                                   iServicesArray;
        TUint32                                           iIapId;
        TBuf<KIptvNsIapNameMaxLength>                     iIapName;
        CIptvNetworkSelection::TConnectionPermission      iConnectionPermission;
        CActiveSchedulerWait*                             iActiveSchedulerWait;
    };


#endif //__CIPTVSMCLIENTSYNCHRONIZER_H__

// End of File
