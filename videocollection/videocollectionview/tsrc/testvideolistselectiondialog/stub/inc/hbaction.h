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
* Description:  stub hbaction for testing video liste selection dialog
* 
*/

#ifndef HBACTION_H
#define HBACTION_H

#include <QObject>
#include <QString>
#include <qpointer.h>
#include "hbwidget.h"
#include "hbdialog.h"

class  HbAction : public HbWidget
{
    Q_OBJECT
public:
    /**
     * Contructor.
     */       
    HbAction( const QString &txt, QGraphicsItem *parent = 0) : mTxt(txt), HbWidget(0) {initializeCount++;};
        
    /**
     * destructor
     */
   virtual ~HbAction(){initializeCount--;}
    
    /**
     * sets mDisable;
     */
    void setDisabled(bool disable){mDisable = disable;}
      
    /**
     * dummy member 
     */
    bool mDisable;
    
    /**
     * text setted
     */
    QString mTxt;
    
    /**
     * counter to make sure alloc dealloc match
     */
    static int initializeCount;
    
signals:
    void triggered();
    
   
           
};

#endif
