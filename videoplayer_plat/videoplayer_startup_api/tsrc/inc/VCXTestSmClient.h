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
* Description:  STIF testclass declaration*
*/


#ifndef VCXTESTSMCLIENT_H_
#define VCXTESTSMCLIENT_H_

//  INCLUDES
#include <e32base.h>
#include "MIptvServiceManagementClientObserver.h"

// CONSTANTS

// DATA TYPES

// FORWARD DECLARATIONS
class CIptvServiceManagementClient;

// CLASS DECLARATION

NONSHARABLE_CLASS(CVCXTestSmClient) : public CBase, public MIptvServiceManagementClientObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CVCXTestSmClient* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CVCXTestSmClient();

    private: // New functions

        /**
        * C++ default constructor.
        */
        CVCXTestSmClient();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // New functions
        
        void GetServiceIdL( TUint32& aServiceId, const TDesC& aName );
        void GetAnyServiceIdL( TUint32& aServiceId );
        
    public: // Functions from base classes
        
        virtual void AddServiceResp(TRespStatus aRespStatus);
        virtual void UpdateServiceResp(TRespStatus aRespStatus);
        virtual void DeleteServiceResp(TRespStatus aRespStatus);
        virtual void GetServicesResp(TRespStatus aRespStatus, CDesC8ArraySeg* aServicesArray);
        virtual void GetUsedIapResp(TUint32 aIapId,
                const TDesC& aIapName,
                CIptvNetworkSelection::TConnectionPermission aConnectionPermission,
                TBool   aWlanWhenGPRS,
                CIptvNetworkSelection::TRespStatus aRespStatus
                );
        virtual void ServerShutdownResp(TRespStatus aRespStatus);
        virtual void HandleSmEvent(CIptvSmEvent& aEvent);

    private: // Data
        CIptvServiceManagementClient* iIptvServiceManagementClient;
    };

#endif /*VCXTESTSMCLIENT_H_*/


