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
* Description:    Header file for CVcxViewMessageUtility.*
*/




#ifndef VCXVIEWMESSAGEUTILITY_H
#define VCXVIEWMESSAGEUTILITY_H

#include <coemain.h>
#include <e32base.h>

#include "mtvobserverutils.inl"

/**
 * Abstract class for observing custom view messages.
 */
class MVcxViewMessageObserver
    {
    public:
        
        /**
         * Handles a custom view message.
         * @param aMessageUid    UID for custom message
         * @param aCustomMessage Message contents
         */
        virtual void HandleCustomViewMessageL( 
            const TUid aMessageUid, const TDesC8 &aCustomMessage ) = 0;
    };

/**
 * Message sending component for MPX views. 
 * Creates a singleton object that will be stored by CCoeEnv.
 */
class CVcxViewMessageUtility : public CCoeStatic 
    {
    public:
        
        /**
         * First call initializes the singleton object. Subsequent calls return
         * created instance.
         * @return singleton object instance.
         */
        IMPORT_C static CVcxViewMessageUtility* InstanceL();

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CVcxViewMessageUtility();

        /**
         * Add observer.
         * @param aObserver Observer.
         */
        IMPORT_C void AddObserverL( MVcxViewMessageObserver& aObserver );

        /**
         * Remove observer.
         * @param aObserver Observer.
         */
        IMPORT_C void RemoveObserver( MVcxViewMessageObserver& aObserver );

        /**
         * Sends message to all registered observers.
         * @param aMessageUid    UID for custom message
         * @param aCustomMessage Message contents
         */
        IMPORT_C void SendCustomViewMessageL( 
            const TUid aMessageUid, const TDesC8 &aCustomMessage );
        
    private:

        /**
         * Default constructor is private because this class is a singleton.
         */
        CVcxViewMessageUtility();

    private: // Data

        /**
         * Observer array.
         */
        MtvObserverUtils::RObserverData<MVcxViewMessageObserver> iObservers;
    };

#endif // VCXVIEWMESSAGEUTILITY_H

