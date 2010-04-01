/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  stub videolistwidget for testing video list selection dialog
* 
*/

#ifndef VIDEOLISTWIDGET_H
#define VIDEOLISTWIDGET_H

#include <QObject>
#include <QItemSelection>
#include <QGraphicsWidget>
#include <QGraphicsLayoutItem>
#include <QGraphicsItem>
#include "hbview.h"
#include "hbabstractitemview.h"

class QGraphicsLayoutItem;
class VideoSortFilterProxyModel;

/**
 * dummy class inherited from QGraphicsItem needed
 * as parent item for videolistwidget
 */
class DummyGraphicsItem : public QGraphicsItem
{
public:
    
    virtual QRectF boundingRect() const {return QRectF();}
    
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }
    
};


/**
 * dummy class inherited from QGraphicsLayoutItem needed
 * as parent layout item for videolistwidget
 */
class DummyGraphicsLayoutItem : public QGraphicsLayoutItem
{    
public:
    
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const 
    {
        Q_UNUSED(which);
        Q_UNUSED(constraint);
        return QSizeF();
    }
    
};

/**
 * dummy selection model, just needed for signal connecting
 */
class DummySelectionModel : public QObject
{
    Q_OBJECT
public:
    DummySelectionModel(){}
    ~DummySelectionModel(){}
signals:
    void selectionChanged (const QItemSelection&, const QItemSelection&);
};


class VideoListWidget : public QObject
{
	Q_OBJECT
	
public:
        
    /**
     * Contructor.
     *
     * @param parent parent of this widget
     */       
	VideoListWidget(VideoSortFilterProxyModel &model);
      
    /**
     * Destructor.
     *
     */   
    virtual ~VideoListWidget();
     
    /**
     * returns model
     */
    VideoSortFilterProxyModel& getModel();
    
    /**
     * dummy implementation
     */
    QGraphicsLayoutItem* parentLayoutItem();
    
    /**
     * dummy implementation
     */
    QGraphicsItem* parentItem();
    
    /**
     * dummy implementation
     */
    void setParentItem(QGraphicsItem *currentParentItem);
    
    /**
     * dummy implementation
     */
    void setParentLayoutItem(QGraphicsLayoutItem *currenParentLayout);

    /**
     * dummy
     */
    void setVisible(bool how);
    
    /**
     * dummy
     */
    void setSelectionMode(HbAbstractItemView::SelectionMode mode);
    
    /**
     * returns mDummySelectionModel
     */
    DummySelectionModel* selectionModel();
    
signals:

    /**
     * selection changed
     */
    void selectionChanged (const QItemSelection&, const QItemSelection&);
    
public slots:
   
    /**
     * sets all items selected
     */
    void selectAll();
   
    /**
     * clears selection
     */
    void clearSelection();
    
	
public:
    
    /**
     * model
     */
    VideoSortFilterProxyModel &mModel;
    
    /**
     * dummy selection model for emitting selectionChanged
     */
    DummySelectionModel *mDummySelectionModel;
    
    /**
     * counter for startselectionmode calls
     */
    int mStartSelModeStartedCount;
    
    /**
     * counter for end selection mode calls
     */
    int mEndSelModelStartedCount;
    
    /**
     * visibility
     */
    bool mVisibility;
    
    /**
     * selection mode
     */
    HbAbstractItemView::SelectionMode mSelectionMode;
    
    /**
     * "parent" item
     */
    QGraphicsItem *mParentItem;
    
    /**
     * "parent" layout
     */
    QGraphicsLayoutItem *mParentLayout;
    
    /**
     * selection
     */
    QItemSelection mInitiallySelected;
};

#endif
