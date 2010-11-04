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
* Description:  video Playback User Input Handler test class for STIF testing
*
*/

// Version : %version: ou1cpsw#4 %

#ifndef USERINPUTHANDLER_TEST_H
#define USERINPUTHANDLER_TEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <e32base.h>
#include <e32def.h>
#include <StifItemParser.h>
#include <W32STD.H>

#include "mpxvideoplaybackcontrol.hrh"
#include "mpxcommonvideoplaybackview.hrh"


// CLASS DECLARATION
class CMPXVideoPlaybackUserInputHandler;
class CMPXVideoPlaybackContainer; 
class CMPXVideoPlaybackControl;

// DATA

// CONSTANTS


// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KUserinputhandlertest_LogPath, "\\logs\\testframework\\userinputhandlertest\\" );

// Log file
_LIT( KUserinputhandlertest_LogFile, "userinputhandlertest.txt" );
_LIT( KUserinputhandlertest_LogFileWithTitle, "userinputhandlertest_[%S].txt" );


/**
*  CUserinputhandlertest class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CUserinputhandlertest) : public CScriptBase
{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CUserinputhandlertest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CUserinputhandlertest();

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
        CUserinputhandlertest( CTestModuleIf& aTestModuleIf );

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
        * Create stubs
        */
        virtual TInt CreateStubsL( CStifItemParser&  aItem );

        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

		/**
		* Issue pointer event
		* @return Symbian OS error code.
		*/
		virtual TInt PointerEvent( CStifItemParser&  aItem );
    
		/**
		* Issue pointer event
		* @return Symbian OS error code.
		*/
		virtual TInt KeyEvent( CStifItemParser&  aItem );    

        /**
        * Issue media key event
        * @return Symbian OS error code.
        */
		virtual TInt MediaKeyEvent( CStifItemParser&  aItem );
		
        /**
        * Issue foreground/background event
        * @return Symbian OS error code.
        */
		virtual TInt SetForeground( CStifItemParser&  aItem );
		
        /**
        * Parse inputs
        * @return Symbian OS error code.
        */
        TInt ParseInputs( CStifItemParser&  aItem, 
                          TMPXVideoControlType& aControlType,
                          TPointerEvent& aEventType, 
                          TEventCode& aEventCode,
                          TKeyEvent& aKeyEvent );
        
        /**
        * Check expected result
        * @return Symbian OS error code.
        */
        TInt CheckExpectedResult( CStifItemParser&  aItem );
        
        /**
        * Check expected command
        * @return Symbian OS error code.
        */
        TInt CheckExpectedCommand( CStifItemParser&  aItem );
 
        
    private:    

        CMPXVideoPlaybackContainer*          iContainer; 
        CMPXVideoPlaybackControl*            iControl;
        CMPXVideoPlaybackUserInputHandler*   iUserInputHandler;
        
};

#endif      // USERINPUTHANDLER_TEST_H

// End of File
