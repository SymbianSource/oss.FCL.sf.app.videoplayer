/**
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
* Description:
*
*/

#include "hbaction.h"
#include "xqserviceutilxtra.h"

void XQServiceUtil::toBackground(bool value)
{
    Q_UNUSED(value);
}

bool XQServiceUtil::isEmbedded()
{
    return false;
}

bool XQServiceUtil::isService()
{
    bool service = false;
    XQServiceUtilXtra* ptr = XQServiceUtilXtra::instance();
    service = ptr->currentService();
    ptr->decreaseReferenceCount();
    return service;
}

// End of file
