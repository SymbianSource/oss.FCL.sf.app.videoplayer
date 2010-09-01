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




#ifndef CIPTVEPGVODMSQQUEUE_H
#define CIPTVEPGVODMSQQUEUE_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include "MIptvTimerObserver.h"

// FORWARD DECLARATIONS
class CIptvTimer;

class TIptvEpgVodMsg
    {
    public:        
        TUint32 iValue;
        TInt iInfo;
    };

// CLASS DECLARATION
class CIptvEpgVodMsqQueue : public CBase, public MIptvTimerObserver
    {
    public:

        /**
         * Two-phased constructor.
         * @param 
         */
        static CIptvEpgVodMsqQueue* NewL();

        /**
         * Destructor.
         */
        ~CIptvEpgVodMsqQueue();

        /**
         * Set request.
         */
        void SetRequest(const RMessage2& aMessage);

        /**
         * Cancel request.
         */
        void CancelRequest();
        
        /**
         * Empty message queue.
         */
        void CancelMsgQueue();

        /**
        * Send message from EPG manager to client.
        * @since            Engine 1.1
        * @param aMsg       Message         
        * @param aInfo      Optional info parameter.       
        */
        void SendMessageToClientL(TInt aMsg, TInt aInfo);     
       
    public: // From MIptvTimerObserver

        /**
        * Timer expired.
        */
        void TimerExpired(CIptvTimer* aTimer);
                                                               
    private:    // New functions

        /**
        * C++ default constructor.
        */
        CIptvEpgVodMsqQueue();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        void ConstructL();
        
        void SetTimer();
        void SendMessageL(TIptvEpgVodMsg& aMsg);
                                                       
    private:    // Data members         
        RArray<TIptvEpgVodMsg> iMsgQueue;
        RMessage2 iObserverMessage;
        TBool iRequestPending;
        CIptvTimer* iTimer;
        TInt iCounter;
    };

#endif  // CIPTVEPGVODMSQQUEUE_H

// End of File
