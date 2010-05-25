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
* Description:      Class for handling move/copy related notes and operations.*
*/




// INCLUDE FILES
#include <CAknMemorySelectionDialogMultiDrive.h>
#include <AknWaitDialog.h>
#include <AknCommonDialogsDynMem.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <vcxmyvideosdefs.h>
#include <vcxhgmyvideos.rsg>
#include "IptvDebug.h"
#include "vcxhgmyvideoscollectionclient.h"
#include "vcxhgmyvideosvideomodelhandler.h"
#include "vcxhgmyvideosmodel.h"
#include "vcxhgmyvideosvideocopier.h"
#include "CIptvDriveMonitor.h"

// =========================== MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::CVcxHgMyVideosVideoCopier()
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosVideoCopier::CVcxHgMyVideosVideoCopier(
        CVcxHgMyVideosVideoListImpl& aVideoList,
        CVcxHgMyVideosVideoModelHandler& aVideoModel,
        CVcxHgMyVideosModel& aModel )
 :  iVideoList( aVideoList ),
    iVideoModel( aVideoModel ),
    iModel( aModel )
    {
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::ConstructL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoCopier::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::NewL()
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosVideoCopier* CVcxHgMyVideosVideoCopier::NewL( 
        CVcxHgMyVideosVideoListImpl& aVideoList,
        CVcxHgMyVideosVideoModelHandler& aVideoModel,
        CVcxHgMyVideosModel& aModel )
    {
    CVcxHgMyVideosVideoCopier* self = 
        new( ELeave ) CVcxHgMyVideosVideoCopier( aVideoList, aVideoModel, aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::~CVcxHgMyVideosVideoCopier()
// ---------------------------------------------------------------------------
//
CVcxHgMyVideosVideoCopier::~CVcxHgMyVideosVideoCopier()
    {
    CloseMoveCopyWaitNote();
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::ShowMenuItemsL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoCopier::ShowMenuItemsL( 
        const RArray<TInt>& aOperationTargets,
        TBool& aShowMoveAndCopySubmenu,
        TBool& aShowCopy,
        TBool& aShowMove )
    {
    CIptvDriveMonitor& driveMonitor = iModel.DriveMonitorL();
    HBufC* videoUri = NULL;
    TInt drive( 0 );

    TUint32 flags;

    for ( TInt i = 0; i < aOperationTargets.Count(); i++ )
        {
        // When we found source file that can be moved/copied, we need
        // to also check that there is target drive that we can use.
        videoUri = iVideoModel.GetVideoUri( aOperationTargets[i] ).AllocLC();
        if ( videoUri->Length() > 0 )
            {
            if ( iModel.FileServerSessionL().CharToDrive( videoUri->Des()[0], drive )
                  == KErrNone )
                {
                for ( TInt j = 0; j < driveMonitor.iAvailableDrives.Count(); j++ )
                    {
                    flags = driveMonitor.iAvailableDrives[j].iFlags;

                    if ( driveMonitor.iAvailableDrives[j].iDrive != drive &&
                            !(flags & TIptvDriveInfo::ELocked) && !(flags & TIptvDriveInfo::EMediaNotPresent) )
                        {
                        aShowMoveAndCopySubmenu = aShowCopy = aShowMove = ETrue;

                        // No need to continue, we know that menu can be shown.
                        CleanupStack::PopAndDestroy( videoUri );
                        return;
                        }
                    }
                }
            }
        CleanupStack::PopAndDestroy( videoUri );
        }
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::MoveOrCopyL()
// ---------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoCopier::MoveOrCopyL( 
        const RArray<TInt>& aOperationTargets,
        TBool aCopy )
    {
    TInt targetDrive( EDriveC );

    if ( QueryTargetDriveL( aCopy, targetDrive ) )
        {
        // Wait note is closed in destructor (CloseMoveCopyWaitNote()), 
        // in DialogDismissedL() or in VideoMoveOrCopyCompletedL().
        OpenMoveCopyWaitNoteL( aOperationTargets, aCopy );

        TRAPD( ret, iVideoModel.MoveOrCopyVideosL( aOperationTargets, targetDrive, aCopy ));
        
        if( ret != KErrNone )
            {
            CloseMoveCopyWaitNote();
            }
        
        iCopy = aCopy;
        }
    }

// ---------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::QueryTargetDriveL()
// ---------------------------------------------------------------------------
//
TBool CVcxHgMyVideosVideoCopier::QueryTargetDriveL( TBool aCopy, TInt& aTargetDrive )
    {
    TCommonDialogType dialogType;
    TBool driveSelected( EFalse );
    TDriveNumber selectedMem( EDriveC );
    TInt includedMedias( 0 );

    if ( aCopy )
        {
        dialogType = ECFDDialogTypeCopy;
        }
    else
        {
        dialogType = ECFDDialogTypeMove;
        }
    
    if ( iModel.DriveMonitorL().MassStorageDrive() != KErrNotFound )
        {
        includedMedias |= AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage;
        }
    if ( iModel.DriveMonitorL().MemoryCardDrive() != KErrNotFound )
        {
        includedMedias |= AknCommonDialogsDynMem::EMemoryTypeMMCExternal;
        }
    if ( iModel.DriveMonitorL().FixedDrive( EFalse ) == EDriveC )
        {
        includedMedias |= AknCommonDialogsDynMem::EMemoryTypePhone;
        }
    // If CIptvDriveMonitor does not publish C-drive, it means that product
    // has (at least) two other usable memories, and therefore there is no
    // need to allow user to move / copy items to (small) C-drive. 
    if ( iModel.DriveMonitorL().PhoneMemoryDrive() == KErrNotFound )
        {
        includedMedias &= ~AknCommonDialogsDynMem::EMemoryTypePhone;
        }

// Skip the dialog in emulator as ASSERT_DEBUG macro does not allow single memory dialog.
#if !defined(__WINS__) && !defined(__WINSCW__)
    CAknMemorySelectionDialogMultiDrive* dlg =
        CAknMemorySelectionDialogMultiDrive::NewL(
            dialogType,
            R_VCXHGMYVIDEOS_MEMORY_SELECTION_DIALOG,
            EFalse,
            includedMedias );
    CleanupStack::PushL( dlg );
    driveSelected = dlg->ExecuteL( selectedMem );
    CleanupStack::PopAndDestroy(); // dlg
#else
    driveSelected = ETrue;
    selectedMem = EDriveE;
#endif // !defined(__WINS__) && !defined(__WINSCW__)

    if ( driveSelected )
        {
        aTargetDrive = static_cast<TInt>( selectedMem );
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::OpenMoveCopyWaitNoteL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoCopier::OpenMoveCopyWaitNoteL( 
    const RArray<TInt>& aOperationTargets,
    TBool aCopy )
    {
    CloseMoveCopyWaitNote();

    HBufC* note = NULL;

    if ( aOperationTargets.Count() == 1 )
        {
        HBufC* name = iVideoModel.GetVideoName( aOperationTargets[0] ).AllocLC();
        if ( aCopy )
            {
            note = StringLoader::LoadL( R_VCXHGMYVIDEOS_COPYING_ONE, *name );
            }
        else
            {
            note = StringLoader::LoadL( R_VCXHGMYVIDEOS_MOVING_ONE, *name );
            }
        CleanupStack::PopAndDestroy( name );
        CleanupStack::PushL( note );
        }
    else
        {
        if ( aCopy )
            {
            note = StringLoader::LoadLC( R_VCXHGMYVIDEOS_COPYING_MANY );
            }
        else
            {
            note = StringLoader::LoadLC( R_VCXHGMYVIDEOS_MOVING_MANY );
            }
        }

    iMoveCopyWaitDialog = new (ELeave) 
            CAknWaitDialog( ( REINTERPRET_CAST( CEikDialog**, &iMoveCopyWaitDialog ) ), ETrue );
    iMoveCopyWaitDialog->SetCallback( this );
    iMoveCopyWaitDialog->SetTextL( *note );
    
    if ( aCopy )
        {
        iMoveCopyWaitDialog->ExecuteLD( R_VCXHGMYVIDEOS_COPY_WAIT_NOTE );
        }
    else
        {
        iMoveCopyWaitDialog->ExecuteLD( R_VCXHGMYVIDEOS_MOVE_WAIT_NOTE );
        }

    CleanupStack::PopAndDestroy( note );
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::CloseMoveCopyWaitNote()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoCopier::CloseMoveCopyWaitNote()
    {
    if ( iMoveCopyWaitDialog )
        {
        TRAPD( error, iMoveCopyWaitDialog->ProcessFinishedL() );
        if ( error != KErrNone )
            {
            delete iMoveCopyWaitDialog;
            }
        iMoveCopyWaitDialog = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::DialogDismissedL()
// Callback about (move/copy) wait note dismissal.
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoCopier::DialogDismissedL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyCancel )
        {
        iModel.CollectionClient().CancelMoveOrCopyVideosL();
        }
    iMoveCopyWaitDialog = NULL;
    }

// -----------------------------------------------------------------------------
// CVcxHgMyVideosVideoCopier::VideoMoveOrCopyCompletedL()
// -----------------------------------------------------------------------------
//
void CVcxHgMyVideosVideoCopier::VideoMoveOrCopyCompletedL( TInt aFailedCount,
                                                           const TDesC& aFailedName )
    {
    CloseMoveCopyWaitNote();

    if ( aFailedCount > 0 )
        {
        HBufC* text = NULL;

        if ( aFailedCount == 1 )
            {
            if ( iCopy )
                {
                text = StringLoader::LoadLC( R_VCXHGMYVIDEOS_FLDR_ITEM_CANNOT_BE_COPIED,
                                             aFailedName );
                }
            else
                {
                text = StringLoader::LoadLC( R_VCXHGMYVIDEOS_FLDR_ITEM_CANNOT_BE_MOVED,
                                             aFailedName );                
                }
            }
        else
            {
            if ( iCopy )
                {
                text = StringLoader::LoadLC( R_VCXHGMYVIDEOS_FLDR_SOME_ITEMS_CANT_COPY, 
                                             aFailedCount );
                }
            else
                {
                text = StringLoader::LoadLC( R_VCXHGMYVIDEOS_FLDR_SOME_ITEMS_CANT_MOVE, 
                                             aFailedCount );                
                }            
            }

        CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );  
        }    
    }
