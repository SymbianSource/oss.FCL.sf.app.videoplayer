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
* Description:   Video list cache. Contains cached data from MDS.*
*/



// INCLUDE FILES
#include <mpxlog.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxmediacontainerdefs.h>
#include <vcxmyvideosdefs.h>
#include <centralrepository.h>
#include <collate.h>
#include <mpxmediavideodefs.h>
#include "vcxmyvideosvideocache.h"
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideosdownloadutil.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideoscategories.h"
#include "vcxmyvideosdownloadcache.h"
#include "vcxmyvideosmessagelist.h"
#include "vcxmyvideosopenhandler.h"

//       If video list fetching is going on and at the same time add/delete events
//       arrive from mds, what to do?

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosVideoCache* CVcxMyVideosVideoCache::NewL(
    CVcxMyVideosCollectionPlugin& aMyVideosCollectionPlugin )
    {
    MPX_FUNC("CVcxMyVideosCollectionCache::NewL");

    CVcxMyVideosVideoCache* self = new (ELeave) CVcxMyVideosVideoCache( aMyVideosCollectionPlugin );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosVideoCache::~CVcxMyVideosVideoCache()
    {
    MPX_FUNC("CVcxMyVideosVideoCache::~CVcxMyVideosVideoCache");
    
    delete iVideoList;
    delete iDownloadCache;
    delete iCenRep;
    
    DeletePartialList();
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosVideoCache::CVcxMyVideosVideoCache(
        CVcxMyVideosCollectionPlugin& aMyVideosCollectionPlugin )
: iCollection( aMyVideosCollectionPlugin )
    {
    MPX_FUNC("CVcxMyVideosVideoCache::CVcxMyVideosVideoCache");
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::ConstructL ()
    {
    MPX_FUNC("CVcxMyVideosVideoCache::ConstructL");

    iVideoList          = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    iVideoListIsPartial = ETrue;

    iDownloadCache = CVcxMyVideosDownloadCache::NewL();
    iPartialVideoList.Reset();
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosVideoCache::CreateVideoListByOriginL
// ----------------------------------------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosVideoCache::CreateVideoListByOriginL( TUint8 aOrigin )
    {
    MPX_FUNC("CVcxMyVideosVideoCache::CreateVideoListByOriginL()");
    
    CMPXMediaArray* allVideosArray = NULL;
    TInt allVideosArrayCount       = 0;

    if ( aOrigin == EVcxMyVideosOriginSideLoaded )
        {
        aOrigin = EVcxMyVideosOriginOther;
        }
            
    allVideosArray = iVideoList->Value<CMPXMediaArray>(
            KMPXMediaArrayContents);

    allVideosArrayCount = allVideosArray->Count();

    MPX_DEBUG2("CVcxMyVideosVideoCache:: iVideoList count = %d", allVideosArrayCount );
    
    CMPXMedia* filteredVideos =
            TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    CleanupStack::PushL( filteredVideos ); // 1->
                        
    CMPXMediaArray* filteredVideosArray = filteredVideos->Value<CMPXMediaArray>(
            KMPXMediaArrayContents );

    CMPXMedia* media = NULL;
    
    for( TInt i = 0; i < allVideosArrayCount; i++ )
        {
        media = (*allVideosArray)[i];

        TUint8 mediaOrigin = EVcxMyVideosOriginOther;

        if ( media->IsSupported( KVcxMediaMyVideosOrigin ) )
            {
            mediaOrigin = *(media->Value<TUint8>( KVcxMediaMyVideosOrigin ));
            
            if ( mediaOrigin == EVcxMyVideosOriginSideLoaded )
                {
                mediaOrigin = EVcxMyVideosOriginOther;
                }
            }
            
        if ( mediaOrigin == aOrigin )
            {
            filteredVideosArray->AppendL( *media );
            }
        }
    
    filteredVideos->SetTObjectValueL( KMPXMediaArrayCount, filteredVideosArray->Count() );
    
    CleanupStack::Pop( filteredVideos );  // <-1
    return filteredVideos;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::AppendToListL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::AppendToListL( CMPXMedia& aToList, CMPXMedia& aFromList,
        TUint8 aOrigin, TInt aNewItemsStartIndex )
    {
    MPX_FUNC("CVcxMyVideosVideoCache::AppendToListL()");
    
    if ( aNewItemsStartIndex < 0 )
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: aNewItemsStartIndex < 0 -> skipping");
        return;
        }
        
    CMPXMediaArray* toArray = aToList.Value<CMPXMediaArray>(
            KMPXMediaArrayContents );

    CMPXMediaArray* fromArray = aFromList.Value<CMPXMediaArray>(
            KMPXMediaArrayContents );

    TInt fromCount = fromArray->Count();
    TInt origin;
    CMPXMedia* media;
    for ( TInt i = aNewItemsStartIndex; i < fromCount; i++ )
        {
        media  = fromArray->AtL( i );
        origin = TVcxMyVideosCollectionUtil::OriginL( *media );
        if ( origin == aOrigin )
            {
            toArray->AppendL( *media );
            } 
        }
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::FindVideoByUriL
// ----------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosVideoCache::FindVideoByUriL( const TDesC& aUri )
    {    
    CMPXMediaArray* cachedVideoArray = iVideoList->Value<CMPXMediaArray>(
                                KMPXMediaArrayContents);

    for ( TInt i = 0; i < cachedVideoArray->Count(); i++ )
        {       
        if ( (*cachedVideoArray)[i]->ValueText( KVcxMediaMyVideosRemoteUrl ) == aUri )
            {
            return (*cachedVideoArray)[i];
            }
        }
    return NULL;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::FindVideoByMdsIdL
// ----------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosVideoCache::FindVideoByMdsIdL( TUint32 aMdsId, TInt& aPos )
    {
    //MPX_FUNC("CVcxMyVideosVideoCache::FindVideoByMdsIdL()");
    
    //MPX_DEBUG2("CVcxMyVideosVideoCache:: looking for %d MDS ID from cache", aMdsId);
    
    aPos = KErrNotFound;
    
    if ( !iVideoList )
        {
        return NULL;
        }
        
    CMPXMediaArray* cachedVideoArray = iVideoList->Value<CMPXMediaArray>(
                                KMPXMediaArrayContents);
   
    TMPXItemId mpxItemId;
    CMPXMedia* media;
    TInt count = cachedVideoArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        media = cachedVideoArray->AtL( i );
        if ( media->IsSupported( KMPXMediaGeneralId ))
            {
            mpxItemId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
            if ( mpxItemId.iId1 == aMdsId && mpxItemId.iId2 == 0 )
                {
                MPX_DEBUG2("CVcxMyVideosVideoCache::FindVideoByMdsIdL() found %d from iVideoList", aMdsId );
                aPos = i;
                return media;
                }
            }
        }

    count = iPartialVideoList.Count();
    TMPXItemId mpxId;  
    for ( TInt i = 0; i < count; i++ )
        {
        mpxId = TVcxMyVideosCollectionUtil::IdL( *(iPartialVideoList[i]) );
        if ( aMdsId ==  mpxId.iId1 )
            {
            MPX_DEBUG2("CVcxMyVideosVideoCache::FindVideoByMdsIdL() found %d from iPartialVideoList", aMdsId );
            MPX_DEBUG1("CVcxMyVideosVideoCache::FindVideoByMdsIdL() returning the pointer and setting aPos to KErrNotFound");
            aPos = KErrNotFound; // this indicates that not found from iVideoList
            return iPartialVideoList[i];
            }       
        }

    MPX_DEBUG2("CVcxMyVideosCollectionPlugin::FindVideoByMdsIdL() MDSID %d NOT FOUND", aMdsId);
    return NULL;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::FindVideoByDownloadIdL
// ----------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosVideoCache::FindVideoByDownloadIdL( TUint aDownloadId )
    {
    MPX_FUNC("CVcxMyVideosVideoCache::FindVideoByDownloadIdL()");
    
    MPX_DEBUG2("CVcxMyVideosVideoCache:: looking for MPX item  with download ID %d from cache", aDownloadId);
        
    if ( aDownloadId == 0 )
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: dl id == 0 -> NOT FOUND");
        return NULL;
        }
    
    if ( !iVideoList )
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: iVideoList is NULL -> NOT FOUND");
        return NULL;
        }

    CMPXMedia* video = iDownloadCache->Get( aDownloadId );
    if ( video )
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: found from Download Cache");
        return video;
        }

    MPX_DEBUG1("CVcxMyVideosVideoCache:: not found from Download Cache");

    CMPXMediaArray* cachedVideoArray = iVideoList->Value<CMPXMediaArray>(
                                KMPXMediaArrayContents);
              
    TUint32 downloadId;
    TInt count = cachedVideoArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        downloadId = TVcxMyVideosCollectionUtil::DownloadIdL( *cachedVideoArray->AtL( i ) );
        if ( downloadId == aDownloadId )
            {
            MPX_DEBUG1("CVcxMyVideosVideoCache:: found from iVideoList");
            MPX_DEBUG1("CVcxMyVideosVideoCache:: adding to iDownloadCache");
            iDownloadCache->Add( downloadId, (*cachedVideoArray)[i] );
            return (*cachedVideoArray)[i];
            }
        }
    
    count = iPartialVideoList.Count();  
    for ( TInt i = 0; i < count; i++ )
        {
        downloadId = TVcxMyVideosCollectionUtil::DownloadIdL( *(iPartialVideoList[i]) );
        if ( downloadId ==  aDownloadId )
            {
            MPX_DEBUG1("CVcxMyVideosVideoCache:: found from iPartialVideoList");
            MPX_DEBUG1("CVcxMyVideosVideoCache:: adding to iDownloadCache");
            iDownloadCache->Add( downloadId, iPartialVideoList[i] );
            return iPartialVideoList[i];
            }       
        }
        
    MPX_DEBUG1("CVcxMyVideosVideoCache:: NOT FOUND");
    return NULL;
    }


// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::GetVideosL
// ----------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosVideoCache::GetVideosL( RArray<TUint32> aMdsIds )
    {
    CMPXMessage* videoList = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    CleanupStack::PushL( videoList ); // 1->

    // copy from iVideoList
    TVcxMyVideosCollectionUtil::CopyFromListToListL(
            *iVideoList, *videoList, aMdsIds );

    // copy from iPartialVideoList
    TVcxMyVideosCollectionUtil::CopyFromListToListL(
            iPartialVideoList, *videoList, aMdsIds );
    
    CleanupStack::Pop( videoList ); // <-1
    return videoList;
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::AddVideosFromMdsL
// Called when item inserted events arrive from mds or from
// KVcxCommandMyVideosGetMediasByMpxId cmd handler.
// Synchronizing with downloads is not done here, caller must take care of it.
// If mpx item is already in cache, or MDS does not contain the item (or was not video item),
// then it is removed from aMdsIds. Ie after this function call aMdsIds contains
// only IDs which are actually added to cache.
// ----------------------------------------------------------------------------
//    
void CVcxMyVideosVideoCache::AddVideosFromMdsL( RArray<TUint32>& aMdsIds,
        TBool& aListFetchingWasCanceled )
    {
    MPX_FUNC("CVcxMyVideosVideoCache::AddVideosFromMdsL");

    TVcxMyVideosSortingOrder sortingOrder = SortingOrderL();
  
    aListFetchingWasCanceled = EFalse;

    MPX_DEBUG2("CVcxMyVideosVideoCache:: adding %d items", aMdsIds.Count());

    for ( TInt j = aMdsIds.Count()-1; j >= 0 ; j-- )
        {
        CMPXMedia* videoToCache;
        TInt pos;
        if ( !FindVideoByMdsIdL( aMdsIds[j], pos ) )
            {
            MPX_DEBUG2("CVcxMyVideosVideoCache:: MDSID(%d) not found from cache, ok", aMdsIds[j]);
            
            //TODO: This doesnt work with new mds cmd queue. Cancel must be done if there is _any_ async req going on.
            //      Maybe the fetching could be changed to asynchronous...
            if ( iCollection.iMyVideosMdsDb->iVideoListFetchingIsOngoing )
                {
                // If list fetching is not canceled, CreateVideoObjectL will leave with KErrNotReady.
                MPX_DEBUG1("CVcxMyVideosVideoCache:: video list fetching is ongoing, canceling it to be able to fetch single items!");
                iCollection.iMyVideosMdsDb->Cancel();
                aListFetchingWasCanceled = ETrue;
                }

            videoToCache = iCollection.iMyVideosMdsDb->CreateVideoL( 
                    aMdsIds[j], EFalse /* brief details */);
                                
            if ( videoToCache )
                {
                CleanupStack::PushL( videoToCache ); // 1->

                TInt err = AddL( videoToCache, sortingOrder );
                
                if ( err == KErrNone )
                    {
                    CleanupStack::Pop( videoToCache ); // <-1
                    }
                else
                    {
                    CleanupStack::PopAndDestroy( videoToCache ); // <-1
                    }
                    
                MPX_DEBUG3("CVcxMyVideosVideoCache:: %d (pointer = %x) added to cache", aMdsIds[j], videoToCache);
                }
            else
                {
                MPX_DEBUG2("CVcxMyVideosVideoCache:: video with %d ID not found from MDS -> skipping add to cache and deleting from id array", aMdsIds[j] );
                aMdsIds.Remove( j );
                }
            }
        else
            {
            MPX_DEBUG2("CVcxMyVideosVideoCache:: %d was already in cache -> skipping add, and deleting from id array", aMdsIds[j]);
            aMdsIds.Remove( j );
            }
        }

    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::AddL()
// This is for single adds, video list fetching does not call this.
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::AddL( CMPXMedia* aVideo, TVcxMyVideosSortingOrder aSortingOrder )
    {
    MPX_FUNC("CVcxMyVideosVideoCache::AddL");
    
    if ( iVideoListIsPartial )
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: iVideoList is partial, adding to iPartialVideoList");
        return AddToPartialListL( aVideo );
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: iVideoList is complete, adding to iVideoList");
        return AddToCorrectPlaceL( *aVideo, aSortingOrder );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::CheckForPartialVideoListItemsL()
// This is called by iCollection during video list fetching.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::CheckForPartialVideoListItemsL( TInt aNewItemsStartIndex )
    {
    // Replace items in iVideoList with iPartialVideoList items.
    if ( !iVideoList || !iVideoList->IsSupported( KMPXMediaArrayContents ) )
        {
        return;
        }
        
    CMPXMediaArray* videoArray = iVideoList->Value<CMPXMediaArray>(
                                KMPXMediaArrayContents);
    
    TInt videoArrayCount = videoArray->Count();

    MPX_DEBUG2("  videoArrayCount           = %d", videoArrayCount);
    MPX_DEBUG2("  iPartialVideoList.Count() = %d", iPartialVideoList.Count());
    
    for ( TInt i = iPartialVideoList.Count()-1; i >= 0 ; i-- )
        {
        for ( TInt j = aNewItemsStartIndex; j < videoArrayCount; j++ )
            {
            if ( TVcxMyVideosCollectionUtil::IdL( *(iPartialVideoList[i]) )
                    == TVcxMyVideosCollectionUtil::IdL( *(videoArray->AtL( j )) ) )
                {
                // Replace item in iVideoList with item from iPartialVideoList
                MPX_DEBUG2( "CVcxMyVideosVideoCache:: replacing %d, in iVideoList with item from iPartialVideoList",
                        TVcxMyVideosCollectionUtil::IdL( *(iPartialVideoList[i]) ).iId1 );

                if ( j == videoArray->Count() -1 )
                    {
                    videoArray->Remove( j );
                    videoArray->AppendL( iPartialVideoList[i] );
                    }
                else
                    {
                    videoArray->Remove( j );
                    videoArray->Insert( iPartialVideoList[i], j );
                    }
                iPartialVideoList.Remove( i );
                iPartialVideoList.Compress();                    
                break; // found -> break from for loop
                }
            }
        }    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::PosOnPartialVideoListL()
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::PosOnPartialVideoListL( CMPXMedia& aVideo )
    {
    TInt count    = iPartialVideoList.Count();
    TUint32 mdsId = TVcxMyVideosCollectionUtil::IdL( aVideo );
    
    for ( TInt i = 0; i < count; i++ )
        {
        if ( TVcxMyVideosCollectionUtil::IdL( *(iPartialVideoList[i]) ).iId1 == mdsId )
            {
            return i;
            }        
        }
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::PosOnVideoListL()
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::PosOnVideoListL( CMPXMedia& aVideo )
    {
    CMPXMediaArray* videoArray = iVideoList->Value<CMPXMediaArray>(
            KMPXMediaArrayContents);

    TInt count    = videoArray->Count();
    TUint32 mdsId = TVcxMyVideosCollectionUtil::IdL( aVideo );
    
    for ( TInt i = 0; i < count; i++ )
        {
        if ( TVcxMyVideosCollectionUtil::IdL( *(videoArray->AtL( i )) ).iId1 == mdsId )
            {
            return i;
            }        
        }
    return KErrNotFound;
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::AddToPartialListL()
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::AddToPartialListL( CMPXMedia* aVideo )
    {
    TInt pos = PosOnPartialVideoListL( *aVideo );
     
    TInt result;
    if ( pos == KErrNotFound )
        {
        iPartialVideoList.Append( aVideo );
        result = KErrNone;
        }
    else
        {
        result = KErrAlreadyExists;
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::DeletePartialList()
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::DeletePartialList()
    {
    TInt count( iPartialVideoList.Count() );
    for ( TInt i = count -1; i >= 0; i-- )
        {
        delete iPartialVideoList[i];
        iPartialVideoList[i] = NULL;
        }
    iPartialVideoList.Close();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::CreateVideoListL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::CreateVideoListL( TBool aForce )
    {
    MPX_FUNC("CVcxMyVideosVideoCache::CreateVideoListL");

    MPX_DEBUG2("CVcxMyVideosVideoCache:: aForce = %d", aForce );
    
    TVcxMyVideosSortingOrder sortingOrder = SortingOrderL();
        
    if ( IsFetchingVideoList
            && sortingOrder == iLastSortingOrder && !aForce )
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: iVideoList creation is already ongoing, skipping");
        return;
        }
                   
    if ( iVideoListIsPartial || (sortingOrder != iLastSortingOrder) || aForce )
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: iVideoList was partial or in wrong order or aForce was ETrue, recreating");

        if ( IsFetchingVideoList )
            {
            MPX_DEBUG1("CVcxMyVideosVideoCache:: video list fetching is ongoing, canceling it");
            iCollection.iMyVideosMdsDb->Cancel( CVcxMyVideosMdsDb::EGetVideoList );
            IsFetchingVideoList = EFalse;
            }
        
        ResetVideoListL();
                        
        TBool ascending = ETrue;
        
        switch ( sortingOrder )
            {
            case EVcxMyVideosSortingId:
            case EVcxMyVideosSortingName:
            case EVcxMyVideosSortingSize:
                ascending = ETrue;
                break;
            case EVcxMyVideosSortingCreationDate:
            case EVcxMyVideosSortingModified:
                ascending = EFalse;
                break;
            }
            
        iCollection.iMyVideosMdsDb->CreateVideoListL( 
                sortingOrder,
                ascending,
                EFalse /* brief list */,
                iVideoList /* use existing */ );

        IsFetchingVideoList = ETrue;
        iLastSortingOrder   = sortingOrder;
        iVideoListIsPartial = ETrue;
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: iVideoList is complete and in right sorting order, doing nothing");
        }              
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::UpdateVideoL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosVideoCache::UpdateVideoL( CMPXMedia& aVideo )
    {
    MPX_FUNC("CVcxMyVideosVideoCache::UpdateVideoL");
    
    TInt pos;
    CMPXMedia* videoInCache = FindVideoByMdsIdL( aVideo.ValueTObjectL<TMPXItemId>(
            KMPXMediaGeneralId ).iId1, pos );
    if ( !videoInCache )
        {
        MPX_DEBUG2("CVcxMyVideosVideoCache:: MDS ID %d not found from cache, leaving",
            aVideo.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ).iId1 );
        User::Leave( KErrNotFound );
        }

    TBool changed = EFalse;
        
    // 1 MPX ID cant be changed
    // 2 
    if ( aVideo.IsSupported( KMPXMediaGeneralTitle ) )
        {
        if ( videoInCache->ValueText( KMPXMediaGeneralTitle ) !=
                aVideo.ValueText( KMPXMediaGeneralTitle ) )
            {
            videoInCache->SetTextValueL( KMPXMediaGeneralTitle,
                    aVideo.ValueText( KMPXMediaGeneralTitle ) );

            //updates sorting order and category attributes if necessarry
            HandleVideoTitleModifiedL( videoInCache );

            changed = ETrue;
            }
        }
    // 3
    if ( aVideo.IsSupported( KMPXMediaGeneralComment ) )
        {
        if ( videoInCache->ValueText( KMPXMediaGeneralComment ) != 
                aVideo.ValueText( KMPXMediaGeneralComment ) )
            {
            videoInCache->SetTextValueL( KMPXMediaGeneralComment,
                    aVideo.ValueText( KMPXMediaGeneralComment ) );
            changed = ETrue;
            }
        }
    // 4
    if ( aVideo.IsSupported( KMPXMediaGeneralUri ) )
        {
        MPX_DEBUG2("CVcxMyVideosVideoCache:: old file path: %S", &videoInCache->ValueText( KMPXMediaGeneralUri ));
        MPX_DEBUG2("CVcxMyVideosVideoCache:: new file path: %S", &aVideo.ValueText( KMPXMediaGeneralUri ));

        if ( videoInCache->ValueText( KMPXMediaGeneralUri ) !=
               aVideo.ValueText( KMPXMediaGeneralUri ) ) 
            {
#if 0 // this should be in mds, or mds should keep title empty if it is not set by client
            TParse parse;
            HBufC* oldTitle = HBufC::NewL( videoInCache->ValueText( KMPXMediaGeneralTitle ).Length() );
            CleanupStack::PushL( oldTitle ); // 1->
            oldTitle->Des() = videoInCache->ValueText( KMPXMediaGeneralTitle );
            oldTitle->Des().LowerCase();
            
            parse.Set( videoInCache->ValueText( KMPXMediaGeneralUri ), NULL, NULL );
            MPX_DEBUG2("CVcxMyVideosVideoCache:: title generated from old file name: %S", &parse.Name());
            MPX_DEBUG2("CVcxMyVideosVideoCache:: old title (lower cased): %S", oldTitle);
            if ( parse.Name() == *oldTitle )
                {
                // filename has been used as a title -> update title also to new filename
                MPX_DEBUG1("CVcxMyVideosVideoCache:: filename has been used as a title -> changing title to new filename");
                parse.Set( aVideo.ValueText( KMPXMediaGeneralUri ), NULL, NULL );
                videoInCache->SetTextValueL( KMPXMediaGeneralTitle, parse.Name() );
                
                //updates sorting order and category attributes if necessarry
                HandleVideoTitleModifiedL( videoInCache );
                }
            CleanupStack::PopAndDestroy( oldTitle ); // <-1
#endif
            
            videoInCache->SetTextValueL( KMPXMediaGeneralUri,
                    aVideo.ValueText( KMPXMediaGeneralUri ) );
    
            changed = ETrue;
            }
        }
    // 5
	// With 64bit file support this is obsolete.
	// Can be removed if this attribute is removed from MPX framework code
    if ( aVideo.IsSupported( KMPXMediaGeneralSize ) )
        {
        if ( aVideo.ValueTObjectL<TInt>( KMPXMediaGeneralSize ) !=
                videoInCache->ValueTObjectL<TInt>( KMPXMediaGeneralSize ) )
            {
            videoInCache->SetTObjectValueL<TInt>( KMPXMediaGeneralSize,
                    aVideo.ValueTObjectL<TInt>( KMPXMediaGeneralSize ) );

#ifndef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
            TVcxMyVideosSortingOrder sortingOrder = SortingOrderL();
            if ( sortingOrder == EVcxMyVideosSortingSize )
                {
                MPX_DEBUG1("CVcxMyVideosVideoCache:: size updated && sorting order is by size, moving video to correct place on cache");
                MoveToCorrectPlaceL( *videoInCache, sortingOrder );
                videoInCache = FindVideoByMdsIdL( aVideo.ValueTObjectL<TMPXItemId>(
                        KMPXMediaGeneralId ).iId1, pos );

                iCollection.CategoriesL().GenerateCategoryModifiedEventL(
                        *videoInCache, ETrue /* flush */, EVcxMyVideosVideoListOrderChanged );
                }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
            changed = ETrue;
            }
        }

    // 6 Creation date changes values when item is moved from one memory to another.
    //   The first the object is created to MDS with current date, and when file is added
    //   to file system the value is changed to files creation date.
    if ( aVideo.IsSupported( KMPXMediaGeneralDate ) )
        {
        TInt64 newCreationDate;
        TInt64 oldCreationDate;
        newCreationDate = aVideo.ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
        oldCreationDate = videoInCache->ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
        if ( newCreationDate != oldCreationDate )
            {
            videoInCache->SetTObjectValueL<TInt64>( KMPXMediaGeneralDate,
                newCreationDate );

            TVcxMyVideosSortingOrder sortingOrder = SortingOrderL();
            if ( sortingOrder == EVcxMyVideosSortingCreationDate )
                {
                MPX_DEBUG1("CVcxMyVideosVideoCache:: creation date updated && sorting order is by creation date, moving video to correct place on cache");
                MoveToCorrectPlaceL( *videoInCache, sortingOrder );
                videoInCache = FindVideoByMdsIdL( aVideo.ValueTObjectL<TMPXItemId>(
                        KMPXMediaGeneralId ).iId1, pos );

                iCollection.CategoriesL().GenerateCategoryModifiedEventL(
                        *videoInCache, ETrue /* flush */, EVcxMyVideosVideoListOrderChanged );
                }

            changed = ETrue;
            }
        }

    // 7
    if ( aVideo.IsSupported( KMPXMediaGeneralFlags ) )
        {
        TUint32 oldFlags = videoInCache->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
        TUint32 newFlags = aVideo.ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );

        MPX_DEBUG3("CVcxMyVideosVideoCache:: %x -> %x", oldFlags, newFlags);

        // Play pos is written only after playback. Reset new video flag
        // now, so there's no need for changing the flag explitically
        if ( aVideo.IsSupported( KMPXMediaGeneralLastPlaybackPosition ) )
            {
            newFlags &= ~EVcxMyVideosVideoNew;
            MPX_DEBUG3("CVcxMyVideosVideoCache:: %x -> %x", oldFlags, newFlags);
            }

        if ( oldFlags != newFlags )
            {        
            videoInCache->SetTObjectValueL<TUint32>( KMPXMediaGeneralFlags, newFlags );

            if ( (newFlags & EVcxMyVideosVideoNew) != (oldFlags & EVcxMyVideosVideoNew) )
                {
                MPX_DEBUG1("CVcxMyVideosVideoCache:: new video flag modified-> updating categories' new video names and counters");
    
                TBool modified = EFalse;
                iCollection.CategoriesL().UpdateCategoryNewVideoNameAndDateL(
                        *videoInCache, modified );
    
                iCollection.CategoriesL().NewVideoFlagChangedL(
                        oldFlags, newFlags,
                        videoInCache->ValueTObjectL<TUint8>( KVcxMediaMyVideosOrigin ),
                        modified );
                
                if ( !(newFlags & EVcxMyVideosVideoNew) && 
                        TVcxMyVideosCollectionUtil::OriginL( *videoInCache ) == EVcxMyVideosOriginDownloaded )
                    {
                    MPX_DEBUG1("CVcxMyVideosVideoCache::UpdateVideoL - Count of new videos in Downloaded origin has decreased, sending mediator event to notification launcher");                   
                    iCollection.NotifyNewVideosCountDecreasedL( *videoInCache );
                    }
                }
            changed = ETrue;
            }
        }
    // 8
    if ( aVideo.IsSupported( KMPXMediaGeneralCopyright ) )
        {
        if ( videoInCache->ValueText( KMPXMediaGeneralCopyright ) !=
                aVideo.ValueText( KMPXMediaGeneralCopyright ) )
            {
            videoInCache->SetTextValueL( KMPXMediaGeneralCopyright,
                    aVideo.ValueText( KMPXMediaGeneralCopyright ) );
            changed = ETrue;
            }
        }
#if 0 // type cant be changed
    // 9
    if ( aVideo.IsSupported( KMPXMediaGeneralMimeType ) )
        {
        videoInCache->SetTextValueL( aVideo.ValueText( KMPXMediaGeneralMimeType ) );
        changed = ETrue;
        }
#endif

    // 11
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
    if ( aVideo.IsSupported( KMPXMediaGeneralExtSizeInt64 ) )
        {
        if ( aVideo.ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 ) !=
                videoInCache->ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 ) )
            {
            videoInCache->SetTObjectValueL<TInt64>( KMPXMediaGeneralExtSizeInt64,
                    aVideo.ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 ) );

            TVcxMyVideosSortingOrder sortingOrder = SortingOrderL();
            if ( sortingOrder == EVcxMyVideosSortingSize )
                {
                MPX_DEBUG1("CVcxMyVideosVideoCache:: size updated && sorting order is by size, moving video to correct place on cache");
                MoveToCorrectPlaceL( *videoInCache, sortingOrder );
                videoInCache = FindVideoByMdsIdL( aVideo.ValueTObjectL<TMPXItemId>(
                        KMPXMediaGeneralId ).iId1, pos );

                iCollection.CategoriesL().GenerateCategoryModifiedEventL(
                        *videoInCache, ETrue /* flush */, EVcxMyVideosVideoListOrderChanged );
                }
            changed = ETrue;
            }
        }
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

    // 12
    if ( aVideo.IsSupported( KVcxMediaMyVideosModifiedDate ) )
        {
        if ( videoInCache->ValueTObjectL<TInt64>( KVcxMediaMyVideosModifiedDate ) !=
                aVideo.ValueTObjectL<TInt64>( KVcxMediaMyVideosModifiedDate ) )
            {
            videoInCache->SetTObjectValueL<TInt64>( KVcxMediaMyVideosModifiedDate,
                    aVideo.ValueTObjectL<TInt64>( KVcxMediaMyVideosModifiedDate ) );
            changed = ETrue;
            }
        }
    // 13
    if ( aVideo.IsSupported( KVcxMediaMyVideosAgeProfile ) )
        {
        if ( videoInCache->ValueTObjectL<TInt32>( KVcxMediaMyVideosAgeProfile ) != 
                aVideo.ValueTObjectL<TInt32>( KVcxMediaMyVideosAgeProfile ) )
            {
            videoInCache->SetTObjectValueL<TInt32>( KVcxMediaMyVideosAgeProfile,
                    aVideo.ValueTObjectL<TInt32>( KVcxMediaMyVideosAgeProfile ) );
            changed = ETrue;
            }
        }
    // 14
    if ( aVideo.IsSupported( KVcxMediaMyVideosAudioLanguage ) )
        {
        if ( videoInCache->ValueText( KVcxMediaMyVideosAudioLanguage ) != 
                aVideo.ValueText( KVcxMediaMyVideosAudioLanguage ) )
            {
            videoInCache->SetTextValueL( KVcxMediaMyVideosAudioLanguage,
                    aVideo.ValueText( KVcxMediaMyVideosAudioLanguage ) );
            changed = ETrue;
            }
        }
    // 15
    if ( aVideo.IsSupported( KVcxMediaMyVideosAuthor ) )
        {
        if ( videoInCache->ValueText( KVcxMediaMyVideosAuthor ) !=
                aVideo.ValueText( KVcxMediaMyVideosAuthor ))
            {
            videoInCache->SetTextValueL( KVcxMediaMyVideosAuthor,
                    aVideo.ValueText( KVcxMediaMyVideosAuthor ) );
            changed = ETrue;
            }
        }

    // 16
    if ( aVideo.IsSupported( KVcxMediaMyVideosOrigin ) )
        {
        TUint8 prevOrigin( TVcxMyVideosCollectionUtil::OriginL( *videoInCache ) );
        TUint8 newOrigin( TVcxMyVideosCollectionUtil::OriginL( aVideo ) );
                    
        if ( newOrigin != prevOrigin )
            {
            videoInCache->SetTObjectValueL<TUint8>( KVcxMediaMyVideosOrigin, newOrigin );
            iCollection.CategoriesL().OriginChangedL( *videoInCache, prevOrigin, newOrigin );            
            changed = ETrue;
            }
        }

    // 17
    if ( aVideo.IsSupported( KVcxMediaMyVideosDuration ) )
        {
        TReal32 durationInCache = TVcxMyVideosCollectionUtil::DurationL( *videoInCache );
        TReal32 newDuration     = TVcxMyVideosCollectionUtil::DurationL( aVideo );
        
        MPX_DEBUG2("SETTING DURATION TO: %f", newDuration );
        
        if ( durationInCache != newDuration )
            {
            videoInCache->SetTObjectValueL<TReal32>( KVcxMediaMyVideosDuration, newDuration );
            changed = ETrue;
            }
        }
    // 10
    if ( aVideo.IsSupported( KMPXMediaGeneralLastPlaybackPosition ) )
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache::UpdateVideo Last play pos is supported");
        
        if ( videoInCache->ValueTObjectL<TInt>( KMPXMediaGeneralLastPlaybackPosition ) !=
                aVideo.ValueTObjectL<TInt>( KMPXMediaGeneralLastPlaybackPosition ))
            {
            videoInCache->SetTObjectValueL<TInt>( KMPXMediaGeneralLastPlaybackPosition,
                    aVideo.ValueTObjectL<TInt>( KMPXMediaGeneralLastPlaybackPosition ) );
            changed = ETrue;
            }
        }
    // 18
    if ( aVideo.IsSupported( KVcxMediaMyVideosDownloadId ) )
        {
        TUint32 downloadIdInCache( TVcxMyVideosCollectionUtil::DownloadIdL( *videoInCache ) );
        TUint32 downloadIdInNew( TVcxMyVideosCollectionUtil::DownloadIdL( aVideo ) );
        if (  downloadIdInNew != downloadIdInCache )
            {
            videoInCache->SetTObjectValueL<TUint32>( KVcxMediaMyVideosDownloadId,
                    downloadIdInNew );

            if ( downloadIdInNew == 0 )
                {
                iDownloadCache->Delete( videoInCache );
                }
            changed = ETrue;
            }
        }
    // 19
    if ( aVideo.IsSupported( KVcxMediaMyVideosRating ) )
        {
        TUint8 ratingInCache( TVcxMyVideosCollectionUtil::RatingL( *videoInCache ) );
        TUint8 ratingInNew( TVcxMyVideosCollectionUtil::RatingL( aVideo ) );
        if (  ratingInNew != ratingInCache )
            {
            videoInCache->SetTObjectValueL<TUint32>( KVcxMediaMyVideosRating,
                    ratingInNew );

            changed = ETrue;
            }
        }

    // 20
    if ( aVideo.IsSupported( KMPXMediaVideoBitRate ) )
        {
        TUint16 bitRate = aVideo.ValueTObjectL<TUint16>( KMPXMediaVideoBitRate ); 
        if ( bitRate != videoInCache->ValueTObjectL<TUint16>( KMPXMediaVideoBitRate ) )
            {
            videoInCache->SetTObjectValueL<TUint16>( KMPXMediaVideoBitRate,
                                                     bitRate );
            
            changed = ETrue;
            }
        }

    // 21
    if ( aVideo.IsSupported( KVcxMediaMyVideosAudioFourCc ) )
        {
        TUint32 AudioFourCcInCache( TVcxMyVideosCollectionUtil::DownloadIdL( *videoInCache ) );
        TUint32 AudioFourCcInNew( TVcxMyVideosCollectionUtil::AudioFourCcL( aVideo ) );
        if (  AudioFourCcInNew != AudioFourCcInCache )
            {
            videoInCache->SetTObjectValueL<TUint32>( KVcxMediaMyVideosAudioFourCc,
                    AudioFourCcInNew );

            if ( AudioFourCcInNew == 0 )
                {
                iDownloadCache->Delete( videoInCache );
                }
            changed = ETrue;
            }
        }

    // 22
    if ( aVideo.IsSupported( KMPXMediaVideoWidth ) )
        {
        TUint16 width = aVideo.ValueTObjectL<TUint16>( KMPXMediaVideoWidth ); 
        if ( width != videoInCache->ValueTObjectL<TUint16>( KMPXMediaVideoWidth ) )
            {
            videoInCache->SetTObjectValueL<TUint16>( KMPXMediaVideoWidth,
                    width );
            
            changed = ETrue;
            }
        }
    
    // 22
    if ( aVideo.IsSupported( KMPXMediaVideoHeight ) )
        {
        TUint16 height = aVideo.ValueTObjectL<TUint16>( KMPXMediaVideoHeight ); 
        if ( height != videoInCache->ValueTObjectL<TUint16>( KMPXMediaVideoHeight ) )
            {
            videoInCache->SetTObjectValueL<TUint16>( KMPXMediaVideoHeight,
                    height );
            
            changed = ETrue;
            }
        }
                
    // 24 
    if ( aVideo.IsSupported( KMPXMediaVideoArtist ) )
        {
        if ( videoInCache->ValueText( KMPXMediaVideoArtist ) !=
                aVideo.ValueText( KMPXMediaVideoArtist ) )
            {
            videoInCache->SetTextValueL( KMPXMediaVideoArtist,
                    aVideo.ValueText( KMPXMediaVideoArtist ) );

            changed = ETrue;
            }
        }
        
    return changed;    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::HandleVideoTitleModifiedL
// Updates sorting order and category attributes if necessarry
// NOTICE that aVideoInCache is not allowed to own the media object, since this
// function may change the pointer value to point to a new object.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::HandleVideoTitleModifiedL( CMPXMedia*& aVideoInCache )
    {
    MPX_DEBUG1("CVcxMyVideosVideoCache:: video title in cache modified");
    MPX_DEBUG1("                         -> updating categories new video names");

    TBool modified = EFalse;
    iCollection.CategoriesL().UpdateCategoryNewVideoNameAndDateL(
            *aVideoInCache, modified );

    TVcxMyVideosSortingOrder sortingOrder = SortingOrderL();
    if ( sortingOrder == EVcxMyVideosSortingName )
        {
        MPX_DEBUG1(" Title modified and sorting by Name -> moving item to correct place");
        
        TMPXItemId mpxId( aVideoInCache->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
        
        // Deletes the old media object and creates a new one to a new position
        MoveToCorrectPlaceL( *aVideoInCache, SortingOrderL() );
        // Find the new object
        TInt pos;
        aVideoInCache = FindVideoByMdsIdL( mpxId.iId1, pos );
        
        iCollection.CategoriesL().GenerateCategoryModifiedEventL(
                *aVideoInCache, ETrue /* flush */, EVcxMyVideosVideoListOrderChanged );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::FindCorrectPositionL
// Finds the correct position using bisection method.
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::FindCorrectPositionL( CMPXMedia& aMedia,
        TVcxMyVideosSortingOrder aSortingOrder )
    {
    MPX_FUNC("CVcxMyVideosVideoCache::FindCorrectPositionL()");
            
    CMPXMediaArray* cachedVideoArray = iVideoList->Value<CMPXMediaArray>(
                                KMPXMediaArrayContents);

    TInt videoCount = cachedVideoArray->Count();

    MPX_DEBUG2("CVcxMyVideosVideoCache:: video count = %d", videoCount);

    if ( videoCount == 0 )
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: video count was 0, returning pos 0");
        return 0;
        }
        
    TInt stepSize     = 0;
    TInt upLimit      = videoCount - 1; //highest possible place
    TInt downLimit    = 0;              //lowest possible place
    TInt pos          = videoCount / 2; // start from the middle
    
    if ( videoCount > 5 )
        {
        stepSize = videoCount / 4; // we start at middle, the first step size is half of the middle = 1/4
        MPX_DEBUG2("CVcxMyVideosVideoCache:: stepsize: %d", stepSize);
        }
    else
        {
        stepSize = 1;
        MPX_DEBUG1("CVcxMyVideosVideoCache:: Moving with single steps.");
        MPX_DEBUG2("CVcxMyVideosVideoCache:: stepsize: %d", stepSize);

        if ( videoCount == 1 )
            {
            MPX_DEBUG1("CVcxMyVideosVideoCache:: only one video -> stepSize = 0");
            pos      = 0;
            stepSize = 0;
            }
        }
    
    MPX_DEBUG2("CVcxMyVideosVideoCache:: start pos = %d", pos);

    TBool found = EFalse;
    TInt compare;
    while ( !found )
        {
        compare = CompareL( aMedia, *cachedVideoArray->AtL( pos ), aSortingOrder ); 
        if ( compare == 0 )
            {
            MPX_DEBUG1("CVcxMyVideosVideoCache:: equal item found");
            found = ETrue;
            break;
            }            
        else if ( compare == 1 )
            {
            MPX_DEBUG1("CVcxMyVideosVideoCache:: greater item found");
            upLimit = pos - 1;
            if ( upLimit < 0 )
                {
                upLimit = 0;
                }
            if ( upLimit < downLimit )
                {
                upLimit = downLimit;
                }
            pos -= stepSize;
            if ( pos < downLimit )
                {
                pos += stepSize;
                MPX_DEBUG1("CVcxMyVideosVideoCache:: hit the limiter");
                }
            }
        else if ( compare == -1 )
            {
            MPX_DEBUG1("CVcxMyVideosVideoCache:: smaller item found");
            downLimit = pos + 1;
            if ( downLimit > videoCount - 1 )
                {
                downLimit = videoCount - 1;
                }
            if ( downLimit > upLimit )
                {
                downLimit = upLimit;
                }
            pos += stepSize;
            if ( pos > upLimit )
                {
                pos -= stepSize;
                MPX_DEBUG1("CVcxMyVideosVideoCache:: hit the limiter");
                }
            }
        MPX_DEBUG3("CVcxMyVideosVideoCache:: item is between: %d - %d", downLimit, upLimit );            
        MPX_DEBUG2("CVcxMyVideosVideoCache:: new pos = %d", pos);
        
        if ( downLimit == upLimit )
            {
            MPX_DEBUG1("CVcxMyVideosVideoCache:: downLimit and upLimit are the same, this is last item to check");
            compare = CompareL( aMedia, *cachedVideoArray->AtL( pos ), aSortingOrder ); 

            if ( compare == 1 )
                {
                MPX_DEBUG1("CVcxMyVideosVideoCache:: the last possible item had greater value -> this is the correct place");
                }
            else
                {
                MPX_DEBUG1("CVcxMyVideosVideoCache:: last possible item had smaller value (or equal) -> adding one to pos");
                pos++;
                }
            found = ETrue;
            break;
            }

        stepSize = (upLimit - downLimit) / 4;
        MPX_DEBUG2("CVcxMyVideosVideoCache:: stepsize: %d", stepSize);
            
        if ( stepSize < 1 )
            {
            MPX_DEBUG1("CVcxMyVideosVideoCache:: Moving with single steps.");
            stepSize = 1;
            MPX_DEBUG2("CVcxMyVideosVideoCache:: stepsize: %d", stepSize);
            }

        MPX_DEBUG1("CVcxMyVideosVideoCache:: ---- new round ----");
        }        
    MPX_DEBUG2("CVcxMyVideosVideoCache:: returning pos %d", pos);
    return pos;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::CompareL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::CompareL( CMPXMedia& aNewVideo, CMPXMedia& aVideoInList,
        TVcxMyVideosSortingOrder aSortingOrder )
    {
    TInt result = 0;
    
    switch ( aSortingOrder )
        {
        case EVcxMyVideosSortingCreationDate:
            {
            TInt64 newItemsDate  = aNewVideo.ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
            TInt64 listItemsDate = aVideoInList.ValueTObjectL<TInt64>( KMPXMediaGeneralDate );
            MPX_DEBUG3("CVcxMyVideosVideoCache:: new date = %d, list item date = %d", newItemsDate, listItemsDate);
            if ( listItemsDate > newItemsDate /*static_cast<TInt64>(listItemsDate) > static_cast<TInt64>(newItemsDate)*/ )
                {
                result = -1;
                }
            else
                {
                if ( listItemsDate == newItemsDate/*static_cast<TInt64>(listItemsDate) == static_cast<TInt64>(newItemsDate)*/ )
                    {
                    result = 0;
                    }
                else
                    {
                    result = 1;
                    }
                }
            }              
            break;
            
        case EVcxMyVideosSortingName:
            {
            TPtrC newVideoName( aNewVideo.ValueText( KMPXMediaGeneralTitle ) );
            TPtrC videoInListName( aVideoInList.ValueText( KMPXMediaGeneralTitle ));
            MPX_DEBUG3("CVcxMyVideosVideoCache:: newVideoName = %S, videoInListName = %S", &newVideoName, &videoInListName);
            TCollationMethod collationMethod = *Mem::CollationMethodByIndex(0); // get the standard method
            collationMethod.iFlags |= TCollationMethod::EFoldCase;
            result = newVideoName.CompareC( videoInListName, 3, &collationMethod) * -1;
            if ( result < 0 )
                {
                result = -1;
                }
            if ( result > 0 )
                {
                result = 1;
                }
            }
            break;
            
        case EVcxMyVideosSortingSize:
            {
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
            TInt64 newItemsSize  = aNewVideo.ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 );
            TInt64 listItemsSize = aVideoInList.ValueTObjectL<TInt64>( KMPXMediaGeneralExtSizeInt64 );
#else
            TInt newItemsSize  = aNewVideo.ValueTObjectL<TInt>( KMPXMediaGeneralSize );
            TInt listItemsSize = aVideoInList.ValueTObjectL<TInt>( KMPXMediaGeneralSize );
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
            MPX_DEBUG3("CVcxMyVideosVideoCache:: newItemsSize = %ld, listItemsSize = %ld", newItemsSize, listItemsSize);
            if ( listItemsSize > newItemsSize )
                {
                result = 1;
                }
            else
                {
                if ( listItemsSize == newItemsSize )
                    {
                    result = 0;
                    }
                else
                    {
                    result = -1;
                    }
                }
            }              
            break;
        }
        
    return result;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::SortingOrderL
// ----------------------------------------------------------------------------
//    
TVcxMyVideosSortingOrder CVcxMyVideosVideoCache::SortingOrderL()
    {

    if( !iCenRep ) 
        {
        TUid uid;
        uid.iUid = KVcxMyVideosCollectionCenrepUid;
		iCenRep = CRepository::NewL( uid );
        }
    
    TInt sortingOrderInCenrep;

    TInt cenRepError = iCenRep->Get( KVcxMyVideosCollectionCenrepKeySortingOrder,
            sortingOrderInCenrep );
    
    TVcxMyVideosSortingOrder sortingOrder;
    
    if ( cenRepError != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosVideoCache:: error %d while reading sorting order from Cenrep", cenRepError );
        sortingOrder = EVcxMyVideosSortingCreationDate;
        }
    else
        {
        sortingOrder = static_cast<TVcxMyVideosSortingOrder>( sortingOrderInCenrep );
        }
    return sortingOrder;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::AddToCorrectPlaceL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::AddToCorrectPlaceL( CMPXMedia& aVideo,
        TVcxMyVideosSortingOrder aSortingOrder, TBool aUpdateCategories )
    {    
    if ( PosOnVideoListL( aVideo ) != KErrNotFound )
        {
        MPX_DEBUG2( "CVcxMyVideosVideoCache::AddToCorrectPlaceL() %d is already on iVideoList, skipping add",
                TVcxMyVideosCollectionUtil::IdL( aVideo ).iId1 );
        return KErrAlreadyExists;
        }
        
    TInt pos = FindCorrectPositionL( aVideo, aSortingOrder );

    CMPXMediaArray* cachedVideoArray = NULL;            
    cachedVideoArray = iVideoList->Value<CMPXMediaArray>(
            KMPXMediaArrayContents );

    if ( pos > (cachedVideoArray->Count() -1) )
        {
        cachedVideoArray->AppendL( aVideo );                    
        }
    else
        {
        cachedVideoArray->InsertL( aVideo, pos );
        }
        
    if ( aUpdateCategories )
        {
        iCollection.CategoriesL().VideoAddedL( aVideo );
        }

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::MoveToCorrectPlaceL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::MoveToCorrectPlaceL( CMPXMedia& aVideo,
        TVcxMyVideosSortingOrder aSortingOrder )
    {
    if ( PosOnPartialVideoListL( aVideo ) != KErrNotFound )
        {
        MPX_DEBUG2("CVcxMyVideosVideoCache::MoveToCorrectPlaceL() item %d was on iPartialVideoList, not moving ",
                TVcxMyVideosCollectionUtil::IdL( aVideo ).iId1);
        return;
        }
        
    CMPXMedia* video = CMPXMedia::NewL( aVideo );
    CleanupStack::PushL( video ); // 1->
    RemoveL( *video, EFalse /* dont update categories */ );
    AddToCorrectPlaceL( *video, aSortingOrder, EFalse /* dont update categories */ );
    CleanupStack::PopAndDestroy( video ); // <-1
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::RemoveL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::RemoveL( CMPXMedia& aVideo, TBool aUpdateCategories )
    {
    return RemoveL( TVcxMyVideosCollectionUtil::IdL( aVideo ).iId1, aUpdateCategories );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::RemoveL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::RemoveL( TUint32 aMdsId, TBool aUpdateCategories )
    {
    MPX_FUNC("CVcxMyVideosVideoCache::RemoveL");
    
    TInt pos;
    CMPXMedia* video = FindVideoByMdsIdL( aMdsId, pos );

    if ( !video )
        {
        MPX_DEBUG1("CVcxMyVideosVideoCache:: RemoveL failed since the item wasn't on cache");
        return KErrNotFound;
        }

    iDownloadCache->Delete( video );

    TUint32 flags( 0 );
	if ( video->IsSupported( KMPXMediaGeneralFlags ) )
	    {
		flags = video->ValueTObjectL<TUint32>( KMPXMediaGeneralFlags );
		}
    
    if ( ( flags & EVcxMyVideosVideoNew ) && 
            TVcxMyVideosCollectionUtil::OriginL( *video ) == EVcxMyVideosOriginDownloaded )
        {       
        MPX_DEBUG1("CVcxMyVideosVideoCache::RemoveL - New video has been removed from Downloaded origin -> sending mediator event to notification launcher");
        iCollection.NotifyNewVideosCountDecreasedL( *video );
        }
    
    if ( aUpdateCategories &&
            pos != KErrNotFound /* no need to update if item is on iPartialVideoList*/ )
        {
        iCollection.CategoriesL().VideoRemovedL( *video );
        }
        
    if ( pos != KErrNotFound )
        {
        CMPXMediaArray* cachedVideoArray = iVideoList->Value<CMPXMediaArray>(
                KMPXMediaArrayContents );
        
        MPX_DEBUG2("CVcxMyVideosVideoCache:: %d removed from iVideoList", aMdsId);
        cachedVideoArray->Remove( pos );
        }
    else
        {
        MPX_DEBUG2("CVcxMyVideosVideoCache:: %d removed from iPartialVideoList", aMdsId);
        iPartialVideoList.Remove( PosOnPartialVideoListL( *video ) );
        iPartialVideoList.Compress();        
        }
        
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::RemoveL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::RemoveL( RArray<TUint32>& aMdsIds, TBool aUpdateCategories )
    {        
    for ( TInt i = 0; i < aMdsIds.Count(); i++ )
        {
        RemoveL( aMdsIds[i], aUpdateCategories );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::ResetVideoListL
// iVideoList stays the same, media array is deleted and recreated to 0 length,
// media items are deleted.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::ResetVideoListL()
    {
    MPX_FUNC("CVcxMyVideosVideoCache::ResetVideoListL");

    //TODO: when should we cancel...
    //iCollection.iMyVideosMdsDb->Cancel();

    CMPXMediaArray* mediaArray =
            iVideoList->ValueCObjectL<CMPXMediaArray>( KMPXMediaArrayContents );
    CleanupStack::PushL( mediaArray ); // 1->
    
    for ( TInt i = mediaArray->Count() - 1; i >= 0; i-- )
        {
        mediaArray->Remove( i );
        }        

    iVideoList->Delete( KMPXMediaArrayContents );
    iVideoList->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, mediaArray );

    CleanupStack::PopAndDestroy( mediaArray ); // <-1
    
    TInt count = iCollection.iOpenHandler->iVideoListsBeingOpened.Count();
    
    for ( TInt i = 0; i < count; i++ )
        {
        CMPXMedia* videoList = iCollection.iOpenHandler->iVideoListsBeingOpened[i];
        if ( videoList )
            {
            MPX_DEBUG3("CVcxMyVideosVideoCache:: reset iVideoListsBeingOpened[%d] category: %d",
                    i, iCollection.iOpenHandler->iCategoryIdsBeingOpened[i] );
            mediaArray = videoList->ValueCObjectL<CMPXMediaArray>( KMPXMediaArrayContents );
            CleanupStack::PushL( mediaArray ); // 1->
            for ( TInt i = mediaArray->Count() - 1; i >= 0; i-- )
                {
                mediaArray->Remove( i );
                }
            videoList->Delete( KMPXMediaArrayContents );
            videoList->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents,
                    mediaArray );
            CleanupStack::PopAndDestroy( mediaArray ); // <-1
            }
        }
        
    iVideoListIsPartial = ETrue;

    delete iDownloadCache;
    iDownloadCache = NULL;
    iDownloadCache = CVcxMyVideosDownloadCache::NewL();
                
    iCollection.CategoriesL().ResetVideoCountersL(); //this does not send events
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::ReCreateVideoListL
// iVideoList and its media array are deleted and recreated. New instances point to
// different global heap position. Media objects stay the same and point to
// same global heap.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::ReCreateVideoListL()
    {
    MPX_FUNC("CVcxMyVideosVideoCache::ReCreateVideoListL");
    
    CMPXMedia* newList = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    CleanupStack::PushL( newList ); // 1->

    CMPXMediaArray* oldArray = iVideoList->Value<CMPXMediaArray>(
            KMPXMediaArrayContents );
    TInt count = oldArray->Count();

    CMPXMediaArray* newArray = newList->Value<CMPXMediaArray>(
            KMPXMediaArrayContents );
    
    CMPXMedia* video;
    for ( TInt i = 0; i < count; i++ )
        {
        video = CMPXMedia::NewL( *oldArray->AtL( i ) ); // points to same heap object
        CleanupStack::PushL( video ); // 2->
        newArray->AppendL( video ); // ownership moves
        CleanupStack::Pop( video ); // <-2
        }
    
    delete iVideoList;
    iVideoList = NULL;
    iVideoList = newList;
    CleanupStack::Pop( newList ); // <-1

    delete iDownloadCache;
    iDownloadCache = NULL;
    iDownloadCache = CVcxMyVideosDownloadCache::NewL();
    }
// End of file

