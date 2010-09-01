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




#ifndef CIPTVVODSCHEDULEDDOWNLOADACTIVESCHEDULER_H
#define CIPTVVODSCHEDULEDDOWNLOADACTIVESCHEDULER_H

//  INCLUDES
#include <e32base.h>
#include "CIptvUtil.h"
#include "iptvvodscheduleddownloaddefinitions.h"

// CONSTANTS

// CLASS DECLARATION
class CCseScheduledProgram;
class CCseSchedulerApi;
class CIptvVODScheduledDownloadScheduler;

/**
*  Scheduled download scheduler. This class handles the calculating new
*  schedules to the CSE when settings have been changed.
*
*  @lib IptvServer.exe
*  @since 
*/
NONSHARABLE_CLASS ( CIptvVODScheduledDownloadActiveScheduler ):
        public CTimer
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aScheduler The owning scheduler.
        * @param aServiceId The service id.
        * @param aNetworkCondition The network type to use when downloading.
        * @param aDownloadTime The download time.
        * @param aDownloadType The download type.
        */
        static CIptvVODScheduledDownloadActiveScheduler* NewL(
                CIptvVODScheduledDownloadScheduler& aScheduler,
                TIptvServiceId aServiceId,
                TIptvVodScheduleConnectionCondition aNetworkCondition,
                TIptvVodScheduleDownloadtimeCombination aDownloadTime,
                TIptvVodScheduleDownloadTypeCombination aDownloadType );
        
        /**
        * Destructor.
        */
        virtual ~CIptvVODScheduledDownloadActiveScheduler( );

    public: // From CTimer

    	void RunL();

    private: // New functions

        /**
        *  Internal helper class representing one shcedule slot.
        *
        *  @lib iptvuiengine.lib
        *  @since 
        */
        class TScheduleSlot
            {
            public:
                /**
                * C++ constructor.
                * @param aStartTime The start time of the slot.
                * @param aEndTime The end time of the slot.
                * @param aShouldHave Should the slot have a schedule.
                */
                TScheduleSlot(
                    const TTime& aStartTime,
                    const TTime& aEndTime,
                    TBool aShouldHave ) :
                    iStartTime( aStartTime), iEndTime( aEndTime ),
                    iShouldHaveSchedule( aShouldHave ),
                    iAlreadyHasSchedule( EFalse )
                    {
                    }

            private:
                TScheduleSlot() {}

            public:
                /**
                * The start time of the slot.
                */
                TTime   iStartTime;

                /**
                * The end time of the slot.
                */
                TTime   iEndTime;

                /**
                * Should the slot have a schedule.
                */
                TBool   iShouldHaveSchedule;

                /**
                * Does the slot have a schedule.
                */
                TBool   iAlreadyHasSchedule;
            };

        /**
        * Get existing schedules for this service and resechedule them.
        */
        void UpdateScheduleL();

        /**
        * Add new schedules and remove the found schedules if needed.
        */
        void ReScheduleL();

        /**
        * Create a list of time slots from now to KNumberOfDaysToSchedule
        * days forward midnight.
        * 
        * @param aScheduleSlots The scheduled download slots.
        */
        void BuildScheduleSlotsL(
            RArray<TScheduleSlot>& aScheduleSlots ) const;

        /**
        * Get the application data from the scheduled download
        * and return the service id from it.
        * 
        * @return The service id.
        */
        TIptvServiceId GetServiceIdFromScheduledProgramL(
            CCseScheduledProgram& aProgram ) const;

        /**
        * Create application data from the service id.
        * 
        * @return The created applicatiuon data. Caller will delete the data.
        */
        HBufC8* WriteApplicationDataL() const;

        /**
        * Get the service id from the application data.
        * 
        * @param aDataPtr The data to read from.
        * @return The service id.
        */
        TIptvServiceId ReadApplicationDataL( TPtrC8 aDataPtr ) const;

        /**
        * Return randomized time which fits the given slot. Also take into
        * account the current time.
        * 
        * @param aSlot The time slot to randomize between.
        * @return The randomized time.
        */
        TTime RandomizeRunTime( const TScheduleSlot& aSlot ) const;

    private:

        /**
        * C++ constructor.
        *
        * @param aScheduler The owning scheduler.
        * @param aServiceId The service id.
        * @param aNetworkCondition The network type to use when downloading.
        * @param aDownloadTime The download time.
        * @param aDownloadType The download type.
        */
        CIptvVODScheduledDownloadActiveScheduler(
                CIptvVODScheduledDownloadScheduler& aScheduler,
                TIptvServiceId aServiceId,
                TIptvVodScheduleConnectionCondition aNetworkCondition,
                TIptvVodScheduleDownloadtimeCombination aDownloadTime,
                TIptvVodScheduleDownloadTypeCombination aDownloadType );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        /**
        * The schedule how created us and has a pointer to us.
        */
        CIptvVODScheduledDownloadScheduler& iScheduler;

        /**
        * The service to use.
        */
        TIptvServiceId iServiceId;

        /**
        * The network condition.
        */
        TIptvVodScheduleConnectionCondition iNetworkCondition;

        /**
        * The download time.
        */
        TIptvVodScheduleDownloadtimeCombination iDownloadTime;

        /**
        * The download type.
        */
        TIptvVodScheduleDownloadTypeCombination iDownloadType;

        /**
        * Common Scheduling Engine API. Owned.
        */
        CCseSchedulerApi*    iCse;

        /**
        * Temporary holding place for scheduled downloads. Owned.
        */
        RPointerArray<CCseScheduledProgram> iScheduledDownloads;
    };

#endif      // CIPTVVODSCHEDULEDDOWNLOADACTIVESCHEDULER_H
            
// End of File
