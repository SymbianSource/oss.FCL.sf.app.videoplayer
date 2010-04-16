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

#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessage2.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>

#include "videocollectionlistener.h"
#include "videocollectionclient.h"
#include "videodatasignalreceiver.h"
#include "videocollectionutils.h"
#include "videocollectioncommon.h"

// -----------------------------------------------------------------------------
// VideoCollectionListener
// -----------------------------------------------------------------------------
//
VideoCollectionListener::VideoCollectionListener(VideoCollectionClient &collectionClient,
                                                VideoDataSignalReceiver &signalReceiver) : 
mCollectionClient(collectionClient),
mSignalReceiver(signalReceiver),
mVideoUtils(VideoCollectionUtils::instance())
{

}

// -----------------------------------------------------------------------------
// ~VideoCollectionListener
// -----------------------------------------------------------------------------
//
VideoCollectionListener::~VideoCollectionListener()
{

}

// -----------------------------------------------------------------------------
// HandleCollectionMediaL
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::HandleCollectionMediaL( 
        const CMPXMedia& /*aMedia*/,
        TInt /*aError*/)
{
   // NOP
}

// -----------------------------------------------------------------------------
// HandleOpenL
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::HandleOpenL( 
        const CMPXMedia& aEntries,
        TInt /*aIndex*/,
        TBool /*aComplete*/,
        TInt aError)
{
    if(aError != KErrNone)
    {
        return;
    }

    // Check that current level is valid and entries has collection path. 
    if(mCollectionClient.getCollectionLevel() < VideoCollectionCommon::ELevelCategory || 
       !aEntries.IsSupported(KMPXMediaGeneralContainerPath))
    {
        return;
    }
    
    CMPXMediaArray *array =
                    mVideoUtils.mediaValuePtr<CMPXMessageArray>(&aEntries, KMPXMediaArrayContents);
    if(!array)
    {
        // no videos!
        return;
    }

    CMPXCollectionPath* path = aEntries.Value<CMPXCollectionPath>(KMPXMediaGeneralContainerPath); 
    if(!path)
	{
        return;
	}

    TMPXItemId pathId = path->Id();
    
    TBool categoryOrAlbumVideoList = false;
    if(path->Levels() == VideoCollectionCommon::PathLevelVideos && pathId.iId2 != KVcxMvcMediaTypeVideo)
	{
        categoryOrAlbumVideoList = true;
	}

    if(categoryOrAlbumVideoList)
    {
        mSignalReceiver.albumListAvailableSlot(pathId, array);

        // Update also all video list in case this is a default category. 
        if(pathId.iId2 == KVcxMvcMediaTypeCategory)
		{
            mSignalReceiver.newVideoListSlot(array);
		}
    }
    else
    {
        mSignalReceiver.newVideoListSlot(array);
    }
 }

// -----------------------------------------------------------------------------
// HandleOpenL
// -----------------------------------------------------------------------------
//                      
void VideoCollectionListener::HandleOpenL( 
        const CMPXCollectionPlaylist& /*aPlaylist*/,
        TInt /*aError*/)
{
    // NOP
}

// -----------------------------------------------------------------------------
// HandleCommandComplete
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::HandleCommandComplete( 
        CMPXCommand* aCommandResult, 
        TInt aError)
{
    if(aError != KErrNone || !aCommandResult)
    {
        return;
    }
    int commandId = -1;
    
    if(mVideoUtils.mediaValue<int>(aCommandResult, KVcxMediaMyVideosCommandId, commandId))
    {
        if(commandId == KVcxCommandMyVideosGetMediaFullDetailsByMpxId)
        {
            handleGetVideoDetailsResp(aCommandResult);
        }
        else if(commandId == KVcxCommandMyVideosRemoveAlbums)
        {
			CMPXMediaArray *messageArray = 
				mVideoUtils.mediaValuePtr<CMPXMediaArray>(aCommandResult, KMPXMediaArrayContents);
			
			if(!messageArray || messageArray->Count() == 0)
			{
				return;
			}
			QList<TMPXItemId> failedIds;    
			TMPXItemId itemId;  
			int count = messageArray->Count();
			int failedFlag = 0;
			CMPXMedia* item = NULL;
			
			// go throught all removed albums and see if some has failed
			for (int i = 0; i < count; ++i)
			{
				item = (*messageArray)[i];
				if(!mVideoUtils.mediaValue<TMPXItemId>(item, KMPXMediaGeneralId, itemId))
				{
					// invalid message, return 
					return;
				}
				// if there's error while fetching value, it means that value does not exists,
				// so we can assume remove was succefull
				if(mVideoUtils.mediaValue<int>(item, KVcxMediaMyVideosInt32Value, failedFlag))
				{
					if (failedFlag)
					{
						failedIds.append(itemId);
					}
					failedFlag = 0;
				}       
			}
			if (failedIds.count())
			{
				mSignalReceiver.albumRemoveFailureSlot(&failedIds);
			}
        }
    }
}

