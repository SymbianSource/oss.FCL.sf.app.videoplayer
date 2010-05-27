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
* Description:  stub HbMessageBox class declaration.
*
*/


#ifndef HBMESSAGEBOX_H
#define HBMESSAGEBOX_H

#include <QObject>
#include <QString>
#include <QGraphicsItem>

#include <hbaction.h>

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
    
    void show();
    
    /**
     * Create new HbMessageBox and call emitDialogFinished after this one to finish the sequence.
     */
    void open( QObject* receiver = 0, const char* member = 0 );
    
    void emitDialogFinished( QObject* receiver, const char* member, int actionNum );
    
    void setAttribute(int attribute);
    
    /**
     * saves provided text to mLatestTxt
     */
    static void warning(QString text)
    {
        mLatestTxt = text;
    }
    
    /**
     * saves provided text to mLatestTxt
     */
    static void information(QString text)
    {
        mLatestTxt = text;
    }
    
    QList<HbAction*> actions() const
    {
        return mActions;
    }
    
    /**
     * saved text
     */
    static QString mLatestTxt;
    
    static int mType;
    static int mAttribute;
    static int mOpenCallCount;
    static int mShowCallCount;
    
    QList<HbAction*> mActions;
    
signals:

    void finished(HbAction *action);
};

#endif // HBMESSAGEBOX_H
