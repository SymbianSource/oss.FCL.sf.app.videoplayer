/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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




#include <e32std.h>
#include <sysutil.h>
#include "IptvDebug.h"
#include <bautils.h>
#include "CIptvTimer.h"
#include "CIptvRssDownload.h"
#include "CIptvRssPlugin.h"
#include "CIptvEpgService.h"
#include "CIptv3XmlContentHandler.h"
#include "ciptvxmldatetime.h"

// 0x10281F1E is the uid of the IptvUtil dll. This is taken
// into use by reason following:
// There can be two simultaneous RSS downloads ongoing at the
// same time; one for search and one for vod. Download manager
// prevents simultaneous downloads if uid given during Connect
// is already active. That's why we take another uid into use
// to allow search operations to run simultaneously with the
// normal vod update.
const TUid KIptvSearchUid = { 0x10281F1E };

const TInt KIptvMaxTimeoutInSeconds( 60 );
const TInt KIptvMaxPauseInSeconds( 10 );

const TInt KIptvTime_1_second( 1000000 );
const TInt KIptvTime_200_microsecond( 200 );

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::ConstructL()
    {
    iTimer = CIptvTimer::NewL( CActive::EPriorityUserInput, *this );
    }

// --------------------------------------------------------------------------
// Two-phased constructor.
// Create instance of concrete interface implementation
// --------------------------------------------------------------------------
//
CIptvRssDownload* CIptvRssDownload::NewL(
    CIptvRssPlugin& aPlugin,
    RFs& aFs,
    CIptvXmlContentHandler& aXmlContentHandler )
    {
    CIptvRssDownload* self = new( ELeave ) CIptvRssDownload(
        aPlugin,
        aFs,
        aXmlContentHandler );
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CIptvRssDownload::~CIptvRssDownload()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::~CIptvRssDownload" );

    if ( iTimer )
        {
        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        delete iTimer;
        iTimer = NULL;
        }

    Disconnect();

    iService = NULL;

    delete iDlDeleteTimer;
    delete iFileName;
    delete iUri;
    delete iETag;

    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::~CIptvRssDownload exit" );
    }

// --------------------------------------------------------------------------
// C++ default constructor
// --------------------------------------------------------------------------
//
CIptvRssDownload::CIptvRssDownload(
    CIptvRssPlugin& aPlugin,
    RFs& aFs,
    CIptvXmlContentHandler& aXmlContentHandler ) :
    iService( NULL ),
    iPlugin( aPlugin ),
    iThumbnail( EFalse ),
    iDownloadId( 0 ),
    iTimer( NULL ),
    iConnected( EFalse ),
    iState( ENormal ),
    iPauseCounter( 0 ),
    iFs( aFs ),
    iXmlContentHandler( aXmlContentHandler )
    {
    }

// --------------------------------------------------------------------------
// Start download.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::DownloadL(
    const TDesC8& aUri,
    const TDesC& aFileName,
    TBool aThumbnail,
    const TDesC& aETag,
    const TTime& aLastUpdated )
    {
    if ( iState != ENormal && iState != EFinished )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "RSS Plugin --- CIptvRssDownload::DownloadL Busy Leave" );
        User::Leave( KErrAlreadyExists );
        }
    if ( ( 0 == aFileName.Length() ) || ( 0 == aUri.Length() ) )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "RSS Plugin --- CIptvRssDownload::DownloadL Argument Leave" );
        User::Leave( KErrArgument );
        }
    ChangeState( ENormal );

#ifdef _DEBUG

    // Debug print filename.
    TBuf<KIptvMaxPath> debugFileName;
    debugFileName.Copy( aFileName );
    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::DownloadL aFileName :         %S",
        &debugFileName );

    // Debug print Uri.
    TBuf<KIptvUrlMaxLength> debugUri;
    debugUri.Copy( aUri );
    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::DownloadL aUri :              %S",
        &debugUri );

