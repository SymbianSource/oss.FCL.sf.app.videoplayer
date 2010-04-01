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
mOpenCategoryAlbum(0,9),
mCollectionPathLevel(VideoCollectionClient::ELevelInvalid)
{

}

// -----------------------------------------------------------------------------
// initialize()
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::initialize()
{
    if(mCollectionUtility && mCollectionListener)
    {
        // already initialized
        return 0;
    }
    if(!mCollectionListener)
    {
        mCollectionListener = new VideoCollectionListener(*this);
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


// -----------------------------------------------------------------------------
// setVideoModelObserver
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::connectCollectionSignalReceiver(
                                            VideoDataSignalReceiver *signalReceiver)
{
    if(!mCollectionListener || !signalReceiver)
    {   
        return -1;
    }


    if(!QObject::connect(mCollectionListener, SIGNAL(newVideoList(CMPXMediaArray*)),
                       signalReceiver, SLOT(newVideoListSlot(CMPXMediaArray*)),
                       Qt::DirectConnection))
    {
        return -1;
    }
    
    if(!QObject::connect(mCollectionListener, SIGNAL(videoListAppended(CMPXMediaArray*)),
                           signalReceiver, SLOT(appendVideoListSlot(CMPXMediaArray*)),
                           Qt::DirectConnection))
    {
        return -1;
    }
    
    if(!QObject::connect(mCollectionListener, SIGNAL(newVideoAvailable(CMPXMedia*)),
                       signalReceiver, SLOT(newVideoAvailableSlot(CMPXMedia*)),
                       Qt::DirectConnection))
    {
        return -1;
    }
    
    if(!QObject::connect(mCollectionListener, SIGNAL(videoDeleted(TMPXItemId)),
                       signalReceiver, SLOT(videoDeletedSlot(TMPXItemId)),
                       Qt::DirectConnection))
    {
        return -1;
    }
    
    if(!QObject::connect(mCollectionListener, SIGNAL(videoDeleteCompleted(int, QList<TMPXItemId>*)),
                       signalReceiver, SLOT(videoDeleteCompletedSlot(int, QList<TMPXItemId>*)),
                       Qt::DirectConnection))
    {
        return -1;
    }
    
    if(!QObject::connect(mCollectionListener, SIGNAL(videoDetailsCompleted(TMPXItemId)),
                       signalReceiver, SLOT(videoDetailsCompletedSlot(TMPXItemId)),
                       Qt::DirectConnection))
    {
        return -1;
    }
    return 0;
}


// ---------------------------------------------------------------------------
// categoryIds
// ---------------------------------------------------------------------------
//
void VideoCollectionClient::getCategoryIds(int& id, int& type)
{
    id = mOpenCategoryAlbum.iId1; //unique id
    type = mOpenCategoryAlbum.iId2; //category or album
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
// openMedia
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::openVideo(TMPXItemId &mediaId)
{
    if(!mCollectionUtility)
    {
        return -1;
    } 
    
    TInt error;
    if (getCollectionLevel() == VideoCollectionClient::ELevelVideos)
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
// addNewCollection
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::addNewCollection(QString name, QString thumbnail, QList<TMPXItemId> mediaIds)
{
    if(!mCollectionUtility)
    {
        return -1;
    }
    
    TRAPD(error, addNewCollectionL(name, thumbnail, mediaIds));
    
    return error;
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
    if (level == VideoCollectionClient::ELevelVideos)
    {
    	collectionPath->AppendL( KVcxMvcCategoryIdAll );
    	
		mOpenCategoryAlbum.iId1 = KVcxMvcCategoryIdAll;
    	mOpenCategoryAlbum.iId2 = 1;
    	
    	mCollectionPathLevel = VideoCollectionClient::ELevelVideos;
    }
    else
    {
    	mOpenCategoryAlbum.iId1 = 0;
    	mOpenCategoryAlbum.iId2 = 9;
    	
    	mCollectionPathLevel = VideoCollectionClient::ELevelCategory;
    }
    mCollectionUtility->Collection().OpenL( *collectionPath );
    CleanupStack::PopAndDestroy( collectionPath );  
    mCollectionListener->setRequestNewMediaArray(true);
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
    path->AppendL( TMPXItemId( videoId, 0 ) );
    path->SelectL( TMPXItemId( videoId, 0 ) );
      
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
    mCollectionListener->setRequestNewMediaArray(true);
    
	mOpenCategoryAlbum.iId1 = id;
	mOpenCategoryAlbum.iId2 = 1;
    
	mCollectionPathLevel = VideoCollectionClient::ELevelVideos;
    
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
    	mCollectionListener->setRequestNewMediaArray(true);
    	mOpenCategoryAlbum.iId1 = 0;
    	mOpenCategoryAlbum.iId2 = 9;
    	
    	mCollectionPathLevel = VideoCollectionClient::ELevelCategory;
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
// addNewCollectionL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::addNewCollectionL(QString name, QString thumbnail, QList<TMPXItemId> mediaIds)
{
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    }
    
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, -1 );
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, ETrue );
    cmd->SetTObjectValueL( KMPXMediaGeneralTitle, name );
    cmd->SetTObjectValueL( KMPXMediaGeneralThumbnail1, thumbnail );
    
    // TODO real command id missing, uncomment only after the collection supports adding collections
//    mCollectionUtility->Collection().CommandL( *cmd );
    
    CleanupStack::PopAndDestroy( cmd );
    
    if(mediaIds.size() > 0) {
        cmd = CMPXCommand::NewL();
        CleanupStack::PushL( cmd );
        CMPXMediaArray* idMediaArray = CMPXMediaArray::NewL();
        CleanupStack::PushL( idMediaArray );
        
        cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
        cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, -1 );
        cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId,
                                       TUid::Uid( KVcxUidMyVideosMpxCollection ) );
        
        // TODO need to add the mpxid of the newly created collection here.
        
        TMPXItemId mediaId;
        foreach(mediaId, mediaIds) {
            CMPXMedia* media = CMPXMedia::NewL();
            CleanupStack::PushL( media );
            media->SetTObjectValueL( KMPXMessageMediaGeneralId, mediaId );
            idMediaArray->AppendL( *media );
            CleanupStack::PopAndDestroy( media );
        }
        
        cmd->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, idMediaArray );
        cmd->SetTObjectValueL( KMPXMediaArrayCount, idMediaArray->Count() );
        
        // TODO real command id missing, only uncomment after collection supports adding collections.
//        mCollectionUtility->Collection().CommandL( *cmd );
        
        CleanupStack::PopAndDestroy( idMediaArray );
        CleanupStack::PopAndDestroy( cmd );
    }
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


