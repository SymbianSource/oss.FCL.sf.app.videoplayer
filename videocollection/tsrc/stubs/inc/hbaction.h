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
#include <qaction.h>
#include "hbnamespace.h"
#include "hbicon.h"
#include "hbmenu.h"

class  HbAction : public QAction
{
    Q_OBJECT
public:
    
    /**
     * Constructor
     */
    explicit HbAction(QObject *parent = 0) : QAction(parent)
    {
        Q_UNUSED(parent);
        initializeCount++;
    }
    
    /**
     * Contructor.
     */       
    explicit HbAction(const QString &txt, QObject *parent = 0) : mTxt(txt), QAction(parent) 
    {
        Q_UNUSED(parent);
        initializeCount++;
    }
    
    /**
     * Contructor.
     */ 
    explicit HbAction(Hb::SoftKeyAction actType, QObject *parent = 0) : mTxt(""), QAction(parent) 
    {
        Q_UNUSED(parent);
        Q_UNUSED(actType);
        initializeCount++;
    }
    
    explicit HbAction(Hb::NavigationAction action, QObject* parent = 0) : mTxt(""), QAction(parent)
    {
        Q_UNUSED(parent);
        Q_UNUSED(action);
        initializeCount++;
    }
    
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
     * not stubbed yet.
     */
    void setIcon(const HbIcon &icon) { Q_UNUSED(icon); }
    
    /**
     * not stubbed yet.
     */
    HbIcon icon() const { return HbIcon(); }
    
    /**
     * not stubbed yet.
     */
    HbMenu *menu() const { return 0; }
    
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
