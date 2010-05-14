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

#include <qobject.h>
#include <qstring.h>
#include "hbaction.h"

class QGraphicsWidget;
class QGraphicsScene;
class QGraphicsItem;

class HbMessageBox : public QObject
{   
    Q_OBJECT
    
public:
    enum MessageBoxType {
        MessageTypeInformation,
        MessageTypeQuestion,
        MessageTypeWarning
    };
    
    HbMessageBox(MessageBoxType type = MessageTypeInformation, QGraphicsItem *parent = 0);
    HbMessageBox(const QString &text, MessageBoxType type = MessageTypeInformation, QGraphicsItem *parent = 0);
    ~HbMessageBox();
    
    /**
    * Create new HbMessageBox and call emitDialogFinished after this one to finish the sequence.
     */
    void open( QObject* receiver = 0, const char* member = 0 );
    
    void emitDialogFinished( QObject* receiver, const char* member, int actionNum );

    void show();
    
    void setAttribute(int attribute);
    
    /** stubbed from HbMessgeBox */
/*    static void question(const QString &questionText,
                            QObject *receiver,
                            const char *member,
                            const QString &primaryButtonText = tr("Yes"),
                            const QString &secondaryButtonText = tr("No"),
                            QGraphicsWidget *headWidget = 0,
                            QGraphicsScene *scene = 0,
                            QGraphicsItem *parent = 0 );*/

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
    
    QList<HbAction*> actions() const
    {
        return mActions;
    }

signals:

    void finished(HbAction *action);
    
public:
    
    QList<HbAction*> mActions;
    
};

#endif // HBMESSAGEBOX_H
