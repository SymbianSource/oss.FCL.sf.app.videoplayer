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




#ifndef IPTVSERVICEMANAGEMENTAPITEST_H
#define IPTVSERVICEMANAGEMENTAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include "CIptvServiceManagementSync.h"
#include <badesca.h>

#include "CIptvService.h"
#include "MIptvServiceManagementClientObserver.h"

// CONSTANTS
const TInt32 KNoFilter = 0;
const CIptvServiceManagementClient::TOrder KNoSort = CIptvServiceManagementClient::EDateAscending;
const TInt KLastServiceInDb = 60000;

_LIT(K2kLongUrl, "http://193.65.182.78/misc/2kcharslongpath/256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpat/256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpat/256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpat/256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpat/256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpat/256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpat/256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpat/256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslongpath256charslo/123456.xml");
_LIT(K256charsLongSearchUrl, "http://193.65.182.78/search.php?file=nseries.xml&q=#search#&256charslongsearchurl___charslongsearchurl___charslongsearchurl___charslongsearchurl___charslongsearchurl___charslongsearchurl___charslongsearchurl___charslongsearchurl___charslongsearchurl___1X");
_LIT(K256LongFile, "http://193.65.182.78/misc/256charslongfilenames/256charslongfilename256charslongfilename256charslongfilename256charslongfilename256charslongfilename256charslongfilename256charslongfilename256charslongfilename256charslongfilename256charslongfilename256charslongfilename256charslongfilename256charslon.xml");

// Logging path
_LIT( KIptvServiceManagementApiTestLogPath, "\\logs\\testframework\\IptvServiceManagementApiTest\\" );
// Log file
_LIT( KIptvServiceManagementApiTestLogFile, "IptvServiceManagementApiTest.txt" );

// MACROS

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CIptvServiceManagementApiTest;
class CIptvTestUtilities;
class CIptvServiceManagementSync;
class CIptvTestMobilecrashWatcher;
class CIptvServiceInfo;
class CIptvTestServiceStore;
class CIptvTestUtilALR;

// DATA TYPES
#define ESucceeded          MIptvServiceManagementClientObserver::ESucceeded
#define EServiceNotFound    MIptvServiceManagementClientObserver::EServiceNotFound
#define EGeneralError       MIptvServiceManagementClientObserver::EGeneralError

// CLASS DECLARATION

