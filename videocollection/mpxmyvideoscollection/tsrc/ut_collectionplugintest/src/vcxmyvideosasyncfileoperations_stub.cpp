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
* Description:    Implementation for MPX My Videos Collection Move, Copy and Delete operations.*
*/




// INCLUDE FILES
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideosasyncfileoperations_stub.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations* CVcxMyVideosAsyncFileOperations::NewL(
    CVcxMyVideosCollectionPlugin& aCollection )
    {
    CVcxMyVideosAsyncFileOperations* self = new (ELeave) CVcxMyVideosAsyncFileOperations(
            aCollection );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations::~CVcxMyVideosAsyncFileOperations()
    {
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations::CVcxMyVideosAsyncFileOperations( CVcxMyVideosCollectionPlugin& aCollection )
: iCollection( aCollection )
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::ConstructL ()
    {
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::DeleteVideoL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::DeleteVideoL( TUint32 /*aMdsId*/, TBool /*aForce*/ )
    {
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::HandleMoveOrCopyStepL
// ----------------------------------------------------------------------------
//
MVcxMyVideosActiveTaskObserver::TStepResult
        CVcxMyVideosAsyncFileOperations::HandleMoveOrCopyStepL()
    {
    return MVcxMyVideosActiveTaskObserver::EDone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::InitMoveOrCopyOperationsL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::InitMoveOrCopyOperationsL( CMPXMedia& /*aCmd*/ )
    {
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::CancelOperationL
// Called when leave or cancel occurs for the operation, generates resp msg.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::CancelOperationL( TInt /*aErr*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::SendOperationRespL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::SendOperationRespL( TInt /*aCmdId*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::MoveOrCopyVideoL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::MoveOrCopyVideoL( TUint32 /*aMdsId*/ )
    {
    }    

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::InitSingleMoveOrCopyL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::InitSingleMoveOrCopyL( TUint32 /*aMdsId*/ )
    {
    }


// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::HandleFileCopyCompletedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileOperations::HandleFileCopyCompletedL( TInt /*aErr*/ )
    {
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::DriveHasEnoughFreeSpaceL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAsyncFileOperations::DriveHasEnoughFreeSpaceL( const TDesC& /*aPath*/, TInt /*aDrive*/ )
    {
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::GenerateTargetPathForMoveOrCopyL
// ----------------------------------------------------------------------------
//    
void CVcxMyVideosAsyncFileOperations::GenerateTargetPathForMoveOrCopyL(
        const TDesC& /*aSourcePath*/, TDes& /*aTargetPath*/, TInt /*aTargetDrive*/ )
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileOperations::HandleDeleteStepL
// ----------------------------------------------------------------------------
//
MVcxMyVideosActiveTaskObserver::TStepResult CVcxMyVideosAsyncFileOperations::HandleDeleteStepL()
    {
    return MVcxMyVideosActiveTaskObserver::EDone;
    }


