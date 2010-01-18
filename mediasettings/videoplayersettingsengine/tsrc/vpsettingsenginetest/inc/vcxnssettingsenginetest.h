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




#ifndef VCXNSSETTINGSENGINETEST_H
#define VCXNSSETTINGSENGINETEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <f32file.h>

#include "videoplayersettingsengine.h"
#include "IptvTestUtilALR.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KVcxNsSettingsEngineTestLogPath, "\\logs\\testframework\\VcxNsSettingsEngineTest\\" ); 
// Log file
_LIT( KVcxNsSettingsEngineTestLogFile, "VcxNsSettingsEngineTest.txt" ); 
_LIT( KVcxNsSettingsEngineTestLogFileWithTitle, "VcxNsSettingsEngineTest[%S].txt" );
// Log method usage
_LIT( KTestModuleName, "VcxNsSettingsEngineTest" );
_LIT( KLogLocation, "In %S" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CVcxNsSettingsEngineTest;
class CVcxNsSettingsEngine;
class CVideoSettingsDriveMonitor;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

enum TDriveType
    {
    EFixedDrive = 1,
    EMassStorageDrive,
    EPhoneMemoryDrive,
    EMemoryCardDrive
    };

// CLASS DECLARATION

/**
*  CVcxNsSettingsEngineTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CVcxNsSettingsEngineTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVcxNsSettingsEngineTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CVcxNsSettingsEngineTest();

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
        CVcxNsSettingsEngineTest( CTestModuleIf& aTestModuleIf );

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
        * Log current test method
        * @since ?Series60_version
        */
        void LogMethod( TPtrC aMethod );
        
        TInt FindDrive( TInt aType );

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
        virtual TInt GetUsedMemoryL( CStifItemParser& aItem );
        virtual TInt SetUsedMemoryL( CStifItemParser& aItem );
        virtual TInt GetParentControl( CStifItemParser& aItem );
        virtual TInt SetParentControl( CStifItemParser& aItem );
        virtual TInt GetVideoOnDemandThumbnails( CStifItemParser& aItem );
        virtual TInt SetVideoOnDemandThumbnails( CStifItemParser& aItem );
        virtual TInt GetUsedSnapId( CStifItemParser& aItem );
        virtual TInt SetUsedSnapId( CStifItemParser& aItem );
        virtual TInt GetAllowUseGprs( CStifItemParser& aItem );
        virtual TInt SetAllowUseGprs( CStifItemParser& aItem );
        virtual TInt GetAllowRoaming( CStifItemParser& aItem );
        virtual TInt SetAllowRoaming( CStifItemParser& aItem );
        virtual TInt CheckLockCodeL( CStifItemParser& aItem );
        virtual TInt GetPlayerVolume( CStifItemParser& aItem );
        virtual TInt SetPlayerVolume( CStifItemParser& aItem );

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
        Int64 iIadUpdateTime;
        TInt iUsedMemory;
        CVcxNsSettingsEngine* iSettingsEngine;
        CVideoSettingsDriveMonitor* iDriveMonitor;
        RFs iFs;
    };

#endif      // VCXNSSETTINGSENGINETEST_H

// End of File
