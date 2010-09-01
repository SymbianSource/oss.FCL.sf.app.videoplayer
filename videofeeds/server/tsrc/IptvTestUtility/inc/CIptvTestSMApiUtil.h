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




#ifndef CIPTVTESTSMAPIUTIL_H
#define CIPTVTESTSMAPIUTIL_H

//  INCLUDES
#include "CIptvIapList.h"
#include <D32DBMS.H>
#include <S32FILE.H>

#include "CIptvService.h"
#include "CIptvServiceManagementClient.h"
#include "MIptvServiceManagementClientObserver.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvTestActiveWait;

// CLASS DECLARATION

/**
* CIptvTestSMApiUtil
*
*  @since
*/
class CIptvTestSMApiUtil : public CBase, public MIptvServiceManagementClientObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CIptvTestSMApiUtil* NewL();

        /**
        * Destructor.
        */
        virtual ~CIptvTestSMApiUtil();

    public: // New functions

        /**
        * GetUsedIap
        */
		TBool GetUsedIap(TUint32 aServiceId, TUint32& aIAPId,TDes& aIapName);

		/**
		 * Gets service from service cache which should be up to date.
		 */
        CIptvService* GetServiceFromDb(TInt aId);

    	/**
        * Gets services from SM client.
        * @param aServicesArray array where services will be added
        * @param aFlagsFilter only services with these flags will be added, use 0 to skip flags test
        * @param aOrder CIptvServiceManagementClient::TOrder enumeration to sort the services array or -1 to use no sort
        */
       TBool GetServicesFromDbL(RPointerArray<CIptvService>& aServicesArray, TUint32 aFlagsFilter, CIptvServiceManagementClient::TOrder aOrder);

       /**
       * Gets services from service cache which should be up to date.
       * @param aServicesArray array where services will be added
       */
      TBool GetServicesFromDbL(RPointerArray<CIptvService>& aServicesArray);

       /**
        * SetUsedIap
        */
       TInt SetUsedIap(TUint32 aServiceId, TUint32 aIapId);

       /**
        * Creates a copy of the service with same name and deletes the original.
        */
       TInt RefreshService(TUint32 aServiceId, TTime aLastDownloadTime);

    public: // From base classes

        /**
         * AddServiceResp
         */
        void AddServiceResp(TRespStatus /*aRespStatus*/){};

        /**
         * UpdateServiceResp
         */
        void UpdateServiceResp(TRespStatus /*aRespStatus*/){};

        /**
         * DeleteServiceResp
         */
        void DeleteServiceResp(TRespStatus /*aRespStatus*/){};

        /**
         * GetServicesResp
         */
        void GetServicesResp(TRespStatus aRespStatus, CDesC8ArraySeg* aServicesArray);

        /**
         * ServerShutdownResp
         */
        void ServerShutdownResp(TRespStatus /*aRespStatus*/){};

        /**
         * GetUsedIapResp
         */
        void GetUsedIapResp(TUint32 /*aIapId*/,
                            const TDesC& /*aIapName*/,
                            CIptvNetworkSelection::TConnectionPermission /*aConnectionPermission*/,
                            TBool   /* aWlanWhenGPRS */,
                            CIptvNetworkSelection::TRespStatus /*aRespStatus*/){};

        /**
        * HandleSmEvent
        * Callback for Service Manager generated events.
        * @param aEvent
        */
        void HandleSmEvent(CIptvSmEvent& aEvent );

    private:

        /**
        * C++ default constructor.
        */
        CIptvTestSMApiUtil();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
         * Refreshes iServicesArray
         */
        TBool RefreshServicesL();

        /**
         * IsServiceIncludedInSearch
         */
        TBool IsServiceIncludedInSearch(TUint32 aSearchLimitFlags, CIptvService& aIptvService);

        /*
         * CopyServiceL
         */
        CIptvService* CopyServiceL( CIptvService& aService );

    private:    // Data

    	CIptvTestActiveWait* iActiveWait;

    	CIptvServiceManagementClient*  iIptvServiceManagementClient;

		//For use of iServicesFileStore / iPasswordsFileStore
        RFs              iFsSession;

        // Response status of ServiceManager is stored here
        TRespStatus iSMRespStatus;

        //
        RPointerArray<CIptvService> iServicesArray;

        //
        TBool iRefreshingServices;


        TBool iRefreshNeeded;
    };

#endif      // CIPTVTESTSMAPIUTIL_H

// End of File
