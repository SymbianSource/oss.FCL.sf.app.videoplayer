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
* Description:    Listens events from server (CIptvEventGenerator).*
*/





#ifndef CIPTVEVENTLISTENER_H
#define CIPTVEVENTLISTENER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvClientBase;
class MIptvEvent;

// CLASS DECLARATION

/**
 * Listens events on client side. Events are sent by CIptvEventGenerator on server side.
 * This is a general purpose event system. Events can be anything, they just have to
 * inherit MIptvEvent virtual class.
 */
class CIptvEventListener : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aClient             Reference to client instance
        * @param aEventProto         Pointer to MIptvEvent derived class. This defines the event
        *                            type which this event listener listens to. Pointer contains function
        *                            implementations which are used when handling MIptvEvent type
        *                            objects. Ownership moves, i.e CIptvEventListener will free the pointer.
        * @param aSizeReqMsgId       Message ID which is used when client side requests incoming event size.
        * @param aDataMsgId          Message ID which is used when client side queries data (syncronously).
        * @param aCancelSizeReqMsgId Message ID which is used when client size cancels pending size request.
        */
        static CIptvEventListener* NewL( CIptvClientBase& aClient,
                                         MIptvEvent* aEventProto,
                                         TInt aSizeReqMsgId,
                                         TInt aDataMsgId,
                                         TInt aCancelSizeReqMsgId );
        
        /**
        * Destructor.
        */
        virtual ~CIptvEventListener();

    private:

        /**
        * From CActive, handles an active object’s request completion event
        */
        void RunL();

        /**
        * From CActive, implements cancellation of an outstanding request.
        * This function is called as part of the active object’s Cancel().
        */
        void DoCancel();
                    
    private:

        /**
        * C++ default constructor.
        *
        * @param aClient             Reference to client instance
        * @param aEventProto         Pointer to MIptvEvent derived class. This defines the event
        *                            type which this event listener listens to. Pointer contains function
        *                            implementations which are used when handling MIptvEvent type
        *                            objects. Ownership moves, i.e CIptvEventListener will free the pointer.
        * @param aSizeReqMsgId       Message ID which is used when client side requests incoming event size.
        * @param aDataMsgId          Message ID which is used when client side queries data (syncronously).
        * @param aCancelSizeReqMsgId Message ID which is used when client size cancels pending size request.
        */
        CIptvEventListener( CIptvClientBase& aClient,
                            MIptvEvent* aEventProto,
                            TInt aSizeReqMsgId,
                            TInt aDataMsgId,
                            TInt aCancelSizeReqMsgId );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        void SendSizeReqL();

    private:    // Data

        CIptvClientBase&  iClient;

        /**
         * Client provides this. It is used to make new instances of the event (GetCopy() method).
         */                
        MIptvEvent* iEventProto;
        
        /**
         * Memory area allocated for size request ipc msg.
         */
        TBuf8<4> iSizeRequestIpcMsg;
                
        /**
         * Pending message id is stored when the request is made.
         */        
        TInt    iPendingMsgId;

        /**
         * If size request is completed with error code (for example server side leaved)
         * this value is increased and new size request is sent.
         */        
        TInt    iErrorCounter;
        
        /**
        * Message ID which is used when client side requests incoming event size.
        */
        TInt iSizeReqMsgId;
        
        /**
        * Message ID which is used when client side queries data (syncronously).
        */
        TInt iDataMsgId;

        /**
        * Message ID which is used when client size cancels pending size request.
        */        
        TInt iCancelSizeReqMsgId;

    };

#endif      // CIPTVSMEVENTLISTENER_H   
            
// End of File
