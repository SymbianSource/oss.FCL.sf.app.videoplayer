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




#ifndef CIPTVRSSDOWNLOAD_H
#define CIPTVRSSDOWNLOAD_H

#include <e32std.h>
#include <downloadmgrclient.h>
#include "CIptvUtil.h"
#include "MIptvTimerObserver.h"

class CIptvEpgService;
class CIptvRssPlugin;
class CIptvTimer;
class CIptvXmlContentHandler;

/**
 *  Download client.
 *
 *  Controls feed and thumbnail download.
 *
 *  @lib IptvRssPlugin.dll
 *  @since S60 v3.1
 */
class CIptvRssDownload : public CBase,
                         public MHttpDownloadMgrObserver,
                         public MIptvTimerObserver
    {

public:

    static CIptvRssDownload* NewL(
        CIptvRssPlugin& aPlugin,
        RFs& aFs,
        CIptvXmlContentHandler& aXmlContentHandler );

    ~CIptvRssDownload();

public:

    /**
     * Initialize plugin.
     */
    void InitializeL();

    /**
     * Disconnect download manager session.
     */
    void Disconnect();

    /**
     * When either aETag or aLastUpdated is given, downloads only the headers
     * from the server. If ETag or aModifiedSince do not match to the headers
     * from the server the download is finished with
     * TDownloadStatus::EDownloadNeeded and download will be restarted for
     * actual content. Otherwise download is completed with
     * TDownloadStatus::EDownloadNoNeed.
     * When neither is given, actual download is performed.
     *
     * @param aUri          Download Url.
     * @param aFileName     File to be downloaded
     * @param aThumbnail    Enable thumbnail image download.
     * @param aETag         Previous ETag
     * @param aLastUpdated  Previous update time
     */
    void DownloadL(
        const TDesC8& aUri,
        const TDesC& aFileName,
        TBool aThumbnail,
        const TDesC& aETag = KIptvEmptyDes,
        const TTime& aLastUpdated = TTime( 0LL ) );

    /**
     * From MHttpDownloadMgrObserver
     *
     * @param aDownload     Handle to download.
     * @param aEvent        Download event.
     */
    void HandleDMgrEventL(
        RHttpDownload& aDownload,
        THttpDownloadEvent aEvent );

    /**
     * Set service information.
     *
     * @param aService      Epg service implementation.
     */
    void SetServiceInformation( CIptvEpgService* aService );

    /**
     * Set failed.
     */
    void SetFailed();

    /**
     * No need for current download.
     */
    void SkipCurrentDownloadAsyncL();

    /**
     * Getter for ETag
     */
    void GetETag( TDes& aETag ) const;

    /**
     * Getter for LastModifiedSince
     */
    void GetLastModifiedSince( TTime& aLastUpdated ) const;

// From MIptvTimerObserver

    /**
     * Timer expired.
     *
     * @param aTimer        Not used here.
     */
    void TimerExpired( CIptvTimer* aTimer );

private:

    CIptvRssDownload(
        CIptvRssPlugin& aPlugin,
        RFs& aFs,
        CIptvXmlContentHandler& aXmlContentHandler );

    void ConstructL();

    /**
     * Restart previous download.
     */
    void DownloadL();

    /**
     * Set authentication information.
     *
     * @param aDownload     Handle to download.
     */
    void SetAuthenticationInformationL(
        RHttpDownload& aDownload ) const;

    /**
     * Get download error code.
     *
     * @param aDownload     Handle to download.
     * @param aError        Download error code
     */
    void GetDownloadErrorCode(
        RHttpDownload& aDownload,
        TIptvDlError& aError ) const;

    /**
     * Delete current download asynchronously.
     */
    void DeleteDownloadAsyncL();

    /**
     * Finish and delete current download.
     */
    void DeleteCurrentDownload();

    /**
     * Change the state of the download.
     */
    void ChangeState( TInt aState );

private: // Data members

    /**
     * Download progress state.
     */
    enum TState
        {
        ENormal = 0,
        EPause,
        EThumbnail,
        EWaiting,
        EFinished,
        ETimeout,
        ENoDiskSpace
        };

    /**
     * Download manager.
     */
    RHttpDownloadMgr iDownloadManager;

    /**
     * Epg service implementation.
     * Not own.
     */
    CIptvEpgService* iService;

    /**
     * Pluging having started us.
     */
    CIptvRssPlugin& iPlugin;

    /**
     * Service username.
     */
    TBuf8<KIptvEpgServiceUserNameMaxLength> iUserName;

    /**
     * Service password.
     */
    TBuf8<KIptvEpgServicePasswordMaxLength> iPassword;

    /**
     * Thumbnail image download allowed.
     */
    TBool iThumbnail;

    /**
     * Identifies download.
     */
    TInt32 iDownloadId;

    /**
     * DownloadId above contains valid and existing value.
     */
    TBool iDownloadIdValid;

    /**
     * Pointer to CIptvTimer.
     * Own.
     */
    CIptvTimer* iTimer;

    /**
     * Set when download manager is successfully connected.
     */
    TBool iConnected;

    /**
     * Download progress state.
     */
    TInt iState;

    /**
     * Download expires after no progress.
     */
    TInt iPauseCounter;

    /**
     * File server session.
     */
    RFs& iFs;

    /**
     * XML-parser callback class.
     * Not own.
     */
    CIptvXmlContentHandler& iXmlContentHandler;

    /**
     * Pointer to CIptvTimer. Deletes download.
     * Own.
     */
    CIptvTimer* iDlDeleteTimer;

    /**
     * Download error code is stored here to keep it in memory
     * while iDlDeleteTimer is running.
     */
    TIptvDlError iDlError;

    /**
     * Download status code is stored here to keep it in memory
     * while iDlDeleteTimer is running. See TDownloadStatus from CIptvRssPlugin.h
     * for values.
     */
    TInt iDlStatus;

    /**
     * ETag
     */
    HBufC* iETag;

    /**
     * Last update of this download.
     */
    TTime iLastUpdated;

    /**
     * Header info is requested and we are waiting for it.
     */
    TBool iWaitingContentTypeCheck;

    /**
     * Header was not available, dont do further header check.
     */
    TBool iDisableLastModifiedCheck;

    /**
     * Header successfully downloaded and actual download is needed.
     */
    TBool iRestartDownload;

    /**
     * File name for dowload retained for restart.
     * Own.
     */
    HBufC* iFileName;

    /**
     * Uri for dowload retained for restart.
     */
    HBufC8* iUri;

    };

#endif // CIPTVRSSDOWNLOAD_H
