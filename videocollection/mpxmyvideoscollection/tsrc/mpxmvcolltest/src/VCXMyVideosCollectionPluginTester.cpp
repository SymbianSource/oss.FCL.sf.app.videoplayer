/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description*
*/


// INCLUDE FILES
#include <e32svr.h>
#include <e32std.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroyPushL

#include <e32cons.h>
#include <mpxcollectionutility.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionpath.h>
#include <mpxmessage2.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediabase.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>

#include "VCXTestCommon.h"
#include "VCXTestLog.h"
#include "CIptvTestTimer.h"
#include "CIptvTestActiveWait.h"
#include "VCXTestStatsKeeper.h"

#include "VCXMyVideosCollectionPluginTester.h"
#include "VCXMyVideosTestDlWatcher.h"
#include "VCXMyVideosTestCommon.h"

#include "vcxmyvideoscollection.hrh"
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideosdefs.h"

// CONSTANTS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::NewL
// -----------------------------------------------------------------------------
//
CVCXMyVideosCollectionPluginTester* CVCXMyVideosCollectionPluginTester::NewL(
            MVCXMyVideosCollectionPluginTesterObserver* aObserver, CVCXTestCommon* aTestCommon, CVCXTestStatsKeeper* aStatsKeeper )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::NewL");
    CVCXMyVideosCollectionPluginTester* self = 
        new (ELeave) CVCXMyVideosCollectionPluginTester( aObserver, aTestCommon, aStatsKeeper );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::CVCXMyVideosCollectionPluginTester
// -----------------------------------------------------------------------------
//
CVCXMyVideosCollectionPluginTester::CVCXMyVideosCollectionPluginTester( 
        MVCXMyVideosCollectionPluginTesterObserver* aObserver,
        CVCXTestCommon* aTestCommon, CVCXTestStatsKeeper* aStatsKeeper )
 : iObserver( aObserver ), iTestCommon( aTestCommon ), iStats( aStatsKeeper )
    {

    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::~CVCXMyVideosCollectionPluginTester
// -----------------------------------------------------------------------------
//
CVCXMyVideosCollectionPluginTester::~CVCXMyVideosCollectionPluginTester( )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::~CVCXMyVideosCollectionPluginTester");

    if( iProgressTimer )
        {
        iProgressTimer->CancelTimer();
        delete iProgressTimer;
        iProgressTimer = NULL;
        }

    delete iDlWatcher;
    iDlWatcher = NULL;

    if( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }
    iCollectionUtility = NULL;
    
    delete iCollectionEntries;
    iCollectionEntries = NULL;

    if( iOldMediaArray )
        {
        iOldMediaArray->Reset();
        delete iOldMediaArray;
        iOldMediaArray = NULL;
        }

    if( iMediaArray )
        {
        iMediaArray->Reset();
        delete iMediaArray;
        iMediaArray = NULL;
        }

    iFs.Close();

    if( iActiveWait )
        {
        iActiveWait->Stop();
        delete iActiveWait;
        iActiveWait = NULL;
        }

    iRequestedMediaIds.Close();

    if( iFullMedia )
        {
        delete iFullMedia;
        iFullMedia = NULL;
        }

    iInsertedItemIds.Close();
    iDeletedItemIds.Close();

    delete iCurrentLevelName;
    iCurrentLevelName = NULL;
    
    iTransactions->CheckTransactions();
    delete iTransactions;
    iTransactions = NULL;
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::~CVCXMyVideosCollectionPluginTester");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::ConstructL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::ConstructL()
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::ConstructL");
    iAutomaticContentRefresh = ETrue;

    iActiveWait = CIptvTestActiveWait::NewL();
    User::LeaveIfError( iFs.Connect() );
    
    iTransactions = CVCXMyVideosTestTransactions::NewL();
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeIsolated );
    iDlWatcher = CVCXMyVideosTestDlWatcher::NewL( iObserver, iStats );
    iUpdateDownloads = ETrue;
    iCurrentOpenedLevelIndex = -1;
    iProgressTimer = CIptvTestTimer::NewL( *this, 0 );
    iProgressTimer->After( 1000000 );
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::ConstructL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::OpenCollectionL( TUint32 aCollectionUid )
    {
    VCXLOGLO2(">>>CVCXMyVideosCollectionPluginTester::OpenCollectionL (%d)", this);
    
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    iCollectionUid = TUid::Uid( aCollectionUid );
    path->AppendL( aCollectionUid );
    
    TRAP_IGNORE( iStats->ActionStartL( KOpenCollectionActionId, _L("Open collection") ) );
    iCurrentActionHasResponse = EFalse;
    iCurrentActionId = KOpenCollectionActionId;
    SetRefreshStatus( ETrue );
    
    iCollectionUtility->Collection().OpenL( *path );
    
    CleanupStack::PopAndDestroy( path );
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::OpenCollectionL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::OpenLevelL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::OpenLevelL( TInt aIndex )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::OpenLevelL");

    if( GetCurrentLevel() != 2 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Error. My videos collection must be open!");
        User::Leave( KErrGeneral );
        }

    if( aIndex < 0 || aIndex > iMediaArray->Count() )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Error. Index (%d) out of bounds!", aIndex);
        User::Leave( KErrGeneral );
        }

    // Save the name of level

    CMPXMedia* media(NULL);
    media = (*iMediaArray)[aIndex];

    if( media->IsSupported( KMPXMediaGeneralTitle ) )
        {
        delete iCurrentLevelName;
        iCurrentLevelName = NULL;
        iCurrentLevelName = media->ValueText( KMPXMediaGeneralTitle ).AllocL();
        }

    // Open the level
    TRAP_IGNORE( iStats->ActionStartL( KOpenCollectionLevelActionId, _L("Open level") ) );
    iCurrentActionHasResponse = EFalse;
    iCurrentActionId = KOpenCollectionLevelActionId;
    
    iCollectionUtility->Collection().OpenL( aIndex );
    
    iCurrentOpenedLevelIndex = aIndex;
    SetRefreshStatus( ETrue );
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::OpenLevelL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::RefreshContentsL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::RefreshContentsL()
    {
    //VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::RefreshContentsL");

    // No need to refresh if My Videos category is not open.
    if( GetCurrentLevel() != 3)
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: No refresh because level.");
        return;
        }

    // Wait previous refresh to complete.
    if( IsRefreshing() )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Refresh already active.");
        return;
        }

    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------");
    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Refreshing video list ----->");
    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------");

    TRAP_IGNORE( iStats->ActionStartL( KRefreshCollectionActionId, _L("Refreshing collection") ) );
    iCurrentActionHasResponse = EFalse;
    iCurrentActionId = KRefreshCollectionActionId;
    iCollectionUtility->Collection().OpenL();
    SetRefreshStatus( ETrue );

    //VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::RefreshContentsL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::GetMediasByMpxIdL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::GetMediasByMpxIdL( TInt aStartIndex, TInt aEndIndex, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::GetMediasByMpxIdL");

    if ( iCollectionUid.iUid == 0 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Error, collection must be opened!");
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::GetMediasByMpxIdL");
        User::Leave( KErrNotReady );
        }

    if( IsRefreshing() )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Refresh already active.");
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::GetMediasByMpxIdL");
        return;
        }
    
    CMPXCommand* cmd( NULL );
    
    cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosGetMediasByMpxId, aSync );    
    
    CMPXMediaArray* requestedMediaObjects = SelectMediasL( -1, aStartIndex, aEndIndex );
    CleanupStack::PushL( requestedMediaObjects );

    if( requestedMediaObjects )
        {
        cmd->SetCObjectValueL( KMPXMediaArrayContents, requestedMediaObjects );
        cmd->SetTObjectValueL( KMPXMediaArrayCount, requestedMediaObjects->Count() );
        }

    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Doing request.");
    iActionCount++;
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Get medias by MPX ID") ) );
    iCurrentActionHasResponse = ETrue;
    iCollectionUtility->Collection().CommandL( *cmd );

    SetRefreshStatus( ETrue );

    CleanupStack::PopAndDestroy( requestedMediaObjects );
    CleanupStack::PopAndDestroy( cmd );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::GetMediasByMpxIdL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::GetMediaFullDetailsL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::GetMediaFullDetailsL( TInt aDrive, TInt aIndex )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::GetMediaFullDetailsL");

    if( GetCurrentLevel() < 2 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Error. My videos category must be open!");
        User::Leave( KErrGeneral );
        }

    TInt realIndex = GetMediaIndexInCollectionL( aDrive, aIndex );

    RArray<TMPXAttribute> attrs;
    CleanupClosePushL(attrs);
    // Empty attributes to get all the details.
    CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( path );
    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Selecting %d", realIndex);
    path->SelectL( realIndex );
    TRAP_IGNORE( iStats->ActionStartL( KGetMediaFullDetailsActionId, _L("Get media full details") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
    CleanupStack::PopAndDestroy( path );
    CleanupStack::PopAndDestroy( &attrs );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::GetMediaFullDetailsL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::GetMediaFullDetailsByMpxIdL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::GetMediaFullDetailsByMpxIdL( TInt aDrive, TInt aIndex, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::GetMediaFullDetailsByMpxIdL");

    if( GetCurrentLevel() < 2 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Error. My videos category must be open!");
        User::Leave( KErrGeneral );
        }

    TInt realIndex = GetMediaIndexInCollectionL( aDrive, aIndex );
    CMPXMedia* media = media = (*iCollectionMediaArray)[realIndex];
    TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
    
    CMPXCommand* cmd( NULL );

    cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosGetMediaFullDetailsByMpxId, aSync );    
    cmd->SetTObjectValueL( KMPXMediaGeneralId, itemId );    
    TRAP_IGNORE( iStats->ActionStartL( KGetMediaFullDetailsActionId, _L("Get media full details by MPX ID") ) );
    iCollectionUtility->Collection().CommandL( *cmd );
    
    iCurrentActionHasResponse = EFalse;
    
    CleanupStack::PopAndDestroy( cmd );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::GetMediaFullDetailsByMpxIdL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::CloseLevelL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::CloseLevelL( )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::CloseLevelL");

    if( GetCurrentLevel() <= 1 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Error. Cannot go back from root level!");
        User::Leave( KErrGeneral );
        }

    iCurrentOpenedLevelIndex = -1;

    delete iCurrentLevelName;
    iCurrentLevelName = NULL;

    iCollectionUtility->Collection().BackL();
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::CloseLevelL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::ProcessCurrentEntriesL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::ProcessCurrentEntriesL()
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::ProcessCurrentEntriesL");

    if( GetCurrentLevel() > 0 )
        {
        if( iMediaArray )
            {
            if( !iQuietMode )
                {
                if( !iCurrentLevelName )
                    {
                    PrintMediasL( iMediaArray, ETrue, _L("Collection") );
                    }
                else
                    {
                    PrintMediasL( iMediaArray, ETrue, *iCurrentLevelName );
                    }
                }

            // Update download states and info
            if( GetCurrentLevel() == 3 && iUpdateDownloads && iDownloadsStarted )
                {
                UpdateDownloadsL( iQuietMode );
                }
            }
        else
            {
            VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester:: iMediaArray is NULL");
            }
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::ProcessCurrentEntriesL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::UpdateDownloadsL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::UpdateDownloadsL( TBool aQuietMode )
    {
    if( !iMediaArray || !iDlWatcher )
        {
        return;
        }

    if( !aQuietMode )
        VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::UpdateDownloadsL ---------->");

    iDlWatcher->PrepareCheck();

    // Print short info about downloads
    CMPXMedia* media( NULL );

    for( TInt i = 0; i < iMediaArray->Count(); i++ )
        {
        media = (*iMediaArray)[i];

        // If it's download then get info about it and update download watcher.
        if( media->IsSupported( KVcxMediaMyVideosDownloadState ) )
            {
            TInt state = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );

            TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));

            HBufC* titleBuff = NULL;
            HBufC* urlBuff = NULL;
            HBufC* pathBuff = NULL;

            if( media->IsSupported( KMPXMediaGeneralUri ) )
                {
                const TDesC& url = media->ValueText( KMPXMediaGeneralUri );
                pathBuff = url.AllocL();
                CleanupStack::PushL( pathBuff );
                }

            if( media->IsSupported( KMPXMediaGeneralTitle ) )
                {
                const TDesC& title = media->ValueText( KMPXMediaGeneralTitle );
                titleBuff = title.AllocL();
                CleanupStack::PushL( titleBuff );
                }

            if( media->IsSupported( KVcxMediaMyVideosRemoteUrl ) )
                {
                const TDesC& url = media->ValueText( KVcxMediaMyVideosRemoteUrl );
                urlBuff = url.AllocL();
                CleanupStack::PushL( urlBuff );
                }

            TInt progress = 0;
            if( media->IsSupported( KVcxMediaMyVideosDownloadProgress ) )
                {
                progress = media->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress );
                }

            TUint32 downloadId = 0;
            if( media->IsSupported( KVcxMediaMyVideosDownloadId ) )
                {
                downloadId = media->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );
                }

            TInt downloadError = 0;
            if( media->IsSupported( KVcxMediaMyVideosDownloadError ) )
                {
                downloadError = media->ValueTObjectL<TInt32>( KVcxMediaMyVideosDownloadError );
                }

            TInt globalError = 0;
            if( media->IsSupported( KVcxMediaMyVideosDownloadGlobalError ) )
                {
                globalError = media->ValueTObjectL<TInt32>( KVcxMediaMyVideosDownloadGlobalError );
                }

            CVCXMyVideosTestDownload* dl = iDlWatcher->GetDownloadByMpxId( itemId.iId1 );

            if( !dl )
                {
                // Tell dl watcher that dl has started. 
                if( state != EVcxMyVideosDlStateNone )
                    {
                    iDlWatcher->StartDownloadL( *urlBuff, *pathBuff, downloadId, itemId.iId1, *titleBuff,
                            static_cast<TVcxMyVideosDownloadState>( state ), progress );
                    }
                }
            else
                {
                // Update download. 
                    iDlWatcher->UpdateDownloadStateL( itemId.iId1, downloadId, 
                            static_cast<TVcxMyVideosDownloadState>( state ), progress, 
                            downloadError, globalError );
                    
                }
            
            if( state == EVcxMyVideosDlStatePaused && iAutoResume )
                {
                if( dl && !dl->iWaitingResume )
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: <---------- Autoresuming the paused download. ---------->");
                    ResumeDownloadL( _L("resume"), dl->iIapId, dl->iServiceId, dl->iContentId, *dl->iUrl, dl->iSyncCall, *dl->iUserName, *dl->iPassword, NULL );
                    iDlWatcher->SetDownloadResumedFlagL( itemId.iId1, downloadId );
                    dl->iWaitingPause = EFalse;
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: <---------- Resume ok ---------->");
                    }
                }

            if( urlBuff )
                {
                CleanupStack::PopAndDestroy( urlBuff );
                }
            if( titleBuff )
                {                
                CleanupStack::PopAndDestroy( titleBuff );
                }
            if( pathBuff )
                {                       
                CleanupStack::PopAndDestroy( pathBuff );
                }
            }
        }

    if( !aQuietMode )
        {
        iDlWatcher->PrintDownloads();
        }

    iDlWatcher->FinishCheckL();
    if( !aQuietMode )
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::UpdateDownloadsL <----------");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::CreateMpxCommandLC
// -----------------------------------------------------------------------------
//
CMPXCommand* CVCXMyVideosCollectionPluginTester::CreateMpxCommandLC( TInt aCommandGeneralId, TInt aMyVideosCommandId, TBool aSync )
    {
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL( KMPXCommandGeneralId, aCommandGeneralId );
    if( aCommandGeneralId == KVcxCommandIdMyVideos )
        {
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, aMyVideosCommandId );
        }

    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, aSync );

    if( !aSync )
        {
        // Transaction id is used also tracking stats, so increment for every command.
        iTransactions->NextTransactionId();

        if( aCommandGeneralId == KVcxCommandIdMyVideos )
            {
            cmd->SetTObjectValueL( KVcxMediaMyVideosTransactionId, iTransactions->TransactionId() );
            iTransactions->AddTransactionId( aMyVideosCommandId );
            }
        }

    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, iCollectionUid.iUid );

    return cmd;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::SelectMediasL
