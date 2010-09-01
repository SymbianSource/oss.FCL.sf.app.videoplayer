/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef VCXMEDIATOREVENTWATCHER_H
#define VCXMEDIATOREVENTWATCHER_H

//  INCLUDES
#include <e32base.h>
#include <mediatoreventconsumer.h>

#include "VCXTestMyVideosObserver.h"
#include "VCXTestMessageWait.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CVCXTestMyVideosCollectionClient;

// DATA TYPES

// CLASS DECLARATION

/**
*  CVCXMediatorEventWatcher class.
* 
* Looks after mediator events from mpx my videos collection plugin. 
* Monitoring starts right at creation time.
*
* KVcxNsMpxEventDownloadStarted 
* KVcxNsMpxEventDownloadCompleted
* KVcxNsMpxEventAllDownloadsEnded
*  - these are checked with counter, messages from collection client increase 
*    counters and mediator events decrease them.   
*  
* KVcxNsMpxEventNewVideosCountDecreased
*  - checked by refreshing collection and count of new videos.   
* 
*
*  @lib ?library
*  @since ?Series60_version
*/
class CVCXMediatorEventWatcher : public CBase,
                                 public MVCXTestMyVideosObserver,
                                 public MVCXTestMessageWaitObserver,
                                 public MMediatorEventObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVCXMediatorEventWatcher* NewL();

        /**
        * Destructor.
        */
        ~CVCXMediatorEventWatcher();

    public: // Functions from base classes
        
        /**
         * Received when there's change in mpx my videos plugin contents.
         */
        void VideoEventL( TVCXTestVideoEvent aEvent, TUint32 aParam1, TUint32 aParam2, TInt aError );

        /**
         * Callback for message waiter.
         */
        void MessageWaitComplete( TInt aError );
        
        /**
         * From MMediatorEventObserver
         * 
         * Catches the events
         */
        void MediatorEventL( TUid aDomain, TUid aCategory, TInt aEventId, const TDesC8& aData );       
        
    private:

        /**
        * C++ default constructor.
        */
        CVCXMediatorEventWatcher();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    public: // New functions.

        /**
         * Adds event that is expected to be received before check.
         */
        void AddExpectedEventL( TInt aEvent );
        
        /**
         * Leaves if there has been any error with the events.
         */
        void CheckEventsL();
        
        /**
         * Waits until all expected events have arrived.
         */
        void WaitEventsL( TInt aTimeoutSeconds );

    private:
        
        /**
         * Registers and subscribes for mediator event.
         */
        void RegisterAndSubscribeEventL( TInt aEvent );
        
        /**
         * Deregisters and unsubscribes for all mediator events.
         */
        void UnsubscribeAndDeregisterEvents();
        
        /**
         * Checks if video count has decreased.
         */
        TBool HasVideoCountDecreased( TBool aForceUpdate );
        
    private:    // Data
        
        CMediatorEventConsumer* iMediatorEventConsumer; 
        
        TUid iMediatorDomain;
        TUid iMediatorCategory;
        TVersion iMediatorVersion;
        // Arry of subscribed mediator event IDs.
        RArray<TInt> iSubscribedEvents;
        
        CVCXTestMyVideosCollectionClient* iMyVideosCollectionClient;
        CVCXTestMessageWait* iVideoMessageWait;
        
        CVCXTestMessageWait* iMediatorEventWait;
        
        TInt iDownloadStartedEventCount;
        TInt iDownloadCompletedEventCount;
        TInt iAllDownloadsCompletedEventCount;
        
        TInt iLastNewVideoCount;
        TBool iNewVideoCountHasDecreased;
        
        TInt iError;
        
        RArray<TInt> iExpectedEvents;
    };

#endif      // VCXMEDIATOREVENTWATCHER_H

// End of File
