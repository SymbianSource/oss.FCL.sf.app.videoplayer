/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Implementation of the scheduled download plugin engine.*
*/



#ifndef CIPTVSCHEDULEDDOWNLOADPLUGINENGINE_H
#define CIPTVSCHEDULEDDOWNLOADPLUGINENGINE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32cmn.h>
#include "iptvvodscheduleddownloaddefinitions.h"

#include "CIptvVodDlDownloadList.h"
#include "CIptvVodDlDownloadListItem.h"
#include "CIptvVodDlDownloadEvent.h"
#include "CIptvVodDlClient.h"
#include "MIptvVodDlClientObserver.h"
#include "iptvbatterywatcherobserver.h"
#include "iptvphoneregistrationobserver.h"
#include "CIptvServiceManagementClient.h"
#include "MIptvServiceManagementClientObserver.h"
#include "CIptvVodContentClient.h"
#include "MIptvVodContentClientObserver.h"
#include "CIptvVodContentContentBriefDetails.h"
#include "CIptvVodContentCategoryBriefDetails.h"
#include "vcnsscheduleddownloadmpxclient.h"

// CONSTANTS


// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CCseScheduledProgram;
class MCsePluginObserver;
class CIptvServiceManagementClient;
class CIptvVodContentClient;
class CIptvServices;
class CIptvVodDlDownloadEvent;
class CIptvService;
class CIptvBatteryWatcher;
class CProfileChangeNotifyHandler;
class CIptvPhoneRegistrationWatcher;
class CVcxNsScheduledDownloadMpxClient;
class CIptvVodContentContentBriefDetails;

