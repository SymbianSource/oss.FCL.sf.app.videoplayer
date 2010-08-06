/**
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
* Description:
*
*/

#include "xqsettingsmanagerstub.h"

QVariant XQSettingsManager::mReadItemValueReturnValue = QVariant();

QVariant XQSettingsManager::readItemValue(XQCentralRepositorySettingsKey &key, int type)
{
    Q_UNUSED(key);
    Q_UNUSED(type);
    return mReadItemValueReturnValue;
}

XQSettingsManager::XQSettingsManager()
{
    
}

XQSettingsManager::~XQSettingsManager()
{
    
}
