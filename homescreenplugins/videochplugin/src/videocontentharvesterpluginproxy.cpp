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
* Description:    Plugin implementation proxy*
*/




// INCLUDE FILES
#include <e32std.h>
#include <e32def.h>
#include <e32cmn.h>
#include <ecom/implementationproxy.h>

#include "videocontentharvesterplugin.h"
#include "videocontentharvesterplugin.hrh"


// ============================= LOCAL FUNCTIONS ===============================

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KVcxNsContentHarvesterPluginUid, 
                                    CVcxNsContentHarvesterPlugin::NewL )
    };

// ----------------------------------------------------------------------------
// Exported proxy for instantiation method resolution
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
                                                TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
