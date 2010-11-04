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
#include "MediatorEventVerifier.h"
#include <SettingServerClient.h>

#include "VCXMediatorEventWatcher.h"

// CONSTANTS

// MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorEventVerifier::CMediatorEventVerifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMediatorEventVerifier::CMediatorEventVerifier( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CMediatorEventVerifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMediatorEventVerifier::ConstructL()
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
        logFileName.Format(KMediatorEventVerifierLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KMediatorEventVerifierLogFile);
        }

    iLog = CStifLogger::NewL( KMediatorEventVerifierLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    
    iMediatorEventWatcher = CVCXMediatorEventWatcher::NewL();
    }

// -----------------------------------------------------------------------------
// CMediatorEventVerifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMediatorEventVerifier* CMediatorEventVerifier::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CMediatorEventVerifier* self = new (ELeave) CMediatorEventVerifier( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CMediatorEventVerifier::~CMediatorEventVerifier()
    { 

    // Delete resources allocated from test methods
    Delete();

    delete iMediatorEventWatcher;
    iMediatorEventWatcher = NULL;
    
    // Delete logger
    delete iLog; 
    }

//-----------------------------------------------------------------------------
// CMediatorEventVerifier::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CMediatorEventVerifier::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("MediatorEventVerifier.dll");

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

    return ( CScriptBase* ) CMediatorEventVerifier::NewL( aTestModuleIf );

    }


//  End of File
