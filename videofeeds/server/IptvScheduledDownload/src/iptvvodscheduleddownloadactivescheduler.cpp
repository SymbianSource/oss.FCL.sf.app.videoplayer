/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Scheduled download scheduler active object.*
*/




#include <ipvideo/CCseScheduledProgram.h>
#include <ipvideo/CCseSchedulerAPI.h>
#include "IptvDebug.h"
#include <e32math.h>
#include <s32mem.h>
#include "IptvEngineUids.h"

#include "iptvvodscheduleddownloadactivescheduler.h"
#include "iptvvodscheduleddownloadscheduler.h"

// CONSTANTS
const TInt KIptvNumberOfDaysToSchedule(7);  //  Days to schedule
const TInt KIptvMorningStartHour = 6;
const TInt KIptvNoonStartHour = 11;
const TInt KIptvAfternoonStartHour = 13;
const TInt KIptvEveningStartHour = 18;
const TInt KIptvTenPercentDivider = 10;
const TInt KIptvTwiceTenPercent = 2;

//#define RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC

#ifdef RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC
#warning The scheduled download test flag has been defined! Do not release this build!
const TInt KIptvDayMonthAdvance = 1;
#endif // RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC

#if defined(_DEBUG) && ! defined(RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC)
const TInt KIptvDayMonthAdvanceDebug = 1;
#endif // _DEBUG && ! RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC

#ifdef RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC
const TInt KIptv30SecondsStartDelay = 30;
const TInt Kiptv60MinutesTestSlot = 60;
#endif // RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC

#if defined(RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC) || defined(_DEBUG)
const TInt KIptvDebugStringLength = 80;
#endif // RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC || _DEBUG

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::CIptvVODScheduledDownloadActiveScheduler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvVODScheduledDownloadActiveScheduler::CIptvVODScheduledDownloadActiveScheduler(
        CIptvVODScheduledDownloadScheduler& aScheduler,
        TIptvServiceId aServiceId,
        TIptvVodScheduleConnectionCondition aNetworkCondition,
        TIptvVodScheduleDownloadtimeCombination aDownloadTime,
        TIptvVodScheduleDownloadTypeCombination aDownloadType )
        : CTimer( EPriorityNormal ), iScheduler( aScheduler ),
            iServiceId( aServiceId ), iNetworkCondition( aNetworkCondition ),
            iDownloadTime( aDownloadTime ), iDownloadType( aDownloadType )
    {
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvVODScheduledDownloadActiveScheduler::ConstructL()
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadActiveScheduler::ConstructL()");

    CTimer::ConstructL();
    CActiveScheduler::Add ( this );

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::ConstructL()");
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvVODScheduledDownloadActiveScheduler* CIptvVODScheduledDownloadActiveScheduler::NewL(
        CIptvVODScheduledDownloadScheduler& aScheduler,
        TIptvServiceId aServiceId,
        TIptvVodScheduleConnectionCondition aNetworkCondition,
        TIptvVodScheduleDownloadtimeCombination aDownloadTime,
        TIptvVodScheduleDownloadTypeCombination aDownloadType )
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadActiveScheduler::NewL()");
    CIptvVODScheduledDownloadActiveScheduler* self =
        new( ELeave ) CIptvVODScheduledDownloadActiveScheduler(
            aScheduler,
            aServiceId,
            aNetworkCondition,
            aDownloadTime,
            aDownloadType );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::NewL()");

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CIptvVODScheduledDownloadActiveScheduler::~CIptvVODScheduledDownloadActiveScheduler()
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadActiveScheduler::~CIptvVODScheduledDownloadActiveScheduler()");

    //  Remove us from the list
    iScheduler.RemoveActive( this );

    delete iCse;
    iScheduledDownloads.ResetAndDestroy();

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::~CIptvVODScheduledDownloadActiveScheduler()");
    }


// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::RunL
// -----------------------------------------------------------------------------
//
void CIptvVODScheduledDownloadActiveScheduler::RunL()
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadActiveScheduler::RunL()");

    CleanupStack::PushL( this );

    //  This must be done here and not in the constructor, because the plugin
    //  will cause the creation of this instance and the next call will cause
    //  deadlock if this is in the constructor
    if (!iCse)
        {
        iCse = CCseSchedulerApi::NewL();
        }

    TRAP_IGNORE( UpdateScheduleL() );

    //  This will DESTROY us, don't do anything after this
    CleanupStack::PopAndDestroy( this );

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::RunL()");
    }


// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::UpdateScheduleL
// -----------------------------------------------------------------------------
//
void CIptvVODScheduledDownloadActiveScheduler::UpdateScheduleL()
    {
	IPTVLOGSTRING2_HIGH_LEVEL("CIptvVODScheduledDownloadActiveScheduler::UpdateScheduleL(), service %d", iServiceId);

    //  Just in case, reset the schedules
    iScheduledDownloads.ResetAndDestroy();

    //  Get current schedules
    User::LeaveIfError(
        iCse->GetSchedulesByPluginUid(
            KIptvScheduledDownloadPluginImplementationUid,
            iScheduledDownloads ) );

    //  Remove from array schedules which are not for this service or app uid
    for (TInt i = iScheduledDownloads.Count() - 1; i >= 0; i--)
        {
        if (GetServiceIdFromScheduledProgramL( *(iScheduledDownloads[i] ) ) !=
            iServiceId || iScheduledDownloads[i]->AppUid() != IPTV_SERVER_UID)
            {
            delete iScheduledDownloads[i];
            iScheduledDownloads.Remove( i );
            }
        }

    //  If manual update, no schedule or no download,
    //  remove all schedules from CSE
    if (iNetworkCondition  == EManual ||
        iDownloadTime == ENoSchedule ||
        iDownloadType == ENoDownload)
        {
    	IPTVLOGSTRING_HIGH_LEVEL("CIptvVODScheduledDownloadActiveScheduler::UpdateScheduleL() removing all schedules");

        //  Remove all schedules
        for (TInt i = 0; i < iScheduledDownloads.Count(); i++)
            {
            User::LeaveIfError( iCse->RemoveSchedule(
                    iScheduledDownloads[i]->DbIdentifier() ) );
            }
        }
    else
        {
        //  Reschedule the schedules
        ReScheduleL();
        }

    iScheduledDownloads.ResetAndDestroy();

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::UpdateScheduleL()");
    }


// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::ReScheduleL
// -----------------------------------------------------------------------------
//
void CIptvVODScheduledDownloadActiveScheduler::ReScheduleL()
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadActiveScheduler::ReScheduleL()");

    //  Create all time slots
    RArray<TScheduleSlot> scheduleSlots;
    CleanupClosePushL(scheduleSlots);
    BuildScheduleSlotsL( scheduleSlots );

    //  Go through the scheduled downloads, try to find a place for them
    for (TInt i = 0; i < iScheduledDownloads.Count(); i++)
        {
        CCseScheduledProgram* schedule = iScheduledDownloads[i];
        TTime& scheduleTime = schedule->StartTime();

        //  Go through the schedule slots and
        //  try to find place for the schedule
        for (TInt j = 0; j < scheduleSlots.Count(); j++)
            {
            TScheduleSlot& slot = scheduleSlots[j];
            if (slot.iStartTime <= scheduleTime &&
                slot.iEndTime > scheduleTime)
                {
                //  Slot found, we can finish the loop after handling this
                j = scheduleSlots.Count();

                //  Should there be a schedule and if so, is it empty
                if (slot.iShouldHaveSchedule && !slot.iAlreadyHasSchedule)
                    {
                    //  Suitable empty slot found
                    slot.iAlreadyHasSchedule = ETrue;
                    }
                else
                    {
                    //  Slot should not have schedule or it already has a
                    //  schedule, cancel this
                    User::LeaveIfError(
                        iCse->RemoveSchedule( schedule->DbIdentifier() ) );
                    }
                }
            }
        }

#ifdef RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC  //  this is just for test

        CCseScheduledProgram*   newSchedule = CCseScheduledProgram::NewL();
        CleanupStack::PushL( newSchedule );

        TTime start;
        start.UniversalTime();
        start += TTimeIntervalSeconds( KIptv30SecondsStartDelay );

        newSchedule->SetStartTime( start );
        start += TTimeIntervalMinutes( Kiptv60MinutesTestSlot );
        newSchedule->SetEndTime( start );
        newSchedule->SetAppUid( IPTV_SERVER_UID );
        newSchedule->SetPluginUid(
                KIptvScheduledDownloadPluginImplementationUid );
        newSchedule->SetScheduleType(
                CCseScheduledProgram::ECseScheduleDownload );
        HBufC8* data = WriteApplicationDataL();
        CleanupStack::PushL( data );
        newSchedule->SetApplicationDataL( data->Des() );
        CleanupStack::PopAndDestroy( data );

        User::LeaveIfError( iCse->AddSchedule( *newSchedule ) );

        TDateTime date( start.DateTime() );
        _LIT( KTimeFormat, "Scheduled download for service %d at %d.%d.%d %d:%d, 30s test schedule" );
        TBuf<KIptvDebugStringLength> buffer;
        buffer.Format(
                KTimeFormat,
                iServiceId,
                date.Day() + KIptvDayMonthAdvance,
                date.Month() + KIptvDayMonthAdvance,
                date.Year(),
                date.Hour(),
                date.Minute() );
        IPTVLOGTEXT_HIGH_LEVEL( buffer );

        CleanupStack::PopAndDestroy( newSchedule );

