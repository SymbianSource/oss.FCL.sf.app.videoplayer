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
* Description:    Utility class and constants used by Iptv application.*
*/




#ifndef CIPTVUTIL_H
#define CIPTVUTIL_H

#include <e32std.h>
#include <e32const.h>
#include <e32base.h>
#include <f32file.h>           // RFs
#include <e32property.h>
#include <bldvariant.hrh>      // for __SERIES60_30__ or similar
#include <commdb.h>
#include "IptvDebug.h"
#include "iptvcustommessageids.h"

class TDataType;
class TIptvExternalFolderInfo;

_LIT( KIptvEmptyDes, "" );
_LIT8( KIptvEmptyDes8, "" );

// Max thumbnail size for RSS plugin in bytes
const TInt KIptvRssMaxThumbnailSize                     = 512000;

// KMaxPath - 1 (256 - 1 = 255). This is maximun path length defined in DB.
const TInt KIptvMaxPath                                 = 255;
                                                                            
const TInt KIptvIdMaxLength                             = 255;

// VOD client API constants
const TInt KIptvVodContentMaxPath                       = KIptvMaxPath;
const TInt KIptvVodContentMaxCategoryNameLen            = 100;
const TInt KIptvVodContentMaxCategoryDescriptionLen     = 255;
const TInt KIptvVodContentMaxCategoryBrowserUrlLen      = KIptvMaxPath;
const TInt KIptvVodContentCategoryRootId                = 0;

// Fixed category id for "invalid" category.
const TUint32 KIptvRssInvalidCategoryId                 = KMaxTUint32 - 2;
const TInt KIptvVodContentMaxLanguageLen                = 10;
const TInt KIptvVodContentMaxContentNameLen             = 100;
const TInt KIptvVodContentMaxContentDescriptionLen      = 255;
const TInt KIptvVodContentMaxThumbnailPathLen           = KIptvMaxPath;

// EPG manager and plugin server side constants

// Category
const TInt KIptvEpgMaxCategoryCount                     = 20;
const TInt KIptvEpgMaxContentCountInCategory            = 200;
const TInt KIptvEpgCategoryStartIndex                   = 1;
const TInt KIptvEpgCategoryNameMaxLength                = 100;
const TInt KIptvEpgCategoryIconFilePathMaxLength        = KIptvMaxPath;
const TInt KIptvEpgCategoryDescriptionMaxLength         = 255;
const TInt KIptvEpgCategoryLongDescriptionMaxLength     = 500;

// Content
const TInt KIptvEpgMaxContentCount                      = 500;
const TInt KIptvEpgContentNameMaxLength                 = 100;
const TInt KIptvEpgContentIconFilePathMaxLength         = KIptvMaxPath;
const TInt KIptvEpgContentDescriptionMaxLength          = 255;
const TInt KIptvEpgContentLongDescriptionMaxLength      = 500;
const TInt KIptvEpgContentBrowserUrlMaxLength           = 2048;
const TInt KIptvEpgContentAuthorLength                  = 100;
const TInt KIptvEpgContentCopyrightLength               = 100;
const TUint32 KIptvEpgContentIdNone                     = KMaxTUint32 - 3;

// 'Search'
const TInt KIptvRssSearchMaxSearchStringLength          = 255;
// Fixed category id for search category.
const TUint32 KIptvRssSearchCategoryId                  = KMaxTUint32 - 1;
const TUint32 KIptvRssSearchInitialContentId            = 1000000;

// Content access
const TInt KIptvCAContentUrlLength                      = 2048;
const TInt KIptvCAPurchaseUrlLength                     = 2048;
const TInt KIptvCALanguageLength                        = 10;
const TInt KIptvCAPriceLength                           = 10;
const TInt KIptvCAMimeTypeLength                        = 64;

const TInt KIptvMyVideosNameMaxLength                   = 100;
const TInt KIptvMyVideosVideoMaxMimeTypeLength          = 64;

// MyVideos constants
const TInt  KIptvMyVideosFolderNameMaxLength            = 16;
const TInt  KIptvMyVideosFolderThumbnailPathMaxLength   =
                KIptvVodContentMaxThumbnailPathLen;
const TInt  KIptvMyVideosFolderDescriptionMaxLength     =
                KIptvVodContentMaxCategoryDescriptionLen;
const TInt  KIptvMyVideosFolderLongDescriptionMaxLength = 500;
const TInt  KIptvMyVideosFolderRootId                   =
                KIptvVodContentCategoryRootId;
