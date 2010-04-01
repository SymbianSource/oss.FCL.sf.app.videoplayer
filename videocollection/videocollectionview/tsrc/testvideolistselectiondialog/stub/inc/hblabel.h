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
* Description:  stub hblabel for testing video list selection dialog
* 
*/

#ifndef HBLABEL_H
#define HBLABEL_H

#include <QGraphicsWidget>
#include <QString>
#include "hbwidget.h"

class HbLabel : public HbWidget
{
    Q_OBJECT
    
public:
   
    /**
    * Contructor.
    */
    HbLabel(QGraphicsItem *parent = 0) : HbWidget(parent), mPlainTxt(""){initializeCount++;}
    
    /**
     * Contructor.
     */ 
    HbLabel(const QString &txt,  QGraphicsItem *parent = 0) : HbWidget(parent), mPlainTxt(txt) {initializeCount++;}
      
    /**
     * Destructor.
     */   
    virtual ~HbLabel(){initializeCount--;}
    
    /**
     * counter to make sure alloc dealloc match
     */
    static int initializeCount;
    
    /**
     * set text
     */
    void setPlainText(QString txt);
    
    /**
     * sets alignment flag
     */
    void setAlignment(Qt::AlignmentFlag alignment);

    /**
     * clears text
     */
    void clear();
    
    /**
     * text setted 
     */
    QString mPlainTxt;   
    
    /**
     * alinment setted
     */
    Qt::AlignmentFlag mAlignment;
    
};

#endif
