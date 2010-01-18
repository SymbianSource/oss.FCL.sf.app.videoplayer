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
* Description:   definitions for action handler plugin dll*
*/


#ifndef VIDEOAHPLUGINCONSTANTS_H_
#define VIDEOAHPLUGINCONSTANTS_H_

#include <e32def.h>


// command names, also used in XML configuration
_LIT( KVcxNsExtLaunchVideoCmd,        "ELaunchVideoToPlayer");
_LIT( KVcxNsExtLaunchLastWatchedCmd,  "ELaunchLastWatched");
_LIT( KVcxNsExtLaunchLiveTVLastWatch, "EIPTVMessageLastWatched");
_LIT( KVcxNsExtOpenStorageCmd,        "EOpenVideoStorage");
_LIT( KVcxNsExtInternetVideoCmd,      "EOpenInternetVideos");
_LIT( KVcxNsExtServiceCatalCmd,       "EOpenServiceCatalogue");
_LIT( KVcxNsExtVideoShopCmd,          "EOpenVideoShop");
_LIT( KVcxNsExtLaunchVCSettingsCmd,   "EOpenVCSettings");
_LIT( KVcxNsExtOpenLiveTVCmd,         "EOpenLiveTV");
_LIT( KVcxNsExtLoadTvVideoSuite,      "ELoadTvVideoSuite" );
_LIT( KVcxNsExtUnloadTvVideoSuite,    "EUnloadTvVideoSuite" );

// Mediator defs.
const TInt KVcxNsAhMediatorDomain     = 0x20016B9E;
const TInt KVcxNsAhMediatorCategory   = 0x1;
const TInt KVcxNsAhEventSuiteLoaded   = 0x1;
const TInt KVcxNsAhEventSuiteUnloaded = 0x2;
const TInt KVcxNsAhEventVersion       = 1;

#endif /*VIDEOAHPLUGINCONSTANTS_H_*/

// End of file
