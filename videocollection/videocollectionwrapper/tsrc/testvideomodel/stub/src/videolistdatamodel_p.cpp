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
* Description:  stubclass of videolistdata for testing VideoListDataModel class methods*
*/
// INCLUDE FILES

#include <mpxmediageneraldefs.h>
#include <thumbnailmanager_qt.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <qvariant.h>

#include "videolistdatamodel.h"
#include "videocollectionutils.h"
#include "videolistdatamodel_p.h"
#include "videothumbnaildata.h"
#include "vcxmyvideosdefs.h"
#include "videocollectioncommon.h"

inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

// helper funtion to parse QString -data from media
template<class T>
bool mediaValue(const CMPXMedia *media, const TMPXAttributeData& attribute, QString& result)
{
    bool status = false;
    if( media && media->IsSupported(attribute))
    {
        HBufC* valueText = NULL;        
        TRAPD(error, valueText = media->ValueText( attribute ).AllocL() );
        if (error == KErrNone && valueText->Length() > 0)
        {
            // temp QString needed to make sure string is initialized correctly (Error  #424)
            QString text((QChar*)valueText->Des().Ptr(),valueText->Length());
            result = text;
            status = true;
        }
        delete valueText;
    }
    return status;
}

bool VideoListDataModelPrivate::mReturnInvalidMarked = false;


/**
 * init failure flag
 */
bool VideoListDataModelPrivate::mFailInit = false;


bool VideoListDataModelPrivate::mOverrideSize = false;
quint32 VideoListDataModelPrivate::mSize = 0;

bool VideoListDataModelPrivate::mOverrideDuration = false;
quint32 VideoListDataModelPrivate::mDuration = 0;

int VideoListDataModelPrivate::mGetVideoCountFail = -1;

bool VideoListDataModelPrivate::mBelongsToAlbum = false;

// -----------------------------------------------------------------------------
// CVideoListData
// -----------------------------------------------------------------------------
//
VideoListDataModelPrivate::VideoListDataModelPrivate(VideoListDataModel *model) :
q_ptr(model),
mMediaData(*(new DummyData)),
mMediaArray(0)
{
    mMediaData.mObj = this;
    mIcon = QIcon(":/icons/default_thumbnail.svg");
}

// -----------------------------------------------------------------------------
// ~VideoListDataModelPrivate
// -----------------------------------------------------------------------------
//
VideoListDataModelPrivate::~VideoListDataModelPrivate()
{
    delete &mMediaData;
}

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
//
int VideoListDataModelPrivate::initialize()
{
    if(mFailInit)
    {
        return -1;
    }
  
    return 0;
}

// -----------------------------------------------------------------------------
// callModelDisconnect
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::callModelDisconnect()
{
    q_ptr->disconnectSignals();
}

// -----------------------------------------------------------------------------
// callModelAsyncReport
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::callModelAsyncReport(int status, QVariant data)
{
    q_ptr->reportAsyncStatus(status, data);
}

// -----------------------------------------------------------------------------
// getVideoCount
// -----------------------------------------------------------------------------
//
int VideoListDataModelPrivate::getVideoCount()
{
    int count = 0;
    if(mMediaArray)
    {
        count = mMediaArray->Count();        
    }
    mGetVideoCountFail--;
    if(mGetVideoCountFail == 0)
    {
        return 0;
    }
    return count;
}

// -----------------------------------------------------------------------------
// getVideoNameFromIndex
// -----------------------------------------------------------------------------
//
const QString VideoListDataModelPrivate::getVideoNameFromIndex( int index )  const
{
    QString txt = "";
    CMPXMedia* media = getMediaFromIndex(index);
    
    mediaValue<QString>(media, KMPXMediaGeneralTitle, txt );
    return txt;   
}
  
// -----------------------------------------------------------------------------
// getVideoThumbnailFromIndex
// -----------------------------------------------------------------------------
//
const QIcon* VideoListDataModelPrivate::getVideoThumbnailFromIndex( int index )  const
{
    Q_UNUSED(index);
    
    return &mIcon;
}