#endif

    // When header information is given, fist dowload will fetch these values
    // for check. Thumbnail file must also exist to enable check.
    iWaitingContentTypeCheck =
        ( ( 0 != aETag.CompareC( KIptvEmptyDes ) ) ||
          ( TTime( 0LL ) != aLastUpdated ) )  &&
        !iDisableLastModifiedCheck &&
        ( !aThumbnail || BaflUtils::FileExists( iFs, aFileName ) );
    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::DownloadL Content type check: %d",
        iWaitingContentTypeCheck );

    // Store argument data for further use.
    if ( iUri )
        {
        delete iUri;
        iUri = NULL;
        }
    iUri = HBufC8::NewL( aUri.Length() );
    iUri->Des().Copy( aUri );

    if ( iFileName )
        {
        delete iFileName;
        iFileName = NULL;
        }
    iFileName = HBufC::NewL( aFileName.Length() );
    iFileName->Des().Copy( aFileName );

    if ( iETag )
        {
        delete iETag;
        iETag = NULL;
        }
    if ( 0 < aETag.Length() )
        {
        iETag = HBufC::NewL( aETag.Length() );
        iETag->Des().Copy( aETag );
        }

    iLastUpdated = aLastUpdated;
    iThumbnail = aThumbnail;

    // When no check, old thumbnail can be removed at once.
    if ( iThumbnail && iFileName && !iWaitingContentTypeCheck )
        {
        iPlugin.RemoveIconFromList( iFileName, ETrue );
        }

    // Create new download.
    RHttpDownload& download =
        iDownloadManager.CreateDownloadL( aUri );

    // Set download attributes.
    download.SetBoolAttribute( EDlAttrNoContentTypeCheck, !iWaitingContentTypeCheck );
    SetAuthenticationInformationL( download );
    if ( !iWaitingContentTypeCheck )
        {
        // Content check will not require filename and DownloadManager would anyway
        // delete it.
        download.SetStringAttribute( EDlAttrDestFilename, aFileName );
        }

    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::DownloadL Using IAP: %d",
        iService->iIap );
    User::LeaveIfError(
        iDownloadManager.SetIntAttribute( EDlMgrIap, iService->iIap ) );

    // Start download.
    User::LeaveIfError( download.Start() );
    download.GetIntAttribute( EDlAttrId, iDownloadId );
    iDownloadIdValid = ETrue;
    }

// --------------------------------------------------------------------------
// Restart download.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::DownloadL()
    {
    if ( iState != ENormal && iState != EFinished )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "RSS Plugin --- CIptvRssDownload::DownloadL Busy Leave" );
        User::Leave( KErrAlreadyExists );
        }
    if ( !iFileName || !iUri )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "RSS Plugin --- CIptvRssDownload::DownloadL Argument Leave" );
        User::Leave( KErrArgument );
        }
    ChangeState( ENormal );

#ifdef _DEBUG

    // Debug print filename.
    TBuf<KIptvMaxPath> debugFileName;
    debugFileName.Copy( iFileName->Des() );
    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::DownloadL aFileName :         %S",
        &debugFileName );

#endif

    iWaitingContentTypeCheck = EFalse;

    // Create new download.
    RHttpDownload& download =
        iDownloadManager.CreateDownloadL( iUri->Des() );

    // Set download attributes.
    download.SetBoolAttribute( EDlAttrNoContentTypeCheck, ETrue );
    SetAuthenticationInformationL( download );
    download.SetStringAttribute( EDlAttrDestFilename, iFileName->Des() );
    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::DownloadL Using IAP: %d",
        iService->iIap );
    User::LeaveIfError(
        iDownloadManager.SetIntAttribute( EDlMgrIap, iService->iIap ) );

    // Start download.
    User::LeaveIfError( download.Start() );
    download.GetIntAttribute( EDlAttrId, iDownloadId );
    iDownloadIdValid = ETrue;
    }

