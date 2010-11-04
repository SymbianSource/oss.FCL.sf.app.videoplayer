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
* Description:  Video Playback User Input Handler test class for STIF testing
*
*/

// Version : %version: 1 %


// INCLUDE FILES
#include <Stiftestinterface.h>
#include <ecom.h>
#include <SettingServerClient.h>

#include <mpxcollectionplugin.h>
#include <mpxcollectionmessagedefs.h>
#include <vcxmyvideosdefs.h>
#include "vcxmyvideosactivetask.h"
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideosvideocache_stub.h"
#include "ut_collectionplugintest.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCollectionPluginTest::CCollectionPluginTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCollectionPluginTest::CCollectionPluginTest( CTestModuleIf& aTestModuleIf )
    : CScriptBase( aTestModuleIf )
{
}

// -----------------------------------------------------------------------------
// CCollectionPluginTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCollectionPluginTest::ConstructL()
{
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;

    TInt ret = settingServer.Connect();

    if ( ret != KErrNone )
    {
        User::Leave(ret);
    }

    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 

    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);

    if ( ret != KErrNone )
    {
        User::Leave(ret);
    } 

    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;
    
    if ( loggerSettings.iAddTestCaseTitle )
    {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KCollectionplugintest_LogFileWithTitle, &title);
    }
    else
    {
        logFileName.Copy(KCollectionplugintest_LogFile);
    }

    iLog = CStifLogger::NewL( KCollectionplugintest_LogPath, 
                              logFileName,
                              CStifLogger::ETxt,
                              CStifLogger::EFile,
                              EFalse );
    
    SendTestClassVersion();
    
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles );
    
    CreateL();
}

// -----------------------------------------------------------------------------
// CCollectionPluginTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCollectionPluginTest* CCollectionPluginTest::NewL( CTestModuleIf& aTestModuleIf )
{
CCollectionPluginTest* self = new (ELeave) CCollectionPluginTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// Destructor
CCollectionPluginTest::~CCollectionPluginTest()
{ 
    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 
    
    REComSession::FinalClose();
}

//-----------------------------------------------------------------------------
// CCollectionPluginTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CCollectionPluginTest::SendTestClassVersion()
{
    TVersion moduleVersion;
    moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
    moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
    moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
    
    TFileName moduleName;
    moduleName = _L("ut_collectionplugintest.dll");

    TBool newVersionOfMethod = ETrue;
    TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( CTestModuleIf& aTestModuleIf ) 
{
    return ( CScriptBase* ) CCollectionPluginTest::NewL( aTestModuleIf );
}

//  End of File
