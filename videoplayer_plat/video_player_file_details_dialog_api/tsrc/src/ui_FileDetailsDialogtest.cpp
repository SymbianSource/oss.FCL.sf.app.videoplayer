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


// Version : %version: 4 %

// INCLUDE FILES
#include <Stiftestinterface.h>
#include "ui_FileDetailsDialogtest.h"
#include <SettingServerClient.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cui_FileDetailsDialogtest::Cui_FileDetailsDialogtest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
Cui_FileDetailsDialogtest::Cui_FileDetailsDialogtest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// Cui_FileDetailsDialogtest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void Cui_FileDetailsDialogtest::ConstructL()
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
        logFileName.Format(Kui_FileDetailsDialogtestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(Kui_FileDetailsDialogtestLogFile);
        }

    iLog = CStifLogger::NewL( Kui_FileDetailsDialogtestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    iControlEventQueue = CVcxTestControlEventQueue::NewL( iLog );
    
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// Cui_FileDetailsDialogtest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
Cui_FileDetailsDialogtest* Cui_FileDetailsDialogtest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    Cui_FileDetailsDialogtest* self = new (ELeave) Cui_FileDetailsDialogtest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
Cui_FileDetailsDialogtest::~Cui_FileDetailsDialogtest()
    { 
    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    delete iControlEventQueue;
    iControlEventQueue = NULL;
    }

//-----------------------------------------------------------------------------
// Cui_FileDetailsDialogtest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void Cui_FileDetailsDialogtest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("ui_FileDetailsDialogtest.dll");

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

    return ( CScriptBase* ) Cui_FileDetailsDialogtest::NewL( aTestModuleIf );

    }


//  End of File