// -----------------------------------------------------------------------------
//
CMPXMediaArray* CVCXMyVideosCollectionPluginTester::SelectMediasL( TInt aDriveFilter, TInt aStartIndex, TInt aEndIndex )
    {
    if( aStartIndex == -4 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester::SelectMediasL - returning NULL");
        return NULL;
        }

    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::SelectMediasL");
    VCXLOGLO2("CVCXMyVideosCollectionPluginTester::SelectMediasL: aStartIndex: %d", aStartIndex);

    if( !iMediaArray )
        {
        User::Leave( KErrNotReady );
        }

    iRequestedMediaIds.Reset();

    CMPXMediaArray* medias = CMPXMediaArray::NewL();

    // None
    if( aStartIndex > aEndIndex )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Selected none of the videos.");
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::SelectMediasL");
        return medias;
        }

    CleanupStack::PushL( medias );

    if( aStartIndex >= 0 )
        {
        if( aEndIndex > iMediaArray->Count() )
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTester::SelectMediasL end index: %d is out of bounds.", aEndIndex);
            User::Leave( KErrArgument );
            }

        TInt countToSelect = aEndIndex - aStartIndex;
        TInt indexOfVideoOnDrive = 0;
        
        for( TInt i = 0; i < iMediaArray->Count(); i++ )
            {
            CMPXMedia* media = (*iMediaArray)[i];

            if( media )
                {
                TBool selectThis( EFalse );
                
                if( aDriveFilter != -1 )
                    {
                    // Drive specified, check path and index of video on the drive.
                    const TDesC& localFilePath = media->ValueText( KMPXMediaGeneralUri );
                    TInt drive( 0 );
                    User::LeaveIfError( iFs.CharToDrive( localFilePath[0], drive ) );

                    if( drive == aDriveFilter )
                        {
                        if( indexOfVideoOnDrive >= aStartIndex && indexOfVideoOnDrive < aEndIndex )
                            {
                            selectThis = ETrue;
                            }
                        indexOfVideoOnDrive++;                        
                        }
                    }
                else
                    {
                    // No drive specified, just index check.
                    if( i >= aStartIndex && i < aEndIndex )
                        {
                        selectThis = ETrue;
                        }
                    }

                if( selectThis )
                    {
                    TMPXItemId mpxId = *(*media).Value<TMPXItemId>( KMPXMediaGeneralId );
                    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Selected media. MPX ID: %d", mpxId.iId1);
                    iRequestedMediaIds.Append( mpxId.iId1 );
                    CMPXMedia* newMedia = CMPXMedia::NewL();
                    CleanupStack::PushL( newMedia );
                    newMedia->SetTObjectValueL( KMPXMessageMediaGeneralId, mpxId );
                    newMedia->SetTObjectValueL( KMPXMediaGeneralId, mpxId );
                    medias->AppendL( *newMedia );
                    CleanupStack::PopAndDestroy( newMedia );
    
                    if( medias->Count() >= countToSelect )
                        {
                        break;
                        }
                    }
                }
            }
        }

    // Invalid IDs
    if( aStartIndex == -1 )
        {
        for( TInt i = 0; i < aEndIndex; i++ )
            {
            TMPXItemId mpxId;
            mpxId.iId1 = i+66666;
            mpxId.iId2 = 0;

            VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Selected media. Invalid MPX ID: %d", mpxId.iId1);

            CMPXMedia* newMedia = CMPXMedia::NewL();
            CleanupStack::PushL( newMedia );
            newMedia->SetTObjectValueL( KMPXMessageMediaGeneralId, mpxId );
            newMedia->SetTObjectValueL( KMPXMediaGeneralId, mpxId );
            medias->AppendL( *newMedia );
            CleanupStack::PopAndDestroy( newMedia );
            }
        }

    // Duplicate IDs
    if( aStartIndex == -2 )
        {
        if( aEndIndex > iMediaArray->Count() )
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTester::SelectMediasL end index: %d is out of bounds.", aEndIndex);
            User::Leave( KErrArgument );
            }

        for( TInt i = 0; i < aEndIndex; i++ )
            {
            CMPXMedia* media = (*iMediaArray)[i];

            if( media )
                {
                TMPXItemId mpxId = *(*media).Value<TMPXItemId>( KMPXMediaGeneralId );

                VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Selected media. Add twice MPX ID: %d", mpxId.iId1);
                iRequestedMediaIds.Append( mpxId.iId1 );
                iRequestedMediaIds.Append( mpxId.iId1 );
                CMPXMedia* newMedia = CMPXMedia::NewL();
                CleanupStack::PushL( newMedia );
                newMedia->SetTObjectValueL( KMPXMessageMediaGeneralId, mpxId );
                newMedia->SetTObjectValueL( KMPXMediaGeneralId, mpxId );
                medias->AppendL( *newMedia );
                medias->AppendL( *newMedia );
                CleanupStack::PopAndDestroy( newMedia );
                }
            }
        }

    // Every second ID
    if( aStartIndex == -3 )
        {
        if( aEndIndex > iMediaArray->Count() )
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTester::SelectMediasL end index: %d is out of bounds.", aEndIndex);
            User::Leave( KErrArgument );
            }

        for( TInt i = 0; i < aEndIndex; i+=2 )
            {
            CMPXMedia* media = (*iMediaArray)[i];

            if( media )
                {
                TMPXItemId mpxId = *(*media).Value<TMPXItemId>( KMPXMediaGeneralId );

                VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Selected media. MPX ID: %d", mpxId.iId1);
                iRequestedMediaIds.Append( mpxId.iId1 );
                CMPXMedia* newMedia = CMPXMedia::NewL();
                CleanupStack::PushL( newMedia );
                newMedia->SetTObjectValueL( KMPXMessageMediaGeneralId, mpxId );
                newMedia->SetTObjectValueL( KMPXMediaGeneralId, mpxId );
                medias->AppendL( *newMedia );
                CleanupStack::PopAndDestroy( newMedia );
                }
            }
        }

    CleanupStack::Pop( medias );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::SelectMediasL");
    return medias;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::UpdateOwnedMediaArray
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::UpdateOwnedMediaArrayL()
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::UpdateOwnedMediaArrayL");

    if ( !iCollectionEntries )
        {
        return;
        }

    if( !iCollectionEntries->IsSupported( KMPXMediaArrayContents ) )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: No support for KMPXMediaArrayContents, cannot continue!");
        User::Leave( KErrNotSupported );
        }

    // Get up to date list of all medias in collection.
    CMPXMediaArray* medias = iCollectionEntries->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    
    // Delete array of old medias
    if( iOldMediaArray )
        {
        iOldMediaArray->Reset();
        delete iOldMediaArray;
        }

    // Update the old array and create new
    iOldMediaArray = iMediaArray;
    iMediaArray = CMPXMediaArray::NewL();

    TBool categories( EFalse );
    
    // Make copies of the medias.
    for( TInt i=0; i<medias->Count(); i++ )
        {
        CMPXMedia* media = (*medias)[i];
        
        TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
        if( itemId.iId2 != 0 )
             {
             categories = ETrue;
             }
        
        // Ignore medias on ROM.
        TBool isOnRom( EFalse );
        
        if( media->IsSupported( KMPXMediaGeneralUri ) )
            {
            const TDesC& url = media->ValueText( KMPXMediaGeneralUri );
            if( url.FindC( _L("z:" ) ) != KErrNotFound )
                {
                isOnRom = ETrue;
                }
            }
        
        if( !isOnRom )
            {
            iMediaArray->AppendL( *media );
            }
        }
    
    iMediaCount = iMediaArray->Count();
    
    // Update count of videos on ROM.
    if( !categories )
        {
        iVideosOnRomCount = 0;

        for( TInt i=0; i<medias->Count(); i++ )
            {
            CMPXMedia* media = (*medias)[i];            
            
            if( media->IsSupported( KMPXMediaGeneralUri ) )
                {
                const TDesC& url = media->ValueText( KMPXMediaGeneralUri );
                if( url.FindC( _L("z:" ) ) != KErrNotFound )
                    {
                    iVideosOnRomCount++;
                    }
                }
            }

            VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: iVideosOnRom: %d", iVideosOnRomCount);
        }
    
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::UpdateOwnedMediaArrayL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetChangedMedias
// -----------------------------------------------------------------------------
//
CMPXMediaArray* CVCXMyVideosCollectionPluginTester::GetChangedMedias( const CMPXMediaArray* aNewMedias )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::GetChangedMedias");

    CMPXMediaArray* changedMedias = CMPXMediaArray::NewL();

    if( !aNewMedias )
        {
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::GetChangedMedias");
        return changedMedias;
        }

    CleanupStack::PushL( changedMedias );

    for( TInt i=0; i<aNewMedias->Count(); i++ )
        {
        CMPXMedia* media(NULL);
        media = (*aNewMedias)[i];
        
        if( !media )
            {
            continue;
            }
        
        TMPXItemId mpxId = *(*media).Value<TMPXItemId>( KMPXMediaGeneralId );

        CMPXMedia* oldMedia = GetMediaByMpxId( iOldMediaArray, mpxId );

        TBool add( EFalse );

        // It's a new.
        if( !oldMedia )
            {
            add = ETrue;
            }
        // Check changes
        else
            {
            const TArray<TMPXAttribute> newAttributes = media->Attributes();
            const TArray<TMPXAttribute> oldAttributes = oldMedia->Attributes();
            if( newAttributes.Count() != oldAttributes.Count() )
                {
                add = ETrue;
                }
            else
                {
                for( TInt e=0; e<newAttributes.Count(); e++ )
                    {
                    if( !oldMedia->Match( *media, newAttributes[e] ) )
                        {
                        add = ETrue;
                        break;
                        }
                    }
                }
            }

        // Add a copy of the media.
        if( add )
            {
            changedMedias->AppendL( *media );
            }
        }

    CleanupStack::Pop( changedMedias );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::GetChangedMedias");
    return changedMedias;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetMediaByMpxId
