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
* Description:  Stub implementation for central repository
*
*/

// Version : %version: 2 %



//
//  INCLUDE FILES
//
#include "centralrepository.h"


CRepository* CRepository::NewLC( TUid /* aRepositoryUid */ )
{
    CRepository* self = new (ELeave) CRepository();
    
    CleanupStack::PushL( self );

    return self;
}


TInt CRepository::Get( TUint32 /* aKey */, TInt& aValue )
{
    aValue = 10;
    return KErrNone;        
}

CRepository::~CRepository()
{
}


//  EOF
