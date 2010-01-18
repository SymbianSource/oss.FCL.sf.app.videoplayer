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
* Description:    Listens VOD DL events*
*/





#ifndef CIPTVVODDLEVENTLISTENER_H
#define CIPTVVODDLEVENTLISTENER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvVodDlClient;

// CLASS DECLARATION

/**
 * Listens events from VOD DL Manager on client side.
 */
class CIptvVodDlEventListener : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIptvVodDlEventListener* NewL(CIptvVodDlClient& aClient, TUint32 aClientId);
        
        /**
        * Destructor.
        */
        virtual ~CIptvVodDlEventListener();

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

    public:        
                    
    private:

        /**
        * C++ default constructor.
        */
        CIptvVodDlEventListener(CIptvVodDlClient& aClient, TUint32 aClientId);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        void SendSizeReqL();

    public:     // Data
    
    protected:  // Data

    private:    // Data

        CIptvVodDlClient& iClient;
        TUint32           iClientId;

        /**
         * Heap object to reserve memory for IPC messaging.
         */
        HBufC8* iIpcMsg;
        
        /**
         * Passed to RIptvClientSession object to store the sent and received data.
         * Points to iIpcMsg heap object.
         */
        TPtr8   iIpcMsgPtr;
        
        TBuf8<4> iSizeRequestIpcMsg;
        
        /**
         * Response message id is stored when the request is made. That way
         * it is not necessarry to convey msg id through IPC.
         */        
        TInt    iResponseMsgId;

        /**
         * If size request is completed with error code (for example server side leaved)
         * this value is increased and new size request is sent.
         */        
        TInt    iErrorCounter;

    };

#endif      // CIPTVVODDLEVENTLISTENER_H   
            
// End of File
