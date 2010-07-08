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
* Description:  XQSettingsManager class stub.
*
*/

#ifndef XQSETTINGSMANAGER_H
#define XQSETTINGSMANAGER_H

#include <QObject>
#include <QVariant>

class XQCentralRepositorySettingsKey : public QObject
{

public:
    
    XQCentralRepositorySettingsKey(int uid, int key)
    {
        Q_UNUSED(uid);
        Q_UNUSED(key);
    }
};

class XQSettingsManager
{

    enum Type
    {
        TypeVariant = 0,
        TypeInt,
        TypeDouble,
        TypeString,
        TypeByteArray
    };
    
public:
    QVariant readItemValue(XQCentralRepositorySettingsKey &key, int type)
    {
        Q_UNUSED(key);
        Q_UNUSED(type);
        return QVariant();
    }
    
};

#endif // XQSETTINGSMANAGER_H
