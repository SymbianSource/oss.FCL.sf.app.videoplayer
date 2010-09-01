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
* Description:    Implementation of the scheduled download plugin engine.*
*/



// INCLUDE FILES
#include <bldvariant.hrh>
#include <e32cmn.h>
#include <s32mem.h>
#include <e32std.h>
#include <e32base.h>
#include <e32msgqueue.h>
#include <ipvideo/CCseScheduledProgram.h>
#include <ipvideo/MCsePluginObserver.h>
#include "IptvLiveDataStructures.h"
#include <hwrmpowerstatesdkpskeys.h>
#include <commdb.h>
#include "IptvDebug.h"
#include <CProfileChangeNotifyHandler.h>
#include <Profile.hrh>
#include <MProfileEngine.h>
#include "CIptvMediaContent.h"

#include "vcnsscheduleddownloadpluginengine.h"
#include "iptvbatterywatcher.h"
#include "iptvphoneregistrationwatcher.h"
#include "vcnsscheduleddownloadmpxclient.h"

// MACROS
#define RETURNONERROR(e) \
    { \
    if ( ( e ) != KErrNone ) \
        { \
        IPTVLOGSTRING2_HIGH_LEVEL( \
            ">>> CIptvScheduledDownloadPluginEngine return with error code: %d", ( e ) ); \
        return; \
        } \
    }

// LOCAL CONSTANTS AND MACROS
const TInt KIptvTimeoutInMicroseconds = 15 * 60 * 1000 * 1000 ; // 15 minutes
const TInt KIptvSearchStringmaxLen = 10;
const TInt KIptvGetAll = 0;
const TInt KIptvNoFileOrDrive = 0;


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::NewL
//
// -----------------------------------------------------------------------------
//
CIptvScheduledDownloadPluginEngine* CIptvScheduledDownloadPluginEngine::NewL(
        MCsePluginObserver& aObserver )
	{	
	CIptvScheduledDownloadPluginEngine* self =
	        new ( ELeave ) CIptvScheduledDownloadPluginEngine( aObserver );
	   
    CleanupStack::PushL( self );
    self->ConstructL( );    
    CleanupStack::Pop( self );    
    
    return self;		
	}
	
// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::ConstructL
//
// -----------------------------------------------------------------------------
//
void CIptvScheduledDownloadPluginEngine::ConstructL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::ConstructL");
        
	iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait;
	iVodDownloads = CIptvVodDlDownloadList::NewL();
    iIptvServiceManagementClient = CIptvServiceManagementClient::NewL( *this );
    iBatteryWatcher = CIptvBatteryWatcher::NewL();
    iBatteryWatcher->AddObserverL( this );
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    iProfChangeHandler = CProfileChangeNotifyHandler::NewL( this );
    iPhoneRegistrationWatcher = CIptvPhoneRegistrationWatcher::NewL();
    iPhoneRegistrationWatcher->AddObserverL( this );

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::ConstructL");
	}

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::CIptvScheduledDownloadPluginEngine
//
// -----------------------------------------------------------------------------
//
CIptvScheduledDownloadPluginEngine::CIptvScheduledDownloadPluginEngine(
        MCsePluginObserver& aObserver ) :
        CTimer( EPriorityNormal ), iObserver( aObserver )
    {
	}

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::~CIptvScheduledDownloadPluginEngine
//
// -----------------------------------------------------------------------------
//	
CIptvScheduledDownloadPluginEngine::~CIptvScheduledDownloadPluginEngine()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::~CIptvScheduledDownloadPluginEngine");

    // Set error value to KErrAbort. This will cause nearly instant return from 
    // RunTaskL -method in case we're aborting plugin execution. This would mean
    // that we're actually running dead code. Class is already destroyed, but the
    // stack is still alive and kicking. This isn't a problems unless we change
    // internal class variables (class is destroyed, but the stack is still there).
    if ( iErrorReason )
        {
        *iErrorReason = KErrAbort;
        }

    // Uses iActiveSchedulerWait so pointer must exists.
    if ( iActiveSchedulerWait )
        {
        ActiveWait( EFalse );
        }

    delete iPhoneRegistrationWatcher;    
    delete iProfChangeHandler;    
    delete iBatteryWatcher;	
	delete iVodDlClient;    
    delete iIptvVodContentClient;    
    delete iIptvServiceManagementClient;    
    delete iService;
	delete iVodDownloads;	
    delete iActiveSchedulerWait;

	iEcgList.ResetAndDestroy();
	iCategoryList.ResetAndDestroy();

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::~CIptvScheduledDownloadPluginEngine");
	}

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::RunTaskL
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::RunTaskL(
            CCseScheduledProgram& aProg,
            TInt& aEngineReturnValue )
	{
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::RunTaskL");

    TInt errorReason( KErrNone );
        
    iErrorReason = &errorReason;
    
    iState = EStarted;

    RestartTimeoutTimer();

    //  Check that we are not in offline mode, reschedule if we are
    CheckOfflineModeL();
    aEngineReturnValue = errorReason;
    RETURNONERROR( errorReason )

	// Get used Service ID
	iServiceID = GetServiceIdL( aProg );

    if (iIptvVodContentClient)
        {
        delete iIptvVodContentClient;
        iIptvVodContentClient = NULL;
        }
    iIptvVodContentClient = CIptvVodContentClient::NewL( iServiceID, *this );

	// Check Battery Status
	GetBatteryStatus();
	aEngineReturnValue = errorReason;
    RETURNONERROR( errorReason )

    //  Get and check the used IAP
    GetUsableIapL();
    aEngineReturnValue = errorReason;
	RETURNONERROR( errorReason )

    iState = EUpdatingContentGuide;

    //  Remember the time, this will be used as last successfull download time
    TTime now;
    now.HomeTime();

    // Update ECG, blocks until the update either is either finished or fails
    UpdateEpgL();
    aEngineReturnValue = errorReason;
	RETURNONERROR( errorReason);
	
	RestartTimeoutTimer();

    //  Update epg and start downloads
    LoadContentL();
    aEngineReturnValue = errorReason;
	RETURNONERROR( errorReason )

    //  Are we downloading
    if (iState == EDownloadingFiles)
        {
        //  This block until the files are downloaded
        IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::RunTaskL starting to wait the downloads to finish");
        ActiveWait( ETrue );
        }
        
    aEngineReturnValue = errorReason;        
	RETURNONERROR( errorReason )

    IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::RunTaskL downloads finished, updating schedules");

    iState = EFinished;

    //  Update all schedules by changing the last successfull download time
    UpdateLastDownloadTimeL( now );

    iObserver.PluginCompleted( KErrNone );

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::RunTaskL");

	}

// from CTimer, CActive

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::RunL
//
// -----------------------------------------------------------------------------
//
void CIptvScheduledDownloadPluginEngine::RunL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::RunL");

    //  No activity in KTimeoutInMilliseconds
    //  Assuming some kind of error situation and aborting all activities
    *iErrorReason = KErrTimedOut;
    CancelDownloads();
    ActiveWait( EFalse );

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::RunL");
    }

//  from MIptvServiceManagementClientObserver

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::AddServiceResp
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::AddServiceResp(
        TRespStatus /*aRespStatus*/)
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>><<< CIptvScheduledDownloadPluginEngine::AddServiceResp");
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::UpdateServiceResp
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::UpdateServiceResp(
        TRespStatus /*aRespStatus*/)
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>><<< CIptvScheduledDownloadPluginEngine::UpdateServiceResp");
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::DeleteServiceResp
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::DeleteServiceResp(
        TRespStatus /*aRespStatus*/)
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>><<< CIptvScheduledDownloadPluginEngine::DeleteServiceResp");
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::GetServicesResp
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::GetServicesResp(
        TRespStatus /*aRespStatus*/, 
        CDesC8ArraySeg* /*aServicesArray*/)
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>><<< CIptvScheduledDownloadPluginEngine::GetServicesResp");
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::GetUsedIapResp
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::GetUsedIapResp(
        TUint32 /*aIapId*/,
        const TDesC& /*aIapName*/,
        CIptvNetworkSelection::TConnectionPermission /*aConnectionPermission*/,
        TBool /*aWlanWhenGPRS*/,
        CIptvNetworkSelection::TRespStatus /*aRespStatus*/ )
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>><<< CIptvScheduledDownloadPluginEngine::GetUsedIapResp");
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::ServerShutdownResp
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::ServerShutdownResp(
        TRespStatus /*aRespStatus*/)
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>><<< CIptvScheduledDownloadPluginEngine::ServerShutdownResp");
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::HandleSmEvent
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::HandleSmEvent(
        CIptvSmEvent& /*aEvent*/)
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>><<< CIptvScheduledDownloadPluginEngine::HandleSmEvent");
    }


