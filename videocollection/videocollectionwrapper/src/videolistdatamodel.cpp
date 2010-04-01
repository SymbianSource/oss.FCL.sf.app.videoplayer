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
* Description: VideoListDataModel class implementation
* 
*/
// INCLUDE FILES

#include <hbglobal.h>
#include <vcxmyvideosdefs.h>
#include "videolistdatamodel.h"
#include "videolistdatamodel_p.h"
#include "videocollectionclient.h"
#include "videocollectioncommon.h"
#include "videocollectionutils.h"
#include "videodeleteworker.h"
#include "videocollectionwrapper.h"


// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoListDataModel()
// -----------------------------------------------------------------------------
//
VideoListDataModel::VideoListDataModel(QObject *parent) :
QAbstractItemModel(parent),
d_ptr( new VideoListDataModelPrivate(this)), 
mCollectionClient(0),
mDeleteWorker(0),
mInitialized(false)
{
}

// -----------------------------------------------------------------------------
// ~VideoListDataModel()
// -----------------------------------------------------------------------------
//
VideoListDataModel::~VideoListDataModel()
{
    delete mDeleteWorker;
    delete d_ptr;
    delete mCollectionClient;
}

// -----------------------------------------------------------------------------
// initialize()
// -----------------------------------------------------------------------------
//
int VideoListDataModel::initialize()
{
    if(mInitialized)
    {
        return 0;
    }
    if(!mCollectionClient)
    {
        mCollectionClient = new VideoCollectionClient();
        if(!mCollectionClient || mCollectionClient->initialize(d_ptr) < 0)
        {
            delete mCollectionClient;
            mCollectionClient = 0;
            return -1;
        }
    }

    if(!mDeleteWorker)
    {
        mDeleteWorker = new VideoDeleteWorker(*mCollectionClient);
        if(!mDeleteWorker)
        {
            delete mCollectionClient;
            mCollectionClient = 0;
            return -1;
        }
    }
    
    if( d_ptr->initialize() == -1)            
    {
        return -1;
    }

    if(connectSignals() == -1)
    {
        disconnectSignals();
        return -1;
    }

    mInitialized = true;
    return 0;
}

// -----------------------------------------------------------------------------
// getCollectionClient()
// -----------------------------------------------------------------------------
//
VideoCollectionClient* VideoListDataModel::getCollectionClient()
{
    return mCollectionClient;
}
 
// -----------------------------------------------------------------------------
// connectSignals()
// -----------------------------------------------------------------------------
//
int VideoListDataModel::connectSignals()
{
    if(!connect(d_ptr, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                           this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&))))
    {
        return -1;
    }
    if(!connect(d_ptr, SIGNAL(videoDetailsReady(TMPXItemId)),
                       this, SIGNAL(fullVideoDetailsReady(TMPXItemId))))
    {
        return -1;
    }
    if(!connect(this, SIGNAL(modelChanged()), mDeleteWorker, SLOT(continueSlot())))
    {
        return -1;
    }
    if(!connect(this, SIGNAL(modelReady()), mDeleteWorker, SLOT(continueSlot())))
    {
        return -1;
    }
    if(!connect(mDeleteWorker, SIGNAL(deleteStartupFailed(QList<TMPXItemId>)), 
            this, SLOT(deleteStartingFailsSlot(QList<TMPXItemId>))))
    {
        return -1;
    }
   
    return 0;
}

// -----------------------------------------------------------------------------
// disconnectSignals()
// -----------------------------------------------------------------------------
//
void VideoListDataModel::disconnectSignals()
{
    disconnect(d_ptr, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
                               this, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)));
    disconnect(d_ptr, SIGNAL(videoDetailsReady(TMPXItemId)),
                           this, SIGNAL(fullVideoDetailsReady(TMPXItemId)));
    disconnect(this, SIGNAL(modelChanged()), mDeleteWorker, SLOT(continueSlot()));
    disconnect(this, SIGNAL(modelReady()), mDeleteWorker, SLOT(continueSlot()));
    disconnect(mDeleteWorker, SIGNAL(deleteStartupFailed(QList<TMPXItemId>)), 
                this, SLOT(deleteStartingFailsSlot(QList<TMPXItemId>)));
}

// -----------------------------------------------------------------------------
// mediaIdAtIndex()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoListDataModel::mediaIdAtIndex(int index) const
{
    TMPXItemId mpxId =TMPXItemId::InvalidId();

    if(index >= 0 && index < d_ptr->getVideoCount())
    {
        mpxId = d_ptr->getMediaIdFromIndex(index);
    }

    return mpxId;
}

