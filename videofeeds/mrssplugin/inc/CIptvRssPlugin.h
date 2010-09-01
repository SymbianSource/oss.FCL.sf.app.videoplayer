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
* Description:    RSS plugin.*
*/




#ifndef CIPTVRSSPLUGIN_H
#define CIPTVRSSPLUGIN_H

#include <e32std.h>
#include <xml/parser.h>
#include "MIptvTimerObserver.h"
#include "CIptvDownloadItem.h"
#include "CIptvEpgPluginInterface.h"

class CIptvXmlContentHandler;
class MIptvEpgVodCallback;
class MIptvEpgLiveTvCallback;
class CIptvEpgService;
class CIptvRssDownload;
class CIptvTimer;
class CMD5;

using namespace Xml;

enum TDownloadStatus
    {
    EDownloadFailed = 0,
    EDownloadSucceeded,
    EDownloadNoNeed,
    EDownloadAlreadyDownloaded,
    EDownloadNeeded,
    };

/**
 *  RSS plugin.
 *
 *  Controls content parsing and download.
 *
 *  @lib IptvRssPlugin.dll
 *  @since S60 v3.1
 */
class CIptvRssPlugin : public CIptvEpgPluginInterface,
                       public MIptvTimerObserver
    {

public:

    static CIptvRssPlugin* NewL();

    ~CIptvRssPlugin();

    /**
     * Handles download finished.
     *
     * @param aState        New download state.
     * @param aError        Download status.
     */
    void DownloadFinishedL( TInt aState, TIptvDlError aError );

    /**
     * Static entry point for CIdle callback.
     *
     * @param aPluginImpl   Own class implementation for callback.
     * @return              Continue parsing.
     */
    static TInt ParseIdleCallback( TAny* aPluginImpl );

    /**
     * Handle idle callback functionality.
     *
     * @return              Continue parsing.
     */
    TInt HandleParseIdleCallback();

    /**
     * Add thumbnail to download queue.
     *
     * @param aItem         Item to be added.
     */
    void AddToDlQueueL( const CIptvDownloadItem* aItem );

    /**
     * Set parsing failed.
     */
    void SetParsingFailed();

    /**
     * Download thumbnails.
     */
    void DownloadThumbnailsL();

    /**
     * Content update completed.
     */
    void ContentUpdateCompletedL();

// From CIptvEpgPluginInterface interface

    /**
     * @see CIptvEpgPluginInterface
     */
    void RunPlugin();

    /**
     * Set callback pointer to MIptvEpgVodCallback. Plugin uses
     * MIptvEpgVodCallback interface to supply Epg data to system.
     *
     * @param aVodCallback Pointer to callback interface.
     */
    void SetVodCallback( MIptvEpgVodCallback* aVodCallback );

    /**
     * Set callback pointer to MIptvEpgLiveTvCallback.
     *
     * @param aLiveTvCallback Pointer to callback interface.
     */
    void SetLiveTvCallback( MIptvEpgLiveTvCallback* aLiveTvCallback );

    /**
     * Checks whether plugin is performing normal update or search operation.
     *
     * @return ETrue if plugin is performing search.
     */
    TBool IsSearchOperation() const { return ( EIptvRssSearch == iAction ); }

    /**
     * @see CIptvEpgPluginInterface
     */
    void SetServiceInformation(CIptvEpgService* aService, TBool aUseSearchAddress );

    /**
     * @see CIptvEpgPluginInterface
     */
    void SetServiceInformationL( CIptvEpgService* aService, TIptvRSSAction aAction );

    /**
     * Set first update.
     *
     * @param aFirstUpdate ETrue if plugin in running first time
     *                     after EPG database is created.
     */
    void SetFirstUpdate( TBool aFirstUpdate );

    /**
     * Remove thumbnails not referenced during download.
     */
    void RemoveObsoleteIconsL();

    /**
     * Thumbnail has been referenced during download.
     * It is either already valid without need to be re-downloaded, or it is
     * obsolete and will be deleted prior download.
     *
     * @param aIconName Thumbnail to be removed from list.
     * @param aDelete   Delete thumbnail file.
     */
    void RemoveIconFromList(
        const HBufC* aIconName,
        TBool aDelete );

// From MIptvTimerObserver

    /**
     * Delayed plugin start.
     *
     * @param aTimer        Not used.
     */
    void TimerExpired( CIptvTimer* aTimer );

private:

    /**
     * C++ default constructor.
     */
    CIptvRssPlugin();

    /**
     * By default Symbian 2nd phase constructor is private.
     * @param
     */
    void ConstructL();

    /**
     * Delayed plugin start.
     */
    void DoTimerExpiredL();

    /**
     * Start plugin.
     */
    void RunPluginL();

    /**
     * Set plugin failed.
     *
     * @param aError        Download status.
     */
    void PluginFailedL( TIptvDlError aError );

    /**
     * Set plugin succeeded.
     */
    void PluginSucceededL();

    /**
     * Parse EPG data.
     */
    void ParseL();

    /**
     * Parse EPG data.
     *
     * @param aFileName     Downloaded XML file.
     */
    void StartParsingXmlFileL( const TDesC& aFileName );

    /**
     * Check is thumbnail download allowed.
     */
    TBool ThumbnailDlAllowedL() const;

    /**
     * Build hashed pathname for download object.
     * Name is unique with adequate propability.
     * @param aUrl Download url for hash basis.
     * @param aPathSelection Select path according to type.
     * @param aDefaultExtension Extension for use when url doesnt have any.
     * @param aPathName Generated name
     */
    void BuildUniqueuPathNameL(
        const TDesC& aUrl,
        const TIptvPathSelection aPathSelection,
        const TDesC& aDefaultExtension,
        const TBool aGenerateName,
        TPtr aPathName );

    /**
     * Save successfully saved path to keep track of already downloaded
     * thumbnails to avoid to redownload.
     */
    void SetAlreadyDownloadedPathL();

    /**
     * Keep track of already downloaded thumbnails to avoid to redownload.
     *
     * @return Pathname of thumbnail when already downloaded, otherwise NULL.
     */
    HBufC* GetAlreadyDownloadedPath();

    /**
     * Build list of item thumbnails associated to the current service.
     */
    void ListCurrentIconsL();

private:    // Data members

    /**
     * XML-parser callback class.
     */
    CIptvXmlContentHandler* iXmlContentHandler;

    /**
     *   XML reader to be used to parse XML file.
     */
    CParser* iParser;

    /**
     * Idle.
     */
    CIdle* iParserIdle;

    /**
     * Download manager.
     */
    CIptvRssDownload* iDownload;

    /**
     * Pointer to MIptvEpgVodCallback interface.
     */
    MIptvEpgVodCallback* iVodCallback;

    /**
     * Service address.
     */
    TBuf8<KIptvCAContentUrlLength> iServiceAddress;

    /**
     * File server session
     */
    RFs iFs;

    /**
     * File content.
     */
    HBufC8* iXmlFileContent;

    /**
     * Current parse position.
     */
    TInt iCurrentParsePosition;

    /**
     * Pointer to CIptvTimer. Runs plugin after timer has expired.
     */
    CIptvTimer* iStartTimer;

    /**
     * RSS temp file.
     */
    TBuf<KIptvMaxPath> iRssPath;

    /**
     * Service id.
     */
    TUint32 iServiceId;

    /**
     * Thumbnail download queue.
     */
    RPointerArray<CIptvDownloadItem> iDlQueue;

    /**
     * Current thumbnail downloading.
     */
    CIptvDownloadItem* iItem;

    /**
     * Download path.
     */
    TBuf<KIptvMaxPath> iDownloadPath;

    /**
     * Download thumbnail.
     */
    TBool iDlThumbnail;

    /**
     * Is thumbnail download allowed.
     */
    TBool iThumbnailDlAllowed;

    /**
     * Parsing failed.
     */
    TBool iParsingFailed;

    /**
     * Defines the action RSS plugin is used for.
     */
    TIptvRSSAction iAction;

    /**
     * Defines if RSS plugin is used for search purposes
     */
    TBool iIsSearchOperation;

    /**
     * Only one file will be downloaded without parse.
     */
    TBool iIsGroupOperation;

    /**
     * MD5 message digest class.
     * Required to generate identificable thumbnail names.
     * Own.
     */
    CMD5* iMD5Calculator;

    /**
     * Last update time of this service.
     */
    TTime iLastUpdated;

    /**
     * List of old thumbnails.
     * When either thumbnail is replaced by a new one or found still valid,
     * It will be removed from this list.
     * At the end of downloads, list contains obsolete thumbnails, i.e.
     * those not referenced from any of the still existing items.
     * Own.
     */
    RPointerArray<HBufC> iIconList;

    /**
     * List of already downloaded thumbnail URL addresses.
     * Items in this list correspond items in iPreviouslyDownloadedPaths.
     * Handling of these two list must be done parallel.
     * Own.
     */
    RPointerArray<HBufC8> iPreviouslyDownloadedAddresses;

    /**
     * List of already downloaded local download paths.
     * Items in this list correspond items in iPreviouslyDownloadedAddresses.
     * Handling of these two list must be done parallel.
     * Own.
     */
    RPointerArray<HBufC> iPreviouslyDownloadedPaths;

    };

#endif  // CIPTVRSSPLUGIN_H
