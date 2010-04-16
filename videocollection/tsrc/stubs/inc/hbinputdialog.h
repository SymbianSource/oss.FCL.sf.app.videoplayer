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
#include <qgraphicsitem.h>

class HbInputDialog : public HbWidget
{
    Q_OBJECT

public:
    HbInputDialog(QGraphicsItem *parent= 0) : HbWidget(parent) {}
    ~HbInputDialog() {}

    /**
     * Returns string in parameter text if mGetTextReturnValue is null QString.
     */
    static QString getText(const QString &label,const QString &text = QString(),
            bool *ok = 0, QGraphicsScene *scene = 0, QGraphicsItem *parent = 0);
    
    static bool mGetTextFails;
    
    static QString mGetTextReturnValue;
    
    static int mGetTextCallCount;
};

#endif //HBINPUT_DIALOG_H

