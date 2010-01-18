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


#ifndef FUSIONMPXPLAYERTESTCMDLISTENER_H
#define FUSIONMPXPLAYERTESTCMDLISTENER_H


//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32msgqueue.h>
#include "VCXTestPSObserver.h"

#include "FusionMpxPlayerTester.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CVCXTestPSSubscriber;

class MFusionMpxPlayerTestShutdownObserver
    {
public:

    virtual void ShutdownCommandReceived() = 0;

    };

/**
*  @since
*/
class CFusionMpxPlayerTestCmdListener : public CBase, 
                                        public MVCXTestPSObserver,
                                        public MFusionMpxPlayerTesterObserver
                                        
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CFusionMpxPlayerTestCmdListener * NewL( TUint aProcessId, MFusionMpxPlayerTestShutdownObserver* aObserver );

        /**
        * Destructor.
        */
        virtual ~CFusionMpxPlayerTestCmdListener();

    public: // New functions
        
        TInt PlayLocalVideo( const TDesC& aFileName );
        
    private: // New functions
        
        void SendResponse( const TInt aResponse, TInt aError );
        
    private: // From base classes

        void ValueChangedL( const TUid& aUid, const TUint32& aKey, const TInt& aValue );

        void ValueChangedL( const TUid& aUid, const TUint32& aKey, const TDesC& aValue );

        void TesterCommandComplete();
        
    private:

        /**
        * C++ default constructor.
        */
        CFusionMpxPlayerTestCmdListener();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TUint aProcessId, MFusionMpxPlayerTestShutdownObserver* aObserver );

    private:    // Data
        CFusionMpxPlayerTester* iTester;

        CVCXTestPSSubscriber* iGlobalShutdownSubcriber;

        CVCXTestPSSubscriber* iCmdSubcriber;

        TInt iPSKeyBase;

        MFusionMpxPlayerTestShutdownObserver* iObserver;
    };

#endif // FUSIONMPXPLAYERTESTCMDLISTENER_H

// End of File
