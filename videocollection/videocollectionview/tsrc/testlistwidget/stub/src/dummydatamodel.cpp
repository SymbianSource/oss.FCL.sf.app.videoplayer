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
#include "videocollectioncommon.h"

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// DummyDataModel::DummyDataModel()
// -----------------------------------------------------------------------------
//
DummyDataModel::DummyDataModel()
{
    reset();
}

// -----------------------------------------------------------------------------
// DummyDataModel::~DummyDataModel()
// -----------------------------------------------------------------------------
//
DummyDataModel::~DummyDataModel()
{
    reset();
}

// -----------------------------------------------------------------------------
// DummyDataModel::lastIndex()
// -----------------------------------------------------------------------------
//
QModelIndex DummyDataModel::lastIndex()
{
    return mLastIndex;
}

// -----------------------------------------------------------------------------
// DummyDataModel::dataAccessCount()
// -----------------------------------------------------------------------------
//
int DummyDataModel::dataAccessCount()
{
    return mDataAccessCount;
}

// -----------------------------------------------------------------------------
// DummyDataModel::reset()
// -----------------------------------------------------------------------------
//
void DummyDataModel::reset()
{
    mLastIndex = QModelIndex();
    mDataAccessCount = 0;
    mRowCount = 0;
    mData.clear();
}

// -----------------------------------------------------------------------------
// DummyDataModel::setData()
// -----------------------------------------------------------------------------
//
void DummyDataModel::setData(int role, QVariant data)
{
    mData.insert(role, data);
}

// -----------------------------------------------------------------------------
// DummyDataModel::setRowCount()
// -----------------------------------------------------------------------------
//
void DummyDataModel::setRowCount(int count)
{
    if ( count == mRowCount ) return;
    
    if ( count > mRowCount ) {
        beginInsertRows(QModelIndex(), mRowCount, count);
        mRowCount = count;
        endInsertRows();
    } else {
        beginRemoveRows(QModelIndex(), count, mRowCount);
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
        itemData.insert(VideoCollectionCommon::KeyFilePath, data(index, VideoCollectionCommon::KeyFilePath));
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
    mLastIndex = index;
    mDataAccessCount++;
    if (index.isValid() && index.row() < mRowCount) 
    {
        returnValue = mData.value(role);
    }
    
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




/*
// -----------------------------------------------------------------------------
// DummyDataModel::prepareDetailRow()
// -----------------------------------------------------------------------------
//
const QString DummyDataModel::prepareDetailRow( int index ) const
{
    // TODO: - metadata?
    //       - download -status?

    const QString secondLineSeparator = ", ";

    const QString lengthStr = prepareLengthString(index);
    
    const QString sizeStr = prepareSizeString(index);
    
    QString detailRow("");
    
    if( lengthStr.count() > 0 )
    {
        detailRow = lengthStr;
    }
   
    if( sizeStr.count() > 0 )
    {
        if( detailRow.count() > 0)
        {
            detailRow += secondLineSeparator;
        }
        detailRow += sizeStr;
    }
    return detailRow;
}

// -----------------------------------------------------------------------------
// DummyDataModel::prepareSizeString()
// -----------------------------------------------------------------------------
//
const QString DummyDataModel::prepareSizeString( int index ) const
{
    const int videoSizeGB( 0x40000000 );
    const int videoSizeHalfGB( 0x20000000 );
    const int videoSizeMB( 0x100000 );
    const int videoSizeHalfMB( 0x80000 );
    const int videoSizeKB( 0x400 );
    const int videoSizeHalfKB( 0x200 );
    
    QString sizeStr("");
    if(!mVideoList)
    {
        return sizeStr;
    } 
    
    uint size = mVideoList->getVideoSizeFromIndex(index);
    
    if ( size > 0 )
    {
        uint dispSize = 0;
        
        if ( size >= videoSizeGB )
        {
            dispSize  = size + videoSizeHalfGB;
            dispSize /= videoSizeGB;
            sizeStr = QString(QString::number(dispSize)+ tr(" GB"));
        }
        else if ( size >= videoSizeMB )
        {
            dispSize  = size + videoSizeHalfMB;
            dispSize /= videoSizeMB;
            sizeStr = QString(QString::number(dispSize)+ tr(" MB"));
        }
        else
        {
            dispSize  = size + videoSizeHalfKB;
            dispSize /= videoSizeKB;
            sizeStr = QString(QString::number(dispSize)+ tr(" kB"));
        }
     }
    return sizeStr;
}

// -----------------------------------------------------------------------------
// DummyDataModel::prepareLengthString()
// -----------------------------------------------------------------------------
//
const QString DummyDataModel::prepareLengthString( int index ) const
{
    const int secondsInMinute( 60 );
    const int secondsInHour( 3600 );
    QString lengthStr("");
    if(!mVideoList)
    {
        return lengthStr;
    } 
    uint total = static_cast<uint>(mVideoList->getVideodurationFromIndex(index));
    
    // TODO: simplify this!
    
    if ( total > 0 )
    {
        uint hours = total / secondsInHour;
        uint minutes = total / secondsInMinute % secondsInMinute;
        uint seconds = total % secondsInMinute;
        
        if ( hours > 0 )
        {
            lengthStr = QString(QString::number(hours)+ tr("  hours ")); 
            lengthStr += QString(QString::number(minutes)+ tr(" minutes"));
        }

        else if ( minutes > 0 )
        {
            lengthStr = QString(QString::number(minutes)+ tr(" minutes "));
            lengthStr += QString(QString::number(seconds)+ tr(" seconds"));  
        }
        else
        {
            lengthStr = QString(QString::number(seconds)+ tr(" seconds"));                                        
        }
    }
    return lengthStr;
   
}
*/

// End of file
