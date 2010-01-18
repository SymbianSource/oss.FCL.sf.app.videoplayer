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
* Description: 
*
*/




// INCLUDE FILES
#include <mpxlog.h>
#include <utf.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>

#include "vcxmyvideosdownloadutil.h" //this has to be before vcxconnectionutility.h
#include <ipvideo/vcxconnectionutility.h>

#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>
#include <mpxmediageneraldefs.h>
#include <MediatorEventProvider.h>
#include <centralrepository.h>
#include <bautils.h>
#include "vcxmyvideosdrivemonitor.h"

// CONSTANTS
#ifdef _DEBUG
_LIT(KVcxHttpDlCreatedDes, "DlCreated");
_LIT(KVcxHttpDlInprogressDes, "DlInprogress");
_LIT(KVcxHttpDlPausedDes, "DlPaused");
_LIT(KVcxHttpDlCompletedDes, "DlCompleted");
_LIT(KVcxHttpDlFailedDes, "DlFailed");
_LIT(KVcxHttpDlMovedDes, "DlMoved");
_LIT(KVcxHttpDlMediaRemovedDes, "DlMediaRemoved");
_LIT(KVcxHttpDlMediaInsertedDes, "DlMediaInserted");
_LIT(KVcxHttpDlPausableDes, "DlPausable");
_LIT(KVcxHttpDlNonPausableDes, "DlNonPausable");
_LIT(KVcxHttpDlDeletedDes, "DlDeleted");
_LIT(KVcxHttpDlAlreadyRunningDes, "DlAlreadyRunning");
_LIT(KVcxHttpDlDeletingDes, "DlDeleting");
_LIT(KVcxHttpDlMultipleMOStartedDes, "DlMultipleMOStarted");
_LIT(KVcxHttpDlMultipleMOCompletedDes, "DlMultipleMOCompleted");
_LIT(KVcxHttpDlMultipleMOFailedDes, "DlMultipleMOFailed");

_LIT(KVcxUnknownStateDes, "UnknownStateDes");


_LIT(KVcxHttpProgNoneDes, "ProgNone");
_LIT(KVcxHttpStartedDes, "Started");
_LIT(KVcxHttpProgCreatingConnectionDes, "ProgCreatingConnection");
_LIT(KVcxHttpProgConnectionNeededDes, "ProgConnectionNeeded");
_LIT(KVcxHttpProgConnectedDes, "ProgConnected");
_LIT(KVcxHttpProgConnectionSuspendedDes, "ProgConnectionSuspended");
_LIT(KVcxHttpProgDisconnectedDes, "ProgDisconnected");
_LIT(KVcxHttpProgDownloadStartedDes, "ProgDownloadStarted");
_LIT(KVcxHttpContentTypeRequestedDes, "ContentTypeRequested");
_LIT(KVcxHttpContentTypeReceivedDes, "ContentTypeReceived");
_LIT(KVcxHttpProgSubmitIssuedDes, "ProgSubmitIssued");
_LIT(KVcxHttpProgResponseHeaderReceivedDes, "ProgResponseHeaderReceived");
_LIT(KVcxHttpProgResponseBodyReceivedDes, "ProgResponseBodyReceived");
_LIT(KVcxHttpProgRedirectedPermanentlyDes, "ProgRedirectedPermanently");
_LIT(KVcxHttpProgRedirectedTemporarilyDes, "ProgRedirectedTemporarily");
_LIT(KVcxHttpProgDlNameChangedDes, "ProgDlNameChanged");
_LIT(KVcxHttpProgContentTypeChangedDes, "ProgContentTypeChanged");
_LIT(KVcxHttpProgCodDescriptorDownloadedDes, "ProgCodDescriptorDownloaded");
_LIT(KVcxHttpProgCodDownloadStartedDes, "ProgCodDownloadStarted");
_LIT(KVcxHttpProgCodDescriptorAcceptedDes, "ProgCodDescriptorAccepted");
_LIT(KVcxHttpProgCodLoadEndDes, "ProgCodLoadEnd");
_LIT(KVcxHttpProgSupportedMultiPartDes, "ProgSupportedMultiPart");
_LIT(KVcxHttpProgMovingContentFileDes, "ProgMovingContentFile");
_LIT(KVcxHttpProgContentFileMovedDes, "ProgContentFileMoved");
_LIT(KVcxUnknownProgressDes, "UnknownProgress");
#endif

const TInt KMaxPathLength = 255;

_LIT( KVcxExtensionDcf, ".dcf" );
_LIT( KVcxExtensionOcf, ".odf" );
_LIT( KVcxExtensionDm, ".dm" );
_LIT( KVcxExtensionMp4, ".mp4" );
_LIT( KVcxExtensionMpeg4, ".mpeg4" );
_LIT( KVcxExtensionM4v, ".m4v" );
_LIT( KVcxExtension3gpp, ".3gpp" );
_LIT( KVcxExtension3gp, ".3gp" );
_LIT( KVcxExtensionRm, ".rm" );
_LIT( KVcxExtensionMpg4, ".mpg4" );
_LIT( KVcxExtensionRmvb, ".rmvb" );
_LIT( KVcxExtension3g2, ".3g2" );
_LIT( KVcxExtensionWmv, ".wmv" );
_LIT( KVcxExtensionAsf, ".asf" );

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosDownloadUtil* CVcxMyVideosDownloadUtil::NewL(
    MVcxMyVideosDownloadUtilObserver& aObserver, RFs& aFs )
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::NewL");

    CVcxMyVideosDownloadUtil* self = new (ELeave) CVcxMyVideosDownloadUtil( aObserver, aFs );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosDownloadUtil::~CVcxMyVideosDownloadUtil()
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::~CVcxMyVideosDownloadUtil");
    iDownloadManager.Disconnect(); //disconnects all downloads and puts them to pause
    iDownloadManager.Close();
    delete iDownloadDeleteTimer;
    iDownloadsToDelete.Close();
    iDeleteContent.Close();

    if ( iConnUtil )
        {
        iConnUtil->RemoveObserver( this );
        iConnUtil->DecreaseReferenceCount();
        }

    delete iMediatorEventProvider;
    delete iDownloadProgressTimer;
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosDownloadUtil::CVcxMyVideosDownloadUtil(
        MVcxMyVideosDownloadUtilObserver& aObserver, RFs& aFs )
