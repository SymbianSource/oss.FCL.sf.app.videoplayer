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




#ifndef VCXScheduledReminderTest_H
#define IPTVSCHEDULINGENGINETEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include "MIptvTestTimerObserver.h"

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KVCXScheduledReminderTestLogPath, "\\logs\\testframework\\VCXScheduledReminderTest\\" ); 
// Log file
_LIT( KVCXScheduledReminderTestLogFile, "VCXScheduledReminderTest.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CVCXScheduledReminderTest;
class CCseSchedulerApi;
class CIptvTestUtilities;
class CVCXTestServiceUtilModule;
class CIptvTestTimer;
class CCseScheduledProgram;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CVCXScheduledReminderTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CVCXScheduledReminderTest) : public CScriptBase, MIptvTestTimerObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVCXScheduledReminderTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CVCXScheduledReminderTest();


    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
        void TimerComplete(TInt aTimerId, TInt aError);

    private:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
    
        TUint32 GetServiceL( CStifItemParser& aItem );   
         
        TUint32 GetIapL( CStifItemParser& aItem );
        
		TBool CompareScheduleLists( RPointerArray<CCseScheduledProgram> aList );
		
		void PrintArray( RPointerArray<CCseScheduledProgram> aList );

		TBool CompareScheduleItems( CCseScheduledProgram* aFirst, CCseScheduledProgram* aSecond );

		TInt32 GetPluginUidByNumber( TInt aValue );
		
		TInt SetTimeoutL( CStifItemParser& aItem );


    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();
        

    private:

        /**
        * C++ default constructor.
        */
        CVCXScheduledReminderTest( CTestModuleIf& aTestModuleIf );

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
        
        virtual TInt CreateL( CStifItemParser& aItem );

        virtual TInt RefreshServiceL( CStifItemParser& aItem );
        
        virtual TInt SetIapL( CStifItemParser& aItem );
        
        virtual TInt ScheduleDownloadL( CStifItemParser& aItem );
        
        virtual TInt AddScheduleL( CStifItemParser& aItem );
        
        virtual TInt RemoveScheduleL( CStifItemParser& aItem );
        
        virtual TInt RemoveAllSchedulesL( CStifItemParser& aItem );
        
        virtual TInt GetScheduleL( CStifItemParser& aItem );
        
		virtual TInt GetSchedulesByAppUidL( CStifItemParser& aItem );
		
		virtual TInt GetOverlappingSchedulesL( CStifItemParser& aItem );
		
		virtual TInt GetSchedulesByPluginUidL( CStifItemParser& aItem );
		
		virtual TInt GetSchedulesByTypeL( CStifItemParser& aItem );
		
		virtual TInt GetSchedulesByTimeL( CStifItemParser& aItem );
		
		virtual TInt CheckCountL( CStifItemParser& aItem );
        
        virtual TInt AdjustClockL( CStifItemParser& aItem );
        
        virtual TInt EmptyArray( CStifItemParser& aItem );
        
        virtual TInt RemoveAllFromArrayL( CStifItemParser& aItem );
        
    private:    // Data
    
     	CIptvTestTimer*		iShutdownTimer;	
    
        CCseSchedulerApi*	iSchedulerClient;
        
        CIptvTestUtilities*	iTestUtil;

		RPointerArray<CCseScheduledProgram> iScheduleArray;
    };

#endif      // IPTVSCHEDULINGENGINETEST_H

// End of File
