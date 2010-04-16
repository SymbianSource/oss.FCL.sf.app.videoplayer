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


#ifndef HBMESSAGEBOX_H
#define HBMESSAGEBOX_H

#include <qstring.h>

class QGraphicsWidget;
class QGraphicsScene;
class QGraphicsItem;

class HbMessageBox
{   
public:
    /** stubbed from HbMessgeBox */
    static bool question(const QString &questionText,
        const QString &primaryButtonText = QString(),
        const QString &secondaryButtonText = QString(),
        QGraphicsWidget *headWidget = 0,
        QGraphicsScene *scene = 0,
        QGraphicsItem *parent = 0);

    /** stubbed from HbMessgeBox */
    static void information(const QString &informationText,
        QGraphicsWidget *headWidget = 0,
        QGraphicsScene *scene = 0,
        QGraphicsItem *parent = 0);

    /** stubbed from HbMessgeBox */
    static void warning(const QString &warningText,
        QGraphicsWidget *headWidget = 0,
        QGraphicsScene *scene = 0,
        QGraphicsItem *parent = 0 );
};



#endif // HBMESSAGEBOX_H
