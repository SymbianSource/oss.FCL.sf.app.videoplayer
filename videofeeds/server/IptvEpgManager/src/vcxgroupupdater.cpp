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




#include <e32base.h>
#include <apgtask.h>
#include <apgcli.h>
#include <bautils.h>    // file helpers
#include "vcxgroupupdater.h"
#include "CIptvServer.h"
#include "CIptvEpgSession.h"
#include "CIptvService.h"
#include "IptvDebug.h"
#include "CIptvTimer.h"
#include "CIptvEpgService.h"
#include "CIptvEpgVodMsqQueue.h"

const TInt KIptvVcfgFileParseWait( 500000 ); // 0.5 s
const TInt KIptvVcfgFileParseMaxTry( 10 );   // 5 s

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructors and destructors.
// ---------------------------------------------------------------------------
//
CVcxGroupUpdater* CVcxGroupUpdater::NewL(
    CIptvServer& aServer,
    CIptvEpgSession& aSession,
    CIptvEpgVodMsqQueue& aMsgQueue )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxGroupUpdater::NewL" );

    CVcxGroupUpdater* self =
        new( ELeave ) CVcxGroupUpdater( aServer, aSession, aMsgQueue );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CVcxGroupUpdater::CVcxGroupUpdater(
    CIptvServer& aServer,
    CIptvEpgSession& aSession,
    CIptvEpgVodMsqQueue& aMsgQueue ) :
    iServer( aServer ),
    iSession( aSession ),
    iMsgQueue( aMsgQueue ),
    iCurrentAction( CIptvEpgPluginInterface::EIptvRssNone )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CVcxGroupUpdater::CVcxGroupUpdater" );
    }

void CVcxGroupUpdater::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxGroupUpdater::ConstructL" );

    User::LeaveIfError( iFs.Connect() );
    }

CVcxGroupUpdater::~CVcxGroupUpdater()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CVcxGroupUpdater::~CVcxGroupUpdater" );

    if ( iVcfgParseTimer )
        {
        iVcfgParseTimer->Cancel();
        delete iVcfgParseTimer;
        }

    delete iDownloadedVcfgPath;
    delete iPlugin;
    REComSession::FinalClose();

    iFs.Close();
    }

// ---------------------------------------------------------------------------
// Start download.
// ---------------------------------------------------------------------------
//
void CVcxGroupUpdater::StartGroupUpdateL( CIptvEpgService* aService )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CVcxGroupUpdater::StartGroupDownloadL - enter" );

    iLocalValid = EFalse;

    // Create download plugin.
    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        REComSession::FinalClose();
        }

    if ( !iPlugin )
        {
        iPlugin = CIptvEpgPluginInterface::NewL( aService->iUid );
        }

    iCurrentGroupServiceId = aService->iServiceId;
    iDownloadStartTime.UniversalTime();

    // Initialize download plugin.
    iPlugin->SetVodCallback( this );
    iCurrentAction = CIptvEpgPluginInterface::EIptvRssServiceGroup;
    iPlugin->SetServiceInformationL( aService, iCurrentAction );

    // Start actual download.
    iPlugin->RunPlugin();

    IPTVLOGSTRING_LOW_LEVEL(
        "CVcxGroupUpdater::StartDownloadL - exit" );
    }

// ---------------------------------------------------------------------------
// Cancel current download download, if any.
// ---------------------------------------------------------------------------
//
void CVcxGroupUpdater::CancelGroupUpdate()
    {
    IPTVLOGSTRING3_LOW_LEVEL(
        "CVcxGroupUpdater::CancelGroupUpdate -- %d/%d",
        iPluginRunning,
        iCurrentAction );

    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        REComSession::FinalClose();
        }

    iPluginRunning = EFalse;
    iCurrentAction = CIptvEpgPluginInterface::EIptvRssNone;
    }

// ---------------------------------------------------------------------------
// Enquire is download ongoing.
// ---------------------------------------------------------------------------
//
TBool CVcxGroupUpdater::IsGroupUpdateOngoing()
    {
    IPTVLOGSTRING2_LOW_LEVEL(
        "CVcxGroupUpdater::IsGroupUpdateOngoing -- %d",
        iPluginRunning );

    return iPluginRunning;
    }

