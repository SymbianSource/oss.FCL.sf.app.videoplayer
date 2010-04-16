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

class  HbAction : public QObject
{
    Q_OBJECT

public:
    
    /**
     * Constructor
     */
    explicit HbAction(QObject *parent = 0)
    {
        Q_UNUSED(parent);
        initializeCount++;
        mTriggeredCount = 0;
    }
    
    /**
     * Contructor.
     */       
    explicit HbAction(const QString &txt, QObject *parent = 0) : mTxt(txt) 
    {
        Q_UNUSED(parent);
        initializeCount++;
        mTriggeredCount = 0;
    }
    
    /**
     * Contructor.
     */ 
    explicit HbAction(Hb::SoftKeyAction actType, QObject *parent = 0) : mTxt("")
    {
        Q_UNUSED(parent);
        Q_UNUSED(actType);
        initializeCount++;
        mTriggeredCount = 0;
    }
    
    explicit HbAction(Hb::NavigationAction action, QObject* parent = 0) : mTxt("")
    {
        Q_UNUSED(parent);
        Q_UNUSED(action);
        initializeCount++;
        mTriggeredCount = 0;
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
     * Trigger stub
     */
    void trigger() { mTriggeredCount++; };
    
    /**
     * SetText stub
     */
    void setText(QString text) { mText = text; };

signals:
        
        /**
         * dummy triggered
         */
        void triggered();
        
public: // data
    
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
    
    /**
     * Count how many times this was triggered.
     */
    int mTriggeredCount;
    
    /**
     * Set text.
     */
    QString mText;    
};

#endif
