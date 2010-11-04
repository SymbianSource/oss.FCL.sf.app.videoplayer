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
* Description:  This file contains testclass implementation.*
*/


// INCLUDE FILES
#include <Stiftestinterface.h>
#include "VcxHgMyVideosMainViewTest.h"
#include <SettingServerClient.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::CVcxHgMyVideosMainViewTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosMainViewTest::CVcxHgMyVideosMainViewTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosMainViewTest::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;
    
    if(loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KVcxHgMyVideosMainViewTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KVcxHgMyVideosMainViewTestLogFile);
        }

    iLog = CStifLogger::NewL( KVcxHgMyVideosMainViewTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVcxHgMyVideosMainViewTest* CVcxHgMyVideosMainViewTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CVcxHgMyVideosMainViewTest* self = new (ELeave) CVcxHgMyVideosMainViewTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }

// Destructor
CVcxHgMyVideosMainViewTest::~CVcxHgMyVideosMainViewTest()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

//-----------------------------------------------------------------------------
// CVcxHgMyVideosMainViewTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CVcxHgMyVideosMainViewTest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("VcxHgMyVideosMainViewTest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CVcxHgMyVideosMainViewTest::NewL( aTestModuleIf );

    }
