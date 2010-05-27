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
* Description:  hbinputdialog stub
*
*/


#ifndef HBINPUT_DIALOG_H
#define HBINPUT_DIALOG_H

#include "hbwidget.h"
#include "hbaction.h"
#include <qvariant.h>
#include <qgraphicsitem.h>

class HbInputDialog : public HbWidget
{
    Q_OBJECT

public:
    HbInputDialog(QGraphicsItem *parent= 0);
    ~HbInputDialog();

    /**
     * Returns string in parameter text if mGetTextReturnValue is null QString.
     */
    /*
    static QString getText(const QString &label,const QString &text = QString(),
            bool *ok = 0, QGraphicsScene *scene = 0, QGraphicsItem *parent = 0);*/
    
    /**
     * Create new HbInputDialog after call to this and call emitDialogFinished to finish the sequence.
     */
    void getText(const QString &heading
            ,QObject *receiver
            ,const char *member
            ,const QString &text=QString()
            ,QGraphicsScene *scene=0
            ,QGraphicsItem *parent=0);
    
    void emitDialogFinished( QObject* receiver, const char* member, int actionNum );
    
    void setAttribute(int attribute);
    
    QList<HbAction*> actions() const
    {
        return mActions;
    }
    
    QVariant value(int row = 0) { Q_UNUSED(row); mValueCallCount++; return mValueReturnValue; }
    
signals:

    void finished(HbAction *action);
    
public:

    QList<HbAction*> mActions;
    
    static QString mLastHeading;
    static QString mLastText;
    static int mGetTextCallCount;
    static int mAttribute;
    
    static QVariant mValueReturnValue;
    static int mValueCallCount;
};

#endif //HBINPUT_DIALOG_H

