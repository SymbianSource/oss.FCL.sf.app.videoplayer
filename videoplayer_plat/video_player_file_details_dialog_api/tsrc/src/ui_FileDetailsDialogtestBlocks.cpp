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


// Version : %version: 9 %

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "ui_FileDetailsDialogtest.h"
#include <coeaui.h>
#include "EdwinTestControl.h"
#include <EIKENV.H>
#include <e32des16.h>
#include "VCXTestLog.h"
#include <ScreenSaverInternalPSKeys.h>
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cui_FileDetailsDialogtest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cui_FileDetailsDialogtest::Delete()
    {
    VCXLOGLO1(">>>Cui_FileDetailsDialogtest::Delete");
    
    VCXLOGLO1("Cui_FileDetailsDialogtest:: delete hwlight");
    delete iHwLight;
    iHwLight = NULL;
    
    VCXLOGLO1("Cui_FileDetailsDialogtest:: delete testcontrol");
    
    if( iEdwinTestControl )
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack( iEdwinTestControl );    
        delete iEdwinTestControl;
        iEdwinTestControl = NULL;
        }

    delete iMPFileDetails;
    iMPFileDetails = NULL;
    
    VCXLOGLO1("Cui_FileDetailsDialogtest:: delete queue");
    delete iControlEventQueue;
    iControlEventQueue = NULL;
    
    delete iTimer;
    iTimer = NULL;
    
    VCXLOGLO1("<<<Cui_FileDetailsDialogtest::Delete");
    }

// -----------------------------------------------------------------------------
// Cui_FileDetailsDialogtest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cui_FileDetailsDialogtest::RunMethodL(CStifItemParser& aItem)
    {
    static TStifFunctionInfo const
        KFunctions[] =
            {
            // Copy this line for every implemented function.
            // First string is the function name used in TestScripter script file.
            // Second is the actual implementation member function. 

            // [test cases entries] - Do not remove
            // test cases for the header "MPFileDetailsDialog.h"
            ENTRY( "CreateNewDialog", Cui_FileDetailsDialogtest::CreateNewDialog ),
            ENTRY( "ExecuteLDFromMedia", Cui_FileDetailsDialogtest::ExecuteLDFromMedia ),
            ENTRY( "ExecuteLDFromDetails", Cui_FileDetailsDialogtest::ExecuteLDFromDetails ),
            // test cases for the header "MPFileDetails.h"
            ENTRY( "CreateNewFileDetails", Cui_FileDetailsDialogtest::CreateNewFileDetails ),
            
            // Test methods for pointer events.
            ENTRY( "AddPointerEvent", Cui_FileDetailsDialogtest::AddPointerEventL ),
            ENTRY( "AddKeyEvent", Cui_FileDetailsDialogtest::AddKeyEventL ),
            ENTRY( "EnableControlEvents", Cui_FileDetailsDialogtest::EnableControlEvents ),
            ENTRY( "DisableControlEvents", Cui_FileDetailsDialogtest::DisableControlEvents ),
            ENTRY( "ResetControlEvents", Cui_FileDetailsDialogtest::ResetControlEvents ),
            };

const TInt count = sizeof( KFunctions ) /
sizeof( TStifFunctionInfo );

return RunInternalL( KFunctions, count, aItem );
}

