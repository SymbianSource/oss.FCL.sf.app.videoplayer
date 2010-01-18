/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroyPushL
#include <e32property.h>

#include "VCXTestLog.h"
#include "VCXTestCommon.h"
#include "VCXTestPSSubscriber.h"
#include "VCXTestPSObserver.h"

#include "FusionMpxPlayerTestCommon.h"
#include "FusionMpxPlayerTestCmdListener.h"
#include "FusionMpxPlayerTester.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTestCmdListener::
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFusionMpxPlayerTestCmdListener* CFusionMpxPlayerTestCmdListener::NewL( TUint aProcessId, MFusionMpxPlayerTestShutdownObserver* aObserver )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTestCmdListener::NewL");
    CFusionMpxPlayerTestCmdListener* self = new (ELeave) CFusionMpxPlayerTestCmdListener();
    CleanupStack::PushL(self);
    self->ConstructL( aProcessId, aObserver );
    CleanupStack::Pop( self );
    VCXLOGLO1("<<<CFusionMpxPlayerTestCmdListener::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTestCmdListener::~CFusionMpxPlayerTestCmdListener
// -----------------------------------------------------------------------------
//
CFusionMpxPlayerTestCmdListener::~CFusionMpxPlayerTestCmdListener()
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTestCmdListener::~CFusionMpxPlayerTestCmdListener");

    delete iGlobalShutdownSubcriber;
    iGlobalShutdownSubcriber = NULL;

    delete iCmdSubcriber;
    iCmdSubcriber = NULL;

    delete iTester;
    iTester = NULL;
    
    VCXLOGLO1("<<<CFusionMpxPlayerTestCmdListener::~CFusionMpxPlayerTestCmdListener");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTestCmdListener::CFusionMpxPlayerTestCmdListener
// -----------------------------------------------------------------------------
//
CFusionMpxPlayerTestCmdListener::CFusionMpxPlayerTestCmdListener()
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTestCmdListener::CFusionMpxPlayerTestCmdListener");

    VCXLOGLO1("<<<CFusionMpxPlayerTestCmdListener::CFusionMpxPlayerTestCmdListener");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTestCmdListener::ConstructL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTestCmdListener::ConstructL( TUint aProcessId, MFusionMpxPlayerTestShutdownObserver* aObserver )
    {
    VCXLOGLO1(">>>CFusionMpxPlayerTestCmdListener::ConstructL");

    iObserver = aObserver;
    iTester = CFusionMpxPlayerTester::NewL( this );

    TInt key( 0 );
    
    // Define and listen global shutdown key.
    RProperty::Define( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExeGlobalShutdownKey, RProperty::EInt );
    
    iGlobalShutdownSubcriber = CVCXTestPSSubscriber::NewL( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExeGlobalShutdownKey, RProperty::EInt, this );
    iGlobalShutdownSubcriber->Start();
    VCXLOGLO2("CFusionMpxPlayerTestCmdListener:: Listening P&S key KFusionMpxPlayerTestExeGlobalShutdownKey: %d", KFusionMpxPlayerTestExeGlobalShutdownKey);
    
    // Get PS key base.
    
    iPSKeyBase = 0;
    
    TInt err( KErrNone );
    
    err = CVCXTestCommon::GetPSProperty( 
            KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExeGlobalTesterCount, iPSKeyBase );
    if( err == KErrNotFound )
        {
        User::LeaveIfError( RProperty::Define( 
                KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExeGlobalTesterCount, RProperty::EInt ) );
        User::LeaveIfError( CVCXTestCommon::SetPSProperty( 
                KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExeGlobalTesterCount, 1 ) );
        }
    else
    if( err != KErrNone ) 
        {
        User::Leave( err );
        }
    User::LeaveIfError( CVCXTestCommon::SetPSProperty( 
            KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExeGlobalTesterCount, ++iPSKeyBase ) );
    
    iPSKeyBase = iPSKeyBase * 1000 + aProcessId;

    VCXLOGLO2("CVCXConnUtilTestExeTester:: iPSKeyBase: %d", iPSKeyBase);    
    
    // Define private P&S keys.

    key = KFusionMpxPlayerTestExePsKeyCmd + iPSKeyBase;
    RProperty::Define( KFusionMpxPlayerTestPScategory, key, RProperty::EInt );
    VCXLOGLO2("CFusionMpxPlayerTestCmdListener:: Defined KFusionMpxPlayerTestPScategory P&S key %d", key);
    
    key = KFusionMpxPlayerTestExePsKeyCmdIntParam1 + iPSKeyBase;
    RProperty::Define( KFusionMpxPlayerTestPScategory, key, RProperty::EInt );
    VCXLOGLO2("CFusionMpxPlayerTestCmdListener:: Defined KFusionMpxPlayerTestExePsKeyCmdIntParam1 P&S key %d", key);
    
    key = KFusionMpxPlayerTestExePsKeyCmdIntParam2 + iPSKeyBase;
    RProperty::Define( KFusionMpxPlayerTestPScategory, key, RProperty::EInt );
    VCXLOGLO2("CFusionMpxPlayerTestCmdListener:: Defined KFusionMpxPlayerTestExePsKeyCmdIntParam2 P&S key %d", key);
    
    key = KFusionMpxPlayerTestExePsKeyCmdDescParam1 + iPSKeyBase;
    RProperty::Define( KFusionMpxPlayerTestPScategory, key, RProperty::EText );
    VCXLOGLO2("CFusionMpxPlayerTestCmdListener:: Defined KFusionMpxPlayerTestExePsKeyCmdDescParam1 P&S key %d", key);
    
    key = KFusionMpxPlayerTestExePsKeyResponse + iPSKeyBase;
    RProperty::Define( KFusionMpxPlayerTestPScategory, key, RProperty::EInt );
    VCXLOGLO2("CFusionMpxPlayerTestCmdListener:: Defined KFusionMpxPlayerTestExePsKeyResponse P&S key %d", key);

    key = KFusionMpxPlayerTestExePsKeyResponseError + iPSKeyBase;
    RProperty::Define( KFusionMpxPlayerTestPScategory, key, RProperty::EInt );
    VCXLOGLO2("CFusionMpxPlayerTestCmdListener:: Defined P&S KFusionMpxPlayerTestExePsKeyResponseError key %d", key);
    
    // Subscribe P&S command key.

    iCmdSubcriber = CVCXTestPSSubscriber::NewL( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyCmd + iPSKeyBase, RProperty::EInt, this );
    iCmdSubcriber->Start();
    VCXLOGLO2("CFusionMpxPlayerTestCmdListener:: Listening P&S key KFusionMpxPlayerTestExePsKeyCmd: %d", 
            KFusionMpxPlayerTestExePsKeyCmd + iPSKeyBase);

    VCXLOGLO1("<<<CFusionMpxPlayerTestCmdListener::ConstructL");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTestCmdListener::PlayLocalVideo
// -----------------------------------------------------------------------------
//
TInt CFusionMpxPlayerTestCmdListener::PlayLocalVideo( const TDesC& aFileName )
    {
    return iTester->OpenLocalFile( aFileName );
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTestCmdListener::SendResponse
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTestCmdListener::SendResponse( const TInt aResponse, TInt aError )
    {
    // Set response that cmd has been gotten.
    VCXLOGLO3("CFusionMpxPlayerTestCmdListener:: Sending response %d, err: %d ----->", aResponse, aError);
    CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory,
            KFusionMpxPlayerTestExePsKeyResponseError + iPSKeyBase, aError );
    CVCXTestCommon::SetPSProperty( KFusionMpxPlayerTestPScategory, 
            KFusionMpxPlayerTestExePsKeyResponse + iPSKeyBase, aResponse );
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTestCmdListener::ValueChangedL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTestCmdListener::ValueChangedL( const TUid& aUid, const TUint32& aKey, const TInt&  aValue )
    {
    // Right now this is only category we are subscribing for.
    if( aUid != KFusionMpxPlayerTestPScategory )
        {
        return;
        }

    if( aKey == KFusionMpxPlayerTestExeGlobalShutdownKey )
        {
        if( aValue == 1 )
            {
            VCXLOGLO1("CFusionMpxPlayerTestCmdListener::ValueChangedL: cmd GLOBAL SHUTDOWN ----->");

            if( iObserver )
                {
                iObserver->ShutdownCommandReceived();
                }
            }
        return;
        }

    VCXLOGLO1(">>>CFusionMpxPlayerTestCmdListener::ValueChangedL TInt");

    // Store possible parameters.
    TInt intParam1(0);
    TInt intParam2(0);
    TBuf<256> descParam1;

    CVCXTestCommon::GetPSProperty( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExePsKeyCmdIntParam1 + iPSKeyBase, intParam1 );
    CVCXTestCommon::GetPSProperty( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExePsKeyCmdIntParam2 + iPSKeyBase, intParam2 );
    CVCXTestCommon::GetPSProperty( KFusionMpxPlayerTestPScategory, KFusionMpxPlayerTestExePsKeyCmdDescParam1 + iPSKeyBase, descParam1 );

    TInt err( KErrNone );
    
    // Handle commands.
    if( aKey == KFusionMpxPlayerTestExePsKeyCmd + iPSKeyBase )
        {
        switch( aValue )
            {
            case EVcxFusionMpxPlayerCommandShutdown:
                {
                VCXLOGLO1("CFusionMpxPlayerTestCmdListener:: cmd EVcxFusionMpxPlayerCommandShutdown ----->");

                err = iTester->CheckExpectedMessages();

                iTester->PrepareCloseL();
                
                if( iObserver )
                    {
                    iObserver->ShutdownCommandReceived();
                    }
                }
                break;

            case EVcxFusionMpxPlayerCommandOpenLocalFile:
                {
                VCXLOGLO1("CFusionMpxPlayerTestCmdListener:: cmd EVcxFusionMpxPlayerCommandOpenLocalFile ----->");
                err = PlayLocalVideo( descParam1 );  
                }
                break;
                        
            case EVcxFusionMpxPlayerCommandOpenUrl:
                {
                VCXLOGLO1("CFusionMpxPlayerTestCmdListener:: cmd EVcxFusionMpxPlayerCommandOpenUrl ----->");
                TRAP( err, iTester->OpenUrlL( descParam1, intParam1 ) );
                }
                break;
                
            case EVcxFusionMpxPlayerCommandGeneralPlaybackCommand:
                {
                switch( intParam1 )
                    {
                    // List commands here which need 1 int param.
                    case EPbCmdSetVolume:
                        {
                        TRAP( err, iTester->CreateGeneralPlaybackCommandL( 
                                static_cast<TMPXPlaybackCommand>( intParam1 ), intParam2 ) );
                        }
                        break;
                        
                    // List commands without params here.
                    default:
                        TRAP( err, iTester->CreateGeneralPlaybackCommandL( 
                                static_cast<TMPXPlaybackCommand>( intParam1 ) ) );
                        break;
                    }
                }
                break;
                
            case EVcxFusionMpxPlayerCommandVideoPlaybackCommand:
                {
                TRAP( err, 
                        iTester->CreateVideoPlaybackCommandL( static_cast<TMPXVideoPlaybackCommand>( intParam1 ) ) );
                }
                break;
                
            default:
                {
                SendResponse( EVcxFusionMpxPlayerResponseCommandComplete, KErrGeneral );                
                VCXLOGLO2("CFusionMpxPlayerTestCmdListener:: UNKNOWN cmd %d! ----->", aValue);
                }
                break;
            }
        }
    
    if( err )
        {
        VCXLOGLO2("CFusionMpxPlayerTestCmdListener:: Error handling the command: %d.", err);
        SendResponse( EVcxFusionMpxPlayerResponseCommandComplete, err );
        }

    VCXLOGLO1("<<<CFusionMpxPlayerTestCmdListener::ValueChangedL TInt");
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTestCmdListener::ValueChangedL
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTestCmdListener::ValueChangedL( const TUid& /* aUid */, const TUint32& /* aKey */, const TDesC& /* aValue */ )
    {
    
    }

// -----------------------------------------------------------------------------
// CFusionMpxPlayerTestCmdListener::TesterCommandComplete
// -----------------------------------------------------------------------------
//
void CFusionMpxPlayerTestCmdListener::TesterCommandComplete()
    {
    VCXLOGLO1("CFusionMpxPlayerTestCmdListener:: Command complete ----->");
    SendResponse( EVcxFusionMpxPlayerResponseCommandComplete, KErrNone );
    }

//  End of File
