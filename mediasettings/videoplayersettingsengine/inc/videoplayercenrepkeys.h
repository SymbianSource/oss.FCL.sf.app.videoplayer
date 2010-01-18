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
* Description:   App settings handles storing and loading of application settings*
*/



#ifndef VIDEOPLAYERCENREPKEYS_H
#define VIDEOPLAYERCENREPKEYS_H

// INCLUDES


// CONSTANTS 

// CenRep: UIDs
//
const TUid KIptvCenRepUid                       = { 0x102750E2 };
const TUid KIptvAlrCenRepUid                    = { 0x2000B438 };

// CenRep: Keys
//
const TInt KIptvCenRepPreferredMemoryKey        = 0x1;
const TInt KIptvCenRepConnectionsKey            = 0x2;
const TInt KIptvCenRepCellularKey               = 0x3;
const TInt KIptvCenRepVoDThumbnailsKey          = 0x4;
const TInt KIptvCenRepActiveServiceIdKey        = 0x5;
const TInt KIptvCenRepVideoPlayerVolumeKey      = 0x6;
const TInt KIptvCenRepMainAppServiceFocusKey    = 0x7;
const TInt KIptvCenRepFirstTimeKey              = 0x8;
const TInt KIptvCenRepServiceUpdatedKey         = 0x9;
const TInt KIptvCenRepUpdatedServiceIdKey       = 0xA;
const TInt KIptvCenRepAPModeKey                 = 0xB;
const TInt KIptvCenRepParentControlKey          = 0xC;

const TInt KIptvCenRepUsedSnapIdKey             = 0x1;
const TInt KIptvCenRepAllowUseGprsKey           = 0x2;
const TInt KIptvCenRepAllowRoamingKey           = 0x3;
const TInt KIptvCenRepMaxUsedMemoryKey          = 0x4;
const TInt KIptvCenRepIadUpdateCheckTimeKey     = 0xA;

// CenRep: Key values
//
const TInt KIptvCenRepPreferredMemoryKeyPhone   = 0;
const TInt KIptvCenRepPreferredMemoryKeyMmc     = 1;
const TInt KIptvCenRepParentControlKeyOff       = 0;
const TInt KIptvCenRepAPModeAlwaysAsk           = 0;
const TInt KIptvCenRepAPModeUserDefined         = 1;

const TInt KIptvCenRepConnectionKeyValueYes     = 1;
const TInt KIptvCenRepConnectionKeyValueNo      = 0;
const TInt KIptvCenRepSnapAlwaysAsk             = 0;

// CenRep: Default values
//
const TInt KIptvCenRepPreferredMemoryKeyDefault = KIptvCenRepPreferredMemoryKeyMmc;
const TInt KIptvCenRepFirstTimeKeyDefault       = 8;
const TInt KIptvCenRepParentControlKeyDefault   = KIptvCenRepParentControlKeyOff;
const TInt KIptvCenRepAPModeKeyDefault          = KIptvCenRepAPModeUserDefined;

#endif //VIDEOPLAYERCENREPKEYS_H

// End of File
