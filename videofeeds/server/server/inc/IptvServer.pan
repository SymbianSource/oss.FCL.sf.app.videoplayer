/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Panic codes for IptvServer*
*/





#ifndef __IPTVSERVER_PAN__
#define __IPTVSERVER_PAN__

/** Panic Category */
_LIT(KIptvServer, "CIptvServer");
_LIT(KIptvServiceManagementClient, "CIptvServiceManagementClient");
_LIT(KIptvServiceManager, "CIptvServiceManager");
_LIT(KIptvNetworkSelection, "CIptvNetworkSelection");
_LIT(KIptvVodDownloadManager, "CIptvVodDownloadManager");
_LIT(KIptvVodDownloadMsgHandler, "CIptvVodDownloadMsgHandler");
_LIT(KIptvVodDlClient, "CIptvVodDlClient");
_LIT(KIptvEventListener, "CIptvEventListener");
_LIT(KIptvVodDlEventListener, "CIptvVodDlEventListener");
_LIT(KIptvEventGenerator, "CIptvEventGenerator");
_LIT(KIptvEpgDb, "CIptvEpgDb");
_LIT(KIptvMyVideosDb, "CIptvMyVideosDb");
_LIT(KIptvMyVideosMsgHandler, "CIptvMyVideosMsgHandler");
_LIT(KIptvMyVideosManagerImpl, "CIptvMyVideosManagerImpl");

/* IptvServer panic codes.                                */
/* Use these or ones in e32err.h (they are all negative). */
enum TIptvServerPanic
    {
    EBadRequest = 1,
    EBadDescriptor,
    EMainSchedulerError,
    ECreateTrapCleanup,
    EIptvEngineServerBadState,
    EIptvClientBadState,
    EFailedToCreateEpgDatabase,
    EIptvFailedToCreateRootCategory,
    EIptvUnknownMessage,
    EIptvSmCouldNotCreateServicesFile,
    EIptvSmCouldNotCreatePasswordsFile,
    EIptvSmCouldNotOpenServicesDb,
    EIptvSmCouldNotOpenPasswordsDb,
    EIptvSmCouldNotCreateWritableRootPath,
    EMaxErrorsExceeded,
    EBadState
    };

#endif // __IPTVSERVER_PAN__

// End of File