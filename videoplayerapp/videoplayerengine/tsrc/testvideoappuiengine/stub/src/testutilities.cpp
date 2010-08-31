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
* Description:  mpx utilities class stub for unit testing
*
*/

// Version : %version: 1 %

#include "testutilities.h"

void TestUtilities::DeleteCollectionUtility()
{
    delete iCollectionUtility;
    iCollectionUtility = NULL;

    CloseUtility();
}

void TestUtilities::DeletePlaybackUtility()
{
    delete iPlaybackUtility;
    iPlaybackUtility = NULL;

    CloseUtility();
}

void TestUtilities::CloseUtility()
{
    if ( iCollectionUtility == NULL && iPlaybackUtility == NULL )
    {
        delete this;

        //Dll::SetTls( NULL );
        UserSvr::DllSetTls(12345678, NULL);
    }
}
