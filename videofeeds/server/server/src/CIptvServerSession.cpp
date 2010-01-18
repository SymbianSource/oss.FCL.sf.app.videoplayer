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
* Description: 
*
*/





// INCLUDE FILES
#include <e32svr.h>

#include "IptvClientServerCommon.h"
#include "CIptvServerSession.h"
#include "CIptvServer.h"
#include "RIptvClientSession.h"
#include "CIptvServiceManager.h"
#include "CIptvServiceMsgHandler.h"
#include "CIptvEpgMsgHandler.h"
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
#include "CIptvVodDownloadMsgHandler.h"
#include "CIptvMyVideosMsgHandler.h"
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
#include "CIptvNetworkSelectionMsgHandler.h"
#include "IptvDebug.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CIptvServerSession::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvServerSession* CIptvServerSession::NewL( RThread& aClient,
                                              CIptvServer& aServer)
    {
    CIptvServerSession* self = CIptvServerSession::NewLC( aClient, 
                                                          aServer);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvServerSession::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvServerSession* CIptvServerSession::NewLC( RThread& aClient,
                                               CIptvServer& aServer)
    {
    CIptvServerSession* self = new ( ELeave ) CIptvServerSession( aClient,
                                                                  aServer);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CIptvServerSession::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvServerSession::ConstructL()
    {
    iServer.IncrementSessions();
    }

// -----------------------------------------------------------------------------
// CIptvServerSession::CIptvServerSession()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CIptvServerSession::CIptvServerSession(RThread& /*aClient*/, CIptvServer& aServer)
:   iServer(aServer)
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
   ,iMyVideosMsgHandler(NULL)
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    {
    }

// -----------------------------------------------------------------------------
// CIptvServerSession::~CIptvServerSession()
// Destructor.
// -----------------------------------------------------------------------------
//
CIptvServerSession::~CIptvServerSession()
    {
    IPTVLOGSTRING_LOW_LEVEL("~CIptvServerSession()");

    delete iServiceMsgHandler;
    iServiceMsgHandler = NULL;
    delete iEpgMsgHandler;
    iEpgMsgHandler = NULL;
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    delete iVodDownloadMsgHandler;
    iVodDownloadMsgHandler = NULL;
    delete iMyVideosMsgHandler;
    iMyVideosMsgHandler = NULL;
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
    delete iNetworkSelectionMsgHandler;
    iNetworkSelectionMsgHandler = NULL;
    	
    TRAPD(err, iServer.DecrementSessionsL());
    if (err != KErrNone)
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvServerSession:: Leave occured in DecrementSessionsL (%d)", err);
        }
    }

// -----------------------------------------------------------------------------
// CIptvServerSession::ServiceL()
// Service request from client.
// -----------------------------------------------------------------------------
//
void CIptvServerSession::ServiceL( const RMessage2& aMessage )
    {	
    switch ( aMessage.Function() )
        {		
        case EIptvEngineSmGetServicesSizeReq:
        case EIptvEngineSmGetServicesDataReq:
        case EIptvEngineSmGetServicesUsingIdSizeReq:
        case EIptvEngineSmGetServicesUsingIdDataReq:
        case EIptvEngineSmGetServiceSizeUsingProviderId:
        case EIptvEngineSmGetServiceDataUsingProviderId:
        case EIptvEngineSmAddServiceReq:        	
        case EIptvEngineSmDeleteServiceReq:
        case EIptvEngineSmUpdateServiceReq:
        case EIptvEngineSmCancel:
        case EIptvEngineSmGetEventSizeReq:
        case EIptvEngineSmGetEventData:
        case EIptvEngineSmCancelGetEventSizeReq:

            {
            if(!iServiceMsgHandler)
                {
                iServiceMsgHandler = CIptvServiceMsgHandler::NewL(iServer);
                }
                
            TRAPD(error, iServiceMsgHandler->HandleServiceManagerMsgL(aMessage));
            
            if(error != KErrNone)
                {
                _LIT(KNetworkSelection, "Service");
                HandleMsgHandlerLeave(error,
                                      KNetworkSelection,
                                      aMessage);
                }
            }
            break;
        
        case EIptvEngineNsSetUsedIap:
        case EIptvEngineNsClearUsedIap:
        case EIptvEngineNsGetUsedIapReq:
        case EIptvEngineNsIsConnectionAllowed:
        case EIptvEngineNsSetConnectionAllowed:
        case EIptvEngineNsCancel:
            {
            if(!iNetworkSelectionMsgHandler)
                {
                iNetworkSelectionMsgHandler = CIptvNetworkSelectionMsgHandler::NewL(iServer);
                }
            
            TRAPD(error, iNetworkSelectionMsgHandler->HandleMsgL(aMessage));
            
            if(error != KErrNone)
                {
                _LIT(KNetworkSelection, "Network Selection");
                HandleMsgHandlerLeave(error,
                                      KNetworkSelection,
                                      aMessage);
                }
            }
            break;
            
        // VoD Content
        case EIptvEngineVodContentSetIap:
        case EIptvEngineVodGetUpdateTime:
        case EIptvEngineVodContentUpdateEcg:
        case EIptvEngineVodContentCheckGroup:
        case EIptvEngineVodContentServiceId:
        case EIptvEngineVodContentObserver:
        case EIptvEngineVodContentCancel:
        case EIptvEngineVodContentCancelUpdate:
        case EIptvEngineVodContentGetEcgCategoryListSizeReq:
        case EIptvEngineVodContentGetEcgCategoryListDataReq:
        case EIptvEngineVodContentGetCategoryDetailsSizeReq:
        case EIptvEngineVodContentGetCategoryDetailsDataReq:
        case EIptvEngineVodContentGetParentCategoryReq:
        case EIptvEngineVodContentGetEcgListSizeReq:
        case EIptvEngineVodContentGetEcgListDataReq:
        case EIptvEngineVodContentGetEcgAllListSizeReq:
        case EIptvEngineVodContentGetEcgAllListDataReq:
        case EIptvEngineVodContentGetContentDetailsSizeReq:
        case EIptvEngineVodContentGetContentDetailsDataReq:
        case EIptvEngineVodContentGetCAListSizeReq:
        case EIptvEngineVodContentGetCAListDataReq:
        case EIptvEngineVodContentSetLastPosition:
        case EIptvEngineVodContentSetMpxId:
        case EIptvEngineVodContentRssSearch:
        case EIptvEngineVodContentRssSearchCancel:
        case EIptvEngineVodContentResetGlobalId:
        case EIptvEngineVodContentCheckUpdate:
        
        // Live messages
        case EIptvEngineLiveTvUpdateEPGReq:
        case EIptvEngineLiveTvContentServiceId:
        case EIptvEngineLiveTvUpdateChannelReq:
        case EIptvEngineLiveTvSetIap:
            {            
            if ( !iEpgMsgHandler )
                {
                iEpgMsgHandler = CIptvEpgMsgHandler::NewL(iServer);
                }
                
            TRAPD(err, iEpgMsgHandler->HandleEpgMsgL(aMessage));
            
            if (err != KErrNone)
                {
                _LIT(KEpg, "EPG");
                HandleMsgHandlerLeave(err,
                                      KEpg,
                                      aMessage);
                }
            }
            break;

        // VoD Download
        case EIptvEngineVodDlGetEventSizeReq:
        case EIptvEngineVodDlGetEventData:
        case EIptvEngineVodDlCancelGetEventSizeReq:
        case EIptvEngineVodDlPurchaseReq:
        case EIptvEngineVodDlDownloadReq:
        case EIptvEngineVodDlDownload:
        case EIptvEngineVodDlCancelDownloadReq:
        case EIptvEngineVodDlCancelDownload:
        case EIptvEngineVodDlGetDownloadList:
        case EIptvEngineVodDlGetDownloadListSize:
        case EIptvEngineVodDlPauseDownload:
        case EIptvEngineVodDlResumeDownload:
        case EIptvEngineVodDlResumeAllDownloads:
        case EIptvEngineVodDlPauseAllDownloads:
        case EIptvEngineVodDlCancel:
        case EIptvEngineVodDlGetDownloadProgressReq:
        case EIptvEngineVodDlUpdateDownloadNotification:
        
            {
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)            
            // Add trace here
            if( ! iVodDownloadMsgHandler )
                {
                iVodDownloadMsgHandler = CIptvVodDownloadMsgHandler::NewL(iServer);
                }
        	    
            TRAPD(error, iVodDownloadMsgHandler->HandleVodDownloadMsgL(aMessage));
            
            if(error != KErrNone)
                {
                _LIT(KVodDownload, "VOD Download");
                HandleMsgHandlerLeave(error,
                                      KVodDownload,
                                      aMessage);
                }
#else
            aMessage.Complete( KErrNotSupported );
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)   
            }
            break;

        // My Videos
        case EIptvEngineMyVideosPassMaintenanceInfoReq:
        case EIptvEngineMyVideosGetFolderListSizeReq:
        case EIptvEngineMyVideosGetFolderListDataReq:
        case EIptvEngineMyVideosGetParentFolderSizeReq:
        case EIptvEngineMyVideosGetParentFolderDataReq:
        case EIptvEngineMyVideosGetFolderNameSizeReq:
        case EIptvEngineMyVideosGetFolderNameDataReq:
        case EIptvEngineMyVideosDeleteFolderRequestReq:
        case EIptvEngineMyVideosDeleteFolderReq:
        case EIptvEngineMyVideosGetVideoListSizeReq:
        case EIptvEngineMyVideosGetVideoListDataReq:
        case EIptvEngineMyVideosGetVideoDetailsSizeReq:
        case EIptvEngineMyVideosGetVideoDetailsDataReq:
        case EIptvEngineMyVideosGetVideoDetailsForPathSizeReq:
        case EIptvEngineMyVideosGetVideoDetailsForPathDataReq:        
        case EIptvEngineMyVideosGetTotalVideoLenghtRequestReq:
        case EIptvEngineMyVideosContentUpdateReq:
        case EIptvEngineMyVideosSetVideoDetailsReq:
        case EIptvEngineMyVideosDeleteVideoRequestReq:
        case EIptvEngineMyVideosDeleteVideoReq:
        case EIptvEngineMyVideosCopyFileRequestReq:
        case EIptvEngineMyVideosCopyFolderRequestReq:
        case EIptvEngineMyVideosMoveFileRequestReq:
        case EIptvEngineMyVideosMoveFolderRequestReq:
        case EIptvEngineMyVideosCopyFileReq:
        case EIptvEngineMyVideosCopyFolderSizeReq:
        case EIptvEngineMyVideosCopyFolderDataReq:
        case EIptvEngineMyVideosMoveFileReq:
        case EIptvEngineMyVideosMoveFolderSizeReq:
        case EIptvEngineMyVideosMoveFolderDataReq:
        case EIptvEngineMyVideosCancelCopyOrMoveReq:
        case EIptvEngineMyVideosRenameDatabaseFolderReq:
        case EIptvEngineMyVideosNewDatabaseFolderSizeReq:
        case EIptvEngineMyVideosNewDatabaseFolderDataReq:
        case EIptvEngineMyVideosCancelReq:
        case EIptvEngineMyVideosCreateVideoSizeReq:
        case EIptvEngineMyVideosCreateVideoDataReq:
        case EIptvEngineMyVideosEnsureFreeSpaceReq:
        case EIptvEngineMyVideosEnsureFreeSpaceSizeReq:
        case EIptvEngineMyVideosEnsureFreeSpaceDataReq:
        case EIptvEngineMyVideosAddMtpEntrySizeReq:
        case EIptvEngineMyVideosAddMtpEntryDataReq:
        case EIptvEngineMyVideosGetMtpEntrySizeReq:
        case EIptvEngineMyVideosGetMtpEntryDataReq:
        case EIptvEngineMyVideosListMtpEntriesSizeReq:
        case EIptvEngineMyVideosListMtpEntriesDataReq:
        case EIptvEngineMyVideosSetMtpEntryReq:
        case EIptvEngineMyVideosDeleteMtpEntryReq:
        case EIptvEngineMyVideosDeleteAllMtpEntriesReq:
        case EIptvEngineMyVideosRenameMtpEntryFileReq:
        case EIptvEngineMyVideosEnsureMtpFreeSpaceReq:
            {
#if defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
            TRAPD( error,
                {            
                if ( ! iMyVideosMsgHandler )
                    {
                    iMyVideosMsgHandler = CIptvMyVideosMsgHandler::NewL(iServer);
                    }

                iMyVideosMsgHandler->HandleMyVideosMsgL(aMessage);

                }); // TRAPD

            if ( error != KErrNone )
                {
                // Complete message if it is still uncompleted.
                if ( ! aMessage.IsNull() )
                    {
                    IPTVLOGSTRING_LOW_LEVEL("CIptvServerSession::ServiceL() Leave in My Videos! Completing with error!");
                    aMessage.Complete( error );
                    }
                if ( error == KErrNoMemory )
                    {
                    IPTVLOGSTRING_LOW_LEVEL("CIptvServerSession::ServiceL() KErrNoMemory in My Videos! Leaving!");
                    User::LeaveNoMemory();
                    }
                }
#else
            aMessage.Complete( KErrNotSupported );
#endif // defined(__SERIES60_30__) || defined(__SERIES60_31__) || defined(__SERIES60_32__)
            }
            break;

        // Common
        case EIptvEngineServerShutdownReq:
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvServerSession:: no implementation currently for EIptvEngineServerShutdownReq");
            //no implementation currently, shutdown not supported
            aMessage.Complete(KErrNotSupported);
            }
            break;
            
        case EIptvEngineServerCancel:
            break;
        	
        default:
            PanicClient( EBadRequest );
        }
    }

// -----------------------------------------------------------------------------
// CIptvServerSession::PanicClient()
// Causes the client thread to panic.
// -----------------------------------------------------------------------------
//
void CIptvServerSession::PanicClient(TInt /*aPanic*/) const
    {
    IPTVLOGSTRING_LOW_LEVEL("CIptvServerSession::PanicClient()");
    }

// -----------------------------------------------------------------------------
// CIptvServerSession::HandleMsgHandlerLeave
// -----------------------------------------------------------------------------
//
void CIptvServerSession::HandleMsgHandlerLeave(TInt aError,
#if IPTV_LOGGING_METHOD != 0
                                               const TDesC& aMsgHandlerName,
#else
                                               const TDesC& /*aMsgHandlerName*/,
#endif
                                               const RMessage2& aMessage)
    {
    IPTVLOGSTRING3_LOW_LEVEL("CIptvServerSession::HandleMsgHandlerLeave %S msg handler leaved with error code %d", &aMsgHandlerName, aError);
                
    if (!aMessage.IsNull())
        {
        IPTVLOGSTRING2_LOW_LEVEL("CIptvEngineSession::HandleMsgHandlerLeave completing pending message with %d", aError);
        aMessage.Complete(aError);
        }
    }
