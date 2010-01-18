/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    General purpose timer*
*/





// INCLUDE FILES
#include "CIptvTimer.h"
#include "MIptvTimerObserver.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CIptvTimer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTimer* CIptvTimer::NewL(const TInt aPriority,
                                      MIptvTimerObserver& aTimerObserver)
    {
    CIptvTimer* self = CIptvTimer::NewLC(aPriority, aTimerObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvTimer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTimer* CIptvTimer::NewLC(const TInt aPriority,
                                       MIptvTimerObserver& aTimerObserver)
    {
    CIptvTimer* self = new ( ELeave ) CIptvTimer(aPriority, aTimerObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvTimer::CIptvTimer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTimer::CIptvTimer(const TInt aPriority,
                                MIptvTimerObserver& aTimerObserver)
: CTimer( aPriority ), iObserver( aTimerObserver )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CIptvTimer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIptvTimer::~CIptvTimer()
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvTimer::~CIptvTimer()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CIptvTimer::RunL()
// Called when operation completes.
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvTimer::RunL()
    {
    // Timer request has completed, so notify the timer's owner
    if ( iStatus == KErrNone )
        {
        iObserver.TimerExpired(this);
        }
    }

// End of File