const TUint KIptvMyVideosFolderInvalidId                = KMaxTUint32;

const TInt KIptvMyVideosVideoNameMaxLength              =
                KIptvVodContentMaxContentNameLen;
const TInt KIptvMyVideosVideoThumbnailPathMaxLength     =
                KIptvVodContentMaxThumbnailPathLen;
const TInt KIptvMyVideosVideoDescriptionMaxLength       =
                KIptvVodContentMaxContentDescriptionLen;
const TInt KIptvMyVideosVideoPathMaxLength              =
                KIptvMaxPath;
const TInt KIptvMyVideosVideoLongDescriptionMaxLength   =
                KIptvEpgContentLongDescriptionMaxLength;

const TInt KIptvMyVideosVideoMaxAudioLanguageLen        = 50;

const TInt KIptvMyVideosVideoMaxAuthorLength            = 255;
const TInt KIptvMyVideosVideoMaxCopyrightLength         = 255;
const TInt KIptvMyVideosVideoMaxFileSourceNameLength    = 128;

// Service manager
// in 16bit words
const TInt KIptvSmServicesDbNameMaxLength               = 80;
const TInt KIptvSmServicesDbAddressMaxLength            = 255;
const TInt KIptvSmServicesDbIconPathMaxLength           = KIptvMaxPath;
// Check which plugin has shortest and use it. Currently using
// KMaxDefAttrLength from HttpDownloadMgrCommon.h
const TInt KIptvSmPasswordsDbUserNameMaxLength          = 32;
// Currently using KMaxDefAttrLength from HttpDownloadMgrCommon.h
const TInt KIptvSmPasswordsDbPasswordMaxLength          = 32;
const TInt KIptvSmServicesDbDescMaxLength               = 200;
const TInt KIptvSmServicesDbTempIdMaxLength             = 127;
const TInt KIptvSmServicesDbMaxIaps                     = 10;
const TInt KIptvSmServicesDbProviderIdMaxLength         =
                KIptvSmServicesDbAddressMaxLength;
const TInt KIptvSmServicesDbAccountManagementUrlMaxLength =
                KIptvSmServicesDbAddressMaxLength;
const TInt KIptvSmServicesDbIapNameMaxLength            = 255;
const TInt KIptvSmServicesDbSearchUrlMaxLength          = 2048;
const TInt KIptvSmServicesDbRecordUrlMaxLength          = 2048;

// in 8-bit bytes
const int KIptvSmServicesDbIapListMaxLength             =
    1 /*TUint8 count*/ +
    KIptvSmServicesDbMaxIaps *
        ( 4 /*TUint32 iId*/ + 2 /*TUint16 priority*/ );

// CIptvEpgService constants
const TInt KIptvEpgServiceAddressMaxLength              =
                KIptvSmServicesDbAddressMaxLength;
const TInt KIptvEpgServiceUserNameMaxLength             =
                KIptvSmPasswordsDbUserNameMaxLength;
const TInt KIptvEpgServicePasswordMaxLength             =
                KIptvSmPasswordsDbPasswordMaxLength;

//Network Selection
const TInt KIptvNsIapNameMaxLength                      = 255;

//vod dl
const TInt KIptvVodDlUserNameMaxLength                  =
                KIptvSmPasswordsDbUserNameMaxLength;
const TInt KIptvVodDlPasswordMaxLength                  =
                KIptvSmPasswordsDbUserNameMaxLength;

// EPG manager server side error codes
const TInt KIptvErrorGetUpdateInformationLNotFound      = 123;
const TInt KIptvErrorNoService                          = 140;

// aInfo parameters for error codes. Notice these are not actual error codes!
const TInt KIptvRssParserError                          = 145;

// EPG manager messages sent to client!
const TInt KIptvErrorVodNoIap                   = 139; // aInfo is 0
const TInt KIptvVodUpdateStarted                = 136; // aInfo is 0
const TInt KIptvContentUpdateCompleted          = 146; // aInfo is 0
const TInt KIptvServiceThumbnailDownloaded      = 143; // aInfo parameter is
                                                       // service id
const TInt KIptvContentThumbnailDownloaded      = 144; // aInfo parameter is
                                                       // content id
const TInt KIptvErrorEpgUpdateFailed            = 133; // KIptvRssParserError
                                                       // or TIptvDlError
                                                       // (defined in this file)
const TInt KIptvErrorEpgUpdateSuccessed         = 134; // aInfo is 0
const TInt KIptvVodUpdateNotStarted             = 137; // aInfo is 0
const TInt KIptvErrorEpgUpdateStopped           = 138; // aInfo is 0

