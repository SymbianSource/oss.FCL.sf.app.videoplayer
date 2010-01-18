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
* Description:    Observer for completed total video length requests.*
*/




#ifndef CIPTVMYVIDEOSTOTALVIDEOLENGTHOBSERVER_H
#define CIPTVMYVIDEOSTOTALVIDEOLENGTHOBSERVER_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

// FORWARD DECLARATIONS
class MIptvMyVideosClientObserver;
class RIptvClientSession;

/**
 * Observer for completed total video length requests.
 *
 * @lib IptvClientApi.dll
 */
class CIptvMyVideosTotalVideoLengthObserver : public CActive
    {
    public:

        /**
         * Two-phased constructor.
         *
         * @param aClientObserver Observer.
         * @param aSession        Server client session.         
         */
        static CIptvMyVideosTotalVideoLengthObserver* NewL(MIptvMyVideosClientObserver& aClientObserver, RIptvClientSession& aSession);

        /**
         * Destructor.
         */ 
        ~CIptvMyVideosTotalVideoLengthObserver();
        
        /**
         * Sets this object active. At the same time the total video length request
         * is sent so request will be completed when total video length is counted.
         */
        void SetActiveL();
        
    public: // From CActive
        
        /**
         * Called when request completes.
         */        
        void RunL();
        
        /**
         * Called when request needs to be cancelled.
         */        
        void DoCancel();
        
    protected:

        /**
         * Constructor.
         *
         * @param aClientObserver Observer.
         * @param aSession        Server client session.         
         */
        CIptvMyVideosTotalVideoLengthObserver(MIptvMyVideosClientObserver& aClientObserver, RIptvClientSession& aSession);
        
        /**
         * EPOC constructor.
         */
        void ConstructL();
        
    protected: // Data
        
        MIptvMyVideosClientObserver& iClientObserver;
        RIptvClientSession&          iSession;
        HBufC8*                      iMsg;
        TPtr8                        iMsgPtr;        
    };

#endif // CIPTVMYVIDEOSTOTALVIDEOLENGTHOBSERVER_H

// End of File
