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
* Description: 
*
*/




#ifndef CIPTVNETWORKSELECTIONMSGHANDLER_H
#define CIPTVNETWORKSELECTIONMSGHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"

#include "IptvClientServerCommon.h"
#include "TIptvGetUsedIapReq.h"
#include "MIptvNetworkSelectionObserver.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIptvServer;

// CLASS DECLARATION

/**
 * Handles Network Selection messages from client process. Uses CIptvNetworkSelection
 * object. This object is instantiated (in CIptvServerSession) if client uses
 * Network Selection API functions from CIptvServiceManagementClient object.
 */
class CIptvNetworkSelectionMsgHandler : public CBase, public MIptvNetworkSelectionObserver
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIptvNetworkSelectionMsgHandler* NewL(CIptvServer& aServer);
        
        /**
        * Destructor.
        */
        virtual ~CIptvNetworkSelectionMsgHandler();
        
    public: // New functions
        
        /**
         * Handles all network selection messages.
         * @param aMessage, IPC message.
         */
        void HandleMsgL(const RMessage2& aMessage);
        
        /**
        * from MIptvNetworkSelectionObserver.
        */        
        void GetUsedIapResp(TIptvGetUsedIapReq* aRequestOb);
        
    private:

        /**
        * C++ default constructor.
        */
        CIptvNetworkSelectionMsgHandler(CIptvServer& aServer);

        /**
        *
        */
        void GetUsedIapRespL();

    private: // Data

        /**
        *
        */		
		CIptvServer&       iServer;
		
        /**
        *
        */		
		TIptvGetUsedIapReq iGetUsedIapReq;
		
        /**
        *
        */		
		RMessage2          iMessage;
		
        /**
        *
        */		
		HBufC8*            iIpcMsg;
		
        /**
        *
        */		
		TIptvEngineServerReq iMsgPending;
    };

#endif // CIPTVNETWORKSELECTIONMSGHANDLER_H   
