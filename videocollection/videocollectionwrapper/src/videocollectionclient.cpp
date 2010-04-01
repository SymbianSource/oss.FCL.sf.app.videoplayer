/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   VideoCollectionClient class implementation
* 
*/

// INCLUDE FILES
#include <QObject>
#include <centralrepository.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessage2.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <vcxmyvideosuids.h>

#include "videodatasignalreceiver.h"
#include "videocollectionclient.h"
#include "videocollectionlistener.h"

// -----------------------------------------------------------------------------
// VideoCollectionClient()
// -----------------------------------------------------------------------------
//
VideoCollectionClient::VideoCollectionClient() :
mCollectionUtility(0),
mCollectionOpenStatus(ECollectionNotOpen),  
mCollectionListener(0),
mOpenCategoryAlbum(TMPXItemId::InvalidId()),
mCollectionPathLevel(VideoCollectionCommon::ELevelInvalid)
{

}

// -----------------------------------------------------------------------------
// initialize()
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::initialize(VideoDataSignalReceiver *signalReceiver)
{
    if(!signalReceiver)
    {
        return -1;
    }
    if(mCollectionUtility && mCollectionListener)
    {
        // already initialized
        return 0;
    }
    if(!mCollectionListener)
    {
        mCollectionListener = new VideoCollectionListener(*this, *signalReceiver);
        if(!mCollectionListener)
        {
            return -1;
        }
    }
    if(!mCollectionUtility)
    {
        TRAPD( error, mCollectionUtility = MMPXCollectionUtility::NewL( mCollectionListener, KMcModeDefault ) );
        if(error)
        {
            delete mCollectionListener;
            mCollectionListener = 0;
            return error;
        }
    }
    return 0;
}

// -----------------------------------------------------------------------------
// ~VideoCollectionClient()
// -----------------------------------------------------------------------------
//
VideoCollectionClient::~VideoCollectionClient()
{
    if (mCollectionUtility)
    {
        // closing deallocates collection utility pointer
        mCollectionUtility->Close();
    }
    delete mCollectionListener;
}

// ---------------------------------------------------------------------------
// categoryIds
// ---------------------------------------------------------------------------
//

void VideoCollectionClient::getCategoryId(TMPXItemId &id)
{
    id = mOpenCategoryAlbum;
}

// ---------------------------------------------------------------------------
// getCollectionLevel
// ---------------------------------------------------------------------------
//
int VideoCollectionClient::getCollectionLevel()
{
    return mCollectionPathLevel;
}

// ---------------------------------------------------------------------------
// getOpenStatus
// ---------------------------------------------------------------------------
//
int VideoCollectionClient::getOpenStatus()
{
	return mCollectionOpenStatus;
}

// ---------------------------------------------------------------------------
// setOpenStatus
// ---------------------------------------------------------------------------
//
void VideoCollectionClient::setOpenStatus(int status)
{
    mCollectionOpenStatus = status;
    if(mCollectionOpenStatus == ECollectionOpened)
    {
        startOpenCurrentState();
    }
}

// -----------------------------------------------------------------------------
// startOpenCollection
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::startOpenCollection(int level)
{
    if(!mCollectionUtility)
    {
        return -1;
    }
    
	if((mCollectionOpenStatus == ECollectionOpening) && (getCollectionLevel() == level))
    {
        // allready opening/opened
        return 0;
    }
    mCollectionOpenStatus = ECollectionNotOpen;
    TRAPD(error, startOpenCollectionL(level));
    return error;
}

// -----------------------------------------------------------------------------
// startOpenCurrentState
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::startOpenCurrentState()
{
    int error = -1;
    if(mCollectionUtility && mCollectionOpenStatus == ECollectionOpened)
    {
        TRAP(error, mCollectionUtility->Collection().OpenL());
    }
    return error;
}

// -----------------------------------------------------------------------------
// deleteFile
// -----------------------------------------------------------------------------
int VideoCollectionClient::deleteVideos(QList<TMPXItemId> *mediaIds)
{  
    if(!mCollectionUtility || !mediaIds)
    {
        return -1;
    }    
    TRAPD(error, deleteVideosL(*mediaIds));
    return error;    
}

// -----------------------------------------------------------------------------
// openItem
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::openItem(TMPXItemId &mediaId)
{
    if(!mCollectionUtility)
    {
        return -1;
    } 
    
    TInt error;
    if (mediaId.iId2 == KVcxMvcMediaTypeVideo)
    {
    	TRAP(error, openVideoL(mediaId));
    }
    else
    {
    	TRAP(error, openCategoryL(mediaId));
    }
    return error;
}

// -----------------------------------------------------------------------------
// back
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::back()
{
    if(!mCollectionUtility)
    {
        return -1;
    } 
    TRAPD(error, backL());
    return error;
}

// -----------------------------------------------------------------------------
// fetchMpxMediaByMpxId
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::fetchMpxMediaByMpxId(TMPXItemId &mpxId)
{
    if(!mCollectionUtility)
    {
        return -1;
    }
   
    TRAPD(error, fetchMpxMediaByMpxIdL(mpxId));
    return error;
}

