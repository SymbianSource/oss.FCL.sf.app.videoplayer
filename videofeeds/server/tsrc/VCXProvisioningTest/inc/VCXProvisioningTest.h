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




#ifndef IPTVPROVISIONINGTEST_H
#define IPTVPROVISIONINGTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include "CIptvService.h"
#include "MIptvProvisioningObserver.h"
#include "MIptvServiceManagementClientObserver.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KIptvProvisioningTestLogPath, "\\logs\\testframework\\IptvProvisioningTest\\" );
// Log file
_LIT( KIptvProvisioningTestLogFile, "IptvProvisioningTest.txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CIptvProvisioningTest;
class CIptvProvisioningProcessor;
class CIptvServiceManagementClient;
class CIptvTestUtilities;
// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CIptvProvisioningTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CIptvProvisioningTest) : public CScriptBase, public MIptvServiceManagementClientObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CIptvProvisioningTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CIptvProvisioningTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );



    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();
        virtual void AddServiceResp(TRespStatus /* aRespStatus */) {};
        virtual void UpdateServiceResp(TRespStatus /* aRespStatus */) {};
        virtual void DeleteServiceResp(TRespStatus /* aRespStatus */) {};
        virtual void GetServicesResp(TRespStatus /* aRespStatus */,
                                     CDesC8ArraySeg* /* aServicesArray */ ) {};
        virtual void GetUsedIapResp(TUint32 /* aIapId */,
                                    const TDesC& /* aIapName */,
                                    CIptvNetworkSelection::TConnectionPermission /* aConnectionPermission */,
                                    TBool   /* aWlanWhenGPRS */,
                                    CIptvNetworkSelection::TRespStatus /* aRespStatus */
                                    ) {};
        virtual void ServerShutdownResp(TRespStatus /* aRespStatus */ ) {};

        virtual void HandleSmEvent(CIptvSmEvent& /* aEvent */ ) {};

    private:

        /**
        * C++ default constructor.
        */
        CIptvProvisioningTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below.
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt Create( CStifItemParser& aItem );
        virtual TInt Destroy( CStifItemParser& aItem );
        virtual TInt ExternalProvision( CStifItemParser& aItem );
        virtual TInt LocalProvision( CStifItemParser& aItem );
        virtual TInt EnableVerifying( CStifItemParser& aItem );
        virtual TInt Verify( CStifItemParser& aItem );

        virtual TInt CheckIptvXmlSchema(  TDesC& aFileName );
        void DeAllocateResources();
        TInt AddService(CIptvService& aService);

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data

        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

        CIptvProvisioningProcessor* iIptvProvisioningProcessor;
        CIptvTestUtilities* iIptvTestUtilities;
        CIptvServiceManagementClient* iIptvServiceManagementClient;

        TBuf<32> iCaseId;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // IPTVPROVISIONINGTEST_H

// End of File
