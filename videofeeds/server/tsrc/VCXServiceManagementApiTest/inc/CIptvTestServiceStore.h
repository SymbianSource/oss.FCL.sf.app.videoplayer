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




#ifndef CIPTVTESTSERVICESTORE_H
#define CIPTVTESTSERVICESTORE_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "CIptvServiceManagementClient.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvTestServiceStore;
class CIptvService;
class CIptvTestUtilities;

// CLASS DECLARATION

/**
*
*
*
*
*  @since
*/
class CIptvTestServiceStore : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CIptvTestServiceStore* NewL( CIptvTestServiceStore* aStore = NULL, CIptvTestUtilities* aTestUtil = NULL);

        /**
        * Two-phased constructor.
        */
        static CIptvTestServiceStore* NewL( CIptvServices* aServices );

        /**
        * C++ default constructor.
        */
        CIptvTestServiceStore( );

        /**
        * Destructor.
        */
        virtual ~CIptvTestServiceStore();

    public: // New functions

        /**
        * RefreshServicesFromDbL
        * Fetches services from service DB.
        * @param aFlagsFilter
        * @param aOrder
        * @return KErrNotReady if class was not instantiated with pointer of CIptvTestUtilities
        */
        void RefreshServicesFromDbL(TUint32 aFlagsFilter, CIptvServiceManagementClient::TOrder aOrder);

        /**
        * AddServiceL
        * @param aService
        */
        void AddServiceL(CIptvService& aService);

        /**
        * AddServicesL
        * @param aServiceArray
        * Ownership of the services does not move.
        */
        void AddServicesL(RPointerArray<CIptvService>& aServiceArray);

        /**
        * ReplaceServiceL
        * @param aServiceName Service which will be replaced
        * @param aService Service which will be used to replace
        */
        void ReplaceServiceL(TInt aId, CIptvService& aService);

        /**
        * DeleteService
        * param aServiceName
        * @return Symbian error code.
        */
        TInt DeleteService(TInt aId);

        /**
        * Compare
        * Compares services in this and aStore.
        * @param aStore
        * @return Symbian error code. KErrNone if all services match.
        */
        TInt Compare(CIptvTestServiceStore& aStore);

        /**
        * GetService
        * Ownership of the service does not move.
        * @param aIndex
        * @return Instance to the service, NULL if not found.
        */
        CIptvService* GetService(TInt aIndex);

        /**
        * GetServiceById
        * Ownership of the service does not move.
        * @param aId
        * @return Instance to the service, NULL if not found.
        */
        CIptvService* GetServiceById(TInt aId);

        /**
        * GetServices
        * Ownership of the services does not move.
        * @param aServicesArray
        * @return Symbian error code.
        */
        TInt GetServices(RPointerArray<CIptvService>& aServicesArray);

        /**
        * GetServicesByProviderId
        * Ownership of the services does not move.
        * @param aServicesArray
        * @param aProviderId
        * @return Symbian error code.
        */
        TInt GetServicesByProviderId(RPointerArray<CIptvService>& aServicesArray, TDes& aProviderId);

        /**
        * GetGroupedServicesByGroupId
        * Ownership of the services does not move.
        * @param aServicesArray
        * @param aGroupId
        * @return Symbian error code.
        */        
        TInt GetGroupedServicesByGroupId(RPointerArray<CIptvService>& aServicesArray, TInt aGroupId);
        
        /**
        * ServiceCount
        * @return Count of services.
        */
        TInt ServiceCount();

        /**
        * CompareServices
        * @aS1
        * @aS2
        * @return ETrue if services match, otherwise EFalse
        */
        TBool CompareServices(CIptvService& aS1, CIptvService& aS2);

        /**
        * PrintServices
        * @return Symbian error code.
        */
        TInt PrintServices();

        /**
        * DeleteAllServices
        */
        void DeleteAllServices();

        /**
        * CloneServiceL
        * @param aService
        * @return Copy of the service.
        */
        CIptvService* CloneServiceL(CIptvService& aService);

    protected: //from base classes

    private:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CIptvTestServiceStore* aStore, CIptvTestUtilities* aTestUtil, CIptvServices* aServices );

        /**
        * FindService
        * Searches the array for a service. Array is enumerated in reveresed order.
        * @param aServices Array to look for the service.
        * @param aId 
        * @return Index in the array if service was found, otherwise Symbian error code.
        */
        TInt FindService(RPointerArray<CIptvService>& aServices, TInt aId);
        
        /**
        * FindServiceByGroupId
        * Searches the array for a service. Array is enumerated in reveresed order.
        * @param aServices Array to look for the service.
        * @param aGroupId 
        * @return Index in the array if service was found, otherwise Symbian error code.
        */
        TInt FindServiceByGroupId(RPointerArray<CIptvService>& aServices, TInt aGroupId);

    private:    // Data
        // Array of services
        RPointerArray<CIptvService> iServices;

        // Instance of CIptvTestUtilities. Not owned by the class.
        CIptvTestUtilities* iIptvTestUtilities;
    };


#endif      // CIPTVTESTSERVICESTORE_H

// End of File
