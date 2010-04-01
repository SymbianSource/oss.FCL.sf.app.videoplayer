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
* Description:   stub Videosortfilterproxymodel for VideoCollectionWrapperPrivate unit tests  
* 
*/

#ifndef VIDEOSORTFILTERPROXYMODEL_H
#define VIDEOSORTFILTERPROXYMODEL_H

#include <qsortfilterproxymodel.h>

class QTimer;
class VideoListDataModel;
class VideoCollectionClient;

class VideoSortFilterProxyModel : public QObject 
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
     * Starts sorting using idle timer: starts timer with zero 
     * interval. When system has time to run timer sort() -method
     * is called.
     *  
     */
    void doSorting(int role, Qt::SortOrder order, bool async = true);
    
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
     * Starts fetching full details for the defined item.
     * 
     * @param index index of the item from where to get details from
     * @return int 0 succeeds, < 0 if detail fetching startup does not succeed.
     */   
    int fetchItemDetails(const QModelIndex &index);
    
    /**
     * sets gInitFails
     * 
     * @param bool
     */
    static void setInitFailure(bool fails);
    
signals:

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
 
private:
    
    /**
     * Pointer to the actual model.
     * Not owned.
     */
    VideoListDataModel *mModel;
    
    /**
     * Currently open level.
     */
    int mLevel;
    
    /**
     * sort role
     */
    int mWantedSortRole;
    
    /**
     * sort order
     */
    Qt::SortOrder mWantedSortOrder;
    
};



#endif  //VIDEOSORTFILTERPROXYMODEL_H
