/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Implementation of the scheduled download plugin*
*/



// INCLUDE FILES
#include <ipvideo/CCseScheduledProgram.h>
#include <ipvideo/CCseSchedulerAPI.h>
#include <ipvideo/MCsePluginObserver.h>
#include "IptvDebug.h"
#include "iptvscheduleddownloadlugin.h"
#include "vcnsscheduleddownloadpluginengine.h"

// CONSTANTS
const TInt KIptvRescheduleInterval( 15 );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CIptvSchedulerPlugin::NewL
//
// -----------------------------------------------------------------------------
//
CIptvScheduledDownloadPlugin* CIptvScheduledDownloadPlugin::NewL()
	{
	CIptvScheduledDownloadPlugin* self =
            new (ELeave) CIptvScheduledDownloadPlugin();

    return self;		
	}

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPlugin::CIptvScheduledDownloadPlugin
//
// -----------------------------------------------------------------------------
//
CIptvScheduledDownloadPlugin::CIptvScheduledDownloadPlugin()
	{
	}

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPlugin::~CIptvScheduledDownloadPlugin
//
// -----------------------------------------------------------------------------
//	
CIptvScheduledDownloadPlugin::~CIptvScheduledDownloadPlugin()
	{
	IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPlugin::~CIptvScheduledDownloadPlugin");

	if ( iIsDead )
	    {
	    *iIsDead = ETrue;
	    }
    delete iEngine;
    iEngine = NULL;

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPlugin::~CIptvScheduledDownloadPlugin");
	}

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPlugin::RunTaskL
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPlugin::RunTaskL( CCseScheduledProgram& aProg,
						 		             MCsePluginObserver* aObserver )
	{
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPlugin::RunTaskL");

    TBool isDead( EFalse );
    
    iIsDead = &isDead;
    
	//  Is there already a download going on
	if (iEngine)
	    {
        IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPlugin::RunTaskL rescheduling");

	    //  Yes, reschedule this for later time
	    TRAP_IGNORE( RescheduleL(
	        aObserver,
	        aProg,
	        TTimeIntervalMinutes( KIptvRescheduleInterval ) ) );

        IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPlugin::RunTaskL rescheduled");
	    return ;
	    }

	//  Create always new engine, we are not deleted because we might be reused
	//  Engine has callbacks which should not be left on when not needed
	iEngine = CIptvScheduledDownloadPluginEngine::NewL( *aObserver );

	// Pass it through to engine.
	TInt engineReturnValue = 0;	
	TRAPD( err, iEngine->RunTaskL( aProg, engineReturnValue ) );
			
	// When we come out from RunTaskL it might be that the operation was cancelled
	// by deleting ScheduledDownloadPlugin. In such case we are now running "dead code"
	// which means the class is already destroyed, but the stack is still around. We need
	// to get out from here as soon as possible and avoid changing class internal variables.
	if( isDead )
	    {
	    IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPlugin::RunTaskL - RunTaskL aborted, GTFO from here");
	    return;
	    }

    if (err)
        {
        IPTVLOGSTRING2_HIGH_LEVEL("CIptvScheduledDownloadPlugin::RunTaskL engine finished with error %d ", err);

        aObserver->PluginCompleted( err );
        }

    //  Did the plugin ask for reschedule
    if ( engineReturnValue == CIptvScheduledDownloadPluginEngine::EReschedule)
        {
	    //  Yes, reschedule this for later time
        aObserver->PluginCompleted( KErrNone );
	    RescheduleL(
	        aObserver, aProg, TTimeIntervalMinutes( KIptvRescheduleInterval ) );
        }
    else if ( engineReturnValue == CIptvScheduledDownloadPluginEngine::EDontReschedule )
        {
	    //  Reschedule is no more possible
	    IPTVLOGSTRING_HIGH_LEVEL(
	        "CIptvScheduledDownloadPlugin::RunTaskL Plugin completed without reschedule" );

        aObserver->PluginCompleted( KErrNone );
        }
    else
        {
        if (engineReturnValue < KErrNone)
            {
            aObserver->PluginCompleted( engineReturnValue );
            }
        }

    delete iEngine;
    iEngine = NULL;

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPlugin::RunTaskL");
    }

// -----------------------------------------------------------------------------
// CIptvScheduledDownloadPlugin::RescheduleL
//
// -----------------------------------------------------------------------------
//	
void CIptvScheduledDownloadPlugin::RescheduleL(
            MCsePluginObserver* /*aObserver*/,
            CCseScheduledProgram& aProg,
            TTimeIntervalMinutes aDelay )
    {
    IPTVLOGSTRING_HIGH_LEVEL(">>> CIptvScheduledDownloadPlugin::RescheduleL");

    TTime start;
    start.UniversalTime();
    start += aDelay;

    //  Only reschedule if it fits the slot
    if (start < aProg.EndTime())
        {
        CCseSchedulerApi* scheduler = CCseSchedulerApi::NewL();
        CleanupStack::PushL( scheduler );

        CCseScheduledProgram* newSchedule = CCseScheduledProgram::NewL();
        CleanupStack::PushL( newSchedule );

        newSchedule->SetStartTime( start );
        newSchedule->SetEndTime( aProg.EndTime() );
        newSchedule->SetAppUid( aProg.AppUid() );
        newSchedule->SetPluginUid( aProg.PluginUid() );
        newSchedule->SetScheduleType( aProg.ScheduleType() );
        newSchedule->SetApplicationDataL( aProg.ApplicationData() );

        User::LeaveIfError( scheduler->AddSchedule( *newSchedule ) );

        IPTVLOGSTRING_HIGH_LEVEL("CIptvScheduledDownloadPlugin::RescheduleL Rescheduling done");

        CleanupStack::PopAndDestroy( newSchedule );

        CleanupStack::PopAndDestroy( scheduler );
        }

    IPTVLOGSTRING_HIGH_LEVEL("<<< CIptvScheduledDownloadPlugin::RescheduleL");
    }
// End of File
