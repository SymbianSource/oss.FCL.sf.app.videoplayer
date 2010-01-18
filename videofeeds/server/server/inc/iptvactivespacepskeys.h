/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    P & S category and keys for new video and service count.*
*/




#ifndef IPTVACTIVESPACEPSKEYS_H
#define IPTVACTIVESPACEPSKEYS_H

// Publish & subscribe category uid for IptvServer
const TUid KPSUidIptvServer = {0x102750D5};

/**
* Indicates new video count of video storage
* @type RProperty::EInt
*/
const TUint32 KIptvAsNewVideoCount = 0x00000001;

/**
* Indicates VodCast feed count
* @type RProperty::EInt
*/
const TUint32 KIptvAsVodCastFeedCount = 0x00000002;

/**
* Indicates newest video name
* @type RProperty::ELargeText
*/
const TUint32 KIptvNewestVideoName = 0x00000003;

/**
* Indicates newest video thumbnail path
* @type RProperty::ELargeText
*/
const TUint32 KIptvNewestVideoThumbnailPath = 0x00000004;

#if !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)

/**
* Indicates Live tv (Internet tv) service count
* @type RProperty::EInt
*/
const TUint32 KIptvLiveTVServiceCount = 0x00000005;

#endif // !defined(__SERIES60_30__) && !defined(__SERIES60_31__) && !defined(__SERIES60_32__)

#endif      // IPTVACTIVESPACEPSKEYS_H
