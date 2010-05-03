/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of HbEffect
*
*/

// Version : %version:   2 %



#ifndef HBEFFECT_H
#define HBEFFECT_H

#include <hbwidget.h>


class HbEffect : public HbWidget
{
    Q_OBJECT

    public:
        struct EffectStatus
        {
            Hb::EffectEvent reason;
        };

    public:
        static bool add( QGraphicsItem *item, const QString &filePath, const QString &effectEvent );
        static bool remove( QGraphicsItem *item, const QString &filePath, const QString &effectEvent );
        
        static bool start( QGraphicsItem *item, 
                           const QString &effectEvent,
                           QObject *receiver = 0,
                           const char *member = 0 );

        static bool cancel( QGraphicsItem *item );

        static bool effectRunning( QGraphicsItem *item, const QString &effectEvent = QString() );

    private:
        void started();
};

#endif /*HBEFFECT_H*/