// ---------------------------------------------------------------------------
// Cui_FileDetailsDialogtest::CreateNewDialog
// ---------------------------------------------------------------------------
//
TInt Cui_FileDetailsDialogtest::CreateNewDialog(CStifItemParser& /*aItem*/)
    {
    VCXLOGLO1(">>>Cui_FileDetailsDialogtest::CreateNewDialog");
        
    CRepository* cenRep( NULL );
    TRAPD( err, cenRep = CRepository::NewL( KPSUidScreenSaver ) );
    if( err == KErrNone && cenRep )
        {
        err = cenRep->Set( KScreenSaverAllowScreenSaver, 1 );
        if( err != KErrNone )
            {
            VCXLOGLO2("Cui_FileDetailsDialogtest:: cenrep set for KScreenSaverAllowScreenSaver failed: %d", err);
            }
        }
    else
        {
        VCXLOGLO2("Cui_FileDetailsDialogtest:: CRepository::NewL( KPSUidScreenSaver ) failed: %d", err);
        }
    
    if ( !iHwLight )
        {
        TRAP( err, iHwLight = CHWRMLight::NewL( this ) );
        }

    if ( err == KErrNone && iHwLight )
        {
        iTarget = CHWRMLight::EPrimaryDisplay;

        // KErrNotReady is get from ReserveLightL, if the application is currently
        // on the background.
        TRAP( err, iHwLight->ReserveLightL( iTarget );
            iHwLight->LightOnL( iTarget ) );
        if ( err != KErrNone )
            {
            VCXLOGLO2("Cui_FileDetailsDialogtest:: Light activation for CHWRMLight failed: %d", err);
            }
        }
    else
        {
        VCXLOGLO2("Cui_FileDetailsDialogtest:: CHWRMLight::NewL failed: %d", err);
        }
    
    iEdwinTestControl = CEdwinTestControl::NewL();
    CleanupStack::PushL(iEdwinTestControl);

    CCoeEnv::Static()->AppUi()->AddToStackL(iEdwinTestControl);
    
    iEdwinTestControl->MakeVisible(ETrue);
    CleanupStack::Pop(iEdwinTestControl);
    TRAP(err, iMPFileDetailsDialog = CMPFileDetailsDialog::NewL());
    if (err != KErrNone)
        {
        VCXLOGLO2("Cui_FileDetailsDialogtest:: CMPFileDetailsDialog::NewL failed: %d", err);
        }

    VCXLOGLO1("<<<Cui_FileDetailsDialogtest::CreateNewDialog");
    return err;
    }

//
// -----------------------------------------------------------------------------
// Cui_FileDetailsDialogtest::ExecuteLDFromMedia
// -----------------------------------------------------------------------------
//
TInt Cui_FileDetailsDialogtest::ExecuteLDFromMedia(CStifItemParser& aItem)
    {
    VCXLOGLO1(">>>Cui_FileDetailsDialogtest::ExecuteLDFromMedia");
    
    TInt err = KErrNotFound;
    TPtrC string;
    if ( (err = aItem.GetNextString(string) ) == KErrNone )
        {
        TBuf<120> KMediaFile;
        
        // If it's not full path then search from default location.
        if( string.FindF( _L(":\\") ) == KErrNotFound ) 
            {
            KMediaFile.Append(Kui_FileDetailsDialogtest_testPath);
            }
        KMediaFile.Append(string);

        // Start timeout timer.
        if( iControlEventsEnabled ) 
            {
            if( !iTimer )
                {
                iTimer = CVcxTestTimer::NewL( *this, 0 );
                }
            iTimer->After( 1000000 * 60 * 2 ); // 2 minutes.
            }
        
        TRAP(err, iMPFileDetailsDialog->ExecuteLD( KMediaFile ));

        if (err != KErrNone)
            {
            if (err == KErrNotSupported)
                {
                VCXLOGLO2("Cui_FileDetailsDialogtest:: File is not valid media file: %d", err);
                }
            else
                {
                VCXLOGLO2("Cui_FileDetailsDialogtest:: ExecuteLD returned with err: %d", err);
                }
            }
        
        err = ( err != KErrNone ? err : iError );
        if( iTimer )
            {
            iTimer->CancelTimer();
            }
        }
    else
        {
        VCXLOGLO1("Cui_FileDetailsDialogtest:: Parameter missing");
        }
    
    VCXLOGLO1("<<<Cui_FileDetailsDialogtest::ExecuteLDFromMedia");
    return err;
    }

