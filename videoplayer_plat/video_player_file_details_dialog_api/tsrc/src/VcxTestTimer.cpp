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


// INCLUDE FILES
#include "VcxTestTimer.h"

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
// CVcxTestTimer::
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVcxTestTimer* CVcxTestTimer::NewL(MVcxTestTimerObserver& aObserver, TInt aId)
    {
    CVcxTestTimer* self = new (ELeave) CVcxTestTimer(aObserver, aId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::~CVcxTestTimer
// destructor
// -----------------------------------------------------------------------------
//
CVcxTestTimer::~CVcxTestTimer()
    {
    iRestartAfterCancel = EFalse;
    Cancel();
    iTimer.Close();

   	if( IsAdded() )
        {
        Deque(); // calls also Cancel()
        }
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::~CVcxTestTimer
// destructor
// -----------------------------------------------------------------------------
//
void CVcxTestTimer::CancelTimer()
    {
    if( iIsRunning )
    	{
    	iRestartAfterCancel = EFalse;
    	iIsRunning = EFalse;
    	DoCancel( );
    	}
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::CVcxTestTimer
//
// -----------------------------------------------------------------------------
//
CVcxTestTimer::CVcxTestTimer(MVcxTestTimerObserver& aObserver, TInt aId) : CActive(EPriorityStandard), iObserver(aObserver),
	iIsRunning(EFalse), iId(aId)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::ConstructL
//
// -----------------------------------------------------------------------------
//
void CVcxTestTimer::ConstructL()
    {
    User::LeaveIfError(iTimer.CreateLocal());
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::After
//
// -----------------------------------------------------------------------------
//
void CVcxTestTimer::After(TTimeIntervalMicroSeconds32 aInterval)
    {
    iInterval = aInterval;

    if( !IsActive() )
    	{
	    iTimer.After(iStatus, aInterval);
	    iIsRunning = ETrue;
	    SetActive();
	    iRestartAfterCancel = EFalse;
    	}
	else
		{
		// Cancel and start again when cancel is done.
    	CancelTimer();
    	iRestartAfterCancel = ETrue;
		}
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::RunL
//
// -----------------------------------------------------------------------------
//
void CVcxTestTimer::RunL()
    {
    TInt status = iStatus.Int();

    if( iIsRunning )
        {
        iObserver.TimerComplete( iId, status );
        }

    if( status == KErrCancel && iRestartAfterCancel )
        {
        iRestartAfterCancel = EFalse;
        iTimer.After( iStatus, iInterval );
        iIsRunning = ETrue;
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::DoCancel
//
// -----------------------------------------------------------------------------
//
void CVcxTestTimer::DoCancel()
    {
    iTimer.Cancel();
    iIsRunning = EFalse;
    }

//  End of File
