/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef CIPTVEPGMANAGERUPDATER_H
#define CIPTVEPGMANAGERUPDATER_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MIptvEpgManagerUpdateObserver;

// CLASS DECLARATION
class CIptvEpgManagerUpdater : public CActive
    {
    public:
        /**
         * Two-phased constructor.
         * @param 
         */
        IMPORT_C static CIptvEpgManagerUpdater* NewL(MIptvEpgManagerUpdateObserver& aObserver);

        /**
         * Destructor.
         */
        virtual ~CIptvEpgManagerUpdater();

    public:
        /**
         * Start.
         */
        void Start();

        /**
         * Stop.
         */
        void Stop();

        /**
         * Handles an active object’s request completion event.
         */
        void RunL();

        /**
         * Implements cancellation of an outstanding request.
         */
        void DoCancel();
                                
    private: // New functions           
        /**
        * C++ default constructor.
        */
        CIptvEpgManagerUpdater(MIptvEpgManagerUpdateObserver& aObserver);

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();
                              
    private: // Data members        
        /**
        * Reference to MIptvEpgManagerUpdateObserver.
        */
        MIptvEpgManagerUpdateObserver& iObserver;

        /**
        * Timer.
        */
        RTimer iTimer;
    };

#endif  // CIPTVEPGMANAGERUPDATER_H

// End of File
