
/**
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
* Description: Stub collection client to be used when unit testing videocollectionsortfilterproxy 
* 
*/

#ifndef __STUB_VIDEOLISTDATAMODELFORPROXY_H
#define __STUB_VIDEOLISTDATAMODELFORPROXY_H

#include <QObject>
#include <QList>
#include <QString>
#include <qabstractitemmodel.h>
#include <qdatetime.h>
#include "videocollectioncommon.h"

// CVideoListDataModel::Data returns valid variant when used this and name exists for the row. 
const int INVALID_ROLE_FOR_SORTING = VideoCollectionCommon::KeyStatus+10;

class VideoCollectionClient;


class  VideoListDataModel : public QAbstractItemModel
{

    Q_OBJECT
    
public: // Constructors and destructor

    /**
     * Constructor
     *
     */
    VideoListDataModel(QObject *parent = NULL);

    /**
     * destructor
     *
     */
    virtual ~VideoListDataModel();

	/**
     * Initializes model. Sets model cache proxy and cache's plugin to 
     * to be this object. Creates videolist and puts it to observe 
     * collection client. Calls  connectSignals to connect signals into
     * appropriate slots
     * 
     * @paran collection mpx video collectionobject
     * 
     * @return int: 0 if everything ok
     */
	int initialize( VideoCollectionClient* collection);
    
	/**
     * Method calls video list data to check if there are valid media object 
     * at the given index. If there is, item id is returned.
     * 
     * @param index index of the item to be opened
     * @return int item id ( > 0), < 0 if no valid item.
     */ 
	int mediaIdAtIndex(int index) const;

    /**
     * Method calls video list data to check if there are valid media object 
     * for the given media id. If there is, file path for is returned.
     * 
     * @param mediaId id of the item to be opened
     * @return QString file path of the media at index, empty string if not valid item.
     */ 
    QString mediaFilePathForId(int mediaId) const;
    
	/**
     * Called by the client when removal of videos are requested.
     * Sets video status to be removed to videolistdata and
     * eventually calls collection to handle the actual removing.
     * 
     * @param indexlist list of indeces of items requested for deletion
     * 
     * @return bool true if removal startup succeeds
     */
    bool removeRows(const QModelIndexList &indexList);
            
public: // from QAbstractItemModel
    
    /**
     * Returns mRowCount value
     *
     * @param parent - not used
     * 
     * @return int
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
    /**
     * Method does nothing in this stub
     *
     * @param index - not used
     * 
     * @return QMap empty map
     */
    QMap<int, QVariant> itemData(const QModelIndex &index) const;
    
    /**
     * Returns item value based on the role defined:
     * Qt::DisplayRole: from mNames -list
     * VideoCollectionCommon::KeySizeValue: from mSizes -list
     * VideoCollectionCommon::KeyDate: from mDates -list
     *
     * @param index - item index to return
     * @parema role - role indicating wanted value
     * 
     * @return QVariant
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    
    /**
     * Returns 0 always.
     *
     * @param parent - not used
     * 
     * @return int
     */
    int columnCount(const QModelIndex & parent = QModelIndex()) const;

    /**
     * Calls QAbstractItemModel::createIndex to return appropriate QModelIndex
     *
     * @param row - row index 
     * @param column - col index
     * @param parent - not used
     * 
     * @return QModelIndex
     */
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    
    /**
     * returns invalid QModelIndex
     *
     * @return QModelIndex
     */
    QModelIndex parent(const QModelIndex & index) const;     

    
public: // helper methods for test
    
    /**
     * sets given value tomRowCount
     *
     * @param count - value to set
     */
    void setUsedRowCount(int count);
    
    /**
     * appends given string to mNames
     *
     * @param data - value to append to list
     */
    void appendData(QString data);
    
    /**
     * appends given uint to mSizes
     *
     * @param data - value to append to list
     */
    void appendData(uint data);
    
    /**
     * appends given QDate to mDates
     *
     * @param data - value to append to list
     */
    void appendData(QDateTime data);
    
    /**
     * Appends given status to mStatuses.
     *
     * @param status - value to append to list.
     */
    void appendStatus(int status);
    
signals:

	/**
	* This signal is connected to video list's details ready
	* -signal indicating that video details data is fetched ok
	* 
	* @param index index of the video item
	*/
	void fullVideoDetailsReady(int index);
	
    /**
     * Signals that the model is ready, ie. loaded all data from
     * myvideocollection.
     */
    void modelReady();	
	
public:
    
	static bool mInitFails;
	static bool mRemoveRowsFails;
	static bool mGetMediaIdAtIndexFails;
	static QString mMediaFilePathReturnValue;
	
	static int mLastDeletedIndexRow;
    
private:
    
    /**
     * Setted count of rows. Not necessary match the actual count.
     */
    int mRowCount;
    
    /**
     * List of strings when fetching Qt::DisplayRole 
     */
    QList<QString> mNames;
    
    /**
     * List of uints when fetching  VideoCollectionCommon::KeySizeValue
     */
    QList<uint> mSizes;
    
    /**
    * List of QDates when fetching  VideoCollectionCommon::KeyDate
    */
    QList<QDateTime> mDates;
    
    /**
    * List of statuses when fetching  VideoCollectionCommon::KeyStatus
    */
    QList<int> mStatuses;    
    
};

#endif // __STUB_VIDEOLISTDATAMODELFORPROXY_H


