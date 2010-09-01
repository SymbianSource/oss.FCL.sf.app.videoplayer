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


// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "MediatorEventVerifier.h"
#include "VCXMediatorEventWatcher.h"
#include "VCXTestLog.h"

// CONSTANTS

// MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorEventVerifier::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMediatorEventVerifier::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMediatorEventVerifier::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMediatorEventVerifier::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CheckEvents", CMediatorEventVerifier::CheckEvents ),
        ENTRY( "AddEvent", CMediatorEventVerifier::AddEvent ),
        ENTRY( "WaitEvents", CMediatorEventVerifier::WaitEvents ),
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CMediatorEventVerifier::CheckEvents
// -----------------------------------------------------------------------------
//
TInt CMediatorEventVerifier::CheckEvents( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::CheckEvents");
         
    // Print to UI
    _LIT( KMediatorEventVerifier, "MediatorEventVerifier" );
    _LIT( KWhere, "In CheckEvents" );
    TestModuleIf().Printf( 0, KMediatorEventVerifier, KWhere );
    // Print to log file
    iLog->Log( KWhere );

    TRAPD( err, iMediatorEventWatcher->CheckEventsL() );
            
    VCXLOGLO1("<<<CVCXMediatorEventWatcher::CheckEvents");
    return err;
    }

// -----------------------------------------------------------------------------
// CMediatorEventVerifier::AddEvent
// -----------------------------------------------------------------------------
//
TInt CMediatorEventVerifier::AddEvent( CStifItemParser& aItem )
    {  
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::AddEvent");
         
    // Print to UI
    _LIT( KMediatorEventVerifier, "MediatorEventVerifier" );
    _LIT( KWhere, "In AddEvent" );
    TestModuleIf().Printf( 0, KMediatorEventVerifier, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    TInt event;
    User::LeaveIfError( aItem.GetNextInt( event ) );
    TRAPD( err, iMediatorEventWatcher->AddExpectedEventL( event ) ); 
    
    VCXLOGLO1("<<<CVCXMediatorEventWatcher::AddEvent");
    return err;
    }

// -----------------------------------------------------------------------------
// CMediatorEventVerifier::WaitEvents
// -----------------------------------------------------------------------------
//
TInt CMediatorEventVerifier::WaitEvents( CStifItemParser& aItem )
    {  
    VCXLOGLO1(">>>CVCXMediatorEventWatcher::WaitEvents");
         
    // Print to UI
    _LIT( KMediatorEventVerifier, "MediatorEventVerifier" );
    _LIT( KWhere, "In WaitEvents" );
    TestModuleIf().Printf( 0, KMediatorEventVerifier, KWhere );
    // Print to log file
    iLog->Log( KWhere );
    
    TInt seconds;
    if( aItem.GetNextInt( seconds ) != KErrNone ) 
        {
        seconds = 2 * 60;
        }
        
    TRAPD( err, iMediatorEventWatcher->WaitEventsL( seconds ) );
    
    VCXLOGLO1("<<<CVCXMediatorEventWatcher::WaitEvents");
    return err;
    }


//  [End of File] - Do not remove