// -----------------------------------------------------------------------------
// HandleCollectionMessage
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::HandleCollectionMessage( 
        CMPXMessage* aMessage,
        TInt aError )
{
    if(aError)
    {
        return;
    }  

    TMPXMessageId mainMessageId;
    if(!mVideoUtils.mediaValue<TMPXMessageId>(aMessage, KMPXMessageGeneralId, mainMessageId))
    {
        return;
    }  
    
    if(mCollectionClient.getOpenStatus() == VideoCollectionClient::ECollectionOpened )
    {
        // after colletion has been opened we handle messages from our collection plugin only
        TUid collectionUid = {0};
        bool status = mVideoUtils.mediaValue<TUid>(aMessage, KMPXMessageCollectionId, collectionUid);
        if(!status || collectionUid.iUid != KVcxUidMyVideosMpxCollection)
        {
            return;
        }
    }
    else
    {
        // before opening we're just excepting general messages
        if(mainMessageId == KMPXMessageGeneral)
        {
            handleGeneralMPXMessage(aMessage);
        }
        return;
    }
           
    if( mainMessageId == KVcxCommandIdMyVideos)
    {        
        int myVideosMainMsgId = -1; 

        if(!mVideoUtils.mediaValue<int>(aMessage, KVcxMediaMyVideosCommandId, myVideosMainMsgId ))
        {
            return;
        }

        if ( myVideosMainMsgId == KVcxMessageMyVideosMessageArray )
        {
            handleMyVideosMessageArray(aMessage); 
        }
        else
        {
            handleMyVideosMPXMessage(myVideosMainMsgId, aMessage);
        }
    }
    else
    {
        handleMPXMessage(mainMessageId, aMessage);
    }
}

// -----------------------------------------------------------------------------
// handleMyVideosMessageArray
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleMyVideosMessageArray(CMPXMessage *aMessage)
{   
    CMPXMessageArray* messageArray = NULL;
    
    messageArray = mVideoUtils.mediaValuePtr<CMPXMessageArray>(aMessage, KMPXMessageArrayContents);
    if(!messageArray)
    {
        return;
    }
    
    int count = messageArray->Count();    
    int myVideosMsgId;
    TMPXMessageId mpxMessageId;
    
    CMPXMessage *singleMessage;
    for ( int i = 0; i < count; ++i )
    {
        singleMessage = (*messageArray)[i];
        if(mVideoUtils.mediaValue<int>(singleMessage, KVcxMediaMyVideosCommandId, myVideosMsgId))
        {
            handleMyVideosMPXMessage(myVideosMsgId, singleMessage);
        }
        else if(mVideoUtils.mediaValue<TMPXMessageId>(
                                        singleMessage, KMPXMessageGeneralId, mpxMessageId))
        {
            handleMPXMessage(mpxMessageId, singleMessage);
        }
    }    
}