: iObserver( aObserver ), iFs( aFs )
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::CVcxMyVideosDownloadUtil");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::ConstructL ()
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::ConstructL");
    
    TUid uid = TUid::Uid( KVcxUidMyVideosMpxCollection );

    iDownloadManager.ConnectL( uid, *this, ETrue );
    
    iDownloadDeleteTimer = CVcxMyVideosTimer::NewL( 0, *this );
    iDownloadProgressTimer = CVcxMyVideosTimer::NewL( 0, *this );

    iDownloadsToDelete.Reset();
    iDeleteContent.Reset();

    iConnUtil = CVcxConnectionUtility::InstanceL();
    iConnUtil->RegisterObserverL( this ); 
    
    iMediatorEventProvider = CMediatorEventProvider::NewL();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::StartDownloadL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::StartDownloadL( CMPXMedia& aDownload )
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::StartDownloadL");
    // Requesting a new connection, update state
    iRoamingOngoing = EFalse;
    TInt err( KErrNone );

    TUint32 iap( 0 );
    iap = *(aDownload.Value<TUint>( KVcxMediaMyVideosIapId ));

    if( !iap )
        {
        // no iap gotten yet. Use connectionutility to get one
        err = iConnUtil->GetIap( iap, EFalse );

        if( err != KErrNone || iap == 0 )
            {
            MPX_DEBUG2("CVcxMyVideosDownloadUtil:: failed to get IAP from connectionutility: %d", err);
            User::Leave( KErrCancel );
            }        
        }

    //if not set, the default is used from commsdb
    
    if ( iap != iCurrentIap )
        {
        err = iDownloadManager.SetIntAttribute( EDlMgrIap, iap );
        }
        
    iCurrentIap = iap;

    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosDownloadUtil:: failed to set IAP: %d", err);
        }

    TMPXItemId mpxId = aDownload.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );

    MPX_DEBUG2( "URL      = %S", &(aDownload.ValueText( KVcxMediaMyVideosRemoteUrl )) );
    MPX_DEBUG2( "Username = %S", &(aDownload.ValueText( KVcxMediaMyVideosUsername )) );
    MPX_DEBUG2( "Password = %S", &(aDownload.ValueText( KVcxMediaMyVideosPassword )) );
    MPX_DEBUG2( "Filename = %S", &(aDownload.ValueText( KMPXMediaGeneralUri )) );
    MPX_DEBUG2( "IAP ID   = %d", *(aDownload.Value<TUint>( KVcxMediaMyVideosIapId )) );
    MPX_DEBUG2( "MDS ID   = %d", mpxId.iId1 );
    
    iUrl.Copy( aDownload.ValueText( KVcxMediaMyVideosRemoteUrl ) );
        
    TInt32 downloadId = 0;
    
    // try to get download id from the download request
    if ( aDownload.IsSupported(KVcxMediaMyVideosDownloadId) )
        {
        MPX_DEBUG1("CVcxMyVideosDownloadUtil:: dl id is supported");
        downloadId = static_cast<TInt32>(
            aDownload.ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId ));
        if ( downloadId != 0 )
            {
            MPX_DEBUG1("CVcxMyVideosDownloadUtil:: dl id found -> resuming download");
            } 
        }
        
    if ( downloadId == 0 )
        {
        MPX_DEBUG1("CVcxMyVideosDownloadUtil:: creating download");
        RHttpDownload& dl =  iDownloadManager.CreateDownloadL( iUrl );
        dl.GetIntAttribute( EDlAttrId, downloadId );
        }
    
    RHttpDownload* download = Download( downloadId );
    
    if ( !download )
        {
        MPX_DEBUG1("CVcxMyVideosDownloadUtil:: no download found");
        User::Leave( KErrNotFound );
        }
            
    CnvUtfConverter::ConvertFromUnicodeToUtf8(iUserName,
            aDownload.ValueText( KVcxMediaMyVideosUsername ) );
    CnvUtfConverter::ConvertFromUnicodeToUtf8(iPassword,
            aDownload.ValueText( KVcxMediaMyVideosPassword ) );    
    download->SetStringAttribute( EDlAttrDestFilename,
            aDownload.ValueText( KMPXMediaGeneralUri ) );        
    download->SetStringAttribute( EDlAttrUsername, iUserName );
    download->SetStringAttribute( EDlAttrPassword, iPassword );
    download->SetBoolAttribute( EDlAttrSilent, EFalse) ;
    download->SetBoolAttribute( EDlAttrNoContentTypeCheck, ETrue );
        
    err = download->Start();

    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosDownloadUtil:: could not start download: %d", err);
        }

    aDownload.SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, downloadId );
    
    if( !err )
        {
        NotifyDownloadStarted();
        StartProgressTimer();
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::PauseDownload
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosDownloadUtil::PauseDownload( TUint32 aDownloadId )
    {
    RHttpDownload* download = Download( aDownloadId );
    
    if ( !download )
        {
        MPX_DEBUG1("CVcxMyVideosDownloadUtil:: no download found");
        return KErrNotFound;
        }
        
    return download->Pause();    
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::HandleDMgrEventL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::HandleDMgrEventL( RHttpDownload& aDownload,
    THttpDownloadEvent aEvent )
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::HandleDMgrEventL");

    TInt32 downloadId;
    aDownload.GetIntAttribute( EDlAttrId, downloadId );

#ifdef _DEBUG

    MPX_DEBUG4("CVcxMyVideosDownloadUtil::HandleDMgrEventL(DL ID: %d, %S, %S)",
            downloadId,
            &GetS60DlProgressDes(aEvent.iProgressState),
            &GetS60DlStateDes(aEvent.iDownloadState));

    if ( aEvent.iProgressState == EHttpContentTypeReceived )
        {
        const TInt debugBufMaxSize = 255;
        TBuf<debugBufMaxSize> contentType;
        aDownload.GetStringAttribute( EDlAttrContentType, contentType );
        MPX_DEBUG2("CVcxMyVideosDownloadUtil:: Content type = %S", &contentType);
        }
#endif

    TInt8 progress( 0 );
    TUint64 downloaded( 0 );
    
    TInt32 remoteFileSize;
    aDownload.GetIntAttribute( EDlAttrLength, remoteFileSize );
    
    if ( remoteFileSize > 0 )
        {
        TBool silent;
        aDownload.GetBoolAttribute( EDlAttrSilent, silent );
        TBool progressive;
        aDownload.GetBoolAttribute( EDlAttrProgressive, progressive );        
        if ( !silent && !progressive )
            {
            // remote file size received, we can set download to silent mode.
            MPX_DEBUG2("CVcxMyVideosDownloadUtil:: setting download %d to silent mode", downloadId);
            aDownload.SetBoolAttribute( EDlAttrSilent, ETrue );
            }

        if ( silent && progressive )
            {
            aDownload.SetBoolAttribute( EDlAttrSilent, EFalse );
            }
            
        progress = DownloadProgress( aDownload, downloaded, EFalse );
        }
        
    TVcxMyVideosDownloadState state;
    
    switch ( aEvent.iDownloadState )
        {
        case EHttpDlPaused:
            {
            state = EVcxMyVideosDlStatePaused;
            iObserver.HandleDlEvent( state, static_cast<TUint32>(downloadId),
                    progress, downloaded, 0, 0 );
            NotifyIfNoActiveDownloads();
            StopProgressTimer();
            }
            break;
            
        case EHttpDlCompleted:
        case EHttpDlMultipleMOCompleted:
            state = EVcxMyVideosDlStateDownloaded;
            iObserver.HandleDlEvent(
                    state, static_cast<TUint32>(downloadId), progress, downloaded, 0, 0 );
            DeleteDownloadAsync( downloadId, EFalse /* dont delete content */ );
            StopProgressTimer();
            break;
            
        case EHttpDlFailed:
        case EHttpDlMultipleMOFailed:
            {
            TInt32 errorId;
            TInt32 globalErrorId;
            aDownload.GetIntAttribute( EDlAttrErrorId, errorId );
            aDownload.GetIntAttribute( EDlAttrGlobalErrorId, globalErrorId );
            MPX_DEBUG2("CVcxMyVideosDownloadUtil:: DL error ID: %d", errorId);
            MPX_DEBUG2("CVcxMyVideosDownloadUtil:: DL global error ID: %d", globalErrorId);

            state = EVcxMyVideosDlStateFailed;
            iObserver.HandleDlEvent(
                    state,
                    static_cast<TUint32>(downloadId),
                    progress,
                    downloaded,
                    errorId,
                    globalErrorId );
            NotifyIfNoActiveDownloads();
            StopProgressTimer();
            }
            break;

            
        case EHttpDlCreated:            
        case EHttpDlInprogress:           
        case EHttpDlMultipleMOStarted:
            iObserver.HandleDlEvent( EVcxMyVideosDlStateDownloading,
                    static_cast<TUint32>(downloadId), progress, downloaded, 0, 0 );
            StartProgressTimer();
            break;       

        /// Download is going to be deleted. Cancel or download complete will trigger this.
        case EHttpDlDeleting:
        /// Download is deleted from another client instance. Normally not received, included here just to be sure.
        case EHttpDlDeleted:
            {
            NotifyIfNoActiveDownloads();
            StopProgressTimer();
            }
            break;

        /// download is moved from one client instance to another one.
        case EHttpDlMoved:
        /// MMC card or other storage media is removed from the phone.
        case EHttpDlMediaRemoved:
        /** MMC card or other storage media inserted and 
        * downloaded content file found on it. 
        * If MMC card inserted, but (partially) downloaded content file 
        * is not found on it, download is failed with error reason
        * EContentFileIntegrity. */
        case EHttpDlMediaInserted:
        /** Download process can be paused again. This event only occurs after
        * EHttpDlNonPausable. */
        case EHttpDlPausable:
        /// Download process cannot be paused, or the content will be lost.
        case EHttpDlNonPausable:
        /// Download is started when it's already progressing
        case EHttpDlAlreadyRunning:
        /// Internally used download event. Do NOT deal with them
        case EHttpDlCancelTransaction:

        default:
            break;
        }

    }

// ---------------------------------------------------------
// CVcxMyVideosDownloadUtil::Download
// ---------------------------------------------------------
//        
RHttpDownload* CVcxMyVideosDownloadUtil::Download( TUint32 aDownloadId )
    {
    if ( aDownloadId == 0 )
        {
        return NULL;
        }
    
    TInt count = iDownloadManager.CurrentDownloads().Count();
    TInt i;
    TInt32 downloadId;
    
    for ( i = 0; i < count; i++ )
        {
        iDownloadManager.CurrentDownloads()[i]->GetIntAttribute( EDlAttrId, downloadId );
        if ( downloadId == aDownloadId )
            {
            return iDownloadManager.CurrentDownloads()[i];
            }
        }
    
    MPX_DEBUG2("CVcxMyVideosDownloadUtil:: no download found with %d ID", aDownloadId);        
    return NULL;
    }

// ---------------------------------------------------------
// CVcxMyVideosDownloadUtil::Download
// ---------------------------------------------------------
//        
RHttpDownload* CVcxMyVideosDownloadUtil::Download( const TDesC& aFileName )
    {    
    MPX_FUNC("CVcxMyVideosDownloadUtil::Download()");

    TInt count = iDownloadManager.CurrentDownloads().Count();
    TInt i;
    
    for ( i = 0; i < count; i++ )
        {
        const TInt KMaxFileNameLength = 256;
        TBuf<KMaxFileNameLength> fileName;
        iDownloadManager.CurrentDownloads()[i]->GetStringAttribute( EDlAttrDestFilename, fileName );

        MPX_DEBUG3("CVcxMyVideosDownloadUtil:: download[%d] filename = %S", i, &fileName);        

        if ( aFileName == fileName )
            {
            return iDownloadManager.CurrentDownloads()[i];
            }
        }
    
    MPX_DEBUG2("CVcxMyVideosDownloadUtil:: no download found with %S filename", &aFileName);        
    return NULL;
    }

// ---------------------------------------------------------
// CVcxMyVideosDownloadUtil::Downloads
// ---------------------------------------------------------
//        
const CDownloadArray& CVcxMyVideosDownloadUtil::Downloads()
    {    
    return iDownloadManager.CurrentDownloads();
    }

// ---------------------------------------------------------
// CVcxMyVideosDownloadUtil::CancelDownload
// ---------------------------------------------------------
//
TInt CVcxMyVideosDownloadUtil::CancelDownload( TInt32 aId, TBool aDeleteContent )
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::CancelDownload");

    if ( iDownloadManager.CurrentDownloads().Count() < 1 )
        {
        return KErrNotFound;
        }
    
    RHttpDownload* download = Download( aId );
    
    if ( !download )
        {
        return KErrNotFound;
        }

    MPX_DEBUG1("CVcxMyVideosDownloadUtil:: setting download to non silent mode");
    download->SetBoolAttribute( EDlAttrSilent, EFalse );

    if ( aDeleteContent )
        {
        MPX_DEBUG1("CVcxMyVideosDownloadUtil:: deleting content");
        download->Reset();
        }

    MPX_DEBUG1("CVcxMyVideosDownloadUtil:: deleting download from dl manager");
    download->Delete();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::RequestIsRoamingAllowedL
// -----------------------------------------------------------------------------
//    
TBool CVcxMyVideosDownloadUtil::RequestIsRoamingAllowedL()
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::RequestIsRoamingAllowedL");
    iDownloadManager.Disconnect();
    iRoamingOngoing = ETrue;
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::IapChangedL
// -----------------------------------------------------------------------------
// 
void  CVcxMyVideosDownloadUtil::IapChangedL()
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::IapChangedL");	
    if ( !iRoamingOngoing )
        {
		// This is not roaming situation, must not call GetIap()
        return;
        }
    iRoamingOngoing = EFalse;
    TUint32 iap( 0 );
    TInt err = iConnUtil->GetIap( iap, EFalse );
    if ( err == KErrNone )
        {
        iDownloadManager.SetIntAttribute( EDlMgrIap, iap );
        // Since download pause state is always result of download manager, not from user,
        // we can resume all
        iDownloadManager.StartAll();
        }
    else
        {
        MPX_DEBUG2("CVcxMyVideosDownloadUtil::IapChangedL failed to get IAP from connectionutility: %d", err); 
        }
    }   