const TInt KIptvErrorRssSearchStarted           = 170;
const TInt KIptvErrorRssSearchFailed            = 171;
const TInt KIptvErrorRssSearchSucceed           = 172;
const TInt KIptvErrorRssSearchNoIap             = 173;
const TInt KIptvErrorRssSearchCancelled         = 174;  // Currently used only internally in VoD UI
const TInt KIptvRssSearchContentUpdateCompleted = 175;
const TInt KIptvRssSearchThumbnailCount         = 176; // aInfo is thumbnail count
const TInt KIptvRssSearchContentThumbnailDownloaded = 177; // aInfo is content id

const TInt KIptvThumbnailCount                  = 141; // aInfo is thumbnail count

// EPG manager messages sent to client (LIVE TV ONLY)
const TInt KIptvErrorFailedToLoadPlugin         = 135;

// Notification plugin
const TInt KIptvInstallService                  = 200;
const TInt KIptvDontInstallService              = 201;
const TInt KIptvAddVodService                   = 202;
const TInt KIptvDontAddVodService               = 203;
const TInt KIptvAddVodcastService               = 204;
const TInt KIptvDontAddVodcastService           = 205;

// Provisioning processor
const TInt KIptvSchemaMismatch                  = 301;
const TInt KIptvInvalidServiceType              = 302;
const TInt KIptvInvalidUrl                      = 303;

_LIT( KIptvEpgDbName, "epgdb." );
const TInt KIptvEpgDbLastModifiedMaxLength      = 64;
const TInt KIptvEpgDbXmlFileHashMaxLength       = 20;
const TInt KIptvServiceIdMaxLength              = 10;

// Time
const TInt64 KIptvMinuteAsMicrosecond           = 60000000;

enum                        /* Price list indexes */
    {
    EIptvPriceStream = 0,   /* Streamed */
    EIptvPriceImmediate,    /* Immediate download */
    EIptvPricePostponed,    /* Postponed download */
    EIptvPriceScheduled,    /* Scheduled download */
    EIptvPriceIdList        /* Amount of defined price indexes */
    };

// CenRep: UIDs
//
const TUid KIptvCenRepUid                            = { 0x102750E2 };
const TUid KIptvAlrCenRepUid                         = { 0x2000B438 };

// CenRep: Keys
//
const TInt KIptvCenRepPreferredMemoryKey             = 0x1;
const TInt KIptvCenRepConnectionsKey                 = 0x2;
const TInt KIptvCenRepCellularKey                    = 0x3;
const TInt KIptvCenRepVoDThumbnailsKey               = 0x4;
const TInt KIptvCenRepActiveServiceIdKey             = 0x5;
const TInt KIptvCenRepVideoPlayerVolumeKey           = 0x6;
const TInt KIptvCenRepMainAppServiceFocusKey         = 0x7;
const TInt KIptvCenRepFirstTimeKey                   = 0x8;
const TInt KIptvCenRepServiceUpdatedKey              = 0x9;
const TInt KIptvCenRepUpdatedServiceIdKey            = 0xA;
const TInt KIptvCenRepAPModeKey                      = 0xB;
const TInt KIptvCenRepParentControlKey               = 0xC;
const TInt KIptvCenRepLastPlaybackPositionFeatureKey = 0xD;

const TInt KIptvCenRepUsedSnapIdKey                  = 0x1;
const TInt KIptvCenRepAllowUseGprsKey                = 0x2;
const TInt KIptvCenRepAllowRoamingKey                = 0x3;
const TInt KIptvCenRepMaxUsedMemoryKey               = 0x4;
const TInt KIptvCenRepIadUpdateCheckTimeKey          = 0xA;

// CenRep: Key values
//
const TInt KIptvCenRepPreferredMemoryKeyPhone        = 0;
const TInt KIptvCenRepPreferredMemoryKeyMmc          = 1;
const TInt KIptvCenRepParentControlKeyOff            = 0;
const TInt KIptvCenRepAPModeAlwaysAsk                = 0;
const TInt KIptvCenRepAPModeUserDefined              = 1;

const TInt KIptvCenRepConnectionKeyValueYes          = 1;
const TInt KIptvCenRepConnectionKeyValueNo           = 0;
const TInt KIptvCenRepSnapAlwaysAsk                  = 0;

