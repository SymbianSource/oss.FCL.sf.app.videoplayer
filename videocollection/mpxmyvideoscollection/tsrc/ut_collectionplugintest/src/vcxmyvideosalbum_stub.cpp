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
* Description:   Class representing album in My Videos collection.
*/

#include <mpxlog.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include "vcxmyvideosalbum.h"
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideosvideocache.h"
#include "vcxmyvideoscollectionutil.h"


// ---------------------------------------------------------------------------
// TVcxMyVideosAlbumVideo::TVcxMyVideosAlbumVideo
// ---------------------------------------------------------------------------
//
TVcxMyVideosAlbumVideo::TVcxMyVideosAlbumVideo()
    {
    Set( 0, 0, 0 );
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosAlbumVideo::operator=
// ---------------------------------------------------------------------------
//                
TVcxMyVideosAlbumVideo& TVcxMyVideosAlbumVideo::operator=( const TVcxMyVideosAlbumVideo& aVideo )
    {
    Set( aVideo.iMdsId, aVideo.iRelationMdsId, aVideo.iMedia );
    return *this;
    }

// ---------------------------------------------------------------------------
// TVcxMyVideosAlbumVideo::Set
// ---------------------------------------------------------------------------
//            
void TVcxMyVideosAlbumVideo::Set( TUint32 aMdsId, TUint32 aRelationMdsId, CMPXMedia* aVideo )
    {
    iMdsId         = aMdsId;
    iRelationMdsId = aRelationMdsId;
    iMedia         = aVideo;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::CVcxMyVideosAlbum
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbum::CVcxMyVideosAlbum( CVcxMyVideosCollectionPlugin& aCollectionPlugin )
: iCollection( aCollectionPlugin )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::~CVcxMyVideosAlbum
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbum::~CVcxMyVideosAlbum()
    {
    iVideoList.Close();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::NewLC
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbum* CVcxMyVideosAlbum::NewLC( CVcxMyVideosCollectionPlugin& aCollectionPlugin )
    {
    CVcxMyVideosAlbum* self = new (ELeave) CVcxMyVideosAlbum( aCollectionPlugin );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::NewL
// ---------------------------------------------------------------------------
//
CVcxMyVideosAlbum* CVcxMyVideosAlbum::NewL( CVcxMyVideosCollectionPlugin& aCollectionPlugin )
    {
    CVcxMyVideosAlbum* self = CVcxMyVideosAlbum::NewLC( aCollectionPlugin );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::ConstructL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::Sort
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::Sort()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::SetL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::SetL( /*CMPXMedia& aAlbum*/ )
    {
    //TODO:
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::BelongsToAlbum
// ---------------------------------------------------------------------------
//
TBool CVcxMyVideosAlbum::BelongsToAlbum( TUint32 /*aMdsId*/ )
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::Video
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosAlbum::Video( TUint32 /*aMdsId*/, TVcxMyVideosAlbumVideo& /*aVideo*/, TInt& /*aIndex*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::AddL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::AddL( TVcxMyVideosAlbumVideo /*aVideo*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::Remove
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::Remove( RArray<TUint32>& /*aMdsIds*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::Remove
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::Remove( TUint32 /*aMdsId*/, TBool /*aCompress*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::CompareVideosByMdsId
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosAlbum::CompareVideosByMdsId( const TVcxMyVideosAlbumVideo& /*aVideo1*/,
        const TVcxMyVideosAlbumVideo& /*aVideo2*/ )
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::CreateVideoListL
// Creates album video list from iCollection.iCache->iVideoList.
// ---------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosAlbum::CreateVideoListL()
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::CreateVideoListL
// Appends to video list the items which belong to this album.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosAlbum::AppendToVideoListL( CMPXMedia& /*aFromVideoList*/,
        CMPXMedia& /*aToVideoList*/, TInt /*aNewItemStartIndex*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosAlbum::CalculateAttributesL
// ---------------------------------------------------------------------------
//
TBool CVcxMyVideosAlbum::CalculateAttributesL()
    {
    return EFalse;
    }

