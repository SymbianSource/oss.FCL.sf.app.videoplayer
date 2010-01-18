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




#include <e32base.h>
#include <sysutil.h>
#include <centralrepository.h>
#include <bautils.h>
#include <xml/parser.h>

#include "IptvDebug.h"
#include "MIptvEpgVodCallback.h"
#include "MIptvEpgLiveTvCallback.h"
#include "CIptvTimer.h"
#include <hash.h>

#include "CIptvEpgService.h"
#include "iptvlastwatcheddata.h"
#include "iptvlastwatchedapi.h"

#include "CIptvRssPlugin.h"
#include "CIptvRssDownload.h"
#include "CIptv3XmlContentHandler.h"

_LIT8( KMimeType, "text/xml" );

_LIT( KIptvRssFileName, "rss." );
_LIT( KIptvThumbnailExtension, ".jpg" );
_LIT( KIptvVCFGExtension, ".vcfg" );
_LIT( KIptvRssSearchFileName, "rsssearch." );
_LIT( KIptvAllFiles, "*.*" );
_LIT8( KIptvHexByteFormatStr, "%02x" );

const TInt64 KIptvRssUpdateSuccesInterval( 60 );
const TInt64 KIptvRssUpdateFailedInterval( 0 );
const TInt KIptvExtensionMaxSize( 5 );

const TInt KIptvRssPluginFreeSpace( 512000 ); // 500 KB

static const TInt KReadAtOnceLength( 1000 );

const TInt KIptvTime_200_microsecond( 200 );

const TInt KIptvHexCharInByte( 2 );
/**
* Enable this flag to update feed from local XML file. The path
* to XML file is defined with KIptvLocalTestFeed variable below.
*/
//#define IPTV_RSS_LOCAL_TEST

#ifdef IPTV_RSS_LOCAL_TEST
_LIT( KIptvLocalTestFeed, "C:\\videofeed.xml" );
#warning Local feed test flag IPTV_RSS_LOCAL_TEST has been defined!!!
#endif

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "RSS Plugin --- CIptvRssPlugin::ConstructL" );

    User::LeaveIfError( iFs.Connect() );
    iDownload = CIptvRssDownload::NewL( *this, iFs, *iXmlContentHandler );
    iThumbnailDlAllowed = ThumbnailDlAllowedL();

    if ( iThumbnailDlAllowed )
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "RSS Plugin --- CIptvRssPlugin::ConstructL --- Thumbnail downloading is allowed!" );
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "RSS Plugin --- CIptvRssPlugin::ConstructL --- Thumbnail downloading is not allowed!" );
        }

    iParserIdle = CIdle::NewL( CActive::EPriorityIdle );
    iXmlContentHandler = CIptvXmlContentHandler::NewL( *this );
    iParser = CParser::NewL( KMimeType, *iXmlContentHandler );
    }

// --------------------------------------------------------------------------
// Two-phased constructor.
// Create instance of concrete ECOM interface implementation
// --------------------------------------------------------------------------
//
CIptvRssPlugin* CIptvRssPlugin::NewL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "RSS Plugin --- CIptvRssPlugin::NewL" );

    CIptvRssPlugin* self = new( ELeave ) CIptvRssPlugin();
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CIptvRssPlugin::~CIptvRssPlugin()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::~CIptvRssPlugin" );

    delete iItem;
    delete iParser;
    delete iDownload;
    delete iParserIdle;
    delete iXmlContentHandler;
    delete iXmlFileContent;
    delete iStartTimer;
    delete iMD5Calculator;
    iDlQueue.ResetAndDestroy();
    iIconList.ResetAndDestroy();
    iPreviouslyDownloadedAddresses.ResetAndDestroy();
    iPreviouslyDownloadedPaths.ResetAndDestroy();

    iFs.Close();
    iVodCallback = NULL;
    }

// --------------------------------------------------------------------------
// C++ default constructor
// --------------------------------------------------------------------------
//
CIptvRssPlugin::CIptvRssPlugin() :
    iXmlContentHandler( NULL ),
    iParser( NULL ),
    iParserIdle( NULL ),
    iDownload( NULL ),
    iVodCallback( NULL ),
    iXmlFileContent( NULL ),
    iStartTimer( NULL ),
    iServiceId( 0 ),
    iItem( NULL ),
    iDlThumbnail( EFalse ),
    iThumbnailDlAllowed( EFalse ),
    iParsingFailed( EFalse )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::CIptvRssPlugin" );
    }

// --------------------------------------------------------------------------
// From CIptvEpgPluginInterface
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::RunPlugin()
    {
    IPTVLOGSTRING_LOW_LEVEL( "RSS Plugin --- CIptvRssPlugin::RunPlugin" );

    // Timer removes UI jamming in update sequence.
    if ( !iStartTimer )
        {
        TRAP_IGNORE( iStartTimer =
            CIptvTimer::NewL( CActive::EPriorityUserInput, *this ) );
        }

    if ( iStartTimer )
        {
        if ( !iStartTimer->IsActive() )
            {
            iStartTimer->After( KIptvTime_200_microsecond );
            }
        }
    }