// --------------------------------------------------------------------------
// From MHttpDownloadMgrObserver.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::HandleDMgrEventL(
    RHttpDownload& aDownload,
    THttpDownloadEvent aEvent )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL" );

    // In EThumbnail and ENoDiskSpace states we do not need to do anything
    // for new events. We are just waiting for our timer to step in.
    // When iTimer is NULL, we are shutting down.
    if ( ( iState == EThumbnail ) || ( iState == ENoDiskSpace ) || !iTimer || iDlDeleteTimer )
        {
        return;
        }

    //lint -e{961} Else block not needed, default is no operation.
    if ( iState == EPause )
        {
        iTimer->Cancel();
        ChangeState( ENormal );
        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL PAUSED %d SECONDS, NOW CONTINUING!",
            iPauseCounter );
        iPauseCounter = 0;
        }
    else if ( iState == EWaiting )
        {
        iTimer->Cancel();
        ChangeState( ENormal );
        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL WAITED EVENTS FROM DL-MANAGER %d SECONDS",
            iPauseCounter );
        iPauseCounter = 0;
        }
    else if ( iState == ETimeout )
        {
        if ( aEvent.iDownloadState == EHttpDlCreated )
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL download state was EHttpDlCreated." );
            ChangeState( ENormal );
            iPauseCounter = 0;
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL iState == ETimeout -> return" );
            return;
            }
        }

    TInt32 fileSizeReported = 0;
    aDownload.GetIntAttribute( EDlAttrUserData, fileSizeReported );

    // We keep checking file size until it is available in EDlAttrLength.
    if ( fileSizeReported == 0 )
        {
        TInt32 fullSize = 0;

        aDownload.GetIntAttribute( EDlAttrLength, fullSize );
        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL  Full size: %d", fullSize );

        if ( fullSize > 0 )
            {
            aDownload.SetIntAttribute( EDlAttrUserData, ETrue );

            // For thumbnails, we check if size of file exceeds the maximum
            // allowed. If it does, the file is not downloaded at all.
            if ( iThumbnail && fullSize > KIptvRssMaxThumbnailSize )
                {
                IPTVLOGSTRING3_LOW_LEVEL(
                    "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL Thumbnail too big %d, max size %d, download cancelled!!!",
                    fullSize,
                    KIptvRssMaxThumbnailSize );

#ifdef _DEBUG

                    switch ( iState )
                        {
                        case ENormal:
                            IPTVLOGSTRING_LOW_LEVEL(
                                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL iState changed from ENormal -> EThumbnail" );
                            break;

                        case EThumbnail:
                            IPTVLOGSTRING_LOW_LEVEL(
                                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL iState changed from EThumbnail -> EThumbnail" );
                            break;

                        case EFinished:
                            IPTVLOGSTRING_LOW_LEVEL(
                                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL iState changed from EFinished -> EThumbnail" );
                            break;

                        default:
                            IPTVLOGSTRING_LOW_LEVEL(
                                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL iState changed from ? -> EThumbnail" );
                            break;
                        }

#endif // _DEBUG

                iState = EThumbnail;

                if ( iTimer )
                    {
                    iTimer->Cancel();
                    iTimer->After( KIptvTime_200_microsecond );
                    return;
                    }
                }
            // For all other files, those that will be actually downloaded,
            // we check that we don't exceed critical disk space levels.
            else
                {
                TBool checkResult = EFalse;
                TRAPD( checkError, checkResult =
                    SysUtil::DiskSpaceBelowCriticalLevelL(
                        &iFs, static_cast<TInt64>(fullSize), EDriveC ) );
                if ( checkError != KErrNone || checkResult )
                    {
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL Disk space under critical level!!!" );

                    ChangeState( ENoDiskSpace );

                    if ( iTimer )
                        {
                        iTimer->Cancel();
                        iTimer->After( KIptvTime_200_microsecond );
                        return;
                        }
                    }

#ifdef _DEBUG

                else
                    {
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL Disk space ok." );
                    }

#endif // _DEBUG

                }
            }
        }

    switch ( aEvent.iProgressState )
        {
        case EHttpContentTypeReceived:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpContentTypeReceived" );

            // Fetch header info to find out whether item has bee updated.
            TBuf<KMaxGeneralHeaderFieldLength> responseETag;
            TBuf<KMaxGeneralHeaderFieldLength> responseModifiedSince;
            aDownload.GetStringAttribute( EDlAttrResponseETag, responseETag );
            aDownload.GetStringAttribute( EDlAttrEntityLastModified, responseModifiedSince );
            iWaitingContentTypeCheck = EFalse;

            // Compare header info.
            TBool eTagMatch( EFalse );
            if ( iETag )
                {
                if ( responseETag.Length() > 0 )
                    {
                    eTagMatch = ( responseETag.Compare( iETag->Des() ) == 0 );

                    IPTVLOGSTRING2_LOW_LEVEL( "EDlAttrResponseETag :            %S",
                        &responseETag );
                    IPTVLOGSTRING2_LOW_LEVEL( "Local eTag valid :               %d",
                        eTagMatch );
                    }

                delete iETag;
                iETag = NULL;
                iETag = HBufC::NewL( responseETag.Length() );
                iETag->Des().Copy( responseETag );
                }

            TBool modifiedSinceMatch( EFalse );
            if ( responseModifiedSince.Length() > 0 )
                {
                TTime lastUpdated( static_cast<TUint64>( 0 ) );

                // Parse time string to TTime
                HBufC* lastUpdatedStr = HBufC::NewLC( responseModifiedSince.Length() );
                lastUpdatedStr->Des().Copy( responseModifiedSince );
                CIptvXmlDateTime::ParseGmtL( lastUpdatedStr, lastUpdated );
                CleanupStack::PopAndDestroy( lastUpdatedStr );

                modifiedSinceMatch = lastUpdated <= iLastUpdated;

#ifdef _DEBUG

                // Debug print last updated info.
                TBuf<KIptvEpgDbLastModifiedMaxLength> ecgDatePrint;
                TBuf<KIptvEpgDbLastModifiedMaxLength> itemDatePrint;
                _LIT( KIptvDatePrint, "%D%M%Y%/0%1%/1%2%/2%3%/3" );
                iLastUpdated.FormatL( ecgDatePrint, KIptvDatePrint );
                lastUpdated.FormatL( itemDatePrint, KIptvDatePrint );

                TBuf<KIptvEpgDbLastModifiedMaxLength> ecgTimePrint;
                TBuf<KIptvEpgDbLastModifiedMaxLength> itemTimePrint;
                _LIT( KIptvTimePrint, "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B" );
                iLastUpdated.FormatL( ecgTimePrint, KIptvTimePrint );
                lastUpdated.FormatL( itemTimePrint, KIptvTimePrint );

                IPTVLOGSTRING2_LOW_LEVEL( "EDlAttrEntityLastModified :      %S",
                    &responseModifiedSince );
                IPTVLOGSTRING3_LOW_LEVEL( "Local entity was last updated :  %S %S",
                    &ecgDatePrint,
                    &ecgTimePrint );
                IPTVLOGSTRING3_LOW_LEVEL( "Server entity was last updated : %S %S",
                    &itemDatePrint,
                    &itemTimePrint );
                IPTVLOGSTRING2_LOW_LEVEL( "Local entity valid :             %d",
                    modifiedSinceMatch );

#endif

                iLastUpdated = lastUpdated;

                }
            else
                {
                iLastUpdated = TTime( static_cast<TUint64>( 0 ) );
                }


            if ( eTagMatch || modifiedSinceMatch )
                {
                // Download headers match, no download required.
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL: Headers match to previous." );
                iDlStatus = EDownloadNoNeed;
                iDlError  = EIptvDlNoError;

                DeleteDownloadAsyncL();

                // Retain old Thumbnail and remove it from list.
                if ( iThumbnail && iFileName )
                    {
                    iPlugin.RemoveIconFromList( iFileName, EFalse );
                    }
                }
            else
                {
                // Download headers do not match.
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL: Headers do not match." );
                iDlStatus = EDownloadNeeded;
                iDlError  = EIptvDlNoError;
                ChangeState( ENormal );
                iRestartDownload = ETrue;

                // When service will not support header info, do not request it further.
                if ( ( 0 ==  responseETag.Length() ) &&
                     ( 0 == responseModifiedSince.Length() ) )
                    {
                    IPTVLOGSTRING_LOW_LEVEL(
                        "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL: Further header check disabled." );
                    iDisableLastModifiedCheck = ETrue; // Suppress further check
                    }

                DeleteDownloadAsyncL();

                if ( iFileName )
                    {
                    // Delete old Thumbnail if exists and remove it from list.
                    if ( iThumbnail )
                        {
                        iPlugin.RemoveIconFromList( iFileName, ETrue );
                        }
                    else
                        {
                        // Delete old file if exist.
                        if ( BaflUtils::FileExists( iFs, iFileName->Des() ) )
                            {
                            iFs.Delete( iFileName->Des() );
                            }
                        }
                    }
                }
            }
            break;

        case EHttpProgNone:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgNone" );
            }
            break;

        case EHttpStarted:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpStarted" );
            }
            break;

        case EHttpProgCreatingConnection:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgCreatingConnection" );
            }
            break;

        case EHttpProgConnectionNeeded:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgConnectionNeeded" );
            }
            break;

        case EHttpProgConnected:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgConnected" );
            }
            break;

        case EHttpProgConnectionSuspended:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgConnectionSuspended" );
            }
            break;

        case EHttpProgDisconnected:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgDisconnected" );
            }
            break;

        case EHttpProgDownloadStarted:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgDownloadStarted" );
            }
            break;

        case EHttpContentTypeRequested:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpContentTypeRequested" );
            }
            break;

        case EHttpProgSubmitIssued:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgSubmitIssued" );
            }
            break;

        case EHttpProgResponseHeaderReceived:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgResponseHeaderReceived" );
            }
            break;

        case EHttpProgResponseBodyReceived:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgResponseBodyReceived" );
            }
            break;

        case EHttpProgRedirectedPermanently:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgRedirectedPermanently" );
            }
            break;

        case EHttpProgRedirectedTemporarily:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgRedirectedTemporarily" );
            }
            break;

        case EHttpProgDlNameChanged:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgDlNameChanged" );
            }
            break;

        case EHttpProgContentTypeChanged:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgContentTypeChanged" );
            }
            break;

        case EHttpProgCodDescriptorDownloaded:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgCodDescriptorDownloaded" );
            }
            break;

        case EHttpProgCodDownloadStarted:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgCodDownloadStarted" );
            }
            break;

        case EHttpProgCodDescriptorAccepted:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgCodDescriptorAccepted" );
            }
            break;

        case EHttpProgCodLoadEnd:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgCodLoadEnd" );
            }
            break;

        case EHttpProgSupportedMultiPart:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL HttpProgSupportedMultiPart" );
            }
            break;

        case EHttpProgMovingContentFile:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgMovingContentFilep" );
            }
            break;

        case EHttpProgContentFileMoved:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpProgContentFileMoved" );
            }
            break;

        default:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL Unknown state" );
            break;
            }
        }

    switch ( aEvent.iDownloadState )
        {
        case EHttpDlCompleted:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlCompleted" );
            iDlStatus = EDownloadSucceeded;
            iDlError  = EIptvDlNoError;
            DeleteDownloadAsyncL();
            }
            break;

        case EHttpDlFailed:
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlFailed" );
            // When header download failed, we can still retry actual download.
            if ( iWaitingContentTypeCheck )
                {
                iDlStatus = EDownloadNeeded;
                iDlError  = EIptvDlNoError;
                ChangeState( ENormal );

                // Delete old Thumbnail and remove it from list.
                if ( iThumbnail && iFileName )
                    {
                    iPlugin.RemoveIconFromList( iFileName, ETrue );
                    }

                iRestartDownload = ETrue;
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL Further header check disabled." );
                iDisableLastModifiedCheck = ETrue; // Suppress further check
                iWaitingContentTypeCheck = EFalse;
                DeleteDownloadAsyncL();
                }
            else
                {
                GetDownloadErrorCode( aDownload, iDlError );
                // No state change, when only progress information.
                if ( EIptvDlNoError != iDlError )
                    {
                    iDlStatus = EDownloadFailed;
                    DeleteDownloadAsyncL();
                    }
                }
            }
            break;

        case EHttpDlCreated:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlCreated" );
            break;

        case EHttpDlInprogress:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlInprogress" );
            break;

        case EHttpDlPaused:
            {
            // Ignore paused state when content type is received because download is finished earlier.
            if ( aEvent.iProgressState != EHttpContentTypeReceived )
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlPaused" );

                ChangeState( EPause );

                if ( iTimer )
                    {
                    if ( !iTimer->IsActive() )
                        {
                        iTimer->After( KIptvTime_1_second );
                        }
                    }
                }
            }
            break;

        case EHttpDlMoved:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlMoved" );
            break;

        case EHttpDlMediaRemoved:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlMediaRemoved" );
            break;

        case EHttpDlMediaInserted:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlMediaInserted" );
            break;

        case EHttpDlPausable:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlPausable" );
            break;

        case EHttpDlNonPausable:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlNonPausable" );
            break;

        case EHttpDlDeleted:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlDeleted" );
            break;

        case EHttpDlAlreadyRunning:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlAlreadyRunning" );
            break;

        case EHttpDlDeleting:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlDeleting" );
            break;

        case EHttpDlCancelTransaction:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL EHttpDlCancelTransaction" );
            break;

        default:
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL Unknown state" );
            break;
        }

    if ( iState == ENormal )
        {
        ChangeState( EWaiting );
        if ( iTimer )
            {
            if ( !iTimer->IsActive() )
                {
                iTimer->After( KIptvTime_1_second );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// Set authentication information.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::SetAuthenticationInformationL(
    RHttpDownload& aDownload ) const
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::SetAuthenticationInformation" );

    User::LeaveIfError(
        aDownload.SetStringAttribute( EDlAttrUsername, iUserName ) );
    User::LeaveIfError(
        aDownload.SetStringAttribute( EDlAttrPassword, iPassword ) );
    }

// --------------------------------------------------------------------------
// Set service information.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::SetServiceInformation( CIptvEpgService* aService )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::SetServiceInformation" );

    iService = aService;

    if ( aService )
        {
        iUserName.Copy( aService->iUserName );
        iPassword.Copy( aService->iPassword );
        }
    }

