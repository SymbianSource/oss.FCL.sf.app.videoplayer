/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDES
#include "VCXTestMyVideosCollectionClient.h"
#include "VCXTestCommon.h"
#include "VCXTestLog.h"
#include "CIptvTestTimer.h"
#include "CIptvTestActiveWait.h"

#include <mpxcollectionutility.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionpath.h>
#include <mpxmessage2.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediabase.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>

#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CVCXTestMyVideosCollectionClient* CVCXTestMyVideosCollectionClient::NewL( MVCXTestMyVideosObserver* aObserver )
    {
    CVCXTestMyVideosCollectionClient* self = new (ELeave) CVCXTestMyVideosCollectionClient( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::~CVCXTestMyVideosCollectionClient
// -----------------------------------------------------------------------------
//
CVCXTestMyVideosCollectionClient::~CVCXTestMyVideosCollectionClient()
     {
     VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::~CVCXTestMyVideosCollectionClient");
     if( iTimer )
         {
         iTimer->CancelTimer();
         delete iTimer;
         iTimer = NULL;
         }

     if( iCollectionUtility )
         {
         iCollectionUtility->Close();
         }
     iCollectionUtility = NULL;
     
     delete iCurrentEntries;
     iCurrentEntries = NULL;
     
     if( iMediaArray )
         {
         iMediaArray->Reset();
         delete iMediaArray;
         iMediaArray = NULL;
         }

     iFs.Close();

     iActiveWait->Stop();
     delete iActiveWait;
     iActiveWait = NULL;
     
     delete iTestCommon;
     iTestCommon = NULL;

     if( iDownloads )
         {
         iDownloads->Reset();
         delete iDownloads;
         iDownloads = NULL;
         }
     
     if( iVideos ) 
         {
         iVideos->Reset();
         delete iVideos;
         iVideos = NULL;
         }
     
     iInsertedVideoMdsIds.Reset();
     
     iOngoingDownloads.Reset();
     
     iCompletedDownloads.Reset();
     
     VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::~CVCXTestMyVideosCollectionClient");
     }

 // -----------------------------------------------------------------------------
 // CVCXTestMyVideosCollectionClient::CVCXTestMyVideosCollectionClient
 // -----------------------------------------------------------------------------
 //
CVCXTestMyVideosCollectionClient::CVCXTestMyVideosCollectionClient( MVCXTestMyVideosObserver* aObserver )
 : iObserver( aObserver )
    {
    
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::ConstructL
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::ConstructL( )
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::ConstructL");
    User::LeaveIfError( iFs.Connect() );
    
    iCollectionUtility = MMPXCollectionUtility::NewL( this, KMcModeIsolated );

    iTestCommon = CVCXTestCommon::NewL();
    iActiveWait = CIptvTestActiveWait::NewL();
    iTimer = CIptvTestTimer::NewL( *this, 0 );
    
    iCollectionUid = KVcxUidMyVideosMpxCollection;
    iOpenLevelIndex = -1;
    
    OpenCollectionL();
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::OpenCollectionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::OpenCollectionL( )
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::OpenCollectionL =----->");
    
    TInt levels = LevelsL();
    
    if( levels == KVCXTestMpxLevelRoot || levels == 0 )
        {
        VCXLOGLO1("CVCXTestMyVideosCollectionClient:: Opening collection.");
        // Open collection.
        CMPXCollectionPath* path = CMPXCollectionPath::NewL();
        CleanupStack::PushL( path );
        path->AppendL( iCollectionUid );
        iCollectionUtility->Collection().OpenL( *path );
        CleanupStack::PopAndDestroy( path );
        }
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::OpenCollectionL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::OpenCollectionLevelL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::OpenCollectionLevelL( TUint32 aIndex )
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::OpenLevelL =----->");
    
    TInt levels = LevelsL();
    
    if( levels == KVCXTestMpxLevelCategory )
        {
        VCXLOGLO1("CVCXTestMyVideosCollectionClient:: Opening category.");
        // Open all videos.
        iOpenLevelIndex = aIndex;
        iCollectionUtility->Collection().OpenL( iOpenLevelIndex );
        }

    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::OpenLevelL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::RefreshCollectionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::RefreshCollectionL( )
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::RefreshCollectionL =----->");
    
    TInt levels = LevelsL();
    
    if( levels >= KVCXTestMpxLevelCategory )
        {
        VCXLOGLO1("CVCXTestMyVideosCollectionClient:: Refreshing collection.");
        // Just refresh
        iCollectionUtility->Collection().OpenL();
        }
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::RefreshCollectionL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::LevelsL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVCXTestMyVideosCollectionClient::LevelsL()
    {
    if ( iCollectionUtility )
        {
        CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
        TInt levels = path->Levels();
        delete path;
        VCXLOGLO2("CVCXTestMyVideosCollectionClient:: levels: %d", levels);
        return levels;
        }    
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::UpdateVideoListL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::UpdateVideoListL( TBool aComplete )
    {
    if( !iCurrentEntries ) return;

    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::UpdateVideoListL =----->");
    
    // Create new list.    
    delete iVideos;
    iVideos = CMPXMediaArray::NewL();        

    TBool isCategoryList( EFalse );
    
    // Add videos to the list.
    
    for( TInt i = 0; i < iMediaArray->Count(); i++ )
        {
        CMPXMedia* media = (*iMediaArray)[i];
        
        TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
        
        if( itemId.iId2 != 0 )
            {
            isCategoryList = ETrue;
            }
        
        // Add non downloads and completed downloads to the list.
        if( media->IsSupported( KVcxMediaMyVideosDownloadState ) )
            {
            TUint8 state = 0; 
            state = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );
            
            if( state == EVcxMyVideosDlStateDownloaded )
                {
                iVideos->AppendL( *media );
                VCXLOGLO3("CVCXTestMyVideosCollectionClient::UpdateVideoListL media is completed download, id: %d, state: %d", itemId.iId1, state);
                }
            else
                {
                VCXLOGLO3("CVCXTestMyVideosCollectionClient::UpdateVideoListL media is downloading, id: %d, state: %d", itemId.iId1, state);
                }
            }
        else
            {
            VCXLOGLO2("CVCXTestMyVideosCollectionClient::UpdateVideoListL media is video, id: %d", itemId.iId1);
            iVideos->AppendL( *media );
            
            // Send add video event if the video has been just added.
            if( iInsertedVideoMdsIds.Find( itemId.iId1 ) != -1 )
                {
                iObserver->VideoEventL( MVCXTestMyVideosObserver::EVCXTestVideoVideoAdded, itemId.iId1, 0, KErrNone );
                }
            }
        }
    
    iInsertedVideoMdsIds.Reset();
    
    if( isCategoryList )
        {
        VCXLOGLO2("CVCXTestMyVideosCollectionClient::UpdateVideoListL Categories (%d): =----->", iVideos->Count());
        }
    else
        {
        VCXLOGLO2("CVCXTestMyVideosCollectionClient::UpdateVideoListL Videos (%d): =----->", iVideos->Count());
        }
    
    PrintMediasL( iVideos );
        
    if( aComplete )
        {
        if( !isCategoryList )
            {
            iObserver->VideoEventL( MVCXTestMyVideosObserver::EVCXTestVideoListRefreshed, 0, 0, KErrNone );
            }
        
        // Open category.
        if( LevelsL() == KVCXTestMpxLevelCategory ) 
            {
            OpenCollectionLevelL( 0 );
            }        
        }
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::UpdateVideoListL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::UpdateDownloadListL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::UpdateDownloadListL()
    {
    if( LevelsL() != KVCXTestMpxLevelVideo || !iCurrentEntries ) return;

    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::UpdateDownloadListL =----->");
    
    TBool ongoingDownloadsChanged( EFalse );    
    TInt iCompletedDownloadsBefore = iCompletedDownloads.Count();
    
    TInt downloadCountBefore( 0 );
    if( iDownloads )
        {
        downloadCountBefore = iDownloads->Count(); 
        }
    
    // Create new list.
    delete iDownloads;
    iDownloads = NULL;
    iDownloads = CMPXMediaArray::NewL();
    
    // Add videos to the list.    
    for( TInt i = 0; i < iMediaArray->Count(); i++ )
        {
        CMPXMedia* media = (*iMediaArray)[i];
        
        TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
        
        // Add only downloads which are not complete to the list.
        if( media->IsSupported( KVcxMediaMyVideosDownloadState ) )
            {
            TUint8 state = 0; 
            state = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );
            
            VCXLOGLO3("CVCXTestMyVideosCollectionClient::UpdateDownloadListL: media mpx id: %d, state: %d", itemId.iId1, state);
            
            if( state != EVcxMyVideosDlStateDownloaded )
                {
                iDownloads->AppendL( *media );
                }

            if( UpdateDownloadL( media ) )
                {
                ongoingDownloadsChanged = ETrue;
                }
            }
        else
            {
            VCXLOGLO2("CVCXTestMyVideosCollectionClient::UpdateDownloadListL: media mpx id %d is not a download.", itemId.iId1);
            }
        }
    
    VCXLOGLO4("CVCXTestMyVideosCollectionClient::UpdateDownloadListL: ongoing downloads changed: %d, completed downloads before: %d and now: %d", ongoingDownloadsChanged, iCompletedDownloadsBefore, iCompletedDownloads.Count());
    
    // Update video list if more downloads have been completed.
    if( iCompletedDownloadsBefore != iCompletedDownloads.Count() )
        {
        UpdateVideoListL( ETrue );
        
        // Inform client about new video(s).
        TInt newCompletedDownloads = iCompletedDownloads.Count() - iCompletedDownloadsBefore;
        
        for( TInt i = 0; i < newCompletedDownloads; i++ )
            {
            TInt index = ( iCompletedDownloads.Count() - 1 ) - i;
            TInt32 mdsId = iCompletedDownloads[ index ].iMdsId;
            iObserver->VideoEventL( MVCXTestMyVideosObserver::EVCXTestVideoVideoAdded, mdsId, 0, KErrNone );
            }
        }
    
    // Print downloads if they have changed.
    if( ongoingDownloadsChanged || downloadCountBefore != iDownloads->Count())
        {
        VCXLOGLO3("CVCXTestMyVideosCollectionClient:: Downloads (%d, ongoing: %d): =----->", iDownloads->Count(), iOngoingDownloads.Count());
        PrintMediasL( iDownloads );
        
        if( iOngoingDownloads.Count() == 0 && downloadCountBefore > 0 )
            {
            iObserver->VideoEventL( MVCXTestMyVideosObserver::EVCXTestVideoAllDownloadsCompleted, 0, 0, KErrNone );
            }
        }
    
    // Start download update timer if there's downloads.
    if( iDownloads->Count() > 0 )
        {
        iTimer->After( 1000000 );
        }
    else
        {
        iTimer->Cancel();
        }
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::UpdateDownloadListL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::UpdateMediaArrayL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::UpdateMediaArrayL() 
    {
    if( !iCurrentEntries ) return;
    
    // Reset old medias.
    if( iMediaArray )
        {
        iMediaArray->Reset();
        delete iMediaArray;
        iMediaArray = NULL;
        }
    
    // Create copies of the current medias.    
    iMediaArray = CMPXMediaArray::NewL();
    
    CMPXMediaArray* medias = iCurrentEntries->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    
    // Make copies of the medias.
    for( TInt i=0; i<medias->Count(); i++ )
        {
        CMPXMedia* media(NULL);
        media = (*medias)[i];

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
    
    VCXLOGLO2("CVCXTestMyVideosCollectionClient::UpdateMediaArrayL: iMediaCount: %d", iMediaArray->Count());
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::PrintMediasL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::PrintMediasL( CMPXMediaArray* aMediaArray ) 
    {
    for( TInt i = 0; i < aMediaArray->Count(); i++ )
        {
        CMPXMedia* media = (*aMediaArray)[i];

        TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
        
        // Category.
        if( itemId.iId2 != 0 )
            {
            TBuf<256> title;
            if( media->IsSupported( KMPXMediaGeneralTitle ) )
                {
                title = media->ValueText( KMPXMediaGeneralTitle );
                }

            TInt itemCount(0);
            TInt newItemCount(0);
            TBuf<256> newItemName;
            if( media->IsSupported( KVcxMediaMyVideosCategoryNewItemName ) )
                {
                newItemName = media->ValueText( KVcxMediaMyVideosCategoryNewItemName );
                }
            if( media->IsSupported( KVcxMediaMyVideosCategoryItemCount ) )
                 {
                 itemCount = *(media->Value<TUint32>( KVcxMediaMyVideosCategoryItemCount ));
                 }
            if( media->IsSupported( KVcxMediaMyVideosCategoryNewItemCount ) )
                 {
                 newItemCount = *(media->Value<TUint32>( KVcxMediaMyVideosCategoryNewItemCount ));
                 }
            VCXLOGLO5("CVCXTestMyVideosCollectionClient: title: %S, medias: %d, new: %d, new media: %S", &title, itemCount, newItemCount, &newItemName);
            }
        // Media.
        else
            {
            HBufC* pathBuff = NULL;
            HBufC* titleBuff = NULL;        
            TInt size(0);
            TInt origin(-1);
    
            // Download specific.
            HBufC* urlBuff = NULL;        
            TUint8 state = 0; 
            TInt8 progress = 0;
            TUint32 downloadId = 0;
            TInt32 downloadError = 0;
            TInt32 globalError = 0;
        
            if( media->IsSupported( KVcxMediaMyVideosDownloadState ) )
                {
                state = media->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );
                }
            
            if( media->IsSupported( KVcxMediaMyVideosDownloadId ) )
                {
                downloadId = media->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );
                }

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
        
            if( media->IsSupported( KMPXMediaGeneralSize ) )
                 {
                 size = *(media->Value<TInt>( KMPXMediaGeneralSize ));
                 }
            
            if( media->IsSupported( KVcxMediaMyVideosOrigin ) )
                {
                origin = *(media->Value<TUint>( KVcxMediaMyVideosOrigin ));
                }
            
            if( media->IsSupported( KVcxMediaMyVideosRemoteUrl ) )
                {
                const TDesC& url = media->ValueText( KVcxMediaMyVideosRemoteUrl );
                urlBuff = url.AllocL();
                CleanupStack::PushL( urlBuff );
                }
            
            if( media->IsSupported( KVcxMediaMyVideosDownloadProgress ) )
                {
                progress = media->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress );
                }
            
            if( media->IsSupported( KVcxMediaMyVideosDownloadError ) )
                {
                downloadError = media->ValueTObjectL<TInt32>( KVcxMediaMyVideosDownloadError );
                }
        
            if( media->IsSupported( KVcxMediaMyVideosDownloadGlobalError ) )
                {
                globalError = media->ValueTObjectL<TInt32>( KVcxMediaMyVideosDownloadGlobalError );
                }
            
            VCXLOGLO3("CVCXTestMyVideosCollectionClient: iId1: %d, iId2: %d", itemId.iId1, itemId.iId2 );
            VCXLOGLO5("CVCXTestMyVideosCollectionClient: title: %S, size: %d, origin: %d, path: %S", titleBuff, size, origin, pathBuff);
            if( media->IsSupported( KVcxMediaMyVideosDownloadState ) )        
                {
                VCXLOGLO2("CVCXTestMyVideosCollectionClient: url: %S", urlBuff);
                VCXLOGLO4("CVCXTestMyVideosCollectionClient: state: %d, progress: %d, dl id: %d", state, progress, downloadId);
                if( downloadError != KErrNone || globalError != KErrNone )
                    {
                    VCXLOGLO3("CVCXTestMyVideosCollectionClient: downloadError: %d, globalError: %d, dl id: %d", downloadError, globalError);
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
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::UpdateDownloadL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVCXTestMyVideosCollectionClient::UpdateDownloadL( CMPXMedia* aMedia )
    {
    if( !aMedia->IsSupported( KVcxMediaMyVideosDownloadState ) || 
        !aMedia->IsSupported( KVcxMediaMyVideosDownloadId ) )
        {
        return EFalse;
        }

    TMPXItemId itemId = *(aMedia->Value<TMPXItemId>( KMPXMediaGeneralId ));
    
    TUint8 state = 0; 
    TUint32 downloadId = 0;
    TInt8 progress = 0;
    TInt32 downloadError = 0;
    TInt32 globalError = 0;
    
    state = aMedia->ValueTObjectL<TUint8>( KVcxMediaMyVideosDownloadState );
    
    downloadId = aMedia->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );
    
    if( aMedia->IsSupported( KVcxMediaMyVideosDownloadProgress ) )
        {
        progress = aMedia->ValueTObjectL<TInt8>( KVcxMediaMyVideosDownloadProgress );
        }
    
    if( aMedia->IsSupported( KVcxMediaMyVideosDownloadError ) )
        {
        downloadError = aMedia->ValueTObjectL<TInt32>( KVcxMediaMyVideosDownloadError );
        }      
    
    if( aMedia->IsSupported( KVcxMediaMyVideosDownloadGlobalError ) )
        {
        globalError = aMedia->ValueTObjectL<TInt32>( KVcxMediaMyVideosDownloadGlobalError );
        }
    
    if( downloadError == KErrNone ) 
        {
        downloadError = globalError;
        }    
    
    TVCXTestDownload download;
    download.iMdsId = itemId.iId1;
    download.iState = state;
    download.iDownloadId = downloadId;
    download.iProgress = progress;
    download.iDownloadError = downloadError;
    download.iGlobalError = globalError;    
    
    TInt index = iOngoingDownloads.Find( download, CVCXTestMyVideosCollectionClient::CompareDownloads );
    
    TBool changesMade( EFalse );
    
    switch( state )
        {
        case EVcxMyVideosDlStateDownloading:
            {
            // It's new download if not found, otherwise check for progress.
            if( index == KErrNotFound )
                {
                iOngoingDownloads.AppendL( download );
                iObserver->VideoEventL( 
                        MVCXTestMyVideosObserver::EVCXTestVideoDownloadStarted, itemId.iId1, 0, KErrNone );
                changesMade = ETrue;
                }
            else
                {
                if( progress >= iOngoingDownloads[index].iProgress+10 || 
                   ( progress == 100 && iOngoingDownloads[index].iProgress != 100 ) )
                    {
                    iOngoingDownloads.Remove( index );
                    iOngoingDownloads.AppendL( download );
                    iObserver->VideoEventL( 
                            MVCXTestMyVideosObserver::EVCXTestVideoDownloadProgressed, itemId.iId1, progress, KErrNone );
                    changesMade = ETrue;
                    }
                }
            }
            break;
            
        case EVcxMyVideosDlStateFailed:
            {
            // Update only if state has changed.
            if( index != KErrNotFound && iOngoingDownloads[index].iState != state )
                {
                iOngoingDownloads.Remove( index );
                iObserver->VideoEventL( 
                        MVCXTestMyVideosObserver::EVCXTestVideoDownloadFailed, itemId.iId1, 0, downloadError );
                changesMade = ETrue;
                }            
            }
            break;
            
        case EVcxMyVideosDlStatePaused:
            {
            // Update only if state has changed.
            if( index != KErrNotFound && iOngoingDownloads[index].iState != state )
                {
                iOngoingDownloads.Remove( index );
                iObserver->VideoEventL( 
                        MVCXTestMyVideosObserver::EVCXTestVideoDownloadPaused, itemId.iId1, 0, KErrNone );
                changesMade = ETrue;
                }            
            }
            break;
            
        case EVcxMyVideosDlStateDownloaded:
            {
            // Remove from ongoing dl list.
            if( index != KErrNotFound )
                {
                iOngoingDownloads.Remove( index );
                iObserver->VideoEventL( 
                        MVCXTestMyVideosObserver::EVCXTestVideoDownloadCompleted, itemId.iId1, 0, KErrNone );
                changesMade = ETrue;
                }
            
            // Add to completed dl list.
            index = iCompletedDownloads.Find( download, CVCXTestMyVideosCollectionClient::CompareDownloads );
            if( index == KErrNotFound )
                {
                iCompletedDownloads.AppendL( download );
                }
            }
            break;
        
        default: 
            break;
        }
    
    return changesMade;
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::CompareDownloads
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CVCXTestMyVideosCollectionClient::CompareDownloads( const TVCXTestDownload& a1, const TVCXTestDownload& a2 )
    {
    return a1.iMdsId == a2.iMdsId;
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::CreateMpxCommandLC
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXCommand* CVCXTestMyVideosCollectionClient::CreateMpxCommandLC( TInt aCommandGeneralId, TInt aMyVideosCommandId, TBool aSync )
    {
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL( KMPXCommandGeneralId, aCommandGeneralId );
    if( aCommandGeneralId == KVcxCommandIdMyVideos )
        {
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, aMyVideosCommandId );
        }

    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, aSync );

    // If async command then set transaction ID.
    if( !aSync )
        {
        cmd->SetTObjectValueL( KVcxMediaMyVideosTransactionId, 0 );
        }

    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, iCollectionUid );

    return cmd;
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::VideoCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVCXTestMyVideosCollectionClient::VideoCount() 
    {
    if( !iVideos )
        {
        return 0;
        }
    return iVideos->Count();
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::GetVideoL
// -----------------------------------------------------------------------------
//
EXPORT_C CMPXMedia* CVCXTestMyVideosCollectionClient::GetVideoL( TUint aIndex )
    {   
    CMPXMedia* media = NULL;
        
    if( !iVideos || iVideos->Count() <= aIndex )
        {
        User::Leave( KErrArgument );
        }
    
    media = (*iVideos)[aIndex];
    
    return media;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::RemoveVideoL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::RemoveVideoL( TUint aIndex )
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::RemoveVideoL");

    if( !iVideos || iVideos->Count() <= aIndex )
        {
        User::Leave( KErrNotReady );
        }
    
    CMPXMedia* media = NULL;

    media = (*iVideos)[aIndex];
    TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
    
    VCXLOGLO2("CVCXTestMyVideosCollectionClient::RemoveVideoL: mpxId: %d", itemId.iId1);
        
    CMPXCommand* cmd = CreateMpxCommandLC( KMPXCommandIdCollectionRemoveMedia, 0, EFalse );
    cmd->SetTObjectValueL( KMPXMediaGeneralId, itemId );
    iCollectionUtility->Collection().CommandL( *cmd );
    CleanupStack::PopAndDestroy( cmd );

    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::RemoveVideoL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::DownloadCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVCXTestMyVideosCollectionClient::DownloadCount() 
    {
    if( !iDownloads )
        {
        return 0;
        }
    return iDownloads->Count();
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::DownloadL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::DownloadL( const TDesC& aUrl, TUint aIapId, const TDesC& aUserName, const TDesC& aPassword )
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::DownloadL");
    
    CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosStartDownload, EFalse );
    
    CMPXMedia* startDownloadReq(NULL);
    startDownloadReq = CMPXMedia::NewL();
    CleanupStack::PushL( startDownloadReq );

    startDownloadReq->SetTObjectValueL( KVcxMediaMyVideosIapId, aIapId );
    startDownloadReq->SetTObjectValueL( KMPXMediaGeneralFlags, EVcxMyVideosServiceHasReadOnlyIap | EVcxMyVideosSilent );
    startDownloadReq->SetTextValueL( KMPXMediaGeneralTitle, _L("testdownload") );
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosRemoteUrl, aUrl );
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosUsername, aUserName);
    startDownloadReq->SetTextValueL( KVcxMediaMyVideosPassword, aPassword);

    cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, startDownloadReq );
    iCollectionUtility->Collection().CommandL( *cmd );
    
    CleanupStack::PopAndDestroy( startDownloadReq );
    CleanupStack::PopAndDestroy( cmd );
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::DownloadL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::EnsureMediaFilesAreNotInUseL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::EnsureMediaFilesAreNotInUseL()
    {
    if( iMediaArray->Count() <= 0 ) return;
    
    TInt retries = 4;
    RFile file;
    TInt error( KErrInUse );
    
    // Check all files that they are not in use, retry few times.
    while( --retries >= 0 && error == KErrInUse ) 
        {
        for( TInt i=0; i<iMediaArray->Count(); i++ )
            {
            CMPXMedia* media = (*iMediaArray)[i];
            
            if( media->IsSupported( KMPXMediaGeneralUri ) )
                {
                const TDesC& localFilePath = media->ValueText( KMPXMediaGeneralUri );

                error = file.Open( iFs, localFilePath, EFileShareExclusive );
                if( error == KErrInUse )
                    {
                    User::After( 1000000 * 60 ); // Wait a minute
                    break;
                    }
                file.Close();
                }
            }
        }
        
    if( error != KErrNone && error != KErrNotFound )
        {
        VCXLOGLO2("CVCXTestMyVideosCollectionClient::EnsureMediaFilesAreNotInUseL: error: %d", error);
        User::Leave( error );
        }        
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::GetCountOfOngoingDownloads
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVCXTestMyVideosCollectionClient::GetCountOfOngoingDownloads()
    {
    return iOngoingDownloads.Count();
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::GetCountOfNewVideos
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVCXTestMyVideosCollectionClient::GetCountOfNewVideos( TBool aIncludeDownloads )
    {
    TInt count( 0 );
    
    if( iVideos )
        {
        for( TInt i = 0; i < iVideos->Count(); i++ )
            {
            CMPXMedia* media = (*iVideos)[i];
            if( media->IsSupported( KMPXMediaGeneralFlags ) )
                {
                TUint value = *(*media).Value<TUint>( KMPXMediaGeneralFlags );
                if( value & EVcxMyVideosVideoNew )
                    {
                    count++;
                    }
                }
            }
        }
    
    if( aIncludeDownloads && iDownloads )
        {
        for( TInt i = 0; i < iDownloads->Count(); i++ )
            {
            CMPXMedia* media = (*iDownloads)[i];
            if( media->IsSupported( KMPXMediaGeneralFlags ) )
                {
                TUint value = *(*media).Value<TUint>( KMPXMediaGeneralFlags );
                if( value & EVcxMyVideosVideoNew )
                    {
                    count++;
                    }
                }
            }
        }    
    
    return count;
    }

// -----------------------------------------------------------------------------
// CVcxMyVideosCollectionTester::CancelDownloadL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVCXTestMyVideosCollectionClient::CancelDownloadL( TUint aIndex )
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::CancelDownloadL");

    if( !iDownloads || iDownloads->Count() <= aIndex )
        {
        User::Leave( KErrNotReady );
        }
    
    CMPXMedia* media = NULL;

    media = (*iDownloads)[aIndex];
    TMPXItemId itemId = *(media->Value<TMPXItemId>( KMPXMediaGeneralId ));
    
    VCXLOGLO2("CVCXTestMyVideosCollectionClient::CancelDownloadL: mpxId: %d", itemId.iId1);
    
    if( media->IsSupported( KVcxMediaMyVideosDownloadId ) && media->IsSupported( KMPXMediaGeneralUri ) )
        {
        TUint32 downloadId = 0;    
        downloadId = media->ValueTObjectL<TUint32>( KVcxMediaMyVideosDownloadId );
        HBufC* path = media->ValueText( KMPXMediaGeneralUri ).AllocL();
        CleanupStack::PushL( path );
        
        CMPXCommand* cmd = CreateMpxCommandLC( KVcxCommandIdMyVideos, KVcxCommandMyVideosCancelDownload, EFalse );        
        
        CMPXMedia* cancelDownloadReq = CMPXMedia::NewL();
        CleanupStack::PushL( cancelDownloadReq );

        cancelDownloadReq->SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, TMPXItemId ( itemId.iId1, 0 ) );
        cancelDownloadReq->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId, downloadId );
        cancelDownloadReq->SetTextValueL( KMPXMediaGeneralUri, *path );

        cmd->SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, cancelDownloadReq );
        
        CleanupStack::PopAndDestroy( cancelDownloadReq );
        iCollectionUtility->Collection().CommandL( *cmd );
        CleanupStack::PopAndDestroy( cmd );
        CleanupStack::PopAndDestroy( path );
        }
    else
        {    
        User::Leave( KErrArgument );
        }
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::RemoveMediaL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::HandleCollectionMediaL
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::HandleCollectionMediaL(const CMPXMedia& aMedia,
                                        TInt aError)
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::HandleCollectionMediaL");
    VCXLOGLO2("CVCXTestMyVideosCollectionClient:: aError: %d", aError);

    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::HandleCollectionMediaL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::HandleCollectionMessage
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::HandleCollectionMessage( CMPXMessage* aMessage, TInt aError )
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::HandleCollectionMessage");
    VCXLOGLO2("CVCXTestMyVideosCollectionClient:: aError: %d", aError);

    if( !aMessage ) return;
    
    if( aError == KErrNone )
        {
        TInt err( KErrNone );

        TMPXMessageId messageId = *(aMessage->Value<TMPXMessageId>( KMPXMessageGeneralId ));
        
        TInt myVideosCmd = -1;
        if ( messageId == KVcxCommandIdMyVideos )
            {
            myVideosCmd = aMessage->ValueTObjectL<TInt>( KVcxMediaMyVideosCommandId );
            }

        // Handle single message.
        if ( myVideosCmd != KVcxMessageMyVideosMessageArray )
            {
            TRAP( err, HandleSingleCollectionMessageL( aMessage ) );
            }
        else
            {
            // Expand and handle the messages in My Videos message array.
            if( err == KErrNone && aMessage->IsSupported( KMPXMessageArrayContents ) )
                {
                const CMPXMessageArray* messageArray =
                        aMessage->Value<CMPXMessageArray>(KMPXMessageArrayContents);

                TInt count = messageArray->Count();

                for( TInt i = 0; i < count; i++ )
                    {
                    TRAP( err, HandleSingleCollectionMessageL( (*messageArray)[i] ) );
                    if( err != KErrNone )
                        {
                        VCXLOGLO2("CVCXTestMyVideosCollectionClient:: Leave at handling, %d =----->", err);
                        }
                    }
                }
            }
        
        if( err != KErrNone )
            {
            aError = err;
            }
        }
    
    if( aError != KErrNone )
        {
        iObserver->VideoEventL( MVCXTestMyVideosObserver::EVCXTestVideoGeneral, 0, 0, aError );
        }
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::HandleCollectionMessage");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::HandleSingleCollectionMessageL
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::HandleSingleCollectionMessageL( CMPXMessage* aMessage )
    {    
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::HandleSingleCollectionMessageL");
    
    TMPXMessageId messageId = *(aMessage->Value<TMPXMessageId>( KMPXMessageGeneralId ));

    // Check that message is for open collection.
    if( aMessage->IsSupported( KMPXMessageCollectionId ) )
        {
        TUid uid( *(aMessage->Value<TUid>( KMPXMessageCollectionId )) );
        if( TUid::Uid( iCollectionUid ) != uid )
            {
            VCXLOGLO1("CVCXTestMyVideosCollectionClient:: wrong collection id");
            VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::HandleSingleCollectionMessage");
            return;
            }
        }
    
    switch( messageId )
        {
        case KVcxCommandIdMyVideos:
            {
            VCXLOGLO1("CVCXTestMyVideosCollectionClient:: KVcxCommandIdMyVideos =----->");
            HandleMyVideosMessageL( aMessage );
            }
            break;
            
        case KMPXMessageGeneral:
            {
            VCXLOGLO1("CVCXTestMyVideosCollectionClient:: KMPXMessageGeneral =----->");
            HandleGeneralMessageL( aMessage );
            }
            break;
        
        case KMPXMessageIdItemChanged:        
            {
            VCXLOGLO1("CVCXTestMyVideosCollectionClient:: KMPXMessageIdItemChanged =----->");
            HandleItemChangedMessageL( aMessage );
            }
            break;
            
        default:
            VCXLOGLO1("CVCXTestMyVideosCollectionClient:: unknown message ID");
            break;
        }
        
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::HandleSingleCollectionMessageL");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::HandleMyVideosMessageL
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::HandleMyVideosMessageL( CMPXMessage* aMessage )
    {   
    if ( !aMessage->IsSupported( KVcxMediaMyVideosCommandId ) ) 
        {
        return;
        }
    
    TInt myVideosCommandId = *(aMessage->Value<TInt>( KVcxMediaMyVideosCommandId ));
    
    VCXLOGLO2("CVCXTestMyVideosCollectionClient:: KVcxMediaMyVideosCommandId: %d", myVideosCommandId);
        
    switch ( myVideosCommandId )
        {
        case KVcxMessageMyVideosDeleteResp:
        case KVcxMessageMyVideosDeleteStarted:
        case KVcxMessageMyVideosCopyResp:
        case KVcxMessageMyVideosMoveResp:
        case KVcxMessageMyVideosMoveOrCopyStarted:
        case KVcxMessageMyVideosGetMediasByMpxIdResp:
            {
            }
            break;
            
        /** 
         * This is received when list is open already and the list contains new items
         */
        case KVcxMessageMyVideosItemsAppended:
            {
            VCXLOGLO1("CVCXTestMyVideosCollectionClient:: KVcxMessageMyVideosItemsAppended =----->");

            if( LevelsL() < KVCXTestMpxLevelCategory )
                {
                VCXLOGLO1("CVCXTestMyVideosCollectionClient:: Category list or video list not open, ignoring.");
                break;
                }

            UpdateMediaArrayL();
            UpdateVideoListL( EFalse  );
            UpdateDownloadListL();
            // Wait list complete event and do nothing here.
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
            VCXLOGLO1("CVCXTestMyVideosCollectionClient:: KVcxMessageMyVideosListComplete =----->");
            
            UpdateMediaArrayL();
            UpdateVideoListL( ETrue );
            UpdateDownloadListL();
            }
            break;
            
        default:
            break;
        }   
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::HandleGeneralMessageL
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::HandleGeneralMessageL( CMPXMessage* aMessage )
    {
    TInt event( *(aMessage->Value<TInt>( KMPXMessageGeneralEvent )) );
    TInt type( *(aMessage->Value<TInt>( KMPXMessageGeneralType )) );
    TInt data( *(aMessage->Value<TInt>( KMPXMessageGeneralData )) );

    VCXLOGLO4("CVCXTestMyVideosCollectionClient:: event: %d, type: %d, data: %d", event, type, data);
    
    if ( event == TMPXCollectionMessage::EPathChanged && data == EMcContainerOpened ) // 4 & 1
        {
        RefreshCollectionL();
        }
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::HandleItemChangedMessageL
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::HandleItemChangedMessageL( CMPXMessage* aMessage )
    {   
    TMPXItemId itemId( *aMessage->Value<TMPXItemId>(KMPXMessageMediaGeneralId) );
    VCXLOGLO3("CVCXTestMyVideosCollectionClient:: Item Id1: %d, Id2: %d", itemId.iId1, itemId.iId2);

    // Check the IDs for categories
    if( itemId.iId2 != 0 && ( itemId.iId1 != KVcxMvcCategoryIdAll &&
            itemId.iId1 != KVcxMvcCategoryIdDownloads &&
            itemId.iId1 != KVcxMvcCategoryIdTvRecordings &&
            itemId.iId1 != KVcxMvcCategoryIdCaptured &&
            itemId.iId1 != KVcxMvcCategoryIdOther ) )
        {
        VCXLOGLO1("CVCXTestMyVideosCollectionClient:: Error. INVALID ITEM ID.");
        return;
        }

    // Skip message if it's for category but not for the open category.
    if( itemId.iId2 > 0 && itemId.iId1 != iOpenLevelIndex )
        {
        VCXLOGLO1("CVCXTestMyVideosCollectionClient:: Not for open category. Skip.");
        return;
        }

    TBool refreshNeeded( EFalse );
    
    TMPXChangeEventType changeEvent( *aMessage->Value<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
    
    switch( changeEvent )
        {
        case EMPXItemInserted:
            {
            VCXLOGLO1("CVCXTestMyVideosCollectionClient:: changeEvent EMPXItemInserted =----->");
            refreshNeeded = ETrue;
            
            iInsertedVideoMdsIds.Append( itemId.iId1 );
            }
            break;
    
        case EMPXItemDeleted:
            {
            VCXLOGLO1("CVCXTestMyVideosCollectionClient:: changeEvent EMPXItemDeleted =----->");
            refreshNeeded = ETrue;

            iObserver->VideoEventL( MVCXTestMyVideosObserver::EVCXTestVideoVideoDeleted, itemId.iId1, 0, KErrNone );
            }
            break;
    
        case EMPXItemModified:
            {
            VCXLOGLO1("CVCXTestMyVideosCollectionClient:: changeEvent EMPXItemModified =----->");
            
            if ( aMessage->IsSupported( KVcxMediaMyVideosInt32Value ) )
                {
                TInt32 extraInfo = aMessage->ValueTObjectL<TInt32>( KVcxMediaMyVideosInt32Value );
                if ( extraInfo == EVcxMyVideosVideoListOrderChanged )
                    {
                    VCXLOGLO1("CVcxMyVideosCollectionTester:: Received EVcxMyVideosVideoListOrderChanged");
                    refreshNeeded = ETrue;
                    }
                }
            }
            break;
    
        default:
            break;
        }    
    
    if( refreshNeeded )
        {
        RefreshCollectionL();
        }
    
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::HandleOpenL
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::HandleOpenL( const CMPXMedia& aEntries,
        TInt aIndex, TBool aComplete, TInt aError )
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::HandleOpenL");
    VCXLOGLO2("CVCXTestMyVideosCollectionClient:: aError: %d", aError);

    if( aError == KErrNone )
       {
       delete iCurrentEntries;
       iCurrentEntries = NULL;
       iCurrentEntries = CMPXMedia::NewL( aEntries );
       }
    
    UpdateMediaArrayL();
    UpdateVideoListL( aComplete );
    UpdateDownloadListL();
    
    if( aError != KErrNone )
        {
        iObserver->VideoEventL( MVCXTestMyVideosObserver::EVCXTestVideoGeneral, 0, 0, aError );
        }    
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::HandleOpenL");
    }    

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::HandleOpenL
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::HandleOpenL( const CMPXCollectionPlaylist& aPlaylist, 
        TInt aError )
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::HandleOpenL (playlist)");
    
    if( aError != KErrNone )
        {
        iObserver->VideoEventL( MVCXTestMyVideosObserver::EVCXTestVideoGeneral, 0, 0, aError );
        }
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::HandleOpenL (playlist)");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::HandleCommandComplete
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::HandleCommandComplete( CMPXCommand* /* aCommandResult */,
        TInt aError)
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::HandleCommandComplete");
    VCXLOGLO2("CVCXTestMyVideosCollectionClient:: aError: %d", aError);

    if( aError != KErrNone )
        {
        iObserver->VideoEventL( MVCXTestMyVideosObserver::EVCXTestVideoGeneral, 0, 0, aError );
        }
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::HandleCommandComplete");
    }

// -----------------------------------------------------------------------------
// CVCXTestMyVideosCollectionClient::TimerComplete
// -----------------------------------------------------------------------------
//
void CVCXTestMyVideosCollectionClient::TimerComplete(TInt /* aTimerId */, TInt aError)
    {
    VCXLOGLO1(">>>CVCXTestMyVideosCollectionClient::TimerComplete");
    VCXLOGLO2("CVCXTestMyVideosCollectionClient:: aError: %d", aError);

    UpdateDownloadListL();
    
    VCXLOGLO1("<<<CVCXTestMyVideosCollectionClient::TimerComplete");
    }

// EOF
