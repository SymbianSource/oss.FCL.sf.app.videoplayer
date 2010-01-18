/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class to handle subscribtions from PS*
*/




#ifndef __VCXNS_CONTENTHARVERSTERPSSUBSCRI_H
#define __VCXNS_CONTENTHARVERSTERPSSUBSCRI_H

#include <e32base.h>	
#include <e32property.h>

NONSHARABLE_CLASS ( MCHPSObserver )
    {
    public:
        virtual void ValueChanged( const TUint32& aKey, const TInt& aValue ) = 0;  
    
    };

 
NONSHARABLE_CLASS ( CVcxNsCHPSSubscriber ) : public CActive
    {

    public: // constuct / destruct
    
        /**
         * Construction.
         * 
         * @param aUid      category for the property
         * @param aKey      key id for the property
         * @param aType     type of the key (at the moment only int -type is supported)
         * @param aObserver observer to who to notify about changes
         * 
         * @return The newly constructed CVcxConnUtilPubSub
         */
        static CVcxNsCHPSSubscriber* NewL( const TUid aUid, 
                                           const TUint32 aKey,
                                           RProperty::TType aType,
                                           MCHPSObserver* aObserver );
        
        /**
         * Destructor.
         *
         */
        virtual ~CVcxNsCHPSSubscriber();
        
        /**
         * Returns a pubsub value from this property
         * 
         *@param &aValue value to fetch
         * 
         *@return TInt KErrNone or some system level error code
         */
        TInt Get( TInt& aValue );
            
        /**
         * Start subscribing if not already started
         */
        void Start();
        
        
    private:  // constuct / destruct
        
        /**
         * Constructor
         *
         */
        CVcxNsCHPSSubscriber( const TUid aUid, 
                              const TUint32 aKey, 
                              RProperty::TType aType, 
                              MCHPSObserver* aObserver );
        
        /**
         * Symbian 2nd phase construction
         *   
         */
        void ConstructL();
     
    private: // from CActive
        
        /**
         * From CActive. Calles when value subscribed to changes 
         * 
         */  
        void RunL();
        
        /**
         * From CActive. Cancels subscribtion
         * 
         */
        void DoCancel();
        
    private:
        
        /**
         * ETrue if object is initialized and subscribtion starts
         */
        TBool            iInitialized;
        
        /**
         * Property to subscribe
         */
        RProperty        iProperty;
        
        /**
         * Category uid for the property
         */
        const TUid       iUid;
        
        /**
         * Key id for the property
         */
        const TUint32    iKey;
        
        /**
         * type of the property
         */
        RProperty::TType iKeyType;
        
        /**
         * observer, not owned
         */
        MCHPSObserver*   iObserver;

    };      

#endif // __VCXNS_CONTENTHARVERSTERPSSUBSCRI_H
