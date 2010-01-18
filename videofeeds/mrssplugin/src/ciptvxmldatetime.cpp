/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Utility class and constants used by Iptv application.*
*/




// INCLUDE FILES
#include <e32base.h>

#include "ciptvxmldatetime.h"
#include "ciptvxmlconsts.h"
#include "IptvDebug.h"

_LIT( KIptvEmptyDes, "" );

// --------------------------------------------------------------------------
// Parse GMT time.
// --------------------------------------------------------------------------
//
void CIptvXmlDateTime::ParseGmtL( const HBufC* aBuffer, TTime& aTime )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::ParseGmtL" );

    TBool success( EFalse );
    TInt index( KErrNotFound );
    TInt gmtIndex( KErrNotFound );
    TInt gmtLength( KErrNotFound );

    if ( !aBuffer )
        {
        User::Leave( KErrArgument );
        }
    else
        {
        TInt bufLength = aBuffer->Length();
        index = aBuffer->Locate( ',' );
        gmtIndex = aBuffer->LocateReverse( ' ' );
        gmtLength = bufLength - gmtIndex;

        if ( ( index != KErrNotFound ) && ( gmtIndex != KErrNotFound ) )
            {
            success = ETrue;
            }
        }

    TInt parseStatus( KErrNone );

    if ( success )
        {
        // Time shifting from GMT
        TInt hourShift = 0;

        // Get zone
        HBufC* zone = HBufC::NewLC( gmtLength );
        TPtr16 zonePtr = zone->Des();
        zonePtr.Copy( aBuffer->Mid( gmtIndex, gmtLength ) );
        zonePtr.Trim();

        // Get hour shift from time zone.
        hourShift = GetHourShiftFromZone( zonePtr );
        IPTVLOGSTRING2_LOW_LEVEL(
            "RSS Plugin::CIptvXmlContentHandler::ParseGmtL Hour shift from time zone = %d",
            hourShift );

        // Do we increment or decrement from time zone.
        gmtLength = zonePtr.Length();
        TBuf<KIptvShiftNumberMaxCount > temp;
        TBool add = ETrue;
        for ( TInt i = 0; i < gmtLength; i++ )
            {
            //lint -e{961} Else block not needed, other characters ignored.
            if ( TChar( zonePtr[i] ).IsDigit() )
                {
                if ( KIptvShiftNumberMaxCount > temp.Length() )
                    {
                    temp.Append( zonePtr[i] );
                    }
                }
            else if ( zonePtr[i] == '+' )
                {
                add = ETrue;
                }
            else if ( zonePtr[i] == '-' )
                {
                add = EFalse;
                }
            }

        // Additional shift
        TInt additionalHourShift = 0;
        TInt additionalMinuteShift = 0;
        TInt shiftLength = temp.Length();
        TBuf<KIptvHourNumberCount> hs;
        TBuf<KIptvMinuteNumberCount> ms;

        //lint -e{961} Else block not needed, default is empty.
        if ( shiftLength == KIptvShiftNumberCountH )
            {
            hs.Append( temp[KIptvShiftHourHighIndex] );
            }
        else if ( shiftLength == KIptvShiftNumberCountHH )
            {
            hs.Append( temp[KIptvShiftHourHighIndex] );
            hs.Append( temp[KIptvShiftHourLowIndex] );
            }
        else if ( shiftLength == KIptvShiftNumberCountHHM )
            {
            hs.Append( temp[KIptvShiftHourHighIndex] );
            hs.Append( temp[KIptvShiftHourLowIndex] );
            ms.Append( temp[KIptvShiftMinuteHighIndex] );
            }
        else if ( shiftLength == KIptvShiftNumberCountHHMM )
            {
            hs.Append( temp[KIptvShiftHourHighIndex] );
            hs.Append( temp[KIptvShiftHourLowIndex] );
            ms.Append( temp[KIptvShiftMinuteHighIndex] );
            ms.Append( temp[KIptvShiftMinuteLowIndex] );
            }

        if ( hs.Compare( KIptvEmptyDes ) != 0 )
            {
            TLex hlex( hs );
            hlex.Val( additionalHourShift );
            }

        if ( ms.Compare( KIptvEmptyDes ) != 0 )
            {
            TLex mlex( ms );
            mlex.Val( additionalMinuteShift );
            }

        hourShift = hourShift + additionalHourShift;

        TTimeIntervalHours hourInterval( hourShift );
        TTimeIntervalMinutes minuteInterval( additionalMinuteShift );
        CleanupStack::PopAndDestroy( zone );
        zone = NULL;

        // Date
        TInt dateLength = gmtIndex - index;
        HBufC* date = HBufC::NewLC( dateLength );
        TPtr16 datePtr = date->Des();
        datePtr.Copy( aBuffer->Mid( index + 1, dateLength - 1 ) );
        datePtr.Trim();

        // Convert english strings "jan", "feb" etc. to local "tam", "hel" etc.
        // Needed because TTime always performs locale specific conversion.
        HBufC* fixedDate = ConvertMonthStringToLocaleL( datePtr );
        CleanupStack::PushL( fixedDate );

        // Parse the actual date string.
        parseStatus = aTime.Parse( *fixedDate );
        CleanupStack::PopAndDestroy( fixedDate );
        fixedDate = NULL;
        CleanupStack::PopAndDestroy( date );
        date = NULL;

        if ( parseStatus >= 0 )
            {

#ifdef _DEBUG
            const TInt KIptvDebugBufLen( 100 );
            TBuf<KIptvDebugBufLen> datePrint;
            _LIT( KIptvDatePrint, "%D%M%Y%/0%1%/1%2%/2%3%/3" );
            aTime.FormatL( datePrint, KIptvDatePrint );
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- Publish date : %S",
                &datePrint );

            TBuf<KIptvDebugBufLen> timePrint;
            _LIT( KIptvTimePrint, "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B" );
            aTime.FormatL( timePrint, KIptvTimePrint );
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- Publish time : %S",
                &timePrint );

            IPTVLOGSTRING_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- In universal time" );
