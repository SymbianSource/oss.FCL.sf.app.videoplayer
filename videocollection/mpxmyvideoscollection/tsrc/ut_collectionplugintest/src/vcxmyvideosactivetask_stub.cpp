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
    MPX_FUNC("CVcxMyVideosActiveTask::StartL");
    delete iCurCommand;
    iCurCommand = NULL;
    iCurCommand = CMPXMedia::NewL( aCommand );
    iCurTask = aTask;
    iCurStep = 0;
    }

// ---------------------------------------------------------------------------
// Sets current command, unit test function
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::SetNewCommandL( const CMPXCommand& aCommand )
    {
    delete iCurCommand;
    iCurCommand = NULL;
    iCurCommand = CMPXMedia::NewL( aCommand );
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
        }
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosActiveTask::Done
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::Done()
    {
    iObserver.HandleOperationCompleted( KErrNone );
    delete iCurCommand;
    iCurCommand = NULL;
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosActiveTask::ContinueStepping
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::ContinueStepping()
    {
    }

void CVcxMyVideosActiveTask::Cancel()
    {
    if( iCurCommand )
        {
        iObserver.HandleOperationCompleted( KErrCancel );
        delete iCurCommand;
        iCurCommand = NULL;
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosActiveTask::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// END OF FILE
