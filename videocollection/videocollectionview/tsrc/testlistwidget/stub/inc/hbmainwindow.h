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
* Description:  stub hbmainwindow
* 
*/

#ifndef HBMAINWINDOW_H
#define HBMAINWINDOW_H

#include <QObject>
#include "hbwidget.h"
#include "hbaction.h"

class HbMainWindow : public QObject
{
    Q_OBJECT

public:

    /**
     * constructor
     */
    HbMainWindow(QWidget *parent = 0, Hb::WindowFlags windowFlags = Hb::WindowFlagNone) :
    mSoftKeyAction(0)
    {
        Q_UNUSED(parent);
        Q_UNUSED(windowFlags);
    }
    
    /**
     * destructor
     */
    ~HbMainWindow(){};
    
    /**
     * dummy
     */
    void addSoftKeyAction(Hb::SoftKeyTypes type, HbAction *action)
    {
        Q_UNUSED(type);
        mSoftKeyAction = action;
    }
    
    void removeSoftKeyAction(Hb::SoftKeyTypes type, HbAction *action)
    {
        Q_UNUSED(type);
        if(mSoftKeyAction == action)
        {
            mSoftKeyAction = 0;
        }    
    }
    
    /**
     * returns mSoftKeyAction
     */
    HbAction* softKeyAction(Hb::SoftKeyTypes type)
    {
        Q_UNUSED(type);
        return mSoftKeyAction;
    }

    /**
     * value to return from softKeyAction
     */
    HbAction *mSoftKeyAction; 
};
#endif