#else //  RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC, this is normal case

    //  All schedules in the CSE have been checked,
    //  now check if we need to add
    for (TInt k = 0; k < scheduleSlots.Count(); k++)
        {
        TScheduleSlot& slot = scheduleSlots[k];

        //  Should we have schedule here and we don't have
        if (slot.iShouldHaveSchedule && !slot.iAlreadyHasSchedule)
            {
            //  We need to add new schedule here
            CCseScheduledProgram*   newSchedule =
                    CCseScheduledProgram::NewL();
            CleanupStack::PushL( newSchedule );
            //  Randomize the run time
            newSchedule->SetStartTime( RandomizeRunTime( slot ) );
            newSchedule->SetEndTime( slot.iEndTime );
            newSchedule->SetAppUid( IPTV_SERVER_UID );
            newSchedule->SetPluginUid(
                    KIptvScheduledDownloadPluginImplementationUid );
            newSchedule->SetScheduleType(
                    CCseScheduledProgram::ECseScheduleDownload );
            HBufC8* data = WriteApplicationDataL();
            CleanupStack::PushL( data );
            newSchedule->SetApplicationDataL( data->Des() );
            CleanupStack::PopAndDestroy( data );

            User::LeaveIfError( iCse->AddSchedule( *newSchedule ) );

#ifdef _DEBUG
            //  Print time for debugging purpose
            TDateTime date( newSchedule->StartTime().DateTime() );
            _LIT( KTimeFormat, "Scheduled download for service %d at %d.%d.%d %d:%d" );
            TBuf<KIptvDebugStringLength> buffer;
            buffer.Format(
                    KTimeFormat,
                    iServiceId,
                    date.Day() + KIptvDayMonthAdvanceDebug,
                    date.Month() + KIptvDayMonthAdvanceDebug,
                    date.Year(),
                    date.Hour(),
                    date.Minute() );
	        IPTVLOGTEXT_HIGH_LEVEL( buffer );
#endif

            CleanupStack::PopAndDestroy( newSchedule );
            }
        }

