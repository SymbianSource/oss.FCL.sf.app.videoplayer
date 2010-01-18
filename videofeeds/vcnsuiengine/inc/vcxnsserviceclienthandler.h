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




#ifndef VCXNSSERVICECLIENTHANDLER_H_
#define VCXNSSERVICECLIENTHANDLER_H_

// INCLUDES
#include <e32base.h>

#include "MIptvServiceManagementClientObserver.h"

// FORWARD DECLARATIONS
class CIptvServiceManagementClient;
class CVcxNsUiEngine;

/**
* Class CVcxNsServiceClientHandler handles service client interactions.
*
* @lib vcxnsuiengine.lib
*/
class CVcxNsServiceClientHandler : public CBase,
                                   public MIptvServiceManagementClientObserver
    {

public:

    /**
     * Constructor.
     * 
     * @param aUiEngine Ui engine
     */
    static CVcxNsServiceClientHandler* CVcxNsServiceClientHandler::NewL( CVcxNsUiEngine& aUiEngine );
    
    virtual ~CVcxNsServiceClientHandler();

    /**
     * Returns service management client.
     * @return Pointer to CIptvServiceManagementClient object.
     */
    CIptvServiceManagementClient* GetServiceManagementClientL();

// From MIptvServiceManagementClientObserver

    /**
     * Not used. Mandatory to implement as Service Manager client.
     */
    void AddServiceResp( TRespStatus /*aRespStatus*/ ) { ; }

    /**
     * Not used. Mandatory to implement as Service Manager client.
     */
    void UpdateServiceResp( TRespStatus /*aRespStatus*/ ) { ; }

    /**
     * Not used. Mandatory to implement as Service Manager client.
     */
    void DeleteServiceResp( TRespStatus /*aRespStatus*/ ) { ; }
    
    /**
     * Not used. Mandatory to implement as Service Manager client.
     */
    void GetServicesResp( TRespStatus /*aRespStatus*/, 
                          CDesC8ArraySeg* /*aServicesArray*/ ) { ; }

    /**
     * Not used. Mandatory to implement as Service Manager client.
     */
    void GetUsedIapResp( TUint32 /*aIapId*/,
                         const TDesC& /*aIapName*/,
                         CIptvNetworkSelection::TConnectionPermission /*aConnectionPermission*/,
                         TBool /*aWlanWhenGPRS*/,
                         CIptvNetworkSelection::TRespStatus /*aRespStatus*/ ) { ; }

    /**
     * Not used. Mandatory to implement as Service Manager client.
     */
    void ServerShutdownResp( TRespStatus /*aRespStatus*/ ) { ; }
    
    /**
     * Used to forward events of service db changes to observing clients..
     * @param aEvent event
     */
    void HandleSmEvent( CIptvSmEvent& aEvent ); 
   
public:
    
    /**
     * Delete service from database
     * @param aId Service id 
     */
    void DeleteFeedL( TUint32 aId );
    
    /**
     * Update feed.
     * @param aService Service data to update 
     */
    void UpdateFeedL( CIptvService& aService );
    
private:

    /**
     * Constructor.
     * 
     * @param aUiEngine Ui engine
     */
    CVcxNsServiceClientHandler( CVcxNsUiEngine& aUiEngine );
    
// Data

    /**
     * CIptvServiceManagementClient
     */
    CIptvServiceManagementClient* iIptvServiceManagementClient;
    
    /**
     * CVcxNsUiEngine
     */
    CVcxNsUiEngine& iUiEngine; 

    };

#endif /*VCXNSSERVICECLIENTHANDLER_H_*/