// --------------------------------------------------------------------------
// Delayed plugin start.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::TimerExpired( CIptvTimer* /* aTimer */ )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CIptvRssPlugin --- CIptvRssPlugin::TimerExpired" );

    TRAPD( error, DoTimerExpiredL() );
    if ( error != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssPlugin::TimerExpired --- Leave while DoTimerExpiredL() = %d",
            error );
        TRAP( error, PluginFailedL( EIptvDlNoError ) );
        if ( error != KErrNone )
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssPlugin::TimerExpired --- Leave while PluginFailedL() = %d",
                error );
            }
        }
    }

// --------------------------------------------------------------------------
// Delayed plugin start.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::DoTimerExpiredL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::DoTimerExpiredL" );

    iDownload->InitializeL();
    RunPluginL();
    }

// --------------------------------------------------------------------------
//  Start plugin.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::RunPluginL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "RSS Plugin --- CIptvRssPlugin::RunPluginL" );

    if ( ! iIsGroupOperation )
        {
        // If exist, delete old rss file
        if ( BaflUtils::FileExists( iFs, iRssPath ) )
            {
            BaflUtils::DeleteFile( iFs, iRssPath );
            }
        }

    // Clear obsolete thumbnail list.
    iIconList.ResetAndDestroy();

    // Clear already downloaded thumbnail list.
    iPreviouslyDownloadedAddresses.ResetAndDestroy();
    iPreviouslyDownloadedPaths.ResetAndDestroy();

    // Before we even start the download, we check that there is sensible
    // amount of free disk space for content guide and thumbnail files.
    TBool checkResult( EFalse );
    TRAPD(
        checkError,
        checkResult = SysUtil::DiskSpaceBelowCriticalLevelL(
            &iFs, KIptvRssPluginFreeSpace, EDriveC ) );
    if ( checkError != KErrNone || checkResult )
        {
        // Not enough space on disk, return immediately without any downloading.
        if ( iIsSearchOperation )
            {
            iVodCallback->ErrorL( KIptvErrorRssSearchFailed, EIptvDlDiskFull );
            }
        else
            {
            iVodCallback->ErrorL( KIptvErrorEpgUpdateFailed, EIptvDlDiskFull );
            }
        return;
        }

    // Callback EPG manager that plugin is started
    if ( iVodCallback )
        {
        iVodCallback->StartedL();
        }
    else
        {
        IPTVLOGSTRING_LOW_LEVEL(
            "RSS Plugin --- CIptvRssPlugin::RunPluginL --- iVodCallback is NULL" );
        }

#ifndef IPTV_RSS_LOCAL_TEST

    if ( ( EIptvRssAddServiceThumbnailToDlQueue == iAction ) ||
         ( EIptvRssAddItemThumbnailToDlQueue == iAction ) )
        {
        // Download only previously listed thumbnails.
        iParsingFailed = EFalse;
        DownloadThumbnailsL();
        }
    else
        {
        // Download RSS-file.
        iDownload->DownloadL(
            iServiceAddress,
            iRssPath,
            EFalse,
            KIptvEmptyDes,
            iLastUpdated );
        }

#else

    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::RunPluginL --- Parsing XML from local filesystem!" );
    iRssPath.Copy( KIptvLocalTestFeed );
    DownloadFinishedL( EDownloadSucceeded, EIptvDlNoError );

#endif
    }

// --------------------------------------------------------------------------
// Set callback pointer to MIptvEpgVodCallback. Plugin uses
// MIptvEpgVodCallback interface to supply Epg data to system.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::SetVodCallback( MIptvEpgVodCallback* aVodCallback )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::SetVodCallback" );

    iXmlContentHandler->SetVodCallback( aVodCallback );
    iVodCallback = aVodCallback;
    }

