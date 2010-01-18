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
* Description:    Implements customised service update*
*/




#ifndef VCXGROUPUPDATER_H_
#define VCXGROUPUPDATER_H_

#include <e32std.h>
#include "MIptvTimerObserver.h"
#include "MIptvEpgVodCallback.h"
#include "CIptvEpgPluginInterface.h"

class CIptvServer;
class CIptvEpgSession;
class CIptvEpgService;
class CIptvEpgVodMsqQueue;
class CIptvTimer;

/**
 *  RSS plugin.
 *
 *  Controls content parsing and download.
 *
 *  @lib IptvRssPlugin.dll
 *  @since S60 v3.1
 */
class CVcxGroupUpdater : public CBase,
                         public MIptvEpgVodCallback,
                         public MIptvTimerObserver
    {

public:

    static CVcxGroupUpdater* NewL(
        CIptvServer& aServer,
        CIptvEpgSession& iSession,
        CIptvEpgVodMsqQueue& aMsgQueue );

    ~CVcxGroupUpdater();

    /**
     * Start download.
     *
     * @param aService Group service.
     */
    void StartGroupUpdateL(
        CIptvEpgService* aService );

    /**
     * Cancel current download download, if any.
     */
    void CancelGroupUpdate();

    /**
     * Enquire is download ongoing.
     * @return ETrue, if download is ongoing.
     */
    TBool IsGroupUpdateOngoing();

// From MIptvEpgVodCallback interface

    /**
    * Plugin started.
    */
    void StartedL();

    /**
     * ECG content update from XML completed. Thumbnails are not downloaded yet.
     */
    void ContentUpdateCompletedL();

    /**
     * Plugin finished.
     *
     * @param aUpdateInterval       Update interval defines how long EPG data is valid.
     */
    TInt FinishedL( TTime& aUpdateInterval );

    /**
     * Add new category to database.
     *
     * @param aCategory             Reference to CIptvEpgCategory object.
     * @param aCategoryKey          New category key value is inserted to aCategoryKey
     * @return                      KErrNone if successful, otherwise another of the system-wide error codes.
     */
    TInt AddCategoryL( CIptvEpgCategory& aCategory, TUint32& aCategoryKey );

    /**
     * Add new content to given category.
     *
     * @param aContent          Reference to CIptvEpgContent object.
     * @param aMediaContents    Pointer array containing media content objects. There can be n media content's for one
     *                          content.
     * @param aContentKey       New content key value is inserted to aContentKey
     * @return                  KErrNone if successful, otherwise another of the system-wide error codes.
     */
    TInt AddContentL(
        CIptvEpgContent& aContent,
        RPointerArray<CIptvMediaContent> aMediaContents,
        TUint32& aContentKey );

    /**
     * Update service thumbnail.
     *
     * @param aIconFilePath     Full path to service thumbnail in local filesystem.
     * @param aGrouped          if true updates also grouped item, otherwise not.
     * @return                  KErrNone if successful, otherwise another of the system-wide error codes.
     */
    TInt UpdateServiceThumbnailL( TDesC& aIconFilePath, const TBool aGrouped );

    /**
     * Add content to category.
     *
     * @param aCategoryKey      Category key.
     * @param aContentKey       Content key.
     */
    TInt AddRelationL( TUint32& aCategoryKey, TUint32& aContentKey );

    /**
     * Error
     *
     * @param aError            Error code.
     * @param aInfo             Further info.
     */
    void ErrorL( TInt aError, TInt aInfo );

    /**
     * Update category icon.
     *
     * @param aCategoryKey      Category key.
     * @param aIconFileId       Icon file id to search from the database.
     * @param aIconFilePath     Full path to icon file or URL + filename.
     * @param aIsUrl            If aIconFilePath is URL aIsUrl == ETrue, if aIconFilePath is path
     *                          to local filesystem aIsUrl == EFalse.
     * @return                  KErrNone if successful, otherwise another of the system-wide error codes.
     */
    TInt UpdateCategoryIconL(
        TUint32& aCategoryKey,
        TUint32& aIconFileId,
        TDesC& aIconFilePath,
        TBool aIsUrl );

    /**
     * Update content icon.
     *
     * @param aCategoryKey      Content key.
     * @param aIconFileId       Icon file id to search from the database.
     * @param aIconFilePath     Full path to icon file or URL + filename.
     * @param aIsUrl            If aIconFilePath is URL aIsUrl == ETrue, if aIconFilePath is path
     *                          to local filesystem aIsUrl == EFalse.
     * @return                  KErrNone if successful, otherwise another of the system-wide error codes.
     */
    TInt UpdateContentIconL(
        TUint32& aContentKey,
        TUint32& aIconFileId,
        TDesC& aIconFilePath,
        TBool aIsUrl );

// From MiptvTimerObserver.

    /**
     * @param aTimer Pointer to timer which expired.
     */
    void TimerExpired( CIptvTimer* aTimer );

private:

    /**
     * C++ default constructor.
     */
    CVcxGroupUpdater(
        CIptvServer& aServer,
        CIptvEpgSession& aSession,
        CIptvEpgVodMsqQueue& aMsgQueue );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Starts parsing of downloaded VCFG file.
     * @param aPathName VCFG file path name.
     */
    void StartVcfgFileParseL( const TDesC& aPathName );

    /**
    * Timer leaving implementation.
    *
    * @param aTimer Pointer to timer which expired.
    */
    void DoTimerExpiredL( CIptvTimer* aTimer );

    /**
     * Reset .vcfg parsed flag.
     *
     * @param aServiceId Id of the service to be checked.
     */
    void SetNotParsedL( TUint32 aServiceId );

    /**
     * Check whether given service has been parsed.
     *
     * @param aServiceId Id of the service to be checked.
     * @return true if parsing completed, otherwise false.
     */
    TBool IsServiceParsed( TUint32 aServiceId );

    /**
     * Set service Last Modified.
     *
     * @param aServiceId Id of the service to be checked.
     * @param aLastModified New last modified time.
     */
    void SetLastModifiedL(
        TUint32 aServiceId,
        TTime aLastModified );

    /**
     * Collect list of required thumbnails and start download.
     */
    void StartThumbnailDownloadL();

private:    // Data members

    /**
     * Server
     */
    CIptvServer& iServer;

    /**
     * calling session.
     */
    CIptvEpgSession& iSession;

    /**
     * Service id of currently ongoing single file downloads.
     */
    TUint32 iCurrentGroupServiceId;

    /**
     * Download plugin.
     * Own
     */
    CIptvEpgPluginInterface* iPlugin;

    /**
     * Informs observers about update state.
     */
    CIptvEpgVodMsqQueue& iMsgQueue;

    /**
    * Common file server session, used by CIptvVodDownload objects too
    */
    RFs iFs;

    /**
     * Indicates download is in progress.
     */
    TBool iPluginRunning;

    /**
     * Current action/type of file to be downloaded.
     */
    CIptvEpgPluginInterface::TIptvRSSAction iCurrentAction;

    /**
    * Server object was not updated, no download was actually needed.
    */
    TBool iLocalValid;

    /**
     * Downloaded path.
     * Own.
     */
    HBufC* iDownloadedVcfgPath;

    /**
     * This is started when there is file sent to provision parsing.
     * Own.
     */
    CIptvTimer* iVcfgParseTimer;

    /**
     * Keep count of vcfg parsing tries.
     */
    TInt iVcfgParseMaxCount;

    /**
     * To be saved to service after successfull dowload and parse.
     */
    TTime iDownloadStartTime;

    };

#endif /* VCXGROUPUPDATER_H_ */
