/*
* Copyright (c) 2001 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __VCXNSSETTINGSENGINE_PAN__
#define __VCXNSSETTINGSENGINE_PAN__

#include <e32std.h>

_LIT(KPanicVcxNsSettingsEngine, "VcxNsSettingsEngine");

/** VCxNsSettings Engine panic codes */
enum TVcxNsSettingsEnginePanics 
    {
    EVcxNsSettingsEngineArgument = 1,
    EVcxNsSettingsEngineBadState
    // add further panics here
    };

#endif // __VCXNSSETTINGSENGINE_PAN__