// -----------------------------------------------------------------------------
// indexOfId()
// -----------------------------------------------------------------------------
//
QModelIndex VideoListDataModel::indexOfId(TMPXItemId id)
{
    int rowIndex = d_ptr->mMediaData.indexOfId(id);
    return index(rowIndex, 0, QModelIndex());     
}

// -----------------------------------------------------------------------------
// mediaFilePathForId()
// -----------------------------------------------------------------------------
//
QString VideoListDataModel::mediaFilePathForId(TMPXItemId mediaId) const
{
    QString filePath;
    filePath = d_ptr->getFilePathForId(mediaId);

    return filePath;
}

// -----------------------------------------------------------------------------
// rowCount()
// -----------------------------------------------------------------------------
//
int VideoListDataModel::rowCount(const QModelIndex &parent ) const
{
    // according to Qt documentation if parent is valid this should return 0 if
    // implementing a table based implementation like this.
    if (parent.isValid())
    {
        return 0;
    }
    
    return d_ptr->getVideoCount();
}

// -----------------------------------------------------------------------------
// itemData()
// -----------------------------------------------------------------------------
//
QMap<int, QVariant> VideoListDataModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> itemData;
    
    if (index.isValid()) 
    {
        // returns only basic data of the item
        itemData.insert(Qt::DisplayRole, data(index, Qt::DisplayRole));
        itemData.insert(Qt::DecorationRole, data(index, Qt::DecorationRole)); 
        itemData.insert(Qt::BackgroundRole, data(index, Qt::BackgroundRole)); 
    }
    return itemData;

}

// -----------------------------------------------------------------------------
// prepareDetailRow()
// -----------------------------------------------------------------------------
//
QString VideoListDataModel::prepareDetailRow( int index ) const
{
    // TODO: download -status?

    QString retString;
    
    TMPXItemId itemId = mediaIdAtIndex(index);
    
    if(itemId.iId2 != KVcxMvcMediaTypeVideo) //category || album
    {
        //TODO: get real items and total length
        int items          = 99;
        QString minutesStr = "10";
        QString secondsStr = "01";
        retString = hbTrId("txt_videos_dblist_val_ln_videos_l1l2", items).arg(minutesStr).arg(secondsStr);
    }
    else //video
    {
        const QString sizeStr = prepareSizeString(index);
        const QStringList list = prepareLengthStrings( index );
        retString = hbTrId("txt_videos_dblist_captured_val_1_l1l2").arg(sizeStr).arg(list.at(0)).arg(list.at(1));
    }
    return retString; 
}

// -----------------------------------------------------------------------------
// prepareSizeString()
// -----------------------------------------------------------------------------
//
QString VideoListDataModel::prepareSizeString( int index ) const
{
    QString sizeStr("");

    quint32 size = d_ptr->getVideoSizeFromIndex(index);
    sizeStr = VideoCollectionUtils::instance().prepareSizeString(size);

    return sizeStr;
}

// -----------------------------------------------------------------------------
// VideoListDataModel::prepareLengthStrings()
// -----------------------------------------------------------------------------
//
QStringList VideoListDataModel::prepareLengthStrings( int index ) const
{
    quint32 total = d_ptr->getVideodurationFromIndex(index);
    return VideoCollectionUtils::instance().prepareLengthStrings(total);
}

// -----------------------------------------------------------------------------
// data()
// -----------------------------------------------------------------------------
//
QVariant VideoListDataModel::data(const QModelIndex & index, int role) const
{
    QVariant returnValue = QVariant();

   
    if (index.isValid()) 
    {
        int rowIndex = index.row();

        if (role == Qt::DisplayRole)
        {
            QStringList itemTxt;
            itemTxt.append(d_ptr->getVideoNameFromIndex(rowIndex));
                
            // Item's 2nd row: details
            itemTxt.append(prepareDetailRow(rowIndex));
            returnValue = itemTxt;
        }
        else if (role == Qt::DecorationRole)
        {
            const QIcon *thumbnail = d_ptr->getVideoThumbnailFromIndex(rowIndex); 
            QVariant tnVariant(QVariant::Icon, thumbnail);
            returnValue = tnVariant;
        }
        else if (role == VideoCollectionCommon::KeyDateTime)
        {
            QDateTime date = d_ptr->getVideoDateFromIndex(rowIndex);
            returnValue = date;
        }
        else if(role == VideoCollectionCommon::KeySizeValue)
        {
            quint32 size = d_ptr->getVideoSizeFromIndex(rowIndex);
            returnValue = size;
            
        }
        else if(role == VideoCollectionCommon::KeyMetaData)
        {
            returnValue = d_ptr->getMetaDataFromIndex(rowIndex);
        }
        else if(role == VideoCollectionCommon::KeyFilePath)
        {
            QString path = d_ptr->getFilePathFromIndex(rowIndex);
            if(!(path.isNull()) && !(path.isEmpty())) { 
                returnValue = path;
            }
        }
    }
    
    return returnValue;
}

