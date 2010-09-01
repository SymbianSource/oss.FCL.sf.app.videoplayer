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
* Description:  STIF testclass declaration*
*/


#ifndef VCXSCHEDULEDDOWNLOADTEST_H
#define VCXSCHEDULEDDOWNLOADTEST_H

//  INCLUDES
#include <f32file.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include "VCXTestStifScriptBaseExt.h"

// CONSTANTS

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KVCXScheduledDownloadTestLogPath, "\\logs\\testframework\\VCXScheduledDownloadTest\\" ); 
// Log file
_LIT( KVCXScheduledDownloadTestLogFile, "VCXScheduledDownloadTest.txt" ); 
_LIT( KVCXScheduledDownloadTestLogFileWithTitle, "VCXScheduledDownloadTest_[%S].txt" );

// FORWARD DECLARATIONS
class CVCXScheduledDownloadTest;
class CVCXScheduledDownloadTester;
class CVCXTestCommon;

// DATA TYPES

// CLASS DECLARATION

/**
*  CVCXScheduledDownloadTest test class for STIF Test Framework TestScripter.
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CVCXScheduledDownloadTest) : public CVCXTestStifScriptBaseExt
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVCXScheduledDownloadTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CVCXScheduledDownloadTest();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private:

        /**
        * C++ default constructor.
        */
        CVCXScheduledDownloadTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * CreateL test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CreateL( CStifItemParser& aItem );
        
        /**
        * RemoveAllMediasL test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RemoveAllMediasL( CStifItemParser& aItem );
        
        /**
        * RemoveAllSchedulesL test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RemoveAllSchedulesL( CStifItemParser& aItem );        
        
        /**
        * RemoveAllVideosL test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt RemoveVideoL( CStifItemParser& aItem );        
        
        /**
        * DownloadL test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt DownloadL( CStifItemParser& aItem );        
                
        /**
        * CancelDownloadL test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CancelDownloadL( CStifItemParser& aItem );        
                
        /**
        * WaitForMessagesL test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt WaitForMessagesL( CStifItemParser& aItem );
        
        /**
        * CoolDown test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CoolDown( CStifItemParser& aItem );

        /**
        * CheckVideoCount test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CheckVideoCount( CStifItemParser& aItem );
        
        /**
        * CheckMinimumVideoCount test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CheckMinimumVideoCount( CStifItemParser& aItem );

        /**
        * CheckMaximumVideoCount test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CheckMaximumVideoCount( CStifItemParser& aItem );
        
        /**
        * CheckError test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt CheckError( CStifItemParser& aItem );

        /**
        * SetPreferredMemoryL test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        TInt SetPreferredMemoryL( CStifItemParser& aItem );
        
        /**
         * Other methods.
         */
        
        TInt GetDriveParam( CStifItemParser& aItem, TInt& aDrive );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //[TestMethods] - Do not remove

    private:    // Data
        
        RFs iFs;
        
        CVCXScheduledDownloadTester* iTester;
        
        CVCXTestCommon* iTestCommon;
    };

#endif      // VCXSCHEDULEDDOWNLOADTEST_H

// End of File
