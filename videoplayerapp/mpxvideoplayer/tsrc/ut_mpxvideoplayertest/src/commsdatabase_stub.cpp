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
* Description:  comms database stub for STIF testing
*
*/

// Version : %version: ou1cpsw#3 %


#include <commdb.h>

#include "mpxvideo_debug.h"

CCommsDatabase*
CCommsDatabase::NewL( TCommDbDatabaseType /*aDbType*/ )
{
    MPX_ENTER_EXIT(_L("CCommsDatabase::NewL()"));

    CCommsDatabase* self = new (ELeave) CCommsDatabase();
    return self;
}


CCommsDatabaseBase::CCommsDatabaseBase()
{
}

CCommsDatabaseBase::~CCommsDatabaseBase()
{
}

CCommsDatabase::CCommsDatabase()
{
    MPX_ENTER_EXIT(_L("CCommsDatabase::CCommsDatabase()"));
}

CCommsDatabase::~CCommsDatabase()
{
    MPX_ENTER_EXIT(_L("CCommsDatabase::~CCommsDatabase()"));
}

void CCommsDatabase::CreateDatabaseL()
{
}
