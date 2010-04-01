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
* Description:  stub hbaction 
* 
*/

#ifndef HBACTION_H
#define HBACTION_H

#include <QObject>
#include <QString>
#include <qpointer.h>
#include "hbwidget.h"


class  HbAction : public HbWidget
{
    Q_OBJECT
public:
    /**
     * Contructor.
     */       
    HbAction( const QString &txt, QGraphicsItem *parent = 0) : mTxt(txt), HbWidget(0) 
    {
        Q_UNUSED(parent);
        initializeCount++;
    };
    
    /**
     * Contructor.
     */ 
    HbAction( Hb::ActionTypes actType, QGraphicsItem *parent = 0) : mTxt(""), HbWidget(0) 
    {
        Q_UNUSED(parent);
        Q_UNUSED(actType);
        initializeCount++;
    };
    
    /**
     * destructor
     */
   virtual ~HbAction(){initializeCount--;}
    
    /**
     * sets mDisable;
     */
    void setDisabled(bool disable){mDisable = disable;}
    
    /**
     * sets mVisible;
     */
    void setVisible(bool visible){mVisible = visible;}
      
    /**
     * dummy member 
     */
    bool mDisable;
    
    /**
     * dummy member
     */
    bool mVisible;
    
    /**
     * text setted
     */
    QString mTxt;
    
    /**
     * counter to make sure alloc dealloc match
     */
    static int initializeCount;
    
signals:
    
    /**
     * dummy triggered
     */
    void triggered();
        
};

#endif
