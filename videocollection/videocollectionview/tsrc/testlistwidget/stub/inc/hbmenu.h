/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  stub hbmenu
*
*/


#ifndef HBMENU_H
#define HBMENU_H


#include <QGraphicsWidget>
#include <QPointF>
#include <QMap>
#include <QList>
#include "hbaction.h"

class HbMenu : public QGraphicsWidget
{

public:

    /**
     * constructor
     */
    HbMenu(QGraphicsItem *parent = 0)
    {
        Q_UNUSED(parent);
    }

    /**
     * destructor
     */
    virtual ~HbMenu()
    {
        QMap<QString, HbAction*>::const_iterator iter = mActions.constBegin();
        while(iter != mActions.constEnd())
        {
            delete *iter;           
            iter++;
        }
        mActions.clear();
    };
    
    /**
     * dummy method
     */
    void setEnabled(bool value)
    {
        mEnabledSetted = value;
    }
    
    /**
     * dummy impl
     */
    void exec(QPointF point)
    {
        mExecPoint = point;
    }
    
    /**
     * dummy impl
     */
    HbAction* addAction(const QString &text, const QObject *receiver, const char *member)
    {
        Q_UNUSED(receiver);
        Q_UNUSED(member);
        
        if(mActions.contains(text))
        {
            delete mActions[text];
        }
        HbAction *action = new HbAction(text); 
        mActions[text] = action;
        return action;
    }
    
    /**
     * dummy impl
     */
    QList<HbAction*> actions()
    {
        if(mReturnEmptyActionList)
        {
            return QList<HbAction*>();
        }
        return mActions.values();
    }
            
    
    QMap<QString, HbAction*> mActions;
    
    /**
     * is enable setted
     */
    static bool mEnabledSetted;
    
    /**
     * should we return empty actionlist
     */
    static bool mReturnEmptyActionList;
    
    /**
     * point provided from exec
     */
    static QPointF mExecPoint;

};
#endif // HBMENU_H
