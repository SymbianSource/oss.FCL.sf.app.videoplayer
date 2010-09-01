/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Scheduled download scheduler.*
*/




#include <ipvideo/CCseScheduledProgram.h>
#include <ipvideo/CCseSchedulerAPI.h>
#include "IptvDebug.h"
#include <e32math.h>
#include <s32mem.h>

#include "iptvvodscheduleddownloadscheduler.h"
#include "iptvvodscheduleddownloadactivescheduler.h"
#include "CIptvServiceManager.h"
#include "CIptvSmEvent.h"

const TInt KNoDelay = 0;

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadScheduler::CIptvVODScheduledDownloadScheduler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIptvVODScheduledDownloadScheduler::CIptvVODScheduledDownloadScheduler(
    CIptvServiceManager& aServiceManager ) : iServiceManager( aServiceManager )
    {
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadScheduler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIptvVODScheduledDownloadScheduler::ConstructL()
    {
    User::LeaveIfError( iServiceManager.RegisterObserver( this ) );
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadScheduler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIptvVODScheduledDownloadScheduler* CIptvVODScheduledDownloadScheduler::NewL(
        CIptvServiceManager& aServiceManager )
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadScheduler::NewL()");

    CIptvVODScheduledDownloadScheduler* self =
        new( ELeave ) CIptvVODScheduledDownloadScheduler( aServiceManager );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadScheduler::NewL()");

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CIptvVODScheduledDownloadScheduler::~CIptvVODScheduledDownloadScheduler()
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadScheduler::~CIptvVODScheduledDownloadScheduler()");

    iServiceManager.DeRegisterObserver( this );

    for (TInt i = 0; i < iActiveSchedulers.Count(); i++)
        {
        iActiveSchedulers[i]->Cancel();
        }

    iActiveSchedulers.ResetAndDestroy();

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadScheduler::~CIptvVODScheduledDownloadScheduler()");
    }


// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadScheduler::HandleSmEvent
// -----------------------------------------------------------------------------
//
void CIptvVODScheduledDownloadScheduler::HandleSmEvent( CIptvSmEvent& aEvent )
    {
    TRAP_IGNORE( HandleSmEventL( aEvent ) );
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadScheduler::HandleSmEventL
// -----------------------------------------------------------------------------
//
void CIptvVODScheduledDownloadScheduler::HandleSmEventL(
        CIptvSmEvent& aEvent )
    {
	IPTVLOGSTRING2_HIGH_LEVEL(">>> CIptvVODScheduledDownloadScheduler::HandleSmEvent(), event: %d", aEvent.iEvent);

    switch (aEvent.iEvent)
        {
        case CIptvSmEvent::EServiceAdded:
        case CIptvSmEvent::EServiceScheduleModified:
            {
            //  Only reschedule if there is a service
            if (aEvent.iService)
                {
                //  Create, add and activate scheduler object
                CIptvVODScheduledDownloadActiveScheduler* active =
                    CIptvVODScheduledDownloadActiveScheduler::NewL(
                        *this,
                        aEvent.iServiceId,
                        (TIptvVodScheduleConnectionCondition)
                                aEvent.iService->ScheduleDlNetwork(),
                        aEvent.iService->ScheduleDlTime(),
                        aEvent.iService->ScheduleDlType());
                active->After( TTimeIntervalMicroSeconds32( KNoDelay ) );

                //  The scheduling object will remove itself from the list
                CleanupStack::PushL( active );
                iActiveSchedulers.AppendL( active );
                CleanupStack::Pop( active );
                }
            }
            break;

        case CIptvSmEvent::EServiceDeleted:
            {
            //  Create, add and activate scheduler object
            //  The settings cause all schedules to be deleted
            CIptvVODScheduledDownloadActiveScheduler* active =
                CIptvVODScheduledDownloadActiveScheduler::NewL(
                    *this,
                    aEvent.iServiceId,
                    EManual,
                    ENoSchedule,
                    ENoDownload);
            active->After( TTimeIntervalMicroSeconds32( KNoDelay ) );

            //  The scheduling object will remove itself from the list
            CleanupStack::PushL( active );
            iActiveSchedulers.AppendL( active );
            CleanupStack::Pop( active );
            }
        break;

        default:
        break;
        }

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadScheduler::HandleSmEvent()");
    }

// -----------------------------------------------------------------------------
// CIptvVODScheduledDownloadScheduler::RemoveActive
// -----------------------------------------------------------------------------
//
void CIptvVODScheduledDownloadScheduler::RemoveActive(
            CIptvVODScheduledDownloadActiveScheduler* aActive )
    {
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvVODScheduledDownloadScheduler::RemoveActive()");

    TInt index = iActiveSchedulers.Find( aActive );
    if (index != KErrNotFound)
        {
        iActiveSchedulers.Remove( index );
        }

	IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvVODScheduledDownloadScheduler::RemoveActive()");
    }

// End of File  
