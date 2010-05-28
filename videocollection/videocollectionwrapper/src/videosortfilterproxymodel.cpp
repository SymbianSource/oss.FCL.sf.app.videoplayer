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

// Version : %version: 66 %

// INCLUDE FILES
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
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::VideoSortFilterProxyModel(VideoCollectionCommon::TModelType type, QObject *parent) :
QSortFilterProxyModel(parent),
mModel(0),
mCollectionClient(0),
mType(type),
mLevel(VideoCollectionCommon::ELevelInvalid),
mGenericFilterId(TMPXItemId::InvalidId()),
mGenericFilterValue(false),
mIdleSortTimer(0),
mWantedSortRole(VideoCollectionCommon::KeyDateTime),
mWantedSortOrder(Qt::AscendingOrder)
{
	FUNC_LOG_ADDR(this);
	INFO_2("VideoSortFilterProxyModel::VideoSortFilterProxyModel() [0x%x]: type: %d", this, type);
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::~VideoSortFilterProxyModel
// -----------------------------------------------------------------------------
//
VideoSortFilterProxyModel::~VideoSortFilterProxyModel()
{
	FUNC_LOG_ADDR(this);
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::initialize
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::initialize(VideoListDataModel *sourceModel)
{
	FUNC_LOG_ADDR(this);
    if(!sourceModel)
    {
        return -1;
    }
    
    if(!mModel) 
    {
        mModel = sourceModel;
        if(!connectSignals()) 
        {
            ERROR_1(-1, "VideoSortFilterProxyModel::initialize() [0x%x] failed to connect signals.", this);
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
	FUNC_LOG_ADDR(this);
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
    if(mType == VideoCollectionCommon::EModelTypeCollectionContent)
    {
        if(!connect(mModel, SIGNAL(albumChanged()),
                    this, SLOT(albumChangedSlot()))) 
        {
            return false;
        }
    }
    if(mType == VideoCollectionCommon::EModelTypeCollections)
    {
        if(!connect(mModel, SIGNAL(itemModified(const TMPXItemId &)),
                    this, SLOT(itemModifiedSlot(const TMPXItemId &)))) 
        {
            return false;
        }
    }
    return true;
}
   
// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::disconnectSignals
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::disconnectSignals()
{
	FUNC_LOG_ADDR(this);
	disconnect(mModel, SIGNAL(modelReady()), this, SIGNAL(modelReady()));
    disconnect(mModel, SIGNAL(modelChanged()), this, SIGNAL(modelChanged()));
    if(mType == VideoCollectionCommon::EModelTypeCollectionContent)
    {
        disconnect(mModel, SIGNAL(albumChanged()), this, SLOT(albumChangedSlot()));
    }
    if(mType == VideoCollectionCommon::EModelTypeCollections)
    {
        disconnect(mModel, SIGNAL(itemModified(const TMPXItemId &)), 
                   this, SLOT(itemModifiedSlot(const TMPXItemId &)));
    }
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::open
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::open(VideoCollectionCommon::TCollectionLevels level)
{
	FUNC_LOG_ADDR(this);
	INFO_2("VideoSortFilterProxyModel::open() [0x%x] level: %d", this, level);
	
    if(!mCollectionClient)
    {
        return -1;
    }
    
    if(mLevel != level) 
    {
        INFO_1("VideoSortFilterProxyModel::open() [0x%x] opening different level, invalidating.", this);
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
	FUNC_LOG_ADDR(this);
	INFO_3("VideoSortFilterProxyModel::doSorting() [0x%x] sortingRole: %d, order: %d", this, sortingRole, order);
	
    if(sortingRole != VideoCollectionCommon::KeyTitle &&
       sortingRole != VideoCollectionCommon::KeySizeValue &&
       sortingRole != VideoCollectionCommon::KeyDateTime &&
       sortingRole != VideoCollectionCommon::KeyNumberOfItems)
    {
        // default sorting order is by date for other models but collections.
        if(mType != VideoCollectionCommon::EModelTypeCollections)
        {   
            mWantedSortRole = VideoCollectionCommon::KeyDateTime;
        }
        else
        {
            mWantedSortRole = VideoCollectionCommon::KeyTitle;
        }
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
    }
    else
    {
        processSorting();
    }
}
// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getSorting
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::getSorting(int &sortingRole, Qt::SortOrder &order)
{
	FUNC_LOG_ADDR(this);
	sortingRole = mWantedSortRole;
	order       = mWantedSortOrder;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::deleteItems
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::deleteItems(const QModelIndexList &indexList)
{
	FUNC_LOG_ADDR(this);
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
	FUNC_LOG_ADDR(this);
    if(mediaId != TMPXItemId::InvalidId() && mCollectionClient)
    {
        if(mCollectionClient->openItem(mediaId) == 0)
        {
            if(mediaId.iId2 != KVcxMvcMediaTypeVideo)
            { 
                mLevel = VideoCollectionCommon::ELevelAlbum;
                mModel->setAlbumInUse(mediaId);
                INFO_1("VideoSortFilterProxyModel::open() [0x%x] opening album or category, invalidating.", this);
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
	FUNC_LOG_ADDR(this);
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
	FUNC_LOG_ADDR(this);
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
	FUNC_LOG_ADDR(this);
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
    // Recently played (missing currently)
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
    //  VideoCollectionCommon::KeyTitle,
    //  VideoCollectionCommon::KeySizeValue,
    //  VideoCollectionCommon::KeyNumberOfItems,
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
    if(sRole == VideoCollectionCommon::KeyTitle)
    {
        QString leftString = leftData.toString().toUpper();
        QString rightString = rightData.toString().toUpper();
        return QString::localeAwareCompare(leftString, rightString) < 0;
    }
    else if(sRole == VideoCollectionCommon::KeySizeValue ||
            sRole == VideoCollectionCommon::KeyNumberOfItems)
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
bool VideoSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);
    
    if(!sourceModel())
    {
        return false;
    }

    if(source_row < 0 || source_row >= sourceModel()->rowCount())
    {
        return false;
    }
    TMPXItemId id = mModel->mediaIdAtIndex(source_row);
    if(id == TMPXItemId::InvalidId())
    {
        return false;
    }
    
    if (mType == VideoCollectionCommon::EModelTypeAllVideos)
    {
        if(id.iId2 == KVcxMvcMediaTypeVideo)
        {
            return true;
        }
    }
    else if(mType == VideoCollectionCommon::EModelTypeCollections)
    {
        if(mLevel == VideoCollectionCommon::ELevelCategory && id.iId2 != KVcxMvcMediaTypeVideo)
        {
            return true;
        }       
    }
    else if (mType == VideoCollectionCommon::EModelTypeCollectionContent)
    {
        // if item belongs to the open album, accept it
        if (mModel->belongsToAlbum(id))
        {
            return true;
        }
    }
    else if(mType == VideoCollectionCommon::EModelTypeGeneric)
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
    TMPXItemId mpxId = TMPXItemId::InvalidId();
    if(index.isValid())
    {
        QModelIndex sourceIndex = mapToSource(index);
        if(mModel && sourceIndex.isValid())
        {
            mpxId = mModel->mediaIdAtIndex(sourceIndex.row());
        }
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
	FUNC_LOG_ADDR(this);
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
	FUNC_LOG_ADDR(this);
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
	FUNC_LOG_ADDR(this);
    QString resolvedName = albumName.trimmed();

    // for checking names, we need to use collection list proxy model 
    // to get all existing album names including default ones
    VideoSortFilterProxyModel *collectionModel = 
                       VideoCollectionWrapper::instance().getModel(
                                   VideoCollectionCommon::EModelTypeCollections);
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
        data = mModel->data(collectionModel->mapToSource(proxyIndex), VideoCollectionCommon::KeyTitle);
        if (data.isValid())
        {
            names.insert(data.toString());
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
int VideoSortFilterProxyModel::addItemsInAlbum(TMPXItemId &albumId,
        const QList<TMPXItemId> &mediaIds)
{
	FUNC_LOG_ADDR(this);
    int err(-1);
    
    if (mCollectionClient)
    {        
        // add items in album
        err = mCollectionClient->addItemsInAlbum(albumId, mediaIds);
    }
    
    return err;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::removeItemsFromAlbum()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::removeItemsFromAlbum(TMPXItemId &albumId, 
        const QList<TMPXItemId> &mediaIds)
{
	FUNC_LOG_ADDR(this);
    int err(-1);
       
    if (mModel)
    {        
        // remove items in album
        err = mModel->removeItemsFromAlbum(albumId, mediaIds);
        if(err > 0)
        {
            // if there really were items to be removed, invalid filter
            INFO_1("VideoSortFilterProxyModel::removeItemsFromAlbum() [0x%x] items removed, invaliding.", this);
            invalidateFilter();
            err = 0;
        }
    }
    return err;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::renameAlbum()
// -----------------------------------------------------------------------------
//
int VideoSortFilterProxyModel::renameAlbum(const TMPXItemId &albumId, const QString &newTitle)
{
	FUNC_LOG_ADDR(this);
    int err(-1);

    if(mCollectionClient)
    {
        return mCollectionClient->renameAlbum(albumId, newTitle);
    }
    return err;
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getOpenItem()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoSortFilterProxyModel::getOpenItem() const
{
	FUNC_LOG_ADDR(this);
    TMPXItemId itemId = TMPXItemId::InvalidId();
    
    if(mModel && mCollectionClient)
    {
        if(mType == VideoCollectionCommon::EModelTypeAllVideos)
        {
            INFO_1("VideoSortFilterProxyModel::getOpenItem() [0x%x] all videos is open.", this);
            itemId.iId1 = KVcxMvcCategoryIdAll;
            itemId.iId2 = KVcxMvcMediaTypeCategory;
        }
        else if(mType == VideoCollectionCommon::EModelTypeCollectionContent)
        {
            INFO_1("VideoSortFilterProxyModel::getOpenItem() [0x%x] category or album is open.", this);
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
	FUNC_LOG_ADDR(this);
    if(mType == VideoCollectionCommon::EModelTypeGeneric)
    {
        INFO_3("VideoSortFilterProxyModel::setGenericIdFilter() [0x%x] itemId.iId2: %d, filterValue: %d", this, itemId.iId2, filterValue);
        mGenericFilterId = itemId;
        mGenericFilterValue = filterValue;
        INFO_1("VideoSortFilterProxyModel::setGenericIdFilter() [0x%x] invalidating.", this);
        invalidateFilter();
    }
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::setAlbumInUse()
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::setAlbumInUse(TMPXItemId albumId)
{
	FUNC_LOG_ADDR(this);
    if(mModel)
    {
        INFO_1("VideoSortFilterProxyModel::setAlbumInUse() [0x%x] invalidating.", this);
        mModel->setAlbumInUse(albumId);
        invalidateFilter();
    }
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::albumChangedSlot()
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::albumChangedSlot()
{
	FUNC_LOG_ADDR(this);
    // ignore if not collection content model
    if (mType == VideoCollectionCommon::EModelTypeCollectionContent)
    {
        INFO_1("VideoSortFilterProxyModel::albumChangedSlot() [0x%x] invalidating.", this);
	    // sort and invalidate filtering, otherwise newle created album content won't sort
    	invalidateFilter();
        setSortRole(mWantedSortRole);
        sort(0, mWantedSortOrder);
    }
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::itemModifiedSlot()
// -----------------------------------------------------------------------------
//
void VideoSortFilterProxyModel::itemModifiedSlot(const TMPXItemId &itemId)
{
	FUNC_LOG_ADDR(this);
    if(mType == VideoCollectionCommon::EModelTypeCollections &&
       (itemId.iId2 == KVcxMvcMediaTypeAlbum || itemId.iId2 == KVcxMvcMediaTypeCategory))
    {
        INFO_1("VideoSortFilterProxyModel::itemModifiedSlot() [0x%x] invalidating.", this);
        invalidate();
    }
}

// -----------------------------------------------------------------------------
// VideoSortFilterProxyModel::getType()
// -----------------------------------------------------------------------------
//
VideoCollectionCommon::TModelType VideoSortFilterProxyModel::getType()
{
	FUNC_LOG_ADDR(this);
	return mType;
}

// End of file