// -----------------------------------------------------------------------------
// getVideoSizeFromIndex
// -----------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getVideoSizeFromIndex( int index ) const
{
     if(mOverrideSize)
     {
         return mSize;
     }
    quint32 size(0);
    CMPXMedia *media = getMediaFromIndex(index);
    if(media && media->IsSupported( KMPXMediaGeneralSize ))
    {
        size = *( media->Value<quint32>( KMPXMediaGeneralSize ) ); // In bytes
    }
    return size;    
}

// -----------------------------------------------------------------------------
// getVideodurationFromIndex
// -----------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getVideodurationFromIndex( int index ) const
{
    if(mOverrideDuration)
    {
        return mDuration;
    }
    quint32 returnDuration(0);
    float duration(0);
    CMPXMedia *media = getMediaFromIndex(index);
    if(media && media->IsSupported( KMPXMediaGeneralDuration ))
    {
        duration = *( media->Value<float>( KMPXMediaGeneralDuration ) );
    }
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
    CMPXMedia *media = getMediaFromIndex(index);
    if(media && media->IsSupported(KMPXMediaGeneralDate)) {
        
        TInt64 temp = *(media->Value<TInt64>(KMPXMediaGeneralDate));
        TDateTime tempDate = TTime(temp).DateTime();
        date = QDateTime(QDate(tempDate.Year(), tempDate.Month()+1, tempDate.Day()+1), 
                         QTime(tempDate.Hour(), tempDate.Minute(), tempDate.Second(), tempDate.MicroSecond()));
    }
    return date;    
}

// -----------------------------------------------------------------------------
// getMetaDataFromIndex
// -----------------------------------------------------------------------------
//
QMap<QString, QVariant> VideoListDataModelPrivate::getMetaDataFromIndex(int /*index*/) const
{
    QMap<QString, QVariant> map;
    
    // no need to actually populate the map with real like data, as the datamodel
    // is not responsible in anyway what data the actual map contains. So here we
    // only need to make sure that the map received in test has been gotten by
    // calling this function.
    map["metadata"] = QString("metadata");
    
    return map;
}

// -----------------------------------------------------------------------------
// getMediaIdFromIndex
// -----------------------------------------------------------------------------
//
TMPXItemId VideoListDataModelPrivate::getMediaIdFromIndex( int index ) const
{
    return getMediaId( getMediaFromIndex(index) );
}

// -----------------------------------------------------------------------------
// getFilePathForId
// -----------------------------------------------------------------------------
//
const QString VideoListDataModelPrivate::getFilePathForId( TMPXItemId mediaId ) const
{
    return getFilePathFromIndex( indexOfMediaId(mediaId) );
}

