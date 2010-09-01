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
* Description:    contains mediator domain, category and event ids for raising*
*/




#ifndef VCXNSPLAYERWRAPPERMEDIATOR_
#define VCXNSPLAYERWRAPPERMEDIATOR_


const TUid KVcxNsVideoCenterMediatorDomain = { 0x05A04D9E };

/**
 * Category and events for local and stream playback cases.
 */
const TUid KVcxNsPlayerGeneralCategory = { 0x05A04D9E };
const TInt KVcxNsMediatorEventPlayerLastWatch = 0x1;
const TInt KVcxNsMediatorEventPreloadedLastWatch = 0x2;
const TInt KVcxNsMediatorPlayerVersion = 1;

/**
 * Category and events for IPTV cases.
 */
const TUid KVcxNsIPTVGeneralCategory = { 0x102750F7 };
const TInt KVcxNsMediatorEventIPTVLastWatch = 0x1;
const TInt KVcxNsMediatorIPTVVersion   = 1;

/**
 * Maximum length of the view title, value taken from vcxnsiptvcommon.h.
 */
const TInt KVcxNsLiveTVMaxTitleName( 272 );

/**
 *
 */
class TIPTVMediatorMsg
    {
    public:

        /**
         *
         */
        TBuf< KVcxNsLiveTVMaxTitleName > iChannelName;

        /**
         *
         */
        TBuf< KMaxPath > iIconPath;
    };


#endif /*VCXNSPLAYERWRAPPERMEDIATOR_*/
