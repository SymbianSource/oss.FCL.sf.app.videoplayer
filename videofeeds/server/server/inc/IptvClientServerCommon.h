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
* Description:    General client api definitions.*
*/




#ifndef __IPTVCLIENTSERVERCOMMON_H__
#define __IPTVCLIENTSERVERCOMMON_H__

#include <e32base.h>

_LIT( KIptvServerName, "IptvServer" ); // Server name
_LIT( KIptvServerSemaphoreName, "IptvServerSemaphore" );
_LIT( KIptvServerFileName, "IptvServer.exe" );

// The server version. A version must be specified when
// creating a session with the server.
const TUint KIptvServMajorVersionNumber( 0 );
const TUint KIptvServMinorVersionNumber( 1 );
const TUint KIptvServBuildVersionNumber( 1 );

const TUint32 KIptvVodGetUpdateTimeRequestSize( 8 );
const TUint32 KIptvSizeRequestSize( 4 );
const TUint32 KIptvVodObserverRequestSize( 8 );
const TUint32 KIptvMyVideosObserverRequestSize( 12 );
const TUint32 KIptvVodSendLastPositionSize( 12 );
const TUint32 KIptvVodSendMpxIdSize( 12 );
const TUint32 KIptvChannelOrderReqSize( 64 );
const TUint32 KIptvVodUpdateCheckRequestSize ( 4 );

// DATA TYPES
// Opcodes used in message passing between client and server

typedef enum
    {
    EIptvEngineVodObserverEcgCategoryListUpdated,
    EiptvEngineVodObserverEcgContentListUpdated,
    EIptvEngineVodObserverError
    } MIptvVodContentClientObserverMsg;

