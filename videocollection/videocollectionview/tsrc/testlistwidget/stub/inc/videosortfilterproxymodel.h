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
* Description:   Videolist sort filter proxy stub implementation
* 
*/

#ifndef VIDEOSORTFILTERPROXYMODEL_H
#define VIDEOSORTFILTERPROXYMODEL_H

#include <qsortfilterproxymodel.h>
#include <QList>
#include <QModelIndex>
#include <vcxmyvideosdefs.h>

class VideoSortFilterProxyModel : public QSortFilterProxyModel 
{
    Q_OBJECT
    
public: 
    
    /**
     * Contructor.
     *
     * @param parent parent of this widget
     */
    VideoSortFilterProxyModel(QObject *parent=0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoSortFilterProxyModel();
    
public:
    
    /**
     * dummy impl
     *  
     */
    void doSorting(int sortingOrder);
    
    /**
     * dummy impl
     */
    int deleteItems(const QModelIndexList &indexList);
    
    /**
    * dummy impl
    */    
    int openItem(const QModelIndex &index);
     
    /**
     * dummy impl
     */   
    int fetchItemDetails(const QModelIndex &index);
    
    
    /**
     * dummy impl
     *  
     */
     TMPXItemId getMediaIdAtIndex(QModelIndex index);
    
    /**
    * Return to collection level
    *
    */    
    void back();
    
    static void reset();
    
    
signals:

    /**
    * dummy impl
    */ 
    void shortDetailsReady(int index);
    
    /**
    * dummy impl
    */ 
    void fullDetailsReady(int index);
    
    /**
     * dummy impl
     */
    void modelReady();
 
protected: // from QSortFilterProxyModel
    
    /**
     * dummy impl
     *  
     */
    bool lessThan ( const QModelIndex & left, const QModelIndex & right ) const;
    
private slots:

    /**
     * Timeout slot for zero timer. Stops timer and calls sort(). 
     * After call is finished, emits sortingReady.
     *  
     */
    void processSorting();
    
public:
    
    /**
     * list containing some mpx ids
     */
    static QList<TMPXItemId> mItemIds;
    
    /**
     * dummy
     */
    static int mSortOrder;
    
    /**
     * last accessed index
     */
    static QModelIndex mLastIndex;
    
    /**
     * details value
     */
    static int mDetailsReturnValue;
    
};



#endif  //VIDEOSORTFILTERPROXYMODEL_H