// -----------------------------------------------------------------------------
// getVideoDetails
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::getVideoDetails(TMPXItemId &mediaId)
{
    if(!mCollectionUtility)
    {
        return -1;
    }
    
    TRAPD(error, getVideoDetailsL(mediaId));
   
    return error;
}

// -----------------------------------------------------------------------------
// addNewAlbum
// -----------------------------------------------------------------------------
//
TMPXItemId VideoCollectionClient::addNewAlbum(const QString &title)
{
    TMPXItemId id = TMPXItemId::InvalidId();
    
    if (mCollectionUtility && title.length())
    {
        TRAPD(err, id = createAlbumL(title));
        if(err)
        {
            id = TMPXItemId::InvalidId();
        }
    }

    return id;
}

// -----------------------------------------------------------------------------
// removeAlbums
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::removeAlbums(const QList<TMPXItemId> &mediaIds)
{
    int err(-1);
    
    if (mCollectionUtility)
    {
        TRAP(err, removeAlbumsL(mediaIds));
    }

    return err;
}

// -----------------------------------------------------------------------------
// addItemsInAlbum
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::addItemsInAlbum(TMPXItemId albumId,
    const QList<TMPXItemId> &mediaIds)
{
    int err(-1);
    
    if (mCollectionUtility && albumId != TMPXItemId::InvalidId() &&
        albumId.iId2 == KVcxMvcMediaTypeAlbum)
    {
        TRAP(err, addItemsInAlbumL(albumId, mediaIds));
    }
    
    return err;
}

// -----------------------------------------------------------------------------
// startOpenCollectionL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::startOpenCollectionL(int level)
{
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    }
    CMPXCollectionPath* collectionPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL( collectionPath );
  
    collectionPath->AppendL( KVcxUidMyVideosMpxCollection );
    if (level == VideoCollectionCommon::ELevelVideos)
    {
    	collectionPath->AppendL( KVcxMvcCategoryIdAll );
    	
		mOpenCategoryAlbum.iId1 = KVcxMvcCategoryIdAll;
    	mOpenCategoryAlbum.iId2 = 1;
    	
    	mCollectionPathLevel = VideoCollectionCommon::ELevelVideos;
    }
    else
    {
        mOpenCategoryAlbum = TMPXItemId::InvalidId();
    	
    	mCollectionPathLevel = VideoCollectionCommon::ELevelCategory;
    }
    mCollectionUtility->Collection().OpenL( *collectionPath );
    CleanupStack::PopAndDestroy( collectionPath );  

    mCollectionOpenStatus = ECollectionOpening;
}


// -----------------------------------------------------------------------------
// deleteVideosL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::deleteVideosL(QList<TMPXItemId> &mediaIds)
{
    if(!mCollectionUtility || mediaIds.count() == 0)
    {
        User::Leave(KErrGeneral);
    }
    CMPXMediaArray* mediasToDelete = CMPXMediaArray::NewL();
    CleanupStack::PushL( mediasToDelete );
    
    CMPXMedia* media = NULL;

    TMPXItemId currentId;
    foreach(currentId, mediaIds)
    {
        media = CMPXMedia::NewL();
        CleanupStack::PushL( media );
        media->SetTObjectValueL( KMPXMediaGeneralId, currentId );
        mediasToDelete->AppendL( *media );
        CleanupStack::PopAndDestroy( media );
    }
    
    CMPXCommand* cmd = CMPXMedia::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosDelete );    
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, 
                           TUid::Uid( KVcxUidMyVideosMpxCollection ) );
    cmd->SetCObjectValueL( KMPXMediaArrayContents, mediasToDelete );

    mCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );  
    CleanupStack::PopAndDestroy( mediasToDelete ); 
}

// -----------------------------------------------------------------------------
// openVideoL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::openVideoL(TMPXItemId &videoId)
{
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    } 
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( KVcxUidMyVideosMpxCollection );
    path->AppendL( KVcxMvcCategoryIdAll );
    path->AppendL( videoId );
    path->SelectL( videoId );
      
    mCollectionUtility->Collection().OpenL( *path );
    CleanupStack::PopAndDestroy( path );  
}

// -----------------------------------------------------------------------------
// openCategoryL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::openCategoryL(TMPXItemId &id)
{    
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    } 
    CMPXCollectionPath* collectionPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL( collectionPath );
  
    collectionPath->AppendL( KVcxUidMyVideosMpxCollection );
    collectionPath->AppendL( id );
    mCollectionUtility->Collection().OpenL( *collectionPath );
    CleanupStack::PopAndDestroy( collectionPath );  
    
	mOpenCategoryAlbum = id;
	mCollectionPathLevel = VideoCollectionCommon::ELevelAlbum;
    mCollectionOpenStatus = ECollectionOpening;
}

// -----------------------------------------------------------------------------
// backL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::backL()
{
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    } 
    
    if (getCollectionLevel() > 2 )
    {
    	mCollectionUtility->Collection().BackL();
    	mCollectionOpenStatus = ECollectionOpening;
    	
    	mOpenCategoryAlbum = TMPXItemId::InvalidId();

    	mCollectionPathLevel = VideoCollectionCommon::ELevelCategory;
    }
}

