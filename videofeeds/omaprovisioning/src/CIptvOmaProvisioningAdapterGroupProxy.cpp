/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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




// INCLUDE FILES
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "CIptvOmaProvisioningAdapter.h"
#include "IptvOmaProvisioningAdapterUIDs.h"

// EXTERNAL DATA STRUCTURES

// CONSTANTS
const TImplementationProxy KImplementationTable[] = 
{
	IMPLEMENTATION_PROXY_ENTRY( PROVISIONING_IPTV_ADAPTER_UID, CIptvOmaProvisioningAdapter::NewL )
};

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// ========================== OTHER EXPORTED FUNCTIONS =========================
// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns: TImplementationProxy*: pointer to TImplementationProxy
// -----------------------------------------------------------------------------

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
{
    aTableCount = sizeof( KImplementationTable ) / sizeof( TImplementationProxy );
    return KImplementationTable;
}

//  End of File  