// -----------------------------------------------------------------------------
// handleMyVideosMPXMessage
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleMyVideosMPXMessage(int &myVideosMsgId, CMPXMessage *aMessage)
{
    switch (myVideosMsgId)
    {
        case KVcxMessageMyVideosGetMediasByMpxIdResp:
            handleGetMediasByMpxIdResp(aMessage);
            break;
        case KVcxMessageMyVideosDeleteResp:
            handleMyVideosDeleteMessage(aMessage);
            break;
        case KVcxMessageMyVideosDeleteStarted:
            break;
        case KVcxMessageMyVideosListComplete:
            mCollectionClient.startOpenCurrentState();
            break;
        case KVcxMessageMyVideosItemsAppended:
            mCollectionClient.startOpenCurrentState();
            break;
        case KVcxMessageMyVideosMoveOrCopyStarted:
            break;
        case KVcxMessageMyVideosMoveResp:
            break;
        case KVcxMessageMyVideosCopyResp:
            break;
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
// handleCommonMPXMessage
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleMPXMessage(TMPXMessageId &mpxMessageId, CMPXMessage *aMessage)
{
    switch(mpxMessageId)
    {
        case KMPXMessageGeneral:
            handleGeneralMPXMessage(aMessage);
        break;
        case KMPXMessageIdItemChanged:
            handleMyVideosItemsChanged(aMessage);
        break;
        default:
        break;
    }
}

// -----------------------------------------------------------------------------
// handleGeneralMPXMessage
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleGeneralMPXMessage(CMPXMessage* aMessage)
{
    if(mCollectionClient.getOpenStatus() != VideoCollectionClient::ECollectionOpening)
    {
        return;
    }
    int event = 0;
    if(!mVideoUtils.mediaValue<int>(aMessage, KMPXMessageGeneralEvent, event))
    {
        return;
    }

    int data = 0;
    if(!mVideoUtils.mediaValue<int>(aMessage, KMPXMessageGeneralData, data))
    {
        return;
    }

    if( event == TMPXCollectionMessage::EPathChanged &&    
        data == EMcContainerOpened )
    {
        mCollectionClient.setOpenStatus(VideoCollectionClient::ECollectionOpened);
    }
}

// -----------------------------------------------------------------------------
// handleMyVideosItemsChanged
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleMyVideosItemsChanged(CMPXMessage* aMessage)
{

    TMPXChangeEventType eventType = EMPXItemModified; 
    if(!mVideoUtils.mediaValue<TMPXChangeEventType>(
        aMessage,KMPXMessageChangeEventType, eventType))
    {
        return;
    }
    TMPXItemId itemId = TMPXItemId::InvalidId(); 
    if(!mVideoUtils.mediaValue<TMPXItemId>(
        aMessage,KMPXMessageMediaGeneralId, itemId))
    {       
        return;
    }
    
    switch(eventType)
    {
        case EMPXItemDeleted:
        {
            mSignalReceiver.itemDeletedSlot(itemId);
            break;
        }
        case EMPXItemInserted:
        {
            CMPXMedia *media = mVideoUtils.mediaValuePtr<CMPXMedia>(
                aMessage, KMPXCommandColAddMedia);
            if (media)
            {
                mSignalReceiver.newVideoAvailableSlot(media); 
            }
            else
            {
                mCollectionClient.fetchMpxMediaByMpxId(itemId);
            }
            break;
        }
        case EMPXItemModified:
        {
            if (itemId.iId2 == KVcxMvcMediaTypeAlbum)
            {
                // re-open the album in case album corresponds recently opened.
                // to fetch the album contents.
                mCollectionClient.openItem(itemId);
            }
            break;
        }
        default:
        {
            // invalid event type
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// handleMyVideosDeleteMessage
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleMyVideosDeleteMessage(CMPXMessage* aMessage)
    {

    CMPXMediaArray *messageArray = 
        mVideoUtils.mediaValuePtr<CMPXMediaArray>(aMessage, KMPXMediaArrayContents);
    if(!messageArray || messageArray->Count() == 0)
    {
        return;
    }
    
    QList<TMPXItemId> failedIds;    
    TMPXItemId itemId;  
    int count = messageArray->Count();
    int failedFlag = 0;
    CMPXMedia* item = NULL;
    
    // go throught all removed videos and see if some has failed
    for (int i = 0; i < count; ++i)
    {
        item = (*messageArray)[i];
        if(!mVideoUtils.mediaValue<TMPXItemId>(item, KMPXMediaGeneralId, itemId))
        {
            // invalid message, return 
            return;
        }
        // if there's error while fetching value, it means that value does not exists,
        // so we can assume deletion was succefull
        if(mVideoUtils.mediaValue<int>(item, KVcxMediaMyVideosInt32Value, failedFlag))
        {
            if (failedFlag)
            {
                failedIds.append(itemId);
            }
            failedFlag = 0;
        }       
    }
    mSignalReceiver.videoDeleteCompletedSlot(count, &failedIds);
}

// -----------------------------------------------------------------------------
// handleGetMediasByMpxIdResp
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleGetMediasByMpxIdResp(CMPXMessage* aMessage)
{
    
    CMPXMediaArray* array = 
        mVideoUtils.mediaValuePtr<CMPXMediaArray>(aMessage, KMPXMediaArrayContents);
    if(!array || array->Count() < 1)
    {
        return;
    }
    mSignalReceiver.newVideoAvailableSlot((*array)[0]);    
}

// -----------------------------------------------------------------------------
// handleGetVideoDetailsResp
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleGetVideoDetailsResp(CMPXMessage* aMessage)
{
    CMPXMedia *item = mVideoUtils.mediaValuePtr<CMPXMedia>(aMessage,KMPXCommandColAddMedia); 
    if(!item)
    {
        return;
    }
    TMPXItemId itemId;
    if( !mVideoUtils.mediaValue<TMPXItemId>(item, KMPXMediaGeneralId, itemId))
    {
        return;
    }
    mSignalReceiver.videoDetailsCompletedSlot(itemId);
}

