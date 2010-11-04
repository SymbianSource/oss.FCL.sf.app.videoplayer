/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECOM proxy table for this plugin*
*/


// Version : %version: 2 %



// System includes
#include <e32std.h>
#include <implementationproxy.h>

// User includes
#include "GSVideoPlugin.h"

// Constants
const TImplementationProxy KGSVideoPluginImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY( 0x10275069,	CGSVideoPlugin::NewL )
	};


// ---------------------------------------------------------------------------
// ImplementationGroupProxy
// 
// Gate/factory function
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
                                                  TInt& aTableCount )
	{
	aTableCount = sizeof( KGSVideoPluginImplementationTable ) 
        / sizeof( TImplementationProxy );
	return KGSVideoPluginImplementationTable;
	}