// --------------------------------------------------------------------------
// Initialize plugin.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::InitializeL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin  --- CIptvRssDownload::InitializeL" );

    if ( !iConnected )
        {
        if ( iPlugin.IsSearchOperation() )
            {
            iDownloadManager.ConnectL( KIptvSearchUid, *this, ETrue );
            }
        else
            {
            iDownloadManager.ConnectL( iService->iUid, *this, ETrue );
            }
        iConnected = ETrue;
        }

    iDownloadManager.DeleteAll();
    }

// --------------------------------------------------------------------------
// Disconnect download manager session.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::Disconnect()
    {
    IPTVLOGSTRING_LOW_LEVEL( "RSS Plugin  --- CIptvRssDownload::Disconnect" );

    if ( iConnected )
        {
        iDownloadManager.DeleteAll();
        iDownloadManager.RemoveObserver( *this );
        iDownloadManager.Close();

        iConnected = EFalse;
        }
    }

// --------------------------------------------------------------------------
// Get download error code.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::GetDownloadErrorCode(
    RHttpDownload& aDownload, TIptvDlError& aError ) const
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin  --- CIptvRssDownload::GetDownloadErrorCode" );

    TInt32 errorId;
    TInt32 globalErrorId;
    aDownload.GetIntAttribute( EDlAttrErrorId, errorId );
    aDownload.GetIntAttribute( EDlAttrGlobalErrorId, globalErrorId );
    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::GetDownloadErrorCode:: DL error ID: %d",
        errorId );
    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::GetDownloadErrorCode:: DL global error ID: %d",
        globalErrorId );

    switch ( errorId )
        {
        case EConnectionFailed:
            {
            aError = EIptvDlConnectionFailed;
            }
            break;

        case EHttpAuthenticationFailed:
            {
            aError = EIptvDlAuthFailed;
            }
            break;

        case EProxyAuthenticationFailed:
            {
            aError = EIptvDlProxyAuthFailed;
            }
            break;

        case EDestFileInUse:
            {
            aError = EIptvDlDestFileInUse;
            }
            break;

        case EBadUrl:
            {
            aError = EIptvDlBadUrl;
            }
            break;

        case EMMCRemoved:
            {
            aError = EIptvDlMmcRemoved;
            }
            break;

        case EDiskFull:
            {
            aError = EIptvDlDiskFull;
            }
            break;

        case EObjectNotFound:
        case ETransactionFailed:
            {
            aError = EIptvDlContentNotFound;
            }
            break;

        default:
            {
            aError = EIptvDlGeneral;
            }
            break;
        }

    switch ( globalErrorId )
        {
        case EHttpResponsePaymentRequired:
            {
            aError = EIptvDlContentNotFound;
            }
            break;

        case EHttpResponseContinue:
        case EHttpResponseSwitchingProtocols:
            {
            aError = EIptvDlNoError;
            }
            break;

        default:
            // Default should not affect aError, due only special cases
            // are handled here.
            break;
        }
    }

