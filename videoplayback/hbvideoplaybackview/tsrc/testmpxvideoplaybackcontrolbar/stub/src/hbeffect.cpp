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

// Version : %version:   3 %



#include "mpxvideo_debug.h"
#include "hbeffect.h"


// -------------------------------------------------------------------------------------------------
// HbEffect::add
// -------------------------------------------------------------------------------------------------
//
bool HbEffect::add( QGraphicsItem *item, const QString &filePath, const QString &effectEvent )
{
    MPX_DEBUG(_L("HbEffect::add") );

    Q_UNUSED( item );
    Q_UNUSED( filePath );
    Q_UNUSED( effectEvent );

    return true;
}

// -------------------------------------------------------------------------------------------------
// HbEffect::remove
// -------------------------------------------------------------------------------------------------
//
bool HbEffect::remove( QGraphicsItem *item, const QString &filePath, const QString &effectEvent )
{
    MPX_DEBUG(_L("HbEffect::remove") );

    Q_UNUSED( item );
    Q_UNUSED( filePath );
    Q_UNUSED( effectEvent );

    return true;
}

// -------------------------------------------------------------------------------------------------
// HbEffect::start
// -------------------------------------------------------------------------------------------------
//
bool HbEffect::start( 
    QGraphicsItem *item, const QString &effectEvent, QObject *receiver, const char *member )
{
    MPX_DEBUG(_L("HbEffect::start") );

    Q_UNUSED( item );
    Q_UNUSED( effectEvent );

    HbEffect::EffectStatus status;
    status.reason = Hb::EffectFinished;

    QMetaObject::invokeMethod(
        receiver,
        member,
        Qt::AutoConnection, 
        QGenericReturnArgument(),
        Q_ARG( HbEffect::EffectStatus, status ) );

    return true;
}

// -------------------------------------------------------------------------------------------------
// HbEffect::cancel
// -------------------------------------------------------------------------------------------------
//
bool HbEffect::cancel( QGraphicsItem *item )
{
    MPX_DEBUG(_L("HbEffect::cancel"));

    Q_UNUSED( item );

    return true;
}

// -------------------------------------------------------------------------------------------------
// HbEffect::effectRunning
// -------------------------------------------------------------------------------------------------
//
bool HbEffect::effectRunning( QGraphicsItem *item, const QString &effectEvent )
{
    MPX_DEBUG(_L("HbEffect::effectRunning"));

    Q_UNUSED( item );
    Q_UNUSED( effectEvent );

    return false;
}

// End of file
