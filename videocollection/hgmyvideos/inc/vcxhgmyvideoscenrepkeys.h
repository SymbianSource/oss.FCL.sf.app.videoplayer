
/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/



#ifndef VCXHGMYVIDEOSMAINVIEWCENREPKEYS_H
#define VCXHGMYVIDEOSMAINVIEWCENREPKEYS_H


enum TMyVideosListItemType
	{
	EMyVideosListItemTypeEmpty = 0,
	EMyVideosListItemTypeLastWatched,
	EMyVideosListItemTypeCapturedVideos,
	EMyVideosListItemTypeOtherVideos,
	EMyVideosListItemTypeExtra1,
	EMyVideosListItemTypeExtra2,
	EMyVideosListItemTypeExtra3
	};

// CONSTANTS 

// CenRep: UID
const TUid KMyVideosMainViewCenRepUid = { 0x20021191 };

// CenRep: Keys
const TInt KCRVideoPlayerItemLocation1            = 0x01;
const TInt KCRVideoPlayerItemLocation2            = 0x02;
const TInt KCRVideoPlayerItemLocation3            = 0x03;
const TInt KCRVideoPlayerItemLocation4            = 0x04;
const TInt KCRVideoPlayerItemLocation5            = 0x05;
const TInt KCRVideoPlayerItemLocation6            = 0x06;
const TInt KCRVideoPlayerItemLocation7            = 0x07;

const TInt KCRVideoPlayerCapturedVideosIconPath   = 0x0A;
const TInt KCRVideoPlayerOtherVideosIconPath      = 0x0B;

const TInt KCRVideoPlayerExtraItem1IconPath       = 0x10;
const TInt KCRVideoPlayerExtraItem1Title          = 0x11;
const TInt KCRVideoPlayerExtraItem1Text           = 0x12;
const TInt KCRVideoPlayerExtraItem1Url            = 0x13;
const TInt KCRVideoPlayerExtraItem1Uid            = 0x14;

const TInt KCRVideoPlayerExtraItem2IconPath       = 0x20;
const TInt KCRVideoPlayerExtraItem2Title          = 0x21;
const TInt KCRVideoPlayerExtraItem2Text           = 0x22;
const TInt KCRVideoPlayerExtraItem2Url            = 0x23;
const TInt KCRVideoPlayerExtraItem2Uid            = 0x24;

const TInt KCRVideoPlayerExtraItem3IconPath       = 0x30;
const TInt KCRVideoPlayerExtraItem3Title          = 0x31;
const TInt KCRVideoPlayerExtraItem3Text           = 0x32;
const TInt KCRVideoPlayerExtraItem3Url            = 0x33;
const TInt KCRVideoPlayerExtraItem3Uid            = 0x34;

#endif //VCXHGMYVIDEOSMAINVIEWCENREPKEYS_H

// End of File
