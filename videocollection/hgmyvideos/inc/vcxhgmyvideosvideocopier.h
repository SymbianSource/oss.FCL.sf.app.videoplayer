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




#ifndef VCXHGMYVIDEOSVIDEOCOPIER_H
#define VCXHGMYVIDEOSVIDEOCOPIER_H

// INCLUDE FILES
#include <e32base.h>
#include <AknProgressDialog.h>

// FORWARD DECLARATIONS
class CVcxHgMyVideosVideoListImpl;
class CVcxHgMyVideosVideoModelHandler;
class CVcxHgMyVideosModel;
class CAknWaitDialog;

/**
 * Class for handling move/copy related notes and operations.
 * 
 * @lib vcxhgmyvideos.lib 
 */
NONSHARABLE_CLASS( CVcxHgMyVideosVideoCopier ) : public CBase,
                                                 public MProgressDialogCallback
    {
    public:

        /**
         * Two-phased constructor.
         * 
         * @param aVideoList Video list implementation.
         * @param aVideoModel Video model handler.
         * @param aModel Component's model.
         */
        static CVcxHgMyVideosVideoCopier* NewL( CVcxHgMyVideosVideoListImpl& aVideoList,
                                                CVcxHgMyVideosVideoModelHandler& aVideoModel,
                                                CVcxHgMyVideosModel& aModel );

        /**
         * Destructor.
         */
        virtual ~CVcxHgMyVideosVideoCopier();
    
        /**
         * Cheks status of currently selected items and UI list. Returns
         * flags that tell which move/copy realted menu items can be shown. 
         * 
         * @param aOperationTargets List of operation targets on UI.
         * @param aShowMoveAndCopySubmenu On return, ETrue if move/copy submenu can be shown.
         * @param aShowCopy On return, ETrue if copy menu item can be shown.
         * @param aShowMove On return, ETrue if move menu item can be shown.
         */
        void ShowMenuItemsL( 
                const RArray<TInt>& aOperationTargets,
                TBool& aShowMoveAndCopySubmenu,
                TBool& aShowCopy,
                TBool& aShowMove );

        /**
         * Moves or copies videos. Also shows memory selection dialog and
         * move/copy wait note.
         *
         * @param aOperationTargets List of operation targets on UI.
         * @param aCopy ETrue for copy operation, EFalse for move. 
         */        
        void MoveOrCopyL( 
                const RArray<TInt>& aOperationTargets,
                TBool aCopy );

        /**
         * Called by video list impl when move or copy command is completed.
         * 
         * @param aFailedCount Count of items that could not be moved/copied.
         * @param aFailedName Name of video where move/copy failed, if there was only one.
         */
        void VideoMoveOrCopyCompletedL( TInt aFailedCount, const TDesC& aFailedName );           

    public: // From MProgressDialogCallback

        /**
         * Called when (move/copy) wait dialog is dismissed.
         *
         * @aButtonId Button with which dialog was dismissed.
         */     
        void DialogDismissedL( TInt aButtonId );           
        
    protected:

        CVcxHgMyVideosVideoCopier( CVcxHgMyVideosVideoListImpl& aVideoList,
                                   CVcxHgMyVideosVideoModelHandler& aVideoModel,
                                   CVcxHgMyVideosModel& aModel );                

        void ConstructL();

        /**
         * Queries user for target drive.
         * 
         * @param aCopy ETrue for copy operation, EFalse for move.
         * @param aTargetDrive On return, target drive.
         * @return ETrue if user accepted target.
         */
        TBool QueryTargetDriveL( TBool aCopy, TInt& aTargetDrive );
        
        /**
         * Opens the move/copy wait note.
         * 
         * @param aOperationTargets Operation targets.
         * @param aCopy ETrue for copy operation, EFalse for move.
         */
        void OpenMoveCopyWaitNoteL( const RArray<TInt>& aOperationTargets,
                                    TBool aCopy );

        /**
         * Closes the move/copy wait note.
         */
        void CloseMoveCopyWaitNote();         

    protected: // data

        /**
         * Reference to video list implementation.
         * Not own.
         */
        CVcxHgMyVideosVideoListImpl& iVideoList;
        
        /**
         * Reference to video model handler.
         * Not own.
         */        
        CVcxHgMyVideosVideoModelHandler& iVideoModel;
        
        /**
         * Reference to component's model.
         * Not own.
         */
        CVcxHgMyVideosModel& iModel;

        /**
         * Move/copy wait dialog.
         * Own.
         */
        CAknWaitDialog* iMoveCopyWaitDialog;
        
        /**
         * Flag telling if latest operation was copy (ETrue) or move (EFalse).
         */
        TBool iCopy;
    };

#endif // VCXHGMYVIDEOSVIDEOCOPIER_H