// from MIptvVodContentClientObserver

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::HandleEpgManagerMsgL
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::HandleEpgManagerMsgL(
        TInt aMsg,
        TInt /*aInfo*/,
        TIptvServiceId /*aServiceId*/)
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::HandleEpgManagerMsgL");
    IPTVLOGSTRING2_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::HandleEpgManagerMsgL, msg = %d", aMsg);

	switch (aMsg)
		{
		case KIptvErrorEpgUpdateFailed:
		    {
		    //  EPG update failed for some reason, try again later
		    *iErrorReason = EReschedule;
		    ActiveWait( EFalse );
		    }
		    break;

		case KIptvVodUpdateNotStarted:
		case KIptvErrorEpgUpdateSuccessed:
		    {
		    if (iState == EUpdatingContentGuide)
		        {
		        ActiveWait( EFalse );
		        }
		    }
			break;	

		case KIptvErrorVodNoIap:
		    {
		    if (iState == EUpdatingContentGuide)
		        {
		        IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::HandleEpgManagerMsgL, setting iap");
		        iIptvVodContentClient->SetIapL( iIapId );
		        }
		    }
		    break;

		case KIptvVodUpdateStarted:
		case KIptvErrorEpgUpdateStopped:
		case KIptvContentUpdateCompleted:
		case KIptvServiceThumbnailDownloaded:
		case KIptvContentThumbnailDownloaded:
		default:
		    {
		    // No operation, update still in progress.
		    }
			break;
        }

    RestartTimeoutTimer();

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::HandleEpgManagerMsgL");
    }

//  from MIptvVodDlClientObserver

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::HandleVodDlDownloadEvent
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::HandleVodDlDownloadEvent(
                CIptvVodDlDownloadEvent& aEvent) 
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::HandleVodDlDownloadEvent");
    IPTVLOGSTRING2_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::HandleVodDlDownloadEvent, event = %d", aEvent.iEvent);

    switch (aEvent.iEvent)
        {
        case CIptvVodDlDownloadEvent::EDownloadState:
            {
            switch ( aEvent.iState )
                {
                case KIptvVodDlDownloadInQueue:
                case KIptvVodDlDownloadStarting:
                case KIptvVodDlDownloadDeleted:
                case KIptvVodDlDownloadPermanentlyPausing:
                case KIptvVodDlDownloadCreated:
                case KIptvVodDlDownloadIdle:
                case KIptvVodDlDownloadGlobalFileIdIsValid:
                case KIptvVodDlDownloadRunning:
                case KIptvVodDlDownloadPausing:
                case KIptvVodDlDownloadPaused:
                    {
                    }
                    break;

                case KIptvVodDlDownloadPermanentlyPaused:
                case KIptvVodDlDownloadFailed:
                    {
                    if (DeleteFromList( aEvent.iServiceId, aEvent.iContentId ))
                        {
                        TRAP_IGNORE( iVodDlClient->CancelDownloadL( 
                                aEvent.iServiceId,
                                aEvent.iContentId ) )
                        }
                    }
                    break;

                case KIptvVodDlDownloadDeletedCompletely:
                case KIptvVodDlDownloadSucceeded:
                    {
                    DeleteFromList( aEvent.iServiceId, aEvent.iContentId );
                    }
                    break;

                default:
                    break;            
                }
            }
            break;

        case CIptvVodDlDownloadEvent::EDownloadReqSucceeded:
        case CIptvVodDlDownloadEvent::ECancelReqFailed:
        case CIptvVodDlDownloadEvent::ECancelReqSucceeded:
        case CIptvVodDlDownloadEvent::EDownloadReqFailed:
        case CIptvVodDlDownloadEvent::EDownloadItemsModified:
        break;

        default:
        break;
        }

    RestartTimeoutTimer();

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::HandleVodDlDownloadEvent");
    }



// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::BatteryLow
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::BatteryLow()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::BatteryLow");

    //  Set error reason, cancel downloads and finish the plugin
    *iErrorReason = KErrBadPower;
    CancelDownloads();
    ActiveWait( EFalse );

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::BatteryLow");
    }


// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::HandleActiveProfileEventL
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::HandleActiveProfileEventL(
            TProfileEvent aProfileEvent,
            TInt aProfileId ) 
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::HandleActiveProfileEventL");

    if (aProfileEvent == EProfileNewActiveProfile)
        {
        if (aProfileId == EProfileOffLineId)
            {
            IPTVLOGSTRING_LOW_LEVEL("CIptvScheduledDownloadPluginEngine::HandleActiveProfileEventL phone changed to offline mode");
            *iErrorReason = EReschedule;
            CancelDownloads();
            ActiveWait( EFalse );
            }
        }

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::HandleActiveProfileEventL");
    }


// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::OnHomeNetwork
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::OnHomeNetwork( TBool aOnHomeNetwork )
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::OnHomeNetwork");

    if (!aOnHomeNetwork)
        {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::OnHomeNetwork, not in home network");
        //  Set error reason, cancel downloads and finish the plugin
        *iErrorReason = EReschedule;
        CancelDownloads();
        ActiveWait( EFalse );
        }

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::OnHomeNetwork");
    }


// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::GetBatteryStatus
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::GetBatteryStatus()
	{
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::GetBatteryStatus");

    if (iBatteryWatcher->IsLowPower())
        {
        *iErrorReason = KErrBadPower;
        IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::GetBatteryStatus low power, aborting");
        }

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::GetBatteryStatus");
	}	


// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::CancelDownloads
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::CancelDownloads()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::CancelDownloads");

    if (iState == EDownloadingFiles)
        {
        //  Cancel the downloads
        while (iVodDownloads->iList.Count())
            {
            //  Remove in reverse order, otherwise DL manager will start new dl
            //  after we cancel active one which are in the beginning of the list
            CIptvVodDlDownloadListItem* download =
                    iVodDownloads->iList[iVodDownloads->iList.Count() - 1];
            TUint32 contentId = download->iContentId;

            IPTVLOGSTRING3_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::CancelDownloads Cancelling %d %d", iServiceID, contentId );
            iVodDownloads->Delete( iServiceID, contentId );

            TRAP_IGNORE(
                    iVodDlClient->CancelDownloadL( iServiceID, contentId ) )
            }
        }

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::CancelDownloads");
    }


// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::GetUsableIapL
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::GetUsableIapL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::GetUsableIapL");

	CIptvNetworkSelection::TConnectionPermission connectionPermission =
	        CIptvNetworkSelection::ENotAllowed;
    TBuf<KIptvNsIapNameMaxLength> apName;
    CIptvNetworkSelection::TRespStatus respStatus;

	iIptvServiceManagementClient->GetUsedIapL(
	            iServiceID,
	            iIapId,
	            apName,
	            connectionPermission,
	            respStatus);

	// Check if IAP is retrieved successfully
	if (respStatus == CIptvNetworkSelection::ESucceeded)
	    {
	    //  We even skip the CIptvNetworkSelection::EAlreadyActive case
	    //  because we don't want to download when something else is going on

        //  Set not allowed and test the iap against scheduled download settings
        connectionPermission = CIptvNetworkSelection::ENotAllowed;

        //  IAP was found, get the schduled download settings
        FetchServiceL();
    	if( *iErrorReason != KErrNone ) 
        	{
            User::Leave( *iErrorReason );
        	}

        TIptvVodScheduleConnectionCondition scheduledConnectionType =
                static_cast<TIptvVodScheduleConnectionCondition>(
                iService->ScheduleDlNetwork() );

        *iErrorReason = KErrCouldNotConnect;

		// Check connection permission
        switch (scheduledConnectionType)
            {
            case EAlways:
                {
                connectionPermission = CIptvNetworkSelection::EAllowed;
                }
                break;

            case EWLAN:
                {
                if (CIptvUtil::ConnectionTypeL(iIapId) == CIptvUtil::EWlan)
                    {
                    connectionPermission = CIptvNetworkSelection::EAllowed;
                    }
                else
                    {
                    //  Iap does not support wlan, try again later
                    *iErrorReason = EReschedule;
                    }
                }
                break;

            case EHomeCellular:
                {
                CIptvUtil::TConnectionType
                        connectionType = CIptvUtil::ConnectionTypeL(iIapId);
                if (connectionType == CIptvUtil::EWlan)
                    {
                    //  Allow wlan always
                    connectionPermission = CIptvNetworkSelection::EAllowed;
                    }
                else
                    {
                    if (connectionType == CIptvUtil::EGprs)
                        {
                        IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::GetUsableIapL, connection allowed only in home network, checking");
                        //  Allow connection on home network
                        if (iPhoneRegistrationWatcher->IsOnHomeNetwork())
                            {
                            IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::GetUsableIapL, allowed");
                            connectionPermission = CIptvNetworkSelection::EAllowed;
                            }
                        else
                            {
                            //  Not on home network, reschedule
                            *iErrorReason = EReschedule;
                            }
                        }
                    }
                }
                break;

            default:
                {
                }
                break;
            }

		if (connectionPermission == CIptvNetworkSelection::EAllowed)
		    {
		    IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::GetUsableIapL, connection allowed");
        	// Set the connection to be allowed
        	iIptvServiceManagementClient->SetConnectionAllowedL(
        	        ETrue, iIapId, respStatus );

            if(respStatus == CIptvNetworkSelection::ESucceeded) 
                {
                //  Set IAP
                iIptvVodContentClient->SetIapL( iIapId );

                //  All is fine
                *iErrorReason = KErrNone;
                }
		    }
	    }
	else if ( respStatus == CIptvNetworkSelection::EFailedAlwaysAskSelected )
	    {
	    connectionPermission = CIptvNetworkSelection::ENotAllowed;
        *iErrorReason = EDontReschedule; // No reschedule, when 'Always Ask' is selected.
	    }
    else
        {
        //  No iap, try again later
        *iErrorReason = EReschedule;
        }

    RestartTimeoutTimer();

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::GetUsableIapL");
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::GetServiceIdL
//
// -----------------------------------------------------------------------------
//	
TInt CIptvScheduledDownloadPluginEngine::GetServiceIdL(
        CCseScheduledProgram& aProg ) const
    {
	// Get Service ID
	RDesReadStream readStream;
    CleanupClosePushL( readStream );
    readStream.Open( aProg.ApplicationData() );
    TUint32 serviceId = readStream.ReadUint32L();
    CleanupStack::PopAndDestroy( &readStream );
    
    return serviceId;
	}

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::UpdateEpgL
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::UpdateEpgL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::UpdateEpgL");

    iIptvVodContentClient->UpdateEcgL();
    
    ActiveWait( ETrue );
        
    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::UpdateEpgL");
    }


// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::StartDownloadL
//
// -----------------------------------------------------------------------------
//	
TInt CIptvScheduledDownloadPluginEngine::StartDownloadL( CIptvVodContentContentBriefDetails*  aContent )
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::StartDownloadL");

    // Create download item so that download events can be matched
    CIptvVodDlDownloadListItem* download = CIptvVodDlDownloadListItem::NewL();
    CleanupStack::PushL(download); // 1->
    download->iIapId     = iIapId;
    download->iServiceId = iServiceID;
    download->iContentId = aContent->iContentId;
    download->iDlType    = EIptvDownloadTypeImmediate;
    iVodDownloads->iList.AppendL( download );
    CleanupStack::Pop(download); // <-1

    TIptvDlError err = iVodDlClient->DownloadL(
                iIapId,
                iService,
                aContent->iContentId,
                aContent->iName );

    if (err != EIptvDlNoError)
        {
        //  Download not started
        iVodDownloads->Delete( iServiceID, aContent->iContentId );
        iVodDownloads->iList.Remove( iVodDownloads->iList.Count() - 1 );
        }
    else
        {
        IPTVLOGSTRING3_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::StartDownloadL Download started %d %d", iServiceID, aContent->iContentId );
        }

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::StartDownloadL");

    RestartTimeoutTimer();

    return err == EIptvDlNoError ? KErrNone : KErrCancel;
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::ActiveWait
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPluginEngine::ActiveWait( TBool aStart )
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::ActiveWait");
    if (aStart)
        {
	    if(!iActiveSchedulerWait->IsStarted())
	        {
	        IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::ActiveWait - On!");
	        iActiveSchedulerWait->Start( );		
	        }
        }
    else
        {
		if(iActiveSchedulerWait->IsStarted())
			{			
            iActiveSchedulerWait->AsyncStop( );
			IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::ActiveWait - Off!");
			}
        }
    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::ActiveWait");        
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::DeleteFromList
//
// -----------------------------------------------------------------------------
//	
TBool CIptvScheduledDownloadPluginEngine::DeleteFromList(
        TUint32 aServiceId,
        TUint32 aContentId )
    {
    TBool isInList = EFalse;
    if (iState == EDownloadingFiles)
        {
        CIptvVodDlDownloadListItem* item =
                iVodDownloads->GetListItem( aServiceId, aContentId );
        if (item)
            {
            isInList = ETrue;
            iVodDownloads->Delete( aServiceId, aContentId );
	        if (iVodDownloads->iList.Count() == 0)
	            {
	            ActiveWait( EFalse );
	            }
	        }
        }
    return isInList;
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::FetchServiceL
//
// -----------------------------------------------------------------------------
//
void CIptvScheduledDownloadPluginEngine::FetchServiceL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::FetchServiceL");

    delete iService;
    iService = NULL;

    CDesC8ArraySeg* serializedServices;
    MIptvServiceManagementClientObserver::TRespStatus respStatus;

    TInt err = iIptvServiceManagementClient->GetServicesL(
                iServiceID, iServiceID, serializedServices, respStatus );

    if (err == KErrNone && respStatus == ESucceeded)
        {
        CleanupStack::PushL( serializedServices );
        //  There should be one item, just check that it exists
        if (serializedServices->Count() > 0)
            {
            iService = CIptvService::NewL();
            iService->SetL( serializedServices->MdcaPoint(0) );
            }
        else
            {
            err = KErrGeneral;
            }
        CleanupStack::PopAndDestroy( serializedServices );
        }
    else
        {
        //  Didin't get the service, return error code
        err = (err == KErrNone) ? KErrGeneral : err;
        }

    if (err)
        {
        *iErrorReason = err;
        }

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::FetchServiceL");
    }


// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::LoadContentL
//
// -----------------------------------------------------------------------------
//
TInt CIptvScheduledDownloadPluginEngine::LoadContentL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::LoadContentL");

    iState = EGettingContent;

    //  Get the categories
    iCategoryList.ResetAndDestroy();
	*iErrorReason = iIptvVodContentClient->GetEcgCategoryListL(
	        KIptvVodContentCategoryRootId, iCategoryList );
	
	if( *iErrorReason != KErrNone )
	    {
	    return *iErrorReason;
	    }
	
	if( iVodDlClient )
	    {
	    delete iVodDlClient;
	    iVodDlClient = NULL;
	    }
	
    iVodDlClient = CVcxNsScheduledDownloadMpxClient::NewL( *this, *iIptvVodContentClient, iServiceID );

    iState = ECheckingDownloadedFiles;

    //  Download all episodes which have been added after the last successfull
    //  scheduled download and which aren't already downloaded
    TTime lastScheduledDl = iService->GetScheduledLastDownloadTime();

    //  Go through categories and load their content
	TInt count = iCategoryList.Count();
	for (TInt i = 0; i < count && *iErrorReason == KErrNone; i++)
	    {
	    TUint32 totalAmount = 0;
        TBuf<KIptvSearchStringmaxLen> searchString( KNullDesC );

        //  If video search is in use and the category is search category,
        //  the download is skipped
        if (!iCategoryList[i]->iIsSearchCategory)
            {
            // Get the episodes
    	    iEcgList.ResetAndDestroy();
            IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::LoadContentL getting categories");
    		*iErrorReason = iIptvVodContentClient->GetEcgListL(
    		        iCategoryList[i]->iCategoryId,
    		        searchString,
    		        KIptvGetAll,
    		        KIptvGetAll,
    		        totalAmount,
    		        iEcgList );
    		
    		if( *iErrorReason != KErrNone )
		        {
		        return *iErrorReason;
		        }

            // Check if the episodes contain publish date
            TBool hasPublishDate = EFalse;

            for (TInt j = 0; j < iEcgList.Count() && !hasPublishDate; j++)
                {
                if (iEcgList[j]->iPubDate.Int64() != 0)
                    {
                    hasPublishDate = ETrue;
                    }
                }

            if (hasPublishDate)
                {
                IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::LoadContentL feed has publish dates.");
                
                // Publish date exists, download after last scheduled download
                for (TInt j = 0;
                        j < iEcgList.Count() && *iErrorReason == KErrNone;
                        j++)
                    {
                    CIptvVodContentContentBriefDetails* details = iEcgList[j];

                    // Check whether it has been downloaded already.
                    if ( details && details->iFileId &&
                         details->iFileId->iDrive == KIptvNoFileOrDrive &&
                         details->iFileId->iFileId == static_cast<TIptvFileId>(
                             KIptvNoFileOrDrive ) )
                        {
                        // Check whether it has been added after the last scheduled
                        // download.
                        if (lastScheduledDl <= details->iPubDate)
                            {
                            IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::LoadContentL video is new enough.");
                            
                            // Check whether it is a downloadable file.
                            if ( IsDownloadableL( details->iContentId ) )
                                {
                                // Remember the old state.
                                TState  oldState = iState;
                                iState = EDownloadingFiles;
                                if ( KErrNone != StartDownloadL( details ) )
                                    {
                                    // Download not going on, restore old state.
                                    iState = oldState;
                                    }
                                }
                            }
                        else
                            {
                            IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::LoadContentL video is not new enough.");
                            }
                        }
                    }
                }
            else
                {
                IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::LoadContentL feed does not have publish dates.");
                
                // No publish date, download only the first one.
                if (iEcgList.Count() > 0)
                    {
                    CIptvVodContentContentBriefDetails* details = iEcgList[0];

                    // Check whether it has been downloaded already.
                    if ( details && details->iFileId &&
                         details->iFileId->iDrive == KIptvNoFileOrDrive &&
                         details->iFileId->iFileId == static_cast<TIptvFileId>(
                             KIptvNoFileOrDrive ) )
                        {
                        // Check whether it is a downloadable file.
                        if ( IsDownloadableL( details->iContentId ))
                            {
                            // Remember the old state.
                            TState  oldState = iState;
                            iState = EDownloadingFiles;
                            if ( KErrNone != StartDownloadL( details ) )
                                {
                                //  Download not going on, restore old state.
                                iState = oldState;
                                }
                            }
                        }
                    }
                }
            }
	    }

    iEcgList.ResetAndDestroy();
    iCategoryList.ResetAndDestroy();

    RestartTimeoutTimer();

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::LoadContentL");

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::IsDownloadableL
//
// -----------------------------------------------------------------------------
//
TBool CIptvScheduledDownloadPluginEngine::IsDownloadableL( TIptvContentId aContentId )
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::IsDownloadableL");

    TBool retVal = EFalse;
    RPointerArray<CIptvMediaContent> caDetails;
    CleanupClosePushL( caDetails );

    if (iIptvVodContentClient->GetContentAccessListL(
            aContentId,
            caDetails ) == KErrNone)
        {
        //  Go through the content access list and check it is downloadable
        for (TInt i = 0; i < caDetails.Count() && !retVal; i++)
            {
            retVal = caDetails[i]->iDownloadType == EIptvDownloadTypeImmediate
                    && caDetails[i]->iExpression == CIptvMediaContent::EFull;
            }
        }

    CleanupStack::PopAndDestroy( &caDetails );	            

    IPTVLOGSTRING2_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::IsDownloadableL: %d", retVal);

    return retVal;
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::UpdateLastDownloadTimeL
//
// -----------------------------------------------------------------------------
//
void CIptvScheduledDownloadPluginEngine::UpdateLastDownloadTimeL(
        const TTime& aTimestamp )
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::UpdateLastDownloadTimeL");

    //  First get the service
    FetchServiceL();
    IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::UpdateLastDownloadTimeL service fetched");

    //  Update the last download time
    iService->SetScheduledLastDownloadTime( aTimestamp );

    //  Update the service.
    //  No check on the result since nothing can be done if it fails
    MIptvServiceManagementClientObserver::TRespStatus respStatus;
    iIptvServiceManagementClient->UpdateServiceL( *iService, respStatus );

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::UpdateLastDownloadTimeL");
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::RestartTimeoutTimer
//
// -----------------------------------------------------------------------------
//
void CIptvScheduledDownloadPluginEngine::RestartTimeoutTimer()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::RestartTimeoutTimer");

    Cancel();
    After( TTimeIntervalMicroSeconds32( KIptvTimeoutInMicroseconds ) );

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::RestartTimeoutTimer");
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPluginEngine::CheckOfflineModeL
//
// -----------------------------------------------------------------------------
//
void CIptvScheduledDownloadPluginEngine::CheckOfflineModeL()
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPluginEngine::CheckOfflineModeL");

	MProfileEngine* profileEngine = CreateProfileEngineL();

    if (profileEngine)
        {
        if ( profileEngine->ActiveProfileId() == EProfileOffLineId )
            {
            *iErrorReason = EReschedule;
            IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPluginEngine::CheckOfflineModeL offline mode");
            }

        profileEngine->Release();
        }

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPluginEngine::CheckOfflineModeL");
    }