// -----------------------------------------------------------------------------
// getFilePathForId
// -----------------------------------------------------------------------------
//
bool VideoListDataModelPrivate::belongsToAlbum(TMPXItemId itemId, TMPXItemId albumId)
{
    Q_UNUSED(itemId);
    Q_UNUSED(albumId);
    return mBelongsToAlbum;
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
// markVideoRemoved
// -----------------------------------------------------------------------------
//
TMPXItemId VideoListDataModelPrivate::markVideoRemoved(const QModelIndex &itemIndex)
{
    TMPXItemId id = TMPXItemId::InvalidId();
    if(!mReturnInvalidMarked)
    {
        if(!itemIndex.isValid() || itemIndex.row() >=  mMediaArray->Count())
        {
            return id;
        }
        id = getMediaIdFromIndex(itemIndex.row());
        mItemsUnderDeletion.append(id);
    }
    return id;
}

// -----------------------------------------------------------------------------
// restoreRemoved
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::restoreRemoved(QList<TMPXItemId> *idList)
{
    TMPXItemId id;
    foreach(id, *idList)
    {
        mItemsUnderDeletion.removeAll(id);
    }
}

// -----------------------------------------------------------------------------
// getFilePathFromIndex
// -----------------------------------------------------------------------------
//
const QString VideoListDataModelPrivate::getFilePathFromIndex(int index) const
{
    QString filePath;
    VideoCollectionUtils::instance().mediaValue<QString>(getMediaFromIndex(index), 
            KMPXMediaGeneralUri, filePath );
    return filePath;
}

// -----------------------------------------------------------------------------
// getMediaFromIndex
// -----------------------------------------------------------------------------
//
CMPXMedia* VideoListDataModelPrivate::getMediaFromIndex( int index ) const
{
    CMPXMedia *media = NULL;
    if( mMediaArray && index >= 0 && index < mMediaArray->Count() )
    {
        media = (*mMediaArray)[index];
    }        
    return media;
}

// -----------------------------------------------------------------------------
// getMediaId
// -----------------------------------------------------------------------------
//
TMPXItemId VideoListDataModelPrivate::getMediaId(const CMPXMedia *media ) const
{
    TMPXItemId mediaId = TMPXItemId::InvalidId();
    if( media &&  media->IsSupported( KMPXMediaGeneralId ) )
    {
        TRAPD(err,  mediaId = media->ValueTObjectL<TMPXItemId>( KMPXMediaGeneralId ) );
    }
    return mediaId;
}

// -----------------------------------------------------------------------------
// indexOfMediaId
// -----------------------------------------------------------------------------
//
int VideoListDataModelPrivate::indexOfMediaId(TMPXItemId mediaId) const
{
    int index = -1;
    if( !mMediaArray )
    {
        // nothing to look for
        return index;
    }
    const CMPXMedia* media = NULL;
    int count = mMediaArray->Count();
    for(int i = 0; i < count; ++i)
    {
        media = getMediaFromIndex(i);
        if(media )
        {
            TMPXItemId itemId = getMediaId(media);
            if( mediaId == itemId)
            {
                index = i;
                break;
            }
            media = NULL;
        }
    }    
    return index;
}

// -----------------------------------------------------------------------------
// newVideoList
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::newVideoListSlot( CMPXMediaArray* aVideoList )
{
    mMediaArray = aVideoList;
}
       
// -----------------------------------------------------------------------------
// appendVideoListSlot
// -----------------------------------------------------------------------------
// 
void VideoListDataModelPrivate::appendVideoListSlot(CMPXMediaArray* aVideo)
{
    Q_UNUSED(aVideo);  
}

// -----------------------------------------------------------------------------
// newVideoAvailable
// -----------------------------------------------------------------------------
// 
void VideoListDataModelPrivate::newVideoAvailableSlot(CMPXMedia* aVideo)
{
    Q_UNUSED(aVideo);   
}

// -----------------------------------------------------------------------------
// videoDeleted
// -----------------------------------------------------------------------------
// 
void VideoListDataModelPrivate::videoDeletedSlot(TMPXItemId videoId)
{
    Q_UNUSED(videoId);    
}

// -----------------------------------------------------------------------------
// videoDeleteCompleted
// -----------------------------------------------------------------------------
// 
void VideoListDataModelPrivate::videoDeleteCompletedSlot(int overallCount, QList<TMPXItemId> *failedMediaIds)
{
    Q_UNUSED(overallCount);
    Q_UNUSED(failedMediaIds);
    mItemsUnderDeletion.clear();  
}

// -----------------------------------------------------------------------------
// videoDetailsCompleted
// -----------------------------------------------------------------------------
// 
void VideoListDataModelPrivate::videoDetailsCompletedSlot(TMPXItemId videoId)
{
    Q_UNUSED(videoId);      
}

// -----------------------------------------------------------------------------
// albumListAvailableSlot
// -----------------------------------------------------------------------------
// 
void VideoListDataModelPrivate::albumListAvailableSlot(TMPXItemId albumId, 
                                                        CMPXMediaArray *albumItems)
{
    Q_UNUSED(albumId);
    Q_UNUSED(albumItems);
}

// -----------------------------------------------------------------------------
// albumRemoveFailureSlot
// -----------------------------------------------------------------------------
// 
void VideoListDataModelPrivate::albumRemoveFailureSlot(QList<TMPXItemId> *items)
{
    Q_UNUSED(items);    
}

// -----------------------------------------------------------------------------
// itemDeletedSlot
// -----------------------------------------------------------------------------
// 
void VideoListDataModelPrivate::itemDeletedSlot(TMPXItemId id)
{
    Q_UNUSED(id);    
}

// End of file
