/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Implementation of the Live TV timer class.*
*/




// INCLUDE FILES
#include "CIptvLiveUIObjectTimer.h"		// Header for this class
#include "MIptvLiveUITimerObserver.h"	// MIptvLiveUITimerObserver
#include "IptvLiveLogger.h" 			// Live TV logging macros


// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIptvLiveUIObjectTimer::CIptvLiveUIObjectTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// (Other items are commented in header)
// -----------------------------------------------------------------------------
//
CIptvLiveUIObjectTimer::CIptvLiveUIObjectTimer( 
									MIptvLiveUITimerObserver* aObserver ) :
    CTimer( CActive::EPriorityLow ), iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIObjectTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// (Other items are commented in header)
// -----------------------------------------------------------------------------
// 
void CIptvLiveUIObjectTimer::ConstructL()
    {        
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIObjectTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvLiveUIObjectTimer* CIptvLiveUIObjectTimer::NewL( 
										MIptvLiveUITimerObserver* aObserver )
    {
    CIptvLiveUIObjectTimer* self = 
							new ( ELeave ) CIptvLiveUIObjectTimer( aObserver );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIObjectTimer::~CIptvLiveUIObjectTimer
// Destructor. Frees reserved resources
// -----------------------------------------------------------------------------
//
EXPORT_C CIptvLiveUIObjectTimer::~CIptvLiveUIObjectTimer()
    {
    Cancel();
	iObserver = NULL;
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIObjectTimer::StartTimer
// -----------------------------------------------------------------------------
//
EXPORT_C void CIptvLiveUIObjectTimer::StartTimer( const TTime& aTime )
    {
	LIVE_TV_TRACE1(_L("CIptvLiveUIObjectTimer::StartTimer() in"));
    Cancel();
    At( aTime );
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIObjectTimer::RunL
// From CActive
// -----------------------------------------------------------------------------
//
void CIptvLiveUIObjectTimer::RunL()
    {
    if ( iStatus == KErrNone )
        {
        iObserver->TimerFires();
        }
    else
        {
		LIVE_TV_TRACE2(_L("CIptvLiveUIObjectTimer::RunL() iStatus: %d"), iStatus.Int());
        iObserver->TimerError( iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIObjectTimer::DoCancel
// From CActive
// -----------------------------------------------------------------------------
//
void CIptvLiveUIObjectTimer::DoCancel()
    {
    LIVE_TV_TRACE1(_L("CIptvLiveUIObjectTimer::DoCancel()"));
	CTimer::DoCancel();       
    }

// -----------------------------------------------------------------------------
// CIptvLiveUIObjectTimer::RunError
// From CActive, called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CIptvLiveUIObjectTimer::RunError( TInt aError )
    {
	LIVE_TV_TRACE2(_L("CIptvLiveUIObjectTimer::RunError() aError: %d"), aError);
	if ( iObserver )
		{
		iObserver->TimerError( aError );	
		}
	
	return KErrNone;
    }

//  End of File