// --------------------------------------------------------------------------
// Timer expired.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::TimerExpired( CIptvTimer* aTimer )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::TimerExpired" );

    if ( aTimer == iDlDeleteTimer )
        {
        delete iDlDeleteTimer;
        iDlDeleteTimer = NULL;
        if ( iDownloadIdValid )
            {
            DeleteCurrentDownload();
            }

        if ( iRestartDownload )
            {
            // Create new download.
            iRestartDownload = EFalse;
            if ( iFileName )
                {
                TRAP_IGNORE( DownloadL() )
                }
        }
        else if ( iState == EThumbnail )
            {
            TRAP_IGNORE( iPlugin.DownloadThumbnailsL() )
            }
        else
            {
            TRAP_IGNORE( iPlugin.DownloadFinishedL( iDlStatus, iDlError ) )
            }

        return;
        }

    //lint -e{961} Else block not needed, default is no operation.
    if ( iState == EPause )
        {
        iPauseCounter++;
        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssDownload::TimerExpired PAUSED %d SECONDS",
            iPauseCounter );

        if ( iPauseCounter > KIptvMaxPauseInSeconds )
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::TimerExpired PAUSED TOO LONG, PLUGIN FAILED, PROBABLY WLAN CONNECTION LOST" );
            iDlStatus = EDownloadFailed;
            iDlError  = EIptvDlConnectionFailed;
            TRAP_IGNORE( DeleteDownloadAsyncL() )
            }
        else
            {
            if ( iTimer && !iTimer->IsActive() )
                {
                iTimer->After( KIptvTime_1_second );
                }
            }
        }
    else if ( iState == EWaiting )
        {
        iPauseCounter++;
        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssDownload::TimerExpired WAITING EVENTS FROM DL-MANAGER %d SECONDS",
            iPauseCounter );

        if ( iPauseCounter > KIptvMaxTimeoutInSeconds )
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::TimerExpired TIMEOUT %d REACHED!",
                KIptvMaxTimeoutInSeconds );
            iDlStatus = EDownloadFailed;
            iDlError  = EIptvDlNoError;
            TRAP_IGNORE( DeleteDownloadAsyncL() )
            }
        else
            {
            if ( iTimer && !iTimer->IsActive() )
                {
                iTimer->After( KIptvTime_1_second );
                }
            }
        }
    else if ( iState == EThumbnail )
        {
        TRAP_IGNORE( DeleteDownloadAsyncL() )
        }
    else if ( iState == ENoDiskSpace )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "RSS Plugin --- CIptvRssDownload::TimerExpired No disk space, deleting download." );
        iDlStatus = EDownloadFailed;
        iDlError  = EIptvDlDiskFull;
        TRAP_IGNORE( DeleteDownloadAsyncL() )
        }
    }