// CenRep: Default values
//
const TInt KIptvCenRepPreferredMemoryKeyDefault      = KIptvCenRepPreferredMemoryKeyMmc;
const TInt KIptvCenRepFirstTimeKeyDefault            = 8;
const TInt KIptvCenRepParentControlKeyDefault        = KIptvCenRepParentControlKeyOff;
const TInt KIptvCenRepAPModeKeyDefault               = KIptvCenRepAPModeUserDefined;

// DATA TYPES
typedef TUint32 TIptvServiceId;
typedef TUint32 TIptvGroupId;
typedef TUint32 TIptvFolderId;
typedef TUint32 TIptvCategoryId;
typedef TUint32 TIptvContentId;
typedef TUint32 TIptvFileId;
typedef TUint32 TIptvPlayTime;
typedef TReal32 TIptvPrice;
typedef TUint32 TIptvFileSize;
typedef TUint32 TIptvContentLanguage;

typedef enum            /* Content download choises */
    {
    EIptvDownloadTypeStream              = 0x1,
    EIptvDownloadTypeImmediate           = 0x2,
    EIptvDownloadTypePostponed           = 0x4,
    EIptvDownloadTypeScheduled           = 0x8
    } TIptvDownloadType;

// Publish & subscribe properties offered by Iptv Engine with
// IPTV_SERVER_UID category */
typedef enum
    {
    EIptvVodDownloadStateProperty //used by CIptvVodDlClient
    } TIptvServerProperty;

// Filepaths
typedef enum            /* File path choises */
    {
    EIptvPathWritableRoot       = 0x1,      // Root path
    EIptvPathTmp                = 0x2,      // temp folder
    EIptvPathMyVideosDb         = 0x3,      // My Videos DB root
    EIptvPathEcg                = 0x4,      // Content guide storage root
    EIptvPathService            = 0x5,      // Service thumbnails
    EIptvPathMyVideosVideo      = 0x6,      // My Videos video root
    EIptvPathEcgLive            = 0x7,
    EIptvPathTmpLive            = 0x8,
    EIptvPathRssThumbnail       = 0x9,      // vod rss thumbnail path
    EIptvPathRssSearchThumbnail = 0x10      // search thumbnail path
    } TIptvPathSelection;


typedef enum
    {
    EIptvDlNoError,
    EIptvDlOutOfMemory,

    EIptvDlOutOfMemoryInVodDlPluginCreate,
    EIptvDlVodDlPluginNotFound,
    EIptvDlGeneralErrorInVodDlPluginCreate,

    //errors before the download starts
    EIptvDlMessageAlreadyPending,
    EIptvDlTypeNotSupported,
    EIptvDlMaxDownloadsExceeded,
    EIptvDlAlreadyExists,
    KIptvDlCouldNotGetServiceData, //wrong service id?
    EIptvDlEpgManagerNotReady,
    EIptvDlCouldNotGetContentFullDetails,
    EIptvDlFailedToAddNewVideoToMyVideos,
    EIptvDlCouldNotConnectToS60DlMgr,
    EIptvDlCouldNotSetIapAttrToS60DlMgr,
    EIptvDlCouldNotCreateS60Download,
    EIptvDlCouldNotStartS60Download,

    //errors during & after download
    EIptvDlConnectionFailed,// IAP wrong or no network coverage
    EIptvDlAuthFailed,
    EIptvDlProxyAuthFailed,
    EIptvDlContentNotFound, // HTTP 402
    EIptvDlDiskFull,
    EIptvDlDestFileInUse,
    EIptvDlBadUrl,          // Syntax error in url
    EIptvDlMmcRemoved,
    EIptvDlGeneral,         // Wwrong url, content removed, network does not
                            // work, server side leaved with error, ...
    EIptvDlDisconnected,    // Connection to network lost
    EIptvDlContentUnchanged, // ETag or last modified data hasn't been changed
                            // so no need to download the file cause it's not
                            // changed
    EIptvDlInvalidIap,      // Invalid IAP
    EIptvDlNoErrorUpToDate, // No download needed, current content is valid
    EIptvResumingFromBeginning // Resume not possible, starting from beginning
    } TIptvDlError;

// This value is defined here because of the following explanation:
// When download manager does see an http error code (similar to the codes
// listed in TIptvHttpResponse enumeration below) it re-maps that error code
// with "magical" number -25000. This -25000 is defined in HttpDownload.cpp
// file located in \s60\mw\BrowserUtilities\DownloadMgr\DownloadMgrServEng\Src.
// Since the constant is defined in .cpp file we cannot include it in our code.
// This constant is used in TIptvHttpResponse enumeration to map those values
// to the one that is used by download manager -> If download manager ever
// changes the error base value we only modify this constant to make mapping
// correct again.
const TInt KIptvHttpErrorBase               = -25000;

