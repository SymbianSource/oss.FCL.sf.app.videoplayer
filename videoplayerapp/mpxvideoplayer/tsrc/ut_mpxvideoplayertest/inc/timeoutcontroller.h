/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Simple player for preemption purposes
*
*/

// Version : %version: ou1cpsw#2 %



#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>


class MTimeoutObserver
{
    public: // New functions

        virtual void HandleTimeout(TInt error) = 0;
};


_LIT(KFormatTimeStamp, "%:0%J%:1%T%:2%S%.%*C4%:3%+B");


class CSimpleTimeout : public CActive
{
    public:     // Constructors and destructor

        static CSimpleTimeout* NewL( MTimeoutObserver* aObserver );

        ~CSimpleTimeout();

    public:     // New functions

        void Start( TTimeIntervalMicroSeconds aTimeout );

        void Stop();


    public:     // Functions from base classes

        void RunL();

        void DoCancel();

        TInt RunError( TInt aError );

    private:

        void ConstructL( MTimeoutObserver* aObserver );
        CSimpleTimeout();


    private:    // Data
        MTimeoutObserver*   iObserver;
        RTimer              iTimer;
        TTime               iTestCaseTimeout;
};
