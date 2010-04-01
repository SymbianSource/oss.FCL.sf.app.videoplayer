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
* Description:  stub hbwidget
* 
*/

#ifndef HBWIDGET_H
#define HBWIDGET_H
#include <QGraphicsWidget>
#include <hbnamespace.h>

 /**
 * counter to make sure alloc dealloc match
 */


class HbWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    HbWidget(QGraphicsItem *parent = 0) : QGraphicsWidget(parent){initializeCount++;}
    
    virtual ~HbWidget(){initializeCount--;}
    
    void clearActions() {}
    
    static int initializeCount;
};

#endif
