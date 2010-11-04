/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:      Panic literal and codes for component*
*/




#ifndef VCXHGMYVIDEOSPANICS_H
#define VCXHGMYVIDEOSPANICS_H

/**
 * Panic literal for all MPX My Videos UI panics.
 */
_LIT( KVcxHgMyVideosPanic, "MPX My Videos UI" );

/**
 * Panic codes for MPX My Videos UI panics.
 */
enum KVcxHgMyVideosPanicCodes
    {
    EVcxHgMyVideosPanicLogicalCategory = 800,
    EVcxHgMyVideosPanicLogicalObserver,
    EVcxHgMyVideosPanicLogicalVideo,
    EVcxHgMyVideosPanicLogicalDownload,
    EVcxHgMyVideosPanicLogicalSearch
    };

#endif // VCXHGMYVIDEOSPANICS_H