// -----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::GetDownloadState
// Gets MPX/MDS version of the download state.
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::GetDownloadState(
        RHttpDownload& aDownload,
        TVcxMyVideosDownloadState& aDownloadState )
    {
    TInt32 dlStateInDlManager;
    aDownload.GetIntAttribute( EDlAttrState, dlStateInDlManager );

    switch( dlStateInDlManager )
        {
        case EHttpDlPaused:
            aDownloadState = EVcxMyVideosDlStatePaused;
            break;
            
        case EHttpDlCompleted:
            aDownloadState = EVcxMyVideosDlStateDownloaded;
            break;
            
        case EHttpDlFailed:
            aDownloadState = EVcxMyVideosDlStateFailed;
            break;
            
        case EHttpDlCreated:            
        case EHttpDlInprogress:
            aDownloadState = EVcxMyVideosDlStateDownloading;
            break;
            
        default:
            aDownloadState = EVcxMyVideosDlStateNone;
            break;
        }
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::DownloadProgress
// -----------------------------------------------------------------------------
//
TInt8 CVcxMyVideosDownloadUtil::DownloadProgress( RHttpDownload& aDownload, TUint64& aDownloaded,
                                                   TBool aAllowSilentReset )
    {
    TInt64 downloaded;
    TInt32 fullSize;

    TBool isProgressive;
    aDownload.GetBoolAttribute( EDlAttrProgressive, isProgressive );

    if ( isProgressive )
        {
        // this doesn't work if download is in silent mode
        // unless it is in progressive mode
        TInt32 downloaded32;
        aDownload.GetIntAttribute( EDlAttrDownloadedSize, downloaded32 );
        downloaded  = downloaded32;
        }
    else
        {
        const TInt KMaxFileNameLength = 256;
        TBuf<KMaxFileNameLength> fileName;
        aDownload.GetStringAttribute( EDlAttrDestFilename, fileName );
    
        TEntry entry;
        TInt err = iFs.Entry( fileName, entry );
        
        if ( err != KErrNone )
            {
            MPX_DEBUG1("CVcxMyVideosDownloadUtil:: Could not find file entry.");
            aDownloaded = 0;
            return 0;
            }
            
        downloaded = entry.iSize;
        }

    aDownload.GetIntAttribute( EDlAttrLength, fullSize );
    
    if( aAllowSilentReset && fullSize == KErrNotFound )
        {    
        
        TBool silent;
        aDownload.GetBoolAttribute( EDlAttrSilent, silent ) ;
        if ( silent )
            {
            // if file size has corrupted, remove silent mode so that
            // HandleDlMgrEvent can refresh size. This happens, in case
            // we resume dl which have to start from the beginning
            aDownload.SetBoolAttribute( EDlAttrSilent, EFalse );            
            }
        }
    TInt64 progress64( 0 );
    aDownloaded = downloaded;
    TInt64 fullsize64( fullSize );
    
    if ( fullsize64 > 0 )
        {
        progress64 = 100 * aDownloaded / fullsize64;
        }
    
    return static_cast<TInt8>( progress64 );
    }
    
#ifdef _DEBUG
// -----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::GetS60DlStateDes
// -----------------------------------------------------------------------------
//
const TDesC& CVcxMyVideosDownloadUtil::GetS60DlStateDes( TInt32 aState )
    {
    switch ( aState )
        {
        case EHttpDlCreated:
            return KVcxHttpDlCreatedDes;
            
        case EHttpDlInprogress:
            return KVcxHttpDlInprogressDes;
            
        case EHttpDlPaused:
            return KVcxHttpDlPausedDes;
            
        case EHttpDlCompleted:
            return KVcxHttpDlCompletedDes;
            
        case EHttpDlFailed:
            return KVcxHttpDlFailedDes;
            
        case EHttpDlMoved:
            return KVcxHttpDlMovedDes;
            
        case EHttpDlMediaRemoved:
            return KVcxHttpDlMediaRemovedDes;
            
        case EHttpDlMediaInserted:
            return KVcxHttpDlMediaInsertedDes;
            
        case EHttpDlPausable:
            return KVcxHttpDlPausableDes;
            
        case EHttpDlNonPausable:
            return KVcxHttpDlNonPausableDes;

        case EHttpDlDeleted:
            return KVcxHttpDlDeletedDes;
            
        case EHttpDlAlreadyRunning:
            return KVcxHttpDlAlreadyRunningDes;

        case EHttpDlDeleting:
            return KVcxHttpDlDeletingDes;

        case EHttpDlMultipleMOStarted:
            return KVcxHttpDlMultipleMOStartedDes;
            
        case EHttpDlMultipleMOCompleted:
            return KVcxHttpDlMultipleMOCompletedDes;
            
        case EHttpDlMultipleMOFailed:
            return KVcxHttpDlMultipleMOFailedDes;
            
        default:
            return KVcxUnknownStateDes;
        }
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::GetS60DlProgressDes
// -----------------------------------------------------------------------------
//
const TDesC& CVcxMyVideosDownloadUtil::GetS60DlProgressDes( TInt32 aProgress )
    {
    switch ( aProgress )
        {
        case EHttpProgNone:
            return KVcxHttpProgNoneDes;
            
        case EHttpStarted:
            return KVcxHttpStartedDes;
            
        case EHttpProgCreatingConnection:
            return KVcxHttpProgCreatingConnectionDes;
            
        case EHttpProgConnectionNeeded:
            return KVcxHttpProgConnectionNeededDes;
        
        case EHttpProgConnected:
            return KVcxHttpProgConnectedDes;
            
        case EHttpProgConnectionSuspended:
            return KVcxHttpProgConnectionSuspendedDes;
            
        case EHttpProgDisconnected:
            return KVcxHttpProgDisconnectedDes;
            
        case EHttpProgDownloadStarted:
            return KVcxHttpProgDownloadStartedDes;
            
        case EHttpContentTypeRequested:
            return KVcxHttpContentTypeRequestedDes;
            
        case EHttpContentTypeReceived:
            return KVcxHttpContentTypeReceivedDes;
            
        case EHttpProgSubmitIssued:
            return KVcxHttpProgSubmitIssuedDes;
            
        case EHttpProgResponseHeaderReceived:
            return KVcxHttpProgResponseHeaderReceivedDes;
            
        case EHttpProgResponseBodyReceived:
            return KVcxHttpProgResponseBodyReceivedDes;
            
        case EHttpProgRedirectedPermanently:
            return KVcxHttpProgRedirectedPermanentlyDes;
            
        case EHttpProgRedirectedTemporarily:
            return KVcxHttpProgRedirectedTemporarilyDes;
            
        case EHttpProgDlNameChanged:
            return KVcxHttpProgDlNameChangedDes;
        
        case EHttpProgContentTypeChanged:
            return KVcxHttpProgContentTypeChangedDes;
            
        case EHttpProgCodDescriptorDownloaded:
            return KVcxHttpProgCodDescriptorDownloadedDes;
            
        case EHttpProgCodDownloadStarted:
            return KVcxHttpProgCodDownloadStartedDes;
            
        case EHttpProgCodDescriptorAccepted:
            return KVcxHttpProgCodDescriptorAcceptedDes;
            
        case EHttpProgCodLoadEnd:
            return KVcxHttpProgCodLoadEndDes;
            
        case EHttpProgSupportedMultiPart:
            return KVcxHttpProgSupportedMultiPartDes;
            
        case EHttpProgMovingContentFile:
            return KVcxHttpProgMovingContentFileDes;
            
        case EHttpProgContentFileMoved:
            return KVcxHttpProgContentFileMovedDes;
            
        default:
            return KVcxUnknownProgressDes;
        }
    }
#endif

// -----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::DeleteDownloadAsync
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::DeleteDownloadAsync( TInt32 aDownloadId, TBool aDeleteContent )
    {
    iDownloadsToDelete.Append( aDownloadId );
    iDeleteContent.Append( aDeleteContent );
    if ( !iDownloadDeleteTimer->IsActive() )
        {
        iDownloadDeleteTimer->After( 0 );
        }
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::TimerExpired
// -----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::TimerExpired( CVcxMyVideosTimer* aTimer )
    {
    if ( aTimer == iDownloadDeleteTimer )
        {
        for ( TInt i = 0; i < iDownloadsToDelete.Count(); i++ )
            {
            CancelDownload( iDownloadsToDelete[i], iDeleteContent[i] );
            }
        iDownloadsToDelete.Reset();
        iDeleteContent.Reset();
        return;
        }

    if ( aTimer == iDownloadProgressTimer )
        {
        const CDownloadArray& downloads = Downloads();
        
        TInt count( downloads.Count() );
        TInt32 downloadId;
        TInt8 progress( 0 );
        TUint64 downloaded;
        TVcxMyVideosDownloadState downloadState;
        
        for ( TInt i = 0; i < count; i++ )
            {
            GetDownloadState( *(downloads[i]), downloadState );
            if ( downloadState == EVcxMyVideosDlStateDownloading || 
                 downloadState == EVcxMyVideosDlStatePaused )
                {
                downloads[i]->GetIntAttribute( EDlAttrId, downloadId );
                progress = DownloadProgress( *(downloads[i]), downloaded, 
                                            ( downloadState == EVcxMyVideosDlStateDownloading ) );
                iObserver.HandleDlEvent( downloadState,
                    static_cast<TUint32>(downloadId), progress, downloaded, 0, 0 );
                }
            }
        NotifyIfNoActiveDownloads(); // Handles connection lost etc. state updates correctly.
        StartProgressTimer();
        return;
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::ClearOrphanDownloadsL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::ClearOrphanDownloadsL( CMPXMedia& aVideoList )
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::ClearOrphanDownloadsL");
    
    const CDownloadArray& downloads = Downloads();

    CMPXMediaArray* videoArray = aVideoList.Value<CMPXMediaArray>(
                                KMPXMediaArrayContents);    
    
    TBool found;
    TInt32 downloadId;

    for ( TInt i = 0; i < downloads.Count(); i++ )
        {
        downloads[i]->GetIntAttribute( EDlAttrId, downloadId );
        found = EFalse;
        for ( TInt j = 0; j < videoArray->Count(); j++ )
            {
            if ( (*videoArray)[j]->IsSupported( KVcxMediaMyVideosDownloadId ))
                {
                if ( (*videoArray)[j]->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId ) ==
                        downloadId )
                    {
                    found = ETrue;
                    break;
                    }
                }
            }
        if ( !found )
            {
            MPX_DEBUG2("CVcxMyVideosDownloadUtil:: download id (%d) not found from MPX/MDS, deleting download!", downloadId );
            CancelDownload( downloadId, ETrue /* delete content */);
            }
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::NotifyDownloadStarted
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::NotifyDownloadStarted()
    {
    iMediatorEventProvider->RaiseEvent( TUid::Uid( KVcxNsMpxMediatorDomain ),
                                        TUid::Uid( KVcxNsMpxMediatorCategory ), 
                                        KVcxNsMpxEventDownloadStarted ,
                                        TVersion( KVcxNsMpxEventVersion, 0, 0 ),
                                        KNullDesC8() );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::NotifyIfNoActiveDownloads
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::NotifyIfNoActiveDownloads()
    {
    if ( !ActiveDownloadCount() )
        {
        iMediatorEventProvider->RaiseEvent( TUid::Uid( KVcxNsMpxMediatorDomain ),
                                            TUid::Uid( KVcxNsMpxMediatorCategory ), 
                                            KVcxNsMpxEventAllDownloadsEnded,
                                            TVersion( KVcxNsMpxEventVersion, 0, 0 ),
                                            KNullDesC8() );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::NotifyDownloadCompleted
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::NotifyDownloadCompleted( const TDesC8& aMsg )
    {
    iMediatorEventProvider->RaiseEvent( TUid::Uid( KVcxNsMpxMediatorDomain ),
                                        TUid::Uid( KVcxNsMpxMediatorCategory ), 
                                        KVcxNsMpxEventDownloadCompleted,
                                        TVersion( KVcxNsMpxEventVersion, 0, 0 ),
                                        aMsg );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::NotifyNewVideosCountDecreased
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::NotifyNewVideosCountDecreased( const TDesC8& aMsg )
    {
    iMediatorEventProvider->RaiseEvent( TUid::Uid( KVcxNsMpxMediatorDomain ),
                                        TUid::Uid( KVcxNsMpxMediatorCategory ), 
                                        KVcxNsMpxEventNewVideosCountDecreased,
                                        TVersion( KVcxNsMpxEventVersion, 0, 0 ),
                                        aMsg );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::ActiveDownloadCount
// ----------------------------------------------------------------------------
//
TInt32 CVcxMyVideosDownloadUtil::ActiveDownloadCount()
    {
    const CDownloadArray& dlarray = Downloads();
    
    TInt32 dlCount( 0 );
    
    TVcxMyVideosDownloadState state;
    
    for ( TInt i = dlarray.Count() - 1; i >=0; i-- )
        {
        GetDownloadState( *(dlarray[i]), state );

        if( state == EVcxMyVideosDlStateDownloading )
            {
            dlCount++;
            }
        }
    return dlCount;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::CreateFilePathL
// ----------------------------------------------------------------------------
//
HBufC* CVcxMyVideosDownloadUtil::CreateFilePathL( const CMPXMedia& aMedia )
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::CreateFilePathL");
    
    HBufC* path = HBufC::NewL( KMaxPathLength );
    CleanupStack::PushL( path ); // 1->
    
    TPtr pathPtr( path->Des() );
    
    UsedMemoryDesL(); // generates new value to iUsedMemoryDrivePath
    
    iSubDir.Zero();
        
    TInt counter = 0;
    GeneratePathL( aMedia, pathPtr, counter );
        
    while( BaflUtils::FileExists( iFs, pathPtr ) )
        {
        GeneratePathL( aMedia, pathPtr, ++counter );                
        }

    TText badChar( ' ' );
    TInt badCharLoc( KErrNotFound );
    while ( ! iFs.IsValidName( pathPtr, badChar ) )
        {
        if ( badChar != ' ' )
            {
            badCharLoc = pathPtr.LocateReverse( badChar );
            if ( KErrNotFound != badCharLoc )
                {
                pathPtr[badCharLoc] = '_';
                }
            }
        // Note, for some reason File Server's IsValidName() returns EFalse if there
        // is question mark as last character of string, but fails to return the
        // correct character in parameter badChar. Must get past that to avoid never
        // ending loop. Reported to symbian with EJSI-7LZG8C.
        else
            {
            for ( TInt i = 0; i < pathPtr.Length(); i++ )
                {
                if ( pathPtr[i] == '?' || pathPtr[i] == '*' )
                    {
                    pathPtr[i] = '_';
                    }
                }
            }
        }

    MPX_DEBUG2("CVcxMyVideosDownloadUtil:: path = %S", path);

    CleanupStack::Pop( path ); // <-1
    return path;
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::GeneratePathL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::GeneratePathL( const CMPXMedia& aMedia, TDes& aPath, TInt aCounter )
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::GeneratePathL");

    _LIT( KBasePathNormal, "\\My Videos\\Downloads\\" );
    _LIT( KBasePathSystemDrive, "\\Data\\My Videos\\Downloads\\" );
    
    aPath.Zero();
    aPath = iUsedMemoryDrivePath;
    
    if ( iUsedDrive == iFs.GetSystemDrive() )
        {
        aPath.Append( KBasePathSystemDrive );
        }
    else
        {
        aPath.Append( KBasePathNormal );
        }
        
    if ( iSubDir.Length() == 0 )
        {
        FindSubDirL( aPath ); // recalculates iSubDir
        }
        
    aPath.Append( iSubDir );

    TPtrC title( aMedia.ValueText( KMPXMediaGeneralTitle ) );
    if ( title.Length() == 0 )
        {
        _LIT( KNameless, "Default");
        aPath.Append( KNameless );
        }
    else
        {
        const TInt KVcxMaxCounterAndExtension( 12 ); // 6 + 6
        TInt maxTitle = aPath.MaxLength() - aPath.Length() - KVcxMaxCounterAndExtension;
        aPath.Append( title.Left( title.Length() <= maxTitle ? title.Length() : maxTitle ) );
        }

    //001-009
    //010-099

    if ( aCounter > 0 )
        {    
        if ( aCounter < 10 )
            {
            aPath.Append( _L("_(00") );
            }
        else if ( aCounter > 9 && aCounter < 100 )
            {
            aPath.Append( _L("_(0") );
            }
        TBuf<8> buf;
        buf.Format( _L("%d)"), aCounter );
        aPath.Append( buf );
        }

    // The algorithm is not final, this is the best we can do with current SW.
    // See errors EJSI-7LZHLU, EJSI-7M2BW7, EJSI-7M2HMV.
    //
    TBool extFound( EFalse );
    TInt extPos = aMedia.ValueText( KVcxMediaMyVideosRemoteUrl ).LocateReverse('.');
    if ( extPos != KErrNotFound )
        {
        TPtrC extension( aMedia.ValueText( KVcxMediaMyVideosRemoteUrl ).Mid( extPos ) );
        TInt extensionLength( KErrNotFound );
        
        if ( extension.FindF( KVcxExtensionDcf ) == 0 )
            {
            extensionLength = KVcxExtensionDcf().Length();
            }
        else if ( extension.FindF( KVcxExtensionOcf ) == 0 )
            {
            extensionLength = KVcxExtensionOcf().Length();
            }
        else if ( extension.FindF( KVcxExtensionDm ) == 0 )
            {
            extensionLength = KVcxExtensionDm().Length();
            }
        else if ( extension.FindF( KVcxExtensionMp4 ) == 0 )
            {
            extensionLength = KVcxExtensionMp4().Length();
            }
        else if ( extension.FindF( KVcxExtensionMpeg4 ) == 0 )
            {
            extensionLength = KVcxExtensionMpeg4().Length();
            }
        else if ( extension.FindF( KVcxExtensionM4v ) == 0 )
            {
            extensionLength = KVcxExtensionM4v().Length();
            }
        else if ( extension.FindF( KVcxExtension3gpp ) == 0 )
            {
            extensionLength = KVcxExtension3gpp().Length();
            }
        else if ( extension.FindF( KVcxExtension3gp ) == 0 )
            {
            extensionLength = KVcxExtension3gp().Length();
            }
        else if ( extension.FindF( KVcxExtensionRm ) == 0 )
            {
            extensionLength = KVcxExtensionRm().Length();
            }
        else if ( extension.FindF( KVcxExtensionMpg4 ) == 0 )
            {
            extensionLength = KVcxExtensionMpg4().Length();
            }
        else if ( extension.FindF( KVcxExtensionRmvb ) == 0 )
            {
            extensionLength = KVcxExtensionRmvb().Length();
            }
        else if ( extension.FindF( KVcxExtension3g2 ) == 0 )
            {
            extensionLength = KVcxExtension3g2().Length();
            }    
        else if ( extension.FindF( KVcxExtensionWmv ) == 0 )
            {
            extensionLength = KVcxExtensionWmv().Length();
            }    
        else if ( extension.FindF( KVcxExtensionAsf ) == 0 )
            {
            extensionLength = KVcxExtensionAsf().Length();
            }            

        if ( extensionLength > 0 )
            {
            // Remove possible extra characters after actual extension.
            if ( extension.Length() > extensionLength )
                {
                extension.Set( extension.Left( extensionLength ) );
                }

            extFound = ETrue;
            aPath.Append( extension );
            }
        }

    if ( ! extFound )
        {
        aPath.Append( KVcxExtensionMp4 );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::FindSubDirL
// Finds subdir and stores it to iSubDir
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::FindSubDirL( const TDesC& aPath )
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::FindSubDirL");

    MPX_DEBUG2("CVcxMyVideosDownloadUtil:: aPath = %S", &aPath);

    _LIT(KSubDirFormatterDes, "%d\\");

    TBuf<KMaxPathLength> path;
    TBuf<KVcxMaxSubDirsLength> foundSubDirs;
    TBuf<KVcxMaxSubDirLength> currentSubDir;
        
    //search subdirs
    TBool found;
    TInt subDirCounter;
    TInt fileCount = 0;
    for ( TInt i = 0; i < KVcxSubDirLevels; i++ )
        {
        found         = EFalse;
        subDirCounter = 1;
        while( !found )
            {
            currentSubDir.Format( KSubDirFormatterDes, subDirCounter );
            path.Zero();
            path.Append( aPath ); 
            path.Append( foundSubDirs ); 
            path.Append( currentSubDir );        
            MPX_DEBUG2("CVcxMyVideosDownloadUtil:: testing path: %S", &path);
            fileCount = FileCountL( path );
            MPX_DEBUG2("CVcxMyVideosDownloadUtil:: file count = %d", fileCount);
            if ( fileCount < KVcxMaxItemsInDir )
                {
                found = ETrue;
                foundSubDirs.Append( currentSubDir );
                }
            else
                {
                subDirCounter++;
                }
            }
        }
    
    iSubDir = foundSubDirs;       
    
    MPX_DEBUG2("CVcxMyVideosDownloadUtil:: calling ensurepath for %S", &path);
    BaflUtils::EnsurePathExistsL( iFs, path );
    
    MPX_DEBUG2("CVcxMyVideosDownloadUtil:: iSubDir = %S", &iSubDir);
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::FileCount
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosDownloadUtil::FileCountL( const TDesC& aPath )
    {
    TInt count = 0; // return 0 in case of fail
    
    CDir* dir = NULL;
    TInt err = iFs.GetDir( aPath, KEntryAttNormal | KEntryAttMatchMask, ESortBySize, dir );
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosDownloadUtil:: GetDir failed: %d", err);
        }
    else
        {
        count = dir->Count();
        }
    delete dir;
    return count;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::UsedMemoryDesL
// ----------------------------------------------------------------------------
//
const TDesC& CVcxMyVideosDownloadUtil::UsedMemoryDesL()
    {
    MPX_FUNC("CVcxMyVideosDownloadUtil::UsedMemoryDesL");

    CVcxMyVideosDriveMonitor* driveMonitor = CVcxMyVideosDriveMonitor::NewL( iFs );
    CleanupStack::PushL( driveMonitor ); // 1->
    driveMonitor->GetUsedMemoryL( iUsedDrive );    
    CleanupStack::PopAndDestroy( driveMonitor ); // <-1
               
    TInt driveChar = 'a' + iUsedDrive;
    iUsedMemoryDrivePath.Zero();
    iUsedMemoryDrivePath.Append( driveChar );
    _LIT(KColon, ":"); 
    iUsedMemoryDrivePath.Append( KColon );
    return iUsedMemoryDrivePath;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::StopProgressTimer
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::StopProgressTimer()
    {
    if ( ActiveDownloadCount() == 0 )
        {
        MPX_DEBUG1("CVcxMyVideosDownloadUtil:: stopped iDownloadProgressTimer");
        iDownloadProgressTimer->Cancel();
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosDownloadUtil::StartProgressTimer
// ----------------------------------------------------------------------------
//
void CVcxMyVideosDownloadUtil::StartProgressTimer()
    {
    if ( !iDownloadProgressTimer->IsActive() && ActiveDownloadCount() > 0 )
        {
        MPX_DEBUG1("CVcxMyVideosDownloadUtil:: started iDownloadProgressTimer");
        iDownloadProgressTimer->After( 3000000 );
        }
    }
