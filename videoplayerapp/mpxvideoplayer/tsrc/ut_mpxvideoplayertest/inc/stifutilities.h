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

#ifndef STIFUTILITIES_H_
#define STIFUTILITIES_H_

#include "mpxcollectionutilityimp_stub.h"
#include "mpxcollectionutilityimp_stub.h"
#include "mpxplaybackutilityimp_stub.h"
#include "mpxviewutilityimp_stub.h"

class StifUtilities : public CBase
{
    public:

        void DeleteCollectionUtility();
        void DeleteViewUtility();
        void DeletePlaybackUtility();

        void CloseUtility();

        CMPXCollectionUtility* iCollectionUtility;
        CMPXPlaybackUtility*   iPlaybackUtility;
        CMPXViewUtility*       iViewUtility;
};
#endif /*STIFUTILITIES_H_*/