#endif

            if ( add )
                {
                IPTVLOGSTRING2_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- decrement %d hours",
                    hourShift );
                IPTVLOGSTRING2_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- decrement %d minutes",
                    additionalMinuteShift );
                aTime -= hourInterval;
                aTime -= minuteInterval;
                }
            else
                {
                IPTVLOGSTRING2_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- increment %d hours",
                    hourShift );
                IPTVLOGSTRING2_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- increment %d minutes",
                    additionalMinuteShift );
                aTime += hourInterval;
                aTime += minuteInterval;
                }

#ifdef _DEBUG
            aTime.FormatL( datePrint, KIptvDatePrint );
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- Publish date : %S",
                &datePrint );

            aTime.FormatL( timePrint, KIptvTimePrint );
            IPTVLOGSTRING2_LOW_LEVEL(
                "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- Publish time : %S",
                &timePrint );
#endif

            }
        else
            {
            success = EFalse;
            }
        }

    // On case failure, return zero time.
    if ( !success )
        {
        TTime ztime( TInt64( 0 ) );
        aTime = ztime;

#ifdef _DEBUG

        switch ( parseStatus )
            {
            case KErrGeneral:
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- parse error KErrGeneral" );
                break;

            case KErrNotSupported:
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- parse error KErrNotSupported" );
                break;

            case KErrArgument:
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- parse error KErrArgument" );
                break;

            default:
                IPTVLOGSTRING_LOW_LEVEL(
                    "RSS Plugin::CIptvXmlContentHandler::ParseGmtL -- parse error default" );
                break;
            }
#endif

        }
    }

