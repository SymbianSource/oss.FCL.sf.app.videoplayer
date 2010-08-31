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
* Description:  HbMessageBox class declaration.
*
*/


#ifndef HBMENU_H
#define HBMENU_H

#include <hbwidget.h>

class QGraphicsItem;
class QPointF;
class HbAction;

class HbPopup
{
public:
    enum Placement
    {
        TopLeftCorner,
        TopRightCorner,
        BottomLeftCorner,
        BottomRightCorner,
        TopEdgeCenter,
        RightEdgeCenter,
        BottomEdgeCenter,
        LeftEdgeCenter,
        Center
    };

    enum DismissPolicy
    {
        NoDismiss   = 0,
        TapInside   = 1,
        TapOutside  = 2,
        TapAnywhere = TapInside | TapOutside
    };
};

class HbMenu: public HbWidget
{
    Q_OBJECT
    
public:
    HbMenu(QGraphicsItem *parent = 0);
    virtual ~HbMenu();
    
public:
    /** stubbed from HbMenu */
    HbAction* exec(const QPointF &pos, HbAction *action = 0);

    /** stubbed from HbMenu */
    HbAction* menuAction();

    /** stubbed from HbMenu */
    HbAction* activeAction();

    /** stubbed from HbMenu */
    void setActiveAction(HbAction *action);
    
    /** stubbed from HbMenu */
    void setEnabled(bool value);
    
    /** stubbed from HbMenu */
    HbAction* addAction(const QString &text,
        const QObject *receiver,
        const char *member);
    
    /** stubbed from HbMenu */
    QList<HbAction*> actions();
    
    /** stubbed from HbMenu */
    void setPreferredPos(const QPointF& position,
        HbPopup::Placement placement = HbPopup::TopLeftCorner);
    
    /** stubbed from HbMenu */
    void setDismissPolicy(HbPopup::DismissPolicy dismissPolicy);
    
public:
    /** dummy actions */
    QMap<QString, HbAction*> mActions;
    
    /** active action */
    HbAction* mActiveAction;
};

#endif // HBMENU_H