// Url max length in iptv.
// As far as I know url specification allows url to be as big as 2048, but
// S60 (v3.1/w31) download manager crashes if url length goes somewhere around
// 1200-1400 so we need to limit it here. If someone finds out that
// download manager get fixed please feel free to change value to 2048 
const TInt KIptvUrlMaxLength                = 1024;

/**
* Enumeration containing all the supported http response messages.
* These enumerations can be use by the download operations to check
* necessary operations needed when receiving messages from S60
* download manager.
*/
typedef enum
  {
  EHttpResponseContinue                     = KIptvHttpErrorBase - 100,
  EHttpResponseSwitchingProtocols           = KIptvHttpErrorBase - 101,
  
  EHttpResponseOk                           = KIptvHttpErrorBase - 200,
  EHttpResponseCreated                      = KIptvHttpErrorBase - 201,
  EHttpResponseAccepted                     = KIptvHttpErrorBase - 202,
  EHttpResponseNonAuthoritativeInformation  = KIptvHttpErrorBase - 203,
  EHttpResponseNoContent                    = KIptvHttpErrorBase - 204,
  EHttpResponseResetContent                 = KIptvHttpErrorBase - 205,
  EHttpResponsePartialContent               = KIptvHttpErrorBase - 206,
  
  EHttpResponseMultipleChoices              = KIptvHttpErrorBase - 300,
  EHttpResponseMovedPermanently             = KIptvHttpErrorBase - 301,
  EHttpResponseFound                        = KIptvHttpErrorBase - 302,
  EHttpResponseSeeOther                     = KIptvHttpErrorBase - 303,
  EHttpResponseNotModified                  = KIptvHttpErrorBase - 304,
  EHttpResponseUseProxy                     = KIptvHttpErrorBase - 305,
  EHttpResponseUnused                       = KIptvHttpErrorBase - 306, // Not used by the protocol
  EHttpResponseTemporaryRedirect            = KIptvHttpErrorBase - 307,
  
  EHttpResponseBadRequest                   = KIptvHttpErrorBase - 400,
  EHttpResponseUnautorized                  = KIptvHttpErrorBase - 401,
  EHttpResponsePaymentRequired              = KIptvHttpErrorBase - 402,
  EHttpResponseForbidden                    = KIptvHttpErrorBase - 403,
  EHttpResponseNotFound                     = KIptvHttpErrorBase - 404,
  EHttpResponseMethodNotAllowed             = KIptvHttpErrorBase - 405,
  EHttpResponseNotAcceptable                = KIptvHttpErrorBase - 406,
  EHttpResponseProxyAuthenticationRequired  = KIptvHttpErrorBase - 407,
  EHttpResponseRequestTimeout               = KIptvHttpErrorBase - 408,
  EHttpResponseConflict                     = KIptvHttpErrorBase - 409,
  EHttpResponseGone                         = KIptvHttpErrorBase - 410,
  EHttpResponseLengthRequired               = KIptvHttpErrorBase - 411,
  EHttpResponsePreconditionFailed           = KIptvHttpErrorBase - 412,
  EHttpResponseRequestEntityTooLarge        = KIptvHttpErrorBase - 413,
  EHttpResponseRequestUriTooLong            = KIptvHttpErrorBase - 414,
  EHttpResponseUnsupportedMediaType         = KIptvHttpErrorBase - 415,
  EHttpResponseRequestedRangeNotSatisfiable = KIptvHttpErrorBase - 416,
  EHttpResponseExpectationFailed            = KIptvHttpErrorBase - 417,
  
  EHttpResponseInternalServerError          = KIptvHttpErrorBase - 500,
  EHttpResponseNotImplemented               = KIptvHttpErrorBase - 501,
  EHttpResponseBadGateway                   = KIptvHttpErrorBase - 502,
  EHttpResponseServiceUnavailable           = KIptvHttpErrorBase - 503,
  EHttpResponseGatewayTimeout               = KIptvHttpErrorBase - 504,
  EHttpResponseHttpVersionNotSupported      = KIptvHttpErrorBase - 505
  } TIptvHttpResponse;

/**
 * Vod download states, used in CIptvVodDlDownloadEvent::iState and
 * CIptvVodDownload::iState
 */