// -----------------------------------------------------------------------------
//
CMPXMedia* CVCXMyVideosCollectionPluginTester::GetMediaByMpxId( CMPXMediaArray* aMedias, TMPXItemId& aMpxId )
    {
    if( !aMedias )
        {
        return NULL;
        }

    for( TInt i=0; i<aMedias->Count(); i++ )
        {
        CMPXMedia* media = (*aMedias)[i];
        TMPXItemId mpxId = *(*media).Value<TMPXItemId>( KMPXMediaGeneralId );

        if( mpxId == aMpxId )
            {
            return media;
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::SetRefreshStatus
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::SetRefreshStatus( TBool aRefreshingCollection )
    {
    iRefreshingCollection = aRefreshingCollection;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::CheckRequestMediaArrayL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::CheckRequestMediaArrayL( CMPXMediaArray& aRequestResultMedias, RArray<TInt32>& aRequestedMediaIds )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::CheckRequestMediaArrayL");

    // Print requested ids
    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Requested IDs:");
    for( TInt i = 0; i < aRequestedMediaIds.Count(); i++ )
        {
        VCXLOGLO2("Requested: iId1: %d", aRequestedMediaIds[i] );
        }

    // Print actual result ids and check for errors
    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Request result media IDs:");
    for( TInt i = 0; i < aRequestResultMedias.Count(); i++ )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: at index: %d:", i);
        CMPXMedia* media = aRequestResultMedias[i];
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: got media");
        if( media )
            {
            TMPXItemId mpxId = *(*media).Value<TMPXItemId>( KMPXMediaGeneralId );
            VCXLOGLO3("Result: iId1: %d, iId2: %d", mpxId.iId1, mpxId.iId2 );
            if( media->IsSupported( KVcxMediaMyVideosInt32Value ) )
                {
                TInt32 result = (*media).ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value );
                VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: result: %d:", result);
                if( result != KErrNone && !( result == KErrCancel && iCancelRequested ) )
                    {
                    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: ERROR in results: %d!", result);
                    User::Leave( result );
                    }
                }
            }
        else
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Result is NULL at index: %d", i);
            }
        }

    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: rest of the checks");
    
    // First check the counts match
    if( aRequestResultMedias.Count() != aRequestedMediaIds.Count() )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Did not get all the requested entries!");
        User::Leave( KErrCorrupt );
        }
    else
    // Check that all requested ids are in the results
    for( TInt i = 0; i < aRequestedMediaIds.Count(); i++ )
        {
        TMPXItemId mpxId;
        mpxId.iId1 = aRequestedMediaIds[i];
        mpxId.iId2 = 0;

        TBool found( EFalse );
        for( TInt e = 0; e < aRequestResultMedias.Count(); e++ )
            {
            CMPXMedia* media2 = aRequestResultMedias[e];
            if( media2 )
                {
                TMPXItemId mpxId2 = *(*media2).Value<TMPXItemId>( KMPXMediaGeneralId );

                if( mpxId == mpxId2 )
                    {
                    found = ETrue;
                    }
                }
            }

        if( !found )
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Requested media not found from results!");
            User::Leave( KErrCorrupt );
            }
        }
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::CheckRequestMediaArrayL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::PrintMediasL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::PrintMediasL( CMPXMediaArray* aMediaArray, TBool aCheckChanges, const TDesC& aTitle )
    {
    if( aMediaArray == NULL )
        {
        aMediaArray = iMediaArray;
        }
    
    TInt count(0);
    TInt changedCount(0);

    CMPXMediaArray* medias( NULL );

    if( aCheckChanges )
        {
         medias = GetChangedMedias( aMediaArray );
         changedCount = medias->Count();
         count = aMediaArray->Count();
         CleanupStack::PushL( medias );
        }
    else
        {
        medias = aMediaArray;
        count = aMediaArray->Count();
        }

    if( count == 0 )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: <----------- %S - No Entries ----------->", &aTitle);
        }
    else if( changedCount == 0 )
        {
        VCXLOGLO3("CVCXMyVideosCollectionPluginTester:: <----------- %S - %d Entries, no changes ----------->", &aTitle, count );
        }
    else
        {
        VCXLOGLO4("CVCXMyVideosCollectionPluginTester:: <----------- %S - %d Entries, %d has changed ----------->", &aTitle, count, changedCount );
        }

    CMPXMedia* media(NULL);
    for( TInt i = 0; i < medias->Count(); i++ )
        {
        media = (*medias)[i];
        if( media )
            PrintMPXMediaL( *media, ETrue );
        }

    if( aCheckChanges )
        {
        medias->Reset();
        CleanupStack::PopAndDestroy( medias );
        }
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::PrintMPXMediaL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::PrintMPXMediaL( const CMPXMedia& aMedia, TBool aPrintAllDetails )
    {
    TMPXItemId itemId = *aMedia.Value<TMPXItemId>( KMPXMediaGeneralId );

    if( !aPrintAllDetails )
        {
        TBuf<256> title;
        if( aMedia.IsSupported( KMPXMediaGeneralTitle ) )
            {
            title = aMedia.ValueText( KMPXMediaGeneralTitle );
            }

        // Category
        if( itemId.iId2 != 0 )
            {
            TInt itemCount(0);
            TInt newItemCount(0);
            TBuf<256> newItemName;
            if( aMedia.IsSupported( KVcxMediaMyVideosCategoryNewItemName ) )
                {
                newItemName = aMedia.ValueText( KVcxMediaMyVideosCategoryNewItemName );
                }
            if( aMedia.IsSupported( KVcxMediaMyVideosCategoryItemCount ) )
                 {
                 itemCount = *aMedia.Value<TUint32>( KVcxMediaMyVideosCategoryItemCount );
                 }
            if( aMedia.IsSupported( KVcxMediaMyVideosCategoryNewItemCount ) )
                 {
                 newItemCount = *aMedia.Value<TUint32>( KVcxMediaMyVideosCategoryNewItemCount );
                 }
            VCXLOGLO5("KMPXMediaGeneralTitle: title: %S, medias: %d, new: %d, new media: %S", &title, itemCount, newItemCount, &newItemName);
            }
        // Media
        else
            {
            TBuf<256> path;
            if( aMedia.IsSupported( KMPXMediaGeneralUri ) )
                {
                const TDesC& localFilePath = aMedia.ValueText( KMPXMediaGeneralUri );
                if( localFilePath.Length() >= 256 )
                    {
                    path = localFilePath.Left( 255 );
                    }
                else
                    {
                    path = localFilePath;
                    }
                }
            TInt64 size(0);
            TInt origin(-1);
            if( aMedia.IsSupported( KMPXMediaGeneralExtSizeInt64 ) )
                 {
                 size = *aMedia.Value<TInt64>( KMPXMediaGeneralExtSizeInt64 );
                 }
            if( aMedia.IsSupported( KVcxMediaMyVideosOrigin ) )
                {
                origin = *aMedia.Value<TUint>( KVcxMediaMyVideosOrigin );
                }
            VCXLOGLO3("iId1: %d, iId2: %d", itemId.iId1, itemId.iId2 );
            VCXLOGLO5("KMPXMediaGeneralTitle: title: %S, size: %d, origin: %d, path: %S", &title, size, origin, &path);
            }

        return;
        }

    // Print all details

    if( itemId.iId2 != 0 )
        {
        VCXLOGLO1("--------------------------- MEDIA CATEGORY -------------------------------");
        }
    else
        {
        VCXLOGLO1("--------------------------- MEDIA OBJECT -------------------------------");
        }

    VCXLOGLO3("iId1: %d, iId2: %d", itemId.iId1, itemId.iId2 );

    if( aMedia.IsSupported( KVcxMediaMyVideosCategoryItemCount ) )
         {
         TUint32 value = *aMedia.Value<TUint32>( KVcxMediaMyVideosCategoryItemCount );
         VCXLOGLO2("KVcxMediaMyVideosCategoryItemCount: %d", value);
         }

    if( aMedia.IsSupported( KVcxMediaMyVideosCategoryNewItemCount ) )
         {
         TUint32 value = *aMedia.Value<TUint32>( KVcxMediaMyVideosCategoryNewItemCount );
         VCXLOGLO2("KVcxMediaMyVideosCategoryNewItemCount: %d", value);
         }

    if( aMedia.IsSupported( KVcxMediaMyVideosCategoryNewItemName ) )
        {
        const TDesC& itemName = aMedia.ValueText( KVcxMediaMyVideosCategoryNewItemName );
        VCXLOGLO2("KVcxMediaMyVideosCategoryNewItemName: %S", &itemName);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralTitle ) )
        {
        const TDesC& title = aMedia.ValueText( KMPXMediaGeneralTitle );
        VCXLOGLO2("KMPXMediaGeneralTitle: %S", &title);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralComment ) )
        {
        const TDesC& desc = aMedia.ValueText( KMPXMediaGeneralComment );
        VCXLOGLO2("KMPXMediaGeneralComment: %S", &desc);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralUri ) )
        {
        const TDesC& localFilePath = aMedia.ValueText( KMPXMediaGeneralUri );
        VCXLOGLO2("KMPXMediaGeneralUri: %S", &localFilePath);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralExtSizeInt64 ) )
         {
         TInt64 value = *aMedia.Value<TInt64>( KMPXMediaGeneralExtSizeInt64 );
         VCXLOGLO2("KMPXMediaGeneralExtSizeInt64 (bytes): %Ld", value);
         }

    if( aMedia.IsSupported( KMPXMediaGeneralDate ) )
        {
        TInt64 value = *aMedia.Value<TInt64>( KMPXMediaGeneralDate );
        TTime time ( value );
        TBuf<60> timeStr;
        _LIT(KDateTimeString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
        time.FormatL(timeStr, KDateTimeString);
        VCXLOGLO2("KMPXMediaGeneralDate: %S", &timeStr);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralFlags ) )
         {
         TUint value = *aMedia.Value<TUint>( KMPXMediaGeneralFlags );

         if( value & EVcxMyVideosVideoNew )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosVideoNew");
             }
         if( value & EVcxMyVideosVideoScheduledDownload )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosVideoScheduledDownload");
             }
         if( value & EVcxMyVideosVideoDeleteWarning )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosVideoDeleteWarning");
             }
         if( value & EVcxMyVideosVideoProtected )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosVideoProtected");
             }
         if( value & EVcxMyVideosVideoDrmProtected )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosVideoDrmProtected");
             }
         if( value & EVcxMyVideosVideoMemoryCard )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosVideoMemoryCard");
             }
         if( value & EVcxMyVideosVideoPreview )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosVideoPreview");
             }
         if( value & EVcxMyVideosVideoRecPartiallyFailed )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosVideoRecPartiallyFailed");
             }
         if( value & EVcxMyVideosVideoRecFailed )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosVideoRecFailed");
             }
         if( value & EVcxMyVideosServiceHasReadOnlyIap )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosServiceHasReadOnlyIap");
             }
         if( value & EVcxMyVideosSilent )
             {
             VCXLOGLO1("KMPXMediaGeneralFlags: EVcxMyVideosSilent");
             }
         }

    if( aMedia.IsSupported( KMPXMediaGeneralCopyright ) )
        {
        const TDesC& copyright = aMedia.ValueText( KMPXMediaGeneralCopyright );
        VCXLOGLO2("KMPXMediaGeneralCopyright: %S", &copyright);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralMimeType ) )
        {
        const TDesC& mimeType = aMedia.ValueText( KMPXMediaGeneralMimeType );
        VCXLOGLO2("KMPXMediaGeneralMimeType: %S", &mimeType);
        }

    if( aMedia.IsSupported( KVcxMediaMyVideosModifiedDate ) )
        {
        TInt64 value = *aMedia.Value<TInt64>( KVcxMediaMyVideosModifiedDate );
        TTime time ( value );
        TBuf<60> timeStr;
        _LIT(KDateTimeString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
        time.FormatL(timeStr, KDateTimeString);
        VCXLOGLO2("KVcxMediaMyVideosModifiedDate: %S", &timeStr);
        }

    /* 
          //10.
    if ( aObject.Property( *iLastModifiedDatePropertyDef, property, 0 ) != KErrNotFound
        && aFullDetails )
        {
        aVideo.SetTObjectValueL<TInt64>( KVcxMediaMyVideosModifiedDate,
                static_cast<CMdETimeProperty*>(property)->Value().Int64() );
        }
      */

    if( aMedia.IsSupported( KVcxMediaMyVideosAgeProfile ) )
        {
        TUint value = *aMedia.Value<TUint>( KVcxMediaMyVideosAgeProfile );
        VCXLOGLO2("KVcxMediaMyVideosAgeProfile: %d", value);
        }

    if( aMedia.IsSupported( KVcxMediaMyVideosAudioLanguage ) )
        {
        const TDesC& value = aMedia.ValueText( KVcxMediaMyVideosAudioLanguage );
        VCXLOGLO2("KVcxMediaMyVideosAudioLanguage: %S", &value);
        }

    if( aMedia.IsSupported( KVcxMediaMyVideosAuthor ) )
        {
        const TDesC& value = aMedia.ValueText( KVcxMediaMyVideosAuthor );
        VCXLOGLO2("KVcxMediaMyVideosAuthor: %S", &value);
        }

    if( aMedia.IsSupported( KVcxMediaMyVideosOrigin ) )
        {
        TUint8 value = *aMedia.Value<TUint>( KVcxMediaMyVideosOrigin );
        VCXLOGLO2("KVcxMediaMyVideosOrigin: %d", value);
        }

    if( aMedia.IsSupported( KVcxMediaMyVideosDuration ) )
        {
        TReal32 value = *aMedia.Value<TReal32>( KVcxMediaMyVideosDuration );
        VCXLOGLO2("KVcxMediaMyVideosDuration: %f", value);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralLastPlaybackPosition ) )
        {
        TReal32 value = *aMedia.Value<TReal32>( KMPXMediaGeneralLastPlaybackPosition );
        VCXLOGLO2("KMPXMediaGeneralLastPlaybackPosition: %f", value);
        }

    if( aMedia.IsSupported( KVcxMediaMyVideosRemoteUrl ) )
        {
        const TDesC& value = aMedia.ValueText( KVcxMediaMyVideosRemoteUrl );
        VCXLOGLO2("KVcxMediaMyVideosRemoteUrl: %S", &value);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralDrive ) )
        {
        const TDesC& drive = aMedia.ValueText( KMPXMediaGeneralDrive );
        VCXLOGLO2("KMPXMediaGeneralDrive: %S", &drive);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralCount ) )
        {
        TInt value = *aMedia.Value<TInt>( KMPXMediaGeneralCount );
        VCXLOGLO2("KMPXMediaGeneralCount: %d", value);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralLastPlaybackPosition ) )
        {
        TInt value = *aMedia.Value<TInt>( KMPXMediaGeneralLastPlaybackPosition );
        VCXLOGLO2("KMPXMediaGeneralLastPlaybackPosition: %d", value);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralLastPlaybackTime ) )
        {
        TInt64 value = *aMedia.Value<TInt64>( KMPXMediaGeneralLastPlaybackTime );
        TTime time ( value );
        TBuf<60> timeStr;
        _LIT(KDateTimeString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%H%:1%T%:2%S%.%*C2%:3%-B");
        time.FormatL(timeStr, KDateTimeString);
        VCXLOGLO2("KMPXMediaGeneralLastPlaybackTime: %S", &timeStr);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralPlayCount ) )
        {
        TUint value = *aMedia.Value<TUint>( KMPXMediaGeneralPlayCount );
        VCXLOGLO2("KMPXMediaGeneralPlayCount: %d", value);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralContainerPath ) )
        {
        const TDesC& value = aMedia.ValueText( KMPXMediaGeneralContainerPath );
        VCXLOGLO2("KMPXMediaGeneralContainerPath: %S", &value);
        }

    if( aMedia.IsSupported( KMPXMediaGeneralPath ) )
        {
        // Media collection path; CMPXCollectionPath
        const TDesC& value = aMedia.ValueText( KMPXMediaGeneralPath );
        VCXLOGLO2("KMPXMediaGeneralPath: %S", &value);
        }
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::HandleCollectionMessage
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::HandleCollectionMessage");
    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: aError: %d", aError);

    if( !aMessage )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: aMessage is NULL");
        aError = KErrGeneral;
        }

    if( aError == KErrNone )
        {
        TInt err( KErrNone );

        TMPXMessageId messageId = *(aMessage->Value<TMPXMessageId>( KMPXMessageGeneralId ));
        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Message id: %d", messageId);

        TInt myVideosCmd = -1;
        if ( messageId == KVcxCommandIdMyVideos )
            {
            myVideosCmd = aMessage->ValueTObjectL<TInt>( KVcxMediaMyVideosCommandId );
            }

        if ( myVideosCmd != KVcxMessageMyVideosMessageArray )
            {
            TRAP( err, HandleSingleCollectionMessageL( aMessage ) );
            }
        else
            {
            if( err == KErrNone && aMessage->IsSupported( KMPXMessageArrayContents ) )
                {
                const CMPXMessageArray* messageArray =
                        aMessage->Value<CMPXMessageArray>(KMPXMessageArrayContents);

                TInt count = messageArray->Count();
                VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Message has array of %d messages.", count);

                for( TInt i = 0; i < count; i++ )
                    {
                    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Handling message at index: %d", i);
                    TRAP( err, HandleSingleCollectionMessageL( (*messageArray)[i] ) );
                    }
                }
            else
                {
                VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: MY VIDEOS MESSAGE ARRAY WITHOUT ARRAY ARRIVED");
                err = KErrCorrupt;
                }
            }

        if( err != KErrNone )
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Error %d while handling messages!", err );
            aError = err;
            }
        }

    if( aError != KErrNone )
        {
        SetRefreshStatus( EFalse );
        iWaitingForItemChange = EFalse;
        iActiveWait->Stop();
        TRAP_IGNORE( iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionMessageError, aError ) );
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::HandleCollectionMessage");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::HandleSingleCollectionMessage
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::HandleSingleCollectionMessageL( CMPXMessage* aMessage )
    {
    if( !aMessage || !aMessage->IsSupported( KMPXMessageGeneralId ) ) return;
    
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::HandleSingleCollectionMessage");    
    
    TMPXMessageId messageId = *(aMessage->Value<TMPXMessageId>( KMPXMessageGeneralId ));

    if( aMessage->IsSupported( KMPXMessageCollectionId ) )
        {
        TUid uid( *(aMessage->Value<TUid>( KMPXMessageCollectionId )) );
        if( iCollectionUid != uid )
            {
            VCXLOGLO3("CVCXMyVideosCollectionPluginTester:: Collection UIDs don't match: iCollectionUid %d, uid %d", iCollectionUid.iUid, uid.iUid);
            VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::HandleSingleCollectionMessage");
            return;
            }
        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Collection UID: %d", uid.iUid);
        }

    TUint32 transactionId( 0 );
    if( aMessage->IsSupported( KVcxMediaMyVideosTransactionId ) )
        {
        transactionId = aMessage->ValueTObjectL<TUint32>( KVcxMediaMyVideosTransactionId );
        iTransactions->TransactionResponse( transactionId );
        }

    switch( messageId )
        {
        // Handle My Videos specific command results.
        case KVcxCommandIdMyVideos:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: -------------------------------------------");
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KMPXMessageGeneralId: KVcxCommandIdMyVideos");
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: -------------------------------------------");
            if ( aMessage->IsSupported( KVcxMediaMyVideosCommandId ) )
                {
                TInt myVideosCommandId = *(aMessage->Value<TInt>( KVcxMediaMyVideosCommandId ));

                switch ( myVideosCommandId )
                    {
                    case KVcxMessageMyVideosGetMediasByMpxIdResp:
                        {
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: -------------------------------------------------------------------");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KVcxMediaMyVideosCommandId: KVcxMessageMyVideosGetMediasByMpxIdResp");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: -------------------------------------------------------------------");

                        if( iActionCount <= 0 )
                            {
                            break;
                            }
                        iActionCount--;

                       // Update current entries
                        delete iCollectionEntries;
                        iCollectionEntries = NULL;
                        iCollectionEntries = CMPXMedia::NewL( *aMessage );

                        UpdateOwnedMediaArrayL();

                        SetRefreshStatus( EFalse );

                        // add downloads to the requested ID array always and update downloads too.
                        TBool backup = iUpdateDownloads;
                        iUpdateDownloads = EFalse;
                        ProcessCurrentEntriesL();
                        iUpdateDownloads = backup;

                        // Check
                        TRAPD( err, CheckRequestMediaArrayL( *iMediaArray, iRequestedMediaIds ) );
                        if( err != KErrNone )
                            {
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionMessageError, err );
                            }

                        // Inform observer
                        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageGotMediasByKMpxId, KErrNone );
                        
                        TRAPD( actionErr, iStats->ActionEndL( transactionId, err ) );
                        if( actionErr == KErrAbort )
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                        
                        }
                        break;

                    // This is received when list is open already and the list contains new items
                    case KVcxMessageMyVideosItemsAppended:
                        {
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ------------------------------------------------------------");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KVcxMediaMyVideosCommandId: KVcxMessageMyVideosItemsAppended");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ------------------------------------------------------------");

                        if( !iCollectionEntries )
                            {
                            break;
                            }

                        TInt before = iMediaCount;

                        UpdateOwnedMediaArrayL();

                        // New items were appended or removed
                        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Items before: %d", before);
                        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Items now: %d", iMediaArray->Count());

                        ProcessCurrentEntriesL();
                        }
                        break;

                    /**
                    * Collection sends this message when media list fetching has ended
                    * and no more KVcxMessageMyVideosItemsAppended will be sent.
                    * If client has fetched items with KVcxCommandMyVideosGetMediasByMpxId,
                    * it should refetch items when this event arrives.
                    */
                    case KVcxMessageMyVideosListComplete:
                        {
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------------------------------");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KVcxMediaMyVideosCommandId: KVcxMessageMyVideosListComplete");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------------------------------");
                        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageListComplete, KErrNone );
                        }
                        break;

                    case KVcxMessageMyVideosMoveOrCopyStarted:
                        {
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------------------------------");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KVcxMediaMyVideosCommandId: KVcxMessageMyVideosMoveOrCopyStarted");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------------------------------");

                        if( iActionCount <= 0 )
                            {
                            break;
                            }
                        iActionCount--;

                        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCopyOrMoveStarted, KErrNone );
                        
                        TRAP_IGNORE( iStats->ActionProgressL( transactionId, _L("Move/Copy started.") ) );
                        }
                        break;

                    case KVcxMessageMyVideosMoveResp:
                        {
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: -------------------------------------------------------");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KVcxMediaMyVideosCommandId: KVcxMessageMyVideosMoveResp");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: -------------------------------------------------------");

                        if( iActionCount <= 0 )
                            {
                            break;
                            }
                        iActionCount--;

                        CMPXMedia* medias = CMPXMedia::NewL( *aMessage );
                        CleanupStack::PushL( medias );
                        CMPXMediaArray* results = medias->Value<CMPXMediaArray>( KMPXMediaArrayContents );

                        TRAPD( err, CheckRequestMediaArrayL( *results, iRequestedMediaIds ) );

                        if( err != KErrNone )
                            {
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionMessageError, err );
                            }
                        else
                            {
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageMoveResp, KErrNone );
                            }
                        
                        if( iCancelRequested )
                            {
                            TRAP_IGNORE( iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCommandCanceled, KErrNone ) );
                            iCancelRequested = EFalse;
                            }                        

                        TRAPD( actionErr, iStats->ActionEndL( transactionId, err ) );
                        if( actionErr == KErrAbort )
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                        
                        CleanupStack::PopAndDestroy( medias );
                        }
                        break;

                    case KVcxMessageMyVideosCopyResp:
                        {
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: -------------------------------------------------------");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KVcxMediaMyVideosCommandId: KVcxMessageMyVideosCopyResp");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: -------------------------------------------------------");

                        if( iActionCount <= 0 )
                            {
                            break;
                            }
                        iActionCount--;

                        CMPXMedia* medias = CMPXMedia::NewL( *aMessage );
                        CleanupStack::PushL( medias );
                        CMPXMediaArray* results = medias->Value<CMPXMediaArray>( KMPXMediaArrayContents );

                        TRAPD( err, CheckRequestMediaArrayL( *results, iRequestedMediaIds ) );
                        if( err != KErrNone )
                            {
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionMessageError, err );
                            }
                        else
                            {
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCopyResp, KErrNone );
                            }

                        if( iCancelRequested )
                            {
                            TRAP_IGNORE( iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCommandCanceled, KErrNone ) );
                            iCancelRequested = EFalse;
                            }                        
                        
                        TRAPD( actionErr, iStats->ActionEndL( transactionId, err ) );
                        if( actionErr == KErrAbort )
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                        
                        CleanupStack::PopAndDestroy( medias );
                        }
                        break;

                    case KVcxMessageMyVideosDeleteStarted:
                        {
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------------------------------");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KVcxMediaMyVideosCommandId: KVcxMessageMyVideosDeleteStarted");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------------------------------");

                        if( iActionCount <= 0 )
                            {
                            break;
                            }
                        iActionCount--;

                        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDeleteStarted, KErrNone );
                        
                        TRAP_IGNORE( iStats->ActionProgressL( transactionId, _L("Delete started.") ) );
                        }
                        break;

                    case KVcxMessageMyVideosDeleteResp:
                        {
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------------------------------");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KVcxMediaMyVideosCommandId: KVcxMessageMyVideosDeleteResp");
                        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------------------------------");

                        if( iActionCount <= 0 )
                            {
                            break;
                            }
                        iActionCount--;

                        CMPXMedia* medias = CMPXMedia::NewL( *aMessage );
                        CleanupStack::PushL( medias );
                        CMPXMediaArray* results = medias->Value<CMPXMediaArray>( KMPXMediaArrayContents );

                        TRAPD( err, CheckRequestMediaArrayL( *results, iRequestedMediaIds ) );

                        if( err != KErrNone )
                            {
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionMessageError, err );
                            }
                        else
                            {
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageDeleteResp, KErrNone );
                            }

                        if( iCancelRequested )
                            {
                            TRAP_IGNORE( iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCommandCanceled, KErrNone ) );
                            iCancelRequested = EFalse;
                            }
                        
                        TRAPD( actionErr, iStats->ActionEndL( transactionId, err ) );
                        if( actionErr == KErrAbort )
                            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                        
                        CleanupStack::PopAndDestroy( medias );
                        }
                        break;

                    default:
                        {
                        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: ERROR. message %d has no handler!", myVideosCommandId );
                        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionMessageError, KErrCorrupt );
                        }
                        break;
                    }
                }
            else
                {
                VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ERROR: KVcxCommandIdMyVideos NOT SUPPORTED!");
                iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionMessageError, KErrCorrupt );
                }
            }
            break;

        // Handle general collection messages
        case KMPXMessageGeneral:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------");
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KMPXMessageGeneralId: KMPXMessageGeneral");
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------");

            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, KErrNone );

            TInt event( *(aMessage->Value<TInt>( KMPXMessageGeneralEvent )) );
            TInt type( *(aMessage->Value<TInt>( KMPXMessageGeneralType )) );
            TInt data( *(aMessage->Value<TInt>( KMPXMessageGeneralData )) );

            VCXLOGLO4("CVCXMyVideosCollectionPluginTester:: event: %d, type: %d, data: %d", event, type, data);

            // Next message will be EPathChanged, nothing to do here.
            if ( event == TMPXCollectionMessage::ECollectionChanged )
                {
                VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: event ECollectionChanged.");
                }

            if ( event == TMPXCollectionMessage::EPathChanged )
                {
                VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: event EPathChanged");

                // Collection or level changed, get entries for it.
                if( data == EMcContainerOpened )
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: data EMcContainerOpened");
                    TRAP_IGNORE( iCollectionUtility->Collection().OpenL() );
                    }
                else if ( data == EMcItemOpened )
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: data EMcItemOpened");
                    }
                }
            }
            break;

        case KMPXMessageIdItemChanged:
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------------");
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: KMPXMessageGeneralId: KMPXMessageIdItemChanged");
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: ----------------------------------------------");

            TMPXItemId itemId = *aMessage->Value<TMPXItemId>(KMPXMessageMediaGeneralId);
            VCXLOGLO3("CVCXMyVideosCollectionPluginTester:: Item Id1: %d, Id2: %d", itemId.iId1, itemId.iId2);

