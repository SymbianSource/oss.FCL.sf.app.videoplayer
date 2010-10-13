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
* Description:   Handles collection Open operation related functionality.*
*/



// INCLUDE FILES
#include <e32base.h>
#include <mpxlog.h>
#include <mpxcollectionplugin.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>

#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideosopenhandler_stub.h"
#include "vcxmyvideosvideocache_stub.h"
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideoscategories.h"
#include "vcxmyvideosmessagelist.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosalbums.h"
#include "vcxmyvideosmdsalbums.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosOpenHandler::CVcxMyVideosOpenHandler( CVcxMyVideosCollectionPlugin& aCollection,
        CVcxMyVideosVideoCache& aCache,
        CVcxMyVideosMdsDb& aMds )
: iCollection( aCollection ), iCache( aCache ), iMds( aMds )
    {
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::ConstructL()
    {
    iCategoryIdsBeingOpened.Reset();
    iVideoListsBeingOpened.Reset();
    iAlbumIdsBeingOpened.Reset();
    iAlbumVideoListsBeingOpened.Reset();
    }

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosOpenHandler* CVcxMyVideosOpenHandler::NewL( CVcxMyVideosCollectionPlugin& aCollection,
        CVcxMyVideosVideoCache& aCache,
        CVcxMyVideosMdsDb& aMds )
    {
    CVcxMyVideosOpenHandler* self = new(ELeave) CVcxMyVideosOpenHandler( aCollection,
            aCache, aMds );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosOpenHandler::~CVcxMyVideosOpenHandler()
    {
    iCategoryIdsBeingOpened.Close();
    
    TInt count = iVideoListsBeingOpened.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        delete iVideoListsBeingOpened[i];
        iVideoListsBeingOpened[i] = NULL; 
        }
    iVideoListsBeingOpened.Close();

    iAlbumIdsBeingOpened.Close();
    
    count = iAlbumVideoListsBeingOpened.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        delete iAlbumVideoListsBeingOpened[i];
        iAlbumVideoListsBeingOpened[i] = NULL; 
        }
    iAlbumVideoListsBeingOpened.Close();    
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::OpenL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::OpenL(
    const CMPXCollectionPath& /*aPath*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::OpenCategoryL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::OpenCategoryL( TUint32 /*aCategoryId*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::DoHandleCreateVideoListRespL
// New items fetched from MDS. iCache.iVideoList = aVideoList.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::DoHandleCreateVideoListRespL(
        CMPXMedia* /*aVideoList*/, TInt aNewItemsStartIndex, TBool /*aComplete*/ )
    {
    if ( aNewItemsStartIndex == KErrNotFound )
        User::Leave( KErrNotFound );
    
    return;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::HandleAlbumOpenL
// Album list has been fetched already.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::HandleAlbumOpenL()
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::HandleGetAlbumContentVideosResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::HandleGetAlbumContentVideosResp(
        TUint32 /*aAlbumId*/, CMPXMedia& /*aVideoList*/, TInt /*aError*/,
        TInt /*aFirstNewItemIndex*/, TInt /*aNewItemCount*/, TBool /*aComplete*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::HandleGetAlbumContentVideosRespL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::HandleGetAlbumContentVideosRespL(
        TUint32 /*aAlbumId*/, CMPXMedia& /*aVideoList*/, TInt /*aError*/,
        TInt /*aFirstNewItemIndex*/, TInt /*aNewItemCount*/, TBool /*aComplete*/ )
    {
    }

// END OF FILE
