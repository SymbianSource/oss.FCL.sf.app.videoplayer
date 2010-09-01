/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    ?Description*
*/




#ifndef CIPTVEPGMSGHANDLER_H
#define CIPTVEPGMSGHANDLER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "CIptvUtil.h"
          
#include "MIptvVodContentClientObserver.h"
          
// FORWARD DECLARATIONS
class CIptvEpgVodMsqQueue;
class CIptvEpgSession;
class CIptvServer;
class CIptvEpgManagerImpl;
class CIptvVodContentCategoryBriefDetails;
class CIptvVodContentContentBriefDetails;
class CIptvVodContentContentFullDetails;
class CIptvMediaContent;
class CIptvEpgDatabase;

// CLASS DECLARATION
class CIptvEpgMsgHandler : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CIptvEpgMsgHandler* NewL(CIptvServer& aServer);
        
        /**
        * Destructor.
        */
        virtual ~CIptvEpgMsgHandler();
                      
    public: // New functions
        
        /**
         * Handles EIptvEngineServerGetAllEpgsReq and
         * EIptvEngineServerGetAllSelectedEpgsReq messages.
         * @param aMessage, IPC message where the response is written.
         * @param aCriteria criteria which limits the get results.
         */
        void HandleEpgMsgL(const RMessage2& aMessage);

        /**
        * Send message from EPG manager to client.
        * @since            Engine 1.1
        * @param aMsg       Message         
        * @param aInfo      Optional info parameter.       
        */
        void SendMessageToClientL(TInt aMsg, TInt aInfo);

    private:

        /**
        * C++ default constructor.
        */
        CIptvEpgMsgHandler(CIptvServer& aServer);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
         * Handle EIptvEngineVodContentGetEcgCategoryListSizeReq
         * @param aMessage
         */
        void HandleGetEcgCategoryListSizeReqL(const RMessage2& aMessage);

        /**
         * Handle EIptvEngineVodContentGetEcgCategoryListDataReq
         * @param aMessage
         */
        void HandleGetEcgCategoryListDataReqL(const RMessage2& aMessage);

        /**
         * Handle EIptvEngineVodContentGetCategoryDetailsSizeReq
         * @param aMessage
         */
        void HandleGetCategoryDetailsSizeReqL(const RMessage2& aMessage);
    
        /**
         * Handle EIptvEngineVodContentGetCategoryDetailsDataReq
         * @param aMessage
         */
        void HandleGetCategoryDetailsDataReqL(const RMessage2& aMessage);

        /**
         * Handle EIptvEngineVodContentGetParentCategoryReq
         * @param aMessage
         */
        void HandleGetParentCategoryReqL(const RMessage2& aMessage);

        /**
         * Handle EIptvEngineVodContentGetEcgListSizeReq
         * @param aMessage
         */
        void HandleGetEcgListSizeReqL(const RMessage2& aMessage);

        /**
         * Handle EIptvEngineVodContentGetEcgListDataReq
         * @param aMessage
         */
        void HandleGetEcgListDataReqL(const RMessage2& aMessage);

        /**
         * Handle EIptvEngineVodContentGetEcgAllListSizeReq
         * @param aMessage
         */
        void HandleGetEcgAllListSizeReqL(const RMessage2& aMessage);

        /**
         * Handle EIptvEngineVodContentGetEcgAllListDataReq
         * @param aMessage
         */
        void HandleGetEcgAllListDataReqL(const RMessage2& aMessage);

        /**
         * Handle EIptvEngineVodContentGetContentDetailsSizeReq
         * @param aMessage
         */
        void HandleGetContentDetailsSizeReqL(const RMessage2& aMessage);

        /**
         * Handle EIptvEngineVodContentGetContentDetailsDataReq
         * @param aMessage
         */            
        void HandleGetContentDetailsDataReqL(const RMessage2& aMessage);
      
        /**
         * Handle EIptvEngineVodContentServiceId message from client and get
         * CIptvEpgManagerImpl pointer from server.
         * @param aMessage
         */
        void HandleServiceIdL(const RMessage2& aMessage);

        /**
        * Handle EIptvEngineLiveTvUpdateChannelReq message
        * @param aMessage Reference to RMessage2
        */

        void HandleGetCAListSizeReqL(const RMessage2& aMessage);
        void HandleGetCAListDataReqL(const RMessage2& aMessage);
        void SetLastPositionL(const RMessage2& aMessage);
        void HandleSetMpxIdL(const RMessage2& aMessage);
        
        /**
        * Handle EIptvEngineVodContentCheckUpdate message
        * @param aMessage Reference to RMessage2
        */
        void HandleContentCheckUpdateL( const RMessage2& aMessage );

        /**
        *
        */
        void HandleUpdateChannelOrderReqL( const RMessage2& aMessage );

        /**
        * Handle EIptvEngineVodGetUpdateTime message
        * @param aMessage Reference to RMessage2
        */
        void HandleGetUpdateTimeL(const RMessage2& aMessage);

        /**
        * Handle EIptvEngineVodContentSetIap message
        * @param aMessage Reference to RMessage2
        */
        void HandleSetIapL(const RMessage2& aMessage);

        /*
         * Handle Reset Global Id request.
         *
         * @aMessage Message from client.
         */
        void HandleResetGlobalIdL(
            const RMessage2& aMessage );
                   
    private:    // Data
        CIptvEpgVodMsqQueue* iVodMsgQueue;
        CIptvEpgSession* iEpgSession;
        CIptvServer& iServer;
        RPointerArray<CIptvVodContentCategoryBriefDetails> iCategoryBriefDetailsArray;
        RPointerArray<CIptvVodContentContentBriefDetails> iContentBriefDetailsArray;
        RPointerArray<CIptvMediaContent> iMediaContentArray;
        TUint32 iDataSize;
        CIptvVodContentCategoryBriefDetails* iCategory;
        CIptvVodContentContentFullDetails* iContentFullDetails;
        TUint32 iTotalAmount;
        TIptvServiceId iServiceId; 
        TUint32 iIapId;
        CIptvEpgDatabase* iEpgDatabase;     
    };

#endif // CIPTVEPGMSGHANDLER_H   