// --------------------------------------------------------------------------
// Parses hour shift value from time zone.
// --------------------------------------------------------------------------
//
TInt CIptvXmlDateTime::GetHourShiftFromZone(
    const TPtr16& aZone )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::GetHourShiftFromZone" );

    TInt hourShift( KIptvTimeZoneShift_None );

    if ( ( aZone.CompareF( KIptvTimeZone_GMT ) == 0 ) ||
         ( aZone.CompareF( KIptvTimeZone_UT ) == 0 ) ||
         ( aZone.CompareF( KIptvTimeZone_Z ) == 0 ) )
        {
        hourShift = KIptvTimeZoneShift_Z;
        }
    else if ( aZone.CompareF( KIptvTimeZone_A ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_A;
        }
    else if ( aZone.CompareF( KIptvTimeZone_B ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_B;
        }
    else if ( aZone.CompareF( KIptvTimeZone_C ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_C;
        }
    else if ( ( aZone.CompareF( KIptvTimeZone_EDT ) == 0 ) ||
              ( aZone.CompareF( KIptvTimeZone_D ) == 0 ) )
        {
        hourShift = KIptvTimeZoneShift_D;
        }
    else if ( ( aZone.CompareF( KIptvTimeZone_EST ) == 0 ) ||
              ( aZone.CompareF( KIptvTimeZone_CDT ) == 0 ) ||
              ( aZone.CompareF( KIptvTimeZone_E ) == 0 ) )
        {
        hourShift = KIptvTimeZoneShift_E;
        }
    else if ( ( aZone.CompareF( KIptvTimeZone_CST ) == 0 ) ||
              ( aZone.CompareF( KIptvTimeZone_MDT ) == 0 ) ||
              ( aZone.CompareF( KIptvTimeZone_F ) == 0 ) )
        {
        hourShift = KIptvTimeZoneShift_F;
        }
    else if ( ( aZone.CompareF( KIptvTimeZone_MST ) == 0 ) ||
              ( aZone.CompareF( KIptvTimeZone_PDT ) == 0 ) ||
              ( aZone.CompareF( KIptvTimeZone_G ) == 0 ) )
        {
        hourShift = KIptvTimeZoneShift_G;
        }
    else if ( ( aZone.CompareF( KIptvTimeZone_PST ) == 0 ) ||
              ( aZone.CompareF( KIptvTimeZone_H ) == 0 ) )
        {
        hourShift = KIptvTimeZoneShift_H;
        }
    else if ( aZone.CompareF( KIptvTimeZone_I ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_I;
        }
    else if ( aZone.CompareF( KIptvTimeZone_K ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_K;
        }
    else if ( aZone.CompareF( KIptvTimeZone_L ) == 0 )
        {
        hourShift = KIptvTimeZoneShiftL;
        }
    else if ( aZone.CompareF( KIptvTimeZone_M ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_M;
        }
    else if ( aZone.CompareF( KIptvTimeZone_N ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_N;
        }
    else if ( aZone.CompareF( KIptvTimeZone_O ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_O;
        }
    else if ( aZone.CompareF( KIptvTimeZone_P ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_P;
        }
    else if ( aZone.CompareF( KIptvTimeZone_Q ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_Q;
        }
    else if ( aZone.CompareF( KIptvTimeZone_R ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_R;
        }
    else if ( aZone.CompareF( KIptvTimeZone_S ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_S;
        }
    else if ( aZone.CompareF( KIptvTimeZone_T ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_T;
        }
    else if ( aZone.CompareF( KIptvTimeZone_U ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_U;
        }
    else if ( aZone.CompareF( KIptvTimeZone_V ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_V;
        }
    else if ( aZone.CompareF( KIptvTimeZone_W ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_W;
        }
    else if ( aZone.CompareF( KIptvTimeZone_X ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_X;
        }
    else if ( aZone.CompareF( KIptvTimeZone_Y ) == 0 )
        {
        hourShift = KIptvTimeZoneShift_Y;
        }
    else
        {
        hourShift = KIptvTimeZoneShift_None;
        }

    return hourShift;
    }

// --------------------------------------------------------------------------
// Converts english month in date string to locale specific month string.
// --------------------------------------------------------------------------
//
HBufC* CIptvXmlDateTime::ConvertMonthStringToLocaleL(
    const TDesC& aDateString )
    {
    IPTVLOGSTRING_LOW_LEVEL(
        "RSS Plugin::CIptvXmlContentHandler::ConvertMonthStringToLocaleL" );

    HBufC* newString =
        HBufC::NewLC( aDateString.Length() + KMaxMonthNameAbb );
    TInt monthIndex = KErrNotFound;
    TMonthNameAbb localMonth;

    *newString = aDateString;
    newString->Des().LowerCase();

    // Find the month string and init matching locale specific string.
    if ( KErrNotFound !=
         ( monthIndex = newString->Find( KIptvJan ) ) )
        {
        localMonth.Set( EJanuary );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvFeb ) ) )
        {
        localMonth.Set( EFebruary );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvMar ) ) )
        {
        localMonth.Set( EMarch );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvApr ) ) )
        {
        localMonth.Set( EApril );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvMay ) ) )
        {
        localMonth.Set( EMay );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvJun ) ) )
        {
        localMonth.Set( EJune );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvJul ) ) )
        {
        localMonth.Set( EJuly );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvAug ) ) )
        {
        localMonth.Set( EAugust );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvSep ) ) )
        {
        localMonth.Set( ESeptember );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvOct ) ) )
        {
        localMonth.Set( EOctober );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvNov ) ) )
        {
        localMonth.Set( ENovember );
        }
    else if ( KErrNotFound !=
              ( monthIndex = newString->Find( KIptvDec ) ) )
        {
        localMonth.Set( EDecember );
        }
    else
        {
        CleanupStack::Pop( newString );
        return newString;
        }

     // 3 = length of old month string.
    HBufC* temp =
        HBufC::NewLC( newString->Length() -
            ( monthIndex + KIptvMonthShortNameLen ) );
    *temp = newString->Mid( monthIndex + KIptvMonthShortNameLen );

    // Re-construct the string with new month name.
    newString->Des().SetLength( monthIndex );
    newString->Des().Append( localMonth );
    newString->Des().Append( *temp );

    CleanupStack::PopAndDestroy( temp );
    temp = NULL;
    CleanupStack::Pop( newString );
    return newString;
    }

