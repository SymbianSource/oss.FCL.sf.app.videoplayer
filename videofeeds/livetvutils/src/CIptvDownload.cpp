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
* Description:    Http download handler for epg plugin*
*/



#include "IptvLiveLogger.h"
#include "CIptvEpgService.h"
#include "CIptvDownload.h"
#include "MIptvDownloadObserver.h"
#include "CIptvLiveUIObjectTimer.h"

// ---------------------------------------------------------
// CIptvDownload::NewL
//
// ---------------------------------------------------------
//
EXPORT_C CIptvDownload* CIptvDownload::NewL( MIptvDownloadObserver* aObserver )
	{
	CIptvDownload* self = new (ELeave) CIptvDownload( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CIptvDownload::CIptvDownload
//
// ---------------------------------------------------------
//
CIptvDownload::CIptvDownload( MIptvDownloadObserver* aObserver ) :
	iObserver( aObserver ), iConnected( EFalse )
	{
	}

// ---------------------------------------------------------
// CIptvDownload::~CIptvDownload
//
// ---------------------------------------------------------
//
CIptvDownload::~CIptvDownload()
	{
	if ( iConnected )
		{
		iDownloadManager.RemoveObserver( *this );
		}
   	iDownloadManager.Close();
	iService = NULL;

	delete iTimer;
	iTimer = NULL;
	}

// ---------------------------------------------------------
// CIptvDownload::ConstructL
//
// ---------------------------------------------------------
//
void CIptvDownload::ConstructL()
	{
    iTimer = CIptvLiveUIObjectTimer::NewL( this );
	}

// ---------------------------------------------------------
// CIptvDownload::SetService
//
// ---------------------------------------------------------
//
EXPORT_C void CIptvDownload::SetService( CIptvEpgService* aService )
	{
	iService = aService;
	TRAPD( err, InitializeDownloadL() );
	if ( err != KErrNone )
		{
		LIVE_TV_TRACE2(_L("CIptvDownload::SetService: InitializeDownloadL FAILED: %d"), err);
		}
	}

// ---------------------------------------------------------
// CIptvDownload::DownloadL
//
// ---------------------------------------------------------
//
EXPORT_C void CIptvDownload::DownloadL(
    TUid /*aUid*/,
    const TDesC8& aUri,
	TUint32 /*aIapId*/,
	const TDesC& aFileName,
	const TDesC& aETag,
	const TDesC& aModifiedSince )
	{
#if defined( LIVE_TV_RDEBUG_TRACE ) || defined( LIVE_TV_FILE_TRACE )
	TName url; url.Copy( aUri );
	LIVE_TV_TRACE2( _L("CIptvDownload::DownloadL IN, aUri: %S"), &url );
#endif // LIVE_TV_RDEBUG_TRACE || LIVE_TV_FILE_TRACE

	iFileName.Copy( aFileName );
    iDownloadETag.Copy( aETag );
    iDownloadLastModified.Copy( aModifiedSince );
    iUrl.Copy( aUri );
	
	CreateDownloadL( aUri, EFalse );

	LIVE_TV_TRACE1( _L("CIptvDownload::DownloadL OUT") );
	}

// ---------------------------------------------------------
// CIptvDownload::CreateDownloadL
//
// ---------------------------------------------------------
//
void CIptvDownload::CreateDownloadL(
    const TDesC8& aUri,
    TBool aNoContentTypeCheck )
    {
    LIVE_TV_TRACE1( _L("CIptvDownload::CreateDownloadL IN") );
    RHttpDownload &download = iDownloadManager.CreateDownloadL( aUri );
    User::LeaveIfError( download.SetStringAttribute( EDlAttrUsername, iService->iUserName ) );
    User::LeaveIfError( download.SetStringAttribute( EDlAttrPassword, iService->iPassword ) );
    User::LeaveIfError( download.SetStringAttribute( EDlAttrDestFilename, iFileName ) );
    User::LeaveIfError( download.SetBoolAttribute( EDlAttrNoContentTypeCheck, aNoContentTypeCheck ) );
    User::LeaveIfError( download.SetIntAttribute( EDlAttrAction, EMove ) );
    User::LeaveIfError( download.Start() );
    User::LeaveIfError( download.GetIntAttribute( EDlAttrId, iDownloadId ) );
    LIVE_TV_TRACE1( _L("CIptvDownload::CreateDownloadL OUT") );
    }

// ---------------------------------------------------------
// CIptvDownload::HandleDMgrEventL
//
// ---------------------------------------------------------
//
void CIptvDownload::HandleDMgrEventL(
    RHttpDownload& aDownload,
	THttpDownloadEvent aEvent )
    {
    LIVE_TV_TRACE3( _L("CIptvDownload::HandleDMgrEventL IN, aEvent.iProgress: %d, aEvent.iDownloadState %d"), aEvent.iProgressState, aEvent.iDownloadState );
	TBuf<50> responseETag;
	TBuf<50> responseModifiedSince;
	aDownload.GetStringAttribute( EDlAttrResponseETag, responseETag );
	aDownload.GetStringAttribute( EDlAttrEntityLastModified, responseModifiedSince );

    switch (aEvent.iProgressState)
        {
        // Content type received
        case EHttpContentTypeReceived:
            LIVE_TV_TRACE1(_L("EHttpContentTypeReceived"));
        	// Compare old ETag and last modified values. Length check is done
        	// because if service provides neither ETag nor last modified time
        	// we need to run update cause there is no data to verify the match.
        	if( ( responseETag.Length() > 0 && iDownloadETag.Compare( responseETag ) == 0 ) ||
        		( responseModifiedSince.Length() > 0 && iDownloadLastModified.Compare( responseModifiedSince ) == 0 ) )
        		{
        		// Delete download asynchronously and signalize the observer
        		// that data is up to date -> no need to download
        		iDlError = EIptvDlContentUnchanged;
        		DeleteDownloadAsyncL();
        		}
        	// Neither ETag nor last modified values didn't match -> do download
        	else
        		{
                // Content has changed. Delete this download and create a new to get
                // content.
        		iRestartDownloadAfterDeletion = ETrue;
        		DeleteDownloadAsyncL();
        		}
            break;

        case EHttpProgDisconnected:
            {
            LIVE_TV_TRACE1(_L("EHttpProgDisconnected"));
            TIptvDlError dlError( EIptvDlNoError );
            ResolveDownloadErrorCode( aDownload, dlError );
            // Connection lost, current Download Manager version sends
            // this progress state if connection is stopped right after the
            // download is initialized and started. In that case it never
            // sends the EHttpDlFailed so lets finish our work here:
            iDlError = dlError;
    		DeleteDownloadAsyncL();
            }
            break;

        default:
            break;
        }

	switch (aEvent.iDownloadState)
        {
        case EHttpDlCompleted:
            {
            LIVE_TV_TRACE1(_L("EHttpDlCompleted"));
            iDlError = KErrNone;
    		DeleteDownloadAsyncL();
			// Send new modified since and ETag to the caller
			iObserver->SetModifiedSinceAndETag( responseModifiedSince, responseETag );
            // Set the values to an empty strings. These strings are re-set at
            // DownloadL method with the values given by the caller.
            iDownloadLastModified.Zero();
            iDownloadETag.Zero();
            }
            break;

        case EHttpDlFailed:
            {
            LIVE_TV_TRACE1(_L("EHttpDlFailed"));
            TIptvDlError dlError( EIptvDlNoError );
            ResolveDownloadErrorCode( aDownload, dlError );
            iDlError = dlError;
    		DeleteDownloadAsyncL();
            // Set the values to an empty strings. These strings are re-set at
            // DownloadL method with the values given by the caller.
            iDownloadLastModified.Zero();
            iDownloadETag.Zero();
           }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CIptvDownload::TimerFires
//
// ---------------------------------------------------------
//
void CIptvDownload::TimerFires()
    {
    LIVE_TV_TRACE1( _L("CIptvDownload::TimerFires IN") );

    if( iDeleteDownloadTimerRunning )
        {
        iDeleteDownloadTimerRunning = EFalse;
        LIVE_TV_TRACE1( _L("CIptvDownload::TimerFires() Deleting the download.") );

        TInt count = iDownloadManager.CurrentDownloads().Count();
        TInt32 downloadId;
        for ( TInt i = 0; i < count; i++ )
            {
            iDownloadManager.CurrentDownloads()[i]->GetIntAttribute(
                EDlAttrId, downloadId );
            if ( downloadId == iDownloadId )
                {
                RHttpDownload* dl = iDownloadManager.CurrentDownloads()[i];
                if ( dl )
                    {
                    dl->Delete();
                    }
                }
            }

        if( iRestartDownloadAfterDeletion )
            {
            // Create new download.
            iRestartDownloadAfterDeletion = EFalse;
            TRAP( iDlError, CreateDownloadL( iUrl, ETrue ) ); // No content type check
            if( iDlError != KErrNone )
                {
                LIVE_TV_TRACE2( _L("CIptvDownload::TimerFires() Error creating download: %d."), iDlError );
                iObserver->DownloadFinished( iDlError );
                }
            }
        else
            {
            // Inform observer that download is finished.
            LIVE_TV_TRACE2( _L("CIptvDownload::TimerFires() Sending %d to observer."), iDlError );
            iObserver->DownloadFinished( iDlError );
            }
        }
    LIVE_TV_TRACE1( _L("CIptvDownload::TimerFires OUT") );
    }

// ---------------------------------------------------------
// CIptvDownload::TimerError
//
// ---------------------------------------------------------
//
void CIptvDownload::TimerError( const TInt aError )
    {
    TRAP_IGNORE( HandleTimerErrorL( aError ) );
    }

// ---------------------------------------------------------
// CIptvDownload::HandleTimerErrorL
//
// ---------------------------------------------------------
//
void CIptvDownload::HandleTimerErrorL( const TInt aError )
    {
    LIVE_TV_TRACE1( _L("CIptvDownload::HandleTimerErrorL IN") );

    // Start the timer in case of error
    if ( aError == KErrOverflow || aError == KErrUnderflow )
        {
        delete iTimer;
        iTimer = NULL;
        iTimer = CIptvLiveUIObjectTimer::NewL( this );
        iDeleteDownloadTimerRunning = ETrue;
        iTimer->After(1);
        }

    LIVE_TV_TRACE1( _L("CIptvDownload::HandleTimerErrorL OUT") );
    }

// ---------------------------------------------------------
// CIptvDownload::InitializeDownloadL
//
// ---------------------------------------------------------
//
void CIptvDownload::InitializeDownloadL()
	{
    iDownloadManager.ConnectL( iService->iUid, *this, EFalse );
	iConnected = ETrue;
    User::LeaveIfError( iDownloadManager.SetIntAttribute(
        EDlMgrIap, iService->iIap ) );
    User::LeaveIfError( iDownloadManager.SetBoolAttribute(
        EDlMgrSilentMode, ETrue ) );
	}

// ---------------------------------------------------------
// CIptvDownload::ResolveDownloadErrorCode
//
// ---------------------------------------------------------
//
void CIptvDownload::DeleteDownloadAsyncL()
    {
    delete iTimer;
    iTimer = NULL;
    iTimer = CIptvLiveUIObjectTimer::NewL( this );
    iDeleteDownloadTimerRunning = ETrue;
    iTimer->After(1);
    }

// ---------------------------------------------------------
// CIptvDownload::ResolveDownloadErrorCode
//
// ---------------------------------------------------------
//
void CIptvDownload::ResolveDownloadErrorCode(
    RHttpDownload& aDownload,
	TIptvDlError& aError ) const
	{
    TInt32 errorId( 0 );
	TInt32 globalErrorId( 0 );
    // Get errors from aDownload object
    aDownload.GetIntAttribute(EDlAttrErrorId, errorId);
    aDownload.GetIntAttribute(EDlAttrGlobalErrorId, globalErrorId);
    LIVE_TV_TRACE3(_L("CIptvDownload::ResolveDownloadErrorCode: errorId: %d, globalErrorId: %d"), errorId, globalErrorId);

    // Handle error
    switch( errorId )
    	{
        case EConnectionFailed:
            aError = EIptvDlConnectionFailed;
            break;

        case EHttpAuthenticationFailed:
    	    aError = EIptvDlAuthFailed;
    	    break;

        case EProxyAuthenticationFailed:
    	    aError = EIptvDlProxyAuthFailed;
            break;

        case EDestFileInUse:
            aError = EIptvDlDestFileInUse;
            break;

        case EBadUrl:
            aError = EIptvDlBadUrl;
            break;

        case EMMCRemoved:
            aError = EIptvDlMmcRemoved;
            break;

        case EDiskFull:
            aError = EIptvDlDiskFull;
			break;

    	case EObjectNotFound:
    	    aError = EIptvDlContentNotFound;
    	    break;

		case ETransactionFailed:
    		{
    		if ( globalErrorId == KErrCancel )
    			{
    			// Connection stopped
				aError = EIptvDlDisconnected;
    			}
			else
				{
				aError = EIptvDlGeneral;
				}
    		}
			break;

        default:
            aError = EIptvDlGeneral;
            break;
        }
	}


// End of file
