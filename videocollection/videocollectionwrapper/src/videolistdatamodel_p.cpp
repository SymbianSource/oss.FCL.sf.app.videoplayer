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
* Description: VideoListDataModelPrivate class implementation
* 
*/

// INCLUDE FILES

#include <mpxmediageneraldefs.h>
#include <thumbnailmanager_qt.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <qvariant.h>

#include "videolistdatamodel.h"
#include "videocollectionwrapper.h"
#include "videolistdatamodel_p.h"
#include "videothumbnaildata.h"
#include "vcxmyvideosdefs.h"
#include "videocollectionutils.h"
#include "videocollectioncommon.h"

/**
 * private global inline hash function for TMPXItemId keys in QSet
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoListDataModelPrivate
// -----------------------------------------------------------------------------
//
VideoListDataModelPrivate::VideoListDataModelPrivate(VideoListDataModel *model) :
q_ptr(model),
mVideoThumbnailData(VideoThumbnailData::instance()),
mInitialized(false)
{
}
	
// -----------------------------------------------------------------------------
// ~VideoListDataModelPrivate
// -----------------------------------------------------------------------------
//
VideoListDataModelPrivate::~VideoListDataModelPrivate()
{
    mMediaData.clear();
}

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
//
int VideoListDataModelPrivate::initialize()
{
    if(mInitialized)
    {
        return 0;
    }
    if(!connect( &mVideoThumbnailData, SIGNAL(thumbnailsFetched( QList<TMPXItemId> )), 
                  this, SLOT(thumbnailsFetchedSlot( QList<TMPXItemId> ))))
    {
        return -1;
    }

    mInitialized = true;
    return 0;
}

// -----------------------------------------------------------------------------
// getVideoCount
// -----------------------------------------------------------------------------
//
int VideoListDataModelPrivate::getVideoCount()
{
    return mMediaData.count();
}

// -----------------------------------------------------------------------------
// getMediaIdFromIndex
// -----------------------------------------------------------------------------
//
TMPXItemId VideoListDataModelPrivate::getMediaIdFromIndex( int index ) const
{
    return mMediaData.idFromIndex(index);
}

// -----------------------------------------------------------------------------
// getVideoNameFromIndex
// -----------------------------------------------------------------------------
//
const QString VideoListDataModelPrivate::getVideoNameFromIndex( int index )  const
{
    QString txt;
    CMPXMedia* media = mMediaData.fromIndex(index);
    VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralTitle, txt );
    return txt;
}
 
// -----------------------------------------------------------------------------
// getVideoThumbnailFromIndex
// -----------------------------------------------------------------------------
//
const QIcon* VideoListDataModelPrivate::getVideoThumbnailFromIndex( int index )  const
{
    return mVideoThumbnailData.getThumbnail( mMediaData.idFromIndex(index) );
}

// -----------------------------------------------------------------------------
// getVideoSizeFromIndex
// -----------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getVideoSizeFromIndex( int index ) const
{
    quint32 size(0);
    CMPXMedia *media = mMediaData.fromIndex(index);
    VideoCollectionUtils::instance().mediaValue<quint32>(media, KMPXMediaGeneralSize, size );    
    return size;    
}

// -----------------------------------------------------------------------------
// getVideoAgeProfileFromIndex
// ---------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getVideoAgeProfileFromIndex( int index ) const
{
    quint32 ageProfile(0);
    CMPXMedia *media = mMediaData.fromIndex(index);
    VideoCollectionUtils::instance().mediaValue<quint32>(media, KVcxMediaMyVideosAgeProfile, ageProfile );    
    return ageProfile; 
}

// -----------------------------------------------------------------------------
// getVideodurationFromIndex
// -----------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getVideodurationFromIndex( int index ) const
{
    quint32 returnDuration(0);
    float duration(0);
    CMPXMedia *media = mMediaData.fromIndex(index);
    VideoCollectionUtils::instance().mediaValue<float>(media, KVcxMediaMyVideosDuration, duration );
    returnDuration = static_cast<quint32>(duration);
    return returnDuration;
}

// -----------------------------------------------------------------------------
// getVideoDateFromIndex
// -----------------------------------------------------------------------------
//
QDateTime VideoListDataModelPrivate::getVideoDateFromIndex( int index ) const
{
    QDateTime date;
    quint64 dateTimeValue;
    if(!VideoCollectionUtils::instance().mediaValue<quint64>(mMediaData.fromIndex(index), KMPXMediaGeneralDate, dateTimeValue)) 
    {
        return date;
    }
    TDateTime temp = TTime( dateTimeValue ).DateTime();
    date = QDateTime(QDate(temp.Year(), temp.Month()+1, temp.Day()+1), 
                       QTime(temp.Hour(), temp.Minute(), temp.Second(), temp.MicroSecond()));
    return date;
}

// -----------------------------------------------------------------------------
// getMetaDataFromIndex
// -----------------------------------------------------------------------------
//
QMap<QString, QVariant> VideoListDataModelPrivate::getMetaDataFromIndex(int index) const
{
    using namespace VideoCollectionCommon;
    QMap<QString, QVariant> map;
    CMPXMedia *media = mMediaData.fromIndex(index);
    if(!media) {
        return map;
    }

    // MetaKeyDate
    QDateTime date = getVideoDateFromIndex(index);
    if(date.isValid()) {
        map[MetaKeyDate] = date.date();
    }
    
    // MetaKeyDurationString
    quint32 dur = getVideodurationFromIndex(index);
    QString duration = VideoCollectionUtils::instance().prepareLengthString(dur);
    if(!duration.isNull() && !duration.isEmpty()) {
        map[MetaKeyDurationString] = duration;
    }
    
    // MetaKeySizeString
    quint32 s = getVideoSizeFromIndex(index);
    QString size = VideoCollectionUtils::instance().prepareSizeString(s);
    if(!size.isNull() && !size.isEmpty()) {
        map[MetaKeySizeString] = size; 
    }
    
    // MetaKeyStarRating
    quint8 rating = 0;
    if(VideoCollectionUtils::instance().mediaValue<quint8>(media, KVcxMediaMyVideosRating, rating)) 
    {
        map[MetaKeyStarRating] = rating;
    }
    
    // MetaKeyDRMInfo
    
    // MetaKeyServiceURL
    
    // MetaKeyDescription
    QString desc;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralComment, desc)) {
        map[MetaKeyDescription] = desc;
    }
    
    // MetaKeyModifiedDate
    quint64 dateTimeValue;
    if(VideoCollectionUtils::instance().mediaValue<quint64>(media, KVcxMediaMyVideosModifiedDate, dateTimeValue)) {
        TDateTime temp = TTime( dateTimeValue ).DateTime();
        QDateTime date = QDateTime(QDate(temp.Year(), temp.Month()+1, temp.Day()), 
                           QTime(temp.Hour(), temp.Minute(), temp.Second(), temp.MicroSecond()));
        map[MetaKeyModifiedDate] = date.date();
    }

    // MetaKeyShotLocation
    
    // MetaKeyAuthor
    QString author;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KVcxMediaMyVideosAuthor, author)) {
        map[MetaKeyAuthor] = author;
    }
    
    // MetaKeyCopyright
    QString copyright;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralCopyright, copyright)) {
        map[MetaKeyCopyright] = copyright;
    }
    
    // MetaKeyAudioType
    
    // MetaKeyLanguageString
    QString language;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KVcxMediaMyVideosAudioLanguage, language)) {
        map[MetaKeyLanguageString] = language;
    }
    
    // MetaKeyKeywords
    
    // MetaKeyVideoResolutionString
    
    // MetaKeyBitRate
    
    // MetaKeyFormat
    QString format;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralMimeType, format)) {
        map[MetaKeyFormat] = format;
    }
    
    return map;
}

// -----------------------------------------------------------------------------
// getVideoStatusFromIndex
// -----------------------------------------------------------------------------
//
int VideoListDataModelPrivate::getVideoStatusFromIndex(int index) const
{
    int status = 0;
    if(mItemsUnderDeletion.contains(getMediaIdFromIndex(index)))
    {
        status = VideoCollectionCommon::StatusDeleted;        
    }
    return status;
}

// -----------------------------------------------------------------------------
// markVideosRemoved
// -----------------------------------------------------------------------------
//
QList<TMPXItemId> VideoListDataModelPrivate::markVideosRemoved(const QModelIndexList &itemIndexes)
{
    QList<TMPXItemId> idList;
    TMPXItemId id;
    QModelIndexList::const_iterator iter = itemIndexes.constBegin();
    while(iter != itemIndexes.constEnd())
    {
        id = getMediaIdFromIndex((*iter).row());
        if(id != TMPXItemId::InvalidId())
        {
            mItemsUnderDeletion.insert(id);
            idList.append(id);
        }
        iter++;
    }
    return idList;
}

// -----------------------------------------------------------------------------
// unMarkVideosRemoved
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::unMarkVideosRemoved(QList<TMPXItemId> &itemIds)
{
    QList<TMPXItemId>::const_iterator iter = itemIds.constBegin();
    while(iter != itemIds.constEnd())
    {
        mItemsUnderDeletion.remove(*iter);
        iter++;
    }
}

// -----------------------------------------------------------------------------
// getFilePathFromIndex
// -----------------------------------------------------------------------------
//
const QString VideoListDataModelPrivate::getFilePathFromIndex(int index) const
{
    QString filePath;
    VideoCollectionUtils::instance().mediaValue<QString>(mMediaData.fromIndex(index), 
            KMPXMediaGeneralUri, filePath );
    return filePath;
}

// -----------------------------------------------------------------------------
// getFilePathForId
// -----------------------------------------------------------------------------
//
const QString VideoListDataModelPrivate::getFilePathForId(TMPXItemId mediaId) const
{
    QString filePath;
    int index = mMediaData.indexOfId(mediaId);
    VideoCollectionUtils::instance().mediaValue<QString>(mMediaData.fromIndex(index), 
            KMPXMediaGeneralUri, filePath );
    return filePath;
}

// -----------------------------------------------------------------------------
// getMediaId
// -----------------------------------------------------------------------------
//
TMPXItemId VideoListDataModelPrivate::getMediaId(const CMPXMedia *media ) const
{
    TMPXItemId mediaId = TMPXItemId::InvalidId();
    
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>(media, KMPXMediaGeneralId, mediaId );

    return mediaId;
}

// -----------------------------------------------------------------------------
// appendDataToContainerL
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::appendDataToContainerL(CMPXMediaArray *videoArray, unsigned int startIndex)
{
    if(!videoArray || startIndex >= videoArray->Count())
    {
        return;
    }
    // get id used check wether we need category or video list masking
    TMPXItemId mpxFirstId = TMPXItemId::InvalidId(); 
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>(videoArray->AtL(0), KMPXMediaGeneralId, mpxFirstId );
    if(mpxFirstId == TMPXItemId::InvalidId())
    {
        // invalid
        return;
    }
    
    int count = videoArray->Count();
    CMPXMedia *newMedia = 0;
    TMPXItemId itemId = TMPXItemId::InvalidId();
    for(int i = startIndex; i < count; ++i)
    {
        newMedia = 0;
        itemId = TMPXItemId::InvalidId();
        itemId = getMediaId(videoArray->AtL(i));
        // type of list is taken from the first item 
        // remaining items are accepted only if they match the type
        // TODO: currently albums are filtered out
        if((mpxFirstId.iId2 == 0 && itemId != TMPXItemId::InvalidId()) || 
           (mpxFirstId.iId2 >= 1  && (itemId.iId1 == KVcxMvcCategoryIdDownloads) || 
           (itemId.iId1 == KVcxMvcCategoryIdCaptured)) && mpxFirstId.iId2 == itemId.iId2)
        {
            newMedia = CMPXMedia::NewL(*(videoArray->AtL(i)));            
            mMediaData.append(newMedia);
        }
    }    
}

// -----------------------------------------------------------------------------
// thumbnailsFetchedSlot
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::thumbnailsFetchedSlot(QList<TMPXItemId> mediaIds)
{
    // TODO: find way to optimize dataChanged events 
    
    TMPXItemId id;
    QModelIndex rowIndex;
    
    while(!mediaIds.isEmpty())
    {
        id = mediaIds.takeFirst();
        rowIndex = q_ptr->index(mMediaData.indexOfId(id), 0);
		if(rowIndex.isValid())
		{
	        emit dataChanged( rowIndex, rowIndex);
		}
    }
}

// -----------------------------------------------------------------------------
// newVideoListSlot
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::newVideoListSlot( CMPXMediaArray *newVideoList )
{
    if( !newVideoList )
    {
        return;
    }

    int count = newVideoList->Count();
   
    // clear old content
    if((mMediaData.count() > 0))
    {   
        q_ptr->beginRemoveRows(QModelIndex(), 0, mMediaData.count() - 1);
        mMediaData.clear();
        q_ptr->endRemoveRows();
    }
    
    // decide wether we we are inserting new or not, if provided list is empty -> we do not insert any videos
    count == 0 ? q_ptr->beginInsertRows(QModelIndex(), 0, 0) : q_ptr->beginInsertRows(QModelIndex(), 0, count - 1);

    TRAP_IGNORE(appendDataToContainerL(newVideoList));
	
	q_ptr->endInsertRows();
	emit q_ptr->modelReady();
}

// -----------------------------------------------------------------------------
// appendVideoListSlot
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::appendVideoListSlot(CMPXMediaArray* videoList)
{
    if(!videoList)
    {
        return;
    }
    if(videoList->Count() == 0 || videoList->Count() <= mMediaData.count())
    {
        // nothing to append or count matches, no changes -> nothing to append
        return;
    }
            
    // array gotten is in same order as existing data from the start,
    // so we only need to go throught the appendding array from the index
    // at position where first new items is excepted to exists.    
    int startIndex = mMediaData.count();
    
    q_ptr->beginInsertRows(QModelIndex(), startIndex, videoList->Count() - 1);

    TRAP_IGNORE(appendDataToContainerL(videoList, startIndex));

    q_ptr->endInsertRows();
}

// -----------------------------------------------------------------------------
// newVideoAvailable
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::newVideoAvailableSlot(CMPXMedia *newVideo)
{
    if(!newVideo ) 
    {
        return;        
    }

    // try to get id of the video, if not found consider invalid clip
    TMPXItemId mediaId = getMediaId(newVideo);
    if( mediaId == TMPXItemId::InvalidId())
    {
        return;
    }

    // append video into list, ownership of provided mediaobject is not transferred, 
    // so we need to create copy of CMPXMedia
    CMPXMedia *videoToAppend = 0;
    TRAP_IGNORE(videoToAppend = CMPXMedia::NewL(*newVideo));  
    
    q_ptr->beginInsertRows(QModelIndex(), mMediaData.count(), mMediaData.count());
    
    mMediaData.append(videoToAppend);    
  
    q_ptr->endInsertRows();

}

// -----------------------------------------------------------------------------
// videoDeleted
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::videoDeletedSlot(TMPXItemId videoId)
{
    int index = -1;

    if(mItemsUnderDeletion.contains(videoId))
    {
        mItemsUnderDeletion.remove(videoId);
    }
    index = mMediaData.indexOfId(videoId);
 
    if(index >= 0 && index < mMediaData.count())
    {     
        q_ptr->beginRemoveRows(QModelIndex(), index, index);
        mMediaData.remove(videoId);
        mVideoThumbnailData.removeThumbnail( videoId );
        q_ptr->endRemoveRows();
    }  
}

// -----------------------------------------------------------------------------
// videoDeleteCompleted
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::videoDeleteCompletedSlot(int overallCount, QList<TMPXItemId> *failedMediaIds)
{
    // overallCount not used at the moment, since there is no need of
    // reporting UI about succeed deletion
    Q_UNUSED(overallCount);
    
    if(!failedMediaIds)
    {
        return;
    }
    QModelIndex failedIndex;
    int failedCount = 0;
    
    // delete requested from UI
    // go failed list throught and remove all failed from 
    // list under deletion
    QList<TMPXItemId>::const_iterator iter = failedMediaIds->constBegin();
    while(iter != failedMediaIds->constEnd())
    {
        if(mItemsUnderDeletion.contains(*iter))
        {
            // failure found, remove from list and notify UI about changed status
            failedIndex = q_ptr->index(mMediaData.indexOfId(*iter), 0);
            mItemsUnderDeletion.remove(*iter);  
            failedCount++;
            emit dataChanged( failedIndex, failedIndex);
        }
        iter++;
    }
    // send status throught videocollectionwraper:
    // - failed status, if some user requested deletion failed:
    // --- if one item, get name and pass it along signal
    // --- if multiple items, pass count of items
    if(failedCount > 0)
    {     
        VideoCollectionWrapper *wrapper = VideoCollectionWrapper::instance();
        QVariant data;
        if(failedCount == 1)
        {
            data = getVideoNameFromIndex(failedIndex.row());
            wrapper->sendAsyncStatus(VideoCollectionCommon::statusSingleDeleteFail, data); 

        }
        else
        {
            data = failedCount;
            wrapper->sendAsyncStatus(VideoCollectionCommon::statusMultipleDeleteFail, data);  
        } 
        wrapper->decreaseReferenceCount();
        wrapper = 0;
    } 
}

// -----------------------------------------------------------------------------
// VideoDetailsCompleted 
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::videoDetailsCompletedSlot(TMPXItemId videoId)
{
    int index = mMediaData.indexOfId(videoId);
    if(index >= 0)
    {
        emit videoDetailsReady(index);
    }
}

// End of file
