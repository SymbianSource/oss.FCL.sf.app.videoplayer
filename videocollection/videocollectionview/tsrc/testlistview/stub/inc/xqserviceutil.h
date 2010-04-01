/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef XQSERVICEUTIL_H
#define XQSERVICEUTIL_H

#include <QObject>
#include <stub/inc/xqserviceutilxtra.h>

class XQServiceUtilXtra;

namespace XQServiceUtil
{
    static void toBackground(bool value){};
    static bool isEmbedded(){return false;};
    static bool isService()
    {
    	bool service;
    	XQServiceUtilXtra* ptr = XQServiceUtilXtra::instance();
    	service = ptr->currentService();
    	ptr->decreaseReferenceCount();
    	return service;
    };
}



#endif
