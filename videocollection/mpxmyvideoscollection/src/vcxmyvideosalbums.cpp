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
#include <e32base.h>
#include <mpxlog.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmediageneraldefs.h>
#include "vcxmyvideosalbums.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideosmdsalbums.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosmessagelist.h"
#include "vcxmyvideosopenhandler.h"

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
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::CreateAlbumListL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::CreateAlbumListL()
    {
    if ( iAlbumList )
        {
        return;
        }
    iAlbumList = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
    iCollection.iMyVideosMdsDb->iAlbums->GetAlbumsL( iAlbumList, *this );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::CreateAlbumsL
// Called when iAlbumList is ready, fills iAlbums.
// Pointers to CVcxMyVideosVideoCache::iVideoList are not filled in yet.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::CreateAlbumsL()
    {
    CMPXMediaArray* albumArray = iAlbumList->Value<CMPXMediaArray>( KMPXMediaArrayContents );

    TInt count = albumArray->Count();
    CVcxMyVideosAlbum* album;
    CMPXMedia* media;
    for ( TInt i = 0; i < count; i++ )
        {
        media = albumArray->AtL( i );
        album = CVcxMyVideosAlbum::NewLC( iCollection );
        album->iMdsId = TVcxMyVideosCollectionUtil::IdL( *media ).iId1;
        album->iMedia = media;
        iAlbums.AppendL( album );
        CleanupStack::Pop( album );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::GetAlbumContentIdsL
// Fetches MDS IDs for all albums.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::GetAlbumContentIdsL()
    {
    TInt count = iAlbums.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        // These calls go to queue and responses arrive asynchronously
        // to HandleGetAlbumContentIdsRespL.
        iCollection.iMyVideosMdsDb->iAlbums->GetAlbumContentIdsL( iAlbums[i]->iMdsId,
                iAlbums[i]->iVideoList, *this );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::Album
// ----------------------------------------------------------------------------
//
CVcxMyVideosAlbum* CVcxMyVideosAlbums::Album( TUint32 aMdsId, TInt* aPos )
    {
    TInt count = iAlbums.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iAlbums[i]->iMdsId == aMdsId )
            {
            if ( aPos )
                {
                *aPos = i;
                }
            return iAlbums[i];
            }
        }
    return NULL;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::AddVideosToAlbumL
// Command from MPX client.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::AddVideosToAlbumL( CMPXMedia* aCmd )
    {    
    iCollection.iMyVideosMdsDb->iAlbums->AddVideosToAlbumL( aCmd, *this );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::RemoveVideosFromAlbumL
// Converts video mpx ids to relation ids and calls RemoveRelationsL.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::RemoveVideosFromAlbumL( CMPXMedia* aCmd )
    {
    TInt albumId = TVcxMyVideosCollectionUtil::Uint32ValueL( *aCmd );
    CMPXMediaArray* array = TVcxMyVideosCollectionUtil::MediaArrayL( *aCmd );
    TVcxMyVideosAlbumVideo video;
    TInt index;
    TInt error;
    CMPXMedia* media;
    TInt count = array->Count();
    iMdsOpTargetIds.Reset();
    iMdsOpTargetIds.Reserve( count );
    iRemoveFromAlbumVideos.Reset();
    iRemoveFromAlbumVideos.Reserve( count );
    TVcxMyVideosAlbumVideo paddingVideo;
    paddingVideo.iMdsId         = 0;
    paddingVideo.iRelationMdsId = 0;
    
    for ( TInt i = 0; i < count; i++ )
        {
        media = array->AtL( i );
        error = Album( albumId )->Video(
                TVcxMyVideosCollectionUtil::IdL( *media ).iId1, video, index );
        if ( error == KErrNone )
            {
            iRemoveFromAlbumVideos.AppendL( video ); //store album video data now to avoid second search when response arrives
            iMdsOpTargetIds.AppendL( video.iRelationMdsId );
            }
        else
            {
            iRemoveFromAlbumVideos.AppendL( paddingVideo ); //add dummy video to keep media array and iRemoveFromAlbumVideos in sync
            }
        }
    iMdsOpResultsUint32.Reset();
    iCollection.iMyVideosMdsDb->iAlbums->RemoveRelationsL(
            iMdsOpTargetIds, iMdsOpResultsUint32, *this );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::AddAlbumL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::AddAlbumL( CMPXMedia& aCmd )
    {
    iCollection.iMyVideosMdsDb->iAlbums->AddAlbumL( aCmd );
    
    TUint32 mdsId = TVcxMyVideosCollectionUtil::IdL( aCmd ).iId1;
    if ( !Album( mdsId ) )
        {
        CMPXMediaArray* albumArray = iAlbumList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
        albumArray->AppendL( aCmd );
        
        CVcxMyVideosAlbum* album = CVcxMyVideosAlbum::NewLC( iCollection );
        album->iMdsId = mdsId;
        album->iMedia = albumArray->AtL( albumArray->Count() - 1 );
        iAlbums.AppendL( album );
        CleanupStack::Pop( album );
        
        iCollection.iMessageList->AddEventL( TMPXItemId( mdsId, KVcxMvcMediaTypeAlbum ),
                EMPXItemInserted, EVcxMyVideosListNoInfo, album->iMedia );
        iCollection.iMessageList->SendL();
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::RemoveAlbumsFromMdsOnlyL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::RemoveAlbumsFromMdsOnlyL( CMPXMedia* aCmd )
    {
    //TODO: find out what happens to relations, do we have to clean them out
    iCollection.iMyVideosMdsDb->iAlbums->RemoveAlbumsL( aCmd, *this );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::RemoveAlbumsL
// From MDS delete event.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::RemoveAlbumsL( RArray<TUint32>& aAlbumIds )
    {
    TInt count = aAlbumIds.Count();
    for ( TInt i = count -1; i >= 0; i-- )
        {
        TBool removed = RemoveAlbumL( aAlbumIds[i], EFalse /* dont compress */);
        if ( removed )
            {
            aAlbumIds.Remove( i );
            }
        }
    iAlbums.Compress();
    
    iCollection.iMessageList->SendL();        
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::RemoveAlbumL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAlbums::RemoveAlbumL( TUint32 aMdsId, TBool aCompress )
    {
    TInt pos;
    CVcxMyVideosAlbum* album = Album( aMdsId, &pos );
    if ( album )
        {
        CMPXMediaArray* albumArray = TVcxMyVideosCollectionUtil::MediaArrayL( *iAlbumList );
        
        iAlbums.Remove( pos );
        albumArray->Remove( pos );
        
        MPX_DEBUG2("CVcxMyVideosAlbums:: removing album from pos %d", pos);
        
        if ( aCompress )
            {
            iAlbums.Compress();
            }
        iCollection.iMessageList->AddEventL( TMPXItemId( aMdsId, KVcxMvcMediaTypeAlbum ),
                EMPXItemDeleted, EVcxMyVideosListNoInfo );
        return ETrue;
        }
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
    //TODO: implement if we want to support albums which are added by someone else
    //      than My Videos Collection.
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleGetAlbumsRespL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleGetAlbumsRespL( CMPXMedia* aAlbumList )
    {
    // aAlbumList = iAlbumList
    CMPXMediaArray* array = aAlbumList->Value<CMPXMediaArray>( KMPXMediaArrayContents );
    TInt count = array->Count();
    TMPXItemId mpxId;
    CMPXMedia* album;
    for ( TInt i = 0; i < count; i++ )
        {
        album = array->AtL( i );
        mpxId = TVcxMyVideosCollectionUtil::IdL( *album );
        iCollection.iMessageList->AddEventL( mpxId, EMPXItemInserted, 0, album );
        }
    CreateAlbumsL(); //fills in iAlbums from iAlbumList
    GetAlbumContentIdsL(); //fills iAlbums with MDS IDs of the videos asynchronously
    iCollection.iMessageList->SendL();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleGetAlbumContentIdsRespL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleGetAlbumContentIdsRespL( TUint32 aAlbumId,
        RArray<TVcxMyVideosAlbumVideo>& /*aAlbumContentIds*/ )
    {    
    //&aAlbumContent = iAlbums[x]->iVideoList
    
    CVcxMyVideosAlbum* album( Album( aAlbumId ) ); 
    if ( album )
        {
        album->Sort(); //sorts iAlbum[x]->iVideoList by MDS ID.
        }

    //if last album -> album list is complete
    if ( iAlbums[iAlbums.Count() -1]->iMdsId == aAlbumId )
        {
        MPX_DEBUG1("CVcxMyVideosAlbums:: content ids for last album arrived -> album list is complete" );
        iAlbumListIsComplete = ETrue;

        if ( iCollection.iOpenHandler->iPendingAlbumOpenId )
            {
            MPX_DEBUG1("CVcxMyVideosAlbums:: there was album open pending, calling HandleAlbumOpenL()" );

            TRAPD( err, iCollection.iOpenHandler->HandleAlbumOpenL() );
            if ( err != KErrNone )
                {
                MPX_DEBUG3("CVcxMyVideosAlbums:: HandleAlbumOpenL() left: %d, calling HandleOpen(NULL,%d)",
                        err, err);
                iCollection.iObs->HandleOpen( static_cast<CMPXMedia*>(NULL), err );
                }
            }
        }
    
    
#ifdef _DEBUG
    if ( album )
        {
        TInt count = album->iVideoList.Count();

        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: content ids for album %d arrived", aAlbumId);

        for ( TInt i = 0; i < album->iVideoList.Count(); i++ )
            {
            MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: mds id = %d", album->iVideoList[i].iMdsId );
            }
        }
#endif
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleGetAlbumsResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleGetAlbumsResp( CMPXMedia* aAlbumList )
    {
    TRAP_IGNORE( HandleGetAlbumsRespL( aAlbumList ) );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleGetAlbumContentIdsResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleGetAlbumContentIdsResp( TUint32 aAlbumId,
        RArray<TVcxMyVideosAlbumVideo>& aAlbumContent )
    {
    TRAP_IGNORE( HandleGetAlbumContentIdsRespL( aAlbumId, aAlbumContent ) );    
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleAddVideosToAlbumResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleAddVideosToAlbumResp( CMPXMedia* aCmd,
        RPointerArray<CMdEInstanceItem>& aItemArray )
    {    
    TRAPD( err, DoHandleAddVideosToAlbumRespL( aCmd, aItemArray ) );
    
    if ( err != KErrNone )
        {
        CMPXMedia* cmd = iCollection.iActiveTask->Command(); 
        if ( cmd )
            {
            cmd->SetTObjectValueL<TInt>( KVcxMediaMyVideosInt32Value, err );
            iCollection.iActiveTask->Done();
            }
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::DoHandleAddVideosToAlbumResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::DoHandleAddVideosToAlbumRespL( CMPXMedia* aCmd,
        RPointerArray<CMdEInstanceItem>& aItemArray )
    {
    MPX_DEBUG1("CVcxMyVideosAlbums::DoHandleAddVideosToAlbumResp() start");

    CMPXMediaArray* mediaArray = TVcxMyVideosCollectionUtil::MediaArrayL( *aCmd );
    
    TVcxMyVideosAlbumVideo video;
    TUint albumId;
    CVcxMyVideosAlbum* album;
    
    TInt count = aItemArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        video.iRelationMdsId = aItemArray[i]->Id(); 
        MPX_DEBUG3( "CVcxMyVideosAlbums:: item result[%d] = %d (id)", i, video.iRelationMdsId );
        if ( video.iRelationMdsId == KNoId )
            {
            mediaArray->AtL( i )->SetTObjectValueL<TInt>( KVcxMediaMyVideosInt32Value,
                    KErrGeneral );
            }
        else
            {
            mediaArray->AtL( i )->SetTObjectValueL<TInt>( KVcxMediaMyVideosInt32Value, KErrNone );
            video.iMdsId = TVcxMyVideosCollectionUtil::IdL( *mediaArray->AtL( i ) ).iId1;
            albumId      = TVcxMyVideosCollectionUtil::Uint32ValueL( *aCmd );
            album = Album( albumId );
            if ( album )
                {
                album->AddL( video );
                iCollection.iMessageList->AddEventL( TMPXItemId( albumId, KVcxMvcMediaTypeAlbum ),
                        EMPXItemModified, EVcxMyVideosVideoListOrderChanged );
                }
            }
        }
    
    iCollection.iActiveTask->Done();
    iCollection.iMessageList->SendL();
    
    MPX_DEBUG1("CVcxMyVideosAlbums::DoHandleAddVideosToAlbumResp() exit");
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleRemoveRelationsResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleRemoveRelationsResp( RArray<TUint32>& /*aRelationIds*/,
        RArray<TUint32>& aResults )
    {
    MPX_DEBUG1("CVcxMyVideosAlbums::HandleRemoveRelationsResp() start");
    
    // iRemoveFromAlbumVideos and mediaArray are in sync
    
    TRAP_IGNORE(
    
    CMPXMedia* cmd             = iCollection.iActiveTask->Command();
    CMPXMediaArray* mediaArray = TVcxMyVideosCollectionUtil::MediaArrayL( *cmd );
    TInt count                 = iRemoveFromAlbumVideos.Count();
    CMPXMedia* video;
    TInt relationId;
    for ( TInt i = 0; i < count; i++ )
        {
        video = mediaArray->AtL( i );
        relationId = iRemoveFromAlbumVideos[i].iRelationMdsId;
        if ( relationId != 0 && 
                aResults.Find( relationId ) != KErrNotFound )
            {
            video->SetTObjectValueL( KVcxMediaMyVideosInt32Value, KErrNone );
            }
        else
            {
            video->SetTObjectValueL( KVcxMediaMyVideosInt32Value, KErrGeneral );
            }
        }
    
    );
    
    iRemoveFromAlbumVideos.Reset();
    
    iCollection.iActiveTask->Done();

    MPX_DEBUG1("CVcxMyVideosAlbums::HandleRemoveRelationsResp() exit");
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleRemoveAlbumsResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleRemoveAlbumsResp( CMPXMedia* aCmd,
        RArray<TUint32>& aResultIds )
    {    
    TRAP_IGNORE(
    
    CMPXMediaArray* mediaArray = TVcxMyVideosCollectionUtil::MediaArrayL( *aCmd );
    TInt count = mediaArray->Count();
    TUint32 mdsId;
    CMPXMedia* album;
    for ( TInt i = 0; i < count; i++ )
        {
        album = mediaArray->AtL( i );
        mdsId = TVcxMyVideosCollectionUtil::IdL( *album ).iId1;
        if ( aResultIds.Find( mdsId ) != KErrNotFound )
            {
            album->SetTObjectValueL( KVcxMediaMyVideosInt32Value, KErrNone );
            }
        else
            {
            album->SetTObjectValueL( KVcxMediaMyVideosInt32Value, KErrGeneral );
            }
        }
    
    );
    
    iCollection.iActiveTask->Done();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::HandleRelationEvent
// Only relation removed events are received here.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAlbums::HandleRelationEvent( TObserverNotificationType /*aType*/,
        const RArray<TMdERelation>& aRelationArray )
    {    
    TRAP_IGNORE(
    
    TUint albumId;
    CVcxMyVideosAlbum* album;
    TInt count = aRelationArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        albumId = aRelationArray[i].LeftObjectId();
        
        if ( iAlbumListIsComplete )
            {
            album = Album( albumId );
            if ( album )
                {
                album->Remove( aRelationArray[i].RightObjectId(), ETrue /* compress */ );                
                iCollection.iMessageList->AddEventL( TMPXItemId( albumId, KVcxMvcMediaTypeAlbum ),
                        EMPXItemModified, EVcxMyVideosVideoListOrderChanged );
                }
            }
        else
            {
            //We are fetching albums already, we assume that it will be up to date.
            //If that is not the case, then we should set some flag here and restart
            //album fetching from scratch.
            }
        }
    iCollection.iMessageList->SendL();
    
    );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAlbums::UpdateAlbumL
// Updates album attributes from aAlbum, if album is not found from memory,
// nothing is done (no fetching from MDS).
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAlbums::UpdateAlbumL( const CMPXMedia& aAlbum )
    {
    TBool changed = EFalse;
    
    TMPXItemId mpxId = TVcxMyVideosCollectionUtil::IdL( aAlbum );
    CVcxMyVideosAlbum* album = Album( mpxId.iId1 );

    if ( album && album->iMedia )
        {
        CMPXMedia* media = album->iMedia;

        if ( media->IsSupported( KMPXMediaGeneralTitle ) )
            {
            TPtrC newTitle( TVcxMyVideosCollectionUtil::Title( aAlbum ) );
            TPtrC oldTitle( TVcxMyVideosCollectionUtil::Title( *media ) );

            if ( newTitle != oldTitle )
                {
                media->SetTextValueL( KMPXMediaGeneralTitle, newTitle );
                iCollection.iMessageList->AddEventL( mpxId, EMPXItemModified );
                changed = ETrue;
                }
            }
        }
    return changed;
    }
// END OF FILE
