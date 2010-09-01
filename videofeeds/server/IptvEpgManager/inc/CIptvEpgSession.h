/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Controls epg services update*
*/




#ifndef CIPTVEPGSESSION_H
#define CIPTVEPGSESSION_H

// INCLUDES
#include <e32std.h>
#include "CIptvUtil.h"
#include "MIptvEpgManagerUpdateObserver.h"
#include "MIptvServiceManagerObserver.h"

// FORWARD DECLARATIONS
class CIptvMyVideosGlobalFileId;
class CIptvEpgVodClientImpl;
class CIptvEpgVodMsqQueue;
class CIptvEpgLiveTvCallbackImpl;
class CIptvEpgPluginInterface;
class CIptvEpgVodCallbackImpl;
class CIptvEpgDb;
class CIptvSearchManager;
class TIptvRssSearchQuery;
class CIptvEpgDatabase;
class CIptvEpgService;
class CIptvServer;
class CIptvEpgManagerUpdater;
class CVcxGroupUpdater;

// CLASS DECLARATION
class CIptvEpgSession : public CBase,
                        public MIptvEpgManagerUpdateObserver,
                        public MIptvServiceManagerObserver
    {
    public:

        /**
         * Two-phased constructor.
         * @param 
         */
        static CIptvEpgSession* NewL(
            CIptvEpgDb* aEpgDb,
            TUint32 aServiceId,
            CIptvEpgVodMsqQueue* aMsgQueue,
            CIptvServer& aServer,
            TBool aPluginCanRun,
            TInt& aError );

        /**
         * Two-phased constructor.
         * @param 
         */
        static CIptvEpgSession* NewL(
            CIptvEpgDatabase* aLiveTvDb,
            TUint32 aServiceId,
            CIptvEpgVodMsqQueue* aMsgQueue,
            CIptvServer& aServer,
            TBool aPluginCanRun,
            TInt& aError );

        /**
         * Destructor.
         */
        ~CIptvEpgSession();

    public: // From MIptvEpgManagerUpdateObserver interface
        
        /**
         * Check is plugin running.
         */
        void CheckIsPluginRunning();

    public:

        /**
         * Get reference count.
         */        
        TInt GetReferenceCount() { return iReferenceCount; };

        /**
         * Set reference count.
         */
        void SetReferenceCount(TInt aReferenceCount) { iReferenceCount = aReferenceCount; };

        /**
         * Get service id.
         */
        TUint32 GetServiceId() { return iServiceId; };
        
        /**
         * Reset global Id.
         */
        TInt ResetGlobalId(CIptvMyVideosGlobalFileId& aId);

        /**
         * Set global Id.
         */
        TInt SetGlobalId(TUint32 aContentKey, CIptvMyVideosGlobalFileId& aId, TUint32 aIndex);
        
        /**
         * Is Epg manager ready?
         */
        TBool IsReadyL();

        /**
         * Set Epg manager ready.
         */
        void SetReady();
      
        /**
         * Delete EPG-database. Mark database to be deleted. Database file is deleted 
         * when EPG-manager is deleted.
         */
        void DeleteDatabaseFile();
        
        /**
         * Get pointer to Vod client implementation.
         */
        CIptvEpgVodClientImpl* GetVodClientImplL();

        /**
         * Update Ecg.
         * @since
         * @param aForceUpdate Update required without last modified check.
         * @return Error code, KErrNone if no errors, otherwise one of the system wide error codes.
         */
        TInt UpdateEcg( TBool aForceUpdate );

        /**
         * Handle search request and forward the message to search manager 
         * to be handled.
         *
         * @param	aSearchQuery	Class containing search details
         * @return	KErrNone if successful, otherwise Symbian error code
         */
        TInt Search( TIptvRssSearchQuery& aSearchQuery );

        /**
         * Gathers a list of used Content ID numbers and updates them to 
         * Search Manager. This is done to prevent overlapping of ID numbers
         * between downloads from old and new search results.
         */
        void UpdateSearchUsedContentIdsListL();

        /**
         * Updates the Content IDs that have changed during DB update to
         * other interested parties (in practise Download Manager).
         *
         * @param aOldContentIds Array of old Content IDs.
         * @param aNewContentIds Array of new Content IDs.
         */
        void UpdateChangedContentIdsL( RArray<TUint32>& aOldContentIds,
                                       RArray<TUint32>& aNewContentIds );

        /**
         * Returns true if plugin is running.
         *
         * @return ETrue if plugin is running.
         */
        TBool IsPluginRunning() { return iPluginStarted; }
        
		/**
		*	Cancel the current active search
		*	@param	None
		*	@return	KErrNone if successful, otherwise Symbian error code
		*/
        TInt CancelSearch();

        /**
         * Initialize
         */
        void InitializeL();

        /**
         * Set IAP for this service session.
         */
        void SetIapL(TUint32 aIap);

        /**
         * Update service thumbnail.
         */
        void UpdateServiceThumbnailL( const TDesC& aIconFilePath, const TBool aGrouped );
        
        /**
         * Check if vod update is needed
         * @return ETrue if update is needed for content.
         * otherwise EFalse.
         */
        TBool IsVodUpdateNeededL();
        
        /**
         * Vod plugin cancelled.
         */
        void VodPluginCancelled();
        
        /**
         * Sets last update time for the service. 
		 * @param aLastUpdate The last update time.
		 * @return Error code from database handling functions.
         */
        TInt SetLastUpdateTimeL( const TTime& aLastUpdate );
    
    public: // From MIptvServiceManagerObserver

        /**
         * CIptvServiceManager calls this every time a service is
         * modified in the database.
         * @param aEvent Event to inform
         */
        void HandleSmEvent( CIptvSmEvent& aEvent );

    private:    // New functions

        /**
         * Check is VOD database update needed.
         */
        void CheckIsVodUpdateNeededL();

        /**
        * C++ default constructor.
        */
        CIptvEpgSession(
            CIptvEpgDb* aEpgDb,
            TUint32 aServiceId,
            CIptvEpgVodMsqQueue* aMsgQueue,
            CIptvServer& aServer,
            TBool aPluginCanRun );

        /**
        * C++ default constructor.
        */
        CIptvEpgSession(
            CIptvEpgDatabase* aLiveTvDb,
            TUint32 aServiceId,
            CIptvEpgVodMsqQueue* aMsgQueue,
            CIptvServer& aServer,
            TBool aPluginCanRun );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param 
        */
        TInt ConstructL();

        /**
        * Get service information.
        * @param none
        */
        TInt GetServiceInformationL();

        /**
         * Run VOD plugin.
         * @param aForceUpdate Update required without last modified check.
         */
        void RunVodPluginL( TBool aForceUpdate = EFalse );

        /**
         * Run VOD plugin.
         * @param aForceUpdate Update required without last modified check.
         */
        void RunVodPluginForFeedL( TBool aForceUpdate = EFalse );

        /**
         * Run VOD plugin.
         * @param aForceUpdate Update required without last modified check.
         */
        void RunVodPluginForGroupL( TBool aForceUpdate = EFalse );

        /**
         * Run SmartVision plugin.
         */
        void RunSmartVisionPluginL();

        /**
         * VOD Plugin stopped.
         */
        void VodPluginStopped();

        /**
         * Check is VOD update allowed.
         * @param aErrorCode KErrServerBusy enables retry.
         */
        TBool IsVodUpdateAllowedL( TInt& aErrorCode );

        /**
         * Check do we have valid VOD database available.
         */
        TBool IsValidVodDbAvailableL(TTime& aLastUpdated, TTime& aInterval);

        /**
         * Update VOD ecg.
         * @param aForceUpdate Update required without last modified check.
         */
        void UpdateVodEcgL( TBool aForceUpdate );
        
        /**
         * Update Thomson epg.
         */
        TInt UpdateLiveEpg();

        /**
         * Send message to client.
         */
        void SendMessageToClientL(TInt aMsg, TInt aInfo);
        
        /**
         * Leaving part of search.
         * @param error Completion code.
         */
        void DoSearchL( TInt& error );
                                                  
    private:    // Data members                         

        CIptvServer& iServer;

        CIptvEpgManagerUpdater* iUpdater;

        CIptvEpgVodClientImpl* iVodClient;

        CIptvEpgPluginInterface* iPlugin;        

        CIptvEpgVodCallbackImpl* iVodCallback;

        CIptvSearchManager* iSearchManager;
        
        /**
         * Session for updating costomized service group. 
         * own
         */
        CVcxGroupUpdater* iGroupUpdater;

        CIptvEpgLiveTvCallbackImpl* iLiveTvCallback;
        
        CIptvEpgDb* iEpgDb;
        
        CIptvEpgDatabase* iLiveTvDatabase;
        
        CIptvEpgService* iService;

        CIptvEpgVodMsqQueue* iMsgQueue;        

        TBool iPluginStarted;

        TBool iDeleteDatabase;

        TUint32 iServiceId;

        TBool iPluginCanRun;

        TInt iReferenceCount;

        TBool iInitialized;

        TBool iIsIap;

        TInt iIap;

        TBool iUpdatePending;

        TBool iReady;

        RFs iFs;    	

        TBool iForceUpdatePending;

    };

#endif  // CIPTVEPGSESSION_H
