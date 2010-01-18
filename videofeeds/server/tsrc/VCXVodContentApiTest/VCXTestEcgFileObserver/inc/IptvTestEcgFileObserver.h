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




#ifndef IPTVTESTECGFILEOBSERVER_H
#define IPTVTESTECGFILEOBSERVER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <f32file.h>
#include <BAUTILS.H>
#include "MTestUtilDownloadObserver.h"

// CONSTANTS
// Logging path
_LIT( KIptvTestEcgFileObserverLogPath, "\\logs\\testframework\\IptvTestEcgFileObserver\\" );
// Log file
_LIT( KIptvTestEcgFileObserverLogFile, "IptvTestEcgFileObserver.txt" );
_LIT( KIptvTestEcgFileObserverLogFileWithTitle, "IptvTestEcgFileObserver_[%S].txt" );

// DATA TYPES

// FORWARD DECLARATIONS
class CIptvTestUtilModule;
class CIptvTestUtilities;
class CTestUtilConnection;
class CTestUtilConnectionWaiter;
class CIptvTestDownloadManager;
class CIptvTestMobilecrashWatcher;
class CIptvTestEcgFileObserver;

// CLASS DECLARATION

/**
*  CIptvTestEcgFileObserver test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CIptvTestEcgFileObserver) : public CScriptBase, public MTestUtilDownloadObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CIptvTestEcgFileObserver* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CIptvTestEcgFileObserver();

    public: // New functions

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    	void ConnectionCreated();
    	void ConnectionClosed();
    	void ConnectionFailed();
	    void ConnectionAlreadyExists();
	    void ConnectionTimeout();

	    void DownloadFinished(TInt aError);

    private:

        /**
        * C++ default constructor.
        */
        CIptvTestEcgFileObserver( CTestModuleIf& aTestModuleIf );

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
        * Test methods are listed below.
        */

		/**
        * Start a download.
        */
		virtual TInt DownloadL( CStifItemParser& aItem );

        /**
        * Attemps to parse given address to a long test url.
        */
        virtual TBool ParseAddress( TPtrC& aAddress );

        /**
        * Sets the observed path for a service.
        * 
        * @param service name.
        */
        virtual TInt SetObservedServicePathL( CStifItemParser& aItem );

        /**
        * Adds file to observed filelist.
        * 
        * @param file name of thumbnail.
        */
        virtual TInt AddObservedFilenameL( CStifItemParser& aItem );

        /**
        * Gets and saves the last modified date for all the observed files.
        */
        virtual TInt GetFileDateL( CStifItemParser& aItem );

        /**
        * Gets the last modified date of the observed files and compares it to saved value.
        * 
        * @param list of expected values for each file ( optional, default is false )
        */
        virtual TInt CheckFileDateL( CStifItemParser& aItem );

        /**
        * Compares the existance of the observed files to the given value.
        * 
        * @param list of expected values for each file ( optional, default is true )
        */
        virtual TInt CheckFileState( CStifItemParser& aItem );

        /**
        * Synchronize system time with server time.
        */
        virtual TInt SyncTimeWithServerL( CStifItemParser& aItem );

    private:    // Data
		HBufC* iObservedServicePath;
		RPointerArray<HBufC> iObservedFiles;
		RArray<TTime> iFileModifiedDates;
        RFs iFs;
		CIptvTestDownloadManager* iDownloadManager;
    };

#endif      // IPTVTESTECGFILEOBSERVER_H

// End of File
