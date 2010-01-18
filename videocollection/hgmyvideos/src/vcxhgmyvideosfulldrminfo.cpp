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
* Description:   DRM related function definitions for hgmyvideos*
*/




// INCLUDE FILES
#include <StringLoader.h>
#include <DRMCommon.h>
#include <DRMHelper.h>

#include "IptvDebug.h"
#include <vcxhgmyvideos.rsg>
#include "vcxhgmyvideosfulldrminfo.h"

// CONSTANTS
_LIT( KVcxHgExtensionWmWithDot,".wm" );
_LIT( KVcxHgExtensionWmvWithDot,".wmv" );
_LIT( KVcxHgExtensionAsfWithDot,".asf" );
_LIT( KVcxHgSeparator, ": " ); // IPTV: Changed from "\t" to ": "
_LIT( KVcxHgDateFormat1, "%1" );
_LIT( KVcxHgDateFormat2, "%2" );
_LIT( KVcxHgDateFormat3, "%3" );
_LIT( KVcxHgTimeFormatBefore, " %-B %J:%T" );
_LIT( KVcxHgTimeFormatAfter, " %J:%T %+B" );
_LIT( KVcxHgEmptyChar, " " );
const TUint KVcxHgSecondSeparator = 1;
const TUint KVcxHgThirdSeparator  = 2;

// ============================ LOCAL FUNCTIONS ===============================