/**
 * Created. Event only, download does not stay in this state.
 */
const TUint32 KIptvVodDlDownloadCreated = 0;

/**
 * Not doing anything, does not exist in VOD DL Plugin.
 */
const TUint32 KIptvVodDlDownloadIdle = 1;

/**
 * VOD DL plugin is starting the download.
 */
const TUint32 KIptvVodDlDownloadStarting = 2;

/**
 * VOD DL plugin is downloading, deprecated, use KIptvVodDlDownloadRunning
 * instead.
 */
const TUint32 KIptvVodDlDownloadStarted = 3;

/**
 * VOD DL plugin is downloading.
 */
const TUint32 KIptvVodDlDownloadRunning = 3;

/**
 * Indicates that global file id won't change anymore,
 * ie client can start using it.
 * Event only, download does not stay in this state.
 */
const TUint32 KIptvVodDlDownloadGlobalFileIdIsValid = 4;

/**
 * In queue. Either not created in VOD DL plugin yet or in paused or failed
 * state in VOD DL plugin.
 */
const TUint32 KIptvVodDlDownloadInQueue = 5;

/**
 * VOD DL Manager is pauseing the download temporarily.
 * Paused state is entered when download is paused in VOD DL plugin.
 * This is currently not used, it could be used to pause download
 * temporarily when checking the available disk space for the download.
 */
const TUint32 KIptvVodDlDownloadPausing = 6;

/**
 * Paused, exists in VOD DL Plugin. This state is not published.
 * Idea is to hide short term pauses (less than 10 second) from client.
 */
const TUint32 KIptvVodDlDownloadPaused = 7;

/**
 * VOD DL Manager is pausing the download permanently. It's permanently
 * paused when vod dl plugin sends paused event.
 */
const TUint32 KIptvVodDlDownloadPermanentlyPausing = 8;

/**
 * If KIptvVodDlDownloadPaused state lasts more than
 * KIptvVodDlPermanentPauseDelay this state is entered.
 * This state is published to clients.
 */
const TUint32 KIptvVodDlDownloadPermanentlyPaused = 9;

/**
 * Download finished successfully. Event only, download does not stay in this
 * state.
 */
const TUint32 KIptvVodDlDownloadSucceeded  = 10;

/**
 * Event only, download does not stay in this state.
 */
const TUint32 KIptvVodDlDownloadFailed = 11;

/**
 * After receiving event with this state, no further events will arrive of
 * this download. Download is deleted from VOD DL Manager, file is not
 * deleted from MyVideos.
 * Event only.
 */
const TUint32 KIptvVodDlDownloadDeleted = 12;

/**
 * After receiving event with this state, no further events will arrive of
 * this download.
 * Download is deleted from VOD DL plugin, VOD DL Manager and MyVideos.
 * Event only.
 */
const TUint32 KIptvVodDlDownloadDeletedCompletely = 13;

/**
 * Maximum simultaneus downloads in VOD DL Manager.
 */
const TInt KIptvVodDlMaxDownloads = 20;

/**
 * Maximum simultaneous downloads in KIptvVodDlDownloadRunning state.
 */
const TInt KIptvVodDlMaxActiveDownloads = 3;

/**
* Maximum length of text field in db.
*/
const TInt KIptvUtilMaxTextFieldLengthInDb = 255;

_LIT(KIptvUtilLanService, "LANService");
_LIT(KIptvUtilOutGoingGprs, "OutgoingGPRS");
_LIT(KIptvUtilDialOutISP, "DialOutISP");
class RWriteStream;
class RReadStream;

/**
 *  Utility class used by Iptv application.
 *
 *  @lib IptvClientApi.lib
 *  @since Engine 1.0
 */
