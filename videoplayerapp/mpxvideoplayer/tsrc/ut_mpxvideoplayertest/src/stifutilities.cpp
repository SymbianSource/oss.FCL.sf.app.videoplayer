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
* Description:  mpx utilities class stub for STIF testing
*
*/

// Version : %version: ou1cpsw#2 %

#include "stifutilities.h"

void StifUtilities::DeleteCollectionUtility()
{
    delete iCollectionUtility;
    iCollectionUtility = NULL;

    CloseUtility();
}

void StifUtilities::DeleteViewUtility()
{
    delete iViewUtility;
    iViewUtility = NULL;

    CloseUtility();
}

void StifUtilities::DeletePlaybackUtility()
{
    delete iPlaybackUtility;
    iPlaybackUtility = NULL;

    CloseUtility();
}

void StifUtilities::CloseUtility()
{
    if ( iCollectionUtility == NULL && iPlaybackUtility == NULL && iViewUtility == NULL )
    {
        delete this;

        Dll::SetTls( NULL );
    }
}
