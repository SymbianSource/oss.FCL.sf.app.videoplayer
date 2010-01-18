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
* Description:    Timer*
*/




#ifndef __CVCXMYVIDEOSTIMER_H__
#define __CVCXMYVIDEOSTIMER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CVcxMyVideosTimer;

// CLASS DECLARATION
/**
*  This class specifies the function to be called when a timeout occurs.
*  Used in conjunction with CVcxTimer class.
*/
class MVcxMyVideosTimerObserver
    {
    public: // New functions

        /**
        * The function to be called when a timeout occurs.
        *
        * @param aTimer Pointer to timer which expired.
        */
        virtual void TimerExpired(CVcxMyVideosTimer* aTimer) = 0;
    };

// CLASS DECLARATION
/**
* This class will notify an object after a specified timeout.
*/
NONSHARABLE_CLASS(CVcxMyVideosTimer) : public CTimer
    {
    public: // Constructors and destructors

        /**
        * Two-phased constructor.
        * Creates a CVcxMyVideosTimer object using two phase construction,
        * and returns a pointer to the created object.
        *
        * @param aPriority      Priority to use for this timer.
        * @param aTimerObserver Object to notify of timeout event.
        * @return               A pointer to the created instance of CVcxMyVideosTimer.
        */
        static CVcxMyVideosTimer* NewL( const TInt aPriority,
            MVcxMyVideosTimerObserver& aTimerObserver );

        /**
        * Two-phased constructor.
        * Creates a CVcxMyVideosTimer object using two phase construction,
        * and returns a pointer to the created object.
        *
        * @param aPriority      Priority to use for this timer.
        * @param aTimerObserver Object to notify of timeout event.
        * @return               A pointer to the created instance of CVcxMyVideosTimer.
        */
        static CVcxMyVideosTimer* NewLC( const TInt aPriority,
            MVcxMyVideosTimerObserver& aTimerObserver );

        /**
        * Destructor.
        * Destroys the object and releases all memory objects.
        */
        virtual ~CVcxMyVideosTimer();

    protected: // Functions from base classes

        /**
        * From CActive, RunL.
        * Called when operation completes.
        */
        void RunL();

    private: // Constructors and destructors

        /**
        * C++ default constructor.
        * Performs the first phase of two phase construction.
        *
        * @param aPriority      Priority to use for this timer.
        * @param aTimeOutNotify An observer to notify.
        */
        CVcxMyVideosTimer( const TInt aPriority,
            MVcxMyVideosTimerObserver& aTimerObserver );

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // Data

        /**
        * The observer for this objects events.
        */
        MVcxMyVideosTimerObserver& iObserver;
    };

#endif // __CVCXMYVIDEOSTIMER_H__

// End of File