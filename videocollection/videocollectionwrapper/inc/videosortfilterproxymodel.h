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
* Description:   Videolist sort filter proxy implementation
* 
*/

#ifndef VIDEOSORTFILTERPROXYMODEL_H
#define VIDEOSORTFILTERPROXYMODEL_H

#include <qsortfilterproxymodel.h>
#include <e32const.h>
#include <mpxitemid.h>
#include "videocollectionexport.h"

class QTimer;
class VideoListDataModel;
class VideoCollectionClient;

class VIDEOCOLLECTION_DLL_EXPORT VideoSortFilterProxyModel : public QSortFilterProxyModel 
{
    Q_OBJECT
    
public: 
    
    /**
     * Contructor.
     *
     * @param parent parent of this widget
     * @param client Collection client pointer to use.
     */
    VideoSortFilterProxyModel(QObject *parent=0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoSortFilterProxyModel();
    
    /**
     * Initializes model, calls initialize to source model, and sets
     * the given sourceModel as source model for this proxy model.
     * 
     * @param collection mpx video collectionobject
     * @param sourceModel the source model
     * 
     * @return int: 0 if everything ok
     */
    int initialize(VideoListDataModel *sourceModel);

public:
    
    /**
     * First call opens the collection at the defined level. Subsequent calls only change the level.
     * This starts populating the model data from the MyVideos collection plugin.
     * 
     * @param level The level where the collection is opened.
     * @return 0 if opening was successful, negative if not.
     */
    int open(int level);
    
    /**
     * Starts sorting. If async parameter is defined as true uses idle timer: 
     * starts timer with zero interval. When system has time to run timer sort()
     * -method is called. If async is false, then the sort() -method is called
     * immediatelly.
     * 
     * @param sortingRole The role that sorting is done by.
     * @param sortingOrder The sorting order, e.g. ascending or descending.
     * @param async Defines if the sorting is done asyncronously or not. Default
     *      is true.
     */
    void doSorting(int sortingRole, Qt::SortOrder order, bool async = true);
    
    /**
     * Method to delete items at provided indeces
     *
     * @param indexList list of indeces.
     * @return int 0 succeeds, -1 does not succeed.
     */
    int deleteItems(const QModelIndexList &indexList);
    
    /**
    * Opens the defined item. This is quite abstract as depending
    * on the type of the item, the behaviour or result could be
    * quite different.
    * 
    * @param index index of the item to be opened
    * @return int 0 succeeds, <0 if opening does not succeed.
    */    
    int openItem(const QModelIndex &index);

    /**
    * Return to collection level
    *
    */    
    int back();

    /**
     * Starts fetching full details for the defined item.
     * 
     * @param index index of the item from where to get details from
     * @return int 0 succeeds, < 0 if detail fetching startup does not succeed.
     */   
    int fetchItemDetails(const QModelIndex &index);

    /**
     * Method checks that model exists and asks for mpx id from
     * given object.
     * 
     * @param index index of the item in UI.
     * @return TMPXItemId mpx id if succeeds TMPXItemId::InvalidId() in case of error
     *  
     */
    TMPXItemId getMediaIdAtIndex(const QModelIndex &index);

    /**
     * Method checks that model exists and asks for file path from
     * given object.
     * 
     * @param mediaId id of the item.
     * @return QString file path if succeeds, empty string in case of error.
     *  
     */
    QString getMediaFilePathForId(TMPXItemId mediaId);
        
    /**
     * Adds a collection (album).
     * 
     * @param name Name for the collection.
     * @param thumbnail Path for the image file to use as a thumbnail for the
     *      collection. Can be empty.
     * @param mediaIds Media ids of the videos that are added to the collection 
     *      at creation time. Size can be zero.
     * @return 0, if collection creation was success, below 0 if there was an error.
     */
    int addNewCollection(QString name, QString thumbnail, QList<TMPXItemId> mediaIds);

signals:

// TODO should the index be QModelIndex instead?

    /**
    * Signal to be emitted if detail fetching started ok
    * 
    * @param index, index of the item
    */ 
    void shortDetailsReady(int index);
    
    /**
    * Signal to be emitted after all details are being fetched.
    * 
    * @param index, index of the item
    */ 
    void fullDetailsReady(int index);
    
    /**
     * Signals that the model is ready, ie. that all data has been
     * loaded from myvideoscollection.
     */
    void modelReady();
 
protected: // from QSortFilterProxyModel
    
    /**
     * Compares items based in the role setted before sorting.
     *  
     */
    bool lessThan ( const QModelIndex & left, const QModelIndex & right ) const;
    
    /**
     * Filtering functionality. Called by the fw.
     *  
     * @param source_row row to chexk
     * @param source_parent row's parent
     * 
     * @return bool row accepted(true) or not accepted(false)
     */
    bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;

private slots:

    /**
     * Timeout slot for zero timer. Stops timer and calls sort(). 
     * After call is finished, emits sortingReady.
     *  
     */
    void processSorting();
    
private:
    
    /**
     * Pointer to the actual model.
     * Not owned.
     */
    VideoListDataModel *mModel;
    
    /**
     * Collection client.
     * Not owned. 
     */
    VideoCollectionClient *mCollectionClient;
    
    /**
     * Currently open level.
     */
    int mLevel;
    
    /**
     * Timer object. Owned.
     */
    QTimer  *mIdleSortTimer;
    
    /**
     * sort role to be setted after idle timer time outs for sorting to be able to be executed
     */
    int mWantedSortRole;
    
    /**
     * sort order (ascending or descending).
     */
    Qt::SortOrder mWantedSortOrder;
    
};

#endif  //VIDEOSORTFILTERPROXYMODEL_H