// --------------------------------------------------------------------------
// Set callback pointer to MIptvEpgLiveTvCallback.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::SetLiveTvCallback(
    MIptvEpgLiveTvCallback* /*aLiveTvCallback*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::SetLiveTvCallback" );
    }

// --------------------------------------------------------------------------
// From CIptvEpgPluginInterface interface
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::SetServiceInformation(
    CIptvEpgService* aService,
    TBool aUseSearchAddress )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::SetServiceInformation" );

    TIptvRSSAction action( EIptvRssService );
    if ( aUseSearchAddress )
        {
        action = EIptvRssSearch;
        }

    TRAP_IGNORE( SetServiceInformationL( aService, action ) );
    }

// --------------------------------------------------------------------------
// From CIptvEpgPluginInterface interface
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::SetServiceInformationL(
    CIptvEpgService* aService,
    TIptvRSSAction aAction )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::SetServiceInformationL" );

    if ( !aService )
        {
        User::Leave( KErrArgument );
        }

    iAction = aAction;
    iIsGroupOperation  =
        ( ( EIptvRssServiceGroup == iAction ) ||
          ( EIptvRssAddServiceThumbnailToDlQueue == iAction ) ||
          ( EIptvRssAddItemThumbnailToDlQueue == iAction ) );
    iIsSearchOperation = ( EIptvRssSearch == iAction );

    if ( ( EIptvRssAddServiceThumbnailToDlQueue == iAction ) ||
         ( EIptvRssAddItemThumbnailToDlQueue == iAction ) )
        {
        CIptvDownloadItem* item = CIptvDownloadItem::NewL();
        CleanupStack::PushL( item );
        item->iThumbnailUrl.Copy( aService->iAddress );
        item->iServiceThumbnail = ( EIptvRssAddServiceThumbnailToDlQueue == iAction );
        item->iId = aService->iServiceId;
        AddToDlQueueL( item );
        CleanupStack::Pop( item );
        }
    else
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssPlugin::SetServiceInformationL --- USERNAME %S",
            &aService->iUserName );
        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssPlugin::SetServiceInformationL --- PASSWORD %S",
            &aService->iPassword );

        iServiceId = aService->iServiceId;
        if ( iIsSearchOperation )
            {
            iServiceAddress.Copy( aService->SearchUrl() );
            }
        else
            {
            iServiceAddress.Copy( aService->iAddress );
            }

        iDownload->SetServiceInformation( aService );

        if ( iIsGroupOperation )
            {
            HBufC* path = HBufC::NewLC( KIptvMaxPath );
            TPtr16 pathPtr = path->Des();
            TBuf<KIptvCAContentUrlLength> url;
            url.Copy( iServiceAddress );

            // Get pathname for vcfg file.
            BuildUniqueuPathNameL(
                url,
                EIptvPathTmp,
                KIptvVCFGExtension,
                ETrue,
                pathPtr );

            iRssPath.Copy( *path );
            CleanupStack::PopAndDestroy( path );
            }
        else
            {
            // Get RSS temp file path
            TRAP_IGNORE( CIptvUtil::GetPathL( iFs, EIptvPathTmp, iRssPath ) );

            if ( iIsSearchOperation )
                {
                iRssPath.Append( KIptvRssSearchFileName );
                }
            else
                {
                iRssPath.Append( KIptvRssFileName );
                }

            TBuf<KIptvMaxPath> service;
            service.Num( TInt64( iServiceId ) );
            iRssPath.Append( service );
            }

        iLastUpdated = aService->iLastUpdated;

        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssPlugin::SetServiceInformation --- RSS temp file : %S",
            &iRssPath );
        }
    }

// --------------------------------------------------------------------------
// Handles download finished.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::DownloadFinishedL( TInt aState, TIptvDlError aError )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::DownloadFinishedL" );

    TBuf<1> empty;

    if ( !iDlThumbnail )
        {
        switch ( aState )
            {
            case EDownloadFailed:
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin --- CIptvRssPlugin::DownloadFinishedL --- EDownloadFailed" );
                PluginFailedL( aError );
                }
                break;

            case EDownloadSucceeded:
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin --- CIptvRssPlugin::DownloadFinishedL --- EDownloadSucceeded" );
                if ( iIsGroupOperation )
                    {
                    // This is used for group VCFG file download indication.
                    iVodCallback->UpdateServiceThumbnailL( iRssPath, ETrue );
                    }
                else
                    {
                    ParseL();
                    }
                }
                break;

            case EDownloadNoNeed:
            case EDownloadAlreadyDownloaded:
                {
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin --- CIptvRssPlugin::DownloadFinishedL --- EDownloadNoNeed" );
                // When rss file has not been changed, also thumnails are
                // assumed not having been changed.
                if ( iIsGroupOperation )
                    {
                    iVodCallback->ErrorL(
                        KIptvContentUpdateCompleted,
                        EIptvDlNoErrorUpToDate );
                    iVodCallback->UpdateServiceThumbnailL( iRssPath, ETrue );
                    }
                else
                    {
                    PluginSucceededL();
                    }
                }
                break;

            default:
                break;
            }
        }
    else
        {
        if ( iItem )
            {
            if ( iItem->iServiceThumbnail )
                {
                switch ( aState )
                    {
                    case EDownloadFailed:
                        {
                        IPTVLOGSTRING_LOW_LEVEL(
                            "RSS Plugin --- CIptvRssPlugin::DownloadFinishedL --- Service thumbnail download finished --- EDownloadFailed" );
                        iVodCallback->UpdateServiceThumbnailL( empty, EFalse );

                        // Thumbnail file id is not used
                        TUint32 iconFileId( 0 );
                        iVodCallback->UpdateCategoryIconL(
                            iItem->iId, iconFileId, empty, EFalse );
                        }
                        break;

                    case EDownloadSucceeded:
                    case EDownloadNoNeed:
                        // Set as already downloaded only first time,
                        // when really done it.
                        SetAlreadyDownloadedPathL();
                    //lint -fallthrough intended here
                    case EDownloadAlreadyDownloaded:
                        {
                        IPTVLOGSTRING2_LOW_LEVEL(
                            "RSS Plugin --- CIptvRssPlugin::DownloadFinishedL --- Service thumbnail %S",
                            &iDownloadPath );
                        iVodCallback->UpdateServiceThumbnailL(
                            iDownloadPath, EFalse );

                        // Thumbnail file id is not used
                        TUint32 iconFileId( 0 );
                        iVodCallback->UpdateCategoryIconL(
                            iItem->iId,
                            iconFileId,
                            iDownloadPath,
                            EFalse );
                        iVodCallback->ErrorL(
                            KIptvServiceThumbnailDownloaded,
                            iServiceId );
                        }
                        break;

                    default:
                        break;
                    }
                }
            else
                {
                switch ( aState )
                    {
                    case EDownloadFailed:
                        {
                        IPTVLOGSTRING_LOW_LEVEL(
                            "RSS Plugin:: --- CIptvRssPlugin::DownloadFinishedL --- Content thumbnail download finished --- EDownloadFailed" );
                        // Thumbnail file id is not used
                        TUint32 iconFileId( 0 );
                        iVodCallback->UpdateContentIconL(
                            iItem->iId,
                            iconFileId,
                            empty,
                            EFalse );
                        }
                        break;

                    case EDownloadSucceeded:
                    case EDownloadNoNeed:
                        // Set as already downloaded only first time,
                        // when really done it.
                        SetAlreadyDownloadedPathL();
                    //lint -fallthrough intended here
                    case EDownloadAlreadyDownloaded:
                        {
                        IPTVLOGSTRING2_LOW_LEVEL(
                            "RSS Plugin:: --- CIptvRssPlugin::DownloadFinishedL --- Content thumbnail %S",
                            &iDownloadPath );
                        // Thumbnail file id is not used
                        TUint32 iconFileId( 0 );
                        iVodCallback->UpdateContentIconL(
                            iItem->iId,
                            iconFileId,
                            iDownloadPath,
                            EFalse );
                        if ( iIsSearchOperation )
                            {
                            iVodCallback->ErrorL(
                                KIptvRssSearchContentThumbnailDownloaded,
                                iItem->iId );
                            }
                        else
                            {
                            iVodCallback->ErrorL(
                                KIptvContentThumbnailDownloaded,
                                iItem->iId );
                            }
                        }
                        break;

                    default:
                        break;
                    }
                }
            }

        iDlThumbnail = EFalse;
        DownloadThumbnailsL();
        }
    }

// --------------------------------------------------------------------------
// Set plugin failed.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::PluginFailedL( TIptvDlError aError )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin:: --- CIptvRssPlugin::PluginFailedL" );

    TTime valid( KIptvRssUpdateFailedInterval );

    if ( iParsingFailed )
        {
        if ( iIsSearchOperation )
            {
            iVodCallback->ErrorL(
                KIptvErrorRssSearchFailed, KIptvRssParserError );
            }
        else
            {
            iVodCallback->ErrorL(
                KIptvErrorEpgUpdateFailed, KIptvRssParserError );
            }
        }
    else
        {
        if ( iIsSearchOperation )
            {
            iVodCallback->ErrorL( KIptvErrorRssSearchFailed, aError );
            }
        else
            {
            iVodCallback->ErrorL( KIptvErrorEpgUpdateFailed, aError );
            }
        }

    iVodCallback->FinishedL( valid );
    }

// --------------------------------------------------------------------------
// Set plugin succeeded.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::PluginSucceededL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin:: --- CIptvRssPlugin::PluginSucceededL" );

    TInt64 ms;
    ms = KIptvMinuteAsMicrosecond * KIptvRssUpdateSuccesInterval;
    TTime valid( ms );

    if ( iIsSearchOperation )
        {
        iVodCallback->ErrorL( KIptvErrorRssSearchSucceed, 0 );
        }
    else
        {
        iVodCallback->ErrorL( KIptvErrorEpgUpdateSuccessed, 0 );
        }

    iVodCallback->FinishedL( valid );
    }

// --------------------------------------------------------------------------
// Parse EPG data.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::ParseL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "RSS Plugin:: --- CIptvRssPlugin::ParseL" );

    TRAPD( err, StartParsingXmlFileL( iRssPath ) );
    if ( err != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssPlugin::ParseL --- Leave while Parse() = %d",
            err );
        PluginFailedL( EIptvDlNoError );
        }

#ifndef IPTV_RSS_LOCAL_TEST

    // If exist, delete downloaded rss file.
    if ( BaflUtils::FileExists( iFs, iRssPath ) )
        {
        BaflUtils::DeleteFile( iFs, iRssPath );
        }

#endif

    }

// --------------------------------------------------------------------------
// Parse EPG data.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::StartParsingXmlFileL( const TDesC& aFileName )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::StartParsingXmlFileL" );

    RFile phile;
    User::LeaveIfError( phile.Open( iFs, aFileName, EFileRead ) );
    CleanupClosePushL( phile );
    TInt size( 0 );
    User::LeaveIfError( phile.Size( size ) );
    delete iXmlFileContent;
    iXmlFileContent = NULL;
    if ( 0 < size )
        {
        iXmlFileContent = HBufC8::NewL( size );
        TPtr8 fileContentPtr = iXmlFileContent->Des();
        User::LeaveIfError( phile.Read( fileContentPtr ) );
        size = iXmlFileContent->Length();
        }
    CleanupStack::PopAndDestroy( &phile );

    // There was at least some content in the file
    if ( 0 < size )
        {
        iCurrentParsePosition = 0;
        iParser->ParseBeginL();
        if ( iParserIdle->IsActive() )
            {
            iParserIdle->Cancel();
            }
        TCallBack cb( ParseIdleCallback, this );
        iParserIdle->Start( cb );
        }
    else
        {
        iParsingFailed = ETrue;
        PluginFailedL( EIptvDlContentNotFound );
        }
    }

// --------------------------------------------------------------------------
// Static entry point for CIdle callback.
// --------------------------------------------------------------------------
//
TInt CIptvRssPlugin::ParseIdleCallback( TAny* aPluginImpl )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::ParseIdleCallback" );

    CIptvRssPlugin* self = static_cast<CIptvRssPlugin*> ( aPluginImpl );
    if ( self )
        {
        return self->HandleParseIdleCallback();
        }
    else
        {
        return NULL;
        }
    }

// --------------------------------------------------------------------------
// Add thumbnail to download queue.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::AddToDlQueueL( const CIptvDownloadItem* aItem )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::AddToDlQueueL" );

    iDlQueue.AppendL( aItem );
    }

// --------------------------------------------------------------------------
// Download thumbnails from download queue.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::DownloadThumbnailsL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::DownloadThumbnailsL" );

    if ( iParsingFailed )
        {
        PluginFailedL( EIptvDlNoError );
        return;
        }

    iDlThumbnail = ETrue;
    TBool download( EFalse );
    TBool loop( ETrue );
    while ( loop )
        {
        download = EFalse;
        TInt count( iDlQueue.Count() );
        if ( count > 0 )
            {
            delete iItem;
            iItem = NULL;
            iItem = iDlQueue[0];
            iDlQueue.Remove( 0 );

            if ( iItem )
                {
                iServiceAddress.Copy( iItem->iThumbnailUrl );

                // Create variable for thumbnail path
                HBufC* path = HBufC::NewLC( KIptvMaxPath );
                TPtr16 pathPtr = path->Des();

                // If downloading service thumbnail
                if ( iItem->iServiceThumbnail )
                    {
                    TBool doDownload( EFalse );

                    if ( ! iIsSearchOperation )
                        {
                        doDownload = ETrue;
                        }
                    else
                        {
                        // we do nothing here. We dont want to download service
                        // thumbnails for search results
                        }

                    // Download service thumbnail only for "normal" vod.
                    // When doing search service thumbnail is not downloaded.
                    if ( doDownload )
                        {
                        // Get pathname for service thumbnail.
                        BuildUniqueuPathNameL(
                            iItem->iThumbnailUrl,
                            EIptvPathService,
                            KIptvThumbnailExtension,
                            EFalse,
                            pathPtr );

                        // If exists, add it to thumbnail list.
                        if ( BaflUtils::FileExists( iFs, *path ) )
                            {
                            HBufC* srvIcon = HBufC::NewLC( path->Length() );
                            TPtr16 srvIconPtr = srvIcon->Des();
                            srvIconPtr.Copy( *path );
                            iIconList.AppendL( srvIcon );
                            CleanupStack::Pop( srvIcon );
                            }

                        loop = EFalse;
                        download = ETrue;
                        iDownloadPath.Copy( *path );
                        IPTVLOGSTRING2_LOW_LEVEL(
                            "RSS Plugin --- CIptvRssPlugin::DownloadThumbnailsL --- Downloading service thumbnail -> %S",
                            &iDownloadPath );
                        }
                    else
                        {
                        loop = ETrue;
                        download = EFalse;
                        }
                    }
                else
                    {
                    if ( iThumbnailDlAllowed )
                        {
                        if ( iIsSearchOperation )
                            {
                            // Get pathname for search thumbnail.
                            BuildUniqueuPathNameL(
                                iItem->iThumbnailUrl,
                                EIptvPathRssSearchThumbnail,
                                KIptvThumbnailExtension,
                                ETrue,
                                pathPtr );
                            }
                        else if ( iIsGroupOperation )
                            {
                            // Get pathname for item thumbnail.
                            BuildUniqueuPathNameL(
                                iItem->iThumbnailUrl,
                                EIptvPathService,
                                KIptvThumbnailExtension,
                                ETrue,
                                pathPtr );
                            }
                        else
                            {
                            // Get pathname for item thumbnail.
                            BuildUniqueuPathNameL(
                                iItem->iThumbnailUrl,
                                EIptvPathRssThumbnail,
                                KIptvThumbnailExtension,
                                ETrue,
                                pathPtr );
                            }

                        loop = EFalse;
                        download = ETrue;
                        iDownloadPath.Copy( *path );
                        IPTVLOGSTRING2_LOW_LEVEL(
                            "RSS Plugin --- CIptvRssPlugin::DownloadThumbnailsL ---  Downloading content thumbnail -> %S",
                            &iDownloadPath );
                        }
                    }
                CleanupStack::PopAndDestroy( path );
                }
            }
        else
            {
            loop = EFalse;
            PluginSucceededL();
            if ( EIptvRssService == iAction )
                {
                RemoveObsoleteIconsL();
                }

            // Clear already downloaded thumbnail list.
            iPreviouslyDownloadedAddresses.ResetAndDestroy();
            iPreviouslyDownloadedPaths.ResetAndDestroy();
            }
        }

    if ( download )
        {
        // Get address from possible previously downloaded.
        HBufC* alreadyDownloaded = GetAlreadyDownloadedPath();
        if ( NULL == alreadyDownloaded )
            {
            TRAPD(
                dlError,
                iDownload->DownloadL(
                    iServiceAddress,
                    iDownloadPath,
                    ETrue,
                    KIptvEmptyDes,
                    iLastUpdated ) );

            // Starting of thumbnail download can fail for example because of
            // invalid URL.
            if ( dlError != KErrNone )
                {
                DownloadFinishedL( EDownloadFailed, EIptvDlGeneral );
                }
            }
        else
            {
            // When already downloaded, use it.
            if ( 0 != iDownloadPath.CompareF( *alreadyDownloaded ) )
                {
                BaflUtils::CopyFile( iFs, *alreadyDownloaded, iDownloadPath );
                }

            // Retain old Thumbnail and remove it from list.
            HBufC* fileName = HBufC::NewLC( iDownloadPath.Length() );
            fileName->Des().Copy( iDownloadPath );
            RemoveIconFromList( fileName, EFalse );
            CleanupStack::PopAndDestroy( fileName );

            // Signaling must be done async to avoid recursive looping.
            iDownload->SkipCurrentDownloadAsyncL();
            }
        }
    }

// --------------------------------------------------------------------------
// Keep track of already downloaded thumbnails to avoid to redownload.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::SetAlreadyDownloadedPathL()
    {

#ifdef _DEBUG

    // Debug print filename and url.
    TBuf<KIptvMaxPath> debugFileName;
    debugFileName.Copy( iDownloadPath );

    TBuf<KIptvMaxPath> debugUrl;
    debugUrl.Copy( iServiceAddress );

    IPTVLOGSTRING3_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::SetAlreadyDownloadedPathL --- Set to already downloaded: %S From: %S",
        &debugFileName,
        &debugUrl );

#endif

    // Append new service address.
    TInt len( iServiceAddress.Length() );
    HBufC8* addr = HBufC8::NewLC( len );
    TPtr8 addrPtr = addr->Des();
    addrPtr.Copy( iServiceAddress );
    iPreviouslyDownloadedAddresses.AppendL( addr );
    CleanupStack::Pop( addr );

    // Append corresponding path.
    len = iDownloadPath.Length();
    HBufC* path = HBufC::NewLC( len );
    TPtr16 pathPtr = path->Des();
    pathPtr.Copy( iDownloadPath );
    iPreviouslyDownloadedPaths.AppendL( path );
    CleanupStack::Pop( path );
    }

// --------------------------------------------------------------------------
// Keep track of already downloaded thumbnails to avoid to redownload.
// --------------------------------------------------------------------------
//
HBufC* CIptvRssPlugin::GetAlreadyDownloadedPath()
    {
    HBufC* alreadyDownloaded( NULL );

    TInt icon( -1 );

    // Find address from list.
    for ( TInt i = 0;
          ( i < iPreviouslyDownloadedAddresses.Count() ) && ( -1 == icon );
          i++ )
        {
        if ( 0 == iServiceAddress.CompareF( *iPreviouslyDownloadedAddresses[i] ) )
            {
            icon = i;
            }
        }

    // When found, return it, otherwise append new one to the list.
    if ( icon >= 0 )
        {
        if ( iPreviouslyDownloadedPaths.Count() > icon )
            {
            alreadyDownloaded = iPreviouslyDownloadedPaths[icon];

#ifdef _DEBUG

            // Debug print filename and url.
            TBuf<KIptvMaxPath> debugFileName;
            debugFileName.Copy( *alreadyDownloaded );

            TBuf<KIptvMaxPath> debugUrl;
            debugUrl.Copy( iServiceAddress );

            IPTVLOGSTRING3_LOW_LEVEL(
                "RSS Plugin --- CIptvRssPlugin::GetAlreadyDownloadedPathL --- Already downloaded: %S From: %S",
                &debugFileName,
                &debugUrl );

#endif

            }
        }

    return alreadyDownloaded;
    }

// --------------------------------------------------------------------------
// Handle idle callback functionality.
// --------------------------------------------------------------------------
//
TInt CIptvRssPlugin::HandleParseIdleCallback()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::HandleParseIdleCallback" );

    // By default add KReadAtOnceLength to current parse position
    TInt lengthToBeExtracted( KReadAtOnceLength );
    TBool isLastPart( EFalse );

    // Dealing with last part of the data in the buffer
    if ( ( iCurrentParsePosition + lengthToBeExtracted ) >
         iXmlFileContent->Length() )
        {
        lengthToBeExtracted =
            iXmlFileContent->Length() - iCurrentParsePosition;
        isLastPart = ETrue;
        }

    // Get next data part to be parsed
    TPtr8 midPtr = iXmlFileContent->Des().MidTPtr(
        iCurrentParsePosition, lengthToBeExtracted );
    TInt err( KErrNone );

    // Trap possible leave. If there was something, wrong, just stop parsing
    TRAP( err, iParser->ParseL( midPtr ) );

    TInt retVal( 0 );
    // ParseL went just fine
    if ( err == KErrNone )
        {
        // We are dealing with last part of the data -> let's not call this
        // callback again
        if ( isLastPart )
            {
            // If some leave occured, just ignore it
            TRAP( err, iParser->ParseEndL() );
            iCurrentParsePosition = 0;

#ifndef IPTV_RSS_LOCAL_TEST

            if ( iThumbnailDlAllowed && ( EIptvRssService == iAction ) )
                {
                // We will start thumbnail downloading. Lets list
                // current icons for comparing them with new available.
                TRAP_IGNORE( ListCurrentIconsL() )
                }

            TRAP( err, DownloadThumbnailsL() );

#else

            TRAP_IGNORE( PluginSucceededL() );

#endif // IPTV_RSS_LOCAL_TEST

            }
        // There will still be more data to be parsed in the buffer
        else
            {
            iCurrentParsePosition += KReadAtOnceLength;
            retVal = 1;
            }
        }
    // ParseL leaved. Tell manager, that we have finished our job
    else
        {
        TRAP( err, PluginFailedL( EIptvDlNoError ) );
        retVal = 0;
        }

    return retVal;
    }

// --------------------------------------------------------------------------
// Check is thumbnail download allowed.
// --------------------------------------------------------------------------
//
TBool CIptvRssPlugin::ThumbnailDlAllowedL() const
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::ThumbnailDlAllowedL" );

    TBool allow( EFalse );
    CRepository* cenRep = CRepository::NewL( KIptvCenRepUid );
    if ( cenRep )
        {
        CleanupStack::PushL( cenRep );
        TInt value;
        TInt error = cenRep->Get( KIptvCenRepVoDThumbnailsKey, value );
        if ( error == KErrNone && value == 1 )
            {
            allow = ETrue;
            }
        CleanupStack::PopAndDestroy( cenRep );
        }

    return allow;
    }

// --------------------------------------------------------------------------
// XML parsing failed.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::SetParsingFailed()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::SetParsingFailed" );

    iParsingFailed = ETrue;
    }

// --------------------------------------------------------------------------
// Content update completed.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::ContentUpdateCompletedL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::ContentUpdateCompletedL" );

    iVodCallback->ContentUpdateCompletedL();

    if ( EIptvRssService == iAction )
        {
        iVodCallback->ErrorL( KIptvThumbnailCount, iDlQueue.Count() );
        }
    else if ( EIptvRssSearch == iAction )
        {
        iVodCallback->ErrorL( KIptvRssSearchThumbnailCount, iDlQueue.Count() );
        }
    }

// --------------------------------------------------------------------------
// Set first update.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::SetFirstUpdate( TBool /*aFirstUpdate*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::SetFirstUpdate" );
    }

// --------------------------------------------------------------------------
// Build unique pathname for download object.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::BuildUniqueuPathNameL(
    const TDesC& aUrl,
    const TIptvPathSelection aPathSelection,
    const TDesC& aDefaultExtension,
    const TBool aGenerateName,
    TPtr aPathName )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::BuildUniqueuPathNameL" );

    // Lets get files type extension ex. .jpg, vcgf, ...
    TInt urlLength( aUrl.Length() );
    TInt index( aUrl.LocateReverse( '.' ) );
    TInt extensionLength( 0 );

    if ( ( urlLength - index ) < KIptvExtensionMaxSize )
        {
        extensionLength = urlLength - index;
        }

    // Get path for download object.
    if ( ( EIptvPathService == aPathSelection ) ||
         ( EIptvPathTmp == aPathSelection ) )
        {
        CIptvUtil::GetPathL( iFs, aPathSelection, aPathName );
        }
    // Item thumbnail must be hashed to keep track on it.
    else
        {
        CIptvUtil::GetPathL( iFs, aPathSelection, iServiceId, aPathName );
        }

    if ( aGenerateName )
        {
        // Check whether md5 calculator is already constructed.
        if ( !iMD5Calculator )
            {
            iMD5Calculator = CMD5::NewL();
            }

        // Calculate the 128 bit, 16 byte hash
        iMD5Calculator->Reset();
        TBuf8<KIptvMaxPath> buffer;
        buffer.Copy( aUrl );

        TPtrC8 hash = iMD5Calculator->Hash( buffer );

        // Now print it as a 32 byte hex number
        TBuf8<KIptvHexCharInByte> hexNumber;
        for ( TInt i = 0; i < hash.Length(); i++ )
            {
            hexNumber.Zero();
            hexNumber.Format( KIptvHexByteFormatStr, hash[i] );
            aPathName.Append( hexNumber[0] );
            aPathName.Append( hexNumber[1] );
            }
        }
    else
        {
        // Service thumbnail can be with service identifier.
        aPathName.AppendNum( static_cast<TUint64>( iServiceId ) );
        }

    // Add thumbnail file extension.
    if ( extensionLength > 0 )
        {
        // If there was a extension for thumbnail use it
        aPathName.Append( aUrl.Right( extensionLength ) );
        }
    else
        {
        // By default add .jpg extension
        aPathName.Append( aDefaultExtension );
        }

#ifdef _DEBUG

    // Debug print filename.
    TBuf<KIptvMaxPath> debugFileName;
    debugFileName.Copy( aPathName );

    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::BuildUniqueuPathNameL Generated path name: %S",
        &debugFileName );

#endif

    }

// --------------------------------------------------------------------------
// List current icons.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::ListCurrentIconsL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::ListCurrentIconsL" );

    CDir* iconList( NULL );

    // Create mask path for thumbnail search
    HBufC* mask = HBufC::NewLC( KIptvMaxPath );
    TPtr16 maskPtr = mask->Des();

    // list item icons.
    CIptvUtil::GetPathL( iFs, EIptvPathRssThumbnail, iServiceId, maskPtr );
    TInt dirLen( mask->Length() );
    maskPtr.Append( KIptvAllFiles );

    if ( iFs.GetDir( maskPtr,
                     KEntryAttNormal,
                     ESortNone,
                     iconList ) == KErrNone )
        {
        CleanupStack::PushL( iconList );

        for ( TInt i = 0; i < iconList->Count(); i++ )
            {
            TInt len( ( *iconList )[i].iName.Length() );
            HBufC* icon = HBufC::NewLC( dirLen + len );
            TPtr16 iconPtr = icon->Des();
            iconPtr.Copy( maskPtr.Mid( 0, dirLen ) );
            iconPtr.Append( ( *iconList )[i].iName.Mid( 0, len ) );
            iIconList.AppendL( icon );

            CleanupStack::Pop( icon );
            }

        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin --- CIptvRssPlugin::ListCurrentIconsL found: %d",
            iIconList.Count() );

        CleanupStack::PopAndDestroy( iconList );
        }

    CleanupStack::PopAndDestroy( mask );
    }

// --------------------------------------------------------------------------
// Remove icons stored to list. These icons are either from remove items or
// all icons are decided to be removed.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::RemoveObsoleteIconsL()
    {
    IPTVLOGSTRING2_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::RemoveObsoleteIconsL Count       : %d",
        iIconList.Count() );

    HBufC* iconPath( NULL );

    // Fetch thumbnail of last watched clip, when any.
    CIptvLastWatchedApi* lastWatchedApi = CIptvLastWatchedApi::NewL();
    CleanupStack::PushL( lastWatchedApi );

    CIptvLastWatchedData* lwData = CIptvLastWatchedData::NewL();
    if ( lwData )
        {
        CleanupStack::PushL( lwData );
        TInt err( KErrNone );
        TRAPD( leaveErr, err = lastWatchedApi->GetLastWatchedDataL( *lwData ) );

        if ( KErrNone == leaveErr && KErrNone == err )
            {
            // Fetch thumbnail, no need to put it to cleanup stack.
            iconPath = HBufC::NewL( lwData->IconPath().Length() );
            iconPath->Des().Copy( lwData->IconPath() );
            }
        CleanupStack::PopAndDestroy( lwData );
        }
    CleanupStack::PopAndDestroy( lastWatchedApi );

    for ( TInt icon = 0; icon < iIconList.Count(); icon++ )
        {
        TBool wasLastWatched( EFalse );

        // Check whether thumbnail belongs to last watched clip.
        if ( iconPath )
            {
            if ( 0 == iIconList[icon]->Des().CompareF( *iconPath ) )
                {
                wasLastWatched = ETrue;
                }
            }

#ifdef _DEBUG

        // Debug print filename.
        TBuf<KIptvMaxPath> debugFileName;
        debugFileName.Copy( iIconList[icon]->Des() );
        if ( wasLastWatched )
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssPlugin::RemoveObsoleteIconsL Thumbnail of last watched clip saved: %S",
                &debugFileName );
            }
        else
            {
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin --- CIptvRssPlugin::RemoveObsoleteIconsL Thumbnail deleted: %S",
                &debugFileName );
            }

#endif

        if ( !wasLastWatched )
            {
            // Delete list thumbnail.
            iFs.Delete( iIconList[icon]->Des() );
            }
        }

    if ( iconPath )
        {
        delete iconPath;
        iconPath = NULL;
        }

    // Remove all icons from list.
    iIconList.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// Find thumbnail from list and delete it when found.
// --------------------------------------------------------------------------
//
void CIptvRssPlugin::RemoveIconFromList(
    const HBufC* aIconName,
    TBool aDelete )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin --- CIptvRssPlugin::RemoveIconFromList" );

    if ( aIconName && ( EIptvRssService == iAction ) )
        {
        TInt icon( -1 );

        // Find icon from list.
        for ( TInt i = 0; ( i < iIconList.Count() ) && ( -1 == icon ); i++ )
            {
            if ( 0 == iIconList[i]->Des().CompareF( *aIconName ) )
                {
                icon = i;
                }
            }

        // When found, remove it.
        if ( icon >= 0 )
            {
            if ( aDelete )
                {

#ifdef _DEBUG

                // Debug print filename.
                TBuf<KIptvMaxPath> debugFileName;
                debugFileName.Copy( iIconList[icon]->Des() );
                IPTVLOGSTRING2_LOW_LEVEL(
                    "RSS Plugin --- CIptvRssPlugin::RemoveIconFromList --- Thumbnail deleted: %S",
                    &debugFileName );

#endif

                // Delete list thumbnail.
                iFs.Delete( iIconList[icon]->Des() );
                }

            // Remove thumbnail from list.
            delete iIconList[icon];
            iIconList.Remove( icon );
            }
        }
    }
