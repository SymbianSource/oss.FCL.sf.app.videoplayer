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
* Description:    CTimer based timer class for Live TV application.*
*/




#ifndef __CIPTV_LIVE_UI_OBJECT_TIMER_H
#define __CIPTV_LIVE_UI_OBJECT_TIMER_H

// INCLUDES
#include <e32base.h>		// CTimer 
// FORWARD DECLARATIONS
class MIptvLiveUITimerObserver;

// CLASS DECLARATION
/**
*  Live TV timer class derived from CTimer.
*/
class CIptvLiveUIObjectTimer : public CTimer
    {
  
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
		* @param aObserver Timer observer.
        */
        IMPORT_C static CIptvLiveUIObjectTimer* NewL( 
									MIptvLiveUITimerObserver* aObserver );
                        
        /**
        * Virtual destructor.
        */
        IMPORT_C virtual ~CIptvLiveUIObjectTimer();

    public:      // New functions
        
        /**
        * Start a timer.
        * @param aTime Time when timer fires.
        */
        IMPORT_C void StartTimer( const TTime& aTime );
        

    public:      // Functions from base classes
        
		/**
        * From CActive.
		* Handles an active object's request completion event.
        */
		void RunL();

		/**
        * From CActive.
		* Implements cancellation of an outstanding request.
        */
		void DoCancel();

		/**
        * From CActive.
		* Handles a leave occurring in the request completion event handler
		* RunL().
		* @param aError Error code.
		* @return KErrNone.
        */
		TInt RunError( TInt aError );

    private:	// Constructors

        /**
        * Private C++ constructor.
		* @param aObserver Timer observer.
        */
        CIptvLiveUIObjectTimer( MIptvLiveUITimerObserver* aObserver );             

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
 

    private:	// Data
    
		/**
		* Timer observer.
		*/
        MIptvLiveUITimerObserver* iObserver;

    };

#endif // __CIPTV_LIVE_UI_OBJECT_TIMER_H

// End of File