// --------------------------------------------------------------------------
// Finish and delete current download.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::DeleteCurrentDownload()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::DeleteCurrentDownload" );

    ChangeState( EFinished );

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
                iDownloadIdValid = EFalse;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// Change the state of the download.
// --------------------------------------------------------------------------
//
void CIptvRssDownload::ChangeState( TInt aState )
    {
    switch ( iState )
        {
        case ENormal:
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload:: iState changed -> ENormal, was: %d", iState );
            break;
            }

        case EPause:
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload:: iState changed -> EPause, was: %d", iState );
            break;
            }

        case EThumbnail:
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload:: iState changed -> EThumbnail, was: %d", iState );
            break;
            }

        case EWaiting:
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload:: iState changed -> EWaiting, was: %d", iState );
            break;
            }

        case EFinished:
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload:: iState changed -> EFinished, was: %d", iState );
            break;
            }

        case ETimeout:
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload:: iState changed -> ETimeout, was: %d", iState );
            break;
            }

        case ENoDiskSpace:
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssDownload::HandleDMgrEventL iState changed -> ENoDiskSpace, was: %d", iState );
            break;
            }

        default:
            {
            break;
            }
        }

    iState = aState;
    }

// --------------------------------------------------------------------------
// CIptvRssDownload::SkipCurrentDownloadAsyncL
// --------------------------------------------------------------------------
//
void CIptvRssDownload::SkipCurrentDownloadAsyncL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssDownload::SkipCurrentDownloadAsyncL" );

    iDlStatus = EDownloadAlreadyDownloaded;
    iDlError  = EIptvDlNoError;

    DeleteDownloadAsyncL();
    }

// --------------------------------------------------------------------------
// CIptvRssDownload::DeleteDownloadAsyncL
// --------------------------------------------------------------------------
//
void CIptvRssDownload::DeleteDownloadAsyncL()
    {
    delete iDlDeleteTimer;
    iDlDeleteTimer = NULL;
    iDlDeleteTimer = CIptvTimer::NewL( 0, *this );
    iDlDeleteTimer->After( 1 ); //just to make it async
    }

// --------------------------------------------------------------------------
// Getter for ETag
// --------------------------------------------------------------------------
//
void CIptvRssDownload::GetETag( TDes& aETag ) const
    {
    if ( iETag )
        {
        aETag.Copy( iETag->Des() );
        }
    else
        {
        aETag.Copy( KIptvEmptyDes );
        }
    }

// --------------------------------------------------------------------------
// Getter for LastModifiedSince
// --------------------------------------------------------------------------
//
void CIptvRssDownload::GetLastModifiedSince( TTime& aLastUpdated ) const
    {
    aLastUpdated = iLastUpdated;
    }
