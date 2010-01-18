/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   STIF testclass declaration*
*/



#ifndef MPXMEDIASETTINGSENGINETEST_H
#define MPXMEDIASETTINGSENGINETEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include "MPSettingsModelForROP.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KMPXMediaSettingsEngineTestLogPath, "\\logs\\testframework\\MPXMediaSettingsEngineTest\\" ); 
// Log file
_LIT( KMPXMediaSettingsEngineTestLogFile, "MPXMediaSettingsEngineTest.txt" ); 
_LIT( KMPXMediaSettingsEngineTestLogFileWithTitle, "MPXMediaSettingsEngineTest_[%S].txt" );

_LIT( KTestModuleName, "MPXMediaSettingsEngineTest" );
_LIT( KLogLocation, "In %S" );

const TUint32 KDefaultIapId = 666;

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CMPXMediaSettingsEngineTest;
class CMPSettingsModelForROP;
class CCommsDatabase;
class CVCXTestCommon;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CMediaSettingsEngineTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMPXMediaSettingsEngineTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPXMediaSettingsEngineTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMPXMediaSettingsEngineTest();

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

    private:

        /**
        * C++ default constructor.
        */
        CMPXMediaSettingsEngineTest( CTestModuleIf& aTestModuleIf );

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
        * Logs the method name
        * @since ?Series60_version
        */
        void LogMethod( TPtrC aMethod );

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CreateL( CStifItemParser& aItem );
        virtual TInt ListImplementationsL( CStifItemParser& aItem );
        virtual TInt LoadSettingsLL( CStifItemParser& aItem );
        virtual TInt StoreSettingsLL( CStifItemParser& aItem );
        virtual TInt SetVideoContrastL( CStifItemParser& aItem );
        virtual TInt GetVideoContrastL( CStifItemParser& aItem );
        virtual TInt SetProxyModeL( CStifItemParser& aItem );
        virtual TInt GetProxyModeL( CStifItemParser& aItem );
        virtual TInt SetProxyHostNameLL( CStifItemParser& aItem );
        virtual TInt GetProxyHostNameL( CStifItemParser& aItem );
        virtual TInt SetProxyPortL( CStifItemParser& aItem );
        virtual TInt GetProxyPortL( CStifItemParser& aItem );
        virtual TInt SetDefaultApL( CStifItemParser& aItem );
        virtual TInt GetDefaultApL( CStifItemParser& aItem );
        virtual TInt SetBandwidthControlModeL( CStifItemParser& aItem );
        virtual TInt GetBandwidthControlModeL( CStifItemParser& aItem );
        virtual TInt SetMaxBandwidthL( CStifItemParser& aItem );
        virtual TInt GetMaxBandwidthL( CStifItemParser& aItem );
        virtual TInt SetConnectionTimeoutL( CStifItemParser& aItem );
        virtual TInt GetConnectionTimeoutL( CStifItemParser& aItem );
        virtual TInt SetServerTimeoutL( CStifItemParser& aItem );
        virtual TInt GetServerTimeoutL( CStifItemParser& aItem );
        virtual TInt SetMinUDPPortL( CStifItemParser& aItem );
        virtual TInt GetMinUDPPortL( CStifItemParser& aItem );
        virtual TInt SetMaxUDPPortL( CStifItemParser& aItem );
        virtual TInt GetMaxUDPPortL( CStifItemParser& aItem );
        virtual TInt GetControllerVersionInfoL( CStifItemParser& aItem );
        virtual TInt GetControllerBuildDateL( CStifItemParser& aItem );
        virtual TInt GetControllerAdditionalInfoL( CStifItemParser& aItem );
        virtual TInt SetDemandBwFactorL( CStifItemParser& aItem );
        virtual TInt GetDemandBwFactorL( CStifItemParser& aItem );
        virtual TInt SetSustainBandwidthL( CStifItemParser& aItem );
        virtual TInt GetSustainBandwidthL( CStifItemParser& aItem );
        virtual TInt GetSustainBwPresetsLL( CStifItemParser& aItem );
        virtual TInt GetMaxBwPresetsLL( CStifItemParser& aItem );
        virtual TInt SetAutoDisconTimeLL( CStifItemParser& aItem );
        virtual TInt AutoDisconTimeLL( CStifItemParser& aItem );
        virtual TInt SetVideoRepeatLL( CStifItemParser& aItem );
        virtual TInt IsVideoRepeatOnLL( CStifItemParser& aItem );
        virtual TInt SetDefaultViewLL( CStifItemParser& aItem );
        virtual TInt IsDefaultViewOnLL( CStifItemParser& aItem );
        virtual TInt SetRockerKeysLL( CStifItemParser& aItem );
        virtual TInt ShowRockerKeysLL( CStifItemParser& aItem );
        virtual TInt IsRockerKeysSupportedLL( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

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

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

        CMPSettingsModel* iModel;
        CMPSettingsModelForROP* iSettingsModel;
        CApUtils* iApUtils;
        CCommsDatabase* iCommDb;
        CVCXTestCommon* iVCXTestCommon;
    };

#endif      // MEDIASETTINGSENGINETEST_H

// End of File
