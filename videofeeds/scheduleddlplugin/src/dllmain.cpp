/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Scheduled download plug-in entry point.*
*/




// INCLUDES
#include <ecom/implementationproxy.h>
#include "IptvEngineUids.h"

#include "iptvscheduleddownloadlugin.h"


// -----------------------------------------------------------------------------
// ImplementationTable
//
// -----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    { 
    {{KIptvScheduledDownloadPluginImplementationUid},
        (TProxyNewLPtr)CIptvScheduledDownloadPlugin::NewL }
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
//
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
        TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }


