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
* Description:  stub hbdialog
* 
*/

#ifndef HBDIALOG_H
#define HBDIALOG_H

#include <QObject>
#include <qsize.h>
#include "hbwidget.h"
class QGraphicsItem;
class QGraphicsWidget;
class HbAction;

class HbDialog :  public HbWidget
{
   
public:
    
    enum DefaultTimeout
    {
       NoTimeout,
       ConfirmationNoteTimeout,
       StandardTimeout,
       ContextMenuTimeout,
    };

    enum DismissPolicy
    {
       NoDismiss   = 0,
       TapInside   = 1,
       TapOutside  = 2,
       TapAnywhere = TapInside | TapOutside
    };
    
    /**
     * contructor
     */
    HbDialog(QGraphicsItem *parent=0);
    
    /**
     * destructor
     */
    ~HbDialog();
    
    /**
     * sets mDismissPolicy
     */
    void setDismissPolicy(HbDialog::DismissPolicy dismissPolicy);
    
    /**
     * sets mTimeout
     */
    void setTimeout(HbDialog::DefaultTimeout timeout);
    
    /**
     * sets mHeadingWidget; 
     */
    void setHeadingWidget(QGraphicsWidget *headingWidget);

    /**
     * sets mContentWidget
     */
    void setContentWidget(QGraphicsWidget *contentWidget);

    /**
     * sets mPrimaryAction
     */
    void setPrimaryAction(HbAction *action);
    
    /**
     * add action into actions -list
     */
    void addAction(HbAction *action);
    
    /**
     * gets mPrimaryAction;
     */
    HbAction* primaryAction();

    /**
     * sets mSecondaryAction
     */
    void setSecondaryAction(HbAction *action);
    
    /**
     * gets mSecondaryAction;
     */
    HbAction* secondaryAction();
           
    /**
     * sets mMinSize
     */
    void setMinimumSize(QSize size);
    
    /**
     * sets mMaxSize
     */
    void setMaximumSize(QSize size); 
    
    /**
     * returns mPrimaryAction if execReturnPrimary is true.
     */
    HbAction* exec();
    
    /**
     * stubbed member
     */
    QGraphicsWidget *mHeadingWidget;
    
    /**
     * stubbed member
     */
    QGraphicsWidget *mContentWidget;
    
    /**
     * stubbed member
     */
    HbAction *mPrimaryAction;
    
    /**
     * stubbed member
     */
    HbAction *mSecondaryAction;
    
    /**
     * stubbed member
     */
    QSize mMinSize;
    
    /**
     * stubbed member
     */
    QSize mMaxSize;
    
    /**
     * stubbed member
     */
    HbDialog::DefaultTimeout mTimeout;
    
    /**
     * stubbed member
     */
    HbDialog::DismissPolicy mDismissPolicy;
    
    /**
     * actions other tham primary or secondary
     */
    QList<HbAction*> mActions;
       
    /**
     * address of current instance zeroed during destcructor
     * this is for making sure object is removed correctly
     */
    static HbDialog *currentInstance;
        
    /**
     * if true, exec returns mPrimaryAction
     */
    static bool execReturnPrimary;
    
    /**
     * if true, primaryAction() -method returns null
     */
    static bool primaryReturnNull;   
    
    /**
     * if true, secondaryAction() -method returns null
     */
    static bool secondaryReturnNull;
    
};

#endif
