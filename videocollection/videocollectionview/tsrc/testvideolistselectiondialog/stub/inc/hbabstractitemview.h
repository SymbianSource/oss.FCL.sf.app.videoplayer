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

#include <QGraphicsWidget>
#include <QItemSelectionModel>
#include "hbwidget.h"

class HbAbstractItemView : public HbWidget
{
public:
    
    enum SelectionMode
    {
        NoSelection,
        MultiSelection
    };
    
    HbAbstractItemView(QGraphicsWidget *parent = 0) : HbWidget(parent){}
    
};

#endif
