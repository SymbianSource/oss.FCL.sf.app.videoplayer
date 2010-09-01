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
* Description: 
*
*/




#include <bldvariant.hrh>
#include "IptvDebug.h"

#include "CIptvServiceManagementClient.h"
#include "vcxnsserviceclienthandler.h"
#include "vcxnsuiengine.h"
#include "vcxnseventrouter.h"


// -----------------------------------------------------------------------------
// CVcxNsServiceClientHandler::CVcxNsServiceClientHandler()
// -----------------------------------------------------------------------------
//
CVcxNsServiceClientHandler::CVcxNsServiceClientHandler( CVcxNsUiEngine& aUiEngine ):
    iUiEngine( aUiEngine )
    {
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceClientHandler::NewL()
// -----------------------------------------------------------------------------
//
CVcxNsServiceClientHandler* CVcxNsServiceClientHandler::NewL( CVcxNsUiEngine& aUiEngine )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceClientHandler::NewL()");
    
    CVcxNsServiceClientHandler* self = new (ELeave) CVcxNsServiceClientHandler( aUiEngine );

    return self;    
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceClientHandler::~CVcxNsServiceClientHandler()
// -----------------------------------------------------------------------------
//
CVcxNsServiceClientHandler::~CVcxNsServiceClientHandler()
    {
    delete iIptvServiceManagementClient;
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceClientHandler::GetServiceManagementClientL()
// -----------------------------------------------------------------------------
//
CIptvServiceManagementClient* CVcxNsServiceClientHandler::GetServiceManagementClientL()
    {
    if ( ! iIptvServiceManagementClient )
        {
        iIptvServiceManagementClient = CIptvServiceManagementClient::NewL( *this );
        }
    
    return iIptvServiceManagementClient;    
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceClientHandler::HandleSmEvent()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceClientHandler::HandleSmEvent( CIptvSmEvent& aEvent )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceClientHandler::HandleSmEvent()");
    //implement better handling for events
    //Do not always send RefreshView!
    iUiEngine.GetEventRouter()->HandleSmEvent( aEvent );
    }


// -----------------------------------------------------------------------------
// CVcxNsServiceClientHandler::DeleteFeedL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceClientHandler::DeleteFeedL( TUint32 aId )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceClientHandler::DeleteFeedL()");
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    GetServiceManagementClientL()->DeleteServiceL( aId, respStatus );
    }

// -----------------------------------------------------------------------------
// CVcxNsServiceClientHandler::UpdateFeedL()
// -----------------------------------------------------------------------------
//
void CVcxNsServiceClientHandler::UpdateFeedL( CIptvService& aService )
    {
    IPTVLOGSTRING_HIGH_LEVEL("UI Engine ## CVcxNsServiceClientHandler::UpdateFeedL()");
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    GetServiceManagementClientL()->UpdateServiceL( aService, respStatus );
    }

