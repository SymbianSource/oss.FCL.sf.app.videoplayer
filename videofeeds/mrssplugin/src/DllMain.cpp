/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    RSS plugin entry point*
*/




#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "IptvEngineUids.h"
#include "CIptvRssPlugin.h"

// --------------------------------------------------------------------------
// Provide a key pair value table.
// Used to identify the correct construction function
// for the requested interface.
// --------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    //lint -e{1924, 611} Errors inside system macro.
    IMPLEMENTATION_PROXY_ENTRY( KIptvRssPluginImplementationUid, CIptvRssPlugin::NewL )
    };

// --------------------------------------------------------------------------
// Return an instance of the proxy table.
// --------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

#ifndef EKA2

// Standard Symbian OS DLL entry point.
// DO NOT REMOVE.
TBool E32Dll( TDllReason )
    {
    return ETrue;
    }
#endif // EKA2
