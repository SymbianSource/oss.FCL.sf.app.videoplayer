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
* Description: DummyDataModel class implementation
* 
*/
// INCLUDE FILES
#include "dummydatamodel.h"

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// DummyDataModel::DummyDataModel()
// -----------------------------------------------------------------------------
//
DummyDataModel::DummyDataModel() : 
mRowCount(0)
{

}

// -----------------------------------------------------------------------------
// DummyDataModel::~DummyDataModel()
// -----------------------------------------------------------------------------
//
DummyDataModel::~DummyDataModel()
{

}

// -----------------------------------------------------------------------------
// DummyDataModel::setRowCount()
// -----------------------------------------------------------------------------
//
void DummyDataModel::setRowCount(int count)
{
    if ( count == mRowCount ) return;
    
    if ( count > mRowCount ) {
        beginInsertRows(QModelIndex(), mRowCount, count - 1);
        mRowCount = count;
        endInsertRows();
    } else {
        beginRemoveRows(QModelIndex(), count, mRowCount - 1);
        mRowCount = count;
        endRemoveRows();
    }
}

// -----------------------------------------------------------------------------
// DummyDataModel::rowCount()
// -----------------------------------------------------------------------------
//
int DummyDataModel::rowCount(const QModelIndex &parent ) const
{
    // according to Qt documentation if parent is valid this should return 0 if
    // implementing a table based implementation like this.
    if (parent.isValid())
    {
        return 0;
    }
    
    return mRowCount;
}

// -----------------------------------------------------------------------------
// DummyDataModel::itemData()
// -----------------------------------------------------------------------------
//
QMap<int, QVariant> DummyDataModel::itemData(const QModelIndex &index) const
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
// DummyDataModel::data()
// -----------------------------------------------------------------------------
//
QVariant DummyDataModel::data(const QModelIndex & index, int role) const
{
    QVariant returnValue = QVariant();
        
    return returnValue;
}

// -----------------------------------------------------------------------------
// DummyDataModel::columnCount()
// -----------------------------------------------------------------------------
//
int DummyDataModel::columnCount(const QModelIndex & parent) const
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
// DummyDataModel::index()
// -----------------------------------------------------------------------------
//
QModelIndex DummyDataModel::index(int row, int column, const QModelIndex & /*parent*/) const
{
    return createIndex(row, column);
}

// -----------------------------------------------------------------------------
// DummyDataModel::parent()
// -----------------------------------------------------------------------------
//
QModelIndex DummyDataModel::parent(const QModelIndex & /*index*/) const
{
    return QModelIndex();
}

// End of file