// ---------------------------------------------------------------------------
// From MIptvEpgVodCallback interface
// ---------------------------------------------------------------------------
//
void CVcxGroupUpdater::StartedL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CVcxGroupUpdater::StartedL -- Rss Plugin started" );

    iPluginRunning = ETrue;
    }

// ---------------------------------------------------------------------------
// From MIptvEpgVodCallback interface
// ---------------------------------------------------------------------------
//
void CVcxGroupUpdater::ContentUpdateCompletedL()
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CVcxGroupUpdater::ContentUpdateCompletedL" );
    }

// ---------------------------------------------------------------------------
// From MIptvEpgVodCallback interface
// ---------------------------------------------------------------------------
//
TInt CVcxGroupUpdater::FinishedL(
    TTime& /* aUpdateInterval */ )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CVcxGroupUpdater::FinishedL -- Rss Plugin finished" );

    iPluginRunning = EFalse;
    iCurrentAction = CIptvEpgPluginInterface::EIptvRssNone;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MIptvEpgVodCallback interface
// ---------------------------------------------------------------------------
//
TInt CVcxGroupUpdater::AddCategoryL(
    CIptvEpgCategory& /* aCategory */,
    TUint32& /* aCategoryKey */ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxGroupUpdater::AddCategoryL" );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MIptvEpgVodCallback interface
// ---------------------------------------------------------------------------
//
TInt CVcxGroupUpdater::AddContentL(
    CIptvEpgContent& /* aContent */,
    RPointerArray<CIptvMediaContent> /* aMediaContents */,
    TUint32& /* aContentKey */ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxGroupUpdater::AddContentL" );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MIptvEpgVodCallback interface
// ---------------------------------------------------------------------------
//
TInt CVcxGroupUpdater::UpdateServiceThumbnailL(
    TDesC& aFilePath,
    const TBool aGrouped )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "CVcxGroupUpdater::UpdateServiceThumbnailL" );

    // Also VCFG file download is indicated here.
    if ( CIptvEpgPluginInterface::EIptvRssServiceGroup == iCurrentAction )
        {
        StartVcfgFileParseL( aFilePath );
        }
    else
        {
        iSession.UpdateServiceThumbnailL( aFilePath, aGrouped );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MIptvEpgVodCallback interface
// ---------------------------------------------------------------------------
//
TInt CVcxGroupUpdater::AddRelationL(
    TUint32& /* aCategoryKey */,
    TUint32& /* aContentKey */ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxGroupUpdater::AddRelationL" );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From MIptvEpgVodCallback interface
// ---------------------------------------------------------------------------
//
void CVcxGroupUpdater::ErrorL(
    TInt aError,
    TInt aInfo )
    {
    IPTVLOGSTRING3_HIGH_LEVEL(
        "CVcxGroupUpdater::ErrorL( %d, %d )", aError, aInfo );

#ifdef _DEBUG

    switch ( aError )
        {
        case KIptvContentUpdateCompleted:
            IPTVLOGSTRING_HIGH_LEVEL( " -- KIptvContentUpdateCompleted" );
            break;
        case KIptvServiceThumbnailDownloaded:
            IPTVLOGSTRING_HIGH_LEVEL( " -- KIptvServiceThumbnailDownloaded" );
            break;
        case KIptvThumbnailCount:
            IPTVLOGSTRING_HIGH_LEVEL( " -- KIptvThumbnailCount" );
            break;
        case KIptvRssSearchThumbnailCount:
            IPTVLOGSTRING_HIGH_LEVEL( " -- KIptvRssSearchThumbnailCount" );
            break;
        case KIptvErrorEpgUpdateSuccessed:
            IPTVLOGSTRING_HIGH_LEVEL( " -- KIptvErrorEpgUpdateSuccessed" );
            break;
        case KIptvErrorRssSearchSucceed:
            IPTVLOGSTRING_HIGH_LEVEL( " -- KIptvErrorRssSearchSucceed" );
            break;
        case KIptvContentThumbnailDownloaded:
            IPTVLOGSTRING_HIGH_LEVEL( " -- KIptvContentThumbnailDownloaded" );
            break;
        case KIptvRssSearchContentThumbnailDownloaded:
            IPTVLOGSTRING_HIGH_LEVEL( " -- KIptvRssSearchContentThumbnailDownloaded" );
            break;
        case KIptvErrorEpgUpdateFailed:
            IPTVLOGSTRING_HIGH_LEVEL( " -- KIptvErrorEpgUpdateFailed" );
            break;
        case KIptvErrorRssSearchFailed:
            IPTVLOGSTRING_HIGH_LEVEL( " -- KIptvErrorRssSearchFailed" );
            break;
        default:
            IPTVLOGSTRING_HIGH_LEVEL( " -- Unknown" );
        }

#endif // _DEBUG

    if ( ( KIptvContentUpdateCompleted == aError ) &&
         ( EIptvDlNoErrorUpToDate == aInfo ) )
        {
        IPTVLOGSTRING_HIGH_LEVEL( " -- Local VCFG-file valid" );
        iLocalValid = ETrue;
        }
    else
        {
        if ( ( KIptvErrorEpgUpdateSuccessed == aError ) &&
             ( KErrNone == aInfo ) )
            {

#ifdef _DEBUG

                // Debug print last updated info.
                TBuf<KIptvEpgDbLastModifiedMaxLength> lastModifiedDatePrint;
                _LIT( KIptvDatePrint, "%D%M%Y%/0%1%/1%2%/2%3%/3" );
                iDownloadStartTime.FormatL( lastModifiedDatePrint, KIptvDatePrint );

                TBuf<KIptvEpgDbLastModifiedMaxLength> lastModifiedTimePrint;
                _LIT( KIptvTimePrint, "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B" );
                iDownloadStartTime.FormatL( lastModifiedTimePrint, KIptvTimePrint );

                IPTVLOGSTRING3_LOW_LEVEL( " -- Group update ok, last modified set: %S %S",
                    &lastModifiedDatePrint,
                    &lastModifiedTimePrint );

#endif // _DEBUG

            SetLastModifiedL( iCurrentGroupServiceId, iDownloadStartTime );
            }

        // Inform clients about failure.
        iMsgQueue.SendMessageToClientL( aError, aInfo );
        }
    }

// ---------------------------------------------------------------------------
// From MIptvEpgVodCallback interface
// ---------------------------------------------------------------------------
//
TInt CVcxGroupUpdater::UpdateCategoryIconL(
    TUint32& /* aCategoryKey */,
    TUint32& /* aIconFileId */,
    TDesC& /* aIconFilePath */,
    TBool /* aIsUrl */ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxGroupUpdater::UpdateCategoryIconL" );

    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// From MIptvEpgVodCallback interface
// ---------------------------------------------------------------------------
//
TInt CVcxGroupUpdater::UpdateContentIconL(
    TUint32& aContentKey,
    TUint32& /* aIconFileId */,
    TDesC& aIconFilePath,
    TBool /* aIsUrl */ )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxGroupUpdater::UpdateContentIconL" );

    // Update icon pathname to service.
    CIptvService* service = iServer.iServiceManager->GetServiceL(
        aContentKey, ETrue /* do open/close */ );
    if ( service )
        {
        CleanupStack::PushL( service );  // 1->
        service->SetIconPath( aIconFilePath );
        iServer.iServiceManager->UpdateServiceL( *service );
        CleanupStack::PopAndDestroy( service ); //<-1
        return KErrNone;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Starts parsing of downloaded VCFG file.
// ---------------------------------------------------------------------------
//
void CVcxGroupUpdater::StartVcfgFileParseL(
    const TDesC& aPathName )
    {
    IPTVLOGSTRING_HIGH_LEVEL(
        "CVcxGroupUpdater::StartVcfgFileParseL" );

    TInt error( KErrNone );

    if ( iLocalValid )
        {
        iMsgQueue.SendMessageToClientL(
            KIptvContentUpdateCompleted,
            EIptvDlNoErrorUpToDate );
        }
    else
        {
        // Save file name for deletion.
        if ( iDownloadedVcfgPath )
            {
            delete iDownloadedVcfgPath;
            iDownloadedVcfgPath = NULL;
            }

        iDownloadedVcfgPath = aPathName.AllocL();

        // Clear vcfg received flag
        SetNotParsedL( iCurrentGroupServiceId );

        // Send vcfg to provision
        RApaLsSession ls;
        error = ls.Connect();
        if ( error == KErrNone )
            {
            // Get UID of application which handles this type of file
            TUid appUid( TUid::Null() );
            TDataType dataType( appUid );
            error = ls.AppForDocument( aPathName, appUid, dataType );
            if ( appUid == KNullUid )
                {
                error = KErrNotSupported;
                }

            // Start application
            if ( error == KErrNone )
                {
                TThreadId id( 0 );
                error = ls.StartDocument( aPathName, appUid, id );
                }

            ls.Close();
            }
        }

    if ( error == KErrNone )
        {
        // Start timer to wait provision parsing
        if ( !iVcfgParseTimer )
            {
            iVcfgParseTimer =
                CIptvTimer::NewL( CActive::EPriorityStandard, *this );
            }

        if ( !iVcfgParseTimer->IsActive() )
            {
            iVcfgParseTimer->After( KIptvVcfgFileParseWait );
            }

        iVcfgParseMaxCount = KIptvVcfgFileParseMaxTry;
        }
    else
        {
        // Notfy immediately on error case
        iMsgQueue.SendMessageToClientL(
            KIptvContentUpdateCompleted,
            error );
        iPluginRunning = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CVcxGroupUpdater::TimerExpired
// -----------------------------------------------------------------------------
//
void CVcxGroupUpdater::TimerExpired( CIptvTimer* aTimer )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxGroupUpdater::TimerExpired()" );

    TRAPD( err, DoTimerExpiredL( aTimer ) );
    if ( err != KErrNone )
        {
        IPTVLOGSTRING2_LOW_LEVEL(
            "CVcxGroupUpdater::DoTimerExpiredL leaved (%d)",
            err );
        }
    }

// -----------------------------------------------------------------------------
// CVcxGroupUpdater::DoTimerExpiredL
// -----------------------------------------------------------------------------
//
void CVcxGroupUpdater::DoTimerExpiredL( CIptvTimer* aTimer )
    {
    IPTVLOGSTRING_LOW_LEVEL( "CVcxGroupUpdater::DoTimerExpiredL()" );

    // %% How about parse really failed...
    if ( aTimer == iVcfgParseTimer )
        {
        TBool completed( EFalse );

        if ( iLocalValid )
            {
            // Parsing not needed.
            completed = ETrue;
            iLocalValid = EFalse;
            StartThumbnailDownloadL();
            }
        else if ( IsServiceParsed( iCurrentGroupServiceId ) )
            {
            // Parsing complete.
            iMsgQueue.SendMessageToClientL(
                KIptvContentUpdateCompleted,
                KErrNone );
            completed = ETrue;
            StartThumbnailDownloadL();
            }
        else if ( 0 < iVcfgParseMaxCount-- )
            {
            // Continue to wait parsing complete.
            if ( !iVcfgParseTimer->IsActive() )
                {
                iVcfgParseTimer->After( KIptvVcfgFileParseWait );
                }
            }
        else
            {
            IPTVLOGSTRING_LOW_LEVEL(
                "CVcxGroupUpdater:: NotifySingleFileDownloadCompleteL timeout" );
            iMsgQueue.SendMessageToClientL(
                KIptvErrorEpgUpdateFailed,
                KIptvRssParserError );
            completed = ETrue;
            iPluginRunning = EFalse;
            }

        // Parsed file is no more needed, so delete it.
        if ( completed && iDownloadedVcfgPath )
            {
            if ( BaflUtils::FileExists( iFs, iDownloadedVcfgPath->Des() ) )
                {
                iFs.Delete( iDownloadedVcfgPath->Des() );
                }

            delete iDownloadedVcfgPath;
            iDownloadedVcfgPath = NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// Initialize VCFG provision done flag.
// ---------------------------------------------------------------------------
//
void CVcxGroupUpdater::SetNotParsedL(
    TUint32 aServiceId )
    {
    IPTVLOGSTRING_HIGH_LEVEL( "CVcxGroupUpdater::SetNotParsedL" );

    // Set parsing status of service.
    CIptvService* service( NULL );
    service = iServer.iServiceManager->GetServiceL(
        aServiceId, ETrue /* do open/close */ );
    if ( service )
        {
        CleanupStack::PushL( service );  // 1->
        service->SetFlags(
            service->GetFlags() & ~CIptvService::EGroupReceived );
        iServer.iServiceManager->UpdateServiceL( *service );
        CleanupStack::PopAndDestroy( service ); //<-1
        }
    }

// ---------------------------------------------------------------------------
// Indication of VCFG file download completion.
// ---------------------------------------------------------------------------
//
TBool CVcxGroupUpdater::IsServiceParsed(
    TUint32 aServiceId )
    {
    IPTVLOGSTRING_HIGH_LEVEL( "CVcxGroupUpdater::IsServiceParsed" );

    TBool parsed( EFalse );

    // Get parsing status of service.
    CIptvService* service( NULL );
    TRAPD( err, service = iServer.iServiceManager->GetServiceL(
        aServiceId, ETrue /* do open/close */ ) );
    if ( service )
        {
        if ( KErrNone == err )
            {
            parsed = ( service->GetFlags() & CIptvService::EGroupReceived ) > 0;
            }

        delete service;
        }

    return parsed;
    }

// ---------------------------------------------------------------------------
// Set service Last Modified.
// ---------------------------------------------------------------------------
//
void CVcxGroupUpdater::SetLastModifiedL(
    TUint32 aServiceId,
    TTime aLastModified )
    {
    IPTVLOGSTRING_HIGH_LEVEL( "CVcxGroupUpdater::SetLastModifiedL" );

    CIptvService* service( NULL );
    service = iServer.iServiceManager->GetServiceL(
        aServiceId, ETrue /* do open/close */ );
    if ( service )
        {
        CleanupStack::PushL( service );  // 1->
        service->SetEpgUpdateTimeL( aLastModified );
        iServer.iServiceManager->UpdateServiceL( *service );
        CleanupStack::PopAndDestroy( service ); //<-1
        }
    }

// ---------------------------------------------------------------------------
// Collect list of required thumbnails and start download.
// ---------------------------------------------------------------------------
//
void CVcxGroupUpdater::StartThumbnailDownloadL()
    {
    if ( iPlugin )
        {
        // Service group thumbnail
        TUint32 groupId( 0 );
        TInt thumbnailCount( 0 );
        CIptvService* tempService = iServer.iServiceManager->GetServiceL(
            iCurrentGroupServiceId, ETrue /* do open/close */ );
        if ( tempService )
            {
            CleanupStack::PushL( tempService );// 1->

            CIptvEpgService* service = CIptvEpgService::NewL();
            CleanupStack::PushL( service ); // 2->
            service->iAddress.Copy( tempService->GetIconUrl() );
            service->iServiceId = tempService->GetId();

            thumbnailCount++;
            groupId = tempService->GetGroupId();
            iPlugin->SetServiceInformationL(
                service,
                CIptvEpgPluginInterface::EIptvRssAddServiceThumbnailToDlQueue );

            CleanupStack::PopAndDestroy( service ); //<-2
            CleanupStack::PopAndDestroy( tempService ); //<-1
            }

        iCurrentAction =
            CIptvEpgPluginInterface::EIptvRssAddServiceThumbnailToDlQueue;
        // For each sub service thumbnail
        CIptvServices* services = iServer.iServiceManager->GetServicesL(
            CIptvServiceManagementClient::EGroupedServices,
            CIptvServiceManagementClient::EDisplayOrderDescending,
            0 /*Latest*/ );
        if ( services )
            {
            CleanupStack::PushL( services ); // 1->

            // Find all services matching to group.
            for ( TInt i( services->Count() - 1 ); i >= 0 ; i-- )
                {
                tempService = services->GetServiceL( i );
                CleanupStack::PushL( tempService ); // 2->

                // Verify group id
                if ( groupId == tempService->GetGroupId() )
                    {
                    CIptvEpgService* service = CIptvEpgService::NewL();
                    CleanupStack::PushL( service ); // 3->
                    service->iAddress.Copy( tempService->GetIconUrl() );
                    service->iServiceId = tempService->GetId();

                    thumbnailCount++;
                    iPlugin->SetServiceInformationL(
                        service,
                        CIptvEpgPluginInterface::EIptvRssAddItemThumbnailToDlQueue );

                    CleanupStack::PopAndDestroy( service ); //<-3
                    }

                CleanupStack::PopAndDestroy( tempService ); //<-2
                }

            CleanupStack::PopAndDestroy( services ); //<-1
            }

        iMsgQueue.SendMessageToClientL( KIptvThumbnailCount, thumbnailCount );
        iPlugin->RunPlugin();
        }
    }
