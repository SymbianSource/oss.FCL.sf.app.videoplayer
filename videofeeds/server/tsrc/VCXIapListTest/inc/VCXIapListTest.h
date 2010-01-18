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




#ifndef IPTVIAPLISTTEST_H
#define IPTVIAPLISTTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KIptvIapListTestLogPath, "\\logs\\testframework\\IptvIapListTest\\" ); 
// Log file
_LIT( KIptvIapListTestLogFile, "IptvIapListTest.txt" ); 

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CIptvIapListTest;
class CIptvIapList;
// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CIptvIapListTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CIptvIapListTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CIptvIapListTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CIptvIapListTest();

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
        CIptvIapListTest( CTestModuleIf& aTestModuleIf );

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
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
                
        virtual TInt Create( CStifItemParser& aItem );
        
        // params: id, priority
        virtual TInt AddIap( CStifItemParser& aItem );
        
        // params: list index, 
        virtual TInt DeleteIap( CStifItemParser& aItem );

        // params: list index
        virtual TInt DeleteIapByReference( CStifItemParser& aItem );
        
        // params: list index, new id, new priority
        virtual TInt ModifyIap( CStifItemParser& aItem );
        
        //params: list index
        virtual TInt GetIap( CStifItemParser& aItem );
        
        //params: expected count (optional)
        virtual TInt CountIaps( CStifItemParser& aItem );
        
        virtual TInt SetIapList( CStifItemParser& aItem );
        
        virtual TInt SortListByPriority( CStifItemParser& aItem );
        
        // params: id
        virtual TInt FindIap( CStifItemParser& aItem );
       
		virtual TInt PrintIapList( CStifItemParser& aItem );
       	virtual TInt CreateTestIapList( CStifItemParser& aItem );
        
        // externalizes and internalizes the data to new IptvIapList which is assigned then to iIptvIapList
        virtual TInt DoExternalizeAndInternalize( CStifItemParser& aItem );
        
        virtual TInt Destroy( CStifItemParser& aItem );
       
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
        
        CIptvIapList* iIptvIapList;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // IPTVIAPLISTTEST_H

// End of File
