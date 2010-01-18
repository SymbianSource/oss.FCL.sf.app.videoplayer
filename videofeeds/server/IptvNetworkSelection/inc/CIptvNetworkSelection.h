/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Header file for CIptvNetworkSelection.h class*
*/



#ifndef __CIPTVNETWORKSELECTION_H__
#define __CIPTVNETWORKSELECTION_H__

#include <e32base.h>
//#include <cdbcols.h>
#include <wlanmgmtclient.h>
#include <wlanscaninfo.h>
#include "TIptvIap.h"
#include "MIptvTimerObserver.h"
#include <rconnmon.h>
#include <MProfileChangeObserver.h> //profile change notifier
#include "IptvDebug.h"
#include "TIptvGetUsedIapReq.h"
#include "MIptvNetworkSelectionObserver.h"

//#define ISIAIMATCH_USED
#define ICISIMATCH_USED

class CIptvServer;
class CIptvIapList;
class CIptvService;
class CIptvTimer;
class CProfileChangeNotifyHandler;

// CLASS DECLARATION

/**
* CIptvServer owned class. This class is used to find out correct IAP for accessing
* a Video Center service. This class also keeps track of connection permissions to
* different type networks.
*/
class CIptvNetworkSelection : public CActive,
                              public MIptvTimerObserver,
                              public MProfileChangeObserver

    {
    
    public: // Enumerations

        /**
        * Response statuses.
        */
        enum TRespStatus
            {
            EProcessing,
            ESucceeded,
            EGeneralError,
            /**
            * Service IAP list didn't contain any IAP which could be used for
            * connecting at the moment and no currect IAP set.
            */
            ENoSuitableIapFound,
            
            /**
            * Couldn't get IAP without WLAN scan,
            * calls TIptvGetUsedIapReq->iNsObserver->GetUsedIapResp()
            * when WLAN scan has finished.
            */
            EDoingWlanScan,
            EServiceNotFound,
            ECancelled,
            EFailedAlwaysAskSelected
            };

        enum TConnectionPermission
            {
            EAllowed,
            ENotAllowed,
            EMustAskConfirmation
            };
                    
    public:
        /**
        * Constructor.
        */
        static CIptvNetworkSelection* NewL(CIptvServer& aServer);
        
        /**
        * Destructor.
        */
        virtual ~CIptvNetworkSelection();

    private:
        CIptvNetworkSelection(CIptvServer& aServer);
        
    public:

        /* Network selection API */

        /**
        * Sets IAP aIapId to be used as a default untill server is shutdown
        * or value is changed.
        * I.e this setting is valid for the server lifetime.
        * @param aIapId
        * @param aServiceId.
        * @param aSetToDefaultForService if True, the IAP is set to all services
        *        to service db (current implementation ignores aServiceId).
        * @param aRespStatus
        */   
        void SetUsedIapL(TUint32 aIapId,
                         TUint32 aServiceId,
                         TBool aSetToDefaultForService,
                         TRespStatus& aRespStatus);
        /**
        * Gets IAP which is used for connecting to aGetUsedIapReq->iServiceId.
        * Function is synchronous if IAP can be retrieved without wlan scan.
        * If wlan scan is needed aGetUsedIapReq->iRespStatus will contain EDoingWlanScan
        * value, in that case GetUsedIapResp() is called from aGetUsedIapReq->iNsObserver
        * when operation is completed.
        * Caller must inherit MIptvNetworkSelectionObserver class and implement callback
        * functions from there.
        * Caller keeps ownership to aGetUsedIapReq.
        * @param aGetUsedIapReq->iNsObserver pointer to object which issued the request. 
        * @param aGetUsedIapReq->iServiceId
        * @param aGetUsedIapReq->iIapId will contain IAP ID.
        * @param aGetUsedIapReq->iIapName will contain IAP name.
        * @param aGetUsedIapReq->iConnectionPermission will contain connection permission.
        * @param aGetUsedIapReq->iRespStatus will contain resp status.
        */
        void GetUsedIapReqL(TIptvGetUsedIapReq* aGetUsedIapReq);
        
        /**
        * Cancels all GetUsedIapReqs for aObserver.
        * WLAN scan is not cancelled (it is not possible). On WINSCW the WLAN simulation timer is cancelled.
        * @param aObserver
        */                                 
        TInt CancelGetUsedIapReq(MIptvNetworkSelectionObserver* aObserver);
                 
        /**
        * Sets connection allowed for connection type aIapId represents.
        * See TConnectionType for possible types.
        * @param aConnectionAllowed
        * @param aIapId
        * @param aRespStatus
        */
        void SetConnectionAllowedL(TBool aConnectionAllowed,
                                   TUint32 aIapId,
                                   TRespStatus& aRespStatus);

        /**
        * @param aIapId Iap ID which connection permission is queryed.
        * @return Connection permission
        */
        TConnectionPermission IsConnectionAllowedL(TUint32 aIapId);

        /**
        * From MIptvTimerObserver.
        * @param aTimer pointer to timer which expired.
        */
        void TimerExpired(CIptvTimer* aTimer);

        /**
        * ClearUsedIap.
        */
        void ClearUsedIap();
        
    private:
        void ConstructL();

        /**
        * Gets used IAP for aServiceId. This is called after wlan scan.
        * @param aGetUsedIapReq request message from the client.
        */        
        void GetUsedIapAfterWlanScanL(TIptvGetUsedIapReq& aGetUsedIapReq);

        /**
        * GetActiveIapsL.
        * Gets currently active IAPS in the system.
        * Used by UpdateActiveIapsL method.
        */        
        CIptvIapList* GetActiveIapsL();

        /**
        * UpdateActiveIapsL.
        * Updates iActiveIaps member variable.
        */                                         
        void UpdateActiveIapsL();
        
        /**
        * UpdateWlanScannedIapsL.
        * Performs Wlan scan and updates iWlanScannedIaps member variable.
        */                                 
        void UpdateWlanScannedIapsL();

        /**
        * FindCommonIapFromLists.
        * Picks the highest priority IAP from aServiceIaps list which is also
        * on aCompare list.
        * @param aServiceIaps
        * @param aCompareIaps
        * @param aIap The found iap is written here.
        * @return System-wide error code. aIap will contain valid value only
        *         if this is KErrNone.
        */
        TInt FindCommonIapFromListsL(CIptvIapList& aServiceIaps,
                                     CIptvIapList& aCompareIaps,
                                     TIptvIap& aIap);

        /**
        * CreateFilteredIapListL
        * Picks IAPs from aIapList which have matching connection type
        * with aConnectionTypeMask.
        * @param aIapList list to be filtered
        * @param aConnectionTypeMask
        * @return filtered IAP list, ownership moves to caller.
        */
        CIptvIapList* CreateFilteredIapListL(CIptvIapList& aIapList,
                                            TUint32 aConnectionTypeMask);

#ifdef ISIAIMATCH_USED   
        /**
        * Updates iSiAiMatch member variable.
        */
        void UpdateSiAiMatchL();
#endif

        /**
        * Updates iSiSwiMatch member variable.
        */
        void UpdateSiSwiMatchL();

#ifdef ICISIMATCH_USED
        /**
        * Updates iCiSiMatch member variable.
        */
        void UpdateCiSiMatch();
#endif

        /**
        * Updates iCiAiMatch member variable.
        */
        void UpdateCiAiMatch();

        /**
        * Updates iCiSwiMatch member variable.
        */
        void UpdateCiSwiMatchL();

        /**
        * Handles all pending GetUsedIapRequests from clients.
        * This is called after WLAN scan.
        */
        void HandleGetUsedIapRequests();

        /**
        * 
        */
        void GetWlanIapFromAiL(TIptvIap& aIap);

        /**
        * Gets IAP name, if empty, tries to find SSID. Leaves with KErrNotFound
        * if not found.
        * @param aIapId IAP to get
        * @param aIapName IAP name or SSID is written here.
        */
        void GetIapNameL(TUint32 aIapId, TDes& aIapName);
        
        /**
         * Gets the first IAP ID with matching IAP name from commsdb.
         * @param aIapId   The found IAP ID.
         * @param aIapName The IAP name used in comparasion.
         * @return         KErrNone if IAP was found, KErrNotFound if no matching IAP name was found.
         */
        TInt GetIapIdL(TUint32& aIapId, const TDesC& aIapName);

        /**
        * Gets SSID from commsdb using wlan service id.
        * @param aWlanServiceId
        * @param aSsid
        */
        TInt GetSsidL(TUint32 aWlanServiceId, TDes& aSsid);

        /**
        * Current IAP selected, fills aGetUsedIapReq accordingly.
        * @param aGetUsedIapReq
        */
        void SelectCiL(TIptvGetUsedIapReq& aGetUsedIapReq);

        /**
        * aIapId selected, fills aGetUsedIapReq accordingly.
        * @param aGetUsedIapReq
        * @param aIapId
        */
        void SelectL(TIptvGetUsedIapReq& aGetUsedIapReq, TUint32 aIapId);

        /**
        * Updates all IAP lists except iCiSwiMatch which requires wlan scan.
        */
        void UpdateAllButWlanListsL(TIptvGetUsedIapReq& aGetUsedIapReq);

        /**
        * Updates iService by getting new value from Service Manager. If service has IAP name field set,
        * it is tried to convert into an IAP ID. Conversion is stored to Service Manager.
        */
        void UpdateServiceL(TUint32 aServiceId);

#if IPTV_LOGGING_METHOD != 0
        void PrintIapList(CIptvIapList& aIapList);
#endif

        /**
        * Adds all hidden WLAN IAPs from iService to iScannedWlanIaps.
        */
        void AddHiddenWlanSiIapsToSwiL();
        
        /**
        * Clears iCurrent IAP if it is EGPRS or ECSD type.
        * Clears all EGPRS and ECSD type IAPs from iService IAPs.
        * This function is used in offline mode to ignore GPRS and CSD
        * type IAPs.
        */
        void ClearGprsAndCsdIapsL();

    public: //From MProfileChangeObserver

        /**
        * Callback function which is called when an active profile event
        * completes. 
        * @param aProfileEvent Profile event
        * @param aProfileId Active profile id
        */
        void HandleActiveProfileEventL( TProfileEvent aProfileEvent, TInt aProfileId ); 

    private:

#if IPTV_LOGGING_METHOD != 0
        const TDesC& ConnectionTypeDes( CIptvUtil::TConnectionType aConnectionType );
#endif

    protected: // Functions from base classes

        /**
        * From CActive, RunL.
        * Callback function.
        * Invoked to handle responses from the server.
        */
        void RunL();

        /**
        * From CActive, DoCancel.
        * Cancels any outstanding operation.
        */
        void DoCancel();
        
        /**
        * From CActive, RunError.
        * Handles possible leaves during RunL.
        */
        TInt RunError( TInt aError );

    private:
        
        /**
        * System has these IAPs active at the moment
        */
        CIptvIapList*      iActiveIaps;
        
        /**
        * These IAPs have been scanned from the neighborhood.
        */
        CIptvIapList*      iScannedWlanIaps;

        /**
        * Working iaplist, used when calculating IAP to select
        */
        CIptvIapList*      iAvailableIaps;
                
        /**
        * Currently used IAP, set with SetUsedIap() method.
        */
        TIptvIap           iCurrentIap;

#ifdef ISIAIMATCH_USED        
        /**
        * Contains the highest priority IAP from iService IAPs which is also
        * in iActiveIaps.
        */
        TIptvIap           iSiAiMatch;
#endif        
        
        /**
        * The highest priority service IAP which is also found from scanned WLAN IAPs.
        */
        TIptvIap           iSiSwiMatch;

#ifdef ICISIMATCH_USED        
        /**
        * Contains iCurrentIap if it is found from iService IAP list. 
        */
        TIptvIap           iCiSiMatch;
#endif

        /**
        * Contains current IAP if it is found from active IAPs.
        */
        TIptvIap           iCiAiMatch;
        
        /**
        * Contains current IAP if it is found from scanned WLAN IAPs.
        */
        TIptvIap           iCiSwiMatch;
        
        /**
        * Service object, IAP list is fetched from it.
        */
        CIptvService*      iService;
        
        /**
        * A mask defining which connection types are allowed for connection.
        * Bit values from TConnectionType are used.
        */        
        TUint32            iConnectionAllowedMask;
                        
        /**
        * The owner of this class
        */
        CIptvServer&                 iServer;        
        
        /**
        * All GetUsedIap requests are stored here and a single
        * WLAN scan is performed for them all.
        */
        RArray<TIptvGetUsedIapReq*>  iGetUsedIapReqs;

#ifdef __WINSCW__
        CIptvTimer*                  iWlanScanSimulationTimer;
#endif

        /**
        * Used to perform WLAN scan.
        */
        RConnectionMonitor           iConnMon;
        
        /**
        * Contains WLAN scan result.
        */
        TConnMonIapInfoBuf           iIapInfoBuf;
        
        /**
        * Used to scan offline state, iOffline is updated accordingly.
        */
        CProfileChangeNotifyHandler* iProfChangeHandler;
        
        /**
        * ETrue if phone is in offline mode, if phone is in offline mode
        * then GPRS IAPS are not used.
        */
        TBool                        iOffline;
        
        /**
        * Used to store iCurrentIap feasiblity status in GetUsedIap procedure.
        */
        TBool                        iCurrentIapIsFeasible;

        /**
        * Used to store if WLAN iap available in other snap when GPRS iap selected.
        */
        TBool                       iWlanWhenGPRS;

    };

#endif // __CIPTVNETWORKSELECTION_H__
        