// -----------------------------------------------------------------------------
// getVideoDetailsL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::getVideoDetailsL(TMPXItemId &videoId)
{
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    }
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosGetMediaFullDetailsByMpxId );
    cmd->SetTObjectValueL( KMPXMediaGeneralId, videoId );
    mCollectionUtility->Collection().CommandL( *cmd );
    
    CleanupStack::PopAndDestroy( cmd );  
}

// -----------------------------------------------------------------------------
// removeAlbumsL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::removeAlbumsL(const QList<TMPXItemId> &mediaIds)
{
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    }

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL(KVcxMediaMyVideosCommandId, KVcxCommandMyVideosRemoveAlbums);
    cmd->SetTObjectValueL(KMPXCommandGeneralDoSync, EFalse);
    cmd->SetTObjectValueL(KMPXCommandGeneralCollectionId, TUid::Uid(KVcxUidMyVideosMpxCollection));
  
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL( array );
    CMPXMedia* media = 0;
    int count = mediaIds.count();
    for (int i = 0; i < count; i++)
    {
        media = CMPXMedia::NewL();
        CleanupStack::PushL(media);
        media->SetTObjectValueL(KMPXMediaGeneralId, mediaIds.at(i));
        array->AppendL(*media);
        CleanupStack::PopAndDestroy(media);
    }
    cmd->SetCObjectValueL(KMPXMediaArrayContents, array);

    mCollectionUtility->Collection().CommandL(*cmd);

    CleanupStack::PopAndDestroy( array );  
    CleanupStack::PopAndDestroy( cmd );  
}

// -----------------------------------------------------------------------------
// createAlbumL
// -----------------------------------------------------------------------------
//
TMPXItemId VideoCollectionClient::createAlbumL(const QString &title)
{
    TMPXItemId albumId = TMPXItemId::InvalidId();
    
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    // 1. create album
    TPtrC titlePtrC(title.utf16());
    cmd->SetTObjectValueL(KMPXCommandGeneralId, KVcxCommandIdMyVideos);
    cmd->SetTObjectValueL(KVcxMediaMyVideosCommandId, KVcxCommandMyVideosAddAlbum);
    cmd->SetTextValueL(KMPXMediaGeneralTitle, titlePtrC);
    cmd->SetTObjectValueL(KMPXCommandGeneralDoSync, ETrue);
    cmd->SetTObjectValueL(KMPXCommandGeneralCollectionId, TUid::Uid(KVcxUidMyVideosMpxCollection));
    
    mCollectionUtility->Collection().CommandL(*cmd);
    
    // get album id
    if (cmd->IsSupported(KMPXMediaGeneralId))
    {
        albumId = cmd->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);        
    }

    // cleanup
    CleanupStack::PopAndDestroy(cmd);

    return albumId;
}

// -----------------------------------------------------------------------------
// addItemsInAlbumL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::addItemsInAlbumL(TMPXItemId albumId,
    const QList<TMPXItemId> &mediaIds)
{
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL(cmd);
    cmd->SetTObjectValueL(KMPXCommandGeneralId, KVcxCommandIdMyVideos);
    cmd->SetTObjectValueL(KVcxMediaMyVideosCommandId, KVcxCommandMyVideosAddToAlbum);
    cmd->SetTObjectValueL(KVcxMediaMyVideosUint32Value, albumId.iId1);
    cmd->SetTObjectValueL(KMPXCommandGeneralDoSync, EFalse);
    cmd->SetTObjectValueL(KMPXCommandGeneralCollectionId, TUid::Uid(KVcxUidMyVideosMpxCollection));

    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL( array );
    CMPXMedia* video = 0;
    int count = mediaIds.count();
    for (int i = 0; i < count; i++)
    {
        video = CMPXMedia::NewL();
        CleanupStack::PushL(video);
        video->SetTObjectValueL(KMPXMediaGeneralId, mediaIds.at(i));
        array->AppendL(*video);
        CleanupStack::PopAndDestroy(video);
    }
    cmd->SetCObjectValueL(KMPXMediaArrayContents, array);

    mCollectionUtility->Collection().CommandL(*cmd);

    CleanupStack::PopAndDestroy(array);
    CleanupStack::PopAndDestroy(cmd);
}

// -----------------------------------------------------------------------------
// fetchMpxMediaByMpxIdL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::fetchMpxMediaByMpxIdL(TMPXItemId &aMpxId)
{
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    }

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosGetMediasByMpxId );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId,
                                   TUid::Uid( KVcxUidMyVideosMpxCollection ) );
   
    CMPXMediaArray* idMediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL( idMediaArray );
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    media->SetTObjectValueL( KMPXMessageMediaGeneralId, aMpxId );
    idMediaArray->AppendL( *media );
    cmd->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, idMediaArray );
    cmd->SetTObjectValueL( KMPXMediaArrayCount, idMediaArray->Count() );
    mCollectionUtility->Collection().CommandL( *cmd );
   
    CleanupStack::PopAndDestroy( media );  
    CleanupStack::PopAndDestroy( idMediaArray );  
    CleanupStack::PopAndDestroy( cmd );  
}

