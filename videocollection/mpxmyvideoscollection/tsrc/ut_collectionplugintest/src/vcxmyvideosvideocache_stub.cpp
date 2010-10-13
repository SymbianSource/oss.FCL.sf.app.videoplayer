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
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include "vcxmyvideosvideocache_stub.h"
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosopenhandler_stub.h"

// constants used for testing
const TInt KMPXIdLeave = 999;
const TInt KMPXIdNotFound = 998;
const TInt KMPXIdChanged = 997;


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// constructor.
// ----------------------------------------------------------------------------
//
TVcxMyVideosVideo::TVcxMyVideosVideo()
:iMdsId( 0 ), iVideo( NULL ), iPos( KErrNotFound )
    {
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosVideo::operator=
// ---------------------------------------------------------------------------
//                
TVcxMyVideosVideo& TVcxMyVideosVideo::operator=( const TVcxMyVideosVideo& aVideo )
    {
    Set( aVideo.iMdsId, aVideo.iPos, aVideo.iVideo );
    return *this;
    }

// ----------------------------------------------------------------------------
// TVcxMyVideosVideo::Set
// ----------------------------------------------------------------------------
//
void TVcxMyVideosVideo::Set( TUint32 aMdsId, TInt aPos, CMPXMedia* aVideo )
    {
    iMdsId = aMdsId;
    iPos   = aPos;
    iVideo = aVideo;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::NewL
// ----------------------------------------------------------------------------
//
CVcxMyVideosVideoListIndex* CVcxMyVideosVideoListIndex::NewL()
    {
    CVcxMyVideosVideoListIndex* self = new (ELeave) CVcxMyVideosVideoListIndex();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::CVcxMyVideosVideoListIndex
// ----------------------------------------------------------------------------
//
CVcxMyVideosVideoListIndex::CVcxMyVideosVideoListIndex()
    {
    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::ConstructL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoListIndex::ConstructL ()
    {
    iVideoArray.Reset();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::~CVcxMyVideosVideoListIndex
// ----------------------------------------------------------------------------
//
CVcxMyVideosVideoListIndex::~CVcxMyVideosVideoListIndex()
    {
    iVideoArray.Close();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::~CVcxMyVideosVideoListIndex
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoListIndex::SetL( const CMPXMedia& /*aVideoList*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::Find
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoListIndex::Find( TUint32 aMdsId, TVcxMyVideosVideo& aVideo )
    {
    return KErrNone;
    
    const TLinearOrder<TVcxMyVideosVideo> KOrderByMdsId(
            CVcxMyVideosVideoListIndex::CompareVideosByMdsId );
    
    TInt index;
    TVcxMyVideosVideo video;
    video.iMdsId = aMdsId;
    TInt err = iVideoArray.FindInOrder( video, index, KOrderByMdsId );
    
    if ( err != KErrNone )
        {
        return KErrNotFound;
        }
    
    aVideo = iVideoArray[index];
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::Remove
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoListIndex::Remove( TUint32 aMdsId, TBool aCompress )
    {
    const TLinearOrder<TVcxMyVideosVideo> KOrderByMdsId(
            CVcxMyVideosVideoListIndex::CompareVideosByMdsId );

    TVcxMyVideosVideo video;
    video.iMdsId = aMdsId;
    TInt pos;
    TInt posInVideoList;
    TInt err = iVideoArray.FindInOrder( video, pos, KOrderByMdsId );
    if ( err == KErrNone )
        {
        posInVideoList = iVideoArray[pos].iPos;
        iVideoArray.Remove( pos );

        // update all indexes which are bigger than posInVideoList
        TInt count = iVideoArray.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            if ( iVideoArray[i].iPos > posInVideoList )
                {
                iVideoArray[i].iPos--;
                }
            }
        }
    
    if ( aCompress )
        {
        iVideoArray.Compress();
        }
    
    return err;
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::AddL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoListIndex::AddL( CMPXMedia* aVideo, TInt aPos )
    {
    TVcxMyVideosVideo video;
    video.Set( TVcxMyVideosCollectionUtil::IdL( *aVideo ).iId1, aPos, aVideo );

    const TLinearOrder<TVcxMyVideosVideo> KOrderByMdsId(
            CVcxMyVideosVideoListIndex::CompareVideosByMdsId );

    // update indexes
    TInt count = iVideoArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iVideoArray[i].iPos >= aPos )
            {
            iVideoArray[i].iPos++;
            }
        }

    iVideoArray.InsertInOrderAllowRepeatsL( video, KOrderByMdsId );

    }

#ifdef _DEBUG
// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::VideoArray
// ----------------------------------------------------------------------------
//
const RArray<TVcxMyVideosVideo>& CVcxMyVideosVideoListIndex::VideoArray()
    {
    return iVideoArray;
    }
#endif

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::Sort
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoListIndex::Sort()
    {
    const TLinearOrder<TVcxMyVideosVideo> KOrderByMdsId(
            CVcxMyVideosVideoListIndex::CompareVideosByMdsId );

    iVideoArray.Sort( KOrderByMdsId );    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoListIndex::CompareVideosByMdsId
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoListIndex::CompareVideosByMdsId( const TVcxMyVideosVideo& aVideo1,
            const TVcxMyVideosVideo& aVideo2 )
    {
    if ( aVideo1.iMdsId == aVideo2.iMdsId )
        {
        return 0;
        }

    if ( aVideo1.iMdsId < aVideo2.iMdsId )
        {
        return -1;
        }
    return 1;    
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosVideoCache* CVcxMyVideosVideoCache::NewL(
    CVcxMyVideosCollectionPlugin& aMyVideosCollectionPlugin )
    {
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
    delete iVideoList;
    DeletePartialList();
    delete iVideoListIndex;
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosVideoCache::CVcxMyVideosVideoCache(
        CVcxMyVideosCollectionPlugin& aMyVideosCollectionPlugin )
: iCollection( aMyVideosCollectionPlugin )
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::ConstructL ()
    {
    iVideoList          = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    SetComplete( EFalse );
    
    iPartialVideoList.Reset();
    iVideoListIndex = CVcxMyVideosVideoListIndex::NewL();
    
    iCancelListFetching = EFalse; // TESTING STUFF
    iLeaveOnReset = EFalse; // TESTING STUFF
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosVideoCache::CreateVideoListByOriginL
// ----------------------------------------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosVideoCache::CreateVideoListByOriginL( TUint8 /*aOrigin*/ )
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::AppendToListL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::AppendToListL( CMPXMedia& /*aToList*/, CMPXMedia& /*aFromList*/,
        TUint8 /*aOrigin*/, TInt /*aNewItemsStartIndex*/ )
    {
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::FindVideoByUriL
// ----------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosVideoCache::FindVideoByUriL( const TDesC& /*aUri*/ )
    {    
    return NULL;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::FindVideoByMdsIdL
// ----------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosVideoCache::FindVideoByMdsIdL( TUint32 aMdsId, TInt& /*aPos*/ )
    {
    if( aMdsId == KMPXIdNotFound )
        {
        return NULL;
        }
        
    CMPXMediaArray* videoarray = iVideoList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    if( videoarray->Count() > 0 )
        {
        return videoarray->AtL(0);
        }
        
    return NULL;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::GetVideosL
// ----------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosVideoCache::GetVideosL( RArray<TUint32>& aMdsIds )
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
// If mpx item is already in cache, or MDS does not contain the item (or was not video item),
// then it is removed from aMdsIds. Ie after this function call aMdsIds contains
// only IDs which are actually added to cache. aNonVideoIds will contain IDs
// which were not video objects.
// ----------------------------------------------------------------------------
//    
void CVcxMyVideosVideoCache::AddVideosFromMdsL( RArray<TUint32>& /*aMdsIds*/,
        TBool& aListFetchingWasCanceled, RArray<TUint32>* /*aNonVideoIds*/, TBool /*aUpdateCategories*/  )
    {
    aListFetchingWasCanceled = iCancelListFetching;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::AddL()
// This is for single adds, video list fetching does not call this.
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::AddL( CMPXMedia* /*aVideo*/, TVcxMyVideosSortingOrder /*aSortingOrder*/,
        TBool /*aUpdateCategories*/ )
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::CheckForPartialVideoListItemsL()
// This is called by iCollection during video list fetching.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::CheckForPartialVideoListItemsL( TInt /*aNewItemsStartIndex*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::PosOnPartialVideoListL()
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::PosOnPartialVideoListL( CMPXMedia& /*aVideo*/ )
    {
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::PosOnVideoListL()
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::PosOnVideoListL( CMPXMedia& /*aVideo*/ )
    {
    return KErrNotFound;
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::AddToPartialListL()
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::AddToPartialListL( CMPXMedia* /*aVideo*/ )
    {
    return KErrNotFound;
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
void CVcxMyVideosVideoCache::CreateVideoListL( TBool /*aForce*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::UpdateVideoL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosVideoCache::UpdateVideoL( CMPXMedia& aVideo )
    {
    TBool changed( EFalse );
    
    TMPXItemId mpxId = aVideo.ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId );
    
    switch( mpxId.iId1 )
        {
        case KMPXIdLeave: // Leave
            User::Leave( KErrCorrupt );
            break;
        case KMPXIdNotFound: // Not found
            User::Leave( KErrNotFound );
            break;
        case KMPXIdChanged: // Changed
            changed = ETrue;
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
void CVcxMyVideosVideoCache::HandleVideoTitleModifiedL( CMPXMedia*& /*aVideoInCache*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::FindCorrectPositionL
// Finds the correct position using bisection method.
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::FindCorrectPositionL( CMPXMedia& /*aMedia*/,
        TVcxMyVideosSortingOrder /*aSortingOrder*/ )
    {
    return 0;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::CompareL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::CompareL( CMPXMedia& /*aNewVideo*/, CMPXMedia& /*aVideoInList*/,
        TVcxMyVideosSortingOrder /*aSortingOrder*/ )
    {
    return 0;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::SortingOrderL
// ----------------------------------------------------------------------------
//    
TVcxMyVideosSortingOrder CVcxMyVideosVideoCache::SortingOrderL()
    {
    return EVcxMyVideosSortingNone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::AddToCorrectPlaceL
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::AddToCorrectPlaceL( CMPXMedia& aVideo,
        TVcxMyVideosSortingOrder aSortingOrder, TBool aUpdateCategories )
    {    
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::MoveToCorrectPlaceL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::MoveToCorrectPlaceL( CMPXMedia& /*aVideo*/,
        TVcxMyVideosSortingOrder /*aSortingOrder*/ )
    {
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
// All removes end up here (except ResetVideoListL).
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosVideoCache::RemoveL( TUint32 aMdsId, TBool aUpdateCategories )
    {
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::RemoveL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::RemoveL( RArray<TUint32>& /*aMdsIds*/, TBool /*aUpdateCategories*/ )
    {        
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::ResetVideoListL
// iVideoList stays the same, media array is deleted and recreated to 0 length,
// media items are deleted.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::ResetVideoListL()
    {
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
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::IsComplete
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosVideoCache::IsComplete()
    {
    return iVideoListIsComplete;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::SetComplete
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::SetComplete( TBool aComplete )
    {
    iVideoListIsComplete = aComplete;
    }

#ifdef _DEBUG
// ----------------------------------------------------------------------------
// CVcxMyVideosVideoCache::SetComplete
// ----------------------------------------------------------------------------
//
void CVcxMyVideosVideoCache::CheckVideoListIndexL()
    {
    }
#endif

// End of file

