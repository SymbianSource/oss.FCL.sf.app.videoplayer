/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    header file for MIptvServiceManagementClientObserver class*
*/



#ifndef __MIPTVSERVICEMANAGEMENTCLIENTOBSERVER_H__
#define __MIPTVSERVICEMANAGEMENTCLIENTOBSERVER_H__

#include "CIptvNetworkSelection.h"
#include "CIptvService.h"

class CDesC8ArraySeg;
class CIptvSmEvent;

// CLASS DECLARATION

/**
* MIptvServiceManagementClientObserver
* class specifies the interface for CIptvServiceManagementClient callback functions.
* Client must inherit this and implement the virtual functions.
*/
class MIptvServiceManagementClientObserver
    {
    
    public: // Enumerations
    
        /**
        * TRespStatus.
        * Response statuses for requests.
        */
        enum TRespStatus
            {
            ESucceeded,
            EServiceNotFound,
            EGeneralError,
            ECancelled,
            EEmpty            
            };

    public:

        /**
        * AddServiceResp.
        * Callback function for AddServiceReq operation.
        * @param aRespStatus
        */
        virtual void AddServiceResp(TRespStatus aRespStatus) = 0;

        /**
        * UpdateServiceResp.
        * Callback function for UpdateServiceReq.
        * @param aRespStatus
        */
        virtual void UpdateServiceResp(TRespStatus aRespStatus) = 0;


        /**
        * DeleteServiceResp.
        * Callback function for DeleteServiceReq operation.
        * @param aRespStatus
        */        
        virtual void DeleteServiceResp(TRespStatus aRespStatus) = 0;

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
        virtual void GetServicesResp(TRespStatus aRespStatus, 
                                     CDesC8ArraySeg* aServicesArray) = 0;

        /**
        * GetUsedIapResp.
        * Callback function for GetUsedIapReq() method.
        * @param aIapId IAP ID which client should use when connecting to service.
        * @param aIapName
        * @param aConnectionPermission
        * @param aWlanWhenGPRS
        * @param aRespStatus If != ESucceeded, aIapId, aIapName and aConnectionPermission
        *                    do not contain valid data.
        */
        virtual void GetUsedIapResp(TUint32 aIapId,
                                    const TDesC& aIapName,
                                    CIptvNetworkSelection::TConnectionPermission aConnectionPermission,
                                    TBool   aWlanWhenGPRS,
                                    CIptvNetworkSelection::TRespStatus aRespStatus
                                    ) = 0;

        /**
        * ServerShutdownResp.
        * Callback function for ServerShutdownReq operation.
        * @param aRespStatus
        */
        virtual void ServerShutdownResp(TRespStatus aRespStatus) = 0;

        /**
        * Service Manager generated events handle function.
        * @param aEvent
        */        
        virtual void HandleSmEvent(CIptvSmEvent& aEvent) = 0;

    };

#endif // __MIPTVSERVICEMANAGEMENTCLIENTOBSERVER_H__
