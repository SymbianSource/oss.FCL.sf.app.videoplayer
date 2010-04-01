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
	 * sets provided row count:
	 * if count given is larger that existing count, rows are inserted
	 * if count given is smaller than existing count, rows are removed
	 */
	void setRowCount(int count);

public: // from QAbstractItemModel
    
    /**
     * Returns row count
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * Dummy implementation
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
     * dummy data
     */
    QMap<int, QVariant> mData;

    /**
     * row count
     */
    int mRowCount;
    
};
#endif  // __DUMMYDATAMODEL_H__

// End of file
    


