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


#ifndef VIDEOPLAYERSTARTUPAPITEST_H
#define VIDEOPLAYERSTARTUPAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#include "videoplayeractivationmessage.h"
#include "videoplayercustommessage.h"

#include "VCXTestMessageWait.h"
#include "VCXTestMyVideosObserver.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KVideoPlayerStartupApiTestLogPath, "\\logs\\testframework\\VideoPlayerStartupApiTest\\" ); 
// Log file
_LIT( KVideoPlayerStartupApiTestLogFile, "VideoPlayerStartupApiTest.txt" ); 
_LIT( KVideoPlayerStartupApiTestLogFileWithTitle, "VideoPlayerStartupApiTest_[%S].txt" );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CVideoPlayerStartupApiTest;
class CVCXTestMyVideosCollectionClient;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

class CTestProcess 
    {
    public:
        static CTestProcess* NewL() { CTestProcess* p = new (ELeave) CTestProcess(); return p; };

        CTestProcess() {};
        virtual ~CTestProcess() { iProcess.Close(); delete iName; };
        
    public: // Data
        RProcess iProcess;
        HBufC* iName;
    };

/**
*  CVideoPlayerStartupApiTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CVideoPlayerStartupApiTest) : public CScriptBase,
                                                public MVCXTestMyVideosObserver,
                                                public MVCXTestMessageWaitObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVideoPlayerStartupApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CVideoPlayerStartupApiTest();

    private: // New functions

        /**
         * Sends activation message to video player. 
         */
        void SendActivationMessageL( TVideoPlayerActivationMessage& aMessage );
        
        /**
         * Gets list of process names.
         */        
        void GetAllProcessNames( RPointerArray<HBufC>& aArray );
        
        /**
         * Gets processes for processes that do not exist in aExistingProcesses.
         */
        void GetHandlesForNewProcessesL( RPointerArray<HBufC>& aExistingProcesses, RPointerArray<CTestProcess>& aHandles );
    
        /**
         * Checks the exit status of process handles in aHandles array.
         */
        TInt CheckProcessExits( RPointerArray<CTestProcess>& aHandles );
        
    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

        void VideoEventL( TVCXTestVideoEvent aEvent, TUint32 aParam1, TUint32 aParam2, TInt aError );        
        
        void MessageWaitComplete( TInt aError );
        
    private:

        /**
        * C++ default constructor.
        */
        CVideoPlayerStartupApiTest( CTestModuleIf& aTestModuleIf );

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
       
        virtual TInt ActivationMessageL( CStifItemParser& aItem );
        
        virtual TInt KillFusionL( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    private:    // Data
        
        RPointerArray<CTestProcess> iProcessHandles;
        
        CVCXTestMyVideosCollectionClient* iMyVideosCollectionClient;
        CVCXTestMessageWait* iMessageWait;
        TInt iCollectionError;
    };

#endif      // VIDEOPLAYERSTARTUPAPITEST_H

// End of File