namespace // unnamed namespace for local definitions
    {
    // ------------------------------------------------------------------------
    // AppendLabelAndDataToArrayL
    // ------------------------------------------------------------------------
    //
    void AppendLabelAndDataToArrayL( CDesCArray& aArray,
                                     const TDesC& aLabel,
                                     const TDesC& aData )
        {
        HBufC* dataStr = HBufC::NewLC( aLabel.Length() +
                                       KVcxHgSeparator().Length() +
                                       aData.Length() );

        TPtr dataPtr( dataStr->Des() );
        dataPtr.Append( aLabel );
        dataPtr.Append( KVcxHgSeparator );
        dataPtr.Append( aData );
        aArray.AppendL( dataPtr );

        IPTVLOGSTRING2_LOW_LEVEL("Video Storage UI ## CVcxHgMyVideosFullDrmInfo/AppendLabelAndDataToArrayL - %S", dataStr);

        CleanupStack::PopAndDestroy( dataStr );
        }

    // ------------------------------------------------------------------------
    // DateTimeStringLC
    // ------------------------------------------------------------------------
    //
    HBufC* DateTimeStringLC( const TTime& aTime )
        {
        TBuf<20> dateStr;
        TBuf<20> timeStr;
        TBuf<20> dateStrFormat;

        // Localized date separator form
        TLocale local;
        dateStrFormat.Append( KVcxHgDateFormat1 );
        dateStrFormat.Append( local.DateSeparator( KVcxHgSecondSeparator ) );
        dateStrFormat.Append( KVcxHgDateFormat2 );
        dateStrFormat.Append( local.DateSeparator( KVcxHgThirdSeparator ) );
        dateStrFormat.Append( KVcxHgDateFormat3 );
        aTime.FormatL( dateStr, dateStrFormat );

        if ( local.AmPmSymbolPosition() == ELocaleBefore )
            {
            aTime.FormatL( timeStr, KVcxHgTimeFormatBefore );
            }
        else
            {
            aTime.FormatL( timeStr, KVcxHgTimeFormatAfter );
            }

        HBufC* buf = HBufC::NewLC( dateStr.Length() +
                                   KVcxHgEmptyChar().Length() +
                                   timeStr.Length() );
        TPtr ptrBuffer( buf->Des() );
        ptrBuffer.Append( dateStr );
        ptrBuffer.Append( KVcxHgEmptyChar );
        ptrBuffer.Append( timeStr );

        return buf;
        }

    // ------------------------------------------------------------------------
    // DateTimeL
    // ------------------------------------------------------------------------
    //
    void DateTimeL( CDesCArray& aArray,
                    const TTime& aTime,
                    TInt aResourceId,
                    const TDesC& aType )
        {
        HBufC* label = StringLoader::LoadLC( aResourceId, aType );
        HBufC* dateTime = DateTimeStringLC( aTime );

        AppendLabelAndDataToArrayL( aArray, *label, *dateTime );

        CleanupStack::PopAndDestroy( dateTime );
        CleanupStack::PopAndDestroy( label );
        }

    // ------------------------------------------------------------------------
    // FillCounterInfoL
    // ------------------------------------------------------------------------
    //
    void FillCounterInfoL( CDesCArray& aArray,
                           CDRMHelperRightsConstraints& aRights,
                           const TDesC& aType )
        {
        TUint32 count( 0 );
        TUint32 timedCount( 0 );
        TUint32 ignore1( 0 );
        TUint32 ignore2( 0 );
        TTimeIntervalSeconds ignore3( 0 );
        TInt err( KErrNone );

        TRAPD( errCount, aRights.GetCountersL( count, ignore1 ) );
        TRAPD( errTimedCount, aRights.GetTimedCountL( timedCount, ignore2,
            ignore3 ) );

        if ( errCount == KErrNone && errTimedCount == KErrNone )
            {
            // Both counts present, use minimum
            count = Min( count, timedCount );
            err = KErrNone;
            }
        else if ( errCount == KErrNone )
            {
            // Use count
            err = KErrNone;
            }
        else if ( errTimedCount == KErrNone )
            {
            // Use timed count
            count = timedCount;
            err = KErrNone;
            }
        else
            {
            // Neither regular nor timed count constraint
            // present, return error
            err = KErrNotFound;
            }

        if ( err == KErrNone )
            {
            // "Times left (%U)"
            HBufC* label =
                    StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_UTL_X, aType );

            HBufC* data = NULL;

            if ( count == 1 )
                {
                // "1 count"
                data = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_1_COUNT );
                }
            else
                {
                // "%N counts"
                data = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_N_COUNTS, count );
                }

            AppendLabelAndDataToArrayL( aArray, *label, *data );

            CleanupStack::PopAndDestroy( data );
            CleanupStack::PopAndDestroy( label );
            }
        }

    // ---------------------------------------------------------
    // SplitTime
    // ---------------------------------------------------------
    //
    void SplitTime( const TTimeIntervalSeconds& aInterval,
            TInt& aIntYrs, TInt& aIntMon, TInt& aIntDay,
            TInt& aIntHrs, TInt& aIntMin, TInt& aIntSec )
        {
        TInt temp( 0 );
        TInt i( 0 );
        const TInt secsInMin( 60 );
        const TInt secsInHour( secsInMin * 60 );
        const TInt secsInDay( secsInHour * 24 );

        // includes leap year day
        const TInt maxDaysInMonths[12] = {  31,  62,  92, 123, 153, 184, 
                                            215, 245, 276, 306, 337, 366 };

        // calculate full days
        temp = aInterval.Int() / secsInDay;

        // calculate full years, calculate without leap year for user to get the 
        // longest time possible
        aIntYrs = temp / ( maxDaysInMonths[11] - 1 );

        // calc remainder days
        temp = temp % ( maxDaysInMonths[11] - 1 );

        aIntMon = 0;

        i = 0;
        if ( temp >= maxDaysInMonths[0] )
            {
            for ( i = 0; i < 11; i++ )
                {
                // found correct amount of months
                if ( temp >= maxDaysInMonths[i] && temp < maxDaysInMonths[i+1] )
                    {
                    // i now contains amount of full months (+1 because of table index)
                    aIntMon = i + 1;
                    break;
                    }
                }
            }

        // calc remainder days = allSecs - secsInFullYears - secsInFullMonts
        if( temp >= maxDaysInMonths[i] )
            {
            aIntDay = temp - maxDaysInMonths[i];
            }
        else
            {
            aIntDay = temp;
            }

        // calculate remainder secs
        temp = aInterval.Int() % secsInDay;

        aIntHrs = temp / secsInHour;

        // calculate remainder secs
        temp = temp % secsInHour;

        aIntMin = temp / secsInMin;

        // calculate remainder secs
        aIntSec = temp % secsInMin;
        }

    // ---------------------------------------------------------
    // AddSinglePartOfTimeL
    // ---------------------------------------------------------
    //
    void AddSinglePartOfTimeL( TInt aNumOfElements,
            TInt aResourceIdSingle,
            TInt aResourceIdOneFinal, 
            TInt aResourceIdTwoFour,
            TInt aResourceIdFiveZero, 
            CDesCArrayFlat* aStrings )
        {
        HBufC* stringHolder = NULL;
        TInt finalOneDigit( aNumOfElements % 10 );
        TInt finalTwoDigits( aNumOfElements % 100 );

        if ( aNumOfElements == 1 )
            {
            stringHolder = StringLoader::LoadLC( aResourceIdSingle );
            }
        else if ( finalOneDigit == 1 && finalTwoDigits != 11 )
            {
            stringHolder = StringLoader::LoadLC( aResourceIdOneFinal, 
                aNumOfElements );
            }
        else if ( finalOneDigit == 0 || 
                 ( finalOneDigit >= 5 && finalOneDigit <= 9 ) ||
                 ( finalTwoDigits >= 11 && finalTwoDigits <= 14 ) )
            {
            stringHolder = StringLoader::LoadLC( aResourceIdFiveZero,
                aNumOfElements );
            }
        else
            {
            stringHolder = StringLoader::LoadLC( aResourceIdTwoFour,
                aNumOfElements );
            }

        if ( aStrings )
            {
            aStrings->AppendL( *stringHolder );
            }

        CleanupStack::PopAndDestroy ( stringHolder );
        }

    // ---------------------------------------------------------
    // AddPartsOfTimeLC
    // ---------------------------------------------------------
    //
    HBufC* AddPartsOfTimeLC( TInt aIntYrs, TInt aIntMon, TInt aIntDay, 
                             TInt aIntHrs, TInt aIntMin, TInt aIntSec )
        {
        // Only the two most meaningful data will be showed
        TInt numOfData( 0 );

        CDesCArrayFlat* strings = new ( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( strings );

        if ( aIntYrs > 0 )
            {
            AddSinglePartOfTimeL( aIntYrs, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_YEARS_ONE,
                R_VCXHGMYVIDEOS_DRM_NBR_OF_YEARS_ONE_FINAL,
                R_VCXHGMYVIDEOS_DRM_NBR_OF_YEARS_TWO_FOUR,
                R_VCXHGMYVIDEOS_DRM_NBR_OF_YEARS_FIVE_ZERO,
                strings );
            numOfData++;
            }

        if ( aIntMon > 0 )
            {
            //  Second type not provided because 11 is the maximum!
            AddSinglePartOfTimeL( aIntMon, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_MONTHS_ONE,
                0,       
                R_VCXHGMYVIDEOS_DRM_NBR_OF_MONTHS_TWO_FOUR, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_MONTHS_FIVE_ZERO, 
                strings );
            numOfData++;
            }

        // Only if years or months were missing!
        if ( aIntDay > 0 && numOfData < 2 )
            {
            AddSinglePartOfTimeL( aIntDay, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_DAYS_ONE,
                R_VCXHGMYVIDEOS_DRM_NBR_OF_DAYS_ONE_FINAL,
                R_VCXHGMYVIDEOS_DRM_NBR_OF_DAYS_TWO_FOUR, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_DAYS_FIVE_ZERO, 
                strings );
            numOfData++;
            }

        if ( aIntHrs > 0 && numOfData < 2 )
            {
            AddSinglePartOfTimeL( aIntHrs, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_HOURS_ONE, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_HOURS_ONE_FINAL,
                R_VCXHGMYVIDEOS_DRM_NBR_OF_HOURS_TWO_FOUR, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_HOURS_FIVE_ZERO, 
                strings );
            numOfData++;
            }

        if ( aIntMin > 0 && numOfData < 2 )
            {
            AddSinglePartOfTimeL( aIntMin, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_MINS_ONE,
                R_VCXHGMYVIDEOS_DRM_NBR_OF_MINS_ONE_FINAL,
                R_VCXHGMYVIDEOS_DRM_NBR_OF_MINS_TWO_FOUR, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_MINS_FIVE_ZERO, 
                strings );
            numOfData++;
            }

        // If interval is 0, then it shows "0 seconds" anyway
        if ( ( aIntSec > 0 && numOfData < 2 ) || numOfData == 0 )
            {
            AddSinglePartOfTimeL( aIntSec, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_SECS_ONE, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_SECS_ONE_FINAL,
                R_VCXHGMYVIDEOS_DRM_NBR_OF_SECS_TWO_FOUR, 
                R_VCXHGMYVIDEOS_DRM_NBR_OF_SECS_FIVE_ZERO, 
                strings );
            numOfData++;
            }

        HBufC* stringHolder = NULL;
        if ( numOfData == 1 )
            {
            stringHolder = StringLoader::LoadL( R_VCXHGMYVIDEOS_DRM_MGR_DET_INTER,
                strings->MdcaPoint(0) );
            }
        else
            {
            stringHolder = StringLoader::LoadL( R_VCXHGMYVIDEOS_DRM_MGR_DET_INTER_TWO,
                *strings );
            }

        CleanupStack::PopAndDestroy( strings );
        
        CleanupStack::PushL( stringHolder );
    
        return stringHolder;
        }

    // ------------------------------------------------------------------------
    // FillUsageTimeLeftInfoL
    // ------------------------------------------------------------------------
    //
    void FillUsageTimeLeftInfoL( CDesCArray& aArray,
                                 const TTimeIntervalSeconds& aInterval,
                                 const TDesC& aType,
                                 TBool aIsAccumulatedTime )
        {
        TInt years( 0 );
        TInt months( 0 );
        TInt days( 0 );
        TInt hours( 0 );
        TInt minutes( 0 );
        TInt seconds( 0 );
        HBufC* label = NULL;
        HBufC* data = NULL;

        if ( aIsAccumulatedTime )
            {
            // "Usage time left"
            label = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_ACCUM_TIME_LEFT  );
            }
        else
            {
            // "Time left (%U)"
            label = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_UDL_X, aType );
            }

        SplitTime( aInterval, years, months, days, hours, minutes, seconds );
        data = AddPartsOfTimeLC( years, months, days, hours, minutes, seconds );

        AppendLabelAndDataToArrayL( aArray, *label, *data );

        CleanupStack::PopAndDestroy( data );
        CleanupStack::PopAndDestroy( label );
        }

    // ------------------------------------------------------------------------
    // FillIntervalInfoL
    // ------------------------------------------------------------------------
    //
    void FillIntervalInfoL( CDesCArray& aArray,
                            CDRMHelperRightsConstraints& aRights,
                            const TDesC& aType,
                            TTime& aStartTime,
                            TTime& aEndTime )
        {
        TTimeIntervalSeconds intervalSeconds( 0 );

        TRAPD( err, aRights.GetIntervalL( intervalSeconds ) );

        if ( err != KErrNotFound && err != KErrNone )
            {
            User::Leave( err );
            }

        if ( err == KErrNone )
            {
            TTime intervalStartTime( 0 );

            TRAP( err, aRights.GetIntervalStartL( intervalStartTime ) );

            if ( ( err != KErrNotFound ) && ( err != KErrNone ) )
                {
                User::Leave( err );
                }

            if ( err == KErrNotFound )
                {
                // "Times status (%U)"
                HBufC* label =
                    StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_UTS_X, aType );

                // "Not activated"
                HBufC* data = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_NOT_ACT );

                AppendLabelAndDataToArrayL( aArray, *label, *data );

                CleanupStack::PopAndDestroy( data );
                CleanupStack::PopAndDestroy( label );

                // "Time left (%U)"
                FillUsageTimeLeftInfoL( aArray, intervalSeconds, aType, EFalse );
                }
            else
                {
                TTime endTime( intervalStartTime );
                endTime += intervalSeconds;

                // "Valid from (%U)"
                DateTimeL( aArray, intervalStartTime,
                                            R_VCXHGMYVIDEOS_DRM_MGR_DET_RVF_X, aType );

                // "Valid until (%U)"
                DateTimeL( aArray, endTime, R_VCXHGMYVIDEOS_DRM_MGR_DET_RVT_X, aType );
                
                // IPTV: Caller will need the start and end time for activated interval.
                aStartTime = intervalStartTime;
                aEndTime   = endTime;                
                }
            }
        }

    // ------------------------------------------------------------------------
    // FillTimeInfoL
    // ------------------------------------------------------------------------
    //
    void FillTimeInfoL( CDesCArray& aArray,
                        CDRMHelperRightsConstraints& aRights,
                        const TDesC& aType,
                        TTime& aStartTime,
                        TTime& aEndTime )
        {
        TTime startTime;
        TTime endTime;

        TRAPD( startErr, aRights.GetStartTimeL( startTime ) );
        if ( startErr != KErrNotFound && startErr != KErrNone )
            {
            User::Leave( startErr );
            }
        
        TRAPD( endErr, aRights.GetEndTimeL( endTime ) );
        if ( endErr != KErrNotFound && endErr != KErrNone )
            {
            User::Leave( endErr );
            }
            
        // IPTV: In case we got start & end time, we need to check if they match 
        // the start & end time of activated interval constraint (as parameter).
        // If they match, we should not display the same information twice.            
        if ( startErr == KErrNone && endErr == KErrNone )
            {
            if ( startTime == aStartTime && endTime == aEndTime )
                {
                IPTVLOGSTRING_LOW_LEVEL("Video Storage UI ## CVcxHgMyVideosFullDrmInfo/FillTimeInfoL - DateTime matches Interval.");
                return;
                }
            }

        if ( startErr == KErrNone )
            {
            // "Valid from (%U)"
            DateTimeL( aArray, startTime, R_VCXHGMYVIDEOS_DRM_MGR_DET_RVF_X, aType );
            }

        if ( endErr == KErrNone )
            {
            // "Valid until (%U)"
            DateTimeL( aArray, endTime, R_VCXHGMYVIDEOS_DRM_MGR_DET_RVT_X, aType );
            }
        }

    // ------------------------------------------------------------------------
    // FillAccumulatedTimeInfoL
    // ------------------------------------------------------------------------
    //
    void FillAccumulatedTimeInfoL( CDesCArray& aArray,
                                   CDRMHelperRightsConstraints& aRights,
                                   const TDesC& aType )
        {
        TTimeIntervalSeconds accumSeconds( 0 );
        TRAPD( err, aRights.GetAccumulatedTimeL( accumSeconds ));
        if ( err != KErrNotFound && err != KErrNone )
            {
            User::Leave( err );
            }
        if ( err == KErrNone )
            {
            // "Usage time left"
            FillUsageTimeLeftInfoL( aArray, accumSeconds, aType, ETrue );
            }
        }

    // ------------------------------------------------------------------------
    // FillDrmInfoL
    // ------------------------------------------------------------------------
    //
    void FillDrmInfoL( CDesCArray& aArray,
                       CDRMHelperRightsConstraints* aRights,
                       TInt aResourceId )
        {
        IPTVLOGSTRING_LOW_LEVEL("Video Storage UI ## CVcxHgMyVideosFullDrmInfo::FillDrmInfoL - Enter");
        
        // Check if no rights at all
        if ( !aRights )
            {
            IPTVLOGSTRING_LOW_LEVEL("Video Storage UI ## CVcxHgMyVideosFullDrmInfo::FillDrmInfoL - Exit1");
            return;
            }

        // Rights type is either "Play", "Display", "Execute" or "Print"
        HBufC* type = StringLoader::LoadLC( aResourceId );

        // Check if full rights
        if ( aRights->FullRights() )
            {
            // "Rights (%U)"
            HBufC* label =
                    StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_FULL_X, *type );

            // "Unlimited"
            HBufC* data = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_UNLIMITED );

            AppendLabelAndDataToArrayL( aArray, *label, *data );

            CleanupStack::PopAndDestroy( data );
            CleanupStack::PopAndDestroy( label );
            CleanupStack::PopAndDestroy( type );
            IPTVLOGSTRING_LOW_LEVEL("Video Storage UI ## CVcxHgMyVideosFullDrmInfo::FillDrmInfoL - Exit2");
            return;                           // full rights -> return
            }

        IPTVLOGSTRING_LOW_LEVEL("Video Storage UI ## CVcxHgMyVideosFullDrmInfo::FillDrmInfoL - Fill counter info.");

        // Get detailed counter constraint information
        FillCounterInfoL( aArray, *aRights, *type );

        IPTVLOGSTRING_LOW_LEVEL("Video Storage UI ## CVcxHgMyVideosFullDrmInfo::FillDrmInfoL - Fill interval info.");

        // Get detailed interval constraint information (start time + duration)
        // IPTV: Get start and end time of activated interval.
        TTime startTime( (TInt64) 0 );
        TTime endTime( (TInt64) 0 );
        FillIntervalInfoL( aArray, *aRights, *type, startTime, endTime );

        IPTVLOGSTRING_LOW_LEVEL("Video Storage UI ## CVcxHgMyVideosFullDrmInfo::FillDrmInfoL - Fill time info.");

        // Get detailed time constraint information (start time + end time)
        // IPTV: Pass the start and end time of activated interval to algorithm.
        FillTimeInfoL( aArray, *aRights, *type, startTime, endTime );

        IPTVLOGSTRING_LOW_LEVEL("Video Storage UI ## CVcxHgMyVideosFullDrmInfo::FillDrmInfoL - Fill accumulated time info.");

        // Get detailed accumulated time constraint information (duration)
        FillAccumulatedTimeInfoL( aArray, *aRights, *type );

        CleanupStack::PopAndDestroy( type );
        
        IPTVLOGSTRING_LOW_LEVEL("Video Storage UI ## CVcxHgMyVideosFullDrmInfo::FillDrmInfoL - Exit3");
        }
        
    // -----------------------------------------------------------------------------
    // ResetAndDestroy
    // -----------------------------------------------------------------------------
    //
    void ResetAndDestroy( TAny* aItem )
        {
        static_cast< RPointerArray<CDRMHelperRightsConstraints>* >( aItem )->ResetAndDestroy();
        }

    } // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TVcxHgMyVideosFullDrmInfo::TVcxHgMyVideosFullDrmInfo()
