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
* Description:  DummyDataModel class definition*
*/

#ifndef __DUMMYDATAMODEL_H__
#define __DUMMYDATAMODEL_H__


// INCLUDES
#include <QObject>
#include <qabstractitemmodel.h>



// FORWARD DECLARATIONS


class DummyDataModel : public QAbstractItemModel
{    
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
	Q_DISABLE_COPY(DummyDataModel) 

public: 

	/**
	 * Default constructor
     */
	DummyDataModel();	
	
	/**
     * Destructor
     */
	~DummyDataModel();
	
	/**
	 * latest accessed index
	 */
	QModelIndex lastIndex();
	
	/**
	 * how many time data has qbeen queried
	 */
	int dataAccessCount();
	
	/**
	 * reset static members
	 */
	void reset();
	
	/**
	 * saves data
	 */
	void setData(int role, QVariant data);
	
	/**
	 * sets row count
	 */
	void setRowCount(int count);

public: // from QAbstractItemModel
    
    /**
     * Returns video item count to the proxy cache
     * 
     * @param parent, parent's index, not used
     * 
     * @return int count of items
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * dummy impl
     * 
     * @return QMap<int, QVariant> item data
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
    
private:
    
    /**
     * data map
     */
    QMap<int, QVariant> mData;
    
    /**
     * last index
     */
    mutable QModelIndex mLastIndex;
    
    /**
     * data access count
     */
    mutable int mDataAccessCount;
    
    /**
     * row count
     */
    int mRowCount;
    
};
#endif  // __DUMMYDATAMODEL_H__

// End of file
    