// -----------------------------------------------------------------------------
// columnCount()
// -----------------------------------------------------------------------------
//
int VideoListDataModel::columnCount(const QModelIndex & parent) const
{
    // according to Qt documentation if parent is valid this should return 0 if
    // implementing a table based implementation like this.
    if (parent.isValid())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// -----------------------------------------------------------------------------
// index()
// -----------------------------------------------------------------------------
//
QModelIndex VideoListDataModel::index(int row, int column, const QModelIndex & /*parent*/) const
{

    if(row >= 0 && row < d_ptr->getVideoCount())
    {
        return createIndex(row, column);
    }

    return QModelIndex();
}

// -----------------------------------------------------------------------------
// parent()
// -----------------------------------------------------------------------------
//
QModelIndex VideoListDataModel::parent(const QModelIndex & /*index*/) const
{
    return QModelIndex();
}

// -----------------------------------------------------------------------------
// removeRows()
// -----------------------------------------------------------------------------
//
bool VideoListDataModel::removeRows(const QModelIndexList &indexList)
{
    if(!mCollectionClient || !mDeleteWorker)
    {
        return false;
    }
    // generate list of deletable item ids
    TMPXItemId id = TMPXItemId::InvalidId();
    QList<TMPXItemId> removedIds;
    QModelIndexList sortable(indexList);
    qSort(sortable);
    
    // since items are removed from actual container when being marked
    // causing total item count to be decreased,
    // we need to mark items starting from biggest index, otherwise we might 
    // end up to mark invalid items or we are referring into null item
    QModelIndexList::const_iterator iter = sortable.constEnd();
    while(iter != sortable.constBegin())
    {
        iter--;
        beginRemoveRows(QModelIndex(), (*iter).row(), (*iter).row());
        // marking might fail if either because
        // item does no exist or index is incorrect.
        // Both reasons mean that item is incorrect and should be removed
        // from the view. No need to restain them
        id = d_ptr->markVideoRemoved((*iter));
        endRemoveRows();
        if(id != TMPXItemId::InvalidId())
        {
            removedIds.append(id);
        }
    }
    emit modelChanged();
    if(removedIds.count() > 0 )
    {                         
        mDeleteWorker->requestDelete(removedIds);
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
// belongsToAlbum()
// -----------------------------------------------------------------------------
//
bool VideoListDataModel::belongsToAlbum(const TMPXItemId &itemId,
      TMPXItemId albumId ) const
{
    return d_ptr->belongsToAlbum(itemId, albumId);
}

// -----------------------------------------------------------------------------
// setAlbumInUse()
// -----------------------------------------------------------------------------
//
void VideoListDataModel::setAlbumInUse(TMPXItemId albumId)
{
    d_ptr->setAlbumInUse(albumId);
}

// -----------------------------------------------------------------------------
// albumInUse()
// -----------------------------------------------------------------------------
//

TMPXItemId VideoListDataModel::albumInUse()
{
    return d_ptr->mCurrentAlbum;
}

// -----------------------------------------------------------------------------
// deleteStartingFailsSlot()
// -----------------------------------------------------------------------------
//
void VideoListDataModel::deleteStartingFailsSlot(QList<TMPXItemId> ids)
{
    if(ids.count())
    {        
        d_ptr->restoreRemoved(&ids);
        QVariant data = ids.count();
        reportAsyncStatus(VideoCollectionCommon::statusMultipleDeleteFail, data);
    }    
    // signal for worker to proceed
    emit modelChanged();
}

// -----------------------------------------------------------------------------
// reportAsyncStatus()
// -----------------------------------------------------------------------------
//
void VideoListDataModel::reportAsyncStatus(int statusCode, QVariant &additional)
{
    bool report = true;
    if(statusCode == VideoCollectionCommon::statusSingleDeleteFail ||
       statusCode ==  VideoCollectionCommon::statusMultipleDeleteFail ||
       statusCode ==  VideoCollectionCommon::statusDeleteSucceed)
    {
        report = false;
        if(!mDeleteWorker)
        {
            return;
        }
        mDeleteWorker->updateStatus(statusCode, additional);  
        statusCode = mDeleteWorker->getLastStatus(additional);
        if(!mDeleteWorker->isDeleting())
        {
            report = true;
            // delete procedure ends, reset statuses
            mDeleteWorker->clearStatus();
        }
    }
    if(report)
    {
        VideoCollectionWrapper::instance().sendAsyncStatus(statusCode, additional); 
    }
}

// End of file
