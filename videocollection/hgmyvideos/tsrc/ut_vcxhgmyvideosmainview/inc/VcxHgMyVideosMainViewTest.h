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


#ifndef VCXHGMYVIDEOSMAINVIEWTEST_H
#define VCXHGMYVIDEOSMAINVIEWTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KVcxHgMyVideosMainViewTestLogPath, "\\logs\\testframework\\VcxHgMyVideosMainViewTest\\" ); 

// Log file
_LIT( KVcxHgMyVideosMainViewTestLogFile, "VcxHgMyVideosMainViewTest.txt" ); 
_LIT( KVcxHgMyVideosMainViewTestLogFileWithTitle, "VcxHgMyVideosMainViewTest_[%S].txt" );


// FORWARD DECLARATIONS
class CVcxHgMyVideosMainViewTest;
class CVcxHgMyVideosMainView;


// CLASS DECLARATION

/**
*  CVcxHgMyVideosMainViewTest test class for STIF Test Framework TestScripter.
*/
NONSHARABLE_CLASS(CVcxHgMyVideosMainViewTest) : public CScriptBase
    {
    
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CVcxHgMyVideosMainViewTest* NewL( CTestModuleIf& aTestModuleIf );

    /**
    * Destructor.
    */
    virtual ~CVcxHgMyVideosMainViewTest();

// Functions from base classes

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
    CVcxHgMyVideosMainViewTest( CTestModuleIf& aTestModuleIf );

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
    * 
    * @param aItem Script line containing parameters.
    * @return Symbian OS error code.
    */
    virtual TInt CreateL( CStifItemParser& aItem );
    virtual TInt DeleteL( CStifItemParser& aItem );
    virtual TInt AppUi( CStifItemParser& aItem );
    virtual TInt Cba( CStifItemParser& aItem );
    virtual TInt TitlePaneL( CStifItemParser& aItem );
    virtual TInt PlayerWrapperL( CStifItemParser& aItem );
    virtual TInt CategoryListL( CStifItemParser& aItem );
    virtual TInt VideoListL( CStifItemParser& aItem );
    virtual TInt ActivateCategoryListL( CStifItemParser& aItem );
    virtual TInt ActivateVideoListL( CStifItemParser& aItem );      
    virtual TInt HandleCommandL( CStifItemParser& aItem );        
    virtual TInt UpdateMSKToolbarL( CStifItemParser& aItem );
    virtual TInt HandleKeyEventL( CStifItemParser& aItem );
    virtual TInt DynInitMskL( CStifItemParser& aItem );
    virtual TInt MakeToolbarVisibleL( CStifItemParser& aItem );
    virtual TInt DynInitToolbarL( CStifItemParser& aItem );
    virtual TInt HandleCustomViewMessageL( CStifItemParser& aItem );
    virtual TInt DoActivateL( CStifItemParser& aItem );
    virtual TInt DoDeactivateL( CStifItemParser& aItem );
    
    /**
     * Method used to log version of test class
     */
    void SendTestClassVersion();

// Data
    
    CVcxHgMyVideosMainView* iView;

    };

#endif      // VCXHGMYVIDEOSMAINVIEWTEST_H

