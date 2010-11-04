/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Albums related collection functionality (except db operations).
*
*/


// INCLUDE FILES
#include "vcxmyvideosalbums.h"
#include "vcxmyvideoscollectionplugin.h"

const TInt KMPXIdNotFound = 998;

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbums::CVcxMyVideosAlbums( CVcxMyVideosCollectionPlugin& aCollection )
: iCollection( aCollection )
    {
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::ConstructL()
    {
    iAlbums.Reset();
    iChangedAlbums.Reset();
    }

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbums* CVcxMyVideosAlbums::NewL( CVcxMyVideosCollectionPlugin& aCollection )
    {
    CVcxMyVideosAlbums* self = new(ELeave) CVcxMyVideosAlbums( aCollection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbums::~CVcxMyVideosAlbums()
    {
    delete iAlbumList;
    TInt count = iAlbums.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        delete iAlbums[i];
        }
    iAlbums.Close();
    iMdsOpTargetIds.Close();
    iMdsOpResults.Close();
    iMdsOpResultsUint32.Close();
    iRemoveFromAlbumVideos.Close();
    iChangedAlbums.Close();
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::CreateAlbumListL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::CreateAlbumListL()
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::CreateAlbumsL
// Called when iAlbumList is ready, fills iAlbums.
// Pointers to CVcxMyVideosVideoCache::iVideoList are not filled in yet.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::CreateAlbumsL()
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::GetAlbumContentIdsL
// Fetches MDS IDs for all albums.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::GetAlbumContentIdsL()
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::Album
// ----------------------------------------------------------------------------
//
CVcxMyVideosAlbum* CVcxMyVideosAlbums::Album( TUint32 /*aMdsId*/, TInt* /*aPos*/ )
    {
    return NULL;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::AddVideosToAlbumL
// Command from MPX client.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::AddVideosToAlbumL( CMPXMedia* /*aCmd*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::RemoveVideosFromAlbumL
// Converts video mpx ids to relation ids and calls RemoveRelationsL.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::RemoveVideosFromAlbumL( CMPXMedia* /*aCmd*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::AddAlbumL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::AddAlbumL( CMPXMedia& /*aCmd*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::RemoveAlbumsFromMdsOnlyL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::RemoveAlbumsFromMdsOnlyL( CMPXMedia* /*aCmd*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::RemoveAlbumsL
// From MDS delete event.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::RemoveAlbumsL( RArray<TUint32>& /*aAlbumIds*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::RemoveAlbumL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAlbums::RemoveAlbumL( TUint32 aMdsId, TBool /*aCompress*/ )
    {
    return ( aMdsId == KMPXIdNotFound ) ? EFalse : ETrue;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::CalculateAttributesL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAlbums::CalculateAttributesL()
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::AddAlbumsFromMdsL
// From MDS insert event, no need to fetch content since the album was just
// created.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::AddAlbumsFromMdsL( RArray<TUint32>& /*aAlbumIds*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleGetAlbumsRespL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleGetAlbumsRespL( CMPXMedia* /*aAlbumList*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleGetAlbumContentIdsRespL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleGetAlbumContentIdsRespL( TUint32 /*aAlbumId*/,
        RArray<TVcxMyVideosAlbumVideo>& /*aAlbumContentIds*/ )
    {    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleGetAlbumsResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleGetAlbumsResp( CMPXMedia* /*aAlbumList*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleGetAlbumContentIdsResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleGetAlbumContentIdsResp( TUint32 /*aAlbumId*/,
        RArray<TVcxMyVideosAlbumVideo>& /*aAlbumContent*/ )
    {   
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleAddVideosToAlbumResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleAddVideosToAlbumResp( CMPXMedia* /*aCmd*/,
        RPointerArray<CMdEInstanceItem>& /*aItemArray*/ )
    {    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::DoHandleAddVideosToAlbumResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::DoHandleAddVideosToAlbumRespL( CMPXMedia* /*aCmd*/,
        RPointerArray<CMdEInstanceItem>& /*aItemArray*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleRemoveRelationsResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleRemoveRelationsResp( RArray<TUint32>& /*aRelationIds*/,
        RArray<TUint32>& /*aResults*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleRemoveAlbumsResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleRemoveAlbumsResp( CMPXMedia* /*aCmd*/,
        RArray<TUint32>& /*aResultIds*/ )
    {    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleRelationEvent
// Only relation removed events are received here.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleRelationEvent( TObserverNotificationType /*aType*/,
        const RArray<TMdERelation>& /*aRelationArray*/ )
    {    
    }

//HandleRelationIdEvent

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::UpdateAlbumL
// Updates album attributes from aAlbum, if album is not found from memory,
// nothing is done (no fetching from MDS).
// This func is also called (by MDS modified event) when items are added or
// removed from album.
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAlbums::UpdateAlbumL( const CMPXMedia& /*aAlbum*/ )
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::NewVideoFlagChangedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::NewVideoFlagChangedL( TUint32 /*aMdsId*/ )
    {    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::VideoTitleChangedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::VideoTitleChangedL( TUint32 /*aMdsId*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::VideoAddedOrRemovedFromCacheL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::VideoAddedOrRemovedFromCacheL( CMPXMedia& /*aVideo*/ )
    {
    }
// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::UpdateChangedAlbumsL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::UpdateChangedAlbumsL()
    {
    }
// END OF FILE
