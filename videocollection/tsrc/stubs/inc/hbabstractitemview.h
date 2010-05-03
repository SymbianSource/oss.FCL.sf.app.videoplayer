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
* Description:  stub hbabstractitemview
* 
*/

#ifndef HBABSTRACTITEMVIEW_H
#define HBABSTRACTITEMVIEW_H

#include "hbwidget.h"
#include <QGraphicsWidget>
#include <QItemSelectionModel>

class HbAbstractViewItem;

class HbAbstractItemView : public HbWidget
{
    Q_OBJECT
    
public:
    
    enum SelectionMode
    {
        InvalidSelection,
        NoSelection,
        SingleSelection,
        MultiSelection,
        ContiguousSelection
    };
    
    HbAbstractItemView(QGraphicsWidget *parent = 0) : HbWidget(parent){}
    
signals:

    void activated(const QModelIndex &index);
    
    void longPressed(HbAbstractViewItem *item, const QPointF &point);

};

#endif