// CLASS DECLARATION    
/**
*  Plugin for IPTV to handle reminders
*
*  @lib iptvscheduleddownloadplugin.lib
*/
class CIptvScheduledDownloadPluginEngine :
            public CTimer, 
			public MIptvServiceManagementClientObserver,
			public MIptvVodContentClientObserver,
			public MVcxNsMpxClientObserver,
			public MBatteryWatcherObserver,
			public MProfileChangeObserver,
			public MPhoneRegistrationWatcherObserver
	{
	public:

        /**
         * Enumerations for return values.
         */
	    enum TPluginResult
	        {
	        ENoErr = 0,
	        EReschedule,
	        EDontReschedule
	        };
	    
    public: // Constructors and destructor

        /**
         * Two-phased constructor.        
         */
    	static CIptvScheduledDownloadPluginEngine* NewL( MCsePluginObserver& aObserver );
    	
    	/**
         * C+ destructor
         */
    	virtual ~CIptvScheduledDownloadPluginEngine();

    public: // New methods

    	/**
         * Runs scheduled task. Actions to be taken depends on given schedule
         *
         * @param aProg Schedule that has fired.
         * @return ENoErr, EReschedule or general error code.
         */
    	void RunTaskL( CCseScheduledProgram& aProg,
    	               TInt& aEngineReturnValue );

    private: // from CTimer, CActive

        /**
         * Timeout, too long time has been gone without anything going on.
         */
        void RunL();

    private: // from MIptvServiceManagementClientObserver

        /**
         * AddServiceResp.
         * Callback function for AddServiceReq operation.
         * @param aRespStatus
         */
        void AddServiceResp(TRespStatus aRespStatus);

        /**
         * UpdateServiceResp.
         * Callback function for UpdateServiceReq.
         * @param aRespStatus
         */
        void UpdateServiceResp(TRespStatus aRespStatus);

        /**
         * DeleteServiceResp.
         * Callback function for DeleteServiceReq operation.
         * @param aRespStatus
         */        
        void DeleteServiceResp(TRespStatus aRespStatus);

        /**
         * GetServicesResp.
         * Callback function for GetServicesReq() methods.
         * @param aRespStatus ESucceeded is returned even if no matching
         *        services were found. In that case the aServicesArray contains
         *        empty array. If aRespStatus != ESucceeded, aServicesArray
         *        pointer is NULL.
         * @param aServicesArray contains array of pointers to descriptors.
         *        aServicesArray ownership moves from server to client, ie client
         *        is responsible for freeing the array.
         *        Array elements are binary descriptors built with CIptvService::GetL() method.
         *        CIptvService::SetL() method can be used to init CIptvService class with
         *        array element data.
         *        In case of error, aServicesArray is NULL.
         */
        void GetServicesResp(TRespStatus aRespStatus, 
                                     CDesC8ArraySeg* aServicesArray);

        /**
         * GetUsedIapResp.
         * Callback function for GetUsedIapReq() method.
         * @param aIapId IAP ID which client should use when connecting to service.
         * @param aIapName
         * @param aConnectionPermission
         * @param aRespStatus If != ESucceeded, aIapId, aIapName and aConnectionPermission
         *                    do not contain valid data.
         */
        void GetUsedIapResp( TUint32 aIapId,
            const TDesC& aIapName,
            CIptvNetworkSelection::TConnectionPermission aConnectionPermission,
            TBool aWlanWhenGPRS,
            CIptvNetworkSelection::TRespStatus aRespStatus );

        /**
         * ServerShutdownResp.
         * Callback function for ServerShutdownReq operation.
         * @param aRespStatus
         */
        void ServerShutdownResp(TRespStatus aRespStatus);

        /**
         * Service Manager generated events handle function.
         * @param aEvent 
         */        
        void HandleSmEvent(CIptvSmEvent& aEvent);


    private: // from MIptvVodContentClientObserver

        /**
         * Messages from EPG manager are handled here. Messages are defined in CIptvUtil.h
         * @since            Engine 1.1
         * @param aMsg       Message 
         * @param aServiceId Service id.
         */
    	void HandleEpgManagerMsgL(
    	    TInt aMsg,
    	    TInt aInfo,
    	    TIptvServiceId aServiceId);


    private: // from MIptvVodDlClientObserver

        /**
         * NOTE: It is not allowed to delete CIptvVodDlClient object from this callback
         * function. Do it from other event (for example user initiated event or timer event).
         */
    	void HandleVodDlDownloadEvent(CIptvVodDlDownloadEvent& aEvent);

    private: // from MBatteryWatcherObserver

        /**
         * This callback will be called when the battery level is low.
         */
        void BatteryLow();

    private: // from MProfileChangeObserver

        /**
         * This is a callback function which is called when a active profile event
         * completes. User must derive his class from MProfileChangeObserver
         * and implement this method if he is to use notify services.
         * @since 2.0
         * @param aProfileEvent Profile event
         * @param aProfileId Active profile id
         */
        void HandleActiveProfileEventL(
            TProfileEvent aProfileEvent,
            TInt aProfileId );

    private: // from MPhoneRegistrationWatcherObserver
        /**
         * This callback will be called when the home network state of the phone
         * registration changes.
         */
        void OnHomeNetwork( TBool aOnHomeNetwork );


    private: // new methods
    
   

        /**
         * C++ constructor
         * 
         * @param aObserver The observer.
         */
    	CIptvScheduledDownloadPluginEngine( MCsePluginObserver& aObserver );
    	
    	/**
         * By default Symbian 2nd phase constructor is private.        
         */
    	void ConstructL();
    	
        /**
         * Check that battery is ok.
         * If not, set the error code to KErrBadPower.
         */
        void GetBatteryStatus();

        /**
         * Cancel downloads. This will be called usually in low power situation.
         */
        void CancelDownloads();

        /**
         * Get the used IAP and check that it can be used to connect.
         * @return KErrNone if all is well, system wide error otherwise.
         */
        void GetUsableIapL();

        /**
         * Get the service id from the scheduled program.
         *
         * @param The program where to get the service.
         * @return The service id.
         */
        TInt GetServiceIdL( CCseScheduledProgram& aProg ) const;

 		/**
         * Asks server to update the epg.
         * Blocks until it is either updated or the update failed.
         * Calls ActiveWait.
         */
    	void UpdateEpgL();
    	
    	/**
         * Start or stop active wait. Does not start or stop unnecessarily.
         *
         * @param aStart ETrue tor start, EFalse to stop.
         */
    	void ActiveWait( TBool aStart );

    	/**
         * Start download for single file.
         *
         * @param  CIptvVodContentContentBriefDetails*  aContent 
         * @return General error code.
         */
    	TInt StartDownloadL( CIptvVodContentContentBriefDetails*  aContent );

        /**
         * Delete the download from the download list if it is there.
         *
         * @param aServiceId The service of the downloaded file.
         * @param aContentId The content id of the downloaded file.
         * @return ETrue if the download was in the list.
         */
        TBool DeleteFromList( TUint32 aServiceId, TUint32 aContentId );

        /**
         * Get the service.
         */
        void FetchServiceL();

        /**
         * Update the epg and start downloads.
         *
         * @return Error code if something goes wrong.
         */
        TInt LoadContentL();

        /**
         * Return ETrue if the given content contains downloadable content.
         *
         * @param aContentId The content id.
         * @return ETrue if the content can be downloaded, EFalse otherwise.
         */
        TBool IsDownloadableL( TIptvContentId aContentId );

        /**
         * Update the the last successfull download time.
         *
         * @param aTimestamp The time when the download started.
         */
        void UpdateLastDownloadTimeL( const TTime& aTimestamp );

        /**
         * Restart the timeout. If the timeout occurs,
         * the plugin will be close all downloads and quit with error
         * KErrTimedOut.
         * Calling this method prevents timeout occuring.
         */
        void RestartTimeoutTimer();

        /**
         * Check if we are in offline mode and if so, cause rescheduling.
         */
        void CheckOfflineModeL();

    private: // data

        /**
         * The state of the download.
         */
        enum TState
            {
            EStarted,
            EUpdatingContentGuide,
            EGettingContent,
            ECheckingDownloadedFiles,
            EDownloadingFiles,
            EFinished
            };

        /**
         * The state of the download.
         */
        TState  iState;

 		/**
         * A list of downloads started. Own.
         */
    	CIptvVodDlDownloadList* iVodDownloads;
    	 	
    	/**
         * The download client to use. Own.
         */
    	CVcxNsScheduledDownloadMpxClient* iVodDlClient;

        /**
         * Pointer back to scheduler.
         */
    	MCsePluginObserver& iObserver;

    	/**
         * Content API client to use. Own.
         */
    	CIptvVodContentClient* iIptvVodContentClient;

    	/**
         * The Service ID.
         */
    	TInt iServiceID;

        /**
         * The service management client to use. Own.
         */
        CIptvServiceManagementClient* iIptvServiceManagementClient;

        /**
         * Member for the callback and other methods to set in error situation.
         */
        TInt* iErrorReason;

        /**
         * A list of entries under one category. Own.
         */
        RPointerArray<CIptvVodContentContentBriefDetails> iEcgList;

        /**
         * A list of categories in a service. Own.
         */
        RPointerArray<CIptvVodContentCategoryBriefDetails> iCategoryList;

    	/**
         * The IAP to use.
         */
    	TUint32 iIapId;

    	/**
         * The active scheduler wait to use when something needs to be waited.
         * Own.
         */
    	CActiveSchedulerWait* iActiveSchedulerWait;

        /**
         * The service. Own.
         */
        CIptvService* iService;

        /**
         * The battery watcher. Own.
         */
         CIptvBatteryWatcher* iBatteryWatcher;

        /**
         * The progilfe change watcher. Own.
         */
         CProfileChangeNotifyHandler* iProfChangeHandler;

        /**
         * The phone registration watcher. Own.
         */
         CIptvPhoneRegistrationWatcher* iPhoneRegistrationWatcher;
	};

#endif // CIPTVSCHEDULEDDOWNLOADPLUGINENGINE_H

// End of file.
