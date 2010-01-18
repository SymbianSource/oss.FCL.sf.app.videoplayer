/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


// wait until command is complete( no more expected messages )

#ifndef MPXFUSIONPLAYER_APITEST_H
#define MPXFUSIONPLAYER_APITEST_H

//  INCLUDES
#include "VCXTestLog.h"
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

#include <f32file.h>

#include "MIptvTestTimerObserver.h"
#include "VCXTestPSObserver.h"

// CONSTANTS
const TInt KTimeoutTimerId = 0;
const TUint KFusionTestCommandTimeout = 60*1000000;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KFusionMpxPlayer_ApiTestLogPath, "\\logs\\testframework\\FusionMpxPlayer_ApiTest\\" );
// Log file
_LIT( KFusionMpxPlayer_ApiTestLogFile, "FusionMpxPlayer_ApiTest.txt" );
_LIT( KFusionMpxPlayer_ApiTestLogFileWithTitle, "FusionMpxPlayer_ApiTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CFusionMpxPlayer_ApiTest;
class CIptvTestTimer;
class CIptvTestActiveWait;
class CVCXTestCommon;
class CVCXTestPSSubscriber;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
enum  TFusionPlayerTestSeekType
{
    EFusionPlayerTestSeekForward,
    EFusionPlayerTestSeekBackward,
    EFusionPlayerTestSeekStop,
} ;
// CLASS DECLARATION


/**
*  CFusionMpxPlayer_ApiTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CFusionMpxPlayer_ApiTest) : public CScriptBase,
                                              public MVCXTestPSObserver,
                                              public MIptvTestTimerObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CFusionMpxPlayer_ApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CFusionMpxPlayer_ApiTest();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

        /**
        * From MIptvTestTimerObserver Handles timer completion
        * @since
        * @param aTimerId
        * @param aError
        */
        void TimerComplete(TInt aTimerId, TInt aError);

        /**
         * From MVCXConnUtilTestPSObserver, called when P&S key changes.
         */
        void ValueChangedL( const TUid& aUid, const TUint32& aKey, const TInt&  aValue );

        /**
         * From MVCXConnUtilTestPSObserver, called when P&S key changes.
         */
        void ValueChangedL( const TUid& aUid, const TUint32& aKey, const TDesC& aValue );

    private:

        /**
        * C++ default constructor.
        */
        CFusionMpxPlayer_ApiTest( CTestModuleIf& aTestModuleIf );

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
         * Set 0 to stop wait for command completion message from tester exe. 
         */
        virtual TInt SetCommandCompletionWaitL( CStifItemParser& aItem );
        
        /**
         * Call this before CreateL. Will try to shutdown all possible tester exes.
         */
        virtual TInt PrepareCaseL( CStifItemParser& aItem );

        /**
         * Called at module deletion. Shutdowns tester exe.
         */
        virtual TInt ShutdownTesterL( CStifItemParser& aItem );

        /**
         * Creates the test module.
         */
        virtual TInt CreateL( CStifItemParser& aItem );

        /**
         * Starts playing a random local file using MMPXPlaybackUtility::InitL and activates
         * player view.
         * @param filename
         */
        virtual TInt OpenRandomFileL( CStifItemParser& aItem );        
        
        /**
         * Starts playing a local file using MMPXPlaybackUtility::InitL and activates
         * player view.
         * @param filename
         */
        virtual TInt OpenLocalFileL( CStifItemParser& aItem );

        /**
         * Starts playing stream.
         * @param URL
         * @param IAP
         */
        virtual TInt OpenUrlL( CStifItemParser& aItem );
        
        /**
         * These map to commands defined in mpxplaybackcommanddefs.h
         */

        virtual TInt PlayL( CStifItemParser& aItem );
        virtual TInt PlayWithFadeInL( CStifItemParser& aItem );
        virtual TInt PauseL( CStifItemParser& aItem );
        virtual TInt StopL( CStifItemParser& aItem );
        virtual TInt PlayPauseL( CStifItemParser& aItem );

        virtual TInt NextL( CStifItemParser& aItem );
        virtual TInt PreviousL( CStifItemParser& aItem );
        virtual TInt ReplayL( CStifItemParser& aItem );
        virtual TInt StartSeekForwardL( CStifItemParser& aItem );
        virtual TInt StartSeekBackwardL( CStifItemParser& aItem );
        virtual TInt StopSeekingL( CStifItemParser& aItem );
        virtual TInt DecreaseVolumeL( CStifItemParser& aItem );
        virtual TInt IncreaseVolumeL( CStifItemParser& aItem );
        virtual TInt MuteVolumeL( CStifItemParser& aItem );
        virtual TInt UnMuteVolumeL( CStifItemParser& aItem );
        virtual TInt SetVolumeL( CStifItemParser& aItem );

        virtual TInt CloseL( CStifItemParser& aItem );
        virtual TInt ApplyEffectL( CStifItemParser& aItem );
        virtual TInt DisableEffectL( CStifItemParser& aItem );
        virtual TInt PreservePositionL( CStifItemParser& aItem );
        virtual TInt PreserveStateL( CStifItemParser& aItem );
        virtual TInt CloseItemL( CStifItemParser& aItem );
        virtual TInt CancelInitL( CStifItemParser& aItem );
        virtual TInt ResetPreserveStateL( CStifItemParser& aItem );
        virtual TInt UnloadNonActivePluginL( CStifItemParser& aItem );
        virtual TInt ClearKeyBufferL( CStifItemParser& aItem );

        /**
         * These map to commands defined in mpxvideoplaybackdefs.h
         */

        virtual TInt VideoSeekForwardL( CStifItemParser& aItem );
        virtual TInt VideoSeekBackwardL( CStifItemParser& aItem );
        virtual TInt VideoEndSeekL( CStifItemParser& aItem );
        virtual TInt VideoNaturalAspectRatioL( CStifItemParser& aItem );
        virtual TInt VideoZoomAspectRatioL( CStifItemParser& aItem );
        virtual TInt VideoStretchAspectRatioL( CStifItemParser& aItem );
        virtual TInt VideoDefaultAspectRatioL( CStifItemParser& aItem );
        virtual TInt VideoHandleForegroundL( CStifItemParser& aItem );
        virtual TInt VideoHandleBackgroundL( CStifItemParser& aItem );

        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    private:
        void TesterExeAliveL();

        void SendExeCmdL( TInt aCmd );
        void SendExeCmdL( TInt aCmd, TInt aIntParam1 );
        void SendExeCmdL( TInt aCmd, TInt aIntParam1, TInt aIntParam2 );
        void SendExeCmdL( TInt aCmd, const TDesC& aDescParam1 );
        void SendExeCmdL( TInt aCmd, const TDesC& aDescParam1, TInt aIntParam1 );
        void WaitForCommandL();
        
    private:    // Data
        CVCXTestCommon* iTestCommon;

        TBool iShutdownCalled;

        CVCXTestPSSubscriber* iTesterGetAckResponseSubscriber;
        CVCXTestPSSubscriber* iTesterGetMessageListResponseSubscriber;

        // Testexe
        RProcess iProcess;

        // Timeout timer
        CIptvTestTimer* iTimeoutTimer;

        CIptvTestActiveWait* iWait;

        TInt64 iRandomSeed;
        
        RFs iFs;
        
        TBool iCommandComplete;
        
        TInt iError;
        
        TBool iCommandWait;
        
        TInt iPSKeyBase;
    };

#endif      // MPXFUSIONPLAYER_APITEST_H

// End of File
