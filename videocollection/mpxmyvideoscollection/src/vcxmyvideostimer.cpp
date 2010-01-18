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
* Description:    General purpose timer*
*/





// INCLUDE FILES
#include "vcxmyvideostimer.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CVcxMyVideosTimer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVcxMyVideosTimer* CVcxMyVideosTimer::NewL(const TInt aPriority,
                                      MVcxMyVideosTimerObserver& aTimerObserver)
    {
    CVcxMyVideosTimer* self = CVcxMyVideosTimer::NewLC(aPriority, aTimerObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosTimer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVcxMyVideosTimer* CVcxMyVideosTimer::NewLC(const TInt aPriority,
                                       MVcxMyVideosTimerObserver& aTimerObserver)
    {
    CVcxMyVideosTimer* self = new ( ELeave ) CVcxMyVideosTimer(aPriority, aTimerObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosTimer::CVcxMyVideosTimer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CVcxMyVideosTimer::CVcxMyVideosTimer(const TInt aPriority,
                                MVcxMyVideosTimerObserver& aTimerObserver)
: CTimer( aPriority ), iObserver( aTimerObserver )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosTimer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVcxMyVideosTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosTimer::~CVcxMyVideosTimer()
// Destructor.
// -----------------------------------------------------------------------------
//
CVcxMyVideosTimer::~CVcxMyVideosTimer()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosTimer::RunL()
// Called when operation completes.
// -----------------------------------------------------------------------------
//
void CVcxMyVideosTimer::RunL()
    {
    // Timer request has completed, so notify the timer's owner
    if ( iStatus == KErrNone )
        {
        iObserver.TimerExpired( this );
        }
    }

// End of File
