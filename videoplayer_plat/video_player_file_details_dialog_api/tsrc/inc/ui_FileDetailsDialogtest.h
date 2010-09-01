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
* Description:   STIF for filedetailsdialog*
*/


// Version : %version: 8 %

#ifndef UI_FILEDETAILSDIALOGTEST_H
#define UI_FILEDETAILSDIALOGTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <MPFileDetails.h>
#include <MPFileDetailsDialog.h>
#include "VcxTestControlEventQueue.h"
#include <HWRMLight.h>
#include "VcxTestTimer.h"

// CONSTANTS

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path

#ifdef __WINSCW__
// Logging path
_LIT( Kui_FileDetailsDialogtestLogPath, "C:\\logs\\testframework\\ui_FileDetailsDialogtest\\" ); 
// Log file
_LIT( Kui_FileDetailsDialogtestLogFile, "ui_FileDetailsDialogtest.txt" ); 
_LIT( Kui_FileDetailsDialogtestLogFileWithTitle, "ui_FileDetailsDialogtest_[%S].txt" );
_LIT( Kui_FileDetailsDialogtest_testPath, "C:\\testing\\data\\" );
_LIT( Kui_FileDetailsDialogtest_videoTestPath, "C:\\testing\\data\\video_18kb.3gp" );
_LIT( Kui_FileDetailsDialogtest_videoPath, "C:\\data\\Videos\\" );
#else
_LIT( Kui_FileDetailsDialogtestLogPath, "C:\\logs\\testframework\\ui_FileDetailsDialogtest\\" ); 
// Log file
_LIT( Kui_FileDetailsDialogtestLogFile, "ui_FileDetailsDialogtest.txt" ); 
_LIT( Kui_FileDetailsDialogtestLogFileWithTitle, "ui_FileDetailsDialogtest_[%S].txt" );
_LIT( Kui_FileDetailsDialogtest_testPath, "E:\\testing\\data\\" );
_LIT( Kui_FileDetailsDialogtest_videoTestPath, "E:\\testing\\data\\video_18kb.3gp" );
_LIT( Kui_FileDetailsDialogtest_videoPath, "E:\\data\\Videos\\" );
#endif

// FORWARD DECLARATIONS
class Cui_FileDetailsDialogtest;
class CMPFileDetailsDialog;
class CMPFileDetails;
class CCoeControl;

// DATA TYPES
// Used to map the keys in e32keys.h to values used in STIF script. 
enum TFileDetailsTestKeyCodeMap
    {
    TFileDetailsTestKeyCode_EKeyDevice0,
    TFileDetailsTestKeyCode_EKeyDevice1
    };

// CLASS DECLARATION

/**
*  Cui_FileDetailsDialogtest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cui_FileDetailsDialogtest) : public CScriptBase,
                                               public MHWRMLightObserver,
                                               public MVcxTestTimerObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cui_FileDetailsDialogtest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cui_FileDetailsDialogtest();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
        virtual void LightStatusChanged(TInt aTarget, 
                                        CHWRMLight::TLightStatus aStatus);        
        
        /**
         * From MVcxTestTimerObserver.
         */
        void TimerComplete( TInt aTimerId, TInt aError );
        
    private:

        /**
        * C++ default constructor.
        */
        Cui_FileDetailsDialogtest( CTestModuleIf& aTestModuleIf );

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

        //Test methods for the header "MPFileDetailsDialog.h"
        TInt CreateNewDialog( CStifItemParser& aItem );
        TInt ExecuteLDFromMedia( CStifItemParser& aItem );
        TInt ExecuteLDFromDetails( CStifItemParser& aItem );
        
        //Test methods for the header "MPFileDetails.h"
        TInt CreateNewFileDetails( CStifItemParser& aItem );
        
        // Test Methods for pointer events
        
        /**
         * Adds pointer event to queue.
         * @param delay milliseconds before next event is run. 
         * @param TPointerEvent::TType
         * @param X position
         * @param Y position
         */
        virtual TInt AddPointerEventL( CStifItemParser& aItem );
        
        /**
         * Adds key event to queue.
         * @param delay milliseconds before next event is run. 
         * @param TKeyCode
         * @param TEventCode
         */
        virtual TInt AddKeyEventL( CStifItemParser& aItem );        
        
        /**
         * Enables the running of pointer events.
         * @param aDelay milliseconds before events start.
         */
        virtual TInt EnableControlEvents( CStifItemParser& aItem );
        
        /**
         * Disables the running of pointer events.
         */        
        virtual TInt DisableControlEvents( CStifItemParser& aItem );
        
        /**
         * Resets pointer events.
         */
        virtual TInt ResetControlEvents( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    private: // New methods
        
    private:    // Data
        
    	CMPFileDetailsDialog* iMPFileDetailsDialog;
    	CMPFileDetails* iMPFileDetails;
    	CCoeControl* iEdwinTestControl;
    	CVcxTestControlEventQueue* iControlEventQueue;
    	TBool iControlEventsEnabled;
    	
    	CHWRMLight* iHwLight;
    	CHWRMLight::TLightTarget iTarget;
    	
    	CVcxTestTimer* iTimer;
    	TInt iError;
    };

#endif      // UI_FILEDETAILSDIALOGTEST_H

// End of File