typedef enum
    {
    EIptvEngineNone, // First message. Do not change.

    //Service manager client to server messages
    EIptvEngineSmAddServiceReq,
    EIptvEngineSmGetServicesSizeReq,
    EIptvEngineSmGetServicesDataReq,
    EIptvEngineSmGetServicesUsingIdSizeReq,
    EIptvEngineSmGetServicesUsingIdDataReq,
    EIptvEngineSmGetServiceSizeUsingProviderId,
    EIptvEngineSmGetServiceDataUsingProviderId,
    EIptvEngineSmDeleteServiceReq,
    EIptvEngineSmUpdateServiceReq,
    EIptvEngineSmMsgHandlerResetReq,
    EIptvEngineSmGetEventSizeReq,
    EIptvEngineSmGetEventData,
    EIptvEngineSmCancelGetEventSizeReq,
    EIptvEngineSmCancel,

    //Network selection client to server messages
    EIptvEngineNsSetUsedIap,
    EIptvEngineNsClearUsedIap,
    EIptvEngineNsGetUsedIapReq,
    EIptvEngineNsSetConnectionAllowed,
    EIptvEngineNsIsConnectionAllowed,
    EIptvEngineNsCancel,

    //messages handled by server
    EIptvEngineServerShutdownReq,
    EIptvEngineServerCancel,

    // MyVideos messages
    EIptvEngineMyVideosPassMaintenanceInfoReq,
    EIptvEngineMyVideosGetFolderListSizeReq,
    EIptvEngineMyVideosGetFolderListDataReq,
    EIptvEngineMyVideosGetParentFolderSizeReq,
    EIptvEngineMyVideosGetParentFolderDataReq,
    EIptvEngineMyVideosGetFolderNameSizeReq,
    EIptvEngineMyVideosGetFolderNameDataReq,
    EIptvEngineMyVideosDeleteFolderRequestReq,
    EIptvEngineMyVideosDeleteFolderReq,
    EIptvEngineMyVideosGetVideoListSizeReq,
    EIptvEngineMyVideosGetVideoListDataReq,
    EIptvEngineMyVideosGetVideoDetailsSizeReq,
    EIptvEngineMyVideosGetVideoDetailsDataReq,
    EIptvEngineMyVideosGetVideoDetailsForPathSizeReq,
    EIptvEngineMyVideosGetVideoDetailsForPathDataReq,
    EIptvEngineMyVideosGetTotalVideoLenghtRequestReq,
    EIptvEngineMyVideosContentUpdateReq,
    EIptvEngineMyVideosSetVideoDetailsReq,
    EIptvEngineMyVideosDeleteVideoRequestReq,
    EIptvEngineMyVideosDeleteVideoReq,
    EIptvEngineMyVideosCopyFileRequestReq,
    EIptvEngineMyVideosCopyFileReq,
    EIptvEngineMyVideosCopyFolderRequestReq,
    EIptvEngineMyVideosCopyFolderSizeReq,
    EIptvEngineMyVideosCopyFolderDataReq,
    EIptvEngineMyVideosMoveFileRequestReq,
    EIptvEngineMyVideosMoveFileReq,
    EIptvEngineMyVideosMoveFolderRequestReq,
    EIptvEngineMyVideosMoveFolderSizeReq,
    EIptvEngineMyVideosMoveFolderDataReq,
    EIptvEngineMyVideosCancelCopyOrMoveReq,
    EIptvEngineMyVideosRenameDatabaseFolderReq,
    EIptvEngineMyVideosNewDatabaseFolderSizeReq,
    EIptvEngineMyVideosNewDatabaseFolderDataReq,
    EIptvEngineMyVideosCancelReq,
    EIptvEngineMyVideosCreateVideoSizeReq,
    EIptvEngineMyVideosCreateVideoDataReq,
    EIptvEngineMyVideosEnsureFreeSpaceReq,
    EIptvEngineMyVideosEnsureFreeSpaceSizeReq,
    EIptvEngineMyVideosEnsureFreeSpaceDataReq,
    EIptvEngineMyVideosAddMtpEntrySizeReq,
    EIptvEngineMyVideosAddMtpEntryDataReq,
    EIptvEngineMyVideosGetMtpEntrySizeReq,
    EIptvEngineMyVideosGetMtpEntryDataReq,
    EIptvEngineMyVideosListMtpEntriesSizeReq,
    EIptvEngineMyVideosListMtpEntriesDataReq,
    EIptvEngineMyVideosSetMtpEntryReq,
    EIptvEngineMyVideosDeleteMtpEntryReq,
    EIptvEngineMyVideosDeleteAllMtpEntriesReq,
    EIptvEngineMyVideosRenameMtpEntryFileReq,
    EIptvEngineMyVideosEnsureMtpFreeSpaceReq,

    // VoD Content messages
    EIptvEngineVodContentSetIap,
    EIptvEngineVodGetUpdateTime,
    EIptvEngineVodContentUpdateEcg,
    EIptvEngineVodContentCheckGroup,
    EIptvEngineVodContentServiceId,
    EIptvEngineVodContentObserver,
    EIptvEngineVodContentCancel,
    EIptvEngineVodContentCancelUpdate,
    EIptvEngineVodContentGetEcgCategoryListSizeReq,
    EIptvEngineVodContentGetEcgCategoryListDataReq,
    EIptvEngineVodContentGetCategoryDetailsSizeReq,
    EIptvEngineVodContentGetCategoryDetailsDataReq,
    EIptvEngineVodContentGetParentCategoryReq,
    EIptvEngineVodContentGetEcgListSizeReq,
    EIptvEngineVodContentGetEcgListDataReq,
    EIptvEngineVodContentGetEcgAllListSizeReq,
    EIptvEngineVodContentGetEcgAllListDataReq,
    EIptvEngineVodContentGetContentDetailsSizeReq,
    EIptvEngineVodContentGetContentDetailsDataReq,
    EIptvEngineVodContentGetCAListSizeReq,
    EIptvEngineVodContentGetCAListDataReq,
    EIptvEngineVodContentSetLastPosition,
    EIptvEngineVodContentResetGlobalId,
    EIptvEngineVodContentSetMpxId,
    EIptvEngineVodContentCheckUpdate,

    // RSS search value is used for RSS search that is present
    // since Rel 3
    EIptvEngineVodContentRssSearch,
    EIptvEngineVodContentRssSearchCancel,

    // VOD Download messages
    EIptvEngineVodDlGetEventSizeReq, //this must be the first VOD DL message (due to security check)
    EIptvEngineVodDlGetEventData,
    EIptvEngineVodDlCancelGetEventSizeReq,
    EIptvEngineVodDlPurchaseReq,
    EIptvEngineVodDlDownloadReq,
    EIptvEngineVodDlDownload,
    EIptvEngineVodDlCancelDownloadReq,
    EIptvEngineVodDlCancelDownload,
    EIptvEngineVodDlGetDownloadList,
    EIptvEngineVodDlGetDownloadListSize,
    EIptvEngineVodDlPauseDownload,
    EIptvEngineVodDlResumeDownload,
    EIptvEngineVodDlResumeAllDownloads,
    EIptvEngineVodDlPauseAllDownloads,
    EIptvEngineVodDlCancel,
    EIptvEngineVodDlGetDownloadProgressReq, //this must be the last VOD DL message (due to security check)
    EIptvEngineVodDlUpdateDownloadNotification,

    // Live TV messages
    EIptvEngineLiveTvUpdateEPGReq,
    EIptvEngineLiveTvUpdateChannelReq,
    EIptvEngineLiveTvContentServiceId,
    EIptvEngineLiveTvSetIap,

    EIptvEngineLast // Last message. Do not change.

    } TIptvEngineServerReq;

// Opcodes used to indicate which asynchronous service
// has completed
typedef enum
    {
    EIptvEngineSmAddServiceResp,
    EIptvEngineSmGetServicesSizeResp,
    EIptvEngineSmGetServicesDataResp,
    EIptvEngineSmGetServicesUsingIdSizeResp,
    EIptvEngineSmGetServicesUsingIdDataResp,
    EIptvEngineSmDeleteServiceResp,
    EIptvEngineSmUpdateServiceResp,
    EIptvEngineSmMsgHandlerResetResp,

    // Network selection
    EIptvEngineNsGetUsedIapResp,

    EIptvEngineShutdownResp,
    EIptvEngineCancelResp,

    // MyVideos
    EIptvEngineMyVideosContentsUpdate,

    // VoD Content
    EIptvEngineVodContentEcgCategoryListUpdated,
    EIptvEngineVodContentEcgContentListUpdated,
    EIptvEngineVodContentConnectionFail,
    } TIptvEngineServerResp;

#endif // __IPTVCLIENTSERVERCOMMON_H__
