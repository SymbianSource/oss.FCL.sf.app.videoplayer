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
* Description:  Dummy videolistsortfilterproxymodel class definition*
*/

#ifndef VIDEOSORTFILTERPROXYMODEL_H
#define VIDEOSORTFILTERPROXYMODEL_H

// INCLUDES
#include <QObject>
#include <qabstractitemmodel.h>
#include <mpxitemid.h>

// FORWARD DECLARATIONS


class VideoSortFilterProxyModel : public QAbstractItemModel
{    
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
	Q_DISABLE_COPY(VideoSortFilterProxyModel) 

public: 

	/**
	 * Default constructor
     */
	VideoSortFilterProxyModel();	
	
	/**
     * Destructor
     */
	~VideoSortFilterProxyModel();
	
	/**
	 * sets index.row() to mStartPlaybackIndex
	 */
	int openItem(const TMPXItemId &index);
	
	/**
     * sets first index.row() to mDeleteFileIndex
     */
	int deleteItems(const QModelIndexList &indexList);
	
    /**
     * returns qmodelindex of item id provided.
     * 
     * @param item id
     * @return QModelIndex;
     */
    QModelIndex indexOfId(TMPXItemId id);

	
	TMPXItemId getMediaIdAtIndex(const QModelIndex &index);

signals:
	
    void shortDetailsReady(TMPXItemId index);
    
    void fullVideoDetailsReady(TMPXItemId index);

public: // from QAbstractItemModel
    
    VideoSortFilterProxyModel* sourceModel();
    
    /**
     * Returns video item count 
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Returns video item data from index pointed by.
     */
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    /**
     * data
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    
    /**
     * columnCount
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    
    /**
     * index
     */
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    
    /**
     * Parent
     */
    QModelIndex parent(const QModelIndex & index) const;
    
public: // dummy model helper methods
    /**
     * sets mDatareturnsInvalid
     */
    void setDataReturnInvalid(bool setInvalid);
    
    /**
     * returns mLastIndex
     */
    QModelIndex lastIndex();
    
    TMPXItemId lastId();

    /**
     * returns mDataAccessCount
     */
    int dataAccessCount();
    
    /**
     * clear everything
     */
    void reset();
    
    /**
     * sets role based value to mDatao 
     */
    void setData(int role, QVariant data);
    
    /**
     * sets mRowCount
     */
    void setRowCount(int count);
    
    /**
     * returns mStartPlaybackIndex
     */
    TMPXItemId startPlaybackIndex();
       
    /**
     * returns mDeleteFileIndex
     */
    int deleteFileIndex();
    
private:
    
    /**
     * map item to return from data();
     */
    QMap<int, QVariant> mData;
    
    /**
     * latest used index
     */
    mutable QModelIndex mLastIndex;
    
    /**
     * total data -access count throught data() -method
     */
    mutable int mDataAccessCount;
    
    
    mutable TMPXItemId mLastId;
    
    /**
     * setted "row count"
     */
    int mRowCount;
    
    /**
     * lates passed index of openItem
     */
    TMPXItemId mStartPlaybackIndex;
     
    /**
     * latest passed for delete 
     */
    int mDeleteFileIndex;     
    
    /**
     * if true, data() -method returns invalid QVartiant
     */
    bool mDatareturnsInvalid;
       
};
#endif  // VIDEOSORTFILTERPROXYMODEL_H

// End of file
    