// -----------------------------------------------------------------------------
// Cui_FileDetailsDialogtest::ExecuteLDFromDetails
// -----------------------------------------------------------------------------
//
TInt Cui_FileDetailsDialogtest::ExecuteLDFromDetails(CStifItemParser& /*aItem*/)
    {
    VCXLOGLO1(">>>Cui_FileDetailsDialogtest::ExecuteLDFromDetails");
    
    TInt err = KErrNotReady;

    if ( iMPFileDetails)
        {
        // Start timeout timer.
        if( iControlEventsEnabled ) 
            {
            if( !iTimer )
                {
                iTimer = CVcxTestTimer::NewL( *this, 0 );
                }
            iTimer->After( 1000000 * 60 * 2 ); // 2 minutes.
            }
        
        TRAP(err, iMPFileDetailsDialog->ExecuteLD(iMPFileDetails));

        if (err != KErrNone)
            {
            if (err == KErrNotSupported)
                {
                VCXLOGLO2("Cui_FileDetailsDialogtest:: File is not valid media file %d", err);
                }
            else
                {
                VCXLOGLO2("Cui_FileDetailsDialogtest:: ExecuteLD returned with err: %d", err);
                }
            }
            
        err = ( err != KErrNone ? err : iError );

        if( iTimer )
            {
            iTimer->CancelTimer();
            }
        
        }
    
    VCXLOGLO1("<<<Cui_FileDetailsDialogtest::ExecuteLDFromDetails");
    return err;
    }

// ---------------------------------------------------------------------------
// Cui_FileDetailsDialogtest::CreateNewFileDetails
// ---------------------------------------------------------------------------
//
TInt Cui_FileDetailsDialogtest::CreateNewFileDetails(CStifItemParser& aItem)
    {
    VCXLOGLO1(">>>Cui_FileDetailsDialogtest::CreateNewFileDetails");
    
    TInt err = KErrNone;
    TPtrC string;

    if ( (err = aItem.GetNextString(string) ) == KErrNone)
        {
        do
            {
            TBuf<120> KMediaFilePath;
            TBuf<120> KMediaFileName;

            KMediaFilePath.Append( Kui_FileDetailsDialogtest_videoTestPath );

            delete iMPFileDetails;
            iMPFileDetails = NULL;

            iMPFileDetails = new (ELeave) CMPFileDetails();
            iMPFileDetails->iFilePath = KMediaFilePath.AllocL();
            iMPFileDetails->iFileName = string.AllocL();
            }
        while (aItem.GetNextString(string) == KErrNone);
        }
    
    VCXLOGLO1("<<<Cui_FileDetailsDialogtest::CreateNewFileDetails");
    return err;
    }

// -----------------------------------------------------------------------------
//  Cui_FileDetailsDialogtest::AddPointerEventL
// -----------------------------------------------------------------------------
//
TInt Cui_FileDetailsDialogtest::AddPointerEventL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>Cui_FileDetailsDialogtest::AddPointerEventL");
    
    TInt err( KErrNone );
    
    TInt delay;
    User::LeaveIfError( aItem.GetNextInt( delay ) );
    
    TInt type;
    User::LeaveIfError( aItem.GetNextInt( type ) );
    
    TInt x;
    User::LeaveIfError( aItem.GetNextInt( x ) );
    
    TInt y;
    User::LeaveIfError( aItem.GetNextInt( y ) );
    
    CVcxTestControlEvent* event = CVcxTestControlEvent::NewL();
    event->SetPointerEvent( delay*1000, static_cast<TPointerEvent::TType>(type), TPoint(x, y) );
    
    iControlEventQueue->AddControlEvent( event );
    
    VCXLOGLO1("<<<Cui_FileDetailsDialogtest::AddPointerEventL");
    return err;
    }