/**
*  CIptvServiceManagementApiTest test class for STIF Test Framework TestScripter.
*
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CIptvServiceManagementApiTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CIptvServiceManagementApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CIptvServiceManagementApiTest();

        /**
        * TServiceFieldId
        */
        enum TServiceFieldId
            {
            EId = 0, // TUint32
            EName,
            EAddress,
            EIconPath,
            EEpgPluginUid, // TUid
            EVodPluginUid, // TUid
            EIptvPluginUid, // TUid
            EServiceType, // TServiceType
            ESetIapList, // TInt count of iaps
            EUserName,
            EPassword,
            EDesc,
            EFlags,
            EApplicationUid, // TInt32
            EDisplayOrder, // TUint32
            EProviderId, // TDesC
            EAccountManagementUrl,
            EScheduleDlTime,
            EScheduleDlNetwork,
            EScheduleDlType,
            EScheduleLastDlTime, // time used is the current time
            ESearchUrl // TDesC
            };

        /**
        * TScheduleDlType
        */
        enum  TScheduleDlType
            {
            EIptvTestNoDownload          = 0,
            EIptvTestDownloadLatest      = 1,
            EIptvTestDownloadOldest      = 2,
            EIptvTestDownloadAddedAfterLastScheduledDownload = 3,
            EIptvTestDownloadAll         = 4
            };

    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CIptvServiceManagementApiTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        */
        void Delete();

        /**
        * Test methods are listed below.
        */

        virtual TInt CreateL( CStifItemParser& aItem );
        virtual TInt DestroyL( CStifItemParser& aItem );

        virtual TInt AddHardCodedServiceL( CStifItemParser& aItem );
        virtual TInt UpdateServiceFieldL( CStifItemParser& aItem );
        virtual TInt UpdateServiceL( CStifItemParser& aItem );
        virtual TInt DeleteServiceL( CStifItemParser& aItem );
        virtual TInt DeleteAllServicesL( CStifItemParser& aItem );
        virtual TInt GetServicesL( CStifItemParser& aItem );
        virtual TInt GetServicesByProviderIdL( CStifItemParser& aItem );
        virtual TInt GetAllSelectedServicesL( CStifItemParser& aItem );
        virtual TInt GetServicesFilteredL( CStifItemParser& aItem );
        virtual TInt GetServicesByTypeL( CStifItemParser& aItem );
        virtual TInt ShutdownServerL( CStifItemParser& aItem );
        virtual TInt SetUsedIap( CStifItemParser& aItem );
        virtual TInt GetUsedIap( CStifItemParser& aItem );
        virtual TInt ClearUsedIapL(CStifItemParser& aItem );
        virtual TInt SetAllIapsL( CStifItemParser& aItem );
        virtual TInt SetConnectionAllowedL( CStifItemParser& aItem );
        virtual TInt IsConnectionAllowedL( CStifItemParser& aItem );
        virtual TInt VerifyServerIsShutdownL( CStifItemParser& aItem );

        virtual TInt RepeatNextRequest( CStifItemParser& aItem );
        virtual TInt SetSpecialAction( CStifItemParser& aItem );

        /**
        * Dummy method for old test methods which do nothing but are still called from security tests (nobody updates 'em).
        */
        virtual TInt Dummy( CStifItemParser& /*aItem*/ );

        /**
        * Helper funtions
        */

        /**
        * Deallocates resources used by test module.
        */
        void DeAllocateResources();

        /**
        * Gets IAP id from CommDb.
        * @since
        * @param aIndex
        * @param aIapId
        * @return Symbian OS error code.
        */
        TInt GetCommDbIapIdByOrder(TInt aIndex, TUint32& aIapId);

        /**
        * These methods are in CIptvServiceManagementLogic.cpp
        */

        /**
        * TestUpdateService
        * @since
        * @param aConn
        * @param aIdDestination
        * @param aIdSource
        * @return Symbian OS error code.
        */
        TInt TestUpdateService(CIptvServiceManagementSync::TIptvSmTestConnection aConn, TInt aIdDestination, TInt aIdSource);

        /**
        * IsServiceInDb
        * @since
        * @param aId
        * @return ETrue if service is found from the database.
        */
        TBool IsServiceInDb(TInt aId);

        /**
        * GetHardcodedService
        * @since
        * @param aHardcodedServiceNameOrID
        * @param aIapName
        * @return Instance to service. Ownwership moves to caller.
        */
        CIptvService* GetHardcodedService(TDesC& aHardcodedServiceNameOrID, TDesC& aIapName);

        /**
        * GetHardcodedService
        * @since
        * @param aHardcodedServiceNameOrID
        * @param aIapsToAdd
        * @return Instance to service. Ownwership moves to caller.
        */
        CIptvService* GetHardcodedService(TDesC& aHardcodedServiceNameOrID, TInt aIapsToAdd);

        /**
        * Gets service id
        * @param aItem next parameter in aItem should have service ID or service name
        * @param aServiceId will contain the specified service's ID as a result
        * @param aServiceName will contain the specified service's name as a result
        * @return Symbian OS error code.
        */
        TInt GetServiceIdFromParam(CStifItemParser& aItem, TInt& aServiceId, TDes& aServiceName);

        /**
        * InitHardcodedServicesL
        * @since
        * @return Symbian OS error code.
        */
        TInt InitHardcodedServicesL();

    public:     // Data

    protected:  // Data

    private:    // Data
        // Instance of CIptvTestMobilecrashWatcher
        CIptvTestMobilecrashWatcher* iIptvTestMobilecrashWatcher;

        //Instance of the servicemanagement API
        CIptvServiceManagementSync*   iIptvServiceManagementSync;

        //Instance of testutility class
        CIptvTestUtilities* iIptvTestUtilities;

        // Array which holds the hardcoded test services.
        RPointerArray<CIptvServiceInfo> iHardcodedServices;

        // Array holding known IAP IDs from CommDb
        CArrayFix<TUint32> *iIapNumList;

        // Counter which is incremented when NEXT hardcoded service is asked from STIF script.
        TInt iHardCodedServiceCounter;

        CIptvTestServiceStore* iServiceStore;

        // Sets the iap to highest priority when used iap is set.
        CIptvTestUtilALR* iAlrUtil;
    };

#endif      // IPTVSERVICEMANAGEMENTAPITEST_H

// End of File