#if 1
            // Check the IDs for categories
            if( itemId.iId2 != 0 && ( itemId.iId1 != KVcxMvcCategoryIdAll &&
                    itemId.iId1 != KVcxMvcCategoryIdDownloads &&
                    itemId.iId1 != KVcxMvcCategoryIdTvRecordings &&
                    itemId.iId1 != KVcxMvcCategoryIdCaptured &&
                    itemId.iId1 != KVcxMvcCategoryIdOther ) )
                {
                iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionCategoryChanged, KErrCorrupt );
                VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Error. INVALID ITEM ID.");
                break;
                }
#endif
            // If event if for category, skip it if it's not for the open category.
            if( iCurrentOpenedLevelIndex != -1 && itemId.iId2 > 0 && itemId.iId1 != iCurrentOpenedLevelIndex )
                {
                VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Not for open category. Skip.");
                break;
                }

            if( iWaitingForItemChange && itemId.iId2 == 0 )
                {
                VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Stop wait for item change");
                iWaitingForItemChange = EFalse;
                iActiveWait->Stop();
                }

            TBool refreshNeeded( EFalse );

            TMPXChangeEventType changeEvent( *aMessage->Value<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
            switch( changeEvent )
                {
                case EMPXItemInserted:
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: changeEvent EMPXItemInserted");
                    iInsertedItemIds.Append( itemId.iId1 );

                    refreshNeeded = ETrue;
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageMpxItemInserted, KErrNone );
                    
                    TRAPD( actionErr, iStats->ActionEndL( KSideloadVideoActionId, KErrNone ) );
                    if( actionErr == KErrAbort )
                        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                    
                    }
                    break;

                case EMPXItemDeleted:
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: changeEvent EMPXItemDeleted");
                    iDeletedItemIds.Append( itemId.iId1 );
                    refreshNeeded = ETrue;
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageMpxItemDeleted, KErrNone );
                    
                    TRAPD( actionErr, iStats->ActionEndL( KRemoveMediaActionId, KErrNone ) );
                    if( actionErr == KErrAbort )
                        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                    }
                    break;

                case EMPXItemModified:
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: changeEvent EMPXItemModified");
                    refreshNeeded = EFalse;
                    if ( aMessage->IsSupported( KVcxMediaMyVideosInt32Value ) )
                        {
                        TInt32 extraInfo = aMessage->ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value );
                        if ( extraInfo == EVcxMyVideosVideoListOrderChanged )
                            {
                            VCXLOGLO1("CVcxMyVideosCollectionTester:: Received EVcxMyVideosVideoListOrderChanged ------->");
                            iObserver->HandleVcxMvTesterMessageL( KVCXMyVideosTestMessageVideoListOrderChanged, KErrNone );
                            refreshNeeded = ETrue;
                            }
                        }
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageMpxItemModified, KErrNone );
                    }
                    break;

                default:
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: changeEvent UNKNOWN!");
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionMessageError, KErrCorrupt );
                    }
                    break;
                }

            if( itemId.iId2 == 0 )
                {
                iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionItemChanged, KErrNone );
                }
            else
                {
                iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionCategoryChanged, KErrNone );
                }

            // Refresh if there's need and it's possible.
            if( refreshNeeded  )
                {
                if( IsRefreshing() )
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Refresh already active.");
                    refreshNeeded = EFalse;
                    }

                // No automatic refresh
                if( !iAutomaticContentRefresh )
                     {
                     VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Auto refresh disabled");
                     refreshNeeded = EFalse;
                     }

                // No need to refresh if My Videos category is not open.
                if( GetCurrentLevel() != 3 )
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: No refresh because level");
                    refreshNeeded = EFalse;
                    }

                if( refreshNeeded )
                    {
                    TRAP_IGNORE( RefreshContentsL() );
                    }
                }
            else
                {
                UpdateOwnedMediaArrayL();
                ProcessCurrentEntriesL();
                }
            }
            break;

        default:
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Unknown collection message, id: %d!", messageId);
            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionMessageError, KErrCorrupt );
            }
            break;
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::HandleSingleCollectionMessage");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::HandleOpenL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::HandleOpenL( const CMPXMedia& aEntries,
                                                TInt aIndex,
                                                TBool aComplete,
                                                TInt aError )
    {
    VCXLOGLO2(">>>CVCXMyVideosCollectionPluginTester::HandleOpenL (%d)", this);
    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: aIndex: %d", aIndex);
    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: aComplete: %d", aComplete);
    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: aError: %d", aError);
    
    SetRefreshStatus( EFalse );

    TRAPD( actionErr, iStats->ActionEndL( iCurrentActionId, aError ) );
    if( actionErr == KErrAbort )
        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
    
    iCurrentActionId = -1;
    
    if( aError == KErrNone )
        {
        // Update current entries
        delete iCollectionEntries;
        iCollectionEntries = NULL;
        iCollectionEntries = CMPXMedia::NewL( aEntries );
        
        iCollectionMediaArray = iCollectionEntries->Value<CMPXMediaArray>( KMPXMediaArrayContents );        

        UpdateOwnedMediaArrayL();

        // There could have been changes to the content during update.
        // Check that entries are up to date.

        TBool reRefreshNeeded( EFalse );

        TInt foundInsertedItemCount(0);

        for( TInt e=0; e<iMediaCount; e++ )
            {
            CMPXMedia* media = (*iMediaArray)[e];
            TMPXItemId itemId = *(*media).Value<TMPXItemId>( KMPXMediaGeneralId );

            // Check that deleted items are not on the list.
            for( TInt i=0; i<iDeletedItemIds.Count(); i++ )
                {
                if( itemId.iId1 == iDeletedItemIds[i] )
                    {
                    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Current entries contain deleted  item. Refresh needed. ------->");
                    reRefreshNeeded = ETrue;
                    break;
                    }
                }

            // Check that all inserted items are on the list.
            for( TInt i=0; i<iInsertedItemIds.Count(); i++ )
                {
                if( itemId.iId1 == iInsertedItemIds[i] )
                    {
                    foundInsertedItemCount++;
                    }
                }
            }

        iInsertedItemIds.Reset();
        iDeletedItemIds.Reset();

        if( !reRefreshNeeded && foundInsertedItemCount < iInsertedItemIds.Count() )
            {
            VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Current entries does not contain all inserted items. Rerefresh needed. ------->");
            reRefreshNeeded = ETrue;
            }

        if( reRefreshNeeded )
            {
            TRAP_IGNORE( RefreshContentsL() );
            }
        else
            {
            // Print current entries and update downloads.
            ProcessCurrentEntriesL();

            // Inform observer
            iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionOpened, aError );
            }
        }

    if( aError != KErrNone )
        {
        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionOpened, aError );
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::HandleOpenL (entries)");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::HandleOpenL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::HandleOpenL( const CMPXCollectionPlaylist& /*aPlaylist*/,
                                                TInt aError )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::HandleOpenL (playlist)");
    // Inform observer
    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessagePlaylistOpened, aError );
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::HandleOpenL (playlist)");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::HandleCollectionMediaL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::HandleCollectionMediaL(const CMPXMedia& aMedia,
                                        TInt aError)
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::HandleCollectionMediaL");
    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: aError: %d", aError);

    TRAPD( actionErr, iStats->ActionEndL( KGetMediaFullDetailsActionId, aError ) );
    if( actionErr == KErrAbort )
        iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );

    if( iGettingFullDetailsForAllMedia )
        {
        iActiveWait->Stop();
        }

    delete iFullMedia;
    iFullMedia = NULL;
    iFullMedia = aMedia.CopyL( aMedia );

    // Inform observer
    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionMedia, aError );
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::HandleCollectionMediaL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::HandleCommandComplete
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::HandleCommandComplete( CMPXCommand* aCommandResult,
                                                                TInt aError )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::HandleCommandComplete");
    VCXLOGLO2("CVCXMyVideosCollectionPluginTester::HandleCommandComplete: aError: %d", aError);
    
    if( iCancelRequested && aError == KErrCancel )
        {
        aError = KErrNone;
        }
    
    if( !iObserver )
        {
        VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::HandleCommandComplete");
        return;
        }
    
    if( aError == KErrNone )
        {
        TUint32 transactionId( 0 );
        if( aCommandResult->IsSupported( KVcxMediaMyVideosTransactionId ) )
            {
            transactionId = *aCommandResult->Value<TUint32>( KVcxMediaMyVideosTransactionId );
            iTransactions->TransactionResponse( transactionId );
            if( !iCurrentActionHasResponse )
                {
                TRAPD( actionErr, iStats->ActionEndL( transactionId, aError ) );
                if( actionErr == KErrAbort )
                    iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCollectionGeneral, actionErr );
                }
            }
        
        if( aCommandResult->IsSupported( KMPXCommandGeneralId ) )
            {
            TInt cmd = 0;
            cmd = *aCommandResult->Value<TUint>( KMPXCommandGeneralId );
            VCXLOGLO2("CVCXMyVideosCollectionPluginTester::HandleCommandComplete: KMPXCommandGeneralId: %d", cmd);
    
            if( cmd == KVcxCommandIdMyVideos )
                {
                cmd = 0;
                cmd = *aCommandResult->Value<TUint>( KVcxMediaMyVideosCommandId );
                VCXLOGLO2("CVCXMyVideosCollectionPluginTester::HandleCommandComplete: KVcxMediaMyVideosCommandId: %d", cmd);
                }
            }
        
        if( aCommandResult->IsSupported( KVcxMediaMyVideosCommandId ) )
            {
            TUint32 cmdId;
            cmdId = aCommandResult->ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId );
            
            if ( cmdId == KVcxCommandMyVideosGetMediaFullDetailsByMpxId )
                {
                HandleCollectionMediaL( *(aCommandResult->Value<CMPXMedia>(
                        KMPXCommandColAddMedia)), KErrNone );
                }
            }

        if( aCommandResult->IsSupported( KVcxMediaMyVideosInt32Value ) )
            {
            TInt32 result = aCommandResult->ValueTObjectL<TUint32>( KVcxMediaMyVideosInt32Value );
            
            if( result != KErrNone && !( iCancelRequested && result == KErrCancel ) )
                {
                VCXLOGLO2("CVCXMyVideosCollectionPluginTester::HandleCommandComplete: KVcxMediaMyVideosInt32Value contains error: %d", result);
                aError = result;
                }
            }
        }
    
    if( !iWaitingForItemChange || aError != KErrNone )
        {
        SetRefreshStatus(  EFalse );
        iWaitingForItemChange = EFalse;
        
        if( iActiveWait )
            {
            iActiveWait->Stop();
            }

        if( aError == KErrNotFound )
            {
            // transaction ids and cancel download to accept -1 when file doesn't exist.
            TRAP_IGNORE( iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCommandComplete, KErrNone ) );
            }
        else
            {
            // Inform observer
            TRAP_IGNORE( iObserver->HandleVcxMvTesterMessageL( KVCXMYVideosTestMessageCommandComplete, aError ) );
            }
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::HandleCommandComplete");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetMediaCount
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTester::GetMediaCount()
    {
    TInt count = 0;
    if( iMediaArray )
        {
        count = iMediaArray->Count();
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::AddMediaL()
// ---------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::AddMediaL( CMPXMedia* aMedia, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::AddMediaL");

    TBool newCreated( EFalse );
    if( aMedia )
        {
        PrintMPXMediaL( *aMedia, ETrue );
        }
    else
        {
        newCreated = ETrue;
        aMedia = CMPXMedia::NewL();
        CleanupStack::PushL( aMedia );
        }

    CMPXCommand* cmd = CreateMpxCommandLC( KMPXCommandIdCollectionAdd, 0, aSync );
    cmd->SetCObjectValueL(KMPXCommandColAddMedia, aMedia );
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Add media") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().CommandL(*cmd);

    CleanupStack::PopAndDestroy( cmd );

    if( newCreated )
        {
        CleanupStack::PopAndDestroy( aMedia );
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::AddMediaL");
    }

// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::SetMediaL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::SetMediaL( CMPXMedia* aMedia, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::SetMediaL");

    CMPXCommand* cmd = CreateMpxCommandLC( KMPXCommandIdCollectionSet, 0, aSync );
    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColSetMedia, aMedia );

    PrintMPXMediaL( *aMedia, ETrue );

    iCollectionUtility->Collection().CommandL( *cmd );
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Set media") ) );
    iCurrentActionHasResponse = EFalse;
    CleanupStack::PopAndDestroy( cmd );
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::SetMediaL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::RemoveMediaL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::RemoveMediaL( TInt aDrive, TInt aIndex, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::RemoveMediaL");

    CMPXMedia* media = NULL;

    TInt realIndex = GetMediaIndexInCollectionL( aDrive, aIndex );

    media = (*iCollectionMediaArray)[realIndex];
    
    const TDesC& localFilePath = media->ValueText( KMPXMediaGeneralUri );
    iTestCommon->EnsureFileIsNotInUse( localFilePath );
    
    TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));

    CMPXCommand* cmd = CreateMpxCommandLC( KMPXCommandIdCollectionRemoveMedia, 0, aSync );
    cmd->SetTObjectValueL( KMPXMediaGeneralId, itemId );

    TRAP_IGNORE( iStats->ActionStartL( KRemoveMediaActionId, _L("Remove media") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::RemoveMediaL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::RemoveMediaByMpxIdL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::RemoveMediaByMpxIdL( TInt aMpxId, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::RemoveMediaByMpxIdL");

    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: mpx ID: %d", aMpxId);

    CMPXCommand* cmd = CreateMpxCommandLC( KMPXCommandIdCollectionRemoveMedia, 0, aSync );
    cmd->SetTObjectValueL( KMPXMediaGeneralId, aMpxId );

    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Remove media") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::RemoveMediaByMpxIdL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetAllMediaFullDetailsL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::GetAllMediaFullDetailsL()
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::GetAllMediaFullDetailsL");

    CMPXMediaArray* fullMedias = CMPXMediaArray::NewL();

    CleanupStack::PushL( fullMedias );

    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: media count: %d", iMediaArray->Count() );

    if( GetCurrentLevel() != 3 )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: My videos category must be open!");
        User::Leave( KErrGeneral );
        }

    for( TInt i=0; i<iMediaArray->Count(); i++ )
        {
        RArray<TMPXAttribute> attrs;
        CleanupClosePushL(attrs);
        // Empty attributes to get all the details.
        CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( path );
        path->SelectL( i );

        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Details for media at index: %d", i );

        iGettingFullDetailsForAllMedia = ETrue;
        iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
        CleanupStack::PopAndDestroy( path );
        CleanupStack::PopAndDestroy( &attrs );

        iActiveWait->Start();
        iGettingFullDetailsForAllMedia = EFalse;

        fullMedias->AppendL( iFullMedia->CopyL( *iFullMedia ) );
        }

    PrintMediasL( fullMedias, EFalse, _L("FullMedias") );

    for( TInt i=0; i<fullMedias->Count(); i++ )
        {
        CMPXMedia* media = fullMedias->AtL(i);
        }

    CleanupStack::PopAndDestroy( fullMedias );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::GetAllMediaFullDetailsL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::DeleteAllMediaFilesL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::DeleteAllMediaFilesL( )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::DeleteAllMediaFilesL");

    RPointerArray<HBufC> fileList;
    CleanupResetAndDestroyPushL( fileList );
    
    CMPXMedia* media = NULL;

    for( TInt i = 0; i < iMediaArray->Count(); i++ )
        {
        media = (*iMediaArray)[i];

        if( media->IsSupported( KMPXMediaGeneralUri ) )
            {
            const TDesC& localFilePath = media->ValueText( KMPXMediaGeneralUri );
            fileList.Append( localFilePath.AllocL() );
            }
        }

    for( TInt i = 0; i < fileList.Count(); i++ )
        {
        VCXLOGLO3("Item: %d, Local file path: %S", i, fileList[i]);

        TInt err = iFs.Delete( *fileList[i] );
        if( err != KErrNone )
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: delete error: %d", err);
            }
        }

    CleanupStack::PopAndDestroy( &fileList );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::DeleteAllMediaFilesL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::DeleteFileOfMediaL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::DeleteFileOfMediaL( TInt aDrive, TInt aIndex )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::DeleteFileOfMediaL");

    CMPXMedia* media = NULL;

    TInt realIndex = GetMediaIndexInCollectionL( aDrive, aIndex );

    media = (*iCollectionMediaArray)[realIndex];
    if( media->IsSupported( KMPXMediaGeneralUri ) )
        {
        const TDesC& localFilePath = media->ValueText( KMPXMediaGeneralUri );
        VCXLOGLO2("Local file path: %S", &localFilePath);

        iTestCommon->EnsureFileIsNotInUse( localFilePath );
        TInt err = iFs.Delete( localFilePath );
        if( err != KErrNone )
            {
            VCXLOGLO2("CVCXMyVideosCollectionPluginTest:: delete error: %d", err);
            }
        }
    else
        {
        VCXLOGLO2("Item: %d, KMPXMediaGeneralUri not supported!", aIndex);
        }

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::DeleteFileOfMediaL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetMediaL
// -----------------------------------------------------------------------------
//
CMPXMedia* CVCXMyVideosCollectionPluginTester::GetMediaL( TInt aDrive, TInt aIndex )
    {
    CMPXMedia* media = NULL;

    TInt realIndex = GetMediaIndexInCollectionL( aDrive, aIndex );

    media = (*iCollectionMediaArray)[realIndex];
    return media;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::MoveMediasL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::MoveMediasL( TInt aSourceDrive, TInt aStartIndex, TInt aEndIndex, TInt aDestDrive, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::MoveMediasL");

    CMPXMediaArray* medias = SelectMediasL( aSourceDrive, aStartIndex, aEndIndex );
    CleanupStack::PushL( medias );

    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosMove, aSync );
    cmd->SetTObjectValueL( KVcxMediaMyVideosInt32Value, aDestDrive );
    if( medias )
        {
        cmd->SetCObjectValueL( KMPXMediaArrayContents, medias );
        }
    
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Move medias") ) );
    iCurrentActionHasResponse = ETrue;
    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
    CleanupStack::PopAndDestroy( medias );

    iActionCount += 2;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::MoveMediasL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::CancelMoveL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::CancelMoveOrCopyL( TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::CancelMoveOrCopyL");

    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosCancelMoveOrCopy, aSync );
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Cancel move or copy") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );
    
    iCancelRequested = ETrue;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::CancelMoveOrCopyL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::CopyMediasL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::CopyMediasL( TInt aSourceDrive, TInt aStartIndex, TInt aEndIndex, TInt aDestDrive, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::CopyMediasL");

    CMPXMediaArray* medias = SelectMediasL( aSourceDrive, aStartIndex, aEndIndex );
    CleanupStack::PushL( medias );

    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosCopy, aSync );
    cmd->SetTObjectValueL( KVcxMediaMyVideosInt32Value, aDestDrive );
    if( medias )
        {
        cmd->SetCObjectValueL( KMPXMediaArrayContents, medias );
        }
    
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Copy medias") ) );
    iCurrentActionHasResponse = ETrue;
    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
    CleanupStack::PopAndDestroy( medias );

    iActionCount += 2;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::CopyMediasL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::DeleteMediasL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::DeleteMediasL( TInt aSourceDrive, TInt aStartIndex, TInt aEndIndex, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::DeleteMediasL");

    CMPXMediaArray* medias = SelectMediasL( aSourceDrive, aStartIndex, aEndIndex );
    CleanupStack::PushL( medias );

    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosDelete, aSync );
    if( medias )
        {
        cmd->SetCObjectValueL( KMPXMediaArrayContents, medias );
        }
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Delete media") ) );
    iCurrentActionHasResponse = ETrue;
    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
    CleanupStack::PopAndDestroy( medias );

    iActionCount += 2;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::DeleteMediasL");
    }


// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::DeleteMediasByMpxIdsL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::DeleteMediasByMpxIdsL( CMPXMediaArray* aMedias, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::DeleteMediasByMpxIdsL");

    iRequestedMediaIds.Reset();
    for( TInt i = 0; i < aMedias->Count(); i++ )
        {
        CMPXMedia* media = (*aMedias)[i];
        TMPXItemId mpxId = *( media->Value<TMPXItemId>( KMPXMediaGeneralId ) );
        iRequestedMediaIds.Append( mpxId.iId1 );
        }
        
    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosDelete, aSync );
    if( aMedias )
        {
        cmd->SetCObjectValueL( KMPXMediaArrayContents, aMedias );
        }
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Delete media") ) );
    iCurrentActionHasResponse = ETrue;
    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );

    iActionCount += 2;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::DeleteMediasByMpxIdsL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::DeleteMediaByMpxIdL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::DeleteMediaByMpxIdL( TMPXItemId& aMpxId, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::DeleteMediaByMpxIdL");
    
    CMPXMediaArray* medias = CMPXMediaArray::NewL();
    CleanupStack::PushL( medias );

    iRequestedMediaIds.Reset();
    iRequestedMediaIds.Append( aMpxId.iId1 );
    
    CMPXMedia* newMedia = CMPXMedia::NewL();
    CleanupStack::PushL( newMedia );
    newMedia->SetTObjectValueL( KMPXMessageMediaGeneralId, aMpxId );
    newMedia->SetTObjectValueL( KMPXMediaGeneralId, aMpxId );
    medias->AppendL( *newMedia );
    CleanupStack::PopAndDestroy( newMedia );
    
    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosDelete, aSync );
    if( medias )
        {
        cmd->SetCObjectValueL( KMPXMediaArrayContents, medias );
        }
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Delete media") ) );
    iCurrentActionHasResponse = ETrue;
    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
    CleanupStack::PopAndDestroy( medias );

    iActionCount += 2;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::DeleteMediaByMpxIdL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::CancelDeleteL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::CancelDeleteL( TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::CancelDeleteL");

    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosCancelDelete, aSync );
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Cancel delete") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );

    iCancelRequested = ETrue;

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::CancelDeleteL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::SetAutomaticRefresh
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::SetAutomaticRefresh( TBool aValue )
    {
    iAutomaticContentRefresh = aValue;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::IsRefreshing
// -----------------------------------------------------------------------------
//
TBool CVCXMyVideosCollectionPluginTester::IsRefreshing()
    {
    return iRefreshingCollection;
    }
// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::SetQuietMode
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::SetQuietMode( TBool aValue )
    {
    iQuietMode = aValue;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::StartDownloadL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::StartDownloadL( const TDesC& aTitle, TInt aIapId, TInt aServiceId,
        TInt aContentId, const TDesC& aUrl, TBool aSync, const TDesC& aUserName, const TDesC& aPassword,
        CMPXMedia* aMedia )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::StartDownloadL");

    iUpdateDownloads = ETrue;
    iDownloadsStarted = ETrue;

    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosStartDownload, aSync );

    CMPXMedia* startDownloadReq(NULL);
    if( !aMedia )
        {
        startDownloadReq = CMPXMedia::NewL();
        CleanupStack::PushL( startDownloadReq );
        }
    else
        {
        startDownloadReq = aMedia;
        }

    //startDownloadReq->SetTObjectValueL( KVcxMediaMyVideosIapId, aIapId );
    startDownloadReq->SetTObjectValueL( KVcxMediaMyVideosIapId, 0 );

    // read only iap needed?
    TUint flags = EVcxMyVideosServiceHasReadOnlyIap | EVcxMyVideosSilent;

    startDownloadReq->SetTObjectValueL( KMPXMediaGeneralFlags, flags );

    VCXLOGLO3("CVCXMyVideosCollectionPluginTester:: UserName: %S, Password: %S.", &aUserName, &aPassword);
    
    startDownloadReq->SetTextValueL( KMPXMediaGeneralTitle, aTitle );
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosRemoteUrl, aUrl );
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosUsername, aUserName);
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosPassword, aPassword);
    
    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, startDownloadReq );

    iDlWatcher->CreateDownloadL( aIapId, aServiceId, aContentId, aUrl, aSync, aUserName, aPassword );

    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Making command.");
    iCollectionUtility->Collection().CommandL( *cmd );

    if( !aMedia )
        {
        CleanupStack::PopAndDestroy( startDownloadReq );
        }

    CleanupStack::PopAndDestroy( cmd );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::StartDownloadL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::ResumeDownloadL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::ResumeDownloadL( const TDesC& aTitle, TInt aIapId, TInt aServiceId,
        TInt aContentId, const TDesC& aUrl, TBool aSync, const TDesC& aUserName, const TDesC& aPassword,
        CMPXMedia* aMedia
        )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::ResumeDownloadL");

    iUpdateDownloads = ETrue;
    iDownloadsStarted = ETrue;

    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosStartDownload, aSync );

    CMPXMedia* startDownloadReq(NULL);
    if( !aMedia )
        {
        startDownloadReq = CMPXMedia::NewL();
        CleanupStack::PushL( startDownloadReq );
        }
    else
        {
        startDownloadReq = aMedia;
        }

    //startDownloadReq->SetTObjectValueL( KVcxMediaMyVideosIapId, aIapId );
    startDownloadReq->SetTObjectValueL( KVcxMediaMyVideosIapId, 0 );

    // read only iap needed?
    TUint flags = EVcxMyVideosServiceHasReadOnlyIap | EVcxMyVideosSilent;

    startDownloadReq->SetTObjectValueL( KMPXMediaGeneralFlags, flags );

    startDownloadReq->SetTextValueL( KMPXMediaGeneralTitle, aTitle );
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosRemoteUrl, aUrl );
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosUsername, aUserName);
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosPassword, aPassword);

    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, startDownloadReq );

    CVCXMyVideosTestDownload* dl = iDlWatcher->GetDownload( aServiceId, aContentId, aUrl );
    if( dl )
        {
        startDownloadReq->SetTObjectValueL( KVcxMediaMyVideosDownloadId, dl->iDownloadId );

        TMPXItemId itemId;
        itemId.iId1 = dl->iMpxId;
        itemId.iId2 = 0;
        startDownloadReq->SetTObjectValueL( KMPXMediaGeneralId, itemId );
        
        dl->iInformed = EFalse;
        dl->iWaitingPause = EFalse;
        }
    else
        {
        startDownloadReq->SetTObjectValueL( KVcxMediaMyVideosDownloadId, 6666 );
        iDlWatcher->CreateDownloadL( aIapId, aServiceId, aContentId, aUrl, aSync, aUserName, aPassword );
        }

    EnsureMediaFilesAreNotInUseL();
    
    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Making command.");
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Resume download") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().CommandL( *cmd );

    if( !aMedia )
        {
        CleanupStack::PopAndDestroy( startDownloadReq );
        }

    CleanupStack::PopAndDestroy( cmd );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::ResumeDownloadL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::ResumeAllDownloadsL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::ResumeAllDownloadsL()
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::ResumeAllDownloadsL");

    iUpdateDownloads = ETrue;
    iDownloadsStarted = ETrue;

    for( TInt i = 0; i < iDlWatcher->GetDownloadCount(); i++ )
        {
        CVCXMyVideosTestDownload* dl;
        dl = iDlWatcher->GetDownloadByIndex( i );
        if( dl && dl->iState == EVcxMyVideosDlStatePaused )
            {
            ResumeDownloadL( _L("resume"), dl->iIapId, dl->iServiceId, dl->iContentId, *dl->iUrl, dl->iSyncCall, *dl->iUserName, *dl->iPassword, NULL );
            }
        }
     
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::ResumeAllDownloadsL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::CancelDownloadL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::CancelDownloadL( CVCXMyVideosTestDownload* aDownload, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::CancelDownloadL");

    if( !aDownload )
        {
        User::Leave( KErrArgument );
        }

    iUpdateDownloads = ETrue;

    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosCancelDownload, aSync );

    CMPXMedia* cancelDownloadReq = CMPXMedia::NewL();
    CleanupStack::PushL( cancelDownloadReq );

    cancelDownloadReq->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, TMPXItemId ( aDownload->iMpxId, 0 ) );
    cancelDownloadReq->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, aDownload->iDownloadId );
    cancelDownloadReq->SetTextValueL( KMPXMediaGeneralUri, aDownload->iPath->Des() );

    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, cancelDownloadReq );
    
    iDlWatcher->CancelDownloadL( aDownload->iServiceId, aDownload->iContentId, *aDownload->iUrl );

    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Making command.");
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Cancel download") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cancelDownloadReq );
    CleanupStack::PopAndDestroy( cmd );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::CancelDownloadL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::CancelDownloadL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::CancelDownloadL( TInt aMpxId, TInt aDownloadId, const TPtrC& aDownloadPath, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::CancelDownloadL (by IDs)");

    iUpdateDownloads = ETrue;

    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosCancelDownload, aSync );

    CMPXMedia* cancelDownloadReq = CMPXMedia::NewL();
    CleanupStack::PushL( cancelDownloadReq );

    CVCXMyVideosTestDownload* dl = iDlWatcher->GetDownloadByIndex(0);

    TPtrC downloadPath( aDownloadPath );

    if( aMpxId == -1 && dl )
        {
        aMpxId = dl->iMpxId;
        }

    if( aDownloadId == -1 && dl )
        {
        aDownloadId = dl->iDownloadId;
        }

    if( aDownloadPath == _L("NULL") && dl )
        {
        downloadPath.Set( *dl->iPath );
        }

    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: mpxId: %d", aMpxId);
    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: downloadId: %d", aDownloadId);
    VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: downloadPath: %S", &aDownloadPath);

    cancelDownloadReq->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, TMPXItemId ( aMpxId, 0 ) );
    cancelDownloadReq->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, aDownloadId );
    cancelDownloadReq->SetTextValueL( KMPXMediaGeneralUri, downloadPath );

    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, cancelDownloadReq );

    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Making command.");
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Cancel download") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cancelDownloadReq );
    CleanupStack::PopAndDestroy( cmd );

    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::CancelDownloadL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::PauseDownloadL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::PauseDownloadL( TInt aServiceId, TInt aContentId, const TDesC& aUrl, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::PauseDownloadL");

    iUpdateDownloads = ETrue;

    CVCXMyVideosTestDownload* dl = iDlWatcher->GetDownload( aServiceId, aContentId, aUrl );
    if( dl )
        {
        dl->iWaitingPause = ETrue;
        }
    else
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Download not found!");
        User::Leave( KErrNotFound );
        }
    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosPauseDownload, aSync );

    cmd->SetTObjectValueL( KVcxMediaMyVideosDownloadId, dl->iDownloadId );

    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Making command.");
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Pause download") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::PauseDownloadL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::PauseDownloadL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::PauseDownloadL( const TDesC& aUrl, TBool aSync )
    {
    VCXLOGLO1(">>>CVCXMyVideosCollectionPluginTester::PauseDownloadL");

    iUpdateDownloads = ETrue;

    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosPauseDownload, aSync );

    CVCXMyVideosTestDownload* dl = iDlWatcher->GetDownload( aUrl );
    if( !dl )
        {
        VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Download not found!");
        User::Leave( KErrNotFound );
        }
    else
        {
        dl->iWaitingPause = ETrue;
        }
    

    cmd->SetTObjectValueL( KVcxMediaMyVideosDownloadId, dl->iDownloadId );

    VCXLOGLO1("CVCXMyVideosCollectionPluginTester:: Making command.");
    TRAP_IGNORE( iStats->ActionStartL( iTransactions->TransactionId(), _L("Pause download") ) );
    iCurrentActionHasResponse = EFalse;
    iCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );
    VCXLOGLO1("<<<CVCXMyVideosCollectionPluginTester::PauseDownloadL");
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetDownloadWatcher
// -----------------------------------------------------------------------------
//
CVCXMyVideosTestDlWatcher* CVCXMyVideosCollectionPluginTester::GetDownloadWatcher()
    {
    return iDlWatcher;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetActiveDownloadCountL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTester::GetActiveDownloadCountL()
    {
    TInt count = 0;

    CMPXMedia* media ( NULL );
    for( TInt i = 0; i < iMediaArray->Count(); i++ )
        {
        media = (*iMediaArray)[i];

        if( media->IsSupported( KVcxMediaMyVideosDownloadState ) )
            {
            TInt state = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );

            if( state == EVcxMyVideosDlStateDownloading || ( state == EVcxMyVideosDlStatePaused && iAutoResume ) )
                {
                count++;
                }
            }
        }

    VCXLOGLO2(">>>CVCXMyVideosCollectionPluginTester:: Active downloads: %d", count);

    return count;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::SetAutoResume
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::SetAutoResume( TBool aValue )
    {
    iAutoResume = aValue;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetCurrentLevel
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTester::GetCurrentLevel()
    {
    if ( iCollectionUtility )
        {
        CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
        TInt levels = path->Levels();
        delete path;
        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: levels: %d", levels);
        return levels;
        }    
    return KErrNotFound;    
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetCurrentOpenLevelIndex
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTester::GetCurrentOpenLevelIndex()
    {
    return iCurrentOpenedLevelIndex;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetLastFullMedia
// -----------------------------------------------------------------------------
//
CMPXMedia* CVCXMyVideosCollectionPluginTester::GetLastFullMedia()
    {
    return iFullMedia;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetMediaIndexInCollectionL
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTester::GetMediaIndexInCollectionL( TInt aDrive, TInt aIndex )
    {
    if( aIndex >= iMediaArray->Count() )
        {
        VCXLOGLO2("CVCXMyVideosCollectionPluginTester:: Index (%d) out of bounds.", aIndex);
        User::Leave( KErrArgument );
        }

    TInt foundIndex( KErrNotFound );
    
    if( aDrive == -1 )
        {
        // No drive specified.
        foundIndex = aIndex;
        }
    else
        {
        // Get the index in iMediaArray for a video on aDrive at aIndex.
        TInt indexOfMediaWithDrive(0);
           
        for( TInt i=0; i<iMediaArray->Count(); i++ )
            {
            CMPXMedia* media = (*iMediaArray)[i];
    
            if( media->IsSupported( KMPXMediaGeneralUri ) )
                {
                const TDesC& localFilePath = media->ValueText( KMPXMediaGeneralUri );
                TInt drive(0);
                User::LeaveIfError( iFs.CharToDrive( localFilePath[0], drive ) );
               
                if( drive == aDrive )
                    {
                    // We are at requested index for the drive. 
                    if( indexOfMediaWithDrive == aIndex )
                        {
                        foundIndex = i;
                        break;
                        }
                    indexOfMediaWithDrive++;
                    }            
                }
            }
        }
    
    if( foundIndex == KErrNotFound )
        {
        VCXLOGLO3("CVCXMyVideosCollectionPluginTester:: media not found with drive: %d, index: %d", aDrive, aIndex);
        User::Leave( KErrNotFound );
        }

    // Get the video index in My Videos collection, it's not always same because videos on ROM are ignored.  

    CMPXMedia* media = (*iMediaArray)[foundIndex];
    
    for( TInt i = 0; i < iCollectionMediaArray->Count(); i++ )
        {
        CMPXMedia* collMedia = (*iCollectionMediaArray)[i];
        TMPXItemId itemId1 = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
        TMPXItemId itemId2 = *(collMedia->Value<TMPXItemId>( KMPXMediaGeneralId ));
        
        if( itemId1 == itemId2 )
            {
            foundIndex = i;
            break;
            }
        }
    
    if( foundIndex == KErrNotFound )
         {
         VCXLOGLO3("CVCXMyVideosCollectionPluginTester:: media not found in collection array. drive: %d, index: %d", aDrive, aIndex);
         User::Leave( KErrNotFound );
         }    
    
    return foundIndex;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetVideoCountForDrive
// -----------------------------------------------------------------------------
//
TInt CVCXMyVideosCollectionPluginTester::GetVideoCountForDrive( TInt aDrive )
    {  
    if( !iMediaArray || iMediaArray->Count() == 0 ) return 0;
    
    TInt count( 0 );
    
    for( TInt i=0; i<iMediaArray->Count(); i++ )
        {
        CMPXMedia* media = (*iMediaArray)[i];

        if( media->IsSupported( KMPXMediaGeneralUri ) )
            {
            const TDesC& localFilePath = media->ValueText( KMPXMediaGeneralUri );
            TInt drive(0);
            User::LeaveIfError( iFs.CharToDrive( localFilePath[0], drive ) );
           
            if( drive == aDrive )
                {
                count++;
                }     
            }
        }
    return count;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::GetAllCollectionMedia
// -----------------------------------------------------------------------------
//
const CMPXMediaArray* CVCXMyVideosCollectionPluginTester::GetAllCollectionMedia()
    {
    return iCollectionMediaArray;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::EnsureMediaFilesAreNotInUseL
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::EnsureMediaFilesAreNotInUseL()
    {
    if( iMediaArray->Count() <= 0 ) return; 
    
    TInt retries = 200;
    RFile64 file;
    TInt error( KErrInUse );
    
    const TInt KEnsureMediasNotInUseID = 1234567;
    
    TRAP_IGNORE( iStats->ActionStartL( KEnsureMediasNotInUseID, _L("Ensure files.") ) );
    
    TBool filesLocked( ETrue );
    // Check all files that they are not in use, retry few times.
    while( --retries >= 0 && filesLocked ) 
        {
        filesLocked = EFalse;
        
        for( TInt i=0; i<iMediaArray->Count(); i++ )
            {
            CMPXMedia* media = (*iMediaArray)[i];
            
            if( media->IsSupported( KMPXMediaGeneralUri ) )
                {
                const TDesC& localFilePath = media->ValueText( KMPXMediaGeneralUri );

                error = file.Open( iFs, localFilePath, EFileShareExclusive );
                if( error == KErrInUse || error == KErrLocked )
                    {
                    filesLocked = ETrue;
                    User::After( 1000000 * 5 ); // Wait
                    break;
                    }
                file.Close();
                }
            }
        }

    if( error != KErrNone && error != KErrNotFound && error != KErrBadName )
        {
        TRAP_IGNORE( iStats->ActionEndL( KEnsureMediasNotInUseID, error ) );
        VCXLOGLO2("CVCXMyVideosCollectionPluginTester::EnsureMediaFilesAreNotInUseL: error: %d", error);
        User::Leave( error );
        }
    else
        {
        TRAP_IGNORE( iStats->ActionEndL( KEnsureMediasNotInUseID, KErrNone ) );
        }
    
    VCXLOGLO1("CVCXMyVideosCollectionPluginTester::EnsureMediaFilesAreNotInUseL: All ok.");
    }
    
// -----------------------------------------------------------------------------
// CVCXMyVideosCollectionPluginTester::TimerComplete
// -----------------------------------------------------------------------------
//
void CVCXMyVideosCollectionPluginTester::TimerComplete( TInt /* aTimerId */, TInt /* aError */ )
    {
    if( iProgressTimer )
        {
        iProgressTimer->After( 1000000 );
        }

    if( !iMediaArray || !iDlWatcher )
       {
       return;
       }

    if( iDownloadsStarted && iUpdateDownloads && GetCurrentLevel() == 3 )
        {
       // Print short info about downloads
       CMPXMedia* media( NULL );

       VCXLOGLO2("CVCXMyVideosCollectionPluginTester::TimerComplete: medias: %d", iMediaArray->Count());
       for( TInt i = 0; i < iMediaArray->Count(); i++ )
           {
           media = (*iMediaArray)[i];

           if( media->IsSupported( KVcxMediaMyVideosDownloadState ) )
               {
               TInt state = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );

               TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));

               TInt progress = -1;
               if( media->IsSupported( KVcxMediaMyVideosDownloadProgress ) )
                   {
                   progress = media->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress );
                   }

               TUint32 downloadId = 0;
               if( media->IsSupported( KVcxMediaMyVideosDownloadId ) )
                   {
                   downloadId = media->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );
                   }

               CVCXMyVideosTestDownload* dl = iDlWatcher->GetDownloadByMpxId( itemId.iId1 );

               if( dl )
                   {
//                   VCXLOGLO5("CVCXMyVideosCollectionPluginTester:: DL ID: %d, MPX ID: %d, state: %d, progress: %d", downloadId, itemId.iId1, state, progress );
//                   VCXLOGLO3("CVCXMyVideosCollectionPluginTester:: serviceId: %d, contentId: %d", dl->iServiceId, dl->iContentId );
                   iDlWatcher->UpdateDownloadProgressL( itemId.iId1, downloadId, progress );
                   }
               }
           }
        }
    }
//  End of File
