/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef CIPTV3XMLDATETIME_H
#define CIPTV3XMLDATETIME_H

#include <e32std.h>

NONSHARABLE_CLASS( CIptvXmlDateTime ): public CBase
    {

public:

    /**
     * Parse GMT time.
     *
     * @param aBuffer       Time string to be parsed.
     * @param aTime         Parsed time.
     */
    static void ParseGmtL( const HBufC* aBuffer, TTime& aTime);

    /**
     * Parses hour shift value from time zone.
     *
     * @param aZone         Time zone string to be parsed.
     * @return              Hour shift.
     */
    static TInt GetHourShiftFromZone( const TPtr16& aZone );

    /**
     * Converts english month in date string to locale specific month string.
     *
     * @param aDateString   Original month string.
     * @return              Converted month string.
     */
    static HBufC* ConvertMonthStringToLocaleL( const TDesC& aDateString );

    };

#endif // CIPTV3XMLDATETIME_H