#endif  //  RD_IPTV_FEA_SCHEDULE_DOWNLOAD_IN_30_SEC

    CleanupStack::PopAndDestroy( &scheduleSlots );

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::ReScheduleL()");
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::BuildScheduleSlotsL
// -----------------------------------------------------------------------------
//
void CIptvVODScheduledDownloadActiveScheduler::BuildScheduleSlotsL(
        RArray<TScheduleSlot>& aScheduleSlots ) const
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadActiveScheduler::BuildScheduleSlotsL()");

    TTime now;
    now.UniversalTime();
    TTime homeNow;
    homeNow.HomeTime();
    TTimeIntervalMicroSeconds homeUtcDiff = homeNow.MicroSecondsFrom( now );

    aScheduleSlots.Reset();

    //  Set the seconds, minutes and micros to zero
    TDateTime start = now.DateTime();
    start.SetMinute( 0 );
    start.SetSecond( 0 );
    start.SetMicroSecond( 0 );

    //  Set slot start time for today in correct time
    start.SetHour( 0 );
    TTime night( start );
    night -= homeUtcDiff; //  change to UTC

    start.SetHour( KIptvMorningStartHour );
    TTime morning( start );
    morning -= homeUtcDiff; //  change to UTC

    start.SetHour( KIptvNoonStartHour );
    TTime noon( start );
    noon -= homeUtcDiff; //  change to UTC

    start.SetHour( KIptvAfternoonStartHour );
    TTime afternoon( start );
    afternoon -= homeUtcDiff; //  change to UTC

    start.SetHour( KIptvEveningStartHour );
    TTime evening( start );
    evening -= homeUtcDiff; //  change to UTC

    //  Create slots for KIptvNumberOfDaysToSchedule days
    for (TInt i = 0; i < KIptvNumberOfDaysToSchedule; i++)
        {
        User::LeaveIfError( aScheduleSlots.Append(
                TScheduleSlot( night, morning, iDownloadTime & ENight ) ) );
        User::LeaveIfError( aScheduleSlots.Append(
                TScheduleSlot( morning, noon, iDownloadTime & EMorning ) ) );
        User::LeaveIfError( aScheduleSlots.Append(
                TScheduleSlot( noon, afternoon, iDownloadTime & ENoon ) ) );
        User::LeaveIfError( aScheduleSlots.Append(
                TScheduleSlot(
                    afternoon,
                    evening,
                    iDownloadTime & EAfternoon ) ) );
        User::LeaveIfError( aScheduleSlots.Append(
                TScheduleSlot(
                    evening,
                    night + TTimeIntervalDays(1),
                    iDownloadTime & EEvening ) ) );

        //  Advance one day
        night += TTimeIntervalDays(1);
        morning += TTimeIntervalDays(1);
        noon += TTimeIntervalDays(1);
        afternoon += TTimeIntervalDays(1);
        evening += TTimeIntervalDays(1);
        }

    //  Some schedules may be in the past, remove them
    for (TInt i = 0; i < aScheduleSlots.Count(); i++)
        {
        if (now > aScheduleSlots[i].iEndTime)
            {
            aScheduleSlots.Remove( i );
            i--;
            }
        }

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::BuildScheduleSlotsL()");
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::GetServiceIdFromScheduledProgramL
// -----------------------------------------------------------------------------
//
TIptvServiceId CIptvVODScheduledDownloadActiveScheduler::GetServiceIdFromScheduledProgramL(
        CCseScheduledProgram& aProgram ) const
    {
    return ReadApplicationDataL( aProgram.ApplicationData() );
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::WriteApplicationDataL
// -----------------------------------------------------------------------------
//
HBufC8* CIptvVODScheduledDownloadActiveScheduler::WriteApplicationDataL() const
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadActiveScheduler::WriteApplicationDataL()");

    HBufC8* data = HBufC8::NewLC( sizeof(TIptvServiceId) );
    TPtr8 dataPtr( data->Des() );

    RDesWriteStream writeStream;
    CleanupClosePushL( writeStream );
    writeStream.Open( dataPtr );
    writeStream.WriteUint32L( iServiceId );
    CleanupStack::PopAndDestroy( &writeStream );

    CleanupStack::Pop( data );

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::WriteApplicationDataL()");

    return data;
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::ReadApplicationDataL
// -----------------------------------------------------------------------------
//
TIptvServiceId CIptvVODScheduledDownloadActiveScheduler::ReadApplicationDataL(
        TPtrC8 aDataPtr ) const
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadActiveScheduler::ReadApplicationDataL()");

    RDesReadStream readStream;
    CleanupClosePushL( readStream );
    readStream.Open( aDataPtr );
    TIptvServiceId serviceId = readStream.ReadUint32L();
    CleanupStack::PopAndDestroy( &readStream );

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::ReadApplicationDataL()");

    return serviceId;
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadActiveScheduler::RandomizeRunTime
// -----------------------------------------------------------------------------
//
TTime CIptvVODScheduledDownloadActiveScheduler::RandomizeRunTime(
        const TScheduleSlot& aSlot ) const
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadActiveScheduler::RandomizeRunTime()");

    TTime start;
    start.UniversalTime();
    TBool moveStart = EFalse;

    //  Earliest start is either slot start or current time
    if (start < aSlot.iStartTime)
        {
        start = aSlot.iStartTime;
        moveStart = ETrue;
        }

    //  Get difference in minutes
    TTimeIntervalMinutes    diffMinutes;
    aSlot.iEndTime.MinutesFrom( start, diffMinutes );

    //  If the end time is earlier than start time, just use the start time
    if (diffMinutes.Int() < 0)
        {
    	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::RandomizeRunTime()");
        return start;
        }

    TInt diff = diffMinutes.Int();

    TTime retVal( start.Int64() );
    TInt32 tenPercent = diff / KIptvTenPercentDivider;
    TTimeIntervalMinutes tenPercentMinutes( tenPercent );

    //  Start time is moved 10% forward if the start time is now
    if (moveStart)
        {
        diff -= tenPercent;
        retVal += tenPercentMinutes;
        }

    //  Start it at least 20% before the end time
    diff -= KIptvTwiceTenPercent * tenPercent;

    //  Randomize the differences
    TUint32 random = Math::Random();
    TUint64 resultRandom =
            (TUint64(random) * TUint64(diff)) / TUint64(KMaxTUint32);

    TTimeIntervalMinutes    randomMinutes( resultRandom );

    //  Move random time to forward
    retVal += randomMinutes;

    // Note: We need to set microseconds to zero because for some reason schedule 
    // data reading from CSE database does not work correctly in E90 if microsecond
    // component is set to non-zero value.
    TDateTime fixedDate = retVal.DateTime();
    fixedDate.SetMicroSecond( 0 );
    TTime retValWithoutMicroSeconds( fixedDate );

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadActiveScheduler::RandomizeRunTime()");

    return retValWithoutMicroSeconds;
    }


// End of File  
