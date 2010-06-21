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
* Description: STIF testclass declaration
*
*/

#ifndef VCXMYVIDEOSMDSDBTEST_H
#define VCXMYVIDEOSMDSDBTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <f32file.h>


#include <mpxcollectionplugin.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>

//#include <mpxmedia.h>


#include <mdesession.h>
#include <mdequery.h>
#include <harvesterclient.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>
#include <e32property.h>
#include "vcxmyvideosalbum.h"

#define protected public
#define private public
#include "vcxmyvideosmdsdb.h"
#undef private
#undef protected



// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KVcxMyVideosMdsDbTestLogPath, "\\logs\\testframework\\VcxMyVideosMdsDbTest\\" ); 
// Log file
_LIT( KVcxMyVideosMdsDbTestLogFile, "VcxMyVideosMdsDbTest.txt" ); 
_LIT( KVcxMyVideosMdsDbTestLogFileWithTitle, "VcxMyVideosMdsDbTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CVcxMyVideosMdsDbTest;
//class CVcxMyVideosMdsDb;
//class CMPXMedia;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;
extern CVcxMyVideosMdsDbTest* test;

// CLASS DECLARATION

/**
*  CVcxMyVideosMdsDbTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsDbTest) : public CScriptBase, public MVcxMyVideosMdsDbObserver
    {
    public:  // Constructors and destructor


        /**
        * Two-phased constructor.
        */
        static CVcxMyVideosMdsDbTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CVcxMyVideosMdsDbTest();
        
    // FROM MVcxMyVideosMdsDbObserver
        
        /**
        * Handler function for database events.
        *
        * @param aEvent Event type.
        * @param aId    Array of IDs in database.
        */
        void HandleMyVideosDbEvent( TMPXChangeEventType aEvent, RArray<TUint32>& aId, TInt aEventsLeft  );

        /**
        * Handler function for list fetching events. This callback is called as a response
        * to CreateVideoList( aSync = EFalse ).
        *
        * @param aVideoList          List of videos fetched from MDS.
        * @param aNewItemsStartIndex Start position for the new items added. If -1, then no new
        *                            items were added.
        * @param aComplete           ETrue if list is complete and no more items are expected,
        *                            EFalse if there are more to come.
        */
        void HandleCreateVideoListResp( CMPXMedia* aVideoList,
                TInt aNewItemsStartIndex, TBool aComplete );

        /**
        * Handler function for media remove/insert events. This is called every time
        * media is removed or inserted (objects become present or non present).
        */
        void HandleObjectPresentNotification(){};

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
        CVcxMyVideosMdsDbTest( CTestModuleIf& aTestModuleIf );

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

        virtual TInt DeleteL( CStifItemParser& aItem );

        virtual TInt CancelL( CStifItemParser& aItem );

        virtual TInt AddVideoL( CStifItemParser& aItem );

        virtual TInt RemoveVideoL( CStifItemParser& aItem );

        virtual TInt UpdateVideoL( CStifItemParser& aItem );

        virtual TInt CreateVideoListL( CStifItemParser& aItem );
        
        virtual TInt DoCreateVideoListL( CStifItemParser& aItem );

        virtual TInt CreateVideoL( CStifItemParser& aItem );
        
        virtual TInt HandleQueryNewResultsL( CStifItemParser& aItem );

        virtual TInt HandleQueryCompletedL( CStifItemParser& aItem );
        
        virtual TInt AsyncHandleQueryCompletedL( CStifItemParser& aItem );
        
        virtual TInt HandleSessionErrorL( CStifItemParser& aItem );
        
        virtual TInt HandleObjectNotificationL( CStifItemParser& aItem );

        virtual TInt HandleObjectPresentNotification( CStifItemParser& aItem );

        virtual TInt ShutdownNotificationL( CStifItemParser& aItem );        
        
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        
        void PrepareMdsL(  CStifItemParser& aItem  ) ;

    private:    // Data
        
        CVcxMyVideosMdsDb* iMdsDb;

        RFs iFs;
        
    public:
    
        TInt mdssessionerror,  mdssessionasyncerror,
        mdsoperationleaves, mdsoperationreturns, mdeobjectopenresult,
        querycountresult, propertynotdefined, propertyisnull, handledbeventleave ;
        
        
        CMPXMedia* iVideoList;
        
    };



#endif      // VCXMYVIDEOSMDSDBTEST_H
