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
* Description:    Custom message TUid constant value definitions.*
*/




#ifndef IPTVCUSTOMMESSAGEIDS_H
#define IPTVCUSTOMMESSAGEIDS_H

// Custom message TUid constant values that are used to pass information
// about exit type between VoD & Main, My Videos & Main and from Main to
// Mobile TV application.
enum
    {
    EIptvEmbeddedAppBack = 1,
    EIptvEmbeddedAppExit,
    EIptvVodAppBackAndSettings,
    EIptvVodAppBackAndVodSubscriptions,
    EIptvVodAppBackAndVodcastSubscriptions,
    EIptvVodAppBackAndSettingsAddService,
    EIptvVodAppBackAndLiveTv,
    EIptvMyVideosAppBackAndSettings,
    EIptvLiveTvAppBackAndVod
    };

#endif // IPTVCUSTOMMESSAGEIDS_H
