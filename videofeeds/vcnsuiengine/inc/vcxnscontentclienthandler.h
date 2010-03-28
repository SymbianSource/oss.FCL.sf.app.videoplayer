/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef VCXNSCONTENTCLIENTHANDLER_H_
#define VCXNSCONTENTCLIENTHANDLER_H_

// INCLUDES
#include <e32base.h>

#include "MIptvVodContentClientObserver.h"
#include <ipvideo/vcxconnutilengineobserver.h>

// FORWARD DECLARATIONS
class CIptvVodContentClient;
class CVcxNsUiEngine;
class CVcxConnectionUtility;

// TYPEDEFS
typedef TUint32 TIptvServiceId;

/**
* Class CVcxNsContentClientHandler handles content client interactions.
*
* @lib vcxnsuiengine.lib
*/
class CVcxNsContentClientHandler : public CBase,
                                   public MIptvVodContentClientObserver,
                                   public MConnUtilEngineObserver 
    {

public:

    /**
     * Constructor.
     * 
     * @param aUiEngine Ui engine
     * @param aConnUtil Connection util
     */
    static CVcxNsContentClientHandler* NewL( 
                    CVcxNsUiEngine& aUiEngine, CVcxConnectionUtility& aConnUtil );
    
    virtual ~CVcxNsContentClientHandler();

    /**
     * Returns content management client.
     * @return Pointer to CIptvVodContentClient object.
     */
    CIptvVodContentClient* GetVodContentClientL( TUint32 aServiceId );
    
    /**
     * Updates the given service's ecg.
     * This causes the iap to be reasked if necessary.
     * 
     * @param   aServiceId  Service id
     * @return Error code
     */
    TInt UpdateEcgL( TUint32 aServiceId );

    /**
     * Stores mpx media id to content database.
     * @param    aServiceId          Service id
     * @param    aContentId          Content id
     * @param    aCaIndex            Content access index
     * @param    aMpxId              Mpx id.
     */
    void StoreMpxIdToEcgDbL( TUint32 aServiceId,
                             TUint32 aContentId,
                             TUint32 aCaIndex,
                             TUint32 aMpxId );
    

    /**
     * Stores Last Playing Position to content database.
     * @param    aServiceId          Service id
     * @param    aContentId          Content id
     * @param    aCaIndex            Content access index
     * @param    aPos                Last play pos.
     */
    void StoreLastPlayPosL( TUint32 aServiceId,
                            TUint32 aContentId,
                            TUint32 aCaIndex,
                            TReal32 aPos );

// From MIptvVodContentClientObserver

    /**
    * Messages from EPG manager are handled here. Messages are defined in CIptvUtil.h
    * @since            Engine 1.1
    * @param aMsg       Message 
    * @param aInfo      Info 
    * @param aServiceId Service id.
    */
    void HandleEpgManagerMsgL( TInt aMsg, TInt aInfo, TIptvServiceId aServiceId );   
  
// from MConnUtilEngineObserver
    
    /**
    * This method is called by the vcxconnectionutility when 
    * it has received a notification about the availibility of
    * "better" iap in the network. This might happen  
    * when WLAN defined in the corresponding destination becomes
    * available or when WLAN carrier is lost and there is defined
    * 3G/gprs iap (or another available WLAN) in the destination
    * being used.
    *
    * Client don't have to accept the changing of the iap. If client 
    * returns EFalse notifying it does not want to roam, then the 
    * iap will not be changed.
    *
    * This however might result in diconnecting, if the old iap is 
    * no longer available, like it would happen when WLAN carrier is lost
    *
    * @return TBool  ETrue: roaming accepted, EFalse: Roaming not accepted
    */
    TBool RequestIsRoamingAllowedL();
    
    /**
     * This method is called by the vcxconnectionutility when 
     * there have been some change in the iap currently using:
     * 
     * Connection lost: this happens when for example WLAN carrier 
     * is lost and there is no other iap to use to or when corresponding
     * connection is closed by some other application.
     *
     * Iap change by the roaming: if client has accepted roaming
     * vcxconnectionutility notifies about new iap is ready to use
     *
     * When client is being called by this method, it can request a
     * iap id by calling CVcxConnectionUtility::GetIap-
     */
    void IapChangedL();
    
    /**
     * Return iap query status.
     * 
     * @return ETrue if connection for service has been created or
     *         will be triggered via MIptvVodContentClientObserver
     *         interface, otherwise EFalse
     */
    TBool IapAsked( TUint32 aServiceId );
    
    /**
     * Set specified service iap query status.
     * 
     * @param aServiceId the service id, that value is to be set.
     * @param aSet Value to be set to iap asked flag
     */
    void SetIapAsked( TUint32 aServiceId, TBool aSet );
    
private:


    /**
     * Constructor.
     * 
     * @param aUiEngine Ui engine
     * @param aConnUtil Connection util
     */
    CVcxNsContentClientHandler( CVcxNsUiEngine& aUiEngine, 
                                CVcxConnectionUtility& aConnUtil );
    
    /**
     * Private 2nd phase construction.
     */
    void ConstructL();
    
// Data

    /**
     * Pointers to VOD Content Clients.
     * Own.
     */
    RPointerArray<CIptvVodContentClient> iVodContentClients;

    /**
     * Array of content client service id's. 
     */
    RArray<TInt> iVodContentClientIds;
    
    /**
     * Array of flags, if the iap query is needed for service
     * This must be set to ETrue by default. If user cancels in 
     * connection creation dialog, then set to EFalse
     */
    RArray<TBool> iIapAsked;

    /**
     * CVcxNsUiEngine
     */
    CVcxNsUiEngine& iUiEngine; 
    
    /**
     * Reference to connection utility
     */
    CVcxConnectionUtility& iConnUtil;
    
    };

#endif /*VCXNSCONTENTCLIENTHANDLER_H_*/
