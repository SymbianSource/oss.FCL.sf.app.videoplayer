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
#include "videocollectionclient.h"
#include "videodeleteworker.h"

#include "videolistdatamodel_p.h"
#include "videothumbnaildata.h"
#include "vcxmyvideosdefs.h"
#include "videocollectionutils.h"
#include "videocollectioncommon.h"

/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
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
mVideoThumbnailData(0),
mInitialized(false),
mCurrentAlbum(TMPXItemId::InvalidId())
{
}
	
// -----------------------------------------------------------------------------
// ~VideoListDataModelPrivate
// -----------------------------------------------------------------------------
//
VideoListDataModelPrivate::~VideoListDataModelPrivate()
{
    QHash<TMPXItemId, QSet<TMPXItemId> >::iterator iter = mAlbumData.begin();
    while(iter != mAlbumData.end())
    {
        iter->clear();
        ++iter;
    }
    mAlbumData.clear();
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
    mVideoThumbnailData = &(VideoThumbnailData::instance());
    if(!connect( mVideoThumbnailData, SIGNAL(thumbnailsFetched( QList<TMPXItemId> )), 
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
    if(mVideoThumbnailData)
    {
        return mVideoThumbnailData->getThumbnail( mMediaData.idFromIndex(index) );
    }
    return 0;
}

// -----------------------------------------------------------------------------
// getCategoryVideoCountFromIndex
// -----------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getCategoryVideoCountFromIndex( int index ) const
{
    quint32 count(0);
    CMPXMedia *media = mMediaData.fromIndex(index);
    VideoCollectionUtils::instance().mediaValue<quint32>(media, KVcxMediaMyVideosCategoryItemCount, count );
    return count;
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
    
    //TODO: Localisation
    // MetaKeyDurationString
    quint32 dur = getVideodurationFromIndex(index);
    QString duration = VideoCollectionUtils::instance().prepareLengthString(dur);
    if(!duration.isNull() && !duration.isEmpty()) {
        map[MetaKeyDurationString] = duration;
    }
    
    //TODO: Localisation
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
// markVideosRemoved
// -----------------------------------------------------------------------------
//
TMPXItemId VideoListDataModelPrivate::markVideoRemoved(const QModelIndex &itemIndex)
{
    return mMediaData.markItemRemoved(itemIndex.row());
}

// -----------------------------------------------------------------------------
// restoreRemoved
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::restoreRemoved(QList<TMPXItemId> *idList)
{
    int startIndex = mMediaData.count();
    int restored = mMediaData.restoreRemovedItems(idList);
    if(restored > 0)
    {
        q_ptr->beginInsertRows(QModelIndex(), startIndex, startIndex + (restored - 1));
        q_ptr->endInsertRows();
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
// belongsToAlbum
// -----------------------------------------------------------------------------
//
bool VideoListDataModelPrivate::belongsToAlbum(const TMPXItemId &itemId, TMPXItemId albumId) const
{
    bool contains(false);
    QHash<TMPXItemId, QSet<TMPXItemId> >::const_iterator iter;
    if(albumId == TMPXItemId::InvalidId())
    {    
        // album id not specified, use the current album
        iter = mAlbumData.constFind(mCurrentAlbum);
    }
    else
    {
        iter = mAlbumData.constFind(albumId);
    }     
    if (iter != mAlbumData.constEnd())
    {
        contains = iter->contains(itemId);
    }
    return contains;
}

// -----------------------------------------------------------------------------
// setAlbumInUse
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::setAlbumInUse(TMPXItemId albumId)
{
    mCurrentAlbum = albumId;
}

// -----------------------------------------------------------------------------
// removeItemsFromAlbum
// -----------------------------------------------------------------------------
//
int VideoListDataModelPrivate::removeItemsFromAlbum(TMPXItemId &albumId, 
                                        const QList<TMPXItemId> &items)
{
    QHash<TMPXItemId, QSet<TMPXItemId> >::iterator iter = mAlbumData.find(albumId);
    if(iter == mAlbumData.end())
    {
        return 0;
    }
    int removeCount = 0;
    int count = items.count();
    for(int i = 0; i < count; ++i)
    {
        if(iter->remove(items.at(i)))
        {
            ++removeCount;
        }
    }   
    return removeCount;
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
void VideoListDataModelPrivate::appendDataToContainerL(
    CMPXMediaArray *videoArray,
    unsigned int startIndex)
{
    int count = videoArray->Count();
    if (!videoArray ||
        startIndex >= count)
    {
        return;
    }
    
    CMPXMedia *newMedia = 0;
    CMPXMedia *mediaFromArray = 0;
    TMPXItemId itemId = TMPXItemId::InvalidId();
    for(int i = startIndex; i < count; ++i)
    {
        mediaFromArray = videoArray->AtL(i);
        itemId = getMediaId(mediaFromArray);
        if (isValid(*mediaFromArray, itemId))
        {
            newMedia = CMPXMedia::NewL(*mediaFromArray);
            mMediaData.append(newMedia);
            newMedia = 0;
        }
    }    
}

// -----------------------------------------------------------------------------
// isValid
// -----------------------------------------------------------------------------
//
bool VideoListDataModelPrivate::isValid(const CMPXMedia &media,
    const TMPXItemId &itemId) const
{
    if(itemId == TMPXItemId::InvalidId())
    {
        return false;
    }    
    // temporary to remove all items with out a title
    QString title;
    VideoCollectionUtils::instance().mediaValue<QString>(&media,
        KMPXMediaGeneralTitle,
        title);
    if (!title.length())
    {
        return false;
    }
    // recently played and podcasts are not yet supported
    if(itemId.iId2 == KVcxMvcMediaTypeCategory && 
        (itemId.iId1 != KVcxMvcCategoryIdDownloads &&
         itemId.iId1 != KVcxMvcCategoryIdCaptured))
    {
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// albumDataChangedL
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::albumDataChangedL(TMPXItemId albumId,
    CMPXMediaArray *videoArray)
{
    if (!videoArray || albumId == TMPXItemId::InvalidId())
    {
        return;
    }
    QSet<TMPXItemId> items;
       
    // remove existing
    mAlbumData.remove(albumId);
    
    int videoCount = videoArray->Count();
    CMPXMedia *media = 0;
    TMPXItemId id = TMPXItemId::InvalidId();
    QModelIndex index;
    for (int i = videoCount - 1; i >= 0; i--)
    {
        media = videoArray->AtL(i);
        id = getMediaId(media);
        if (id != TMPXItemId::InvalidId() && id.iId2 == KVcxMvcMediaTypeVideo)
        {          
            items.insert(id);
        }
    }  
 
    mAlbumData[albumId] = items;
    
    // signal that album has been updated
    emit q_ptr->albumChanged();

    // signal that model is ready
    emit q_ptr->modelReady();
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

    int startIndex = mMediaData.count();

    TRAP_IGNORE(appendDataToContainerL(newVideoList));
	
    int endIndex = mMediaData.count() - 1;
    if(startIndex <= endIndex)
    {
        q_ptr->beginInsertRows(QModelIndex(), startIndex, endIndex);
        q_ptr->endInsertRows();
    }

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
    emit q_ptr->modelChanged();
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
    if (isValid(*newVideo, mediaId))
    {
        CMPXMedia *videoToAppend = 0;
        TRAP_IGNORE(videoToAppend = CMPXMedia::NewL(*newVideo));  
    
        q_ptr->beginInsertRows(QModelIndex(), mMediaData.count(), mMediaData.count());
    
        mMediaData.append(videoToAppend);    
  
        q_ptr->endInsertRows();
    
        emit q_ptr->modelChanged();
    }
}

// -----------------------------------------------------------------------------
// itemDeletedSlot
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::itemDeletedSlot(TMPXItemId itemId)
{
	if(itemId == TMPXItemId::InvalidId())
    {
        return;
    }    

	if(itemId.iId2 == KVcxMvcMediaTypeVideo)
    {
    	videoDeleted(itemId);
    }
    else if (itemId.iId2 == KVcxMvcMediaTypeAlbum)
    {
    	albumRemoved(itemId);
    }
}

// -----------------------------------------------------------------------------
// albumRemoved 
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::albumRemoved(TMPXItemId albumId)
{
    if (albumId == TMPXItemId::InvalidId() || albumId.iId2 != KVcxMvcMediaTypeAlbum)
    {
        return;
    }
    
    bool changed(false);

    if(mVideoThumbnailData)
    {
        mVideoThumbnailData->removeThumbnail( albumId );
    }

    int index = mMediaData.indexOfId(albumId);
    if(index >= 0)
    {
        q_ptr->beginRemoveRows(QModelIndex(), index, index);
        mMediaData.remove(albumId);            
        q_ptr->endRemoveRows();
        changed = true;
    }

    //remove album data
    QHash<TMPXItemId, QSet<TMPXItemId> >::iterator iter;
    iter =  mAlbumData.find(albumId);
    if(iter != mAlbumData.end())
    {
        iter->clear();
        mAlbumData.remove(albumId);
        changed = true;
    }

    if(changed)
    {
    	emit q_ptr->modelChanged();
    }
}

// -----------------------------------------------------------------------------
// videoDeleted
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::videoDeleted(TMPXItemId videoId)
{
    if(videoId == TMPXItemId::InvalidId())
    {
        return;
    }    
    // check if provided id is in the deleted -container and remove 
    // it from there if it is.
    QList<TMPXItemId> ids;
    ids.append(videoId);
    if(!mMediaData.clearRemoved(&ids))
    {
        q_ptr->mDeleteWorker->removeFromRequest(videoId);
        if(mVideoThumbnailData)
        {
            mVideoThumbnailData->removeThumbnail( videoId );
        }
        // media removed was not in the deleted -container, so we have external 
        // removal: in this case we need to indicate UI         
        int index = mMediaData.indexOfId(videoId);
        if(index >= 0)
        {
            q_ptr->beginRemoveRows(QModelIndex(), index, index);
            mMediaData.remove(videoId);            
            q_ptr->endRemoveRows();
            emit q_ptr->modelChanged();
        }
    }     
}

// -----------------------------------------------------------------------------
// videoDeleteCompleted
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::videoDeleteCompletedSlot(int overallCount, QList<TMPXItemId> *failedMediaIds)
{    
    Q_UNUSED(overallCount);
    if(!failedMediaIds)
    {
        // invalid signal received, do nothing
        return;
    }
    int status(VideoCollectionCommon::statusDeleteSucceed);
    QVariant data;
    if(failedMediaIds->count() > 0)
    {     
        QString nameOfFirstFailed("");
        CMPXMedia *media = mMediaData.getRemovedMedia( TMPXItemId(failedMediaIds->at(0)));
        VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralTitle, nameOfFirstFailed );
        if(failedMediaIds->count() == 1)
        {
            status = VideoCollectionCommon::statusSingleDeleteFail;
            data = nameOfFirstFailed;
        }
        else
        {
            status = VideoCollectionCommon::statusMultipleDeleteFail;
            data = failedMediaIds->count();
        }  
        restoreRemoved(failedMediaIds);
    }   
    q_ptr->reportAsyncStatus(status, data);
    emit q_ptr->modelChanged();
}

// -----------------------------------------------------------------------------
// albumRemoveFailureSlot
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::albumRemoveFailureSlot(QList<TMPXItemId> *failedMediaIds)
{
    if(!failedMediaIds)
    {
        return;
    }
    int status(VideoCollectionCommon::statusRemoveSucceed);
    QVariant data;
    if(failedMediaIds->count() > 0)
    {     
        QString nameOfFirstFailed("");
        CMPXMedia *media = mMediaData.getRemovedMedia( TMPXItemId(failedMediaIds->at(0)));
        VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralTitle, nameOfFirstFailed );
        if(failedMediaIds->count() == 1)
        {
            status = VideoCollectionCommon::statusSingleRemoveFail;
            data = nameOfFirstFailed;
        }
        else
        {
            status = VideoCollectionCommon::statusMultiRemoveFail;
            data = failedMediaIds->count();
        }  
    }   
    q_ptr->reportAsyncStatus(status, data);
}

// -----------------------------------------------------------------------------
// VideoDetailsCompleted 
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::videoDetailsCompletedSlot(TMPXItemId videoId)
{
    if(videoId != TMPXItemId::InvalidId())
    {
        emit videoDetailsReady(videoId);
    }
}

// -----------------------------------------------------------------------------
// albumListAvailableSlot 
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::albumListAvailableSlot(TMPXItemId albumId,
    CMPXMediaArray *albumItems)
{
    // currently only one album is kept in memory
    TRAP_IGNORE(albumDataChangedL(albumId, albumItems));
}

// End of file
