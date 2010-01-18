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
* Description:    CVcxNsChPublisher class declaration*
*/




#ifndef _VCXNSCHIPTVSERVERCLIENT_H
#define _VCXNSCHIPTVSERVERCLIENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "MIptvServiceManagementClientObserver.h"
#include <MediatorEventConsumer.h>

// FWD DECLS
class CIptvServiceManagementClient;
class CVcxNsChPublishableData;
class CVcxNsChPublisher;
class CVcxNsChMCategoryListener;

// CLASS DECLARATION

/**
 *  CVcxNsChIptvServerClient 
 *
 *  Does content harvester plugin's videocenter dserver related actions.
 *
 *  @lib vcxnscontentharverterplugin.dll
 */
class CVcxNsChIptvServerClient : public CBase, 
                                 public MIptvServiceManagementClientObserver,
                                 public MMediatorEventObserver
    {
    
public: // construction / destruction

    /**
     * Destructor
     * 
     */
    ~CVcxNsChIptvServerClient();

    /**
     * constructor
     * 
     * @param aPublisher pointer to CH plugins CP publisher
     * @param aPublishableData pointer to CH plugin data
     * 
     * @return CVcxNsChIptvServerClient*
     */
    static CVcxNsChIptvServerClient* NewL( CVcxNsChPublisher* aPublisher,
                                           CVcxNsChPublishableData* aPublishableData );
    
private:

    /**
     * constructor
     * 
     * @param aPublisher pointer to CH plugins CP publisher
     * @param aPublishableData pointer to CH plugin data
     */
    CVcxNsChIptvServerClient( CVcxNsChPublisher* aPublisher,
                              CVcxNsChPublishableData* aPublishableData ); 
    
    /**
     * default constructor, not implemented
     * 
     */
    CVcxNsChIptvServerClient();

    /**
     * Symbian 2nd phase constructor
     * 
     */
    void ConstructL();

    
public: // FROM SM CLIENT
    
    /**
     * empty implementation
     */ 
    void AddServiceResp( TRespStatus ){};
    
    /**
     * empty implementation
     */ 
    void UpdateServiceResp( TRespStatus ){};
    
    /**
     * empty implementation
     */
    void DeleteServiceResp( TRespStatus ){};
    
    /**
     * empty implementation
     */
    void GetServicesResp( TRespStatus, CDesC8ArraySeg* ){};
    
    /**
     * empty implementation
     */
    void GetUsedIapResp( TUint32, const TDesC&, CIptvNetworkSelection::
                         TConnectionPermission, TBool, CIptvNetworkSelection::TRespStatus ){};
    
    /**
     * empty implementation
     */
    void ServerShutdownResp( TRespStatus ){};
    
    /**
     * Method is used to check if the preloaded service is 
     * deleted (unsubscribed) or added (subscribed).
     * If deleted, item's data is deleted from the CP 
     * If added back item's data is returned to CP
     * 
     * @param aEvent event to check
     * 
     * 
     */
    void HandleSmEvent( CIptvSmEvent& aEvent );
    
public: // From MMediatorEventObserver
    
    /** 
     * Catches the mediator events. In this context we handle
     * 
     */
    void MediatorEventL( TUid aDomain,
                         TUid aCategory, 
                         TInt aEventId, 
                         const TDesC8& aData );
    
public: // new methods
    
    /**
     * Method creates CIptvServiceManagementClient object
     * to startup IPTV server.
     */
    void StartIPTVServerInterfaceL();
    
    /**
     * Deletes CIptvServiceManagementClient object.
     * IPTV server is stopped if there are no other 
     * instances using it.
     */
    void StopIPTVServerInterface();
    
    /**
     * Method is used to get an id from the service,
     * based on the uri provided.
     * This method should only be called once during 
     * plugin startup.
     * 
     * @param aServiceUri uri to be used to check the service
     * 
     * @return service id or KErrNotFound, if service is not found
     * 
     */
    TInt GetServiceGroupIdL( const TDesC& aServiceUri );
    
    

 
private: // Data
    
    CIptvServiceManagementClient* iSmClient;
    
    /**
     * Publishable data container. Not owned. 
     */
    CVcxNsChPublishableData* iPublishableData;
    
    /**
     * Publisher object used to publish data from CVcxNsChPublishableData
     * to service api. Not owned. 
     */
    CVcxNsChPublisher* iPublisher;
    
    /**
     * Mediator helper object for registering and subscribing 
     * matrix action handler mediator events. Owned.
     */
    CVcxNsChMCategoryListener* iAhCategoryListener; 
    
    };

#endif // _VCXNSCHIPTVSERVERCLIENT_H