// -----------------------------------------------------------------------------
//  Cui_FileDetailsDialogtest::AddKeyEventL
// -----------------------------------------------------------------------------
//
TInt Cui_FileDetailsDialogtest::AddKeyEventL( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>Cui_FileDetailsDialogtest::AddKeyEventL");
    
    TInt err( KErrNone );
    
    TInt delay;
    User::LeaveIfError( aItem.GetNextInt( delay ) );
    
    TInt keyCodeMapping;
    User::LeaveIfError( aItem.GetNextInt( keyCodeMapping ) );
    
    TStdScanCode keyCode( EStdKeyNull );
    switch( keyCodeMapping )
        {
        case TFileDetailsTestKeyCode_EKeyDevice0:
            {
            keyCode = EStdKeyDevice0; 
            }
            break;
         
        case TFileDetailsTestKeyCode_EKeyDevice1:
            {
            keyCode = EStdKeyDevice1; 
            }
            break;
            
        default:
            break;
        }
    
    TInt eventCode;
    User::LeaveIfError( aItem.GetNextInt( eventCode ) );
    
    CVcxTestControlEvent* event = CVcxTestControlEvent::NewL();
    event->SetKeyEvent( delay*1000, 
            keyCode, static_cast<TRawEvent::TType>( eventCode ) );
    
    iControlEventQueue->AddControlEvent( event );
    
    VCXLOGLO1("<<<Cui_FileDetailsDialogtest::AddKeyEventL");
    return err;
    }

// -----------------------------------------------------------------------------
//  Cui_FileDetailsDialogtest::EnableControlEvents
// -----------------------------------------------------------------------------
//
TInt Cui_FileDetailsDialogtest::EnableControlEvents( CStifItemParser& aItem )
    {
    VCXLOGLO1(">>>Cui_FileDetailsDialogtest::EnableControlEvents");
    
    TInt err( KErrNone );
    
    TInt delay;
    User::LeaveIfError( aItem.GetNextInt( delay ) );
      
    iControlEventQueue->EnableControlEvents( delay*1000 );
    iControlEventsEnabled = ETrue;
    
    VCXLOGLO1("<<<Cui_FileDetailsDialogtest::EnableControlEvents");
    return err;
    }

// -----------------------------------------------------------------------------
//  Cui_FileDetailsDialogtest::DisableControlEvents
// -----------------------------------------------------------------------------
//
TInt Cui_FileDetailsDialogtest::DisableControlEvents( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>Cui_FileDetailsDialogtest::DisableControlEvents");
    
    TInt err( KErrNone );    
    iControlEventQueue->DisableControlEvents();
    iControlEventsEnabled = EFalse;
    
    VCXLOGLO1("<<<Cui_FileDetailsDialogtest::DisableControlEvents");
    return err;
    }

// -----------------------------------------------------------------------------
//  Cui_FileDetailsDialogtest::ResetControlEvents
// -----------------------------------------------------------------------------
//
TInt Cui_FileDetailsDialogtest::ResetControlEvents( CStifItemParser& /* aItem */ )
    {
    VCXLOGLO1(">>>Cui_FileDetailsDialogtest::ResetControlEvents");
    TInt err( KErrNone );
    iControlEventQueue->ResetControlEvents();    
    VCXLOGLO1("<<<Cui_FileDetailsDialogtest::ResetControlEvents");    
    return err;
    }

// -----------------------------------------------------------------------------
//  Cui_FileDetailsDialogtest::LightStatusChanged
// -----------------------------------------------------------------------------
//
void Cui_FileDetailsDialogtest::LightStatusChanged(TInt /* aTarget */, CHWRMLight::TLightStatus /* aStatus */ )
    {
    
    }   

// -----------------------------------------------------------------------------
//  Cui_FileDetailsDialogtest::TimerComplete
// -----------------------------------------------------------------------------
//
void Cui_FileDetailsDialogtest::TimerComplete( TInt aTimerId, TInt aError )
    {
    // Calls to CMPFileDetailsDialog are blocking.
    if( iMPFileDetailsDialog )
        {
        VCXLOGLO1("Cui_FileDetailsDialogtest:: Timeout!");
        iMPFileDetailsDialog->Close();
        iError = KErrTimedOut;
        }
    }   

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
