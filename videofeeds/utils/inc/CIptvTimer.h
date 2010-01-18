/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __CIPTVTIMER_H__
#define __CIPTVTIMER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MIptvTimerObserver;

// CLASS DECLARATION
/**
* This class will notify an object after a specified timeout.
* @lib IptvUtil.lib
*/
class CIptvTimer : public CTimer
    {
    public: // Constructors and destructors

        /**
        * Two-phased constructor.
        * Creates a CIptvTimer object using two phase construction,
        * and returns a pointer to the created object.
        *
        * @param aPriority      Priority to use for this timer.
        * @param aTimerObserver Object to notify of timeout event.
        * @return               A pointer to the created instance of CIptvTimer.
        */
        IMPORT_C static CIptvTimer* NewL( const TInt aPriority,
            MIptvTimerObserver& aTimerObserver );

        /**
        * Two-phased constructor.
        * Creates a CIptvTimer object using two phase construction,
        * and returns a pointer to the created object.
        *
        * @param aPriority      Priority to use for this timer.
        * @param aTimerObserver Object to notify of timeout event.
        * @return               A pointer to the created instance of CIptvTimer.
        */
        IMPORT_C static CIptvTimer* NewLC( const TInt aPriority,
            MIptvTimerObserver& aTimerObserver );

        /**
        * Destructor.
        * Destroys the object and releases all memory objects.
        */
        virtual IMPORT_C ~CIptvTimer();

    protected: // Functions from base classes

        /**
        * From CActive, RunL.
        * Called when operation completes.
        */
        IMPORT_C void RunL();

    private: // Constructors and destructors

        /**
        * C++ default constructor.
        * Performs the first phase of two phase construction.
        *
        * @param aPriority      Priority to use for this timer.
        * @param aTimeOutNotify An observer to notify.
        */
        IMPORT_C CIptvTimer( const TInt aPriority,
            MIptvTimerObserver& aTimerObserver );

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        IMPORT_C void ConstructL();

    private: // Data

        /**
        * iNotify, the observer for this objects events.
        */
        MIptvTimerObserver& iObserver;
    };

#endif // __CIPTVTIMER_H__

// End of File