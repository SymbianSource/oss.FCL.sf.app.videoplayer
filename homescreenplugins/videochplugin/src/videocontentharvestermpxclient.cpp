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
* Description: 
*
*/


#include <e32std.h>
#include <mpxcollectionutility.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionpath.h>
#include <mpxmessage2.h>
#include <mpxcollectionmessage.h>
#include <mpxlog.h>
#include <mpxitemid.h>
#include <mpxmessagecontainerdefs.h>
#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include "IptvDebug.h"

#include "videocontentharvesterplugin.h"
#include "videocontentharvestermpxclient.h"
#include "videochpublishabledata.h"
#include "videochpublisher.h"
#include "videonotificationlauncher.h"
#include "videochmcategorylistener.h"
#include "videoahpluginconstants.h"
#include "iptvlastwatchedapi.h"
#include "iptvlastwatcheddata.h"

const TInt KVcxMpxLevelCategories = 2;
const TInt KVcxMpxLevelVideos     = 3;

const TUint32 KVcxnsCHUid = 0x2001B2A8;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CVcxNsContentHarvesterMpxClient::CVcxNsContentHarvesterMpxClient( CVcxNsChPublisher* aPublisher,
                                                                  CVcxNsChPublishableData* aPublishableData,
                                                                  CVcxNsContentHarvesterPlugin* aPlugin ):
    iCollectionOpenStatus( EVcxNsCollectionNotOpen ),
    iPublisher( aPublisher ),
    iPublishableData( aPublishableData ),
    iPlugin( aPlugin )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::ConstructL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::ConstructL IN");
    
    TUid domain   = TUid::Uid( KVcxNsMpxMediatorDomain );
    TUid category = TUid::Uid( KVcxNsMpxMediatorCategory );
    TVersion version = TVersion( KVcxNsMpxEventVersion, 0, 0 );
    
    iMCategoryListener = CVcxNsChMCategoryListener::NewL( this,
                                                          domain,
                                                          category,
                                                          version );
    TInt event =  TInt( KVcxNsMpxEventDownloadStarted );
    iMCategoryListener->RegisterAndSubscribeL( event );
    
    event = TInt( KVcxNsMpxEventAllDownloadsEnded );
    iMCategoryListener->RegisterAndSubscribeL( event );

    domain   = TUid::Uid( KVcxNsAhMediatorDomain );
    category = TUid::Uid( KVcxNsAhMediatorCategory );
    version = TVersion( KVcxNsAhEventVersion, 0, 0 );
    
    iAhCategoryListener= CVcxNsChMCategoryListener::NewL( this,
                                                          domain,
                                                          category,
                                                          version );

    event = TInt( KVcxNsAhEventSuiteLoaded );
    iAhCategoryListener->RegisterAndSubscribeL( event );
    
    event = TInt( KVcxNsAhEventSuiteUnloaded );
    iAhCategoryListener->RegisterAndSubscribeL( event );
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::ConstructL OUT");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CVcxNsContentHarvesterMpxClient* CVcxNsContentHarvesterMpxClient::NewL( CVcxNsChPublisher* aPublisher,
                                                                        CVcxNsChPublishableData* aPublishableData,
                                                                        CVcxNsContentHarvesterPlugin* aPlugin )
    {
    CVcxNsContentHarvesterMpxClient* self = new( ELeave ) CVcxNsContentHarvesterMpxClient( aPublisher,
                                                                                           aPublishableData,
                                                                                           aPlugin );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CVcxNsContentHarvesterMpxClient::~CVcxNsContentHarvesterMpxClient()
     {
     delete iMCategoryListener;
     delete iAhCategoryListener;
     
     delete iObservedCategory;
     CloseCollection();
     }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::HandleCollectionMediaL( 
        const CMPXMedia& /*aMedia*/,
        TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::HandleCollectionMessage( 
        CMPXMessage* aMessage,
        TInt aError )
    {
    TRAP_IGNORE( HandleCollectionMessageL( aMessage, aError ) );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::HandleCollectionMessageL( 
        CMPXMessage* aMessage,
        TInt aError )
    {
    HandleSingleCollectionMessageL( aMessage, aError );
    
    if ( aMessage->IsSupported( KMPXMessageArrayContents ) )
        {
        const CMPXMessageArray* messageArray =
            aMessage->Value<CMPXMessageArray>( KMPXMessageArrayContents );
        for( TInt i = 0; i < messageArray->Count(); i++ )
            {
            HandleSingleCollectionMessageL( (*messageArray)[i], KErrNone );
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::HandleSingleCollectionMessageL( 
        CMPXMessage* aMessage,
        TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::HandleSingleCollectionMessageL IN");
    
    if ( aMessage && aError == KErrNone )
        {
        switch ( *( aMessage->Value<TMPXMessageId>( KMPXMessageGeneralId ) ) )
            {
            case KVcxCommandIdMyVideos:
                {
                if ( aMessage->IsSupported( KVcxMediaMyVideosCommandId ) )
                    {
                    TInt mvMsgId = *(aMessage->Value<TInt>( KVcxMediaMyVideosCommandId ) );

                    switch ( mvMsgId )
                        {
                        case KVcxMessageMyVideosItemsAppended:
                            HandleMyVideosItemsAppendedL( aMessage, aError );
                            break;
                        case KVcxMessageMyVideosGetMediasByMpxIdResp:
                            break;
                        default:
                            break;
                        }
                    }
                }
                break;
            case KMPXMessageGeneral:
                {
                TInt event( *(aMessage->Value<TInt>( KMPXMessageGeneralEvent )) );
                TInt data( *(aMessage->Value<TInt>( KMPXMessageGeneralData )) );

                switch ( event )
                    {
                    case TMPXCollectionMessage::EPathChanged:
                        {
                        if ( data == EMcContainerOpened  )
                            {
                             if ( iCollectionOpenStatus == EVcxNsCollectionOpening )
                                {
                                iCollectionOpenStatus = EVcxNsCollectionOpened;
                                }
                             if( iCollectionUtility )
                                 {
                                 iCollectionUtility->Collection().OpenL();
                                 }
                            }
                        }
                        break;
                        
                    case TMPXCollectionMessage::ECollectionChanged:                     
                    case TMPXCollectionMessage::EBroadcastEvent:
                    case TMPXCollectionMessage::ENoEvent:
                    case TMPXCollectionMessage::EError:
                    case TMPXCollectionMessage::EMediaChanged:
                    case TMPXCollectionMessage::EItemChanged:
                    case TMPXCollectionMessage::EFocusChanged:
                    case TMPXCollectionMessage::EAsyncOpComplete:
                    case TMPXCollectionMessage::EExtendedMessage:
                    default:
                        break;
                    }
                }
                break;
            case KMPXMessageIdItemChanged:
                {
                HandleMyVideosItemsChangedL( aMessage, aError );
                }
                break;
            
            default:
                break;
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::HandleSingleCollectionMessageL OUT");
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::HandleOpenL( 
        const CMPXMedia& aEntries,
        TInt /*aIndex*/,
        TBool /*aComplete*/,
        TInt aError )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::HandleOpenL IN");
    if ( aError == KErrNone )
        {
        TInt levels = CollectionLevelL();

        // should the interesting level be the root?
        if ( levels == KVcxMpxLevelCategories )
            {
            // Check that entry has an array of contents.
            if ( aEntries.IsSupported( KMPXMediaArrayContents ) )
                {
                CMPXMediaArray* pointerArray = 
                    aEntries.Value<CMPXMediaArray>(KMPXMediaArrayContents);
                CMPXMediaArray* categoryArray = CMPXMediaArray::NewL( *pointerArray );
                CleanupStack::PushL( categoryArray );

                SearchAndStoreCategoryL( categoryArray );

                CleanupStack::Pop( categoryArray );
                }
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::HandleOpenL OUT");
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//                      
void CVcxNsContentHarvesterMpxClient::HandleOpenL( 
        const CMPXCollectionPlaylist& /*aPlaylist*/,
        TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::HandleCommandComplete( 
        CMPXCommand* /*aCommandResult*/, 
        TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::HandleMyVideosItemsAppendedL( CMPXMessage* /*aMessage*/,
                                                                   TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::HandleMyVideosItemsAppendedL IN");

    TInt levels = CollectionLevelL();

    if ( levels == KVcxMpxLevelCategories )
        {
        UpdateNewVideoCountL();       
        }
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::HandleMyVideosItemsAppendedL OUT");
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::HandleMyVideosItemsChangedL( CMPXMessage* aMessage,
                                                                   TInt /*aError*/ )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::HandleMyVideosItemsChangedL IN");
    
    if( aMessage && aMessage->IsSupported( KMPXMessageChangeEventType ) )
        {
        TMPXChangeEventType eventType( 
                *aMessage->Value<TMPXChangeEventType>( KMPXMessageChangeEventType ) );
   
       
        if( eventType == EMPXItemDeleted )
            {        
            HandleContentModifiedL( aMessage );
            }
    
        TMPXItemId eventData( 
                *aMessage->Value<TMPXItemId>( KMPXMessageMediaGeneralId ) );

        TInt levels = CollectionLevelL();

        if ( levels == KVcxMpxLevelCategories )
            {
            UpdateNewVideoCountL();
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::HandleMyVideosItemsChangedL OUT");
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::HandleContentModifiedL( CMPXMessage* aMessage )
    {
    // check preloaded content allways, in case it's data still exists in the PS
    TPtrC preloadedName = 
        iPublishableData->GetDesCDataL( EVCxNsCPContentPreloaded,
                                        TPtrC8( KTvVideoName ) );
    
    TPtrC preloadedPath = 
                       iPublishableData->GetDesCDataL( EVCxNsCPContentPreloaded,
                                                       TPtrC8( KTvVideoPath ) );
    
    TPtrC lastWatchedName = 
            iPublishableData->GetDesCDataL( EVCxNsCPContentLastWatched,
                                            TPtrC8( KTvVideoName ) );
    TInt lastWType = 
            iPublishableData->GetIntDataL( EVCxNsCPContentLastWatched,
                                            TPtrC8( KTvVideoLastWatchedType ) );
    if( preloadedName.Length() > 0 )
        {
        TPtrC filePath = 
            iPublishableData->GetDesCDataL( EVCxNsCPContentPreloaded,
                                            TPtrC8( KTvVideoPath ) );
        if( !iPublishableData->FileExists( filePath ) )
            {
            // file no longer exists, remove it's data from the CP
            iPublisher->DeleteContentL( EVCxNsCPContentPreloaded );
            } 
        else
            {
            iPublisher->PublishContentL( EVCxNsCPContentPreloaded );
            }
        }
    
    // check last watched item
    if( lastWatchedName.Length() > 0 && lastWType == EVCxNsCPContentLastWatched )
        {
        
        TMPXItemId itemId = aMessage->ValueTObjectL< TMPXItemId >( KMPXMessageMediaGeneralId );      
        // check if deleted was last watched
        CIptvLastWatchedData* data =  CIptvLastWatchedData::NewL();
        CleanupStack::PushL( data );
        iPublishableData->LastWatchedApi().GetLastWatchedDataL( *data );
        if( data->MpxId() == itemId.iId1 )
            {
            // last watched removed, clear data 
            iPublishableData->ClearData( EVCxNsCPContentLastWatched );
            iPublisher->PublishContentL( EVCxNsCPContentLastWatched );
            }
        else
            {
            // check if preloaded was last watched
            if( preloadedName.Length() > 0 && !data->Uri().CompareF( preloadedPath ) )
                {
                // it was, so clear last watched
                iPublishableData->ClearData( EVCxNsCPContentLastWatched );
                iPublisher->PublishContentL( EVCxNsCPContentLastWatched );
                }
            
            }
        CleanupStack::PopAndDestroy( data );
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::MediatorEventL( TUid aDomain,
                                                      TUid aCategory,
                                                      TInt aEventId,
                                                      const TDesC8& )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::MediatorEventL IN");
    
    if( aDomain    == TUid::Uid( KVcxNsMpxMediatorDomain ) &&
        aCategory  == TUid::Uid( KVcxNsMpxMediatorCategory ) )
        {
        if( aEventId == KVcxNsMpxEventDownloadStarted )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::MediatorEventL dl started");

            iOngoingDownloads = ETrue;
            }
        if( aEventId == KVcxNsMpxEventAllDownloadsEnded )
            {
            IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::MediatorEventL dls finished");

            iOngoingDownloads = EFalse;
            }
        }
    
    if( aDomain    == TUid::Uid( KVcxNsAhMediatorDomain ) &&
        aCategory  == TUid::Uid( KVcxNsAhMediatorCategory ) )
        {
        if( aEventId == KVcxNsAhEventSuiteLoaded )
            {
            iTvVideoSuiteLoaded = ETrue;
            }
        if( aEventId == KVcxNsAhEventSuiteUnloaded )
            {
            iTvVideoSuiteLoaded = EFalse; 
            
            if( iOngoingDownloads )
                {   
                iPlugin->NotificationLauncher()->LaunchExitingNotificationL();
                }
            }
        }
    
    if ( iCollectionOpenStatus == EVcxNsCollectionNotOpen )
        {
        if ( iTvVideoSuiteLoaded || iOngoingDownloads )
            {
            OpenCollectionL();
            UpdateCategoryL();
            }
        }
    else
        {
        if( !iTvVideoSuiteLoaded && !iOngoingDownloads  )
            {
            CloseCollection();
            }
        }       
     
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::MediatorEventL OUT");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CVcxNsContentHarvesterMpxClient::CollectionLevelL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::CollectionLevelL");
    
    if ( iCollectionUtility )
        {
        CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( path );
        TInt levels = path->Levels();
        CleanupStack::PopAndDestroy( path );
        
        return levels;
        }

    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::UpdateNewVideoCountL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::UpdateNewVideoCountL IN");
    
    if( iObservedCategory && 
        iObservedCategory->IsSupported( KVcxMediaMyVideosCategoryNewItemCount ) &&
        iObservedCategory->IsSupported( KVcxMediaMyVideosCategoryItemCount ) )
        {
        TInt newVideosCount = iObservedCategory->ValueTObjectL<TUint32>( KVcxMediaMyVideosCategoryNewItemCount );
        TInt videoCount = iObservedCategory->ValueTObjectL<TUint32>( KVcxMediaMyVideosCategoryItemCount );
        TBool hasNewVideos = newVideosCount > 0;

        TBool hadNewVideos = 
                iPublishableData->GetIntDataL( EVCxNsCPContentMyVideos,
                                               TPtrC8( KTvVideoHasNewVideos ) );

        TInt savedCount = 
                iPublishableData->GetIntDataL( EVCxNsCPContentMyVideos,
                                               TPtrC8( KTvVideoCount ) );
        
        if( hasNewVideos != hadNewVideos ||
            ( hasNewVideos && newVideosCount != savedCount ) ||
            ( !hasNewVideos && videoCount != savedCount ) )
            {
            if( hasNewVideos )
                {
                iPublishableData->SetDataL( EVCxNsCPContentMyVideos, 
                                            TPtrC8( KTvVideoCount ),
                                            newVideosCount );
                iPublishableData->SetDataL( EVCxNsCPContentMyVideos, 
                                            TPtrC8( KTvVideoHasNewVideos ),
                                            ETrue );                                
                TPtrC newVideoName( iObservedCategory->ValueText( KVcxMediaMyVideosCategoryNewItemName ) );
                
                iPublishableData->SetDataL( EVCxNsCPContentMyVideos, 
                                            TPtrC8( KTvVideoName ),
                                            newVideoName );     
                }
            else
                {
                iPublishableData->SetDataL( EVCxNsCPContentMyVideos, 
                                            TPtrC8( KTvVideoCount ),
                                            videoCount );
                iPublishableData->SetDataL( EVCxNsCPContentMyVideos, 
                                            TPtrC8( KTvVideoHasNewVideos ),
                                            EFalse );
                iPublishableData->SetDataL( EVCxNsCPContentMyVideos, 
                                            TPtrC8( KTvVideoName ),
                                            KNullDesC );
                }
            
            iPublisher->PublishContentL( EVCxNsCPContentMyVideos );
            }
        }

    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::UpdateNewVideoCountL OUT");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::UpdateCategoryL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::UpdateCategoryL IN");
    
    if ( iCollectionOpenStatus == EVcxNsCollectionNotOpen )
        {
        // Not opened, open the collection.
        OpenCollectionL(); 
        }
    else
        {
        if ( !iCollectionUtility )
            {
            return;
            }
        
        if ( CollectionLevelL() == KVcxMpxLevelVideos )
            {
            // Openend in video level, back
            iCollectionUtility->Collection().BackL();
            }
        else
            {
            // Already open, refetch the data
            iCollectionUtility->Collection().OpenL();
            }
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::UpdateCategoryL OUT");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::SearchAndStoreCategoryL( CMPXMediaArray* categoryArray )
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::SearchAndStoreCategoryL IN");
    
    if( categoryArray->Count() > 0 )
        {
        CMPXMedia* category = categoryArray->AtL( 0 );
        
        if( category )
             {
             if( iObservedCategory )
                 {
                 delete iObservedCategory;
                 iObservedCategory = NULL;
                 }
             
             iObservedCategory = CMPXMedia::NewL( *category );
             
             UpdateNewVideoCountL();
             }
        }
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::SearchAndStoreCategoryL OUT");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::OpenCollectionL()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::OpenCollectionL IN");
    
    if( !iCollectionUtility )
        {
        // use our content harvester plugin's uid as a mode
        // to prevent collection utility we use to be binded 
        // into context of other possible collection utility clients
        // in other content harvester plugins
        TUid mode = TUid::Uid( KVcxnsCHUid );
        iCollectionUtility = MMPXCollectionUtility::NewL( this, mode );
        }

    iCollectionOpenStatus = EVcxNsCollectionOpening;
    
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( KVcxUidMyVideosMpxCollection ); //my videos
    iCollectionUtility->Collection().OpenL( *path );
    CleanupStack::PopAndDestroy( path );
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::OpenCollectionL OUT");
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CVcxNsContentHarvesterMpxClient::CloseCollection()
    {
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::CloseCollection IN");
    
    if( iCollectionUtility )
        {
        iCollectionUtility->Close();
        iCollectionUtility = NULL;
        iCollectionOpenStatus = EVcxNsCollectionNotOpen;
        }
    
    IPTVLOGSTRING_LOW_LEVEL("CVcxNsContentHarvesterMpxClient::CloseCollection OUT");
    }