// -----------------------------------------------------------------------------
//
TVcxHgMyVideosFullDrmInfo::TVcxHgMyVideosFullDrmInfo()
    {
    }
    
// -----------------------------------------------------------------------------
// TVcxHgMyVideosFullDrmInfo::~TVcxHgMyVideosFullDrmInfo()
// -----------------------------------------------------------------------------
// 
TVcxHgMyVideosFullDrmInfo::~TVcxHgMyVideosFullDrmInfo()
    {
    }

// -----------------------------------------------------------------------------
// TVcxHgMyVideosFullDrmInfo::GetFullDrmInfoL()
// -----------------------------------------------------------------------------
//
void TVcxHgMyVideosFullDrmInfo::GetFullDrmInfoL( CDesCArray& aItemArray,
                                                 const TDesC& aFileName )
    {
    TParsePtrC parse( aFileName );
    TPtrC ext = parse.Ext();
    TBool useCaf( EFalse );

    if ( ext.CompareF( KVcxHgExtensionWmWithDot) == 0 || 
         ext.CompareF( KVcxHgExtensionWmvWithDot) == 0 || 
         ext.CompareF( KVcxHgExtensionAsfWithDot) == 0 )
        {
        useCaf = ETrue;
        }

    if ( useCaf )
        {
        ContentAccess::CData* cdata         = NULL;
        TInt                  cafError      = KErrNone;
        TBool                 isProtected   = EFalse;
        TBool                 isForwardable = EFalse;
        TBool                 canPlay       = EFalse;

        cdata = CData::NewLC( (TVirtualPathPtr) aFileName, EPeek, EContentShareReadWrite );

        cafError = cdata->GetAttribute( ContentAccess::EIsProtected, isProtected );

        if ( cafError == KErrNone && isProtected )
            {
            cafError = cdata->GetAttribute( ContentAccess::ECanPlay, canPlay );

            HBufC* statusLabel = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_STAT );
            HBufC* statusData  = NULL;
            if ( cafError == KErrNone && canPlay )
                {
                statusData = StringLoader::LoadLC( R_VCXHGMYVIDEOS_WMDRM_VALID );
                }
            else
                {
                statusData = StringLoader::LoadLC( R_VCXHGMYVIDEOS_WMDRM_INVALID );
                }
            AppendLabelAndDataToArrayL( aItemArray, *statusLabel, *statusData );
            CleanupStack::PopAndDestroy( statusData );
            CleanupStack::PopAndDestroy( statusLabel );

            cafError = cdata->GetAttribute( ContentAccess::EIsForwardable, isForwardable );

            if ( cafError == KErrNone && isForwardable )
                {
                HBufC* sendingLabel = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_CS );
                HBufC* sendingData  = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_ALLOWED );
                AppendLabelAndDataToArrayL( aItemArray, *sendingLabel, *sendingData );
                CleanupStack::PopAndDestroy( sendingData );
                CleanupStack::PopAndDestroy( sendingLabel );                
                }
            }
            
        CleanupStack::PopAndDestroy( cdata );
        }
    else
        {
        TBool expired ( EFalse );
        TBool sendingAllowed( EFalse );
        RPointerArray<CDRMHelperRightsConstraints> tempArr;
        TCleanupItem cleanupItem( ResetAndDestroy, &tempArr );
        CleanupStack::PushL( cleanupItem );
        tempArr.AppendL( NULL ); // Play
        tempArr.AppendL( NULL ); // Display
        tempArr.AppendL( NULL ); // Execute
        tempArr.AppendL( NULL ); // Print

        CDRMHelper* drmHelper = CDRMHelper::NewLC();

        TRAPD( err, drmHelper->GetRightsDetailsL(
                        aFileName,
                        0, // Details for everything
                        expired,
                        sendingAllowed,
                        tempArr[0],
                        tempArr[1],
                        tempArr[2],
                        tempArr[3] ) );
        CleanupStack::PopAndDestroy( drmHelper );

        // Check if the rights are valid or expired
        // Should probably be able to append this information also when the
        // rights have expired.
        // "Status"
        HBufC* label = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_STAT );
        HBufC* data = NULL;
        // "Valid" or "Expired"
        TInt resId = ( expired || err == DRMCommon::ENoRights ?
            R_VCXHGMYVIDEOS_DRM_MGR_DET_EXP : R_VCXHGMYVIDEOS_DRM_MGR_DET_VALID );
        data = StringLoader::LoadLC( resId );
        AppendLabelAndDataToArrayL( aItemArray, *label, *data );
        CleanupStack::PopAndDestroy( data );
        CleanupStack::PopAndDestroy( label );

        // "Play"
        FillDrmInfoL( aItemArray, tempArr[0], R_VCXHGMYVIDEOS_DRM_MGR_DET2_PLAY );

        // "Display"
        FillDrmInfoL( aItemArray, tempArr[1], R_VCXHGMYVIDEOS_DRM_MGR_DET2_DISPLAY );

        // "Execute"
        FillDrmInfoL( aItemArray, tempArr[2], R_VCXHGMYVIDEOS_DRM_MGR_DET2_EXECUTE );

        // "Print"
        FillDrmInfoL( aItemArray, tempArr[3], R_VCXHGMYVIDEOS_DRM_MGR_DET2_PRINT );

        CleanupStack::PopAndDestroy( &tempArr );

        // Check whether sending is allowed or not

        // "Sending"
        label = StringLoader::LoadLC( R_VCXHGMYVIDEOS_DRM_MGR_DET_CS );
        data = NULL;
        // "Allowed" or "Forbidden"
        resId = ( sendingAllowed ? R_VCXHGMYVIDEOS_DRM_MGR_DET_ALLOWED
                                 : R_VCXHGMYVIDEOS_DRM_MGR_DET_FORBID );
        data = StringLoader::LoadLC( resId );
        AppendLabelAndDataToArrayL( aItemArray, *label, *data );
        CleanupStack::PopAndDestroy( data );
        CleanupStack::PopAndDestroy( label );
        }
    }
