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




#ifndef IPTVSERVICETEST_H
#define IPTVSERVICETEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KIptvServiceTestLogPath, "\\logs\\testframework\\IptvServiceTest\\" ); 
// Log file
_LIT( KIptvServiceTestLogFile, "IptvServiceTest.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CIptvServiceTest;
class CIptvService;
class CIptvServices;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CIptvServiceTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CIptvServiceTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CIptvServiceTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CIptvServiceTest();

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
        
        TInt CompareService( TPtr aErrPtr, CIptvService& aOriginalService, CIptvService& aNewService );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CIptvServiceTest( CTestModuleIf& aTestModuleIf );

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
        * Test methods for CIptvServices
        */

        virtual TInt CreateServicesL( CStifItemParser& aItem );
        virtual TInt DestroyServicesL( CStifItemParser& aItem );
		virtual TInt ServicesAddL( CStifItemParser& aItem );
		virtual TInt ServicesCountL( CStifItemParser& aItem );
		virtual TInt ServicesInternalizeL( CStifItemParser& aItem );
		virtual TInt ServicesExternalizeL( CStifItemParser& aItem );
		virtual TInt ServicesGetServiceL( CStifItemParser& aItem );
		virtual TInt ServicesGetServiceByRefL( CStifItemParser& aItem );		

        /**
        * Test methods for CIptvService
        */
        
        virtual TInt CreateServiceL( CStifItemParser& aItem );
        virtual TInt DestroyServiceL( CStifItemParser& aItem );
        virtual TInt SetIdL( CStifItemParser& aItem );
        virtual TInt SetNameL( CStifItemParser& aItem );
        virtual TInt SetAddressL( CStifItemParser& aItem );
        virtual TInt SetIconPathL( CStifItemParser& aItem );
        virtual TInt SetEpgPluginUidL( CStifItemParser& aItem );
        virtual TInt SetVodPluginUidL( CStifItemParser& aItem );
        virtual TInt SetIptvPluginUidL( CStifItemParser& aItem );
        virtual TInt SetTypeL( CStifItemParser& aItem );
        
        // params: number of iaps
        virtual TInt CreateIapListL( CStifItemParser& aItem );
        // params: expected count of iaps
        virtual TInt CheckIapListL( CStifItemParser& aItem );
        
        virtual TInt SetUserNameL( CStifItemParser& aItem );
        virtual TInt SetPasswordL( CStifItemParser& aItem );
        virtual TInt SetDescL( CStifItemParser& aItem );
        // param: flag1 ... flag5
        virtual TInt SetFlagsL( CStifItemParser& aItem );
        virtual TInt SetApplicationUidL( CStifItemParser& aItem );
        virtual TInt SetDisplayOrderL( CStifItemParser& aItem );
        virtual TInt SetProviderIdL( CStifItemParser& aItem );
        virtual TInt SetAccountManagementUrlL( CStifItemParser& aItem );
        
        virtual TInt SetL( CStifItemParser& aItem );
        //virtual TInt GetL( CStifItemParser& aItem );
        //virtual TInt CountSize( CStifItemParser& aItem );
        //virtual TInt CountExternalizeSize( CStifItemParser& aItem );               

        virtual TInt DoInternalizeL( CStifItemParser& aItem );
        virtual TInt DoExternalizeL( CStifItemParser& aItem );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        /** 
        * CIptvService 
        */        
        CIptvService* iService;
        HBufC8* iBuff; // for testing internalize and externalize 
        
        
        
        /** 
        * CIptvServices 
        */
        CIptvServices *iServices;
        HBufC8* iServicesBuff; // for testing internalize and externalize 
        RPointerArray<CIptvService> iServicesArray; // backup copies of the services
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // IPTVSERVICETEST_H

// End of File