class CIptvUtil : public CBase
    {

public:

    /**
     * To limit changes in current functionality, the implementation
     * of MIME type check is different for different components.
     */
    enum TIptvMimeTypeSource
        {
        EIptvMimeTypeSourcePlayer = 1,
        EIptvMimeTypeSourceRssPlugin
        };

    /**
     *
     */
    enum TConnectionType
        {
        EUnknown   = (1 << 0),
        EWlan      = (1 << 1), //table = IAP_TABLE col = IAP_SERVICE_TYPE value = _L("LANService")) and found from wlan table
        EGprs      = (1 << 2), //table = IAP_TABLE col = IAP_SERVICE_TYPE value = _L("OutgoingGPRS")
        ECsd       = (1 << 3), //table = IAP_TABLE col = IAP_SERVICE_TYPE value = _L("DialOutISP")
        };

public:

    /**
     * Prints long strings (RDebug::Print does not work correctly).
     *
     * @param aData String to print.
     */
    IMPORT_C static void LongPrintL( TDesC& aData );

    /**
     * Converts TDesC& descriptor to TUint32 value.
     *
     * @param value String to convert.
     * @return TUint32 value.
     */
    IMPORT_C static TUint32 StringToInt( TDesC& value );

    /**
     * Returns Iptv writable path. If last parameter is ETrue, also checks
     * the path for existence.
     *
     * @param aFs Reference to File Server session.
     * @param aSelection Indentifies which path is requested.
     * @param aPath On return, the requested path.
     * @param aDriveLetter Drive on which the path should point. Default EDriveC.
     * @param aCreatePath If ETrue, creates the path. Default ETrue.
     */
    IMPORT_C static void GetPathL(
        RFs& aFs,
        TIptvPathSelection aSelection,
        TDes& aPath,
        TDriveNumber aDriveNumber = EDriveC,
        TBool aCreatePath = ETrue );

    /**
     * Returns Iptv path. If last parameter is ETrue, also checks the path for
     * existence. Currently this method servers requests only for following 
     * TIptvPathSelections:
     *  - EIptvPathEcgLive
     *  - EIptvPathTmpLive
     *  - EIptvPathRssThumbnail
     *  - EIptvPathRssSearchThumbnail
     * Functionality can be extended if there are other situations where
     * service id needs to be part of the returned path.
     *
     * @param aFs Reference to file server session
     * @param aSelection Identifies which path is requested
     * @param aServiceId Added to the path to make allow unique directories to be
     *                   made (i.e. \videocenter\ecg\<aServiceId>\ )
     * @param aPath On return contains the requested path
     * @param aCreatePath If ETrue, creates the path. Default ETrue.
     */
    IMPORT_C static void GetPathL(
        RFs& aFs, 
        TIptvPathSelection aSelection,
        TUint32 aServiceId,
        TDes& aPath,
        TBool aCreatePath = ETrue );

    /**
     * Read a string from a stream.
     *
     * @param  aDes        Data to be read.
     * @param  aStream     A stream where data is read from.
     */
    IMPORT_C static void ReadDesFromStreamL(
        TDes& aDes,
        RReadStream& aStream );

    /**
     * Write a string to a stream.
     *
     * @param  aDes        Data to be written.
     * @param  aStream     A stream where data is written to.
     */
    IMPORT_C static void WriteDesToStreamL(
        const TDesC& aDes,
        RWriteStream& aStream );

    /**
     * Read a string from a stream.
     *
     * @param  aDes        Data to be read.
     * @param  aStream     A stream where data is read from.
     */
    IMPORT_C static void ReadDes8FromStreamL(
        TDes8& aDes,
        RReadStream& aStream );

    /**
     * Write a string to a stream.
     *
     * @param  aDes        Data to be written.
     * @param  aStream     A stream where data is written to.
     */
    IMPORT_C static void WriteDes8ToStreamL(
        const TDesC8& aDes,
        RWriteStream& aStream );

    /**
     * Read TUint64 from a stream.
     *
     * @param  aData   Data to read.
     * @param  aStream A stream where data is read from.
     */
    IMPORT_C static void ReadUint64FromStreamL(TUint64& aData,
        RReadStream& aStream);

    /**
     * Write TUint64 to a stream.
     *
     * @param  aData   Data to be written.
     * @param  aStream A stream where data is written to.
     */
    IMPORT_C static void WriteUint64ToStreamL(const TUint64 aData,
        RWriteStream& aStream);

    /**
     * Reads HBufC from stream. Ownership moves to caller,
     * ie caller is responsible for freeing the returned pointer.
     * Use WriteHBufCToStreamL for writing.
     *
     * @param  aStream A stream where data is read from.
     * @return The read data.
     */
    IMPORT_C static HBufC* ReadHBufCFromStreamL( RReadStream& aStream );

    /**
     * Writes HBufC to a stream. Ownership does not move.
     * Maximum length of the data is 0xFFFFFFFF. Use ReadHBufCFromStreamL
     * to read data from stream.
     *
     * @param  aData   Data to be written.
     * @param  aStream A stream where data is written to.
     */    
    IMPORT_C static void WriteHBufCToStreamL( HBufC* aData, RWriteStream& aStream );

    /**
     * Return language code corresponding given string.
     *
     * @param  aCountryCode Language string.
     * @return Corresponding code.
     */
    IMPORT_C static TLanguage GetLanguageByCountryCode(
        const TDesC& aCountryCode );

    /**
     * Runs recognizers on the given file to determine its mime type
     *
     * @param aFsSession File server session to be used to read the file
     * @param aFilePath Full path to the file to be recognized
     * @param aDataType On return, the mime type of the file
     * @return System wide error code as returned from RFs, RFile or
     *         RApaLsSession
     */
    IMPORT_C static TInt RecognizeFile(
        RFs& aFsSession,
        const TDesC& aFilePath,
        TDataType& aDataType );

    /**
     * Runs recognizers on the given file to determine its mime type.
     *
     * Note! The file handle must be sharable or the recognition will fail
     * with KErrBadHandle. This function is meant for embedding cases where
     * the file resides inside another processes private folder and it can
     * only be accessed through a shared file handle.
     * A file handle is sharable if the file server session that opened the
     * file is made sharable by calling ShareProtected() before opening the
     * file. A dedicate file server session should be used for this purpose.
     *
     * @param aFileHandle RFile handle to the file to be recognized
     * @param aDataType On return, the mime type of the file
     * @return System wide error code as returned from RFs, RFile or
     *         RApaLsSession
     */
    IMPORT_C static TInt RecognizeFile(
        const RFile& aFileHandle,
        TDataType& aDataType );

    /**
    * Gets IAP connection type.
    *
    * @param aIapId IAP ID which connection type is queryed.
    * @return       Connection type
    */
    IMPORT_C static TConnectionType ConnectionTypeL( TUint32 aIapId );

    /**
    * Gets IAP connection type, service ID and WLAN hidden status.
    *
    * @param aIapId     IAP ID which connection type is queryed.
    * @param aIsHidden  WLAN hidden status, this is valid only if type is WLAN.
    * @return           Connection type
    */
    IMPORT_C static TConnectionType ConnectionTypeL(
        TUint32 aIapId,
        TBool& aIsHiddenIap );
        
    /**
    * Checks if aIapId is a WLAN IAP.
    * @param aIapId
    * @return       ETrue if WLAN IAP, EFalse otherwise.
    */
    IMPORT_C static TBool IsWlanIapL( TUint32 aIapId );

    /**
    * Checks if aIapId is WLAN IAP or hidden WLAN IAP.
    * @param aIapId
    * @param aIsHiddenWlanIap ETrue if aIapId was hidden IAP.
    * @return                 ETrue if aIapId was wlan IAP.
    */
    IMPORT_C static TBool IsWlanIapL( TUint32 aIapId, TBool& aIsHiddenWlanIap );

    /**
    * Reads list of allowed access points from commsdb. Hidden access points are 
    * not allowed so they are not included on this list. Anyone selecting access
    * points by reading 'IAP' table from commsdb should use this list to check
    * that the access point is on allowed list.
    *
    * @param aAllowedIaps On return, and if no leaves happened, a list of allowed
    *                     access points.
    */
    IMPORT_C static void GetAllowedIapsListL( RArray<TUint32>& aAllowedIaps );
    
    /**
    * Checks if given access point is on list of allowed access points.
    *
    * @param aIapId ID of access point to check.
    * @param aAllowedIaps List of allowed access points.
    * @return ETrue if access point is allowed to use.
    */
    IMPORT_C static TBool IsAllowedIapL( TUint32 aIapId, RArray<TUint32>& aAllowedIaps );

    /**
    * Ensures that given path exists and hides the lowest level sub-folder in the
    * path if third parameter is ETrue (default). This method should be used to ensure
    * Iptv private data paths, as they should be hidden so that harversters don't find
    * them.
    *
    * @param aFsSession File Server session.
    * @param aPath Path.
    * @param aHide If ETrue, path is hidden.
    */
    IMPORT_C static void EnsureAndHidePathL( RFs& aFsSession,
                                             const TDesC& aPath,
                                             TBool aHide = ETrue );
											 
    /**
    * This method can be used to check whether last playback position feature 
    * is supported. Fetches value from Cenrep.
    *
    * @return ETrue if last playback position feature is supported, otherwise EFalse. 
    */
    IMPORT_C static TBool LastPlaybackPositionFeatureSupported();      											 

protected:

    /**
     * Removes trailing backslashes from a HBufC string.
     *
     * @param aString String to modify.
     */
    static void RemoveTrailingBackSlashesL( HBufC* & aString );
    };

#endif // CIPTVUTIL_H
