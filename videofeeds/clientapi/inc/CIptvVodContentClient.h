/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    VoD ECG content API*
*/




#ifndef CIPTVVODCONTENTCLIENT_H
#define CIPTVVODCONTENTCLIENT_H

// INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"

#include "MIptvVodContent.h"
#include "CIptvService.h"
#include "RIptvClientSession.h"
        
// FORWARD DECLARATIONS
class CIptvVodContentCategoryBriefDetails;
class CIptvVodContentContentBriefDetails;
class CIptvVodContentContentFullDetails;
class MIptvVodContentClientObserver;
class CIptvVodContentUpdateObserver;
class CIptvMediaContent;
class TIptvRssSearchQuery;
class CIptvMyVideosGlobalFileId;

// CLASS DECLARATION

/**
*  VoD ECG content API
*
*  @lib IptvClientApi.lib
*  @since Engine 1.0
*/
class CIptvVodContentClient : public CActive, MIptvVodContent
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static IMPORT_C CIptvVodContentClient* NewL(TIptvServiceId aServiceId, MIptvVodContentClientObserver& aClientObserver );
        /**
        * Two-phased constructor.
        */
        static IMPORT_C CIptvVodContentClient* NewLC(TIptvServiceId aServiceId, MIptvVodContentClientObserver& aClientObserver );
        
        /**
        * Destructor.
        */
        virtual ~CIptvVodContentClient();
    
    public: // New functions
        
        /**
        * Get list of ECG categories. 
        * The order of categories is defined by database originated from service provider.
        * @since                        Engine 1.0
        * @param    aParentCategoryId   Points to category ID which sub-categories should be listed. Use KIptvVodContentCategoryRootId if the root is referenced.
        * @param    aCategoryList       Reference to a array where pointer to created category list is returned.
        * @return                       Error code.
        */
        IMPORT_C TInt GetEcgCategoryListL(TIptvCategoryId aParentCategoryId, RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList);

        /**
        * Get full detais of one category item.
        * @since                        Engine 1.0 Notice! In Engine 1.1 CIptvVodContentCategoryFullDetails class is removed! There is only one category class CIptvVodContentCategoryBriefDetails.
        * @param    aCategoryId         Selected category ID identifier.
        * @param    aCategory           Reference to a full details data returned.
        * @return                       Error code.
        */
        IMPORT_C TInt GetCategoryDetailsL(TIptvCategoryId aCategoryId, CIptvVodContentCategoryBriefDetails& aCategory);

        /**
        * Get parent category ID identifier. 
        * This method allows application to browse toward root category, then two consecutive calls are needed.
        * Note that root is KIptvVodContentCategoryRootId.
        * @since                        Engine 1.0
        * @param    aCategoryId         Points to category ID which parent category is requested.
        * @param    aParentId           Reference to returned parend ID.
        * @return                       Error code.        
        */
        IMPORT_C TInt GetParentCategoryL(TIptvCategoryId aCategoryId, TIptvCategoryId& aParentId);

        /**
        * Get list of ECG contents in the specified category in order defined by database originated from service provider.
        * @since                        Engine 1.0
        * @param    aCategoryId         Selected category ID identifier. Use KIptvVodContentCategoryRootId if the root is referenced.
        * @param    aSearchString       A search string used to filter contents search. If empty string, no filtering issued.
        * @param    aFrom               The start index for listed contents.
        * @param    aAmount             Amount of content elementes to be listes. If zero, all starting from aFrom.
        * @param    aTotalAmount        Reference to a variable where total amount of content is returned.
        * @param    aEcgList            Reference to list of returned items.
        * @return                       Error code.        
        */
        IMPORT_C TInt GetEcgListL(TIptvCategoryId aCategoryId, TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount, TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList);
        
        /**
        * Get list of all ECG contents in alphabetical order.
        * @since                        Engine 1.0
        * @param    aSearchString       A search string used to filter contents search. If empty string, no filtering issued.
        * @param    aFrom               The start index for listed contents.
        * @param    aAmount             Amount of content elementes to be listes. If zero, all starting from aFrom.
        * @param    aTotalAmount        Reference to a variable where total amount of content is returned.
        * @param    aEcgList            Reference to list of returned items.
        * @return                       Error code.        
        */
        IMPORT_C TInt GetEcgAllListL(TDesC& aSearchString, TUint32 aFrom, TUint32 aAmount, TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList);

        /**
        * Get full detais of one content item.
        * @since                        Engine 1.0
        * @param    aContentId          Selected content ID identifier.
        * @param    aContentFullDetails Reference to returned full details data.
        * @return                       Error code.        
        */
        IMPORT_C TInt GetContentDetailsL(TIptvContentId aContentId, CIptvVodContentContentFullDetails& aContentFullDetails);

        /**
        * Set IAP for service.
        * @since                        Engine 1.0
        * @param    aIap                Internet access point.
        * @return                       Error code.
        */
        IMPORT_C TInt SetIapL(const TUint32 aIap);

        /**
        * Update Ecg.
        * @since                        Engine 1.0
        * @return                       Error code.
        */
        IMPORT_C TInt UpdateEcgL();

        /**
        * Verify service group.
        * @since                        Engine 1.0
        * @return                       Error code.
        */
        IMPORT_C TInt CheckGroupL();

        /**
        * Get time when service was last updated.
        * @since                        Engine 1.0
        * @param    aTime               Reference to TTime object when DB was last updated.
        * @return                       Error code.
        */
        IMPORT_C TInt GetUpdateTimeL(TTime& aTime);

        /** 
        * Get content access objects belonging to given content.
        * @since                        Engine 1.1
        * @param    aContentId          Content ID.
        * @param    aCAList             Content access list.
        * @return                       Error code.
        */
        IMPORT_C TInt GetContentAccessListL(TUint32 aContentId, RPointerArray<CIptvMediaContent>& aCAList);

        /** 
        * Set last play position to content access.
        * @since    Engine 1.1
        * @param    aContentId          Content id
        * @param    aIndex              Content access index (id).
        * @param    aPosition           Position.
        * @return                       Error code.
        */
        IMPORT_C TInt SetLastPositionL(TUint32 aContentId, TUint32 aIndex, TUint32 aPosition);

        /** 
        * Start search operation initiated by the UI
        * @since
        * @param    aQuery	Query containing the keyword used for search
        * @return	One of the Symbian error codes.
        */
		IMPORT_C TInt Search( TIptvRssSearchQuery& aQuery );
		
        /** 
        * Cancel currently active search operation if any.
        * @since
        * @param    None
        * @return	One of the Symbian error codes.
        */
		IMPORT_C TInt CancelSearch();
		
        /** 
        * Set Mpx media id to content.
        * @param    aContentId          Content id
        * @param    aCaIndex            Content access index
        * @param    aMpxId              Mpx id.
        * @return                       Error code.
        */
        IMPORT_C TInt SetMpxIdL( TUint32 aContentId,
                                 TUint32 aCaIndex,
                                 TUint32 aMpxId );

        /**
         * Reset global Id.
         *
         * @param aFileId    Global file id to be reset.
         * @param aServiceId Service id.
         */
        IMPORT_C void ResetGlobalId(
            CIptvMyVideosGlobalFileId& aFileId,
            TUint32 aServiceId );
    
        /**
         * Check if automatic update is needed for  
         * contents.
         * @param On return, ETrue if update is needed. 
         *        Otherwise EFalse
         * 
         * @return One of the Symbian error codes.
         */
        IMPORT_C TInt UpdateAvailableL( TBool& aAvailable );
        
        /**
         * Cancel ecg update.
         */
        IMPORT_C void CancelUpdate();

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
        */
        CIptvVodContentClient(TIptvServiceId aServiceId, MIptvVodContentClientObserver& aClientObserver);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Send size request to server.
        */
        void SendSizeRequestL(TInt aMessage, TUint32 aParam, TRequestStatus& aStatus);

        /**
        * Send size request to server.
        */
        void SendSizeRequestL( TInt aMessage,
                               TUint32 aUintParam1,
                               TUint32 aUintParam2,
                               const TDesC& aDesCParam,
                               TRequestStatus& aStatus );

        /**
        * Send size request to server.
        */
        void SendSizeRequestL( TInt aMessage,
                               TUint32 aUintParam1,
                               TUint32 aUintParam2,
                               TUint32 aUintParam3,
                               const TDesC& aDesCParam,
                               TRequestStatus& aStatus );

        /**
        * Send request to server.
        *
        * @param aMessage Message to send.
        * @param aFileIdParam Message parameter, global file Id.
        * @param aUintParam   Message parameter, TUint32.
        * @return KErrNone or one of system wide error codes.
        */         
        TInt SendRequestL(
            TInt aMessage,
            CIptvMyVideosGlobalFileId& aFileIdParam,
            TUint32 aUintParam);

        /**
        * Send request to server.
        */
        void SendRequestL(TInt aMessage, TUint32 aParam, TRequestStatus& aStatus);
        
        /**
        * Handle category list size request.
        */
        void HandleVodContentGetEcgCategoryListSizeReqL();

        /**
        * Handle category list data request.
        */
        void HandleVodContentGetEcgCategoryListDataReqL(RPointerArray<CIptvVodContentCategoryBriefDetails>& aCategoryList);        

        /**
        * Handle category details size request.
        */
        void HandleVodContentGetCategoryDetailsSizeReqL();

        /**
        * Handle category details data request.
        */
        void HandleVodContentGetCategoryDetailsDataReqL(CIptvVodContentCategoryBriefDetails& aCategory);

        /**
        * Handle get parent category request.
        */
        void HandleVodContentGetParentCategoryReqL(TIptvCategoryId& aParentId);

        /**
        * Handle get ecg list size request.
        */
        void HandleVodContentGetEcgListSizeReqL();

        /**
        * Handle get ecg list data request.
        */
        void HandleVodContentGetEcgListDataReqL(TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList);

        /**
        * Handle get ecg all list size request.
        */
        void HandleVodContentGetEcgAllListSizeReqL();

        /**
        * Handle get ecg all list data request.
        */
        void HandleVodContentGetEcgAllListDataReqL(TUint32& aTotalAmount, RPointerArray<CIptvVodContentContentBriefDetails>& aEcgList);

        /**
        * Handle content details size request.
        */
        void HandleVodContentGetContentDetailsSizeReqL();

        /**
        * Handle content details data request.
        */
        void HandleVodContentGetContentDetailsDataReqL(CIptvVodContentContentFullDetails& aContentFullDetails);

        /**
        * Send cancel request.
        */
        void SendCancelRequest();
        
        /**
        * Send cancel update request.
        */
        void SendCancelUpdateRequest();

        /**
        * Handle update time.
        */
        void HandleVodContentGetUpdateTimeL( TTime& aTime );

        /**
        * Send get update time request.
        */
        void SendUpdateTimeRequestL( TRequestStatus& aStatus );

        /**
        * CleanupL
        */
        void CleanupL();            

        void HandleVodContentGetCAListSizeReqL();
        void HandleVodContentGetCAListDataReqL(RPointerArray<CIptvMediaContent>& aCAList);
        void SendLastPositionL(TInt aMessage, TUint32 aContentId, TUint32 aIndex, TUint32 aPosition, TRequestStatus& aStatus);

        /**
        * Write mpx id to ecg database.
        * @param    aContentId          Content id
        * @param    aCaIndex            Content access index
        * @param    aMpxId              Mpx id.
        * @param    aStatus             Request status.
        */
        void SendMpxIdL( TInt aMessage, 
                         TUint32 aContentId, 
                         TUint32 aCaIndex, 
                         TUint32 aMpxId, 
                         TRequestStatus& aStatus );
        
        /**
        * Send update check request.
        *
        */
        void SendUpdateCheckRequestL( TRequestStatus& aStatus );
        
        /**
        * Handle vod content update check 
        * @param aAvailable On return, ETrue if update is available and needed,
        * otherwise EFalse.
        */
        void HandleVodContentUpdateCheckL( TBool& aAvailable );

    private:    // Data

        // Reference to observer class implemented by the application.
        MIptvVodContentClientObserver& iClientObserver;

        /**
        * iSession, the server session.
        */
        RIptvClientSession iSession;

        /**
        * iMsg Heap object to restore memory for IPC messaging.
        */
        HBufC8* iMsg;

        /**
        * iMsgPtr Passed to RIptvClientSession object to store the sent and received data.
        * Points to iData heap object.
        */
        TPtr8 iMsgPtr;		                
                
        /**
        * Service ID.
        */
        TIptvServiceId iServiceId;        

        /**
        * Pointer to CIptvVodContentUpdateObserver.
        */    
        CIptvVodContentUpdateObserver* iObserver;
    };

#endif      // CIPTVVODCONTENTCLIENT_H
