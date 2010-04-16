/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Active object to split up long running db tasks*
*/



// INCLUDE FILES
#include <e32base.h>
#include <mpxlog.h>
#include "vcxmyvideosactivetask.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosActiveTask::CVcxMyVideosActiveTask( MVcxMyVideosActiveTaskObserver& aObserver ) :
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosActiveTask* CVcxMyVideosActiveTask::NewL( MVcxMyVideosActiveTaskObserver& aObserver )
    {
    CVcxMyVideosActiveTask* self = new(ELeave) CVcxMyVideosActiveTask( aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosActiveTask::~CVcxMyVideosActiveTask()
    {
    Cancel();
    delete iCurCommand;
    }

// ---------------------------------------------------------------------------
// Start the operation
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::StartL( TMPXCommandId aTask, const CMPXCommand& aCommand )
    {
    delete iCurCommand;
    iCurCommand = NULL;
    iCurCommand = CMPXMedia::NewL( aCommand );
    iCurTask = aTask;
    iCurStep = 0;

    // Start the AO
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// Get the current step
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosActiveTask::GetStep()
    {
    return iCurStep;
    }

// ---------------------------------------------------------------------------
// Get the current task
// ---------------------------------------------------------------------------
//
TMPXCommandId CVcxMyVideosActiveTask::GetTask()
    {
    return iCurTask;
    }

// ---------------------------------------------------------------------------
// Get the current media
// ---------------------------------------------------------------------------
//
CMPXMedia& CVcxMyVideosActiveTask::GetCommand()
    {
    return *iCurCommand;
    }

// ---------------------------------------------------------------------------
// Get the current media
// ---------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosActiveTask::Command()
    {
    return iCurCommand;
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::RunL()
    {
    switch ( iObserver.HandleStepL() )
        {
        case MVcxMyVideosActiveTaskObserver::EDone:
            Done();
            break;

        case MVcxMyVideosActiveTaskObserver::EMoreToCome:
            ContinueStepping();
            break;

        //observer is responsible for calling Done or ContinueStepping
        case MVcxMyVideosActiveTaskObserver::EStopStepping:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosActiveTask::Done
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::Done()
    {
    MPX_DEBUG1("CVcxMyVideosActiveTask::Done() start");

    iObserver.HandleOperationCompleted( KErrNone );
    delete iCurCommand;
    iCurCommand = NULL;

    MPX_DEBUG1("CVcxMyVideosActiveTask::Done() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosActiveTask::ContinueStepping
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::ContinueStepping()
    {
    ++iCurStep;
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::DoCancel()
    {
    // Callback and cleanup
    iObserver.HandleOperationCompleted( KErrCancel );
    delete iCurCommand;
    iCurCommand = NULL;
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosActiveTask::RunError( TInt aError )
    {
    // Callback and cleanup
    iObserver.HandleOperationCompleted( aError );
    delete iCurCommand;
    iCurCommand = NULL;

    return KErrNone;
    }

// END OF FILE
