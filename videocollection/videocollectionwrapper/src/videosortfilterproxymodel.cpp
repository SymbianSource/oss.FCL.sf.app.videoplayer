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
* Description:   Videosortfilterproxymodel implementation
*
*/

#include <qstringlist.h>
#include <qset.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <vcxmyvideosdefs.h>

#include "videothumbnaildata.h"
#include "videocollectioncommon.h"
#include "videosortfilterproxymodel.h"
#include "videolistdatamodel.h"
#include "videocollectionclient.h"
#include "videocollectionwrapper.h"

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::VideoSortFilterProxyModel(int type, QObject *parent) :
 QSortFilterProxyModel(parent),
 mModel(0),
 mCollectionClient(0),
 mType(type),
 mLevel(-1),
 mGenericFilterId(TMPXItemId::InvalidId()),
mGenericFilterValue(false),
 mIdleSortTimer(0),
 mWantedSortRole(VideoCollectionCommon::KeyDateTime)
{
    // NOP
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::~VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::~VideoSortFilterProxyModel()
{
    // NOP
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::initialize
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::initialize(VideoListDataModel *sourceModel)
{
    if(!sourceModel)
    {
        return -1;
    }
    
    if(!mModel) 
    {
        mModel = sourceModel;
        if(!connectSignals()) 
        {
            disconnectSignals();
            mModel = 0;
            return -1;
        }
        mCollectionClient = mModel->getCollectionClient();
        setSourceModel(sourceModel); 
    }           
    return 0;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::connectSignals
// -----------------------------------------------------------------------------
//
bool VideoSortFilterProxyModel::connectSignals()
{
    if(!connect(mModel, SIGNAL(fullVideoDetailsReady(TMPXItemId)),
                    this, SIGNAL(fullDetailsReady(TMPXItemId)))) 
    {
        return false;
    }
    
    if(!connect(mModel, SIGNAL(modelReady()),
            this, SIGNAL(modelReady()))) 
    {
        return false;
    }
    if(!connect(mModel, SIGNAL(modelChanged()),
                    this, SIGNAL(modelChanged()))) 
    {
        return false;
    }
    if(!connect(mModel, SIGNAL(albumChanged()),
                  this, SLOT(albumChangedSlot()))) 
    {
        return false;
    }
    return true;
}
   
// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::disconnectSignals
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::disconnectSignals()
{
    disconnect(mModel, SIGNAL(fullVideoDetailsReady(TMPXItemId)),
                                this, SIGNAL(fullDetailsReady(TMPXItemId)));
    disconnect(mModel, SIGNAL(modelReady()), this, SIGNAL(modelReady()));
    disconnect(mModel, SIGNAL(modelChanged()), this, SIGNAL(modelChanged()));
    disconnect(mModel, SIGNAL(albumChanged()), this, SLOT(albumChangedSlot())); 
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::open
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::open(int level)
{
    if(!mCollectionClient)
    {
        return -1;
    }
    
    if(mLevel != level) 
    {
        mLevel = level;   
        invalidateFilter();
    }
    // need to call open every time to make sure all items are 
    // inserted to UI ( recent open might have been cancelled)
    return mCollectionClient->startOpenCollection(level);
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::doSorting
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::doSorting(int sortingRole, Qt::SortOrder order, 
        bool async)
{
    if((sortingRole != Qt::DisplayRole) &&
       (sortingRole != VideoCollectionCommon::KeySizeValue) &&
       (sortingRole != VideoCollectionCommon::KeyDateTime))
    {
        // default sorting order is by date
        mWantedSortRole = VideoCollectionCommon::KeyDateTime;
    }
    else
    {
        mWantedSortRole = sortingRole;
    }
    
    mWantedSortOrder = order;
    
    if(async)
    {
        if(mIdleSortTimer)
        {
            if(mIdleSortTimer->isActive())
            {
                mIdleSortTimer->stop();
            }
        }
        else
        {
            mIdleSortTimer = new QTimer(this);
            connect(mIdleSortTimer, SIGNAL(timeout()), this, SLOT(processSorting()));
        }
        mIdleSortTimer->start(0);
    } else {
        processSorting();
    }
}
// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getSorting
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::getSorting(int &sortingRole, Qt::SortOrder &order)
{
	sortingRole = mWantedSortRole;
	order       = mWantedSortOrder;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::deleteItems
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::deleteItems(const QModelIndexList &indexList)
{
    if(mModel)
    {
        QModelIndexList mappedList;
        for(int i = 0; i < indexList.count(); ++i)
        {
            mappedList.append(mapToSource(indexList.at(i)));
        }
        
        if(mModel->removeRows(mappedList))
        {
            // Start fetching thumbnails at start of the model.
            VideoThumbnailData::instance().startBackgroundFetching(0, 0);
            
            return 0;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::openItem
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::openItem(TMPXItemId mediaId)
{
    if(mediaId != TMPXItemId::InvalidId() && mCollectionClient)
    {
        mModel->setAlbumInUse(TMPXItemId::InvalidId());
        
        if(mCollectionClient->openItem(mediaId) == 0)
        {
            if(mediaId.iId2 != KVcxMvcMediaTypeVideo)
            { 
                mLevel = VideoCollectionCommon::ELevelAlbum;
                mModel->setAlbumInUse(mediaId);
                invalidateFilter();
            } 
            return 0;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::back
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::back()
{

    if(mCollectionClient && mCollectionClient->back() == 0)
    {

        if(mLevel == VideoCollectionCommon::ELevelAlbum)
        {
            mLevel = VideoCollectionCommon::ELevelCategory;
        }
        else
        {
            mLevel = VideoCollectionCommon::ELevelVideos;
        }
        return 0;
    }

    return -1;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::fetchItemDetails
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::fetchItemDetails(const QModelIndex &index)
{
    // no need to map index beforehand, because 
	// getMediaIdAtIndex maps it
    TMPXItemId mpxId1 = getMediaIdAtIndex(index);
    if(mpxId1 != TMPXItemId::InvalidId() && mCollectionClient)
    {
        if(mCollectionClient->getVideoDetails(mpxId1) == 0)
        {

            emit shortDetailsReady(mpxId1);
            return 0;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::processSorting
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::processSorting()
{
    if(mIdleSortTimer)
    {
        mIdleSortTimer->stop();
    }

    if(sortRole() != mWantedSortRole)
    {
        setSortRole(mWantedSortRole);
    }
    
    if(sortColumn() == -1 || sortOrder() != mWantedSortOrder)
    {
        // if sorting column is not yet setted, it indicates
        // that we've setted the sorting order for the first time
        // and sorting has not yet been done. After we have called sort once,
        // the sort columnt gets updated and following sorts gets executed
        // by setSortRole -call as long as role changes.
    
        // Another case is when we need to change the sort order, as there is
        // no other way of setting the sort order than calling sort().
        sort(0, mWantedSortOrder);
    }
    
    // Start fetching thumbnails at start of the model.
    VideoThumbnailData::instance().startBackgroundFetching(0, 0);
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::lessThan
// -----------------------------------------------------------------------------
//
bool VideoSortFilterProxyModel::lessThan(const QModelIndex &left,
    const QModelIndex &right) const
{
    if (!mModel)
    {
        return false;
    }
    TMPXItemId leftId = mModel->mediaIdAtIndex(left.row());
    TMPXItemId rightId = mModel->mediaIdAtIndex(right.row());
    
    // Default categories are always first in the following order:
    // Recently played (missing currently
    // Captured
    // Downloaded
    // Podcasts (missing currently)
    bool lessThan(false);
    bool proceedDataSorting(false);
    if(leftId.iId2  == KVcxMvcMediaTypeCategory &&
       rightId.iId2 == KVcxMvcMediaTypeCategory)
    {
        if(leftId.iId1 == KVcxMvcCategoryIdCaptured)
        {
            lessThan = true;
        }
   
        else if(leftId.iId1 == KVcxMvcCategoryIdDownloads)
        {
            if(rightId.iId1 != KVcxMvcCategoryIdCaptured)
            {
                lessThan = true;
            }
        }
    }
    else if(leftId.iId2 == KVcxMvcMediaTypeCategory ||
            rightId.iId2 == KVcxMvcMediaTypeCategory)
    {
        lessThan = (leftId.iId2 == KVcxMvcMediaTypeCategory);
    }
    else
    {
        proceedDataSorting = true;
    }

    if(!proceedDataSorting)
    {
        return mWantedSortOrder == Qt::AscendingOrder ? lessThan : !lessThan;
    }
    
    // Do comparisation based on the role:
    //  Qt::DisplayRole,
    //  VideoCollectionCommon::KeySizeValue,
    //  VideoCollectionCommon::KeyDateValue
    // 
    // If role does not match, do not sort
    int sRole = sortRole();
    QVariant leftData = sourceModel()->data(left, sRole);
    QVariant rightData = sourceModel()->data(right, sRole);

    if(!leftData.isValid() || !rightData.isValid())
    {
        return false;
    }
    if(sRole == Qt::DisplayRole)
    {
        QString leftString = leftData.toStringList().first().toUpper();
        QString rightString = rightData.toStringList().first().toUpper();
        return QString::localeAwareCompare(leftString, rightString) < 0;
    }
    else if(sRole == VideoCollectionCommon::KeySizeValue)
    {
        quint32 leftSize = leftData.toUInt();
        quint32 rightSize = rightData.toUInt();
        return leftSize < rightSize;
    }
    else if(sRole == VideoCollectionCommon::KeyDateTime)
    {
        QDateTime leftDateTime = leftData.toDateTime();
        QDateTime rightDateTime = rightData.toDateTime();
        // datetime sorting role has inverted sorting order compared to other roles 
        return rightDateTime < leftDateTime;
    }
    return false;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoSortFilterProxyModel::filterAcceptsRow (int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);

    if(!sourceModel())
        return false;

    if(source_row < 0 || source_row >= sourceModel()->rowCount())
    {
        return false;
    }
    TMPXItemId id = mModel->mediaIdAtIndex(source_row);
    if(id == TMPXItemId::InvalidId())
    {
        return false;
    }
    
    if (mType == VideoCollectionWrapper::EAllVideos)
    {
        if(id.iId2 == KVcxMvcMediaTypeVideo)
        {
            return true;
        }
    }
    else if(mType == VideoCollectionWrapper::ECollections)
    {
        
        if(mLevel == VideoCollectionCommon::ELevelCategory && id.iId2 != KVcxMvcMediaTypeVideo)
        {
            return true;
        }       
    }
    else if (mType == VideoCollectionWrapper::ECollectionContent)
    {
        // if item belongs to the open album, accept it
        if (mModel->belongsToAlbum(id))
        {
            return true;
        }
    }
    else if(mType == VideoCollectionWrapper::EGeneric)
    {

        if(mLevel == VideoCollectionCommon::ELevelVideos && id.iId2 == KVcxMvcMediaTypeVideo)
        {
            //  filter items that belong to that album setted as filter id
            // if there's no filter or setted filter is "all videos", we accept everything
            if(mGenericFilterId == TMPXItemId::InvalidId() || 
               (mGenericFilterId.iId1 == KVcxMvcCategoryIdAll && 
                mGenericFilterId.iId2 == KVcxMvcMediaTypeCategory) ||
                mGenericFilterValue == mModel->belongsToAlbum(id, mGenericFilterId))
            {
                return true;
            }
            
        }
        else if(mLevel == VideoCollectionCommon::ELevelCategory && id.iId2 == KVcxMvcMediaTypeAlbum)    
        {
            // we do not filter albums yet
            return true;
        }
    }
    
    return false;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getMediaIdAtIndex()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoSortFilterProxyModel::getMediaIdAtIndex(const QModelIndex &index) const
{
    QModelIndex sourceIndex = mapToSource(index);
    TMPXItemId mpxId = TMPXItemId::InvalidId();
    if(mModel && sourceIndex.isValid())
    {
        mpxId = mModel->mediaIdAtIndex(sourceIndex.row());
    }
    return mpxId;
}

// -----------------------------------------------------------------------------
//  VideoSortFilterProxyModel::indexOfId()
// -----------------------------------------------------------------------------
//
QModelIndex VideoSortFilterProxyModel::indexOfId(TMPXItemId id)
{    
    QModelIndex sourceIndex;
    if(!mModel || id == TMPXItemId::InvalidId())
    {
        return sourceIndex;
    }
    sourceIndex = mModel->indexOfId(id);
    return mapFromSource(sourceIndex);
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getMediaFilePathForId()
// -----------------------------------------------------------------------------
//
QString VideoSortFilterProxyModel::getMediaFilePathForId(TMPXItemId mediaId)
{
    QString filePath;
    if(mModel)
    {
        filePath = mModel->mediaFilePathForId(mediaId);
    }
    return filePath;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::addNewAlbum()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoSortFilterProxyModel::addNewAlbum(const QString &title)
{
    TMPXItemId id = TMPXItemId::InvalidId();
    
    if (mCollectionClient)
    {
        id = mCollectionClient->addNewAlbum(title);
    }
    
    return id;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::removeAlbums()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::removeAlbums(const QModelIndexList &indexList)
{
	int err(-1);
    
	if (mCollectionClient)
	{
		TMPXItemId mpxId;
		QList<TMPXItemId> ids;

		for(int i = 0; i < indexList.count(); ++i)
		{
			mpxId = getMediaIdAtIndex(indexList.at(i));
			if((mpxId != TMPXItemId::InvalidId()) && (mpxId.iId2 == KVcxMvcMediaTypeAlbum))
			{
				ids.append(getMediaIdAtIndex(indexList.at(i)));
			}
		}
		
		if (ids.count())
		{
			err = mCollectionClient->removeAlbums(ids);
		}
	}

    return err;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::resolveAlbumName()
// -----------------------------------------------------------------------------
//
QString VideoSortFilterProxyModel::resolveAlbumName(
    const QString& albumName) const
{
    QString resolvedName = albumName.trimmed();

    // for checking names, we need to use collection list proxy model 
    // to get all existing album names including default ones
    VideoSortFilterProxyModel *collectionModel = 
                       VideoCollectionWrapper::instance().getModel(
                                   VideoCollectionWrapper::ECollections);
    if(!collectionModel || !mModel)
    {
        return resolvedName;
    }    
    
    int i(0);
    QModelIndex proxyIndex = collectionModel->index(i, 0);
    QSet<QString> names;
    QVariant data;
    // create set of existing names
    while (proxyIndex.isValid())
    {
        data = mModel->data(collectionModel->mapToSource(proxyIndex), Qt::DisplayRole);
        if (data.isValid())
        {
            QStringList stringList = data.toStringList();
            names.insert(stringList.at(0));
        }
        proxyIndex = collectionModel->index(++i, 0, QModelIndex());
    }
    i = 0;
    QString firstMatch("");
    // find a name that does not yet exists
    while(names.contains(resolvedName))
    {
        if (!firstMatch.length())
        {
            firstMatch = resolvedName;
        }
        ++i;
        resolvedName = tr("%1 (%2)").arg(firstMatch).arg(i);
    }
    return resolvedName;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::addItemsInAlbum()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::addItemsInAlbum(TMPXItemId albumId,
        const QList<TMPXItemId> &mediaIds)
{
    int err(-1);
    
    if (mCollectionClient)
    {        
        // add items in album
        err = mCollectionClient->addItemsInAlbum(albumId, mediaIds);
    }
    
    return err;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getOpenItem()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoSortFilterProxyModel::getOpenItem() const
{
    TMPXItemId itemId = TMPXItemId::InvalidId();
    
    if(mModel && mCollectionClient)
    {
        if(mType == VideoCollectionWrapper::EAllVideos)
        {
            itemId.iId1 = KVcxMvcCategoryIdAll;
            itemId.iId2 = KVcxMvcMediaTypeCategory;        
        }
        else if(mType == VideoCollectionWrapper::ECollectionContent)
        {
            itemId = mModel->albumInUse();
        }
    }

    return itemId;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::setGenericIdFilter()
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::setGenericIdFilter(TMPXItemId itemId, bool filterValue)
{
    if(mType == VideoCollectionWrapper::EGeneric)
    {
        mGenericFilterId = itemId;
        mGenericFilterValue = filterValue;
        invalidateFilter();
    }
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::albumChangedSlot()
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::albumChangedSlot()
{
    // ignore if not collection content model
    if (mType == VideoCollectionWrapper::ECollectionContent)
    {
        invalidateFilter();
    }
}

// end of file

