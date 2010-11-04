/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  mpxvideoplayerappuiengine test class for STIF testing
*
*/

// Version : %version: ou1cpsw#10.1.1 %

#ifndef MPXVIDEOPLAYER_TEST_H
#define MPXVIDEOPLAYER_TEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <mpxplaybackcommanddefs.h>
#include <remconcoreapi.h>
#include <mpxplaybackframeworkdefs.h>

// CONSTANTS


// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( Kmpxvideoplayer_testLogPath, "\\logs\\testframework\\mpxvideoplayer_test\\" );

// Log file
_LIT( Kmpxvideoplayer_testLogFile, "mpxvideoplayer_test.txt" );
_LIT( Kmpxvideoplayer_testLogFileWithTitle, "mpxvideoplayer_test_[%S].txt" );

// FUNCTION PROTOTYPES


// FORWARD DECLARATIONS
class Cmpxvideoplayer_test;
class CMpxVideoPlayer_TestPlaybackUtility;
class CMpxVideoPlayerRemConListener;
class CMpxVideoPlayerMediaKeyHandler;
class CMpxVideoPlayer_AppUiEngineTester;

// DATA TYPES

// CLASS DECLARATION

/**
*  Cmpxvideoplayer_test test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cmpxvideoplayer_test) : public CScriptBase
{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cmpxvideoplayer_test* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cmpxvideoplayer_test();

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
        Cmpxvideoplayer_test( CTestModuleIf& aTestModuleIf );

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

        virtual TInt CreateEngineL( CStifItemParser& aItem );
        virtual TInt CreateGenericParametersTerminateL();
        virtual TInt OpenFileL( CStifItemParser& aItem );
        virtual TInt OpenMediaL( CStifItemParser& aItem );
        virtual TInt SetGenericParamAccessPointL( CStifItemParser& aItem );
        virtual TInt OpenFileEmbeddedPdlL( CStifItemParser& aItem );
        virtual TInt HandleMessageEmbeddedPdlL( CStifItemParser& aItem );
        virtual TInt StartStandAloneL( CStifItemParser& aItem );
        virtual TInt HandleCollectionMessageL( CStifItemParser& aItem );
        virtual TInt HandleCollectionMediaL( CStifItemParser& aItem );
        virtual TInt HandleSoftkeyBackL( CStifItemParser& aItem );
        virtual TInt HandleOpenPlaylistL( CStifItemParser& aItem );
        virtual TInt HandleOpenMediaL( CStifItemParser& aItem );
        virtual TInt HandleViewActivation( CStifItemParser& aItem );
        virtual TInt ProcessCommandParametersL( CStifItemParser& aItem );
        virtual TInt HandleMessageActivationMessageL( CStifItemParser& aItem );
        virtual TInt GetViewDepthL( CStifItemParser& aItem );
        virtual TInt ClearPdlInformation();
        virtual TInt ClearPlaybackUtilityL( CStifItemParser& aItem );
        virtual TInt HandleEmbeddedOpenL( CStifItemParser& aItem );
        virtual TInt ActivateLateConstructTimerL( CStifItemParser& aItem );        

        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();


    private:    // Friend classes

        CMpxVideoPlayer_AppUiEngineTester* iAppUiEngineTester;
};

#endif      // MPXVIDEOPLAYER_TEST_H

// End of File
