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
* Description:    Generates events on server side, CIptvEventListener listens them on*
*/





#ifndef CIPTVEVENTGENERATOR_H
#define CIPTVEVENTGENERATOR_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvEvents;
class MIptvEvent;

// CLASS DECLARATION

/**
 * Generates events on server process, CIptvEventListener receives them in client process.
 */
class CIptvEventGenerator : public CBase
    {
    public:
        enum TState
            {
            EIdle = 0,
            ESizeMsgPending,
            ESizeMsgCompleted,
            EDataMsgPending,
            EDataMsgCompleted
            };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIptvEventGenerator* NewL(MIptvEvent* aEventProto);
        
        /**
        * Destructor.
        */
        virtual ~CIptvEventGenerator();

    private:

        /**
        * C++ default constructor.
        */
        CIptvEventGenerator(MIptvEvent* aEventProto);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Sends size resp to client.
        */    
        void SendSizeRespL();

    public:
    
        /**
         * Called by message handler to inform client about event.
         */
        void AddEventL(MIptvEvent& aEvent);

        /**
         * Client calls (routed through message handler) this to request event size.
         */
        void SizeRequestL(const RMessage2& aSizeRequest);
        
        /**
         * Client calls this (routed through message handler) to request event data,
         * aDataRequest is completed immedetially.
         * @param aDataRequest
         */
        void DataRequestL(const RMessage2& aDataRequest);
         
    private:     // Data
            
        //RMessage2 for size request is stored here
        RMessage2 iMessage;
        
        TState    iState;
        
        //iEventsOutgoing is filled from iEventsIncoming when size req is completed
        CIptvEvents* iEventsOutgoing;
        
        //all new events are put in here
        CIptvEvents* iEventsIncoming;
          
        MIptvEvent* iEventProto;       
    };

#endif      // CIPTVEVENTGENERATOR_H   
            
// End of File
