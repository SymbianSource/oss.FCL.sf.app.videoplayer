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


#ifndef MEDIATOREVENTVERIFIER_H
#define MEDIATOREVENTVERIFIER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

// CONSTANTS

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KMediatorEventVerifierLogPath, "\\logs\\testframework\\MediatorEventVerifier\\" ); 
// Log file
_LIT( KMediatorEventVerifierLogFile, "MediatorEventVerifier.txt" ); 
_LIT( KMediatorEventVerifierLogFileWithTitle, "MediatorEventVerifier_[%S].txt" );

// FORWARD DECLARATIONS
class CMediatorEventVerifier;
class CVCXMediatorEventWatcher;

// DATA TYPES

// CLASS DECLARATION

/**
*  CMediatorEventVerifier test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMediatorEventVerifier) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMediatorEventVerifier* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CMediatorEventVerifier();

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
        CMediatorEventVerifier( CTestModuleIf& aTestModuleIf );

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
         * Checks for errors in events.  
         */
        virtual TInt CheckEvents( CStifItemParser& aItem );
        
        /**
         * Adds expected mediator event. 
         */
        virtual TInt AddEvent( CStifItemParser& aItem );
        
        /**
         * Waits until expected events have been got. Blocks.
         * @param timeout minutes, default is 2.
         */
        virtual TInt WaitEvents( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //[TestMethods] - Do not remove

    private:    // Data
        
        CVCXMediatorEventWatcher* iMediatorEventWatcher;
    };

#endif      // MEDIATOREVENTVERIFIER_H

// End of File
