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
* Description:  This file contains testclass implementation.*
*/


// [INCLUDE FILES]

#include "VCXTestSmClient.h"
#include "CIptvService.h"
#include "CIptvServiceManagementClient.h"
#include "VCXTestLog.h"

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CVCXTestSmClient::NewL
// ----------------------------------------------------------------------------
//
CVCXTestSmClient* CVCXTestSmClient::NewL()
    {
    CVCXTestSmClient* self = new ( ELeave ) CVCXTestSmClient();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::~CVCXTestSmClient
// ----------------------------------------------------------------------------
//
CVCXTestSmClient::~CVCXTestSmClient()
    {
    delete iIptvServiceManagementClient;
    iIptvServiceManagementClient = NULL;
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::CVCXTestSmClient
// ----------------------------------------------------------------------------
//
CVCXTestSmClient::CVCXTestSmClient()
    {
    
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::CVCXTestSmClient
// ----------------------------------------------------------------------------
//
void CVCXTestSmClient::ConstructL()
    {
    iIptvServiceManagementClient = CIptvServiceManagementClient::NewL( *this );
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::CVCXTestSmClient
// ----------------------------------------------------------------------------
//
void CVCXTestSmClient::GetServiceIdL( TUint32& aServiceId, const TDesC& aName )
    {
    CDesC8ArraySeg* servicesArray( NULL );
    MIptvServiceManagementClientObserver::TRespStatus resp;
    TInt ret( KErrNone );
    
    TRAPD( err, ret = iIptvServiceManagementClient->GetServicesL( 
            0, CIptvServiceManagementClient::EDateAscending, servicesArray, resp ) );

    CleanupStack::PushL( servicesArray );
    
    VCXLOGLO4("CVCXTestSmClient:: GetServicesL err: %d, ret: %d, resp: %d", err, ret, resp );
    
    err = ( err != KErrNone ) ? err : ret;
    err = ( err == KErrNone && resp != MIptvServiceManagementClientObserver::ESucceeded ) ? KErrGeneral : KErrNone;
    
    if( err == KErrNone )
        {
        err = KErrNotFound;
        CIptvService* iptvService;
        for(TInt i = 0; i < servicesArray->MdcaCount(); i++)
            {
            iptvService = CIptvService::NewL();
            CleanupStack::PushL( iptvService );
            iptvService->SetL( servicesArray->MdcaPoint(i) );
            VCXLOGLO3("CVCXTestSmClient:: service id: %d, name: %S", iptvService->GetId(), &(iptvService->GetName()) );
            
            if( iptvService->GetName().CompareF( aName ) == KErrNone )
                {
                aServiceId = iptvService->GetId();
                CleanupStack::PopAndDestroy( iptvService );
                iptvService = NULL;
                err = KErrNone;
                break;
                }
            
            CleanupStack::PopAndDestroy( iptvService );
            iptvService = NULL;
            }
        }
    CleanupStack::PopAndDestroy( servicesArray );
    
    User::LeaveIfError( err );
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::CVCXTestSmClient
// ----------------------------------------------------------------------------
//
void CVCXTestSmClient::GetAnyServiceIdL( TUint32& aServiceId )
    {
    CDesC8ArraySeg* servicesArray( NULL );
    MIptvServiceManagementClientObserver::TRespStatus resp;
    TInt ret( KErrNone );
    
    TRAPD( err, ret = iIptvServiceManagementClient->GetServicesL( 
            0, CIptvServiceManagementClient::EDateAscending, servicesArray, resp ) );

    CleanupStack::PushL( servicesArray );
    
    VCXLOGLO4("CVCXTestSmClient:: GetServicesL err: %d, ret: %d, resp: %d", err, ret, resp );
    
    err = ( err != KErrNone ) ? err : ret;
    err = ( err == KErrNone && resp != MIptvServiceManagementClientObserver::ESucceeded ) ? KErrGeneral : KErrNone;
    
    TBool found( EFalse );
    
    if( err == KErrNone )
        {
        for( TInt i=0; i<servicesArray->MdcaCount(); i++ )
            {
            CIptvService* iptvService = CIptvService::NewL();
            CleanupStack::PushL( iptvService );
            iptvService->SetL( servicesArray->MdcaPoint( i ) );
            if( iptvService->GetType() == CIptvService::EVod || iptvService->GetType() == CIptvService::EVodCast )
                {
                aServiceId = iptvService->GetId();
                VCXLOGLO3("CVCXTestSmClient:: service id: %d, name: %S", iptvService->GetId(), &(iptvService->GetName()) );
                found = ETrue;
                }
            CleanupStack::PopAndDestroy( iptvService );
            }
        }
    CleanupStack::PopAndDestroy( servicesArray );

    if( !found )
        {
        err = KErrNotFound;
        }
    
    User::LeaveIfError( err );
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::NewL
// ------------------AddServiceResp---------------------------------------------------------
//
void CVCXTestSmClient::AddServiceResp(TRespStatus /*aRespStatus*/)
    {
    
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::UpdateServiceResp
// ----------------------------------------------------------------------------
//
void CVCXTestSmClient::UpdateServiceResp(TRespStatus /*aRespStatus*/)
    {
    
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::DeleteServiceResp
// ----------------------------------------------------------------------------
//
void CVCXTestSmClient::DeleteServiceResp(TRespStatus /*aRespStatus*/)
    {
    
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::GetServicesResp
// ----------------------------------------------------------------------------
//
void CVCXTestSmClient::GetServicesResp(TRespStatus /*aRespStatus*/, CDesC8ArraySeg* /*aServicesArray*/)
    {
    
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::GetUsedIapResp
// ----------------------------------------------------------------------------
//
void CVCXTestSmClient::GetUsedIapResp(TUint32 /*aIapId*/,
        const TDesC& /*aIapName*/,
        CIptvNetworkSelection::TConnectionPermission /*aConnectionPermission*/,
        TBool   /*aWlanWhenGPRS*/,
        CIptvNetworkSelection::TRespStatus /*aRespStatus*/
        )
    {
    
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::ServerShutdownResp
// ----------------------------------------------------------------------------
//
void CVCXTestSmClient::ServerShutdownResp(TRespStatus /*aRespStatus*/)
    {
    
    }

// ----------------------------------------------------------------------------
// CVCXTestSmClient::HandleSmEvent
// ----------------------------------------------------------------------------
//
void CVCXTestSmClient::HandleSmEvent(CIptvSmEvent& /*aEvent*/)
    {
    
    }
