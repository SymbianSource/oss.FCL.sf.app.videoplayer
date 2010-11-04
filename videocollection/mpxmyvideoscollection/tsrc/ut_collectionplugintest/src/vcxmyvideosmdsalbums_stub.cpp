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
* Description:  Provides albums support utilizing MDS.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <mpxlog.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include "vcxmyvideosmdsalbums.h"
#include "vcxmyvideosmdsdb_stub.h"
#include "vcxmyvideoscollectionutil.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsAlbums::CVcxMyVideosMdsAlbums( CVcxMyVideosMdsDb& aMdsDb,
        MVcxMyVideosMdsAlbumsObserver* aObserver )
: CActive( EPriorityStandard ), iMdsDb( aMdsDb ), iObserver(aObserver) 
    {
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::ConstructL()
    {
    GetSchemaDefinitionsL();
    SetObservingL();
    //CActiveScheduler::Add( this );    
    }

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsAlbums* CVcxMyVideosMdsAlbums::NewL( CVcxMyVideosMdsDb& aMdsDb,
        MVcxMyVideosMdsAlbumsObserver* aObserver )
    {
    CVcxMyVideosMdsAlbums* self = new(ELeave) CVcxMyVideosMdsAlbums( aMdsDb, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosMdsAlbums::~CVcxMyVideosMdsAlbums()
    {
    CancelQueries();
    delete iAlbumQuery;
    delete iVideoQuery;
    delete iRelationQuery;
    iItemArray.Close();
    iResultBuffer.Close();
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::CancelQueries
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::CancelQueries( CVcxMyVideosMdsDb::TRequestType /*aType*/ )
    {
    }

// ---------------------------------------------------------------------------
// From CActive
// CVcxMyVideosMdsAlbums::DoCancel
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::SetObservingL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::SetObservingL()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetAlbumsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::GetAlbumsL( CMPXMedia* /*aAlbumList*/,
        MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetAlbumL
// ---------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosMdsAlbums::GetAlbumL( TUint32 /*aId*/ )
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoGetAlbumsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoGetAlbumsL( CMPXMedia* /*aAlbumList*/,
        MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetAlbumContentIdsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::GetAlbumContentIdsL( TUint32 /*aAlbumId*/,
        RArray<TVcxMyVideosAlbumVideo>& /*aContentArray*/,
        MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    {
    }
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoGetAlbumContentIdsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoGetAlbumContentIdsL( TUint32 /*aAlbumId*/,
        RArray<TVcxMyVideosAlbumVideo>& /*aContentArray*/,
        MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetAlbumContentVideosL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::GetAlbumContentVideosL( TUint32 /*aAlbumId*/, CMPXMedia& /*aVideoList*/,
        MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoGetAlbumContentVideosL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoGetAlbumContentVideosL( TUint32 /*aAlbumId*/, CMPXMedia& /*aVideoList*/,
        MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetSchemaDefinitionsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::GetSchemaDefinitionsL()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::Object2MediaL
// For album objects only.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::Object2MediaL(
        CMdEObject& /*aObject*/,
        CMPXMedia& /*aAlbum*/ )
    {    
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::Media2ObjectL
// Called by AddAlbumL()
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::Media2ObjectL(
        CMPXMedia& /*aAlbum*/,
        CMdEObject& /*aObject*/)
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::GetSchemaDefinitionsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleQueryNewResults(CMdEQuery& /*aQuery*/,
            TInt /*aFirstNewItemIndex*/,
            TInt /*aNewItemCount*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleQueryCompleted
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleQueryCompleted( CMdEQuery& /*aQuery*/, TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleAlbumQueryCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleAlbumQueryCompletedL( CMdEQuery& /*aQuery*/, TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleVideoQueryCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleVideoQueryResultsL( CMdEQuery& /*aQuery*/, TInt /*aError*/,
    TInt /*aFirstNewItemIndex*/, TInt /*aNewItemCount*/, TBool /*aComplete*/  )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleRelationQueryCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleRelationQueryCompletedL( CMdEQuery& /*aQuery*/, TInt /*aError*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::AddVideosToAlbumL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::AddVideosToAlbumL( CMPXMedia* /*aCmd*/,
        MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    {
    }
    
// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoAddVideosToAlbumL
// Called by iMdsDb.iCmdQueue
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoAddVideosToAlbumL( CMPXMedia* /*aCmd*/,
        MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::RemoveRelationsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::RemoveRelationsL( RArray<TUint32>& /*aRelationIds*/,
        RArray<TUint32>& /*aResults*/, MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoRemoveRelationsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoRemoveRelationsL( RArray<TUint32>& /*aRelationIds*/,
        RArray<TUint32>& /*aResults*/, MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::AddAlbumL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::AddAlbumL( CMPXMedia& /*aAlbum*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::SetAlbumL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::SetAlbumL( CMPXMedia& /*aVideo*/ )
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::RemoveAlbumsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::RemoveAlbumsL( CMPXMedia* /*aMpxCmd*/, MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    { 
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::DoRemoveAlbumsL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::DoRemoveAlbumsL( CMPXMedia* /*aMpxCmd*/,
        MVcxMyVideosMdsAlbumsObserver& /*aClient*/ )
    { 
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::RunL
// From CActive.
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::RunL()
    {
    }

//TODO: implement RunError

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleAddVideosToAlbumCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleAddVideosToAlbumCompletedL()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleRemoveRelationsCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleRemoveRelationsCompletedL()
    {
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleRemoveAlbumsCompletedL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleRemoveAlbumsCompletedL()
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosMdsAlbums::HandleRelationItemNotification
// From MMdERelationItemObserver
// ----------------------------------------------------------------------------
//
void CVcxMyVideosMdsAlbums::HandleRelationItemNotification(CMdESession& /*aSession*/, 
        TObserverNotificationType aType,
        const RArray<TMdERelation>& aRelationArray)
    {
    if ( iObserver )
        {
        iObserver->HandleRelationEvent( aType, aRelationArray );
        }
    }

// END OF FILE
