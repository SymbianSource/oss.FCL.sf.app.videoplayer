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
* Description:  stub HbDialog
* 
*/
#include <QGraphicsWidget>

#include "hbaction.h"
#include "hblabel.h"
#include "HbDialog.h"

HbDialog *HbDialog::currentInstance = 0;

bool HbDialog::execReturnPrimary = true;

bool HbDialog::primaryReturnNull = false;

bool HbDialog::secondaryReturnNull = false;


HbDialog::HbDialog(QGraphicsItem *parent) :
HbWidget(parent),
mHeadingWidget(0),
mContentWidget(0),
mPrimaryAction(0),
mSecondaryAction(0),
mTimeout(NoTimeout),
mDismissPolicy(NoDismiss)

{
    mMinSize.setHeight(0);
    mMinSize.setWidth(0);
    mMaxSize.setHeight(0);
    mMaxSize.setWidth(0); 
    currentInstance = this;
}
    
HbDialog::~HbDialog()
{
    delete mHeadingWidget;
    delete mPrimaryAction;
    delete mSecondaryAction;
    delete mContentWidget;
    currentInstance = 0;
    mActions.clear();
}
    
void HbDialog::setDismissPolicy(HbDialog::DismissPolicy dismissPolicy)
{
    mDismissPolicy = dismissPolicy;
}
    
void HbDialog::setTimeout(HbDialog::DefaultTimeout timeout)
{
    mTimeout = timeout;
}
    
void HbDialog::setHeadingWidget(QGraphicsWidget *headingWidget)
{
    mHeadingWidget = headingWidget;
}

void HbDialog::setContentWidget(QGraphicsWidget *contentWidget)
{
    mContentWidget = contentWidget;
}

void HbDialog::setPrimaryAction(HbAction *action)
{
    mPrimaryAction = action;
}

HbAction* HbDialog::primaryAction()
{
    if(primaryReturnNull)
    {
        return 0;
    }
    return mPrimaryAction;
}

void HbDialog::setSecondaryAction(HbAction *action)
{
    mSecondaryAction = action;
}

HbAction* HbDialog::secondaryAction()
{
    if(secondaryReturnNull)
    {
        return 0;
    }
    return mSecondaryAction;
}

void HbDialog::addAction(HbAction *action)
{
    mActions.append(action);
}
           
void HbDialog::setMinimumSize(QSize size)
{
    mMinSize.setHeight(size.height());
    mMinSize.setWidth(size.width());
}
    
void HbDialog::setMaximumSize(QSize size)
{
    mMaxSize.setHeight(size.height());
    mMaxSize.setWidth(size.width());
}

HbAction* HbDialog::exec()
{   
    if(execReturnPrimary)
    {
        return mPrimaryAction;
    }
    return mSecondaryAction;
}
