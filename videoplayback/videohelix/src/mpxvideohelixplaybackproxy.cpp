/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Standard proxy of the ECOM plugin
 *
*/

// Version : %version: 3 %



#include <implementationproxy.h>
#include "mpxvideohelixplayback.h"

#if ( ! defined IMPLEMENTATION_PROXY_ENTRY )
typedef TAny* TProxyNewLPtr;
#define IMPLEMENTATION_PROXY_ENTRY(aUid,aFuncPtr) \
            { {aUid}, (TProxyNewLPtr)(aFuncPtr) }
#endif

// ----------------------------------------------------------------------------
// The list of implementations
// ----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    { IMPLEMENTATION_PROXY_ENTRY( 0x10282551, CMPXVideoHelixPlayback::NewL ) };

// ----------------------------------------------------------------------------
// The proxy of implementations
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

// End of File
